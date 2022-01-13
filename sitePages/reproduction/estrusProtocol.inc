<?php 

 require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/movementSortGate.inc'); //needed for 

class EstrusProtocol extends BasePage {

    

    public function defaultDisplay() {
        
        $paramRepro= new stdClass();
        $paramRepro->type= 'reproductive';
        $paramRepro->bovineID= null;
        $paramRepro->aniNumber = null;
        $a=new QF2_Protocol((object) array('forwardURLArr'=>(new Quickform2Misc)->forwardURLDefaults()),'group',$paramRepro);
        
        $left = <<<HTML
            {$this->displayProtocolEvents('default','reproductive')}
        HTML; 
            
        $right = <<<HTML
            {$a->renderBox('QF2_Protocol_Repro','Record Protocol Event(s)')}   
        HTML; 
                
        $content=(new Bootstrap)->oneThirdTwoThirds($left,$right);                
        print($content.$this->listDifferentTypesOfProtocols());
        
    }

   
    
    function displayProtocolEvents($mode = 'default',$item='reproductive') {

         //must specify what type of protocol to display.
       
        switch ($item) {
            case "reproductive":
                $type = 'reproductive';
                break;
            case "medical":
                $type = 'medical';
                break;
            case "calf":
                $type = 'calf';
                break;
            default:
            throw new Exception("No Prorotocol type, ie calf specified.");
        }

        $sql = "SELECT *,date_start as event_time,protocol_event.id as id FROM bovinemanagement.protocol_event 
        LEFT JOIN bovinemanagement.protocol_type ON protocol_type.id=protocol_event.protocol_type_id 
        LEFT JOIN bovinemanagement.bovineall ON bovineall.id=protocol_event.bovine_id
        WHERE type='$type'
        ORDER by date_start DESC";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
  
              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
              $rowOut['Location'] = ($row['location_name']);
              $rowOut['Scheduled Start Time'] = JQueryDataTable::formattedDate($row['event_time']);
              $rowOut['Protocol'] = ($row['name']);
              $rowOut['Employee'] = ($row['userid']);
              if ($GLOBALS['auth']->getAdminMode() == 1) {
                   $rowOut['UUID'] = ($row['protocol_uuid']);
              }          
            
              
            if ((strtotime($row['create_time']) >= strtotime('-1 week')) OR ($GLOBALS['auth']->getAdminMode() == 1)) {
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
                  $deleteButtonAsString =$this->printActionButtons($row,$callBackURL);
            } else {
                $deleteButtonAsString = '';
            }
            $rowOut['Action'] = $deleteButtonAsString;
              
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Protocol Events', $dataArr,'order: false')); 
         return implode($out); 
        
        
    }
    
     

    /**     * *****************************************
     * Show Delete Button.
     * ******************************************* */
    function printActionButtons($record,$callBackURL) {

        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.

        $sql = "SELECT id,protocol_uuid FROM bovinemanagement.protocol_event WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (($record['id'] == $row['id'])) { //FIXME, should we only show delete button if none of the medicines has been admnistered?
                $returnButtonsAsString = self::deleteQuickForm($row['id'], $row['protocol_uuid'],$callBackURL); //show delete button, with Breeding table id as index for delete.
            }elseif ($GLOBALS['auth']->getAdminMode() == 1) {
                $returnButtonsAsString = self::deleteQuickForm($record['id'], $record['protocol_uuid'],$callBackURL); //show delete button, with Breeding table id as index for delete.
            }
        }


        return $returnButtonsAsString;
    }

    /**     * ******************************************
     * Delete Button Quickform
     * ******************************************** */
    function deleteQuickForm($tableid, $uuid,$callBackURL) {

        $form = new HTML_QuickForm("breedingProtocolDeleteButton_$uuid", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'uuid', $uuid);
        $form->addElement('submit', 'btnDelete', 'Delete', array('class'=>'btn btn-warning','id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete?')")); //call javascript to confirm delete.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $uuid = $form->getElementValue('uuid');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                
                //delete medicines and google claendar first, because of foreign key, then delete rest. 
                (new MedicineScheduled)->protocolMedicineDefaultDelete($uuid);
                
                
                //do protocol delete
                $sql = "DELETE FROM bovinemanagement.protocol_event WHERE id='$tableid'";
                $res = $GLOBALS['pdo']->query($sql);

                //NOTE: Breedings, embry_flush,  all cascade delete on protocol_uuid, which really chould be a foreign key. . 
                //Medical action does not, because if it does, it will not delete google calendar. 
                
                //do breeding deletes.
             

                //do flush deletes
        

                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
                
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();

                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }



            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    
    
    /**     * *****************************************
     * List Different Types Of Breeding Protocols 
     * ******************************************* */
    function listDifferentTypesOfProtocols() {
        $accordionArray[1]['name'] = 'OVSYNCH -48h (original)';
        $accordionArray[2]['name'] = 'PRESYNCH (only for first postpartum AI)';
        $accordionArray[3]['name'] = 'CIDR SYNCH 56h';
        $accordionArray[4]['name'] = 'OVSYNCH-56';
        $accordionArray[5]['name'] = 'Prostaglandin';
        $accordionArray[6]['name'] = 'Retained Placenta';
        $accordionArray[7]['name'] = 'Metritis Cleanup (half ovysync)';
        $accordion = new AccordionImplementationReproProtocols();
        return($accordion->render($accordionArray));
    }

   
   
    
}//end class


      
Class QF2_Protocol extends QF2_Base {

    function definition($form) {


        
         switch ($this->param->type) {
            case "reproductive":
                $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovinesFemale();
                $dateArray = $GLOBALS['MiscObj']->createDailyDatesForPast7Next14DaysAtNoon(); //dates
                $form->addFieldset('header')->setLabel('Record Breeding Protocol Event');
                $sql = "SELECT id,name FROM bovinemanagement.protocol_type WHERE active=true AND type='reproductive' ORDER BY name";
                break;
            case "medical":
                $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();
                 $dateArray =$GLOBALS['MiscObj']->createDailyDatesForLastandNextFewDays();
                $form->addFieldset('header')->setLabel('Record Medical Protocol Event');
                $sql = "SELECT id,name FROM bovinemanagement.protocol_type WHERE active=true AND type='medical' ORDER BY name";
                break;
            case "calf":
                $aniNumber =$GLOBALS['MiscObj']->createListOfAllYoungCalves();
                 $dateArray =$GLOBALS['MiscObj']->createDailyDatesForLastandNextFewDays();
                $sql = "SELECT id,name FROM bovinemanagement.protocol_type WHERE active=true AND type='calf' ORDER BY name";
                break;
            default:
                throw new EXCEPTION("ERROR: No type of protocol selected.");
        }
      
         //grab list of protocol events. 
        $res = $GLOBALS['pdo']->query($sql);
        $protocolEventsArray[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            $protocolEventsArray[$row[0]] = $row[1];
        }

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
        
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of event.");
        $form->addElement('select', 'protocol_type_id', null, array('options' => $protocolEventsArray, 'label' => 'Type of Protocol Event'))->addRule("required", "Please specify your type of protocol.");
        
        $current_date=$date = strtotime(date('Y-m-d 00:00'));

        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array( 'dateEvent' => $current_date))); //set default date to today.
        
        return $form; //required       
    }

    function process($value) {
           
        
           $date_start = date('Y-m-d H:i:sO', $value['dateEvent']);
             //get name for protocol
            $res = $GLOBALS['pdo']->query("SELECT id,name FROM bovinemanagement.protocol_type WHERE id={$value['protocol_type_id']} limit 1"); 
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $protocol_type_name = $row['name'];
            $protocol_type_id=$value['protocol_type_id'];
            
            //loop over animals
          $bovineIdArr=(new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
          foreach ($bovineIdArr as $key => $bovine_id) {
              
               $uuid = $GLOBALS['MiscObj']->uuid_create();  //create a uuid for this transacation.
        
               $this->addProtocoltoDB($protocol_type_id, $protocol_type_name, $bovine_id, $date_start, $uuid);
            
    }//end loop
    
    }
    
    /*
     * meat and potaoes of protocol insert. 
     */
    public function addProtocoltoDB($protocol_type_id, $protocol_type_name, $bovine_id, $date_start, $uuid) {
         //do insert into protocol table
            $query = "INSERT INTO bovinemanagement.protocol_event  (bovine_id,protocol_type_id,date_start,protocol_uuid,userid)
          VALUES ($bovine_id,$protocol_type_id,'$date_start','$uuid','{$GLOBALS['auth']->getUsername()}')";
            $res99 = $GLOBALS['pdo']->exec($query);
                       
          switch ($this->param->type) {
            case "reproductive":
                $this->reproductiveProtocolDecider($protocol_type_name, $bovine_id, $date_start, $uuid);
                break;
            case "medical":
                $this->medicalProtocolDecider($protocol_type_name, $bovine_id, $date_start, $uuid);
                break;
            case "calf":
               $this->calfProtocolDecider($protocol_type_name, $bovine_id, $date_start, $uuid);
                break;
            default:
                throw new EXCEPTION("ERROR: No type of protocol selected.");
        }
        
        
    
    
    
    }
    
    
    
    
    
    function reproductiveProtocolDecider($reproductive_protocol_type_name, $bovine_id, $date_start, $uuid) {

             $obj = new ReproductionProtocolAction();
         
        
        switch ($reproductive_protocol_type_name) {

            case "Ovsynch 48h":
                return $obj->protocol_OVSYNCH48h($bovine_id, $date_start, $uuid);
                break;
            case "Retained Placenta":
                return $obj->protocol_retainedPlacenta($bovine_id, $date_start, $uuid);
                break;
            case "Metritis Cleanup":
                return $obj->protocol_metritisCleanup($bovine_id, $date_start, $uuid);
                break;
            case "Ovsynch 56h":
                return $obj->protocol_OVSYNCH56h($bovine_id, $date_start, $uuid);
                break;
            case "CIDR Sync 56h":
                return $obj->protocol_CIDRSYNC56h($bovine_id, $date_start, $uuid);
                break;
            case "CIDR Sync 56h Heifer":
                return $obj->protocol_CIDRSYNC56h_Heifer($bovine_id, $date_start, $uuid);
                break;      
            case "Ovsynch 56h 8 Day":
                return $obj->protocol_OVSYNCH56h_8day($bovine_id, $date_start, $uuid);
                break;
            case "CIDR Sync 56h 8 Day":
                return $obj->protocol_CIDRSYNC56h_8day($bovine_id, $date_start, $uuid);
                break;
            case "Pg Kamar":
                return $obj->protocol_pg_kamar($bovine_id, $date_start, $uuid);
                break;
            case "Watch Kamar":
                return $obj->protocol_watch_kamar($bovine_id, $date_start, $uuid);
                break;
            case "Ovsynch 56h w/ DNB":
                return $obj->protocol_OVSYNCH56h_DNB($bovine_id, $date_start, $uuid);
                break;
            case "Flush (Modified)":
                return $obj->protocol_FlushModified($bovine_id, $date_start, $uuid);
                break;
            default:
                throw new EXCEPTION("ERROR: No protocol with give name");
                return null;
        }
    }

    function medicalProtocolDecider($medical_protocol_type_name, $bovine_id, $date_start, $uuid) {
        
           $obj = new MedicalProtocolAction();
        
        switch ($medical_protocol_type_name) {

            case "Ketosis 1.3 to 2.4 mmol":
                return $obj->protocol_ketosis1_3to2_4($bovine_id, $date_start, $uuid);
                break;
            case "Ketosis 2.5 to 3.5 mmol":
                return $obj->protocol_ketosis2_5to3_5($bovine_id, $date_start, $uuid);
                break;
             case "Toxic Mastitis":
                return $obj->protocol_toxic_mastitis($bovine_id, $date_start, $uuid);
                break;         
            case "Pneumonia (cow)":
                 return $obj->protocol_pneumonia_cow($bovine_id, $date_start, $uuid);
                break;
            case "DA (post surgery)":
                 return $obj->protocol_post_da($bovine_id, $date_start, $uuid);
                break;
            
            default:
                throw new EXCEPTION("ERROR: No protocol with give name");
                return null;
        }
    }

    function calfProtocolDecider($calf_protocol_type_name, $bovine_id, $date_start, $uuid) {
        
         $obj = new CalfProtocolAction();
        
        switch ($calf_protocol_type_name) {

            case "Temp only (no scours)":
                return $obj->protocol_HighTemp($bovine_id, $date_start, $uuid);
                break;
            
            case "Scours Minor":
                return $obj->protocol_ScoursWithNoTempMinor($bovine_id, $date_start, $uuid);
                break;
            
            
            case "Scours + Temp":
                return $obj->protocol_ScoursWithTemp($bovine_id, $date_start, $uuid);
                break;
            
            case "Scours Only (no temp)":
                return $obj->protocol_ScoursWithNoTemp($bovine_id, $date_start, $uuid);
                break;
            
            case "Pneumonia":
                return $obj->protocol_Pneumonia($bovine_id, $date_start, $uuid);
                break;
            
            case "Cryptosporidiosis Prevention":
                return $obj->protocol_CryptosporidiosisPrevention($bovine_id, $date_start, $uuid);
                break;
            
            default:
                throw new EXCEPTION("ERROR: No protocol with give name");
                return null;
        }
    }

}//end class

class CalfProtocolAction {
    
    //medicine id's for current;y used hormones, these could change in future when we use a new drug.
    private $halocur;
    private $metacam;
    private $vitamins_ad_500;
    private $electrolyte;
    private $oxymycine;
    private $penicillin;
    private $dexamethasone;
    private $muse;
    private $newcells;
    private $trimidox;
    private $resflor;
    private $scourBolus;
    
    public function __construct() {
      
        $this->halocur = 85; //$GLOBALS['config']['MEDICAL']['XXXXXXX']
        $this->metacam = 79;
        $this->vitamins_ad_500 = 68;
        $this->electrolyte = 50;
        $this->oxymycine = 77;
        $this->penicillin = 13;
        $this->penicillin_la = 16;
        $this->dexamethasone = 30;
        $this->muse = 35;
        $this->newcells = 31;
        $this->trimidox=33;
        $this->resflor=69;
        $this->scourBolus=19;
    }
    
    
     //SCOURS NO TEMP
    function protocol_ScoursWithNoTempMinor($bovine_id, $date, $uuid) {
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: metacam 
          Day 0: vitamins_ad_500
          Vet: David March 2021.
         
         */
        //do meds
        $c = new MedicineScheduled();
        //day 0
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), 34575, $uuid); // take temp
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->scourBolus, $uuid);
        //day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->scourBolus, $uuid);
        //day 2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->scourBolus, $uuid);
        //day 3
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->scourBolus, $uuid);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), 34575, $uuid); // take temp

    }
    
    
    
    //SCOURS NO TEMP
    function protocol_ScoursWithNoTemp($bovine_id, $date, $uuid) {
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: metacam 
          Day 0: vitamins_ad_500
          DAY 0: Electrolyte 2x for 3 days 12pm and 8pm.
          Vet: Paige Feb 2021.
         
         */
        //do meds
        $c = new MedicineScheduled();
        //day 0
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->metacam, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->vitamins_ad_500, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->scourBolus, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 4 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 12 hours"), $this->electrolyte, $uuid,null,2);
        //day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->scourBolus, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 28 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 36 hours"), $this->electrolyte, $uuid,null,2);
        //day 2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->scourBolus, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 52 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 60 hours"), $this->electrolyte, $uuid,null,2);
        //day 3
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->scourBolus, $uuid);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), 34575, $uuid); // take temp
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 76 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 84 hours"), $this->electrolyte, $uuid,null,2);

    }
    
    //SCOURS WITH TEMP
    function protocol_ScoursWithTemp($bovine_id, $date, $uuid) {
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: metacam 
          Day 0: vitamins_ad_500
          DAY 0: Electrolyte 2x for 3 days 12pm and 8pm.
          Day 0: Trimidox 1x for 4 days
          Vet: Paige Feb 2021.
         
         */
        //do meds
        $c = new MedicineScheduled();
        //day 0
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->metacam, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->vitamins_ad_500, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 4 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 12 hours"), $this->electrolyte, $uuid,null,2);
        //day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 28 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 36 hours"), $this->electrolyte, $uuid,null,2);
        //day 2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 52 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 60 hours"), $this->electrolyte, $uuid,null,2);
        //day 3
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), 34575, $uuid); // take temp
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 76 hours"), $this->electrolyte, $uuid,null,2);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 84 hours"), $this->electrolyte, $uuid,null,2);

    }
    
    //HIGH TEMP ONLY
    function protocol_HighTemp($bovine_id, $date, $uuid) {
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
         * 
          DAY 0: Metacam
          DAY 0: Trimidox
          DAY +1: Trimidox
          DAY +2: Trimidox
          DAY +3: Trimidox 
          Vet: Paige Feb 2021.
        */
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->metacam, $uuid); 
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), 34575, $uuid); // take temp
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->trimidox, $uuid,null,5);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), 34575, $uuid); // take temp


    }
    
    
    //PNEUMONIA
    function protocol_Pneumonia($bovine_id, $date, $uuid) {
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
         * 
          DAY 0: Resflor subQ
          Day 2: Resflor subQ
          Vet: Paige Feb 2021.
        */
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"),  $this->resflor, $uuid,null,8);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->resflor, $uuid,null,8);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), 34575, $uuid); // take temp


    }
    
    
    //CRYPTO
     function protocol_CryptosporidiosisPrevention($bovine_id, $date, $uuid) {
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          7 days halocur 1x per day morning
         */
        //do meds
        $c = new MedicineScheduled();

        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->muse, $uuid,null,0.5); //Mu-Se givien now because it fits calf mamangement, no related to halocur. 
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->halocur, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->halocur, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->halocur, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->halocur, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->halocur, $uuid);      
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 120 hours"), $this->halocur, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 144 hours"), $this->halocur, $uuid);
       
    }
    
}//end class

/*
 * class for medical protocols.
 */
class MedicalProtocolAction {

    

    
     //medicine id's for current;y used hormones, these could change in future when we use a new drug.
    private $glycol_id;
    private $newcells_id;
    private $flunixin;
    private $trimidox;
    private $penicillin;
    private $sortCls;


    public function __construct() {
        $this->glycol_id = 49;
        $this->newcells_id = 31;
        //
        $this->flunixin = 15;
        $this->trimidox = 33;
        $this->penicillin = 13;
        
        //setup sort gate to use in this class
        (new MovementSortGate);//hack for class load
        $this->sortCls = new QF2_SortGate((object) array('forwardURLArr' => array()), 'individual'); //just so we can call sortAnimal. bit messy.
        
        
    }

    
    
    public function protocol_pneumonia($bovine_id, $date, $uuid) {
        /*
        5 days 50cc trimidox 1x
        3 days dexamathodone 1x
        3 days flunixin 1x
        */
        
        
    }
    
    
    /**
     * Do db insert for ketosis mild protocol
     */
    public function protocol_ketosis1_3to2_4($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
 
          Day 0: 10 ml vitamin B  8am. newcells
          Day 0: 250 ml propylene glycol 8am.
          Day 0: 250 ml propylene glycol 4pm.
          Day 1: 250 ml propylene glycol 8am.
          Day 1: 250 ml propylene glycol 4pm.
          Day 2: 250 ml propylene glycol 8am.
          Day 2: 250 ml propylene glycol 4pm.
          Day 3: 250 ml propylene glycol 8am.
          Day 3: 250 ml propylene glycol 4pm.
          Day 4: 250 ml propylene glycol 8am.
          Day 4: 250 ml propylene glycol 4pm.
          Day 5: 250 ml propylene glycol 8am.
          Day 5: 250 ml propylene glycol 4pm.
         
         */
        //do meds
        //day 0
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->newcells_id, $uuid);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->glycol_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 12 hours"), $this->glycol_id, $uuid);
        //day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->glycol_id, $uuid);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 36 hours"), $this->glycol_id, $uuid);  
        //day 2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->glycol_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 60 hours"), $this->glycol_id, $uuid);
        //day 3       
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->glycol_id, $uuid);      
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 84 hours"), $this->glycol_id, $uuid);
        //day 4
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->glycol_id, $uuid);      
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 108 hours"), $this->glycol_id, $uuid);
        //   
    }
    
    /**
     * Do db insert for ketosis medium protocol
     */
    public function protocol_ketosis2_5to3_5($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Day 0: drench cow with electrolytes
          Day 0: 10 ml vitamin B  8am. newcells
          Day 0: 250 ml propylene glycol 8am.
          Day 0: 250 ml propylene glycol 4pm.
          Day 1: 250 ml propylene glycol 8am.
          Day 1: 250 ml propylene glycol 4pm.
          Day 2: 250 ml propylene glycol 8am.
          Day 2: 250 ml propylene glycol 4pm.
          Day 3: 250 ml propylene glycol 8am.
          Day 3: 250 ml propylene glycol 4pm.
          Day 4: 250 ml propylene glycol 8am.
          Day 4: 250 ml propylene glycol 4pm.
          Day 5: 250 ml propylene glycol 8am.
          Day 5: 250 ml propylene glycol 4pm.
         
         */
        //do meds
        //day 0
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->newcells_id, $uuid);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->glycol_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 12 hours"), $this->glycol_id, $uuid);
        //day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->glycol_id, $uuid);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 36 hours"), $this->glycol_id, $uuid);  
        //day 2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->glycol_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 60 hours"), $this->glycol_id, $uuid);
        //day 3       
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->glycol_id, $uuid);      
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 84 hours"), $this->glycol_id, $uuid);
        //day 4
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->glycol_id, $uuid);      
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 108 hours"), $this->glycol_id, $uuid);
        //   
    }
    
    /**
     * Do db insert for toxic mastitis. 
     */
    public function protocol_toxic_mastitis($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Day 0: saline solution IV and have cow drink water (pump).
          Day 1: 40 ml trimidox 8am.
          Day 2: 40 ml trimidox 8am.
          Day 3: 40 ml trimidox 8am.
          Day 1: 32 ml Flunixin 8am.
          Day 2: 32 ml Flunixin 8am.
          Day 3: 32 ml Flunixin 8am. ($bovine_id, $event_time, $medicine_id, $protocol_uuid, $transaction_uuid = null, $dosage_overide = null, $location_overide = null
    
         */
        //do meds
        //day 0
        $c = new MedicineScheduled();
        // Day 0
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->flunixin, $uuid,null,32);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->trimidox, $uuid);
        // Day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->flunixin, $uuid,null,32);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->trimidox, $uuid);
        // Day2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->flunixin, $uuid,null,32);     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->trimidox, $uuid);
        
    }
    
    
     public function protocol_pneumonia_cow($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Trimidox 50cc 1X for 5 days.
          Flunixin 25cc 1x for 3 days
          Vet: Paige Feb 2021
         */
        //do meds
        //day 0
        $c = new MedicineScheduled();
        // Day 0
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->flunixin, $uuid,null,25,'intramuscular');     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->trimidox, $uuid,null,50);
        // Day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->flunixin, $uuid,null,25,'intramuscular');     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->trimidox, $uuid,null,50);
        // Day2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->flunixin, $uuid,null,25,'intramuscular');     
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->trimidox, $uuid,null,50);
        // Day3  
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->trimidox, $uuid,null,50);
        // Day4  
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->trimidox, $uuid,null,50);
        
    }
    
    /*
     * after DA surgery.
     */
     public function protocol_post_da($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          penicillin 
         */
        //do meds
        //day 0
        $c = new MedicineScheduled();
        // Day 0
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->penicillin, $uuid,null,40);
        // Day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->penicillin, $uuid,null,40);
        // Day2
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->penicillin, $uuid,null,40);
        // Day3  
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->penicillin, $uuid,null,40);
        // Day4  
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->penicillin, $uuid,null,40);
        
    }
    
    
}//end class

class ReproductionProtocolAction {
    
    //medicine id's for current;y used hormones, these could change in future when we use a new drug.
    private $prostaglandin_id;
    private $gnrh_id;
    private $cidr_id;
    private $folltropin_id;
    private $protocol_watch_id;
    private $sortCls;

    public function __construct() {
      
        $this->prostaglandin_id = $GLOBALS['config']['MEDICAL']['prostaglandin_id'];
        $this->gnrh_id = $GLOBALS['config']['MEDICAL']['gnrh_id'];
        $this->cidr_id = $GLOBALS['config']['MEDICAL']['cidr_id'];
        $this->folltropin_id = $GLOBALS['config']['MEDICAL']['folltropin_id'];
        $this->protocol_watch_id = $GLOBALS['config']['ESTRUS']['protocol_watch_id'];
        
         //setup sort gate to use in this class
        (new MovementSortGate);//hack for class load
        $this->sortCls = new QF2_SortGate((object) array('forwardURLArr' => array()), 'individual'); //just so we can call sortAnimal. bit messy.
        
        
    }
    
    
   /**
     * Do db insert OVSYNCH48h protocol
     */
    function protocol_OVSYNCH48h($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '16:00:00');
        /*
          DAY 0: Inject GnRH to ovulate follicle and start a new follicular wave (ie: 3-5 pm)
          DAY 7: Inject PGF2α to regress CL (ie: 3-5 pm)
          DAY 9: Inject GnRH to ovulate follicle (ie: 3-5 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 7-9 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 224 hours"), $this->gnrh_id, $uuid);
        //insert breeding.
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 240 hours"), $uuid);
    }

    /**
     * Do db insert OVSYNCH 56h protocol
     */
    function protocol_OVSYNCH56h($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: Inject GnRH (ie: 6-8 am)
          DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)
          DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), $this->prostaglandin_id, $uuid);
        $this->sortCls->sortAnimal($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"),$uuid); //sort in am
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 224 hours"), $this->gnrh_id, $uuid); // day 9
        //insert breeding.
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 240 hours"), $uuid);
        $this->sortCls->sortAnimal($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 240 hours"),$uuid); //sort in am
       
      
    }
    
    
    /**
     * Do db insert OVSYNCH 56h protocol where we DNB at end.
     */
    function protocol_OVSYNCH56h_DNB($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: Inject GnRH (ie: 6-8 am)
          DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)
          DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 224 hours"), $this->gnrh_id, $uuid); // day 9
       
    }
    
    /**
     * Do db insert OVSYNCH 56h protocol 8 day for no sunday shots.
     */
    function protocol_OVSYNCH56h_8day($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: Inject GnRH (ie: 6-8 am)
          DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)
          DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 192 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 248 hours"), $this->gnrh_id, $uuid); // day 9
        //insert breeding.
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 264 hours"), $uuid);
    }
    

    /**
     * Do db insert CIDR SYNC 56h protocol
     */
    function protocol_CIDRSYNC56h($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: Inject GnRH + insert CIDR (ie: 6-8 am)
          DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)
          DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->cidr_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), 34576, $uuid); // remove CIDR
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 224 hours"), $this->gnrh_id, $uuid); // day 9
        //insert breeding.
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 240 hours"), $uuid);
    }
    
    /**
     * Do db insert CIDR SYNC 56h protocol 8 day, no Sunday shots
     */
    function protocol_CIDRSYNC56h_8day($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: Inject GnRH + insert CIDR (ie: 6-8 am)
          DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)
          DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->cidr_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 192 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 192 hours"), 34576, $uuid); // remove CIDR
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 248 hours"), $this->gnrh_id, $uuid); // day 9
     
        //insert breeding.
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 264 hours"), $uuid);
    }
    

    /**
     * Do db insert CIDR SYNC 56h protocol - Used for HEFIERS ONLY (ie never calved)
     */
    function protocol_CIDRSYNC56h_Heifer($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          DAY 0: Inject GnRH + insert CIDR (ie: 6-8 am)
          DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)
          DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)
          DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->cidr_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicalActionDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), 34576, $uuid); // remove CIDR
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 192 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 224 hours"), $this->gnrh_id, $uuid); // day 9
        //insert breeding.
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 240 hours"), $uuid);
    }

    /**
     * Do db insert retainedPlacenta protocol
     */
    function protocol_retainedPlacenta($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Day 0: Inject PGF2α 8am.
          Day 0: Inject PGF2α 4pm.
          Day 4: Inject GnRH 8am.
          Day 14: Inject PGF2α 8am.
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 8 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 336 hours"), $this->prostaglandin_id, $uuid);
    }
    
    
    /**
     * Do db insert Metritis Cleanup protocol
     */
    function protocol_metritisCleanup($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Day 0: Inject GnRH 8am.
          Day 7: Inject PGF2α 4pm.
          Have vet check.
         */
        //do meds
        $c = new MedicineScheduled();
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->gnrh_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 168 hours"), $this->prostaglandin_id, $uuid);
    }
    
    
     /**
     * Do db insert pg kamar
     */
     function protocol_pg_kamar($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Day 0: Inject PGF2α 8am.
          Day 0: Kamar on 8am.
          Day 3: Watch for heat.
         */
        //do meds
        $c = new MedicineScheduled();
        $e = new EstrusHeats();
        $k = new EstrusKamar();

            //this really should be in a PDO transcation, but protocolMedicineDefaultInsert needs to be atomic for sure. not sure how to do this correctly without a pile of code. 
        try {
            $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->prostaglandin_id, $uuid);
            $e->protocolEstrusDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->protocol_watch_id, $uuid);
            $k->protocolKamarDefaultScheduleInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), 'on', $uuid);
        }
        catch (PDOException $e) {
            //run delete button
            //FIXME.
        }
        
    }

    /**
     *  insert watch kamar 
     */
    function protocol_watch_kamar($bovine_id, $date, $uuid) {
        //start at 8am.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Day 0: Kamar on 8am.
          Day 3: Watch for heat.
         */
        
        //do schedules
        $e= new EstrusHeats();
        $k = new EstrusKamar();
         
        $e->protocolEstrusDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->protocol_watch_id, $uuid);   
        $k->protocolKamarDefaultScheduleInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), 'on', $uuid);
    }
    
      
    
    

    /**
     * Do db insert FLUSH protocol
     */
    function protocol_FlushModified($bovine_id, $date, $uuid) {
        //start at 4pm.
        $start_time = (date('Y-m-d', strtotime($date)) . ' ' . '08:00:00');
        /*
          Tuesday December 10th
          Folltropin  AM and PM   @ 3.0 ml

          Wednesday December 11th
          Folltropin  AM and PM   @ 3.0 ml
          RECIPIENTS  AM Estrumate  AM @ 2.0 ml   OR   Lutalyse  AM @ 5.0 ml

          Thursday December 12th
          Folltropin  AM  @ 2.0 ml
          Estrumate  AM @ 2.0 ml   OR   Lutalyse AM  @ 5.0 ml  DONOR
          and
          Folltropin  PM  @ 2.0 ml
          Estrumate  PM @ 2.0 ml   OR   Lutalyse AM  @ 5.0 ml  DONOR

          Friday December 13th
          Folltropin  AM  and PM @ 2.0 ml

          Saturday December 14th
          Cystorelin  AM @ 3.0 ml
          BREED  PM

          Sunday December 15th
          BREED  AM

          Saturday December 21st
          FLUSH  AM

         */
        //do meds

        $c = new MedicineScheduled();

        //day 1
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 0 hours"), $this->folltropin_id, $uuid, null,3);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 12 hours"), $this->folltropin_id, $uuid,null, 3);
        //day 2 
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 24 hours"), $this->folltropin_id, $uuid,null, 3);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 36 hours"), $this->folltropin_id, $uuid,null,3);
        //needle resipients here
        //day 3
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->folltropin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 48 hours"), $this->prostaglandin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 60 hours"), $this->folltropin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 60 hours"), $this->prostaglandin_id, $uuid);
//day 4
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 72 hours"), $this->folltropin_id, $uuid);
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 84 hours"), $this->folltropin_id, $uuid);
        //day 5
        $c->protocolMedicineDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 96 hours"), $this->gnrh_id, $uuid,null, 3);
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 108 hours"), $uuid);
        //day 6
        EstrusBreedings::insertCowToBeBred($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 120 hours"), $uuid);
        //
        //Schedule flush
        EstrusEmbryo::scheduleFlushDefaultInsert($bovine_id, $GLOBALS['MiscObj']->timeOffset($start_time, " + 264 hours"), $uuid);
    } 
    
}//end class


/* supports both individual and group */

class AccordionImplementationReproProtocols extends AccordionSub {

    function tab1() {
        return('
<div>
<p>
OvSynch uses PGF2α and GnRH to synchronize ovulation in lactating dairy cows. OvSynch was the first synchronization protocol developed that allowed for timed AI (TAI) resulting in conception rates similar to that of A.I. after a detected estrus_event.
</p>
<h4>PROCEDURE:</h4>
<ul>
<li>DAY 0: Inject GnRH to ovulate follicle and start a new follicular wave (ie: 3-5 pm)</li>
<li>DAY 7: Inject PGF2α to regress CL (ie: 3-5 pm)</li>
<li>DAY 9: Inject GnRH to ovulate follicle (ie: 3-5 pm)</li>
<li>DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 7-9 am)</li>
</ul>

<h4>FEATURES:</h4>
<ul>
<li>GnRH causes ovulation and initiation of a new follicular wave.</li>
<li>Prostaglandin causes regression of the corpora lutea.</li>
<li>The second GnRH synchronizes the time of ovulation of the dominant follicle of the follicular wave that began growing after the first GnRH injection.</li>
<li>Cows don\'t necessarily need to respond to the first GnRH injection to synchronize to the second GnRH injection.</li>
</ul>
<img src="/images/protocol/ovsynch48.jpg"/>
<ADDRESS>source:http://www.accelgen.com/Available_Synchronization_Programs.aspx</ADDRESS>

</div>');
    }

    function tab2() {
        return('
<div>
<p>
PreSynch is a modification of OvSynch in which two PGF2α injections 14 days apart are administered, with the second PGF injection 12 days before initiation of the first GnRH injection of OvSynch. PreSynch improves first service conception rate compared to OvSynch and is a good strategy for programming cows to receive their first postpartum timed A.I. Some people prefer to keep using 14 days between the second PGF and the initiation of Ovsynch, but the need to be aware of a small decrease in fertility compared to the 12 day interval.
</p>
<h4>PROCEDURE:</h4>
<ul>
<li>DAY 0: PGF to start pre-synchronization (ie: 6-8 am)</li>
<li>DAY 14: Second PGF should be used at the end of the voluntary waiting period, normally, around 50 days in milk (ie: 6-8 am)</li>
<li>DAY 26: Inject GnRH to ovulate follicle and start a new follicular wave (ie: 6-8 am)</li>
<li>DAY 33: Inject PGF2α to regress CL (ie: 6-8 am)</li>
<li>DAY 35: Inject GnRH to ovulate follicle (ie: 2-4 pm)</li>
<li>DAY 36: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)</li>
</ul>

<h4>FEATURES:</h4>
<ul>
<li>This system requires a long duration and timing. It is good for dairy cows that will receive their first postpartum timed A.I. The figure above shows a time line of DIM, thus a cow receiving TAI at 68 DIM, should receive her first PGF2α. Injection at 30 DIM.</li>
<li>Not a good resynchronization tool because of the long duration.</li>
<li>Studies have shown that conception rate was greater for cows receiving PreSynch vs. OvSynch.</li>
</ul>
<img src="/images/protocol/presynch.jpg"/>
<ADDRESS>source:http://www.accelgen.com/Available_Synchronization_Programs.aspx</ADDRESS>

</div>');
    }

    function tab3() {
        return('
<div>
<p>
CIDR Synch is an OvSynch program with the addition of a CIDR inserted for 7 days (inserted at the time of first GnRH and removed at the time of PGF2α injection). When using this program in Dairy or Beef heifers, the first GnRH shot may not be given, achieving similar results. Two days after CIDR removal and PGF2α, administer GnRH and TAI 16 hours later.
</p>

<h4>PROCEDURE:</h4>
<ul>
<li>DAY 0: Inject GnRH + insert CIDR (ie: 6-8 am)</li>
<li>DAY 7: Inject PGF2α + remove CIDR (ie: 6-8 am)</li>
<li>DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)</li>
<li>DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)</li>
</ul>

<h4>FEATURES:</h4>
<ul>
<li>CIDRs provide progesterone as a means to deal with variation in stage of the estrus cycle.</li>
<li>CIDRs induces cyclicity in anestrous females increasing their probability of a fertile ovulation.</li>
<li>Early heats are prevented.</li>
<li>CIDRs is the most reliable strategy for TAI in dairy heifers.</li>
</ul>
<img src="/images/protocol/cidrsynch56.jpg"/>
<ADDRESS>source:http://www.accelgen.com/Available_Synchronization_Programs.aspx</ADDRESS>

</div>');
    }

    function tab4() {
        return('
<div>
<p>
OvSynch-56 is a new modification of the original protocol that gives the second GnRH injection 56 hours after PGF2α and TAI 16 hours later (72 hours after the PGF2α. OvSynch-56 improved conception rates in almost 10 percent compared to OVSynch.
</p>

<h4>PROCEDURE:</h4>
<ul>
<li>DAY 0: Inject GnRH to ovulate follicle and start a new follicular wave (ie: 6-8 am)</li>
<li>DAY 7: Inject PGF2α to regress CL (ie: 6-8 am)</li>
<li>DAY 9: Inject GnRH to ovulate follicle (ie: 2-4 pm)</li>
<li>DAY 10: Timed AI 12-16 hours after second GnRH - (ie: 6-8 am)</li>
</ul>

<h4>FEATURES:</h4>
<ul>
<li>Should be used in conjunction with PreSynch.</li>
<li>OvSynch-56 improved conception rates in almost 10 percent points compared to OvSynch.</li>
<li>Cows detected in estrus should be bred accordingly.</li>
<li>Allows more time for follicular maturation compared to OvSynch.</li>
</ul>
<img src="/images/protocol/ovsynch56.jpg"/>
<ADDRESS>source:http://www.accelgen.com/Available_Synchronization_Programs.aspx</ADDRESS>


</div>
');
    }

    function tab5() {
        return('
<div>
<p>
An injection of prostaglandin F2a (PGF2α) is given at a random stage of the cycle. Animals with an active corpus luteum (CL) will undergo CL regression after the PGF2α challenge and will come into heat between 2-7 days after the shot. Animals detected in estrus can be bred. Only those animals not showing heat (and not bred) after the first shot, will receive a second PGF2α injection 11-14 days after the first PGF2α and should be bred accordingly. It is not recommended to conduct TAI after a PGF2α synchronization program, since the follicular wave was not synchronized and the stage of follicular maturation is uncertain.
<b>Important: prostaglandin cannot be used in pregnant animals – it will likely induce abortion.</b>
</p>

<h4>FEATURES:</h4>
<ul>
<li>Effective and inexpensive program to breed dairy and beef heifers and beef cows.</li>
<li>Prostaglandin causes regression of the CL. Bred animals should be dropped from the rest of the treatment.</li>
<li>Young heifers (i.e. 11-12 months of age) and early postpartum cows (less that 30 DIM) should not be bred after the first shot.</li>
<li>By rectal palpation or ultrasound examination, you can also determine the presence of a Corpus Luteum (CL), and only these animals will receive the first shot. The second shot will be given to animals not bred and/or not receiving the first shot.</li>
<img src="/images/protocol/Prostaglandin.jpg"/>
<ADDRESS>source:http://www.accelgen.com/Available_Synchronization_Programs.aspx</ADDRESS>
</div>');
    }

    function tab6() {
        return('
        
<div>
<p>Start protocol 8 days after calving for any cow who had a retained placenta.</p>

<h4>PROTOCOL:</h4>
<ul>
<li>Day 0:   Inject PGF2α 8am.</li>
<li>Day 0:   Inject PGF2α 4pm.</li>
<li>Day 4:   Inject GnRH 8am.</li>
<li>Day 14:  Inject PGF2α 8am.</li>

</div>');
    }

}

 function tab7() {
        return('
        
<div>
<p>Used to clean up cows with metritis</p>

<h4>PROTOCOL:</h4>
<ul>
<li>Day 0:   Inject GnRH 8am.</li>
<li>Day 7:   Inject PGF2α 8am.</li>
<li>Have vet check next time.</li>

</div>');
    }


?>