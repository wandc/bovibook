<?php

class BulkTankPickup extends BasePage {

    public function defaultDisplay() {

          print(new BootStrap)->infoCallout(( "NOTE: Record the last day milk was harvested and put in the specific tank. The collection date is the day the truck came."));
           print("<br/>");
             print("<br/>");
         print(new BootStrap)->infoCallout(( "NOTE2: So that means enter Tank A as the day before the pickup (usually) and Tank B on the day of the pickup (usually)."));
           print("<br/>");
            print("<br/>");
           print(new BootStrap)->infoCallout(( "NOTE3: Enter Tank A first."));
             print("<br/>");
            print("<br/>");
          
         print("<div class='row'>");
        
          print("<div class='col-sm-4 '>");
        
        $a = new QF2_BulkTankPickup((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'individual', (object) array('tankNum' => 'A'));
        print($a->renderBox('qf2_BulkTankPickup_a', 'Record Milk Pickup Tank A'));
            print("</div>");
        
             print("<div class='col-sm-4 '>");
            
        
        $b = new QF2_BulkTankPickup((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'individual', (object) array('tankNum' => 'B'));
        print($b->renderBox('qf2_BulkTankPickup_b', 'Record Milk Pickup Tank B'));

         print("</div>");
         
          print($this->checkPickupNotEnteredForLongTime());
         
         print("</div>");
         
       
        
        $this->displayListOfBulkTankPickups();
        
       // $this->plotBulkTankPickups();
    }

    public function summaryDisplay() {

        //$this->displaySemenInventory('summary');
    }

    /* the goal of this is to see if a milk pickup wasn't entered */
    function checkPickupNotEnteredForLongTime()
    {
       $query = "SELECT current_date-max(collection_date) as difference FROM batch.milk_pickup_event";
        $res = $GLOBALS['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row['difference'] >=4) {
        return  print(new BootStrap)->dangerAlertBasic("Missing Milk Pickup.");
        }
        else {return '';}
    }
    
    
    function deleteQuickForm($milk_pickup_eventID) {

        $form = new HTML_QuickForm("addBulkTankPickupDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'milk_pickup_event_id', $milk_pickup_eventID);
        $form->addElement('submit', 'btnDelete', 'Delete',array('class'=>'btn btn-warning'));



        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $milk_pickup_event_id = $form->getElementValue('milk_pickup_event_id');


            //delete the calving event we originally wanted deleted.
            $res = $GLOBALS['pdo']->query("DELETE FROM batch.milk_pickup_event where id='$milk_pickup_event_id'");


            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**************************************************************************************************************//
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function displayListOfBulkTankPickups() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "harvest_date_raw",
                 "visible": false,
                "searchable": false
            },
                
            {
                data: "harvest_date",
                title: "Milk Harvest Date"
            },
                 {
                data: "collection_date",
                title: "Truck Pickup Date"
            },
            {
                data: "tank_num",
                title: "Tank #"
            },
                
                 {
                data: "milk_amount_sold",
                title: "Milk Picked-up"
            },  
        
            {
                data: "userid",
                title: "Employee"
            },        
            {
                data: "action",
                title: "Action"
            },
                 {
                data: "action2",
                title: "Action 2"
            } 
EX;
        print("<h3>Bulk Tank Pickup List</h3>");
        print($dt->start('Title CHANGE ME', 22, 'listOfBulkTankPickups', null, $columnsStr, null, ' paging: false'));
    }

    function listOfBulkTankPickups() {


        $sql = <<<SQL
SELECT id,harvest_date,collection_date,milk_amount_sold ,tank_num ,userid 
FROM batch.milk_pickup_event  
WHERE harvest_date >= (current_date - interval '2 year')
ORDER BY harvest_date DESC
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['harvest_date_raw'] = strtotime($row['harvest_date']);
            $row['harvest_date'] = date("l, M d Y", strtotime($row['harvest_date']));  //has to be at end.  
            $row['action'] = null;
            $row['action'] = $this->printActionButtons($row['id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['action2']= (new JQueryDataTable())->actionDeleteButton($row['id'], 'batch.milk_pickup_event', 'id',null,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
    
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function printActionButtons($milk_pickup_event_id, $callBackURL = null) {



        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM batch.milk_pickup_event ORDER BY harvest_date DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if ($milk_pickup_event_id == $row['id']) {
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $milk_pickup_event_id, 'schemaTable' => 'batch.milk_pickup_event'));
            $deleteButtonAsString = '' . ($a->renderDelete($milk_pickup_event_id));
        }

        return $deleteButtonAsString;
    }

    function printFormattedTime($params) {
        extract($params);
        return date("l, M d Y", strtotime($record['harvest_date']));
    }

    /////////////////////////////////////////////////////////////////////



    public function convertLitresMilkToButterFatKg($milkPickupDate, $milkSoldLitres) {
        $component = self::findComponentTestOnDate($milkPickupDate);

        $kgOfButterfat = ($component['fat'] * 0.01) * $milkSoldLitres;
        return $kgOfButterfat;
    }

    public function convertLitresMilkToProteinKg($milkPickupDate, $milkSoldLitres) {
        $component = self::findComponentTestOnDate($milkPickupDate);

        $kgOfProtein = ($component['protein'] * 0.01) * $milkSoldLitres;
        return $kgOfProtein;
    }

    public function convertLitresMilkToLactoseKg($milkPickupDate, $milkSoldLitres) {
        $component = self::findComponentTestOnDate($milkPickupDate);

        $kgOfLactose = ($component['lactose'] * 0.01) * $milkSoldLitres;
        return $kgOfLactose;
    }

    private function findComponentTestOnDate($milkPickupDate) {

        $pickupDay = date('j', strtotime($milkPickupDate));
        $pickupMonth = date('n', strtotime($milkPickupDate));
        $pickupYear = date('Y', strtotime($milkPickupDate));

        //determine which test period we are in.
        if (($pickupDay >= 1) && ($pickupDay <= 7)) {
            $period = 1;
        } elseif (($pickupDay >= 8) && ($pickupDay <= 15)) {
            $period = 2;
        } elseif (($pickupDay >= 16) && ($pickupDay <= 23)) {
            $period = 3;
        } elseif (($pickupDay >= 24) && ($pickupDay <= 31)) {
            $period = 4;
        } else {
            throw new error("ERROR: Test period conversion not found, estimated test date out of range.");
        }



        //lookup test results for that paticular pickup.
        $query = "SELECT fat, protein, lactose, scc FROM batch.nb_bulk_tank_sample WHERE EXTRACT(MONTH FROM average_test_time)='$pickupMonth' AND  EXTRACT(YEAR FROM average_test_time)='$pickupYear' and test_period=$period LIMIT 1";
        $res = $GLOBALS['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        //if we didn't return any results. Assume the fat test email hasn't come yet and just use the latest test. This is not very robust and could be innaccurate. FIXME: make more robust.
        if ($row == null) {
            $query = "SELECT fat, protein, lactose, scc FROM batch.nb_bulk_tank_sample ORDER BY average_test_time DESC LIMIT 1";
            $res = $GLOBALS['pdo']->query($query);
            $row = $res->fetch(PDO::FETCH_ASSOC);
        }

        $ret['fat'] = $row['fat'];
        $ret['protein'] = $row['protein'];
        $ret['lactose'] = $row['lactose'];
        $ret['scc'] = $row['scc'];

        return $ret;
    }

    public function getQuotaHoldingsWithIncentivesOnTodaysDate() {
        $query = "SELECT batch.quota_total_on_date(current_date)";
        $res = $GLOBALS['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return $row['quota_total_on_date'];
    }

    function findNumberOfMilkingCowsOnSpecificDate($dateOfInterest) {
        //crazy query to give a list of milking cows on a specific date
        $query = "SELECT number_of_cows FROM batch.daily_number_cows_milking WHERE date = '$dateOfInterest'";
        $res = $GLOBALS['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return $row['number_of_cows'];
    }

    function findAverageDIMOnSpecificDate($dateOfInterest) {
        //crazy query to give a list of milking cows on a specific date
        $query = "SELECT average_dim FROM batch.daily_number_cows_milking WHERE date = '$dateOfInterest'";
        $res = $GLOBALS['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return $row['average_dim'];
    }

    function findMilkPriceForLastYear() {
        //find average price for last 12 months
        $sql = "With temp as (
SELECT butterfat,protein,lactose,transport,promotion,admin,lab,research FROM batch.milk_statement ORDER BY date DESC LIMIT 12
)
SELECT avg(butterfat) as butterfat,avg(protein) as protein,avg(lactose) as lactose,avg(transport) as transport,avg(promotion) as promotion,avg(admin) as admin,avg(lab) as lab,avg(research) as research FROM temp";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $ret['butterfat_avg_price'] = floatval($row['butterfat']);
        $ret['protein_avg_price'] = floatval($row['protein']);
        $ret['lactose_avg_price'] = floatval($row['lactose']);
        $ret['transport_avg_price'] = floatval($row['transport']);
        $ret['promotion_avg_price'] = floatval($row['promotion']);
        $ret['admin_avg_price'] = floatval($row['admin']);
        $ret['lab_avg_price'] = floatval($row['lab']);
        $ret['research_avg_price'] = floatval($row['research']);

        return $ret;
    }

    //full registration number of animal needed.
    function plotBulkTankPickups() {



        //select the latest laction...
        $query = "SELECT id,harvest_date::date,milk_amount_sold,userid FROM batch.milk_pickup_event WHERE harvest_date >= (current_Date - interval '2 years') ORDER BY harvest_date ASC";
        $res = $GLOBALS['pdo']->query($query);


        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $harvest_date = strtotime($row['harvest_date']);
            $xArray[$harvest_date] = $harvest_date;
            $yArray[$harvest_date] = $row['milk_amount_sold'];
            $milkSoldArray[$harvest_date] = $row['milk_amount_sold'];
            $bfArray[$harvest_date] = self::convertLitresMilkToButterFatKg($row['harvest_date'], $row['milk_amount_sold']);
            $protArray[$harvest_date] = self::convertLitresMilkToProteinKg($row['harvest_date'], $row['milk_amount_sold']);
            $lactArray[$harvest_date] = self::convertLitresMilkToLactoseKg($row['harvest_date'], $row['milk_amount_sold']);
        }


        //convert bf data to daily quota by dividing by 2.

        foreach ($bfArray as $key => $val) {
            $bfArray[$key] = round($val / 2, 2);
        }

        foreach ($protArray as $key => $val) {
            $protArray[$key] = round($val / 2, 2);
        }

        foreach ($lactArray as $key => $val) {
            $lactArray[$key] = round($val / 2, 2);
        }

        //convert milk shipped to daily	
        foreach ($milkSoldArray as $key => $val) {
            $milkSoldArray[$key] = round($val / 2);
        }




        //calculate price for every pickup based on fixed price form last 12 months
        $priceArr = self::findMilkPriceForLastYear();

        //find the number of cows milking for each day of the milking (that day and day before)
        //average them together and then find out via bf array the butterfat per cow
        foreach ($xArray as $key => $val) {
            $timeToUseStr = date('Y-m-d 00:00:00', $val);
            $timeToUseStr2 = $currentUnixTime = date('Y-m-d 00:00:00', strtotime("$timeToUseStr -1 day"));
            $cowCount = self::findNumberOfMilkingCowsOnSpecificDate($timeToUseStr);
            // print($cowCount);
            $averageDIM[$key] = round(self::findAverageDIMOnSpecificDate($timeToUseStr), 1);

            $bfPerCowArray[$key] = round($bfArray[$key] / (($cowCount)), 2);
            $protPerCowArray[$key] = round($protArray[$key] / (($cowCount)), 2);
            $lactPerCowArray[$key] = round($lactArray[$key] / (($cowCount)), 2);
            $milkPerCowArray[$key] = round($milkSoldArray[$key] / (($cowCount)), 2);

            //the transport is based on liters, the rest are based on total solids
            $totalSoldids = $bfPerCowArray[$key] + $protPerCowArray[$key] + $lactPerCowArray[$key];
            $revenuePerCowArray[$key] = $bfPerCowArray[$key] * floatval($priceArr['butterfat_avg_price']) + $protPerCowArray[$key] * floatval($priceArr['protein_avg_price']) + $lactPerCowArray[$key] * floatval($priceArr['lactose_avg_price']) - $milkPerCowArray[$key] * $priceArr['transport_avg_price'] - $totalSoldids * $priceArr['promotion_avg_price'] - $totalSoldids * $priceArr['admin_avg_price'] - $totalSoldids * $priceArr['lab_avg_price'] - $totalSoldids * $priceArr['research_avg_price'];
        }


        //find number of cows milking on a specific date for milk sold.		
        foreach ($xArray as $key => $val) {
            $timeToUseStr = date('Y-m-d 00:00:00', $val);
            $timeToUseStr2 = $currentUnixTime = date('Y-m-d 00:00:00', strtotime("$timeToUseStr -1 day"));
            $cowCount = self::findNumberOfMilkingCowsOnSpecificDate($timeToUseStr);
            $milkSoldPerCowArray[$key] = round($milkSoldArray[$key] / (($cowCount)), 2);
        }


        //create an array fo quota for dates
        foreach ($xArray as $key => $val) {
            //slow way to do it.....
            $sqlx = "select batch.quota_total_on_date((SELECT TIMESTAMP 'epoch' + $key * INTERVAL '1 second')::date)";
            $resx = $GLOBALS['pdo']->query($sqlx);
            $rowx = $resx->fetch(PDO::FETCH_ASSOC);

            $bfQuotaArray[$key] = $rowx['quota_total_on_date'] + 10; //FIXME: arbitrarly add 10kg.
        }


        /*
          Suppose your numbers range from a to b, and you want to map them onto the interval from a' to b'. For a < x < b, find
          x'=((b'-a')/(b-a))(x-a) + a'
         */
        //$currentQuotaKgButterfatNormalized=((100-0)/($yDataNorm['labelMax']-$yDataNorm['labelMin']))*($currentQuotaKgButterfat-$yDataNorm['labelMin'])+0;
        /////////////////////////////////////////////////////////////
        // incentive days
        //$incentiveDaysQuota=$this->calculateQuotaLimitWithIncentiveDays(2,$currentQuotaKgButterfat); //CHANGE ME FOR number of Incentive days


        print('<div class="bulkTankPickCurve">');

        print("<h3>Daily Butterfat Quota Filled</h3>");
        $this->butterfatShippedGoogleVisLineChart($bfPerCowArray, $bfArray, $bfQuotaArray);


        print("<h3>Milk Shipped</h3>");
        $this->milkShippedGoogleVisLineChart($milkSoldArray, $milkSoldPerCowArray);

        print("<h3>Average DIM</h3>");
        $this->dimGoogleVisLineChart($averageDIM);

       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            print("<h3>Fat / Protein / Lactose Per Cow</h3>");
            $this->butterfatProteinLactoseGoogleVisLineChart($bfPerCowArray, $protPerCowArray, $lactPerCowArray);
            print("<h3>Revenue over Feed Costs per Cow</h3>");
            //find total feed_costs
            
            $feedCostPerCowArray = self::calculateLactatingFeedCosts();
            $this->revenueAndFeedCostGoogleVisLineChart($revenuePerCowArray, $feedCostPerCowArray);
            
            
            //calculate income over feed cost
            foreach ($revenuePerCowArray as $key => $value) {

                if ($feedCostPerCowArray[$key] == 0) {
                    $incomeOverFeedCostArr[$key] = 0;
                } else {

                    $incomeOverFeedCostArr[$key] = $revenuePerCowArray[$key] / $feedCostPerCowArray[$key];
                }
            }
            $this->revenueOverFeedCostGoogleVisLineChart($incomeOverFeedCostArr);
        }

        print('</div> <!-- endbulkTankPickCurve -->');
    }

    /**
     * Takes your current quota and sets your new limit by using incentive days
     */
    function calculateQuotaLimitWithIncentiveDays($numberOfIncetiveDaysThisMonth, $currentQuotaKgButterfat) {
        $numDaysInMonth = cal_days_in_month(CAL_GREGORIAN, date('n'), date('Y'));

        return $currentQuotaKgButterfat + ($currentQuotaKgButterfat * $numberOfIncetiveDaysThisMonth) / ($numDaysInMonth); //incentive quota
    }

    /** calculates the cost of feed for each group in a range of days and gives a total feed cost for that day */
//FIXME
    function calculateLactatingFeedCosts() {
 

        //this query gets all the mix costs on the date teh mix was created, then uses a series of dates to find mix cost for all dates in range. 
        //note: the plus interval 1 month and minus 1 month are so we don't get nulls at the start. it is still possible to get a null if the receipe did not change. This is to speed up query.
        $sql = "
         WITH temp_high as (
SELECT create_time::date,corrected_kg_day_cost FROM nutrition.nrc_archive_sums WHERE location_id::int=40 AND create_time::date>=(current_date  - interval '2 years' ) AND create_time::date<(current_date   )  ORDER BY create_time desc
), temp_low as (
SELECT create_time::date,corrected_kg_day_cost FROM nutrition.nrc_archive_sums WHERE location_id::int=41 AND create_time::date>=(current_date  - interval '2 years' ) AND create_time::date<(current_date   )  ORDER BY create_time desc
), temp_heifer as (
SELECT create_time::date,corrected_kg_day_cost FROM nutrition.nrc_archive_sums WHERE location_id::int=42 AND create_time::date>=(current_date  - interval '2 years' ) AND create_time::date<(current_date   )  ORDER BY create_time desc
)
, temp_date as(
SELECT misc.generate_dates((current_date - interval '2 years')::date,current_date,1) as dates
), temp3 as (
SELECT dates as date,(SELECT corrected_kg_day_cost FROM temp_high WHERE create_time::date<=temp_date.dates ORDER BY create_time DESC limit 1) as cost_high
,(SELECT corrected_kg_day_cost FROM temp_low WHERE create_time::date<=temp_date.dates ORDER BY create_time DESC limit 1) as cost_low
,(SELECT corrected_kg_day_cost FROM temp_heifer WHERE create_time::date<=temp_date.dates ORDER BY create_time DESC limit 1) as cost_heifer
 FROM temp_date
)
SELECT temp3.*,cost_high+cost_low+cost_heifer as cost_total,total_milking_high,total_milking_low,total_milking_heifer
FROM temp3
LEFT JOIN batch.daily_number_cows_milking ON temp3.date=daily_number_cows_milking.date
        ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $harvest_date = strtotime($row['date']);

            //create weighted feed costs by number in group.          
            $total_milking = $row['total_milking_high'] + $row['total_milking_low'] + $row['total_milking_heifer'];
            if ($total_milking == 0) {
                $totalFeedCostArr[$harvest_date] = 0;
            } else {

                $totalFeedCostArr[$harvest_date] = (($row['total_milking_high'] / $total_milking) * $row['cost_high']) + (($row['total_milking_low'] / $total_milking) * $row['cost_low']) + (($row['total_milking_heifer'] / $total_milking) * $row['cost_heifer']);
            }
        }
        
      
        return $totalFeedCostArr;
    }

    function revenueOverFeedCostGoogleVisLineChart($revenueOverFeedCostPerCowArray) {

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Income / Feed Cost ($/$ ratio)'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $revenueOverFeedCostPerCowArray);
        print($x->toString());
    }

    function revenueAndFeedCostGoogleVisLineChart($revenuePerCowArray, $feedCostPerCowArray) {


        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Revenue ($/cow/day)'] = 'number';
        $headerStrArr['Feed Cost ($/cow/day)'] = 'number';

        $opt = "                  
                        series: {
            0:{ lineWidth: 2, targetAxisIndex: 0 },
            1:{ lineWidth: 2, targetAxisIndex: 1 }
         
                                },
                        chartArea: {width: '90%', height: '80%'},
                        legend: {position: 'in'}";


        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $revenuePerCowArray, $feedCostPerCowArray);
        print($x->toString());
    }

    function butterfatProteinLactoseGoogleVisLineChart($bfPerCowArray, $protPerCowArray, $lactPerCowArray) {


        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Daily Butterfat (kg/cow/day)'] = 'number';
        $headerStrArr['Daily Protein (kg/cow/day)'] = 'number';
        $headerStrArr['Daily Lactose (kg/cow/day)'] = 'number';



        $opt = '';
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $bfPerCowArray, $protPerCowArray, $lactPerCowArray);
        print($x->toString());
    }

    function butterfatShippedGoogleVisLineChart($bfPerCowArray, $bfArray, $bfQuotaArray) {




        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Daily Butterfat (kg/cow)'] = 'number';
        $headerStrArr['Daily Butterfat (kg*cow)'] = 'number';
        $headerStrArr['V Quota (kg)'] = 'number';
        $opt = "                  
                        series: {
            0:{ lineWidth: 2, targetAxisIndex: 0 },
            1:{ lineWidth: 2, targetAxisIndex: 1 },
            2:{ lineWidth: 2, targetAxisIndex: 1 }
                                },
                        chartArea: {width: '90%', height: '80%'},
                        legend: {position: 'in'}";


        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $bfPerCowArray, $bfArray, $bfQuotaArray);
        print($x->toString());
    }

    function milkShippedGoogleVisLineChart($milkSoldArray, $milkSoldPerCowArray) {
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Daily Production (l)'] = 'number';
        $headerStrArr['Daily Production (l/cow)'] = 'number';
        $opt = "                  
                        series: {
            0:{ lineWidth: 2, targetAxisIndex: 0 },
            1:{ lineWidth: 2, targetAxisIndex: 1 }
                                },
                        chartArea: {width: '90%', height: '80%'},
                        legend: {position: 'in'}";


        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $milkSoldArray, $milkSoldPerCowArray);
        print($x->toString());
    }

    function dimGoogleVisLineChart($dimArray) {

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Avg DIM'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $dimArray);
        print($x->toString());
    }

    public function error() {
        $error = array();

        //if milk pickup event has not been entered for 51 hours, throw an error.
        $res = $GLOBALS['pdo']->query("SELECT max(harvest_date) as max FROM batch.milk_pickup_event");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ((strtotime($row['max']) + 51 * 3600) <= strtotime("now")) {
            $error[] = new notifyObj(get_class($this), $row['max'], 1, 'Last milk pickup entry is overdue.');
        }
        return $error;
    }

}

//end class

Class QF2_BulkTankPickup extends QF2_Base {

    function definition($form) {

        
      
        
        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $form->addElement('hidden', 'tankNum')->setValue($this->param->tankNum); //client could change this, we leave it here for debugging, but we don't actually use it. 

       


        //get last date from db
        $lastPickupDate = $this->getLastPickupDate();
        $dateArray = $this->nextFewPickupDates($lastPickupDate);
      

        $form->addElement('static', 'text1')->setValue('<h3> Tank '.$this->param->tankNum.'</h3>');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form->addElement('hidden', 'tank_num')->setValue($this->param->tankNum);
        
       
       $form->addElement('text', 'pickup_amount', null, array('label' => 'Pickup Litres of Tank'))->addRule("required", "Please specify your pickup amount.");      
       $form->addElement('select', 'harvest_date', null, array('options' => $dateArray, 'label' => 'Day Milk Put in Tank'))->addRule("required", "Please specify your day milk was put in the tank.");
       $form->addElement('select', 'collection_date', null, array('options' => $dateArray, 'label' => 'Day Truck Collected Tank'))->addRule("required", "Please specify your day milk was picked up by the milk truck.");
 

       if ($this->param->tankNum=='A') {
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array( 'collection_date' => array_key_last($dateArray)))); //when tank A set collection date to one day past harvest date to make easier. 
       }
       
       
        return $form; //required       
    }


    function process($value) {
        
          if (empty($this->param->tankNum)){
            throw new Exception("Error: param->tankNum not provided!");
        }
        
        
        $value['harvest_date'] = date('Y-m-d', $value['harvest_date']);
        $value['collection_date'] = date('Y-m-d', $value['collection_date']);

        //don't put id value, uses next one in DB.
        $query = "INSERT INTO batch.milk_pickup_event (harvest_date,collection_date,milk_amount_sold,tank_num,userid) VALUES ('{$value['harvest_date']}','{$value['collection_date']}',{$value['pickup_amount']},'{$value['tank_num']}','{$GLOBALS['auth']->getUsername()}')";

        $res = $GLOBALS['pdo']->exec($query);
    }

    //looks up latest pickup date 
    function getLastPickupDate() {
        //grab a list of sires.
        $res = $GLOBALS['pdo']->query("SELECT max(harvest_date) as lastpickup FROM batch.milk_pickup_event");

        $row = $res->fetch(PDO::FETCH_ASSOC);

        return strtotime($row['lastpickup']); //return unix timestamp
    }

    function nextFewPickupDates($lastPickupDate) {

        $dateArray = array();
        $dateStart = ( new DateTime("@$lastPickupDate"));
        
        for ($j = 1; $j <= 2; $j++) {
            $dateStart->add(DateInterval::createFromDateString("+1 days"));
            $dateArray[$dateStart->format("U")] = $dateStart->format("l (M d)");
        }

        return $dateArray;
    }
    
}

//end class
?>