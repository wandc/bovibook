<?php


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
    private $bovineBirthDate;
    private $replacement; 

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
               
                $tabArray[1]['name'] = 'Movement';
                $tabArray[1]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[2]['name'] = 'Medical';
                $tabArray[2]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[3]['name'] = 'Reproduction';
                $tabArray[3]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[4]['name'] = ($this->bovineBirthDate >= strtotime('-9 months')) ?'Calf Feeding' :'Production'; //show different tab for calves instead of production.
                $tabArray[4]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[5]['name'] = 'Genetics';
                $tabArray[5]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                //owner only tabs below
               if ($GLOBALS['auth']->getOwnerAccess() == 1) {
                $tabArray[6]['name'] = 'Breeding Choice';
                $tabArray[6]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[7]['name'] = 'Sale Price';
                $tabArray[7]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                       }
            } else {
                //un registered animal   
                //setup for tabs.
                $tabArray[1]['name'] = 'Movement';
                $tabArray[1]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[2]['name'] = 'Medical';
                $tabArray[2]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[3]['name'] = 'Reproduction';
                $tabArray[3]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
                $tabArray[4]['name'] = ($this->bovineBirthDate >= strtotime('-9 months')) ?'Calf Feeding' :'Production'; //show different tab for calves instead of production.
                $tabArray[4]['extra_callback_param'] = "&amp;bovine_id={$this->bovineID}";
            }


            return $tabArray;
        } else {
            throw new Exception("ERROR: bovine_id can't be determined for tabs.<br/>");
        }
    }

    //true is a heifer
    //false is older
    //pretty ruff determination of whether a replacement or not. 
    private function replacementAnimal($birth_date) {
        //find out if heifer age.
            $date1 = new DateTime("now");
            $date2 = new DateTime($birth_date);
            return (($date1->diff($date2))->format('%a') < 430) ? true : false;
    }
    
    private function loadVars($bovine_id = null, $pageid, $local_number = null) {

        if ($bovine_id != -1) {
            $this->bovineID = $bovine_id;
            $this->pageID = $pageid;


            // lookup bovine_id
            $sql = "SELECT full_reg_number,local_number,full_name,rfid_number,birth_date FROM bovinemanagement.bovine WHERE id=$this->bovineID LIMIT 1;";
            $res = $GLOBALS ['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            // fill class variables.
            $this->bovineFullRegNumber = $row ['full_reg_number'];
            $this->bovineLocalNumber = $row ['local_number'];
            $this->bovineRfidNumber = $row ['rfid_number'];
            $this->bovineBirthDate = strtotime($row ['birth_date']);
            $this->bovineShortName = $GLOBALS['MiscObj']->femaleShortName($row['full_name']);      
            $this->replacement = $this->replacementAnimal($row['birth_date']); 
            
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


            $sql = "SELECT full_reg_number,id,rfid_number,birth_date FROM bovinemanagement.bovine WHERE local_number=$this->bovineLocalNumber LIMIT 1;";
            $res = $GLOBALS ['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            // fill class variables.
            $this->bovineRfidNumber = $row ['rfid_number'];
            $this->bovineFullRegNumber = $row ['full_reg_number'];
            $this->bovineID = $row ['id'];
            $this->bovineBirthDate = strtotime($row ['birth_date']);
            $this->replacement = $this->replacementAnimal($row['birth_date']); 
        }
    }

    function AnimalChooseContainer() {

        print ("<h2>Choose Animal From Header Above.</h2>");
    }

    /**
     * Called before render tabs to add the top of page stuff before the tabs (name, slect form, etc.)
     */
    function renderTabsParent($tabArray) {
        
        if ($_REQUEST ['bovine_id'] != null) {
           $headerCls= new BovineQueryHeader($this->bovineID,$this->bovineLocalNumber,$this->bovineShortName,$this->bovineFullRegNumber,$this->bovineRfidNumber);
           print($headerCls->main());
        }
      
        $this->renderTabs($tabArray);
    }


    //MOVEMENT TAB
    function tab1() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        $accord=(new bovineQueryMovementAccordian)->showAccordion($this->bovineID);
        $sortGate=(new BootStrap)->bootstrapSmallBox('Sort Gate', MovementSortGate::individualCowSortStatus($this->bovineID), null, null, 'ion ion-ios-arrow-thin-right');
        //
        $cowTracking=$this->cowTracking();
        $cowTrackingBox=  empty($cowTracking) ?  (new BootStrap)->generalInfoBox('fa-location-arrow','bg-info','Animal Location',$cowTracking) : '';      
        //
        $locationLog=(new BootStrap)->plainCard('Location Log',$this->locationAll());
        $rfidParlor=(new BootStrap)->plainCard('RFID Stats Parlor',$this->showLatestRFIDStatsParlor($this->bovineID));
        $rfidSortGate=(new BootStrap)->plainCard('RFID Stats Sort Gate',$this->showLatestRFIDStatsSortGate($this->bovineID));
   
        
        $left = <<<HTML
            {$locationLog}         
        HTML; 
        
        $centre = <<<HTML
            {$sortGate}
            {$rfidParlor}
            {$rfidSortGate}
            {$cowTrackingBox}        
        HTML;     
            
        $right = <<<HTML
            {$accord}   
           
        HTML; 
        
        $content=(new Bootstrap)->thirdThirdThird($left,$centre,$right);                        
        print($content);
        
    }

    
    
    
    //medical
    function tab2() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        
        //find out if replacement heifer or normal cow
        $extra=($this->replacement == true) ? 'replacement_animal' : null;
        $accordian=(new bovineQueryMedicalAccordian)->showAccordion($this->bovineID,$extra);
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryMedicalLog.inc');
        $bovineQueryMedicalLog = new BovineQueryMedicalLog($this->bovineID, $this->bovineFullRegNumber);
        $diseaseStatus='';
        $scheduled=(new MedicineScheduled)->displayNext8HoursScheduledMedicines($this->bovineID);
        $auxiliary=(new BootStrap)->plainCard('Auxiliary', implode($bovineQueryMedicalLog->auxiliary()));
        $medLogStr=(new BootStrap)->plainCard('Medical Log', $bovineQueryMedicalLog->main());
        
        $left = <<<HTML
           
            {$medLogStr}
        HTML; 
            
          $centre = <<<HTML
                  {$scheduled}
                  {$auxiliary}
                  {$diseaseStatus}
        HTML;     
            
        $right = <<<HTML
            {$accordian}
        HTML; 
                
        $content=(new Bootstrap)->thirdThirdThird($left,$centre,$right);                
        print($content);
        
    }

    //reproduction
    function tab3() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        $accordian=(new bovineQueryReproductionAccordian)->showAccordion($this->bovineID);
        $bovineQueryReproductionLog = (new BovineQueryReproductionLog($this->bovineID, $this->bovineFullRegNumber))->main();
        $reproLogStr=(new BootStrap)->plainCard('Repro Log','<table>'. $bovineQueryReproductionLog. '</table>');
        $kamar=(new EstrusKamar())->displaySingleCowKamarStatusBox($this->bovineID);
        //
        $objEstrusDetector = new EstrusDetector();  //estrus detetctor charts
        $estrusDetector1= (new BootStrap)->plainCardCollapsed('EstrusDetector 1', $objEstrusDetector->oneCowLatestDataChartFilteredVar($this->bovineID));
        $estrusDetector2= (new BootStrap)->plainCardCollapsed('EstrusDetector 2', $objEstrusDetector->oneCowLatestDataChart($this->bovineID));
        //
        $boxOut[] = '<li>'.$this->printPreSelectedSireChoices().'</li>'; //sire choice
        $boxOut[] = '<li>'.("<b>VWP:</b> " . $this->breedingVoluntaryWaitingPeriod($this->bovineID)).'</li>'; // breeding VWP
        $boxOut[] = '<li>'.$this->printDueDate().'</li>';  // due
        $infoCard=(new BootStrap)->plainCard('Info', implode($boxOut));
        //
        
        
        
        $left = <<<HTML
           
            {$reproLogStr}
        HTML; 
            
          $centre = <<<HTML
                  {$infoCard}
                  {$kamar}
                  {$estrusDetector1}
                  {$estrusDetector2}
        HTML;     
            
        $right = <<<HTML
            {$accordian}
        HTML; 
                
        $content=(new Bootstrap)->thirdSmallThird($left,$centre,$right);                
        print($content);
        
    }

    //Calf Feeding OR Production
    function tab4() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        
        if ($this->bovineBirthDate >= strtotime('-9 months')) {
            $out[]=$this->tab4_CalfFeeding();
        }
        else {
            $out[]=$this->tab4_Production();
        }
        print(implode($out));
    }
    
    //tab production
    function tab4_Production() {
        

        //Tab Content

        //check she has a lactation, or don't show anything.
        $sql = "SELECT id FROM bovinemanagement.lactation WHERE bovine_id=({$this->bovineID})";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() > 0) {
            
              $out[]=(new BootStrap)->halfHalf($this->productionAll(),(new bovineQueryPrevLactationsAccordian)->showAccordian($this->bovineFullRegNumber));
                
        } else {
              $out[]=('<h2>No Lactations.</h2>');
        }
        
        return (implode($out));
    }

     //tab production
    function tab4_CalfFeeding() {
       
        //Tab Content
        $calfFeedingClass=new CalfFeeding();
        $tiere_id=$calfFeedingClass->convertLocalNumberToUrbanTeireID($this->bovineLocalNumber);
        $cls = new calfFeedingModal($calfFeedingClass->pdoUrban, $tiere_id, $this->bovineID, $this->bovineLocalNumber);     
        return $cls->toStringContent();     
       
    }
    
    
    /**
     * Genetics from Holstein Canada
     */
    function tab5() {

        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        $regNumber = substr($this->bovineFullRegNumber, 6, strlen($this->bovineFullRegNumber));
        $haplotype = BovineQueryBreedingChoice::showHaplotypeInfo($this->bovineID);

        $left = <<<HTML
             {$this->geneticIndexChangeOverTime($regNumber)}
        HTML; 
        $centre = <<<HTML
               {$haplotype}
                {$this->showFamilyTree()}
        HTML; 
        $right = <<<HTML
            {$this->typeClassificationChart($regNumber)}
        HTML; 
                
        $content=(new Bootstrap)->thirdThirdThird($left,$centre,$right);             
        
        print( $content .  $this->draw4GenFamilyTree());
       
    }

    //breeding choice
    function tab6() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryBreedingChoice.inc');
        $bovineQueryBreedingChoice = new BovineQueryBreedingChoice($this->bovineID, $this->bovineLocalNumber, $this->bovineShortName);
        print($bovineQueryBreedingChoice->main());
    }

    //sale pricing
    function tab7() {
        $this->loadVars($_REQUEST ['bovine_id'], $_REQUEST ['pageid'], $_REQUEST ['bovine_local_number']);
        
          //draw holstein canada iframe.
          include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/holsteinCanadaPageParser.php');
          $holCan=new HolsteinCanadaPageParser(850,900); //reg,iframe,width,height.
          //
          $salePrice= new SalePrice();

          
        $left = <<<HTML
           {$holCan->generateIframe($this->bovineFullRegNumber)}
        HTML; 
        
            
        $right = <<<HTML
           {$salePrice->salePriceCowQuickForm($this->bovineID)}
          {$salePrice->displayPreviousPricesForChosenCow($this->bovineID)}
        HTML; 
                
        $content=(new Bootstrap)->halfHalf($left,$right);                
        print($content);          
    }
    
    
    public function cowTracking() {
        
        try {
            require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusDetector.inc');
            $obj = new Trilateration();
            $content=$obj->oneCow($this->bovineID);
        } catch (\Exception $e) {
            // var_dump($e->getMessage());
            //show nothing.
        }
        return $content;
    }
    
    
    function printPreSelectedSireChoices() {


        require_once ($_SERVER ['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/bovineQueryBreedingChoice.inc');
        $bovineQueryBreedingChoice = new BovineQueryBreedingChoice($this->bovineID, $this->bovineLocalNumber, $this->bovineShortName);
        $retArr = $bovineQueryBreedingChoice->nextBreedingAction($this->bovineID);


        $row = $retArr['row'];

        //change blanks to question marks
        if (isset($row['p1_name'])) {
            $row['p1_name'] = self::changeNulltoQueestionMark($row['p1_name']);
            if (($row['p1_name'] != null) AND ( $row['p1_avail'] == null)) {
                $row['p1_name'] = '<s>' . $row['p1_name'] . '</s>';
            }
        }
        if (isset($row['p2_name'])) {
            $row['p2_name'] = self::changeNulltoQueestionMark($row['p2_name']);
            if (($row['p2_name'] != null) AND ( $row['p2_avail'] == null)) {
                $row['p2_name'] = '<s>' . $row['p2_name'] . '</s>';
            }
        }
        if (isset($row['p3_name'])) {
            $row['p3_name'] = self::changeNulltoQueestionMark($row['p3_name']);
            if (($row['p3_name'] != null) AND ( $row['p3_avail'] == null)) {
                $row['p3_name'] = '<s>' . $row['p3_name'] . '</s>';
            }
        }
        if (isset($row['s1_name'])) {
            $row['s1_name'] = self::changeNulltoQueestionMark($row['s1_name']);
            if (($row['s1_name'] != null) AND ( $row['s1_avail'] == null)) {
                $row['s1_name'] = '<s>' . $row['s1_name'] . '</s>';
            }
        }
        if (isset($row['s2_name'])) {
            $row['s2_name'] = self::changeNulltoQueestionMark($row['s2_name']);
            if (($row['s2_name'] != null) AND ( $row['s2_avail'] == null)) {
                $row['s2_name'] = '<s>' . $row['s2_name'] . '</s>';
            }
        }
        if (isset($row['s3_name'])) {
            $row['s3_name'] = self::changeNulltoQueestionMark($row['s3_name']);
            if (($row['s3_name'] != null) AND ( $row['s3_avail'] == null)) {
                $row['s3_name'] = '<s>' . $row['s3_name'] . '</s>';
            }
        }


        $out[] = ("Next Repro Action: <span class='large1_1'>{$retArr['nextActionTxt']}</span><br/>");
        if (!empty($row)) {
        $out[] = ("Service Sire Choices: 1<sup>st</sup>:") . str_replace(',,', '', "{$row['p1_name']},{$row['p2_name']},{$row['p3_name']}, Later: {$row['s1_name']},{$row['s2_name']},{$row['s3_name']}<br/>");
        }
        return implode($out);
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
        $sql = "SELECT preg_check_method,calculated_potential_due_date,service_sire_short_name,service_sire_full_reg_number,estimate_twins FROM bovinemanagement.pregnant_view WHERE id={$this->bovineID} LIMIT 1";

        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row)) {
        if ($row['estimate_twins']==true) {
            $twins=' <b>TWINS</b>';
        }
        else { $twins='';}
        
        if ($row != null) {
            return ("<b>DUE:</b>:" . $this->daysTillandDate($row ['calculated_potential_due_date']) . $twins . " by {$row['preg_check_method']} to <b>{$row['service_sire_short_name']}</b> <span id='small'>({$row['service_sire_full_reg_number']})</span><br/>");
        } else {
            return false;
        }
    }
    else {
        return false;
    }
    
    }
     /*
     * work around for breeding choice not being a real page for reference with ajax call to page 141 in display jquerydatatable.
     */

    function progenyBreedingChoiceForBovine($request) {
        return (new BovineQueryBreedingChoice)->progenyBreedingChoiceForBovine($request);
    }

    function breedingChoicesHistorical($request) {
        return (new BovineQueryBreedingChoice)->breedingChoicesHistorical($request);
    }

    
    
    function diseaseStatus() {
        
         $boxOut=array();
        //FIXME, not temporal. and should be for each disease.
         $sql = "SELECT disease,status FROM bovinemanagement.medical_disease WHERE bovine_id={$this->bovineID}";
        $res = $GLOBALS ['pdo']->query($sql);
        while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            
            if($row['status']==1) {
                $status='True';
            } elseif ($row['status']==0) {
               $status='False';  
            }else {
                $status='Unknown';
            }
            
          $boxOut[]=$row['disease'] .': '.$status;
        }
        return $boxOut;
    }
    
   
   

    private function showLatestRFIDStatsParlor($bovineID) {
     

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

                 $out[]= "<li><b>RFID</b>: " . $GLOBALS['MiscObj']->daysOrHoursAgo($idTimeRaw) . " <b class=\"smallish\">($ftime)</b> $idTime Auto ID <b>$idStatus</b></li>";
               
        }}
            else {
                $out[]='<li>No Recent RFID Reads.</li>';
            }
            
            
        return implode($out);
    }

    private function showLatestRFIDStatsSortGate($bovineID) {
     

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

                  $out[] = "<li><b>RFID</b>: " . $GLOBALS['MiscObj']->daysOrHoursAgo($idTimeRaw) . " <b class=\"smallish\">($ftime)</b> $idTime Auto ID <b>$idStatus</b></li>";
              
        }}
        else {
           $out[] =("<li>No Recent RFID Reads.</li>");   
        }

        return implode($out);
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

    function typeClassificationChart($regNumber) {
        $outStr=null;
        /**
         * plot holstein canada classification as a bar graph
         */
        $query = "SELECT * FROM batch.holstein_canada_data_curr WHERE reg_no='$regNumber' LIMIT 1";
        $res = $GLOBALS ['pdo']->query($query);

        $row = $res->fetch(PDO::FETCH_OBJ); //get as an object.
         if (!empty($row)) {
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
        $x = new GoogleVisualizationBar($opt, $headerStrArr, $three);
        $outStr=$x->toString();
    }
        return (new BootStrap)->plainCard('Confirmation Chart',$outStr);
    }

    function geneticIndexChangeOverTime($regNumber) {
        $sql = <<<SQL
SELECT extract_date as "Date" ,pro_doll as "Pro$",lpi as "LPI",gebv as "GEBV"
    FROM batch.holstein_canada_data 
        WHERE reg_no='$regNumber' 
            ORDER BY extract_date DESC
SQL;
        $out[] = ( (new JQueryDataTable)->startBasicSql('Genetic Index Change Over Time', $sql));
        return  implode($out);
    }

    function showFamilyTree() {

        $out[] = FamilyTree::drawFemaleFamilyTree($this->bovineFullRegNumber);
        return implode($out);
    }

    /*
     * call recursively to get family tree
     */

    function getAgregateData($full_reg_number) {
        
        if (!empty($full_reg_number)) {
        $sql = "select full_reg_number,full_name,birth_date,prodoll,dam_full_reg_number,sire_full_reg_number from batch.aggregate_view_all where full_reg_number='$full_reg_number' limit 1";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        
        return !empty($results) ? $results[0] : null;
        }
        else {
            return null;
        }
    }

    //shortcut function to make family tree
    private  function familyTreeLine($animalArr) {
            if (!empty($animalArr)) {
                 $str=<<<STR
                '<a href="{$GLOBALS['MiscObj']->createCDNLink($animalArr['full_reg_number'])}">{$animalArr['full_name']}</a><br/><font color="red"><i>{$animalArr['birth_date']}<i></font>'
        STR;
            } else {
               $str=<<<STR
                '<a href="#"> </a><br/><font color="red"><i> <i></font>'
        STR;
            }
            return $str;
        }
       
    
    function draw4GenFamilyTree() {

        $animal =  !empty($this->bovineFullRegNumber) ? $this->getAgregateData($this->bovineFullRegNumber) : '';
        $animal_dam = !empty($animal['dam_full_reg_number']) ?  $this->getAgregateData($animal['dam_full_reg_number']) : '';
        $animal_sire = !empty($animal['sire_full_reg_number']) ?  $this->getAgregateData($animal['sire_full_reg_number']) : '';
        //
        $animal_dam_dam =  !empty($animal_dam ['dam_full_reg_number']) ? $this->getAgregateData($animal_dam ['dam_full_reg_number']) : '';
        $animal_dam_sire = !empty($animal_dam ['sire_full_reg_number']) ?  $this->getAgregateData($animal_dam ['sire_full_reg_number']) : '';
        $animal_sire_dam = !empty($animal_sire ['dam_full_reg_number']) ?  $this->getAgregateData($animal_sire ['dam_full_reg_number']) : '';
        $animal_sire_sire =  !empty($animal_sire ['sire_full_reg_number']) ? $this->getAgregateData($animal_sire ['sire_full_reg_number']) : '';
        //
        $animal_dam_dam_dam =  !empty($animal_dam_dam ['dam_full_reg_number']) ? $this->getAgregateData($animal_dam_dam ['dam_full_reg_number']) : '';
        $animal_dam_sire_dam = '';
        $animal_sire_dam_dam = ''; //don't show dam's of sires to speed up code.
        $animal_sire_sire_dam = '';
        $animal_dam_dam_sire =   !empty($animal_dam_dam ['sire_full_reg_number'])  ? $this->getAgregateData($animal_dam_dam ['sire_full_reg_number']) : '';
        $animal_dam_sire_sire =  !empty($animal_dam_sire ['sire_full_reg_number']) ? $this->getAgregateData($animal_dam_sire ['sire_full_reg_number']) : '';
        $animal_sire_dam_sire =  !empty($animal_sire_dam ['sire_full_reg_number']) ? $this->getAgregateData($animal_sire_dam ['sire_full_reg_number']) : '';
        $animal_sire_sire_sire = !empty($animal_sire_sire['sire_full_reg_number']) ? $this->getAgregateData($animal_sire_sire['sire_full_reg_number']) : '';

        //FOR DEBUG
        /*
          print ($animal['bovine_full_name'] . ' - ' . $animal ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_dam ['bovine_full_name'] . ' - ' . $animal_dam ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_sire ['bovine_full_name'] . ' - ' . $animal_sire ['bovine_birthdate'] . '<br/>' . "\n\r");

          print ($animal_dam_dam ['bovine_full_name'] . ' - ' . $animal_dam_dam ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_dam_sire ['bovine_full_name'] . ' - ' . $animal_dam_sire ['bovine_birthdate'] . '<br/>' . "\n\r");

          print ($animal_sire_dam ['bovine_full_name'] . ' - ' . $animal_sire_dam ['bovine_birthdate'] . '<br/>' . "\n\r");
          print ($animal_sire_sire ['bovine_full_name'] . ' - ' . $animal_sire_sire ['bovine_birthdate'] . '<br/>' . "\n\r");

         */

        
        
       
                

        //Now use google visulations to show the data in an org chart
        $js = <<<JS

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
                data.setCell(0, 0, '0', {$this->familyTreeLine($animal)});
                //row 1
                data.setCell(1, 0, '1f', {$this->familyTreeLine($animal_dam)}); 
                data.setCell(1, 1, '0');
                //row 2
                data.setCell(2, 0, '1m', {$this->familyTreeLine($animal_sire)});
                data.setCell(2, 1, '0');
                //row 3
                data.setCell(3, 0, '2ff', {$this->familyTreeLine($animal_dam_dam)});
                data.setCell(3, 1, '1f');
                //row 4 
                data.setCell(4, 0, '2fm', {$this->familyTreeLine($animal_dam_sire)});
                data.setCell(4, 1, '1f');
                //row 5 
                data.setCell(5, 0, '2mm', {$this->familyTreeLine($animal_sire_sire)});
                data.setCell(5, 1, '1m');
                //row 6 
                data.setCell(6, 0, '2mf', {$this->familyTreeLine($animal_sire_dam)});
                data.setCell(6, 1, '1m');
                /////////////////////////
                //   //row 7
                data.setCell(7, 0, '3fff', {$this->familyTreeLine($animal_dam_dam_dam)});
                data.setCell(7, 1, '2ff');
                //row 8 
                data.setCell(8, 0, '3ffm', {$this->familyTreeLine($animal_dam_dam_sire)});
                data.setCell(8, 1, '2ff');
                //row 9 
                data.setCell(9, 0, '3fmf', {$this->familyTreeLine($animal_dam_sire_dam)});
                data.setCell(9, 1, '2fm');
                //row 10 
                data.setCell(10, 0, '3fmm', {$this->familyTreeLine($animal_dam_sire_sire)});
                //   //row 11
                data.setCell(11, 0, '3mmf', {$this->familyTreeLine($animal_sire_sire_dam)});
                data.setCell(11, 1, '2mm');
                //row 12 
                data.setCell(12, 0, '3mmm', {$this->familyTreeLine($animal_sire_sire_sire)});
                data.setCell(12, 1, '2mm');
                //row 13
                data.setCell(13, 0, '3mff', {$this->familyTreeLine($animal_sire_dam_dam)});
                data.setCell(13, 1, '2mf');
                //row 14 
                data.setCell(14, 0, '3mfm', {$this->familyTreeLine($animal_sire_dam_sire)});
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
         <div id="visualizationF" style="width: 100%; height: 600px;"></div>
             
JS;

        return (new BootStrap)->plainCard('Family Tree', $js);
    }

    
    private function productionFinancialStatsPlusMisc() {
         $ownerStats = '';
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $out[] = ($this->getLatestLactationRevenue($this->bovineID));
            $out[] = ($this->getPercentageManuallyMilked($this->bovineID));
            $out[] = ($this->getPercentageReattatched($this->bovineID));
            $out[] = ($this->findMastitisDoses($this->bovineID));
            $out[] = ($this->getAdjustedProjectedRevenue($this->bovineLocalNumber));
            $ownerStats = (new BootStrap)->plainCard('Milking Stats', implode($out));
        }
        return $ownerStats;
    }
    
    private function productionAll() {
         $productionItems=$this->productionItems($this->bovineID);
         $productionFinancialStatsPlusMisc=$this->productionFinancialStatsPlusMisc();
         
        
         $htmlTop = <<<HTML
   <div class="row">
  $productionItems
</div>   
HTML;
  

        $html = <<<HTML
   <div class="row">
  <div class="col-md-4">{$this->getLatestMilking($this->bovineID)}</div>
  <div class="col-md-4"> {$this->getLatestAdlicTest($this->bovineFullRegNumber)}</div>
  <div class="col-md-4"> $productionFinancialStatsPlusMisc</div>
  
</div>   
HTML;
         $out[]=($htmlTop.$html);


         $out[]=$this->plotLatestLactation($this->bovineFullRegNumber); //causes screen to be blank. document.ready doesnt work on tab switch.
         return implode($out);
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
            $out[] = (new BootStrap)->errorNotify(($txt));
        } else {
            $out[] = (new BootStrap)->sucessNotify(( "<h3>Milk: Clear</h3>"));
        }

        $isAnimalInDryOffProtocol=TransitionDryoff::isAnimalInDryOffProtocol($bovineID);
        if (!empty($isAnimalInDryOffProtocol)) {
             $txt = '<h3>Dry Off Protocol <ul><span class="smallish">since ' . date('D M j Y', $isAnimalInDryOffProtocol) . '</span></ul>';
             $out[] = (new BootStrap)->warningNotify(($txt));
        }
        
        
        
        //dim
          $out[] =  ((new BootStrap)->bootstrapSmallBox('DIM', self::findCurrentDIM($bovineID), null, 'blue', 'ion ion-ios-clock'));

       

        //3 quarters
        $sql = "SELECT three_quarter FROM bovinemanagement.production_item WHERE bovine_id=$bovineID order by event_time DESC LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ((!empty($row['three_quarter'])) AND ( $row['three_quarter'] == 't')) {
            $out[] = (new BootStrap)->warningNotify(( "<h3>Udder: 3 Quarter</h3>"));
        }

        //slow milking (calculate) TODO


        return(implode($out));
    }

    function findCurrentDIM($bovine_id) {
        $sql = "SELECT EXTRACT(DAY FROM (now()- max(fresh_date))) as days_ago FROM bovinemanagement.bovinecurr WHERE id=$bovine_id AND dry_date is null";
    
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

      
            $ans = $row['days_ago'].'ᵈ';
            return $ans;
        
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

    /*
     * based off total solids. 
     */
   function getAdjustedProjectedRevenue($bovineLocalNumber) {
       
        $sql = "SELECT round(batch.calculate_gross_solid_price(fat305,prot305)) as lact_calculate_gross_solid_price FROM batch.valacta_data_latest_test_view WHERE valacta_data_latest_test_view.chain={$bovineLocalNumber}";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $rev = (!empty($row)) ? $row['lact_calculate_gross_solid_price'] : null;
        return ("Adjusted Projected Revenue ($): <b>$$rev</b><br/>");
        }
        
    function summaryMovement() {

        // get three latest locations
        $query = "SELECT event_time,date_trunc('days',now()-event_time) as days_ago,name,userid
FROM bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location_event.location_id = location.id
WHERE bovine_id={$this->bovineID} ORDER BY event_time DESC LIMIT 3";
        $res = $GLOBALS ['pdo']->query($query);
        $counter = 0;

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            if ($counter == 0) {
                print ("<h2>{$row['name']}</h2>");
            }
            $daysSince = $row ['days_ago'];
            $formattedDate = date("M d Y", strtotime($row ['event_time']));
            $out[] = ("<li>$daysSince (<a class=\"smallish\">$formattedDate</a>) {$row['name']}</li>");
            $counter++;
        }

        print( (new BootStrap)->plainCard('Recent Locations', implode($out)));
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
        $daysTime = $GLOBALS['MiscObj']->daysOrHoursAgo(strtotime($time));
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

    
    /**
     * not actually a quickform
     */
    /*
      function animalQueryQuickForm($pageHeader = false) {

      // custom styling when this form is used at the page header, hacky way
      // to do this. should be a css class.
      if ($pageHeader == true) {
      $style = "id='animalSelect'";
      $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines(true);
      } else {
      $style = '';
      $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines(false);
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
            $Due = $row['due'];
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


         $out[]=(self::valactaMilkPlot($dailyMilkYieldArr, $valactaMilkYieldArr));
         $out[]=(self::valactaSCCPlot($valactaSSCArr));
         $out[]=(self::valactaFatPlot($valactaFatArr, $valactaProtArr));
         $out[]=('<br/>');
         $out[]=self::displayMilkLetDownChart($bovineID);
         return implode($out);
    }

    /* this is only used for previous lactations, not the current one */
    /* does not show parlor data */

    public static function valactaMilkPlot($dailyMilkYieldArr = null, $valactaMilkYieldArr = null) {

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
                $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $dailyMilkYieldArr, $valactaMilkYieldArr);
                $str = $str . ($x->toString());
            } elseif (($dailyMilkYieldArr != null) && ($valactaMilkYieldArr == null)) {
                $opt = "   series: { 1:{ lineWidth: 0, pointSize: 6 } },
                 chartArea: {width: '90%', height: '80%'},
                 legend: {position: 'in'}";
                $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $dailyMilkYieldArr);
                $str = $str . ($x->toString());
            } elseif (($dailyMilkYieldArr == null) && ($valactaMilkYieldArr != null)) {
                $opt = "   series: { 0:{ lineWidth: 2, pointSize: 6,color: 'red' } },
                 chartArea: {width: '90%', height: '80%'},
                 legend: {position: 'in'}";
                $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $valactaMilkYieldArr);
                $str = $str . ($x->toString());
            } else {
                $str = $str . ('<p>No milking data yet.</p>');
            }
            $str = $str . ('</div> <!-- end LactationCurve -->');
        }

        return $str;
    }

    public static function valactaSCCPlot($valactaSSCArr) {
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
            $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $valactaSSCArr);
            $str = $str . ($x->toString());
            $str = $str . ('</div> <!-- end LactationCurve -->');
        }
        return $str;
    }

    public static function valactaFatPlot($valactaFatArr, $valactaProtArr) {
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
            $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $valactaFatArr, $valactaProtArr);
            $str = $str . ($x->toString());
            $str = $str . ('</div> <!-- end LactationCurve -->');
        }
        return $str;
    }

    public function getLatestMilking($bovine_id,$days=7) {
        $numberOfmilkings=$days*2;//days multiplied by number of milkings. 
        
        $sql = <<<SQL
SELECT DISTINCT on (date) date as "Date"
,COALESCE( round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=1 AND x.date=cow.date ),1)::text,'??'::text) as "AM"
,COALESCE( round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=2 AND x.date=cow.date ),1)::text,'??'::text) as "PM"
		FROM alpro.cow 
                WHERE bovine_id=$bovine_id AND date >= (current_date - interval '$days days') ORDER BY date DESC limit $numberOfmilkings
SQL;
       
        
        $out[] = ( (new JQueryDataTable)->startBasicSql('Latest Parlor Milkings', $sql));
        return implode($out);
    }
    
    
     public function getLatestMilkingSimple($bovine_id,$days=3,$showErrors=false) {
          $numberOfmilkings=$days*2;//days multiplied by number of milkings. 
          
            $sql = <<<SQL
SELECT DISTINCT on (date) date as "Date"
                    , (SELECT manualkey FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=1 AND x.date=cow.date )::text as "Manual_AM"
                    , (SELECT manualkey FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=2 AND x.date=cow.date )::text as "Manual_PM"
,COALESCE( round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=1 AND x.date=cow.date ),1)::text,'??'::text) as "AM"
,COALESCE( round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=2 AND x.date=cow.date ),1)::text,'??'::text) as "PM"
		FROM alpro.cow 
                WHERE bovine_id=$bovine_id AND date >= (current_date - interval '$days days') ORDER BY date DESC limit $numberOfmilkings
SQL;    
          $res = $GLOBALS ['pdo']->query($sql);
          $arr= $res->fetchAll(PDO::FETCH_ASSOC);

         // var_dump($arr);
          
          $outArr=array();
           $lowMilkError='';
          foreach($arr as  $key => $value) {
              

          $am= (($value['AM']=='??') ? $value['AM'] : round($value['AM']).'ℓ'); //add litres symbol
          $pm= (($value['PM']=='??') ? $value['PM'] : round($value['PM']).'ℓ');
          $manual_am=  ($value['Manual_AM']=='true') ?'ᴹ':'';
          $manual_pm=  ($value['Manual_PM']=='true') ?'ᴹ':'';                 
          
          $outArr[$key]['Date']= date('D',strtotime($value['Date']));
          $outArr[$key]['AM']= $manual_am.$am;
          $outArr[$key]['PM']= $manual_pm.$pm;
          
          //check if milk production extremely low.
         
          if (($value['AM']) <=5 AND ($value['AM'] <=5) AND ($showErrors==true)) {
         $lowMilkError= (new BootStrap)->dangerAlert('Low Milk','Possible toxic mastitis or DA'); 
          }
          }
          
          return (new BootStrap)->simpleTable($outArr).$lowMilkError;
     }
    
    
     
    

    function getLatestAdlicTest($full_reg) {
        $query = "SELECT test_date,total_milk,fresh,days_in_mi,fat_per,prot_per,ssc,bca_milk,bca_fat,bca_prot,ssc,mun FROM batch.valacta_data WHERE reg='{$this->bovineFullRegNumber}' AND test_date= (SELECT max(test_date) FROM batch.valacta_data WHERE reg='{$this->bovineFullRegNumber}' AND total_milk !=0) ORDER BY test_date LIMIT 1";
        $res = $GLOBALS ['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $out=array();
        if (!empty($row)) {
                
        $ssc = $row ['ssc'] / 1000;

        $out[] = "<li>Date {$row['test_date']}</li>";
        $out[] = "<li>BCA {$row['bca_milk']}-{$row['bca_fat']}-{$row['bca_prot']}</li>";
        $out[] = "<li>Milk {$row['total_milk']} kg</li>";
        $out[] = "<li>SSC {$ssc}k</li>";
        $out[] = "<li>MUN @ test {$row['mun']}</li>";
        $out[] = "<li>DIM @ test {$row['days_in_mi']}</li>";
        }
        return (new BootStrap)->plainCard('Latest Valacta Test', implode($out));
    }

    function showLocationLog($outArray) {

        //show herd cull here at top if necessary
        $query = "select * from bovinemanagement.cull_event where bovine_id={$this->bovineID} limit 1";
        $res2 = $GLOBALS ['pdo']->query($query);
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row2['event_time']) + 1; //add 1 seconds to display at top of list.
            $ftime = date('M j, Y', $time);
            $str = "" . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> <b>{$row2['reason']} {$row2['comment']}</b> by {$row2['userid']}<br/>";
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }

        $res = $GLOBALS['pdo']->prepare('SELECT event_time,userid,location.name,date_trunc(\'days\',(current_date-event_time)) as days_ago FROM bovinemanagement.location_event JOIN bovinemanagement.location ON location.id=location_Event.location_id WHERE bovine_id=? ORDER BY event_time DESC');
        $res->execute(array($this->bovineID));
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['event_time']);
            $ftime = date('M j, Y', $time);
            $str = "" . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> <b>{$row['name']}</b> by {$row['userid']}<br/>";
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

    //shows google chart of latest milk let downs.
    function displayMilkLetDownChart($bovine_id) {
$out=array();

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
            
            
$js = <<<JS
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
                $header
                data.addRows([
                $data
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
JS;            
            
         $out[]=$js;
        }      
        
        return implode($out);
    }

}

//end class
