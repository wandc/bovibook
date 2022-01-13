<?php

//keeps track of quota changes.

class Quota extends TabPage {

    public function defaultDisplay() {
        //double check to make sure only admin can see
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            //setup for tabs.
            $tabArray[1]['name'] = 'Dashboard';
            $tabArray[2]['name'] = 'Quota';
            $tabArray[3]['name'] = 'Credit Exchange';
            $tabArray[4]['name'] = 'Credits on Farm';
            $tabArray[5]['name'] = 'Incentive Days';
            $tabArray[6]['name'] = 'Solids Price';
            $tabArray[7]['name'] = 'Revenue';
            $tabArray[8]['name'] = 'Agropur';
            $tabArray[9]['name'] = 'Components';
            $tabArray[10]['name'] = 'P5 Revenue';
            $tabArray[11]['name'] = 'COP Study';

            $this->renderTabs($tabArray);
        }
    }

       protected function tab1() {
           
       }
    
    protected function tab2() {
       $cls=new QuotaHoldings();
       
      
       
       $out[]=(new BootStrap)->oneFifthFourFifth($cls->displayTodaysQuota(),$cls->displayGraphOfQuotaHoldings()); 
       
       $out[]= (new BootStrap)->oneThirdTwoThirds($cls->displayListOfQuotaHoldings($mode = 'default'), $cls->quotaChangeQuickform());       
        
        print(implode($out));
    }

    
     protected function tab3() {
        $cls = new CreditExchange();

       $out[]=(new BootStrap)->oneFifthFourFifth($cls->latestMonthCreditInfo(),$cls->historicalCreditExchangeChart()); 
    
       $out[]= (new BootStrap)->oneThirdTwoThirds($cls->histoicalCreditExchangePriceLog(),$cls->histoicalCreditExchangePriceQuickform());       
        
         print(implode($out));
    }
    
    protected function tab4() {
        $cls = new CreditsOnFarm();

        
        $out[]=(new BootStrap)->twoThirdsOneThird($cls->todaysCreditPosition().$cls->milkCreditEstimation(),$cls->onFarmCreditsChart()); 
        
        
        $left = <<<HTML
             {$cls->creditChangeQuickform()}    
         
       
        HTML;        
        
        
        $out[]= (new BootStrap)->oneThirdTwoThirds($cls->onFarmCreditsPositionHistoricalLog(),$left);       
        
        /*
         
           }
          {}  
         */
        
         $out[]= (new BootStrap)->halfHalf($cls->displayCreditsPurchased(),$cls->displayCreditsPurchasedMarginal());   
       
        
         print(implode($out));
        
        
    }

   

    protected function tab5() {     
           $cls = new IncentiveDays();          
          
      
           
           $out[]=(new BootStrap)->oneFifthFourFifth($cls->currentMonthIncentiveDays(),$cls->displayGraphOfQuotaIncentiveHoldings()); 
        
           $out[]=$cls->incentiveDaysLog();
           
           print(implode($out));
    }

    
    
    protected function tab6() {
     $cls = new SolidsPrice();

      
         
      $out[] = <<<HTML
       {$cls->blendPriceOverTimeChart()}
       {$cls->blendPriceOverTimeChartColumn()}
       {$cls->blendPriceOverTimeChartColumn2()}
       {$cls->solidsGrossPriceOverTimeChart()}
HTML;
       
      $left = <<<HTML
          {$cls->displayListMilkStatementPriceInfo()}
          {$cls->displayListMilkStatementPriceInfoByYear()}
HTML;

       $out[]= (new BootStrap)->oneThirdTwoThirds($left, $cls->milkStatementQuickform());       
       print(implode($out));
    }

    protected function tab7() {
        $cls = new MilkRevenue();
 
        $yearSum = ($cls->calculateLast4YearsYealySumTotalMilkStatement());
        

        
        $out[] = <<<HTML
         {$cls->display4YearsOfLatestHistoricalMilkStatements()}
   HTML;
         
        $out[]= (new BootStrap)->oneThirdTwoThirds(  $yearSum,  $cls->findTheNumberOfPickupDaysForMonth());       
         
     print(implode($out));
    }

    protected function tab8() {
        
        $cls = new AgropurRevenue();
         
        $out[]=(new BootStrap)->halfHalf($cls->agropurYearlyRevenueTable(),''); 
     
           print(implode($out));
    }

    protected function tab9() {
        $cls = new Components();
          
           $out[]=(new BootStrap)->oneFifthFourFifth($cls->todaysMonthlyComponentAverage(),$cls->bulkTankComponentTestPost06_2020()); 
           $out[]=$cls->bulkTankComponentTestPre06_2020();
           $out[]=$cls->displayGraphOfFatKGvsFatPer();
           $out[]=$cls->displayYearlyAvgFatPerCow();
           
                   
           print(implode($out));
    }
    
     protected function tab10() {
         
         
       $cls = new P5_Utilization();
       $a=new QF2_p5RevenueEntry((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
             
        $out[]= (new BootStrap)->oneThirdTwoThirds( $cls->displayP5Revenue(), $cls->checkErrorsFunction().$a->renderBox('QF2_QF2_p5RevenueEntryl','Enter p5 milk data'));     
          
          $out[]= <<<HTML
       {$cls->p5ClassRevenueChart()}
       {$cls->p5ClassRevenueDifferenceOverTimeChart()}
HTML;
       
           print(implode($out));
    }
    
      protected function tab11() {
        
         $cls = new COPStudy();
        
         
          $out[]=(new BootStrap)->halfHalf($cls->displayChartForFixedRealisticComponent(),$cls->displayCOPInfo()); 
     
           print(implode($out));
         
    }

    
    
       /*
        * this must be out of P5 class because of ajax call. FIXME: need a less messy way to call in subclass for page
        */
    public function p5ClassRevenue($request): array {
        
        $date=date('Y-m-d',strtotime($request['item'])); //should filter anything to be safe. maybe bad data, but safe.

        $array = array();

      $yearsAgo=0;
      
        $sql = <<<SQL
 with temp as (
SELECT p5_milk_class_revenue.*,  milk_statement.butterfat, milk_statement.protein, milk_statement.lactose, 3.95 as fat_kg, 3.35 as prot_kg, 5.72 as lactose_kg, 100 as litres
FROM batch.p5_milk_class_revenue 
LEFT JOIN batch.milk_statement ON p5_milk_class_revenue.date=milk_statement.date
WHERE p5_milk_class_revenue.date='$date' 
ORDER BY class DESC
), 

temp2 as(
SELECT *,
fat_kg*butterfat*bf_per_amount/100 as bf_dollar,
(prot_kg*protein+lactose_kg*lactose)* snf_per_amount/100 as snf_dollar
FROM temp
), 

/* used to calculate sum of milk price */
temp3 as (
SELECT *, sum(bf_dollar) over () as bf_dollar_sum, 
sum(snf_dollar) over() as snf_dollar_sum,
bf_dollar+snf_dollar as bf_and_snf_dollars_expense 
FROM temp2
),
temp4 as (
SELECT *,bf_dollar_sum+snf_dollar_sum as total_dollar_sum, revenue_per/100*(bf_dollar_sum+snf_dollar_sum) as revenue_per_normalized_to_blend_price_dollar from temp3 
) SELECT *,(bf_and_snf_dollars_expense/total_dollar_sum)*100 as expense_per	  FROM temp4 ORDER BY class ASC
SQL;
/* NOTE NOTE remove biochlor and Cu when shurgain fixes UOM on spreadsheet. */

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

      
        
     
          
          //transform array
          $tranArr=array();

        //make google viz php format.
       // $array['cols'][] = array('id' => '', 'type' => 'string', 'label' => 'text');   //x axis
         $array['cols'][0] = array('type' => 'string', 'label' => 'class'); //y axis #X   
        
          $array['cols'][1] = array('type' => 'number', 'label' => 'Revenue (as a Percent of Solids Shipped)'); //y axis #X   
       
         $array['cols'][2] = array('type' => 'number', 'label' => 'Farmer Real Value of Milk Shipped (as a Percent of Solids Shipped)'); //y axis #X     
         
        
          
          $counter=0;
            foreach ($results as &$row) {
             //   $tranArr['date'][$counter]='Date('. date('Y',strtotime($row['date'])) . ','.  (intval(date('n',strtotime($row['date']))) - 0)  .',01)';// google format 'Date(year, month, day)'
             
                $tranArr[$counter]['class']=$row['class'];
                   
                $tranArr[$counter]['bf_per_amount']=$row['revenue_per'];
                $tranArr[$counter]['snf_per_amount']=$row['expense_per'];
   
                $counter++;
               }
               

          
        
        
       //put in format for google charts.
        foreach ($tranArr as $key => &$row) {
            $tempArr=array();
              foreach ($row as $key2 => &$row2) {
                 $tempArr[]= array('v' => $row2);
              }
                $array['rows'][]['c'] =$tempArr;
        }
           
     //print_r($array);
        
        return $array;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
      /*
        * this must be out of P5 class because of ajax call. FIXME: need a less messy way to call in subclass for page
        */
    public function p5ClassRevenueDifferenceOverTime($request): array {


        $array = array();

      
        $sql = <<<SQL
with temp as (
SELECT p5_milk_class_revenue.*,  milk_statement.butterfat, milk_statement.protein, milk_statement.lactose, 3.95 as fat_kg, 3.35 as prot_kg, 5.72 as lactose_kg, 100 as litres
FROM batch.p5_milk_class_revenue 
LEFT JOIN batch.milk_statement ON p5_milk_class_revenue.date=milk_statement.date
WHERE p5_milk_class_revenue.date>='2019-01-01' 
ORDER BY date,class DESC
), temp2 as(
SELECT *,
fat_kg*butterfat*bf_per_amount/100 as bf_dollar,
(prot_kg*protein+lactose_kg*lactose)* snf_per_amount/100 as snf_dollar
FROM temp
),
/* used to calculate sum of milk price */
temp3 as (
SELECT *, sum(bf_dollar) over (PARTITION BY date) as bf_dollar_sum, 
sum(snf_dollar) over(PARTITION BY date) as snf_dollar_sum,
bf_dollar+snf_dollar as bf_and_snf_dollars_expense 
FROM temp2 
)
,
temp4 as (
SELECT *,bf_dollar_sum+snf_dollar_sum as total_dollar_sum, revenue_per/100*(bf_dollar_sum+snf_dollar_sum) as revenue_per_normalized_to_blend_price_dollar from temp3 
), temp5 as (

 SELECT *,revenue_per-(bf_and_snf_dollars_expense/total_dollar_sum)*100 as margin ,(bf_and_snf_dollars_expense/total_dollar_sum)*100 as expense_per	  FROM temp4 ORDER BY date,class ASC
), temp6 as (
/* convert to arrays for easier processing. Group by gets order correct for arrays*/
SELECT distinct date, 
(array_agg(class ) OVER (W)) as class_arr,
 (array_agg(revenue_per) OVER (W)) as revenue_arr,  
 (array_agg(expense_per) OVER (W)) as expense_arr,  
 (array_agg(margin) OVER (W))  as margin_arr
FROM temp5 group by date,class,revenue_per,expense_per,margin
WINDOW w AS (PARTITION BY date)
ORDER BY date
) SELECT date, 
array_to_json(class_arr) as class_arr,                 
array_to_json(revenue_arr) as revenue_arr, 
array_to_json(expense_arr) as expense_arr, 
array_to_json(margin_arr) as margin_arr 
FROM temp6
SQL;
/* NOTE NOTE remove biochlor and Cu when shurgain fixes UOM on spreadsheet. */

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

          // var_dump($results);
        
     
        // HEADERS
        //make google viz php format.
        $RetArray['cols'][] = array('id' => '', 'type' => 'string', 'label' => 'date');   //x axis
        foreach (json_decode($results[0]['class_arr']) as &$class) { //asume classes are constant over all DB rows
            $RetArray['cols'][] = array('type' => 'number', 'label' => $class); //y axis #X   
        }
        //END HEADERS
        

        $counter = 0;
        foreach ($results as &$row) {

            //DECODE to array
            $revenue_arr = json_decode($row['revenue_arr']);  //for each month decode the array of price data for the class.       
            $expense_arr = json_decode($row['expense_arr']);  //for each month decode the array of price data for the class. 
            $margin_arr = json_decode($row['margin_arr']);  //for each month decode the array of price data for the class. 
            //DATE is first item
            $tempArr[] = array('v' => $row['date']); //get date from outer loop
            //LOOPS for each json array, margin, revenue, etc.
            foreach ($margin_arr as &$row2) {
                $tempArr[] = array('v' => $row2); //process margin data in innner loop, we could have many of these loops ending up with it serialized. 
            }

            //STORE FOR FINBAL OUTPUT
            $RetArray['rows'][]['c'] = $tempArr;
            $tempArr=null;
            $counter++;
        }
        
        return $RetArray;
    }

    public function summaryDisplay() {
        //nothing
    }

    
  

    /*
     * finds the over or under quota on a daily and monthly basis in liters (for footer)
     * 
     */

    public function calculateLastOverUnderQuota() {
        $AAA = 10; //arbritrary number to give the crew an extra incentive.
        //find the latest daily avg fat kg produced.  
        $sql = <<<SQL
       SELECT date_month, avg_daily_fat_kg FROM batch.monthly_daily_solids order by date_month DESC limit 1 
SQL;
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        //find the monthly virtual quota. 
        $sql2 = <<<SQL2
       SELECT * FROM batch.quota_summary where date=date_trunc('month',current_date)
SQL2;
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);

        //find the latest fat % to then calculate litrs over/under.
        $sql3 = <<<SQL3
        SELECT harvest_date,fat_kg,fat as fat_percentage FROM batch.milk_pickup_solids order by harvest_date DESC limit 1
SQL3;
        $res3 = $GLOBALS['pdo']->query($sql3);
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);

        $ansArr['produced_quota'] = round($row['avg_daily_fat_kg']);
        $ansArr['total_quota'] = round($row2['total_quota']) + $AAA; //add optional crew incentive.
        $ansArr['fat_percentage'] = ($row3['fat_percentage']);
        $ansArr['fat_kg_latest'] = ($row3['fat_kg']);
        return $ansArr;
    }

    public static function showCreditPositionForDateRange($startDate) {

        $sql = "
With temp as(
/* generate a series of dates to always end at the last day of the current month */
SELECT generate_series('$startDate', (date_trunc('month', now())::date +  '1 month'::interval - '1 day'::interval)::date , interval '1 day') as date_series
), temp2 as (
/* milk pickups only occur every second day.  */
SELECT date_series,
(SELECT total_quota as quota FROM batch.quota_summary WHERE date <= date_series order by date DESC limit 1) as quota,
(SELECT quota as perm_quota FROM batch.quota_summary WHERE date <= date_series order by date DESC limit 1) as perm_quota,
(SELECT milk_amount_sold FROM batch.milk_pickup_solids WHERE harvest_date::date = date_series::date limit 1) as pickup_day1,
(SELECT fat_kg FROM batch.milk_pickup_solids WHERE harvest_date::date = date_series::date limit 1) as fat_kg
 FROM temp)
SELECT date_series::date,quota,perm_quota, (pickup_day1) as pickup,fat_kg, avg(fat_kg) OVER (PARTITION BY date_trunc('month',date_series)) as avg_monthly_fat_kg
FROM temp2  ORDER BY    date_series    
                ";
        $res = $GLOBALS['pdo']->query($sql);
        $result = $res->fetchAll(PDO::FETCH_ASSOC);
        
        
        $prev2_fat = null;
        $prev2_pickup = null;
        $prev_fat = null;
        $prev_pickup = null;
        $counter=0;
        
        foreach ($result as $key => $row) {


            //if milk pickup is null, take the previous day  and the one before and use it.


            if ($row['pickup'] == null) {
                $result[$key]['pickup']=($result[$key-1]['pickup'] + $result[$key-2]['pickup'] )/2;
                $row['pickup'] =  $result[$key]['pickup'];
            }

            //when a fat test is null, use the previous fat test
            if ($row['fat_kg'] == null) {
                $result[$key]['fat_kg']=($result[$key-1]['fat_kg'] + $result[$key-2]['fat_kg'] )/2;
                $row['fat_kg'] =  $result[$key]['fat_kg'];  
            }
              


            //////////////////
            $dataArray[$counter]['date'] = $row['date_series'];
            $dataArray[$counter]['quota'] = $row['quota']; //includes incentive days
            $dataArray[$counter]['pickup'] = $row['pickup'];
            $dataArray[$counter]['fat_kg'] = $row['fat_kg'];
            $dataArray[$counter]['over_under_fat_kg'] = $row['fat_kg']-$row['quota'];
            $dataArray[$counter]['perm_quota'] = $row['perm_quota']; //"owned quota"
            $dataArray[$counter]['avg_monthly_fat_kg'] = $row['avg_monthly_fat_kg'];
            $dataArray[$counter]['avg_monthly_fat_kg_vs_all_quota'] = $row['avg_monthly_fat_kg']-$row['quota'];
            $dataArray[$counter]['avg_monthly_fat_kg_vs_perm_quota'] = $row['avg_monthly_fat_kg']-$row['perm_quota'];
            $counter++;
        }
       

        //returns an array with null values removed of future events.
        return $dataArray;
    }

   
}

//end class


class P5_Utilization {
    
    /* display raw class data */
     function displayP5Revenue() {
       $sql = <<<SQL
SELECT    id,date,class,bf_per_amount as "BF %",snf_per_amount as "SNF %",revenue_per as "Revenue %" FROM batch.p5_milk_class_revenue
            ORDER BY update_time DESC,date DESC,class ASC
SQL;
       $statement = $GLOBALS['pdo']->prepare($sql);
       $statement->execute();
       $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $callBackURL=("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        
         $delButton = new QF2_DeleteButtonDecider('anyuser_timeLimit', (object) array('dateTimePeriodDays' => '1', 'dateTimeName' => 'update_time', 'schemaTable' => 'batch.p5_milk_class_revenue'));
        
        //change presentation
        foreach ($results as &$row) {
            
            $rowOut['date'] = date("M Y", strtotime($row['date']));
            $rowOut['class'] =  $row['class'];
            $rowOut['BF %'] =  $row['BF %'];
            $rowOut['SNF %'] =  $row['SNF %'];
            $rowOut['Revenue %'] =  $row['Revenue %'];
            
            
                $rowOut['action'] = $delButton->output($row['id']);
             /*   
             $rowOut['action2'] = (new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'],'deleteName'=>'id', 'schemaTable' => 'batch.p5_milk_class_revenue')))->renderDelete('p5_milk_class_revenue' . $row['id']); 
          
        
            
              if ($GLOBALS['auth']->getAdminMode()==1) {
            $rowOut['action3'] = (new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'],'deleteName'=>'id', 'schemaTable' => 'batch.p5_milk_class_revenue')))->renderDelete($row['id']); 
              }
             if (($record['id'] == $row['id'])) {
              $rowOut['action4'] = (new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'],'deleteName'=>'id', 'schemaTable' => 'batch.p5_milk_class_revenue')))->renderDelete('p5_milk_class_revenue' . $row['id']); 
               }
              else {
               $rowOut['action3'] = '';   
              }
        */
              $resultsOut[] = $rowOut;
              }

       //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('P5 Class Reveue Over Time', $dataArr,$options='searching: false,paging: true,ordering: false,info: false'));
     
        return  implode($out);
       
   }
    
    
    public function p5ClassRevenueDifferenceOverTimeChart() {
         $out=array();
       $js = <<<JS
 <script type="text/javascript">
    
    $(function() {
         google.charts.setOnLoadCallback(drawChartF2); 
    });

      
    // Set a callback to run when the Google Visualization API is loaded.
            $(window).resize(function(){
  google.charts.setOnLoadCallback(drawChartF2);
});
      
    function drawChartF2() {
      var jsonData = $.ajax({
          url: "{$GLOBALS['config']['HTTP']['JSON_API']}61/p5ClassRevenueDifferenceOverTime/",
          dataType:"json",
          async: false
          }).responseText;
          
      // Create our data table out of JSON data loaded from server.
      var data = new google.visualization.DataTable(jsonData);

      // Instantiate and draw our chart, passing in some options.
      var chart = new google.visualization.ColumnChart(document.getElementById('chart_divF2'));
      var options = {
        title: 'Where We lose Money Over Time (P5 Utilization) - 12 Month Period',
        vAxis: {title: 'Margin - Percent of Blend (%)'},
        hAxis: {title: 'Month'},
        pointSize: 5,
         
          series: {
                0: {  lineWidth: 2, pointShape: 'circle' },
            
                
            }
          
      };
      
      
      chart.draw(data, options);
    }

    </script>
     <div id="chart_divF2" style=' height: 900px;'></div>
JS;         
        
         $out[]=$js;
    
    /*
     * 
     */
$html = <<<HTML
Revenue is taken from Milk Matters P5 Utilization Revenue Percent for each class. 
Expenses is taken by assuming 3.95 as butterfat kg, 3.35 as protein kg, 5.72 as lactose kg. Then multiplying these by the percent butterfat solids and percent SNF solids from Milk Producer Magazine P5 Utilization for each class. This gives us the amount of each solid sold into that class over one hectolitre.
We call this "Expenses" for lack of a better word. It is not the expenses for an individual farm, we hope their margin is a bit better, but the expenses as if we were paid for how the market uses our milk. A Dairy Producer is a commodity producer and thus the first litre of milk on the truck and the last litre of milk are the same. 
        
HTML;        
    $body=$html;
    
         $out[]=((new BootStrap)->plainCardCollapsed('Chart Explanation', $body));

          return (new BootStrap())->plainCard('P5 Class Revenue Difference Over Time', implode($out));      
     }
    
     public function p5ClassRevenueChart() {
         
                 //find the latest data in table so we know what month to plot.
  $sql0 = <<<SQL
          SELECT max(date) as max_date FROM batch.p5_milk_class_revenue   
  SQL;      
        $statement0 = $GLOBALS['pdo']->prepare($sql0);
        $statement0->execute();
        $results0 = $statement0->fetchAll(PDO::FETCH_ASSOC);        //get data
        
        $date=date('Y-m-01',strtotime($results0[0]['max_date']));
        $datePretty=date('M Y',strtotime($results0[0]['max_date']));
          
         
        
        
         $out=array();
       $js = <<<JS
 <script type="text/javascript">
    
    $(function() {
         google.charts.setOnLoadCallback(drawChartF); 
    });

      
    // Set a callback to run when the Google Visualization API is loaded.
            $(window).resize(function(){
  google.charts.setOnLoadCallback(drawChartF);
});
      
    function drawChartF() {
      var jsonData = $.ajax({
          url: "{$GLOBALS['config']['HTTP']['JSON_API']}61/p5ClassRevenue/$date",
          dataType:"json",
          async: false
          }).responseText;
          
      // Create our data table out of JSON data loaded from server.
      var data = new google.visualization.DataTable(jsonData);

      // Instantiate and draw our chart, passing in some options.
      var chart = new google.visualization.ColumnChart(document.getElementById('chart_divF'));
      var options = {
        title: 'Where We lose Money (P5 Utilization) - $datePretty',
        vAxis: {title: 'Percent of Blend (%)'},
        hAxis: {title: 'Milk Class'},
        pointSize: 5,
         
          series: {
                0: {  lineWidth: 2, pointShape: 'circle' },
                1: {  lineWidth: 5, pointShape: 'star' }
                
            }
            
      };

      chart.draw(data, options);
    }

    </script>
     <div id="chart_divF" style=' height: 900px;'></div>
JS;         
        
         $out[]=$js;
    
    /*
     * 
     */
$html = <<<HTML
Revenue is taken from Milk Matters P5 Utilization Revenue Percent for each class. 
Expenses is taken by assuming 3.95 as butterfat kg, 3.35 as protein kg, 5.72 as lactose kg. Then multiplying these by the percent butterfat solids and percent SNF solids from Milk Producer Magazine P5 Utilization for each class. This gives us the amount of each solid sold into that class over one hectolitre.
We call this "Expenses" for lack of a better word. It is not the expenses for an individual farm, we hope their margin is a bit better, but the expenses as if we were paid for how the market uses our milk. A Dairy Producer is a commodity producer and thus the first litre of milk on the truck and the last litre of milk are the same. 
        
HTML;        
    $body=$html;
    
         $out[]=((new BootStrap)->plainCardCollapsed('Chart Explanation', $body));

 
 return (new BootStrap())->plainCard('Where We lose Money (P5 Utilization) (One Month)', implode($out));      
         
     }
     
     /* checks to make sure everything sums up in the table too 100% for a month */
     public function checkErrorsFunction() {
         $out=array();
            $sql = <<<SQL
with temp as (
SELECT distinct date,
sum(bf_per_amount) OVER w as bf_per_amount_sum,
sum(snf_per_amount) OVER w as snf_per_amount_sum, 
sum(revenue_per) OVER w as revenue_per_sum
FROM batch.p5_milk_class_revenue 
WINDOW w AS (PARTITION BY date ORDER BY date DESC)
) 
SELECT date,round(bf_per_amount_sum) as bf, round(snf_per_amount_sum) as snf ,round(revenue_per_sum) as  rev FROM temp
SQL;
        $res = $GLOBALS['pdo']->query($sql);
         while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
   
         if (($row['bf']!=100) OR ($row['snf']!=100) OR ($row['rev']!=100)    )   {
             $date=date("M Y",strtotime($row['date']));
             $out[]= (new BootStrap)->errorNotify("Error: For {$date} the sums do not add to 100. BF: ".$row['bf'].' | SNF: '.$row['snf'].' | Rev: '.$row['rev']);
         }}
      
        return implode($out);
    }
     
     
}//end class



class Components {
    
    
    public function latestMonthComponentsAverage() {
           $sql=<<<SQL
       SELECT date_month, avg_daily_fat_kg,avg_daily_protien_kg FROM batch.monthly_daily_solids order by date_month DESC limit 1 
SQL;
         $res = $GLOBALS['pdo']->query($sql); 
      $row = $res->fetch(PDO::FETCH_ASSOC);
      return $row;
    }
    
    
    function todaysMonthlyComponentAverage() {
        $row = $this->latestMonthComponentsAverage();

        $month = date('M, Y', strtotime($row['date_month']));
        $out[] = ("<li>For {$month} </li>");
        $fat_kg = round($row['avg_daily_fat_kg'], 2);
        $prot_kg = round($row['avg_daily_protien_kg'], 2);
        $out[] = ("<li>Avg Kg Fat {$fat_kg} </li>");
        $out[] = ("<li>Avg Kg Prot. {$prot_kg} </li>");

        return (new BootStrap())->plainCard('Current Stats - ' . $month, implode($out));
    }

    function bulkTankComponentTestPre06_2020() {
        $sql=<<<SQL
        SELECT date_trunc('day',average_test_time)::date as "Avg Test Date",test_period as "Period",fat,protein,lactose,scc FROM batch.nb_bulk_tank_sample order by average_test_time DESC   
SQL;
        return ( (new JQueryDataTable)->startBasicSql('Component Tests Pre June 2020', $sql, 'order:false'));
        
    }
    
    function bulkTankComponentTestPost06_2020() {
        
        $sql=<<<SQL
        SELECT 
harvest_date as "Harvest", 	
collection_date	as "Collected",
test_reporting_date as "Rported",	
tank_num as "Tank #",	
milk_amount_sold as "Milk (l)",	
fat as "Fat %",	
protein	as "Protein %",
lactose	as "Lactose %",
scc as "SCC",	
mun as "MUN",	 
fat_kg	as "Fat (kg)",
protein_kg as "Protein (kg)",	
lactose_kg as "Lactose (kg)",	
total_solids_kg as "Total Solids (kg)"
FROM batch.milk_pickup_solids order by harvest_date DESC
SQL;
        return( (new JQueryDataTable)->startBasicSql('Component Tests Post June 2020', $sql, 'order:false'));
        
    }
    
    public function displayYearlyAvgFatPerCow() {
        $sql=<<<SQL
        with temp as (
    SELECT collection_date,
(SELECT number_of_cows FROM batch.daily_number_cows_milking WHERE date=collection_date) as number_of_cows_milking, 
avg(fat) over (PARTITION BY collection_date) as fat_per, 
avg(fat_kg) over (PARTITION BY collection_date) as fat_kg, 
avg(scc) over (PARTITION BY collection_date) as scc, 
avg(milk_amount_sold) over (PARTITION BY collection_date) as milk_litre
     FROM batch.milk_pickup_solids 
ORDER BY collection_date DESC
)
SELECT

 distinct date_part('year',collection_date) as "Year",
round(avg(fat_kg/number_of_cows_milking) over (PARTITION BY date_part('year',collection_date)),2) as "Avg Fat kg/cow" ,
round(avg(fat_kg) over (PARTITION BY date_part('year',collection_date)),2) as "Avg Daily Fat Shipped" ,
round(avg(scc) over (PARTITION BY date_part('year',collection_date)),-3) as "Avg Daily SCC"
FROM temp 
ORDER BY "Year" DESC
SQL;   
    
    
        return ( (new JQueryDataTable)->startBasicSql('Avg Yearly Fat per Cow Sold', $sql, 'order:true')); 
        
        
    }
    
    
    //fat kg graph
    public function displayGraphOfFatKGvsFatPer() {

        $sql=<<<SQL
with temp as (
    SELECT collection_date,(SELECT number_of_cows FROM batch.daily_number_cows_milking WHERE date=collection_date) as number_of_cows_milking, avg(fat) over (PARTITION BY collection_date) as fat_per, avg(fat_kg) over (PARTITION BY collection_date) as fat_kg, avg(milk_amount_sold) over (PARTITION BY collection_date) as milk_litre
     FROM batch.milk_pickup_solids order by collection_date DESC
)
SELECT collection_date,fat_per,fat_kg/number_of_cows_milking as fat_kg_per_cow FROM temp
  
SQL;
    
        $res = $GLOBALS['pdo']->query($sql);
        $bfQuotaArr = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {


            $fatPerArr[strtotime($row['collection_date'])] = $row['fat_per'];
            $fatKgArr[strtotime($row['collection_date'])] = $row['fat_kg_per_cow'];
        }

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Fat (% per cows milk)'] = 'number';
        $headerStrArr['Fat (kg per cow per day)'] = 'number';
        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $fatPerArr,$fatKgArr);
        $out[] = ($x->toString());
         return (new BootStrap())->plainCard('Fat (%) vs Fat (kg) Over Time', implode($out));
    }

    
}//end class







class SolidsPrice {
    
    public function blendPriceOverTimeChart() {
   
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Price ($) BF:4.3%'] = 'number';
        $headerStrArr['Price ($) BF:4.0%'] = 'number';
        $headerStrArr['Price ($) BF:3.7%'] = 'number';
        $opt = " height: '600'  ";
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, self::calculateBlendPrice(4.3), self::calculateBlendPrice(4), self::calculateBlendPrice(3.7));
        $out[]= $x->toString();
        
         return (new BootStrap())->plainCard('Blend Price (Gross w/ fixed components)', implode($out));
    }
        
     public function blendPriceOverTimeChartColumn() {
        //
        $headerStrArr = array();
        $headerStrArr2['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr2['Price ($) BF:4.3%'] = 'number';
        $opt = ' hAxis: { title: "Gross Blend Price over time at 4.3% BF" },   height: 600';
        $x2 = new GoogleVisualizationColumn($opt, $headerStrArr2, self::calculateBlendPrice(888));
        $out[]=($x2->toString());
        return (new BootStrap())->plainCard('Blend Price (Gross w/ fixed components) Column', implode($out));
     }
     
      public function blendPriceOverTimeChartColumn2() {
        //
         $headerStrArr = array();
        $headerStrArr2['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr2['Price ($) BF:4.3%'] = 'number';
        $opt = ' hAxis: { title: "Gross Blend Price over time at 4.3% BF" },   height: 600';
        $x3 = new GoogleVisualizationColumn($opt, $headerStrArr2, self::calculateBlendPrice(4.3));
        $out[]=($x3->toString());
        
       return (new BootStrap())->plainCard('Blend Price (Gross w/ fixed components) Column 2', implode($out));
    }
    
     private function calculateBlendPrice($level = 4.3) {

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
             case 888:
                $sql = "with temp as (
SELECT distinct date_trunc('year',date) as date,avg(butterfat*.0430+protein*.036+lactose*.0572) OVER w as fixed_blend_price 
FROM batch.milk_statement
WINDOW w AS (PARTITION BY date_trunc('year',date))
order by date_trunc('year',date) desc
)
SELECT date,round(fixed_blend_price,3) as fixed_blend_price FROM temp

";
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
    
     public function solidsGrossPriceOverTimeChart() {    
       
        $out[] = (new BootStrap)->infoCallout('Note: butterfat and protein dollars change weighting over time and cows don\'t produce 1:1 BF:Prot, so this is a bit stupid.');
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Gross Solids ($)'] = 'number';
        $headerStrArr['Fit Line'] = 'number';
  
        
        //create a best fit line, hardcode it for now, change some day if milk price ever catches up.....
        
         $bestFit[strtotime('2009-01-01')] = 19.55;
         $bestFit[strtotime('2022-01-01')] = 23;
        
       
        $opt = " height: '600'  ";
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $this->calculateGrossTotalSolidsOverTime(),$bestFit);
        $out[]= $x->toString();
        
        return (new BootStrap())->plainCard('Gross Total Solids over Time', implode($out));
    }
    
    public function calculateGrossTotalSolidsOverTime() {
            $out[] = (new BootStrap)->infoCallout('Fixed Blend: Butterfat 3.95% Protein 3.35% Lactose 5.72%');
        $sql = <<<SQL
                SELECT date,round((butterfat+protein+lactose),3) as gross_solids FROM batch.milk_statement ORDER BY date DESC
SQL;
        
        $res = $GLOBALS ['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            if ($row ['gross_solids'] != 0) {
                $solidsPriceArr[strtotime($row ['date'])] = $row ['gross_solids'];
            }
        }
        return $solidsPriceArr;
    }
    
    
    
     /**     * ***************************** */
    /**    Enter Milk Statement Info * */

    /**     * ***************************** */
    function displayListMilkStatementPriceInfo() {

      
        $out[] = (new BootStrap)->infoCallout('Fixed Blend: Butterfat 3.95% Protein 3.35% Lactose 5.72%');
        $out[] = (new BootStrap)->warningCallout('Expenses transportation uses a fixed value of 100 liters.');
        
        /*
         * transport expense is calcualted per liter. Since we are doing per total solids we can't use this directly. 
         * just multiple by 100 to make fixed and reasonable.
         */
        $sql = <<<SQL
with temp as (
SELECT *,butterfat+protein+lactose as total_gross_rate,
round(butterfat*.0395+protein*.0335+lactose*.0572,3) as fixed_blend_price,
round((transport*100+promotion+admin+lab+research),3) as total_expenses
 FROM batch.milk_statement
)select *,round(avg(fixed_blend_price) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_blend_price from temp ORDER BY date DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDateMonthYear($row['date']);
            $rowOut['Total Gross ($)'] = $row['total_gross_rate'];
            $rowOut['Fixed Expenses ($)'] = $row['total_expenses'];
            $rowOut['BF $'] = $row['butterfat'];
            $rowOut['Prot $'] = $row['protein'];
            $rowOut['lact $'] = $row['lactose'];
            $rowOut['transport'] = $row['transport'];
            $rowOut['promotion'] = $row['promotion'];
            $rowOut['admin'] = $row['admin'];
            $rowOut['lab'] = $row['lab'];
            $rowOut['research'] = $row['research'];
            $rowOut['userid'] = $row['userid'];
            $rowOut['Fixed Blend'] = $row['fixed_blend_price'];
            $rowOut['Yearly Avg Blend'] = $row['yearly_avg_blend_price'];
            
            $sql2="SELECT date as id FROM batch.milk_statement WHERE userid=:currentUserid ORDER BY date DESC LIMIT 1"; //since we use date as key field, need custom SQL. 
            $rowOut['Action'] = (new JQueryDataTable())->actionDeleteButton($row['date'], 'batch.milk_statement', 'id', $sql2, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}", null, null);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Monthly Solids Price Info', $dataArr, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }
    
    
        /**     * ***************************** */
    /**    Enter Milk Statement Info * */

    /**     * ***************************** */
    function displayListMilkStatementPriceInfoByYear() {

     
        $out[] = (new BootStrap)->infoCallout('Fixed Blend: Butterfat 3.95% Protein 3.35% Lactose 5.72%');
        $out[] = (new BootStrap)->warningCallout('Expenses transportation uses a fixed value of 100 liters.');
        
        /*
         * transport expense is calcualted per liter. Since we are doing per total solids we can't use this directly. 
         * just multiple by 100 to make fixed and reasonable.
         */
        $sql = <<<SQL
with temp as (
SELECT *,butterfat+protein+lactose as total_gross_rate,
round(butterfat*.0395+protein*.0335+lactose*.0572,3) as fixed_blend_price,
round((transport*100+promotion+admin+lab+research),3) as total_expenses
 FROM batch.milk_statement
)select distinct date_trunc('year', date) as date,
round(avg(total_gross_rate) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_total_gross_rate ,
round(avg(fixed_blend_price) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_blend_price ,
round(avg(butterfat) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_butterfat,
round(avg(protein) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_protein,
round(avg(lactose) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_lactose,
round(avg(total_expenses) OVER (PARTITION BY date_trunc('year', date)),4) as yearly_avg_total_expenses
FROM temp order by date_trunc('year', date) DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDateYear($row['date']);
            $rowOut['Total Gross ($)'] = $row['yearly_avg_total_gross_rate'];
            $rowOut['Fixed Expenses ($)'] = $row['yearly_avg_total_expenses'];
            $rowOut['BF $'] = $row['yearly_avg_butterfat'];
            $rowOut['Prot $'] = $row['yearly_avg_protein'];
            $rowOut['lact $'] = $row['yearly_avg_lactose'];
            $rowOut['Yearly Avg Blend'] = $row['yearly_avg_blend_price'];
            
          
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Yearly Solids Price Info', $dataArr, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }
    
    
     /**     * ***************************** */
    /**    Enter Milk Statement Entry * */

    /**     * ***************************** */
    public function milkStatementQuickform() {


        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForPrev12Months();

        $form = new HTML_QuickForm("milkStatementQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'milkStatementQuickform', "Enter Milk Statement");
        $form->addElement('select', 'dateEvent', 'Month of Statement', $dateArray);
        //
        $form->addElement('text', 'rate_butterfat', 'Butterfat Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_protein', 'Protein Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_lactose', 'Lactose Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_protein_export', 'Protein Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_lactose_export', 'Lactose Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_transport', 'Transport Rate', 'size="6" maxlength="7"');
        $form->addElement('text', 'rate_promotion', 'Promotion Rate', 'size="6" maxlength="6"');
        $form->addElement('text', 'rate_admin', 'Admin Rate', 'size="6" maxlength="6"');
        $form->addElement('text', 'rate_lab', 'Lab Rate', 'size="6" maxlength="6"');
        $form->addElement('text', 'rate_research', 'Research Rate', 'size="6" maxlength="6"');
        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //defaults (change when there is an increase)
        $form->setDefaults(array('rate_promotion' => '0.1158'));
        $form->setDefaults(array('rate_admin' => '0.0698'));
        $form->setDefaults(array('rate_lab' => '0.0067'));
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
            $rate_protein_export = $form->exportValue('rate_protein_export');
            $rate_lactose_export = $form->exportValue('rate_lactose_export');
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
                    $query = "INSERT INTO batch.milk_statement (date,butterfat,protein,lactose,protein_export,lactose_export,transport,promotion,admin,lab,research,userid)
	  VALUES ('$event_time',$rate_butterfat,$rate_protein,$rate_lactose,$rate_protein_export,$rate_lactose_export,$rate_transport,$rate_promotion,$rate_admin,$rate_lab,$rate_research,'{$GLOBALS['auth']->getUsername()}')";
                    $res = $GLOBALS['pdo']->exec($query);
                }



                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&bovine_id={$this->bovine_id}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }
    
}//end class


class IncentiveDays {
    
    
    
    public function currentMonthIncentiveDays() {
$sql = <<<SQL
                SELECT date,incentive 
FROM batch.incentive_day 
WHERE date_trunc('month',date) = date_trunc('month',current_date) 
LIMIT 1    
SQL;  
         $res = $GLOBALS['pdo']->query($sql);
         $row = $res->fetch(PDO::FETCH_ASSOC);
        
       if (!empty($row)) {
        $month =date('M Y',strtotime($row['date']));
        $out[]= '<h3>'.date('M Y',strtotime($row['date'])).'</h3>';
        $out[]= '<li>'.'Incentive Day '. $row['incentive'].'</li>';
    
        $inCreditUnits=$row['incentive'] * (float)(new CreditsOnFarm)->currentCreditTradeLimits()->quota;
        $out[]= '<li>'.'Equivalent Credits '. $inCreditUnits.'</li>';
       }
       else {
            $out[]= '<li>'.'No Incentive Days'.'</li>';
       }
        
         return (new BootStrap())->plainCard('Current Incentive Days', implode($out));
    }
    
    
     public function displayIncentiveDay() {
        
        $sql = "SELECT id,date,incentive,userid FROM batch.incentive_day ORDER by date DESC";

          $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDateMonthYear($row['date']);
            $rowOut['# Incentive (days)'] = $row['incentive'];
            $rowOut['Userid'] = $row['userid'];
            $rowOut['Action'] = (new JQueryDataTable())->actionDeleteButton($row['id'], 'batch.incentive_day', 'id', null, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}", null, null);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Log', $dataArr, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }

    
    public function incentiveDayQuickform() {

        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForNextAndPrev12Months();

        $form = new HTML_QuickForm("incentiveDayQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
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
                error_log($e->getMessage(), 0);
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    
    public function incentiveDaysLog() {
        $sql=<<<SQL
        SELECT date as "Month" ,incentive as "Incentives (days)" FROM batch.incentive_day ORDER BY date DESC  
SQL;
        return ( (new JQueryDataTable)->startBasicSql('Incentive Days Log', $sql, 'order:false'));
        
    }
    
    //full registration number of animal needed.
    public function displayGraphOfQuotaIncentiveHoldings() {

        //select the latest laction...
        $query = "SELECT * FROM batch.quota_summary ORDER BY date";
        $res = $GLOBALS['pdo']->query($query);
        $bfQuotaArr = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {


            $bfQuotaArr[strtotime($row['date'])] = $row['total_quota'];
        }



   

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['BF Quota+Incentive (kg)'] = 'number';
        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $bfQuotaArr);
        $out[] = ($x->toString());
         return (new BootStrap())->plainCard('Butterfat Quota + Incentive Holdings', implode($out));
    }
}


class QuotaHoldings {

    
    /*
     * Today's Quota
     */
    public function displayTodaysQuota() {
        $sql = <<<SQL
SELECT * FROM batch.quota_summary as main where date=date_trunc('month', current_date)
SQL;
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $month = date('M, Y', strtotime($row['date']));

        $out[] = ("<li>Owned Quota {$row['quota']} </li>");
        $out[] = ("<li>Incentive Quota {$row['quota_incentive']} </li>");
        $out[] = ("<li>Total Quota {$row['total_quota']} </li>");

        return (new BootStrap())->plainCard('Quota - ' . $month, implode($out));
    }

    //full registration number of animal needed.
    public function displayGraphOfQuotaHoldings() {

        //select the latest laction...
        $query = "SELECT * FROM batch.quota_summary as main ORDER BY date";
        $res = $GLOBALS['pdo']->query($query);
        $bfQuotaArr = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            $bfQuotaArr[strtotime($row['date'])] = $row['quota'];
        }

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['BF Quota (kg)'] = 'number';
        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $bfQuotaArr);
        $out[] = ($x->toString());
        
        return (new BootStrap())->plainCard('Quota Holdings Through Time', implode($out));
       
    }

    public function displayListOfQuotaHoldings($mode = 'default') {

      

        $sql = <<<SQL
SELECT id,event_time,trim(event_type) as event_type,butterfat_change,userid,
    (SELECT sum(butterfat_change) FROM batch.quota WHERE event_time<= main.event_time) as to_date_butterfat_holdings
FROM batch.quota as main ORDER BY event_time DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDateMonthYear($row['event_time']);
            $rowOut['Type'] = $this->formattedType($row['event_type'], $row['butterfat_change']);
            $rowOut['BF Δ (kg)'] = ($row['butterfat_change']);           
            $rowOut['User'] = ($row['userid']);
            $rowOut['Action'] = (new JQueryDataTable())->actionDeleteButton($row['id'], 'batch.quota', 'id', null, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}", null, null);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Log', $dataArr,'paging: true,ordering: false,info: true'));
        return implode($out);
    }

    /*
     * show if bought or sold in human readbale and adjusment type.
     */

    private function formattedType($event_type, $butterfat_change) {
        

        switch ($event_type) {

            case 'buy_sell':
                if ($butterfat_change >= 0) {
                    $ret = 'Bought';
                } else {
                    $ret = 'Sold';
                }
                break;

            case 'board_adjustment':
                if ($butterfat_change >= 0) {
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

    public function quotaChangeQuickform() {

        /*         * ******************************* */
        /* Quoata Change Form */
        /*         * ******************************* */

        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForNextAndPrevFewMonths();

        $quotaEvents = array(null => 'Please Select one...', 'buy_sell' => 'Bought / Sold', 'board_adjustment' => 'Board Adjustment');

        $form = new HTML_QuickForm("quotaHoldingsQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

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
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } //end validation




        $form->accept($renderer);
        return $renderer->toHtml();
    }

}

//end class

class CreditsOnFarm {

    public const UPPER_CREDIT_LIMIT = 10; //days
    public const LOWER_CREDIT_LIMIT = -10; //days
    
    public function currentCreditTradeLimits() 
{
      $sql = <<<SQL
       SELECT current_date as date ,batch.quota_on_date(current_date) as quota limit 1
      SQL;
      
       $res = $GLOBALS['pdo']->query($sql); 
       $row = $res->fetch(PDO::FETCH_ASSOC);
       $month =date('M  Y',strtotime($row['date']));
       $out['date']=$month; 
       $out['quota']=$row['quota'];
       $out['upper']=(float)$row['quota']*(float)CreditsOnFarm::UPPER_CREDIT_LIMIT; 
       $out['lower']=(float)$row['quota']*(float)CreditsOnFarm::LOWER_CREDIT_LIMIT; 
       
       return  (object) $out;
}
    
    
    public function todaysCreditPosition() {
        
        $sql = <<<SQL
with temp as (
SELECT  event_time::date,event_type,credit_change 
 , sum(credit_change) OVER ( ORDER BY event_time::date) AS cum_amt
from batch.credit order by event_time DESC
)
SELECT date_trunc('month',event_time) as date_month, cum_amt from temp limit 1
SQL;
      $res = $GLOBALS['pdo']->query($sql); 
      $row = $res->fetch(PDO::FETCH_ASSOC);
        
       $month =date('M t Y',strtotime($row['date_month']));
       $out[]= '<li>'.$month.' Credit Position '. $row['cum_amt'].'</li>';
       

       $out[]= '<li>'.$this->currentCreditTradeLimits()->date.' Lower Credit Limit Credit Limit '. ($this->currentCreditTradeLimits())->lower .'</li>'; 

        //buying limit. 
        $buying= round(abs( ($this->currentCreditTradeLimits())->lower - (float) $row['cum_amt'] )) ;   
        $EightLimit=$this->currentCreditTradeLimits()->quota * 8;      
        $buyingLimit = ($buying > $EightLimit) ? $EightLimit: $buying; //There is an 8 day limit, so we cant trade over that.
        
        $out[]= '<li>'.$this->currentCreditTradeLimits()->date.' Buying Limit Credit Limit '. $buyingLimit.'</li>'; 

       
       
         return (new BootStrap())->plainCard('Current Credit Position', implode($out));
      
    }
    
     
  
    public function milkCreditEstimation() {



        //find out what month it is currently 
        $curr_month = date('F', strtotime("now"));


        //only get the last 2 months. 
        $startDate = date("Y-m-d", strtotime("-1 month", strtotime(date("F") . "1")));
        $arr = Quota::showCreditPositionForDateRange($startDate);

        foreach ($arr as $key => $value) {
      
            $daily_cred_arrEpoch[strtotime($value['date'])] =  $value['over_under_fat_kg']; //with incentive days included, always use this
            $daily_cred_arrEpochPermQuota[strtotime($value['date'])] = $value['fat_kg'] - $value['perm_quota']; //owned quota
            $avg_monthly_fat_kg_vs_all_quota[strtotime($value['date'])] = $value['avg_monthly_fat_kg_vs_all_quota'];
            $avg_monthly_fat_kg_vs_perm_quota[strtotime($value['date'])] = $value['avg_monthly_fat_kg_vs_perm_quota'];
        }


        $totArray = array();
        //now find total credits for each month.
        foreach ($daily_cred_arrEpoch as $key => $value) {

            //only go by month year
            $abrDate = date('Y-F', ($key));
 
            //when it doesn't exist yet, make it 0 first.
            if (!isset($totArray[$abrDate])) {

                $totArray[$abrDate]['Latest Daily Quota (over/under) - w/ incentives'] = 0;
                $totArray[$abrDate]['Latest Daily Quota (over/under) - ignore incentives'] = 0;
                $totArray[$abrDate]['Days in Month'] = 0;
            }
           
            $totArray[$abrDate]['Date'] = '<h3>'.$abrDate.'</h3>';
            $totArray[$abrDate]['Latest Daily Quota (over/under) - w/ incentives'] = round($value, 1);
            $totArray[$abrDate]['Latest Daily Quota (over/under) - ignore incentives'] = round($daily_cred_arrEpochPermQuota[$key], 1);
            $totArray[$abrDate]['Days in Month'] = date('t', ($key));
            $totArray[$abrDate]['Month Credits (over/under) - w/ incentives AVG PROJECTED'] = round(date('t', ($key)) * $avg_monthly_fat_kg_vs_all_quota[$key],1);
            $totArray[$abrDate]['Month Credits (over/under) - ignore incentives AVG PROJECTED'] = round(date('t', ($key)) * $avg_monthly_fat_kg_vs_perm_quota[$key],1);
         }
        
        
        $out[] = (new BootStrap)->infoCallout("Includes incentive days where indicated");
        foreach ($totArray[$abrDate] as $key => $value) {
             $out[] = '<li>'.$key . ' '. $value.'</li>';
        }
  
        
           return (new BootStrap())->plainCard('Credit Position Projection', implode($out));
         
       // $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Credits Estimate', array($totArray[$abrDate])));
        //return implode($out);
    }

    public function onFarmCreditsPositionHistoricalLog() {
       
        $sql =<<<SQL
SELECT id, event_time::date as "Date",event_type as "Type",credit_change as  "# Credit Δ"
 , sum(credit_change) OVER ( ORDER BY event_time::date) AS "Cumalitive Δ"
from batch.credit order by "Date" DESC
SQL;
        
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

         $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'schemaTable' => 'batch.credit'));
        
        //change presentation
        foreach ($results as &$row) {
            
                      
            $rowOut['Date'] = date("M Y", strtotime($row['Date']));  //has to be at end.      
            $rowOut['Type'] = $row['Type'];
            $rowOut['# Credit Δ'] = $row['# Credit Δ'];
            $rowOut['Cumalitive Δ'] = $row['Cumalitive Δ'];
            $rowOut['Action'] = $delButton->output($row['id']);

            
              $resultsOut[]=$rowOut;  
        }
        //output
        $dataArr = $resultsOut;
   
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('On Farm Credit Position Historical Log', $dataArr, 'searching: false,ordering: false,info: false'));
        return implode($out);
    }

    public function onFarmCreditsChart() {
        $sql = "SELECT  event_time::date,event_type,credit_change 
 , sum(credit_change) OVER ( ORDER BY event_time::date) AS cum_amt
from batch.credit order by event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $priceArr[strtotime($row['event_time'])] = $row['cum_amt'];
        }

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Credits (kg)'] = 'number';
        $opt = ' pointSize: 8';
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $priceArr);
        $out[] = ($x->toString());
   
        return (new BootStrap())->plainCard('On Farm Credits Position', implode($out));
      
    }

    public function displayCreditsPurchased() {
    


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
            
                      
            $rowOut['Date'] = date("M Y", strtotime($row['date']));  //has to be at end.           
            $rowOut['Price ($)'] = $row['price'];
            $rowOut['# Credits'] = $row['credits'];
            $rowOut['Amount Purchased ($)'] = $row['amount_purchased'];
            //$rowOut['Marginal Yearly Price ($)'] = $row['marginal_yearly_credit_price'];
            
              $resultsOut[]=$rowOut;  
        }
        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Credits Purchased Dollars', $dataArr, 'searching: false,ordering: false,info: false'));

        
        //2nd chart from same SQL.
        
        
        
        return implode($out);
    }
    
    public function displayCreditsPurchasedMarginal() {
       


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
SELECT distinct date_trunc('year',date) as date,round((sum(amount_purchased) OVER (PARTITION BY date_trunc('year',date))),2) as amount_purchased_year,round((sum(weighting)  OVER (PARTITION BY date_trunc('year',date))),2) as marginal_yearly_credit_price from temp2
 ORDER BY date DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            
                      
            $rowOut['Date'] = date("Y", strtotime($row['date']));  //has to be at end.    
            $rowOut['Amount Purchased ($)'] = $row['amount_purchased_year'];
            $rowOut['Marginal Yearly Price ($)'] = $row['marginal_yearly_credit_price'];
            
              $resultsOut[]=$rowOut;  
        }
        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Yearly Marginal Purchase Price', $dataArr, 'paging:false,searching: false,ordering: false,info: false'));

        
        //2nd chart from same SQL.
        
        
        
        return implode($out);
    }
    
    

    //on farm credit position change form
    public function creditChangeQuickform() {


        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesLastDayMonthForNextAndPrevManyMonths();

        $creditEvents = array(null => 'Please Select one...', 'buy_sell' => '(Bought) / Sold', 'over_under' => 'Over / (Under) for Month', 'above_below' => '(Above) / Below Quota Limit', 'incentive' => '(Incentives) used', 'over_quota' => '(Over) Quota');

        $form = new HTML_QuickForm("creditHoldingsQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

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
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

}

//end class

class CreditExchange {
    
    
    public function latestMonthCreditInfo() {
         $sql = "SELECT date,price,cleared,offered FROM batch.credit_exchange ORDER BY date DESC limit 1";
         $res = $GLOBALS['pdo']->query($sql);
         $row = $res->fetch(PDO::FETCH_ASSOC);
        
        $month =date('M Y',strtotime($row['date']));
        $out[]= '<h3>'.date('M Y',strtotime($row['date'])).'</h3>';
        $out[]= '<li>'.'Price $'.$row['price'].'</li>';
        $out[]= '<li>'.'Amount Cleared '. $row['cleared'].'</li>';
        $out[]= '<li>'.'Amount Offered '. $row['offered'].'</li>';
        
         return (new BootStrap())->plainCard('Credit Exchange - ' . $month, implode($out));
    }
    
     public function historicalCreditExchangeChart() {
        $sql = "SELECT * FROM batch.credit_exchange ORDER by date DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $priceArr[strtotime($row['date'])] = $row['price'];
            $clearedArr[strtotime($row['date'])] = $row['cleared'];
        }
        
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Credit Clearing Price ($)'] = 'number';
        $headerStrArr['Amount Cleared'] = 'number';
        
        
        /*
         * sets up 2 axis for line and bars on same chart.
         */
          $chartOptions = <<<OPT
                  
        curveType: 'function',
        
        hAxis: {
            title: "Time"
        },         
        vAxes: {
                0: {
                // options for left y-axis
                title: 'Price ($)'
                },
                1: {
                // options for right y-axis
                title: '# of Credits Traded'
                }
        },
        series: {
            0: {
                type: "line",
                targetAxisIndex: 0,
                lineWidth: 5,          
            },
            1: {
                type: "bars",
                targetAxisIndex: 1, 
            }
        } 
        OPT;
               
        $x = new GoogleVisualizationLine( null, $chartOptions, $headerStrArr, $priceArr,$clearedArr);
        $out[] = ($x->toString());
      
         return (new BootStrap())->plainCard('Credit Exchange Through Time', implode($out));
    }

    public function histoicalCreditExchangePriceQuickform() {

        //get 1st of the month dates
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForPrev12Months();

        $form = new HTML_QuickForm("milkCreditExchangeQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
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
                return "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    public function histoicalCreditExchangePriceLog() {
        $sql = "SELECT * FROM batch.credit_exchange ORDER by date DESC";
        $out[] = ( (new JQueryDataTable)->startBasicSql('Credit Exchange Log', $sql, 'searching: false,ordering: false,info: false'));
        return implode($out);
    }

}//end class



/*

___  ____ _ _     ______                                
|  \/  (_) | |    | ___ \                               
| .  . |_| | | __ | |_/ /_____   _____ _ __  _   _  ___ 
| |\/| | | | |/ / |    // _ \ \ / / _ \ '_ \| | | |/ _ \
| |  | | | |   <  | |\ \  __/\ V /  __/ | | | |_| |  __/
\_|  |_/_|_|_|\_\ \_| \_\___| \_/ \___|_| |_|\__,_|\___|
                                                        
                  
 * 
 */
class MilkRevenue {
    
    public $COPYearArr;
    
    
    function __construct() {
       $this->customSQL();
    }

    private function customSQL() {
        //create a temp tables to speed things up. //this view is called alot and is slow.
        $sql = "CREATE TEMP TABLE IF NOT EXISTS temp_monthly_daily_solids AS  (SELECT * FROM batch.monthly_daily_solids)";
        $res = $GLOBALS['pdo']->query($sql);
        
        //do a query to see which years we have for COP study.
        $sql2 = "SELECT extract(year from year)::integer as year FROM batch.cop_study ORDER By year"; 
        $res2 = $GLOBALS['pdo']->query($sql2);
        $arr = $res2->fetchAll(PDO::FETCH_COLUMN, 0);
        $this->COPYearArr=$arr;
    }

    //from https://stackoverflow.com/questions/5464919/find-a-matching-or-closest-value-in-an-array
    //find closest value in array of numbers
    private function getClosest($search, $arr) {
        $closest = null;
        foreach ($arr as $item) {
            if ($closest === null || abs($search - $closest) > abs($item - $search)) {
                $closest = $item;
            }
        }
        return $closest;
    }

    //returns BF and SNF in $/kg from COP study of paticular year
    //these values can then be multiplied by what was produced to see if we made COP study amount.
    public function calculateCOPStudyKG($year) {
        
      
        
        //use the year provided or whatever is closest. 
        $closestYear=$this->getClosest($year, $this->COPYearArr);
        $yearMod=$closestYear.'-01-01';
        
               //find provincialCOP infor for that year
        $sql2 = "SELECT * FROM batch.cop_study WHERE year='$yearMod'::date limit 1;"; 
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        
        
        $ret=array();
        $ret['bf_dollars_kg']=($row2['amount'] *  $row2['bf_per_market']*.01) /   $row2['bf_kghl']; 
        $ret['snf_dollars_kg']=($row2['amount'] *  $row2['snf_per_market']*.01) /   $row2['snf_kghl'];      
        return $ret;
    }
    
    
    
    /*
     * this returns the last 4 years of milk statement data
     * 
     */

    public function calculateLast4YearsYealySumTotalMilkStatement() {
        $dataArr = array();

        //get current year
        $year = (new DateTime)->format("Y");
        $year = $year;
        //loop through last 4.
        for ($x = $year; $x >= $year - 8; $x--) {
            $dataArr[] = $this->calculateSpecifcYearsTotalMilkStatement($x);
        }

        
         foreach ($dataArr as &$row) {
           
            $rowOut['Year'] = $row['year'];
            $rowOut['COP vs Real Revenue $']=$row['COP vs Real Revenue $'];

        $rowOut['fat_kg'] =  $row['fat_kg'];
        $rowOut['protein_kg'] =  $row['protein_kg'] ;
        $rowOut['lactose_kg'] =   $row['lactose_kg'];
        $rowOut['SNF_Ratio'] =  $row['SNF_Ratio'];
        $rowOut['revenue'] = $row['revenue'];
        $rowOut['total'] =  $row['total'];
        
        
         $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Yearly Revenue', $dataArr, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }

    private function calculateSpecifcYearsTotalMilkStatement($year = null) {
        if (empty($year)) {
            throw new Exception("Error: valid year needed");
        }
        $xArr = array();
        $retArr = array();


        //fancy new code to make monlthy intervals for a year.
        $start = new DateTime($year . '-01-01');
        $interval = new DateInterval('P1M'); //monthly
        $end = new DateTime($year . '-12-31');
        $period = new DatePeriod($start, $interval, $end);
        foreach ($period as $dt) {
            $xArr[] = $this->milkRevenueProjection($dt->format('Y-m-01'), false);
        }

        $retArr['year'] = $year;
        $retArr['fat_kg'] = round(array_sum(array_column($xArr, 'fat_kg')), 2);
        $retArr['protein_kg'] = round(array_sum(array_column($xArr, 'protein_kg')), 2);
        $retArr['lactose_kg'] = round(array_sum(array_column($xArr, 'lactose_kg')), 2);
        $retArr['SNF_Ratio'] =  round(($retArr['protein_kg']+ $retArr['lactose_kg']) / $retArr['fat_kg'],3);
        $retArr['revenue'] = round(array_sum(array_column($xArr, 'revenue')), 2);
        $retArr['total'] = round($retArr['revenue'] / $retArr['fat_kg'], 2);
        
        //calculate COP stuff
        $copArr=$this->calculateCOPStudyKG($retArr['year']);
        $snf_kg=$retArr['protein_kg']+ $retArr['lactose_kg'];
        $COPRevDollar=round($copArr['bf_dollars_kg']*$retArr['fat_kg'] + $copArr['snf_dollars_kg']*$snf_kg,2);
        $retArr['COP vs Real Revenue $']= round($retArr['revenue']-$COPRevDollar,1);
        
        //$RR2['Year']= $retArr['year'];
        //$RR2['COP vs Real Revenue $']=  $retArr['COP vs Real Revenue $'];
        
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

     // displays milk statements
    function display4YearsOfLatestHistoricalMilkStatements() {

       

        $retArr = array(); //show 48 months.
        for ($x = 0; $x >= -48; $x--) {
            $retArr[] = $this->milkRevenueProjection(date('Y-m-01', strtotime("$x month")), false);
        }
        //var_dump( $retArr);
        //change presentation
        foreach ($retArr as &$row) {

            if($row['fat_kg']!=0) { //only do math if we actually have some data, usually 1st of month, no data. 
                
            //  $rowOut['Date'] = JQueryDataTable::formattedDate($row['datetime']);
            $rowOut['Period'] = $row['period'];
            $rowOut['# Pickups'] = $row['numPickups'];
            $rowOut['Fat %'] = $row['fat_per'];
            $rowOut['Protein %'] = $row['protein_per'];
            $rowOut['Lactose %'] = $row['lactose_per'];
            $rowOut['SNF Ratio'] = $row['SNF'];
            $rowOut['Fat (kg)'] = $row['fat_kg'];
            $rowOut['Protein (kg)'] = $row['protein_kg'];
            $rowOut['Lactose (kg)'] = $row['lactose_kg'];
            //industry info for comparing to milk classses
            $rowOut['SNF (kg)'] = round($row['protein_kg']+$row['lactose_kg'],1);
            $rowOut['BF (% of TS)'] = round(( ($row['fat_kg'])/($row['protein_kg']+$row['lactose_kg']+$row['fat_kg']) )*100,1);
            $rowOut['SNF (% of TS)'] = round((($row['protein_kg']+$row['lactose_kg'])/($row['protein_kg']+$row['lactose_kg']+$row['fat_kg']))*100,1); 
            //daily info
            $rowOut['Fat (kg/day)'] = $row['fat_kg_day'];
            $rowOut['Protein (kg/day)'] = $row['protein_kg_day'];
            $rowOut['Revenue ($/day)'] = $row['revenue_day'];
             //COP
            $copArr=$this->calculateCOPStudyKG('2019');
            $row['COP Revenue $']=round($copArr['bf_dollars_kg']*$row['fat_kg'] + $copArr['snf_dollars_kg']*$rowOut['SNF (kg)'],2);
            $rowOut['COP vs Real Rev $']= round($row['revenue']-$row['COP Revenue $'],1);
            //so far this month revenue
            $rowOut['Revenue $'] = $row['revenue'];
            $rowOut['Expenses $'] = $row['expenses'];
            $rowOut['Total $'] = $row['total'];
            $rowOut['Advance $'] = $row['advance'];


            $resultsOut[] = $rowOut;
            }
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Historical Milk Statements', $dataArr, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }

    
    //used by milk revenue projection to get statement price
    private function milkRevenueProjectionStatementPrice($start_date) {
 
    
        //find milk price information
        //use the previous month if current month has no information
        $sql = "SELECT date,butterfat,protein,lactose,protein_export,lactose_export,transport,promotion,admin,lab,research FROM batch.milk_statement WHERE milk_statement.date::date <= ('$start_date'::date )::date ORDER BY date DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
 
        
        if (empty($row)) {
            throw new Exception("Error: Can't display information because we do not have recent milk statement information entered for date: ".$start_date.'.');
        }
        
     
        
        $ret['date'] = $start_date;
        $ret['priceFat'] = $row['butterfat'];
        $ret['priceProtein'] = $row['protein'];
        $ret['priceLactose'] = $row['lactose'];
        $ret['priceProteinExport'] = $row['protein_export'];
        $ret['priceLactoseExport'] = $row['lactose_export'];
        $ret['priceTransport'] = $row['transport'];
        $ret['pricePromotion'] = $row['promotion'];
        $ret['priceAdmin'] = $row['admin'];
        $ret['priceLab'] = $row['lab'];
        $ret['priceResearch'] = $row['research'];
        
        return $ret;
    }
    
    
    /* SNF revenue calculation for protein and lactose, taking ratios into account and changes in payment over time. */
    /* used by milk revenue to find $$ for protein and lactose */
    function SNFRevenueCalculation($SNFRatio, $SNFDomesticLimt, $SNFExportLimt, $SNFRatioLimit, $total_protein_kg, $total_lactose_kg, $statementPriceArr) {
        $ret = array();
        
        //this is zero when we are doing a projection in the future, we could just use an average or last months to generate a value....
        if ($SNFRatio==0){
        $ret['date']=$statementPriceArr['date']; //for debug    
        $ret['total_protein_dollars'] =0;
        $ret['total_lactose_dollars'] = 0;
        return $ret;   
            
        }
       
        //when we have two protein / lactose prices (after feb 1 2021)
        if (is_numeric($SNFExportLimt) == true) {

            //for domestic protein & lactose 0 to 2.0
            if ($SNFRatio < $SNFDomesticLimt) {
                //full payment domestic. 
                $total_protein_dollars = ($SNFDomesticLimt / $SNFRatio) * $total_protein_kg * $statementPriceArr['priceProtein'];
                $total_lactose_dollars = ($SNFDomesticLimt / $SNFRatio) * $total_lactose_kg * $statementPriceArr['priceLactose'];
            } elseif (($SNFRatio >= $SNFDomesticLimt) && ($SNFRatio < $SNFExportLimt)) {
                //full payment for the part between 2 and 2.3 and export price for the rest. 
                $total_protein_dollars = ($SNFDomesticLimt / $SNFRatio) * $total_protein_kg * $statementPriceArr['priceProtein'] + (1 - $SNFDomesticLimt / $SNFRatio) * $total_protein_kg * $statementPriceArr['priceProteinExport'];
                $total_lactose_dollars = ($SNFDomesticLimt / $SNFRatio) * $total_lactose_kg * $statementPriceArr['priceLactose'] + (1 - $SNFDomesticLimt / $SNFRatio) * $total_lactose_kg * $statementPriceArr['priceLactoseExport'];
            } elseif (SNFRatio >= $SNFExportLimt) {
                //full payment plus export payment plus zero payment.
                $total_protein_dollars = ($SNFDomesticLimt / $SNFRatio) * $total_protein_kg * $statementPriceArr['priceProtein'] + ($SNFExportLimt / $SNFRatio - $SNFDomesticLimt / $SNFRatio) * $total_protein_kg * $statementPriceArr['priceProteinExport'] + ($SNFExportLimt / $SNFRatio) * $total_protein_kg * 0;
                $total_lactose_dollars = ($SNFDomesticLimt / $SNFRatio) * $total_lactose_kg * $statementPriceArr['priceLactose'] + ($SNFExportLimt / $SNFRatio - $SNFDomesticLimt / $SNFRatio) * $total_lactose_kg * $statementPriceArr['priceLactoseExport'] + ($SNFExportLimt / $SNFRatio) * $total_lactose_kg * 0;
            }
        }
        //before Feb 1 2021
        else {

            //SNF ratio (activated when over ratio)
            if (($SNFRatio / $SNFRatioLimit) > 1) {
                $newFactor = ($SNFRatio / $SNFRatioLimit) - 1;
                $toBePaidTotalProteinKg = $total_protein_kg - $total_protein_kg * $newFactor;
                $toBePaidTotalLactoseKg = $total_lactose_kg - $total_lactose_kg * $newFactor;

                $total_protein_dollars = $toBePaidTotalProteinKg * $statementPriceArr['priceProtein'];
                $total_lactose_dollars = $toBePaidTotalLactoseKg * $statementPriceArr['priceLactose'];

                //when under ratio.
            } else {

                $total_protein_dollars = $total_protein_kg * $statementPriceArr['priceProtein'];
                $total_lactose_dollars = $total_lactose_kg * $statementPriceArr['priceLactose'];
            }
        }

        //return $$$$
        $ret['date']=$statementPriceArr['date']; //for debug   
        $ret['total_protein_dollars'] = $total_protein_dollars;
        $ret['total_lactose_dollars'] = $total_lactose_dollars;


        return $ret;
    }

    /** used to guess what the next milk cheque will be */
    //only works for the previous month when all info is available.
    //for one month.
    function milkRevenueProjection($dateOfInterest, $print = true) {
      
        //switched to daily butterfat tests after June 1 2020 and started tracking by day.
        if (strtotime($dateOfInterest) >= strtotime('2020-06-01')) {
            $dailyPickup=true;
        }
        else {
              $dailyPickup=false;
        }
        
        //SNF changed to 2.0 for full pay and 2.0 to 2.3 export pay Feb 1 2021. 
        if (strtotime($dateOfInterest) >= strtotime('2021-02-01')) { 
              $SNFRatioLimit = 2.0; 
        }
        //SNF changed from 2.35 to 2.3 after Feb 1 2020. 
        elseif (strtotime($dateOfInterest) >= strtotime('2020-02-01')) { 
              $SNFRatioLimit = 2.30; 
        } else {
              $SNFRatioLimit = 2.35; //CONST
        }
        
           //Switched to SNF export price after Feb 1 2021.
        if (strtotime($dateOfInterest) >= strtotime('2021-02-01')) { 
              $SNFExportLimt = 2.3; 
               $SNFDomesticLimt = 2.0; 
        } else {
              $SNFExportLimt = false; //CONST
              $SNFDomesticLimt = false; 
        }
        
        
        
        //change dateOfInterest to find month
        $partial = date('Y-m', strtotime($dateOfInterest));
        $start_date = $partial . '-01';
        $end_date = date("Y-m-t", strtotime($start_date));
        
        


        //load all milk pickups from month
        $sql = "SELECT sum(milk_amount_sold) as total_month_milk, count(milk_amount_sold) as num_pickups FROM batch.milk_pickup_event WHERE collection_date >= '$start_date'::date AND collection_date <= '$end_date'::date LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $total_month_milk = $row['total_month_milk'];
        $num_pickups = $row['num_pickups'];

 
        //switched to daily butterfat tests after June 1 2020 and started tracking by day.
        //DB view monthly_daily_solids supports pre and post June 2020.
       
               //find provincial componenet info from that month
        $sql2 = "SELECT * FROM temp_monthly_daily_solids WHERE date_month::date='$dateOfInterest'::date limit 1;"; 
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        
        //pickups were 15 or so in a month in the past, this catches it and doubles it.
        $multiplier=1;
        if ( $num_pickups <= 18) {
            $multiplier=2;
        } 
        if (empty($row2)) {
           
        $avg_fat =0;
        $avg_protein = 0;
        $avg_lactose = 0;
        $total_fat_kg = 0;
        $total_protein_kg = 0;
        $total_lactose_kg = 0;
        $total_solids_kg_actual = 0;
                      
        }else {
        
        $avg_fat = $row2['avg_daily_fat_per'];
        $avg_protein = $row2['avg_daily_protein_per'];
        $avg_lactose = $row2['avg_daily_lactose_per'];
        $total_fat_kg = $row2['avg_daily_fat_kg']*$num_pickups*$multiplier;
        $total_protein_kg = $row2['avg_daily_protien_kg']*$num_pickups*$multiplier;
        $total_lactose_kg = $row2['avg_daily_lactose_kg']*$num_pickups*$multiplier;
        $total_solids_kg_actual = $total_fat_kg + $total_protein_kg + $total_lactose_kg;
        }
        
        //fat corrected for # pickups to days
        if ($total_fat_kg != 0) {
          $fat_kg_day =  $total_fat_kg / ($num_pickups * $multiplier);
        } else {
            $fat_kg_day = 0;
        }
        if ($total_protein_kg != 0) {
             $prot_kg_day =  $total_protein_kg / ($num_pickups * $multiplier);
        } else {
            $prot_kg_day = 0;
        }

        //find actual SNF ratio
        if (!$total_fat_kg) {
            $SNFRatio = 0; //take care of nulls
        } else {
            $SNFRatio = ($total_protein_kg + $total_lactose_kg) / $total_fat_kg;
        }
        
        
       
       //revenue
        $statementPriceArr=$this->milkRevenueProjectionStatementPrice($start_date);      
     
        //SNF revenue calculation for protein and lactose, taking ratios into account and changes in payment over time.
        $snfArr=$this->SNFRevenueCalculation($SNFRatio, $SNFDomesticLimt, $SNFExportLimt, $SNFRatioLimit, $total_protein_kg, $total_lactose_kg, $statementPriceArr);
   
        
        $rev = $total_fat_kg * $statementPriceArr['priceFat'] + $snfArr['total_protein_dollars'] +  $snfArr['total_lactose_dollars'] ;
        if ($rev != 0) {
            //over number of pickups, not actual days in the month, because the milk picked up wasn't necessarily produced in that month.
            //over days after june 1 2020
              $rev_day = $dailyPickup ? $rev / ($num_pickups* $multiplier) : $rev / ($num_pickups * $multiplier);
       
        } else {
            $rev_day = 0;
        }
        //expenses
        $exp = $statementPriceArr['priceTransport'] * $total_month_milk + $total_solids_kg_actual * ($statementPriceArr['pricePromotion'] + $statementPriceArr['priceAdmin'] + $statementPriceArr['priceLab'] + $statementPriceArr['priceResearch']);

        //total
        $total = $rev - $exp;

        //output answer
       if ($print == true) {
            $out[] = ("</br></br></br><hr>");
            $out[] = ("<h2>Milk Statement Calculation</h2>");
            $out[] = ("<h3>" . date('F Y', strtotime($start_date)) . "</h3>");
            $out[] = ("Numer of Pickups in Month: " . $num_pickups . ".</br>");
            $out[] = ("Fat: " . round($avg_fat, 2) . " % &nbsp;&nbsp;&nbsp;  Protein: " . round($avg_protein, 2) . " % &nbsp;&nbsp;&nbsp; Lactose: " . round($avg_lactose, 2) . " % </br>");
            $out[] = ("SNF Ratio:" . round($SNFRatio, 2) . "</br>");
            $out[] = ("Total Fat: " . round($total_fat_kg, 2) . " kg &nbsp;&nbsp;&nbsp;  Protein: " . round($total_protein_kg, 2) . " kg &nbsp;&nbsp;&nbsp; Lactose: " . round($total_lactose_kg, 2) . " kg </br>");
            $out[] = ("Revenue:$ " . round($rev, 2) . "</br>");
            $out[] = ("Expenses:$ " . round($exp, 2) . "</br>");
            $out[] = ("Grand Total:$ " . round($total, 2) . "</br>");
            $out[] = ("Semi Month Total (Advance):$ " . round($total / 2, 2) . "</br>");
           
            return implode($out);
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
    
    // finds the number of pickups in the current and next month(s).
    function findTheNumberOfPickupDaysForMonth() {
       
       

        $sql = "SELECT max(collection_date) as max_event_time FROM batch.milk_pickup_event LIMIT 1";
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

        $pickupDays = ("$monthOne: $monthOnePickupDays | $monthTwo:$monthTwoPickupDays ");


        //esitmate revenue for current month, by using last months data
        $ret = $this->milkRevenueProjection(date('Y-m-01', strtotime("-1 month")), false);
        $revHalfEstimate = round(($monthOnePickupDays * ($ret['revenue'] / $ret['numPickups'])) / 1,2);

        $estimatedFirstPayment = round($revHalfEstimate + ($revHalfEstimate - $ret['advance']),2);
        $estimatedSecondPayment = $revHalfEstimate;

      $name = ("Estimated $monthOne Statment");
        $out[] = '<ul>';
        $out[] = (new BootStrap)->infoCallout("Note: Assumes the same production/componenets/price as month before");
        $out[] ='<br/>';
         $out[] = ("<li>Pickup Days in Future Months: $pickupDays</li>");
        $out[] = ("<li>Estimated $monthTwo 16 Payment: $ $estimatedFirstPayment</li>");
        $out[] = ("<li>Normal Payment: $ $revHalfEstimate</li>");
        $out[] = ("<li>Previous Advance: $ {$ret['advance']}</li>");
        $out[] = ("<li>Estimated $monthTwo 30 Payment: $ $estimatedSecondPayment</li>");
         $out[] = '</ul>';
        
          return (new BootStrap)->plainCard($name, implode($out));
        
       
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

   

    
}//end class


class COPStudy {
    
  public function displayCOPInfo() {
      
   
        $sql = "SELECT * FROM batch.cop_study ORDER by year DESC";
        $out[] = ( (new JQueryDataTable)->startBasicSql('COP Study', $sql, 'searching: false,ordering: false,info: false'));
        return implode($out);    
  }  
             

  public function copForFixedRealisticComponent() {
      
     
       
        $retArr=array();
        
        //find provincialCOP infor for that year
        $sql = "SELECT * FROM batch.cop_study  ORDER BY year ASC"; 
        $res = $GLOBALS['pdo']->query($sql);
       
        while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            //year	amount	bf_kghl	snf_kghl	bf_per_market	snf_per_market
        

            $dol_bf=($row['amount']*$row['bf_per_market']*.01)/$row['bf_kghl'];
            $dol_snf=($row['amount']*$row['snf_per_market']*.01)/$row['snf_kghl'];        
            

            // $out[] = (new BootStrap)->infoCallout('Fixed Blend: Butterfat 4.3% Protein 3.35% Lactose 5.72%');
            $answer=$dol_bf*.043+$dol_snf*(.035+.0572); //realistic component numbers
            //$answer=$dol_bf*.043+$dol_snf*(.089177); //Fred's numbers.
          
            $retArr[date('Y',strtotime($row['year']))]=$answer;        
        }
        
       return $retArr;
  }
          
  
  function displayChartForFixedRealisticComponent() {
       $out[] = (new BootStrap)->infoCallout('Fixed Blend: Butterfat 4.3% Protein 3.35% Lactose 5.72%');
       
      $retArr=$this->copForFixedRealisticComponent();
      
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['COP Price ($)'] = 'number'; 

        
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string';
        $headerStrArr['Confirmation'] = 'number';


        $opt = "                  
                     
                         width:700, height:700,
                          legend: {position: 'none'},
          vAxis: {title: 'COP ($)',  titleTextStyle: {color: 'red'}}
          ";
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $retArr);
        $out[]= $x->toString();
        
        
        return (new BootStrap())->plainCard('Fixed Realistic Components', implode($out));
      
  }
  
  
  
  
  
  
    
}//end class


/**
 
  ___                                   
 / _ \                                  
/ /_\ \ __ _ _ __ ___  _ __  _   _ _ __ 
|  _  |/ _` | '__/ _ \| '_ \| | | | '__|
| | | | (_| | | | (_) | |_) | |_| | |   
\_| |_/\__, |_|  \___/| .__/ \__,_|_|   
        __/ |         | |               
       |___/          |_|               

 */
class AgropurRevenue {
    
     public function agropurYearlyRevenueTable() {
     $dataArr = array();

        //get current year
        $year = (new DateTime)->format("Y");
        //loop through last 4.
        for ($x = $year; $x >= $year - 4; $x--) {
            $dataArr[] = $this->calculateSpecifcAgropurYearsTotalMilkStatement($x);
        }

         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('To Oct 31 Agropur Year-End', $dataArr, ''));
         return implode($out);
    }
    
    
      /* agropur year runs from Nov 1 previous year to Oct 31 of current year */

    public function calculateSpecifcAgropurYearsTotalMilkStatement($year = null) {
        if (empty($year)) {
            throw new Exception("Error: valid year needed");
        }
        $xArr = array();
        $retArr = array();


        //fancy new code to make monlthy intervals for a year.
        $start = new DateTime($year - 1 . '-11-01');
        $interval = new DateInterval('P1M'); //monthly
        $end = new DateTime($year . '-10-31');
        $period = new DatePeriod($start, $interval, $end);
        foreach ($period as $dt) {
            $xArr[] = (new MilkRevenue)->milkRevenueProjection($dt->format('Y-m-01'), false);
        }

        $retArr['year'] = $year;
        $retArr['fat_kg'] = round(array_sum(array_column($xArr, 'fat_kg')),2);
        $retArr['protein_kg'] = round(array_sum(array_column($xArr, 'protein_kg')),2);
        $retArr['lactose_kg'] = round(array_sum(array_column($xArr, 'lactose_kg')),2);
        $retArr['total_solids'] = round($retArr['fat_kg'] + $retArr['protein_kg'] + $retArr['lactose_kg'],2);


        return $retArr;
    }
    
} // end class


/*
 * 
 * entry for p5 chart from last page of milk producer magazine.
 */
Class QF2_p5RevenueEntry extends QF2_Base {

    
    public $classList;
    
    /* stupid PHP and how it handles cosntructors, use this hack instead */
    public function classConstruct() {
       //list of milk classes
        $this->classList = array(null => 'Please Select one...',
            0 => '1-a1',
            1 => '1-b',
            2 => '2-a',
            3 => '2-b4',
            4 => '2-b5',
            5 => '3-a1',
            6 => '3-a2',
            7 => '3-b2',
            8 => '3-c1',
            9 => '3-c2',
            10 => '3-c4',
            11 => '3-c6',
            12 => '3-d',
            13 => '4-a',
            14 => '4-d',
            15 => '5-a',
            16 => '5-b',
            17 => '5-c',
            18 => '5-d');
        
    }
    
    function definition($form) {
       
        $this->classConstruct();
          
        $dateArray = $GLOBALS['MiscObj']->createMonthlyDatesForPrev12Months(); //prev 12 months
    
     
        

        //find last entry, so we can set defaults to same month, but next class in order. 
        $sql = "SELECT  * FROM batch.p5_milk_class_revenue ORDER by create_time DESC limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
      
        $defaultDate=(strtotime( $row['date']));  
        
        
        /* find next value in class array to use as new default */
        $currentKey=array_search($row['class'], $this->classList);
        if ($currentKey < sizeof( $this->classList)) {
            $defaultClass= $currentKey+1;
        } else {
            //overflow
            $defaultClass= null;
        }
        
       
     
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your month");
        $form->addElement('select', 'class', null, array('options' =>  $this->classList, 'label' => 'Milk Class'))->addRule("required", "Please specify your milk class");        
        $form->addElement('text', 'bf_per_amount',  array('rows' => 6))->setLabel("BF class %")->addRule('regex', 'BF must be a decimal number','/^-?[0-9]+(\.[0-9]+)?$/'); //decimal number
        $form->addElement('text', 'snf_per_amount',  array('rows' => 6))->setLabel("SNF class %")->createRule('regex', 'SNF must be a decimal number','/^-?[0-9]+(\.[0-9]+)?$/'); //decimal number
        $form->addElement('text', 'revenue_per',  array('rows' => 6))->setLabel("Revenue class %")->createRule('regex', 'Revenue must be a decimal number','/^-?[0-9]+(\.[0-9]+)?$/'); //decimal number
  
        
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
    'dateEvent' => $defaultDate,
    'class' => $defaultClass
)));
       
        
  return $form; //required   
    }

   
    
    
    function process($value) {

        $this->classConstruct();
         $class=$this->classList[$value['class']]; //using index get class name back. 

        $value['date'] = date('Y-m-1', $value['dateEvent']);
        $res = $GLOBALS['pdo']->prepare("INSERT INTO batch.p5_milk_class_revenue (date,class,bf_per_amount,snf_per_amount,revenue_per) VALUES (:date,:class,:bf_per_amount,:snf_per_amount,:revenue_per)");
        $res->bindParam(':date', $value['date']);
        $res->bindParam(':class', $class);
        $res->bindParam(':bf_per_amount', $value['bf_per_amount']);
        $res->bindParam(':snf_per_amount', $value['snf_per_amount']);
        $res->bindParam(':revenue_per', $value['revenue_per']);
        
        $res->execute();       
        // $res->debugDumpParams();
                 
    }

}//end class   


?>
