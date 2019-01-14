<?php 
class EstrusHormone extends BasePage {

    public function defaultDisplay() {
        print("<div class='row'>"."\n\r");
        
        print("<div class='col-sm-4 col-sm-push-8'>"."\n\r");
       $this->hormoneQuickForm();
        print("</div>"."\n\r");
        print("<div class='col-sm-8 col-sm-pull-4'>"."\n\r");
       $this->displayListOfHormoneInjections();
        print("</div>"."\n\r");
        print("</div>"."\n\r");  
    }

    /*     * ******************************* */
    /* Display list of all medical cases */
    /*     * ******************************* */

    function displayListOfCases($mode = 'default') {
        
    }

    function hormoneQuickForm($mode = 'group', $bovine_id = null) {

        //grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovinecurr ORDER BY local_number");

        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . Misc::femaleShortName($row[1]);
        }


        //grab a list of hormones that take 2ml as default doage, this elimiates oxytosin and cidrs from the list.
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit from bovinemanagement.medicine WHERE medicine_class='hormone' AND default_dosage=2");

        while (($row = $res->fetch())) {
            $medArray[$row[0]] = $row[1];
        }

        $timeArray = Misc::createHourlyDatesForLastDay();
        unset($timeArray[null]); //remove null value.
        //the action=request_uri is the key for this to work with callbacks and tabs. 
        $form = new HTML_QuickForm("hormoneQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hormoneQuickForm', "Repro Hormone Quick Inject");

        //code to turn off fields if we are accessing this from the bovine query page.
        if ($mode == 'group') {
            $form->addElement('select', 'bovine_id', 'Animal', $aniNumber);
        } else {
            $form->addElement('hidden', 'bovine_id', $bovine_id);
        }

        $form->addElement('select', 'medicine', 'Hormone Drug', $medArray);
        $form->addElement('select', 'event_time', 'Event Time', $timeArray);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');




// Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('bovine_id');
            $medicine_id = $form->exportValue('medicine');
            //
            $tempArray = $form->getElementValue('event_time');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);

            $pageid = $form->exportValue('pageid');



            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            $query = "INSERT INTO bovinemanagement.medicine_administered 
				(bovine_id,medicine_index,dosage,dosage_unit,location,userid,event_time,medical_case_id)
		                VALUES ($bovine_id,$medicine_id,'2','ml','intramuscular','{$GLOBALS['auth']->getUsername()}','$event_time',NULL)";
            $res = $GLOBALS['pdo']->exec($query);

            // determine if the commit or rollback
        
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    /*     * ******************************* */
    /* Display list of all medical cases */
    /*     * ******************************* */

    function displayListOfHormoneInjections($mode = 'default') {


        // Setup your database connection

        $sql = "SELECT bovinecurr.id as bovine_id,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time
		FROM bovinemanagement.medicine_administered 
		LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
		LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
		WHERE medicine.medicine_class='hormone' AND event_time >= (current_date - interval '1 year')
		ORDER BY medicine_administered.event_time DESC";
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
             $row['event_time'] = date("l M d H:00", strtotime($row['event_time']));  //has to be at end.         
             unset($row['bovine_id']);
               unset($row['full_name']);
        }
        //output
        $dataArr = $results;
      
         print( (new JQueryDataTable)->startBasicArray($dataArr));
     
    }

}//end class