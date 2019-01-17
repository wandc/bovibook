<?php 


/**
 * Lists soil samples for fields and allows us to enter new ones *
 */
class VetToCheck extends TabPage {

    public static $bovineID;
    public static $mode;

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct($bovineID = null) {
        parent::__construct();

        if ($bovineID != null) {
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
        } else {
            self::$bovineID = null;
            self::$mode = 'group';
        }
    }

    public function defaultDisplay() {

        //setup for tabs.
        $tabArray[1]['name'] = 'List';
        $tabArray[2]['name'] = 'Completed';
        $this->renderTabs($tabArray);
    }

    protected function tab1() {
        print("<h2>Vet List</h2>");
        
        $this->displayVetListCurrent();
        print('</br>');
        print('</br>');
        print('<div id="twoColumnLayout">');
        print('<p>');
        print("<h2>Enter New</h2>");
        self::newVetToCheckForMedicalCaseQuickForm(self::$mode, self::$bovineID);
        print('</p>');
        print('<p>');
        print("<h2>To Complete</h2>");
        self::vetCheckCompletedQuickForm();
        print('</br>');
        print('</br>');
        print('</br>');
        print('</br>');
        print('</p>');
        print('</div>');
    }

    protected function tab2() {
        print("<h2>Completed Vet Checks</h2>");
        $this->displayVetListLog();
    }

    /////////////////////////////////////////////////////////////////////
     public function vetListCurrent($request) {
         $sql = "
Select vet_to_check_event.id, bovine_id,local_number,full_name, location_name, event_time, userid_entered, comment_entered, 
(SELECT string_agg(event_time || ' ' || type || ' '|| text,' | ') FROM  bovinemanagement.reproduction_events(bovine_id) where event_time >= now() - interval '11 day') as repro_log_11day
                    FROM bovinemanagement.vet_to_check_event
                    LEFT JOIN bovinemanagement.bovinecurr ON bovine_id=bovinecurr.id WHERE userid_completed is null ORDER by event_time DESC
";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            
           $row['local_number'] =$GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
           $row['event_time_raw']=strtotime($row['event_time']);
         
           $row['action'] = $this->printActionButtons($row['id'],$row['bovine_id'],$row['event_time'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
           $row['event_time']= date("l M d H:00",strtotime($row['event_time']));  //has to be at end.
          
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    function displayVetListCurrent() {
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "event_time",
                title: "Time (Entered)"
            },
            {
                data: "userid_entered",
                title: "Employee (Entered)"
            },
             {
                data: "repro_log_11day",
                title: "repro_log_11day"
            },
           
             {
                data: "comment_entered",
                title: "Note"
            },
            {
                data: "action",
                title: "Action"
            }
EX;
    print($dt->start(119,'vetListCurrent',null,$columnsStr));     
    
  
}     
    
    
    /////////////////////////////////////////////////////////////////////
    //LOG
 /////////////////////////////////////////////////////////////////////
     public function vetListLog($request) {
         $sql = "Select vet_to_check_event.id as id, bovine_id,local_number,full_name, location_name, event_time, userid_entered, comment_entered ,event_time_completed,userid_completed,comment_completed
                    FROM bovinemanagement.vet_to_check_event
                    LEFT JOIN bovinemanagement.bovinecurr ON bovine_id=bovinecurr.id WHERE userid_completed is not null ORDER by event_time_completed DESC";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
           $row['event_time_raw']=strtotime($row['event_time']);
         
           $row['action'] = $this->printActionButtons2($row['id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
           $row['event_time']= date("l M d H:00",strtotime($row['event_time']));  //has to be at end.
          
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
     function displayVetListLog() {
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "event_time",
                title: "Time (Entered)"
            },
            {
                data: "userid_entered",
                title: "Employee (Entered)"
            },      
             {
                data: "comment_entered",
                title: "Note"
            },
            {
                data: "event_time_completed",
                title: "Time (Completed)"
            },
            {
                data: "userid_completed",
                title: "Employee (Completed)"
            },
        {
                data: "comment_completed",
                title: "Note (Completed)"
            },
            {
                data: "action",
                title: "Action"
            }
EX;
    print($dt->start(119,'vetListLog',null,$columnsStr));     
    
  
}     
    function printActionButtons($id,$bovine_id,$event_time,$callBackURL=null) {
     
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.vet_to_check_event WHERE userid_entered='$currentUserid' ORDER BY id DESC LIMIT 1");
        {
          
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if (($id == $row['id']) OR ( (in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) {

            $deleteButtonAsString = self::deleteQuickForm($id,$callBackURL); //show delete button, with estrus table id as index for delete.
        }

         //sort gate
        require_once $_SERVER['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/movementSortGate.inc'; 
        $checkToBeSorted = MovementSortGate::checkToBeSorted($bovine_id, $event_time);
        
        return $checkToBeSorted.$deleteButtonAsString;
    }

    function deleteQuickForm($vetToCheckID,$callBackURL) {

        $form = new HTML_QuickForm("VetToCheckDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtablelessButton'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
       
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'vetToCheckID', $vetToCheckID);
        $form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'deleteSubmit', "onclick" => "return confirm('Are you sure you want to delete?');")); //call javascript to confirm delete.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $vetToCheckID = $form->getElementValue('vetToCheckID');

            //print("<br>DELETING $vetToCheckID !!!!<br>");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.vet_to_check_event where id='$vetToCheckID'");
            {
              
            } // Always check that result is not an error
            //print("DELETING succesful");
            header($callBackURL);
            exit();
        }
        $form->accept($renderer);
        return $renderer->toHtml();
    }

    function printActionButtons2($id,$callBackURL=null) {
       
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.vet_to_check_event WHERE userid_entered='$currentUserid' ORDER BY id DESC LIMIT 1");
        {
          
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if (($id == $row['id']) OR ( (in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) {

            $deleteButtonAsString = self::deleteQuickFormCompleted($id,$callBackURL); //show delete button, with estrus table id as index for delete.
        }      
        return $deleteButtonAsString;
    }

    function deleteQuickFormCompleted($vetToCheckID,$callBackURL) {

        $form = new HTML_QuickForm("VetToCheckCompletedDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtablelessButton'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();
        
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'vetToCheckID', $vetToCheckID);
        $form->addElement('submit', 'btnDelete', 'Undo Completed', array('id' => 'deleteSubmit', "onclick" => "return confirm('Are you sure you want to UNDO?');")); //call javascript to confirm delete.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $vetToCheckID = $form->getElementValue('vetToCheckID');

            //print("<br>DELETING $vetToCheckID !!!!<br>");
            $res = $GLOBALS['pdo']->query("UPDATE bovinemanagement.vet_to_check_event SET (comment_completed,	userid_completed,	event_time_completed) = (null,null,null) WHERE id='$vetToCheckID'");
            {
              
            } // Always check that result is not an error
            //print("DELETING succesful");
            header($callBackURL);
            exit();
        }
        $form->accept($renderer);
        return $renderer->toHtml();
    }

    public static function newVetToCheckForMedicalCaseQuickForm($mode = 'group', $bovine_id = null) {

        $timeArray = Misc::createDailyDatesForLastSevenDays();
        unset($timeArray [null]); // remove null value.
        // list of pre-canned comments
        $preCannedComment = array(null => 'Please Select one...', 'ok.' => 'ok.', 'recheck.' => 'recheck.');

        $form = new HTML_QuickForm("newVetToCheckForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'newVetToCheckForMedicalCaseQuickForm', 'Vet to Check');
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        // depending on if we are called from aninmal query page or medical treament page.
        if ($mode == 'group') {
            $aniNumber = Misc::createListOfAllAliveCows();     //grab a list of cows with open medical cases.
            
            $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
            $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        } else {
            $form->addElement('hidden', 'animalNumber', $_REQUEST ['bovine_id']);
        }

        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);

        $form->addElement('select', 'comment1', 'Canned Comment', $preCannedComment);
        $form->addElement('textarea', 'comment2', 'OR Custom Comment', array('cols' => 40, 'rows' => 6));

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        if ($mode == 'group') {
            $form->setDefaults(array('animalNumber' => 0)); // set default to no
            // variety.
        }

        // rules
        if ($mode == 'group') {
            $form->addRule('animalNumber', 'Animal number is required', 'required');
        }

        $form->addRule('dateEvent', 'Event Time is required', 'required');
        // custom rule for teats (at least one needs to be selected)
        $form->registerRule('commentRequiredRule', 'function', 'commentRequired', 'MedicalCase');
        $form->addRule(array('animalNumber', 'comment1', 'comment2'), 'A canned or custom comment is required.', 'commentRequiredRule', array('animalNumber', 'comment1', 'comment2'));

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $bovine_id = $form->exportValue('animalNumber');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray [0]);
            $pageid = $form->exportValue('pageid');
            $comment1 = trim($form->exportValue('comment1'));
            $comment2 = trim($form->exportValue('comment2'));

            /*
             * print("bovine_id:$bovine_id<br>");
             * print("comment1:$comment1<br>"); print("comment2:$comment2<br>");
             * print("pageid:$pageid<br>");
             */

            // concatenate comments if they are both entered.
            if (strlen($comment1) == 0) {
                $comment = $comment2;
            } else {
                $comment = $comment1 . ' ' . $comment2;
            }

            // do postgres escape.
            $comment_esc = pg_escape_string($comment);

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.vet_to_check_event (medical_case_id,bovine_id,event_time,comment_entered,userid_entered) VALUES ($medical_case_id,$bovine_id,'$event_time','$comment_esc','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS ['pdo']->exec($query);

            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    private function createListOfAllCowsOpenVetCheck($animalQuery = false) {
        $aniNumber[null] = "Please Select one..."; //dummy value
        if ($animalQuery == true) {
            $aniNumber[null] = "?";
        } //special case for at top of page.
//grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT vet_to_check_event.id,full_name,local_number,event_time from bovinemanagement.vet_to_check_event LEFT JOIN  bovinemanagement.bovinecurr ON bovinecurr.id=vet_to_check_event.bovine_id WHERE userid_completed IS NULL ORDER BY local_number");
        {
          
        } // Always check that result is not an error
        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . Misc::femaleShortName($row[1]);
        }
        return $aniNumber;
    }

    function vetCheckCompletedQuickForm($mode = 'group', $bovine_id = null) {

        $timeArray = Misc::createDailyDatesForLastSevenDays();
        unset($timeArray [null]); // remove null value.
        // list of pre-canned comments
        $preCannedComment = array(null => 'Please Select one...', 'ok.' => 'ok.', 'recheck.' => 'recheck.');

        $form = new HTML_QuickForm("vetCheckCompletedQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
       

        $form->addElement('header', 'vetCheckCompletedQuickForm', 'Vet Check Completed');
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        // depending on if we are called from aninmal query page or not.
        if ($mode == 'group') {
            $aniNumber = self::createListOfAllCowsOpenVetCheck(); // grab a list of all open cases

            $form->addElement('select', 'vetToCheckID', 'Case', $aniNumber);
        } else {
            $sql = "SELECT vet_to_check_event.id WHERE bovine_id={$_REQUEST ['bovine_id']} LIMIT 1";
            $res2 = $GLOBALS['pdo']->query($sql);
            $row2 = $res2->fetch();
            $form->addElement('hidden', 'vetToCheckID', $row2['id']);
        }

        $form->addElement('hidden', 'dateEvent', 'Time of Check', strtotime("now"));
        $form->addElement('select', 'comment1', 'Canned Comment', $preCannedComment);
        $form->addElement('textarea', 'comment2', 'OR Custom Comment', array('cols' => 40, 'rows' => 6));

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        if ($mode == 'group') {
            $form->setDefaults(array('vetToCheckID' => 0)); // set default to no
            // variety.
        }
        // rules
        if ($mode == 'group') {
            $form->addRule('vetToCheckID', 'Animal number is required', 'required');
        }

        $form->addRule('dateEvent', 'Event Time is required', 'required');
        // custom rule for teats (at least one needs to be selected)
        $form->registerRule('commentRequiredRule', 'function', 'commentRequired', 'MedicalCase');
        $form->addRule(array('vetToCheckID', 'comment1', 'comment2'), 'A canned or custom comment is required.', 'commentRequiredRule', array('vetToCheckID', 'comment1', 'comment2'));

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $vetToCheckID = $form->exportValue('vetToCheckID');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', strtotime("now"));
            $pageid = $form->exportValue('pageid');
            $comment1 = trim($form->exportValue('comment1'));
            $comment2 = trim($form->exportValue('comment2'));

            /*
             * print("bovine_id:$bovine_id<br>");
             * print("comment1:$comment1<br>"); print("comment2:$comment2<br>");
             * print("pageid:$pageid<br>");
             */

            // concatenate comments if they are both entered.
            if (strlen($comment1) == 0) {
                $comment = $comment2;
            } else {
                $comment = $comment1 . ' ' . $comment2;
            }

            // do postgres escape.
            $comment_esc = pg_escape_string($comment);

            // don't put id value, uses next one in DB.
            $query = "UPDATE bovinemanagement.vet_to_check_event SET event_time_completed='$event_time',comment_completed='$comment_esc',userid_completed='{$GLOBALS['auth']->getUsername()}' WHERE id=$vetToCheckID";
            $res = $GLOBALS ['pdo']->exec($query);

            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

}//end class
?>