<?php 
class TmrReport extends TabPage {

    public function defaultDisplay() {
        ini_set('memory_limit','512M');

        //setup for tabs.
        $tabArray[1]['name'] = 'Historical Chart';
          $tabArray[2]['name'] = 'Log';
       
        $this->renderTabs($tabArray);
    }

//end function

    function tab1() {
     
       $massArray=$this->tmrLast3DaysMass();
     //  print_r($massArray);
       $this->tmrReportChart(1);
       
    }

     function tmrReportChart($chart_id) {
           print("<h3>TMR Mixer Mass Changes</h3>");
        ?>

        <!--Load the AJAX API-->
      
       
        <script type="text/javascript">

            // Load the Visualization API and the piechart package.
     



            function drawChartB() {
                var jsonData = $.ajax({
                    url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/tmrLast3DaysMass/<?php echo($chart_id); ?>",
                    dataType: "json",
                    async: false
                }).responseText;

                // Create our data table out of JSON data loaded from server.
                var data = new google.visualization.DataTable(jsonData);

                // Instantiate and draw our chart, passing in some options.
                var chart = new google.visualization.LineChart(document.getElementById('chart_divB_<?php echo($chart_id); ?>'));
                chart.draw(data, {width: 1200, height: 300});
            }

            // Set a callback to run when the Google Visualization API is loaded.
            google.charts.setOnLoadCallback(drawChartB, true);
             $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChartB);
            });

        </script>
     
        <div id="chart_divB_<?php echo($chart_id); ?>" ></div>
        <?php 
    }
    
    function c() {
           $sql = "Select *,(bovinemanagement.location_total_at_timestamp(location_id,now()::timestamp)) as location_count, (bovinemanagement.location_total_at_timestamp(location_id,now()::timestamp)*modifier) as location_mod_count from nutrition.nrc_recipe_location "
                   . "LEFT JOIN bovinemanagement.location ON location.id=nrc_recipe_location.location_id WHERE nrc_recipe=$nrc_recipe_id";
        $res = $GLOBALS['pdo']->query($sql);

        $locationNames = '';
        $mixSumArr=array();
        while ($row3 = $res->fetch(PDO::FETCH_ASSOC)) {
            $mixSumArr[]= $mix_by_location_kg_day_wet_sum = round( $row3['location_mod_count'] * $recipe_kg_day_wet_sum);
        }
        
        return $mixSumArr;
    }
    
    //display weigths on mixer scale last 3 days.
    function tmrLast3DaysMass() {
        
		$sql="SELECT event_time,display_weight FROM bas.tmr_event WHERE event_time >= current_date - (interval '2 days') 
AND display_weight > 50
ORDER BY event_time ASC";
                
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
        $array['cols'][] = array('type' => 'number', 'label' => 'mass'); //y axis #1
         $array['cols'][] = array('type' => 'number', 'label' => 'mass2'); //y axis #1

        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_timeNEW']}"),
                array('v' => $row['display_weight']),
                array('v' => 2000),        
            );
        }
        return $array;
    }
                
                
                
                
                
                
          
    
  function dimGoogleVisualizationLineChart($massArray) {

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Display Weight (kg)'] = 'number';
         $opt='';
        $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $massArray);
        print($x->toString());
    }
}
//end class
?>