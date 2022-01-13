<?php 
class WorkSchedule extends BasePage{
  
 
  


public function defaultDisplay() {
    

      
     if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            $right = <<<HTML
                 
         {$this->add63workScheduleDatesQuickform()}
         {$this->add6241workScheduleDatesQuickform()}
         {$this->addSingleWorkShiftQuickform()}
         {$this->addVacationDayQuickform()}
HTML;
            $left = $this->displayGoogleCalendarWorkSchedule();
            print ((new BootStrap)->oneThirdTwoThirds($left, $right));
        } else {
            print($this->displayGoogleCalendarWorkSchedule());
        }
    }

/**
admin function to add single work schedule dates to calendar
*/
function addSingleWorkShiftQuickform() {
//daily dates array, with unix timestamp as index.
  $dateArray=$GLOBALS['MiscObj']->createDailyDatesForNextNineMonthsPrevSixMonthsAtNoon();
  $todayNoonTimeStamp=strtotime(date('Y-m-d 12:00:00',( strtotime('now')))); //so we can set today to the default.
  
  	//grab a list ofemployees
		$res = $GLOBALS['pdo']->query("SELECT firstname FROM wcauthentication.users WHERE onfarm=true AND active=true ORDER BY firstname");
		
		$employeeList[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$employeeList["{$row['firstname']}"] = $row['firstname'];
		}
  
  
  $numDays=array(null =>'Please Select one...','1' => '1', '2' => '2', '3' => '3', '4' => '4', '5' => '5', '6' => '6');
  $ampm=array(null =>'Please Select one...','am' => 'am milking', 'pm' => 'pm milking','day' => 'day work shift');
   
  $form = new HTML_QuickForm("addSingleWorkShiftQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);   $renderer = new HTML_QuickForm_Renderer_Tableless();
  $form->addElement('header', 'hdrQuickform','Add SINGLE Dates To Work Schedule');
  
    $form->addElement('select', 'employeeList', 'Employee', $employeeList);
  $form->addElement('select', 'dateEvent', 'Date of Start of work', $dateArray);
  $form->addElement('select', 'numDays', 'Number of days', $numDays);
  $form->addElement('select', 'ampm', 'Shift (type/time)', $ampm);
  
  $form->addElement('hidden','pageid',$_REQUEST['pageid']);
  
  //submit buttons
  $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
  $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
  $form->addGroup($submitGroup, 'submitGroup');
  
  //defaults
   $form-> setDefaults(array('employeeList' => 0));	
  $form-> setDefaults(array('dateEvent' => $todayNoonTimeStamp));		
  $form-> setDefaults(array('numIterations' => 1));		
  $form-> setDefaults(array('ampm' => 0 ));
  
  //rules
  $form->addRule('employeeList', 'Employee is required', 'required');
  $form->addRule('dateEvent', 'Date of work event is required', 'required');
  $form->addRule('numDays', 'Number of days is required', 'required');	
  $form->addRule('ampm', 'am/pm shift is required', 'required');

  
  // Try to validate a form
  if ($form->validate()) {

    //get values
    $employee=$form->exportValue('employeeList');
    $tempArray=$form->getElementValue('dateEvent');;
    $event_date=date('Y-m-d',$tempArray[0]);
    $numIterations=$form->exportValue('numDays');
    $ampm=$form->exportValue('ampm');
    
    
     //set an initial start date from the event date
     $startDate=$event_date;
 
      if ($ampm=='am') {
     $startTime='04:00:00';
     $endTime='14:00:00';
     $shiftType='Milking Shift';
     $shiftTypeMini='milk';
    }
    elseif ($ampm=='pm') {
     $startTime='12:00:00';
     $endTime='22:00:00';   
     $shiftType='Milking Shift';
     $shiftTypeMini='milk';
    }
    elseif ($ampm=='day') {
     $startTime='07:00:00';
     $endTime='17:00:00';  
     $shiftType='Work Shift';
     $shiftTypeMini='work';
    }
    else {
      throw new error("no am or pm or day specified.");
    }

      //calculate the end date 
      $endDate = date('Y-m-d',strtotime($startDate.' + '. $numIterations .' days'));
     
         $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['WORK_SCHEDULE']); //work_schedule
         $eventId = null;
         $eventId = $gCal->createRecurringEvent($gCal->calendarID, "$shiftTypeMini: $employee", $shiftType, '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $endTime, '-04', $endDate);
         print("RetVal:$eventId<br>");	
      
      
    
    $host  = $_SERVER['HTTP_HOST'];
    header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
    exit();
    
    } //end validation
    
    $form->accept($renderer);
        return $renderer->toHtml();
    
 }

/**
admin function add work schedule dates to calendar
*/
function add63workScheduleDatesQuickform() {

  //daily dates array, with unix timestamp as index.
  $dateArray=$GLOBALS['MiscObj']->createDailyDatesForNextNineMonthsPrevSixMonthsAtNoon();
  $todayNoonTimeStamp=strtotime(date('Y-m-d 12:00:00',( strtotime('now')))); //so we can set today to the default.
  
  	//grab a list ofemployees
		$res = $GLOBALS['pdo']->query("SELECT firstname FROM wcauthentication.users WHERE onfarm=true AND active=true ORDER BY firstname");
		
		$employeeList[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$employeeList["{$row['firstname']}"] = $row['firstname'];
		}
  
  
  $numIterations=array(null =>'Please Select one...','1' => '1', '2' => '2', '3' => '3', '4' => '4', '5' => '5', '6' => '6', '7' => '7', '8' => '8', '9' => '9', '10' => '10', '15' => '15', '20' => '20');
  $ampm=array(null =>'Please Select one...','am' => 'am', 'pm' => 'pm');
   
  $form = new HTML_QuickForm("AddDatesToWorkScheduleQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);   $renderer = new HTML_QuickForm_Renderer_Tableless();
  $form->addElement('header', 'hdrQuickform','Add Milking Shifts To Work Schedule');
  
    $form->addElement('select', 'employeeList', 'Employee', $employeeList);
  $form->addElement('select', 'dateEvent', 'Date of Start of 6&3', $dateArray);
  $form->addElement('select', 'numIterations', 'Number of 9 day iterations', $numIterations);
  $form->addElement('select', 'ampm', 'Shift (am or pm)', $ampm);
  
  $form->addElement('hidden','pageid',$_REQUEST['pageid']);
  
  //submit buttons
  $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
  $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
  $form->addGroup($submitGroup, 'submitGroup');
  
  //defaults
   $form-> setDefaults(array('employeeList' => 0));	
  $form-> setDefaults(array('dateEvent' => $todayNoonTimeStamp));		
  $form-> setDefaults(array('numIterations' => 1));		
  $form-> setDefaults(array('ampm' => 0 ));
  
  //rules
  $form->addRule('employeeList', 'Employee is required', 'required');
  $form->addRule('dateEvent', 'Date of work event is required', 'required');
  $form->addRule('numIterations', 'Number of 6&3 iterarions is required', 'required');	
  $form->addRule('ampm', 'am/pm shift is required', 'required');

  
  // Try to validate a form
  if ($form->validate()) {
    
    //
    
   
    //get values
    $employee=$form->exportValue('employeeList');
    $tempArray=$form->getElementValue('dateEvent');;
    $event_date=date('Y-m-d',$tempArray[0]);
    $numIterations=$form->exportValue('numIterations');
    $ampm=$form->exportValue('ampm');
    
    
     //set an initial start date from the event date
     $startDate=$event_date;
     
  
     //loop through number of iterations
    for ($i = 1; $i <= $numIterations; $i++) {
    
    print("Loop:$i<br>");
 
      if ($ampm=='am') {
     $startTime='04:00:00';
     $endTime='14:00:00';
    }
    elseif ($ampm=='pm') {
     $startTime='12:00:00';
     $endTime='22:00:00';   
    }
    else {
      throw new error("no am or pm specified.");
    }
 
 
      //calculate the end date of the 6 works days
      $endDate = date('Y-m-d',strtotime($startDate.' + 6 days'));
    
       $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['WORK_SCHEDULE']); //work_schedule
      
         $eventId = null;
         $eventId = $gCal->createRecurringEvent($gCal->calendarID, "milk: $employee",   'Milking Shift', '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $endTime, '-04', $endDate);
         print("RetVal:$eventId<br>");	
      
    
	//calculate the next start dateEvent		      
	$startDate = date('Y-m-d',strtotime($startDate.' + 9 days'));		
	
    //switch the am/pm
	switch ($ampm) {
    case 'am':
        $ampm='pm';
	break;
    case 'pm':
        $ampm='am';
	break;
    }
    }

    
    $host  = $_SERVER['HTTP_HOST'];
    header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
    exit();
    
    } //end validation
    
    $form->accept($renderer);
        return $renderer->toHtml();
    
 }
 
/**
admin function add 6 2 4 1 work schedule dates to calendar
*/
function add6241workScheduleDatesQuickform() {

  //daily dates array, with unix timestamp as index.
  $dateArray=$GLOBALS['MiscObj']->createDailyDatesForNextNineMonthsPrevSixMonthsAtNoon();
  $todayNoonTimeStamp=strtotime(date('Y-m-d 12:00:00',( strtotime('now')))); //so we can set today to the default.
  
  	//grab a list ofemployees
		$res = $GLOBALS['pdo']->query("SELECT firstname FROM wcauthentication.users WHERE onfarm=true AND active=true ORDER BY firstname");
		
		$employeeList[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$employeeList["{$row['firstname']}"] = $row['firstname'];
		}
  
  
  $numIterations=array(null =>'Please Select one...','1' => '1', '2' => '2', '3' => '3', '4' => '4', '5' => '5', '6' => '6', '7' => '7', '8' => '8', '9' => '9', '10' => '10', '15' => '15', '20' => '20');

   
  $form = new HTML_QuickForm("Add6241DatesToWorkScheduleQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);   $renderer = new HTML_QuickForm_Renderer_Tableless();
  $form->addElement('header', 'hdrQuickform','Add 6-2-4-1 Dates To Work Schedule');
  
    $form->addElement('select', 'employeeList', 'Employee', $employeeList);
  $form->addElement('select', 'dateEvent', 'Monday start of 6 day stretch of 6-2-4-1', $dateArray);
  $form->addElement('select', 'numIterations', 'Number of 14 day iterations', $numIterations);

  
  $form->addElement('hidden','pageid',$_REQUEST['pageid']);
  
  //submit buttons
  $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
  $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
  $form->addGroup($submitGroup, 'submitGroup');
  
  //defaults
   $form-> setDefaults(array('employeeList' => 0));	
  $form-> setDefaults(array('dateEvent' => $todayNoonTimeStamp));		
  $form-> setDefaults(array('numIterations' => 1));		
  $form-> setDefaults(array('ampm' => 0 ));
  
  //rules
  $form->addRule('employeeList', 'Employee is required', 'required');
  $form->addRule('dateEvent', 'Date of work event is required', 'required');
  $form->addRule('numIterations', 'Number of 6-2-4-1 iterarions is required', 'required');	


  
  // Try to validate a form
  if ($form->validate()) {
    
    //
    
   
    //get values
    $employee=$form->exportValue('employeeList');
    $tempArray=$form->getElementValue('dateEvent');;
    $event_date=date('Y-m-d',$tempArray[0]);
    $numIterations=$form->exportValue('numIterations');

    
     //set an initial start date from the event date
     $startDate=$event_date;
     
      $startTime='07:00:00';
      $endTime='17:00:00';
  
     //loop through number of iterations
    for ($i = 1; $i <= $numIterations; $i++) {
    
     
      //calculate the end date of the 6 works days
      $endDate = date('Y-m-d',strtotime($startDate.' + 6 days'));
     
                
       $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['WORK_SCHEDULE']); //work_schedule
         $eventId = null;
         $eventId = $gCal->createRecurringEvent($gCal->calendarID, "work: $employee",   'Work Shift', '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $endTime, '-04', $endDate);
         print("RetVal:$eventId<br>");	
             
		
                
                
	//now skip two days and do the next 4 work days.
	$startDate = date('Y-m-d',strtotime($endDate.' + 2 days'));
	$endDate = date('Y-m-d',strtotime($startDate.' + 4 days'));
	         
       $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['WORK_SCHEDULE']); //work_schedule
         $eventId = null;
         $eventId = $gCal->createRecurringEvent($gCal->calendarID, "work: $employee",   'Work Shift', '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $endTime, '-04', $endDate);
         print("RetVal:$eventId<br>");	
             
		
	
	//calculate the next start dateEvent		      
	$startDate = date('Y-m-d',strtotime($endDate.' + 2 days'));		
	

    }

    
    $host  = $_SERVER['HTTP_HOST'];
    header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
    exit();
    
    } //end validation
    
    $form->accept($renderer);
        return $renderer->toHtml();
    
 } 
 
 
 
 /**
admin function to add vacation dates to calendar
*/
function addVacationDayQuickform() {

  //daily dates array, with unix timestamp as index.
  $dateArray=$GLOBALS['MiscObj']->createDailyDatesForNextNineMonthsPrevSixMonthsAtNoon();
  $todayNoonTimeStamp=strtotime(date('Y-m-d 12:00:00',( strtotime('now')))); //so we can set today to the default.
  
  	//grab a list ofemployees
		$res = $GLOBALS['pdo']->query("SELECT firstname FROM wcauthentication.users WHERE onfarm=true AND active=true ORDER BY firstname");
		
		$employeeList[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$employeeList["{$row['firstname']}"] = $row['firstname'];
		}
  
  
  $numIterations=array(null =>'Please Select one...','1' => '1', '2' => '2', '3' => '3', '4' => '4', '5' => '5', '6' => '6', '7' => '7', '8' => '8', '9' => '9', '10' => '10', '11' => '11', '12' => '12','13' => '13','14' => '14');
 

 
 $reason=array(null =>'Please Select one...','Milking' => 'Milking','Vacation' => 'Vacation', 'Day Off' => 'Day Off', 'For Holiday' => 'For Holiday', 'Sick' => 'Sick');
   
  $form = new HTML_QuickForm("addVacationDayQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);   $renderer = new HTML_QuickForm_Renderer_Tableless();
  $form->addElement('header', 'hdrQuickform','Add Vacation Dates To Work Schedule');
  
    $form->addElement('select', 'employeeList', 'Employee', $employeeList);
     $form->addElement('select', 'reason', 'Reason', $reason);
  $form->addElement('select', 'dateEvent', 'Date of Start of Vacation', $dateArray);
  $form->addElement('select', 'numIterations', 'Number of days (including first)', $numIterations);
  
  $form->addElement('hidden','pageid',$_REQUEST['pageid']);
  
  //submit buttons
  $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
  $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
  $form->addGroup($submitGroup, 'submitGroup');
  
  //defaults
   $form-> setDefaults(array('employeeList' => 0));	
    $form-> setDefaults(array('reason' => 0));
  $form-> setDefaults(array('dateEvent' => $todayNoonTimeStamp));		
  $form-> setDefaults(array('numIterations' => 1));		
  
  //rules
  $form->addRule('employeeList', 'Employee is required', 'required');
  $form->addRule('reason', 'Reason is required', 'required');
  $form->addRule('dateEvent', 'Date of vacation event is required', 'required');
  $form->addRule('numIterations', 'Number of vacation days is required', 'required');	

  
  // Try to validate a form
  if ($form->validate()) {
    
    //
    
   
    //get values
    $reason=$form->exportValue('reason');
    $employee=$form->exportValue('employeeList');
    $tempArray=$form->getElementValue('dateEvent');;
    $event_date=date('Y-m-d',$tempArray[0]);
    $numIterations=$form->exportValue('numIterations');
   
   
     //set an initial start date from the event date
     $startDate=$event_date;
     
     $startTime='';
     $endTime='';
    
      //calculate the end date based on the number of iterations (days)
      $numIterations=$numIterations - 1; //subtract one because first say is already counted
      $endDate = date('Y-m-d',strtotime($startDate.' + '. $numIterations .' days'));
    
               
         $gCal = new GoogleCalendar($GLOBALS['config']['GOOGLE_CALENDAR']['WORK_SCHEDULE']); //work_schedule
         $eventId = null;
         $eventId = $gCal->createRecurringEvent($gCal->calendarID,  "$reason: $employee",  "$reason", '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $endTime, '-04', $endDate);
         print("RetVal:$eventId<br>");	
           

    $host  = $_SERVER['HTTP_HOST'];
    header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
    exit();
    
    } //end validation
    
    $form->accept($renderer);
        return $renderer->toHtml();
    
 }
     
    
    function displayGoogleCalendarWorkSchedule() {

      /* display a 100% responsive iframe
       FROM:  https://community.weebly.com/t5/How-Do-I-In-the-Community/responsive-Google-Calendar-and-where-to-put-the-CSS-code/m-p/155095/highlight/true#M14287 */
       $html=<<<HTML
               <style>
iframe {
width: 10px !important;
min-width: 100%;
*width: 100%;
}

 

.responsive {
width: 100%;
height: 0;
padding-bottom: 56.25%;
position: relative;
}

 

.responsive iframe {
position: absolute;
width: 100%;
height: 100%;
}
</style>

 

<div class="responsive">
               
<iframe src="https://calendar.google.com/calendar/embed?height=600&wkst=1&bgcolor=%23ffffff&ctz=America%2FHalifax&src=bGl0dGxlcml2ZXIuY2FfMG5iaDhiNHQ5Z2dvNG1pa3NmMTJyOTVnNXNAZ3JvdXAuY2FsZW5kYXIuZ29vZ2xlLmNvbQ&src=bGl0dGxlcml2ZXIuY2FfM3NobmhidGFmcHQ0ZHRrZzA3OGVwaTcwN2NAZ3JvdXAuY2FsZW5kYXIuZ29vZ2xlLmNvbQ&color=%23D50000&color=%2333B679" style="border:solid 1px #777" width="1000" height="600" frameborder="0" scrolling="no"></iframe>
</div>
HTML;
        
        
       return $html;
         
      
    }
    
    
    
    
} //end class
?>