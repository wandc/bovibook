<?php 



require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');


class FeedAdmin extends TabPage {

    public function defaultDisplay() {

        //setup for tabs.
        $tabArray[1]['name'] = 'Current Feeds';
        $tabArray[2]['name'] = 'Add a New Type of Feed';
        $tabArray[3]['name'] = 'Feed Cost';
        $tabArray[4]['name'] = 'Feed Dry Matter';
        $this->renderTabs($tabArray);
    }

    function tab1() {
         $this->addNewFeedLinkQuickform();
        $this->displayCurrentlyAvailableFeeds();
       
    }

    function tab2() {
        $this->addNewFeedsQuickform();
        $this->displayListOfFeeds();
    }

    function tab3() {
        $this->addNewBagFeedCostQuickForm();
        $this->addNewFeedCostQuickForm();
    }

    function tab4() {
        $this->addNewFeedDryMatterQuickForm();
    }

    /**     * ************************************* */
    /** Display Currently Available Feeds      */

    /**     * ************************************* */
    function displayCurrentlyAvailableFeeds($mode = 'default') {

         print("<h3>Currently Available Feeds</h3>");

        $sql = "SELECT * FROM nutrition.feedcurr";
         print( (new JQueryDataTable)->startBasicSql($sql,''));
       
  
       
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

        $timeArray = Misc::createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.



        $form = new HTML_QuickForm("addNewFeedLinkQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

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
            }
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    /**     * ******************************* */
    /** Display list of Feeds            */

    /**     * ******************************* */
    function displayListOfFeeds($mode = 'default') {
       print("<h3>List of Feeds </h3>");
       

        $sql = "SELECT * FROM nutrition.feed_type";
        print( (new JQueryDataTable)->startBasicSql($sql,''));
        
        
    }

    /**     * ******************************* */
    /** Add new feed types               */

    /**     * ******************************* */
    function addNewFeedsQuickform() {

        $form = new HTML_QuickForm("addNewFeedsQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

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

            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
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

        $aniNumber[null] = "Please Select one..."; //dummy value
        $currentBags=null;
        $feedsInBags=null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if ($row['cost'] == null) {
                //parse for last word of name and then make an array for the select list.
                $currentBags[$row['bag_id']] = $row['location'] . " - slot:" . $row['slot'];
                $feedsInBags[$row['bag_id']][$row['bag_feed_id']] = $row['feed_type_name'] . ': ' . $row['footage_start'] . '\' to ' . $row['footage_finish'] . '\'';
            }
        }


        $form = new HTML_QuickForm("addNewBagFeedCostQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Enter Bag Feed Cost');

        //Hierarchical select list
        $opts[] = $currentBags;
        $opts[] = $feedsInBags;
        $hs = $form->addElement('hierselect', 'whichBagAndFeed', 'Which Bag and Feed', array('style' => 'width: 20em;'), '<br />');
        $hs->setOptions($opts);

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
            $bag_id = $whichBagAndFeed[0];
            $bag_feed_id = $whichBagAndFeed[1];
            $cost = trim($form->exportValue('cost'));

            //now look up footages to enter.
            $res = $GLOBALS['pdo']->query("SELECT footage_start,footage_finish
FROM nutrition.bag_feed WHERE bag_feed.id= $bag_feed_id LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $footage_start = $row['footage_start'];
            $footage_finish = $row['footage_finish'];


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_cost (bag_id,footage_start,footage_finish,cost,userid) VALUES ($bag_id,$footage_start,$footage_finish,$cost,'{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
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


        $timeArray = Misc::createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.



        $form = new HTML_QuickForm("addNewFeedCostQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

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
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
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


        $timeArray = Misc::createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.
        //create select list for % DM
        $listOfPercentages = $this->listOfDryMatterPercentages();


        $form = new HTML_QuickForm("addNewFeedMoistureQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

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
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
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