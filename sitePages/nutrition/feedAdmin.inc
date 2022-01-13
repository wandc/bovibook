<?php

class FeedAdmin extends TabPage {

    public function defaultDisplay() {

       
        //setup for tabs.
        $tabArray[1]['name'] = 'Current Feeds';
        $tabArray[2]['name'] = 'Add a New Type of Feed';

        print("<div class='row'>");

        print("<div class='col-sm-4 col-sm-push-8'>");
        $this->showFeedAccordion();
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");

        $this->renderTabs($tabArray);

        print("</div>");
        print("</div>");
        
        
        
    }

    function tab1() {

        $this->displayCurrentlyAvailableFeeds();
        
           print("<h3>Currently Empty Feed Slots</h3>");
        $this->displayListOfEmptyFeedSlots();
    }

    function tab2() {

        $this->displayListOfFeeds();
    }

    function showFeedAccordion() {

        $accordionArray[1]['name'] = 'Make Feed Current';
        $accordionArray[2]['name'] = 'Add New Feed';
        $accordionArray[3]['name'] = 'Enter Feed Cost';
        $accordionArray[4]['name'] = 'Feed Dry Matter %';


        $accordion = new AccordionImplementationFeedAdminQF();
        $accordion->setCSS('accordionNarrow');
        print($accordion->render($accordionArray));
    }

    
    
    /*
     * One off code. 
     * first repair seems to cause sums to be off, this fixes it., But I think I fixed bug in manualRepairNRCArchiveForNullCost
     * so probably nver have ot run this again. 
     */
   function manualRepair2FixRecipeSums() {
        
           $sql = <<<SQL

WITH temp AS (
         SELECT DISTINCT nrc_archive.nrc_recipe_id::int,recipe,
            nrc_archive.nrc_recipe_id AS nrc_recipe_id2,

           (jsonb_array_elements(nrc_archive.recipe) ->> 'kg_day_cost')::numeric AS kg_day_cost,
           (jsonb_array_elements(nrc_archive.recipe) -> 'location_id_arr'::text) AS location_id_arr,
           (jsonb_array_elements(nrc_archive.recipe) -> 'location_count_arr'::text) AS location_count_arr,
           (jsonb_array_elements(nrc_archive.recipe) -> 'location_mod_count_arr'::text) AS location_mod_count_arr,
            jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_dry_sum'::text AS kg_day_dry_sum,
            jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_cost_sum'::text AS kg_day_cost_sum
           FROM nutrition.nrc_archive
             LEFT JOIN nutrition.nrc_recipe_param ON nrc_recipe_param.nrc_recipe = nrc_archive.nrc_recipe_id
        group by nrc_recipe_id
        ), temp2 as (
 SELECT distinct nrc_recipe_id,recipe,kg_day_cost_sum, sum(kg_day_cost::numeric) over  (PARTITION BY nrc_recipe_id) as calculated_sum  from temp
)
SELECT * from temp2 where round(calculated_sum::numeric,4) != round(kg_day_cost_sum::numeric,4)
SQL;


        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            
             print('nrc_recipe_id: ' . $row['nrc_recipe_id'] . ' | ');
                 print('calculated_sum: ' . $row['calculated_sum'] . ' | ');
                      print('original: ' . $row['kg_day_cost_sum'] . ' <br> ');
              $json = $row['recipe'];
              
              
        //replace all instances of kg_day_cost_sum with new value
                $pattern = '/"kg_day_cost_sum": ([0-9\.]+)/';
                $replacement = '"kg_day_cost_sum": ' . $row['calculated_sum'];
                $json= preg_replace($pattern, $replacement, $json);

                //update
                $sqlU = <<<SQLU
UPDATE nutrition.nrc_archive SET recipe = '$json' where nrc_recipe_id = {$row['nrc_recipe_id']};
SQLU;
                $res2 = $GLOBALS['pdo']->exec($sqlU);
 
print("RES:".$res2);
                print("<br><hr>");
        
    }
    }
    
     /*
     * This function is run manuyally once (on test page) if you ever forget to 
     * put feed costs in for some certain item. Hopefully never has to be run again. 
      * FIXME, seems to cause the sums to be off for totals of recipe. 
     */
    function manualRepairNRCArchiveForNullCost() {


        $sql = <<<SQL
with temp as (
 SELECT nrc_archive.nrc_recipe_id,
    nrc_recipe_param.create_time,
nrc_archive.recipe,
    jsonb_array_elements(nrc_archive.recipe) -> 'location'::text AS location,
    jsonb_array_elements(nrc_archive.recipe) -> 'feed_type'::text AS feed_type,
    jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_wet_mix'::text AS kg_day_wet_mix,
    ((jsonb_array_elements(nrc_archive.recipe) ->> 'kg_day_dry'::text)::numeric) * ((jsonb_array_elements(nrc_archive.recipe) ->> 'modified_mix_total_count'::text)::numeric) AS kg_day_dry_mix,
    jsonb_array_elements(nrc_archive.recipe) -> 'location_id_arr'::text AS location_id_arr,
    (jsonb_array_elements(nrc_archive.recipe) ->> 'modified_mix_total_count'::text)::numeric AS modified_mix_total_count,
    jsonb_array_elements(nrc_archive.recipe) -> 'location_mod_count_arr'::text AS location_mod_count_arr,
  jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_dry_sum'::text AS kg_day_dry_sum,
     jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_cost'::text AS kg_day_cost,
  jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_cost_sum'::text AS kg_day_cost_sum,
  jsonb_array_elements(nrc_archive.recipe) -> 'kg_day_dry'::text AS kg_day_dry


   FROM nutrition.nrc_archive
     LEFT JOIN nutrition.nrc_recipe_param ON nrc_recipe_param.nrc_recipe = nrc_archive.nrc_recipe_id
  ORDER BY nrc_recipe_param.create_time, nrc_archive.nrc_recipe_id
)
select * from temp where kg_day_cost ='null' and feed_type='"High Moisture Corn"' 
SQL;


        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            // $arr=(json_decode($row['recipe']));
            // print($arr[0]->kg_day_cost);

            if ($row['kg_day_cost'] == 'null') {

                $kg_day_cost_new = $row['kg_day_dry'] * .25;
                $kg_day_cost_sum_new = $row['kg_day_cost_sum'] + $kg_day_cost_new;

                print('nrc_recipe_id: ' . $row['nrc_recipe_id'] . ' | ');

                print("kg_day_cost_new:$kg_day_cost_new | ");
                print("kg_day_cost_sum_new:$kg_day_cost_sum_new<br><br>");

                //now do search and replace on json string. 
                $json = $row['recipe'];
                $json = str_replace('"kg_day_cost": null', '"kg_day_cost": ' . $kg_day_cost_new, $json);

                //replace all instances of kg_day_cost_sum with new value
                $pattern = '/"kg_day_cost_sum": ([0-9\.]+)/';
                $replacement = '"kg_day_cost_sum": ' . $kg_day_cost_sum_new;
                $json= preg_replace($pattern, $replacement, $json);

                //update
                $sqlU = <<<SQLU
UPDATE nutrition.nrc_archive SET recipe = '$json' where nrc_recipe_id = {$row['nrc_recipe_id']};
SQLU;
                $res2 = $GLOBALS['pdo']->exec($sqlU);


                print("<br><hr>");
            }
        }   
       }
    
    
    /**     * ************************************* */
    /** Display Currently Available Feeds      */

    /**     * ************************************* */
    function displayCurrentlyAvailableFeeds($mode = 'default') {

        print("<h3>Currently Available Feeds</h3>");

        $sql = "SELECT * FROM nutrition.feedcurr";
        print( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql, ''));
    }

    /**     * ******************************* */
    /** Display list of Feeds            */

    /**     * ******************************* */
    function displayListOfFeeds($mode = 'default') {
        print("<h3>List of Feeds </h3>");


        $sql = "SELECT * FROM nutrition.feed_type";
        print( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql, ''));
    }

    /* display slots currently empty */
     function displayListOfEmptyFeedSlots($mode = 'default') {
                 $sql = <<<SQL
  SELECT feed_location.id,feed_location.location FROM 
nutrition.feed_location
 left JOIN nutrition.feedcurr ON feed_location.location = feedcurr.location 
WHERE feedcurr.id is null
order by location
    
SQL;   
    
   print( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql, ''));
    }   
    
}

//end class


class AccordionImplementationFeedAdminQF extends AccordionSub {

    public $pageid;

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct() {




        $this->pageid = $_REQUEST['pageid'];
    }

    function tab1() {
       return $this->addNewFeedLinkQuickform();
    }

    function tab2() {
       return $this->addNewFeedsQuickform();
    }

    function tab3() {
      return  $this->addNewBagFeedCostQuickForm();
    }

    function tab4() {
        return $this->addNewFeedDryMatterQuickForm();
    }

    /**     * ******************************* */
    /** Make a Feed Current              */

    /**     * ******************************* */
    function addNewFeedLinkQuickform() {


        //grab a list of feeds.
        $res = $GLOBALS['pdo']->query("SELECT id,name,comment FROM nutrition.feed_type ORDER BY name");

        $feedList[null] = "Please Select one..."; //dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $feedList[$row['id']] = $row['name'] . " - " . $row['comment'];
        }

        //grab a list of feed locations.
        $res = $GLOBALS['pdo']->query("SELECT id,location FROM nutrition.feed_location WHERE active=true ORDER BY location");

        $feedLoc[null] = "Please Select one..."; //dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $feedLoc[$row['id']] = $row['location'];
        }


        //create select list for dry matter
        $listOfPercentages = $this->listOfDryMatterPercentages();

        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.



        $form = new HTML_QuickForm("addNewFeedLinkQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Make Feed Current');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('select', 'feed', 'Feed', $feedList);
        $form->addElement('select', 'location', 'Location', $feedLoc);
        $form->addElement('select', 'feedMoisture', 'Dry Matter %', $listOfPercentages);
        $form->addElement('text', 'costPerTonneDM', '$ / DM tonne');
        $form->addElement('select', 'dateEvent', 'Date of Cost', $timeArray);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('feed', 'Feed is required', 'required');
        $form->addRule('location', 'location is required.', 'required');
        $form->addRule('feedMoisture', 'Dry Matter % is required.', 'required');
        $form->addRule('costPerTonneDM', 'Cost per Tonne is required.', 'required');
        $form->addRule('dateEvent', 'Date is required.', 'required');
        $form->addRule('costPerTonneDM', 'Cost per Tonne must be a number.', 'numeric');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $feed_type_id = $form->exportValue('feed');
            $feed_location_id = $form->exportValue('location');
            $percentDryMatter = $form->exportValue('feedMoisture');
            $costPerTonneDM = pg_escape_string(trim($form->exportValue('costPerTonneDM')));
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d 00:i:sO', $tempArray[0]); //use midnight as time, so we can use it right away.
            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //insert into main feed table that links the name with the location at a certain time.
                $query = "INSERT INTO nutrition.feed (feed_type_id,feed_location_id,event_time,active) VALUES ($feed_type_id,$feed_location_id,'$event_time','true')";
                $res = $GLOBALS['pdo']->exec($query);
                $feed_id = $GLOBALS['pdo']->lastInsertId('nutrition.feed_id_seq1');

                //insert into moisture table for just inserted feed.
                $query = "INSERT INTO nutrition.feed_moisture (feed_id,date,dry_matter_percent) VALUES ($feed_id,'$event_time',$percentDryMatter)";
                $res = $GLOBALS['pdo']->exec($query);

                //insert into costs table for just inserted feed.
                $query = "INSERT INTO nutrition.feed_cost (feed_id,event_time,cost,userid) VALUES ($feed_id,'$event_time',$costPerTonneDM,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);

                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /**     * ******************************* */
    /** Add new feed types               */

    /**     * ******************************* */
    function addNewFeedsQuickform() {

        $form = new HTML_QuickForm("addNewFeedsQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Add a new feed');
        $form->addElement('text', 'feedName', 'Name of Feed');
        $form->addElement('textarea', 'feedComment', 'Description / Comment');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'colourCode', 'ffffff');

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('feedName', 'Feed Name is required', 'required');
        $form->addRule('colourCode', 'Colour Code is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $feedName = pg_escape_string(trim($form->exportValue('feedName')));
            $feedComment = pg_escape_string(trim($form->exportValue('feedComment')));
            $colourCode = $form->exportValue('colourCode');

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.feed_type (name,comment,colour_code) VALUES ('$feedName','$feedComment','$colourCode')";
            $res = $GLOBALS['pdo']->exec($query);

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /*     * ******************************* */
    /* New Bag Feed Cost                    */
    /*     * ******************************* */

    function addNewBagFeedCostQuickForm() {

        //hier select list.
        $res = $GLOBALS['pdo']->query("SELECT xxx.id as bag_id,yyy.id as bag_feed_id,location,slot,footage_start,footage_finish,feed_type.name as feed_type_name, (SELECT cost FROM nutrition.bag_cost WHERE bag_id=xxx.id AND footage_start=yyy.footage_start AND footage_finish=yyy.footage_finish LIMIT 1) as cost 
FROM nutrition.bag_current as xxx
LEFT JOIN nutrition.bag_feed yyy ON yyy.bag_id = xxx.id 
LEFT JOIN nutrition.feed_type ON yyy.feed_id = feed_type.id 
ORDER BY slot,xxx.id ");

        $feedsInBags[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if ($row['cost'] == null) {
                
              $feedsInBags[$row['bag_feed_id']] =  $row['location'] .'- slot '.   $row['slot']  .' '.  $row['feed_type_name'] . ': ' . $row['footage_start'] . '\' to ' . $row['footage_finish'] . '\'';  
            }
            }
        
        
        

        $form = new HTML_QuickForm("addNewBagFeedCostQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Enter Bag Feed Cost');
        $form->addElement('select', 'whichBagAndFeed', 'Which Bag and Feed',$feedsInBags);
        

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('text', 'cost', 'Cost $ (per DRY tonne)');

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        //rules
        $form->addRule('whichBagAndFeed', 'Feed Type is required', 'required');
        $form->addRule('cost', 'Cost is required', 'required');
        $form->addRule('cost', 'Cost must be a number', 'numeric');


        // Try to validate a form
        if ($form->validate()) {


            //get values
            $pageid = $form->exportValue('pageid');
            $whichBagAndFeed = $form->exportValue('whichBagAndFeed');
            $cost = trim($form->exportValue('cost'));

            //now look up footages to enter.
            $res = $GLOBALS['pdo']->query("SELECT bag_id,footage_start,footage_finish
FROM nutrition.bag_feed WHERE bag_feed.id= $whichBagAndFeed LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $footage_start = $row['footage_start'];
            $footage_finish = $row['footage_finish'];


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_cost (bag_id,footage_start,footage_finish,cost,userid) VALUES ({$row['bag_id']},$footage_start,$footage_finish,$cost,'{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation


        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /*     * ******************************* */
    /* New Feed Cost                    */
    /*     * ******************************* */

    function addNewFeedCostQuickForm() {

        //grab a list of the current feeds.
        $res = $GLOBALS['pdo']->query("SELECT feed_type,location,table_id as id FROM nutrition.feedcurr WHERE type='feed' Order By feed_type");

        $feedType[null] = "Please Select one..."; //dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $feedType[$row['id']] = $row['feed_type'] . " - " . $row['location'];
        }


        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.



        $form = new HTML_QuickForm("addNewFeedCostQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Enter Feed Cost');
        $form->addElement('select', 'feedType', 'Current Feed', $feedType);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('select', 'dateEvent', 'Date of Cost', $timeArray);
        $form->addElement('text', 'cost', 'Cost $ (per DRY tonne)');


        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        //rules
        $form->addRule('feedType', 'Feed Type is required', 'required');
        $form->addRule('dateEvent', 'Event Time is required', 'required');
        $form->addRule('cost', 'Cost is required', 'required');
        $form->addRule('cost', 'Cost must be a number', 'numeric');
        print("	<li><a title=\"x_2\" href=\"#fragmentFeedAdmin-3\"><span>Feed Cost</span></a></li>");


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $feedTypeID = $form->exportValue('feedType');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');
            $cost = trim($form->exportValue('cost'));



            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.feed_cost (feed_id,event_time,cost,userid) VALUES ($feedTypeID,'$event_time',$cost,'{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /*     * ******************************* */
    /* New Feed Dry Matter Percent      */
    /*     * ******************************* */

    function addNewFeedDryMatterQuickForm() {

        //grab a list of the current feeds.
        $res = $GLOBALS['pdo']->query("SELECT feed_type,location,table_id as id FROM nutrition.feedcurr WHERE type='feed' Order By feed_type");

        $feedType[null] = "Please Select one..."; //dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $feedType[$row['id']] = $row['feed_type'] . " - " . $row['location'];
        }


        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.
        //create select list for % DM
        $listOfPercentages = $this->listOfDryMatterPercentages();


        $form = new HTML_QuickForm("addNewFeedMoistureQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Enter Feed Dry Matter %');
        $form->addElement('select', 'feedType', 'Current Feed', $feedType);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('select', 'dateEvent', 'Date of Cost', $timeArray);
        $form->addElement('select', 'dryMatter', 'Dry Matter %', $listOfPercentages);


        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        //rules
        $form->addRule('feedType', 'Feed Type is required', 'required');
        $form->addRule('dateEvent', 'Event Time is required', 'required');
        $form->addRule('dryMatter', 'Cost is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $feed_id = $form->exportValue('feedType');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');
            $percentDryMatter = $form->exportValue('dryMatter');



            //insert into moisture table for just inserted feed.
            $query = "INSERT INTO nutrition.feed_moisture (feed_id,date,dry_matter_percent) VALUES ($feed_id,'$event_time',$percentDryMatter)";
            $res = $GLOBALS['pdo']->exec($query);



            //$_SESSION['selectedTab'] = 2;
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    private function listOfDryMatterPercentages() {
        //create select list for dry matter %
        $listOfPercentages[null] = "Please Select one..."; //dummy value
        for ($i = 5.0; $i <= 100.0; $i = $i + 0.1) {
            $listOfPercentages["$i"] = "$i %";
        }
        return $listOfPercentages;
    }

}

//end class
?>