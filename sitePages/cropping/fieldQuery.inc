<?php


class FieldQuery extends TabPage {

    public $field_id;

    public function __construct() {
        parent::__construct();
        if ((isset($_REQUEST['field_id']) == false) || ($_REQUEST['field_id'] == null)) {
            $_REQUEST['field_id'] = -1;  //dummy value
        }
        if (!filter_var($_REQUEST['field_id'], FILTER_VALIDATE_INT) === false) {
            $this->field_id = $_REQUEST['field_id'];
        } else {
            throw new Exception('Error: field_id request variable is not an interger.');
        }
    }

    function customCSS() {
        print('<link rel="stylesheet" type="text/css" media="all" href="/css/fieldQuery.css" />' . "\n");
    }

    //test function
    function gpsEvents($field_id) {

        echo '{"aaData":' . (TrimbleGPS::gpsEvents($_REQUEST['item'])) . '}';
        return null;
    }

    public function defaultDisplay() {

        if (!empty($this->field_id)) {
       print(new FieldQueryHeader($this->field_id))->main();     
  
//setup for tabs.
        $tabArray[1]['name'] = 'Summary';
        $tabArray[1]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[2]['name'] = 'Sub-Surface';
        $tabArray[2]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[3]['name'] = 'Border Datum';
        $tabArray[3]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[4]['name'] = 'Above Surface';
        $tabArray[4]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[5]['name'] = 'Datum Intersections';
        $tabArray[5]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[6]['name'] = 'GPS Data';
        $tabArray[6]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[7]['name'] = 'Planner';
        $tabArray[7]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[8]['name'] = 'Map';
        $tabArray[8]['extra_callback_param'] = "&field_id={$this->field_id}";
        $this->renderTabs($tabArray);
        }
        //when they are not set, show field select list.
        else {
        print(new BootStrap)->warningCallout("Choose field from select list above.");
        }
        
       
    }

   
    /*

      with temp as (
      SELECT field_id,seed_event.id as seed_event_id,event_time
      event_time,polygon FROM cropping.seed_event LEFT JOIN cropping.datum ON datum.id=seed_event.datum_id
      ), temp2 as (
      SELECT a.seed_event_id as a_seed_event_id,a.event_time as a_event_time,b.seed_event_id as b_seed_event_id,b.event_time as b_event_time,a.polygon as a_polygon,b.polygon as b_polygon FROM temp a
      CROSS JOIN temp b WHERE   (a.polygon && b.polygon) order by b.event_time DESC
      ), temp3 as (
      /* now we need to select the newest */ /*
      SELECT DISTINCT(a_seed_event_id) FROM temp2 xxx WHERE xxx.a_seed_event_id =a_seed_event_id AND xxx.a_event_time= (SELECT max(a_event_time)  FROM temp2 WHERE xxx.a_seed_event_id=a_seed_event_id)
      )
      SELECT * FROM temp3 LEFT JOIN cropping.seed_event ON seed_event.id=temp3.a_seed_event_id WHERE field_id=3247 ORDER BY field_id
     */

    protected function tab1() {


        $this->displayEventsSummary();

        //print($this->displayLatestFieldEvents($this->field_id));
        print('<hr/>');
    }
    
/*
 * sub surface
 */
    protected function tab2() { 

        $accord=(new fieldQuerySoilAccordian)->showAccordion($this->field_id);
        $soilInfo=$this->displayFieldSoilInfo($this->field_id);
        $ss = new SoilSample();
        $getLatestSoilSampleObj = $ss->getLatestSoilSample($this->field_id);
        $displayFieldLimeRequirements=$this->displayFieldLimeRequirements($this->field_id);
        $soybeanNutrientRequirements=$this->soybeanNutrientRequirements($this->field_id);
         
          
        if ($getLatestSoilSampleObj != null) {
            
           $displaySingleSoilSampleFancy=( $ss->displaySingleSoilSampleFancy($getLatestSoilSampleObj) );
           $FertilizerRecommendationPerCrop=$ss->FertilizerRecommendationPerCrop($getLatestSoilSampleObj, 'soybeans');
           
           //2021 version for alfafla
           $k=(new FertilzerRequirements)->pottassiumAlfalfa($getLatestSoilSampleObj->k, $getLatestSoilSampleObj->cec);
           $fertilzerRequirementsAlfalfa= (new BootStrap)->plainCard('Alfalfa Potash Recommendation', $k);
           
           
        }
        
        
        $left = <<<HTML
                {$this->displaySoilEventsSummary()}        
        HTML; 
        
        $centre = <<<HTML
                {$displaySingleSoilSampleFancy}
            {$soilInfo}     
                {$fertilzerRequirementsAlfalfa}
            {$FertilizerRecommendationPerCrop}
             {$FertilizerRecommendationPerCrop}
            {$displayFieldLimeRequirements}
            {$soybeanNutrientRequirements}  
        HTML;     
            
        $right = <<<HTML
            {$accord}   
           
        HTML; 
        
        $content=(new Bootstrap)->oneThirdTwoThirds($left.$centre,$right);                        
        print($content);
    }

    protected function tab3() {


        $this->displayFieldHistoricalDatum($this->field_id);
    }

    protected function tab4() {
      
    }

    protected function tab5() {
        $this->findAllNonBorderDatumsWhoIntersect($this->field_id);
    }

    protected function tab6() {

        //use the current border as the geom to use to see if gps data intersects.
        $sql = "SELECT border_geom FROM cropping.fieldcurr WHERE id=$this->field_id";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        //display map, on tab heigth issues....
        //maybe just display static map to begin with until we figure out what do to with all the data???    
        TrimbleGPS::eventList($this->field_id);

        $trimble = new TrimbleGPS($row['border_geom']);
        $trimble->main();
    }

    protected function tab7() {
        //if we are past Sept 1 assume we are planning for the next year
        if (date('m') >= 9) {
            $year = date('Y', strtotime('+1 year'));
        } else {
            $year = date('Y');
        }

        //find out what crop is scheduled to be planted.
        $row = (new Field)->displayFieldsForCurrentSeasonPlanting($year, $this->field_id);
        if (empty($row)) {
        $specific_type = $row['specific_type'];
        
        $req = new CroppingNutrientRequirements($this->field_id, $year);
        $req->main($specific_type);
        }
        
    }
    
    
    protected function tab8() {
        
         $map=FieldQuery::displayCurrentFieldMap($this->field_id,17,800,800);
         print($map);
    }

   

    function soybeanNutrientRequirements($field_id) {

        $soilTerstArr = SoilSample::latestFieldSoilTest($field_id);
        $out=array();
        if (!empty($soilTerstArr)) {
        $out[]=("<h4>Soybean Requirments</h4>");
        $out[]=("Based on {$soilTerstArr['report_date']} soil test.<br/>");
        $out[]=("Nitrogen: 0 kg/ha</br>"); //always zero for soybeans.
        $Phosphorus = $this->soybeanPhosphorusRequirements($soilTerstArr['phosphorus']);
        $out[]=("Phosphorus: $Phosphorus kg/ha<br>");
        $Potassium = $this->soybeanPotassiumRequirements($soilTerstArr['potassium']);
        $out[]=("Potassium: $Potassium kg/ha<br>");
        }
        
          return (new BootStrap())->plainCard('Soybean Nutrient Requirements', implode($out));
    }

    //http://www.omafra.gov.on.ca/english/crops/pub811/2fertility.htm   
    function soybeanPhosphorusRequirements($P_soil_test) {
        //comparing to true value.   
        //requirements kg/ha
        switch (TRUE) {
            case ($P_soil_test <= 3 ): return 80;
            case ($P_soil_test <= 5): return 60;
            case ($P_soil_test <= 7): return 50;
            case ($P_soil_test <= 9): return 40;
            case ($P_soil_test <= 12): return 30;
            case ($P_soil_test <= 15): return 20;
            case ($P_soil_test <= 30): return 0;
            case ($P_soil_test <= 60): return 0;
            default: return 0; //61+
        }
    }

    //http://www.omafra.gov.on.ca/english/crops/pub811/2fertility.htm 
    function soybeanPotassiumRequirements($K_soil_test) {
        //comparing to true value.    
        //requirements kg/ha
        switch (TRUE) {
            case ($K_soil_test <= 15 ): return 120;
            case ($K_soil_test <= 30): return 110;
            case ($K_soil_test <= 45): return 90;
            case ($K_soil_test <= 60): return 80;
            case ($K_soil_test <= 80): return 60;
            case ($K_soil_test <= 100): return 40;
            case ($K_soil_test <= 120): return 30;
            case ($K_soil_test <= 150): return 0;
            case ($K_soil_test <= 250): return 0;
            default: return 0; //250+
        }
    }

    //////////////////////////////////////////////////////

    /**
     * This is used to find datum polygons that were used to cover a field partially with manure for example.
     */
    function findAllNonBorderDatumsWhoIntersect($field_id) {
        /* finds the latest border id for a field and then see which datum polygon intersects it. */
        $sql = "
   	SELECT datum.id,gis.ST_AsText(gis.ST_Boundary(datum.geom)) AS linestring, gis.ST_Area(gis.ST_Transform(datum.geom, 2036))/10000 as area FROM cropping.datum
   	LEFT JOIN cropping.border_event ON border_event.datum_id=datum.id
   	WHERE geom &&  (SELECT geom FROM cropping.datum WHERE id=
   	 ( SELECT border.datum_id
                   FROM cropping.border_event border
                  WHERE border.field_id = $field_id AND border.event_time = (( SELECT max(border.event_time) AS max
                           FROM cropping.border_event border
                          WHERE border.field_id = $field_id AND border.event_time <= 'now'::text::date)))
   	) 
   	AND datum_id IS null
   	";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            print("<h3>Daturm id:{$row['id']}</h3>");
            $this->fieldMap($row['linestring'], $borderColour = '0xffa50080', $fillColour = '0xffa5004d'); //use a different colour to indicate not a border.
            print("<p>Polygon: {$row['linestring']}</p>");
            print ("<p>Area: " . round(($row['area']), 2) . " ha</p>");
            print('<hr/>');
        }
    }

   
    /**
     * Finds all historical borders for this field
     */
    function displayFieldHistoricalDatum($field_id) {
        $sql = "
   	SELECT datum.id as datum_id,gis.ST_AsText(gis.ST_Boundary(geom)) as linestring,event_time 
        FROM cropping.border_event LEFT JOIN cropping.datum ON datum.id = border_event.datum_id WHERE field_id=$field_id ORDER BY event_time DESC
   	";
        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("<h2>Border from " . date('M d, Y', strtotime($row['event_time'])) . '.</h2>');
            $this->fieldMap($row['linestring']);
            print("<p>Daturm id: {$row['datum_id']}</p>");
            print("<p>{$row['linestring']}</p>");

            $outArr = (new CroppingHelper)->convertLineStringToLatLngArray($row['linestring']);
            $kml = (new CroppingHelper)->ConvertArrayToLatLongHeightStr($outArr);
            print("KML: $kml");

            print('<hr/>');
        }
    }

    public static function displayCurrentFieldMap($field_id, $zoom = 15,$sizeX=320,$sizeY=150) {
        //lookup the current border of the field.
        $sql = "
 	SELECT gis.ST_AsText(gis.ST_Boundary(border_geom)) as linestring FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1
 	";
        $res = $GLOBALS['pdo']->query($sql);

        if ($res->rowCount() > 0) {
            $row = $res->fetch(PDO::FETCH_ASSOC);
            return self::fieldMap($row['linestring'], '0xff0000ff', '0xFF33CC33', $zoom,$sizeX,$sizeY);
        } else {
            //throw new Exception("Field does not have a default border defined.");
            return null; //retired fields have no border.
        }
    }

    /**
     * shows a map of any polygon requested.
     */
    private static function fieldMap($linestring, $borderColour = '0xff0000ff', $fillColour = '0xFF33CC33', $zoom = 17,$sizeX=320,$sizeY=150) {

        //convert to format for google maps
        $latlon = (new GoogleChartsHelper)->postgresLineStringToLatLongArray($linestring);
        $str = (new GoogleChartsHelper)->latLongArrayToGoogleMapsEncodedPolyline($latlon);

        return ("<img id='fieldHeaderMap' src='https://maps.googleapis.com/maps/api/staticmap?key={$GLOBALS['config']['GOOGLE_MAPS']['API_KEY']}&path=color:$borderColour%7Cfillcolor:$fillColour%7cweight:2%7C$str&size={$sizeX}x{$sizeY}&sensor=false&maptype=hybrid&zoom=$zoom'/>");
    }

   

    /**
     * 
     * displays the lastest events for this field.
     */
    function displayLatestFieldEvents($field_id) {
        $str = '';
        $sql = "
 	SELECT * from cropping.all_event WHERE field_id=$field_id ORDER BY event_time DESC
 	";

        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['event_time']);
            $ftime = date('M j, Y', $time);

            $str = $str . "<b>{$row['event_name']}</b>: " . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b>  {$row['id']}<br/>";
        }
        return $str;
    }

    /**
     * Displays info on lime requirements for the fields 
     * 
     */
    function displayFieldLimeRequirements($field_id) {
       return SoilSample::printLimeBudget($mode = 'default', $field_id, 6.8);
    }

    /**
     * Displays info on soil samples for this paticular field.
     * 
     */
    function displayFieldSoilInfo($field_id) {
        return((new SoilSample)->displaySoilSamples('field_id', $field_id));
    }

    /**
     * check if the datum of interest is the current border datum.
     * 
     */
    function checkDatumIsCurrentBorder($field_id, $datum_id) {
        $sql = "
		SELECT border_datum_id from cropping.fieldcurr WHERE id=$field_id LIMIT 1
		";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row['border_datum_id'] != $datum_id) {
            $sql2 = "	SELECT gis.ST_Area(gis.ST_Transform(geom, 2036))/10000 as area FROM cropping.datum WHERE id=$datum_id LIMIT 1";
            $res2 = $GLOBALS['pdo']->query($sql2);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $area = round(($row2['area']), 2);
            return " <b id='boldGreen'>Partial ($area ha).</b> ";
        } else {
            return null;
        }
    }

    private function displaySoilEventsSummary() {
        print("<h3>Soil Fertility  Events</h3>");
        $outArray = array();
        $outArray = $this->processFertilizerEvents($outArray);
        $outArray = $this->processLimeEvents($outArray);
        $outArray = $this->processManureEvents($outArray);
        $outArray = $this->processSoilSampleEvents($outArray);
        return (QueryPageHelper::sortOutArray($outArray)); // print out sorted array.
    }

    private function displayEventsSummary() {

        $outArray = array();
        $outArray = $this->processSeedingScheduled($outArray);
        $outArray = $this->processBagYieldEvents($outArray);
        $outArray = $this->processSprayEvents($outArray);
        $outArray = $this->processSeedEvents($outArray);
        $outArray = $this->processFertilizerEvents($outArray);
        $outArray = $this->processHarvestEvents($outArray);
        $outArray = $this->processLimeEvents($outArray);
        $outArray = $this->processManureEvents($outArray);
        $outArray = $this->processSoilSampleEvents($outArray);
        $outArray = $this->processTillageEvents($outArray);
        $outArray = $this->processGeneralCommentEvents($outArray);
        $outArray = $this->processForageAnalysisEvents($outArray);
        $outArray = $this->processYieldEvents($outArray);
        // print_r(array_keys($outArray));
        // print_r($outArray);
        print(QueryPageHelper::sortOutArray($outArray)); // print out sorted array.
    }

    //take info for a line and generate a generic string that applies to all.
    function genericFieldSummaryLine($time, $type, $user, $text,$extra=null) {
            //find initials 
            $sql = "
            SELECT wcauthentication.initials('$user')
                ";
             $res = $GLOBALS['pdo']->query($sql);
             $row = $res->fetch(PDO::FETCH_ASSOC);
             $initials=$row['initials'];
             
        $ftime = date('M j, Y', $time);
        $str = "<ul class='list'><span class='badge label-info label-custom-". str_replace(' ', '', strtolower($type))   ."'>" .$type.'</span> ' . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$text}  ".(new Bootstrap)->initials($initials)."$extra</ul>";
         return $str;
    }

    function processSeedingScheduled($outArray) {
        $sql = "
            SELECT *,field.id as field_id,seed_event_scheduled.id as seed_event_scheduled_id, seed_event_scheduled.id as delete_table_id, 'seed_event_scheduled' as delete_table_name,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id,event_time,comment
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
        WHERE   field.id={$this->field_id}
                ";
        // if we only want to show this year: 
        // seed_event_scheduled.event_time >= (select extract (year from current_date) || '-01-01')::timestamp AND

        $res = $GLOBALS['pdo']->query($sql);
        $partial = '';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row ['event_time']);
            $text="{$row['general_type']} {$row['specific_type']} - {$row['company']} {$row['name']}  ";
            $str=$this->genericFieldSummaryLine($time,'Seeding Scheduled',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processBagYieldEvents($outArray) {
        $sql = "With temp as (
SELECT bag.id as bag_id,event_time,field.id as field_id,alpha_numeric_id ,abs(footage_finish-footage_start) as footage_yield, diameter_foot, nutrition.bag_ensile_date_at_footage (bag.id, bag_field.footage_start::integer) as ensile_date, nutrition.bag_feed_name_at_footage(bag.id::integer, bag_field.footage_start::integer)::text as feed_type_name
,1 as field_ha,bag.userid
        FROM nutrition.bag
LEFT JOIN nutrition.bag_field ON bag_id=bag.id 
LEFT JOIN cropping.field on field_id=field.id
WHERE field_id={$this->field_id}
)
SELECT *,round(((footage_yield/field_ha)*pi()*(diameter_foot/2)*(diameter_foot/2)*13*0.453592*.001)::numeric,2) as dm_t_yield   FROM temp
";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            
            $time = strtotime($row ['event_time']);
            $text="Bag #{$row['bag_id']} {$row['feed_type_name']}: {$row['footage_yield']} (feet) {$row['dm_t_yield']} (DM tonnes overall)";
            $str=$this->genericFieldSummaryLine($time,'Bag Yield',$row['userid'],$text);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processSprayEvents($outArray) {
        $sql = "
           SELECT * from cropping.spray_event LEFT JOIN cropping.spray ON spray.id=spray_event.spray_id WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
         
            $time = strtotime($row ['event_time']);
            $text=" {$row['name']} @ {$row['amount']} (l/ha)";
            $str=$this->genericFieldSummaryLine($time,'Spraying',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processYieldEvents($outArray) {
        $sql = "
           SELECT event_time,yield_type.name,amount,yield_event.comment, 'system' as userid from cropping.yield_event LEFT JOIN cropping.yield_type ON yield_type.id=yield_event.yield_type_id WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            
            $time = strtotime($row ['event_time']);
            $text=" {$row['name']} @ {$row['amount']}";
            $str=$this->genericFieldSummaryLine($time,'Yield',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processSeedEvents($outArray) {
        $sql = "
           SELECT *,coalesce(amount,population) as combined_amount,seed_event.id as seed_event_id from cropping.seed_event 
           LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id 
           LEFT JOIN cropping.seed_category ON seed.seed_category_id=seed_category.id 
           LEFT JOIN cropping.datum on datum.id=seed_event.datum_id
           WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $partial = $this->checkDatumIsCurrentBorder($this->field_id, $row['datum_id']);
            $time = strtotime($row ['event_time']);
            $text=" $partial {$row['general_type']} {$row['specific_type']} - {$row['company']} {$row['name']} @ {$row['combined_amount']} ";
            $str=$this->genericFieldSummaryLine($time,'Seeding',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

  

    function processFertilizerEvents($outArray) {
        $sql = "
           SELECT * from cropping.fertilizer_event LEFT JOIN cropping.fertilizer_display ON fertilizer_display.id=fertilizer_event.fertilizer_id WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
          
            $time = strtotime($row ['event_time']);
            $text="{$row['fertilizer_display']} @ " . round($row['amount']) . " kg/ha";
            $str=$this->genericFieldSummaryLine($time,'Fertilized',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processHarvestEvents($outArray) {
        $sql = "
           SELECT * from cropping.harvest_event  WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
           
            $time = strtotime($row ['event_time']);
            $text="{$row['name']} @ {$row['amount']} ";
            $str=$this->genericFieldSummaryLine($time,'Harvested',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
            
        }
        return $outArray;
    }

    function processLimeEvents($outArray) {
        $sql = "
           SELECT * from cropping.lime_event LEFT JOIN cropping.lime ON lime.id=lime_event.lime_id WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
         
            $time = strtotime($row ['event_time']);
            $text="{$row['name']} @ {$row['lime_amount']} t/ha.";
            $str=$this->genericFieldSummaryLine($time,'Lime',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processManureEvents($outArray) {
        $sql = "
           SELECT *,round(manure_amount) as manure_amount from cropping.manure_event  WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
         
            $time = strtotime($row ['event_time']);
            $text="{$row['manure_amount']} l/ha ";
            $str=$this->genericFieldSummaryLine($time,'Manure',$row['spreader_userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processSoilSampleEvents($outArray) {
        $sql = "
           SELECT * from cropping.soil_sample_event  WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            
            $time = strtotime($row ['event_time']);
            $text=" pH:{$row['ph']} / CEC:{$row['cec']}";
            $str=$this->genericFieldSummaryLine($time,'Soil Test',$row['userid'],$text);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processTillageEvents($outArray) {
        $sql = "
           SELECT * from cropping.tillage_event LEFT JOIN cropping.tillage ON tillage.id=tillage_event.tillage_id WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            
            $time = strtotime($row ['event_time']);
            $text="{$row['name']}";
            $str=$this->genericFieldSummaryLine($time,'Tillage',$row['userid'],$text,$row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processGeneralCommentEvents($outArray) {
        $sql = "
			SELECT * from cropping.general_comment_event WHERE field_id={$this->field_id}
			";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
    
            $time = strtotime($row ['event_time']);
            $text="{$row['comment']}";
            $str=$this->genericFieldSummaryLine($time,'General Comment',$row['userid'],$text);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
        }
        return $outArray;
    }

    function processForageAnalysisEvents($outArray) {

        $sql = <<<SQL
                           with temp as (
                SELECT bag_analysis.userid,nutrition.bag_feed_name_at_footage(bag_id,footage),bag_id,footage,sample_number,raw_report->'DM' as "DM",raw_report->'pH' as "pH",raw_report->'CP' as "CP",raw_report->'SP' as "SP",raw_report->'ADFCP' as "ADFCP",
                    raw_report->'NDFCP' as "NDFCP",raw_report->'ADF' as "ADF",raw_report->'NDF' as "NDF",raw_report->'lignin' as "lignin",raw_report->'NFC' as "NFC",raw_report->'starch' as "starch",raw_report->'CF' as "CF",raw_report->'TDN' as "TDN",
                        raw_report->'DE' as "DE",raw_report->'Ca' as "Ca",raw_report->'P' as "P",raw_report->'Mg' as "Mg",raw_report->'K' as "K",raw_report->'Na' as "Na",raw_report->'ash' as "ash",raw_report->'RFQ' as "RFQ",nutrition.bag_field_id_at_footage (bag_id, footage) as field_id  
                            FROM nutrition.bag_analysis 
                            )
                           SELECT *, nutrition.bag_ensile_date_at_footage (temp.bag_id, footage) as ensile_date FROM TEMP WHERE field_id={$this->field_id};
SQL;


        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $pdfUrl = "http://{$_SERVER['HTTP_HOST']}/functions/displayForageAnalysisPdf.php?sample_number={$row['sample_number']}";
            
            $rfq = '';
            $starch = '';
            if (!empty($row['RFQ'])) {
                $rfq = 'RFQ: ' . $row['RFQ'];
            }
            if (!empty($row['starch'])) {
                $starch = 'Starch: ' . $row['starch'];
            }
            
            
            $time = strtotime($row ['ensile_date']);
            $text=" CP {$row['CP']} $rfq  $starch <button type='button' class='btn btn-default btn-xs'><a href='$pdfUrl'>PDF<a></button> ";
            $str=$this->genericFieldSummaryLine($time,'Forage Analysis',$row['userid'],$text,empty($row['comment']) ? '' : $row['comment']);
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
            
            
        }
        return $outArray;
    }

}

//end class 