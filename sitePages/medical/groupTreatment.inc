<?php

/** * *************************************
 *  Used to input when a cow was dewormed and who needs dewormed.
 * ****************************************** */
class GroupTreatment extends TabPage {

    public function defaultDisplay() {

        $tabArray[1]['name'] = 'DeWorm';
        $tabArray[2]['name'] = 'DeHorn';
        $tabArray[3]['name'] = 'Disease (Long Term)';
        $this->renderTabs($tabArray);
    }

    function tab1() {

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
        $paramMed = new stdClass();
        $paramMed->medicineListCustom = 'endectocide';
        $b = new QF2_GeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', $paramMed); //group

        $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$b->renderBox('qf2_GeneralTreatment', 'Select Animals / Groups to Treat')}
           
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayListOfDewormersAdministered()}
            {$this->displayListNeedingDeworming()}
        </div>
    </div>    
HTML;

        print($html);
    }

    function tab2() {

        $c = new QF2_Dehorn((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', null); //group

        $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$c->renderBox('qf2_Dehorn', 'Select Animals / Groups to Treat')}
           
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayListOfAnimalsWithNoDehornEvent()}
        </div>
    </div>    
HTML;

        print($html);
    }

    function tab3() {

        $c = new QF2_DiseaseLongTerm((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', null); //group

        $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$c->renderBox('qf2_Dehorn', 'Select Animals / Groups to Record Disease Status')}
           
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayListOfAnimalsWithLongTermDiseaseStatus()}
        </div>
    </div>    
HTML;

        print($html);
    }
    
    function displayListOfAnimalsWithLongTermDiseaseStatus($mode = 'default') {


        $sql = <<<SQL
SELECT bovine.id,bovine.full_name,bovine.local_number, medical_disease.*  
FROM bovinemanagement.medical_disease 
LEFT JOIN bovinemanagement.bovine on bovine_id=bovine.id
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Disease'] = $row['disease'];
            $rowOut['Status'] = $row['status'];

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Animals With Long Term Disease Events', $dataArr, ''));
        return implode($out);
    }

    function displayListOfAnimalsWithNoDehornEvent($mode = 'default') {


        $sql = <<<SQL
 SELECT bovinecurr.id,bovinecurr.full_name,bovinecurr.local_number, dehorn_event.* FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.dehorn_event on bovine_id=bovinecurr.id WHERE dehorn_event_type is null
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Status'] = 'No Event';

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Animals With No DeHorn Events', $dataArr, ''));
        return implode($out);
    }

    /**     * ****************************************** */
    /** Display list of all dewormers Administered   */

    /**     * ****************************************** */
    function displayListOfDewormersAdministered($mode = 'default') {


        $sql = <<<SQL
SELECT medicine_administered.id as medicine_administered_id, bovine_id as bovine_id,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time,medicine_administered.id as medicine_administered_id
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
  WHERE medicine.medicine_class='endectocide' AND event_time > (current_date - interval '2 years')
  ORDER BY medicine_administered.event_time DESC,trade_name,local_number
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

             $delButton=new QF2_DeleteButtonDecider('user_timeLimit',(object) array('dateTimePeriodDays' =>'1','dateTimeName'=>'event_time','schemaTable'=>'bovinemanagement.medicine_administered'));
     
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Which Foot'] = ($row['trade_name']);
            $rowOut['Dosage'] = ($row['dosage']);
            $rowOut['Dosage Unit'] = ($row['dosage_unit']);
            $rowOut['Location'] = ($row['location']);
            //$rowOut['Note'] = ($row['note']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
 
            $rowOut['Action'] = $delButton->output($row['medicine_administered_id']);
                    
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Completed Dewormed', $dataArr, 'order: false'));
        return implode($out);
    }

    /**     * ****************************************** */
    /** Display list of all dewormers Needed   */

    /**     * ****************************************** */
    function displayListNeedingDeworming($mode = 'default') {


        $sql = <<<SQL
   SELECT id,id as bovine_id,local_number,full_name,location_name,(SELECT max(event_time) from bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index where medicine.medicine_class='endectocide'  and bovine_id=bovinecurr.id ) as event_time 
   FROM bovinemanagement.bovinecurr WHERE bovinecurr.birth_date <= (now()- interval '6 months')
   EXCEPT
   SELECT bovinecurr.id,bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name,bovinecurr.location_name,(SELECT max(event_time) from bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index where medicine.medicine_class='endectocide' and bovine_id=bovinecurr.id ) as event_time  
   FROM bovinemanagement.medicine_administered
   LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id=bovinecurr.id
   LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index
   WHERE medicine.medicine_class='endectocide' AND medicine_administered.event_time >= (now() - interval '280 days')
   ORDER BY location_name,local_number DESC
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = ($row['location_name']);
            $rowOut['Drug To Use'] = $this->printDrugToUse($row);
            $rowOut['Last Administered'] = JQueryDataTable::formattedDate($row['event_time']);
           

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Not Dewormed in the Last 280 Days', $dataArr, ''));
        return implode($out);
    }

    /**
     * if the cow could be pregnant we can only use certain vaccines.
     */
    function printDrugToUse($record) {

        //any animal who is less than 49 days from calving or is milking, has to get a 0 milk with-holding endectocide.It is 46 days, because we assume 3 days in sick group.
        $res = $GLOBALS['pdo']->query("
   SELECT id FROM bovinemanagement.bovinecurr WHERE bovinecurr.id={$record['bovine_id']} AND (fresh_date is not null OR (bovinemanagement.due_date({$record['bovine_id']}) - interval '46 days') <= now()  )
  ");
        if ($res->rowCount() >= 1) {
            return 'Cydectin Only.';
        } else {
            return '';
        }
    }

   

}

//end class

/*
 * dewrom
 * 
 */

Class QF2_DeWormAnimal extends QF2_Base {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastFewDays();

        $location = $this->listActiveLocationsAndCounts();

        //grab a list of endectocides'.
        $medArray[null] = "Please Select one..."; //dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE medicine_class='endectocide'");
        while (($row = $res->fetch())) {
            $medArray[$row[0]] = $row[1];
        }

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 

        $doasgeArr = array(null => 'Please Select One', 10 => '10', 15 => '15', 20 => '20', 25 => '25', 30 => '30', 35 => '35', 40 => '40', 45 => '45', 50 => '50', 55 => '55', 60 => '60', 65 => '65', 70 => '70', 75 => '75', 80 => '80');


        $form->addElement('select', 'medicine', null, array('options' => $medArray, 'label' => 'Endectocide'))->addRule("required", "Please specify your Endectocide");
        $form->addElement('select', 'dosage', null, array('options' => $doasgeArr, 'label' => 'Dosage (ml)'))->addRule("required", "Please specify your dosage");
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");


        //defaults
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
                    'animalNumber' => 0,
                    'dosage' => 50
        )));



        return $form; //required   
    }

    

    function process($value) {

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

            $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);


            
             $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);


            
            /*
              INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid)
              VALUES ($bovine_id,$medicine_id,'$event_time','$dosage','$default_dosage_unit','$location',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}')";
             */

     
                //group
                foreach ($bovineIdArr as $k => $v) {

                $query = "/*SINGLE ANIMAL*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ({$value['animalSelected']},'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                $res = $GLOBALS['pdo']->exec($query);
            }


            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
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


/*
 * 
 * DEHORNING
 * 
 */

Class QF2_Dehorn extends QF2_Base {

    function definition($form) {

        
        $note=(new BootStrap)->infoCallout('Metacam and Lidocaine/Rompun are entered automatically with this form.');
        $form->addElement('static','gggg',null, array('content' => $note));
        
        
        //create list of $dehorn events. DB does check.
        $dehornEventsArray = array(null => 'Please Select One', 'burned' => 'burned', 'gouged' => 'gouged', 'sawed' => 'sawed', 'redo' => 'redo', 'polled' => 'polled');


        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 

       
        $form->addElement('select', 'dehornEventType', null, array('options' => $dehornEventsArray, 'label' => 'Type of Dehorn Event'))->addRule("required", "Please specify your type of dehorn state change.");
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of dehorn event");

        //default
          $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('dehornEventType' => 'burned'))); //set defaults
 
        
        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);


        $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);


        try {
            $res = $GLOBALS['pdo']->beginTransaction();



            foreach ($bovineIdArr as $key => $bovineId) {

                
               
                
                //don't put id value, uses next one in DB.
                $query = "INSERT INTO bovinemanagement.dehorn_event (bovine_id,event_time,dehorn_event_type,userid) VALUES ({$bovineId},'{$value['event_time']}','{$value['dehornEventType']}','{$GLOBALS['auth']->getUsername()}')";

                $res = $GLOBALS['pdo']->exec($query);
                
                 //give shot of lidocaine/rompun and metacam too automtically. 
                $medicineID=79;
                $dosage=1.5;
                $query2="INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid) VALUES ($bovineId,$medicineID,'{$value['event_time']}','$dosage','ml','intramuscular',null,null,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query2);
                
                $medicineID=99;
                $dosage=2;
                $query3="INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid) VALUES ($bovineId,$medicineID,'{$value['event_time']}','$dosage','ml','intramuscular',null,null,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query3);
                
                
                
            }
            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

    

}

//end class

/*
 * 
 * Disease Long Term
 * 
 */
Class QF2_DiseaseLongTerm extends QF2_Base {

    function definition($form) {

        //create list of $dehorn events. DB does check.
        $diseaseTypesArray = array(null => 'Please Select One', 'leukosis' => 'Leukosis', 'johnes' => 'Johne\'s');
        $diseaseStatusArray = array(null => 'Please Select One', 'true' => 'True', 'false' => 'False');


        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtMidnight();

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
          if ($this->mode == 'group') {
            $form=(new Quickform2_GroupSelect())->definitionAnimalGroup($form);
        }
        else {
            $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form);
        }


        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form->addElement('select', 'disease', null, array('options' => $diseaseTypesArray, 'label' => 'Type of Disease'))->addRule("required", "Please specify your type of disease.");
        $form->addElement('select', 'status', null, array('options' => $diseaseStatusArray, 'label' => 'Status of Disease'))->addRule("required", "Please specify your type of disease state change.");
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of disease event");

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);


        $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);


        try {
            $res = $GLOBALS['pdo']->beginTransaction();



            foreach ($bovineIdArr as $key => $bovineId) {

                //don't put id value, uses next one in DB.
                $query = "INSERT INTO bovinemanagement.medical_disease (bovine_id,event_time,disease,status,userid) VALUES ({$bovineId},'{$value['event_time']}','{$value['disease']}',{$value['status']},'{$GLOBALS['auth']->getUsername()}')";

                $res = $GLOBALS['pdo']->exec($query);
            }
            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

   

        
}

//end class
?>