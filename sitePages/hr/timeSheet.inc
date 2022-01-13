<?php

class TimeSheet extends BasePage {

    public function defaultDisplay() {

        //only show owner
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            $left = ( $this->showLast3WeeksPayrollForAllCurrentEmployees() );
            $right = $this->showLast3WeeksPayrollSummary() . $this->vacationYearlySummary() .$this->displayNotes();



            //   $this->displayPayrollView();
        } else { //employee view
            $right = $this->addRemoveTimeQuickform() . $this->displayNotes();
            $left = $this->displayTimeLog($GLOBALS['auth']->getUsername());
        }


        print (new BootStrap)->oneThirdTwoThirds($left, $right);
    }

    function displayNotes() {

        $out[] = '<li>Pay periods are from Saturday 00:00 to Friday 23:59.</li>';
        $out[] = '<li>Time Can only be banked for time over 9 hours in a single day.</li>';
        $out[] = '<li>Time Bank withdrawls can only be used to top up to 9 hours in a single day.</li>';
        $out[] = '<li>Time Bank must be used up by the end of the calendar year.</li>';

        $content = (implode($out));
        return( (new BootStrap)->generalInfoBox('fa-sticky-note', 'bg-info', 'Note', $content));
    }

    /*
     * shows what employee entered. Not what was actually paid. 
     */
    private function vacationYearlySummary(){
        
        $sql=<<<SQL
SELECT userid,count(explanation), explanation, (date_part('year', now() - interval '0' year))  as year
FROM hr.time_sheet_view 
WHERE deleted=false 
AND explanation =  ANY(ARRAY['Day Off','Vacation','For Holiday','Sick']::varchar[])
AND event_time BETWEEN date_trunc('year', now()) - interval '0' year AND (date_part('year', current_date) || '-12-31 23:59:59')::timestamp - interval '0' year
GROUP BY userid, explanation
UNION
SELECT userid,count(explanation), explanation, (date_part('year', now() - interval '1' year))  as year
FROM hr.time_sheet_view 
WHERE deleted=false 
AND explanation =  ANY(ARRAY['Day Off','Vacation','For Holiday','Sick']::varchar[])
AND event_time BETWEEN date_trunc('year', now()) - interval '1' year AND (date_part('year', current_date) || '-12-31 23:59:59')::timestamp - interval '1' year
GROUP BY userid, explanation
UNION
SELECT userid,count(explanation), explanation, (date_part('year', now() - interval '2' year))  as year
FROM hr.time_sheet_view 
WHERE deleted=false 
AND explanation =  ANY(ARRAY['Day Off','Vacation','For Holiday','Sick']::varchar[])
AND event_time BETWEEN date_trunc('year', now()) - interval '2' year AND (date_part('year', current_date) || '-12-31 23:59:59')::timestamp - interval '2' year
GROUP BY userid, explanation
ORDER BY userid,year DESC

SQL;        
        return ( (new JQueryDataTable)->startBasicSql('Time Off Summary Last 3 Years', $sql));
        
    }
    
    
    private function showLast3WeeksPayrollForAllCurrentEmployees() {

        $sql = <<<SQL
            SELECT distinct(userid) FROM hr.time_sheet_view WHERE event_time::date BETWEEN (current_date - interval '3 weeks') AND current_date ORDER by userid ASC    
        SQL;
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $out[] = $this->showLast3WeeksPayrollForSpecificEmployee($row['userid']);
        }

        return implode($out);
    }

    private function showLast3WeeksPayrollSummary() {

        $sql = <<<SQL
            SELECT distinct(userid) FROM hr.time_sheet_view WHERE event_time::date BETWEEN (current_date - interval '3 weeks') AND current_date ORDER by userid ASC    
        SQL;
        $res = $GLOBALS['pdo']->query($sql);
        $dataArr = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $employeeDataArr = $this->last3WeeksPayrollForSpecificEmployee($row['userid']);

            foreach ($employeeDataArr as $key => $value) {

                if ((!empty($value['Pay Period Balance'])) AND (strtotime($value['Date']) >= strtotime('-1 week'))) {
                    $rowOut['Date'] = $value['Date'];
                    $rowOut['Employee'] = $row['userid'];
                    $rowOut['Pay Period Balance'] = $value['Pay Period Balance'];
                    $dataArr[] = $rowOut;
                }
            }
        }

        return ( (new JQueryDataTable)->startBasicArray('To Be Paid Summary', $dataArr));
    }

    private function showLast3WeeksPayrollForSpecificEmployee($userid) {

        $dataArr = $this->last3WeeksPayrollForSpecificEmployee($userid);
        $out[] = '<style> table.dataTable { font-size:small;}</style>';
        $out[] = ( (new JQueryDataTable)->startBasicArrayColour('Time Sheet: ' . ucwords($userid), $dataArr, $options = 'sorting: false,paging: true, pageLength: 15', 'highlight_attention'));
        return implode($out);
    }

    private function last3WeeksPayrollForSpecificEmployee($userid) {

        $sql = <<<SQL
    
    /* generate a date series and join hr table */
with temp as (
SELECT series::date as date,to_char(series, 'Day') as day_of_week,date_trunc('WEEK',(series::date + interval '2 day'))- interval '2 day' AS week_starting
,xx.*
FROM generate_series( current_date - interval '3 months', current_date, '1 day'::interval) AS series
LEFT JOIN 
 (SELECT * FROM hr.time_sheet_view WHERE userid='$userid' AND deleted=FALSE ) xx
ON series=xx.event_time::date 
)
/*sum total time over the week period SAT to FRI. */
SELECT *, sum(time_banked) OVER (PARTITION BY (week_starting)) as sum_week_ending_total_time, sum(total_time) OVER (PARTITION BY (week_starting)) as sum_week_ending_total_time
FROM TEMP  
ORDER BY date DESC    
            
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDate($row['date']);
            $rowOut['Shift'] = $row['shift_type'];
            $rowOut['Basic Explanation'] = $row['explanation'];
            $rowOut['Actual Time Worked (hours)'] = $row['time_worked'];
            $rowOut['Time Bank (Deposit / Withdrawl)'] = $row['time_banked'];
            $rowOut['Time Bank Balance'] = $this->printTimeBankToDate($row['userid'], $row['event_time']);
            $rowOut['Pay Period Balance'] = (trim($row['day_of_week']) == 'Friday') ? $row['sum_week_ending_total_time'] : '';
            $rowOut['Overtime Explanation'] = $row['explanation_overtime'];
            $rowOut['colour_row'] = (trim($row['day_of_week']) == 'Friday') ? 1 : 0;

            //only show rows with info.
            if ((!empty($row['shift_type'])) OR (trim($row['day_of_week']) == 'Friday')) {
                $resultsOut[] = $rowOut;
            }
        }

        //output
        $dataArr = $resultsOut;

        return $dataArr;
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
            $this->displayTimeLog(($row['userid']), 'payrollView');
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
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesAtDailyIntervalsForLastSevenDays();
        $dateKeysArray = array_keys($dateArray); //so we can set today to teh default.


        $timeBankEvents = array(null => 'Please Select one...', 'add' => 'Add Time', 'remove' => 'Use Time');

        $listOfWorkTimes = $this->createListofTimesAt15minIntervals(0, 20);
        $listOfTimeBankTimes = $this->createListofTimesAt15minIntervals(-9, 6);

        $reason = array(null => 'Please Select one...', 'Vacation' => 'Vacation', 'Day Off' => 'Day Off', 'For Holiday' => 'For Holiday', 'Sick' => 'Sick');
        $shiftTypeArr = array(null => 'Please Select one...', 'Milking AM' => 'Milking AM', 'Milking PM' => 'Milking PM', 'Cropping' => 'Cropping');

        $form = new HTML_QuickForm("addRemoveTimeFromBankQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Hours Worked');

        $form->addElement('select', 'dateEvent', 'Date of Event', $dateArray);
        $form->addElement('select', 'shiftType', 'Shift Type', $shiftTypeArr);
        $form->addElement('select', 'workTime', 'Hours Actually Worked', $listOfWorkTimes);
        $form->addElement('select', 'bankTime', 'Hours to Deposit (+) / Withdrawl (-) From Bank', $listOfTimeBankTimes);
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
        $form->addRule(array('workTime', 'explanation_overtime'), 'ERROR: No Explanation for overtime worked.', 'checkExplanationForOvertime', array('workTime', 'explanation_overtime'));

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
            if (!array_key_exists('bankTime', $formData)) {
                $bankTime = 0;
            } else {
                $bankTime = $formData['bankTime'];
            }
            //
            if (!array_key_exists('workTime', $formData)) {
                $bankTime = 0;
            } else {
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

            if ($explanation_overtime == '') {
                $explanation_overtime = 'null';
            } else {
                $explanation_overtime = "'" . pg_escape_string($explanation_overtime) . "'";
            }


            //don't put id value, uses next one in DB. //use global username as security feature, instead of passing it.
            $query = "INSERT INTO hr.time_sheet (event_time,time_worked,time_banked,userid,deleted,explanation,shift_type,explanation_overtime) VALUES ('$event_date','$workTime','$bankTime','{$GLOBALS['auth']->getUsername()}','false','$explanation','$shiftType',$explanation_overtime)";

            $res = $GLOBALS['pdo']->exec($query); {
                print_r($res);
            }

            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    public function timeLog($request) {
        $useridtouse = Sanit::reqVal('item', false, 'STR');

        if ($useridtouse != false) {
            //for sercurity reasons,only allow give userid for when admin logged in.
            if ($GLOBALS['auth']->getOwnerAccess() == 1) {
                //when admin allow any userid.ie pass thorugh.
            } else { //employee view
                $useridtouse = $GLOBALS['auth']->getUsername(); //only user userid of user no matter what
            }

            // Setup your database connection
            $sql = "SELECT '$useridtouse' as useridtouse,explanation,shift_type,* FROM hr.time_sheet_view WHERE userid='$useridtouse' AND deleted=false AND event_time>= current_date - interval ' 2 years'  ORDER BY event_time DESC ";

            $statement = $GLOBALS['pdo']->prepare($sql);
            $statement->execute();
            $results = $statement->fetchAll(PDO::FETCH_ASSOC);

            //change presentation
            foreach ($results as &$row) {


                //  $row['full_reg_number'] = '<a href="' . $GLOBALS['MiscObj']->createCDNLink($row['full_reg_number']) . '">' . $row['full_reg_number'] . '</a>';
                $row['event_time_nice'] = date("l  M j, Y", strtotime($row['event_time']));
                $row['time_bank_balance'] = $this->printTimeBankToDate($row['useridtouse'], $row['event_time']);
                $row['pay_period_to_date'] = $this->printPayPeriodToDate($row['useridtouse'], $row['event_time']);
                $row['action'] = $this->printActionButtons($row['id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
                $row['colour_row'] = (date("N", strtotime($row['event_time'])) == 5) ? 1 : 0;
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


        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
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

        return($dt->start('Time Log', 70, 'timeLog', $useridtouse, $columnsStr, 'highlight_attention', false));
    }

    function printTimeBankToDate($useridtouse, $event_time) {

        $rowDate = strtotime($event_time);


        $endDate = date('Y-m-d H:i:sO', $rowDate);

        $sql = "SELECT sum(time_banked) as total_time_banked FROM hr.time_sheet_view where userid='{$useridtouse}' AND deleted=false AND event_time <= '$endDate' limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $returnVal = $row['total_time_banked'];
        }

        return $returnVal;
    }

    function printPayPeriodToDate($useridtouse, $event_time) {

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

    function printActionButtons($id, $callBackURL) {

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM hr.time_sheet WHERE userid='{$GLOBALS['auth']->getUsername()}' AND deleted=false ORDER BY id DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $deleteButtonAsString = null;
        if ($id == $row['id']) {
            $deleteButtonAsString = self::deleteQuickForm($id, $callBackURL); //show delete button
        }

        return $deleteButtonAsString;
    }

    function deleteQuickForm($timeSheetID, $callBackURL) {

        //$estrusID=85;

        $form = new HTML_QuickForm("timeSheetEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'timeSheetID', $timeSheetID);
        $form->addElement('submit', 'btnDelete', 'Delete', array('class' => 'btn btn-warning'));


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

}//end class