<?php 
/* shows how TMR recipes change through time */


require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');

include_once($_SERVER['DOCUMENT_ROOT'] . 'phpCronScripts/vfd_actech.php');

class FeedUsageProjection extends TabPage {

    public function defaultDisplay() {


       
        $tabArray[1]['name'] = 'Hammermill';
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('nutritionist', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
        $tabArray[2]['name'] = 'Projections';
        $tabArray[3]['name'] = 'Feed Tanks';
        $tabArray[4]['name'] = 'Grey Barn';
        }
        $this->renderTabs($tabArray);
    }

    
      function tab1() {
        /* Hammermill */
       $this->hammermill();
    }

    
    function tab2() {

        print("<h2>Projections of Feed Usage Based On Current Recipe</h2>");
        $this->dailyProjection('1day');
        $this->dailyProjection('7day');
        $this->dailyProjection('14day');
        $this->dailyProjection('30day');
        $FeedBinCapacityChart= new FeedBinCapacityChart();
        $FeedBinCapacityChart->displayFeedBinCapacityChart();
    }

    
    function tab3() {
        /* feed tank levels */
     
        
        $this->lastDayFeedTankChart(1);
           $this->lastDayFeedTankChart(4);
              $this->lastDayFeedTankChart(0);
       
    }
    
    function tab4() {
       $sql="SELECT  tank_id, lower(event_range ), extract('epoch' from(upper (event_range)-lower(event_range)) ) as auger_on_time_seconds FROM bas.feed_auger_event_range  WHERE upper (event_range) > current_date - interval '60 days' AND tank_id=11 order by event_range DESC";
       $res = $GLOBALS['pdo']->query($sql);
        $out = array();
          $out[]='<ul>';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $out[] ='<li>'. $row['lower'] . ' on for '. $row['auger_on_time_seconds'].' seconds</li>';
        }
         $out[]='</ul>';
       print(implode('', $out));
        
    }

    

  
       //sho realtime state for hammer-mill.
    function hammermill() {
    
    //get corn tank weight right now.    
    //put in input format for method, so it could also be an ajax call
    $x['item']=4;
    $a=($this->tankScaleData($x,true));
    $mass=round($a[0]['mass']);
    print("<h3>MASS:</h3>");
    print("<h4>Tank #4: $mass kg</h4>");
    print("<hr>");
        
    $cls= new  Hammermill_VFD();
    $cls->hammermillRealtimeInfo();
    
    
    print("Set P204 and P205 to range of hammermill 0 to 51 amps, in our case 0 to 5.1.<br>");
   print("Set P231 to set point you want hammermill to run at, ie 100% is 5.1. This should always be 5.1<br>");
   print("Finally tune the PID loop, by setting P207 to 0.5 if you want it to run at 50% of P231, ie ~26 amps. Set it to 1.0 for normal operation. Set lower if you have problems.<br>");
   print("Basically P207 is the only thing to change. The slide on auger has to be at least mostly open or the auger will spin too fast trying to get corn that is not there. The hammers need to be in good condition or this will not work well.");
    
    //
   //chart
    //
    $this->vfdFeedbackLoadChart('hamm');
    
    
    }
    
    
        //hammermill feed back sensor
        public function vfdFeedbackLoad($request) {
            
     
       if (!empty($request['item'])) {
       if (!filter_var($request['item'], FILTER_SANITIZE_STRING) === false) {  
           $vfd_name=$request['item'];
       
                switch ($vfd_name) {
   case 'hamm':
         $sql = "SELECT  event_time, address_value FROM bas.vfd_event WHERE vfd_id='$vfd_name' AND event_time>=(current_date-interval '2 days') AND address_number=520 ORDER BY event_time DESC ";

         break;
  
     default :
         throw new Exception("Error, invalid tank number requested");
}
        
              
        
        $array = array();
       
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_timeNEW'] = date("M d Y H:i:s O", strtotime($row['event_time']));
        }

        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'string', 'label' => 'datetime');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Load (amps)'); //y axis #1

        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_timeNEW']}"),
                array('v' => $row['address_value']),
            );
        }
        return $array;
    }}}
    
    function vfdFeedbackLoadChart($vfd_id) {
           print("<h3>Hammermill Load Chart</h3>");
           print("<h4>Note: 51 amps is 100% load.</h4>");
        ?>
          <div id="chart_divB_<?php echo($vfd_id); ?>" ></div>
        <!--Load the AJAX API-->
      
     
        <script type="text/javascript">

            // Load the Visualization API and the piechart package.
     



            function drawChartB() {
                var jsonData = $.ajax({
                    url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/vfdFeedbackLoad/<?php echo($vfd_id); ?>",
                    dataType: "json",
                    async: false
                }).responseText;

                // Create our data table out of JSON data loaded from server.
                var data = new google.visualization.DataTable(jsonData);

                // Instantiate and draw our chart, passing in some options.
                var chart = new google.visualization.LineChart(document.getElementById('chart_divB_<?php echo($vfd_id); ?>'));
                chart.draw(data, { height: 300});
            }

            // Set a callback to run when the Google Visualization API is loaded.
            google.charts.setOnLoadCallback(drawChartB, true);
             $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChartB);
            });

        </script>
     
      
        <?php 
    }
    
    
    
    
    
    
    
    function lastDayFeedTankChart($tankNum) {
        //lookup with feed admin, what is in the current tank number
        $sql9="SELECT feed_type from nutrition.feedcurr WHERE location = 'Feed Tank ' || '$tankNum' limit 1";
         $res = $GLOBALS['pdo']->query($sql9);
         $row = $res->fetch(PDO::FETCH_ASSOC);
        
        
           print("<h3>Feed Tank #$tankNum: {$row['feed_type']}</h3>");
        ?>

        <!--Load the AJAX API-->
      
        <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
        <script type="text/javascript">

         
     
            // Set a callback to run when the Google Visualization API is loaded.
            google.charts.setOnLoadCallback(drawChart_tank<?php echo($tankNum); ?>, true);
             $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart_tank<?php echo($tankNum); ?>);
            });


            function drawChart_tank<?php echo($tankNum); ?>() {
                var jsonData = $.ajax({
                    url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/lastDayFeedTank/<?php echo($tankNum); ?>",
                    dataType: "json",
                    async: false
                }).responseText;

                // Create our data table out of JSON data loaded from server.
                var data = new google.visualization.DataTable(jsonData);

                // Instantiate and draw our chart, passing in some options.
                var chart = new google.visualization.LineChart(document.getElementById('chart_divB_<?php echo($tankNum); ?>'));
                chart.draw(data, { height: 300});
            }

          

        </script>
     
        <div id="chart_divB_<?php echo($tankNum); ?>" ></div>
        <?php 
    }

    
    
    //converts to google chart format.
    public function lastDayFeedTank($request) {
      
       $results=$this->tankScaleData($request);
        
        //change presentation
        foreach ($results as &$row) {
            $row['event_timeNEW'] = date("M d Y H:i:s O", strtotime($row['event_time']));
        }

        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'string', 'label' => 'datetime');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'mass'); //y axis #1

        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_timeNEW']}"),
                array('v' => $row['mass']),
            );
        }
        return $array;
    }
    
    
    //if single is true then only return single measurement
    //data from scale before charts processing.
    function tankScaleData($request,$single=false) {
      if (isset($request['item'])) {
       if(filter_var($request['item'], FILTER_VALIDATE_INT) === 0 || filter_var($request['item'], FILTER_VALIDATE_INT)) { //this allows zero to be a valid int, not the default behaviour.
           $tankNum=$request['item'];
        
                switch ($tankNum) {
   case 0:
         $tankEmptyMassVolts=68000; 
         $massMulitplier=.1;
         $sql = "SELECT  event_time, (abs(cells[1]-70000)-$tankEmptyMassVolts)*$massMulitplier as mass from bas.feed_bin_weight_event WHERE tank_id=0 AND event_time>=(current_date-interval '4 days') ORDER BY event_time DESC "; //open scale
         break;                              
   case 1:
         $tankEmptyMassVolts=2.40; //calulated by adding 4 load cells together and subtracting 20 volts from it. 2.417 on Nov 6 2016. Used 2.36 previously.
         $massMulitplier=1;
         $sql = "SELECT  event_time, (abs(cells[1]+cells[2]+cells[3]+cells[4]-20)-$tankEmptyMassVolts)*4000*$massMulitplier as mass from bas.feed_bin_weight_event WHERE tank_id=1 AND event_time>=(current_date-interval '4 days') ORDER BY event_time DESC ";
         break;
   case 4:
          //now has 8 load cells.
        //{3.227126,4.617836,3.962094,4.610732,4.215835,4.046417,4.256606,3.879624}  is empty Nov 8 2018 
        //20-13.899221=6.101 Nov 7 2016, when empty, changed June 19 2017 when empty to 4.2788.
         $tankEmptyMassVolts=32.816;
         $massMulitplier=-1.32; //should by all rights be 2. If blank load cells are not balanced right?
         $sql = "SELECT  event_time, (abs(cells[1]+cells[2]+cells[3]+cells[4]+cells[5]+cells[6]+cells[7]+cells[8]-0)-$tankEmptyMassVolts)*4000*$massMulitplier as mass from bas.feed_bin_weight_event WHERE tank_id=4 AND event_time>=(current_date-interval '4 days')  ORDER BY event_time DESC ";
         break;
     default :
         throw new Exception("Error, invalid tank number requested");
}
       
        if ($single==true) {
         $sql=$sql.' limit 1';//very hacky, but should be ok.    
         }


        
        $array = array();
    
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);   
     return $results;   
    }}}
    
    

    public function dailyProjection($projectionNo) {



        $numberPerPage = 100;



        switch ($projectionNo) {

            case '1day':
                $sql = "	     
SELECT  DISTINCT (feedcurr_id),feed_type,location,round(sum(ingrediant_load_amount)) as wet_total_daily_usage,round((sum(ingrediant_load_amount)*.01*(100-recipe_active.moisture_percent))) as dry_total_daily_usage FROM nutrition.recipe_active GROUP BY feedcurr_id,moisture_percent,feed_type,location ORDER BY location
";
                $title = 'Daily Usage';
                break;
            case '7day':
                $sql = "	     
SELECT  DISTINCT (feedcurr_id),feed_type,location,round(sum(ingrediant_load_amount)*7) as wet_total_daily_usage,round((sum(ingrediant_load_amount)*.01*(100-recipe_active.moisture_percent))*7) as dry_total_daily_usage FROM nutrition.recipe_active GROUP BY feedcurr_id,moisture_percent,feed_type,location ORDER BY location
";
                $title = '7 Day Projection';
                break;
            case '14day':
                $sql = "	     
SELECT  DISTINCT (feedcurr_id),feed_type,location,round(sum(ingrediant_load_amount)*14) as wet_total_daily_usage,round((sum(ingrediant_load_amount)*.01*(100-recipe_active.moisture_percent))*14) as dry_total_daily_usage FROM nutrition.recipe_active GROUP BY feedcurr_id,moisture_percent,feed_type,location ORDER BY location
";
                $title = '14 Day Projection';
                break;
            case '30day':
                $sql = "	     
SELECT  DISTINCT (feedcurr_id),feed_type,location,round(sum(ingrediant_load_amount)*30) as wet_total_daily_usage,round((sum(ingrediant_load_amount)*.01*(100-recipe_active.moisture_percent))*30) as dry_total_daily_usage FROM nutrition.recipe_active GROUP BY feedcurr_id,moisture_percent,feed_type,location ORDER BY location
";
                $title = '30 Day Projection';
                break;
        }
        
        //FIXME: SQL IS FOR PRE NRC 2001 and needs changed to new way.
        
        /*
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

 
            $rowOut['Feed Name'] = ($row['feed_type']);
            $rowOut['Location'] = ($row['location']);
            $rowOut['As Fed Total (kg)'] = ($row['wet_total_daily_usage']);
            $rowOut['DM Total (kg)'] = ($row['dry_total_daily_usage']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ('<h3>$title</h3>');
        $out[] = ( (new JQueryDataTable)->startBasicArray($dataArr));
        print implode($out);
        */
       
    }

//end function

   

}//end class


/**
  Just shows a static chart of feed bin capacities 
 */
 
class FeedBinCapacityChart  {
    
    public function displayFeedBinCapacityChart() {
        ?>

        <h3>FEED BIN CAPACITY CHART</h3>
        <table
            cellSpacing="0" cellPadding="0" width="581" border="0" x:str>
            <tr height="21"> </tr>
            <tr height="21">
                <td width="107" height="65" rowSpan="3"> MODEL NUMBER</td>
                <td width="331" colSpan="4">CAPACITY</td>
                <td width="143" colSpan="2" rowSpan="2"> HEIGHT TO FILL</td>
            </tr>
            <tr height="21">
                <td width="331" colSpan="4" height="21"> Based on 40 Lbs./Cubic Feet (641 Kg/M<sup>3</sup>)</td>
            </tr>
            <tr height="23">
                <td width="53" height="23">Tons</td>
                <td width="112">Metric Tons</td>
                <td width="102">Cubic Feet</td>
                <td width="64">M<sup>3</sup></td>
                <td width="61">FT./IN.</td>
                <td width="82">Meters</td>
            </tr>
            <tr height="21">
                <td width="581" colSpan="7" height="21">6-ft. (1.8-m) diameter bins with 60º hopper</td>
            </tr>
            <tr height="21">
                <td width="107" height="21"> 45-00601</td>
                <td width="53" >3.1</td>
                <td width="112" >2.8</td>
                <td width="102" ="156.9">157</td>
                <td width="64" ="4.44">4.4</td>
                <td width="61">11' 6"</td>
                <td width="82" >3.5</td>
            </tr>
            <tr height="21">
                <td width="107" height="21">45-00602</td>
                <td width="53" >4.6</td>
                <td width="112" >4.2</td>
                <td width="102" ="231.9">232</td>
                <td width="64" ="6.57">6.6</td>
                <td width="61">14' 2"</td>
                <td width="82" >4.3</td>
            </tr>
            <tr height="21"> <td width="107" height="21">45-00603</td>
                <td width="53" >6.1</td>
                <td width="112" >5.6</td>
                <td width="102" ="306.9">307</td>
                <td width="64" ="8.69">8.7</td>
                <td width="61">16' 10"</td>
                <td width="82" >5.1</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00604</td>
                <td width="53" > 7.6</td>
                <td width="112" > 6.9</td>
                <td width="102" ="381.9"> 382</td>
                <td width="64" ="10.81"> 10.8</td>
                <td width="61"> 19' 6"</td>
                <td width="82" > 5.9</td>
            </tr>
            <tr height="21"> <td width="581" colSpan="7" height="21"> 7-ft. (2.1-m) diameter bins with 67º hopper</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00721</td>
                <td width="53" > 4.9</td>
                <td width="112" > 4.5</td>
                <td width="102" ="245.6"> 246</td>
                <td width="64" ="6.95"> 7.0</td>
                <td width="61"> 14' 9"</td>
                <td width="82" > 4.5</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00722</td>
                <td width="53" > 6.9</td>
                <td width="112" > 6.3</td>
                <td width="102" > 347</td>
                <td width="64" ="9.83"> 9.8</td>
                <td width="61"> 17' 5"</td>
                <td width="82" > 5.3</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00723</td>
                <td width="53" > 9.0</td>
                <td width="112" > 8.1</td>
                <td width="102" ="448.4"> 448</td>
                <td width="64" > 12.7</td>
                <td width="61"> 20' 1"</td>
                <td width="82" > 6.1</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> <bold id="boldRed">45-00724</bold></td>
                <td width="53" > 11.0</td>
                <td width="112" > 10.0</td>
                <td width="102" ="549.8"> 550</td>
                <td width="64" ="15.57"> 15.6</td>
                <td width="61"> 22' 9"</td>
                <td width="82" > 6.9</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00725</td>
                <td width="53" > 13.0</td>
                <td width="112" > 11.8</td>
                <td width="102" ="651.2"> 651</td>
                <td width="64" ="18.44"> 18.4</td>
                <td width="61"> 25' 5"</td>
                <td width="82" > 7.7</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00726</td>
                <td width="53" > 15.1</td>
                <td width="112" > 13.7</td>
                <td width="102" ="752.6"> 753</td>
                <td width="64" ="21.31"> 21.3</td>
                <td width="61"> 28' 1"</td>
                <td width="82" > 8.6</td>
            </tr>
            <tr height="21"> <td width="581" colSpan="7" height="21"> 9-ft. (2.7-m) diameter bins with 60º hopper</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00901</td>
                <td width="53" > 8.6</td>
                <td width="112" > 7.8</td>
                <td width="102" ="427.6"> 428</td>
                <td width="64" ="12.11"> 12.1</td>
                <td width="61"> 15' 7"</td>
                <td width="82" > 4.8</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00902</td>
                <td width="53" > 12.0</td>
                <td width="112" > 10.8</td>
                <td width="102" ="597.6"> 598</td>
                <td width="64" ="16.92"> 16.9</td>
                <td width="61"> 18' 8"</td>
                <td width="82" > 5.7</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00903</td>
                <td width="53" > 15.4</td>
                <td width="112" > 13.9</td>
                <td width="102" ="767.6"> 768</td>
                <td width="64" ="21.74"> 21.7</td>
                <td width="61"> 20' 11"</td>
                <td width="82" > 6.4</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00904</td>
                <td width="53" > 18.8</td>
                <td width="112" > 17.0</td>
                <td width="102" ="937.6"> 938</td>
                <td width="64" ="26.55"> 26.6</td>
                <td width="61"> 23' 7"</td>
                <td width="82" > 7.2</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00905</td>
                <td width="53" > 22.2</td>
                <td width="112" > 20.1</td>
                <td width="102" ="1107.6"> 1108</td>
                <td width="64" ="31.36"> 31.4</td>
                <td width="61"> 26' 3"</td>
                <td width="82" > 8.0</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00906</td>
                <td width="53" > 25.6</td>
                <td width="112" > 23.2</td>
                <td width="102" ="1277.6"> 1278</td>
                <td width="64" ="36.18"> 36.2</td>
                <td width="61"> 28' 11"</td>
                <td width="82" > 8.8</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-00907</td>
                <td width="53" > 28.9</td>
                <td width="112" > 26.2</td>
                <td width="102" ="1442.6"> 1443</td>
                <td width="64" ="40.85"> 40.9</td>
                <td width="61"> 31' 7"</td>
                <td width="82" > 9.6</td>
            </tr>
            <tr height="21"> <td width="581" colSpan="7" height="21"> 12-ft. (3.7-m) diameter bins with 60º hopper</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01201</td>
                <td width="53" > 17.8</td>
                <td width="112" > 16.1</td>
                <td width="102" ="887.5"> 888</td>
                <td width="64" ="25.13"> 25.1</td>
                <td width="61"> 19' 5"</td>
                <td width="82" > 5.9</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01202</td>
                <td width="53" > 23.7</td>
                <td width="112" > 21.5</td>
                <td width="102" > 1186</td>
                <td width="64" ="33.58"> 33.6</td>
                <td width="61"> 21' 1"</td>
                <td width="82" > 6.7</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01203</td>
                <td width="53" > 29.7</td>
                <td width="112" > 26.9</td>
                <td width="102" ="1484.4"> 1484</td>
                <td width="64" ="42.03"> 42.0</td>
                <td width="61"> 24' 9"</td>
                <td width="82" > 7.5</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01204</td>
                <td width="53" > 35.7</td>
                <td width="112" > 32.3</td>
                <td width="102" ="1782.9"> 1783</td>
                <td width="64" ="50.49"> 50.5</td>
                <td width="61"> 27' 5"</td>
                <td width="82" > 8.4</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> <bold id="boldRed">45-01205</bold></td>
                <td width="53" > 41.6</td>
                <td width="112" > 37.8</td>
                <td width="102" ="2081.3"> 2081</td>
                <td width="64" ="58.94"> 58.9</td>
                <td width="61"> 30' 1"</td>
                <td width="82" > 9.2</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01206</td>
                <td width="53" > 47.6</td>
                <td width="112" > 43.2</td>
                <td width="102" ="2379.9"> 2380</td>
                <td width="64" ="67.39"> 67.4</td>
                <td width="61"> 32' 9"</td>
                <td width="82" > 10.0</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01207</td>
                <td width="53" > 53.6</td>
                <td width="112" > 48.6</td>
                <td width="102" ="2678.3"> 2678</td>
                <td width="64" ="75.84"> 75.8</td>
                <td width="61"> 35' 5"</td>
                <td width="82" > 10.8</td>
            </tr>
            <tr height="21"> <td width="581" colSpan="7" height="21"> 15-ft. (4.6-m) diameter bins with 60º hopper</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01502</td>
                <td width="53" > 41.5</td>
                <td width="112" > 37.7</td>
                <td width="102" > 2076</td>
                <td width="64" ="58.79"> 58.8</td>
                <td width="61"> 25' 10"</td>
                <td width="82" > 7.9</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01503</td>
                <td width="53" > 50.9</td>
                <td width="112" > 46.1</td>
                <td width="102" ="2542.5"> 2543</td>
                <td width="64" > 72.0</td>
                <td width="61"> 28' 6"</td>
                <td width="82" > 8.7</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01504</td>
                <td width="53" > 60.2</td>
                <td width="112" > 54.6</td>
                <td width="102" ="3008.6"> 3009</td>
                <td width="64" ="85.19"> 85.2</td>
                <td width="61"> 31' 2"</td>
                <td width="82" > 9.5</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01505</td>
                <td width="53" > 69.5</td>
                <td width="112" > 63.0</td>
                <td width="102" ="3474.9"> 3475</td>
                <td width="64" > 98.4</td>
                <td width="61"> 33' 10"</td>
                <td width="82" > 10.3</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01506</td>
                <td width="53" > 78.8</td>
                <td width="112" > 71.5</td>
                <td width="102" ="3941.2"> 3941</td>
                <td width="64" > 111.6</td>
                <td width="61"> 36' 6"</td>
                <td width="82" > 11.1</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01507</td>
                <td width="53" > 88.2</td>
                <td width="112" > 80.0</td>
                <td width="102" ="4407.5"> 4408</td>
                <td width="64" ="124.81"> 124.8</td>
                <td width="61"> 39' 2"</td>
                <td width="82" > 11.9</td>
            </tr>
            <tr height="21"> <td width="107" height="21"> 45-01508</td>
                <td width="53" > 97.5</td>
                <td width="112" > 88.4</td>
                <td width="102" ="4873.8"> 4874</td>
                <td width="64" ="138.01"> 138.0</td>
                <td width="61"> 41' 10"</td>
                <td width="82" > 12.8</td>
            </tr> </table> </td>
        <td width="34%"> </td>
        </tr> </table>
        <?php 
    } 
}//end class
