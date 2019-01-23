<?php




include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');

;

class MedicineScheduled extends TabPage {
function __construct() {
    $this->customSQL();
}
    
    function customSQL() {
//create some temp tables to speed things up. //needed because code that is used in medical case needs it too.
        $sql = "CREATE TEMP TABLE IF NOT EXISTS temp_medical_summary AS  (SELECT * from bovinemanagement.medical_summary)";
        $res = $GLOBALS['pdo']->query($sql);
    }

    public function defaultDisplay() {
        ini_set('display_errors', 1);
        ini_set('display_startup_errors', 1);
     
//setup for tabs.
        $tabArray[1]['name'] = 'Multi-Date Schedule';
        $tabArray[2]['name'] = 'Single Date Schedule';
        $tabArray[3]['name'] = 'Historical Log';

        $this->renderTabs($tabArray);
    }

    function tab1() {
        $this->genericScheduledCalendarTreatmentQuickForm();
        $this->createEpochCalendar();
        $this->displayAllScheduledMedicines('default');
    }

    function tab2() {
        $this->genericScheduledTreatmentQuickForm();
         $this->displayAllScheduledMedicines('default');
    }

    function tab3() {
        print('<h3>Historical Scheduled Medicines</h3>');
         $this->displayAllScheduledMedicines('historical');
    }

    private function createEpochCalendar() {

        print("<link rel='stylesheet' type='text/css' href='/jscript/epochCalendar/epoch_styles.css' />" . "\n\r");
        print("<script type='text/javascript' src='/jscript/epochCalendar/epoch_classes.js'></script>" . "\n\r");


        print("
<script type='text/javascript'>
var ms_cal;      
$(document).ready(function(){ 
	ms_cal  = new Epoch('epoch_multi','flat',document.getElementById('epoch_calendar'),true);
	var datesArray=new Array();" .
                $this->getDatesForCalendarRefresh()
                . "
	//datesArray[99]=new Date(2014,2,3); //example
	//ms_cal.resetSelections;
	//ms_cal.selectDates(datesArray,true);
	//
	//ms_cal.reDraw();
	
	//onclick function for submit button to get dates selected in calendar and store in input var calendar_dates.
	$(function() {          
                $('#genericScheduledTreatmentQuickForm_submit').click(function(){
	            var str = ms_cal.selectedDates; //get all slected dates.            
	            $('#calendar_dates').attr('value', str); //put them in hidden form value.
	         })
	 });
});

//run when form is submitted
//this sends the date in the calendar as a post.
function onFormSubmission()
{
ms_cal.sendForm('genericScheduledCalendarTreatmentQuickForm','calendardates');
}
</script>
");
    }

//check if post variable calendar dates has dates in it or not. if it does, repopulate calendar with these dates.
    private function getDatesForCalendarRefresh() {
        $count = 0;
        $str = "";
        isset($_REQUEST['calendardates']) ? $_REQUEST['calendardates'] : null;
        if (!empty($_REQUEST['calendardates'])) {
            foreach ($_REQUEST['calendardates'] as &$value) {
                $dateUnix = strtotime($value);
                $year = date('Y', $dateUnix);
                $month = date('n', $dateUnix) - 1; //the minus one is to fix the stupidity of months in the calendar app.
                $day = date('j', $dateUnix);
                $str = $str . ("datesArray[$count]=new Date($year,$month,$day);");
                $count++;
            }
        }
        return $str;
    }

    /**     * ********************************************* */
    /**    Calendar drug scheduling of treatment      * */

    /**     * ********************************************* */
    function genericScheduledCalendarTreatmentQuickForm($mode = 'group', $bovine_id = null) {


//grab a list of cows 
        if ($mode == 'group') {
            $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveCows();
        }

//grab a list of Medicines'.
        $medArray[null] = "Please Select one..."; //dummy value
        $dosageArray[null] = ""; //dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine  ORDER BY trade_name");

        while (($row = $res->fetch())) {
            $medArray[$row[0]] = $row[1];
            $dosageArray[$row[0]] = $row[3];
            $defaultDosageArray[$row[0]] = $row[2];
            $defaultBodyLocationArray[$row[0]] = $row[4];
        }

//create javascript to change  dosage to default dosage based on a select statement.
        print('<script type="text/javascript">
			var aryContent = new Array();
			var aryContent2 = new Array();
			var aryContent3 = new Array();');

        foreach ($dosageArray as $key => $value) {
            print("aryContent['$key'] = '<b>$value</b>';");
        }
        foreach ($defaultDosageArray as $key => $value) {
            print("aryContent2['$key'] = '$value';");
        }
        foreach ($defaultBodyLocationArray as $key => $value) {
            print("aryContent3['$key'] = '$value';");
        }

        print("\n\r" . '
			function updateContent(obj){
			        /* this does a div that acts like a trailing label */
				var objContent = document.getElementById("dosage");
				objContent.innerHTML = aryContent[ obj[obj.selectedIndex].value ];
				/* this does the form value */
				var objContent2 = document.getElementById("dosageval");
				objContent2.value = aryContent2[ obj[obj.selectedIndex].value ];
				/* now do the default body location */
				var objContent3 = document.getElementById("bodylocation");
				objContent3.value =  aryContent3[ obj[obj.selectedIndex].value ];
				
			}
  </script>
' . "\n\r");


//locations
        $locationArray = MedicalCase::medicalBodyLocations();

//times per day
        $timesDay[null] = "Please Select one..."; //dummy value
        $timesDay['1a'] = '1X (am)';
        $timesDay['1p'] = '1X (pm)';
        $timesDay['2'] = '2X';
        $timesDay['3'] = '3X';

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForNextThirtyDays();
        unset($timeArray[null]); //remove null value.

        $form = new HTML_QuickForm("genericScheduledCalendarTreatmentQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'genericScheduledTreatmentQuickForm', "Schedule Treatment(s)");
//
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);


        if ($mode == 'group') {
            $form->addElement('select', 'animalNumber', 'Animal', $aniNumber);
        } else {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        }

        $form->addElement('select', 'medicine', 'Medicine', $medArray, array('onchange' => "updateContent(this);"));
        $form->addElement('html', '<div id=\'dosage\'></div>'); //changed via javascript
        $form->addElement('text', 'dosage', 'Dosage', 'size="5" maxlength="5" id="dosageval"');
        $form->addElement('select', 'location', 'Location', $locationArray, 'id="bodylocation"');
        $form->addElement('hidden', 'calendar_dates', '');
// $form->addElement('select', 'event_time', 'Event Time', $timeArray);
        $form->addElement('html', "<br/><tr><td align=\"right\" valign=\"top\"><b>Treatement Date(s)</b></td> <td valign=\"top\" align=\"left\">	<div id='epoch_calendar'></div></td>	</tr>");
        $form->addElement('select', 'times_day', 'Times Per Day', $timesDay);


//submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit', array('id' => 'genericScheduledTreatmentQuickForm_submit')); //, array('onclick' => "alert(ms_cal.selectedDates);")
        $form->addGroup($submitGroup, 'submitGroup');

//rules
        if ($mode == 'group') {
            $form->addRule('animalNumber', 'Animal number is required', 'required');
        }
        $form->addRule('medicine', 'Medicine is required', 'required');
        $form->addRule('location', 'Body Location is required', 'required');
        $form->addRule('times_day', 'Times per day is required', 'required');
        $form->addRule('calendar_dates', 'Dates selected from the calendar are required', 'required'); //FIXME: can't see error, because it is hidden element. make custom rule.
//$form->addRule('event_time', 'Event Time is required', 'required');
        $form->addRule('dosage', 'Enter a valid number for dosage.', 'numeric');


// Try to validate a form
        if ($form->validate()) {
            
//get values
            $bovine_id = $form->exportValue('animalNumber');
            $medicine_id = $form->exportValue('medicine');
            $dosage = $form->exportValue('dosage');
            $location = $form->exportValue('location');
            $times_day = $form->exportValue('times_day');
            $pageid = $form->exportValue('pageid');


//lookup medicine default dosage and use it for now.
            $res = $GLOBALS['pdo']->query("SELECT default_dosage_unit,trade_name FROM bovinemanagement.medicine WHERE id=$medicine_id LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $default_dosage_unit = $row['default_dosage_unit'];
            $trade_name = $row['trade_name'];

//lookup cow's name and number
            $res2 = $GLOBALS['pdo']->query("SELECT full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1");
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $short_name = $GLOBALS['MiscObj']->femaleShortName($row2['full_name']);
            $local_number = $row2['local_number'];

//lookup bovine id and see if there is an open medical case during the requested event_time
//$medical_case_id=self::lookupOpenMedicalCaseForAnimal($bovine_id,$event_time); 
            $medical_case_id = 'null'; //NOTE: FIXME implement in future.
//get dates selected from calendar
            $calendar_dates = $form->exportValue('calendar_dates');
//now tokenize based it and based on how many times a day to give medicine, create an array with all that info.
            $datesArr = (explode(',', $calendar_dates));
            $dateArrDB = array();
            
            //print_r2($form);
            //exit();
            
            foreach ($datesArr as $key => $value) {
                
                $dateString = preg_replace("/\([^)]+\)/","",$value); //remove the (Pacific Standard Time) portion
                if (strtotime($dateString) != false) {
                    $prefix = date('Y-m-d', strtotime($dateString));
//now look how many times per day and create based on that.
                    if ($times_day == '1a') {
                        $dateArrDB[] = date('r', strtotime($prefix . ' 08:00:00'));
                    }
                    if ($times_day == '1p') {
                        $dateArrDB[] = date('r', strtotime($prefix . ' 20:00:00'));
                    }
                    if ($times_day == '2') {
                        $dateArrDB[] = date('r', strtotime($prefix . ' 08:00:00'));
                        $dateArrDB[] = date('r', strtotime($prefix . ' 20:00:00'));
                    }
                    if ($times_day == '3') {
                        $dateArrDB[] = date('r', strtotime($prefix . ' 08:00:00'));
                        $dateArrDB[] = date('r', strtotime($prefix . ' 14:00:00'));
                        $dateArrDB[] = date('r', strtotime($prefix . ' 20:00:00'));
                    }
                }else {
                    throw new Exception("Error No Date given or improper format.<br/>");
                }
            }


//do an insert for each date array.......
            foreach ($dateArrDB as $key => $value) {
                $event_time = null;
                $event_time = $value;
                $this->insertScheduledSingleDoseSingleTime($bovine_id, $short_name, $local_number, $medicine_id, $trade_name, $event_time, $dosage, $default_dosage_unit, $location, $medical_case_id);
            }

//forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    /**
     * private function to do scheduled medical insert of single dose at a singe time.
     * does google calendar and scheduled meds.
     */
    private function insertScheduledSingleDoseSingleTime($bovine_id, $short_name, $local_number, $medicine_id, $trade_name, $event_time, $dosage, $default_dosage_unit, $location, $medical_case_id) {

        try {
            $res = $GLOBALS['pdo']->beginTransaction();  // Open a transaction
//insert into google calendar
            $title = 'Treat: ' . $local_number . ' - ' . $short_name . ' with ' . $trade_name . ' ' . $dosage . '' . $default_dosage_unit . ' ' . $location;
            $description = $title;
            $google_calendar_event_id = $this->insertGoogleCalendarScheduledTreatment($event_time, $title, $description);
            $google_calendar_event_id_escaped = pg_escape_string($google_calendar_event_id);

//administer drug
            $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,scheduled_event_time,dosage,dosage_unit,location,note,medical_case_id,scheduled_userid,calendar_event_id)
	  VALUES ($bovine_id,$medicine_id,'$event_time','$dosage','$default_dosage_unit','$location',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}','$google_calendar_event_id_escaped')";
            $res = $GLOBALS['pdo']->exec($query);

// determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (PDOException $e) {
            try {
                $GLOBALS['pdo']->rollBack();
            } catch (Exception $e2) {
                
            }
            //rollback google calendar insert.
            if (!empty($google_calendar_event_id)) {
                self::deleteGoogleCalendarScheduledTreatment($google_calendar_event_id);
            }
            echo "Failed: " . $e->getMessage();
        }
        // Close the transaction
// Close the transaction
    }

    /**     * ****************************************** */
    /** Display list of all scheduled medicine      */

    /**     * ****************************************** */
    function displayListOfScheduledMedecine($mode = 'default') {

        if ($mode == 'default') {
            
        }
        if ($mode == 'historical') {
             $out[]=('<h3>Historical Scheduled Medicines</h3>');
        }


//show any medicine that is currently scheduled or ever was.
        $sql = "SELECT medicine_administered.id as id, bovineall.id as bovine_id,bovineall.location_name,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  WHERE event_time is null
  ORDER BY medicine_administered.scheduled_event_time ASC ";

        if ($mode == 'historical') {
//show any medicine that is currently scheduled or ever was.
            $sql = "SELECT medicine_administered.id as id, bovineall.id as bovine_id,bovineall.location_name,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  WHERE scheduled_event_time is not null AND event_time is not null
  ORDER BY medicine_administered.scheduled_event_time DESC";
        }

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            
              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
              $rowOut['Location (Group)'] = ($row['location_name']);
              $rowOut['Which Foot'] = ($row['trade_name']);
              $rowOut['Dosage'] = ($row['dosage']);
              $rowOut['Dosage Unit'] = ($row['dosage_unit']);
              $rowOut['Location'] = ($row['location']);
              $rowOut['Scheduled Userid'] = ($row['scheduled_userid']);
              $rowOut['Scheduled Time'] = JQueryDataTable::formattedDate($row['event_time2']);
              $rowOut['Administered userid'] = ($row['userid']);
              $rowOut['Administered Time'] = JQueryDataTable::formattedDate($row['event_time']);
                if ($mode == 'default') {
                     $rowOut['Action'] = $this->printActionButtons($medicine_administered_id, $medicine_administered_close_date,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
                     $rowOut['Action2'] = $this->printActionButtonMedicalScheduledComplete($medicine_administered_id, $medicine_administered_close_date, $medicine_administered_bovine_id,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }
             

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
        
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'order: false')); 
         return implode($out); 
   
    }



    /**     * *********************************************************
     * completed button for scheduled medicine 
     * ********************************************************** */
    function completedScheduledMedicineButtonQuickForm($tableid, $tablename, $bovine_id,$callBackURL) {
    
        $form = new HTML_QuickForm("completedButtonQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('hidden', 'bovine_id', $bovine_id);
        $form->addElement('submit', 'btnCompleted', 'Completed');

//FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY USER.
//DB hack works though, views need to support deletes before this can be fixed.
// Try to validate a form
        if ($form->validate()) {

//get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');
            $bovine_id = $form->getElementValue('bovine_id');

//use current time and userid for values.
            $event_time = date('r', strtotime(date('Y-m-d H:00:00', date(strtotime("now")))));
            $userid = $GLOBALS['auth']->getUsername();

//now we have to find the medical case id for this bovine. We do not know it, because when the sheduled medicine was put in there wasn't necessarily an open case then.
//Does this make sense.  It might not actuially be associted with the medical case.
//lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

// Open a transaction
//update event to say it is completed.
            $res = $GLOBALS['pdo']->query("Update bovinemanagement.$tablename SET event_time='$event_time', userid='$userid',medical_case_id=$medical_case_id  WHERE id='$tableid'");

            /* need to lookup medical action here for same scheduled time as medicine and mark completed. */
            /* FIXME procol_uuid and scheduled event_time and action equal, then mark action completed.*/
            
            
            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

        /**     * *********************************************************
     * see whether to print completed button for scheduled medicine 
     * ********************************************************** */
    function printActionButtonMedicalScheduledComplete($medicine_administered_id, $medicine_administered_close_date, $medicine_administered_bovine_id,$callBackURL = null) {
       // extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();

//select all scheduled medicines that will occur and allow those to be selected.
        $res = $GLOBALS['pdo']->query("SELECT id,tablename,bovine_id,event_time FROM temp_medical_summary WHERE type='Scheduled Meds' AND bovine_id={$medicine_administered_bovine_id}");

        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//only show complete button for those that are not completed and will occur in the next 10 hours OR PAST. 
            if (($medicine_administered_id == $row['id']) && (empty($medicine_administered_close_date)) && (strtotime($row['event_time']) <= strtotime('+10 hour'))) {
                $returnButtonsAsString = self::completedScheduledMedicineButtonQuickForm($row['id'], $row['tablename'], $row['bovine_id'],$callBackURL); //show complete button
            }
        }

//print button
//$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }
    
    
    /**     * *****************************************
     * Show Delete Button.
     * ******************************************* */
    function printActionButtons($medicine_administered_id, $medicine_administered_close_date, $callBackURL = null) {
        //extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();

//find the last entry for the current user and use that as a basais to display the delete button or not.
//We only want the delete button shown once for the latest entry.
//allow any medicine that has not yet been given to be deleted by management.
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $sql = "SELECT id,'medicine_administered' as tablename FROM bovinemanagement.medicine_administered WHERE userid IS NULL ORDER BY update_time DESC";
        } else { //only allow the last one to be deleted for normal users.
            $sql = "SELECT id,'medicine_administered' as tablename FROM bovinemanagement.medicine_administered WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1";
        }
        $res = $GLOBALS['pdo']->query($sql);
        $returnButtonsAsString = '';
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if (($medicine_administered_id == $row['id']) && ($medicine_administered_close_date == null)) {
                $returnButtonsAsString = self::deleteQuickForm($row['id'], $row['tablename'],$callBackURL); //show delete button, with Breeding table id as index for delete.
            }
        }

//print button
//$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    /**     * ******************************************
     * Delete Button Quickform
     * ******************************************** */
    function deleteQuickForm($tableid, $tablename,$callBackURL) {

        $form = new HTML_QuickForm("medSchedDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete?')")); //call javascript to confirm delete.
//FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY USER.
//DB hack works though, views need to support deletes before this can be fixed.
// Try to validate a form
        if ($form->validate()) {

//get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');

// Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

//do google calendar delete.
                $sql = "SELECT calendar_event_id FROM bovinemanagement.$tablename WHERE id='$tableid' LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);

                $row2 = $res2->fetch(PDO::FETCH_ASSOC);
                self::deleteGoogleCalendarScheduledTreatment($row2['calendar_event_id']);

//delete the event
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.$tablename where id='$tableid'");

// determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (PDOException $e) {
                try {
                    $GLOBALS['pdo']->rollBack();
                } catch (Exception $e2) {
                    
                }

                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction



            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    
    /**
     * used for things like repro protocols where we are scheduling a default action ,like CIDR removal.
     * uses protocol_uuid, do not use for scheduleing
     */
    public function protocolMedicalActionDefaultInsert($bovine_id, $event_time, $medical_action_type_id, $uuid = 'null') {

// Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

//assume not associated with a medical case
            $medical_case_id = 'null';


//lookup medicine default dosage and use it for now.
            $res = $GLOBALS['pdo']->query("SELECT id,action FROM bovinemanagement.medical_action_type WHERE id=$medical_action_type_id LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
           

//lookup cow's name
            $res2 = $GLOBALS['pdo']->query("SELECT full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1");
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);

//insert into google calendar
            $title = 'Action: ' . $row2['local_number'] . ' do ' . $row['action'];
            $description = $title;
            $google_calendar_event_id = self::insertGoogleCalendarScheduledTreatment($event_time, $title, $description);
            $google_calendar_event_id_escaped = pg_escape_string($google_calendar_event_id);


//insert action
            
            
            $query = "INSERT INTO bovinemanagement.medical_action (bovine_id,medical_action_type_id,comment,userid,event_time,scheduled_event_time,medical_case_id,scheduled_userid,calendar_event_id,protocol_uuid)
	  VALUES ($bovine_id,$medical_action_type_id,null,null,null,'$event_time',$medical_case_id,'{$GLOBALS['auth']->getUsername()}','$google_calendar_event_id_escaped','$uuid')";
            $res = $GLOBALS['pdo']->exec($query);

            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (PDOException $e) {
            try {
                $GLOBALS['pdo']->rollBack();
            } catch (Exception $e2) {
                
            }
            //rollback google calendar insert.
            if (!empty($google_calendar_event_id)) {
                self::deleteGoogleCalendarScheduledTreatment($google_calendar_event_id);
            }
            echo "Failed: " . $e->getMessage();
        }
        // Close the transaction
    }
    
    
    /**
     * used for things like repro protocols where we are scheduling a default thing.
     * uses protocol_uuid, do not use for scheduleing
     */
    public function protocolMedicineDefaultInsert($bovine_id, $event_time, $medicine_id, $uuid = 'null', $dosage_overide = null) {

// Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

//assume not associated with a medical case
            $medical_case_id = 'null';

//lookup medicine default dosage and use it for now.
            $res = $GLOBALS['pdo']->query("SELECT default_dosage,default_body_location,default_dosage_unit,trade_name FROM bovinemanagement.medicine WHERE id=$medicine_id LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $default_dosage_unit = $row['default_dosage_unit'];
            $dosage = $row['default_dosage'];
            $location = $row['default_body_location'];

//allow us to schedule different then the default dosage.
            if ($dosage_overide != null) {
                $dosage = $dosage_overide;
            }

//lookup cow's name
            $res2 = $GLOBALS['pdo']->query("SELECT full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1");
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);

//insert into google calendar
            $title = 'Treat: ' . $row2['local_number'] . ' with ' . $row['trade_name'] . ' ' . $dosage . '' . $default_dosage_unit . ' ' . $location;
            $description = $title;
            $google_calendar_event_id = self::insertGoogleCalendarScheduledTreatment($event_time, $title, $description);
            $google_calendar_event_id_escaped = pg_escape_string($google_calendar_event_id);


//administer drug
            $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,scheduled_event_time,dosage,dosage_unit,location,note,medical_case_id,scheduled_userid,calendar_event_id,protocol_uuid)
	  VALUES ($bovine_id,$medicine_id,'$event_time','$dosage','$default_dosage_unit','$location',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}','$google_calendar_event_id_escaped','$uuid')";
            $res = $GLOBALS['pdo']->exec($query);

            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (PDOException $e) {
            try {
                $GLOBALS['pdo']->rollBack();
            } catch (Exception $e2) {
                
            }
            //rollback google calendar insert.
            if (!empty($google_calendar_event_id)) {
                self::deleteGoogleCalendarScheduledTreatment($google_calendar_event_id);
            }
            echo "Failed: " . $e->getMessage();
        }
        // Close the transaction
    }

    /**
     * used for things like repro protocols where we are scheduling a default thing.
     */
    public function protocolMedicineDefaultDelete($uuid) {

        //NOTE: this is only called by protocol class. thus we know it is in a DB transaction already.

//only allow us to delete ones that have not yet been administered.
            $res = $GLOBALS['pdo']->query("SELECT id,calendar_event_id FROM bovinemanagement.medicine_administered WHERE protocol_uuid='$uuid' AND event_time is null");
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $res2 = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['id']}'");
                self::deleteGoogleCalendarScheduledTreatment($row['calendar_event_id']);
            }

//NOTE: if this fails then the google calendar deletes will have been performed. No good way to fix this without testing for google calendar failures too. lots of programming for little gain.

    }

    /**
     * Store medicine scheduled treatment in google calendar
     */
    private function insertGoogleCalendarScheduledTreatment($event_time, $title, $description) {
//convert event time to time we can use.
        $startDate = date('Y-m-d', strtotime($event_time));
        $startTime = date('H:i:00', strtotime($event_time));

//create an event to store in bovine_treatment google calendar.

        try {
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/googleCalendar.inc');
            $gCal = new GoogleCalendar('littleriver.ca_phrs87e7efrsf6qc0hk4bfk88g@group.calendar.google.com'); //bovine_treatement
            $eventId = null;
            $eventId = $gCal->createEvent($gCal->calendarID, $title, $description, '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $startTime, '-04');
        } catch (Exception $e) {
            echo 'Caught Google Calendar exception and bypassing: ', $e->getMessage(), "\n";
            return 'NOTINCAL_' . uniqid();
        }


        if ($eventId == null) {
            throw new Exception("Error: Google Calendar insert of $title FAILED!<br/>");
        } else {
            return $eventId;
        }
    }

    /**
     * Delete medicine scheduled treatment in google calendar
     */
    private function deleteGoogleCalendarScheduledTreatment($eventId) {

        if (empty($eventId)) {
            throw new Exception('Error: No Eventid!');
        }

        try {
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/googleCalendar.inc');
            $gCal = new GoogleCalendar('littleriver.ca_phrs87e7efrsf6qc0hk4bfk88g@group.calendar.google.com'); //bovine_treatement
            print("<br>starting delete<br>");
            $ret = null;
            $ret = $gCal->deleteEvent($gCal->calendarID, $eventId);
        } catch (Exception $e) {
            echo 'Caught Google Calendar exception and bypassing: ', $e->getMessage(), "\n";
            return null;
        }

        if ($ret == null) {
            throw new Exception("Error: Google Calendar delete of $eventId FAILED!<br/>");
        } else {
            return true;
        }
    }

    /**     * ****************************************** */
    /** Mini View scheduled medicine      */

    /**     * ****************************************** */
    function miniViewOfScheduledMedecine() {
        
        
          $sql = <<<SQL
SELECT medicine_administered.id as id, bovineall.id as bovine_id,local_number,full_name,dosage || dosage_unit || ' ' ||trade_name as textstr,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  WHERE scheduled_event_time is not null AND event_time IS NULL
  ORDER BY medicine_administered.scheduled_event_time ASC  LIMIT 10
SQL;

        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {

              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
              $rowOut['Trade Name'] = ($row['textstr']);
              $rowOut['Scheduled Time'] = JQueryDataTable::formattedDate($row['event_time2']);
             
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Scheduled Medicines</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'order: false')); 
         return implode($out); 

    }

    /**     * ********************************************* */
    /**    Generic drug scheduling of treatment      * */

    /**     * ********************************************* */
    function genericScheduledTreatmentQuickForm($mode = 'group', $bovine_id = null) {


//grab a list of cows 
        if ($mode == 'group') {
            $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveCows();
        }

//grab a list of Medicines'.
        $medArray[null] = "Please Select one..."; //dummy value
        $dosageArray[null] = ""; //dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE for_mastitis!=true ORDER BY trade_name");

        while (($row = $res->fetch())) {
            $medArray[$row[0]] = $row[1];
            $dosageArray[$row[0]] = $row[3];
            $defaultDosageArray[$row[0]] = $row[2];
            $defaultBodyLocationArray[$row[0]] = $row[4];
        }

//create javascript to change  dosage to default dosage based on a select statement.
        print('<script type="text/javascript">
			var aryContent = new Array();
			var aryContent2 = new Array();
			var aryContent3 = new Array();');

        foreach ($dosageArray as $key => $value) {
            print("aryContent['$key'] = '<b>$value</b>';");
        }
        foreach ($defaultDosageArray as $key => $value) {
            print("aryContent2['$key'] = '$value';");
        }
        foreach ($defaultBodyLocationArray as $key => $value) {
            print("aryContent3['$key'] = '$value';");
        }

        print("\n\r" . '
			function updateContent(obj){
			        /* this does a div that acts like a trailing label */
				var objContent = document.getElementById("dosage");
				objContent.innerHTML = aryContent[ obj[obj.selectedIndex].value ];
				/* this does the form value */
				var objContent2 = document.getElementById("dosageval");
				objContent2.value = aryContent2[ obj[obj.selectedIndex].value ];
				/* now do the default body location */
				var objContent3 = document.getElementById("bodylocation");
				objContent3.value =  aryContent3[ obj[obj.selectedIndex].value ];
				
			}
  </script>
' . "\n\r");


//locations
        $locationArray = MedicalCase::medicalBodyLocations();

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForNextThirtyDays();
        unset($timeArray[null]); //remove null value.

        $form = new HTML_QuickForm("genericScheduledTreatmentQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'genericScheduledTreatmentQuickForm', "Schedule Single Treatment");
//
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        $form->addElement('html', "<tr>	<td align=\"right\" valign=\"top\"><b>Treatement Date(s)</b></td> <td valign=\"top\" align=\"left\">	<div id='epoch_calendar'></div></td>	</tr>");

        if ($mode == 'group') {
            $form->addElement('select', 'animalNumber', 'Animal', $aniNumber);
        } else {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        }

        $form->addElement('select', 'medicine', 'Medicine', $medArray, array('onchange' => "updateContent(this);"));
        $form->addElement('html', '<div id=\'dosage\'></div>'); //changed via javascript
        $form->addElement('text', 'dosage', 'Dosage', 'size="5" maxlength="5" id="dosageval"');
        $form->addElement('select', 'location', 'Location', $locationArray, 'id="bodylocation"');
        $form->addElement('select', 'event_time', 'Event Time', $timeArray);

//submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

//rules
        if ($mode == 'group') {
            $form->addRule('animalNumber', 'Animal number is required', 'required');
        }
        $form->addRule('medicine', 'Medicine is required', 'required');
        $form->addRule('location', 'Body Location is required', 'required');
        $form->addRule('event_time', 'Event Time is required', 'required');
        $form->addRule('dosage', 'Enter a valid number for dosage.', 'numeric');

// Try to validate a form
        if ($form->validate()) {

//get values
            $bovine_id = $form->exportValue('animalNumber');
            $medicine_id = $form->exportValue('medicine');
            $dosage = $form->exportValue('dosage');
            $location = $form->exportValue('location');
// 
            $tempArray = $form->getElementValue('event_time');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');

// Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

//lookup bovine id and see if there is an open medical case during the requested event_time
//$medical_case_id=self::lookupOpenMedicalCaseForAnimal($bovine_id,$event_time); 
                $medical_case_id = 'null';

//lookup medicine default dosage and use it for now.
                $res = $GLOBALS['pdo']->query("SELECT default_dosage_unit,trade_name FROM bovinemanagement.medicine WHERE id=$medicine_id LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $default_dosage_unit = $row['default_dosage_unit'];

//lookup cow's name
                $res2 = $GLOBALS['pdo']->query("SELECT full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1");
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);

//insert into google calendar
                $title = 'Treat: ' . $row2['local_number'] . ' with ' . $row['trade_name'] . ' ' . $dosage . '' . $row['default_dosage_unit'] . ' ' . $location;
                $description = $title;
                $google_calendar_event_id = $this->insertGoogleCalendarScheduledTreatment($event_time, $title, $description);
                $google_calendar_event_id_escaped = pg_escape_string($google_calendar_event_id);


//administer drug
                $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,scheduled_event_time,dosage,dosage_unit,location,note,medical_case_id,scheduled_userid,calendar_event_id)
	  VALUES ($bovine_id,$medicine_id,'$event_time','$dosage','$default_dosage_unit','$location',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}','$google_calendar_event_id_escaped')";
                $res = $GLOBALS['pdo']->exec($query);


                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (PDOException $e) {
                try {
                    $GLOBALS['pdo']->rollBack();
                } catch (Exception $e2) {
                    
                }
                //rollback google calendar insert.
                if (!empty($google_calendar_event_id)) {
                    self::deleteGoogleCalendarScheduledTreatment($google_calendar_event_id);
                }
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction
// Close the transaction
//forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

//shows error.
    function error() {

        $error = array();

////////////////////////
//looks to see if we missed a hormone shot or it is needed in the next 8 hours.
//DOES NOT CHECK FOR OTHER DRUGS!
        $sql = "SELECT medicine_administered.id as id, bovineall.id as bovine_id,local_number,full_name,dosage || dosage_unit || ' ' ||trade_name as textstr,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  WHERE scheduled_event_time is not null AND event_time IS NULL AND (scheduled_event_time <= current_timestamp +interval '8 hours' OR scheduled_event_time <= current_timestamp) AND medicine_class='hormone'
  ORDER BY medicine_administered.scheduled_event_time ASC  
	     ";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error["{$row['local_number']}"] = new notifyObj(get_class($this), $row['event_time2'], 1, $row['local_number'] . ' needs ' . $row['textstr'] . '.');
        }

        return $error;
    }

    public function AllScheduledMedicines($request) {
        $mode = Sanit::reqVal('item', false, 'STR');

        if ($mode == 'default') {
            //show any medicine that is currently scheduled or ever was.
            $sql = "SELECT medicine_administered.id as id, bovineall.id as bovine_id,bovineall.location_name,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  WHERE event_time is null
  ORDER BY medicine_administered.scheduled_event_time ASC ";
        } elseif ($mode == 'historical') {
            //show any medicine that is currently scheduled or ever was.
            $sql = "SELECT medicine_administered.id as id, bovineall.id as bovine_id,bovineall.location_name,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  WHERE scheduled_event_time is not null AND event_time is not null
  ORDER BY medicine_administered.scheduled_event_time DESC";
        } else {
            throw new Exception("Eroror: improper request value, must be default or historical.");
        }


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time2']);


            $row['event_time2'] = date("l M d Y H:00", strtotime($row['event_time2']));  //has to be at end.
             $row['action'] ='';
             $row['action2'] ='';
              
            if ($mode == 'default') {
                $row['action'] = $this->printActionButtons($row['id'], $row['close_date'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
                $row['action2'] = $this->printActionButtonMedicalScheduledComplete($row['id'],$row['close_date'],$row['bovine_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            }

          
        }

        //output
        $output['data'] = $results;
        return ($output);



       
    }

    function displayAllScheduledMedicines($mode) {
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
                data: "location_name",
                title: "Current Location"
            },
         {
                data: "trade_name",
                title: "Trade Name"
            },
            {
                data: "dosage",
                title: "Dosage"
        
            },
         {
                data: "dosage_unit",
                title: "Dosage Unit"
        
            },
        {
                data: "location",
                title: "Body Location"
        
            },
        
            {
                data: "event_time2",
                title: "Scheduled Time",
                type: "date"
            },
            {
                data: "scheduled_userid",
                title: "Shceduler Employee"
            },
             {
                data: "action",
                title: "Action"
            },
            {
                data: "action2",
                title: "Action 2"
            }
EX;
        print($dt->start(79, 'AllScheduledMedicines', $mode, $columnsStr,null,true,"'order': [[0,'asc']],"));

        //    print($dt->startLocal($this->AllHeats7Days(''),$columnsStr,null,' "paging": false'));     
    }

}

//end class
?>