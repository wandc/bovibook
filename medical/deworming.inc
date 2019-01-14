<?php 





include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');

//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if (class_exists('DataGridLR_Ext') != true) { //FFDA7F
    //extends datgrid class with specific logic to colour the line.

    class DataGridLR_Ext extends DataGridLR {

        function colourLineCriteria($row) {
            if ($row['event_time'] == false) {
                return true;
            } else {
                return false;
            }
        }

    }

}

/** * *************************************
 *  Used to input when a cow was dewormed and who needs dewormed.
 * ****************************************** */
class Deworming extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Administered';
        $tabArray[2]['name'] = 'Needed List';
        $tabArray[3]['name'] = 'DeHorn';
        $this->renderTabs($tabArray);
    }

    function tab1() {
        
  
        
        print('<div id="twoColumnLayout">');
        
        //  $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE medicine_class='endectocide'");
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');    
        $paramMed= new stdClass();
        $b=new QF2_GeneralTreatment((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group',null); //group
        print($b->renderBox('qf2_GeneralTreatment','Select Animals / Groups to Treat'));
        print($b->formJS());
        
        
        
        $this->displayListOfDewormersAdministered();
        print('</div>');
    }

    function tab2() {
        print("<h3>Not Dewormed in the Last 280 Days</h3>");
        $this->displayListNeedingDeworming();
    }

    function tab3() {
        print("<h3>Dehorning....</h3>");
         $paramMed= new stdClass();
        $b=new QF2_Dehorn((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group',null); //group
        print($b->renderBox('qf2_Dehorn','Select Animals / Groups to Treat'));
        print($b->formJS());

    }


   

    /**     * ****************************************** */
    /** Display list of all dewromers Administered   */

    /**     * ****************************************** */
    function displayListOfDewormersAdministered($mode = 'default') {
        $numberPerPage = 25;

        $sql = "SELECT bovine_id as bovine_id,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time,medicine_administered.id as medicine_administered_id
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
  WHERE medicine.medicine_class='endectocide'
  ORDER BY medicine_administered.event_time DESC,trade_name,local_number";

        $dg = new DataGridLR($sql, $numberPerPage);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('trade name', 'trade_name', 'trade_name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('dosage', 'dosage', 'dosage', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('dosage unit', 'dosage_unit', 'dosage_unit', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('location', 'location', 'location', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('note', 'note', 'note', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('userid', 'userid', 'userid', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Event Time', 'event_time', 'event_time', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('Deworming', 'printActionButtons()')));

        print("<h3>Completed Dewormed</h3>");
        print($dg->render('datagrid'));
        if ($mode == 'default') {
            $dg->pager();
        }
    }

    /**     * ****************************************** */
    /** Display list of all dewormers Needed   */

    /**     * ****************************************** */
    function displayListNeedingDeworming($mode = 'default') {

        $numberPerPage = 1000;

        $sql = "
   SELECT id,id as bovine_id,local_number,full_name,location_name,(SELECT max(event_time) from bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index where medicine.medicine_class='endectocide'  and bovine_id=bovinecurr.id ) as event_time 
   FROM bovinemanagement.bovinecurr WHERE bovinecurr.birth_date <= (now()- interval '6 months')
   EXCEPT
   SELECT bovinecurr.id,bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name,bovinecurr.location_name,(SELECT max(event_time) from bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index where medicine.medicine_class='endectocide' and bovine_id=bovinecurr.id ) as event_time  
   FROM bovinemanagement.medicine_administered
   LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id=bovinecurr.id
   LEFT JOIN bovinemanagement.medicine ON medicine.id = medicine_administered.medicine_index
   WHERE medicine.medicine_class='endectocide' AND medicine_administered.event_time >= (now() - interval '280 days')
   ORDER BY location_name,local_number DESC
   ";

        $dg = new DataGridLR_Ext($sql, $numberPerPage);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Current Location', 'location_name', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Drug To Use', '', '', array('' => ''), null, array('Deworming', 'printDrugToUse()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Last Administered', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedTime()')));
        
        if ($mode == 'default') {
            print($dg->render('datagrid', true, $GLOBALS['config']['COLOUR']['important']));     //color rows way overdue for deworming
            $dg->pager();
        }
    }

    /**
     * if the cow could be pregnant we can only use certain vaccines.
     */
    function printDrugToUse($params) {
        extract($params);
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
    
     function printActionButtons($params) {
        extract($params);
        $currentUserid = $GLOBALS ['auth']->getUsername();


        $returnButtonsAsString = null;
if ((in_array('owner',$GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin',$GLOBALS['auth']->getAuthData('groups'))) == TRUE) {
            $returnButtonsAsString = MedicalCase::deleteQuickForm($record ['medicine_administered_id'], 'medicine_administered'); // show
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

        $dateArray = Misc::createDailyDatesForLastFewDays();
      
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

            $transaction_id = Misc::generatePseudoRandomTransactionInteger();

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


        $dateArray = Misc::createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $aniNumber = Misc::createListOfAllAliveCows();

        
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