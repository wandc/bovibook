<?php




;
/*
  //line of sql to debug this page if necessary.
  SELECT local_number,location_name,date_trunc('days',now()-fresh_date) as dim FROM bovinecurr WHERE date_trunc('days',now()-fresh_date) is not null ORDER by dim DESC
 */

class TransitionDryoff extends BasePage {

    public function defaultDisplay() {
        print("<div class='row'>");
       
        print("<div class='col-sm-4 col-sm-push-8'>");
        $this->dryoffEventQuickForm();
        $this->markCowToBeDriedOffQuickForm();
        print("</div>");
         print("<div class='col-sm-8 col-sm-pull-4'>");
        $this->displayListOfDryOffs();
        print("</div>");
        print("</div>");
    }

    public function milkTestInputsDisplay() {
        $this->displayListOfDryOffs('milkTestInputs');
    }

    /*     * ******************************* */
    /* Display list of all dryoff cases */
    /*     * ******************************* */

    function displayListOfDryOffs() {
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
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "userid",
                title: "Employee"
            },        
            {
                data: "action",
                title: "Action"
            }
EX;
        print($dt->start(44, 'AllDryOffEvents', null, $columnsStr));
    }

    public function AllDryOffEvents($mode = 'default') {

        $sql = "SELECT bovine.id as bovine_id,dryoff_event.lactation_id,event_time,local_number,full_name,dryoff_event.userid,bovinemanagement.due_date(bovine.id)
                    FROM bovinemanagement.dryoff_event 
LEFT JOIN bovinemanagement.lactation ON lactation.id = dryoff_event.lactation_id
LEFT JOIN bovinemanagement.bovine ON lactation.bovine_id = bovine.id
ORDER BY dryoff_event.event_time DESC";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] = date("l M d, Y", strtotime($row['event_time']));
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['due_date'] = date("l M d, Y", strtotime($row['due_date']));
            $row['userid'] = $row['userid'];
            $row['action'] = $this->printActionButtons($row['lactation_id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function dryoffEventQuickForm() {

        /*         * ******************************* */
        /* dryoff Event Form */
        /*         * ******************************* */

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //hourly array, with unix timestamp as index.
        //grab a list of cows with fresh dates
        $sql = "SELECT id,bovinecurr.fresh_date,bovinecurr.local_number,bovinecurr.full_name, date_trunc('day',current_date-bovinecurr.fresh_date) as dim FROM bovinemanagement.bovinecurr WHERE fresh_date is not null ORDER by local_number";
        $res = $GLOBALS['pdo']->query($sql);
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row['id']] = $row['local_number'] . " - " . $GLOBALS['MiscObj']->femaleShortName($row['full_name'])  . " (" . $row['dim'] . ")";
        }

        $form = new HTML_QuickForm("dryoffQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickformtest', "Dry Cow Off"); //set title
        $form->addElement('select', 'animalNumber', 'Cow Animal Number', $aniNumber);
        $form->addElement('checkbox', 'orbaseal4teats', 'Treated with Orbaseal (all 4 teats)', null, null);
        $form->addElement('checkbox', 'cefidry4teats', 'Treated with Novodry Plus (all 4 teats)', null, null);
        $form->addElement('select', 'dateEvent', 'Estimated Time of Final Milking and Treatement', $dateArray);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        $form->setDefaults(array('orbaseal4teats' => 1));
        $form->setDefaults(array('cefidry4teats' => 1));


        //rules
        $form->addRule('animalNumber', 'Animal Number is required', 'required');
        $form->addRule('orbaseal4teats', 'Treatment is required.', 'required');
        $form->addRule('cefidry4teats', 'Treatment is required.', 'required');
        $form->addRule('dateEvent', 'Date of calving is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $bovine_id = $form->exportValue('animalNumber');

            $dateEvent1Array = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:s', $dateEvent1Array[0]);  //traditional
            ///////////////////////////////////////
            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //do an insert into lactation with dry off date.
                //get lactation id.
                $sql = "SELECT lactation_id FROM bovinemanagement.lactationcurr WHERE bovine_id=$bovine_id LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);
                $lactation_id = $row2['lactation_id'];


                //do an insert into medicine adminsterd for orbaseal and novodry
                $retArrayA = $this->treatTeats($bovine_id, 10, $event_time); //orbeseal is 10
                $retArrayB = $this->treatTeats($bovine_id, 42, $event_time); //novodry is 42.
                //do an insert into dryoff_event
                $query = "INSERT INTO bovinemanagement.dryoff_event (event_time,lactation_id,med_administered_a1,med_administered_a2,med_administered_a3,med_administered_a4,med_administered_b1,med_administered_b2,med_administered_b3,med_administered_b4,userid)
                                 VALUES ('$event_time',$lactation_id,{$retArrayA['front left']},{$retArrayA['front right']},{$retArrayA['rear left']},{$retArrayA['rear right']},{$retArrayB['front left']},{$retArrayB['front right']},{$retArrayB['rear left']},{$retArrayB['rear right']},'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);

                //update lactation id with info.        
                $query = "UPDATE bovinemanagement.lactation SET dry_date='$event_time' WHERE id=$lactation_id";
                $res = $GLOBALS['pdo']->exec($query);

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }

            // Close the transaction
            //////////////////////////////////////////


            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
        } //end validation
        else {
            $form->accept($renderer);
            echo $renderer->toHtml();
        }
    }

    function treatTeats($bovine_id, $medicine_id, $event_time) {
//do all four quarters
        $quarterArray['front left'] = 1;
        $quarterArray['front right'] = 1;
        $quarterArray['rear left'] = 1;
        $quarterArray['rear right'] = 1;


        foreach ($quarterArray as $key => $value) {
            if ($value == 1) { //only do if the value is true
                $locationStr = "$key" . ' teat canal'; //make string from array key value
                //find default dosage
                $sql = "SELECT default_dosage,default_dosage_unit FROM bovinemanagement.medicine WHERE id=$medicine_id LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);

                //administer drug
                $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,medical_case_id,userid)
                                 VALUES ($bovine_id,$medicine_id,'$event_time','{$row2['default_dosage']}','{$row2['default_dosage_unit']}','$locationStr',null,null,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query);
                $retArray[$key] = $GLOBALS['pdo']->lastinsertId('bovinemanagement.medical_generic_id_seq');
            }
        }

        return $retArray;
    }

    function deleteQuickForm($lactationID, $callBackURL) {

        $form = new HTML_QuickForm("dryoffEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'lactationID', $lactationID);
        $form->addElement('submit', 'btnDelete', 'Delete');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $lactationID = $form->getElementValue('lactationID');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //look up what all the med administered ids are and then delete each of them.
                $res = $GLOBALS['pdo']->query("SELECT med_administered_a1,med_administered_a2,med_administered_a3,med_administered_a4,med_administered_b1,med_administered_b2,med_administered_b3,med_administered_b4 FROM bovinemanagement.dryoff_event WHERE lactation_id=$lactationID LIMIT 1");

                $row = $res->fetch(PDO::FETCH_ASSOC);

                //delete the dryoff_event
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.dryoff_event where lactation_id='$lactationID'");

                //delete each med administered
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a1']}'");
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a2']}'");
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a3']}'");
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_a4']}'");
                //
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b1']}'");
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b2']}'");
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b3']}'");
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.medicine_administered where id='{$row['med_administered_b4']}'");

                //update the lactation to remove dry date.
                $query = "UPDATE bovinemanagement.lactation SET dry_date= NULL WHERE id=$lactationID";
                $res = $GLOBALS['pdo']->exec($query);


                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }


            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function printActionButtons($lactation_id, $callBackURL = null) {
        $returnButtonsAsString = '';
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        //print latest delete button (one) for current user
        $res = $GLOBALS['pdo']->query("SELECT lactation_id FROM bovinemanagement.dryoff_event WHERE userid='$currentUserid' ORDER BY event_time DESC LIMIT 1");

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($lactation_id == $row['lactation_id']) {
                $returnButtonsAsString = self::deleteQuickForm($lactation_id, $callBackURL); //show delete button, with Breeding table id as index for delete.
            }
        }

        return $returnButtonsAsString;
    }

////////////////////////////////////////////////////////////////////////
    /**
     * MARK COW TO BE DRIED OFF
     * 
     */
    function markCowToBeDriedOffQuickForm($mode = 'group', $bovine_id = null) {

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForNexttDay(); //hourly array, with unix timestamp as index.
        //grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovine WHERE death_date IS NULL ORDER BY local_number");

        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
        }


        $form = new HTML_QuickForm("markCowToBeDriedOffQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Mark Cow to be Dried Off');

        if ($mode == 'group') {
            $form->addElement('select', 'bovine_id', 'Animal Number', $aniNumber);
        } else {
            $form->addElement('hidden', 'bovine_id', $bovine_id);
        }

        $form->addElement('select', 'DateEstimateDryOff', 'Time of Scheduled Dry Off', $dateArray);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        if ($mode == 'group') {
            $form->setDefaults(array('bovine_id' => 0)); //set default to no variety.
        }
        $form->setDefaults(array('DateEstimateDryOff' => 0));

        //rules
        $form->addRule('bovine_id', 'Animal number is required', 'required');
        $form->addRule('DateEstimateDryOff', 'Date of dry off is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('bovine_id');
            $tempArray = $form->getElementValue('DateEstimateDryOff');
            $DateEstimateDryOff = $tempArray[0];
            $estimated_time = date('Y-m-d H:i:sO', $DateEstimateDryOff);
            $pageid = $form->exportValue('pageid');

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.dry_off_event (bovine_id,estimated_breeding_time,estimated_breeding_userid) VALUES ($bovine_id,'$estimated_time','{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);

            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation
        //displayLastbreedingEvents();
        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    //shows error.
    function error() {

        ////////////////////////
        //looks if cow is 60 days from calving and not dried off.
        $sql = "SELECT bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name, calculated_potential_due_date,bovinecurr.location_id, location_name FROM bovinemanagement.bovinecurr LEFT JOIN bovinemanagement.pregnant_view on bovinecurr.id=pregnant_view.id WHERE calculated_potential_due_date < (current_date + interval '60 days') AND fresh_date is not null and dry_date is null ORDER BY calculated_potential_due_date DESC
	     ";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error["{$row['local_number']}"] = new notifyObj(get_class($this), $row['calculated_potential_due_date'], 1, $row['local_number'] . ' less than 60 days to calving, needs dried off.');
        }
        if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
    }

}

//end class
?>