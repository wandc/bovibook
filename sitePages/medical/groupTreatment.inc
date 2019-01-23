<?php 
/** * *************************************
 *  Used to input when a cow was dewormed and who needs dewormed.
 * ****************************************** */
class GroupTreatment extends TabPage { 

    public function defaultDisplay() {
       
        $tabArray[1]['name'] = 'DeWorm';
        $tabArray[2]['name'] = 'DeHorn';
        $this->renderTabs($tabArray);
    }

    function tab1() {
   
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');    
        $paramMed= new stdClass();
        $b=new QF2_GeneralTreatment((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group',null); //group
        
   $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$b->renderBox('qf2_GeneralTreatment','Select Animals / Groups to Treat')}
            {$b->formJS()}
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
        
         $c=new QF2_Dehorn((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group',null); //group
        
           $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$c->renderBox('qf2_Dehorn','Select Animals / Groups to Treat')}
            {$c->formJS()}
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayListOfAnimalsWithNoDehornEvent()}
        </div>
    </div>    
HTML;
    
        print($html);
        
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
   
         $out[]=("<h3>Animals With No DeHorn Events</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
         return implode($out); 
        
    }

   

    /**     * ****************************************** */
    /** Display list of all dewormers Administered   */

    /**     * ****************************************** */
    function displayListOfDewormersAdministered($mode = 'default') {
        
         
         $sql = <<<SQL
SELECT bovine_id as bovine_id,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time,medicine_administered.id as medicine_administered_id
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
 
            
              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
              $rowOut['Which Foot'] = ($row['trade_name']);
              $rowOut['Dosage'] = ($row['dosage']);
              $rowOut['Dosage Unit'] = ($row['dosage_unit']);
              $rowOut['Location'] = ($row['location']);
              //$rowOut['Note'] = ($row['note']);
              $rowOut['Employee'] = ($row['userid']);
              $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
              $rowOut['Action'] = $this->printActionButtons($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Completed Dewormed</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'order: false')); 
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
              $rowOut['colour_row'] =  (($row['completed']) ? 1 : 0);
              
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
    
         $out[]=("<h3>Not Dewormed in the Last 280 Days</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
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
    
     function printActionButtons($record,$callBackURL) {

        $currentUserid = $GLOBALS ['auth']->getUsername();


        $returnButtonsAsString = null;
if ((in_array('owner',$GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin',$GLOBALS['auth']->getAuthData('groups'))) == TRUE) {
            $returnButtonsAsString = MedicalCase::deleteQuickForm($record ['medicine_administered_id'], 'medicine_administered',$callBackURL); // show
            //delete button, with _____ table id as index for delete.
}

        return $returnButtonsAsString;
    }

}//end class

/*
 * dewrom
 * 
 */
Class QF2_DeWormAnimal extends Quickform2Helper {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastFewDays();
      
        $location = $this->listActiveLocationsAndCounts();

        //grab a list of endectocides'.
        $medArray[null] = "Please Select one..."; //dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE medicine_class='endectocide'");
        while (($row = $res->fetch())) {
            $medArray[$row[0]] = $row[1];
        }
        
        $combinedArr=parent::currentAniamlsAndGroups();  
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($combinedArr);
        } else {         
                $form->addElement('hidden', 'animalSelected')->setValue( $this->param->bovine_id);
        }
        
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

            $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

            $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);


            /*
            INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid)
	  VALUES ($bovine_id,$medicine_id,'$event_time','$dosage','$default_dosage_unit','$location',null,$medical_case_id,'{$GLOBALS['auth']->getUsername()}')";
            */
            
            if (is_array($value['animalSelected'])) {
                //group
                foreach ($value['animalSelected'] as $k => $v) {


                    // GROUP (LOCATION) MOVE
                    if (strstr($v, 'GROUP_') != false) { //when group or pen of animals
                        //remove the 'GROUP_' to find location_id.
                        $integerLocationID = preg_replace('/^GROUP_/', '', $v);
                        if (filter_var($integerLocationID, FILTER_VALIDATE_INT)) {
                            $locationID = $integerLocationID;
                        } else {
                            throw new Exception("Error: bad data, location id not an integer");
                        }

                        //now look up which cows are in that location and we will loop through them. this is inside a transaction, so by doing this here we make it atomic. 
                        $sql2 = "SELECT bovine_id From bovinemanagement.locationcurr WHERE location_id=$integerLocationID";
                        $res2 = $GLOBALS['pdo']->query($sql2);
                        while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {

                            $query = "/*GROUP*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ({$row2['bovine_id']},'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                            $res = $GLOBALS['pdo']->exec($query);
                        }
                        //NORMAL BUNCH OF ANIMALS    
                    } else {

                        //now we insert all the animals that were checked.
                        $bovine_id = $v;
                        $query = "/*ANIMAL*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ($bovine_id,'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                        $res = $GLOBALS['pdo']->exec($query);
                    }
                }
            }
            //individual case
            else {
                $query = "/*SINGLE ANIMAL*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ({$value['animalSelected']},'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
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


/*
 * 
 * DEHORNING
 * 
 */
Class QF2_Dehorn extends Quickform2Helper {

    function definition($form) {

        //create list of $dehorn events. DB does check.
        $dehornEventsArray = array(null => 'Please Select One', 'burned' => 'burned', 'gouged' => 'gouged', 'sawed' => 'sawed', 'redo' => 'redo');


        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveCows();

        
        $combinedArr=parent::currentAniamlsAndGroups();  
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEventsDehorn'))->loadOptions($combinedArr);
        } else {         
                $form->addElement('hidden', 'animalSelected')->setValue( $this->param->bovine_id);
        }
        

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);       
        $form->addElement('select', 'dehornEventType', null, array('options' => $dehornEventsArray, 'label' => 'Type of Dehorn Event'))->addRule("required", "Please specify your type of $dehorn state change.");
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of $dehorn event");

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);

        
        $bovineIdArr=parent::processAnimalsGroupIndividiual($value['animalSelected']);
        

        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            
            
          foreach ($bovineIdArr as $key => $bovineId) {
              
              //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.dehorn_event (bovine_id,event_time,dehorn_event_type,userid) VALUES ({$bovineId},'{$value['event_time']}','{$value['dehornEventType']}','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);
                           
          }
            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
    }

     //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEventsDehorn").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
}

//end class
?>