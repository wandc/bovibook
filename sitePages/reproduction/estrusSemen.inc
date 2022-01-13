<?php

//Lists straws of semen in our inventory
class EstrusSemen extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Inventory';
        $tabArray[2]['name'] = 'Sires';
        $tabArray[3]['name'] = 'Projection';
        $tabArray[4]['name'] = 'Inventory (All)';
        $tabArray[5]['name'] = 'Replace Sire Pick';
        $this->renderTabs($tabArray);
    }

    public function summaryDisplay() {
        
    }

    function tab1() {

        $note = (new BootStrap)->infoNotify("NOTE: Only shows >= 2400 lpi.");

        $left = <<<HTML
            {$note}
            {$this->displayListOfSemenStraws()}
        HTML;

        $right = <<<HTML
            {$this->addSemenToInventoryQuickForm()}
            {$this->markSemenAsDiscardedQuickForm()}
            {$this->markSemenAsAtBottomOfTankQuickForm()}
        HTML;

        $content = (new Bootstrap)->oneThirdTwoThirds($left, $right);
        print($content);
    }

    function tab2() {

        $a = new QF2_BeefSire((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
        $beefSire = ($a->renderBox('QF2_BeefSire', 'Beef Sire'));
  

    

          $left = <<<HTML
          {$this->displayListOfSiresDataTable()}
          HTML;

          $right = <<<HTML
         
          {$beefSire}
           {$this->addSireToSireListQuickForm()}
          HTML;

          $content=(new Bootstrap)->oneThirdTwoThirds($left,$right);
          print($content);
        
    }

    function tab3() {


        $left = <<<HTML
                {$this->displayListOfSemenStraws('projection')}
        HTML;

        $right = <<<HTML
            {$this->addProjectionSemenToInventoryQuickForm()}
            {$this->markSemenAsDiscardedQuickForm('projection')}
        HTML;

        $content = (new Bootstrap)->oneThirdTwoThirds($left, $right);
        print($content);
    }

    function tab4() {

        print($this->displayListOfSemenStraws('all'));
    }
    
     function tab5() {

         //   $a = new QF2_BeefSire((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
       // $beefSire = ($a->renderBox('QF2_BeefSire', 'Beef Sire'));
         
     //   print($this->replaceSirePick());
    }
    

    /*     * ******************************* */
    /* Add Projection Semen Form */
    /*     * ******************************* */

    function addProjectionSemenToInventoryQuickForm() {


        //grab a list of sires.
        $res = $GLOBALS['pdo']->query("SELECT full_reg_number,sire.full_name,short_name
FROM bovinemanagement.sire ORDER BY short_name ASC");
        {
            
        } // Always check that result is not an error
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row['full_reg_number']] = $row['short_name'] . " (" . $row['full_name'] . ")";
        }



        $form = new HTML_QuickForm("addProjectionSemenToInventoryQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Add Projection Semen To Inventory');

        $form->addElement('select', 'sireRegNumber', 'Sire', $aniNumber);


        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);



        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('sireRegNumber', 'A sire is required.', 'required');



        // Try to validate a form
        if ($form->validate()) {

            //get values
            $sireReg = $form->exportValue('sireRegNumber');

            $pageid = $form->exportValue('pageid');

            $freezeCombined = '1970-01-01';
            $numberOfStraws = 1;
            $invoiceCostPerStraw = 1;
            $tank = 'Z';
            $sexedSemen = 'false';


            //get random transaction id
            $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

            $binNumber = 'NULL';

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //lookup what the semen code is depending if the user selected sexed semen or not.
                $res = $GLOBALS['pdo']->query("SELECT semen_code FROM bovinemanagement.sire_semen_code WHERE sire_full_reg_number='$sireReg' AND sexed_semen=$sexedSemen LIMIT 1");

                $row = $res->fetch(PDO::FETCH_ASSOC);
                $semen_code = $row['semen_code'];

                //don't put id value, uses next one in DB.
                $query = "INSERT INTO bovinemanagement.semen_straw (semen_code,freeze_date,supplier_code,invoice_cost,tank,bin,transaction_id,userid,projection) VALUES ('$semen_code','$freezeCombined','$supplierCode',$invoiceCostPerStraw,'$tank',$binNumber,$transaction_id,'{$GLOBALS['auth']->getUsername()}','true')";

                $res = $GLOBALS['pdo']->exec($query);



                // determine whether to commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }

            //print("end insert <br/>");
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation
        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /*     * ******************************* */
    /* Add Sire to List of Sires */
    /*     * ******************************* */

    function addSireToSireListQuickForm() {

        $form = new HTML_QuickForm("addSireToSireListQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Holstein Sire');

        $form->addElement('text', 'sireRegNum', 'Sire Full Registration #');

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');


        //rules
        $form->addRule('sireRegNum', 'Sire Reg number is required', 'required');
        //register custom rule
        $form->registerRule('checkRegNumberIsValidRule', 'function', 'checkRegNumberIsValid');
        $form->addRule(array('sireRegNum'), 'Sire Registration Number is Not Valid.', 'checkRegNumberIsValidRule', array('sireRegNum'));
        //register custom rule
        $form->registerRule('checkRegNumberIsDuplicateRule', 'function', 'checkRegNumberIsDuplicate');
        $form->addRule(array('sireRegNum'), 'Duplicate:Sire is Already in Sire List.', 'checkRegNumberIsDuplicateRule', array('sireRegNum'));

        //rule
        function checkRegNumberIsValid($junk, $valueArr, $elementNameArr) {
            //generic loop to convert $form elements we want into a usuable array
            //I really hope this isn't how quickforms was designed.
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }

            //break it up into parts and check it is valid.
            $returnArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($formData['sireRegNum']);

            if ($returnArray == false) {
                return false;
            } else {
                return true;
            }
        }

        //rule
        function checkRegNumberIsDuplicate($junk, $valueArr, $elementNameArr) {
            //generic loop to convert $form elements we want into a usuable array
            //I really hope this isn't how quickforms was designed.
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }

            $formData['sireRegNum'] = strtoupper(trim($formData['sireRegNum'])); //make it easy to match

            $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.sire WHERE full_reg_number='{$formData['sireRegNum']}' LIMIT 1");
            {
                
            } // Always check that result is not an error
            $row = $res->fetch(PDO::FETCH_ASSOC);

            if (empty($row['id'])) {
                return true;
            } else {
                return false;
            }
        }

        function getSexxedSemenPrefixCode($semenCode) {

            if (empty($semenCode)) {
                throw new Exception("No semen Code provided.");
            }
            
            
            //parse semen code and get prefix.
            preg_match('/([0-9]{4})HO([0-9]{5})/', $semenCode, $regs); //works only with full semen code number (11 chars).
            $semenCodePrefix = $regs[1];
            $semenCodeSuffix = 'HO' . $regs[2]; //create suffix

            switch ($semenCodePrefix) {
                case '0011': //alta gentics
                    $sexxedSemenCode = '0511' . $semenCodeSuffix;
                    break;
                case '0200': //semex alliance
                    $sexxedSemenCode = '0777' . $semenCodeSuffix;
                    break;
                case '0007': //select sires
                    $sexxedSemenCode = '0507' . $semenCodeSuffix;
                    break;
                case '0029': //abs global
                    $sexxedSemenCode = '0529' . $semenCodeSuffix;
                    break;
                case '0001': //genex coop
                    $sexxedSemenCode = '0501' . $semenCodeSuffix;
                    break;
                case '0250': //Genervations
                    $sexxedSemenCode = '0550' . $semenCodeSuffix;
                    break;
                case '0014': //select sires AG
                    $sexxedSemenCode = '0527' . $semenCodeSuffix;
                    break;
                case '0224': //cogent breeding UK
                    $sexxedSemenCode = '0522' . $semenCodeSuffix;
                    break;
                case '0799': //Blondin
                    $sexxedSemenCode = '0799' . $semenCodeSuffix;
                    break;
                case '0322': //DMV GenetiQ Services	(Blondin angus)
                     $sexxedSemenCode = '0322' . $semenCodeSuffix;
                    break;
                case '0522': //cogent breeding UK / ST genetics only has sexxed for sale. so they use sexxed code on CDN. work-around
                    $semenCode='0224'. $semenCodeSuffix;
                    $sexxedSemenCode = '0522' . $semenCodeSuffix;
                    break;
                
                //add others as more sexxed semen comes on the market.
                default: //no match found.
                    throw new Exception("No Sexxed semen code found for equivalent:$semenCodePrefix.");
                    $sexxedSemenCode = null; //none exists
            }

            //because of ST genetics stupidity we sometime change smenecode too after looking it up, so return both. 
             $object = (object) [
                        'semenCode' => $semenCode,
                        'sexxedSemenCode' => $semenCodeSuffix,
            ];


            //print_r($regs);
            return $object;
        }

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $sireRegNum = $form->exportValue('sireRegNum');
            $pageid = $form->exportValue('pageid');

            //take reg number and throw it into a curl function to contact CDN website and automatically pull information.
            $sireInfoArray = self::pullSireInfoFromCDNWebsite($sireRegNum);

            print("fullRegNumber:{$sireInfoArray['fullRegNumber']}<br>");
            print("fullName:{$sireInfoArray['fullName']}<br>");
            print("shortName:{$sireInfoArray['shortName']}<br>");
            print("semenCode:{$sireInfoArray['semenCode']}<br>");

            $fullReg = trim($sireInfoArray['fullRegNumber']);

            if ($fullReg == NULL) {
                throw new Exception('Aggregate Data Connection to CDN website returned NULL. Either invalid sire registration number or CDN website inaccessible.');
            }
var_dump($sireInfoArray);
            
            $sexxedSemenPrefixCodeObj= getSexxedSemenPrefixCode($sireInfoArray['semenCode']);
            $semenCode =$sexxedSemenPrefixCodeObj->semenCode;
            $secondarySemenCode =$sexxedSemenPrefixCodeObj->secondarySemenCode;
            
            
            //full_name and short_name needed to be sanitized for postgresql
            $sireInfoArray['fullName'] = pg_escape_string($sireInfoArray['fullName']);
            $sireInfoArray['shortName'] = pg_escape_string($sireInfoArray['shortName']);


            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //don't put id value, uses next one in DB.
                $query = "INSERT INTO bovinemanagement.sire (full_reg_number,full_name,short_name,userid) VALUES ('$fullReg','{$sireInfoArray['fullName']}','{$sireInfoArray['shortName']}','{$GLOBALS['auth']->getUsername()}')";
                $statement = $GLOBALS['pdo']->prepare($query);
                $statement->execute();


                //insert primary semen code
                $query = "INSERT INTO bovinemanagement.sire_semen_code (semen_code,sire_full_reg_number,sexed_semen) VALUES ('$semenCode','$fullReg','FALSE')";
                $res = $GLOBALS['pdo']->exec($query);


                //insert sexed semen code if necessary
                if ($secondarySemenCode != null) {
                    $query = "INSERT INTO bovinemanagement.sire_semen_code (semen_code,sire_full_reg_number,sexed_semen) VALUES ('$secondarySemenCode','$fullReg','TRUE')";
                    $res = $GLOBALS['pdo']->exec($query);
                }

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            $_SESSION['selectedTab'] = 2;
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /*     * ******************************* */
    /* Contact CDN website and get Sire
      /* Info
      /* ******************************** */

    function pullSireInfoFromCDNWebsite($sireRegNum) {

        $sireRegNum = trim($sireRegNum);
        //don't bother checking if in DB or not, 99% of the time it won't be.

        $aggrObj = new AggregateSiteData;
        $aggrObj->upsertSingleAnimal(trim($sireRegNum), 'all');



        $res = $GLOBALS['pdo']->query("SELECT full_reg_number, full_name,male_short_name, semen_code FROM batch.aggregate_view_all WHERE full_reg_number='$sireRegNum' limit 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);

        //now create array to do sire insert later.
        if (!empty($row['male_short_name'])) {
            $returnArray['fullRegNumber'] = trim($row['full_reg_number']);
            $returnArray['fullName'] = trim($row['full_name']);
            $returnArray['shortName'] = trim($row['male_short_name']);
            $returnArray['semenCode'] = trim($row['semen_code']);
        } else {
            $returnArray = null;
            throw new Exception("Error: New Bull not in database. It's possible that it could not connect to internet site, try again later, or registration number does not exist. No data or null data returned.");
        }

        return $returnArray;
    }

    /////////////////////////////////////////////////////////////////////



    function listOfSires($mode = 'default') {

        $numberPerPage = 40;
        $sql = "SELECT id,sire.full_reg_number,sire.full_name,short_name,create_time,update_time,userid FROM bovinemanagement.sire ORDER BY short_name ASC";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

           $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'dateTimeName' => 'update_time', 'schemaTable' => 'bovinemanagement.sire'));
            
        //change presentation
        foreach ($results as &$row) {


            $row['full_reg_number'] = '<a href="' . $GLOBALS['MiscObj']->createCDNLink($row['full_reg_number']) . '">' . $row['full_reg_number'] . '</a>';
            $row['event_time_raw'] = strtotime($row['update_time']);
            $row['update_time'] = date("l M d Y H:00", strtotime($row['update_time']));  //has to be at end.
            $row['action'] = $delButton->output($row['id']);
          
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayListOfSiresDataTable() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
                 
            {
                data: "full_reg_number",
                title: "full_reg_number"
            },
            {
                data: "full_name",
                title: "full_name"
            },
            {
                data: "short_name",
                title: "short_name"
            },
            {
                data: "update_time",
                title: "Update Time",
                type: "date"
            },
            {
                data: "userid",
                title: "Employee"
            },
            {
                data: "action",
                title: "Action"
            },
                
                
EX;
        return($dt->start('Sire List', 33, 'listOfSires', null, $columnsStr));
    }

    function printFormattedTime($params) {
        extract($params);
        return date("M d Y", strtotime($record['update_time']));
    }

    /////////////////////////////////////////////////////////////////////


    /*     * ******************************* */
    /* Add Semen Form */
    /*     * ******************************* */
    function addSemenToInventoryQuickForm() {

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForNexttDay(); //hourly array, with unix timestamp as index.
        //grab a list of sires.
        $res = $GLOBALS['pdo']->query("SELECT full_reg_number,sire.full_name,short_name
FROM bovinemanagement.sire ORDER BY short_name ASC");
        {
            
        } // Always check that result is not an error
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row['full_reg_number']] = $row['short_name'] . " (" . $row['full_name'] . ")";
        }

        //create list of invoice costs.
        $invoiceCostPerStraw[null] = 'Please Select one...';
        for ($counter = 1; $counter <= 250; $counter += 1) {
            $invoiceCostPerStraw[$counter] = '$' . $counter;
        }


        //$numberOfStrawsArray=array(null=>'Please Select one...',5=>5,10=>10,15=>15,20=>20);
        $numberOfStrawsArray = array(null => 'Please Select one...', 1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5, 6 => 6, 7 => 7, 8 => 8, 9 => 9, 10 => 10, 15 => 15, 20 => 20, 30 => 30, 40 => 40);
        $numberOfBinArray = array(null => 'Please Select one...', 1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5, 6 => 6, 99999 => 'Bottom of Tank');
        $semenSupplierCodeArray = array(null => 'Please Select one...', 'ONAI0007' => 'Select - ONAI0007', 'ONAI0070' => 'EastGen - ONAI0070', 'ONAI0001' => 'Genex - ONAI0001', 'ONAI0029' => 'ABS - ONAI0029', 'ABAI0002' => 'Alta - ABAI0002', 'ONAI0014' => 'Select AG- ONAI0014','ONAI0076'=>'ST Genetics','ONAI0322'=>'DMV Genetics');


        $form = new HTML_QuickForm("addSemenToInventoryQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Add Semen To Inventory');

        $form->addElement('select', 'sireRegNumber', 'Sire', $aniNumber);

        $freezeDateGroup[] = $form->createElement('date', 'freezeDate', 'Freeze Date', array('format' => 'Y-m-d', 'minYear' => 2000, 'maxYear' => date("Y"), 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
        $freezeDateGroup[] = $form->createElement('text', 'freezeCode', 'Freeae Code');
        $form->addGroup($freezeDateGroup, 'freezeDateGroup', 'Freeze Date OR Freeze Code', '   Code:');

        $form->addElement('checkbox', 'sexedSemen', 'Sexed Semen:', 'yes');

        $form->addElement('select', 'numberOfStraws', 'Number of Straws', $numberOfStrawsArray);

        $form->addElement('select', 'invoiceCostPerStraw', 'Invoice Cost per Straw', $invoiceCostPerStraw);

        $form->addElement('select', 'supplier_code', 'Semen Supplier Code', $semenSupplierCodeArray);

        $form->addElement('hidden', 'tank', 'A'); //default to one tank called A.

        $form->addElement('select', 'binNumber', 'Bin Number', $numberOfBinArray);

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);



        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('sireRegNumber', 'A sire is required.', 'required');
        $form->addRule('tank', 'A tank letter is required.', 'required');
        $form->addRule('binNumber', 'A bin number is required.', 'required');
        $form->addRule('numberOfStraws', 'A number of semen straws is required.', 'required');
        $form->addRule('invoiceCostPerStraw', 'Invoice Cost Per Straw is required', 'required');
        $form->addRule('supplier_code', 'Semen Supplier code is required', 'required');
        //group rules one or other is required.
        //$freezeDateGroupRules['freezeDate'][0] = array('Freeze Date is required', 'required');
        //$freezeDateGroupRules['freezeCode'][0] = array('Freeze Code is required', 'required');
        //$form->addGroupRule('freezeDateGroup', $freezeDateGroupRules);
        //register custom rule
        $form->registerRule('checkSexedSemenCodeExistsRule', 'function', 'checkSexedSemenCodeExists');
        $form->addRule(array('sireRegNumber', 'sexedSemen'), 'Sexed Semen Code Does Not Exist For This Sire.', 'checkSexedSemenCodeExistsRule', array('sireRegNumber', 'sexedSemen'));
        //register custom rule 2
        $form->registerRule('checkFreezeDateExistsRule', 'function', 'checkFreezeDateExists');
        $form->addRule('freezeDateGroup', 'A freeze date OR numeric freeze code is required.', 'checkFreezeDateExistsRule', 'freezeDateGroup');

        //function Check that either a freeze date or code was entered.
        function checkFreezeDateExists($junk, $valueArr, $elementNameArr) {

            print_r($valueArr['freezeDate']);
            print_r($valueArr['freezeCode']);

            if ($valueArr['freezeDate']['d'] == '' && $valueArr['freezeDate']['m'] == '' && $valueArr['freezeDate']['Y'] == '') {
                $freezeDate = FALSE; //date not selected, thus false
            } elseif ($valueArr['freezeDate']['d'] == '' || $valueArr['freezeDate']['m'] == '' || $valueArr['freezeDate']['Y'] == '') {
                $freezeDate = FALSE; //partial date selected, thus false
            } else {
                $freezeDate = TRUE;
            }

            if ($valueArr['freezeCode'] == '') {
                $freezeCode = FALSE;
            } elseif (is_numeric($valueArr['freezeCode']) == FALSE) {
                $freezeCode = FALSE;
            } else {
                $freezeCode = TRUE;
            }

            //now determine if only one is entered.
            if ($freezeDate == TRUE && $freezeCode == FALSE || $freezeDate == FALSE && $freezeCode == TRUE) {
                return true;
            } else {
                return false;
            }
        }

        //rule
        function checkSexedSemenCodeExists($junk, $valueArr, $elementNameArr) {
            //generic loop to convert $form elements we want into a usuable array
            //I really hope this isn't how quickforms was designed.
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }
            //print("TTT:{$formData['sireRegNumber']};");
            $res = $GLOBALS['pdo']->query("SELECT semen_code FROM bovinemanagement.sire_semen_code WHERE sire_full_reg_number='{$formData['sireRegNumber']}' AND sexed_semen IS TRUE LIMIT 1");
            {
                
            } // Always check that result is not an error
            $row = $res->fetch(PDO::FETCH_ASSOC);
            //sexed semen code does not exist.
            //print("XXX:{$row['semen_code']};");
            if (($row == null) && ($formData['sexedSemen'] == 1)) {
                return false;
            } else {
                return true;
            }
        }

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $sireReg = $form->exportValue('sireRegNumber');
            $freezeDateGroup = $form->getElementValue('freezeDateGroup');
            $sexedSemen = $form->exportValue('sexedSemen');
            $supplierCode = $form->exportValue('supplier_code');
            $numberOfStraws = $form->exportValue('numberOfStraws');
            $invoiceCostPerStraw = $form->exportValue('invoiceCostPerStraw');
            $tank = $form->exportValue('tank');
            $binNumber = $form->exportValue('binNumber');
            $pageid = $form->exportValue('pageid');

            //break out freeze date
            $tempArray = $freezeDateGroup['freezeDate'];
            $DateOfFreeze = strtotime($tempArray['Y'][0] . '-' . $tempArray['m'][0] . '-' . $tempArray['d'][0]);
            //break out freeze code
            $freezeCode = $freezeDateGroup['freezeCode'];

            if ($freezeCode == '') { //the date is the one to use.
                $freezeCombined = $tempArray['Y'][0] . '-' . $tempArray['m'][0] . '-' . $tempArray['d'][0];
            } else { //the code is the one to use.
                $freezeCombined = trim($freezeCode);
            }


            //get random transaction id
            $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

            //add special case for when semen straw is in the bottom of the tank.
            if ($binNumber == 99999) {
                $binNumber = 'NULL';
            }


            //correct sexed semen boolean for blank=false.
            if ($sexedSemen == true) {
                $sexedSemen = 'true';
            } else {
                $sexedSemen = 'false';
            }

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //lookup what the semen code is depending if the user selected sexed semen or not.
                $res = $GLOBALS['pdo']->query("SELECT semen_code FROM bovinemanagement.sire_semen_code WHERE sire_full_reg_number='$sireReg' AND sexed_semen=$sexedSemen LIMIT 1");
                {
                    
                } // Always check that result is not an error
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $semen_code = $row['semen_code'];

                //don't put id value, uses next one in DB.
                $query = "INSERT INTO bovinemanagement.semen_straw (semen_code,freeze_date,supplier_code,invoice_cost,tank,bin,transaction_id,userid) VALUES ('$semen_code','$freezeCombined','$supplierCode',$invoiceCostPerStraw,'$tank',$binNumber,$transaction_id,'{$GLOBALS['auth']->getUsername()}')";


                //repeat the insert statement for each straw.
                for ($counter = 1; $counter <= $numberOfStraws; $counter++) {
                    $res = $GLOBALS['pdo']->exec($query);
                }


                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }

            //print("end insert <br/>");
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            ////header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&selectedTab=2");
            $_SESSION['selectedTab'] = 2;
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation
        //displayLastbreedingEvents();
        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /////////////////////////////////////////////////////////////////////

    function listOfSemenStraws($mode = 'default') {
        if ($mode == 'default') {
            $sql = "SELECT * FROM bovinemanagement.semen_straw_curr_summary
WHERE (lpi >=2400 OR lpi IS NULL) AND projection IS FALSE
ORDER BY short_name";
        } elseif ($mode == 'projection') {

            $sql = "SELECT * FROM bovinemanagement.semen_straw_curr_summary
WHERE (lpi >=2400 OR lpi IS NULL) AND projection IS TRUE
ORDER BY short_name 
            
";
        } elseif ($mode == 'all') {

            $sql = "
SELECT * FROM bovinemanagement.semen_straw_curr_summary     
ORDER BY short_name
";
        }



        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

         $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'deleteName' =>'create_time','dateTimeName' => 'create_time', 'schemaTable' => 'bovinemanagement.semen_straw'));
      
        //change presentation
        foreach ($results as &$row) {
            $row['short_nameX'] = '<a href="' . $GLOBALS['MiscObj']->createCDNLink($row['full_reg_number']) . "\">{$row['short_name']}</a>";
            $row['sexed_semen'] = ( $row['sexed_semen'] == 't' ? 'true' : '');
            $row['reserved'] = ( $row['reserved'] == 't' ? 'true' : '');
            $row['prodoll'] = empty($row['prodoll']) ? '<i class="fab fa-github-alt"></i>' : $row['prodoll'];
            $row['action'] = $delButton->output($row['create_time'],null,"<i class='fa fa-fw fa-object-group'></i><i class='fa fa-fw fa-trash'></i>");
        }
        //output
        $output['data'] = $results;
        return $output['data'];
    }

    function displayListOfSemenStraws($mode = 'default') {




        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
           
             {
                 data: "short_name",
                 "visible": false,
                "searchable": false
            },   
              
            {
                data: "short_nameX",
                title: "Short Name"
            },
            {
                data: "full_name",
                title: "Name"
            },
            {
                data: "bin",
                title: "Bin #"
            },
            {
                data: "sexed_semen",
                title: "Sexxed?"
            },
        {
                data: "semen_code",
                title: "Semen Code"
            },
            {
                data: "freeze_date",
                title: "Freeze Date"
            },
            {
                data: "prodoll",
                title: "Pro$"
            },
            {
                data: "invoice_cost",
                title: "Cost $"
            },
            {
                data: "reserved",
                title: "Reserved?"
            },
          //  {
          //      data: "ownerid",
          //      title: "ownder"
          //  },
            {
                data: "count",
                title: "Inventory"
            },
                {
                data: "action",
                title: "Action"
            }
                
                
EX;
        return($dt->startLocal('Semen Straws', $this->listOfSemenStraws($mode), $columnsStr, null, ' "paging": false'));
    }

    function printShortNameLink($params) {
        extract($params);
        //create a link to CDN website, so when user clicks they go there for the sire
        $linkStr = $GLOBALS['MiscObj']->createCDNLink($record['full_reg_number']);
        return "<a href=\"$linkStr\">{$record['short_name']}</a>";
    }

    function printBinProj($params) {
        extract($params);
        if ($record['projection'] == 't') {
            return 'Proj';
        } else {
            return $record['bin'];
        }
    }

    function printLPI($params) {
        extract($params);
        return $record['lpi'] . ' (' . $record['reliability_percent'] . '%)';
    }

    function printSexedOrNot($params) {
        extract($params);
        if ($record['sexed_semen'] == 't') {
            $returnArray['sexed'] = 'Yes';
        } else {
            $returnArray['sexed'] = 'No';
        }

        return $returnArray['sexed'];
    }

    function printReservedOrNot($params) {
        extract($params);
        if ($record['reserved'] == 't') {
            $returnArray['reserved'] = 'Yes';
        } else {
            $returnArray['reserved'] = '';
        }

        return $returnArray['reserved'];
    }

    function getVariousTotalsOfSemenInventory() {
//total high lpi straws, that are not in bottom of tank.
        $sql = "SELECT short_name,sire.full_name,sire.full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,lpi,reliability as reliability_percent,invoice_cost,sexed_semen,reserved
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.bin IS NOT NULL AND semen_straw.discarded IS FALSE AND lpi > 2000
ORDER BY short_name";
        $res = $GLOBALS['pdo']->query($sql);
        $returnArray['totalHighLpiStraws'] = $row = $res->rowCount();
//
//total straws, including bottom of tank
        $sql = "SELECT short_name,sire.full_name,sire.full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,lpi,reliability as reliability_percent,invoice_cost,sexed_semen,reserved
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE
ORDER BY short_name";
        $res = $GLOBALS['pdo']->query($sql);
        $returnArray['totalStraws'] = $row = $res->rowCount();
//
//total test sires. (lower than 50% reliability)
        $sql = "SELECT short_name,sire.full_name,sire.full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,lpi,reliability as reliability_percent,invoice_cost,sexed_semen,reserved
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.bin IS NOT NULL AND semen_straw.discarded IS FALSE AND reliability  < 73
ORDER BY short_name";
        $res = $GLOBALS['pdo']->query($sql);
        $returnArray['totalTestStraws'] = $row = $res->rowCount();


        return $returnArray;
    }

    /*     * ******************************* */
    /* Mark Semen to discard */
    /*     * ******************************* */

    function markSemenAsDiscardedQuickForm($mode = 'default') {
        if ($mode == 'default') {
            //list full inventory individually.
            $sql = "SELECT semen_straw.id as semen_straw_id,short_name,sire.full_name,sire.full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,lpi,reliability as reliability_percent,invoice_cost,sexed_semen,reserved 
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE 
ORDER BY short_name";
        } elseif ($mode = 'projection') {
            //list full inventory individually.
            $sql = "SELECT semen_straw.id as semen_straw_id,short_name,sire.full_name,sire.full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,lpi,reliability as reliability_percent,invoice_cost,sexed_semen,reserved 
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE AND projection IS TRUE
ORDER BY short_name";
        }

        $res = $GLOBALS['pdo']->query($sql);
        $inventory[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $inventory[$row['semen_straw_id']] = "{$row['short_name']} ({$row['semen_code']} / {$row['freeze_date']}) Bin: {$row['bin']}";
        }

        $form = new HTML_QuickForm("markSemenAsDiscardedQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        if ($mode == 'default') {
            $form->addElement('header', 'hdrQuickform', 'Mark Semen as Discarded');
        } elseif ($mode = 'projection') {
            $form->addElement('header', 'hdrQuickform', 'Mark Projection Semen as Discarded');
        }
        $form->addElement('select', 'inventory', 'Semen', $inventory);

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('inventory', 'A semen straw choice is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            $semen_straw_id = $form->exportValue('inventory');
            //print("straw to discard:$semen_straw_id<br>");
            //mark discarded for the semen straw id supplied.
            $query = "UPDATE bovinemanagement.semen_straw SET discarded=TRUE WHERE id=$semen_straw_id";

            $res = $GLOBALS['pdo']->exec($query);

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}"); //forward on.
            exit();
        } //end validation


        $form->accept($renderer);
        return $renderer->toHtml();
    }

    /*     * ******************************* */
    /* Mark Semen at bottom of tank     */
    /*     * ******************************* */

    function markSemenAsAtBottomOfTankQuickForm() {

        //list full inventory individually.
        $sql = "SELECT semen_straw.id as semen_straw_id,short_name,sire.full_name,sire.full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,lpi,reliability as reliability_percent,invoice_cost,sexed_semen,reserved 
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE 
ORDER BY short_name";

        $res = $GLOBALS['pdo']->query($sql);
        $inventory[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $inventory[$row['semen_straw_id']] = "{$row['short_name']} ({$row['semen_code']} / {$row['freeze_date']}) Bin: {$row['bin']}";
        }

        $form = new HTML_QuickForm("markSemenAsAtBottomOfTankQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Mark Semen at bottom of tank');

        $form->addElement('select', 'inventory', 'Semen', $inventory);

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('inventory', 'A semen straw choice is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            $semen_straw_id = $form->exportValue('inventory');
            //print("straw to discard:$semen_straw_id<br>");
            //mark discarded for the semen straw id supplied.
            $query = "UPDATE bovinemanagement.semen_straw SET bin=null WHERE id=$semen_straw_id";

            $res = $GLOBALS['pdo']->exec($query);

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}"); //forward on.
            exit();
        } //end validation


        $form->accept($renderer);
        return $renderer->toHtml();
    }

}

//end class
Class QF2_BeefSire extends QF2_Base {

    function definition($form) {



        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        //'minlength', 'The password is too short', 6))
        $reg = $form->addElement('text', 'reg', array('size' => 50, 'maxlength' => 15))->setLabel('Beef reg #'); //
        $reg->addRule("minlength", "Please specify your Reg # at 15 characters.", 15);
        $reg->addRule("minlength", "Please specify your Reg # at 15 characters.", 15);
        $reg->addRule("required", "Please specify your Reg #");
        HTML_QuickForm2_Factory::registerRule('error1', 'HTML_QuickForm2_Rule_Callback', null, array('callback' => 'QF2_BeefSire::checkRegistrationNumberIsValid'));
        $reg->addRule('error1', 'Registration Number is Not Valid!', array('test'));


        
        $form->addElement('text', 'longName', array('size' => 50, 'maxlength' => 255))->setLabel('Long Name')->addRule("required", "Please specify your Long Name");     
        $form->addElement('text', 'shortName', array('size' => 50, 'maxlength' => 32))->setLabel('Short Name')->addRule("required", "Please specify your Short Name");
        
        $semenCode=$form->addElement('text', 'semenCode', array('size' => 50, 'maxlength' => 32))->setLabel('Semen Code (ie 0200AN0XXXX)');
        $semenCode->addRule("minlength", "Please specify your Semen Code # at 11 characters.", 11);
        $semenCode->addRule("minlength", "Please specify your Semen Code # at 11 characters.", 11);
        $semenCode->addRule("required", "Please specify your Semen Code #");
         HTML_QuickForm2_Factory::registerRule('error2', 'HTML_QuickForm2_Rule_Callback', null, array('callback' => 'QF2_BeefSire::checkSemenCodeIsValid'));
        $semenCode->addRule('error2', 'Semen Code is Not Valid!', array('test'));

        
        //defaults
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('reg' => 'AN')));



        return $form; //required       
    }

    //rule called by callback. 
    function checkRegistrationNumberIsValid($value, $one) {
        //var_dump($value);

        $regArr = $GLOBALS['MiscObj']->breakUpFullRegNumber($value, true);

        //if reg is valid an array is returned, else false.
        return (is_array($regArr)) ? true : false;
    }

    function checkSemenCodeIsValid($value) {
        $semenCode=$value;
        
        $pattern = "/\d{4}[a-zA-Z]{2}\d{5}/i"; //matches like 0200AN01235
        preg_match($pattern, $semenCode, $matches, PREG_OFFSET_CAPTURE);
        
        //if array returned we have a match. 
        return (!empty($matches)) ?  true : false;

    }
    
    
    function process($value) {

           try {
               $res = $GLOBALS['pdo']->beginTransaction();
        //sire       
        $query = "INSERT INTO bovinemanagement.sire (full_reg_number,full_name,short_name,userid) VALUES (:reg,:long_name,:short_name,:userid) ON CONFLICT DO NOTHING";
        $statement = $GLOBALS['pdo']->prepare($query);
        $statement->bindValue(':reg', $value['reg'], PDO::PARAM_STR);
        $statement->bindValue(':long_name', $value['longName'], PDO::PARAM_STR);
        $statement->bindValue(':short_name', $value['shortName'], PDO::PARAM_STR);
        $statement->bindValue(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
        
        //semen code
        $query2 = "INSERT INTO bovinemanagement.sire_semen_code (semen_code,sire_full_reg_number,sexed_semen) VALUES (:semen_code,:sire_full_reg_number,:sexed_semen) ON CONFLICT DO NOTHING";
        $statement = $GLOBALS['pdo']->prepare($query2);
        $statement->bindValue(':semen_code', $value['semenCode'], PDO::PARAM_STR);
        $statement->bindValue(':sire_full_reg_number', $value['reg'], PDO::PARAM_STR);
        $statement->bindValue(':sexed_semen', false, PDO::PARAM_BOOL);
        $statement->execute();
       
        
         $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
        
    }

}


//end class

/*
 * used to replace all instances of a sire pick (for active animals) with another sire. Used when switching angus bulls for instance.
 */
Class QF2_ReplaceSirePick extends QF2_Base {

    function definition($form) {



        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        
        
        
        $form->addElement('select', 'heatType', null, array('options' => $heatEvents, 'label' => 'Type of Heat Event'))->addRule("required", "Please specify your type of heat");
        $form->addElement('select', 'heatType', null, array('options' => $heatEvents, 'label' => 'Type of Heat Event'))->addRule("required", "Please specify your type of heat");
        
       
        return $form; //required       
    }

   
    
    
    function process($value) {

           try {
               $res = $GLOBALS['pdo']->beginTransaction();
        //sire       
        $query = "INSERT INTO bovinemanagement.sire (full_reg_number,full_name,short_name,userid) VALUES (:reg,:long_name,:short_name,:userid) ON CONFLICT DO NOTHING";
        $statement = $GLOBALS['pdo']->prepare($query);
        $statement->bindValue(':reg', $value['reg'], PDO::PARAM_STR);
        $statement->bindValue(':long_name', $value['longName'], PDO::PARAM_STR);
        $statement->bindValue(':short_name', $value['shortName'], PDO::PARAM_STR);
        $statement->bindValue(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
        
        //semen code
        $query2 = "INSERT INTO bovinemanagement.sire_semen_code (semen_code,sire_full_reg_number,sexed_semen) VALUES (:semen_code,:sire_full_reg_number,:sexed_semen) ON CONFLICT DO NOTHING";
        $statement = $GLOBALS['pdo']->prepare($query2);
        $statement->bindValue(':semen_code', $value['semenCode'], PDO::PARAM_STR);
        $statement->bindValue(':sire_full_reg_number', $value['reg'], PDO::PARAM_STR);
        $statement->bindValue(':sexed_semen', false, PDO::PARAM_BOOL);
        $statement->execute();
       
        
         $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
        
    }

}

//end class