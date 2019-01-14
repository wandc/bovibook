<?php





include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/holsteinCanadaERAReg.inc');
;

class CalfRegistration extends BasePage {

    public function defaultDisplay() {
        $this->forceAggregateUopdate(); //can be slooooow.
        $this->updateCowNameAndSireFromAggregate();
        $this->checkAggregateVsBovineTable();
        $this->checkForAnimalsOver120DaysWithNoName();


        //
        $this->doUpdateOfDBtoMatchAggreagate();
        print("<h2>To Register List</h2>");
        $this->displayCalvesNeedingRegisterdOrRecentlyRegistered();
        print("<br /><br /><br />");
        print("<h2>Log</h2>");
        $this->displayRegistrationLog();
    }

    //check for errors between aggregate and bovine table and prints them out.
    function checkAggregateVsBovineTable() {
        $sql = "SELECT bovine.local_number,
 bovine.full_reg_number as bo_full_reg_number, bovine.full_name as bo_full_name, bovine.birth_date as bo_birth_date, bovine.sire_full_reg_number as bo_sire_full_reg_number, bovine.dam_full_reg_number  as bo_dam_full_reg_number,
aggregate_view_curr.full_reg_number as agg_full_reg_number, aggregate_view_curr.full_name as agg_full_name, aggregate_view_curr.birth_date as agg_birth_date, aggregate_view_curr.sire_full_reg_number as agg_sire_full_reg_number, aggregate_view_curr.dam_full_reg_number  as agg_dam_full_reg_number,event_time_reg_sent
    FROM bovinemanagement.bovine LEFT JOIN batch.aggregate_view_curr on bovine.full_reg_number = aggregate_view_curr.full_reg_number 
LEFT JOIN batch.holstein_canada_registered ON holstein_canada_registered.bovine_id=bovine.id
WHERE bovine.sire_full_reg_number !='11111111111111111' AND death_date IS NULL AND aggregate_view_curr.authentic=true ORDER BY id
";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            if ($row['bo_full_name'] != $row['agg_full_name']) {
                print("<br/>{$row['local_number']} name problem: (local) {$row['bo_full_name']} vs. (HOL) {$row['agg_full_name']}<br/>");
            }
            if ($row['bo_birth_date'] != $row['agg_birth_date']) {
                print("{$row['local_number']} birth date problem: (local) {$row['bo_birth_date']} vs. (HOL)  {$row['agg_birth_date']}<br/>");
            }
            if ($row['bo_sire_full_reg_number'] != $row['agg_sire_full_reg_number']) {
                print("{$row['local_number']} sire reg number problem: (local) {$row['bo_sire_full_reg_number']} vs. (HOL)  {$row['agg_sire_full_reg_number']}<br/>");
            }
            if ($row['bo_dam_full_reg_number'] != $row['agg_dam_full_reg_number']) {
                print("{$row['local_number']} dam reg number problem: (local) {$row['bo_dam_full_reg_number']} vs. (HOL)  {$row['agg_dam_full_reg_number']}<br/>");
            }
        }
    }

    // this is for when a registration has been sumitted, and all looks ok, but holstein Canada hasn't updated their site in over 30 days.
    function checkForAnimalsOver120DaysWithNoName() {
        $sql = "SELECT bovine.local_number,
 bovine.full_reg_number as bo_full_reg_number, bovine.full_name as bo_full_name, bovine.birth_date as bo_birth_date, bovine.sire_full_reg_number as bo_sire_full_reg_number, bovine.dam_full_reg_number  as bo_dam_full_reg_number,
event_time_reg_sent
    FROM bovinemanagement.bovine 
LEFT JOIN batch.holstein_canada_registered ON holstein_canada_registered.bovine_id=bovine.id
WHERE bovine.sire_full_reg_number ='11111111111111111' AND death_date IS NULL AND event_time_reg_sent is not null AND event_time_reg_sent <= current_date - interval '30 days' ORDER BY id";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = date('Y-m-d', strtotime($row['event_time_reg_sent']));
            print("<br/>{$row['local_number']} probably had some kind of registration failure since {$time}.");
        }
    }

    /**
     * this checks bovine table to make sure it agrees with aggregate table for full_name.
     * This only makes a change if the full_name is blank, otherwise it does nothing.
     * 
     */
    private function updateCowNameAndSireFromAggregate() {

        $sql = 'SELECT * FROM bovinemanagement.bovine WHERE full_name is NULL AND sire_full_reg_number =\'11111111111111111\' AND death_date IS NULL ORDER BY id';
        $res = $GLOBALS['pdo']->query($sql);

        $bovineTotalCount = $res->rowCount();
        $bovineLoopCount = 0;
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            //SSE_Message::send_message($bovineLoopCount, 'Doing #' . $row['local_number'] . ' on bovine iteration ' . $bovineLoopCount . ' of ' . $bovineTotalCount, round(($bovineLoopCount / $bovineTotalCount ) * 100, 1)); //sse

            $calfRegNum = $row['full_reg_number'];

            try {

                $sql = "SELECT 	full_name, birth_date,dam_full_reg_number,sire_full_reg_number FROM batch.aggregate_view_curr WHERE full_reg_number ='$calfRegNum' AND authentic = true limit 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                if ($res2->rowCount() == 1) {
                    $agg = $res2->fetch(PDO::FETCH_ASSOC);



                    if (date('Y-m-d', strtotime($agg['birth_date'])) != $row['birth_date']) {
                        throw new Exception($row['local_number'] . ' Aggregate value and stored value do not match for: birthdate.');
                    }

                    if ($agg['dam_full_reg_number'] != $row['dam_full_reg_number']) {
                        throw new Exception($row['local_number'] . " Aggregate value and stored value do not match for: dam reg number, cdn:{$cdn['dam_full_reg_number']} & local:{$row['dam_full_reg_number']}");
                    }

                    //before we do update check that full_name and sire_reg_number make sense.
                    $prefix = strtoupper($GLOBALS['config']['HOLSTEIN_CANADA']['prefix']);
                    if (!(preg_match("/$prefix\s(?<value>\w+)/", $agg['full_name'], $matches))) {
                        throw new Exception($row['local_number'] . " Aggregate value for Full Name is not valid {$agg['full_name']}");
                    }
                    if (empty($agg['sire_full_reg_number'])) {
                        throw new Exception($row['local_number'] . " Aggregate value for Sire Reg Number is blank. Check parentage");
                    }

                    if (!(preg_match("/[A-Z]{2}([A-Z]{3}||840)M[0-9]+/", $agg['sire_full_reg_number'], $matches))) {
                        throw new Exception($row['local_number'] . " Aggregate value for Sire Reg Number is not valid {$agg['sire_full_reg_number']}");
                    }


//do update
                    $sql = "UPDATE bovinemanagement.bovine SET full_name='{$agg['full_name']}', sire_full_reg_number='{$agg['sire_full_reg_number']}' WHERE id={$row['id']}";
                    $res2 = $GLOBALS['pdo']->exec($sql);
                    print("Animal {$row['local_number']} / {$row['full_reg_number']} in bovine Table updated! <br>" . "\n\r" . "\n\r");

//print out animal details
                }

//catch exception here so we skipped update, but can go to next animal without aborting the full run of animals.
            } catch (Exception $e) {
                print("Animal {$row['local_number']} / {$row['full_reg_number']} ignored because of exception: " . $e->getMessage() . "<br/>\n\r");
            }
            $bovineLoopCount++;
        }//end sql loop
        //SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
    }

    private function forceAggregateUopdate() {
        //find animals who registration has been sent, but aggregate does not yet show registered.
        $sql = "SELECT bovine.full_reg_number FROM
batch.holstein_canada_registered 
LEFT JOIN bovinemanagement.bovine on bovine_id=bovine.id
WHERE local_reg_status = false AND event_time_reg_sent is not null";
        $res2 = $GLOBALS['pdo']->query($sql);
        $res2 = $GLOBALS['pdo']->query($sql);
        $regNumArr = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $regNumArr[] = $row2['full_reg_number'];
        }

        if (!empty($regNumArr)) {
            Print("<h2>Page will Load after checking Holstein Canada for new data</h2>");
            ob_flush(); //make this all an ajax call if too slow.
            //force aggregate DB table update.
            include_once($_SERVER['DOCUMENT_ROOT'] . 'phpCronScripts/AggregateSiteData.inc');
            $AggregateSiteDataObj = new AggregateSiteData;
            $AggregateSiteDataObj->forceAggregateUpdate($regNumArr);
        }
    }

    /**
     * check data from web aggregate db file and if it says registered, it changes local_reg_status to true
     * if it says not registered it changes local_reg_status too to false.
     */
    private function doUpdateOfDBtoMatchAggreagate() {
        $sql = "SELECT  bovine.full_reg_number,holstein_canada_registered.*, aggregate_view_curr.reg_status,
aggregate_view_curr.authentic,aggregate_view_curr.update_time as authetic_update_time
FROM batch.holstein_canada_registered
LEFT JOIN bovinemanagement.bovine ON bovine.id=holstein_canada_registered.bovine_id
LEFT JOIN batch.aggregate_view_curr ON bovine.full_reg_number = aggregate_view_curr.full_reg_number";

        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            if ($row2['reg_status'] == 'A' AND $row2['local_reg_status'] == FALSE) {

                //update true
                $sql3 = "UPDATE batch.holstein_canada_registered SET local_reg_status='true' WHERE bovine_id={$row2['bovine_id']}";
                $res3 = $GLOBALS ['pdo']->exec($sql3);
            } elseif ($row2['reg_status'] == NULL AND $row2['authetic_update_time'] == TRUE AND $row2['authentic'] == TRUE) {
                //update false
                $sql3 = "UPDATE batch.holstein_canada_registered SET local_reg_status='false' WHERE bovine_id={$row2['bovine_id']}";
                $res3 = $GLOBALS ['pdo']->exec($sql3);
            }
        }
    }

    function displayCalvesNeedingRegisterdOrRecentlyRegistered() {
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
                data: "potential_name",
                title: "Potential Name"
            },
            {
                data: "since_birthday",
                title: "Since Birthday"
            },
            {
                data: "short_uuid",
                title: "Submitted File Name"
            },
        {
                data: "parentage_verified",
                title: "parentage_verified"
            },
        {
                data: "dna_pending",
                title: "dna_pending"
            },
         {
                data: "local_reg_status",
                title: "Local Reg Status"
            },
        {
                data: "reg_status",
                title: "Hol Can Reg Status"
            },
            {
                data: "action",
                title: "Action"
            }
EX;
        print($dt->start(120, 'calvesNeedingRegisterdOrRecentlyRegistered', null, $columnsStr, 'highlight_important'));
    }

    function calvesNeedingRegisterdOrRecentlyRegistered() {

        $sql = "SELECT xx.birth_date,local_reg_status,parentage_verified,calf_potential_name.id,potential_name,local_number,xx.id as bovine_id,xx.full_name,authentic,
            short_uuid,event_time_reg_sent,date_trunc('days',now()-xx.birth_date) as since_birthday,
            (SELECT dnatest_type FROM bovinemanagement.dnatest_event WHERE bovine_id=xx.id AND event_time=(SELECT max(event_time) FROM bovinemanagement.dnatest_event WHERE bovine_id=xx.id)) as dna_pending,reg_date,	reg_status,geno_test 
           FROM bovinemanagement.bovine xx
                 LEFT JOIN batch.holstein_canada_registered ON xx.id=bovine_id 
                 LEFT JOIN bovinemanagement.calf_potential_name ON xx.id=calf_potential_name.bovine_id 
                 LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = xx.full_reg_number
                 WHERE (local_reg_status=false OR local_reg_status is null) AND death_date is null AND potential_name is not null ORDER BY local_number";



        /*
          $dg = new DataGridLR_Ext($sql, 100);
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Potential Name', 'potential_name', 'potential_name', array('' => ''), null, null));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Since Birthday', 'since_birthday', 'since_birthday', array('' => ''), null, null));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Submitted File name', 'short_uuid', 'short_uuid', array('' => ''), null, null));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('ERA Sent Date', 'event_time_reg_sent', 'event_time_reg_sent', array('' => ''), null, null));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Hol Can Parentage Status', 'parentage_verified', 'parentage_verified', array('' => ''), null, array('DataGridHelper', 'printTrueFalseGenericEmphasis()')));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('DNA Test', 'dna_pending', 'dna_pending', array('' => ''), null, null));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Local Reg Status', 'local_reg_status', 'local_reg_status', array('' => ''), null, null));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Hol Can Reg Status', 'Reg Status', 'Reg Status', array('' => ''), null, array('CalfRegistration', 'printRegStatus()')));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', 'Action', 'Action', array('' => ''), null, array('CalfRegistration', 'printActionButtons()')));
          print($dg->render());
          $dg->pager();

         */
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['birth_date']);
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_reg_sent_new'] = date("r", strtotime($row['event_time_reg_sent']));
            $row['reg_status'] = $this->printRegStatus($row);
            $row['action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            if (empty($row['local_reg_status'])) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayRegistrationLog() {
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
                data: "potential_name",
                title: "Current Location"
            },
            {
                data: "since_birthday",
                title: "since_birthday"
            },
            {
                data: "short_uuid",
                title: "UUID"
            },
            {
                data: "event_time_reg_sent_new",
                title: "Registration Submitted Date",
                type: "date"
            }
EX;
        print($dt->start(120, 'registrationLog', null, $columnsStr, 'highlight_important'));
    }

    function registrationLog() {
        $sql = "SELECT calf_potential_name.id,potential_name,local_number,bovine.id as bovine_id,bovine.full_name,short_uuid,event_time_reg_sent,date_trunc('days',now()-birth_date) as since_birthday
FROM bovinemanagement.bovine 
                 LEFT JOIN batch.holstein_canada_registered ON bovine.id=bovine_id 
                 LEFT JOIN bovinemanagement.calf_potential_name ON bovine.id=calf_potential_name.bovine_id 
                 WHERE short_uuid is not null ORDER BY event_time_reg_sent DESC,local_number";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time_reg_sent']);
            $row['event_time_reg_sent_new'] = date("r", strtotime($row['event_time_reg_sent']));
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    /*     * *****************************************
     * Show registration status message
     * ******************************************* */

    function printRegStatus($row) {


        if (empty($row['event_time_reg_sent'])) {
            $returnButtonsAsString = 'No Registration Sent.';
        } elseif ($row['reg_status'] == 'A') {
            $returnButtonsAsString = 'Registered';
        }
        //when data file is not updated fully, the update time is null because of bovine query left join.
        elseif ((empty($row['authentic'])) AND ( empty($row['update_time']))) {
            $returnButtonsAsString = 'Status Unknown. Update Data.';
        }
        //when authentic is null and update_time has data, it means not in holstein canada system
        elseif (($row['authentic'] != TRUE) AND ( !empty($row['update_time']))) {
            $returnButtonsAsString = 'NOT registered.';
        } else {
            throw new Exception("Error: Registration status is out of spec.");
        }

        return $returnButtonsAsString;
    }

    /*     * *****************************************
     * Show Submit ERA Button.
     * ******************************************* */

    function printActionButtons($row, $callBackURL = null) {

        $returnButtonsAsString = null;
        $currentUserid = $GLOBALS['auth']->getUsername();
        //when it hasn't been sent before, but cow has potential name, show button.
        if (($row['short_uuid'] == false) AND ( $row['event_time_reg_sent'] == false) AND ( $row['potential_name'] == true)) {

            $returnButtonsAsString = self::submitERAQuickForm($row['bovine_id'], $callBackURL); //show button
        }

        return $returnButtonsAsString;
    }

    /*     * **********************************************
     *  Submit ERA Quickform
     * ******************************************** */

    public static function submitERAQuickForm($bovine_id, $callBackURL) {

        $form = new HTML_QuickForm("submitERAQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        $form->addElement('submit', 'btnSubmit', 'Submit', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to submit holstein canada registration?')")); //call javascript to confirm delete.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $bovine_id = $form->getElementValue('bovine_id');

            //run generation and submission via ftp of file class.
            new HolsteinCanadaERA($bovine_id); //run class
            //db is updated within HolsteinCanadaERA that it was registered.

            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    public function error() {
        $error = array();

        //find who has only 10 days left to be registered.
        $res = $GLOBALS['pdo']->query("
                SELECT count(bovine.id) as count, now() as event_time
                FROM bovinemanagement.bovine 
                 LEFT JOIN batch.holstein_canada_registered ON bovine.id=bovine_id 
                 LEFT JOIN bovinemanagement.calf_potential_name ON bovine.id=calf_potential_name.bovine_id 
                 WHERE local_reg_status=false AND death_date is null AND potential_name is not null AND date_part('epoch',(birth_date))::integer  < date_part('epoch',( now() - interval '80 days'))::integer AND date_part('epoch',(birth_date))::integer  > date_part('epoch',( now() - interval '90 days'))::integer 
                                        ");

        $row = $res->fetch(PDO::FETCH_ASSOC);

        if ($row['count'] != 0) {
            $error[] = new notifyObj(get_class($this), $row['event_time'], 2, $row['count'] . ' calves are almost overdue to be registered.');
        }

        //find who is overdue.
        $res = $GLOBALS['pdo']->query("
                SELECT count(bovine.id) as count, now() as event_time
                FROM bovinemanagement.bovine 
                 LEFT JOIN batch.holstein_canada_registered ON bovine.id=bovine_id 
                 LEFT JOIN bovinemanagement.calf_potential_name ON bovine.id=calf_potential_name.bovine_id 
                 WHERE local_reg_status=false AND death_date is null AND potential_name is not null AND date_part('epoch',(birth_date))::integer  < date_part('epoch',( now() - interval '90 days'))::integer 
                                        ");

        $row = $res->fetch(PDO::FETCH_ASSOC);

        if ($row['count'] != 0) {
            $error[] = new notifyObj(get_class($this), $row['event_time'], 1, $row['count'] . ' calves are now overdue to be registered.');
        }



        //find any cows in system who's naming isn't consistent
        $res2 = $GLOBALS['pdo']->query("
        SELECT local_number from bovinemanagement.bovine WHERE full_name is null AND sire_full_reg_number != '11111111111111111' AND death_date is null;
                ");
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $error[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 3, "{$row2['local_number']} has no name, but should have one.");
        }


        return $error;
    }

}

//end class
//
?>