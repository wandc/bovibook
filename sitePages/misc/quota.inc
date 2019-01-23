<?php 
//keeps track of quota changes.


class Quota extends TabPage {

    public function defaultDisplay() {
        //double check to make sure only admin can see
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            //setup for tabs.
            $tabArray[1]['name'] = 'Quota';
            $tabArray[2]['name'] = 'Credits';
            $tabArray[3]['name'] = 'Incentive Days';
            $tabArray[4]['name'] = 'Revenue';
            $tabArray[5]['name'] = 'Agropur';

            $this->renderTabs($tabArray);
        }
    }

    protected function tab1() {
        $this->quotaChangeQuickform();
        $this->displayListOfQuotaHoldings($mode = 'default');
        $this->displayGraphOfQuotaHoldings();
    }

    protected function tab2() {
        $this->creditChangeQuickform();
        $this->displayCreditsPurchased();
        print("<hr/>");
        $this->onFarmCreditsChart();
        $this->onFarmCreditsPositionHistoricalLog();
        print("<hr/>");
        $this->histoicalCreditExchangePriceQuickform();
        echo "<br><br><br><br><br><br><br><br><br>";
        $this->historicalCreditExchangeChart();
        $this->histoicalCreditExchangePriceLog();
        $this->milkCreditEstimation();
    }

    protected function tab3() {
        $this->incentiveDayQuickform();
        $this->displayIncentiveDay();
        $this->displayGraphOfQuotaIncentiveHoldings();
    }

    protected function tab4() {

        print("<h3>Yearly Revenue</h3>");
        
       $data=($this->calculateLast4YearsTotalMilkStatement());
       print( (new JQueryDataTable)->startBasicArray($data['data'],''));

        $this->milkStatementQuickform();
           print("<h3>Monthly Solids Price</h3>");
        $this->displayListMilkStatementInfo();
        //
        print ("<h3>Blend Price (fixed components)</h3>");
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Price ($) BF:4.3%'] = 'number';
         $headerStrArr['Price ($) BF:4.0%'] = 'number';
         $headerStrArr['Price ($) BF:3.7%'] = 'number';
         $opt = "               
                        height: '600'
                       ";
        $x = new GoogleVisualizationLine($opt, $headerStrArr, self::calculateBlendPrice(4.3),self::calculateBlendPrice(4), self::calculateBlendPrice(3.7));
        print($x->toString());
        //
        $this->displayLast24MonthsMilkRevenue();
        //
        $this->findTheNumberOfPickupDaysForMonth();
    }
    
    protected function tab5() {
       
          $dataArr = array();
        
        //get current year
        $year = (new DateTime)->format("Y");
        //loop through last 4.
         for ($x = $year; $x >= $year-4; $x--) {
        $dataArr[] = $this->calculateSpecifcAgropurYearsTotalMilkStatement($x);
         }
         
         print( (new JQueryDataTable)->startBasicArray($dataArr,''));
    }

    public function summaryDisplay() {
        //nothing
    }

    function displayIncentiveDay() {
        print ("<h3>Incentive Days</h3>");
        $sql = "SELECT date,incentive,userid FROM batch.incentive_day ORDER by date DESC";

         print( (new JQueryDataTable)->startBasicSql($sql,''));

    }

    function incentiveDayQuickform() {

        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForNextAndPrev12Months();

        $form = new HTML_QuickForm("incentiveDayQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'incentiveDayQuickform', "Enter Incentive Days");
        $form->addElement('select', 'dateEvent', 'Month of Incentive Day', $dateArray);
        //
        $form->addElement('text', 'days', '# of days', 'size="2" maxlength="2"');
        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('dateEvent', 'Date is required', 'required');
        $form->addRule('days', 'Price is required', 'required');
        $form->addRule('days', 'Price must be a number', 'numeric');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            // 
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d', $tempArray[0]);
            //
            $days = $form->exportValue('days');
            //
            $pageid = $form->exportValue('pageid');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //lookupdate to make sure it is not already used.
                $res = $GLOBALS['pdo']->query("SELECT date FROM batch.incentive_day WHERE date='$event_time' LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);

                //go ahead and do insert if not already in.
                if ($res->rowCount() == 0) {
                    $query = "INSERT INTO batch.incentive_day (date,incentive,userid)
	  VALUES ('$event_time',$days,'{$GLOBALS['auth']->getUsername()}')";
                    $res = $GLOBALS['pdo']->exec($query);
                }



                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function lastMilkStatementsCalculation() {
        $retArr = array();
        for ($x = 0; $x <= 48; $x++) {
            $retArr[] = $this->milkRevenueProjection(date('Y-m-01', strtotime("-$x month")), false);
        }

        $output['data'] = $retArr;
        return ($output);

        // print_r2($retArr);
    }

    /*
     * this returns the last 4 years of milk statement data
     * 
     */
    public function calculateLast4YearsTotalMilkStatement() {
        $dataArr = array();
        
        //get current year
        $year = (new DateTime)->format("Y");
        $year=$year;
        //loop through last 4.
         for ($x = $year; $x >= $year-5; $x--) {
        $dataArr[] = $this->calculateSpecifcYearsTotalMilkStatement($x);
         }
       
       //return output     
        $output['data'] = $dataArr;
        return ($output);
    }

    private function calculateSpecifcYearsTotalMilkStatement($year=null) {
        if (empty($year)) {
            throw new Exception("Error: valid year needed");
        }
      $xArr = array();
      $retArr=array();
      
      
      //fancy new code to make monlthy intervals for a year.
        $start = new DateTime($year . '-01-01');
        $interval = new DateInterval('P1M'); //monthly
        $end = new DateTime($year . '-12-31');
        $period = new DatePeriod($start, $interval, $end);
        foreach ($period as $dt) {
            $xArr[] = $this->milkRevenueProjection($dt->format('Y-m-01'), false);
        }

        $retArr['year']=$year;
        $retArr['fat_kg'] = round(array_sum(array_column($xArr,'fat_kg')),2);
        $retArr['protein_kg'] = round(array_sum(array_column($xArr,'protein_kg')),2);
        $retArr['lactose_kg'] = round(array_sum(array_column($xArr,'lactose_kg')),2);
        $retArr['revenue'] = round(array_sum(array_column($xArr,'revenue')),2);
         $retArr['total']=  round($retArr['revenue'] / $retArr['fat_kg'],2);
        /*
         $ret['datetime'] = date('Y-M-d', strtotime($start_date));
        $ret['period'] = date('F Y', strtotime($start_date));
        $ret['numPickups'] = $num_pickups;
        $ret['fat_per'] = round($avg_fat, 2);
        $ret['protein_per'] = round($avg_protein, 2);
        $ret['lactose_per'] = round($avg_lactose, 2);
        $ret['SNF'] = round($SNFRatio, 2);
        $ret['fat_kg'] = round($total_fat_kg, 2);
        $ret['protein_kg'] = round($total_protein_kg, 2);
        $ret['lactose_kg'] = round($total_lactose_kg, 2);
        $ret['fat_kg_day'] = round($fat_kg_day, 2);
        $ret['protein_kg_day'] = round($prot_kg_day, 2);
        $ret['revenue'] = round($rev, 2);
        $ret['expenses'] = round($exp, 2);
        $ret['total'] = round($total, 2);
        $ret['advance'] = round($total / 2, 2);
         */
         return $retArr;
    }
    
    /* agropur year runs from Nov 1 previous year to Oct 31 of current year */
     private function calculateSpecifcAgropurYearsTotalMilkStatement($year=null) {
        if (empty($year)) {
            throw new Exception("Error: valid year needed");
        }
      $xArr = array();
      $retArr=array();
      
      
      //fancy new code to make monlthy intervals for a year.
        $start = new DateTime($year-1 . '-11-01');
        $interval = new DateInterval('P1M'); //monthly
        $end = new DateTime($year . '-10-31');
        $period = new DatePeriod($start, $interval, $end);
        foreach ($period as $dt) {
            $xArr[] = $this->milkRevenueProjection($dt->format('Y-m-01'), false);
        }

        $retArr['year']=$year;
        $retArr['fat_kg'] = array_sum(array_column($xArr,'fat_kg'));
        $retArr['protein_kg'] = array_sum(array_column($xArr,'protein_kg'));
        $retArr['lactose_kg'] = array_sum(array_column($xArr,'lactose_kg'));
        $retArr['total_solids'] = $retArr['fat_kg'] +  $retArr['protein_kg'] + $retArr['lactose_kg'];
        
        
         return $retArr;
    }
    
    
    
    
    function displayLast24MonthsMilkRevenue() {
        print("<br/><br/><hr/>");
        print("<h3>Historical Milk Statements</h3>");
        ?>
        <table cellpadding="0" cellspacing="0" border="0" class="display compact" id="example2"></table>
        <script type="text/javascript">
            "use strict";
            $(document).ready(function () {

                var table = $('#example2').DataTable({
                    "ajax": "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/lastMilkStatementsCalculation/",
                    "aaSorting": [[0, 'DESC']],
                    "columns": [
                        {
                            data: "datetime",
                            "visible": false,
                            "searchable": false
                        },
                        {
                            data: "period",
                            title: "period",
                            type: "date"
                        },
                        {
                            data: "numPickups",
                            title: "# Pickups"
                        },
                        {
                            data: "fat_per",
                            title: "Fat %"
                        },
                        {
                            data: "protein_per",
                            title: "Protein %"
                        },
                        {
                            data: "lactose_per",
                            title: "Lactose %"
                        },
                        {
                            data: "SNF",
                            title: "SNF Ratio"
                        },
                        {
                            data: "fat_kg",
                            title: "Fat (kg)"
                        },
                        {
                            data: "protein_kg",
                            title: "Protein (kg)"
                        },
                        {
                            data: "lactose_kg",
                            title: "Lactose (kg)"
                        },
                        {
                            data: "fat_kg_day",
                            title: "Fat (kg/day)"
                        },
                        {
                            data: "protein_kg_day",
                            title: "Protein (kg/day)"
                        },
                        {
                            data: "revenue_day",
                            title: "Revenue ($/day)"
                        },
                        {
                            data: "revenue",
                            title: "Revenue $"
                        },
                        {
                            data: "expenses",
                            title: "Expenses $"
                        },
                        {
                            data: "total",
                            title: "Total $"
                        },
                        {
                            data: "advance",
                            title: "Advance $"
                        }

                    ],
                });
            });
        </script>

        <?php 
    }

    private function calculateBlendPrice($level=4.3) {
        
        switch ($level) {
   case 4.3:
           $sql = "SELECT date,round(butterfat*.0430+protein*.036+lactose*.0572,3) as fixed_blend_price FROM batch.milk_statement ORDER BY date DESC";
         break;
   case 4:
          $sql = "SELECT date,round(butterfat*.0400+protein*.034+lactose*.0572,3) as fixed_blend_price FROM batch.milk_statement ORDER BY date DESC";
         break;
   case 3.7:
           $sql = "SELECT date,round(butterfat*.0370+protein*.032+lactose*.0572,3) as fixed_blend_price FROM batch.milk_statement ORDER BY date DESC";
         break;  
   default:
       throw new Exception("no level specified.");
         break;
}
        


        $res = $GLOBALS ['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            if ($row ['fixed_blend_price'] != 0) {
                $blendPriceArr[strtotime($row ['date'])] = $row ['fixed_blend_price'];
            }
        }
        return $blendPriceArr;
    }

    function displayCreditsPurchased() {
  print("<h3>Credits Purchased</h3>");
       

        $sql = <<<SQL
with temp as (
SELECT event_time::date as date,price,abs(credit_change) as credits,abs(price*credit_change) as amount_purchased
FROM batch.credit 
LEFT JOIN  batch.credit_exchange on date_trunc('month',credit.event_time)=date_trunc('month',date)
WHERE event_type='buy_sell'
), temp2 as (
SELECT *,price*(credits/NULLIF(   sum(credits) OVER (PARTITION BY date_trunc('year',date)) ,0)  ) as weighting
from temp
)
SELECT date,price	,credits,amount_purchased,round((sum(weighting)  OVER (PARTITION BY date_trunc('year',date))),2) as marginal_yearly_credit_price from temp2
 ORDER BY date DESC
SQL;
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
             $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.                  
        }
        //output
        $dataArr = $results;
      
         print( (new JQueryDataTable)->startBasicArray($dataArr,'searching: false,ordering: false,info: false'));
        
     
    }

    function historicalCreditExchangeChart() {
        $sql = "SELECT * FROM batch.credit_exchange ORDER by date DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $priceArr[strtotime($row['date'])] = $row['price'];
        }

        print ('<div class="dfg">');
        print ("<h3>Historical Credit Exchange Price</h3>");


        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Credit Clearing Price ($)'] = 'number';
        $x = new GoogleVisualizationLine(null, $headerStrArr, $priceArr);
        print($x->toString());

        print ('</div> <!-- end chart -->');
    }

    function onFarmCreditsChart() {
        $sql = "SELECT  event_time::date,event_type,credit_change 
 , sum(credit_change) OVER ( ORDER BY event_time::date) AS cum_amt
from batch.credit order by event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $priceArr[strtotime($row['event_time'])] = $row['cum_amt'];
        }

        print ('<div class="dfgd">');
        print ("<h3>On Farm Credits Position</h3>");


        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Credits (kg)'] = 'number';
        $options=' pointSize: 8';
        $x = new GoogleVisualizationLine($options, $headerStrArr, $priceArr);
        print($x->toString());

        print ('</div> <!-- end chart -->');
    }

    function onFarmCreditsPositionHistoricalLog() {
        print ("<h3>On Farm Credit Position Historical Log</h3>");
        $sql = "
SELECT  event_time::date,event_type,credit_change 
 , sum(credit_change) OVER ( ORDER BY event_time::date) AS cum_amt
from batch.credit order by event_time DESC";
         print( (new JQueryDataTable)->startBasicSql($sql,'searching: false,ordering: false,info: false'));
    }

    function histoicalCreditExchangePriceLog() {
         print ("<h3>Credit Exchange Log</h3>");
        $sql = "SELECT * FROM batch.credit_exchange ORDER by date DESC";
        print( (new JQueryDataTable)->startBasicSql($sql,'searching: false,ordering: false,info: false'));
    }

    function histoicalCreditExchangePriceQuickform() {

        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForPrev12Months();

        $form = new HTML_QuickForm("milkCreditExchangeQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'milkCreditExchangeQuickform', "Enter Milk Credit Exchange");
        $form->addElement('select', 'dateEvent', 'Month of Exchange', $dateArray);
        //
        $form->addElement('text', 'price', 'Clearing Price', 'size="6" maxlength="7"');
        $form->addElement('text', 'amount_cleared', 'Amount Cleared', 'size="6" maxlength="7"');
        $form->addElement('text', 'amount_offered', 'Amount Offered', 'size="6" maxlength="7"');

        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);




        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('dateEvent', 'Date is required', 'required');
        $form->addRule('price', 'Price is required', 'required');
        $form->addRule('price', 'Price must be a number', 'numeric');
        //$form->addRule('amount_cleared', 'Amount cleared is required', 'required');
        $form->addRule('amount_cleared', 'Amount cleared must be a number', 'numeric');
        //$form->addRule('amount_offered', 'Amount offered is required', 'required');
        $form->addRule('amount_offered', 'Amount offered must be a number', 'numeric');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            // 
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d', $tempArray[0]);
            //
            $price = $form->exportValue('price');
            $amount_cleared = $form->exportValue('amount_cleared');
            $amount_offered = $form->exportValue('amount_offered');
            //could have only partial information for old exchanges.
            if ($amount_cleared == '') {
                $amount_cleared = 'null';
            }
            if ($amount_offered == '') {
                $amount_offered = 'null';
            }


            $pageid = $form->exportValue('pageid');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();


                //lookupdate to make sure it is not already used.
                $res = $GLOBALS['pdo']->query("SELECT date FROM batch.credit_exchange WHERE date='$event_time' LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);

                //go ahead and do insert if not already in.
                if ($res->rowCount() == 0) {
                    $query = "INSERT INTO batch.credit_exchange (date,price,cleared,offered,userid)
	  VALUES ('$event_time',$price,$amount_cleared,$amount_offered,'{$GLOBALS['auth']->getUsername()}')";
                    $res = $GLOBALS['pdo']->exec($query);
                }



                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    // finds the number of pickups in the current and next month(s).
    function findTheNumberOfPickupDaysForMonth() {
        print("<hr />");
        print("<h2>Pickup Days in Future Months</h2>");

        $sql = "SELECT max(event_time) as max_event_time FROM batch.milk_pickup_event LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $row['max_event_time'];


        $firstOfMonthOne = date('Y-m-01', strtotime("now"));
        $monthOne = date('M', strtotime($firstOfMonthOne));
        $firstOfMonthTwo = date('Y-m-01', strtotime("+1 month"));
        $monthTwo = date('M', strtotime($firstOfMonthTwo));

        //now use the last pickup date as a basis find out the number of pickups.

        $monthOnePickupDays = $this->findNumberOfPickupsInGivenMonth($row['max_event_time'], $firstOfMonthOne);
        $monthTwoPickupDays = $this->findNumberOfPickupsInGivenMonth($row['max_event_time'], $firstOfMonthTwo);

        print("$monthOne: $monthOnePickupDays | $monthTwo:$monthTwoPickupDays <br><br>");


        //esitmate revenue for current month, by using last months data
        $ret = $this->milkRevenueProjection(date('Y-m-01', strtotime("-1 month")), false);
        $revHalfEstimate = ($monthOnePickupDays * ($ret['revenue'] / $ret['numPickups'])) / 2;

        $estimatedFirstPayment = $revHalfEstimate + ($revHalfEstimate - $ret['advance']);
        $estimatedSecondPayment = $revHalfEstimate;

        Print("<h2>Estimated $monthOne Statment</h2>");
        Print("<h3>Note: Assumes the same production/componenets/price as month before</h3>");
        print("Estimated $monthTwo 16 Payment: $ $estimatedFirstPayment<br/>");
        print("Normal Payment: $ $revHalfEstimate<br>");
        print("Previous Advance: $ {$ret['advance']}<br><br>");
        print("Estimated $monthTwo 30 Payment: $ $estimatedSecondPayment<br/>");
    }

    /**
     * find number of milk pickups in a given month (date of interest)
     */
    private function findNumberOfPickupsInGivenMonth($lastPickupDate, $dateOfInterest) {

        //convert epoch seconds to number of days since epoch, does not do leap seconds.

        $lastPickupDate_DaysEpoch = strtotime(date('Y-m-d', strtotime($lastPickupDate)) . " UTC") / 86400;

        $monthStart_DaysEpoch = strtotime(date('Y-m-01', strtotime($dateOfInterest)) . " UTC") / 86400;

        $monthEnd_DaysEpoch = strtotime(date('Y-m-t', strtotime($dateOfInterest)) . " UTC") / 86400;


        //find if the real first pickup of the month is the first or the second of the month
        if (($monthStart_DaysEpoch - $lastPickupDate_DaysEpoch) % 2 == 0) {
            $firstPickupOfMonth_DaysEpoch = $monthStart_DaysEpoch;
        } else {
            $firstPickupOfMonth_DaysEpoch = $monthStart_DaysEpoch + 1;
        }

        //find out if the real last pickup day of the month is the last day or second last day.
        //then find the number of pickups.
        if (($monthEnd_DaysEpoch - $firstPickupOfMonth_DaysEpoch) % 2 == 0) {
            $lastPickupOfMonth_DaysEpoch = $monthEnd_DaysEpoch;
        } else {
            $lastPickupOfMonth_DaysEpoch = $monthEnd_DaysEpoch - 1;
        }

        //plus one counts the first one as a pickup
        $ans = (($lastPickupOfMonth_DaysEpoch - $firstPickupOfMonth_DaysEpoch) / 2) + 1;

        //return number of pickups in a given month.
        return $ans;
    }

   
    /** used to guess what the next milk cheque will be */
    //only works for the previous month when all info is available.
    function milkRevenueProjection($dateOfInterest, $print = true) {
        $SNFRatioLimit = 2.35; //CONST
        //change dateOfInterest to find month
        $partial = date('Y-m', strtotime($dateOfInterest));
        $start_date = $partial . '-01';
        $end_date = date("Y-m-t", strtotime($start_date));


        //load all milk pickups from month
        $sql = "SELECT sum(milk_amount_sold) as total_month_milk, count(milk_amount_sold) as num_pickups FROM batch.milk_pickup_event WHERE event_time::date >= '$start_date'::date AND event_time::date <= '$end_date'::date LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $total_month_milk = $row['total_month_milk'];
        $num_pickups = $row['num_pickups'];


        //find provincial componenet info from that month
        $sql = "With temp as (SELECT DISTINCT (test_time_start) ,test_time_start,fat,protein,lactose 
              FROM batch.nb_bulk_tank_sample WHERE test_time_start::date >= '$start_date'::date AND test_time_finish::date <= '$end_date'::date)
              SELECT avg(fat::numeric) as avg_fat,avg(protein::numeric) as avg_protein,avg(lactose::numeric) as avg_lactose FROM temp limit 1;";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $avg_fat = $row['avg_fat'];
        $avg_protein = $row['avg_protein'];
        $avg_lactose = $row['avg_lactose'];

        //find milk price information
        $sql = "SELECT butterfat,protein,lactose,transport,promotion,admin,lab,research FROM batch.milk_statement WHERE milk_statement.date::date = '$start_date'::date LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        //use the previous month if current month has no information
        if ($res->rowCount() == 0) {
            $sql = "SELECT butterfat,protein,lactose,transport,promotion,admin,lab,research FROM batch.milk_statement WHERE milk_statement.date::date = ('$start_date'::date - interval '1 month')::date LIMIT 1";
            $res = $GLOBALS['pdo']->query($sql);
        }
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $priceFat = $row['butterfat'];
        $priceProtein = $row['protein'];
        $priceLactose = $row['lactose'];
        $priceTransport = $row['transport'];
        $pricePromotion = $row['promotion'];
        $priceAdmin = $row['admin'];
        $priceLab = $row['lab'];
        $priceResearch = $row['research'];

        //find total solids.
        $total_fat_kg = $total_month_milk * $avg_fat * .01;
        $total_protein_kg = $total_month_milk * $avg_protein * .01;
        $total_lactose_kg = $total_month_milk * $avg_lactose * .01;
        $total_solids_kg_actual = $total_fat_kg + $total_protein_kg + $total_lactose_kg;

        //fat corrected for # pickups to days
         if ($total_fat_kg !=0) {
        $fat_kg_day = $total_fat_kg / ($num_pickups * 2);
         }
         else {
           $fat_kg_day =0;  
         }
         if ($total_protein_kg !=0) {
        $prot_kg_day = $total_protein_kg / ($num_pickups * 2);
         }
         else {
            $prot_kg_day=0; 
         }

        //find SNF ratio
        if (!$avg_fat) {
            $SNFRatio = 0; //take care of nulls
        } else {
            $SNFRatio = ($avg_protein + $avg_lactose) / $avg_fat;
        }

        //SNF ratio (activated when over ratio)
        if (($SNFRatio / $SNFRatioLimit) > 1) {
            $newFactor = ($SNFRatio / $SNFRatioLimit) - 1;
            $total_protein_kg = $total_protein_kg - $total_protein_kg * $newFactor;
            $total_lactose_kg = $total_lactose_kg - $total_lactose_kg * $newFactor;
        }

        //revenue
        $rev = $total_fat_kg * $priceFat + $total_protein_kg * $priceProtein + $total_lactose_kg * $priceLactose;
        if ($rev !=0) {
        $rev_day = $rev / ($num_pickups * 2); //over number of pickups, not actual days in the month, because the milk picked up wasn't necessarily produced in that month.
        }
        else {
          $rev_day = 0;   
        }
        //expenses
        $exp = $priceTransport * $total_month_milk + $total_solids_kg_actual * ($pricePromotion + $priceAdmin + $priceLab + $priceResearch);

        //total
        $total = $rev - $exp;

        //output answer
        if ($print == true) {
            print("</br></br></br><hr>");
            print("<h2>Milk Statement Calculation</h2>");
            print("<h3>" . date('F Y', strtotime($start_date)) . "</h3>");
            print("Numer of Pickups in Month: " . $num_pickups . ".</br>");
            print("Fat: " . round($avg_fat, 2) . " % &nbsp;&nbsp;&nbsp;  Protein: " . round($avg_protein, 2) . " % &nbsp;&nbsp;&nbsp; Lactose: " . round($avg_lactose, 2) . " % </br>");
            print("SNF Ratio:" . round($SNFRatio, 2) . "</br>");
            print("Total Fat: " . round($total_fat_kg, 2) . " kg &nbsp;&nbsp;&nbsp;  Protein: " . round($total_protein_kg, 2) . " kg &nbsp;&nbsp;&nbsp; Lactose: " . round($total_lactose_kg, 2) . " kg </br>");
            print("Revenue:$ " . round($rev, 2) . "</br>");
            print("Expenses:$ " . round($exp, 2) . "</br>");
            print("Grand Total:$ " . round($total, 2) . "</br>");
            print("Semi Month Total (Advance):$ " . round($total / 2, 2) . "</br>");
        }

        //return info
        $ret['datetime'] = date('Y-M-d', strtotime($start_date));
        $ret['period'] = date('F Y', strtotime($start_date));
        $ret['numPickups'] = $num_pickups;
        $ret['fat_per'] = round($avg_fat, 2);
        $ret['protein_per'] = round($avg_protein, 2);
        $ret['lactose_per'] = round($avg_lactose, 2);
        $ret['SNF'] = round($SNFRatio, 2);
        $ret['fat_kg'] = round($total_fat_kg, 2);
        $ret['protein_kg'] = round($total_protein_kg, 2);
        $ret['lactose_kg'] = round($total_lactose_kg, 2);
        $ret['fat_kg_day'] = round($fat_kg_day, 2);
        $ret['protein_kg_day'] = round($prot_kg_day, 2);
        $ret['revenue_day'] = round($rev_day, 2);
        $ret['revenue'] = round($rev, 2);
        $ret['expenses'] = round($exp, 2);
        $ret['total'] = round($total, 2);
        $ret['advance'] = round($total / 2, 2);
        return $ret;
    }

    /**
     * Calculations for the milk statement always use the average of
     * 4 tests. Unfortunately at the start of the month we don't have any information 
     * and by mid-month with only maybe have two tests worth.
     * So we'll grab all the information we do have and then make decisions.
     */
    function milkComponenetGuesstimator($dateOfInterest) {

        $avgFat = null;
        $retArr = array();
        $arr = self::milkComponentInfoAtDate($dateOfInterest);

        // print("Date:$dateOfInterest<br>");
        //  print_r2($arr);
        // count the number of rows in the array and take different action
        switch (count($arr)) {
            case 0:
                //when we have zero tests, look up the last months average and use that.
                $date = new DateTime();

                $date = new DateTime($dateOfInterest);
                $date->modify("last day of previous month");
                $lastDayPrevMonth = $date->format("Y-m-d");
                $arrLastMonth = self::milkComponentInfoAtDate($lastDayPrevMonth);
                $avgFat = self::Array2dAvg($arrLastMonth, 'fat');
                break;
            case 1:
                $avgFat = self::Array2dAvg($arr, 'fat');

                break;
            case 2:
                $avgFat = self::Array2dAvg($arr, 'fat');
                break;
            case 3:
                $avgFat = self::Array2dAvg($arr, 'fat');
                break;
            case 4:
                $avgFat = self::Array2dAvg($arr, 'fat');
                break;
            default:
                throw new Exception("Error:Too many milk componements for the given month.<br />\n\r");
                break;
        }

        // print("AvgFat:$avgFat<br><br><br>");
        $retArr['avg_fat'] = $avgFat;
        return $retArr;
    }

    /**
     * used to calculate averages for the componenet test.
     */
    private function Array2dAvg($arr, $indexName) {
        if (empty($arr) == false) {
            $sum = 0;
            $count = 0;
            foreach ($arr as $key => $value) {
                $sum = $sum + $value[$indexName];
                $count++;
            }

            if ($count == 0) {
                return 0;
            } else {
                return $sum / $count; //average    
            }
        } else {
            return null;
        }
    }

    function milkComponentInfoAtDate($dateOfInterest) {
        $arr = array();
        $partial = date('Y-m', strtotime($dateOfInterest));
        $first_date_month = $partial . '-01';
        $last_date_month = date("Y-m-t", strtotime($first_date_month));

        //find all the componenets tests we have for this month.
        $sql = "With temp as (SELECT DISTINCT (test_time_start) ,fat,protein,lactose 
              FROM batch.nb_bulk_tank_sample WHERE test_time_start::date >= '$first_date_month'::date AND test_time_finish::date <= '$last_date_month'::date)
              SELECT * FROM temp;";

        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $arr[$row['test_time_start']]['fat'] = $row['fat'];
            $arr[$row['test_time_start']]['protein'] = $row['protein'];
            $arr[$row['test_time_start']]['lactose'] = $row['lactose'];
        }

        return $arr;
    }

    function milkCreditEstimation() {


       
        //find out what month it is currently 
        $curr_month = date('F', strtotime("now"));


        //only get the last 2 months. 
        $startDate = date("Y-m-d", strtotime("-1 month", strtotime(date("F") . "1")));
        $arr = Quota::showCreditPositionForDateRange($startDate);

        foreach ($arr as $key => $value) {

            $componentArr = self::milkComponenetGuesstimator($value['date']);
            $daily_cred_arrEpoch[strtotime($value['date'])] = $componentArr['avg_fat'] * .01 * $value['pickup'] - $value['quota']; //with incentive days included, always use this
            $daily_cred_arrEpochPermQuota[strtotime($value['date'])] = $componentArr['avg_fat'] * .01 * $value['pickup'] - $value['perm_quota']; //owned quota
            
    
        }


        $totArray = array();
        //now find total credits for each month.
        foreach ($daily_cred_arrEpoch as $key => $value) {

            //only go by month year
            $abrDate = date('Y-F', ($key));
           
            //when it doesn't exist yet, make it 0 first.
           if (!isset($totArray[$abrDate])) {

                $totArray[$abrDate]['Daily Quota (over/under) - w/ incentives'] = 0;
                $totArray[$abrDate]['Daily Quota (over/under) - ignore incentives'] = 0;
                $totArray[$abrDate]['Days in Month'] = 0;
           }


            $totArray[$abrDate]['Date'] = $abrDate;
            $totArray[$abrDate]['Daily Quota (over/under) - w/ incentives'] = round( $value,1);
            $totArray[$abrDate]['Daily Quota (over/under) - ignore incentives'] =round( $daily_cred_arrEpochPermQuota[$key],1);
            $totArray[$abrDate]['Days in Month'] = date('t', ($key));
            $totArray[$abrDate]['Month Credits (over/under) PROJECTED'] = date('t', ($key)) * $totArray[$abrDate]['Daily Quota (over/under) - w/ incentives'];
        }
       // $endMonthCreditPosition = array_sum($daily_cred_arrEpoch);
        
        
        
         print("<h3>Credit Projection Tool</h3>");
        print("note: Includes incentive days<br/>");
           print( (new JQueryDataTable)->startBasicArray(array($totArray[$abrDate])));
        
    }

    /*
     * finds the over or under quota on a daily and monthly basis in liters (for footer)
     * 
     */

    public function calculateLastOverUnderQuota() {
        $AAA = 10; //arbritrary number to give the crew an extra incentive.


        include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/misc/bulkTankPickup.inc');
        include_once($_SERVER['DOCUMENT_ROOT'] . 'template/footer.inc');
//find last milk pickup liters    
        $Footer=new Footer();
        $lastPickupArr = $Footer->findMilkPickupForDateRange(date('Y-m-d H:i:s', strtotime('-5 day')), date('Y-m-d H:i:s', strtotime('now')));
        if ($lastPickupArr == null) {
            print("<h2>ERROR: Latest bulk tank milk pickup amounts have not been entered!!!!</h2>");
            return null;
        }
        $lastPickup = end($lastPickupArr);

//calculate butterfat at last pickup per day
        $BulkTankPickup = new BulkTankPickup();
        $butterfatPickup = $BulkTankPickup->convertLitresMilkToButterFatKg(date('Y-m-d 00:00:00', strtotime('now')), $lastPickup / 2);
//now subtract current quota from that 
        $answer = round($butterfatPickup - $BulkTankPickup->getQuotaHoldingsWithIncentivesOnTodaysDate(), 1);
        $percent = round(($butterfatPickup /$BulkTankPickup->getQuotaHoldingsWithIncentivesOnTodaysDate() * 100) - 100, 1);

        //now convert how much over or under from kg of fat back to liters  
        $fat_per = ($butterfatPickup / ($lastPickup)) * 2;
        $magnitudeLitres = round((($answer - $AAA) / $fat_per)); //FIXME arbitrary number here 10, until incentive days, remove then.


        if ($answer >= 0) {
            $answer = '+' . $answer;
        } //add positive sign if over quota.
        if ($percent >= 0) {
            $percent = '+' . $percent;
        } //add positive sign if over quota.
        //
         if ($magnitudeLitres >= 0) {
            $magnitudeLitres = '+' . $magnitudeLitres;
        }

//calculate percent.
        $ansArr['percent'] = $percent;
        $ansArr['magnitude'] = $answer;
        $ansArr['magnitudeLitres'] = $magnitudeLitres;

        ///////////////////////////////
        //now do for a monthly basis
        ///
        $dataArray = Quota::showCreditPositionForDateRange(date('Y-m-1'));

        //loop from first day of month to current date
        $sumFat = 0;
        $sumLitresOverUnderQuotaMonth = 0;
        // print_r($dataArray);
        for ($x = 1; $x <= date('d'); $x++) {

            //find fat percent for that day 
            if (array_key_exists($x, $dataArray)) { //if no data for day, set fat to zero
                if (($dataArray[$x]['pickup'] != 0) OR ( $dataArray[$x]['pickup'] != null)) {
                    $fat_per = $BulkTankPickup->convertLitresMilkToButterFatKg($dataArray[$x]['date'], $dataArray[$x]['pickup']) / $dataArray[$x]['pickup'];
                } else {
                    $fat_per = 0;
                }
            } else {
                $fat_per = 0;
            }

            if ($fat_per != 0) {
                $sumLitresOverUnderQuotaMonth = $sumLitresOverUnderQuotaMonth + ($BulkTankPickup->convertLitresMilkToButterFatKg($dataArray[$x]['date'], ($dataArray[$x]['pickup'])) - $AAA - $dataArray[$x]['quota']) / $fat_per;
            } else {
                $sumLitresOverUnderQuotaMonth = 0;
            }
        }

        if ($sumLitresOverUnderQuotaMonth >= 0) {
            $magnitudeLitres = '+' . $magnitudeLitres;
        }
        $ansArr['overUnderLitresMonth'] = round($sumLitresOverUnderQuotaMonth);

        return $ansArr;
    }

    public function chartRecentDailyCreditPosition() {

        //plot graph of data.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Credit'] = 'number';

        $priceArr[strtotime($row['date'])] = $row['price'];

        $x = new GoogleVisualizationLine($opt, $headerStrArr, $daily_cred_arrEpoch);
        print($x->toString());
    }

    public function showCreditPositionForDateRange($startDate) {

        $sql = "
With temp as(
/* generate a series of dates to always end at the last day of the current month */
SELECT generate_series('$startDate', (date_trunc('month', now())::date +  '1 month'::interval - '1 day'::interval)::date , interval '1 day') as date_series
), temp2 as (
/* milk pickups only occur every second day.  */
SELECT date_series,
(SELECT total_quota as quota FROM batch.quota_summary WHERE date <= date_series order by date DESC limit 1) as quota,
(SELECT quota as perm_quota FROM batch.quota_summary WHERE date <= date_series order by date DESC limit 1) as perm_quota,
(SELECT milk_amount_sold FROM batch.milk_pickup_event WHERE event_time::date - '1 day'::interval = date_series::date limit 1)/2 as pickup_day1,
(SELECT milk_amount_sold FROM batch.milk_pickup_event WHERE event_time::date = date_series::date limit 1)/2 as pickup_day2,
(SELECT fat FROM batch.nb_bulk_tank_sample WHERE test_time_start::date <= date_series AND test_time_finish::date >= date_series limit 1) as fat
 FROM temp)
SELECT date_series::date,quota,perm_quota, COALESCE (pickup_day1,pickup_day2) as pickup,fat FROM temp2  ORDER BY    date_series           
                ";
        $res = $GLOBALS['pdo']->query($sql);
        $prev_fat = null;
        $prev_pickup = null;
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {



            //if milk pickup is null, take the previous day and use it.

            if ($row['pickup'] == null) {
                $row['pickup'] = $prev_pickup;
            } else {
                $prev_pickup = $row['pickup'];
            }

            //when a fat test is null, use the previous fat test
            if ($row['fat'] == null) {
                $row['fat'] = $prev_fat;
            } else {
                $prev_fat = $row['fat'];
            }


            //////////////////
            $dataArray[$res->rowCount()]['date'] = $row['date_series'];
            $dataArray[$res->rowCount()]['quota'] = $row['quota']; //includes incentive days
            $dataArray[$res->rowCount()]['pickup'] = $row['pickup'];
            $dataArray[$res->rowCount()]['fat'] = $row['fat'];
            $dataArray[$res->rowCount()]['perm_quota'] = $row['perm_quota']; //"owned quota"
        }

        //returns an array with null values removed of future events.
        return $dataArray;
    }

    /**     * ***************************** */
    /**    Enter Milk Statement Entry * */

    /**     * ***************************** */
    function milkStatementQuickform() {


        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForPrev12Months();

        $form = new HTML_QuickForm("milkStatementQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'milkStatementQuickform', "Enter Milk Statement");
        $form->addElement('select', 'dateEvent', 'Month of Statement', $dateArray);
        //
        $form->addElement('text', 'rate_butterfat', 'Butterfat Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_protein', 'Protein Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_lactose', 'Lactose Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_transport', 'Transport Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_promotion', 'Promotion Rate', 'size="6" maxlength="6"');
        $form->addElement('text', 'rate_admin', 'Admin Rate', 'size="6" maxlength="6"');
        $form->addElement('text', 'rate_lab', 'Lab Rate', 'size="6" maxlength="6"');
        $form->addElement('text', 'rate_research', 'Research Rate', 'size="6" maxlength="6"');
        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //defaults (change when there is an increase)
        $form->setDefaults(array('rate_promotion' => '0.1158'));
        $form->setDefaults(array('rate_admin' => '0.0605'));
        $form->setDefaults(array('rate_lab' => '0.0066'));
        $form->setDefaults(array('rate_research' => '0.0016'));


        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('dateEvent', 'Date is required', 'required');
        $form->addRule('rate_butterfat', 'Butterfat rate is required', 'required');
        $form->addRule('rate_butterfat', 'Butterfat rate is required', 'numeric');
        $form->addRule('rate_protein', 'Protein rate is required', 'required');
        $form->addRule('rate_protein', 'Protein rate is required', 'numeric');
        $form->addRule('rate_lactose', 'Lactose rate is required', 'required');
        $form->addRule('rate_lactose', 'Lactose rate is required', 'numeric');
        $form->addRule('rate_transport', 'Transport rate is required', 'required');
        $form->addRule('rate_transport', 'Transport rate is required', 'numeric');
        $form->addRule('rate_promotion', 'Promotion rate is required', 'required');
        $form->addRule('rate_promotion', 'Promotion rate is required', 'numeric');
        $form->addRule('rate_admin', 'Admin rate is required', 'required');
        $form->addRule('rate_admin', 'Admin rate is required', 'numeric');
        $form->addRule('rate_lab', 'Lab rate is required', 'required');
        $form->addRule('rate_lab', 'Lab rate is required', 'numeric');
        $form->addRule('rate_research', 'Research rate is required', 'required');
        $form->addRule('rate_research', 'Research rate is required', 'numeric');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            // 
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d', $tempArray[0]);
            //
            $rate_butterfat = $form->exportValue('rate_butterfat');
            $rate_protein = $form->exportValue('rate_protein');
            $rate_lactose = $form->exportValue('rate_lactose');
            $rate_transport = $form->exportValue('rate_transport') * .01;  //change to hectoliters to liters.
            $rate_promotion = $form->exportValue('rate_promotion');
            $rate_admin = $form->exportValue('rate_admin');
            $rate_lab = $form->exportValue('rate_lab');
            $rate_research = $form->exportValue('rate_research');
            //
            $pageid = $form->exportValue('pageid');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();


                //lookup medicine default dosage and use it for now.
                $res = $GLOBALS['pdo']->query("SELECT date FROM batch.milk_statement WHERE date='$event_time' LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);

                //go ahead and do insert if not already in.
                if ($res->rowCount() == 0) {
                    $query = "INSERT INTO batch.milk_statement (date,butterfat,protein,lactose,transport,promotion,admin,lab,research,userid)
	  VALUES ('$event_time',$rate_butterfat,$rate_protein,$rate_lactose,$rate_transport,$rate_promotion,$rate_admin,$rate_lab,$rate_research,'{$GLOBALS['auth']->getUsername()}')";
                    $res = $GLOBALS['pdo']->exec($query);
                }



                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id={$this->bovine_id}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    /**     * ***************************** */
    /**    Enter Milk Statement Info * */

    /**     * ***************************** */
    function displayListMilkStatementInfo($mode = 'default') {
        
          print((new JQueryDataTable)->startBasicSql("SELECT *,butterfat+protein+lactose as tot_rate,round(butterfat*.0395+protein*.034+lactose*.0572,3) as fixed_blend_price FROM batch.milk_statement ORDER BY date DESC",''));
               
    }

    function quotaChangeQuickform() {

        /*         * ******************************* */
        /* Quoata Change Form */
        /*         * ******************************* */

        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForNextAndPrevFewMonths();

        $quotaEvents = array(null => 'Please Select one...', 'buy_sell' => 'Bought / Sold', 'board_adjustment' => 'Board Adjustment');

        $form = new HTML_QuickForm("quotaHoldingsQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'quotaHoldingsQuickform', 'Record Quota Change Event');

        $form->addElement('select', 'dateEvent', 'Date of Change', $dateArray);

        $form->addElement('select', 'eventType', 'Type of Quota Event', $quotaEvents);

        $form->addElement('text', 'crchange', 'Change in credit (kg)', array('size' => 10, 'maxlength' => 7));

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('dateEvent' => 0));
        $form->setDefaults(array('evenType' => 0));


        //rules

        $form->addRule('dateEvent', 'Date of quota event is required', 'required');
        $form->addRule('eventType', 'Type of quota event is required', 'required');
        $form->addRule('crchange', 'Butter fat quota change amount is required', 'required');
        $form->addRule('crchange', 'A valid number of butter fat change is required.', 'numeric');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $event_type = $form->exportValue('eventType');
            $tempArray = $form->getElementValue('dateEvent');
            $dateEvent = $tempArray[0];
            $event_time = date('Y-m-d H:i:sO', $dateEvent);
            $credit_change = $form->exportValue('crchange');
            $pageid = $form->exportValue('pageid');


            //if it is an adjustment add 1 second to time, so time is unique, adjustments always occur after buy/sells
            if ($event_type == 'board_adjustment') {
                $event_time = date('Y-m-d H:i:01', $dateEvent);
            }

            if (is_numeric($credit_change) != true) {
                throw new Exception('Butter fat amount is not a number, this is already checked, so this should not be called');
            }

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO batch.quota (event_time,event_type,butterfat_change,userid) VALUES ('$event_time','$event_type','$credit_change','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query); {
                print_r($res);
            }

            //print("end insert <br/>");
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation




        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function creditChangeQuickform() {


        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesLastDayMonthForNextAndPrevManyMonths();

        $creditEvents = array(null => 'Please Select one...', 'buy_sell' => '(Bought) / Sold', 'over_under' => 'Over / (Under) for Month', 'above_below' => '(Above) / Below Quota Limit', 'incentive' => '(Incentives) used','over_quota' => '(Over) Quota');

        $form = new HTML_QuickForm("creditHoldingsQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'creditHoldingsQuickform', 'Record credit Change Event');

        $form->addElement('html', 'Note: Buying credits is negative.');

        $form->addElement('select', 'dateEvent', 'Date of Change', $dateArray);

        $form->addElement('select', 'eventType', 'Type of credit Event', $creditEvents);

        $form->addElement('text', 'crchange', 'Change in credit (kg)', array('size' => 10, 'maxlength' => 7));

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('dateEvent' => 0));
        $form->setDefaults(array('evenType' => 0));


        //rules

        $form->addRule('dateEvent', 'Date of credit event is required', 'required');
        $form->addRule('eventType', 'Type of credit event is required', 'required');
        $form->addRule('crchange', 'Credit change amount is required', 'required');
        $form->addRule('crchange', 'A valid number of credit change is required.', 'numeric');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $event_type = $form->exportValue('eventType');
            $tempArray = $form->getElementValue('dateEvent');
            $dateEvent = $tempArray[0];
            $event_time = date('Y-m-d H:i:sO', $dateEvent);
            $credit_change = $form->exportValue('crchange');
            $pageid = $form->exportValue('pageid');




            if (is_numeric($credit_change) != true) {
                throw new Exception('Credit amount is not a number, this is already checked, so this should not be called');
            }

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO batch.credit (event_time,event_type,credit_change,userid) VALUES ('$event_time','$event_type','$credit_change','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query); {
                print_r($res);
            }

            //print("end insert <br/>");
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function displayListOfQuotaHoldings($mode = 'default') {

        print("<h3>Quota Holdings</h3>");
        
        $sql = <<<SQL
SELECT event_time,trim(event_type) as event_type,butterfat_change,userid,(SELECT sum(butterfat_change) FROM batch.quota WHERE event_time<= main.event_time) as to_date_butterfat_holdings
FROM batch.quota as main ORDER BY event_time DESC
SQL;
         print( (new JQueryDataTable)->startBasicSql($sql,'order:false'));
    }

    function printFormattedTime($params) {
        extract($params);
        return date("M jS, Y", strtotime($record['event_time']));
    }

    function printFormattedType($params) {
        extract($params);

        switch ($record['event_type']) {

            case 'buy_sell':
                if ($record['butterfat_change'] >= 0) {
                    $ret = 'Bought';
                } else {
                    $ret = 'Sold';
                }
                break;

            case 'board_adjustment':
                if ($record['butterfat_change'] >= 0) {
                    $ret = 'Positive Adjustment';
                } else {
                    $ret = 'Negative Adjustment';
                }
                break;

            default:
                throw new Exception('Butter fat amount is not a number (2), this is already checked, so this should not be called');
                break;
        }
        return $ret;
    }

    //full registration number of animal needed.
    function displayGraphOfQuotaHoldings() {

        //select the latest laction...
        $query = "SELECT * FROM batch.quota_summary as main ORDER BY date";
        $res = $GLOBALS['pdo']->query($query);
        $bfQuotaArr = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {


            $bfQuotaArr[strtotime($row['date'])] = $row['quota'];
        }



        print("<h3>Butterfat Quota Holdings</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['BF Quota (kg)'] = 'number';
        $x = new GoogleVisualizationLine($opt='', $headerStrArr, $bfQuotaArr);
        print($x->toString());
    }

    //full registration number of animal needed.
    function displayGraphOfQuotaIncentiveHoldings() {

        //select the latest laction...
        $query = "SELECT * FROM batch.quota_summary ORDER BY date";
        $res = $GLOBALS['pdo']->query($query);
        $bfQuotaArr = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {


            $bfQuotaArr[strtotime($row['date'])] = $row['total_quota'];
        }



        print("<h3>Butterfat Quota + Incentive Holdings</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['BF Quota+Incentive (kg)'] = 'number';
        $opt = null;
        $x = new GoogleVisualizationLine($opt, $headerStrArr, $bfQuotaArr);
        print($x->toString());
    }

}

//end class
?>