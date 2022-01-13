<?php

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


//setup for tabs.
        $tabArray[1]['name'] = 'General';
        $tabArray[2]['name'] = 'Medical Protocol';
        $tabArray[3]['name'] = 'Calf Medical Protocol';
        $tabArray[4]['name'] = 'Historical Log';
        $tabArray[5]['name'] = 'Medical Action';

        $this->renderTabs($tabArray);
    }

    function tab1() {
        $this->param = new stdClass();
        $this->param->aniNumber = null;
        $this->param->bovineID = null;
        $QF2_ScheduleSingleGeneralTreatment = new QF2_ScheduleSingleGeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => null)), 'group', $this->param); //group



        $left = <<<HTML
          {$this->displayAllScheduledMedicines('default')}
        HTML;

        $right = <<<HTML
            {$QF2_ScheduleSingleGeneralTreatment->renderBox('QF2_ScheduleSingleGeneralTreatment', 'Schedule Treatment')}
            {$this->genericScheduledCalendarTreatmentQuickForm()}
            {$this->createEpochCalendar()}
        HTML;
        print (new Bootstrap)->oneThirdTwoThirds($left, $right);
    }

    function tab2() {

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusProtocol.inc');
        $param = new stdClass();
        $param->type = 'medical';
        $param->aniNumber = null;
        $param->bovineID = null;
        $a = new QF2_Protocol((object) array('forwardURLArr' => (new Quickform2Misc)->forwardURLDefaults()), 'group', $param);
        //
        $b = (new EstrusProtocol);



        $left = <<<HTML
          {$b->displayProtocolEvents('default', 'medical')}
        HTML;

        $right = <<<HTML
            {$a->renderBox('QF2_Medical_Protocol', 'Record Medical Protocol Event')}
        HTML;

        print (new Bootstrap)->oneThirdTwoThirds($left, $right);


        $accordionArray[1]['name'] = 'Ketosis 1.3 to 2.4 mmol';
        $accordionArray[2]['name'] = 'Ketosis 2.5 to 3.5 mmol';

        $accordion = new AccordionImplementationMedicalProtocols();
        print($accordion->render($accordionArray));
    }

    function tab3() {

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusProtocol.inc');
        $param = new stdClass();
        $param->type = 'calf';
        $param->aniNumber = null;
        $param->bovineID = null;
        $a = new QF2_Protocol((object) array('forwardURLArr' => (new Quickform2Misc)->forwardURLDefaults()), 'group', $param);
        //
        $b = (new EstrusProtocol);

        $left = <<<HTML
           {$b->displayProtocolEvents('default', 'calf')}
        HTML;

        $right = <<<HTML
             {$a->renderBox('QF2_Calf_Protocol', 'Record Medical Calf Protocol Event')}
        HTML;

        print (new Bootstrap)->oneThirdTwoThirds($left, $right);
    }

    function tab4() {
        print('<h3>Historical Scheduled Medicines</h3>');
        print($this->displayAllScheduledMedicinesHistorical('historical'));
    }

    private function createEpochCalendar() {
        $out = array();
        $out[] = ("<link rel='stylesheet' type='text/css' href='/jscript/epochCalendar/epoch_styles.css' />" . "\n\r");
        $out[] = ("<script type='text/javascript' src='/jscript/epochCalendar/epoch_classes.js'></script>" . "\n\r");


        $out[] = ("
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
        return implode($out);
    }

    function tab5() {

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicineScheduled.inc');
        $param = new stdClass();
        $param->aniNumber = null;
        $param->bovineID = null;
        $c = new QF2_ScheduleAction((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', $param); //group

        
        $right=$c->renderBox('QF2_ScheduleAction', 'Select Animals / Groups to Record Medical  Action');
        $left=$this->displayMedicalActions();
                
        print(new BootStrap)->oneThirdTwoThirds($left, $right);
       
    }

    function displayMedicalActions() {


        $sql = <<<SQL
SELECT *,medical_action.id as medical_action_id FROM bovinemanagement.medical_action
LEFT JOIN bovinemanagement.medical_action_type on medical_action_type_id=medical_action_type.id
LEFT JOIN bovinemanagement.bovine on bovine_id=bovine.id         
ORDER BY scheduled_event_time DESC limit 30
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['']= $row['event_time'];
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Scheduled Date'] = JQueryDataTable::formattedDate($row['scheduled_event_time']);
            $rowOut['Med Action'] =$row['action'];
            $rowOut['Action']=(new BovineQueryMedicalLog)->medicalEventsDeleteButton('Scheduled Action',$row['medical_action_id'],'medical_action','bovinemanagement',$row['bovine_id']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayNoSort('Medical Actions Recent Log', $dataArr, ''));
        return implode($out);
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

        $out = array();
//grab a list of cows 
        if ($mode == 'group') {
            $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();
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
        $out[] = ('<script type="text/javascript">
			var aryContent = new Array();
			var aryContent2 = new Array();
			var aryContent3 = new Array();');

        foreach ($dosageArray as $key => $value) {
            $out[] = ("aryContent['$key'] = '<b>$value</b>';");
        }
        foreach ($defaultDosageArray as $key => $value) {
            $out[] = ("aryContent2['$key'] = '$value';");
        }
        foreach ($defaultBodyLocationArray as $key => $value) {
            $out[] = ("aryContent3['$key'] = '$value';");
        }

        $out[] = ("\n\r" . '
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
            //create uuid for db.
            $uuid = $GLOBALS['MiscObj']->uuid_create();
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

                $dateString = preg_replace("/\([^)]+\)/", "", $value); //remove the (Pacific Standard Time) portion
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
                } else {
                    throw new Exception("Error No Date given or improper format.<br/>");
                }
            }


//do an insert for each date array.......
            foreach ($dateArrDB as $key => $value) {
                $event_time = null;
                $event_time = $value;
                $this->insertScheduledSingleDoseSingleTime($bovine_id, $short_name, $local_number, $medicine_id, $trade_name, $event_time, $dosage, $default_dosage_unit, $location, $medical_case_id, $uuid);
            }

//forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation

        $form->accept($renderer);
        $out[] = ($renderer->toHtml());
        return implode($out);
    }

    /**
     * private function to do scheduled medical insert of single dose at a singe time.
     * does google calendar and scheduled meds.
     */
    private function insertScheduledSingleDoseSingleTime($bovine_id, $short_name, $local_number, $medicine_id, $trade_name, $event_time, $dosage, $default_dosage_unit, $location, $medical_case_id, $uuid) {

        try {
            $res = $GLOBALS['pdo']->beginTransaction();  // Open a transaction
//insert into google calendar
            $title = 'Treat: ' . $local_number . ' - ' . $short_name . ' with ' . $trade_name . ' ' . $dosage . '' . $default_dosage_unit . ' ' . $location;
            $description = $title;
            $google_calendar_event_id = $this->insertGoogleCalendarScheduledTreatment($event_time, $title, $description);
            $google_calendar_event_id_escaped = pg_escape_string($google_calendar_event_id);

//administer drug
            $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,scheduled_event_time,dosage,dosage_unit,location,note,medical_case_id,scheduled_userid,calendar_event_id,uuid)
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
            error_log($e->getMessage(), 0);
        }
        // Close the transaction
// Close the transaction
    }

    //jquery version of old datagrid function, keep this one in future
    public function printparticularTreatmentInfoJQ($record) {

        //colour temperature values who are out of the ordinary
        if ($record ['type'] == 'Temperature') {

            $record ['text'] = $this->colourTempNormalOrNot($record ['text']);
        }


        $event_time = date('D M j/Y ga', strtotime($record ['event_time']));
        $str = '<div class=\'smallish\'>' . $event_time . '&nbsp;&nbsp;&nbsp;&nbsp;' . 'by ' . $record ['userid'] . '</div> ' . '<b>' . $record ['type'] . ':</b> ' . $record ['text'];

        $x = (strtotime($record ['event_time']) - strtotime('now'));

        // special case to draw attention to events less than 8 hours away OR
        // events that have passed that are scheduled meds. ANNOYING.
        if ((($x < 28800) and ( $x > 0)) or ( ($x < 0) and( ( $record ['type'] == 'Scheduled Meds') OR ( $record ['type'] == 'Scheduled Meds')) )) {
            $str = '<b id="boldBlue"><b id="boldBlink">' . $str . '</b></b>';
        }
        //if it is more than 8 hours in the future.
        elseif (($x >= 28800) AND (($record ['type'] == 'Scheduled Meds') OR ( $record ['type'] == 'Scheduled Meds'))) {
            $str = '<b id="boldLightBlue">' . $str . '</b>';
        }

        return $str;
    }

    function displayNext8HoursScheduledMedicines($bovine_id = null, $type = 'all') {
        if (!empty($bovine_id)) {
            $sql = "
SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE bovine_id=$bovine_id AND type=ANY(ARRAY['Scheduled Meds','Scheduled Action']) AND event_time <= (current_timestamp + interval '8 hours')	AND event_time >= (current_timestamp - interval '8 hours')	
		ORDER BY medical_summary.event_time ASC
   ";
        } else {
            if ($type == 'calf') { //calf case
                $sql = "
SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type=ANY(ARRAY['Scheduled Meds','Scheduled Action']) AND event_time <= (current_timestamp + interval '8 hours')	AND event_time >= (current_timestamp - interval '8 hours')	
                AND location_id IN(64,65,66,67,68) 
		ORDER BY medical_summary.event_time ASC
   ";
            } else {    //all cases
                $sql = "
SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type=ANY(ARRAY['Scheduled Meds','Scheduled Action']) AND event_time <= (current_timestamp + interval '8 hours')	AND event_time >= (current_timestamp - interval '8 hours')	
		ORDER BY medical_summary.event_time ASC
   ";
            }
        }

        $resultsOut = array();

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $delButton = new QF2_DeleteButtonDecider('user_1only', (object) array('schemaTable' => 'bovinemanagement.medicine_administered'));

        $completeButton = new QF2_CompleteButtonDecider('anyuser_timeLimit', (object) array('dateTimeName' => 'scheduled_event_time', 'buttonClassName' => 'QF2_ScheduleSingleGeneralTreatmentCompleteButton', 'schemaTable' => 'bovinemanagement.medicine_administered'));

        //change presentation
        foreach ($results as &$row) {
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name'], 'display');
            //$rowOut['event_time'] = strtotime($row['event_time']);
            $rowOut['Particular Treatment Info'] = $this->printparticularTreatmentInfoJQ($row);

            $rowOut[' '] = $delButton->output($row['id']) . $completeButton->output($row['id']);


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        //if there is nothing to treat, show nothing.
        if (empty($dataArr)) {
            return '';
        } else {

            return( (new JQueryDataTable)->startBasicArrayNoSort('Scheduled Medications', $dataArr));
        }
    }

    /**     * *********************************************************
     * completed button for scheduled medicine 
     * ********************************************************** */
    function completedScheduledMedicineButtonQuickForm($tableid, $tablename, $bovine_id, $callBackURL) {

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
            $medical_case_id = empty($medical_case_id) ? 'null' : $medical_case_id;

// Open a transaction
//update event to say it is completed.
            $res = $GLOBALS['pdo']->query("Update bovinemanagement.$tablename SET event_time='$event_time', userid='$userid',medical_case_id=$medical_case_id  WHERE id='$tableid'");

            /* need to lookup medical action here for same scheduled time as medicine and mark completed. */
            /* FIXME procol_uuid and scheduled event_time and action equal, then mark action completed. */


            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    /**
     * used for things like repro protocols where we are scheduling a default action ,like CIDR removal.
     * uses protocol_uuid, do not use for scheduleing
     */
    public function protocolMedicalActionDefaultInsert($bovine_id, $event_time, $medical_action_type_id, $protocol_uuid = 'null') {

        if (empty($bovine_id) OR (!is_numeric($bovine_id))) { throw new Exception('Bovine ID not valid.'); }
        if (empty($event_time)) { throw new Exception('Event Time not valid.'); }

        
// Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            //check for mesical case.
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

            $medical_case_id = empty($medical_case_id) ? null : $medical_case_id;

            $protocol_uuid = empty($protocol_uuid) ? null : $protocol_uuid;

//lookup medical_action_type and use it for now.
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
            $names = array('bovine_id', 'medical_action_type_id', 'comment', 'userid', 'event_time', 'scheduled_event_time', 'medical_case_id', 'scheduled_userid', 'calendar_event_id', 'protocol_uuid');
            var_dump($names);
            $blah = $GLOBALS['MiscObj']->createPDOInsertNamesFromArray($names);

//schedule administer drug
            $sqlInsert = <<<SQL
INSERT INTO bovinemanagement.medical_action  ({$blah['column_list']}) VALUES ({$blah['param_list']})
SQL;


            $stmt = $GLOBALS['pdo']->prepare($sqlInsert);
            $stmt->bindValue(':bovine_id', $bovine_id, PDO::PARAM_INT);
            $stmt->bindValue(':medical_action_type_id', $medical_action_type_id, PDO::PARAM_INT);
            $stmt->bindValue(':comment', null, PDO::PARAM_STR);
            $stmt->bindValue(':userid', null, PDO::PARAM_STR);
            $stmt->bindValue(':event_time', null, PDO::PARAM_STR);
            $stmt->bindValue(':scheduled_event_time', $event_time, PDO::PARAM_STR);
            $stmt->bindValue(':medical_case_id', $medical_case_id, PDO::PARAM_INT);
            $stmt->bindValue(':scheduled_userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
            $stmt->bindValue(':calendar_event_id', $google_calendar_event_id_escaped, PDO::PARAM_STR);
            $stmt->bindValue(':protocol_uuid', $protocol_uuid, PDO::PARAM_STR);


            $status = $stmt->execute($param_values);
            //echo( $stmt->debugDumpParams());
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
            error_log($e->getMessage(), 0);
            error_log($stmt->debugDumpParams());
        }
        // Close the transaction
    }

    /*
     * standard delete for a scheduled medicine. 
     */

    public static function protocolMedicineDefaultDeleteByID($tablename, $tableid) {
        var_dump($tablename);
        var_dump($tableid);
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
                //not calendar event is not recreated. this is a bug, but very corner case.
            } catch (Exception $e2) {
                
            }

            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
        // Close the transaction 
    }

    /**
     * used for things like repro protocols where we are scheduling a default thing.
     * uses protocol_uuid, do not use for scheduleing
     */
    public function protocolMedicineDefaultInsert($bovine_id, $event_time, $medicine_id, $protocol_uuid, $transaction_uuid = null, $dosage_overide = null, $location_overide = null) {

// Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();


            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);



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

            if ($location_overide != null) {
                $location = $location_overide;
            }

            $protocol_uuid = empty($protocol_uuid) ? null : $protocol_uuid;
            $transaction_uuid = empty($transaction_uuid) ? null : $transaction_uuid;
            $userid = $GLOBALS['auth']->getUsername();

//lookup cow's name
            $res2 = $GLOBALS['pdo']->query("SELECT full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1");
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);

//insert into google calendar
            $title = 'Treat: ' . $row2['local_number'] . ' with ' . $row['trade_name'] . ' ' . $dosage . '' . $default_dosage_unit . ' ' . $location;
            $description = $title;
            $google_calendar_event_id = self::insertGoogleCalendarScheduledTreatment($event_time, $title, $description);
            $google_calendar_event_id_escaped = pg_escape_string($google_calendar_event_id);



            $out['bovine_id'] = $bovine_id;
            $out['medicine_index'] = $medicine_id;
            $out['scheduled_event_time'] = $event_time;
            $out['dosage'] = $dosage;
            $out['dosage_unit'] = $default_dosage_unit;
            $out['location'] = $location;
            $out['note'] = null;
            $out['medical_case_id'] = $medical_case_id;
            $out['scheduled_userid'] = $userid;
            $out['calendar_event_id'] = $google_calendar_event_id_escaped;
            $out['protocol_uuid'] = $protocol_uuid;
            $out['uuid'] = $transaction_uuid;


//get columns list name. 

            $blah = $GLOBALS['MiscObj']->createPDOInsertNamesFromArrayKeys($out);

//schedule administer drug
            $sqlInsert = <<<SQL
INSERT INTO bovinemanagement.medicine_administered  ({$blah['column_list']}) VALUES ({$blah['param_list']})
SQL;


            $stmt = $GLOBALS['pdo']->prepare($sqlInsert);
            $stmt->bindValue(':bovine_id', $out['bovine_id'], PDO::PARAM_INT);
            $stmt->bindValue(':medicine_index', $out['medicine_index'], PDO::PARAM_STR);
            $stmt->bindValue(':scheduled_event_time', $out['scheduled_event_time'], PDO::PARAM_STR);
            $stmt->bindValue(':dosage', $out['dosage'], PDO::PARAM_STR);
            $stmt->bindValue(':dosage_unit', $out['dosage_unit'], PDO::PARAM_STR);
            $stmt->bindValue(':location', $out['location'], PDO::PARAM_STR);
            $stmt->bindValue(':note', $out['note'], PDO::PARAM_STR);
            $stmt->bindValue(':medical_case_id', $out['medical_case_id'], PDO::PARAM_INT);
            $stmt->bindValue(':scheduled_userid', $out['scheduled_userid'], PDO::PARAM_STR);
            $stmt->bindValue(':calendar_event_id', $out['calendar_event_id'], PDO::PARAM_STR);
            $stmt->bindValue(':protocol_uuid', $out['protocol_uuid'], PDO::PARAM_STR);



            if (($out['uuid']) == null) {
                $stmt->bindValue(':uuid', null, PDO::PARAM_NULL);
            } else {
                $stmt->bindValue(':uuid', $out['uuid'], PDO::PARAM_STR);
            }


            $status = $stmt->execute();
            //  echo( $stmt->debugDumpParams());
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
            error_log($e->getMessage(), 0);
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
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/google/googleCalendar.inc');
            $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['BOVINE_TREATMENT']); //bovine_treatement
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
            throw new Exception('Error: No Calendar Eventid!');
        }

        try {

            $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['BOVINE_TREATMENT']); //bovine_treatement
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
        $out[] = ("<h3>Scheduled Medicines</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
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

    function displayAllScheduledMedicines($mode) {

        $sql = <<<SQL
                SELECT distinct(bovine_id), bovineall.id as bovine_id,bovineall.location_name,local_number,full_name,

(
   SELECT json_agg(
              json_build_object(
                   'medicine_administered_id',(medicine_administered.id),'uuid',(uuid),'trade_name',(trade_name),
                   'dosage',(dosage),'dosage_unit',(dosage_unit),'location',(location),'note',(note),'scheduled_event_time',(scheduled_event_time),'scheduled_userid',(scheduled_userid), 'initials', (wcauthentication.initials(scheduled_userid))
              )::text
          )::text
   FROM bovinemanagement.medicine_administered 
   LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
   WHERE bovine_id=bovineall.id AND event_time is null
) as meds_json

FROM bovinemanagement.medicine_administered 
LEFT JOIN bovinemanagement.bovineall ON medicine_administered.bovine_id = bovineall.id
WHERE event_time is null    
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut = array();

        //change presentation
        foreach ($results as &$row) {

            // $rowOut['event_time_raw'] = strtotime($row['event_time2']);
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = $row['location_name'];


            $arr = array();
            //decode json for repor report
            $arr = (json_decode($row['meds_json'], true));

            $blah = array();
            if (!empty($arr)) {
                foreach ($arr as $key => $value) {
                    $arr2 = array();
                    $arr2 = (json_decode($value, true));
                    //var_dump($arr2);
                    
                    $initialsStr =(new Bootstrap)->initials($arr2['initials']);
                    
                    $blah[] = JQueryDataTable::formattedDate($arr2['scheduled_event_time'], 'M d H:00') . ' <b>' . $arr2['trade_name'] . '</b> ' . $arr2['dosage'] . ' ' . $arr2['dosage_unit'] . ' '.$initialsStr . '<br> ';
                    
               
                $delButton = new QF2_DeleteButtonDecider('user_timeFuture', (object) array('dateTimeName' => 'scheduled_event_time', 'useridColumnName' => 'scheduled_userid', 'buttonClassName' => 'QF2_ScheduleSingleGeneralTreatmentDeleteButton', 'schemaTable' => 'bovinemanagement.medicine_administered'));
                $delButtonTransaction = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'deleteName' => 'uuid', 'schemaTable' => 'bovinemanagement.medicine_administered'));
                $completeButton = new QF2_CompleteButtonDecider('anyuser_timeLimit', (object) array('dateTimeName' => 'scheduled_event_time', 'buttonClassName' => 'QF2_ScheduleSingleGeneralTreatmentCompleteButton', 'schemaTable' => 'bovinemanagement.medicine_administered'));

                $blah[] =   $delButton->output($arr2['medicine_administered_id']) . $delButtonTransaction->output($arr2['uuid'], null, "<i class='fa fa-fw fa-object-group'></i><i class='fa fa-fw fa-trash'></i>") . $completeButton->output($arr2['medicine_administered_id']);
            
                    
                }
            }
            $rowOut['Meds'] = implode($blah);



            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

         if (empty($dataArr)) {
            return (new BootStrap)->infoBadge('No Scheduled Medications.');
        } else {
        
        return( (new JQueryDataTable)->startBasicArrayNoSort('Scheduled Medications', $dataArr));
        }
    }

    function displayAllScheduledMedicinesHistorical($mode='historical') {

        if ($mode == 'historical') {
            //show any medicine that is currently scheduled or ever was.
            $sql = "SELECT medicine_administered.id as id, uuid,bovineall.id as bovine_id,bovineall.location_name,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time, close_date,scheduled_event_time as event_time2,scheduled_userid
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
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {

            $rowOut['event_time_raw'] = strtotime($row['event_time2']);
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = $row['location_name'];
            $rowOut['Trade Name'] = $row['trade_name'];
            $rowOut['Dosage'] = $row['dosage'];
            $rowOut['Dosage Unit'] = $row['dosage_unit'];
            $rowOut['Body Location'] = $row['location'];
            $rowOut['Scheduled Time'] = date("l M d Y H:00", strtotime($row['event_time2']));  //has to be at end.
            $rowOut['Scheduler Employee'] = $row['scheduled_userid'];

            $rowOut['ID'] = $row['id'];
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

  return( (new JQueryDataTable)->startBasicArray('Scheduled Medications Historical', $dataArr));
        

}
}
//end class
Class QF2_DeleteButtonMedicineSchedule extends QF2_Base {

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



class AccordionImplementationMedicalProtocols extends AccordionSub {

    function tab1() {
        return('
<div>
<p>
Ketosis Protcol 1.3 to 2.4 mmol.
</p>
<h4>PROCEDURE:</h4>
<ul>
<li>DAY 0: Newcels and Glycol 2x</li>
<li>DAY 1: Glycol 2x</li>
<li>DAY 2: Glycol 2x</li>
<li>DAY 3: Glycol 2x</li>
<li>DAY 4: Glycol 2x</li>

</div>');
    }

    function tab2() {
        return('
<div>
<p>
Ketosis Protcol 2.5 to 3.5 mmol.
</p>
<h4>PROCEDURE:</h4>
<ul>
<li>NOT DEFINED YET</li>

</ul>

</div>');
    }

}

//end class
Class QF2_ScheduleAction extends QF2_Base {

    function definition($form) {
        // grab a list of action types'.
        $actionArr [null] = "Please Select one..."; // dummy value
        $dosageArray [null] = ""; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,action from bovinemanagement.medical_action_type ORDER BY action");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $actionArr[$row['id']] = $row['action'];
        }

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForNextSevenDays(); //hourly array, with unix timestamp as index.

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
        $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form, $this->param->aniNumber, $this->mode);
        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");
        $form->addElement('select', 'action', null, array('options' => $actionArr, 'label' => 'Action'))->addRule("required", "Action is required.");

        return $form; //required       
    }

    function process($value) {
         $c = new MedicineScheduled();
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);

         $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
        foreach ($bovineIdArr as $key => $bovine_id) {
            (new MedicineScheduled)->protocolMedicalActionDefaultInsert($bovine_id, $value['event_time'], $value['action'], $uuid = null);
        }
        
         
    }

}

//end class
//delete button for above because of calendday functionaility.
Class QF2_ScheduleSingleMedicalActionDeleteButton extends QF2_DeleteButtonGeneric {

    function action() {
        //$this->param should provide everything the form needs for paramaters.  

        MedicineScheduled::protocolMedicineDefaultDeleteByID('medical_action', $this->param->deleteID);
    }

}

Class QF2_ScheduleSingleGeneralTreatment extends QF2_Base {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForNextThirtyDays();

        $location = $this->listActiveLocationsAndCounts();

        // body locations
        $bodyLocationArray = MedicalCase::medicalBodyLocations(true);

        // grab a list of Medicines'.
        $medArray [null] = "Please Select one..."; // dummy value
        $dosageArray [null] = ""; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE for_mastitis!=true ORDER BY trade_name");
        while (($row = $res->fetch())) {
            $medArray [$row [0]] = $row [1];
            $dosageArray [$row [0]] = $row [3];
            $defaultDosageArray [$row [0]] = $row [2];
            $defaultBodyLocationArray [$row [0]] = $row [4];
        }

        // prints out javascript to change dosage defaults when mediciane changes.
        $medList = QF2_GeneralTreatment::listOfMedicines();
        $js = MedicalCase::javascriptForMedDefaults($this->uuid, $medList['dosageArray'], $medList['defaultDosageArray'], $medList['defaultBodyLocationArray']);
        $form->addElement('static', 'script_medicalDefault')->setValue($js);

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form, $this->param->aniNumber, $this->mode);


        $form->addElement('select', 'medicine', array('onChange' => "updateContent{$this->uuid}(this);"), array('options' => $medArray, 'label' => 'Medicine'))->addRule("required", "Please specify your medicine.");
        $form->addElement('text', 'dosage', null, array('label' => 'Dosage (??)'))->addRule("required", "Please specify your dosage.");
        $form->addElement('select', 'bodyLocation', null, array('options' => $bodyLocationArray, 'label' => 'Body Location'))->addRule("required", "Please specify your body location.");
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time.");


        //defaults
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
                    'animalNumber' => 0
        )));



        return $form; //required   
    }

    function process($value) {

        $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);
        $uuid = $this->uuid;

        //  var_dump($value);
        //      var_dump($uuid);
        //       exit();
        //loop over animals
        $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
        foreach ($bovineIdArr as $key => $bovine_id) {
            (new MedicineScheduled)->protocolMedicineDefaultInsert($bovine_id, $value['event_time'], $value['medicine'], null, $uuid, $value['dosage'], $value['bodyLocation']);
        }
    }

    //used for animals movements
    function listActiveLocationsAndCounts() {
        //grab a list of locations with pen counts// SQL SMALL BUG ON COUNT	
        $res = $GLOBALS['pdo']->query("SELECT DISTINCT ON (location.name) location.name,location.id,COUNT(*) as count FROM bovinemanagement.location LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.location_id=location.id   WHERE location.active=true GROUP BY location.name,location.id ORDER BY location.name");

        $activeLocations[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //bug in above sql, prints count of 1 when the pen is empty.
            if ($row['count'] == 1) {
                $row['count'] = null;
                $activeLocations[$row['id']] = $row['name'];
            } else {
                $activeLocations[$row['id']] = $row['name'] . ' (' . $row['count'] . ')';
            }
        }
        return $activeLocations;
    }

}

//end class   

Class QF2_ScheduleSingleGeneralTreatmentDeleteButton extends QF2_DeleteButtonGeneric {

    function action() {
        //$this->param should provide everything the form needs for paramaters.  

        MedicineScheduled::protocolMedicineDefaultDeleteByID('medicine_administered', $this->param->deleteID);
    }

}

Class QF2_ScheduleSingleGeneralTreatmentCompleteButton extends QF2_CompleteButtonGeneric {

    function action() {
        //$this-param should provide everything the form needs for paramaters.  
        //find the bovine_id
        $res = $GLOBALS['pdo']->query("SELECT bovine_id FROM bovinemanagement.medicine_administered WHERE id={$this->param->completeID} LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $bovine_id = $row['bovine_id'];
        $event_time = date('c', strtotime('now'));
        $userid = $GLOBALS['auth']->getUsername();

        $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);
        $medical_case_id = empty($medical_case_id) ? 'null' : $medical_case_id;

// Open a transaction
//update event to say it is completed.
        $res = $GLOBALS['pdo']->query("Update {$this->param->schemaTable} SET event_time='$event_time', userid='$userid',medical_case_id=$medical_case_id  WHERE id={$this->param->completeID}");
    }

}