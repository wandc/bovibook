<?php 

/** Footer to include at the bottom of pages
 */

class Footer {

   
    public function footerContent() {
        ?>
          <!-- Main Footer -->
  <footer class="main-footer">
    <!-- To the right -->
    <div class="pull-right hidden-xs">
      <?php     print($this->showContent()); ?>
    </div>
    <!-- Default to the left -->
    <strong>Copyright &copy; 2017 <a href="#">LR</a>.</strong> 
  </footer>
       
        <?php
    
        
    }
    
    
    //OLD WAY
    public function original() {
        print("<div id='footer'> <!--overwritten with AJAX --></div>" . "\n\r");
        if (isset($_SESSION['access_token'])) {
         $access_token=rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');
        }
        else {
         $access_token='ERRROR: no access token supplied!';   
        }
        ?>          
        <script type="text/javascript" language="javascript"> 
            
            
            
             $(document).ready(function(){ 
                        $(function(){
      function load(){
         $.ajax({
  url: "/api/html/10001/content",
   headers: {"Authorization": "Bearer <?php echo($access_token);?>"}    
  }).done(function(data) {
     $('#footer').html(data);
});
    }
    load();
    setTimeout(load,300000);//300 seconds
    });
                       
                    }); 
            
            
        </script>
        <?php 

    }

    public function content() {
        
        
            $out=self::showContent(); 
         
          return $out;
          
    }

    

    

    
    
    private function dailyOverUnderQuotaLitres($str,$Quota) {
         ///////////
        $retArr=$Quota->calculateLastOverUnderQuota(); 
        $quotaDiff=$retArr['magnitudeLitres'];
        //show quota red if below and green if above.
        if ($quotaDiff >= 0)
        {
             $quotaDiffStr="<b class='quotaDiffFooterGreen'>$quotaDiff &#x2113</b>";
        }
        else {
            $quotaDiffStr="<b class='quotaDiffFooterRed'>$quotaDiff &#x2113</b>";
        }

     

         $str = $str . ('<span class="footerCowCount">');
         $str = $str . ("Quota:&nbsp;$quotaDiffStr");
         $str = $str . ('</span>');
      return $str;  
    }
    
    
     private function monthlyOverUnderQuotaLitres($str,$Quota) {
         ///////////
        $retArr=$Quota->calculateLastOverUnderQuota(); 
   
        $quotaDiff=   $retArr['overUnderLitresMonth'];
        
        //show quota red if below and green if above.
        if ($quotaDiff >= 0)
        {
             $quotaDiffStr="<b class='quotaDiffFooterGreen'>$quotaDiff &#x2113</b>";
        }
        else {
            $quotaDiffStr="<b class='quotaDiffFooterRed'>$quotaDiff &#x2113</b>";
        }

         $str = $str . ('<span class="footerCowCount">');
         $str = $str . ("Month:&nbsp;$quotaDiffStr &nbsp;&nbsp;&nbsp;");
         $str = $str . ('</span>');
      return $str;  
    }
    
    
    /*
     * if group is over crowded change number to be in red for display purposes 
     */
    function checkForOverCrowding($milkCowCountArray) {
        /* **** */
        if ($milkCowCountArray['countHigh'] > 60) {
            $milkCowCountArray['countHigh']='<span id="boldRed">' .$milkCowCountArray['countHigh'].'</span>';
        }
        else {
             $milkCowCountArray['countHigh']='<span id="boldGreen">' .$milkCowCountArray['countHigh'].'</span>';
        }
        
        /* **** */
         if ($milkCowCountArray['countHeifer'] > 60) {
            $milkCowCountArray['countHeifer']='<span id="boldRed">' .$milkCowCountArray['countHeifer'].'</span>';
        }
         else {
             $milkCowCountArray['countHeifer']='<span id="boldGreen">' .$milkCowCountArray['countHeifer'].'</span>';
        }
        
        /* **** */
         if ($milkCowCountArray['countLow'] > 40) {
            $milkCowCountArray['countLow']='<span id="boldRed">' .$milkCowCountArray['countLow'].'</span>';
        }
         else {
             $milkCowCountArray['countLow']='<span id="boldGreen">' .$milkCowCountArray['countLow'].'</span>';
        }
        
        /* **** */
         if ($milkCowCountArray['countFresh'] > 16) {
            $milkCowCountArray['countFresh']='<span id="boldRed">' .$milkCowCountArray['countFresh'].'</span>';
        }
         else {
             $milkCowCountArray['countFresh']='<span id="boldGreen">' .$milkCowCountArray['countFresh'].'</span>';
        }
        
        
        
     return $milkCowCountArray;   
    }
    
    /**
     * now uses javascript to draw sparklines.
     * Works, but slower, delay deploying..,...
     */
    function showContent() {
        $str = '';

        //initial calculations
        $milkCowCountArray = $GLOBALS['MiscObj']->milkingCowCount(); //.08s load time, maybe could be faster.
        $dim = $milkCowCountArray['dimMiklkingTotal']; //find DIM
        $dateNow = date("D H:i:s");
        
       $Quota=new Quota();
        $str=$this->dailyOverUnderQuotaLitres($str,$Quota);
         $str=$this->monthlyOverUnderQuotaLitres($str,$Quota);
        $milkCowCountArrayDisplay=$this->checkForOverCrowding($milkCowCountArray);
       /* High 60 Stalls. Heifer 60 Stalls. Low 40 Stalls. Sick 16 Stalls. */  
       $str = $str . ('<span class="footerCowCount">');    
        $str = $str . ("High {$milkCowCountArrayDisplay['countHigh']} / Heifer {$milkCowCountArrayDisplay['countHeifer']} / Low {$milkCowCountArrayDisplay['countLow']} / Fresh {$milkCowCountArrayDisplay['countFresh']}"  . "\n\r");
       $str = $str . ('</span>');



        /////////////////////////
        //// Number milking 
        $milkCowCountArray = $GLOBALS['MiscObj']->milkingCowCount(); //.08s load time, maybe could be faster.
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $endDate = date('Y-m-d H:i:s', strtotime("now"));
        $dataArray = $this->findNumberMilkingForDateRange($startDate, $endDate);
        //replace the last data point with today's up to the minute data.
        $dataArray[sizeof($dataArray) - 1] = $milkCowCountArray['countTotal'];
        $str = $str . ('<span>');
        $cows=new GoogleVisualizationSparkLine();
        $str = $str . $cows->sparklineContainerVis('# Cows', $dataArray, end($dataArray));
        $str = $str . ('</span>');

        //DIM
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $endDate = date('Y-m-d H:i:s', strtotime("now"));
        $dataArray = $this->findAverageDIMForDateRange($startDate, $endDate);
        $str = $str . ('<span>');
        $dim=new GoogleVisualizationSparkLine();
        $str = $str . $dim->sparklineContainerVis('DIM', $dataArray, round(end($dataArray)));
        $str = $str . ('</span>');

        //// Milk Pickup
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $endDate = date('Y-m-d H:i:s', strtotime("now"));
        $dataArray = $this->findMilkPickupForDateRange($startDate, $endDate);
        $str = $str . ('<span>');
        $pickup=new GoogleVisualizationSparkLine();
        $str = $str . $pickup->sparklineContainerVis('Pickup', $dataArray, end($dataArray));
        $str = $str . ('</span>');

        //// Fat
        //code needed to supply data to fat,prot,scc sparkline
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $dataArray = $this->findProvComponentsForDateRange($startDate);
        $str = $str . ('<span>');
        $fat=new GoogleVisualizationSparkLine();
        $str = $str . $fat->sparklineContainerVis('Fat', $dataArray['fat'], end($dataArray['fat']));
        $str = $str . ('</span>');

        //// Protein
        $str = $str . ('<span>');
        $prot=new GoogleVisualizationSparkLine();
        $str = $str . $prot->sparklineContainerVis('Prot', $dataArray['prot'], end($dataArray['prot']));
        $str = $str . ('</span>');

        //// SCC
       $str = $str . ('<span>');
       $scc=new GoogleVisualizationSparkLine();
       $str = $str . $scc->sparklineContainerVis('SCC (k)', $dataArray['scc'], end($dataArray['scc']));
       
         $str = $str . ('</span>');

      /*  //// breeding rate
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/reports/reproReport.inc');
        $breedingRateInterval = 30;
        $dataArray = ReproReport::calculateTwentyOneDayBreedingAverage(1, $breedingRateInterval, 90); //slow part .03s.
        $str = $str . ('<span>');
        $breedingrate=new GoogleVisualizationSparkLine();
        $str = $str . $breedingrate->sparklineContainerVis('30d BR', $dataArray, end($dataArray));
        $str = $str . ('</span>');
*/
                
        /////////////////////
        //$str = $str . ("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
//////////////////////////////
         //ajax container
        $str = $str . ('<span>');
       
        $str = $str . ('</span>');

     

        return $str;
    }

   
    
    
    
    



   

    public function findProvComponentsForDateRange($startDate, $endDate = null) {
        if ($endDate == null) {
            $endDate = date('Y-m-d H:i:s', strtotime('now'));
        }
//no end date, because average test results could be in the future.
        $query = "SELECT test_time_start ,scc,fat,protein FROM batch.nb_bulk_tank_sample WHERE test_time_start >= '$startDate' AND test_time_start <= '$endDate' ORDER BY test_time_start ASC";

        $res = $GLOBALS['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $sccArray[] = $row['scc'] / 1000; //show in thousands.
            $fatArray[] = $row['fat'];
            $protArray[] = $row['protein'];
        }
//return data
        $dataArray['fat'] = $fatArray;
        $dataArray['prot'] = $protArray;
        $dataArray['scc'] = $sccArray;
        return $dataArray;
    }

   

    public function findMilkPickupForDateRange($startDate, $endDate) {
$dataArray=array();
        $query = "SELECT milk_amount_sold FROM batch.milk_pickup_event WHERE event_time >= '$startDate' AND event_time <= '$endDate' ORDER BY event_time ASC";
        $res = $GLOBALS['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $dataArray[] = $row['milk_amount_sold'];
        }
        return $dataArray;
    }



    public function findAverageDIMForDateRange($startDate, $endDate) {
//crazy query to give a list of milking cows on a specific date
        $query = "SELECT average_dim FROM batch.daily_number_cows_milking WHERE date >= '$startDate' AND date <= '$endDate' ORDER BY date ASC";
        $res = $GLOBALS['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $dimArray[] = $row['average_dim'];
        }
        return $dimArray;
    }


    public function findNumberMilkingForDateRange($startDate, $endDate) {
//crazy query to give a list of milking cows on a specific date
        $query = "SELECT number_of_cows FROM batch.daily_number_cows_milking WHERE date >= '$startDate' AND date <= '$endDate' ORDER BY date ASC";
        $res = $GLOBALS['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $dimArray[] = $row['number_of_cows'];
        }
        return $dimArray;
    }



}

/* sparkline example */
/*
 function drawVisualization() {
  // Create and populate the data table.
  
  
  var data = new google.visualization.DataTable();
    data.addColumn('string', 'x');
    data.addColumn('number', 'blue');
   data.addColumn('number', 'red');
  data.addColumn({type:'string', role:'annotation'});
    data.addRows([
      [,3.6,null, null],
      [,4.4,null, null],
      [,3.2,null, null],
      [,4.1,null, null],
      [,3.8,null, null],
      [,3.9,null, null],
      [,4.6,4.6, null]
    ]);

  // Create and draw the visualization.
  new google.visualization.LineChart(document.getElementById('visualization')).
      draw(data, {curveType: "function",
                  width: 120, height: 30,enableInteractivity: false,legend: 'none',
                 vAxis:{
         baselineColor: '#fff',
         gridlineColor: '#fff',
         textPosition: 'none',          
       },
          series: {
                0: { pointShape: null,pointSize: 0 },
                1: { pointShape: 'circle',pointSize: 4}
                
            }
                 }
          );
}
​
 */


//end class
?>