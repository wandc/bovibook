<?php 



 
 ;



class TimeSheet extends BasePage {

   

    public function defaultDisplay() {

        //only show admin
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $this->displayPayrollView();
            $this->displayNotes();
        } else { //employee view
            $this->addRemoveTimeQuickform();
            $this->displayNotes();
            $this->displayTimeLog($GLOBALS['auth']->getUsername());
        }
    }

    function displayNotes() {
        print("NOTE:<br>");
        print("1. Pay periods are from Saturday 00:00 to Friday 23:59." . "<br>");
        print("2. Time Can only be banked for time over 9 hours in a single day." . "<br>");
        print("3. Time Bank withdrawls can only be used to top up to 9 hours in a single day." . "<br>");
        print("4. Time Bank must be used up by the end of the calendar year." . "<br>");
    }

    function displayPayrollView($mode = 'default') {

        print("<h3>Payroll View</h3>");

//show only active employees who are not owners.
        $sql = "
With temp as (
SELECT *,(SELECT groupid FROM wcauthentication.users_in_groups WHERE users_in_groups.groupid='owner' AND users.userid=userid) as owner
 FROM wcauthentication.users 
WHERE active=true AND onfarm=true and testing=false 
)
SELECT * FROM temp WHERE temp.owner is null ORDER BY temp.lastname    
";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $this->displayTimeLog(strtolower($row['firstname']), 'payrollView');
        }
    }

    function createListofTimesAt15minIntervals($start15minInt, $finish15minInt) {


        $totalTimeIntervals = ((abs($start15minInt) + abs($finish15minInt)) * 4) - 2;


        for ($i = 0; $i <= $totalTimeIntervals; $i++) {

            $temp = $start15minInt + ($i * .25);
            $listOfTimes["$temp"] = "$temp hours";
        }

        return $listOfTimes;
    }

    /**
      allows user to add or remove time from the time bank and calc hours worked
     */
    function addRemoveTimeQuickform() {



        //daily dates rray, with unix timestamp as index.
        $dateArray = Misc::createDailyDatesAtDailyIntervalsForLastSevenDays();
        $dateKeysArray = array_keys($dateArray); //so we can set today to teh default.


        $timeBankEvents = array(null => 'Please Select one...', 'add' => 'Add Time', 'remove' => 'Use Time');

        $listOfWorkTimes = $this->createListofTimesAt15minIntervals(0, 20);
        $listOfTimeBankTimes = $this->createListofTimesAt15minIntervals(-9, 6);

        $reason = array(null => 'Please Select one...', 'Vacation' => 'Vacation', 'Day Off' => 'Day Off', 'For Holiday' => 'For Holiday', 'Sick' => 'Sick');
        $shiftTypeArr = array(null => 'Please Select one...', 'Milking AM' => 'Milking AM', 'Milking PM' => 'Milking PM', 'Cropping' => 'Cropping');

        $form = new HTML_QuickForm("addRemoveTimeFromBankQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Hours Worked');

        $form->addElement('select', 'dateEvent', 'Date of Event', $dateArray);
        $form->addElement('select', 'shiftType', 'Shift Type', $shiftTypeArr);
        $form->addElement('select', 'workTime', 'Hours Actually Worked', $listOfWorkTimes);
        $form->addElement('select', 'bankTime', 'Hours to Deposit / Withdrawl From Bank', $listOfTimeBankTimes);
        $form->addElement('select', 'reason', 'Explanation (optional)', $reason);
        $form->addElement('textarea', 'explanation_overtime', 'Explanation (>9 hours only)', array('cols' => 80, 'rows' => 3));


        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('dateEvent' => $dateKeysArray[1]));
        $form->setDefaults(array('shiftType' => 0));
        $form->setDefaults(array('workTime' => 9));
        $form->setDefaults(array('bankTime' => 0));
        $form->setDefaults(array('reason' => 0));

        //rules
        $form->addRule('dateEvent', 'Date of work event is required', 'required');
        $form->addRule('shiftType', 'Shift Type is required', 'required');
        $form->addRule('workTime', 'Hours Actually Worked is required', 'required');
        $form->addRule('bankTime', 'Hours to Deposit / Withdrawl From Bank is required', 'required');

        //custom rule for time bank hours
        //$form->registerRule('checkTimeBank1','callback', 'checkTimeBankPositive');
        //$form->addRule('checkTimeBank1', 'ERROR: Improper number of digits (8 for female, 16 for male).', 'callback', 'checkTimeBankPositive');
        //check that cow has a due date (ie someone hasn't already entered that they calved) custom rule 
        $form->registerRule('checkTimeBankPositive', 'function', 'checkTimeBankPositive');
        $form->addRule(array('workTime'), 'ERROR: Hours worked time bank combination does not work. Try again.', 'checkTimeBankPositive', array('workTime'));

        //check that cow has a due date (ie someone hasn't already entered that they calved) custom rule 
        $form->registerRule('checkDateNotAlreadyInDB', 'function', 'checkDateNotAlreadyInDB');
        $form->addRule(array('dateEvent'), 'ERROR: Date already entered.', 'checkDateNotAlreadyInDB', array('dateEvent'));

        //custom rule for if over 9 hours of work they need an explanation.
        $form->registerRule('checkExplanationForOvertime', 'function', 'checkExplanationForOvertime');
        $form->addRule(array('workTime','explanation_overtime'), 'ERROR: No Explanation for overtime worked.', 'checkExplanationForOvertime', array('workTime','explanation_overtime'));

        function checkDateNotAlreadyInDB($junk, $valueArr, $elementNameArr) {
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }

            $event_date = date('Y-m-d H:i:sO', $formData['dateEvent']);


            $sql = "SELECT id FROM hr.time_sheet WHERE userid='{$GLOBALS['auth']->getUsername()}' AND event_time='$event_date' AND deleted=false";
            $res = $GLOBALS['pdo']->query($sql);
            if ($res->rowCount() >= 1) {
                $returnStatement = 0; //means throw an error that date is taken.
            } else {
                $returnStatement = 1; //all ok.
            }
            return $returnStatement;
        }

        function checkExplanationForOvertime($junk, $valueArr, $elementNameArr) {
            //generic loop to convert $form elements we want into a usuable array
            //I really hope this isn't how quickforms was designed.
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }

            
            //when workTime is ovetime and textbox is blank...
            if ((trim($formData['explanation_overtime']) == '') AND ($formData['workTime'] > 9)) {
                $retValue = 0; //error
            } else {
                $retValue = 1; //ok
            }

            return $retValue;
        }

        function checkTimeBankPositive($junk, $valueArr, $elementNameArr) {
            //generic loop to convert $form elements we want into a usuable array
            //I really hope this isn't how quickforms was designed.
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }

            //when time bank time is being withdrawn we can only allow the total time to be 9.
            //when time bank time is being deposited we can only allow the total time to drop to 9.
           // $bankTime=0;
           if (!array_key_exists('bankTime',$formData)) {
                $bankTime = 0;
           } 
           else {
                     $bankTime = $formData['bankTime'];
           }
           //
           if (!array_key_exists('workTime',$formData)) {
                $bankTime = 0;
           } 
           else {
                     $workTime = $formData['workTime'];
           }
  

            /*
              work bank
              10   1    =9 ok
              10   2    =8 no
              8    -1   = 9 ok
              8    -.5  = 8.5 ok
             */
            if (( ($workTime - $bankTime) < 9) && ($bankTime != 0)) {    //person worked less than 9 hours and tried to bank time... fail.
                $retVal = 0;
            }
            //elseif () { //FIXME: add more rules. I think I am missing some.
            //$retVal=0; }
            else {
                $retVal = 1;
            } //true ok!
            //check that bankTime does not put overall balance negative.
            $sql = "SELECT sum(time_banked) as sum_time_banked FROM hr.time_sheet_view where userid='{$GLOBALS['auth']->getUsername()}' AND deleted=false limit 1";
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if ($row['sum_time_banked'] + $bankTime <= 0) {
                $retValue = 0;
            }
            return $retVal;
        }

        // Try to validate a form
        if ($form->validate()) {

            //get values


            $tempArray = $form->getElementValue('dateEvent');
            ;
            $event_date = date('Y-m-d H:i:sO', $tempArray[0]);
            $workTime = $form->exportValue('workTime');
            $bankTime = $form->exportValue('bankTime');
            $explanation = $form->exportValue('reason');
            $shiftType = $form->exportValue('shiftType');
            $explanation_overtime = $form->exportValue('explanation_overtime');

            if  ($explanation_overtime =='') {
                $explanation_overtime='null';
            }
            else {
                $explanation_overtime="'".pg_escape_string($explanation_overtime)."'";
            }
                        
            
            //don't put id value, uses next one in DB. //use global username as security feature, instead of passing it.
            $query = "INSERT INTO hr.time_sheet (event_time,time_worked,time_banked,userid,deleted,explanation,shift_type,explanation_overtime) VALUES ('$event_date','$workTime','$bankTime','{$GLOBALS['auth']->getUsername()}','false','$explanation','$shiftType',$explanation_overtime)";
            
            $res = $GLOBALS['pdo']->exec($query);

            {
                print_r($res);
              
            }

            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    
    public function timeLog($request) {
        $useridtouse = Sanit::reqVal('item', false, 'STR');
        
      if ($useridtouse!=false) {
        //for sercurity reasons,only allow give userid for when admin logged in.
          if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            //when admin allow any userid.ie pass thorugh.
              
          
        } else { //employee view
            $useridtouse=$GLOBALS['auth']->getUsername(); //only user userid of user no matter what
            
        }
        
       // Setup your database connection
        $sql = "SELECT '$useridtouse' as useridtouse,explanation,shift_type,* FROM hr.time_sheet_view WHERE userid='$useridtouse' AND deleted=false ORDER BY event_time DESC ";
 
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            
 
	  //  $row['full_reg_number'] = '<a href="' . Misc::createListOfAllCowsMilking($row['full_reg_number']) . '">' . $row['full_reg_number'] . '</a>';
            $row['event_time_nice'] = date("l  M j, Y", strtotime($row['event_time'])); 
            $row['time_bank_balance'] = $this->printTimeBankToDate($row['useridtouse'],$row['event_time']);
            $row['pay_period_to_date']= $this->printPayPeriodToDate($row['useridtouse'],$row['event_time']) ;
            $row['action']=$this->printActionButtons($row['id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['colour_row']= (date("N", strtotime($row['event_time'])) == 5) ? 1 : 0;
                   
        }
        //output
        $output['data'] = $results;
        return ($output);   
      }
      return null;
    }
    
    function displayTimeLog($useridtouse, $mode = 'default') {
       
        if ($mode == 'payrollView') {
            print("<h3>Payroll View : $useridtouse</h3>");
        } else {
            print("<h3>Time Log</h3>");
        }

        
        
      /*
          $dg = new DataGridLR_Ext($sql, 14);

        if ($mode == 'payrollView') {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('employee', 'userid', '', array('' => ''), null, null));
        }
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''), null, array('TimeSheet', 'printFormattedTime()')));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('useridToUse', 'useridtouse', '', array('' => ''), null, null));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Actual Time Worked (hours)', 'time_worked', '', array('' => ''), null, null));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Shift', 'shift_type', '', array('' => ''), null, null));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Explanation', 'explanation', '', array('' => ''), null, null));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Time Bank Deposit / Withdrawl', 'time_banked', '', array('' => ''), null, null));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Time Bank Balance', null, '', array('' => ''), null, array('TimeSheet', 'printTimeBankToDate()')));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Total Daily Time To Be Paid', 'total_time', '', array('' => ''), null, null));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Pay Period Balance', null, '', array('' => ''), null, array('TimeSheet', 'printPayPeriodToDate()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Overtime Explanation', 'explanation_overtime', '', array('' => ''), null, null));
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Overtime', null, '', array('' => ''), null, array('TimeSheet', 'printOvertime()')));
          
        } else {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('TimeSheet', 'printActionButtons()')));  
        }

        print($dg->render('datagrid',true,$GLOBALS['config']['COLOUR']['attention']));
        $dg->pager();
       
       */
         $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time",
                 "visible": false,
                "searchable": false
            },
                 
            {
                data: "event_time_nice",
                title: "Date"
            },
         {
                data: "useridtouse",
                title: "Employee"
            },
            {
                data: "time_worked",
                title: "Actual Time Worked (hours)"
            },
       
            {
                data: "shift_type",
                title: "Shift"
            },
        {
                data: "explanation",
                title: "Explanation"
            },
         {
                data: "time_banked",
                title: "Time Bank (Deposit / Withdrawl)"
            },
            {
                data: "time_bank_balance",
                title: "Time Bank Balance"
            },
            {
                data: "total_time",
                title: "Total Daily Time To Be Paid"
            },
            {
                data: "pay_period_to_date",
                title: "Pay Period Balance"
            },
            {
                data: "explanation_overtime",
                title: "Overtime Explanation"
            },
         {
                data: "action",
                title: "Action"
            }
EX;
    
          print($dt->start(70,'timeLog',$useridtouse,$columnsStr,'highlight_attention',false));       
    
    }

    
   


    function printTimeBankToDate($useridtouse,$event_time) {
      
        $rowDate = strtotime($event_time);


        $endDate = date('Y-m-d H:i:sO', $rowDate);

        $sql = "SELECT sum(time_banked) as total_time_banked FROM hr.time_sheet_view where userid='{$useridtouse}' AND deleted=false AND event_time <= '$endDate' limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $returnVal = $row['total_time_banked'];
        }

        return $returnVal;
    }

    function printPayPeriodToDate($useridtouse,$event_time) {
      
        $rowDate = strtotime($event_time);
        $rowDayOfWeek = date('N', $rowDate);
        //ISO-8601 numeric representation of the day of the week 1 (for Monday) through 7 (for Sunday)
        //find the start of the pay period (the previous saturday).
        switch ($rowDayOfWeek) {
            case 1: //monday
                $payPeriodStartDayOfWeek = 2;
                break;
            case 2: //tuesday
                $payPeriodStartDayOfWeek = 3;
                break;
            case 3: //wednesday
                $payPeriodStartDayOfWeek = 4;
                break;
            case 4: //thursday
                $payPeriodStartDayOfWeek = 5;
                break;
            case 5: //friday
                $payPeriodStartDayOfWeek = 6;
                break;
            case 6: //saturday
                $payPeriodStartDayOfWeek = 0;
                break;
            case 7: //sunday
                $payPeriodStartDayOfWeek = 1;
                break;
        }

        $payPeriodStartDate = $rowDate - $payPeriodStartDayOfWeek * 3600 * 24;

        $startDate = date('Y-m-d H:i:sO', $payPeriodStartDate);
        $endDate = date('Y-m-d H:i:sO', $rowDate);

        $sql = "SELECT sum(total_time) as sum_total_time FROM hr.time_sheet_view where userid='{$useridtouse}' AND event_time::date >= '$startDate'::date AND event_time::date <= '$endDate'::date AND deleted=false";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $returnVal = $row['sum_total_time'];
        }

        if ($rowDayOfWeek == 5) { //then it is friday
            $returnVal = "<b>" . $returnVal . "</b>";
        }

        return $returnVal;
    }

    function printActionButtons($id,$callBackURL) {

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM hr.time_sheet WHERE userid='{$GLOBALS['auth']->getUsername()}' AND deleted=false ORDER BY id DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $deleteButtonAsString = null;
        if ($id == $row['id']) {
            $deleteButtonAsString = self::deleteQuickForm($id,$callBackURL); //show delete button
        }

        return $deleteButtonAsString;
    }

    function deleteQuickForm($timeSheetID,$callBackURL) {

        //$estrusID=85;

        $form = new HTML_QuickForm("timeSheetEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'timeSheetID', $timeSheetID);
        $form->addElement('submit', 'btnDelete', 'Delete');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $timeSheetID = $form->getElementValue('timeSheetID');

            $res = $GLOBALS['pdo']->query("UPDATE hr.time_sheet SET deleted='true' WHERE id='$timeSheetID'");

            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    /**
     * Method to change the color (to red) on fridays (end of pay period
     */
    function colorEndOfPayPeriod($datagrid, $tableBody, $numberPerPage) {
        //get an array of data in the datagrid
        $ds = $datagrid->getDataSource();
        $data = $ds->fetch();

        //go through each row and color it red if the cow hasn't been bred yet.
        $counter = 0;

        foreach ($data as $row) {


            if (date("N", strtotime($row['event_time'])) == 5) {
                $tableBody->setRowAttributes($counter, array('bgcolor' => '#FFDA7F'));
            }

            $counter++;
            if ($counter >= $numberPerPage) {
                break;
            } //only draw colors that cen be seen on first page, gets around a pear bug.
        }
    }

}

//end class
?>