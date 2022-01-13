<?php

/*
  //line of sql to debug this page if necessary.
  SELECT local_number,location_name,date_trunc('days',now()-fresh_date) as dim FROM bovinecurr WHERE date_trunc('days',now()-fresh_date) is not null ORDER by dim DESC
 */

class TransitionDryoff extends BasePage {

    public function defaultDisplay() {

        $this->param = new stdClass();
        $this->param->bovineID = null;
        $this->param->aniNumber = QF2_DryOfCowfEvent::listofCowsEligigbleToDryOff(); //pass along a list of cows that can be dried off.
        $QF2_DryOfCowfEvent = new QF2_DryOfCowfEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => null)), 'list', $this->param); //group
        //
        $this->param->bovineID = null;
        $this->param->aniNumber = QF2_DryOffSchedule::listofLacatingCows(); //pass along a list of cows that are milking
        $QF2_DryOffSchedule = new QF2_DryOffSchedule((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => null)), 'list', $this->param); //group


        $content1 = (new Bootstrap)->oneThirdTwoThirds($this->displayDryOffScheduledCurrent(), $QF2_DryOffSchedule->renderBox('QF2_DryOffSchedule', 'Schedule Cow to Start Dy Off Protocol'));
        $content2 = (new Bootstrap)->oneThirdTwoThirds($this->displayListOfDryOffs(), $QF2_DryOfCowfEvent->renderBox('QF2_DryOfCowfEvent', 'Dry Cow Off'));
        print($content1.$content2);
    }

    public function milkTestInputsDisplay() {
        //show list of dryoffs for valacta input test.
        //the pageid being passed needs to be the one for this class. 
        print($this->displayListOfDryOffs('milkTestInputs'));
    }

    function displayDryOffScheduledCurrent($mode = 'default') {


        $sql = <<<SQL
SELECT dryoff_schedule.*,bovine_id,local_number,full_name FROM bovinemanagement.dryoff_schedule 
    LEFT JOIN bovinemanagement.lactation ON lactation.id=dryoff_schedule.lactation_id
                LEFT JOIN bovinemanagement.bovine ON lactation.bovine_id=bovine.id WHERE dryoff_event_id is null 
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation

        $callBackURL = ("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

        if (empty($results)) {
            return 'No Data';
        } else {
            foreach ($results as &$row) {

                $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
                $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);


                $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'bovinemanagement.dryoff_schedule'));
                $returnButtonsAsString = ($a->renderDelete('salePrice' . $row['id']));
                $rowOut['Action'] = $returnButtonsAsString . MovementSortGate::checkToBeSorted($row['bovine_id'], $row['event_time']);
                $resultsOut[] = $rowOut;
            }

            //output
            $dataArr = $resultsOut;

            $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Dry Off Protocol Start Scheduled', $dataArr));
            return implode($out);
        }
    }

    /*     * ******************************* */
    /* Display list of all dryoff cases */
    /*     * ******************************* */

    function displayListOfDryOffs() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "userid",
                title: "Employee"
            },        
            {
                data: "action",
                title: "Action"
            }
EX;

        //must be static pageid to work on valacta report. 
        $out[] = ($dt->start('Dried Off Log', 44, 'AllDryOffEvents', null, $columnsStr));
        return implode($out);
    }

    
    
    function displayListOfDryOffsMilkTester() {
        
        $arr=$this->AllDryOffEvents($mode = 'default');
       // $dataArr = array_slice($arr, 0, 80); //only latest 80.
        
         print( (new JQueryDataTable)->startBasicArrayOpt('Dry Off List (Latest 80)', $arr));
        
        
    }
    
    public function AllDryOffEvents($mode = 'default') {

        $sql = "SELECT bovine.id as bovine_id,dryoff_event.lactation_id,event_time,local_number,full_name,dryoff_event.userid,bovinemanagement.due_date(bovine.id)
                    FROM bovinemanagement.dryoff_event 
LEFT JOIN bovinemanagement.lactation ON lactation.id = dryoff_event.lactation_id
LEFT JOIN bovinemanagement.bovine ON lactation.bovine_id = bovine.id
ORDER BY dryoff_event.event_time DESC";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] = date("l M d, Y", strtotime($row['event_time']));
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['due_date'] = date("l M d, Y", strtotime($row['due_date']));
            $row['userid'] = $row['userid'];
            $row['action'] = $this->printActionButtons($row['lactation_id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

////////////////////////////////////////////////////////////////////////

   

    function printActionButtons($lactation_id, $callBackURL = null) {
        $returnButtonsAsString = '';
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        //print latest delete button (one) for current user
        $res = $GLOBALS['pdo']->query("SELECT lactation_id FROM bovinemanagement.dryoff_event WHERE userid='$currentUserid' ORDER BY event_time DESC LIMIT 1");

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            
            if ($lactation_id == $row['lactation_id']) {
                $a = new QF2_DeleteButtonDryOff((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $lactation_id));
                $returnButtonsAsString = ($a->renderDelete('deleteButtonDryOff' . $row['lactation_id']));
            } elseif ($GLOBALS['auth']->getAdminMode() == 1) { //ALWAYS SHOW FOR ADMIN. 
                $a = new QF2_DeleteButtonDryOff((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $lactation_id));
                $returnButtonsAsString = ($a->renderDelete('deleteButtonDryOff' . $row['lactation_id']));
            }
        }

        return $returnButtonsAsString;
    }

    //shows error.
    function error() {

        ////////////////////////
        //looks if cow is 60 days from calving and not dried off.
        $sql = "SELECT bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name, calculated_potential_due_date,bovinecurr.location_id, location_name FROM bovinemanagement.bovinecurr LEFT JOIN bovinemanagement.pregnant_view on bovinecurr.id=pregnant_view.id WHERE calculated_potential_due_date < (current_date + interval '60 days') AND fresh_date is not null and dry_date is null ORDER BY calculated_potential_due_date DESC
	     ";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error["{$row['local_number']}"] = new notifyObj(get_class($this), $row['calculated_potential_due_date'], 1, $row['local_number'] . ' less than 60 days to calving, needs dried off.');
        }
        if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
    }

    /*
     * used to tell if curretnly in dry-off protcol for indivudal animal. 
     */
      public static function isAnimalInDryOffProtocol($bovineID) {
          
          $sql = <<<SQL
          SELECT dryoff_schedule.*,bovine_id,local_number,full_name 
FROM bovinemanagement.dryoff_schedule 
    LEFT JOIN bovinemanagement.lactation ON lactation.id=dryoff_schedule.lactation_id
    LEFT JOIN bovinemanagement.bovine ON lactation.bovine_id=bovine.id WHERE dryoff_event_id is null AND bovine.id=$bovineID
SQL;
          
          
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row)) {
            return strtotime($row ['event_time']);
        } else {
            return null;
        }
    }
    
//must be sick group for the 4 or 7 days. 
//total daily milk below: (should be milking 1x per day)    
//4 days at 1x and below 16 liters
//7 days at 1x and below 20 liters

    function inProtocolLongEnoughtoDryOff() {

        ////////////////////////

        $sql = <<<SQL
with temp as (
SELECT dryoff_schedule.*,bovine_id,local_number,full_name 
FROM bovinemanagement.dryoff_schedule 
    LEFT JOIN bovinemanagement.lactation ON lactation.id=dryoff_schedule.lactation_id
    LEFT JOIN bovinemanagement.bovine ON lactation.bovine_id=bovine.id WHERE dryoff_event_id is null 
)
 Select *,bovinemanagement.location_event_id_time_at_location (bovine_id)  ,alpro.latest_full_day_production (bovine_id)
FROM temp where event_time::date  >= (current_date - interval '4 days') AND bovinemanagement.location_id_at_timestamp (bovine_id, current_date)=39

SQL;
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error["{$row['local_number']}"] = new notifyObj(get_class($this), $row['calculated_potential_due_date'], 1, $row['local_number'] . ' less than 60 days to calving, needs dried off.');
        }
        if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
    }

}

//end class

Class QF2_DeleteButtonDryOff extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'deleteID')->setValue($this->param->deleteID); //client could change this, we leave it here for debugging, but we don't actually use it. 

        return $form; //required       
    }

    function process($value) {


        if (empty($this->param->deleteID)) {
            throw new Exception("Error: param->deleteID not provided!");
        }


        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            $lactationID = $this->param->deleteID;

            //look up what all the med administered ids are and then delete each of them.
            $res = $GLOBALS['pdo']->query("SELECT med_administered_a1,med_administered_a2,med_administered_a3,med_administered_a4,med_administered_b1,med_administered_b2,med_administered_b3,med_administered_b4 FROM bovinemanagement.dryoff_event WHERE lactation_id=$lactationID LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);

            //delete the dryoff_event
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.dryoff_event where lactation_id='$lactationID'");

            //delete each med administered
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a1']}'");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a2']}'");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a3']}'");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a4']}'");
            //
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b1']}'");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b2']}'");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b3']}'");
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b4']}'");

            //update the lactation to remove dry date.
            $query = "UPDATE bovinemanagement.lactation SET dry_date= NULL WHERE id=$lactationID";
            $res = $GLOBALS['pdo']->exec($query);


            $res = $GLOBALS['pdo']->prepare("UPDATE bovinemanagement.dryoff_schedule SET dryoff_event_id=null WHERE lactation_id=:lactation_id");
            $res->bindParam(':lactation_id', $lactationID);
            $res->execute();



            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
    }

}

//end class

Class QF2_DryOfCowfEvent extends QF2_Base {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //hourly array, with unix timestamp as index.

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form, $this->param->aniNumber, $this->mode);

        $form->addElement('checkbox', 'sealant', array(), array('label' => 'Treated with Orbaseal (all 4 teats)'))->addRule("required", "Orbaseal is required.");
        $form->addElement('checkbox', 'antibiotic', array(), array('label' => 'Treated with Novodry Plus (all 4 teats)'))->addRule("required", "Novodry is required.");
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Final Milking and Treatement'))->addRule("required", "Please specify your event time.");

        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('sealant' => true, 'antibiotic' => true))); //set defaults

        return $form; //required       
    }

    function process($value) {

        //
        $lactation_id = $value['animalSelected']; //not actually bovine_id, but lactation_id
        $event_time = date('Y-m-d H:i:sO', $value['event_time']);

        ///////////////////////////////////////
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            //do an insert into lactation with dry off date.
            //get lactation id.
            $sql = "SELECT bovine_id FROM bovinemanagement.lactation WHERE id=$lactation_id LIMIT 1";
            $res2 = $GLOBALS['pdo']->query($sql);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $bovine_id = $row2['bovine_id'];


            //do an insert into medicine adminsterd for orbaseal and novodry
            $retArrayA = $this->treatTeats($bovine_id, 10, $event_time); //orbeseal is 10
            $retArrayB = $this->treatTeats($bovine_id, 42, $event_time); //novodry is 42.
            //do an insert into dryoff_event

            $query = "INSERT INTO bovinemanagement.dryoff_event (event_time,lactation_id,med_administered_a1,med_administered_a2,med_administered_a3,med_administered_a4,med_administered_b1,med_administered_b2,med_administered_b3,med_administered_b4,userid)
                                 VALUES ('$event_time',$lactation_id,{$retArrayA['front left']},{$retArrayA['front right']},{$retArrayA['rear left']},{$retArrayA['rear right']},{$retArrayB['front left']},{$retArrayB['front right']},{$retArrayB['rear left']},{$retArrayB['rear right']},'{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);

            $last_dryoff_event_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.dryoff_event_id_seq');

            //update dryoff_schedule with info.        
            $query = "UPDATE bovinemanagement.dryoff_schedule SET dryoff_event_id=$last_dryoff_event_id WHERE lactation_id=$lactation_id";
            $res = $GLOBALS['pdo']->exec($query);

            //update lactation id with info.        
            $query = "UPDATE bovinemanagement.lactation SET dry_date='$event_time' WHERE id=$lactation_id";
            $res = $GLOBALS['pdo']->exec($query);

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
    }

    private function treatTeats($bovine_id, $medicine_id, $event_time) {
//do all four quarters
        $quarterArray['front left'] = 1;
        $quarterArray['front right'] = 1;
        $quarterArray['rear left'] = 1;
        $quarterArray['rear right'] = 1;


        foreach ($quarterArray as $key => $value) {
            if ($value == 1) { //only do if the value is true
                $locationStr = "$key" . ' teat canal'; //make string from array key value
                //find default dosage
                $sql = "SELECT default_dosage,default_dosage_unit FROM bovinemanagement.medicine WHERE id=$medicine_id LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);

                //administer drug
                $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid)
                                 VALUES ($bovine_id,$medicine_id,'$event_time','{$row2['default_dosage']}','{$row2['default_dosage_unit']}','$locationStr',null,null,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);
                $retArray[$key] = $GLOBALS['pdo']->lastinsertId('bovinemanagement.medical_generic_id_seq');
            }
        }

        return $retArray;
    }

    public static function listofCowsEligigbleToDryOff() {

        $sql = <<<SQL
                SELECT 
lactation.id as lactation_id,bovinecurr.fresh_date,bovinecurr.local_number,bovinecurr.full_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim  
FROM bovinemanagement.dryoff_schedule 
                LEFT JOIN bovinemanagement.lactation on lactation.id=dryoff_schedule.lactation_id
                LEFT JOIN bovinemanagement.bovinecurr ON lactation.bovine_id=bovinecurr.id 
WHERE dryoff_event_id is null  
ORDER by local_number 
SQL;

        $res = $GLOBALS['pdo']->query($sql);

        $aniNumber [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            $aniNumber[$row['lactation_id']] = $row['local_number'] . " - " . $GLOBALS['MiscObj']->femaleShortName($row['full_name']) . " (" . $row['dim'] . ")";
        }

        return $aniNumber;
    }

}

//end class

Class QF2_DryOffSchedule extends QF2_Base {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForNext14DaysAtMidnight();

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form, $this->param->aniNumber, $this->mode);

        //owners get to set date of dry off, employees its only today. 
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'DateEstimateDryOff', null, array('options' => $dateArray, 'label' => 'Time of Scheduled Dry Off'))->addRule("required", "Please specify your time.");
            $form->addElement('checkbox', 'sort', array(), array('label' => 'Sort At Scheduled Dry Off'));
            $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('sort' => true))); //set defaults
        }

        return $form; //required       
    }

    function process($value) {

        $lactation_id = $value['animalSelected']; //not actually bovine_id, but lactation_id
        //use selected time for owner and NOW for employee
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $estimated_time = date('Y-m-d H:i:sO', $value['DateEstimateDryOff']);
        } else {
            $estimated_time = date('Y-m-d H:i:sO', strtotime("now"));
        }

        //don't put id value, uses next one in DB.
        $query = "INSERT INTO bovinemanagement.dryoff_schedule (lactation_id,event_time,userid) VALUES ($lactation_id,'$estimated_time','{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);

        //when sort checked.
        if ($value['sort'] == 1) {
            //find bovine_id from lactation_id
            $sql2 = "SELECT bovine_id FROM bovinemanagement.lactationcurr WHERE lactation_id=$lactation_id LIMIT 1";
            $res2 = $GLOBALS['pdo']->query($sql2);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $bovine_id = $row2['bovine_id'];
            //schedule sort
            (new MovementSortGate);//hack for class load
            $a = new QF2_SortGate((object) array('forwardURLArr' => array()), 'individual'); //just so we can call sortAnimal. bit messy.
            $a->sortAnimal($bovine_id, $estimated_time);
        }
    }

    public static function listofLacatingCows() {

        //grab a list of cows with fresh dates
        $sql = "SELECT  lactation.id as lactation_id,bovinecurr.id as bovinecurr_id,bovinecurr.fresh_date,bovinecurr.local_number,bovinecurr.full_name, date_trunc('day',current_date-bovinecurr.fresh_date) as dim  from bovinemanagement.lactation  join bovinemanagement.bovinecurr on bovine_id=bovinecurr.id
where lactation.dry_date is null";
        $res = $GLOBALS['pdo']->query($sql);
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row['lactation_id']] = $row['local_number'] . " - " . $GLOBALS['MiscObj']->femaleShortName($row['full_name']) . " (" . $row['dim'] . ")";
        }
        return $aniNumber;
    }

}

//end class