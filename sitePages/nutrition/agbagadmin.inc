<?php

class AgBagAdmin extends TabPage {

    public function defaultDisplay() {
        //print_r(get_declared_classes());
        print('PAGE:' . $this->pageid . '<br>');

        //setup for tabs.
        $tabArray[1]['name'] = 'Main';
        $tabArray[2]['name'] = 'Extra';

        
        print("<div class='row'>");

        print("<div class='col-sm-4 col-sm-push-8'>");
           $this->showSilageBagAccordion();
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
        
        $this->renderTabs($tabArray);
     
           print("</div>");
        print("</div>");
    }

    function tab1() {

        $this->showErrors();
        
        //draw bag pictures
        $DrawSVGBags = new DrawSVGBags(date('Y-m-d', strtotime("now") + 86400)); //use one day in the future to get around times of midnight vs some point in the day.
        print($DrawSVGBags->main());
       
    }

    function tab2() {
       print($this->displayBagEdits());
    }

    function showSilageBagAccordion() {

        $accordionArray[1]['name'] = 'Add Ag Bag';
        $accordionArray[2]['name'] = 'Add Feed Type';
        $accordionArray[3]['name'] = 'Open/Consume Bag';
        $accordionArray[4]['name'] = 'Add Field Info';
        $accordionArray[5]['name'] = 'Add Ensile Date';
        $accordionArray[6]['name'] = 'Add Comment';
       

        $accordion = new AccordionImplementationSilageBagQF();
        $accordion->setCSS('accordionNarrow');
       print( $accordion->render($accordionArray));
    }

    
    function showErrors() {
        
        $this->checkBagHasAssociatedField();
    }
    
    /*
     * checks that all recipe items have a feed cost entered.
     */
    private function checkBagHasAssociatedField() {
        $str='';
         $sql = "SELECT bag.* from nutrition.bag left join nutrition.bag_feed ON bag_id=bag.id where feed_id is null";
        $res = $GLOBALS['pdo']->query($sql);
         while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
             //shows for individual recipe on recipe page and for all receipes on staging page.
             if ( (empty($recipe_id)) OR ($recipe_id==$row['nrc_recipe'])) {
             //  $str = $str . (new BootStrap)->errorNotify("Error: Missing Cost for Feed Bag id{$row['id ']} {$row['location']} - {$row['slot']}, is cost per tonne not entered?");
             }
         }
         return $str;
    }
    
   public function listOfFootages() {
        //generate list of footages -10 to 320
        $listOfFootages[null] = "Please Select one..."; //dummy value
        for ($i = -20; $i <= 320; $i++) {
            $listOfFootages[$i] = "$i";
        }
        return $listOfFootages;
    }

   
   

    public function listOfBags() {
//grab a list of bags.
        $res = $GLOBALS['pdo']->query("SELECT id,location,slot,event_time,comment FROM nutrition.bag ORDER BY id DESC");

        $listOfBags[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $listOfBags[$row['id']] = $row['slot'] . ' - ' . $row['location'] . ' - ' . $row['event_time'];
        }
        return $listOfBags;
    }

   
     public function displayBagEdits()
    {
        
                $sql = <<<SQL
SELECT bag_field.*,fieldcurr.alpha_numeric_id,fieldcurr.common_name, bag_field.id as bag_field_id   
FROM nutrition.bag_field
LEFT JOIN cropping.fieldcurr ON bag_field.field_id=fieldcurr.id
WHERE bag_field.create_time >= (current_date - (interval '80 week'))
ORDER BY bag_field.create_time DESC limit 100
SQL;        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            $rowOut['bag_field_id'] = ($row['bag_field_id']);
            $rowOut['Time'] = JQueryDataTable::formattedDate($row['create_time']);
            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Bag_id'] = ($row['bag_id']);
           
            $rowOut['footage_start'] = ($row['footage_start']);
            $rowOut['footage_finish'] = ($row['footage_finish']);
            
               if ((strtotime($row['create_time']) >= strtotime('-1 week')) OR ($GLOBALS['auth']->getAdminMode() == 1)) {
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
                  $deleteButtonAsString =$this->printActionButtons($row,$callBackURL);
            } else {
                $deleteButtonAsString = '';
            }
            $rowOut['Action'] = $deleteButtonAsString; 
             
           
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Bag Edits</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr)); 
         return implode($out); 
             
        
    }

    function printActionButtons($record, $callBackURL) {


        $currentUserid = $GLOBALS['auth']->getUsername();
       
        //admin user
        if ($GLOBALS['auth']->getAdminMode() == 1) {
            $sql = "SELECT id FROM nutrition.bag_field";
        } else { //normnal user
            $sql = "SELECT id FROM nutrition.bag_field WHERE userid='$currentUserid' ORDER BY create_time DESC limit 1";
        }

        $res = $GLOBALS['pdo']->query($sql);

        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (($record['bag_field_id'] == $row['id'])) {
                $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'nutrition.bag_field'));
                $returnButtonsAsString = ($a->renderDelete('badfield_edit' . $row['id']));
            }
        }

        //print button
        return $returnButtonsAsString;
    }
    
    
    function addBagConsumptionQuickform() {

        //daily dates rray, with unix timestamp as index.
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();


        //grab a list of bags.


        $sql = "SELECT bag_current.id,bag_current.location,bag_current.slot,bag_current.event_time,bag_current.comment,bag_current,current_feed FROM nutrition.bag_current
        LEFT JOIN  nutrition.bag_with_current_feed ON bag_with_current_feed.id=bag_current.id ORDER BY event_time ASC";
        //get open bags with footages availble in bag.
        $retArr = (new BagConsumption)->openBagsWithValidFootages(false, $sql); //use custom sql and add the first available foot to the list of feet.
        $listOfBags = $retArr[0];
        $listOfFootages = $retArr[1];



        $form = new HTML_QuickForm("addBagConsumptionQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Add Consumption Note About Ag Bag');

//Hierarchical select list
        $opts[] = $listOfBags;
        $opts[] = $listOfFootages;
        $hs = $form->addElement('hierselect', 'whichBag', 'Bag & Footage', array('style' => 'width: 20em;'), '<br />');
        $hs->setOptions($opts);

        $form->addElement('select', 'dateEvent', 'Date of Check', $dateArray);

        $form->addElement('hidden', 'pageid', $this->pageid);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('whichBag' => 0)); //set default to no variety.
        //$form-> setDefaults(array('footageStart' => 0 ));
        $form->setDefaults(array('dateEvent' => 0));

        //rules
        $form->addRule('whichBag', 'Bag is required', 'required');
        //$form->addRule('footage', 'Footage is required', 'required');
        $form->addRule('dateEvent', 'Date is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values

            $bagAndFootage = $form->exportValue('whichBag');
            $bag = $bagAndFootage[0];
            $footage = $bagAndFootage[1];
            $tempArray = $form->getElementValue('dateEvent');
            ;
            $event_date = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_consumption (bag_id,footage,event_time,userid) VALUES ($bag,$footage,'$event_date','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function displayBagConsumptionLog($mode = 'default') {
     

        $sql = <<<SQL
SELECT bag_id,bag_consumption.footage,bag_consumption.event_time,bag.location,bag.slot 
		FROM nutrition.bag_consumption
LEFT JOIN nutrition.bag ON bag_consumption.bag_id = bag.id 
WHERE bag_consumption.event_time >= (current_date - interval '2 year')
ORDER BY event_time DESC
SQL;


        $out[]=( (new JQueryDataTable)->startBasicSql('Consumption of Feed in Ag Bags', $sql));
          return implode($out);
    }

    function displayFeedsInBags($mode = 'default') {


        $sql = <<<SQL
SELECT bag_id,feed_id,footage_start,footage_finish,feed_type.name,bag.location,bag.slot 
FROM nutrition.bag_feed 
LEFT JOIN nutrition.bag ON bag_feed.bag_id = bag.id 
LEFT JOIN nutrition.feed_type ON bag_feed.feed_id = feed_type.id
WHERE bag_feed.update_time >= (current_date - interval '2 year')
ORDER BY bag_id,bag_feed.update_time
SQL;

        $out[]=( (new JQueryDataTable)->startBasicSql('Feeds in Ag Bags', $sql));
        return implode($out);
    }

    
    function displayBagsByLocation($mode = 'default') {
  $sql = <<<SQL
SELECT location,slot,event_time,id as bag_id,comment,userid FROM nutrition.bag ORDER BY event_time DESC,location,slot ASC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Location'] = ($row['location']);
            $rowOut['Slot #'] = ($row['slot']);
            $rowOut['Bag ID'] = ($row['bag_id']);
           
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Current Ag Bags', $dataArr, 'order:false'));
        return implode($out);
        
    }

}

//end class
/* supports both individual and group */

class AccordionImplementationSilageBagQF extends AccordionSub {

    public static $bagID;
    public  $pageid;

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct($bovineID = null) {


        if (!empty($bagID)) {
            self::$bagID = $bagID;
        } else {
            self::$bagID = null;
        }
        
        $this->pageid = $_REQUEST['pageid'];
        
      
        
    }

    function tab1() {
      // print('PAGE ID' . $this->pageID .'/'. $this->pageid);
        $a = new AgBagAdmin();
       // print('PAGE id:' . $a->pageid . '<br>');
        $out[]=$this->addBagQuickform();
        $out[]= ($a->displayBagsByLocation());
          return implode($out);
    }

    function tab2() {
        $a = new AgBagAdmin();
        $out[]=$this->addFeedQuickform();
       $out[]= $a->displayFeedsInBags();
         return implode($out);
    }

    function tab3() {
        $a = new AgBagAdmin();
        $out[]=$this->openNewBagQuickform();
       $out[]= $a->displayBagConsumptionLog();
       return implode($out);
    }

    function tab4() {
        
     
      
       $a=new QF2_addSilageBagFieldInfo((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'individual');
       return($a->renderBox('qf2_addSilageBagFieldInfo','Select Field Info for Bag'));  
      
    }

    function tab5() {
       
       return $this->addBagEnsileDateQuickform();
    }

    function tab6() {
       
       return $this->addBagCommentQuickform();
    }

   

     function addBagQuickform() {
print("pageid is:".$this->pageid);
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //dates
        $slotNumber = array(null => 'Please Select one...', 'N1' => 'N1', 'N2' => 'N2', 'N3' => 'N3', 'N4' => 'N4', 'N5' => 'N5', 'N6' => 'N6', 'N7' => 'N7', 'N8' => 'N8','N9' => 'N9 (extra)','N10' => 'N10 (extra)', 'S1' => 'S1 (half concrete)', 'S2' => 'S2', 'S3' => 'S3', 'S4' => 'S4', 'S5' => 'S5', 'S6' => 'S6 (pavement)', 'S7' => 'S7 (cement)', 'S8' => 'S8','S9' => 'S9 (extra)');

        $form = new HTML_QuickForm("addBagQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Add Ag Bag');
        $form->addElement('select', 'bagStartDate', 'Bag Start Date', $dateArray);
        $form->addElement('select', 'slot', 'Bag Slot Number', $slotNumber);
        //$form->addElement('text', 'location','Location','<b>Location</b> North Silage Pad'); 
        //$form->addElement('html', '<b>Diameter/b> 10 ft.');

        $form->addElement('hidden', 'pageid', $this->pageid);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('bagStartDate' => 0)); //set default to no variety.
        $form->setDefaults(array('slot' => 0));

        //rules
        $form->addRule('bagStartDate', 'Start date is required', 'required');
        $form->addRule('slot', 'Slot number is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $tempArray = $form->getElementValue('bagStartDate');
            $bagStartDate = date('Y-m-d 12:00:00', $tempArray[0]);
            $slot = $form->exportValue('slot');

            //split up slot to letter and number.
            $locAbr = substr($slot, 0, 1);
            $slotNum = substr($slot, 1, 1);

            //long form of location
            if ($locAbr == 'N') {
                $bagLocation = 'North Silage Pad';
            } elseif ($locAbr == 'S') {
                $bagLocation = 'South Silage Pad';
            } else {
                throw new Exception("Bag location not valid.<br>");
            }

            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag (location,slot,diameter_foot,event_time,userid) VALUES ('$bagLocation',$slotNum,10,'$bagStartDate','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    function addFeedQuickform() {

        $listOfBags = $this->listOfBags();

//grab a list of feeds.
        $res = $GLOBALS['pdo']->query("SELECT id,name FROM nutrition.feed_type ORDER BY name ASC");

        $listOfFeeds[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $listOfFeeds[$row['id']] = $row['name'];
        }


        $form = new HTML_QuickForm("addFeedQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Add Feed to Ag Bag');

        $form->addElement('select', 'bag', 'Bag', $listOfBags);
        $form->addElement('select', 'feed', 'Feed', $listOfFeeds);
        $form->addElement('select', 'footageStart', 'Footage Start', $this->listOfFootages());
        $form->addElement('select', 'footageFinish', 'Footage Finish', $this->listOfFootages());

        $form->addElement('hidden', 'pageid', $this->pageid);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('bag' => 0)); //set default to no variety.
        $form->setDefaults(array('feed' => 0));
        $form->setDefaults(array('footageStart' => 0));
        $form->setDefaults(array('footageFinish' => 0));

        //rules
        $form->addRule('bag', 'Bag is required', 'required');
        $form->addRule('feed', 'Feed is required', 'required');
        $form->addRule('footageStart', 'Footage (Start) is required', 'required');
        $form->addRule('footageFinish', 'Footage (Finish) is required', 'required');

// Try to validate a form
        if ($form->validate()) {

            //get values

            $bag = $form->exportValue('bag');
            $feed = $form->exportValue('feed');
            $footageStart = $form->exportValue('footageStart');
            $footageFinish = $form->exportValue('footageFinish');

            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_feed (bag_id,feed_id,footage_start,footage_finish,userid) VALUES ($bag,$feed,$footageStart,$footageFinish,'{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    private function listOfBags() {
//grab a list of bags.
        $res = $GLOBALS['pdo']->query("SELECT id,location,slot,event_time,comment FROM nutrition.bag ORDER BY id DESC");

        $listOfBags[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $listOfBags[$row['id']] = $row['slot'] . ' - ' . $row['location'] . ' - ' . $row['event_time'];
        }
        return $listOfBags;
    }

 function listOfFootages() {
        //generate list of footages -10 to 320
        $listOfFootages[null] = "Please Select one..."; //dummy value
        for ($i = -20; $i <= 320; $i++) {
            $listOfFootages[$i] = "$i";
        }
        return $listOfFootages;
    }
 
     

     function addBagEnsileDateQuickform() {

        $listOfBags = $this->listOfBags();

        //daily dates rray, with unix timestamp as index.
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();




        $form = new HTML_QuickForm("addBagEnsileDateQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Add Ensile Date Information for Ag Bag');

        $form->addElement('select', 'bag', 'Bag', $listOfBags);
        $form->addElement('select', 'dateEvent', 'Date of Ensiling', $dateArray);
        $form->addElement('select', 'footage', 'Footage', $this->listOfFootages());


        $form->addElement('hidden', 'pageid', $this->pageid);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $keys = array_keys($listOfBags);
        $key = $keys[1];
        $form->setDefaults(array('bag' => $key)); //set default to no variety.
        $form->setDefaults(array('footage' => 0));
        $form->setDefaults(array('dateEvent' => 0));


        //rules
        $form->addRule('bag', 'Bag is required', 'required');
        $form->addRule('footage', 'Footage is required', 'required');
        $form->addRule('dateEvent', 'Date is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            //get values

            $bag = $form->exportValue('bag');
            $tempArray = $form->getElementValue('dateEvent');
            ;
            $event_date = date('Y-m-d H:i:sO', $tempArray[0]);
            $footage = $form->exportValue('footage');
            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_ensile_date (bag_id,footage,event_time,userid) VALUES ($bag,$footage,'$event_date','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);


            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }
    
     function addBagCommentQuickform() {

        $listOfBags = $this->listOfBags();

        //daily dates rray, with unix timestamp as index.
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();



        $comment = '';
        $form = new HTML_QuickForm("addBagCommentQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Add Comment at Specific Footage on Ag Bag');

        $form->addElement('select', 'bag', 'Bag', $listOfBags);
        $form->addElement('text', 'comment', 'Comment', $comment);
        $form->addElement('select', 'footage', 'Footage', $this->listOfFootages());


        $form->addElement('hidden', 'pageid', $this->pageid);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('bag' => 0)); //set default to no variety.
        $form->setDefaults(array('footage' => 0));
        $form->setDefaults(array('comment' => 0));


        //rules
        $form->addRule('bag', 'Bag is required', 'required');
        $form->addRule('footage', 'Footage is required', 'required');
        $form->addRule('comment', 'Comment is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            //get values

            $bag = $form->exportValue('bag');
            $comment = $form->exportValue('comment');
            $footage = $form->exportValue('footage');
            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_comment (bag_id,footage,comment,userid) VALUES ($bag,$footage,'$comment','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            $this->pageid = 66;
            $_SERVER['PHP_SELF'] = "/index.php";
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    function openNewBagQuickform() {

        //foward and reverse direction choices generated form SQL.
        //sql to get all the bags that are not fully consumed, and have not been opened yet in a specific direction.
        // this doesn't really work, needs to weed out consumed bags then do forward and reverse comsumption on each
        $sql = "
        
WITH temp as (
SELECT *,'forward' as direction,(SELECT name FROM nutrition.bag_feed LEFT JOIN nutrition.feed_type ON feed_type.id=feed_id WHERE bag_id=bag.id AND footage_start=(SELECT min(footage_start) FROM nutrition.bag_feed WHERE bag_id=bag.id )) as feed_type,(SELECT min(footage_start) FROM nutrition.bag_feed WHERE bag_id=bag.id ) as footage, (SELECT min(footage_start) FROM nutrition.bag_feed WHERE bag_id=bag.id) as bag_min_footage,(SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=bag.id) as bag_max_footage,(SELECT max(footage) FROM nutrition.bag_consumption WHERE bag_id=bag.id) as max_reverse_consumption_footage
FROM nutrition.bag 
WHERE (SELECT id FROM nutrition.bag_consumption WHERE bag_consumption.bag_id=bag.id and direction='forward' LIMIT 1)  is null

UNION

SELECT *,'reverse' as direction,(SELECT name FROM nutrition.bag_feed LEFT JOIN nutrition.feed_type ON feed_type.id=feed_id WHERE bag_id=bag.id AND footage_finish=(SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=bag.id )) as feed_type,(SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=bag.id ) as footage, (SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=bag.id) as bag_min_footage,(SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=bag.id) as bag_max_footage,(SELECT max(footage) FROM nutrition.bag_consumption WHERE bag_id=bag.id) as max_reverse_consumption_footage
FROM nutrition.bag 
WHERE (SELECT id FROM nutrition.bag_consumption WHERE bag_consumption.bag_id=bag.id and direction='reverse' LIMIT 1)  is null 

ORDER BY direction, id
)

SELECT id || '|' || direction as meta_id,* FROM temp WHERE bag_min_footage	!= bag_max_footage


         ";
        $res = $GLOBALS['pdo']->query($sql);

        $bagArray[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $bagArray[$row['meta_id']] = $row['feed_type'] . ' - ' . $row['direction'] . ' - ' . $row['location'] . ' - ' . $row['slot'];
        }

        //daily dates array, with unix timestamp as index.
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();

        $form = new HTML_QuickForm("openNewBagQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'openNewBagQuickformHdr', 'Open a New Bag');
        $form->addElement('select', 'bag', 'Bag', $bagArray);
        $form->addElement('select', 'dateEvent', 'Date of Check', $dateArray);
        $form->addElement('hidden', 'pageid', $this->pageid);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('bag' => 0)); //set default to no variety.		
        $form->setDefaults(array('dateEvent' => 0));
        //rules
        $form->addRule('bag', 'Bag is required', 'required');
        $form->addRule('dateEvent', 'Date is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values

            $bag = $form->exportValue('bag');

            //break-up meta id 
            $outArr = explode('|', $bag);
            $bag_id = $outArr[0];
            $direction = $outArr[1];

            //now look-up starting footage, depending on direction.
            $sql = "SELECT (SELECT min(footage_start) FROM nutrition.bag_feed WHERE bag_id=$bag_id) as bag_min_footage,(SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=$bag_id) as bag_max_footage";
            $res = $GLOBALS['pdo']->query($sql);

            $row = $res->fetch(PDO::FETCH_ASSOC);
            if ($direction == 'forward') {
                $footage = $row['bag_min_footage'] + 1;
            } elseif ($direction == 'reverse') {
                $footage = $row['bag_max_footage'] - 1;
            } else {
                throw new Exception("Error: Invalid direction when opeing bag.<br/>\n\r");
            }


            $tempArray = $form->getElementValue('dateEvent');
            ;
            $event_date = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_consumption (bag_id,footage,event_time,userid,direction) VALUES ($bag_id,$footage,'$event_date','{$GLOBALS['auth']->getUsername()}','$direction')";

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }
    
}//end class




    Class QF2_addSilageBagFieldInfo extends QF2_Base {

        function addBagFieldInfoQuickform() {

       
       
        //defaults
        //use last entered bag as default
        $keys = array_keys($listOfBags);
        $key = $keys[1];
        $form->setDefaults(array('bag' => $key)); //set default to no variety.
        $form->setDefaults(array('footageStart' => 0));
        $form->setDefaults(array('footageFinish' => 0));
        $form->setDefaults(array('field_id' => 0));

        

        // Try to validate a form
        if ($form->validate()) {

            //get values

            $bag = $form->exportValue('bag');
            $fieldId = $form->exportValue('field_id');
            $footageStart = $form->exportValue('footageStart');
            $footageFinish = $form->exportValue('footageFinish');
            ;
            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$this->pageid}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }
        
        
    function definition($form) {

        $listOfBags = (new AgBagAdmin)->listOfBags();
        $listOfFields=(new Field)->currentFieldsArr();
        $footageArr=(new AgBagAdmin)->listOfFootages();
        
        $form->addElement('select', 'bag', null, array('options' => $listOfBags, 'label' => 'Bag'))->addRule("required", "Bag is required.");
        $form->addElement('select', 'field_id', null, array('options' => $listOfFields, 'label' => 'Field'))->addRule("required", "Field is required.");       
        $form->addElement('select', 'footageStart', null, array('options' => $footageArr, 'label' => 'Footage Start'))->addRule("required", "Footage start is required.");
        $form->addElement('select', 'footageFinish', null, array('options' => $footageArr, 'label' => 'Footage Finish'))->addRule("required", "Footage finish is required.");
        
         //defaults
        //use last entered bag as default
       // $keys = array_keys($listOfBags);
     //   $key = $keys[1];
      //  $form->setDefaults(array('bag' => $key)); //set default to no variety.
        
        
         //defaults 
         //find previous fertilizer event and use that as default, unless it is more than a week since it was entered.
 $sql = <<<SQL
SELECT bag_field.*  
FROM nutrition.bag_field
WHERE create_time >= current_date - (interval '1 week')
ORDER BY create_time DESC limit 1
SQL;
     $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row['field_id'])) {
            $defaultsArr=array('field_id'=>$row['field_id'],'bag'=>$row['bag_id'],'footageStart'=>$row['footage_finish'],'footageFinish'=>$row['footage_finish']+10 );
              $form->addDataSource(new HTML_QuickForm2_DataSource_Array($defaultsArr));
        }
      
        
        
 
        return $form; //required       
    }

    function process($value) {
         
        $query = "INSERT INTO nutrition.bag_field (bag_id,field_id,footage_start,footage_finish,userid) VALUES ({$value['bag']},{$value['field_id']},{$value['footageStart']},{$value['footageFinish']},'{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);
    }

}//end class