<?php

/**
 * Lists soil samples for fields and allows us to enter new ones *
 */
class VetToCheck extends TabPage {

    public static $bovineID;
    public static $mode;
    public  $nextVetVisitDate;
    public  $vetCheckDaysArr;
    
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
        
        //find out when next vet visit is.
        $this->vetCheckDaysArr=array_keys(QF2_VetToCheckEvent::whatDayIsTheVetCheck());
        $this->nextVetVisitDate=date('Y-m-d', $this->vetCheckDaysArr[0]);
           
    }

    public function defaultDisplay() {

        //setup for tabs.
        $tabArray[1]['name'] = 'Blocks';
        $tabArray[2]['name'] = 'Completed';
        $tabArray[3]['name'] = 'List';

        $this->renderTabs($tabArray);
    }

    protected function tab1() {
       
        print((new BootStrap)->infoNotify('Note: This pages colour notifications only make sense on the day of the vet check.'));
    //    print($this->vetNEW());
    }

    protected function tab2() {
        print("<h2>Completed Vet Checks</h2>");
        //   $this->displayVetListLog();
    }

    protected function tab3() {

         $this->param = new stdClass();
         $this->param->bovineID = null;
         $this->param->aniNumber = null;
         $a = new QF2_VetToCheckEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->param->bovineID)), 'group', $this->param); //group

         
          $left = <<<HTML
            {$this->notifyVetListFuture()}
            {$this->displayVetListCurrent()}
        HTML;

        $right = <<<HTML
            {$a->renderBox('qf2_VetToCheckEvent', 'Vet To Check')}
        HTML;

        $content = (new Bootstrap)->oneThirdTwoThirds($left, $right);
        print($content);
        
        
    }

    public function VetNEW() {
        $sql = "
Select vet_to_check_event.id, bovine_id,local_number,full_name, location_name, event_time, userid, comment_entered, bovinemanagement.last_breeding(bovine_id) as last_breeding, bovinemanagement.dim(bovine_id),
(SELECT string_agg(event_time || ' ' || type || ' '|| text,' | ') FROM  bovinemanagement.reproduction_events(bovine_id) where event_time >= now() - interval '11 day') as repro_log_11day,
array_to_json (array(SELECT symptom FROM bovinemanagement.vet_to_check_event_symptom LEFT JOIN bovinemanagement.medical_symptom ON medical_symptom.id=medical_symptom_id where vet_to_check_event_id=vet_to_check_event.id)) as symptoms,
array_to_json (array(SELECT medical_symptom.id FROM bovinemanagement.vet_to_check_event_symptom LEFT JOIN bovinemanagement.medical_symptom ON medical_symptom.id=medical_symptom_id where vet_to_check_event_id=vet_to_check_event.id)) as symptoms_id
                    FROM bovinemanagement.vet_to_check_event
                    LEFT JOIN bovinemanagement.bovineall ON bovine_id=bovineall.id 
WHERE userid_completed is null ORDER by event_time DESC
";


        //get data
        $res = $GLOBALS['pdo']->query($sql);
        $out = null;

        //change presentation
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $outS = null;

            //convert json to arrays.
            $symptomsIdArr = json_decode($row['symptoms_id']);
            $symptomsArr = json_decode($row['symptoms']);

            $bootstrapModal = new ModalBox($row['bovine_id']); //modal uniqueid
            $outS = null;
            $outS[] = '<ul>';
            $outS[] = '<li>' . $bootstrapModal->button() . '</li>';
            $outS[] = ('<li>' . '' . $row['comment_entered'] . '</li>');
            $outS[] = ('<li>' . '' . $row['symptoms'] . '</li>');
            $outS[] = ('<li>' . 'DIM: ' . $row['dim'] . '</li>');
            $outS[] = ('<li>' . 'Last Bred: ' . $row['last_breeding'] . '</li>');
            $outS[] = ('<li>' . MovementSortGate::checkToBeSorted($row['bovine_id'], date('Y-m-d')) . '</li>');     //sort gate
            $outS[] = ('<li>' . $this->repoEvents($row['bovine_id']) . '</li>');
            $outS[] = '</ul>';


            $class = $this->boxColourChooser($symptomsIdArr, $row['bovine_id'], $row['last_breeding']); //choose box colour, devault is amber

            $out[] = (new BootStrap)->animalBox($class, $row['bovine_id'], $row['local_number'], implode($outS));

            //modal
            $popoverTitle = 'Animal ' . $row['local_number'];
            $popoverContent = $this->modalContent($row['bovine_id'], $row['repro_log_11day'], $row['symptoms']);
            $out[] = $bootstrapModal->popup($popoverTitle, $popoverContent);
        }
        return(implode($out));
    }

    private function repoEvents($bovine_id) {
        $sql = <<<SQL
 SELECT * from bovinemanagement.reproduction_events($bovine_id) where event_time::date=current_date AND type=ANY ('{"Breeding Scheduled","Kamar Scheduled","Embryo Implant Scheduled","Hormone Scheduled","Protocol"}'::text[])
SQL;
        $res = $GLOBALS['pdo']->query($sql);

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $out[] = $row['type'] . ' ' . $row['text'];
        }
        return implode($out);
    }

    /*
     * this would work better if we need which day the vet was coming for a vet check. maybe we shouldn't choose the date, but the system knows when the vet check is and selects that. 
     * 
     */

    private function pregnantWhatToDo($bovine_id, $class) {
        //pregnant or recheck
        $sql = <<<SQL
SELECT id FROM bovinemanagement.preg_check_event WHERE bovine_id=$bovine_id AND (preg_check_result='pregnant' OR preg_check_result='recheck') and date_trunc('day',event_time)::date = current_date
SQL;
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() > 0) {
            $class = 'calfBoxGreen';
        }

        //protocol started, which means medicines are scheduled and actions are scheduled, so we need those to be zero and we need some medicines to actually be done, or we could still be in the process of 
        $sql = <<<SQL
SELECT * from bovinemanagement.reproduction_events($bovine_id) where event_time::date=current_date AND type=ANY ('{"Kamar Scheduled","Hormone Scheduled"}'::text[])
SQL;
        $res = $GLOBALS['pdo']->query($sql);
        $sql2 = <<<SQL2
SELECT * from bovinemanagement.medical_summary where bovine_id=$bovine_id AND event_time::date=current_date AND type=ANY ('{"Meds"}'::text[])
SQL2;
        $res2 = $GLOBALS['pdo']->query($sql2);



        if (($res->rowCount() == 0) && ($res2->rowCount() > 0)) {
            $class = 'calfBoxGreen';
        }


        return $class;
    }

    private function boxColourChooser($symptomsIdArr, $bovine_id, $last_breeding) {

        $class = 'calfBoxYellow'; //default

        foreach ($symptomsIdArr as $key => $value) {


            switch ($value) {
                case 48: //preg check

                    $class = $this->pregnantWhatToDo($bovine_id, $class);

                    break;

                case 47: //recheck

                    $class = $this->pregnantWhatToDo($bovine_id, $class);

                    break;

                case 54: //protocol progress check
                    $date = new DateTime(); //today
                    $interval = $date->diff(new DateTime($last_breeding));
                    if ($interval->days == 0) {
                        $class = 'calfBoxGreen';
                    }
                    break;

                default:
                    $class = 'calfBoxYellow';
                    break;
            }//end switch
        }
        return $class;
    }

    private function anyMedicineScheduled() {
        $sql = "SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type=ANY(ARRAY['Scheduled Meds','Scheduled Action']) AND event_time >= (current_date )	AND event_time <= (current_date  + interval '12 hours')	
		ORDER BY medical_summary.event_time ASC";
    }

    function modalContent($bovine_id, $reproLog, $symptoms) {
        $out = null;
        $out[] = $reproLog;
        $out[] = $symptoms;
        $out[] = ((new bovineQueryReproductionAccordian)->showAccordion($bovine_id));
        //$out[]=  ((new bovineQueryMedicalAccordian)->showAccordion($bovine_id));



        return implode($out);
    }

    
    //shows future vet list animals on a different list. 
    public function notifyVetListFuture() {
      
        //find out when next vet visit is.
        $nextCheckDate=$this->nextVetVisitDate;

        $str='';
        $sql = <<<SQL
Select vet_to_check_event.id, bovine_id,local_number,full_name, location_name, event_time, userid, comment_entered, 
(SELECT string_agg(event_time || ' ' || type || ' '|| text,' | ') FROM  bovinemanagement.reproduction_events(bovine_id) where event_time >= now() - interval '11 day') as repro_log_11day,
array(SELECT symptom FROM bovinemanagement.vet_to_check_event_symptom LEFT JOIN bovinemanagement.medical_symptom ON medical_symptom.id=medical_symptom_id where vet_to_check_event_id=vet_to_check_event.id) as symptoms
                    FROM bovinemanagement.vet_to_check_event
                    LEFT JOIN bovinemanagement.bovineall ON bovine_id=bovineall.id 
WHERE userid_completed is null  AND event_time > '$nextCheckDate'
ORDER by local_number ASC
SQL;
          $res = $GLOBALS['pdo']->query($sql);
          $inner=array();
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $row['event_time']=date('Y-m-d',strtotime($row['event_time']));
          $inner[]=("<li>#{$row['local_number']},  future vet check scheduled at {$row['event_time']} for  {$row['symptoms']}</li>");
        }
        
        $str=$str.(new BootStrap)->infoNotify(implode($inner));
        
        
     return $str;
        
        
    }

    /////////////////////////////////////////////////////////////////////
    /*
     * shows up to next vet check and before. 
     * 
     */
    public function displayVetListCurrent() {
        
          //find out when next vet visit is.
        $nextCheckDate=$this->nextVetVisitDate;
   
$sql = <<<SQL
        Select vet_to_check_event.id, bovine_id,local_number,full_name, location_name, event_time, userid, comment_entered, fresh_date,bovinemanagement.last_breeding(bovine_id) as last_breeding,
(SELECT json_agg (json_build_array('event_time',(event_time),'type',(type),'text',(text))) FROM bovinemanagement.reproduction_events(bovine_id) where event_time >= now() - interval '11 day')   as repro_log_11day,
(SELECT json_agg (json_build_array(symptom,event_time)) FROM bovinemanagement.vet_to_check_event_symptom LEFT JOIN bovinemanagement.medical_symptom ON medical_symptom.id=medical_symptom_id where vet_to_check_event_id=vet_to_check_event.id) as symptoms
                    FROM bovinemanagement.vet_to_check_event
                    LEFT JOIN bovinemanagement.bovineall ON bovine_id=bovineall.id 
WHERE userid_completed is null  AND event_time <= '$nextCheckDate'::date
ORDER by local_number ASC
SQL;
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            //decode json for repor report
            $arr=(json_decode($row['repro_log_11day'],true));
            $blah=array();
            if (!empty($arr)) {
            foreach ($arr as $key => $value) {
               $blah[]= JQueryDataTable::formattedDate($value[1],'M d') . ' <b>'.  $value[3] . '</b> '. $value[5] . '<br> ';
            }}
                 
            $symptomsArr=json_decode($row['symptoms'],true);
           
            //array can return 0, which is false, so use ===
            if ((array_search(strtotime($symptomsArr[0][1]),$this->vetCheckDaysArr))===false) {
                $row['symptoms']=(new BootStrap)->warningCallout('Out of Date');
            } else {
                  $row['symptoms']= $symptomsArr[0][0];
            }  

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Location (Group)'] = ($row['location_name']);
            $rowOut['Date of Vet Check'] = JQueryDataTable::formattedDate($row['event_time'],'M d');
            $rowOut['Employee (Entered)'] = ($row['userid']);
            $rowOut['Repro Log (11 day)'] = implode($blah);
            $rowOut['Symptom(s)'] = $row['symptoms'];
            $rowOut['Fresh Date'] = JQueryDataTable::formattedDate($row['fresh_date']);
            $rowOut['Last Breeding'] = JQueryDataTable::formattedDate($row['last_breeding']);
            $rowOut['comment'] = ($row['comment_entered']);
            $rowOut['Action'] = $this->printActionButtons($row['id'], $row['bovine_id'], $row['event_time'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);

        $options = <<<OPT
        dom: 'Bfrtip',
        buttons: [
        {
            extend: 'print',
            text: 'Print Table',
            exportOptions: {
                modifier: {
                    page: 'current'
                }
            }
        }
        ],
                 searching: false,paging: false,ordering: true,info: true
OPT;
        
        
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Vet Check List', $dataArr,$options));
        return implode($out);
    }

    /////////////////////////////////////////////////////////////////////
    //LOG
    /////////////////////////////////////////////////////////////////////
    public function vetListLog($request) {
        $sql = "Select vet_to_check_event.id as id, bovine_id,local_number,full_name, location_name, event_time, userid, comment_entered ,event_time_completed,userid_completed,comment_completed
                    FROM bovinemanagement.vet_to_check_event
                    LEFT JOIN bovinemanagement.bovinecurr ON bovine_id=bovinecurr.id WHERE userid_completed is not null ORDER by event_time_completed DESC";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);

            $row['action'] = $this->printActionButtons2($row['id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['event_time'] = date("l M d H:00", strtotime($row['event_time']));  //has to be at end.
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function printActionButtons($id, $bovine_id, $event_time, $callBackURL = null) {

        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.vet_to_check_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");
        {
            
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if (($id == $row['id']) OR ( (in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) {

            $deleteButtonAsString = self::deleteQuickForm($id, $callBackURL); //show delete button, with estrus table id as index for delete.
        }

        //sort gate
        $checkToBeSorted = MovementSortGate::checkToBeSorted($bovine_id, $event_time);

        return $checkToBeSorted . $deleteButtonAsString;
    }

    function deleteQuickForm($vetToCheckID, $callBackURL) {

        $form = new HTML_QuickForm("VetToCheckDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtablelessButton'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'vetToCheckID', $vetToCheckID);
        $form->addElement('submit', 'btnDelete', 'Delete', array('class' => 'btn btn-warning', 'id' => 'deleteSubmit', "onclick" => "return confirm('Are you sure you want to delete?');")); //call javascript to confirm delete.
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

    function printActionButtons2($id, $callBackURL = null) {

        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.vet_to_check_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");
        {
            
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if (($id == $row['id']) OR ( (in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) {

            $deleteButtonAsString = self::deleteQuickFormCompleted($id, $callBackURL); //show delete button, with estrus table id as index for delete.
        }
        return $deleteButtonAsString;
    }

    function deleteQuickFormCompleted($vetToCheckID, $callBackURL) {

        $form = new HTML_QuickForm("VetToCheckCompletedDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtablelessButton'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'vetToCheckID', $vetToCheckID);
        $form->addElement('submit', 'btnDelete', 'Undo Completed', array('class' => 'btn btn-warning', 'id' => 'deleteSubmit', "onclick" => "return confirm('Are you sure you want to UNDO?');")); //call javascript to confirm delete.
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

        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastSevenDays();
        unset($timeArray [null]); // remove null value.


        $res = $GLOBALS['pdo']->query("SELECT id,symptom from bovinemanagement.medical_symptom  ORDER BY reproductive_related,symptom");

        $symptoms[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $symptoms[$row[0]] = $row[1];
        }


        // list of pre-canned comments
        $preCannedComment = array(null => 'Please Select one...', 'ok.' => 'ok.', 'recheck.' => 'recheck.');

        $form = new HTML_QuickForm("newVetToCheckForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'newVetToCheckForMedicalCaseQuickForm', 'Vet to Check');
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        // depending on if we are called from aninmal query page or medical treament page.
        if ($mode == 'group') {
            $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();     //grab a list of cows with open medical cases.

            $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
            $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        } else {
            $form->addElement('hidden', 'animalNumber', $_REQUEST ['bovine_id']);
        }

        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);
        $form->addElement('select', 'dateEvent', 'Symptom', $symptoms);


        $form->addElement('textarea', 'comment2', 'Custom Comment', array('cols' => 40, 'rows' => 6));

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

            $comment2 = trim($form->exportValue('comment2'));

            /*
             * print("bovine_id:$bovine_id<br>");
             * print("comment1:$comment1<br>"); print("comment2:$comment2<br>");
             * print("pageid:$pageid<br>");
             */

            // concatenate comments if they are both entered.
            if (!strlen($comment2) == 0) {
                $comment = $comment2;
            } else {
                $comment = null;
            }

            // do postgres escape.
            $comment_esc = pg_escape_string($comment);

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

            try {
                $res = $GLOBALS['pdo']->beginTransaction();
                $query = "INSERT INTO bovinemanagement.vet_to_check_event (medical_case_id,bovine_id,event_time,comment_entered,userid) VALUES ($medical_case_id,$bovine_id,'$event_time','$comment_esc','{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);
                //get last insertid 
                $last_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.medical_generic_id_seq');

                //  foreach {
                //insert any symptoms, usually one, but we support multiple symptoms. 
                $query = "INSERT INTO bovinemanagement.vet_to_check_event_symptom (vet_to_check_event_id,medical_symptom_id) VALUES ($last_id,$medical_symptim_id)";
                $res = $GLOBALS['pdo']->exec($query);
                //   }


                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }




            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&bovine_id=$bovine_id");
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
            $aniNumber[$row[0]] = $row[2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
        }
        return $aniNumber;
    }

    function vetCheckCompletedQuickForm($mode = 'group', $bovine_id = null) {

        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastSevenDays();
        unset($timeArray [null]); // remove null value.
        // list of pre-canned comments
        $preCannedComment = array(null => 'Please Select one...', 'ok.' => 'ok.', 'recheck.' => 'recheck.');

        $form = new HTML_QuickForm("vetCheckCompletedQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


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
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

}

//end class

Class QF2_VetToCheckEvent extends QF2_Base {

    function definition($form) {


        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();

        $dateArray= $this->whatDayIsTheVetCheck();
        
        unset($dateArray [null]); // remove null value.

        $res = $GLOBALS['pdo']->query("SELECT id,symptom from bovinemanagement.medical_symptom  ORDER BY reproductive_related,symptom");
        $symptoms[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $symptoms[$row[0]] = $row[1];
        }


        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 


        $form->addSelect('symptoms', array('multiple' => 'multiple', 'id' => 'symptoms'))->loadOptions($symptoms)->addRule("required", "Please specify your type of symptom");     //2nd js select2
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");
        $form->addElement('textarea', 'comment', array('style' => 'width: 100%;', 'cols' => 20, 'rows' => 2), array('label' => 'Test Textarea:'))->setLabel('Comment (optional)');

        $form->addElement('static', 'script_symptom')->setValue($this->scriptJS());



        return $form; //required       
    }

    private function scriptJS() {
        $jscript = <<<JSCRIPT
            
           
 <script>           
var s3 = $("#symptoms").select2({
    placeholder: "Choose Symptom(s)",
    tags: false
});   
      </script>
JSCRIPT;
        return $jscript;
    }

    function process($value) {

        $event_time = date('Y-m-d H:i:sO', $value['dateEvent']);
        // do postgres escape.
        $comment_esc = pg_escape_string($value['comment']);
        $symptomsArr = $value['symptoms'];

        try {
            $res = $GLOBALS['pdo']->beginTransaction();
            //loop over animals
            $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
            foreach ($bovineIdArr as $key => $bovine_id) {


                // lookup bovine id and see if there is an open medical case during
                // the requested event_time
                $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);
                $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

                //insert main event.   
                $query = "INSERT INTO bovinemanagement.vet_to_check_event (medical_case_id,bovine_id,event_time,comment_entered,userid) VALUES ( $medical_case_id,$bovine_id, '$event_time','$comment_esc','{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);
                //get last insertid 
                $last_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.medical_generic_id_seq');

                //insert any and all symptoms selected.       
                foreach ($symptomsArr as $key => $value) {
                    $query = "INSERT INTO bovinemanagement.vet_to_check_event_symptom (vet_to_check_event_id,medical_symptom_id) VALUES ($last_id,$value)";
                    $res = $GLOBALS['pdo']->exec($query);
                }
            }//end animals forloop

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
    }

    /*
     * used by home page calendar
     */
     public function whatDayIsTheNextVetCheck() {
          //find out when next vet visit is.
        $vetCheckDaysArr=array_keys($this->whatDayIsTheVetCheck());
        $nextVetVisitDate=date('D M d', $vetCheckDaysArr[0]);
        return $nextVetVisitDate;
    }
    
    public static function whatDayIsTheVetCheck() {
        //occurs every 3rd tuesday and every 3rd friday inbetween.
        //Use Feb 11 2020 as tuesday starting date. 

        $dateArr = array();

        //Tuesday dates.
        $beginTue = new DateTime('2020-02-11'); //use this as a start date because it was a real vet visit. 
        $end = new DateTime(); //today
        $end = $end->modify('+50 day');

        $interval = new DateInterval('P21D');
        $daterange = new DatePeriod($beginTue, $interval, $end);

        foreach ($daterange as $date) {
            if (($date) >=  (new DateTime('yesterday'))) {          
            $dateArr[$date->format("U")] = $date->format("D, Y-m-d");
            }
        }

        //Friday dates. 
        $begin = new DateTime('2020-02-21'); //use this as a start date because it was a real vet visit. 
        $end = new DateTime(); //today
        $end = $end->modify('+50 day');

        $interval = new DateInterval('P21D');
        $daterange = new DatePeriod($begin, $interval, $end);

        foreach ($daterange as $date) {
               if (($date) >=  (new DateTime('yesterday'))) {      
            $dateArr[$date->format("U")] = $date->format("D, Y-m-d");
             }
        }

        ksort($dateArr); //sort array
       
        return($dateArr);
    }

}

//end class