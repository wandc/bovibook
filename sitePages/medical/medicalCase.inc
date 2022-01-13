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

   

// end custom javascript

    protected function tab1() {
        print($this->currentCases());
    }

    protected function tab2() {
       print($this->displayMedicalCases('historical', false));
    }

    protected function tab3() {
        print($this->displayListOfMedecineAdministered());
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
    public function currentCases() {
    
$sickBox=(new BootStrap)->plainCard('99 Fresh/Sick Group', (new SickGroup)->sickCowBoxes());
       
        
        
    $cls=(new bovineQueryMedicalAccordian);
    $MedicineScheduled=(new MedicineScheduled)->displayNext8HoursScheduledMedicines();

$left = <<<HTML
        $sickBox
        {$this->displayMedicalCases('current_sick', true)}
          {$this->displayMedicalCases('current_rest', true)}
        HTML; 
           
        $right = <<<HTML
                {$this->errorNoMedicalCaseOpenAndShouldBe()}
                  {$this->displayHalocurError()}
            {$MedicineScheduled}
           {$cls->showMedicalCaseAccordion()}
        HTML; 
               
        $content=(new Bootstrap)->oneThirdTwoThirds($left,$right);                
        print($content);


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
            $str=$str.(new BootStrap)->warningNotify("Medical Case should be opened for: #{$row['local_number']}, {$row['count']} events in the last two weeks!");
        }
     return $str;
        
        
    }

    public function summaryDisplay() {
        // nothing
    }

    
    /* used for Halocur given more than once per day */
    public function halocurError() {
        $sql=<<<SQL
               with temp as (
SELECT distinct bovine_id,local_number,full_name,count(medicine_index) 
FROM bovinemanagement.medicine_administered 
LEFT JOIN bovinemanagement.bovine ON bovine_id=bovine.id
WHERE medicine_index=85 AND event_time 
BETWEEN now() - (interval '2 days') AND now() GROUP BY bovine_id,local_number,full_name,medicine_index
) SELECT * FROM temp WHERE count >=3
SQL;
       $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);         
       
        $x=array();
//change presentation
        foreach ($results as &$row) {

            $rowOut['Animal']= (new JQueryDataTable())->formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               
                $x[]['home_page'] =   $rowOut['Animal'] .' exceeds the max dosage of once per day. '. $row['count'] .' treatements in the last 2 days.' ;
          
        }
        
           //output
        $output['data'] = $x;
        return ($output);
    }
    
    function displayHalocurError() {

        $ajaxUrl = $GLOBALS['config']['HTTP']['JSON_API'].'/64/halocurError';
        $out[] = ((new BootStrap)->bootstrapSmallBoxAjax('HaloCur', $ajaxUrl, 53, null, 'fas fa-skull-crossbones'));

        return implode($out);
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
        
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $array1, $array2, $array3);
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
        
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $array1, $array2, $array3);
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
       $sql=<<<SQL
               SELECT trade_name as "Trade Name",local_instructions as "Local Instructions",medicine_class as "Medicine Class" FROM bovinemanagement.medicine ORDER by trade_name
               SQL;

        print( (new JQueryDataTable)->startBasicSql('Types of Medicine', $sql)); 
       
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
    function medicalCases($request) {
       $current = Sanit::reqVal('item', false, 'INT');
      
       
if ($current==1) {       
$sql = <<<SQL
SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovinecurrall.full_name, bovinecurrall.local_number, bovinecurrall.location_name, date_trunc('day',current_date-bovinecurrall.fresh_date) as dim,
(SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN  bovinemanagement.medical_diagnosis_type ON diagnosis_type_id = medical_diagnosis_type.id WHERE medical_case_id=medical_case.id ORDER BY event_time DESC limit 1) as diagnosis
FROM bovinemanagement.medical_case 
LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=medical_case.bovine_id 
WHERE close_date is NULL 
ORDER BY local_number,open_date
SQL;
}
else {
$sql = <<<SQL
SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovinecurrall.full_name, bovinecurrall.local_number, bovinecurrall.location_name, date_trunc('day',current_date-bovinecurrall.fresh_date) as dim,
(SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN  bovinemanagement.medical_diagnosis_type ON diagnosis_type_id = medical_diagnosis_type.id WHERE medical_case_id=medical_case.id ORDER BY event_time DESC limit 1) as diagnosis
FROM bovinemanagement.medical_case 
LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=medical_case.bovine_id 
WHERE open_date >= (current_date - interval ' 2 years')
ORDER BY local_number,open_date
SQL;
}
                 
         
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
           $row['event_time_raw'] = strtotime($row['open_date']);
           $row['info'] = $this->printBovineMedicalCaseInfo($row);
            //$row['diagnosis'] = 'hh';
           //$row['treatments'] =$this->printMedecineAdministeredForAPaticularMedicalCase($row);
           $row['treatments'] = ($this->schedMedsOnBovine($row));
            $row['treatments2'] = ($this->treatmentHistorySubTableBovine($row));
       //     $row['action'] = $this->printActionButtons2($row['bovine_id'], $row['event_time'],$row['estrus_id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        //    $row['event_time'] = date("l M d H:00", strtotime($row['event_time']));  //has to be at end.
          
        }

        //output

        
          //output
    //    $output['data'] = $results;
      //  return ($output);
    }
    
    
    
    /*
     * 2020 icons display
     * 
     */
    function displayMedicalCases($mode = 'default', $current = true) {
       /*combined open and all sick group */
        /**
         SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovinecurrall.full_name, bovinecurrall.local_number, bovinecurrall.location_name, date_trunc('day',current_date-bovinecurrall.fresh_date) as dim, 
(SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN  bovinemanagement.medical_diagnosis_type ON diagnosis_type_id = medical_diagnosis_type.id WHERE medical_case_id=medical_case.id ORDER BY event_time DESC limit 1) as diagnosis
FROM bovinemanagement.medical_case 
LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=medical_case.bovine_id 
WHERE close_date is NULL AND location_id=39
UNION

SELECT null as id,bovinecurrall.id as bovine_id,null as open_date, null as close_date, null as close_userid, full_name,local_number,location_name,date_trunc('day',current_date-bovinecurrall.fresh_date) as dim,null as diagnosis
FROM bovinemanagement.bovinecurrall
LEFT JOIN bovinemanagement.medical_case ON bovinecurrall.id=medical_case.bovine_id 
WHERE open_date  is null AND location_id=39 
ORDER BY local_number,open_date 
         */
        
        switch ($mode) {
            case 'current_sick':
                $numberPerPage = 1000;
                $title = 'Open Cases - 99 Fresh/Sick Group';
                $sql = <<<SQL
SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovinecurrall.full_name, bovinecurrall.local_number, bovinecurrall.location_name, date_trunc('day',current_date-bovinecurrall.fresh_date) as dim, 
(SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN  bovinemanagement.medical_diagnosis_type ON diagnosis_type_id = medical_diagnosis_type.id WHERE medical_case_id=medical_case.id ORDER BY event_time DESC limit 1) as diagnosis
FROM bovinemanagement.medical_case 
LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=medical_case.bovine_id 
WHERE close_date is NULL AND location_id=39
ORDER BY local_number,open_date 
SQL;
                break;
            case 'current_rest':
                $numberPerPage = 1000;
                $title = 'Open Cases - Other Groups';
                $sql = <<<SQL
SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovinecurrall.full_name, bovinecurrall.local_number, bovinecurrall.location_name, date_trunc('day',current_date-bovinecurrall.fresh_date) as dim, 
(SELECT diagnosis FROM bovinemanagement.medical_diagnosis LEFT JOIN  bovinemanagement.medical_diagnosis_type ON diagnosis_type_id = medical_diagnosis_type.id WHERE medical_case_id=medical_case.id ORDER BY event_time DESC limit 1) as diagnosis
FROM bovinemanagement.medical_case 
LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=medical_case.bovine_id 
WHERE close_date is NULL AND location_id!=39
ORDER BY local_number,open_date 
SQL;
                break;
            case 'historical':
                $numberPerPage = 10;
                $title = 'Historical Cases';
                $sql = "SELECT medical_case.id,bovine_id,open_date,close_date,close_userid,bovine.full_name, bovine.local_number FROM bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovine ON bovine.id=medical_case.bovine_id WHERE close_date >= (current_date - interval '12 months') ORDER BY open_date DESC";
                break;
            
            default:
                throw new Exception('Error: pick a valid medical case type.');
                break;
        }


         $statement = $GLOBALS['pdo']->prepare($sql);
         $statement->execute();
       
         if (($statement->rowCount() == 0) && ($current == true)) {
             return ('<h3>No Current Medical Cases</h3>');
        }
        
        
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
          
            //name
            $animalName=JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name'], 'display-3');
            
            //parlour data
            $milkProd = '';
            $milkProdData = (new BovineQuery)->getLatestMilkingSimple($row['bovine_id'], 3,true);
            $milkProd = (new BootStrap)->plainCard('Parlour', $milkProdData);
            
            //
            $sickParam=(new SickGroup)->sickGroupParams($row['bovine_id']);
            $schedMeds=($this->schedMedsOnBovine($row));
            
            //stats
            $diagnosis=(empty($row['diagnosis']))?:(new BootStrap)->plainCardPrimarySolidAll('Diagnosis', $row['diagnosis']);
            $caseOpen=(new BootStrap)->plainCardPrimarySolidAll('Open Date', JQueryDataTable::formattedDate($row['open_date']));
            $location=(new BootStrap)->plainCardPrimarySolidAll('Location', ($row['location_name']));
            $dim=(new BootStrap)->plainCardPrimarySolidAll('DIM', $row['dim']);
            $maxTreatmentsMastitis=  $this->maximumTreatmentsMastitis($row['id']);
            
            //treatement past present future
            $treatments=($this->treatmentHistorySubTableBovine2($row));
            
            
          

            
            //choose output mode
             if ($mode == 'current_sick') {
                 
                 $left=<<<LEFT
                         $animalName
                         $diagnosis
                         $caseOpen
                         $location
                         $dim
                         $milkProd
                 LEFT;
                 
                  $right=<<<RIGHT
                        $treatments
                        $maxTreatmentsMastitis
                 RIGHT;
                 
                 $a=(new BootStrap)->twoThirdsOneThird($left,$right);
                 
                 
                 $resultsOut[]=(new BootStrap)->plainCardPrimarySolid($row['local_number'], ($a));  
             }
            else {
                  $rowOut['Animal'] = $animalName;
                  $rowOut[' '] = $schedMeds;
                  $rowOut['Stats'] = $diagnosis. $caseOpen;
                  $rowOut['Treatments'] = ($this->treatmentHistorySubTableBovine2($row));
                  $resultsOut[]=$rowOut;  
            }
          
         
        }

       //output
        $dataArr = $resultsOut;
      
        
           if ($mode == 'current_sick') {
                 $out[]=implode($dataArr);  
           }else {
                     $out[]=( (new JQueryDataTable(false))->startBasicArrayOpt($title, $dataArr,'searching: true,paging: false,ordering: false,info: false'));    
           }
           

         
        
         
         /*
          * change the card based on what we are displaying.
          */
         switch($mode) {
             case 'current_sick':
             return  (new BootStrap)->plainCardWarningSolidAll($title, implode($out));
                 break;
              case 'current_rest':     
                  return  (new BootStrap)->plainCardSecondarySolidAll($title, implode($out));
              break;
            case 'historical':
                   return  (new BootStrap)->plainCardPrimarySolid($title, implode($out));
                break;
              default:
                throw new Exception('Error: pick a valid medical case type 2.');
                break;   
                
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
    function printBovineMedicalCaseInfo($record) {
       

        // find out when milk and beef witholding is:
        $res = $GLOBALS['pdo']->query("SELECT milk_withholding,beef_withholding FROM bovinemanagement.milk_and_beef_withholding({$record['bovine_id']})");
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $record ['location_name'];

        $str = '';
        $str = $str . '<p>Case #: ' . $record ['id'] . '</p>';
        $str = $str . '<p>Location: ' . $record ['location_name'] . '</p>';
        
        // only show milk withholding if it is in the future, else it doesn't
        // apply
          $str = $str .'<p>';
        if (strtotime($row ['milk_withholding']) > strtotime("now")) {
            $str = $str . (new BootStrap)->dangerCallout('Milk Withhold until: '.date('D M j/Y ga', strtotime($row ['milk_withholding'])));
        }
        else {
               $str = $str . (new BootStrap)->sucessCallout('Milk Clear');
        }
        
         $str = $str .'</p>';
          $str = $str .'<p>';
        
        // only show beef withholding if it is in the future, else it doesn't
        // apply
        if (strtotime($row ['beef_withholding']) > strtotime("now")) {
             $str = $str . (new BootStrap)->warningCallout('Beef Withhold until: '.date('D M j/Y ga', strtotime($row ['beef_withholding'])));
        }
        else
        {
              $str = $str . (new BootStrap)->sucessCallout('Beef Clear');
        }
         $str = $str .'</p>';

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
             $title=(new BootStrap)->dangerSpan('Max Treatment Reached');
                 $str = $str .    (new BootStrap)->dangerCallout("$title: {$row['trade_name']} at {$row['location']}. Take milk Sample.<br/>");
            }
        }
        return $str;
    }

    
    function schedMedsOnBovine($record){
          $sql1 = "
      SELECT id,event_time,type,text,userid,bovine_id 
         FROM temp_medical_summary 
         WHERE temp_medical_summary.bovine_id={$record['bovine_id']} 
            AND (type='Scheduled Meds' OR type='Scheduled Action')
            AND event_time > (current_timestamp + interval '8 hours')
         ORDER BY event_time DESC ,type ASC 
   ";
         
          $statement = $GLOBALS['pdo']->prepare($sql1);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

         $results2=array();
        foreach ($results as &$row) {
        $row2['date']=date('D jS H:00',strtotime($row['event_time']));
        $row2['text']=$row['text'];
        $results2[]=$row2;
        }
        
        
        if (empty($results2)) {
            return null;
        }else {
            return $this->build_table($results2);
        }

         
        
        
         // $out[]='</ul>';
        //output
       
        //return implode($out);
    }
    
    /*
     * shows treatment list like bovinequery page
     * 
     */
    function treatmentHistorySubTableBovine2($record) {
        
        
          $sql = "
    (SELECT id,tablename,event_time,type,text,userid,raw_value,bovine_id,false as colour,wcauthentication.initials(userid) AS initials,'bovinemanagement' as schema FROM temp_medical_summary WHERE temp_medical_summary.medical_case_id={$record['id']})
      UNION
    (SELECT id,tablename,event_time,type,text,userid,raw_value,bovine_id,true as colour,wcauthentication.initials(userid) AS initials,'bovinemanagement' as schema FROM temp_medical_summary WHERE temp_medical_summary.bovine_id={$record['bovine_id']} AND (type='Scheduled Meds' OR type='Scheduled Action')
     AND event_time <= (current_timestamp + interval '8 hours')) ORDER BY event_time DESC ,type ASC 
   ";
    
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $results2=array();
        //change presentation
        $outArrayOld=array();
        $outArrayCurrent=array();
        $outArrayFuture=array();
        foreach ($results as &$row) {
           
            //markup text for things like temp out of range and ketones, other text flows through.
            $text=(new BovineQueryMedicalLog)->medEventsMarkup($row['tablename'],$row['text'],$row['raw_value']);
            $deleteButton=(new BovineQueryMedicalLog)->medicalEventsDeleteButton($row['type'],$row['id'],$row['tablename'],$row['schema'],$record['bovine_id']);
            //$deleteButton=null;
            
            
            //old stuff
            if (strtotime($row['event_time']) <= strtotime("-3 day")) {                
             $outArrayOld= $GLOBALS['MiscObj']->makeTimeString($outArrayOld, strtotime($row['event_time']),  (new BovineQueryMedicalLog)->medEventListItem($row['type'],$row['event_time'],$text,$row['initials'],$deleteButton));
            }
            //far future
            elseif (strtotime($row['event_time']) >= strtotime("+1 day")) {       
            $outArrayFuture= $GLOBALS['MiscObj']->makeTimeString($outArrayFuture, strtotime($row['event_time']),  (new BovineQueryMedicalLog)->medEventListItem($row['type'],$row['event_time'],$text,$row['initials'],$deleteButton));
            }
            //current stuff
            else {
               $outArrayCurrent= $GLOBALS['MiscObj']->makeTimeString($outArrayCurrent, strtotime($row['event_time']),  (new BovineQueryMedicalLog)->medEventListItem($row['type'],$row['event_time'],$text,$row['initials'],$deleteButton));
            }
                
       
        }
         
        //now only show them if data exists in them.
        $out[]=!empty($outArrayFuture) ?(new BootStrap)->plainCardCollapsed('Future',implode($outArrayFuture)) :'';
        $out[]=!empty($outArrayCurrent) ?(new BootStrap)->plainCard('Current',implode($outArrayCurrent)) :'';
        $out[]=!empty($outArrayOld) ? (new BootStrap)->plainCardCollapsed('Past',implode($outArrayOld)) :'';
        
        return  implode($out);
        
    }
    
    
    
    
    
    function treatmentHistorySubTableBovine($record) {
               $sql = "
    (SELECT id,tablename,event_time,type,text,userid,raw_value,bovine_id,false as colour FROM temp_medical_summary WHERE temp_medical_summary.medical_case_id={$record['id']})
      UNION
    (SELECT id,tablename,event_time,type,text,userid,raw_value,bovine_id,true as colour FROM temp_medical_summary WHERE temp_medical_summary.bovine_id={$record['bovine_id']} AND (type='Scheduled Meds' OR type='Scheduled Action')
     AND event_time <= (current_timestamp + interval '8 hours')) ORDER BY event_time DESC ,type ASC 
   ";
    
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $results2=array();
        //change presentation
        foreach ($results as &$row) {
            
        //markup text for things like temp out of range and ketones, other text flows through.
        $text=(new BovineQueryMedicalLog)->medEventsMarkup($row['tablename'],$row['text'],$row['raw_value']);
    
        $row2['event_time']= date('M d, Y H:00', strtotime($row['event_time'])); 
        $row2['type']=  $row['type'];
        $row2['text']=  $text;
        $row2['userid']=  $row['userid']; 
   
        $results2[]=$row2;
        }

          if (empty($results2)) {
            return 'N/A';
        }else {
            return $this->build_table($results2);
        }

        
    }
    
    
    //from https://stackoverflow.com/questions/4746079/how-to-create-a-html-table-from-a-php-array
    //take array of form:
    /*
     $array = array(
    array('first'=>'tom', 'last'=>'smith', 'email'=>'tom@example.org', 'company'=>'example ltd'),
    array('first'=>'hugh', 'last'=>'blogs', 'email'=>'hugh@example.org', 'company'=>'example ltd'),
    array('first'=>'steph', 'last'=>'brown', 'email'=>'steph@example.org', 'company'=>'example ltd')
);
     */
       function build_table($array){
    // start table
    $html = '<table>';
    // header row
  //  $html .= '<tr>';
    foreach($array[0] as $key=>$value){
  //          $html .= '<th>' . htmlspecialchars($key) . '</th>';
        }
  //  $html .= '</tr>';

    // data rows
    foreach( $array as $key=>$value){
        $html .= '<tr>';
        foreach($value as $key2=>$value2){
            $html .= '<td>' . ($value2) . '</td>';
        }
        $html .= '</tr>';
    }

    // finish table and return it

    $html .= '</table>';
    return $html;
}
    
    
    /**
     * NESTED *
     */
    function printMedecineAdministeredForAPaticularMedicalCase($record) {
  

        /*
         *   show scheduled events more than 8 hours in the future as its own
         *   , so we can hide it with js.
         */
      
         
         
 //$out[]=((new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql1));   
 
 
 
 
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
        //    print($toggle);

      //      $dg1 = new DataGridLR($sql1, 1000);
       //     $dg1->datagrid->addColumn(new Structures_DataGrid_Column('All', null, '', array('' => ''), null, array('MedicalCase', 'printparticularTreatmentInfo()')));
            //don't show because we have sql for delete button coded to not allow deletes of scheduled meds
            //$dg1->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('MedicalCase', 'printActionButtons()'))); 
        //    $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Action2', null, '', array('' => ''), null, array('MedicineScheduled', 'printActionButtons2()')));
            $strFuture = $strFuture . "<a id='farFutureTreatmentToggle_$uniqueID' href='#'><b id='boldLightBlue'>$numFutureRow Future Event(s) --></b></a>";
        //    $strFuture = $strFuture . "<div id='farFutureTreatment_$uniqueID'>" . $dg1->render('datagrid datagridNested') . '</div>';
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
    
   //  $out[]=((new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql));   
     
     //   $stmt2 = $GLOBALS['pdo']->query($sql);
      //  $arrayDG = $stmt2->fetchAll(PDO::FETCH_ASSOC);

        //need to pass as an array, because if we pass sql directly to nested datagrid, it can't seem to see the temporary table.

    //    $dg3 = new DataGridLRArray($arrayDG, 1000);

     //   $dg3->datagrid->addColumn(new Structures_DataGrid_Column('All', null, '', array('' => ''), null, array('MedicalCase', 'printparticularTreatmentInfo()')));
     //   $dg3->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('MedicalCase', 'printActionButtons()')));
        //render both datagrids, but put a seperate div around the far future ones,so we can hide, them.
     //   $str = '<div id=\'scrollable\'>' . $strFuture . $dg3->render('datagrid datagridNested') . '</div>'; // wrap in


        return implode($out);
    }

    
    public function colourKetoneOrNot($value,$suffix=false) {
        
        //whether or not to print degree symbol
        if ($suffix ==true) {
            $suffix=' mM/ℓ';
        }else {
            $suffix='';
        }
        
       //38.05 C to 39.16 is considered normal, not cited because the experts do not agree.
            if (($value <= 1.2))  {
                //colour green
                $value = '<span class="text-success">' . $value .$suffix. '</span>';
            }    
            elseif (($value <= 2.4))  {
                //colour green
                $value = '<span class="text-warning">' . $value .$suffix. '</span>'; 
            } else {  //abnormal
                $value = '<span class="text-danger">' . $value .$suffix. '</span>';
            }
        
        return $value;
    }
    
    public function colourTempNormalOrNot($value,$degrees=false) {
        
        //whether or not to print degree symbol
        if ($degrees ==true) {
            $suffix='℃';
        }else {
            $suffix='';
        }
        
       //38.05 C to 39.16 is considered normal, not cited because the experts do not agree.
            if (($value >= 38.05) && ($value <= 39.16)) {
                //colour green
                $value = '<span class="text-success">' . $value .$suffix. '</span>';
            } else {  //abnormal
                $value = '<span class="text-danger">' . $value .$suffix. '</span>';
            }
        
        return $value;
    }
    
   
    
    //DELETE me when datagrid gone
    function printparticularTreatmentInfo($params) {
        extract($params);

        //colour temperature values who are out of the ordinary
        if ($record ['type'] == 'Temperature') {

              $record ['text']=$this->colourTempNormalOrNot($record ['text']);
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

    

    
    public static function lookupOpenMedicalCaseForAnimal($bovine_id, $event_time) {

        if (empty($bovine_id) OR (!is_numeric($bovine_id))) { throw new Exception('Bovine ID not valid.'); }
        if (empty($event_time)) { throw new Exception('Event Time not valid.'); }

        
        // lookup bovine id and see if there is an open medical case during the
        // requested event_time
        $sql = "SELECT id as medical_case_id FROM bovinemanagement.medical_case WHERE bovine_id=$bovine_id AND open_date<='$event_time' AND close_date is NULL";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        if ((!empty($row)) AND (is_numeric($row['medical_case_id'])) == true) {
            $medical_case_id = $row ['medical_case_id'];
        } else {
            $medical_case_id = null;
        }
        return $medical_case_id;
    }


   public static function lookupAnimalsWhoDontHaveOpenMedicalCase() {
         
         // grab a list of cows who don't have a medical case already open...
        $res = $GLOBALS['pdo']->query("SELECT bovinecurrall.id,full_name,local_number FROM bovinemanagement.bovinecurrall EXCEPT (SELECT bovine_id,full_name,local_number from bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovine ON bovine.id=medical_case.bovine_id WHERE medical_case.close_date is NULL) ORDER BY local_number");
       
        $aniNumber [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            // parse for last word of name and then make an array for the select
            // list.
            $aniNumber [$row [0]] = $row [2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
        }
         return $aniNumber;
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
            $aniNumber [$row [0]] = $row [2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]) . ' - ' . $row [3];
        }
        return $aniNumber;
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
    public static function javascriptForMedDefaults($uuid,$dosageArray, $defaultDosageArray, $defaultBodyLocationArray) {

        // create javascript to change dosage to default dosage based on a
        // select statement.
        $out[]='';
        $out[]= ('<script type="text/javascript">
			var aryContent = new Array();
			var aryContent2 = new Array();
			var aryContent3 = new Array();');

        foreach ($dosageArray as $key => $value) {
            $out[]=  ("aryContent['$key'] = '($value)';");
        }
        foreach ($defaultDosageArray as $key => $value) {
            $out[]=  ("aryContent2['$key'] = '$value';");
        }
        foreach ($defaultBodyLocationArray as $key => $value) {
            $out[]=  ("aryContent3['$key'] = '$value';");
        }
        
$out[]=<<<JS
                
           function updateContent{$uuid}(obj){

/* change doage units in label */
$("label[for='dosage-0']").contents().first()[0].textContent ="Dosage "+aryContent[ obj[obj.selectedIndex].value ];

/* change dosage input default value */
$('#dosage-0').val(aryContent2[ obj[obj.selectedIndex].value ]);

/* change body location input to default value */
$('#bodyLocation-0').val(aryContent3[ obj[obj.selectedIndex].value ]);		
			}
                 
  </script>     
                
JS;        
        
        
        
        return implode($out);
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

        $sql = "SELECT local_number  as \"local number\",full_name,trade_name,dosage,dosage_unit  as \"dosage unit\",location,note,userid,event_time  as \"event time\",close_date,scheduled_event_time as \"scheduled event time\",scheduled_userid  as \"scheduled userid\"
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovinecurrall ON medicine_administered.bovine_id = bovinecurrall.id
  LEFT JOIN bovinemanagement.medical_case ON medicine_administered.medical_case_id = medical_case.id
  ORDER BY medicine_administered.event_time DESC";
         $out[]=( (new JQueryDataTable)->startBasicSql('List Of Medecine Administered', $sql,'order: false')); 
         return implode($out); 
        
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
 * 
 * 
 */
Class QF2_GeneralTreatment extends QF2_Base {

    function definition($form) {
        
     
        $uuid=uniqid();
        
        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday();
      
        $location = $this->listActiveLocationsAndCounts();

         // body locations
        $bodyLocationArray = MedicalCase::medicalBodyLocations(true);

        
        // prints out javascript to change dosage defaults when mediciane changes.
         $medList= (!empty($this->param->medicineListCustom)) ? self::listOfMedicines($this->param->medicineListCustom) : self::listOfMedicines();

        $js=MedicalCase::javascriptForMedDefaults($uuid,$medList['dosageArray'], $medList['defaultDosageArray'], $medList['defaultBodyLocationArray']);
        $form->addElement('static', 'script_medicalDefault')->setValue($js);
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
        
        $medicine=$form->addElement('select', 'medicine', array('onChange' => "updateContent{$uuid}(this);"), array('options' => $medList['medArray'], 'label' => 'Medicine'));    
        $medicine->addRule("required", "Please specify your medicine.");
        
        $form->addRule('callback', 'ERROR: This same medicine is already scheduled for this animal.', array('callback' => array($this, 'checkMedicineAlreadyScheduled'), 'arguments' => array('animalNumber','medicine','dateEvent')));
        
        $form->addElement('text', 'dosage', null, array( 'label' => 'Dosage (??)'))->addRule("required", "Please specify your dosage.");
        $form->addElement('select', 'bodyLocation', null, array('options' => $bodyLocationArray, 'label' => 'Body Location'))->addRule("required", "Please specify your body location.");
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time.");
        
      
        
        //defaults
         $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
                'animalNumber' => 0
            )));
        
        
        
  return $form; //required   
    }

      //if a medicine was already scheduled for this time period, then prevent entry. 
      function checkMedicineAlreadyScheduled($value) {
          DEBUG ?: var_dump($value);

          if (is_array($value['animalSelected'])) {
        foreach ($value['animalSelected'] as $key => $bovineID) {
        $ans[]=$this->alreadyScheduledRaw($value,$bovineID);
            
        }//end foreach
        
        }
        else {
        $ans[]=$this->alreadyScheduledRaw($value,$value['animalSelected']);  
        }
        
        //when array is all true, return true. 
        return  ($ans===TRUE) ?  false : true;
    }

    private function alreadyScheduledRaw($value,$bovineID) {
        
         $dateEvent = date('c', $value['dateEvent']);
            $sql = <<<SQL
        SELECT id FROM bovinemanagement.medicine_administered WHERE bovine_id={$bovineID} AND medicine_index={$value['medicine']} AND event_time is NULL AND scheduled_event_time BETWEEN ('$dateEvent'::timestamp - interval '4 hours') AND ('$dateEvent'::timestamp + interval '4 hours') LIMIT 1
        SQL;
            $res = $GLOBALS['pdo']->query($sql);
            $x = count($res->fetchAll(PDO::FETCH_ASSOC));
            DEBUG ?: var_dump($sql);
            DEBUG ?: var_dump($res->fetchAll(PDO::FETCH_ASSOC));
            if (empty($x)) {
                return true;
            } else {
                return false;
            }     
    }
    
    function alreadyScheduledError() {
        
        
                $blah = <<<HTML
        <div class="toast">
  <div class="toast-header">
    Toast Header
  </div>
  <div class="toast-body">
    Some text inside the toast body
  </div>
</div>
HTML;
                $blah2 = <<<HTML
                <script>
$(document).ready(function(){
  $('.toast').toast('show');
});
</script>
HTML;

                print($blah . $blah2);
    }
    
    
    
    function process($value) {

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

           
            $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);
            
            $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE id={$value['medicine']} limit 1");
            $row = $res->fetch();
            $value['default_dosage_unit']=$row['default_dosage_unit'];
            

           /* get lists of cows, could be one or many */
           $bovineIdArr=(new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
           //transaction id needed for more then 1 cow.
           if (count($bovineIdArr) > 1)
           {
           $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();  
           } else {
               $transaction_id=null;
           }
           
          
           
           foreach ($bovineIdArr as $key => $bovineId) {
               
               
                // lookup bovine id and see if there is an open medical case during
                               // the requested event_time
                               $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovineId, $value['event_time']);
                               $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;
                            
                                 $query = "/*GROUP: FIXME TRASNSATION ID*/ INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid) VALUES ($bovineId,{$value['medicine']},'{$value['event_time']}','{$value['dosage']}','{$value['default_dosage_unit']}','{$value['bodyLocation']}',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}')";
                                 $res = $GLOBALS['pdo']->exec($query);
               
           }

            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

    /*
     * used by this form and others to get list of medicines. 
     */
    public static function listOfMedicines($type=null) {
        /* defaults to all medicines */
        switch ($type) {
            case 'vaccine':
                $sql="SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE for_mastitis!=true AND medicine_class='vaccine' ORDER BY trade_name";
                break;
            case 'endectocide':
                $sql="SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE for_mastitis!=true AND medicine_class='endectocide' ORDER BY trade_name";
                break;
            default:
              $sql="SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE for_mastitis!=true ORDER BY trade_name";
                break;
        }
        
        
        
        
        // grab a list of Medicines'.
        $medArray [null] = "Please Select one..."; // dummy value
        $dosageArray [null] = ""; // dummy value
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch())) {
            $medArray [$row [0]] = $row [1];
            $dosageArray [$row [0]] = $row [3];
            $defaultDosageArray [$row [0]] = $row [2];
            $defaultBodyLocationArray [$row [0]] = $row [4];
        }

        $out['medArray'] = $medArray;
        $out['dosageArray'] = $dosageArray;
        $out['defaultDosageArray'] = $defaultDosageArray;
        $out['defaultBodyLocationArray'] = $defaultBodyLocationArray;

        return $out;
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


    
Class QF2_MastitisQuickTreat extends QF2_Base {

    function definition($form) {


        
        // grab a list of cows with open medical cases.
        $aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase();

        // grab a list of Medicines'.
        $medArray [null] = "Please Select one..."; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit from bovinemanagement.medicine WHERE for_mastitis=true");
        while (($row = $res->fetch())) {
            $medArray [$row [0]] = $row [1];
        }

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday();
        unset($timeArray [null]); // remove null value.
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
       $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
        
        
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
             $value['bovine_id']=$value['animalSelected'];
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
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

            // administer drug
            self::administerMastisisDrug($value['bovine_id'],  $value['medicine'], $medical_case_id, $event_time, $quarterArray);

            // determine if the commit or rollback

               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            
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
     
Class QF2_MedicalTemperature extends QF2_Base {

    function definition($form) {
       
        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday();
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
     $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
        
        $form->addElement('select', 'temperature', null, array('options' => $temperatureArray, 'label' => 'Temperature'))->addRule("required", "Temperature is required.");
        $form->addElement('select', 'event_time', null, array('options' => $timeArray, 'label' => 'Time of Check'))->addRule("required", "Event Time is required.");
        

        return $form; //required       
    }

    function process($value) {
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);

         // lookup bovine id and see if there is an open medical case during
            // the requested event_time
             $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
             $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;
            
        try {
            // don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_temperature (medical_case_id,bovine_id,event_time,temperature,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','{$value['temperature']}','{$GLOBALS['auth']->getUsername()}')";
 $res = $GLOBALS['pdo']->exec($query);
}
catch (\PDOException $e) {
    if ($e->errorInfo[1] == 23505) {
print("Warning: Already entered in database.");
    }
}
      

    }

}//end class

//$a=new QF2_Magnet((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
//{$a->renderBox('QF2_Magnet','Record Magnet Bolus')}   
Class QF2_Magnet extends QF2_Base {

    function definition($form) {


        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 

        $form->addElement('checkbox', 'MagnetText', array(), array('label'=> 'Magenet Given'));
        $checkbox=$form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Magnet is required.");

        
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('MagnetText' => true))); //set defaults
        
        return $form; //required       
    }

    function process($value) {
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);
        
        $magnet='true';
        var_dump($value);
         //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.medical_magnet (medical_case_id,bovine_id,event_time,magnet,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','$magnet','{$GLOBALS['auth']->getUsername()}')";
    
            $res = $GLOBALS['pdo']->exec($query);
    }

}//end class

 
Class QF2_Ketone extends QF2_Base {

    function definition($form) {

        $ketoneArray = array(null => 'Please Select One');
        for ($counter = 0; $counter <= 20.1; $counter += 0.10) {
            $value = round($counter, 1); //mmol/liter
            $ketoneArray["$value"] = $value;
        }

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
        $form->addElement('select', 'ketone', null, array('options' => $ketoneArray, 'label' => 'Ketone Value'))->addRule("required", "Ketone is required.");
        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");
        
        return $form; //required       
    }

    function process($value) {
            
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);

        
         //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;
            
            $query = "INSERT INTO bovinemanagement.medical_ketone (medical_case_id,bovine_id,event_time,ketone,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','{$value['ketone']}','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);
    }

}//end class

Class QF2_CMT extends QF2_Base {

    function definition($form) {

       
        
         $cmtArray=array('N'=>'N','T'=>'T','1'=>'1','2'=>'2','3'=>'3');
        
    

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form, $this->param->aniNumber, $this->mode);

        $fieldset = $form->addFieldset('udderAll')->setLabel('');
        $group = $fieldset->addGroup('udderFront')->setSeparator('&nbsp;&nbsp;&nbsp;&nbsp;')->setLabel('Front (lf/rf)&nbsp;&nbsp;');
        $group->addElement('select', 'cmt_lf', null, array('options' => $cmtArray, 'label' => 'CMT Value'))->setLabel('LF');
        $group->addElement('select', 'cmt_rf', null, array('options' => $cmtArray, 'label' => 'CMT Value'))->addRule("required", "CMT is required.");

        $group2 = $fieldset->addGroup('udderRear')->setSeparator('&nbsp;&nbsp;&nbsp;&nbsp;')->setLabel('Rear (lr/rr)&nbsp;&nbsp;');
        $group2->addElement('select', 'cmt_lr', null, array('options' => $cmtArray, 'label' => 'CMT Value'))->addRule("required", "CMT is required.");
        $group2->addElement('select', 'cmt_rr', null, array('options' => $cmtArray, 'label' => 'CMT Value'))->addRule("required", "CMT is required.");

        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");

        //defaults
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('udderFront' => array('cmt_lf' => '0','cmt_rf' => '0'),'udderRear' => array('cmt_lr' => '0','cmt_rr' => '0'))));
        
   
        
        
        return $form; //required       
    }

    function process($value) {
            
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);

   
        
        
         //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;
            
            $query = "INSERT INTO bovinemanagement.medical_cmt (medical_case_id,bovine_id,event_time,cmt_lf,cmt_rf,cmt_lr,cmt_rr,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','{$value['udderFront']['cmt_lf']}','{$value['udderFront']['cmt_rf']}','{$value['udderRear']['cmt_lr']}','{$value['udderRear']['cmt_rr']}','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);
    }

}//end class



    /**
     * ********************************
     */
    /**
     * Medical Case Diagnosis
     */
Class QF2_MedicalDiagnosis extends QF2_Base {

    function definition($form) {

      
        $diagArr=self::createDiagnosisArray();
        
        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday(); //hourly array, with unix timestamp as index.
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
        $form->addElement('select', 'diagnosis', null, array('options' => $diagArr, 'label' => 'Diagnosis'))->addRule("required", "Diagnosis is required.");
        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");
        
        return $form; //required       
    }

    function process($value) {
            
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);

        
         //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;
            
            $query = "INSERT INTO bovinemanagement.medical_diagnosis (medical_case_id,bovine_id,event_time,diagnosis_type_id,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','{$value['diagnosis']}','{$GLOBALS['auth']->getUsername()}')";
           
            $res = $GLOBALS['pdo']->exec($query);
    }

    
     public static function createDiagnosisArray() {
        // create diagnonis array
        $sql = "SELECT id,diagnosis FROM bovinemanagement.medical_diagnosis_type ORDER BY diagnosis";
        $res = $GLOBALS['pdo']->query($sql);
        $diagnosisArray = array(null => 'Please Select One');
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $diagnosisArray [$row ['id']] = $row ['diagnosis'];
        }

        return $diagnosisArray;
    }
    
}//end class



/*
 * FIXME: this can probably be a table overlay and combined with other comments. 
 * 
 */
Class QF2_MedicalComment extends QF2_Base {

    function definition($form) {

         $cannedComment = array(null => 'Please Select one...', 'Clear, not treated.' => 'Clear, not treated.', 'not cured - not treated' => 'not cured - not treated', 'Observed to not be eating.' => 'Observed to not be eating.', 'Broken Needle.' => 'Broken Needle.');


        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");
        
        
        if (empty($cannedComment) == false) {
            $comment1=$form->addElement('select', 'comment1', null, array('options' => $cannedComment, 'label' => 'Canned Comment'));
        } else {
            $comment1=$form->addElement('hidden', 'comment1', '')->setLabel("Generic Comment");
        }
        $comment2=$form->addElement('textarea', 'comment2', array('rows' => 6, 'col' => 40))->setLabel("Custom Comment");
        
      
        $comment2->addRule('nonempty','Canned or Custom Comment is Required.')->or_($comment1->createRule('nonempty')); //one or the other needs something enetered. 
        
        return $form; //required       
    }

    function process($value) {
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);

        
              //concatenate comments if they are both entered.
                if (strlen($value['comment1']) == 0) {
                    $comment = $value['comment2'];
                } else {
                    $comment = $value['comment1'] . ' ' . $value['comment2'];
                }

                 $comment_esc = pg_escape_string($comment);
        
        
         //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

            $query = "INSERT INTO bovinemanagement.medical_comment (medical_case_id,bovine_id,event_time,comment,userid) VALUES ($medical_case_id,{$value['bovine_id']},'{$value['event_time']}','$comment_esc','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);
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

    
}//end class

Class QF2_OpenMedicalCase extends QF2_Base {

    function definition($form) {
        
        //$aniNumber=MedicalCase::lookupAnimalsWhoDontHaveOpenMedicalCase(); 
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
        
        
        

        // create diagnonis array
        $diagnosisArray = QF2_MedicalDiagnosis::createDiagnosisArray();

        
        // $form->addElement('select', 'animalNumber', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Animal is required.");
         $form->addElement('select', 'diagnosis', null, array('options' => $diagnosisArray, 'label' => 'Diagnosis'))->addRule("required", "Diagnosis is required.");
          
       
        return $form; //required       
    }

    function process($value) {
            
       $bovine_id=$value['animalSelected'];
       $diagnosis_type_id=$value['diagnosis'];
       $event_time = date('Y-m-d H:i:sO', strtotime("now"));
           
            
        
        
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
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
        
        
       
    }

    
    
}//end class


Class QF2_CloseMedicalCase extends QF2_Base {

    function definition($form) {
        
        
         // grab a list of cows with open medical cases.
        $aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase();

       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
       
        return $form; //required       
    }

    function process($value) {
            
       $bovine_id=$value['animalSelected'];
      
       $event_time = date('Y-m-d H:i:sO', strtotime("now"));
           
            
        
        
        // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();
            
            
 // lookup bovine id and see if there is an open medical case during
            // the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($bovine_id, $event_time);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

            // don't put id value, uses next one in DB.

            $query = "UPDATE bovinemanagement.medical_case SET close_date='$event_time',close_userid='{$GLOBALS['auth']->getUsername()}' WHERE id=$medical_case_id";


      
            $res = $GLOBALS['pdo']->exec($query);
            
            
            
            // determine if the commit or rollback
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
        
        
       
    }
    
}//end class
