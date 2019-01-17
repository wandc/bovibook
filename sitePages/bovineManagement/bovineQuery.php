<?php

 include_once('functions/googleVisualization.inc');


/**
 * ********************************************
 * Class to display info on a bovine.
 * Dec 24, 2008
 * ********************************************
 */
class BovineQuery extends TabPage {

    private $bovineID;
    private $bovineFullRegNumber;
    private $bovineLocalNumber;
    private $bovineShortName;
    private $bovineRfidNumber;

    public function __construct() {
        parent::__construct(); //pageid looked after here. 

        if ((isset($_REQUEST['bovine_id']) == false) || ($_REQUEST['bovine_id'] == null)) {
            $_REQUEST['bovine_id'] = -1; //dummy value
        }
        if (!filter_var($_REQUEST['bovine_id'], FILTER_VALIDATE_INT) === false) {
            $this->bovineID = $_REQUEST['bovine_id'];
        } else {
            throw new Exception('Error: bovineID is request variable is not an interger.');
        }

        if (isset($_REQUEST ['bovine_local_number']) == false) {
            $_REQUEST ['bovine_local_number'] = null;
        }
        // nothing
    }

// end constructor
    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    public function defaultDisplay() {




        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);



        // fill in variables with information from the cow.
        // when cow number is selected from select list of animal query of
        // quickform.
        if ($_REQUEST ['bovine_id'] != null) {
            // do up class variables.
            $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);

            // call main display file.

            $this->renderTabsParent($this->tabsSetup());
        }  // when cow number is typed in directly to text form.
        elseif (($_REQUEST ['bovine_local_number'] != null) && (is_numeric($_REQUEST ['bovine_local_number']) == true)) {
            // lookup bovine_id
            $sql = "SELECT id FROM bovinemanagement.bovine WHERE local_number='{$_REQUEST['bovine_local_number']}' LIMIT 1;";
            $res = $GLOBALS ['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if ($res->rowCount() > 0) {    // only go ahead if cow was a valid number.
                $_REQUEST ['bovine_id'] = $row ['id'];
                $this->loadVars($row ['id'], $_REQUEST ['pageid']);

                $this->renderTabsParent($this->tabsSetup());
            } else {
                print ("<h2> Not a valid cows number, choose again above:</h2>");
                //self::AnimalChooseContainer();
            }
        } else {
            // no cow selected.
            self::AnimalChooseContainer();
        }
    }

    private function tabsSetup() {

        if ($this->bovineID != null) {

            if (!empty($this->bovineFullRegNumber)) {
                //setup for tabs.
                $tabArray[1]['name'] = 'Summary';
                $tabArray[1]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[2]['name'] = 'Movement';
                $tabArray[2]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[3]['name'] = 'Medical';
                $tabArray[3]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[4]['name'] = 'Reproduction';
                $tabArray[4]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[5]['name'] = 'Production';
                $tabArray[5]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[6]['name'] = 'Family Tree';
                $tabArray[6]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[7]['name'] = 'Genetics';
                $tabArray[7]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[8]['name'] = 'Breeding Choice';
                $tabArray[8]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[9]['name'] = 'Estrus Detector';
                $tabArray[9]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
            } else {
                //un registered animal   
                //setup for tabs.
                $tabArray[1]['name'] = 'Summary';
                $tabArray[1]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[2]['name'] = 'Movement';
                $tabArray[2]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[3]['name'] = 'Medical';
                $tabArray[3]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
            }


            return $tabArray;
        } else {
            throw new Exception("ERROR: bovine_id can't be determined for tabs.<br/>");
        }
    }

    private function loadVars($bovine_id = null, $pageid, $local_number = null) {
 
        if ($bovine_id != -1) {
            $this->bovineID = $bovine_id;
            $this->pageID = $pageid;


            // lookup bovine_id
            $sql = "SELECT full_reg_number,local_number,full_name,rfid_number FROM bovinemanagement.bovine WHERE id=$this->bovineID LIMIT 1;";
            $res = $GLOBALS ['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            // fill class variables.
            $this->bovineFullRegNumber = $row ['full_reg_number'];
            $this->bovineLocalNumber = $row ['local_number'];
            $this->bovineRfidNumber = $row ['rfid_number'];
            $this->bovineShortName = Misc::femaleShortName($row['full_name']);
            /*
              //DEBUG
              print("BOVINE ID is: {$this->bovineID}.<br/>");
              print("pageID ID is: {$this->pageID}.<br/>");
              print("bovineFullRegNumber ID is: {$this->bovineFullRegNumber}.<br/>");
              print("bovineLocalNumber ID is: {$this->bovineLocalNumber}.<br/>");
             */
        } elseif ($local_number != null) {
           
            $this->pageID = $pageid;
            $this->bovineLocalNumber = $local_number;


            $sql = "SELECT full_reg_number,id,rfid_number FROM bovinemanagement.bovine WHERE local_number=$this->bovineLocalNumber LIMIT 1;";
            $res = $GLOBALS ['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            // fill class variables.
            $this->bovineRfidNumber = $row ['rfid_number'];
            $this->bovineFullRegNumber = $row ['full_reg_number'];
            $this->bovineID = $row ['id'];
        }
    }

    function AnimalChooseContainer() {

        print ("<h2>Choose Animal From Header Above.</h2>");
    }

    /**
     * Called before render tabs to add the top of page stuff before the tabs (name, slect form, etc.)
     */
    function renderTabsParent($tabArray) {
        // display picture of cow


        print ('<div id="bovineQueryTitle">');




        if ($_REQUEST ['bovine_id'] != null) {
            $this->displayName($this->bovineID);
        }


        print ('</div>');


        $this->renderTabs($tabArray);
    }

    /*
     * work around for breeding choice not being a real page for reference with ajax call to page 141 in display jquerydatatable.
     */
    function progenyBreedingChoiceForBovine($request) {
     return (new BovineQueryBreedingChoice)->progenyBreedingChoiceForBovine($request) ;
}
 function breedingChoicesHistorical($request) {
     return (new BovineQueryBreedingChoice)->breedingChoicesHistorical($request) ;
}

    
    //breeding choice
    function tab8() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryBreedingChoice.inc');
        $bovineQueryBreedingChoice = new BovineQueryBreedingChoice($this->bovineID, $this->bovineLocalNumber, $this->bovineShortName);
        $bovineQueryBreedingChoice->main();
    }

    //summary
    function tab1() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);

        // print("<div id='twoColumnLayout'>");

        $this->summaryMovement();
        $this->summaryReproduction();
        $this->summaryPrice();
        $this->summaryPicture();
        // print('</div>');
    }

    //reproduction
    function tab4() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);

        print("<div class='row'>" . "\n\r");
        print("<div class='col-sm-4 col-sm-push-8'>" . "\n\r");
        $this->showAccordionReproduction($this->bovineID);
        print("</div>" . "\n\r");
        print("<div class='col-sm-8 col-sm-pull-4'>" . "\n\r");
        $this->reproTabContent();
        print("</div>" . "\n\r");
        print("</div>" . "\n\r");
    }

    function reproTabContent() {

        $res2 = $GLOBALS['pdo']->prepare('SELECT * from bovinemanagement.bovinecurr WHERE id=? LIMIT 1');
        $res2->execute(array($this->bovineID));
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        $local_number = $row2 ['local_number'];


        print ('<div id="reproTabInfo">');


        // do info list
        $this->printPreSelectedSireChoices();
        // breeding VWP
        print ("<b>VWP:</b> " . $this->breedingVoluntaryWaitingPeriod($this->bovineID)) . "<br/>";
        // due
        $this->printDueDate();

        print ('</div><!--End Repro Tab Info -->');
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusKamar.inc');

        $estrusKamar = new EstrusKamar();
        $estrusKamar->displayLastAllKamarEvents($this->bovineID);

        print ('<div id="bovine_query_log">');
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryReproductionLog.inc');
        $bovineQueryReproductionLog = new BovineQueryReproductionLog($this->bovineID, $this->bovineFullRegNumber);
        print($bovineQueryReproductionLog->main());
        print ('</div>');


        print ("<hr />");
    }

    function showAccordionReproduction($bovine_id) {

        $accordionArray[1]['name'] = 'Heat';
        $accordionArray[2]['name'] = 'Hormone';
        $accordionArray[3]['name'] = 'To Breed';
        $accordionArray[4]['name'] = 'Comment';
        $accordionArray[5]['name'] = 'Kamar';
        $accordionArray[6]['name'] = 'Preg Check';

        $accordion = new AccordionImplementationReproduction($bovine_id);
        $accordion->setCSS('accordionNarrow');
        $accordion->render($accordionArray);
    }

    function showAccordionMovement($bovine_id) {

        $accordionArray[1]['name'] = 'Movement';
        $accordionArray[2]['name'] = 'Sort';
        $accordionArray[3]['name'] = 'Potential Cull';
        $accordionArray[4]['name'] = 'Feet';
        $accordionArray[5]['name'] = 'Ear Tag';

        $accordion = new AccordionImplementationMovement($bovine_id);
        $accordion->setCSS('accordionNarrow');
        $accordion->render($accordionArray);
    }

    function printPreSelectedSireChoices() {


        require_once ($_SERVER ['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/bovineQueryBreedingChoice.inc');
        $bovineQueryBreedingChoice = new BovineQueryBreedingChoice($this->bovineID, $this->bovineLocalNumber, $this->bovineShortName);
        $retArr = $bovineQueryBreedingChoice->nextBreedingAction($this->bovineID);


        $row = $retArr['row'];

        //change blanks to question marks
        if (isset($row['p1_name'])) {
            $row['p1_name'] = self::changeNulltoQueestionMark($row['p1_name']);
            if (($row['p1_name'] != null) AND ($row['p1_avail']==null)) {
              $row['p1_name'] =   '<s>'.$row['p1_name'].'</s>';
            }
        }
        if (isset($row['p2_name'])) {
            $row['p2_name'] = self::changeNulltoQueestionMark($row['p2_name']);
            if (($row['p2_name'] != null) AND ($row['p2_avail']==null)) {
              $row['p2_name'] =   '<s>'.$row['p2_name'].'</s>';
            }
        }
        if (isset($row['p3_name'])) {
            $row['p3_name'] = self::changeNulltoQueestionMark($row['p3_name']);
            if (($row['p3_name'] != null) AND ($row['p3_avail']==null)) {
              $row['p3_name'] =   '<s>'.$row['p3_name'].'</s>';
            }
        }
        if (isset($row['s1_name'])) {
            $row['s1_name'] = self::changeNulltoQueestionMark($row['s1_name']);
            if (($row['s1_name'] != null) AND ($row['s1_avail']==null)) {
              $row['s1_name'] =   '<s>'.$row['s1_name'].'</s>';
            }
        }
        if (isset($row['s2_name'])) {
            $row['s2_name'] = self::changeNulltoQueestionMark($row['s2_name']);
            if (($row['s2_name'] != null) AND ($row['s2_avail']==null)) {
              $row['s2_name'] =   '<s>'.$row['s2_name'].'</s>';
            }
        }
        if (isset($row['s3_name'])) {
            $row['s3_name'] = self::changeNulltoQueestionMark($row['s3_name']);
            if (($row['s3_name'] != null) AND ($row['s3_avail']==null)) {
              $row['s3_name'] =   '<s>'.$row['s3_name'].'</s>';
            }
        }


        print ("Next Repro Action: <span class='large1_1'>{$retArr['nextActionTxt']}</span><br/>");
        
        print ("Service Sire Choices: 1<sup>st</sup>:"). str_replace(',,','',"{$row['p1_name']},{$row['p2_name']},{$row['p3_name']}, Later: {$row['s1_name']},{$row['s2_name']},{$row['s3_name']}<br/>");
    }

    function changeNulltoQueestionMark($txt) {
        if (($txt == '') OR ( $txt == null)) {
            return '?';
        } else {
            return $txt;
        }
    }

    function printDueDate() {
        // check to see if the cow is already in the DB and if it is, use the
        // default values already in.
        $row = null;
        $sql = "SELECT preg_check_method,calculated_potential_due_date,service_sire_short_name,service_sire_full_reg_number FROM bovinemanagement.pregnant_view WHERE id={$this->bovineID} LIMIT 1";

        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
            print ("<b>DUE:</b>:" . $this->daysTillandDate($row ['calculated_potential_due_date']) . " by {$row['preg_check_method']} to <b>{$row['service_sire_short_name']}</b> <span id='small'>({$row['service_sire_full_reg_number']})</span><br/>");
        }
    }

    //medical
    function tab3() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);


        print("<div class='row'>" . "\n\r");
        print("<div class='col-sm-4 col-sm-push-8'>" . "\n\r");
       self::showMedicalQuickformAccordion($this->bovineID);
        print("</div>" . "\n\r");
        print("<div class='col-sm-8 col-sm-pull-4'>" . "\n\r");
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryMedicalLog.inc');
        $bovineQueryMedicalLog = new BovineQueryMedicalLog($this->bovineID, $this->bovineFullRegNumber);
        print($bovineQueryMedicalLog->main());
        print("</div>" . "\n\r");
        print("</div>" . "\n\r");
    }

    /* called here */
    public static function showMedicalQuickformAccordion($bovine_id) {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');

        //NOTE: array numbers matter, don't change without changing tab functions.
        $accordionArray[1]['name'] = 'General Treatment';
        $accordionArray[3]['name'] = 'Temperature';
        $accordionArray[4]['name'] = 'Ketone';
        $accordionArray[5]['name'] = 'Magnet';
        $accordionArray[6]['name'] = 'Comment';
        $accordionArray[7]['name'] = 'Vet Check';
        $accordionArray[8]['name'] = 'Schedule Action';
        //$accordionArray[8]['name'] = 'Body Cond. Score'; //FIXME, make seperate accordian.
        //$accordionArray[9]['name'] = 'Feet'; //FIXME, make seperate accordian.
        //$accordionArray[10]['name'] = 'Ear Tag'; //FIXME, make seperate accordian.
        $accordion = new AccordionImplementationMedicalQuickforms($bovine_id);
        $accordion->setCSS('accordionNarrowBovineQuery');
        $accordion->render($accordionArray);
    }

    private function showLatestRFIDStatsParlor($bovineID) {
        print ('<h3>RFID Status (at Parlor)</h3>');

        // show latest stats for a cow
        $sql = "SELECT bovine_id,date,idtimemm,manualid,milktime FROM alpro.cow WHERE bovine_id=$bovineID AND date >= now() - interval '10 days' ORDER BY date DESC";
        $res2 = $GLOBALS ['pdo']->query($sql);


        if ($res2->rowCount() > 0) {
            while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
                //checks for manual id, so if manual id was pressed we assume id failed.
                if ($row2 ['manualid'] == FALSE) {
                    $idStatus = 'Successful';
                    $idTimeRaw = strtotime($row2 ['idtimemm']);
                    $idTime = date('H:i', $idTimeRaw);
                    $ftime = date('M j, Y', $idTimeRaw);
                } else { // assume true
                    // for some reason the time of manual id is always 20:00, so use
                    // the milk time instead.
                    $idTimeRaw = strtotime($row2 ['milktime']);
                    $idTime = date('H:i', $idTimeRaw);
                    $ftime = date('M j, Y', $idTimeRaw);
                    $idStatus = 'Failed';
                }

                $str = "<b>RFID</b>: " . Misc::daysOrHoursAgo($idTimeRaw) . " <b class=\"smallish\">($ftime)</b> $idTime Auto ID <b>$idStatus</b> ";
                $outArray [] = ($str);
            }

            foreach ($outArray as $key => $value) {
                print ($value . '<br/>');
            }
        } else {
            print("No Recent RFID Reads.<br/>");
            $outArray = null;
        }

        return $outArray;
    }

    private function showLatestRFIDStatsSortGate($bovineID) {
        print ('<h3>RFID Status (at Sort Gate)</h3>');

        // show latest stats for a cow
        //$sql = "SELECT bovine_id,event_time,data_time FROM alpro.sort_gate_log WHERE bovine_id=$bovineID AND event_time >= now() - interval '10 days' ORDER BY event_time DESC";
        //$res2 = $GLOBALS ['pdo']->query($sql);

        $sql = "with temp as (
select generate_series(current_date+interval '12 hours', current_date-interval '10 day', '-12 hour'::interval) as time
)
SELECT  time, (SELECT event_time FROM alpro.sort_gate_log WHERE bovine_id=$bovineID AND event_time >= temp.time AND event_time < temp.time + interval '12 hour' limit 1) as event_time
from temp
";
        $res2 = $GLOBALS ['pdo']->query($sql);

        if ($res2->rowCount() > 0) {
            while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {

                $idTimeRaw = strtotime($row2 ['time']);
                $ftime = date('M j A', $idTimeRaw);

                if ($row2['event_time'] == '') {
                    $idTime = '';
                    $idStatus = 'Failed';
                } else {
                    $idTime = date('H:i', strtotime($row2['event_time']));
                    $idStatus = 'Successful';
                }

                $str = "<b>RFID</b>: " . Misc::daysOrHoursAgo($idTimeRaw) . " <b class=\"smallish\">($ftime)</b> $idTime Auto ID <b>$idStatus</b> ";
                $outArray [] = ($str);
            }

            foreach ($outArray as $key => $value) {
                print ($value . '</br>');
            }
        } else {
            print("No Recent RFID Reads.<br/>");
            $outArray = null;
        }

        return $outArray;
    }

    //MOVEMENT TAB
    function tab2() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);


        print("<div class='row'>" . "\n\r");
        print("<div class='col-sm-4 col-sm-push-8'>" . "\n\r");
        $this->showAccordionMovement($this->bovineID);
        print("</div>" . "\n\r");
        print("<div class='col-sm-8 col-sm-pull-4'>" . "\n\r");
        //
       
         //sort gate
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementSortGate.inc');
        $ret=MovementSortGate::individualCowSortStatus($this->bovineID);
         print(' <div class="col-lg-3 col-xs-6">');
       $colour='purple';
       print(BootStrap::bootstrapSmallBox('Sort Gate',$ret,null,$colour,'ion ion-ios-arrow-thin-right'));     
       print('</div>'); print('</br>');
       
        
        
        
        print("<h3> Tracking </h3>");
        //show location map
        try {

            require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusDetector.inc');
            $obj = new Trilateration();
            $obj->oneCow($this->bovineID);
        } catch (\Exception $e) {
            // var_dump($e->getMessage());
            //show nothing.
        }

        print("<h3> Location Log </h3>");
        print($this->locationAll());

        
        
       
        
          
          
        print("<h3> RFID Stats </h3>");
        $this->showLatestRFIDStatsParlor($this->bovineID);
        $this->showLatestRFIDStatsSortGate($this->bovineID);
        //      
        print("</div>" . "\n\r");
        print("</div>" . "\n\r");
    }

    private function locationAll() {
        $str = '';
        $outArray = array();
        $outArray = queryPageHelper::calculateLactationLineBreaks($this->bovineID, $outArray);
        $outArray = $this->showLocationLog($outArray);
        $str = $str . (QueryPageHelper::sortOutArray($outArray)); // print out sorted array.

        return $str;
    }

    /*
     * Estrus Detector Test
     * 
     */

    function tab9() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);

        /* draws a chart of last events */
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusDetector.inc');
        $obj = new EstrusDetector();
        $obj->oneCowLatestDataChartFilteredVar($this->bovineID); //filtered data
        $obj->oneCowLatestDataChart($this->bovineID); //raw data
    }

    /**
     * Genetics from Holstein Canada
     */
    function tab7() {

        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        $regNumber = substr($this->bovineFullRegNumber, 6, strlen($this->bovineFullRegNumber));
        
       $haplotype=BovineQueryBreedingChoice::showHaplotypeInfo($this->bovineID);
        
$html = <<<HTML
<div class="row">
  <div class="col-md-8">
        <div class="col-md-4">
        {$this->geneticIndexChangeOverTime($regNumber)}
        </div>
        <div class="col-md-4">
        {$haplotype}
        </div>
  </div>
  <div class="col-md-4">
      {$this->typeClassificationChart($regNumber)}
  </div>
</div>
                
HTML;
  

       
        
      print ($html);
    }
    
    function typeClassificationChart($regNumber) {
        
         /**
         * plot holstein canada classification as a bar graph
         */
        $query = "SELECT * FROM batch.holstein_canada_data_curr WHERE reg_no='$regNumber' LIMIT 1";
        $res = $GLOBALS ['pdo']->query($query);

        $row = $res->fetch(PDO::FETCH_OBJ); //get as an object.
        $one = get_object_vars($row); //array off one record with column names as keys.

        foreach ($one as $key => $value) {
            if (strstr($key, 'conf_') == true) {
                if (is_numeric($value) == true) {
                    $three[$key] = $value;
                }
            }
        }
        unset($three['conf_type_reliability']); //too big of number
        //print_r2($three);



        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string';
        $headerStrArr['Confirmation'] = 'number';


        $opt = "                  
                     
                         width:700, height:700,
                          legend: {position: 'none'},
          vAxis: {title: 'Trait',  titleTextStyle: {color: 'red'}}
          ";
        $x = new GoogleVisBar($opt, $headerStrArr, $three);

        return (new BootStrap)->plainBox('Confirmation Chart',$x->toString());
    }
    
    function geneticIndexChangeOverTime($regNumber) {
$sql = <<<SQL
SELECT extract_date as "Date" ,pro_doll as "Pro$",lpi as "LPI",gebv as "GEBV"
    FROM batch.holstein_canada_data 
        WHERE reg_no='$regNumber' 
            ORDER BY extract_date DESC
SQL;
           $out[]=( (new JQueryDataTable)->startBasicSql($sql)); 
        return (new BootStrap)->plainBox('Genetic Index Change Over Time',implode($out));
    }
    

    //tab FamilyTree
    function tab6() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);


        self::showFamilyTree();

        // self::draw4GenFamilyTree();
    }

    function showFamilyTree() {

        print ("<h3>Family Tree</h3>");

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reports/familyTree.inc');
        FamilyTree::drawFemaleFamilyTree($this->bovineFullRegNumber);
    }

    function draw4GenFamilyTree() {
        // take registration number and look up info on cdn.
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/cdn.inc');

        try {
            $animal = getBovineCDNData($this->bovineFullRegNumber);
            $animal_dam = getBovineCDNData($animal ['dam_full_reg_number']);
            $animal_sire = getBovineCDNData($animal ['sire_full_reg_number']);
            //
            $animal_dam_dam = getBovineCDNData($animal_dam ['dam_full_reg_number']);
            $animal_dam_sire = getBovineCDNData($animal_dam ['sire_full_reg_number']);
            $animal_sire_dam = getBovineCDNData($animal_sire ['dam_full_reg_number']);
            $animal_sire_sire = getBovineCDNData($animal_sire ['sire_full_reg_number']);
            //
            $animal_dam_dam_dam = getBovineCDNData($animal_dam_dam ['dam_full_reg_number']);
            $animal_dam_sire_dam = '';
            $animal_sire_dam_dam = ''; //don't show dam's of sires to speed up code.
            $animal_sire_sire_dam = '';
            $animal_dam_dam_sire = getBovineCDNData($animal_dam_dam ['sire_full_reg_number']);
            $animal_dam_sire_sire = getBovineCDNData($animal_dam_sire ['sire_full_reg_number']);
            $animal_sire_dam_sire = getBovineCDNData($animal_sire_dam ['sire_full_reg_number']);
            $animal_sire_sire_sire = getBovineCDNData($animal_sire_sire ['sire_full_reg_number']);
        } catch (Exception $e) {
            Print("<h2>CDN data not fully available or data corrupt,usually means animal is too young and not registered.</h2>");
        }


        //Now use google visulations to show the data in an org chart
        ?>

        <script type="text/javascript">
            function drawVisualizationF() {
                // Create and populate the data table.
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'Name');
                data.addColumn('string', 'Parent');
                data.addColumn('string', 'ToolTip');
                //
                data.addRows(15);

                //row 0
                data.setCell(0, 0, '0', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal['bovine_full_reg_number'])); ?>"><?php echo($animal['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal['bovine_birthdate']); ?><i></font>');
                //row 1
                data.setCell(1, 0, '1f', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(1, 1, '0');
                //row 2
                data.setCell(2, 0, '1m', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_sire['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(2, 1, '0');
                //row 3
                data.setCell(3, 0, '2ff', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_dam_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(3, 1, '1f');
                //row 4 
                data.setCell(4, 0, '2fm', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_dam_sire['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(4, 1, '1f');
                //row 5 
                data.setCell(5, 0, '2mm', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_sire_sire['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_sire_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(5, 1, '1m');
                //row 6 
                data.setCell(6, 0, '2mf', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_sire_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_sire_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(6, 1, '1m');
                /////////////////////////
                //   //row 7
                data.setCell(7, 0, '3fff', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam_dam_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_dam_dam_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam_dam_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(7, 1, '2ff');
                //row 8 
                data.setCell(8, 0, '3ffm', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam_dam_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_dam_dam_sire['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam_dam_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(8, 1, '2ff');
                //row 9 
                data.setCell(9, 0, '3fmf', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam_sire_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_dam_sire_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam_sire_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(9, 1, '2fm');
                //row 10 
                data.setCell(10, 0, '3fmm', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_dam_sire_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_dam_sire_sire['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_dam_sire_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(10, 1, '2fm');
                //   //row 11
                data.setCell(11, 0, '3mmf', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire_sire_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_sire_sire_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_sire_sire_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(11, 1, '2mm');
                //row 12 
                data.setCell(12, 0, '3mmm', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire_sire_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_sire_sire_sire['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_sire_sire_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(12, 1, '2mm');
                //row 13
                data.setCell(13, 0, '3mff', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire_dam_dam['bovine_full_reg_number'])); ?>"><?php echo($animal_sire_dam_dam['bovine_full_name']); ?></a><br/><font color="red"><i><?php echo($animal_sire_dam_dam['bovine_birthdate']); ?><i></font>');
                data.setCell(13, 1, '2mf');
                //row 14 
                data.setCell(14, 0, '3mfm', '<a href="<?php echo(Misc::createListOfAllCowsMilking($animal_sire_dam_sire['bovine_full_reg_number'])); ?>"><?php echo($animal_sire_dam_sire['bovine_full_name']); ?></a></a><br/><font color="red"><i><?php echo($animal_sire_dam_sire['bovine_birthdate']); ?><i></font>');
                data.setCell(14, 1, '2mf');


                // Create and draw the visualization.
                new google.visualization.OrgChart(document.getElementById('visualizationF')).
                        draw(data, {allowHtml: true});
            }
            setTimeout(drawVisualizationF, 200); //firefox workaround.         
            google.charts.setOnLoadCallback(drawVisualizationF, true);
            $(window).resize(function(){
            google.charts.setOnLoadCallback(drawVisualizationF);
            });
        </script>
        <?php
        print(' <div id="visualizationF" style="width: 100%; height: 600px;"></div>');



        /*
          print ($animal['bovine_full_name'] . ' - ' . $animal ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_dam ['bovine_full_name'] . ' - ' . $animal_dam ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_sire ['bovine_full_name'] . ' - ' . $animal_sire ['bovine_birthdate'] . '<br/>' . "\n\r");

          print ($animal_dam_dam ['bovine_full_name'] . ' - ' . $animal_dam_dam ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_dam_sire ['bovine_full_name'] . ' - ' . $animal_dam_sire ['bovine_birthdate'] . '<br/>' . "\n\r");

          print ($animal_sire_dam ['bovine_full_name'] . ' - ' . $animal_sire_dam ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_sire_sire ['bovine_full_name'] . ' - ' . $animal_sire_sire ['bovine_birthdate'] . '<br/>' . "\n\r");

         */
    }

    //tab production
    function tab5() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);

   
        
        //check she has a lactation, or don't show anything.
        $sql = "SELECT id FROM bovinemanagement.lactation WHERE bovine_id=({$this->bovineID})";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() > 0) {


            print("<div class='row'>" . "\n\r");
            print("<div class='col-md-6'>" . "\n\r");
            $this->productionAll();
            print("</div>" . "\n\r");
            print("<div class='col-md-6'>" . "\n\r");
            $this->previousLactationInfo($this->bovineFullRegNumber); //accordian
            print("</div>" . "\n\r");
            print("</div>" . "\n\r");
        } else {
            print('<h2>No Lactations.</h2>');
        }
    }

    private function productionAll() {
          print($this->productionItems($this->bovineID));

          
          $ownerStats='';
            if ((in_array('owner', $GLOBALS ['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS ['auth']->getAuthData('groups')) == TRUE)) {
            $out[]=($this->getLatestLactationRevenue($this->bovineID));
             $out[]=($this->getPercentageManuallyMilked($this->bovineID));
             $out[]=($this->getPercentageReattatched($this->bovineID));
             $out[]=($this->findMastitisDoses($this->bovineID));
             $ownerStats= (new BootStrap)->plainBox('Milking Stats',implode($out));
        }
          
         
$html = <<<HTML
   <div class="row">
  <div class="col-md-4">{$this->getLatestMilking($this->bovineID)}</div>
  <div class="col-md-4"> {$this->getLatestAdlicTest($this->bovineFullRegNumber)}</div>
  <div class="col-md-4"> $ownerStats</div>
  
</div>   
HTML;
      print($html);    
          

       $this->plotLatestLactation($this->bovineFullRegNumber); //causes screen to be blank. document.ready doesnt work on tab switch.

      
    }

    function findMastitisDoses($bovine_id) {
        $sql = "SELECT (SELECT count(for_mastitis) FROM bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine_index=medicine.id WHERE for_mastitis is true AND bovine_id=$bovine_id) as dose_count";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row['dose_count'])) {
            $ans = $row['dose_count'];
            return ("Lifetime Mastitis Doses: <b>$ans</b>.<br/>");
        }
    }

    //ppp
    function productionItems($bovineID) {

        //milk withholding
        if (!empty(BovineQueryMedicalLog::milkWithHolding($bovineID))) {
            $txt = '<h3>Milk: <b>Withhold</b></h3> <span class="smallish">until ' . date('D M j/Y ga', BovineQueryMedicalLog::milkWithHolding($bovineID)) . '</span>';
            $out[]= BootStrap::errorNotify(($txt));
        } else {
           $out[]= BootStrap::sucessNotify(( "<h3>Milk: Clear</h3>"));
        }

        //dim
$dim=(BootStrap::bootstrapSmallBox('DIM', self::findCurrentDIM($bovineID), null, 'purple', 'ion ion-ios-clock'));

$html = <<<HTML
     <div class="row">
  <div class="col-md-4">$dim</div>
  <div class="col-md-4"><!--.col-md-4--></div>
  <div class="col-md-4"><!--.col-md-4--></div>
  </div> 
HTML;

//print($html);
        $out[]= $html;
      

        //3 quarters
        $sql = "SELECT three_quarter FROM bovinemanagement.production_item WHERE bovine_id=$bovineID order by event_time DESC LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ((!empty($row['three_quarter'])) AND ( $row['three_quarter'] == 't')) {
          $out[]= BootStrap::warningNotify(( "<h3>Udder: 3 Quarter</h3>"));           
        }
        
        //slow milking (calculate) TODO

       
        return(implode($out));
    }

    function findCurrentDIM($bovine_id) {
        $sql = "SELECT date_trunc('days',now()- max(fresh_date)) as days_ago FROM bovinemanagement.bovinecurr WHERE id=$bovine_id AND dry_date is null";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        if (!empty($row['days_ago'])) {
            $ans = $row['days_ago'];
            return $ans;
        }
    }

    function getPercentageManuallyMilked($bovine_id) {
        $sql = "SELECT  (SELECT count(manualkey) FROM alpro.cow WHERE bovine_id=$bovine_id AND manualkey is true) as num,(SELECT count(manualkey) FROM alpro.cow WHERE bovine_id=$bovine_id) as dem";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        if (!empty($row['dem'])) {
            $ans = round($row['num'] / $row['dem'], 2) * 100;
            return ("Lifetime Manual Milking: <b>$ans %</b><br/>");
        }
    }

    function getPercentageReattatched($bovine_id) {
        $sql = "SELECT  (SELECT count(reattatch) FROM alpro.cow WHERE bovine_id=$bovine_id AND reattatch is true) as num,(SELECT count(reattatch) FROM alpro.cow WHERE bovine_id=$bovine_id) as dem";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        if (!empty($row['dem'])) {
            $ans = round($row['num'] / $row['dem'], 2) * 100;
            return ("Lifetime Milking Reattatched: <b>$ans %</b><br/>");
        }
    }

    function getLatestLactationRevenue($bovineID) {
        $sql = "SELECT batch.current_lactation_total_revenue({$bovineID})";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $rev = $row ['current_lactation_total_revenue'];
        return ("Current Lactation Milk Sales: <b>$$rev</b><br/>");
    }

    /**
     * shows accordian of previous lactation info
     */
    function previousLactationInfo($bovineFullRegNumber) {

        $accordionArray = array();
        $query = "SELECT max(lact_nu) as max_lact_nu,min(lact_nu) as min_lact_nu FROM batch.valacta_data WHERE reg='$bovineFullRegNumber'";
        $res = $GLOBALS ['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        // print out each lactation, full info in accordian tab
        if (is_numeric($row['max_lact_nu']) AND is_numeric($row['min_lact_nu'])) {
            for ($i = $row['max_lact_nu']; $i >= $row ['min_lact_nu']; $i--) {
                $accordionArray[$i]['name'] = "Lactation #$i"; //could be off by one.
            }
            $accordion = new AccordionImplementationPreviousLactations($bovineFullRegNumber);
            $accordion->setCSS('accordionNarrowBovineQuery');
         
   //previousLactationInfo         

 
            $accordion->render($accordionArray);
            //used because google visualization doesn't know what size to render chart in hidden div. so force resize when accordian tab opened.     
            $js= <<<JSCRIPT
<script>       
                    $( document ).ready(function() {
 $("#accordion_{$accordion->uuid}").accordion({ activate: function(event, ui) {
  var ev = document.createEvent('Event');
    ev.initEvent('resize', true, true);
    window.dispatchEvent(ev);
  }
}); 
 });
</script>     
JSCRIPT;
            print($js);
           // var_dump($accordion);
            
        }
    }

    function summaryMovement() {
        print ("<div class='summaryBlock'>");
        // get three latest locations
        $query = "SELECT event_time,date_trunc('days',now()-event_time) as days_ago,name,userid
FROM bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location_event.location_id = location.id
WHERE bovine_id={$this->bovineID} ORDER BY event_time DESC LIMIT 3";
        $res = $GLOBALS ['pdo']->query($query);
        $counter = 0;
        print ("<h1>Location</h1>");
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            if ($counter == 0) {
                print ("<h2>{$row['name']}</h2>");
            }
            $daysSince = $row ['days_ago'];
            $formattedDate = date("M d Y", strtotime($row ['event_time']));
            print ("<p>$daysSince (<a class=\"smallish\">$formattedDate</a>) {$row['name']}</p>");
            $counter++;
        }

        print ("</div> <!-- End of summaryBlock Movement -->");
    }

    function summaryReproduction() {
        print ("<div class='summaryBlock'>");
        print ("<h1>Reproduction</h1>");

        $this->printDueDate();

        $str = $this->breedingVoluntaryWaitingPeriod($this->bovineID);
        print ("<b>VWP:</b> $str <br/>");

        print ("</div> <!-- End of summaryBlock Reproduction -->");
    }

    private function breedingVoluntaryWaitingPeriod($bovine_id) {
        // get breeding vwp
        $query = "SELECT bovinemanagement.calculate_breeding_voluntary_waiting_period(id),date_trunc('day',now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(id)) from bovinemanagement.bovinecurr WHERE id={$bovine_id} LIMIT 1";
        $res = $GLOBALS ['pdo']->query($query);
        $str = '';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            if (strtotime($row ['calculate_breeding_voluntary_waiting_period']) >= strtotime("now")) {
                $str = ("Breeding VWP " . $this->daysTillandDate($row ['calculate_breeding_voluntary_waiting_period']));
            } else {
                $str = ("Breeding eligible for " . $this->daysTillandDate($row ['calculate_breeding_voluntary_waiting_period']));
            }
        }
        return $str;
    }

    /**
     * convert time until the number of days before or after and return with
     * date as string
     */
    private function daysTillandDate($time) {
        $strTime = strtotime($time);
        $dateTime = date('M j, Y', $strTime);
        $daysTime = Misc::daysOrHoursAgo(strtotime($time));
        // determine if it is in the future or not.
        if ($daysTime >= 0) {
            $subStr = $daysTime . ' ';
        } else {
            $subStr = 'in ' . abs($daysTime) . ' days ';
        }
        // return final string.
        $str = $subStr . " <b class=\"smallish\">($dateTime)</b>";
        return $str;
    }

    function summaryPicture() {
        print ("<div class='summaryBlock'>");
        print ("<h1>Picture</h1>");
        // show latest bovine pic.
        print ("<div id='bovinePic'>");
        print ("<img alt='{$this->bovineID}' src='/functions/displayBovinePic.php?bovine_id={$this->bovineID}'  height='240' width='320' />");
        print ('</div>');
        print ("</div> <!-- End of summaryBlock Picture -->" . "\n\r");
    }

    function summaryPrice() {
        // only show if userid is admin or owner group.
        if ((in_array('owner', $GLOBALS ['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS ['auth']->getAuthData('groups')) == TRUE)) {

            print ("<div class='summaryBlock'>");
            print ("<h1>Price</h1>");
            // get three latest locations
            $query = "SELECT bovinecurr.id as bovine_id,bovinecurr.full_name, bovinecurr.location_name,bovinecurr.local_number,sale_price.price,sale_price.plus,sale_price.custom_comment,sale_price_comment.comment,sale_price.event_time,sale_price.userid
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.sale_price ON sale_price.id=(SELECT id from bovinemanagement.sale_price WHERE sale_price.bovine_id={$this->bovineID} AND sale_price.event_time=(SELECT max(event_time) FROM bovinemanagement.sale_price WHERE bovine_id={$this->bovineID}))
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id=sale_price_comment.id";
            $res = $GLOBALS['pdo']->query($query);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $row['plus'] = $row['plus'] ?: 'f';
            if ($row['plus'] === 't') {
                $plus = '+';
            } else {
                $plus = '';
            }

            print ("<h2>$ {$row['price']}$plus</h2>");
            print ("Comment: {$row['comment']} {$row['custom_comment']}<br/>");

            print ("</div> <!-- End of summaryBlock Price -->" . "\n\r");
        }
    }

    /**
     * not actually a quickform
     */
    /*
      function animalQueryQuickForm($pageHeader = false) {

      // custom styling when this form is used at the page header, hacky way
      // to do this. should be a css class.
      if ($pageHeader == true) {
      $style = "id='animalSelect'";
      $aniNumber = Misc::createListOfAllAliveCows(true);
      } else {
      $style = '';
      $aniNumber = Misc::createListOfAllAliveCows(false);
      }

      // custom select input
      // with javascript to allow submission on change of select element.
      print ("<select  $style onchange=\"javascript:location.href='?pageid=6&amp;bovine_id='+this.options[this.selectedIndex].value;$('#example &gt; ul').tabs('select', 0);\" name=\"animalNumber\">");
      foreach ($aniNumber as $key => $value) {
      print ("<option value=\"$key\">$value</option>" . "\n\r");
      }
      print ('</select>');


      }
     */

    /**
     * Small form that selects the field.
     */
    public function bovineSelectQuickForm($mode = 'default') {



        if ($mode == 'default') {
            $style = 'bovineSelectPage';
            $aniNumber = Misc::createListOfAllAliveCows(false);
        } else {
            $style = 'bovineSelect';
            $aniNumber = Misc::createListOfAllAliveCows(true);
        }

        $pageid = 6;

        //javascript to allow submission on change of select element.
        $attrs = array('id' => "$style", 'onchange' => "javascript:location.href='?bovine_id='+this.options[this.selectedIndex].value+'&pageid=$pageid';$('#example > ul').tabs('select', 0);");

        //javascript to allow submission when enter is pressed within select form
        //$attrs= array('onKeyPress' => "javascript:if(event.keyCode=='13'){document.frmTest.submit();}");

        $form = new HTML_QuickForm("animalBovineQuickSelect", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        //$form->addElement('header', 'hdrQuickfrmbovineQuickSelect','Select a bovine');
        $form->addElement('select', 'bovineNumber', '', $aniNumber, $attrs);
        $form->addElement('hidden', 'pageid', $pageid);

        //defaults
        if ((isset($this->bovine_id) ) && ($this->bovine_id != null)) {
            $form->setDefaults(array('bovineNumber' => $this->bovine_id)); //set to current bovine
        } else {
            $form->setDefaults(array('bovineNumber' => 0)); //set default to no.  
        }


        // Try to validate a form
        if ($form->validate()) {

            //get value
            $this->bovine_id = $form->exportValue('bovineNumber');
            $this->pageid = $form->exportValue('pageid');
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /** main method with name,etc. at top of page */
    function displayName($bovine_id) {

        // load from bovine table so we can show dead cows, etc.
        $sql = "SELECT bovine.*,dam.full_name as dam_full_name,dam.local_number as dam_local_number, calf_potential_name.potential_name,
            dam.id as dam_id, bovinecurr.location_name, aggregate_view_curr.lpi,aggregate_view_curr.prodoll,aggregate_view_curr.geno_test,
            sire.short_name as sire_short_name, (SELECT dnatest_type FROM bovinemanagement.dnatest_event WHERE bovine_id=$bovine_id ORDER BY event_time DESC limit 1) as dna_pending,
                 batch.prodoll_birthyear_quintile_rank( bovine.id,(EXTRACT(YEAR FROM bovine.birth_date))::integer)
FROM bovinemanagement.bovine
LEFT JOIN bovinemanagement.calf_potential_name ON calf_potential_name.bovine_id = bovine.id
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number=bovine.dam_full_reg_number
LEFT JOIN batch.aggregate_view_curr    ON aggregate_view_curr.full_reg_number = bovine.full_reg_number
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id=bovine.id       
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovine.sire_full_reg_number
WHERE bovine.id=$bovine_id LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        // load classification from batch DB.
        $query = "SELECT class,score,class_all FROM batch.aggregate_view_curr WHERE full_reg_number = '{$this->bovineFullRegNumber}' LIMIT 1";
        $res2 = $GLOBALS ['pdo']->query($query);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);


        //when cow has multiple excellents, show it.
        $class_all = '';
        if ($row2['class_all'] != '') {
            $class_all = '-' . $row2['class_all'];
        }

        //only print out callisfication if they have been classified.
        if ($row2['score'] != '') {
            $classification = "{$row2['class']}-{$row2['score']}$class_all";
        } else {
            $classification = '<b><small>Not Classified</small></b>';
        }


        // print('<div id="wrapper"> <hr />');
        // show reg number
        $holsteinQueryString = HolsteinCanadaHelper::createHolsteinCanadaQuery($this->bovineFullRegNumber); // create
        // string
        // to
        // query
        // holstein
        // Canada
        // from
        // reg
        // number
        // show cow name
        $shortName = Misc::femaleShortName($row ['full_name']);

        if ($shortName == '') {
            $shortName = $row ['potential_name'] . '<span id="potentialName">(potential)</span>';
        }
        //unregisterd means no name
        if (empty($this->bovineFullRegNumber)) {
            $shortName = '';
        }

        // if cow is dead (or not in herd anymore), show name with strikethrough
        $nameStr = "{$row['local_number']}  &nbsp; $shortName";
        if ($row ['death_date'] != null) {
            $nameStr = '<del>' . $nameStr . '</del>';
        }

        $prodoll = empty($row['prodoll']) ? 'Pro$ <small>N/A</small>' : 'Pro$ ' . $row['prodoll'];

        //rank of prodoll

        $prodollRank = empty($row['prodoll_birthyear_quintile_rank']) ? '' : '<small>R' . $row['prodoll_birthyear_quintile_rank'] . '/5</small>';

        if ($row['lpi'] != null) {
            $lpi = 'LPI ' . $row['lpi'];
        }

        $lpi = $prodoll . ' ' . $prodollRank; //overrite lpi with Pro$
        //say a dna test was done, if applicable.
        if (!empty($row['geno_test'])) {
            $lpi = $lpi . '<span id="genoBox"> (' . $row['geno_test'] . ')</span>';
        }
        //or pending dna test, waiting for results.
        elseif (!empty($row['dna_pending'])) {
            $lpi = $lpi . '<span id="genoBox"> (&#8253 ' . substr($row['dna_pending'], 0, 3) . ' &#8253)</span>';
        }


        if ($row['dam_local_number'] != null) {
            $damShortName = Misc::femaleShortName($row ['dam_full_name']);
            $damLink = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['dam_id'],$row['dam_local_number'],$row['dam_full_name']); 
        }





        $owner = '';
        if ($row ['ownerid'] != 'W&C') {
            $owner = 'owner: ' . $row ['ownerid'];
        }

        // pricing data, only show for owner.
        $price = '';
        $query = "SELECT event_time,price,plus,comment FROM bovinemanagement.sale_price_curr WHERE bovine_id=$bovine_id LIMIT 1";
        $res3 = $GLOBALS ['pdo']->query($query);
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);
        $plus = null;
        if ($res3->rowCount() > 0) {
            if ($row3 ['plus'] == 't') {
                $plus = '+';
            }
            if ((in_array('owner', $GLOBALS ['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS ['auth']->getAuthData('groups')) == TRUE)) {
                $price = '$' . $row3 ['price'] . $plus . ' ' . $row3 ['comment'] . ' @ ' . date('M d Y', strtotime($row3 ['event_time']));
            }
        } else {
            $price = 'N/A.';
        }

        $holQuery_sire = HolsteinCanadaHelper::createHolsteinCanadaQuery($row ['sire_full_reg_number']);
        $holQuery_dam = HolsteinCanadaHelper::createHolsteinCanadaQuery($row ['dam_full_reg_number']);


        /* print stuff out */

        /* a div is needed to create a box for the rest */
    

        print ("<div id='animalNameBox'>");
        print ("<div id='animalShortNameNumBox'>#$nameStr</div>");
        if (!empty($this->bovineFullRegNumber)) {
            print ("<div id='animalLongNameNumBox'>{$row['full_name']}</div>");
        } else {
            print ("<div id='animalLongNameNumBox'>UNREGISTERED {$this->bovineRfidNumber}</div>");
        }
        print("<div id='regBox'>" . "\n\r");
        print("<a href=\"$holsteinQueryString\">{$row['full_reg_number']}</a>" . "\n\r");
        print("</div>" . "\n\r");

        print("</div>" . "\n\r");



        print ("<div id='statsBox'>");
        if (!empty($this->bovineFullRegNumber)) {
            /* lpi */
            print("<div id='lpiBox'>" . "\n\r");
            print($lpi);
            print("</div>" . "\n\r");

            /* classification */
            print("<div id='classificationBox'>" . "\n\r");
            $tokClass = explode('-', $classification);
            print( (empty($tokClass[0]) ? '' : $tokClass[0]) . '-' . (empty($tokClass[1]) ? '' : $tokClass[1]) . ' ' . (empty($tokClass[3]) ? '' : $tokClass[3]));
            print("</div>" . "\n\r");
        }
        print("<div id='birthBox'>" . "\n\r");
        print('<img  width="32" height="24" src="/images/birthday-cake-bw.svg" />');
        print(date('M d, Y', strtotime($row['birth_date'])));
        print("</div>" . "\n\r");





        // print();

        /* stats box */
        print("</div>" . "\n\r");



        print ("<div id='statsBox2'>");
        if (!empty($this->bovineFullRegNumber)) {
            print("<div id='cdnBox'>" . "\n\r");
            print('<a href="' . Misc::createListOfAllCowsMilking($row['full_reg_number']) . '"><img   width="50" height="19" src="/images/cdnLogo.svg" /></a>');
            print('&nbsp;&nbsp;&nbsp;&nbsp;');
            print("<a href=\"$holsteinQueryString\">" . '<img  width="28" height="20" src="/images/holsCanadaBWSmall.svg" />' . "</a>" . "\n\r");
            print("</div>" . "\n\r");

            print("<div id='priceBox'>" . "\n\r");
            print('<img  width="20" height="24" src="/images/money_bag_icon.svg" />');
            print('' . $owner . '&nbsp;' . $price . '');
            print("</div>" . "\n\r");
        }
        print("<div id='locationBox'>" . "\n\r");
        print('<img  width="32" height="24" src="/images/small-house.svg" />');

        print($row['location_name'] . '');
        print("</div>" . "\n\r");

        /* END statsBox2 */
        print("</div>" . "\n\r");

        print ("<div id='statsBox3'>");



        print("<div id='sireBox'>" . "\n\r");
        print('<img  width="20" height="20" src="/images/male_black_symbol.svg" />');

        //If not registered yet, display potential sire instead.
        if ($row['sire_full_reg_number'] == '11111111111111111') {
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/holsteinCanadaERAReg.inc');
            try {
                $probArr = HolsteinCanadaERA::listMostLikelyConceptionEvents($row['birth_date'], $row['dam_id']);
            } catch (HolsteinCanadaERA_NoConceptionEventException $exception) {
                return "Error: No conception Event!";
            }

            $str = "";

            foreach ($probArr as $value) {


                $holQuery_sire = HolsteinCanadaHelper::createHolsteinCanadaQuery($value['service_sire_full_reg_number']);
                print ("<small><small> <b>(potential)</b> {$value['days_ago']} <a href='" . Misc::createListOfAllCowsMilking($value['service_sire_full_reg_number']) . "'>{$value['service_sire_short_name']}</a>  &nbsp; &nbsp;<a href=\"$holQuery_sire\"><b>{$value['service_sire_full_reg_number']}</b></a> </small></small> " . '');
            }
        } else {


            print (" <a href='" . Misc::createListOfAllCowsMilking($row['sire_full_reg_number']) . "'>{$row['sire_short_name']}</a>  &nbsp; &nbsp;<a href=\"$holQuery_sire\"><b>{$row['sire_full_reg_number']}</b></a>" . '');
        }
        print("</div>" . "\n\r");

        print("<div id='damBox'>" . "\n\r");
        print('<img  width="15" height="22" src="/images/female_black_symbol.svg" />');
        print(" $damLink &nbsp; &nbsp; <a href=\"$holQuery_dam\"><b>{$row['dam_full_reg_number']}</b></a>" . '');
        print("</div>" . "\n\r");

        /* END statsBox3 */
        print("</div>" . "\n\r");


       
    }

    function displayBreeding($local_number, $row, $row2, $bovine_id) {

        print ('<div id="breed">');

        // print('<div id="location">');
        echo ("Location: <b>{$row['location_name']} </b>");
        // print('</div>');
        // print('<div id="fresh">');
        // convert FRESH date {
        $freshUnix = strtotime($row2 ['fresh_date']);
        $freshDateFormatted = date("M d, Y", $freshUnix);
        $timeUnix = time();
        $DIM = date("z", $timeUnix - $freshUnix);
        echo "<br/>DIM: $DIM <a class=\"small\">from $freshDateFormatted</a> <br/>";


        // ignore due dates in the past.
        echo "Due: ";
        if (($row ['due'] == '0000-00-00') || (strtotime($row ['due']) <= time())) {
            $Due = "None";
        } else {
            $Due = $row ['due'];
        }

        echo "$Due <br/>";
        // print('</div>');

        echo ("<h3>Breeding Info</h3>");
        $sixtyDaysInSeconds = 60 * 24 * 3600;
        $freshUnix = strtotime($row2 ['fresh_date']);
        $freshDateFormatted = date("M d, Y", $freshUnix);

        if (($row ['due'] == '0000-00-00') || (strtotime($row ['due']) <= time())) {
            $breedDate = $sixtyDaysInSeconds + $freshUnix;
            $t = date("M d, Y", $breedDate);
            // echo("{$row2['fresh_date']} Breed Date $t<br/>");
            $timeUnix = time();
            if ($breedDate > $timeUnix) {
                $openFor = date("z", $breedDate - $timeUnix);
                echo ("<a>after </a><a class=\"small\"> $t</a> <br/>");
            } else {
                $openFor = date("z", ($timeUnix - $breedDate));
                echo ("<a>Open for: $openFor d.</a><br/>");
            }
        }
        $breedTo = $row ['breed_to'];
        echo ("<a>To: $breedTo</a><br/>");
        echo "Fresh: $freshDateFormatted <br/>";
        echo "First Choice: {$row['first_ch']} <br/><br/><br/>";

        echo "Service Sire: {$row['servicesir']} <br/>";
        echo "COMMENT: {$row['comment']} <br/>";

        echo "Birth: {$row['birth_d']} <br/>";
        echo "Feet Done: {$row['feet_done']} <br/>";
        echo "TYPE: {$row['type']} <br/>";

        // use postgres function I wrote to calculate.
        $sql = "SELECT bovinemanagement.calculate_breeding_voluntary_waiting_period(id) as breeding_voluntary_waiting_period FROM bovinemanagement.bovinecurr WHERE id={$this->bovineID} LIMIT 1";
        // $res10 = $GLOBALS['pdo']->query($sql);
        // $row10= $res10->fetch(PDO::FETCH_ASSOC);
        $breedAfterDateNew = date("M d Y", strtotime("{$row10['breeding_voluntary_waiting_period']}"));
        echo ("<br/>breed after: $breedAfterDateNew");

        print ('</div> <!--End breed -->');
    }

  

    // full registration number of animal needed.
    function plotLatestLactation($full_reg) {

        $bovineID = $this->bovineID;

        // select latest lactation from alpro    
        $query2 = "	
/* break it down to just one cow and one lactation */		
WITH lact as (
SELECT date,milking_number,milkyield
FROM alpro.cow 
WHERE bovine_id=$bovineID 
AND cow.date >= (SELECT fresh_date::date FROM bovinemanagement.lactationcurr WHERE bovine_id=$bovineID) 
AND ( cow.date <= (SELECT dry_date::date FROM bovinemanagement.lactationcurr WHERE bovine_id=$bovineID) OR  cow.date <= now())
),
/* now we have list of both milking yields for a day and fresh_date*/
daay as (
SELECT DISTINCT ON (lact.date) lact.date,lact.milking_number,lact.milkyield,lact2.milking_number as milking_number2,lact2.milkyield as milkyield2, (SELECT bovinemanagement.round_to_nearest_date(fresh_date) FROM bovinemanagement.lactationcurr WHERE bovine_id=$bovineID) as fresh_date
FROM lact
LEFT JOIN lact as lact2 on lact.date=lact2.date WHERE lact2.milking_number != lact.milking_number
)
/* show dim and total daily milk yield */
SELECT date,abs(fresh_date - date) as dim,fresh_date,milkyield+milkyield2 as dailymilkyield FROM daay
		";

        $res2 = $GLOBALS ['pdo']->query($query2);
        $dailyMilkYieldArr = null;
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $dailyMilkYieldArr[$row2 ['dim']] = $row2 ['dailymilkyield'];
        }

        // select the latest lactation from valacta...
        $query = "
        WITH temp as (
        SELECT test_date,total_milk,fresh,days_in_mi,fat_per,prot_per,ssc,bca_milk,bca_fat,bca_prot,ssc,(SELECT fresh_date::date FROM bovinemanagement.lactationcurr WHERE bovine_id=$bovineID) as fresh_date
        FROM batch.valacta_data WHERE reg='{$this->bovineFullRegNumber}' 
        AND fresh= (SELECT max(fresh) FROM batch.valacta_data WHERE reg='{$this->bovineFullRegNumber}') 
        ) SELECT * FROM temp WHERE
        fresh > (fresh_date::date - interval ' 48 hours' ) AND fresh < (fresh_date::date + interval ' 48 hours' )
        ORDER BY test_date";
        $res = $GLOBALS ['pdo']->query($query);
        $valactaMilkYieldArr = null;
        $valactaFatArr = null;
        $valactaProtArr = null;
        $valactaSSCArr = null;
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            //when a cow is sick, they mark milk_total null.
            if ($row ['total_milk'] != null) {
                $valactaMilkYieldArr [$row ['days_in_mi']] = $row ['total_milk'];
                $valactaFatArr [$row ['days_in_mi']] = $row ['fat_per'];
                $valactaProtArr [$row ['days_in_mi']] = $row ['prot_per'];
                $valactaSSCArr [$row ['days_in_mi']] = $row ['ssc'] / 1000;
            }
        }


        print(self::valactaMilkPlot($dailyMilkYieldArr, $valactaMilkYieldArr));
        print(self::valactaSCCPlot($valactaSSCArr));
        print(self::valactaFatPlot($valactaFatArr, $valactaProtArr));
        print('<br/>');
        self::displayMilkLetDownChart($bovineID);
    }

    /* this is only used for previous lactations, not the current one */
    /* does not show parlor data */

    function valactaMilkPlot($dailyMilkYieldArr = null, $valactaMilkYieldArr = null) {

        //create json from array



        $str = '';

        if (($dailyMilkYieldArr != null) || ($valactaMilkYieldArr != null)) {
            $str = $str . ('<div class="lactationCurve">');
            $str = $str . ("<h3>Lactation Curve</h3>");
            $headerStrArr = array();
            $headerStrArr['xAxis'] = 'number'; //BACKWARDS
            if ($dailyMilkYieldArr != null) {
                $headerStrArr['Parlor Daily Production (l)'] = 'number';
            }
            if ($valactaMilkYieldArr != null) {
                $headerStrArr['Valacta Daily Production (l)'] = 'number';
            }


            if (($dailyMilkYieldArr != null) && ($valactaMilkYieldArr != null)) {
                $opt = "   series: { 1:{ lineWidth: 0, pointSize: 6 } },
                 chartArea: {width: '90%', height: '80%'},
                 legend: {position: 'in'}";
                $x = new GoogleVisLine($opt, $headerStrArr, $dailyMilkYieldArr, $valactaMilkYieldArr);
                $str = $str . ($x->toString());
            } elseif (($dailyMilkYieldArr != null) && ($valactaMilkYieldArr == null)) {
                $opt = "   series: { 1:{ lineWidth: 0, pointSize: 6 } },
                 chartArea: {width: '90%', height: '80%'},
                 legend: {position: 'in'}";
                $x = new GoogleVisLine($opt, $headerStrArr, $dailyMilkYieldArr);
                $str = $str . ($x->toString());
            } elseif (($dailyMilkYieldArr == null) && ($valactaMilkYieldArr != null)) {
                $opt = "   series: { 0:{ lineWidth: 2, pointSize: 6,color: 'red' } },
                 chartArea: {width: '90%', height: '80%'},
                 legend: {position: 'in'}";
                $x = new GoogleVisLine($opt, $headerStrArr, $valactaMilkYieldArr);
                $str = $str . ($x->toString());
            } else {
                $str = $str . ('<p>No milking data yet.</p>');
            }
            $str = $str . ('</div> <!-- end LactationCurve -->');
        }

        return $str;
    }

    function valactaSCCPlot($valactaSSCArr) {
        $str = '';
        if ($valactaSSCArr != null) {
            $str = $str . ('<div class="lactationCurve">');
            $str = $str . ("<h3>SSC Curve</h3>");

            $headerStrArr = array();
            $headerStrArr['xAxis'] = 'number'; //BACKWARDS
            $headerStrArr["SSC (1000\'s)"] = 'number';
            $opt = "                  
                        series: {
            0:{ lineWidth: 2, pointSize: 6 }
                                },
                        chartArea: {width: '90%', height: '80%'},
                        legend: {position: 'in'}";
            $x = new GoogleVisLine($opt, $headerStrArr, $valactaSSCArr);
            $str = $str . ($x->toString());
            $str = $str . ('</div> <!-- end LactationCurve -->');
        }
        return $str;
    }

    function valactaFatPlot($valactaFatArr, $valactaProtArr) {
        $str = '';
        if (($valactaFatArr != null) && ($valactaProtArr != null)) {
            $str = $str . ('<div class="lactationCurve">');
            $str = $str . ("<h3>Fat Curve</h3>");


            $headerStrArr = array();
            $headerStrArr['xAxis'] = 'number'; //BACKWARDS
            $headerStrArr['Fat %'] = 'number';
            $headerStrArr['Protein %'] = 'number';
            $opt = "                  
                        series: {
            0:{ lineWidth: 2, pointSize: 6 },
            1:{ lineWidth: 2, pointSize: 6 }
                                },
                        chartArea: {width: '90%', height: '80%'},
                        legend: {position: 'in'}";
            $x = new GoogleVisLine($opt, $headerStrArr, $valactaFatArr, $valactaProtArr);
            $str = $str . ($x->toString());
            $str = $str . ('</div> <!-- end LactationCurve -->');
        }
        return $str;
    }

    function getLatestMilking($bovine_id) {

$sql = <<<SQL
SELECT DISTINCT on (date) date as "Date"
,round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=1 AND x.date=cow.date ),1) as "AM"
,round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=2 AND x.date=cow.date ),1) as "PM"
		FROM alpro.cow 
                WHERE bovine_id=$bovine_id AND date >= (current_date - interval '7 days') ORDER BY date DESC limit 14
SQL;

   
          $out[]=( (new JQueryDataTable)->startBasicSql($sql));     
          return  (new BootStrap)->plainBox('Latest Parlor Milkings',implode($out));
    }

    function getLatestAdlicTest($full_reg) {
        $query = "SELECT test_date,total_milk,fresh,days_in_mi,fat_per,prot_per,ssc,bca_milk,bca_fat,bca_prot,ssc FROM batch.valacta_data WHERE reg='{$this->bovineFullRegNumber}' AND test_date= (SELECT max(test_date) FROM batch.valacta_data WHERE reg='{$this->bovineFullRegNumber}' AND total_milk !=0) ORDER BY test_date LIMIT 1";
        $res = $GLOBALS ['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $ssc = $row ['ssc'] / 1000;
       
        $out[]="<li>Date {$row['test_date']}</li>";  
        $out[]="<li>BCA {$row['bca_milk']}-{$row['bca_fat']}-{$row['bca_prot']}</li>";  
        $out[]="<li>Milk {$row['total_milk']} kg</li>";  
        $out[]="<li>SSC {$ssc}k</li>";  
        $out[]="<li>DIM @ test {$row['days_in_mi']}</li>";  
        
      return  (new BootStrap)->plainBox('Latest Valacta Test',implode($out));
 
    }

    function showLocationLog($outArray) {

        //show herd cull here at top if necessary
        $query = "select * from bovinemanagement.cull_event where bovine_id={$this->bovineID} limit 1";
        $res2 = $GLOBALS ['pdo']->query($query);
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row2['event_time']) + 1; //add 1 seconds to display at top of list.
            $ftime = date('M j, Y', $time);
            $str = "" . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> <b>{$row2['reason']} {$row2['comment']}</b> by {$row2['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }

        $res = $GLOBALS['pdo']->prepare('SELECT event_time,userid,location.name,date_trunc(\'days\',(current_date-event_time)) as days_ago FROM bovinemanagement.location_event JOIN bovinemanagement.location ON location.id=location_Event.location_id WHERE bovine_id=? ORDER BY event_time DESC');
        $res->execute(array($this->bovineID));
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['event_time']);
            $ftime = date('M j, Y', $time);
            $str = "" . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> <b>{$row['name']}</b> by {$row['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    //shows google chart of latest milk let downs.
    function displayMilkLetDownChart($bovine_id) {


        $sql = "
SELECT cow.date,cow.milking_number,alpro.milkingnumber_to_human_readable(cow.milking_number) as milking_am_pm, employee_shift.userid,7.5 as timeflow0_15,milkflow0_15,22.5 as timeflow15_30,milkflow15_30,45 as timeflow30_60, milkflow30_60,90 as timeflow60_120,milkflow60_120,peakflow,averflow,takeoffflow, EXTRACT(EPOCH FROM duration) as duration, EXTRACT(EPOCH FROM duration)/2 as duration_half
FROM alpro.cow
LEFT JOIN batch.employee_shift ON employee_shift.date=cow.date AND employee_shift.milking_number=cow.milking_number
WHERE bovine_id=$bovine_id AND cow.date > '2014-02-10' AND cow.date <= current_date	
ORDER BY cow.date DESC,cow.milking_number  DESC LIMIT 6           
";



        $res = $GLOBALS ['pdo']->query($sql);
        $counter = 0;
        $spacer = null;
        $spacer2 = null;
        $data = '';
        $header = '';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            //name columns   
            $a = null;
            $a = date('M d, Y', strtotime($row['date'])) . ' ' . $row['milking_am_pm'] . ' ' . $row['userid'];
            $header = $header . "data.addColumn('number', '$a');" . "\n\r";


            //create data array     
            if ($counter == 0) {
                $spacer = null;
                $spacer2 = ',null,null,null,null,null';
            }
            if ($counter == 1) {
                $spacer = ',null';
                $spacer2 = ',null,null,null,null';
            }
            if ($counter == 2) {
                $spacer = ',null,null';
                $spacer2 = ',null,null,null';
            }
            if ($counter == 3) {
                $spacer = ',null,null,null';
                $spacer2 = ',null,null';
            }
            if ($counter == 4) {
                $spacer = ',null,null,null,null';
                $spacer2 = ',null';
            }
            if ($counter == 5) {
                $spacer = ',null,null,null,null,null';
                $spacer2 = null;
            }

            $data = $data . ('[ ' . '0' . $spacer . ',' . '0' . $spacer2 . ' ],');
            $data = $data . ('[ ' . $row['timeflow0_15'] . $spacer . ',' . $row['milkflow0_15'] . $spacer2 . ' ],');
            $data = $data . ('[ ' . $row['timeflow15_30'] . $spacer . ',' . $row['milkflow15_30'] . $spacer2 . ' ],');
            $data = $data . ('[ ' . $row['timeflow30_60'] . $spacer . ',' . $row['milkflow30_60'] . $spacer2 . ' ],');
            $data = $data . ('[ ' . $row['timeflow60_120'] . $spacer . ',' . $row['milkflow60_120'] . $spacer2 . ' ],');
            //$data=$data.('[ '.  $row['duration_half']. $spacer .','.$row['averflow'] .$spacer2.' ],');
            // $data=$data.('[ '.  $row['duration']. $spacer .','.$row['takeoffflow'] .$spacer2.' ],');      
            $counter++;
        }




        if ($data != '') {
            ?>
            <div id="chart_div_letdown" ></div>


            <script type="text/javascript">

                setTimeout(drawChart_letdown, 200); //firefox workaround.   
                google.charts.setOnLoadCallback(drawChart_letdown, true);
                $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart_letdown);
            });
                function drawChart_letdown() {


                    var data = new google.visualization.DataTable();
                    data.addColumn('number', 'liters/min');
            <?php echo($header); ?>
                    data.addRows([
            <?php echo($data); ?>
                    ]);


                    var options = {
                        title: 'Milk Let-Down',
                        interpolateNulls: true,
                        hAxis: {title: 'Milking Time (s)', titleTextStyle: {color: '#000'}},
                        vAxis: {title: 'Milk (l/min)', titleTextStyle: {color: '#000'}},
                        legend: {textStyle: {color: '#000', fontSize: 10}}

                    };


                    var chart = new google.visualization.LineChart(document.getElementById('chart_div_letdown'));
                    chart.draw(data, options);
                }
            </script>


            <?php
        }
    }

}

//end class

/** shows reproduction quickforms * */
class AccordionImplementationReproduction extends AccordionSub {

    public static $bovineID;
    public static $mode;

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct($bovineID = null) {
        if ($bovineID != null) {
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
        } else {
            self::$bovineID = null;
            self::$mode = 'group';
        }
    }

    function tab1() {

        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusHeats.inc');
        $param= new stdClass();
        $param->bovine_id= self::$bovineID;
        $a=new QF2_EstrusHeats((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'],'bovine_id'=>$_REQUEST['bovine_id'])),'individual',$param);
        print($a->renderBox('qf_heatsEntry','Record Heat Event'));       
        

    }

    function tab2() {

        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusHormone.inc');
        (new EstrusHormone())->hormoneQuickForm(self::$mode, self::$bovineID);
    }

    function tab3() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusBreedings.inc');
       $param= new stdClass();
       $param->bovine_id= self::$bovineID;
        $a=new QF2_EstrusBreedingsMarkCowToBeBred((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'],'bovine_id'=>$_REQUEST['bovine_id'])),'individual',$param);
        print($a->renderBox('qf2_BreedingsCowToBeBred','Animal to be Bred'));     
        
    }

    function tab4() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/customComment.inc');
        (new CustomComment())->newCustomCommentQuickForm(self::$mode, self::$bovineID, 'reproductive');
    }

    function tab5() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusKamar.inc');
      $param= new stdClass();
        $param->bovine_id= self::$bovineID;
         $k = new QF2_EstrusKamar((object) array(forwardURLArr => array('pageid' => $_REQUEST['pageid'],'bovine_id'=>$_REQUEST['bovine_id'])), 'individual',$param);
        print($k->renderBox('qf2_kamarEntry', 'Record Kamar Event'));

    }

    function tab6() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusPregnancyCheck.inc');
        (new EstrusPregnancyCheck())->addPreganacyCheckEvenQuickForm(self::$mode, self::$bovineID);
    }

}

//end class

/** shows reproduction quickforms * */
class AccordionImplementationMovement extends AccordionSub {

    public static $bovineID;
    public static $mode;

    /* in case of individual cow items, we use bovine_id. */

    public function __construct($bovineID = null) {

        if ($bovineID != null) {
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
        } else {
            self::$bovineID = null;
            self::$mode = 'group';
        }
    }

    function tab1() {
        $param = new stdClass();
        $param->bovine_id=self::$bovineID;
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        $a=new QF2_MoveAnimal((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'],'bovine_id'=>$_REQUEST['bovine_id'])),self::$mode,$param);
        print($a->renderBox('qf2_MoveAnimal','Move Animal to new Group'));  
        print($a->formJS());
        
    }

    function tab2() {
         require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementSortGate.inc');
         
        $param = new stdClass();
        $param->bovine_id=self::$bovineID;
        $a=new QF2_SortGate((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'],'bovine_id'=>$_REQUEST['bovine_id'])),self::$mode,$param);
        print($a->renderBox('QF2_SortGate','Mark Animal to be Sorted'));  
        print($a->formJS());
    }
    
    function tab3() {

        // require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        print("<h2>TODO:Put potential cull here</h2>");
    }

    function tab4() {

        // require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        print("<h2>TODO:Put Feet here</h2>");
    }

    function tab5() {

        // require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        print("<h2>TODO:Put Ear Tags</h2>");
    }

}

//end class

/**
 * used to display tabs for previous lactations. 
 */
class AccordionImplementationPreviousLactations extends AccordionSub {

    public static $bovineFullRegNumber;
    
    public function __construct($bovineFullRegNumber = null) {
        if ($bovineFullRegNumber != null) {
            self::$bovineFullRegNumber = $bovineFullRegNumber;
        } else {
            throw new Exception("Must Have bovine reg number to run query.");
        }
    }

    private function sqlData($lact_num) {
        $bovineFullRegNumber = self::$bovineFullRegNumber;

        $str = '';

    
        
$sql = <<<SQL
SELECT days_in_mi as "DIM", bca_milk ||'-'|| bca_fat ||'-'||  bca_prot as "BCA", total_milk as "Milk (kg)",fat_per as "Fat %",prot_per as "Prot %",round(ssc/1000,1) as "SSC",round(mun/10,1) as "MUN" , 
    round(total_milk*fat_per*.01,2) as "Fat (kg/day)",round(total_milk*prot_per*.01,2) as "Prot (kg/day)" 
    FROM batch.valacta_data 
        WHERE reg='$bovineFullRegNumber' AND lact_nu=$lact_num 
            ORDER BY test_date DESC
SQL;



        
        // $out[]=( (new JQueryDataTable)->startBasicSql($sql));
        $str = $str .( (new JQueryDataTable)->startBasicSql($sql));
        
        $valactaMilkYieldArr = null;
        $valactaFatArr = null;
        $valactaProtArr = null;
        $valactaSSCArr = null;

        $query = "SELECT lact_nu,test_date,total_milk,fresh,days_in_mi,fat_per,prot_per,ssc,bca_milk,bca_fat,bca_prot,ssc/1000 as ssc,days_in_mi,fat_per,prot_per,mun/10 as mun FROM batch.valacta_data WHERE reg='$bovineFullRegNumber' AND lact_nu=$lact_num ORDER BY test_date DESC";
        $res = $GLOBALS ['pdo']->query($query);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            //used to plot graphs later...
            if ($row ['total_milk'] != null) {
                $valactaMilkYieldArr [$row ['days_in_mi']] = $row ['total_milk'];
                $valactaFatArr [$row ['days_in_mi']] = $row ['fat_per'];
                $valactaProtArr [$row ['days_in_mi']] = $row ['prot_per'];
                $valactaSSCArr [$row ['days_in_mi']] = $row ['ssc'] / 1000;
            }
        }
        $dailyMilkYieldArr = null; //do not sow daily data for previos lactations. No need. 
        $str = $str . (BovineQuery::valactaMilkPlot($dailyMilkYieldArr, $valactaMilkYieldArr));
        $str = $str . (BovineQuery::valactaSCCPlot($valactaSSCArr));
        $str = $str . (BovineQuery::valactaFatPlot($valactaFatArr, $valactaProtArr));


        return $str;
    }

    function printBCA($params) {
        extract($params);
        return "{$record['bca_milk']}-{$record['bca_fat']}-{$record['bca_prot']}";
    }

    //NOTE: only supports 14 lactations.
    function tab1() {
        print(self::sqlData(1));
    }

    function tab2() {
        print(self::sqlData(2));
    }

    function tab3() {
        print(self::sqlData(3));
    }

    function tab4() {
        print(self::sqlData(4));
    }

    function tab5() {
        print(self::sqlData(5));
    }

    function tab6() {
        print(self::sqlData(6));
    }

    function tab7() {
        print(self::sqlData(7));
    }

    function tab8() {
        print(self::sqlData(8));
    }

    function tab9() {
        print(self::sqlData(9));
    }

    function tab10() {
        print(self::sqlData(10));
    }

    function tab11() {
        print(self::sqlData(11));
    }

    function tab12() {
        print(self::sqlData(12));
    }

    function tab13() {
        print(self::sqlData(13));
    }

    function tab14() {
        print(self::sqlData(14));
    }

}

//end class
?>