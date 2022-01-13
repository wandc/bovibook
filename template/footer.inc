<?php 

/** Footer to include at the bottom of pages
 */

class Footer {

   
    public function footerContent() {
        $footer = <<<HTML
    <!-- Main Footer -->
    <footer class="main-footer">
        <!-- To the right -->
       <div class="float-right d-none d-sm-block">
            {$this->showContent()}
              <!-- Default to the left -->
    <span><strong>Copyright &copy; 2020 <a href="#">LR</a>.</strong></span>
        </div>
  
    </footer>
       
      
HTML;    
    $footer2 = <<<HTML
            
          <footer class="main-footer">
  
    <strong>Copyright © 2014-2019 <a href="http://adminlte.io">AdminLTE.io</a>.</strong> All rights
    reserved.
  </footer>
HTML;          
            
        return $footer;
    

    }
    
    
  
    
    private function dailyOverUnderQuotaLitres($str,$Quota) {
         ///////////
        $retArr=$Quota->calculateLastOverUnderQuota(); 

           if (!empty($retArr['fat_percentage'])) {
            $overUnderLitresDaily = round(($retArr['fat_kg_latest'] - $retArr['total_quota']) / ($retArr['fat_percentage'] * .01));
        } else {
            $overUnderLitresDaily = null;
        }

        //show quota red if below and green if above.
        if ((int) $overUnderLitresDaily >= 0)
        {
             $quotaDiffStr="<b class='quotaDiffFooterGreen'>$overUnderLitresDaily &#x2113</b>";
        }
        else {
            $quotaDiffStr="<b class='quotaDiffFooterRed'>$overUnderLitresDaily &#x2113</b>";
        }

     

         $out[]=  ('<span class="footerCowCount">');
         $out[]=  ("Quota:&nbsp;$quotaDiffStr");
         $out[]=  ('</span>');
      return $str;  
    }
    
    
     private function monthlyOverUnderQuotaLitres($str,$Quota) {
         ///////////
        $retArr=$Quota->calculateLastOverUnderQuota(); 

        if (!empty($retArr['fat_percentage'])) {
            $overUnderLitresDaily = round(($retArr['produced_quota'] - $retArr['total_quota']) / ($retArr['fat_percentage'] * .01));
        } else {
            $overUnderLitresDaily = null;
        }

        //find days remaining this month
        $timestamp = strtotime('now');
        $daysRemaining = (int)date('t', $timestamp) - (int)date('j', $timestamp);
        $monthlyOverUnderLitres=$daysRemaining*$overUnderLitresDaily;
        
        
        
        //show quota red if below and green if above.
        if ((int) $monthlyOverUnderLitres >=0)
        {
             $quotaDiffStr="<b class='quotaDiffFooterGreen'>$monthlyOverUnderLitres &#x2113</b>";
        }
        else {
            $quotaDiffStr="<b class='quotaDiffFooterRed'>$monthlyOverUnderLitres &#x2113</b>";
        }


         $out[]=  ('<span class="footerCowCount">');
         $out[]=  ("Month:&nbsp;$quotaDiffStr &nbsp;&nbsp;&nbsp;");
         $out[]=  ('</span>');
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
       
        $str='';
        //initial calculations
        $milkCowCountArray = $GLOBALS['MiscObj']->milkingCowCount(); //.08s load time, maybe could be faster.
        $dim = $milkCowCountArray['dimMiklkingTotal']; //find DIM
        $dateNow = date("D H:i:s");
        
       $Quota=new Quota();
        $str=$this->dailyOverUnderQuotaLitres($str,$Quota);
        $str=$this->monthlyOverUnderQuotaLitres($str,$Quota);
        $milkCowCountArrayDisplay=$this->checkForOverCrowding($milkCowCountArray);
       /* High 60 Stalls. Heifer 60 Stalls. Low 40 Stalls. Sick 16 Stalls. */  
       $out[]=  ('<span class="footerCowCount">');    
       $out[]=  ("High {$milkCowCountArrayDisplay['countHigh']} / Heifer {$milkCowCountArrayDisplay['countHeifer']} / Low {$milkCowCountArrayDisplay['countLow']} / Fresh {$milkCowCountArrayDisplay['countFresh']}"  . "\n\r");
       $out[]=  ('</span>');



        /////////////////////////
        //// Number milking 
        $milkCowCountArray = $GLOBALS['MiscObj']->milkingCowCount(); //.08s load time, maybe could be faster.
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $endDate = date('Y-m-d H:i:s', strtotime("now"));
        $dataArray = $this->findNumberMilkingForDateRange($startDate, $endDate);
        //replace the last data point with today's up to the minute data.
        $dataArray[sizeof($dataArray) - 1] = $milkCowCountArray['countTotal'];
        $out[]=  ('<span>');
        $cows=new GoogleVisualizationSparkLine();
        $out[]=  $cows->sparklineContainerVis('# Cows', $dataArray, end($dataArray));
        $out[]=  ('</span>');

        //DIM
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $endDate = date('Y-m-d H:i:s', strtotime("now"));
        $dataArray = $this->findAverageDIMForDateRange($startDate, $endDate);
        $out[]=  ('<span>');
        $dim=new GoogleVisualizationSparkLine();
        $out[]=  $dim->sparklineContainerVis('DIM', $dataArray, round(end($dataArray)));
        $out[]=  ('</span>');

        //// Milk Pickup
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $endDate = date('Y-m-d H:i:s', strtotime("now"));
        $dataArray = $this->findMilkPickupForDateRange($startDate, $endDate);
        $out[]=  ('<span>');
        $pickup=new GoogleVisualizationSparkLine();
        $out[]=  $pickup->sparklineContainerVis('Pickup', $dataArray, end($dataArray));
        $out[]=  ('</span>');

        //// Fat
        //code needed to supply data to fat,prot,scc sparkline
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $dataArray = $this->findProvComponentsForDateRange($startDate);
        $out[]=  ('<span>');
        $fat=new GoogleVisualizationSparkLine();
        $out[]=  $fat->sparklineContainerVis('Fat', $dataArray['fat'], end($dataArray['fat']));
        $out[]=  ('</span>');

        //// Protein
        $out[]=  ('<span>');
        $prot=new GoogleVisualizationSparkLine();
        $out[]=  $prot->sparklineContainerVis('Prot', $dataArray['prot'], end($dataArray['prot']));
        $out[]=  ('</span>');

        //// SCC
       $out[]=  ('<span>');
       $scc=new GoogleVisualizationSparkLine();
       $out[]=  $scc->sparklineContainerVis('SCC (k)', $dataArray['scc'], end($dataArray['scc']));
       
       $out[]=  ('</span>');

         //ajax container
        $out[]=  ('<span>');
       
        $out[]=  ('</span>');

        return implode($out);
    }


   

    public function findProvComponentsForDateRange($startDate, $endDate = null) {
        if ($endDate == null) {
            $endDate = date('Y-m-d H:i:s', strtotime('now'));
        }
//no end date, because average test results could be in the future.
        $query = "SELECT test_collection_date ,scc,fat,protein FROM batch.nb_bulk_tank_sample_every WHERE test_collection_date >= '$startDate' AND test_collection_date <= '$endDate' ORDER BY test_collection_date ASC";

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
        $query = "SELECT sum(milk_amount_sold)  over (PARTITION BY collection_date) as milk_amount_sold  FROM batch.milk_pickup_event WHERE collection_date >= '$startDate' AND collection_date <= '$endDate' ORDER BY collection_date ASC";
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