<?php 

class MedicalCase extends TabPage {

    static public $medicalSummaryLastUpdateArray; //holds the cache
    public $preparedStatement_ShowDeleteButton;

    public function __construct() {
        parent::__construct();

        // create some temp tables to speed things up.
        $sql = "CREATE OR REPLACE TEMP VIEW temp_medical_summary AS (SELECT * from bovinemanagement.medical_summary);";
        $res = $GLOBALS['pdo']->query($sql);
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    public function defaultDisplay() {


        //setup for tabs.
        $tabArray[1]['name'] = 'Current Cases';
        //$tabArray[1]['load_inline'] = '1'; //don't enable, too slow.
        $tabArray[2]['name'] = 'Historical Cases';
        $tabArray[3]['name'] = 'Medicine Administered Log';
        $tabArray[4]['name'] = 'Medicines';
        $tabArray[5]['name'] = 'Historical Analysis';
        $this->setCSStabs('jquery_tabs_treatment');
        $this->renderTabs($tabArray);
    }

    function showMedicalAccordion() {

        $accordionArray[1]['name'] = 'General Treatment';
        $accordionArray[2]['name'] = 'Mastitis Treatment';
        $accordionArray[3]['name'] = 'Temperature';
        $accordionArray[4]['name'] = 'Ketone';
        $accordionArray[5]['name'] = 'Magnet';
        $accordionArray[6]['name'] = 'Comment';
        $accordionArray[7]['name'] = 'Vet Check';
        $accordionArray[8]['name'] = 'Schedule Action';
        $accordionArray[9]['name'] = 'Diagnosis';
        $accordionArray[10]['name'] = 'Open/Close';

        $accordion = new AccordionImplementationMedicalQuickforms();
        $accordion->setCSS('accordionNarrow');
        $accordion->render($accordionArray);
    }

    function pageJavascript() {
        
    }

// end custom javascript

    protected function tab1() {
        $this->currentCases();
    }

    protected function tab2() {
        $this->displayMedicalCases('default', false);
    }

    protected function tab3() {
        $this->displayListOfMedecineAdministered();
    }

    protected function tab4() {
        $this->displayListOfMedecines();
    }
    
     protected function tab5() {
     
        $this->displayHistoricalData();
    }

    /**
     * everything in current cases tab
     */
    private function currentCases() {
        print($this->errorNoMedicalCaseOpenAndShouldBe());
         print("<div class='row'>");
       
        print("<div class='col-sm-4 col-sm-push-8'>");      
	  $this->showMedicalAccordion();
        print("</div>");
         print("<div class='col-sm-8 col-sm-pull-4'>");
         print($this->displayNext8HoursScheduledMedicines());
         $this->displayMedicalCases('default', true);
        print("</div>");
        print("</div>");

    }

    //checks for animals that have been given medicine and not had a case open. ignores dry off and vaccines and hormones.
    function errorNoMedicalCaseOpenAndShouldBe() {
        $str='';
        $sql = <<<SQL
with temp as (
SELECT distinct(local_number) bovine_id,local_number,full_name,count(medicine_administered.id)
 FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovinecurrall ON medicine_administered.bovine_id = bovinecurrall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
where medicine_class NOT IN ('hormone','vaccine','anti-protozoal')  AND for_dry_treatment is false  AND event_time >= ( current_date - interval '14 days') AND medical_case_id	is null and bovinecurrall.id is not null
group by medicine_administered.bovine_id,local_number,full_name
)
SELECT bovine_id,local_number,full_name,count from temp where count >2
SQL;
          $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $str=$str.BootStrap::warningNotify("Medical Case should be opened for: #{$row['local_number']}, {$row['count']} events in the last two weeks!");
        }
     return $str;
        
        
    }

    public function summaryDisplay() {
        // nothing
    }

    /**
     * shows grpahs of how many cases a year and when they happened.
     * 
     * 
     * 
     */
    function displayHistoricalData() {
        
        $this->plotMetabolicCowLastYearBarGraph();
        $this->plotCalfMaladiesLastYearBarGraph();
        print("<p>Masitis Max 25 cases per 100 cows per year. (Source: Valacta , Stirling)");
    }
     /**
     * plots a bar graph of medical cases COW.
     */
     
    public function plotMetabolicCowLastYearBarGraph() {
print("<h3>Cow Transisiton Maladies (last 2 years)</h3>");
        $array1 = $this->findMonthlyTotals('Milk Fever');
        $array2 = $this->findMonthlyTotals('Mastitis');
        $array3 = $this->findMonthlyTotals('Retained Placenta');
       // $array4 = $this->findMonthlyTotals('Unknown');


        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Milk Fever'] = 'number';
        $headerStrArr['Mastitis'] = 'number';
        $headerStrArr['Retained Placenta'] = 'number';
         $opt='';
        
        $x = new GoogleVisColumn($opt, $headerStrArr, $array1, $array2, $array3);
        print($x->toString());
    }

    private function findMonthlyTotals($diagnosis) {

        /* find all the COWS (over 450 days old) who died (or whatever) in the last year. */
        $sql = "
  With temp as (
SELECT open_date,bovine_id, (SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN bovinemanagement.medical_diagnosis_type ON medical_diagnosis_type.id=diagnosis_type_id WHERE medical_case_id=medical_case.id ORDER BY medical_diagnosis.event_time DESC limit 1) as diagnosis, date_trunc('month',open_date) as month,1 as amount 
FROM bovinemanagement.medical_case
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
WHERE open_date >= now() - interval ' 2 year' AND (open_date-birth_date) > 450
)
SELECT 
DISTINCT(month),diagnosis,sum(amount) OVER(PARTITION BY month,diagnosis) as total 
FROM temp WHERE diagnosis='$diagnosis'

ORDER BY month
";
        $arrayDiagnosis = null;

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $arrayDiagnosis[strtotime($row['month'])] = $row['total'];
        }
     return $arrayDiagnosis;
    }
    /***********************************/
      public function plotCalfMaladiesLastYearBarGraph() {
print("<h3>Calf Maladies (last 2 years)</h3>");
        $array1 = $this->findMonthlyTotalsCalf('Scours');
        $array2 = $this->findMonthlyTotalsCalf('Pneumonia');
        $array3 = $this->findMonthlyTotalsCalf('Unknown');
       // $array4 = $this->findMonthlyTotals('Unknown');


        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Scours'] = 'number';
        $headerStrArr['Pneumonia'] = 'number';
        $headerStrArr['Unknown'] = 'number';
         $opt='';
        
        $x = new GoogleVisColumn($opt, $headerStrArr, $array1, $array2, $array3);
        print($x->toString());
    }

    private function findMonthlyTotalsCalf($diagnosis) {

        /* find all the COWS (over 450 days old) who died (or whatever) in the last year. */
        $sql = "
  With temp as (
SELECT open_date,bovine_id, (SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN bovinemanagement.medical_diagnosis_type ON medical_diagnosis_type.id=diagnosis_type_id WHERE medical_case_id=medical_case.id ORDER BY medical_diagnosis.event_time DESC limit 1) as diagnosis, date_trunc('month',open_date) as month,1 as amount 
FROM bovinemanagement.medical_case
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
WHERE open_date >= now() - interval ' 2 year' AND (open_date-birth_date)  <= 450
)
SELECT 
DISTINCT(month),diagnosis,sum(amount) OVER(PARTITION BY month,diagnosis) as total 
FROM temp WHERE diagnosis='$diagnosis'

ORDER BY month
";
        $arrayDiagnosis = null;

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $arrayDiagnosis[strtotime($row['month'])] = $row['total'];
        }
     return $arrayDiagnosis;
    }
    
    
    
    
    /**
     * ********************************
     */
    /**
     * List of All medicines
     */

    /**
     * ********************************
     */
    function displayListOfMedecines($mode = 'default') {

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
        $sql = "SELECT * FROM bovinemanagement.medicine ORDER by trade_name";
        $dg = new DataGridLR($sql, 1000);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Name', 'trade_name', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Local Instructions', 'local_instructions', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Med Class', 'medicine_class', '', array('' => ''), null, null));

        print($dg->render());
        $dg->pager();
    }

    
    
    function displayNext8HoursScheduledMedicines() {
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
                data: "paticularTreatmentInfo",
                title: "Particular Treatment Info"
            },
        
            {
                data: "action2",
                title: "Action 2"
            }
EX;
        print($dt->start(64, 'next8HoursScheduledMedicines', $mode, $columnsStr));
        

        //    print($dt->startLocal($this->AllHeats7Days(''),$columnsStr,null,' "paging": false'));     
    }
    
    
    function next8HoursScheduledMedicines() {
         $sql = "
SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type=ANY(ARRAY['Scheduled Meds','Scheduled Action']) AND event_time <= (current_timestamp + interval '8 hours')	AND event_time >= (current_timestamp - interval '8 hours')	
		ORDER BY medical_summary.event_time ASC
   "; 
         
          $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['paticularTreatmentInfo'] =  $this->printparticularTreatmentInfoJQ($row);
        $row['action2'] = MedicineScheduled::printActionButtonMedicalScheduledComplete($row['id'],null,$row['bovine_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    
    /**
     * display list of medicines scheduled in the next 8 hours.
     */
    function displayNext8HoursScheduledMedicinesOld() {

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
        $sql = "
SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type='Scheduled Meds' AND event_time <= (current_timestamp + interval '8 hours')	
		ORDER BY medical_summary.event_time ASC
   ";
        $dg = new DataGridLR($sql, 1000);

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Scheduled Meds', null, '', array('' => ''), null, array('MedicalCase', 'printparticularTreatmentInfo()')));
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Action2', 'Action2', 'Action2', array('' => ''), null, array('MedicineScheduled', 'printActionButtonMedicalScheduledComplete()')));


        $str = '<div id="displayNext10HoursScheduledMedicines" id=\'scrollable\'>' . $dg->render('datagrid datagridNested', false) . '</div>'; // wrap in
        // add an extra space when there is data to show.
        if (strlen($str) >= 28) {
            $str = $str . '</br>';
        }
        return $str;
    }

    /**
     * ********************************
     */
    /**
     * Display list of all medical cases
     */

    /**
     * ********************************
     */
    function displayMedicalCases($mode = 'default', $current = true) {



        // Current cases
        if ($current == true) {
            $numberPerPage = 1000;
            $sql = "SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovinecurrall.full_name, bovinecurrall.local_number, bovinecurrall.location_name, date_trunc('day',current_date-bovinecurrall.fresh_date) as dim FROM bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=medical_case.bovine_id WHERE close_date is NULL ORDER BY local_number,open_date";
        } else {
            $numberPerPage = 10;
            // historical cases
            $sql = "SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovine.full_name, bovine.local_number FROM bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovine ON bovine.id=medical_case.bovine_id ORDER BY open_date DESC,local_number";
        }
        $res = $GLOBALS['pdo']->query($sql);
        if (($res->rowCount() == 0) && ($current == true)) {
            print ('<h3>No Current Medical Cases</h3>');
        }

        $dg = new DataGridLR($sql, $numberPerPage);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumberLarge()')));

        if ($current == false) {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Case #', 'id', '', array('' => ''), null, null));
        }
        if ($current == true) {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Info', null, '', array('' => ''), null, array('MedicalCase', 'printBovineMedicalCaseInfo()')));
        }
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Open Date', null, '', array('' => ''), null, array('MedicalCase', 'printOpenDate()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Treatments', null, '', array('' => ''), null, array('MedicalCase', 'printMedecineAdministeredForAPaticularMedicalCase()')));

        if ($current == false) {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Close Date', null, '', array('' => ''), null, array('MedicalCase', 'printCloseDate()')));
        }

        print($dg->render('datagrid datagridParent'));

        if ($mode == 'default') {

            $dg->pager();
        }
    }

    /**
     * *****************************************
     */
    /**
     * Shows extra information for each animal *
     */

    /**
     * *****************************************
     */
    function printBovineMedicalCaseInfo($params) {
        extract($params);

        // find out when milk and beef witholding is:
        $res = $GLOBALS['pdo']->query("SELECT milk_withholding,beef_withholding FROM bovinemanagement.milk_and_beef_withholding({$record['bovine_id']})");
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $record ['location_name'];

        $str = '';
        $str = $str . '' . $record ['location_name'] . '<br/>';
        if ($record ['dim'] != '') {
            $str = $str . 'dim: ' . $record ['dim'] . '<br/>';
        }
        // only show milk withholding if it is in the future, else it doesn't
        // apply
        if (strtotime($row ['milk_withholding']) > strtotime("now")) {
            $str = $str . '<span id=\'colourBack\'>milk withhold until: ' . date('D M j/Y ga', strtotime($row ['milk_withholding'])) . '</span><br/>';
        }
        // only show beef withholding if it is in the future, else it doesn't
        // apply
        if (strtotime($row ['beef_withholding']) > strtotime("now")) {
            $str = $str . 'beef withhold until: ' . date('D M j/Y ga', strtotime($row ['beef_withholding'])) . '<br/>';
        }

        // show too many treatments warning
        $str = $str . self::maximumTreatmentsMastitis($record['id']);
//print_r($params);
        return $str;
    }

    /* this checks if for the open medical case the bovine has already been 
     * treated the maximum in a certain quarter, if so display a message
     */

    function maximumTreatmentsMastitis($medical_case_id) {
        $str = '';
        $sql = "          
SELECT distinct  location,medicine_index,trade_name, count(event_time) OVER (PARTITION BY location,medicine_index)
 from bovinemanagement.medicine_administered 
LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index
WHERE medical_case_id=$medical_case_id AND for_mastitis=true
";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            if ($row['count'] >= 4) {
                $str = $str . "<span id='#colourBackYellow'>Max Treatment Reached: {$row['trade_name']} at {$row['location']}. Take milk Sample.</span><br/>";
            }
        }
        return $str;
    }

    /**
     * NESTED *
     */
    function printMedecineAdministeredForAPaticularMedicalCase($params) {
        extract($params);


        /*
         *   show scheduled events more than 8 hours in the future as its own
         *   datagrid, so we can hide it with js.
         */
        $sql1 = "
      SELECT id,event_time,type,text,userid,bovine_id 
         FROM temp_medical_summary 
         WHERE temp_medical_summary.bovine_id={$record['bovine_id']} 
            AND (type='Scheduled Meds' OR type='Scheduled Action')
            AND event_time > (current_timestamp + interval '8 hours')
         ORDER BY event_time DESC ,type ASC 
   ";

        $res1 = $GLOBALS['pdo']->query($sql1);
        $numFutureRow = $res1->rowCount();

        $strFuture = '';
        //only run if there are actually future events.
        if ($numFutureRow > 0) {

            $uniqueID = uniqid(); //so script knows which link we are referring too.
            //jquery  toggle code is in header to control showing of futrue events.
            $toggle = "<script type='text/javascript' language='javascript'>
            $(document).ready(function(){ 
            
            $('#farFutureTreatment_$uniqueID').toggle(); //hide element when page loads.
            
            $('#farFutureTreatmentToggle_$uniqueID').click(function(){ 
        
             
             $('#farFutureTreatment_$uniqueID').toggle(); //show
            
             })
             });
             </script>
           ";
            print($toggle);

            $dg1 = new DataGridLR($sql1, 1000);
            $dg1->datagrid->addColumn(new Structures_DataGrid_Column('All', null, '', array('' => ''), null, array('MedicalCase', 'printparticularTreatmentInfo()')));
            //don't show because we have sql for delete button coded to not allow deletes of scheduled meds
            //$dg1->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('MedicalCase', 'printActionButtons()'))); 
            $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Action2', null, '', array('' => ''), null, array('MedicineScheduled', 'printActionButtons2()')));
            $strFuture = $strFuture . "<a id='farFutureTreatmentToggle_$uniqueID' href='#'><b id='boldLightBlue'>$numFutureRow Future Event(s) --></b></a>";
            $strFuture = $strFuture . "<div id='farFutureTreatment_$uniqueID'>" . $dg1->render('datagrid datagridNested') . '</div>';
        }


        /*
         * NESTED datagrid
         */



        $sql = "
    (SELECT id,event_time,type,text,userid,bovine_id FROM temp_medical_summary WHERE temp_medical_summary.medical_case_id={$record['id']})
      UNION
    (SELECT id,event_time,type,text,userid,bovine_id FROM temp_medical_summary WHERE temp_medical_summary.bovine_id={$record['bovine_id']} AND (type='Scheduled Meds' OR type='Scheduled Action')
     AND event_time <= (current_timestamp + interval '8 hours')) ORDER BY event_time DESC ,type ASC 
   ";
        $stmt2 = $GLOBALS['pdo']->query($sql);
        $arrayDG = $stmt2->fetchAll(PDO::FETCH_ASSOC);

        //need to pass as an array, because if we pass sql directly to nested datagrid, it can't seem to see the temporary table.

        $dg3 = new DataGridLRArray($arrayDG, 1000);

        $dg3->datagrid->addColumn(new Structures_DataGrid_Column('All', null, '', array('' => ''), null, array('MedicalCase', 'printparticularTreatmentInfo()')));
        $dg3->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('MedicalCase', 'printActionButtons()')));
        //render both datagrids, but put a seperate div around the far future ones,so we can hide, them.
        $str = '<div id=\'scrollable\'>' . $strFuture . $dg3->render('datagrid datagridNested') . '</div>'; // wrap in


        return $str;
    }

    //jquery version of old datagrid function, keep this one in future
    function printparticularTreatmentInfoJQ($record) {
 
        //colour temperature values who are out of the ordinary
        if ($record ['type'] == 'Temperature') {

            //38.05 C to 39.16 is considered normal, not cited because the experts do not agree.
            if (($record ['text'] >= 38.05) && ($record ['text'] <= 39.16)) {
                //colour green
                $record ['text'] = '<b id="boldGreen">' . $record ['text'] . '</b>';
            } else {  //abnormal
                $record ['text'] = '<b id="boldRed">' . $record ['text'] . '</b>';
            }
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
    
    //DELETE me when datagrid gone
    function printparticularTreatmentInfo($params) {
        extract($params);

        //colour temperature values who are out of the ordinary
        if ($record ['type'] == 'Temperature') {

            //38.05 C to 39.16 is considered normal, not cited because the experts do not agree.
            if (($record ['text'] >= 38.05) && ($record ['text'] <= 39.16)) {
                //colour green
                $record ['text'] = '<b id="boldGreen">' . $record ['text'] . '</b>';
            } else {  //abnormal
                $record ['text'] = '<b id="boldRed">' . $record ['text'] . '</b>';
            }
        }


        $event_time = date('D M j/Y ga', strtotime($record ['event_time']));
        $str = '<div class=\'smallish\'>' . $event_time . '&nbsp;&nbsp;&nbsp;&nbsp;' . 'by ' . $record ['userid'] . '</div> ' . '<b>' . $record ['type'] . ':</b> ' . $record ['text'];

        $x = (strtotime($record ['event_time']) - strtotime('now'));

        // special case to draw attention to events less than 8 hours away OR
        // events that have passed that are scheduled meds. ANNOYING.
        if ((($x < 28800) and ( $x > 0)) or ( ($x < 0) and ( $record ['type'] == 'Scheduled Meds'))) {
            $str = '<b id="boldBlue"><b id="boldBlink">' . $str . '</b></b>';
        }
        //if it is more than 8 hours in te future.
        elseif (($x >= 28800) AND ( $record ['type'] == 'Scheduled Meds')) {
            $str = '<b id="boldLightBlue">' . $str . '</b>';
        }

        return $str;
    }

    function printOpenDate($params) {
        extract($params);
        return date('M j, Y', strtotime($record ['open_date']));
    }

    /**
     * ********************************
     */
    /**
     * Open a new Medical Case
     */

    /**
     * ********************************
     */
    public static function openNewMedicalCaseQuickForm() {

        // grab a list of cows who don't have a medical case already open..
        $res = $GLOBALS['pdo']->query("SELECT bovinecurrall.id,full_name,local_number FROM bovinemanagement.bovinecurrall EXCEPT (SELECT bovine_id,full_name,local_number from bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovine ON bovine.id=medical_case.bovine_id WHERE medical_case.close_date is NULL) ORDER BY local_number");
        {
          
        } // Always check that
        // result is not an error
        $aniNumber [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            // parse for last word of name and then make an array for the select
            // list.
            $aniNumber [$row [0]] = $row [2] . " - " . Misc::femaleShortName($row[1]);
        }

        // create diagnonis array
        $diagnosisArray = self::createDiagnosisArray();


        $form = new HTML_QuickForm("openNewMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        

        $form->addElement('header', 'hdrQuickform', 'Open New Medical Case');
        $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
        $form->addElement('select', 'diagnosis', 'Diagnosis', $diagnosisArray);
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('animalNumber' => 0)); // set default to no
        // variety.
        // rules
        $form->addRule('animalNumber', 'Animal number is required', 'required');
        $form->addRule('diagnosis', 'Diagnosis is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $bovine_id = $form->exportValue('animalNumber');
            $event_time = date('Y-m-d H:i:sO', strtotime("now"));
            $pageid = $form->exportValue('pageid');
            $diagnosis_type_id = $form->exportValue('diagnosis');

            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_case (bovine_id,open_date,open_userid) VALUES ($bovine_id,'$event_time','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);
            $medical_case_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.medical_generic_id_seq');

            // insert diagnosis for the just created medical case.
            $query = "INSERT INTO bovinemanagement.medical_diagnosis (medical_case_id,bovine_id,event_time,diagnosis_type_id,userid) VALUES ($medical_case_id,$bovine_id,'$event_time',$diagnosis_type_id,'{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);

            //go through all medicines administerd in last few weeks and update them to add the case, excluding dry treatments hormones 
            $query2 = "

UPDATE bovinemanagement.medicine_administered SET medical_case_id=$medical_case_id
FROM
(
SELECT medicine_administered.id
FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
WHERE bovine_id = $bovine_id AND  medicine_class NOT IN ('hormone','vaccine')  AND for_dry_treatment is false  AND event_time >= ( current_date - interval '14 days')  AND medical_case_id IS NULL
) AS subquery
WHERE subquery.id = medicine_administered.id;                
";
            $res = $GLOBALS['pdo']->exec($query2);
            
            
            
            // determine if the commit or rollback
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            
            // Close the transaction
            // $_SESSION['selectedTab'] = 2;
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    public static function grabListCowsWithOpenMedicalCase() {

        // grab a list of cows with open medical cases.
        $res = $GLOBALS['pdo']->query("SELECT bovine.id,bovine.full_name,bovine.local_number,medical_case.open_date from bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovine ON medical_case.bovine_id=bovine.id WHERE close_date is null ORDER BY local_number");
        {
          
        } // Always check that
        // result is not an error
        $aniNumber [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            // parse for last word of name and then make an array for the select
            // list.
            $aniNumber [$row [0]] = $row [2] . " - " . Misc::femaleShortName($row[1]) . ' - ' . $row [3];
        }
        return $aniNumber;
    }

    /**
     * ********************************
     */
    /**
     * Close an open medical case
     */

    /**
     * ********************************
     */
    public static function closeMedicalCaseQuickForm() {

        // grab a list of cows with open medical cases.
        $aniNumber = self::grabListCowsWithOpenMedicalCase();

        $form = new HTML_QuickForm("closeMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
       

        $form->addElement('header', 'hdrQuickform', 'Close Medical Case');
        $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('animalNumber' => 0)); // set default to no
        // variety.
        // rules
        $form->addRule('animalNumber', 'Animal number is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $bovine_id = $form->exportValue('animalNumber');
            $pageid = $form->exportValue('pageid');
            $event_time = date('r', strtotime("now"));

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = self::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

            // don't put id value, uses next one in DB.

            $query = "UPDATE bovinemanagement.medical_case SET close_date='$event_time',close_userid='{$GLOBALS['auth']->getUsername()}' WHERE id=$medical_case_id";


            $res = $GLOBALS['pdo']->exec($query);

            // $_SESSION['selectedTab'] = 2;
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    function printCloseDate($params) {
        extract($params);
        if ($record ['close_date'] != null) {
            return date('M d, Y', strtotime($record ['close_date'])) . " by " . $record ['close_userid'];
        }
    }

    /**     * ******************************* */
    /** Medical Case Ketone           */

    /**     * ******************************* */
    public static function newKetoneForMedicalCaseQuickForm($mode = 'group', $bovine_id = null) {



        $timeArray = Misc::createHourlyDatesForOnlyToday();
        unset($timeArray[null]); //remove null value.
        //create array of celcius temps.
        $ketoneArray = array(null => 'Please Select One');
        for ($counter = 0; $counter <= 20.1; $counter += 0.10) {
            $value = round($counter, 1); //mmol/liter
            $ketoneArray["$value"] = $value;
        }


        $form = new HTML_QuickForm("newKetoneForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Ketone');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //depending on if we are called from animal query page or not.
        if ($mode == 'group') {
            $aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase();     //grab a list of cows with open medical cases.
            $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
            $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        } elseif ($mode == 'individual') {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        } else {
            throw new Exception("Group or Individual Mode must be selected for ketone test.");
        }


        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);
        $form->addElement('select', 'ketone', 'Ketone (mmol/liter)', $ketoneArray);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');



        //rules
        $form->addRule('animalNumber', 'Animal number is required', 'required');
        $form->addRule('ketone', 'Ketone is required', 'required');
        $form->addRule('dateEvent', 'Event Time is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('animalNumber');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');
            $ketone = $form->exportValue('ketone');

            //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_ketone (medical_case_id,bovine_id,event_time,ketone,userid) VALUES ($medical_case_id,$bovine_id,'$event_time','$ketone','{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id={$_REQUEST['bovine_id']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    /**     * ******************************* */
    /** Medical Case Magnet           */

    /**     * ******************************* */
    public static function newMagnetForMedicalCaseQuickForm($mode = 'group', $bovine_id = null) {

        $timeArray = Misc::createHourlyDatesForOnlyToday();
        unset($timeArray[null]); //remove null value.


        $form = new HTML_QuickForm("newMagnetForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Magnet');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //depending on if we are called from aninmal query page or not.
        if ($mode == 'group') {
            $aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase();     //grab a list of cows with open medical cases.
            $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
            $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        } elseif ($mode == 'individual') {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        } else {
            throw new Exception("Group or Individual Mode must be selected for magnet test.");
        }


        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);
        $form->addElement('hidden', 'magnet', 'true');

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');



        //rules
        $form->addRule('animalNumber', 'Animal number is required', 'required');
        $form->addRule('magnet', 'Magnet is required', 'required');
        $form->addRule('dateEvent', 'Event Time is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('animalNumber');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');
            $magnet = $form->exportValue('magnet');

            //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_magnet (medical_case_id,bovine_id,event_time,magnet,userid) VALUES ($medical_case_id,$bovine_id,'$event_time','$magnet','{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id={$_REQUEST['bovine_id']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    /**
     * ********************************
     */
    /**
     * Medical Case Comment
     */

    /**
     * ********************************
     */
    public static function newCommentForMedicalCaseQuickForm($mode = 'group', $bovine_id = null) {

        $timeArray = Misc::createHourlyDatesForOnlyToday();
        unset($timeArray [null]); // remove null value.
        // list of pre-canned comments
        $treatmentComment = array(null => 'Please Select one...', 'Clear, not treated.' => 'Clear, not treated.', 'Observed to not be eating.' => 'Observed to not be eating.', 'Broken Needle.' => 'Broken Needle.');

        $form = new HTML_QuickForm("newCommentForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
       

        $form->addElement('header', 'hdrQuickform', 'Medical Comment');
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        // depending on if we are called from aninmal query page or not.
        if ($mode == 'group') {
            $aniNumber = self::grabListCowsWithOpenMedicalCase(); // grab a list of
            // cows with open medical
            // cases.
            $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
        } else {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        }

        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);

        $form->addElement('select', 'comment1', 'Canned Comment', $treatmentComment);
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
            $medical_case_id = self::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_comment (medical_case_id,bovine_id,event_time,comment,userid) VALUES ($medical_case_id,$bovine_id,'$event_time','$comment_esc','{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    // check at least one comment was filled in.
    function commentRequired($valueArr, $elementNameArr) {
        // generic loop to convert $form elements we want into a usuable array
        // I really hope this isn't how quickforms was designed.
        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData [$elementName] = $valueArr [$count];
            $count++;
        }

        // only return false when no comment was entered or selected.
        if ((strlen(trim($formData ['comment1'])) != 0) || (strlen(trim($formData ['comment2'])) != 0)) {
            return true;
        } else {
            return false;
        }
    }

    public static function lookupOpenMedicalCaseForAnimal($bovine_id, $event_time) {

        // lookup bovine id and see if there is an open medical case during the
        // requested event_time
        $sql = "SELECT id as medical_case_id FROM bovinemanagement.medical_case WHERE bovine_id=$bovine_id AND open_date<='$event_time' AND close_date is NULL";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        if (is_numeric($row ['medical_case_id']) == true) {
            $medical_case_id = $row ['medical_case_id'];
        } else {
            $medical_case_id = 'null';
        }
        return $medical_case_id;
    }

    /**
     * ********************************
     */
    /**
     * Medical Case Temperature
     */

    /**
     * ********************************
     */
    public static function newTemperatureForMedicalCaseQuickForm($mode = 'group', $bovine_id = null) {


        // grab a list of cows with open medical cases.
        if ($mode == 'group') {
            $aniNumber = self::grabListCowsWithOpenMedicalCase();
        }

        $timeArray = Misc::createHourlyDatesForOnlyToday();
        unset($timeArray [null]); // remove null value.
        // create array of celcius temps.
        $temperatureArray = array(null => 'Please Select One');
        for ($counter = 36.5; $counter <= 42.1; $counter += 0.10) {
            $farenheight = round(((9 / 5) * ($counter) + 32), 1);
            $celcius = round($counter, 1);
            $temperatureArray ["$celcius"] = "$celcius &#8451; &nbsp; / &nbsp; $farenheight &deg;F";
        }

        $form = new HTML_QuickForm("newTemperatureForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        

        $form->addElement('header', 'hdrQuickform', 'Temperature');

        if ($mode == 'group') {
            $form->addElement('select', 'animalNumber', 'Animal', $aniNumber);
        } else {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        }

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);
        $form->addElement('select', 'temperature', 'Temperature', $temperatureArray);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        if ($mode == 'group') {
            $form->setDefaults(array('animalNumber' => 0)); // set default to no variety.
        }

        // rules
        if ($mode == 'group') {
            $form->addRule('animalNumber', 'Animal number is required', 'required');
        }
        $form->addRule('temperature', 'Temperature is required', 'required');
        $form->addRule('dateEvent', 'Event Time is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $bovine_id = $form->exportValue('animalNumber');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray [0]);
            $pageid = $form->exportValue('pageid');
            $temperature = $form->exportValue('temperature');

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = self::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

try {
            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_temperature (medical_case_id,bovine_id,event_time,temperature,userid) VALUES ($medical_case_id,$bovine_id,'$event_time','$temperature','{$GLOBALS['auth']->getUsername()}')";
 $res = $GLOBALS['pdo']->exec($query);
}
catch (\PDOException $e) {
    if ($e->errorInfo[1] == 23505) {
print("Warning: Already in db.");
    }
}

           


            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    private function createDiagnosisArray() {
        // create diagnonis array
        $sql = "SELECT id,diagnosis FROM bovinemanagement.medical_diagnosis_type ORDER BY diagnosis";
        $res = $GLOBALS['pdo']->query($sql);
        $diagnosisArray = array(null => 'Please Select One');
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $diagnosisArray [$row ['id']] = $row ['diagnosis'];
        }

        return $diagnosisArray;
    }

    /**
     * ********************************
     */
    /**
     * Medical Case Diagnosis
     */

    /**
     * ********************************
     */
    public static function newDiagnosisForMedicalCaseQuickForm() {

        // grab a list of cows with open medical cases.
        $aniNumber = self::grabListCowsWithOpenMedicalCase();
        $timeArray = Misc::createHourlyDatesForOnlyToday();

        unset($timeArray [null]); // remove null value.
        // create diagnonis array
        $diagnosisArray = self::createDiagnosisArray();

        $form = new HTML_QuickForm("newDiagnosisForMedicalCaseQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
       

        $form->addElement('header', 'hdrQuickform', 'Diagnosis');
        $form->addElement('select', 'animalNumber', 'Animal Number', $aniNumber);
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);
        $form->addElement('select', 'diagnosis', 'Diagnosis', $diagnosisArray);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('animalNumber' => 0)); // set default to no
        // variety.
        // rules
        $form->addRule('animalNumber', 'Animal number is required', 'required');
        $form->addRule('diagnosis', 'Diagnosis is required', 'required');
        $form->addRule('dateEvent', 'Event Time is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $bovine_id = $form->exportValue('animalNumber');
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray [0]);
            $pageid = $form->exportValue('pageid');
            $diagnosis_type_id = $form->exportValue('diagnosis');

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = self::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);

            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_diagnosis (medical_case_id,bovine_id,event_time,diagnosis_type_id,userid) VALUES ($medical_case_id,$bovine_id,'$event_time',$diagnosis_type_id,'{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            // $_SESSION['selectedTab'] = 2;
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    
    

    

    /**
     * ***********************************
     */
    /**
     * Generic method for changing *
     */
    /**
     * doage amounts to default *
     */

    /**
     * ********************************
     */
    public static function javascriptForMedDefaults($dosageArray, $defaultDosageArray, $defaultBodyLocationArray) {

        // create javascript to change dosage to default dosage based on a
        // select statement.
        print ('<script type="text/javascript">
			var aryContent = new Array();
			var aryContent2 = new Array();
			var aryContent3 = new Array();');

        foreach ($dosageArray as $key => $value) {
            print ("aryContent['$key'] = '($value)';");
        }
        foreach ($defaultDosageArray as $key => $value) {
            print ("aryContent2['$key'] = '$value';");
        }
        foreach ($defaultBodyLocationArray as $key => $value) {
            print ("aryContent3['$key'] = '$value';");
        }

        print ("\n\r" . '
			function updateContent(obj){

/* change doage units in label */
$("label[for=\'dosage-0\']").contents().first()[0].textContent ="Dosage "+aryContent[ obj[obj.selectedIndex].value ];

/* change dosage input default value */
$(\'#dosage-0\').val(aryContent2[ obj[obj.selectedIndex].value ]);

/* change body location input to default value */
$(\'#bodyLocation-0\').val(aryContent3[ obj[obj.selectedIndex].value ]);		
			}
                 
  </script>
' . "\n\r");
    }

    //schedule a medical action.
    public static function medicalActionScheduleQuickForm($mode = 'group', $bovine_id = null) {

        // grab a list of cows with open medical cases.
        if ($mode == 'group') {
            $aniNumber = self::grabListCowsWithOpenMedicalCase();
        }

        // grab a list of action types'.
        $actionArr [null] = "Please Select one..."; // dummy value
        $dosageArray [null] = ""; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,action from bovinemanagement.medical_action_type ORDER BY action");
        {
          
        } // Always check that
        // result is not an error

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $actionArr[$row['id']] = $row['action'];
        }


        $timeArray = Misc::createHourlyDatesForNextSevenDays();
        unset($timeArray [null]); // remove null value.

        $form = new HTML_QuickForm("medicalActionScheduleQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
    

        $form->addElement('header', 'medicalActionScheduleQuickForm', "Schedule Action");
        //
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        if ($mode == 'group') {
            $form->addElement('select', 'animalNumber', 'Animal', $aniNumber);
        } else {
            $form->addElement('hidden', 'animalNumber', $bovine_id);
        }

        $form->addElement('select', 'action', 'Action', $actionArr);

        $form->addElement('select', 'event_time', 'Event Time', $timeArray);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // rules
        if ($mode == 'group') {
            $form->addRule('animalNumber', 'Animal number is required', 'required');
        }
        $form->addRule('action', 'Action is required', 'required');

        $form->addRule('event_time', 'Event Time is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            // get values
            $bovine_id = $form->exportValue('animalNumber');
            $medicine_id = $form->exportValue('action');

            //
            $tempArray = $form->getElementValue('event_time');
            $event_time = date('Y-m-d H:i:sO', $tempArray [0]);

            $pageid = $form->exportValue('pageid');

            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = self::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);
    


            // record actiona schedule
            $query = "INSERT INTO bovinemanagement.medical_action (bovine_id,medical_Action_type_index,scheduled_event_time,medical_case_id,scheduled_userid)
	  VALUES ($bovine_id,$medicine_id,'$event_time',$medical_case_id,'{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);

            // determine if the commit or rollback

               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            
            }
            // Close the transaction
            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

   
    /**
     * *******************************************
     */
    /**
     * Display list of all medicine Administered
     */

    /**
     * *******************************************
     */
    function displayListOfMedecineAdministered($mode = 'default') {

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
        $sql = "SELECT local_number  as \"local number\",full_name,trade_name,dosage,dosage_unit  as \"dosage unit\",location,note,userid,event_time  as \"event time\",close_date,scheduled_event_time as \"scheduled event time\",scheduled_userid  as \"scheduled userid\"
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovinecurrall ON medicine_administered.bovine_id = bovinecurrall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  ORDER BY medicine_administered.event_time DESC";
        $dg = new DataGridLR($sql, 50);
        print($dg->render());
        $dg->pager();
    }

    function printActionButtons($params) {
        // print_r2($params);
        extract($params);
        $currentUserid = $GLOBALS ['auth']->getUsername();

        // find the last entry for the current user and use that as a basais to
        // display the delete button or not.
        // We only want the delete button shown once for the latest entry.
        // print latest delete button (one) for current user
        // the id is unique for this view, because of the counter we used.
        // don't show for scheduled meds, because the delete involves google
        // calendar.
        //this seems to be the fastest way, caching doesn't help.
        $res = $GLOBALS['pdo']->query("SELECT id,tablename FROM temp_medical_summary WHERE userid='$currentUserid' AND type!='Scheduled Meds' ORDER BY update_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $returnButtonsAsString ='';
        if (isset($record['close_date'])) {
        if (($record ['id'] == $row['id']) && ($record ['close_date'] == null)) {
            $returnButtonsAsString = self::deleteQuickForm($row['id'], $row['tablename']); // show
            //delete button, with _____ table id as index for delete.
        }}


        // print button
        return $returnButtonsAsString;
    }

    static function deleteQuickForm($tableid, $tablename) {

        $form = new HTML_QuickForm("genericEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
       

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete');

        // FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY
        // USER.
        // DB hack works though, views need to support deletes before this can
        // be fixed.
        // Try to validate a form
        if ($form->validate()) {

            // get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');

            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            // delete the dryoff_event
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.$tablename where id='$tableid'");

            // determine if the commit or rollback

               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
           

            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id={$_REQUEST['bovine_id']}");
            exit();
        }
        $form->accept($renderer);
        return ($renderer->toHtml()); // pass on as html string to display later in the
        // table
    }

    /**
     * small method to return body locations, use true for quickform select
     * lists.
     */
    public static function medicalBodyLocations($qf = false) {
        if ($qf = true) {
            $locationArray [null] = "Please Select one..."; // dummy value
        }

        $locationArray ['oral'] = 'oral';
        $locationArray ['subcutaneous'] = 'subcutaneous';
        $locationArray ['intramuscular'] = 'intramuscular';
        $locationArray ['intravenous'] = 'intravenous';
        $locationArray ['intraperitoneal'] = 'intraperitoneal';
        $locationArray ['front left teat canal'] = 'front left teat canal';
        $locationArray ['front right teat canal'] = 'front right teat canal';
        $locationArray ['front left teat canal'] = 'front left teat canal';
        $locationArray ['rear right teat canal'] = 'rear right teat canal';
        $locationArray ['intravaginal'] = 'intravaginal';
        $locationArray ['skin'] = 'skin';

        return $locationArray;
    }

    
}//end class

/*
 * dewrom
 * 
 */
Class QF2_GeneralTreatment extends Quickform2Helper {

    function definition($form) {

        $dateArray = Misc::createHourlyDatesForOnlyToday();
      
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
        MedicalCase::javascriptForMedDefaults($dosageArray, $defaultDosageArray, $defaultBodyLocationArray);
        
        
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            
            //allow over riding the list of cows. 
        if (!empty($this->param->aniNumber)) {
            $combinedArr=$this->param->aniNumber;
        }
        else {
        $combinedArr=parent::currentAniamlsAndGroups();  
        }
            
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($combinedArr);
        } else {         
                $form->addElement('hidden', 'animalSelected')->setValue( $this->param->bovine_id);
        }
      
    
        $form->addElement('select', 'medicine', array('onChange' => "updateContent(this);"), array('options' => $medArray, 'label' => 'Medicine'))->addRule("required", "Please specify your medicine.");
         $form->addElement('text', 'dosage', null, array( 'label' => 'Dosage (??)'))->addRule("required", "Please specify your dosage.");
          $form->addElement('select', 'bodyLocation', null, array('options' => $bodyLocationArray, 'label' => 'Body Location'))->addRule("required", "Please specify your body location.");
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time.");
        
        
        //defaults
         $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
                'animalNumber' => 0
            )));
        
        
        
  return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

           
            $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);

           /* get lists of cows, could be one or many */
           $bovineIdArr=parent::processAnimalsGroupIndividiual($value['animalSelected']);
           //transaction id needed for more then 1 cow.
           if (count($bovineIdArr) > 1)
           {
           $transaction_id = Misc::generatePseudoRandomTransactionInteger();  
           } else {
               $transaction_id=null;
           }
           
          
           
           foreach ($bovineIdArr as $key => $bovineId) {
               
               
                // lookup bovine id and see if there is an open medical case during
                               // the requested event_time
                               $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovineId, $value['event_time']);
                            
                                 $query = "/*GROUP: FIXME TRASNSATION ID*/ INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid) VALUES ($bovineId,{$value['medicine']},'{$value['event_time']}','{$value['dosage']}','{$value['default_dosage_unit']}','{$value['bodyLocation']}',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}')";
                                 $res = $GLOBALS['pdo']->exec($query);
               
           }

            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
    }

    //used for animals movements
function listActiveLocationsAndCounts(){
    //grab a list of locations with pen counts// SQL SMALL BUG ON COUNT	
		$res = $GLOBALS['pdo']->query("SELECT DISTINCT ON (location.name) location.name,location.id,COUNT(*) as count FROM bovinemanagement.location LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.location_id=location.id   WHERE location.active=true GROUP BY location.name,location.id ORDER BY location.name");
		
		
		
		$activeLocations[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//bug in above sql, prints count of 1 when the pen is empty.
			if ($row['count'] ==1) {
			$row['count'] =null;
			$activeLocations[$row['id']] = $row['name'];
			}
			else {
			$activeLocations[$row['id']] = $row['name'].' ('.$row['count'].')';
			}
		}
return $activeLocations;
}

}//end class   




/* supports both individual and group */
class AccordionImplementationMedicalQuickforms extends AccordionSub {

    public static $bovineID;
    public static $mode;

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct($bovineID = null) {

       // parent::__construct(); ddoes not work?

        if ($bovineID != null) {
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
        } else {
            self::$bovineID = null;
            self::$mode = 'group';
        }
    }

    function tab1() {
       
    //cover both group and individual case. 
           $param= new stdClass();
           $param->aniNumber= medicalCase::grabListCowsWithOpenMedicalCase(); //pass along list of animals to show for group. this case open medical cases
           $param->bovine_id= self::$bovineID ;
           $a=new QF2_GeneralTreatment((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'],'bovine_id'=>self::$bovineID)),self::$mode,$param); //group
        print($a->renderBox('qf2_GeneralTreatment','Select Animal to Treat'));  
        print($a->formJS());
        
       
         //MedicalCase::genericTreatmentQuickForm(self::$mode, self::$bovineID);
        
    }

    function tab2() {
         //group only 
         $a=new QF2_MastitisQuickTreat((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('qf2_mastitisEvent','Mastitis Event'));    
    }

    function tab3() {
        MedicalCase::newTemperatureForMedicalCaseQuickForm(self::$mode, self::$bovineID);
    }

    function tab4() {
        MedicalCase::newKetoneForMedicalCaseQuickForm(self::$mode, self::$bovineID);
    }

    function tab5() {
        MedicalCase::newMagnetForMedicalCaseQuickForm(self::$mode, self::$bovineID);
    }

    function tab6() {
        MedicalCase::newCommentForMedicalCaseQuickForm(self::$mode, self::$bovineID);
    }

    function tab7() {
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/vetToCheck.inc');
        VetToCheck::newVetToCheckForMedicalCaseQuickForm(self::$mode, self::$bovineID);
    }

    //probably change this to medicalActionCompletedQuickForm
    function tab8() {
         if ((in_array('owner', $GLOBALS ['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS ['auth']->getAuthData('groups')) == TRUE)) {
        MedicalCase::medicalActionScheduleQuickForm(self::$mode, self::$bovineID);
         }
    }

    function tab9() {
        //group only 
        MedicalCase::newDiagnosisForMedicalCaseQuickForm();
    }

    function tab10() {
        //group only 
        MedicalCase::openNewMedicalCaseQuickForm();
        print ('<br/>' . "\n\r");
        MedicalCase::closeMedicalCaseQuickForm();
    }

}

//end class

require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_MastitisQuickTreat extends Quickform2Helper {

    function definition($form) {


        $mode = 'group';
        
        // grab a list of cows with open medical cases.
        $aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase();

        // grab a list of Medicines'.
        $medArray [null] = "Please Select one..."; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit from bovinemanagement.medicine WHERE for_mastitis=true");
        while (($row = $res->fetch())) {
            $medArray [$row [0]] = $row [1];
        }

        $timeArray = Misc::createHourlyDatesForOnlyToday();
        unset($timeArray [null]); // remove null value.
        
        // custom rule for teats (at least one needs to be selected)
      //  $form->registerRule('checkTeatSelectedRule', 'function', 'checkTeatSelected', 'MedicalCase');
      //  $form->addRule(array('animalNumber', 'udderTop', 'udderBottom'), 'At leat one teat must be selected.', 'checkTeatSelectedRule', array('animalNumber', 'udderTop', 'udderBottom'));
        //////////////////
        
        

        ////
     /*   $fieldset = $form->addFieldset('header')->setLabel('Mastitis Treatment'); */
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
        if ($mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");
            
        } else {
            $form->addElement('hidden', 'bovine_id')->setValue($bovine_id);
        }
        
        
          $form->addElement('select', 'medicine', null, array('options' => $medArray, 'label' => 'Teat Drug'))->addRule("required", "Please specify your animal #.");     
        
    //    $form->addElement('select', 'medicine', null, array('options' => $medArray, 'label' => 'Teat Drug'))->addRule("required", "Medicine is required");
      $form->addElement('select', 'event_time', null, array('options' => $timeArray, 'label' => 'Event Time'))->addRule("required", "Please specify your time of heat");
        
        
        $fieldset = $form->addFieldset('udderAll')->setLabel('Udder Quarters');
$group    = $fieldset->addGroup('udderFront')->setSeparator('&nbsp;&nbsp;&nbsp;&nbsp;');
$group->addElement('checkbox', 'lf', array('id' => 'boxId', 'value' => '1'))->setContent('LF')->setLabel('LF');
$group->addElement('checkbox', 'rf', array('id' => 'boxId', 'value' => '1'))->setContent('RF')->setLabel('RF');


$group2    = $fieldset->addGroup('udderRear')->setSeparator('&nbsp;&nbsp;&nbsp;&nbsp;');
$group2->addElement('checkbox', 'lr', array('id' => 'boxId', 'value' => '1'))->setContent('LR')->setLabel('LR');
$group2->addElement('checkbox', 'rr', array('id' => 'boxId', 'value' => '1'))->setContent('RR')->setLabel('RR');

          $form->addRule(new checkTeatSelectedRule($form));
        

        return $form; //required       
    }

    function process($value) {
        
            //
             $event_time = date('Y-m-d H:i:sO',$value['event_time']);
            //
            $quarterArray ['front left'] = (!empty($value['udderFront']['lf'])) ? 1 : 0;
            $quarterArray ['front right'] = (!empty($value['udderFront']['rf'])) ? 1 : 0;
            $quarterArray ['rear left'] = (!empty( $value['udderRear']['lr'])) ? 1 : 0;
            $quarterArray ['rear right'] =(!empty( $value['udderRear']['rr'])) ? 1 : 0;
             
            
           // $pageid = $form->exportValue('pageid');

            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $event_time);

            // administer drug
            self::administerMastisisDrug($value['bovine_id'],  $value['medicine'], $medical_case_id, $event_time, $quarterArray);

            // determine if the commit or rollback

               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            
            }
            // Close the transaction

       
    }

   private static function administerMastisisDrug($bovine_id, $medicine_id, $medical_case_id, $event_time, $quarterArray) {
        foreach ($quarterArray as $key => $value) {
            if ($value == 1) { // only do if the value is true
                $locationStr = "$key" . ' teat canal'; // make string from array key
                // value
                // find default dosage
                $sql = "SELECT default_dosage,default_dosage_unit FROM bovinemanagement.medicine WHERE id=$medicine_id LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);

                // administer drug
                $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid)
	  VALUES ($bovine_id,$medicine_id,'$event_time','{$row2['default_dosage']}','{$row2['default_dosage_unit']}','$locationStr',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);
            }
        }
    }
    
}//end class
/*
 * complicated rule to check that one teat is selected.
 * 
 */
class checkTeatSelectedRule extends HTML_QuickForm2_Rule
{
    protected function validateOwner()
    {
        $arr=$this->owner->getValue();
        
        //we do function logic here so we don't have to check for empty again, because checkbox returns nothing if it is not checked. STUPID.
        if ((!empty($arr['udderFront'])) OR (!empty($arr['udderRear']))) {
             return true;
        }else {
             return false;
        }
       
    }

    protected function setOwnerError()
    {
       //adding '-0', seems to be default way to name id. There also is not getElementByName method, so instead we get array and get first value. 
         $this->owner->getElementsByName('udderFront')[0]->setError('ERROR: At least one quarter needs selected.');   
    }
    
}
//end class
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_MedicalTemperature extends Quickform2Helper {

    function definition($form) {
       
        $timeArray = Misc::createHourlyDatesForOnlyToday();
        unset($timeArray [null]); // remove null value.
        
        // create array of celcius temps.
        $temperatureArray = array(null => 'Please Select One');
        for ($counter = 36.5; $counter <= 42.1; $counter += 0.10) {
            $farenheight = round(((9 / 5) * ($counter) + 32), 1);
            $celcius = round($counter, 1);
            $temperatureArray ["$celcius"] = "$celcius &#8451; &nbsp; / &nbsp; $farenheight &deg;F";
        }

        //defaults at run time 
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
            'dateEvent' => 1,
             'temperature' => 0
        )));

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
        if ($this->mode == 'group') {
            $aniNumberArr = MedicalCase::grabListCowsWithOpenMedicalCase();
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumberArr, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");
            
        } else {
            $form->addElement('hidden', 'bovine_id')->setValue($bovine_id);
        }
        $form->addElement('select', 'event_time', null, array('options' => $timeArray, 'label' => 'Time of Check'))->addRule("required", "Event Time is required.");
        $form->addElement('select', 'temperature', null, array('options' => $temperatureArray, 'label' => 'Temperature'))->addRule("required", "Temperature is required.");
        

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);
        $value['event_time'] = $value['medical_case_id'];
        
         // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = self::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            
        try {
            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_temperature (medical_case_id,bovine_id,event_time,temperature,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','{$value['temperature']}','{$GLOBALS['auth']->getUsername()}')";
 $res = $GLOBALS['pdo']->exec($query);
}
catch (\PDOException $e) {
    if ($e->errorInfo[1] == 23505) {
print("Warning: Already in db.");
    }
}
      

    }

}//end class
?>
