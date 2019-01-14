<?php

//

//require_once($_SERVER['DOCUMENT_ROOT'].'/functions/croppingHelper.inc');
;


/**
  Test page to try new things with.
 */
class ShurgainReport extends BasePage {

    function defaultDisplay() {
        
  
        $this->shurGainYearChart();
        print("<h3>Note: Totals with tax need tax added on, not included in sum.</h3>");
        $this->displayShurGainOneMonth(strtotime("-1 month"));
        $this->displayShurGainOneMonth(strtotime("-2 month"));
        $this->displayShurGainOneMonth(strtotime("-3 month"));
    }

    
    
    public function shurGainYearly() {
        
          $sql = <<<SQL
        with temp as (
SELECT  distinct (item_num,unit_measure_corrected) as index ,item_desc,event_time,price_total FROM batch.shur_gain_invoice_data 
) 
SELECT distinct(index,date_trunc('year',event_time)) as index1, index ,item_desc,date_trunc('year',event_time)as year,sum(price_total) OVER(PARTITION BY index,date_trunc('year',event_time)) as price_sum_item_year,    
sum(price_total) OVER(PARTITION BY date_trunc('year',event_time)) as price_sum_overall_year    
FROM temp order by  year,item_desc
        
SQL;
         
          
          
         
        
    }


    
    public function shurGainOneMonth($request) {
       
        $dateStart= date('Y-m-01',($request['item']));
        $dateEnd=   date('Y-m-d', strtotime('+1 month', strtotime($dateStart))); 
        
        
        $sql = <<<SQL
 with temp as (
SELECT distinct (item_num,uom), item_desc,sum(mass) as mass_sum,sum(price_total) as price_sum,tax_code FROM batch.shur_gain_invoice_data where event_time >= '$dateStart' and event_time < '$dateEnd' group by item_num,uom,item_desc,tax_code
)
SELECT *,(SELECT sum(price_sum) from temp) as overall_sum from temp 
SQL;
        
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
         
        }

        //output
        $output['data'] = $results;
        return ($output);
        
    }
    
    
     function displayShurGainOneMonth($dateOfInterest) {
         
         print("<H1> Monthly Purchased Feed: ". date('M Y',$dateOfInterest) ."</h1>");
         
         
         
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                         
            {
                data: "item_desc",
                title: "item_desc"
               
            },
            {
                data: "mass_sum",
                title: "mass_sum"
            },
            
            {
                data: "price_sum",
                title: "price_sum"
             
            },
            {
                data: "tax_code",
                title: "tax_code"
            },
             {
                data: "overall_sum",
                title: "overall_sum"
            }
EX;
        print($dt->start($this->pageid, 'shurGainOneMonth', $dateOfInterest, $columnsStr));

        //    print($dt->startLocal($this->AllHeats7Days(''),$columnsStr,null,' "paging": false'));     
    }
    
    
    
    
    public function shurGainYearChart() {
         ?>
          <!--Load the AJAX API-->
  
   
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
          url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php echo($this->pageid);?>/shurGainMonthlyPurchases/",
          dataType:"json",
          async: false
          }).responseText;
          
      // Create our data table out of JSON data loaded from server.
      var data = new google.visualization.DataTable(jsonData);

      // Instantiate and draw our chart, passing in some options.
      var chart = new google.visualization.ColumnChart(document.getElementById('chart_divF'));
      var options = {
          
        height: 800,
        legend: { position: 'top', maxLines: 12 },
        bar: { groupWidth: '75%' },
        isStacked: true,
        title: 'ShurGain This Year',         
        hAxis: {
                  format: 'MM/yyyy',
                  title: 'Date', 
                },
        vAxis: {
                  title: 'Price (total $)'
                }, 
        };
      
      
      chart.draw(data, options);
    }

    </script>
     <div id="chart_divF" class="chartGoogle"></div>
    
    <?php 
     }
    
    
    
    public function shurGainMonthlyPurchases($request): array {


        $array = array();

      $yearsAgo=1;
      
        $sql = <<<SQL
 /* NOTE:  this makes an assumption with the unest of 'unit of measure' that they all array units are the same. this seems to be the case */
/* NOTE2: this is the main part it takes flat file and breaks it up by sums of price and mass per month. The problem is that unit_measure all needs to be put into one unit,ie kg. */

WITH temp as (
SELECT 
  index, item_desc,unit_measure,

 sum(case when date_part('month', event_time) = '01'::double precision then "price_sum" else 0 end)  as "month_1_price",
 sum(case when date_part('month', event_time) = '01'::double precision then "mass_sum" else 0 end)  as "month_1_mass",
sum(case when date_part('month', event_time) = '02'::double precision then "price_sum" else 0 end)  as "month_2_price",
 sum(case when date_part('month', event_time) = '02'::double precision then "mass_sum" else 0 end)  as "month_2_mass",
sum(case when date_part('month', event_time) = '03'::double precision then "price_sum" else 0 end)  as "month_3_price",
 sum(case when date_part('month', event_time) = '03'::double precision then "mass_sum" else 0 end)  as "month_3_mass",
sum(case when date_part('month', event_time) = '04'::double precision then "price_sum" else 0 end)  as "month_4_price",
 sum(case when date_part('month', event_time) = '04'::double precision then "mass_sum" else 0 end)  as "month_4_mass",
sum(case when date_part('month', event_time) = '05'::double precision then "price_sum" else 0 end)  as "month_5_price",
 sum(case when date_part('month', event_time) = '05'::double precision then "mass_sum" else 0 end)  as "month_5_mass",
sum(case when date_part('month', event_time) = '06'::double precision then "price_sum" else 0 end)  as "month_6_price",
 sum(case when date_part('month', event_time) = '06'::double precision then "mass_sum" else 0 end)  as "month_6_mass",
sum(case when date_part('month', event_time) = '07'::double precision then "price_sum" else 0 end)  as "month_7_price",
 sum(case when date_part('month', event_time) = '07'::double precision then "mass_sum" else 0 end)  as "month_7_mass",
sum(case when date_part('month', event_time) = '08'::double precision then "price_sum" else 0 end)  as "month_8_price",
 sum(case when date_part('month', event_time) = '08'::double precision then "mass_sum" else 0 end)  as "month_8_mass",
sum(case when date_part('month', event_time) = '09'::double precision then "price_sum" else 0 end)  as "month_9_price",
 sum(case when date_part('month', event_time) = '09'::double precision then "mass_sum" else 0 end)  as "month_9_mass",
sum(case when date_part('month', event_time) = '10'::double precision then "price_sum" else 0 end)  as "month_10_price",
 sum(case when date_part('month', event_time) = '10'::double precision then "mass_sum" else 0 end)  as "month_10_mass",
sum(case when date_part('month', event_time) = '11'::double precision then "price_sum" else 0 end)  as "month_11_price",
 sum(case when date_part('month', event_time) = '11'::double precision then "mass_sum" else 0 end)  as "month_11_mass",
sum(case when date_part('month', event_time) = '12'::double precision then "price_sum" else 0 end)  as "month_12_price",
 sum(case when date_part('month', event_time) = '12'::double precision then "mass_sum" else 0 end)  as "month_12_mass"

FROM (
  SELECT distinct (item_num,unit_measure_corrected) as index,event_time, item_desc,sum(mass_corrected) as mass_sum,sum(price_total) as price_sum ,unit_measure_corrected as unit_measure
  FROM batch.shur_gain_invoice_data  
  WHERE date_part('year', event_time) =  date_part('year', current_date - interval '$yearsAgo year') 
  GROUP BY item_num,unit_measure_corrected,item_desc,event_time 
) p
GROUP BY index,item_desc,unit_measure
)
/* adds arrays for easier downstream processing */
SELECT *,        array_to_json(ARRAY[month_1_mass,month_2_mass,month_3_mass,month_4_mass,month_5_mass,month_6_mass,month_7_mass,month_8_mass,month_9_mass,month_10_mass,month_11_mass,month_12_mass]) as mass_arr_json,
   array_to_json(ARRAY[month_1_price,month_2_price,month_3_price,month_4_price,month_5_price,month_6_price,month_7_price,month_8_price,month_9_price,month_10_price,month_11_price,month_12_price]) as price_arr_json,
       array_to_json((with temp as (SELECT (date_trunc('year', current_date) + (interval '1' month * generate_series(0,11))) as x) select array_agg(x) from temp )) as date_arr_json
   
 FROM temp where item_desc != 'Bio-Chlor' AND item_desc !=  'Cu sulphate 21% Cu acidified' AND item_desc !='Silage Bag B/W 8.0M 10X300'
SQL;
/* NOTE NOTE remove biochlor and Cu when shurgain fixes UOM on spreadsheet. */

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

       // var_dump($results);
        
     

        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'string', 'label' => 'date');   //x axis
          foreach ($results as &$row) {
        $array['cols'][] = array('type' => 'number', 'label' => $row['item_desc']); //y axis #X   
          }
   
          //transform array
          $tranArr=array();
          
            for ($x = 0; $x <= 11; $x++) {
          foreach ($results as &$row) {
           
        $date_arr=json_decode($row['date_arr_json']); //dates for 12 months of specific thing, ie APF fat.  
        $price_arr=json_decode($row['price_arr_json']); //prices for 12 months of specific thing, ie APF fat.   
        $mass_arr=json_decode($row['mass_arr_json']); //prices for 12 months of specific thing, ie APF fat.   
      
        $tranArr[$date_arr[$x]]['date']='Date('. date('Y',strtotime($date_arr[$x])) . ','.  (intval(date('n',strtotime($date_arr[$x]))) - 1)  .',01)';// google format 'Date(year, month, day)'
        $tranArr[$date_arr[$x]][$row['item_desc'].' '.$row['index']]=$price_arr[$x];
      
            }}
        
      // var_dump($tranArr);
        
       //put in format for google charts.
        foreach ($tranArr as $key => &$row) {
            $tempArr=array();
              foreach ($row as $key2 => &$row2) {
                 $tempArr[]= array('v' => $row2);
              }
                $array['rows'][]['c'] =$tempArr;
        }
           
        return $array;
    }

}

//end class
?>