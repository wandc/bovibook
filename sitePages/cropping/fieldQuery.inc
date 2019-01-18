<?php 


require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/croppingHelper.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
require_once($_SERVER['DOCUMENT_ROOT'] . '/global.php');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/cropping/soilSample.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/cropping/field.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . 'functions/googleCharts.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . 'functions/queryPageHelper.inc');


class FieldQuery extends TabPage {

    public $field_id;

    public function __construct() {
        parent::__construct();
  if ((isset($_REQUEST['field_id']) == false) || ($_REQUEST['field_id'] == null)) {
      $_REQUEST['field_id'] =-1;  //dummy value
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
        require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/trimbleGPS.php');
        echo '{"aaData":' . (TrimbleGPS::gpsEvents($_REQUEST['item'])) . '}';
        return null;
    }

    public function defaultDisplay() {

        //check if class vars are filled
        //if they are filled, show field page.
        if ($this->field_id != null) {
            //$this->fieldSelectQuickForm();
            /** put everything here */
            $this->displayMain();
        }
        //when they are not set, show field select list.
        else {
            print("<h2>Choose Field Above.</h2>");
        }
    }

    private function displayMain() {

        print ('<div id="fieldQueryHeader">');
        print ('<div id="fieldQueryTitle">');
        print(CroppingHelper::displayFieldInfo($this->field_id));
        print('<div id="fieldQueryHeaderParamaters">');

        print($this->displayFieldParamaters($this->field_id));
        print($this->standAge($this->field_id));

        print ('</div>');
        print('<div id="fieldQueryCloseFields">');
        print($this->findClosestFields($this->field_id));
        print ('</div>');
        print ('</div>');

        print ('<div id="fieldQueryTitleImage">');
        print($this->displayCurrentFieldMap($this->field_id));
        print ('</div>');

        print ('</div>');

        //setup for tabs.
        $tabArray[1]['name'] = 'Summary';
        $tabArray[1]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[2]['name'] = 'Soil, Lime, Fertilizer';
        $tabArray[2]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[3]['name'] = 'Border Datum';
        $tabArray[3]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[4]['name'] = 'Commands';
        $tabArray[4]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[5]['name'] = 'Datum Intersections';
        $tabArray[5]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[6]['name'] = 'GPS Data';
        $tabArray[6]['extra_callback_param'] = "&field_id={$this->field_id}";
        $tabArray[7]['name'] = 'Planner';
        $tabArray[7]['extra_callback_param'] = "&field_id={$this->field_id}";
        $this->renderTabs($tabArray);
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

    protected function tab2() {

        $ss=new SoilSample();
        
        $obj=$ss->getLatestSoilSample($this->field_id);
        print('<hr/>');
        if ($obj != null) {
        $ss->displaySingleSoilSampleFancy($obj);
          print('<hr/>');
        }
        
        $this->displayFieldSoilInfo($this->field_id);
  print('<hr/>');
        $this->displayFieldLimeRequirements($this->field_id);
         print('<hr/>');
          if ($obj != null) {
          $ss->FertilizerRecommendationPerCrop($obj,'soybeans');
         }
        $this->soybeanNutrientRequirements($this->field_id);
          print('<hr/>');
        $this->displaySoilEventsSummary();
    }

    protected function tab3() {


        $this->displayFieldHistoricalDatum($this->field_id);
    }

    protected function tab4() {
        print('<div id="threeColumnLayout">');
        $x = new Field();
        $x->addNewSeedingToAFieldQuickform('potential', $this->field_id);
        print('<hr/>');
        $x->addNewGeneralCommentQuickform('default', $this->field_id);
        print('<hr/>');
        $x->addNewSeedingToAFieldQuickform('default', $this->field_id);
        print('<hr/>');
        $x->addNewLimeEventQuickform('default', $this->field_id);
        print('<hr/>');
        require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/cropping/manure.inc');
        $manure=new Manure();
        $manure->addNewManureLoadsEventQuickform('default', $this->field_id);
        print('<hr/>');
        $x->addNewFertilizerEventQuickform('default', $this->field_id);
        print('<hr/>');
        $x->addNewFieldParamQuickform('default', $this->field_id);
        print('<hr/>');
        $x->addNewFieldBorderQuickform('default', $this->field_id);
        print('<hr/>');
        $x->addNewYieldEventQuickform('default', $this->field_id);
        print('</div>');
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
        require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/trimbleGPS.php');
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
        $row = Field::displayFieldsForCurrentSeasonPlanting($year, $this->field_id);
        $specific_type = $row['specific_type'];
        require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/croppingNutrientRequirements.inc');
        $req = new CroppingNutrientRequirements($this->field_id, $year);
        $req->main($specific_type);
    }

    /**
     * lists the four closest fields to the given one.
     * @param type $field_id 
     */
    function findClosestFields($field_id) {
        $str = '';
        $sql = "SELECT 
id,alpha_numeric_id	,common_name, st_distance(border_geom,(SELECT border_geom FROM cropping.fieldcurr WHERE Id =$field_id)) as min_distance 
FROM cropping.fieldcurr 
WHERE Id !=$field_id ORDER BY min_distance LIMIT 4";

        $str = $str . ("<h6>Nearby Fields:</h6>");
        $res = $GLOBALS['pdo']->query($sql);
          $str = $str .'<ul>';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $str = $str . ("<li><small><a href='/index.php?pageid=107&field_id={$row['id']}'>{$row['alpha_numeric_id']} - {$row['common_name']} </a></small></li>");
        }
      $str = $str .'</ul>';
        return $str;
    }

    function soybeanNutrientRequirements($field_id) {

        $soilTerstArr = SoilSample::latestFieldSoilTest($field_id);
        print("<br/>");
        print("<h4>Soybean Requirments</h4>");
        print("Based on {$soilTerstArr['report_date']} soil test.<br/>");
        print("Nitrogen: 0 kg/ha</br>"); //always zero for soybeans.
        $Phosphorus = $this->soybeanPhosphorusRequirements($soilTerstArr['phosphorus']);
        print("Phosphorus: $Phosphorus kg/ha<br>");
        $Potassium = $this->soybeanPotassiumRequirements($soilTerstArr['potassium']);
        print("Potassium: $Potassium kg/ha<br>");
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
     * Find what year of an alfalfa stand or how many years corn on corn, etc.
     */
    function standAge($field_id) {
        $sql = "
   SELECT * FROM cropping.rotationcurr WHERE id=$field_id LIMIT 1
   ";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $age = date('Y', strtotime("now")) - date('Y', strtotime($row['event_time']));
        return ("<li>Rotation: {$row['specific_type']} " . $age . " years.</li>");
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

            $outArr = CroppingHelper::convertLineStringToLatLngArray($row['linestring']);
            $kml = CroppingHelper::ConvertArrayToLatLongHeightStr($outArr);
            print("KML: $kml");

            print('<hr/>');
        }
    }

    public function displayCurrentFieldMap($field_id,$zoom=16) {
        //lookup the current border of the field.
        $sql = "
 	SELECT gis.ST_AsText(gis.ST_Boundary(border_geom)) as linestring FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1
 	";
        $res = $GLOBALS['pdo']->query($sql);

        if ($res->rowCount() > 0) {
            $row = $res->fetch(PDO::FETCH_ASSOC);
            return self::fieldMap($row['linestring'],'0xff0000ff','0xFF33CC33',$zoom);
        } else {
            throw new Exception("Field does not have a default border defined.");
        }
    }

    /**
     * shows a map of any polygon requested.
     */
    private function fieldMap($linestring, $borderColour = '0xff0000ff',  $fillColour = '0xFF33CC33', $zoom=17) {

        //convert to format for google maps
        $latlon = GoogleChartsHelper::postgresLineStringToLatLongArray($linestring);
        $str = GoogleChartsHelper::latLongArrayToGoogleMapsEncodedPolyline($latlon);

        return ("<img id='fieldHeaderMap' src='https://maps.googleapis.com/maps/api/staticmap?key={$GLOBALS['config']['GOOGLE_MAPS']['API_KEY']}&path=color:$borderColour%7Cfillcolor:$fillColour%7cweight:2%7C$str&size=520x300&sensor=false&maptype=hybrid&zoom=$zoom'/>");
    }

    function displayFieldParamaters($field_id) {
        $str = "";
        $sql = "
 	SELECT * from cropping.field_parameter WHERE field_id=$field_id ORDER BY event_time DESC LIMIT 1
 	";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() > 0) {
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $str = $str . "<li>Alfalfa suitable: {$row['alfalfa_suitable']}</li>";
            $str = $str . "<li>Corn suitable: {$row['corn_suitable']}</li>";
        }

        return $str;
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

            $str = $str . "<b>{$row['event_name']}</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b>  {$row['id']}<br/>";
        }
        return $str;
    }

    /**
     * Displays info on lime requirements for the fields 
     * 
     */
    function displayFieldLimeRequirements($field_id) {
        SoilSample::printLimeBudget($mode = 'default', $field_id, 6.8);
    }

    /**
     * Displays info on soil samples for this paticular field.
     * 
     */
    function displayFieldSoilInfo($field_id) {
        SoilSample::displaySoilSamples('field_id', $field_id);
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
        print(QueryPageHelper::sortOutArray($outArray)); // print out sorted array.
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
        $partial='';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row ['event_time']);
            $ftime = date('M j, Y', $time);
            $str = "<a class='tooltip' href='#'>Seeding Scheduled<span class='custom info'><img src='/images/tooltip/info.png' alt='Error' height='48' width='48' /><em>Info</em>Seed Event Id: {$row['seed_event_scheduled_id']}." /* .$this->fieldMap( $row['polygon']) */ . "</span></a>" . ": " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> $partial {$row['general_type']} {$row['specific_type']} - {$row['company']} {$row['name']}  by  {$row['userid']} comment: {$row['comment']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processBagYieldEvents($outArray) {
        $sql = "With temp as (
SELECT bag.id as bag_id,event_time,field.id as field_id,alpha_numeric_id ,abs(footage_finish-footage_start) as footage_yield, diameter_foot, nutrition.bag_ensile_date_at_footage (bag.id, bag_field.footage_start::integer) as ensile_date, nutrition.bag_feed_name_at_footage(bag.id::integer, bag_field.footage_start::integer)::text as feed_type_name
,1 as field_ha
        FROM nutrition.bag
LEFT JOIN nutrition.bag_field ON bag_id=bag.id 
LEFT JOIN cropping.field on field_id=field.id
WHERE field_id={$this->field_id}
)
SELECT *,round(((footage_yield/field_ha)*pi()*(diameter_foot/2)*(diameter_foot/2)*13*0.453592*.001)::numeric,2) as dm_t_yield   FROM temp
";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row ['ensile_date']);
            $ftime = date('M j, Y', $time);
            $str = "<b>Bag Yield</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> Bag #{$row['bag_id']} {$row['feed_type_name']}: {$row['footage_yield']} (feet) {$row['dm_t_yield']} (DM tonnes overall)<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Sprayed</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['name']} @ {$row['amount']} (l/ha) by  {$row['sprayer_userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processYieldEvents($outArray) {
        $sql = "
           SELECT event_time,yield_type.name,amount,yield_event.comment from cropping.yield_event LEFT JOIN cropping.yield_type ON yield_type.id=yield_event.yield_type_id WHERE field_id={$this->field_id}
          ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row ['event_time']);
            $ftime = date('M j, Y', $time);
            $str = "<b>Yield</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['name']} @ {$row['amount']}, {$row['comment']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Seed Event:</b> " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> $partial {$row['general_type']} {$row['specific_type']} - {$row['company']} {$row['name']} @ {$row['combined_amount']}  by  {$row['userid']} comment: {$row['comment']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processPotentialSeedEvents($outArray) {
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Seed Event Scheduled:</b> " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> $partial {$row['general_type']} {$row['specific_type']} - {$row['company']} {$row['name']} @ {$row['combined_amount']}  by  {$row['userid']} comment: {$row['comment']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $fert = $row['fertilizer_display'];
            $str = "<b>Fertilized</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> $fert @ ". round($row['amount']) ." kg/ha , {$row['comment']} by  {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Harvest</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['name']} @ {$row['amount']}  by  {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Lime</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['name']} @ {$row['lime_amount']} t/ha  by  {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Manure</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> @ {$row['manure_amount']} l/ha by  {$row['spreader_userid']} {$row['comment']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Soil Test</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> pH:{$row['ph']} / CEC:{$row['cec']}  by  {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>Tillage</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['name']} : {$row['comment']}  by  {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
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
            $ftime = date('M j, Y', $time);
            $str = "<b>General Comment</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['comment']}  by  {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    function processForageAnalysisEvents($outArray) {
     
                   $sql = <<<SQL
                           with temp as (
                SELECT nutrition.bag_feed_name_at_footage(bag_id,footage),bag_id,footage,sample_number,raw_report->'DM' as "DM",raw_report->'pH' as "pH",raw_report->'CP' as "CP",raw_report->'SP' as "SP",raw_report->'ADFCP' as "ADFCP",
                    raw_report->'NDFCP' as "NDFCP",raw_report->'ADF' as "ADF",raw_report->'NDF' as "NDF",raw_report->'lignin' as "lignin",raw_report->'NFC' as "NFC",raw_report->'starch' as "starch",raw_report->'CF' as "CF",raw_report->'TDN' as "TDN",
                        raw_report->'DE' as "DE",raw_report->'Ca' as "Ca",raw_report->'P' as "P",raw_report->'Mg' as "Mg",raw_report->'K' as "K",raw_report->'Na' as "Na",raw_report->'ash' as "ash",raw_report->'RFQ' as "RFQ",nutrition.bag_field_id_at_footage (bag_id, footage) as field_id  
                            FROM nutrition.bag_analysis 
                            )
                           SELECT *, nutrition.bag_ensile_date_at_footage (temp.bag_id, footage) as ensile_date FROM TEMP WHERE field_id={$this->field_id};
SQL;
   

        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $pdfUrl = "http://{$_SERVER['HTTP_HOST']}/functions/displayForageAnalysisPdf.php?sample_number={$row['sample_number']}";
            $time = strtotime($row ['ensile_date']);
            $ftime = date('M j, Y', $time);
            $rfq=''; 
            $starch='';
            if (!empty($row['RFQ'])) {
                $rfq='RFQ: '.$row['RFQ'];
            }
            if (!empty($row['starch'])) {
                $starch='Starch: '.$row['starch'];
            }
            $str = "<b>Forage Analysis</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> CP {$row['CP']} $rfq  $starch  {$row['comment']}  <a href='$pdfUrl'>test<a><br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

}//end class 