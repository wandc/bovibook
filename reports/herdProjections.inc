<?php 

require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/googleCharts.inc');
class HerdProjections extends BasePage {
    
    
    public function defaultDisplay() {
     /*   $this->plotHeifers();
        $this->plotPreg();
       $this->lactationsOfMilkingHerd();
      */
        
        $cls=new milkProjection();
        $cls->main();
    }
    
    //not really the rigth place for this, but the goal should be 50% of herd 3+ lactations.
    function lactationsOfMilkingHerd() {
        print("<h3>Lactations of current milking herd</h3>");
    $sql="SELECT distinct bovinemanagement.lactation_number (bovinecurr.id),count( bovinemanagement.lactation_number (bovinecurr.id)) FROM bovinemanagement.bovinecurr WHERE location_id= ANY('{39,40,41,42}'::int[]) group by bovinemanagement.lactation_number (bovinecurr.id)";

    $res = $GLOBALS['pdo']->query($sql);

            while($row = $res->fetch(PDO::FETCH_ASSOC)) {    
                print("lact num/count : ".$row['lactation_number']." ". $row['count']."<br/>");
            }
    }
    
                
     function plotHeifers(){
         print("<h2>Eligibile heifers to breed at 13 Months</h2>");
        $sql="
             CREATE TEMP TABLE two AS 
SELECT xx.id,local_number,pro_doll,xx.full_name,xx.birth_date
 FROM bovinemanagement.bovinecurr xx
LEFT JOIN batch.holstein_canada_data_curr ON holstein_canada_data_curr.full_reg_number = xx.full_reg_number
WHERE xx.birth_date >= current_date - interval '24 months'; 
             ";
       $res = $GLOBALS['pdo']->query($sql);
       
       $sql2="CREATE TEMP TABLE three AS SELECT * FROM   bovinemanagement.bovinecurr;
/* the count query con bovinecurr is really slow, so temp table created */";
           $res = $GLOBALS['pdo']->query($sql2);
       
       $sql3="SELECT (xx.birth_date + interval '13 months') as birth_date14 ,pro_doll,local_number, 
           (SELECT count(id)  FROM three WHERE birth_date < xx.birth_date + interval '1.5 months' AND  birth_date >= xx.birth_date - interval '1.5 months' )  
           FROM two xx
";
     
     ?>
<script type="text/javascript">
   
    
        setTimeout(drawChart, 200); //firefox workaround. 
      google.charts.setOnLoadCallback(drawChart,true);
         $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart);
            });
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
             ['local_number', 'birth_date14', 'pro_doll', 'count'],
           <?php 
              $res = $GLOBALS['pdo']->query($sql3);
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("['{$row['local_number']}', ".GoogleChartsHelper::javascriptDate($row['birth_date14']).",	{$row['pro_doll']},   {$row['count']}],");    
            }       
                    ?>
        
         
        ]);

        var options = {
          title: 'HEIFER Yields',
          hAxis: {title: '13 Months'},
          vAxis: {title: 'Pro$'},
          bubble: {textStyle: {fontSize: 11}},
          colorAxis: {colors: ['yellow', 'red']}
        };




        var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all'));
        chart.draw(data, options);
      }
    </script>

 


<?php 
print("<div id='chart_divv_all'></div>");
 
    }
    
       
            
            function plotPreg(){
        $sql="
           CREATE TEMP TABLE one AS 
SELECT xx.id,xx.local_number,xx.calculated_potential_due_date,pro_doll
            FROM bovinemanagement.pregnant_view xx
            LEFT JOIN bovinemanagement.bovine ON bovine.id = xx.id
            LEFT JOIN batch.holstein_canada_data_curr ON holstein_canada_data_curr.full_reg_number = bovine.full_reg_number;
             ";
          $res = $GLOBALS['pdo']->query($sql);
          
       $sql2="SELECT *, (SELECT count(id)  FROM one WHERE calculated_potential_due_date < xx.calculated_potential_due_date + interval '1.5 months' AND  calculated_potential_due_date >= xx.calculated_potential_due_date - interval '1.5 months' )  
FROM one xx
            ";
     ?>
<script type="text/javascript">
   
     
        setTimeout(drawChart, 200); //firefox workaround. 
      google.charts.setOnLoadCallback(drawChart,true);
       $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart);
            });
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
             ['local_number', 'calculated_potential_due_date', 'pro_doll', 'count'],
           <?php 
              $res = $GLOBALS['pdo']->query($sql2);
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("['{$row['local_number']}', ".GoogleChartsHelper::javascriptDate($row['calculated_potential_due_date']).",	{$row['pro_doll']},   {$row['count']}],");    
            }       
                    ?>
        
         
        ]);

        var options = {
          title: 'PREGNANCY Yields',
          hAxis: {title: 'Calving Date'},
          vAxis: {title: 'Pro$'},
          bubble: {textStyle: {fontSize: 11}},
          colorAxis: {colors: ['yellow', 'red']}
        };




        var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all2'));
        chart.draw(data, options);
      }
    </script>

 


<?php 
print("<div id='chart_divv_all2' ></div>");
 
    }
    
    
    
} //end class


/*
 * take days dry, lactation info. etc. to project:
 * number cows milking
 * days in milk
 * butterfat being produced
 * 
 */
class milkProjection {
    
    //so we should do every cow in the herd, even baby calves, since we are a closed herd.
    
    //create an array for each cow going through time for the next five years that projects what will happen.
    
    
    function main() {
        
    $sql = "SELECT * FROM bovinemanagemnet.bovinecurr";
    
    //three classes:
    //currently milking
    //dry
    //replacement
    
       $period = new DatePeriod(new DateTime('now'), new DateInterval('P1D'), new DateTime('now +400 day'));
    foreach ($period as $date) {
        $dates[] = $date->format("Y-m-d");
    }
    
    print_r($dates);
    
    
    
    }
    
    
    
    
}//end class
?>