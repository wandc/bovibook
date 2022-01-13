<?php 
class RowCropping extends TabPage {

    function pageHeadJavascript() {
        // print('<script type="text/javascript" src="javascript/jquery.mobile-1.2.0/jquery.mobile-1.2.0.js"></script>'."\n"); //change to only be loaded by mobile devices???
    }

    function customCSS() {
        //   print('<link rel="stylesheet" href="/javascript/jquery.mobile-1.2.0/jquery.mobile-1.2.0.min.css" type="text/css" media="screen" />'."\n");//jquery ui theme  
    }

    public function defaultDisplay() {

        //setup for tabs.
        $tabArray[1]['name'] = 'List';
        $tabArray[2]['name'] = 'List2 by ph';
        $tabArray[3]['name'] = 'Lime Chart';
        $tabArray[4]['name'] = 'Lime for Scheduled Seedings';
        $tabArray[5]['name'] = 'Potassium Requirements';

        $this->renderTabs($tabArray);
    }

    protected function tab1() {

        //$this->soilTestNeeded(null);
        print($this->forageFields());
    }

    protected function tab2() {

        $this->displaySoilTestNeeded('pH');
      
    }

     protected function tab3() {

        $this->LimeChart();
      
    }
    
    
    protected function tab4() {


        
       print( $this->pHforCurrentScheduledFields());
    }

    protected function tab5() {
        
        
       print $this->fertPotassiumRecomendation();
        
        
    }
    
    
    public function fertPotassiumRecomendation() {

        $sql = <<<SQL
                          with temp as (
          SELECT *,id as field_id,(SELECT max(event_time) FROM cropping.soil_sample_event WHERE foragecurr.id=field_id) as last_soil_test 
           FROM cropping.foragecurr 
            )
SELECT *,
(SELECT cec from cropping.soil_sample_event WHERE field_id=temp.field_id AND event_time=temp.last_soil_test) as cec,
                 (SELECT potassium from cropping.soil_sample_event WHERE field_id=temp.field_id AND event_time=temp.last_soil_test) as k
     FROM temp order by alfalfa_suitable DESC
SQL;



        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

       
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/cropping/soilSample.inc');
        $fertObj = (new FertilzerRequirements);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Area'] = ($row['area']);
            $rowOut['K Recommended'] = $fertObj->pottassiumAlfalfa($row['k'], $row['cec']);

            $resultsOut[] = $rowOut;
        }
        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('K Recommended', $dataArr, 'order: false'));
        return implode($out);
    }
    
    

    public function pHforCurrentScheduledFields()
    {
        
                $sql = <<<SQL
with temp as (
SELECT *,
seed_event_scheduled.field_id as field_idX,
seed_event_scheduled.id as seed_event_scheduled_id, 
seed_event_scheduled.id as delete_table_id, 'seed_event_scheduled' as delete_table_name,
seed_event_scheduled.event_time,
comment,(SELECT max(event_time) FROM cropping.soil_sample_event WHERE fieldcurr.id=field_id) as last_soil_test


        FROM cropping.seed_event_scheduled 
LEFT JOIN cropping.fieldcurr on fieldcurr.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        WHERE  seed_event_scheduled.event_time >='2021-01-01' AND seed_event_scheduled.event_time < '2021-12-31'  ORDER BY specific_type
)
SELECT *,
(SELECT ph from cropping.soil_sample_event WHERE field_id=temp.field_idX AND event_time=temp.last_soil_test) as ph,
(SELECT buffer_ph from cropping.soil_sample_event WHERE field_id=temp.field_idX AND event_time=temp.last_soil_test) as buffer_ph,
(SELECT magnesium from cropping.soil_sample_event WHERE field_id=temp.field_idX AND event_time=temp.last_soil_test) as magnesium,
(SELECT sum(lime_amount) from cropping.lime_event WHERE field_id=temp.field_idX AND lime_id=3971 AND event_time>=temp.last_soil_test) as mag_lime_since_last_test,
(SELECT sum(lime_amount) from cropping.lime_event WHERE field_id=temp.field_idX AND lime_id=3972 AND event_time>=temp.last_soil_test) as cal_lime_since_last_test

 FROM temp  
SQL;        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            
            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Area'] = ($row['area']);
           
            $rowOut['specific_type'] = ($row['specific_type']);
            
            
              $rowOut['ph'] = ($row['ph']);
             $rowOut['magnesium'] = ($row['magnesium']);
             
               $rowOut['mag_lime_since_last_test'] = ($row['mag_lime_since_last_test']);
                 $rowOut['cal_lime_since_last_test'] = ($row['cal_lime_since_last_test']);
                  $rowOut['last_soil_test'] = ($row['last_soil_test']);
           
              
            
           
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
    
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Lime for Scheduled Seedings', $dataArr,'order: false')); 
         return implode($out); 
        
        
        
    }
    
    
    /*
     * shows fields that need lime all at once.
     */
             public function LimeChart(){
        $sql = "With temp as (
SELECT fieldx.id,alpha_numeric_id,common_name, alfalfa_suitable,corn_suitable,trefoil_suitable, area,
(SELECT max(event_time) FROM cropping.soil_sample_event WHERE fieldx.id=field_id) as last_soil_test
FROM cropping.fieldcurr AS  fieldx
WHERE (alfalfa_suitable = true OR corn_suitable = true OR trefoil_suitable = true) AND active=true
)
SELECT temp.id as field_id,*,(SELECT ph from cropping.soil_sample_event WHERE field_id=temp.id AND event_time=temp.last_soil_test) as ph,
(SELECT buffer_ph from cropping.soil_sample_event WHERE field_id=temp.id AND event_time=temp.last_soil_test) as buffer_ph,
(SELECT magnesium from cropping.soil_sample_event WHERE field_id=temp.id AND event_time=temp.last_soil_test) as magnesium,
(SELECT sum(lime_amount) from cropping.lime_event WHERE field_id=temp.id AND lime_id=3971 AND event_time>=temp.last_soil_test) as mag_lime_since_last_test,
(SELECT sum(lime_amount) from cropping.lime_event WHERE field_id=temp.id AND lime_id=3972 AND event_time>=temp.last_soil_test) as cal_lime_since_last_test
FROM temp 
ORDER BY (substr(temp.alpha_numeric_id::text, 0, strpos(temp.alpha_numeric_id::text, '-'::text) + 1)::character(5)), (substr(temp.alpha_numeric_id::text, strpos(temp.alpha_numeric_id::text, '-'::text))::integer) DESC;
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
             ['alpha_numeric_id', 'pH', 'magnesium', 'region', 'area'],
           <?php 
              $res = $GLOBALS['pdo']->query($sql);
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
                if ($row['ph'] >= 6) {
                    $row['colour']='Over 6 pH';
                }else {
                     $row['colour']='Needs Cal-Lime';
                }
                
                if  ($row['magnesium'] < 150) {
                     $row['colour']='Needs Mag-Lime';
                }
                
                //if field had lime since last test. basically 1.1 tonne raises pH .1 per hectar. //my observation.
            if ((!empty($row['mag_lime_since_last_test'])) AND ( !empty($row['cal_lime_since_last_test']))) {
                $row['ph']=$row['ph']+($row['mag_lime_since_last_test']+$row['cal_lime_since_last_test'])*.1;
                 $row['magnesium']=$row['magnesium']+11.12*$row['mag_lime_since_last_test'];
                  $row['colour']='Had Lime since last soil test';
            } elseif (!empty($row['mag_lime_since_last_test'])) {
                  $row['ph']=$row['ph']+($row['mag_lime_since_last_test'])*.1;
                  //from brookville's website: Calcium 21%, Magnesium 11.5% in dolmitic lime, 
                  // http://msue.anr.msu.edu/uploads/resources/pdfs/Facts_about_Soil_E1566.pdf
                  // multiply by 0.29 to get pounds of actual element M per tonne. 0.30 times 2,000lb/tonne times 0.29
                  // for us that is 33.35 kg/ha.
                  //DIVIDE by 3 to get ppm for 30cm of soil.
                  $row['magnesium']=$row['magnesium']+11.12*$row['mag_lime_since_last_test'];
                    $row['colour']='Had Lime since last soil test';
            } elseif (!empty($row['cal_lime_since_last_test'])) {
                  $row['ph']=$row['ph']+($row['cal_lime_since_last_test'])*.1;
                    $row['colour']='Had Lime since last soil test';
            }


            print("['{$row['alpha_numeric_id']}', {$row['ph']},	{$row['magnesium']},  '{$row['colour']}', {$row['area']}],");    
            }       
                    ?>
        
         
        ]);

        var options = {
          title: 'Fields Needing Lime',
          hAxis: {title: 'pH'},
          vAxis: {title: 'Magnesium'},
          bubble: {textStyle: {fontSize: 11}},
          colorAxis: {colors: ['yellow', 'red']}
        };




        var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all2'));
        chart.draw(data, options);
      }
    </script>

 


<?php 
print("<div id='chart_divv_all2' style='width: 100%; height: 600px;'></div>");
print("<h3>Uses soil tests and if lime has been applied, it estimates what the actual level in the soil is, the day after it was applied.</h3>");
 
    }
    
    
    function forageFields() {

        $sql = "SELECT *,id as field_id from cropping.foragecurr order by alfalfa_suitable DESC,corn_suitable DESC";
        
           $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            
            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Area'] = ($row['area']);
            $rowOut['alfalfa_suitable'] = ($row['alfalfa_suitable']);
            $rowOut['corn_suitable'] = ($row['corn_suitable']);
            $rowOut['trefoil_suitable'] = ($row['trefoil_suitable']);
            
           
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Forage Fields</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'order: false')); 
         return implode($out); 
        
        
    }

   

    
    
    
    
    public function soilTestsNeeded($request) {
      
      
          $mode = Sanit::reqVal('item', false, 'STR');
          
        if ($mode == 'pH') {
            $sql = "With temp as (
SELECT fieldx.id,alpha_numeric_id,common_name, alfalfa_suitable,corn_suitable,trefoil_suitable, area,(SELECT max(event_time) FROM cropping.soil_sample_event WHERE fieldx.id=field_id) as last_soil_test

FROM cropping.fieldcurr AS  fieldx

WHERE (alfalfa_suitable = true OR corn_suitable = true OR trefoil_suitable = true) AND active=true
)
SELECT temp.id as field_id,*,(SELECT ph from cropping.soil_sample_event WHERE field_id=temp.id AND event_time=temp.last_soil_test) as ph,(SELECT buffer_ph from cropping.soil_sample_event WHERE field_id=temp.id AND event_time=temp.last_soil_test) as buffer_ph,(SELECT magnesium from cropping.soil_sample_event WHERE field_id=temp.id AND event_time=temp.last_soil_test) as magnesium
FROM temp 
ORDER BY (substr(temp.alpha_numeric_id::text, 0, strpos(temp.alpha_numeric_id::text, '-'::text) + 1)::character(5)), (substr(temp.alpha_numeric_id::text, strpos(temp.alpha_numeric_id::text, '-'::text))::integer) DESC;
";
        } else {
throw new exception("Error called with wrong paramater for soil test needed.");
        }
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
  
     $row['field'] =JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'],$row['alpha_numeric_id']);    
           $row['event_time_raw']=strtotime($row['last_soil_test']);
          
           $row['event_time']= date("M d Y H:00",strtotime($row['last_soil_test']));  //has to be at end.
         
              $row['colour_row']= $this->printImportantField($row);
              $row['lime_requirement']=(new SoilSample)->printLimeRequirementsNB($row['ph'], $row['buffer_ph'] );
         //  if  ((($row['estrus_type_id'])==1) AND empty($row['note']))  {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
     function displaySoilTestNeeded($mode='pH') {
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },                
                {
                data: "field",
                title: "Field"
            },                
            {
                data: "ph",
                title: "pH"
            },                
            {
                data: "magnesium",
                title: "Magnesium (Mg)"
            },
            {
                data: "area",
                title: "Size (Ha)"
            },
            {
                data: "colour_row",
                title: "Important Field"
            },
            {
                data: "lime_requirement",
                title: "lime_requirement"
            },
            {
                data: "event_time",
                title: "Event Time"
            }
EX;
         print($dt->startCustom('Title Custom CHANGE ME', 124,'soilTestsNeeded',$mode,$columnsStr,'highlight_attention',' paging: false'));     
     
    }
    
    /* prints the ph with strike through when there has been lime event since soil test */

    function printPh($params) {
        extract($params);

        if (!empty($record['last_soil_test'])) {

            $sql = "SELECT id FROM cropping.lime_event WHERE field_id={$record['id']} AND event_time >='{$record['last_soil_test']}' AND event_time <= now()";
            $res = $GLOBALS['pdo']->query($sql);

            if ($res->rowCount() > 0) {
                return $record['ph'] . ' <b>*</b>';
            } else {
                return $record['ph'] . '';
            }
        } else {
            return 'Needs Soil Test';
        }
    }

    function printImportantField($record) {


        if ($record['alfalfa_suitable'] == 't' AND $record['corn_suitable'] == 't' AND $record['trefoil_suitable'] == 't') {

            return 1;
        } else {
            return 0;
        }
    }
}

//end class
?>         
    
    
    