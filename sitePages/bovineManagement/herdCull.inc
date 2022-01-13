<?php

/** records an actual cull from the herd and why */
class HerdCull extends BasePage {

    public function defaultDisplay() {



        $a = new QF2_HerdCullAnimal((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');

        $left = <<<HTML
            {$this->plotCullBarGraph()}
            {$this->displayCowsWhoHaveBeenCulled('default', $this->pageid)}
        HTML;

        $right = <<<HTML
            {$a->renderBox('QF2_HerdCullAnimal', 'Mark Animal(s) Culled')}
        HTML;

        $content = (new Bootstrap)->oneThirdTwoThirds($left, $right);
        print($content);
    }

    /**
     * plots a bar graph of who died, etc.
     */
    public function plotCullBarGraph() {

        $arrayDied = $this->findMonthlyTotals('Died');
        $arrayBeefed = $this->findMonthlyTotals('Beefed');
        $arraySold = $this->findMonthlyTotals('Sold');


        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Died'] = 'number';
        $headerStrArr['Beefed'] = 'number';
        $headerStrArr['Sold'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $arrayDied, $arrayBeefed, $arraySold);
        return ($x->toString());
    }

    private function findMonthlyTotals($reason) {

        /* find all the cows who died (or whatever) in the last year. */
        $sql = "
  With temp as (
Select bovine_id,date_trunc('month',event_time) as month,1 as amount FROM bovinemanagement.cull_event
WHERE reason='$reason' AND event_time between date_trunc('month',current_date) - interval '1 year'  AND date_trunc('month',current_date)  + interval '1 month'
)
SELECT 
DISTINCT(month),sum(amount) OVER(PARTITION BY month) as total 
FROM temp ORDER BY month
";
        $arrayReason = null;

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $arrayReason[strtotime($row['month'])] = $row['total'];
        }



        return $arrayReason;
    }

    function displayCowsWhoHaveBeenCulled($mode = 'default', $pageid) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
{
                data: "event_time",
                title: "Date",
                type: "date"
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"       
            },
            {
                data: "reg_num",
                title: "Reg #"
            },
            {
                data: "reason",
                title: "Reason"
            },
            {
                data: "details",
                title: "Details"
            },
            {
                data: "truck_driver",
                title: "Driver"
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
        return ($dt->start('Aniamls Who Have Been Culled', $pageid, 'cowsWhoHaveBeenCulled', $mode, $columnsStr));
    }

    public function cowsWhoHaveBeenCulled($request) {
        if (!empty($request['item'])) {



            if ($request['item'] == 'default') {
                // Setup your database connection
                $sql = "	     
SELECT bovine.*,cull_event.*,calving_event.calf_breed as breed,cull_event.id as id FROM bovineManagement.cull_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id 
LEFT JOIN bovinemanagement.calving_event ON bovine.rfid_number=calving_event.calf_rfid_number 
ORDER BY cull_event.event_time DESC;
";
            } elseif ($request['item'] == 'milkTestInputsDisplay') {
                $sql = "
                    SELECT bovine.*,cull_event.*,calving_event.calf_breed as breed,cull_event.id as id FROM bovineManagement.cull_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id 
LEFT JOIN bovinemanagement.calving_event ON bovine.rfid_number=calving_event.calf_rfid_number 
WHERE cull_event.event_time  >= (current_date - interval '10 weeks') 
ORDER BY cull_event.event_time DESC;
";
            } else {
                throw new Exception("ERROR invalid paramater sent with request");
            }
        } else {
            throw new Exception("ERROR no paramater sent with request");
        }


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] = date("M d, Y", strtotime($row['event_time']));
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name'],null,$row['breed']);
            $row['reg_num'] = (new JQueryDataTable)->formattedShortRegNum($row['full_reg_number']);
            $row['truck_driver'] = $row['truck_driver'];
            $row['reason'] = $row['reason'];
            $row['details'] = $row['comment'];

            $row['action'] = $this->printActionButtons($row['id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

//end function
    //show delete button
    function printActionButtons($id, $callBackURL = null) {
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.

        $res = $GLOBALS['pdo']->query("SELECT id FROM bovineManagement.cull_event WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1");
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (($id == $row['id'])) {

                $returnButtonsAsString = self::deleteQuickForm($row['id'], $callBackURL); //show delete button
            }
        }


        return $returnButtonsAsString;
    }

    function deleteQuickForm($tableid, $callBackURL) {

        $form = new HTML_QuickForm("CullEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('submit', 'btnDelete', 'Delete', array('class' => 'btn btn-warning'));


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');


            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();


                //delete movement
                $res = $GLOBALS['pdo']->query("SELECT location_event_id,bovine_id FROM bovineManagement.cull_event WHERE id='$tableid' LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $location_event_id = $row['location_event_id'];
                $bovine_id = $row['bovine_id'];

                //delete the cull event
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.cull_event where id='$tableid'");

                //delete movement
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.location_event where id='$location_event_id'");


                //change the bovine so that the death date is erased.
                $query = "UPDATE bovinemanagement.bovine SET death_date=null WHERE id=$bovine_id";
                $res = $GLOBALS['pdo']->exec($query);


                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }



            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

}

//end class

/*
 * Herd Cull animal form.
 * 
 */
Class QF2_HerdCullAnimal extends QF2_Base {

    function definition($form) {

        //only show if in owner group.
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastSevenDays(); //for admin only.   //$GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon //use if needed.
        } else {
            $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.  
        }



        //list of cull reasons
        $cullReason = array(null => 'Please Select one...', 'Sold' => 'Sold (put who sold to)', 'Beefed' => 'Beefed  (put name of driver)', 'Died' => 'Died  (put cuase of death)');

        //list of drivers
        $truckDriverList = array(null => 'Please Select one...', 'Ron Beckwith' => 'Ron Beckwith', 'Raine Beckwith' => 'Raine Beckwith', 'Jerry Bos' => 'Jerry Bos','Jordan Anderson' => 'Jordan Anderson','Unknown' => 'Unknown');

        if ($this->mode == 'group') {
            $form = (new Quickform2_GroupSelect())->definitionAnimalGroup($form);
        } else {
            $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form);
        }

        $form->addElement('select', 'reason', null, array('options' => $cullReason, 'label' => 'Reason'))->addRule("required", "Please specify your type of cull");
        $form->addElement('textarea', 'details', array('rows' => 6))->setLabel("Comments : ");

        $form->addElement('select', 'driver', null, array('options' => $truckDriverList, 'label' => 'Truck Driver\'s Name'));
        HTML_QuickForm2_Factory::registerRule('error1', 'HTML_QuickForm2_Rule_Callback', null, array('callback' => 'QF2_HerdCullAnimal::checkDriverNameRequired'));
        $form->addRule('error1', 'Truck Driver Name is required!', array('test'));



        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");

        return $form; //required   
    }

    //if animal died skip the driver requirement.
    function checkDriverNameRequired($value) {

        if ((empty($value['driver'])) AND ($value['reason'] != 'Died')) {
            return false;
        } else {
            return true;
        }
    }

    function process($value) {


        $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);

        if (is_array($value['animalSelected'])) {
            //group
            foreach ($value['animalSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $bovine_id = $v;

                //get random transaction id
                $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

                //get location id from reason.
                $location_id = $this->offFarmLcoations($value['reason']);
                // Open a transaction
                try {
                    $res = $GLOBALS['pdo']->beginTransaction();

                    $query = "INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ($bovine_id,'{$value['event_time']}',$location_id,'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                    $res = $GLOBALS['pdo']->exec($query);
                    //get last insertid for location event	
                    $last_location_event_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.location_event_id_seq');

                    //record in cull_event
                    $query = "INSERT INTO bovinemanagement.cull_event (bovine_id,event_time,reason,comment,userid,location_event_id,truck_driver) VALUES ($bovine_id,'{$value['event_time']}','{$value['reason']}','{$value['details']}','{$GLOBALS['auth']->getUsername()}',$last_location_event_id,'{$value['driver']}')";
                    $res = $GLOBALS['pdo']->exec($query);

                    //record death_date in bovine
                    $query = "UPDATE bovinemanagement.bovine SET death_date='{$value['event_time']}' WHERE id=$bovine_id";
                    $res = $GLOBALS['pdo']->exec($query);

                    $GLOBALS['pdo']->commit();
                } catch (Exception $e) {
                    $GLOBALS['pdo']->rollBack();
                    echo "Failed: " . $e->getMessage();
                    error_log($e->getMessage(), 0);
                }
            }
        }
        //individual case
        else {
            //TODO: not implemented.
        }


        // determine if the commit or rollback
    }

    //this is kind of stupid. not really andy difference from harcoding the id numbers. 
    function offFarmLcoations($reason) {
        //safe way to find location_event id.
        switch ($reason) {
            case 'Sold':
                $sql = "SELECT id FROM bovineManagement.location WHERE name='Off Farm - Sold' LIMIT 1";
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $location_id = $row['id'];
                break;
            case 'Beefed':
                $sql = "SELECT id FROM bovineManagement.location WHERE name='Off Farm - Beef' LIMIT 1";
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $location_id = $row['id'];
                break;
            case 'Died':
                $sql = "SELECT id FROM bovineManagement.location WHERE name='Off Farm - Dead' LIMIT 1";
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $location_id = $row['id'];
                break;
            default:
                $location_id = 'null'; //should never be called.
                throw new Exception("Error: reason method input not given to offFarmLcoations method.");
        }
        return $location_id;
    }

}//end class   
