<?php 
/* display data from DB on current beef prices */

    class BeefReport extends BasePage{            
             
         function defaultDisplay() {
              print("<div class='row'>");

                    print("<div class='col-md-12' >");
                    print($this->beefQuebecLatestDaysPrice());
                    print("<br/>");
                    $this->BeefQuebecHistoricalPriceChart();
                    print("</div>");
              print("</div>");
            
         }
        
         
        public function beefQuebecLatestDaysPrice() {
             $sql="SELECT date,cull_cow_avg_price,cull_cow_volume, good_male_calf_90_120_avg_price,good_male_calf_90_120_volume, good_male_calf_121_140_avg_price, good_male_calf_121_140_avg_volume,avg_male_calf_90_140_avg_price, avg_male_calf_90_140_avg_volume,misc_calf_all_weights_avg_price,misc_calf_all_weights_avg_volume FROM batch.beef_report_quebec_all_auctions ORDER BY date DESC limit 1";
             $row = $GLOBALS ['pdo']->query($sql)->fetch(PDO::FETCH_ASSOC);
             if (strtotime($row['date']) > strtotime("-1 week")) {
           $out[]= '<h2> Latest prices as of '. date('F d, Y',strtotime($row['date'])) .'</h2><br/>';
           $out[]='<div class="container">
                       <div class="row">';    
                $out[]= $this->priceItem('Good male calf 90 to 120 lb',$row['good_male_calf_90_120_avg_price'],$row['good_male_calf_90_120_volume']);
                $out[]= $this->priceItem('Good male calf 121 to 140 lb',$row['good_male_calf_121_140_avg_price'],$row['good_male_calf_121_140_avg_volume']);
                $out[]= $this->priceItem('Average male calf 90 to 140 lb',$row['avg_male_calf_90_140_avg_price'],$row['avg_male_calf_90_140_avg_volume']);
                $out[]= $this->priceItem('Misc calf all weights',$row['misc_calf_all_weights_avg_price'],$row['misc_calf_all_weights_avg_volume']);
                $out[]= $this->priceItem('Cull Cow',$row['cull_cow_avg_price'],$row['cull_cow_volume']);
          $out[]='</div></div>';
             }
             else {
                 $out[]= '<h2> No current Quebec Beef prices available.</h2>';  
             }
             return implode('', $out);    
         }
        
    private function priceItem($name,$price,$volume) {
$html = <<<HTML

    <div class="col-md-6">
         <div class="info-box">
        <span class="info-box-icon bg-green"><i class="fa fa-gavel"></i></span>
         <div class="info-box-content">
      <h4 class="list-group-item-heading">$name</h4>
      <p style="padding-top: 10px;" class="list-group-item-text small"><strong>Price ($/lb): </strong>$price</p>
      <p class="list-group-item-text small"><strong>Volume (head): </strong>$volume</p>
      <span style="clear: both;"></span>
         </div>  </div>
    </div>
     
        
HTML;

return $html;
        
    }
         
         
    

    public function BeefQuebecLatestVolumeMaleCalf($request) {
         $array = array();
         
        //latest data available 
        $sql = "SELECT * FROM batch.beef_report_quebec_all_auctions ORDER BY date DESC limit 1"; 
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        //only 1 row
        foreach ($results as &$row) {
        //make google viz php format.
        $array['cols'][] = array('id' =>'','label' => 'Volume','type' => 'string', 'pattern'=>''); //y axis #1
        $array['cols'][] = array('id' =>'','label' => 'price','type' => 'number', 'pattern'=>''); //y axis
         
            $array['rows'][]['c'] = array( array('v' => 'Good 90-120lb: $'.$row['good_male_calf_90_120_avg_price']), array('v' =>  (float)$row['good_male_calf_90_120_volume']));
            $array['rows'][]['c'] = array( array('v' => 'Good 121-140lb: $'.$row['good_male_calf_121_140_avg_price'], 'f'=>null), array('v' => (float)$row['good_male_calf_121_140_volume'], 'f'=>null));
            $array['rows'][]['c'] = array( array('v' => 'Avg 90-140lb: $'.$row['avg_male_calf_90_140_avg_price'], 'f'=>null),   array('v' =>  (float)$row['avg_male_calf_90_140_volume'], 'f'=>null));
            $array['rows'][]['c'] = array( array('v' => 'Misc All Weights: $'.$row['misc_calf_all_weights_avg_price']), array('v' => (float)$row['misc_calf_all_weights_volume']));             
          }
      
        return $array;
    }
     
     public function BeefQuebecLatestPriceMaleCalf($request) {
         $array = array();
         $sql = "SELECT * FROM batch.beef_report_quebec_all_auctions ORDER BY event_time DESC limit 1";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        
        //only 1 row
        foreach ($results as &$row) {
        //make google viz php format.
        $array['cols'][] = array('id' =>'','label' => 'Volume','type' => 'string', 'pattern'=>''); //y axis #1
        $array['cols'][] = array('id' =>'','label' => 'price','type' => 'number', 'pattern'=>''); //y axis
         
            $array['rows'][]['c'] = array( array('v' => 'good_male_calf_90_120_avg_priceDD'), array('v' =>  (float)$row['good_male_calf_90_120_volume']));
            $array['rows'][]['c'] = array( array('v' => 'good_male_calf_121_140_avg_price', 'f'=>null), array('v' => (float)$row['good_male_calf_121_140_volume'], 'f'=>null));
            $array['rows'][]['c'] = array( array('v' => 'avg_male_calf_90_140_avg_price', 'f'=>null),   array('v' =>  (float)$row['avg_male_calf_90_140_volume'], 'f'=>null));
            $array['rows'][]['c'] = array( array('v' => 'misc_calf_all_weights_avg_price'), array('v' => (float)$row['misc_calf_all_weights_volume']));             
          }
      
        return $array;
    }
     
    
     
     
     
    
     public function BeefQuebecHistoricalPriceChart() {
         ?>
          <!--Load the AJAX API-->
  
    <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
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
          url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php echo($this->pageid);?>/BeefQuebecAllPrices/",
          dataType:"json",
          async: false
          }).responseText;
          
      // Create our data table out of JSON data loaded from server.
      var data = new google.visualization.DataTable(jsonData);

      // Instantiate and draw our chart, passing in some options.
      var chart = new google.visualization.LineChart(document.getElementById('chart_divF'));
      var options = {
                                    title: 'Quebec Beef Auction: Historical Pricing',
                                            
                                           
                                           hAxis: {
                  title: 'Date'
                },
                vAxis: {
                  title: 'Avg Price Live Weight ($/lb)'
                }, 
                chartArea: {left:"5%", width:"80%",height:"60%"},
               
                                    };
      
      
      chart.draw(data, options);
    }

    </script>
     <div id="chart_divF" class="chartGoogle"></div>
    
    <?php 
     }
    
         public function BeefQuebecAllPrices($request) :array {
             $array = array();
         $sql = "SELECT date,cull_cow_avg_price, good_male_calf_90_120_avg_price, good_male_calf_121_140_avg_price, avg_male_calf_90_140_avg_price, misc_calf_all_weights_avg_price FROM batch.beef_report_quebec_all_auctions";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        
        //change presentation
        foreach ($results as &$row) {
           $row['event_time']=date("m-d-y",strtotime($row['event_time']));          
        }
       
         //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'string', 'label' => 'date');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Cull Cow'); //y axis #1
        $array['cols'][] = array('type' => 'number', 'label' => 'Good Male Calf 90-120'); //y axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Good Male Calf 121-140'); //y axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Avg Male Calf 90-140'); //y axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Misc Calf All Weights'); //y axis
        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['date']}"),
                array('v' => $row['cull_cow_avg_price']),
                array('v' => $row['good_male_calf_90_120_avg_price']),
                array('v' => $row['good_male_calf_121_140_avg_price']),
                array('v' => $row['avg_male_calf_90_140_avg_price']),
                array('v' => $row['misc_calf_all_weights_avg_price'])
            );
        }
        return $array;
    }
    
    
     public function error() {
        $error = array();

        //if beef price has not been entered for 51 hours, throw an error.
        $res = $GLOBALS['pdo']->query("SELECT max(date) as max FROM batch.beef_report_quebec_all_auctions");
        //first check there is anything to check.
        if ($res->rowCount()==0) {
             $error[] = new notifyObj(get_class($this), date('Y-m-d',strtotime("now")), 3, 'Quebec Beef Report script has error in it.'); 
             return $error;
        }
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ((strtotime($row['max']) + 120*3600) <= strtotime("now")) {
            if (empty($row['max'])) {$row['max']='01-01-1970';} //this is called when table is empty
            $error[] = new notifyObj(get_class($this), $row['max'], 2, 'Quebec Beef Report is overdue.');
        }
        return $error;
    }
    
    
    }//end class
 ?>