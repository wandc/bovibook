<?php

class EstrusHormone extends BasePage {

    public function defaultDisplay() {
  
            $this->param = new stdClass();
            $this->param->bovineID = null;
            $this->param->aniNumber = null;
         $a = new QF2_HormoneInjection((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->param->bovineID)), 'list', $this->param); //group

        $left = <<<HTML
            {$this->displayListOfHormoneInjections()}
        HTML;

        $right = <<<HTML
            {$a->renderBox('QF2_HormoneInjection', 'Repro Hormone Injection')}
        HTML;

        $content = (new Bootstrap)->oneThirdTwoThirds($left, $right);
        print($content);
    }

   
   
    /*     * ******************************* */
    /* Display list of all medical cases */
    /*     * ******************************* */

    function displayListOfHormoneInjections($mode = 'default') {


        // Setup your database connection

        $sql = "SELECT bovinecurr.id as bovine_id,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time,medicine_administered.id as medicine_administered_id
		FROM bovinemanagement.medicine_administered 
		LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
		LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
		WHERE medicine.medicine_class='hormone' AND event_time >= (current_date - interval '1 year')
		ORDER BY medicine_administered.event_time DESC";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $delButton=new QF2_DeleteButtonDecider('user_timeLimit',(object) array('dateTimePeriodDays' =>'1','schemaTable'=>'bovinemanagement.medicine_administered'));
        
        //change presentation
        foreach ($results as &$row) {
             
         
            
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Date'] = date("l M d Y, H:00", strtotime($row['event_time']));      
            $rowOut['Medicine'] = $row['trade_name'];
            $rowOut['Employee'] = $row['userid'];
            $rowOut['action']=  $delButton->output($row['medicine_administered_id']);     
      
            $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;

        return ( (new JQueryDataTable)->startBasicArrayOpt('Hormone Log', $dataArr));
    }

} //end class

Class QF2_HormoneInjection extends QF2_Base {

    function definition($form) {

        // grab a list of action types'.
        $hormoneArr [null] = "Please Select one..."; // dummy value
         //grab a list of hormones that take 2ml as default doage, this elimiates oxytosin and cidrs from the list.
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit from bovinemanagement.medicine WHERE medicine_class='hormone' AND default_dosage=2");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $hormoneArr[$row['id']] = $row['trade_name'];
        }

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); 
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");
        $form->addElement('select', 'hormone', null, array('options' => $hormoneArr, 'label' => 'Hormone Drug'))->addRule("required", "Hormone Drug is required.");
        
        return $form; //required       
    }

    function process($value) {
         
        
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);
       
        
         //lookup bovine id and see if there is an open medical case during the requested event_time
            $medical_case_id = MedicalCase::lookupOpenMedicalCaseForAnimal($value['bovine_id'], $value['event_time']);
            $medical_case_id=empty($medical_case_id) ? 'null' : $medical_case_id;

            $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,dosage,dosage_unit,location,userid,event_time,medical_case_id) VALUES ({$value['bovine_id']},{$value['hormone']},'2','ml','intramuscular','{$GLOBALS['auth']->getUsername()}','{$value['event_time']}',$medical_case_id)";
            
            $res = $GLOBALS['pdo']->exec($query);
    }

}//end class
