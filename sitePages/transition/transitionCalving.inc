<?php





include_once ($_SERVER ['DOCUMENT_ROOT'] . 'template/accordionSub.php');
include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/holsteinCanadaERAReg.inc');
;

/** accordian class */
class AccordionImplementation extends AccordionSub {

    function tab1() {
        $qfOne = new QF2_CalvingEvent((object) array(forwardURLArr=>array('pageid' => $_REQUEST['pageid'])), 1);
        print($qfOne->renderBox('qf_calf1','Record Calving Event'));
    }

    function tab2() {
        $qfTwo = new QF2_CalvingEvent((object) array(forwardURLArr=>array('pageid' => $_REQUEST['pageid'])), 2);
        print($qfTwo->renderBox('qf_calf2','Record Calving Event'));
    }

}

class TransitionCalving extends BasePage {

    const REGTOLOCALCORRECTION = 0; //change this to change ie 12701940 - 610 = 1330 as local number

    public function defaultDisplay() {



        $accordionArray[1]['name'] = 'Single';
        $accordionArray[2]['name'] = 'Twins';
        $accordion = new AccordionImplementation();
        $accordion->setCSS('accordionCalving');
        $accordion->render($accordionArray);
        $this->displayLast100DaysCalvingEvents();
        //   $this->displayLastcalvingEvents();
        $this->calvingSOP();
    }

    public function summaryDisplay() {
        $this->displayLastcalvingEvents('summary');
    }

    public function milkTestInputsDisplay() {
        //$this->displayLastcalvingEvents('summary');
        $this->displayLast100DaysCalvingEvents();
    }

    function calvingSOP() {
        print("<h3>SOP</h3>");
        print("Vaccinate all female calves");
    }

    function nextTagsToUse() {
        $res = $GLOBALS['pdo']->query("");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $html = <<<HTML
   <div class="callout callout-info">
                <h4>Next Tag to Use:</h4>

                <p>$nextHeifertag.</p>
              </div>theH
HTML;
    }

    ///list potential breedings or embryo transfers
    function PotentialSireandRecipient($record) {

        extract($record); //HACK, if called from datagrid it expands into record array. if not it just passes it by for js datatable 

        $calvingTime = $record['birth_time'];
        $probArr = array();
        try {
            $probArr = HolsteinCanadaERA::listMostLikelyConceptionEvents($calvingTime, $record['conceiver_id']);
        } catch (HolsteinCanadaERA_NoConceptionEventException $exception) {
            return "Error: No conception Event!";
        }

        $str = "";
        foreach ($probArr as $value) {
            $time = strtotime($value['event_time']);
            $ftime = date('M j, Y', $time);
            if ($value['type'] == 'breeding') {
                $str = $str . "{$value['days_ago']} <b class=\"smallish\">($ftime)</b> to {$value['service_sire_short_name']} <br>";
            } elseif ($value['type'] == 'embryo7day') {
                $str = $str . "{$value['days_ago']} <b class=\"smallish\">($ftime)</b> to <b>EMBRYO</b> from #{$value['donor_dam_local_number']} {$value['donor_dam_short_name']} & {$value['service_sire_short_name']} <br>";
            }
        }
        return $str;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
///choose who sire and recipient 
    function chooseSireandRecipientSub($bovine_id, $calvingTime) {

        $sql = "SELECT event_time,recipient_bovine_id,service_sire_full_name,service_sire_short_name,service_sire_full_reg_number,recipient_full_reg_number,donor_dam_bovine_id,donor_dam_local_number,donor_dam_full_name,donor_dam_full_reg_number,calculated_potential_due_date,
	date_trunc('days','$calvingTime'- ( calculated_potential_due_date - interval '280 days')) as days_ago
 FROM bovinemanagement.combined_breeding_embryo_view 
WHERE recipient_bovine_id=$bovine_id  AND calculated_potential_due_date >= (date '$calvingTime'- interval '14 days') AND calculated_potential_due_date <= (date '$calvingTime'+ interval '14 days')";


        /** Break this out into three different problems:
          1. Who is the sire?
          2. Who is the dam?
          3. Who is the recipient (always known, its the bovine_id who gave birth)
         */
        $ret = array();

        /// 1. Who is the sire?
        $res = $GLOBALS['pdo']->query($sql);
        $sires = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $sires[] = $row['service_sire_full_reg_number'];
        }

        $answer = array_unique($sires); //return all the unique sires. 

        if (count($answer) == 1) {  //when there is only one valid sire and we can be sure this is it.
            //we can just use the first sql row, because the sire is all the same.
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $ret['sire_full_reg_number'] = $row['service_sire_full_reg_number'];
        } else { // Cannot be determined.;
            $ret['sire_full_reg_number'] = '';
        }

        ///2. Who is the dam (and recipient).
        //FIXME DOES NOT WORK.
        $type = array();
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            if (isset($row['type'])) {
                $type[] = $row['type']; //breeeding or embryo7day
            }
        }
        $answer2 = array_unique($type); //return all the unique values. 

        if (in_array("embryo7day", $type)) {
            if ($res->RowCount() == 1) {
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $ret['donor_dam_full_reg_number'] = $row['donor_dam_full_reg_number'];
                $ret['recipient_full_reg_number'] = $row['recipient_full_reg_number'];
            } else {  // Cannot be determined.;
                $ret['donor_dam_full_reg_number'] = '';
                $ret['recipient_full_reg_number'] = '';
            }
        } elseif (in_array("breeding", $type) && (count($answer2) == 1)) {
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $ret['donor_dam_full_reg_number'] = $row['donor_dam_full_reg_number'];
            $ret['recipient_full_reg_number'] = $row['recipient_full_reg_number'];
        } else {
            // Cannot be determined.;
            $ret['donor_dam_full_reg_number'] = '';
            $ret['recipient_full_reg_number'] = '';
        }

        return $ret;
    }

///chooses who the sire and recipient are for datagrid
    function chooseSireandRecipient($record) {


        $calvingTime = $record['event_time'];

        //does all the work
        $ret = self::chooseSireandRecipientSub($record['bovine_id'], $calvingTime);
        //print_r($ret);
        if ($ret['sire_full_reg_number'] == '') {
            return 'Cannot be determined.';
        } else {
            return $ret['sire_full_reg_number'];
        }
    }

    function createCanadianRegNumber($calfBreedAbbrev, $animalSexAbbrev, $calf_rfid_number) {
        
        $calf_patial_rfid_number = ltrim( substr($calf_rfid_number, 3), '0');  //drop leading 124 + zeros
        
        $calfBreedAbbrev = strtoupper($calfBreedAbbrev); //make calpitalized HO
        $animalSexAbbrev = strtoupper($animalSexAbbrev); //make sure capilitlaized M or F letter
        //create full registration number (assumes CANADA).
        $calf_full_reg_number = $calfBreedAbbrev . 'CAN' . $animalSexAbbrev . $calf_patial_rfid_number;
        return $calf_full_reg_number;
    }

    function printFormattedVaccination($record) {

        if (is_numeric($record['vaccination']) == true) {

            return 'yes';
        } else {
            return 'no';
        }
    }

    
    

    function printCalfTwinOrNot($twin) {



        switch ($twin) {
            case 1:
                return 'Single';
                break;
            case 2:
                return 'Twin';
                break;
            case 3:
                return 'Triplet';
                break;
            default:
                throw new Exception('Unknow number of calves for a lactation. Should be single, twin, etc.');
                return null;
                break;
        }
    }

    function printActionButtons($record, $callBackURL = null) {

        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.calving_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if ($record['calving_event_id'] == $row['id']) {
            $deleteButtonAsString = self::deleteQuickForm($record['calving_event_id'], $callBackURL); //show delete button, with estrus table id as index for delete.
        }
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $deleteButtonAsString = self::deleteQuickForm($record['calving_event_id'], $callBackURL); //show delete button, with estrus table id as index for delete.
        }



        return $deleteButtonAsString;
    }

    /////////////////////////////////////////////////////////////////////

    function deleteQuickForm($calvingEventID, $callBackURL) {

        //$estrusID=85;

        $form = new HTML_QuickForm("calvingEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'calvingEventID', $calvingEventID);
        $form->addElement('submit', 'btnDelete', 'Delete');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $calvingEventID = $form->getElementValue('calvingEventID');



            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //look up what the lactation id
                //The DB using foreign keys will cascade delete from lacation->all calving_events
                //thus we delete the lacation and even if there are triplets, everything goes.
                //BOVINE table does not get deleted because we can't have a 1:1 foreign key relationship, because if we buy cows, they don't have a caliving ease.
                //we delete it seperately.
                $res = $GLOBALS['pdo']->query("SELECT lactation_id,calf_rfid_number FROM bovinemanagement.calving_event WHERE id=$calvingEventID");

                $row = $res->fetch(PDO::FETCH_ASSOC);
                $lactation_id = $row['lactation_id'];


                //now lookup anything connected to the calcing_event and delete in bovine table.
                $res2 = $GLOBALS['pdo']->query("SELECT calf_bovine_id FROM bovinemanagement.calving_event WHERE lactation_id='$lactation_id'");

                while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {

                    //delete all bovine records created for this lactation.
                    if (!empty($row2['calf_bovine_id'])) {
                        $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.bovine where id={$row2['calf_bovine_id']}");
                    }
                }

                //this deletes lacation and cascades to all calving_events
                $res2 = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.lactation where id='$lactation_id'");

                $GLOBALS['pdo']->commit();
            } catch (PDOException $e) {
                try {
                    $GLOBALS['pdo']->rollBack();
                } catch (Exception $e2) {
                    
                }
                echo "Failed: " . $e->getMessage();
            }

            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    

//end function
//passes along error info.
    public function error() {
        $error = array();

        $error = self::checkDataConsistencyWithHolsteinCanadaWhoWho();


        return $error;
    }

    /* checks to see if the sire and dam are correct with holstein canada data */
    /* relies on holstein canada data being up to date.
      /* usually catches embryo transfers, but could also be a breeding error. */

    function checkDataConsistencyWithHolsteinCanadaWhoWho() {
        $errorSub = array();

        $sql = "With temp as (
SELECT *, holstein_canada_data.dam_breed || holstein_canada_data.dam_country || holstein_canada_data.dam_sex || holstein_canada_data.dam_reg_no  as hol_dam_full_reg_number,
holstein_canada_data.sire_breed || holstein_canada_data.sire_country || holstein_canada_data.sire_sex || holstein_canada_data.sire_reg_no  as hol_sire_full_reg_number

 FROM bovinemanagement.bovine LEFT JOIN batch.holstein_canada_data ON holstein_canada_data.breed || holstein_canada_data.country || holstein_canada_data.sex || holstein_canada_data.reg_no = bovine.full_reg_number
)
SELECT DISTINCT(id) ,id, local_number,full_reg_number, sire_full_reg_number,dam_full_reg_number, recipient_full_reg_number,	hol_sire_full_reg_number, hol_dam_full_reg_number
 FROM temp WHERE temp.dam_full_reg_number != temp.hol_dam_full_reg_number";

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $str = "Holstein Sire: {$row['hol_sire_full_reg_number']} / LR Sire: {$row['sire_full_reg_number']} &nbsp;&nbsp;" .
                    "Holstein Dam: {$row['hol_dam_full_reg_number']} / LR Sire: {$row['dam_full_reg_number']} &nbsp;&nbsp;" .
                    "LR Recipient: {$row['recipient_full_reg_number']} &nbsp;.";

            $errorSub[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 3, $row['local_number'] . " Sire/Dam Data inconsistent: $str");
        }

        return $errorSub;
    }

    public function AllCalvings7Days($request) {
        $numberToshow = 100;


        $sql = <<<SQL
   Select calving_event_ease_types.name as calving_ease, bovine.full_name, bovine.local_number,bovine.id as bovine_id,
    bovine.id as conceiver_id,calving_event.event_time as birth_time, calving_event.id,calving_event.update_time,calving_event.event_time,
    calving_event.userid,calving_event.create_time,calving_event.alive_or_dead,calving_event.calf_rfid_number,calving_event.calf_sex,
    calving_event.id as calving_event_id,calving_event.calf_mass,calving_event.lactation_id,bovine.full_reg_number,bovine.birth_date as dam_birth_date,
(SELECT  right(calf_rfid_number::varchar,local_num_sig_digits)::numeric FROM bovinemanagement.eartag_valid where rfid_range @> calf_rfid_number::numeric limit 1 ) as calf_local_number,
(SELECT id from bovinemanagement.bovine where calf_rfid_number::numeric=rfid_number limit 1) as calf_bovine_id
,(SELECT count(id) as count FROM bovinemanagement.calving_event WHERE lactation_id=lactation.id) as twin    
,(SELECT medicine_administered.id as med_id FROM bovinemanagement.medicine_administered WHERE medicine_index=3 AND bovine_id=calving_event.calf_bovine_id ORDER BY medicine_administered.id DESC LIMIT 1) as vaccination

   FROM bovinemanagement.calving_event 
    LEFT JOIN bovinemanagement.lactation ON lactation.id=calving_event.lactation_id 
    LEFT JOIN bovinemanagement.calving_event_ease_types ON calving_event_ease_types.id = calving_event.calving_ease 
    LEFT JOIN bovinemanagement.bovine ON bovine.id=lactation.bovine_id ORDER BY calving_event.event_time  DESC LIMIT   $numberToshow
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $row['event_time_raw'] = strtotime($row['birth_time']);
            $row['calving_time'] = date("l M d H:00 Y", strtotime($row['birth_time']));
            $row['dam_birth_date'] = date("M d, Y", strtotime($row['dam_birth_date']));
            $row['dam'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['calf'] = JQueryDataTable::formattedAnimalNameLink($row['calf_bovine_id'], $row['calf_local_number'], '');
            $row['calving_ease'] = $row['calving_ease'];
            $row['calf_weight'] = $row['calf_mass'];
            $row['calf_sex'] = $row['calf_sex'];
            $row['calf_vitality'] = $row['alive_or_dead'];
            $row['calf_twin'] = $this->printCalfTwinOrNot($row['twin']);
            $row['calf_potentialSire'] = $this->potentialSireandRecipient($row);
            $row['calf_chosenSire'] = $this->chooseSireandRecipient($row);
            $row['calf_vaccinated'] = $this->printFormattedVaccination($row);


            $row['empploee'] = $row['userid'];
            //  $row['action'] = $this->printActionButtons($row['estrus_id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            $row['action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayLast100DaysCalvingEvents($mode = 'default') {


        $numberToshow = 300;
        //be smart here and decide how many to show by going back to the 1st of the preceeding month for milk tester.
        if ($mode == 'milkTestInputs') {
            $query = "SELECT id,event_time FROM bovinemanagement.calving_event WHERE event_time >= date_trunc('month',CURRENT_DATE - interval '8 weeks') OR event_time IS NULL";
            $res2 = $GLOBALS['pdo']->query($query);
            $numberPerPage = $res2->rowCount();
        } else {
            $numberPerPage = 10;
        }

        if ($mode == 'default') {
            $showColumn = 'false';
            $showColumn2 = 'true';
        } elseif ($mode == 'milkTestInputs') {
            $showColumn = 'true';
            $showColumn2 = 'false';
        } else {
            Throw new Exception("Error: no proper mode input into method.");
        }

        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },  
            {
               data: "calving_time",
               title: "calving_time",
               type: "date"
            },
            {
                data: "dam",
                title: "Dam",
                 type: "num-html"
            },
             {
                data: "full_reg_number",
                title: "Dam Reg #",
                 
            },
             {
                data: "dam_birth_date",
                title: "Dam Birth Date",
                type: "date"
                  
            },
            {
                data: "calving_ease",
                title: "Calving Ease"
            },
            {
                data: "calf_weight",
                title: "Calf Mass"
            },
            {
                data: "calf_sex",
                title: "Calf Sex"
            },
            {
                data: "calf_vitality",
                title: "Alive or Dead?"
            },
            {
                data: "calf_twin",
                title: "Tiwn?"
            },
            {
                data: "calf_rfid_number",
                title: "Calf RFID #"
            },
            {
                data: "calf",
                title: "Calf Local #"
            },
             {
                data: "calf_potentialSire",
                title: "Potential Sire(s)"
            },
            {
                data: "calf_chosenSire",
                title: "Chosen Sire"
            },
            {
                data: "calf_vaccinated",
                title: "Vaccianted?"
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
        print($dt->start(43, 'AllCalvings7Days', null, $columnsStr));
    }

}

//end calving class

/**
 * calving event form
 * 
 * 
 */
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';

Class QF2_CalvingEvent extends Quickform2Helper {

    function definition($form) {
        $numberOfCalves = $this->mode;


        //$dateArray=Misc::createDailyDatesForLastYearAtNoon(); //hourly array, with unix timestamp as index.
        $dateArray = Misc::createHourlyDatesForLastSevenDays();
        //grab a list of cows with a due date.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number,calculated_potential_due_date from bovinemanagement.pregnant_view ORDER BY local_number");

        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $splitted = preg_split("/[\s,]+/", $row[1]);
            $aniNumber[$row[0]] = $row[2] . " - " . end($splitted) . " (due: " . $row[3] . ")";
        }

        //grab list of calving ease types
        //grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,name FROM bovinemanagement.calving_event_ease_types ORDER BY id");

        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $calvingEaseTypes[$row[0]] = $row[1];
        }

        
        /*
         * find next ear tag numbers in series. using 2 series holstein and bull tags.
         * 
         */
        $tagArray=array();
        //find last eartag used, purebred 
        $sql1 = "SELECT rfid_number, purebred,  right(rfid_number::varchar,local_num_sig_digits)::numeric as local_number
                 FROM bovinemanagement.bovine
                 LEFT JOIN bovinemanagement.eartag_valid ON rfid_range @> rfid_number
                 WHERE purebred=true
                 ORDER BY birth_date desc,rfid_number desc
                 LIMIT 1
               ";
        $res1 = $GLOBALS['pdo']->query($sql1); 
        $row1 = $res1->fetch();
        //
        //find last eartag used, grade 
        $sql2 = "SELECT rfid_number, purebred,  right(rfid_number::varchar,local_num_sig_digits)::numeric as local_number
                 FROM bovinemanagement.bovine
                 LEFT JOIN bovinemanagement.eartag_valid ON rfid_range @> rfid_number
                 WHERE purebred=false
                 ORDER BY birth_date desc,rfid_number desc
                 LIMIT 1
               ";
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch();
        
        //FIXME: can overflow eartag range number by +1, I think.
        
         $tagArray[null] = "Please Select one..."; //dummy value
            //show first 2 male and female, mix them up to reducde entry error. 
         $tagArray[(intval($row1['rfid_number'])+1)]= 'Female '. (intval($row1['local_number'])+1) .' ('. (intval($row1['rfid_number'])+1) .')';
         $tagArray[(intval($row2['rfid_number'])+1)]= 'Male '. (intval($row2['local_number'])+1) .' ('. (intval($row2['rfid_number'])+1) .')';
         $tagArray[(intval($row1['rfid_number'])+2)]= 'Female '. (intval($row1['local_number'])+2) .' ('. (intval($row1['rfid_number'])+2) .')';
         $tagArray[(intval($row2['rfid_number'])+2)]= 'Male '. (intval($row2['local_number'])+2) .' ('. (intval($row2['rfid_number'])+2) .')';
        
         $tagArray['dead']='Dead';
        
        
        /* input data arrays */
        $massArray = array(null => 'Please Select One', 20 => '20', 21 => '21', 22 => '22', 23 => '23', 24 => '24', 25 => '25', 26 => '26', 27 => '27', 28 => '28', 29 => '29', 30 => '30', 31 => '31', 32 => '32', 33 => '33', 34 => '34', 35 => '35', 36 => '36', 37 => '37', 38 => '38', 39 => '39', 40 => '40', 41 => '41', 42 => '42', 43 => '43', 44 => '44', 45 => '45', 46 => '46', 47 => '47', 48 => '48', 49 => '49', 50 => '50', 51 => '51', 52 => '52', 53 => '53', 54 => '54', 55 => '55', 56 => '56', 57 => '57', 58 => '58', 59 => '59', 60 => '60', 61 => '61', 62 => '62', 63 => '63', 64 => '64', 65 => '65');
        $deadAliveArray = array(null => 'Please Select One', 'alive' => 'Alive', 'dead' => 'Dead');
        $deliveryMethodArray = $calvingEaseTypes;
        $sexArray = array(null => 'Please Select One', 'male' => 'Male', 'female' => 'Female');
        //$twinArray=array(null=>'Please Select One',1=>'Single',2=>'Twins',3=>'Triplets');
        $vaccArray = array(null => 'Please Select One', 'no' => 'No', 'yes' => 'Yes');
        $colourArray = array(null => 'Please Select One', 'b&w' => 'Black & White', 'r&w' => 'Red & White');

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //GENERAL
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($numberOfCalves == 1) {
            $form->addElement('hidden', 'calfTwin')->setValue('1');
        } elseif ($numberOfCalves == 2) {
            $form->addElement('hidden', 'calfTwin')->setValue('2');
        } else {
            throw new Exception("Error: Must be single or twin!.");
        }

        //defaults at run time for calf 1.
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
            'animalNumber' => 0,
            'calfDeliveryMethod1' => 0,
            'calfDeadAlive1' => 'alive',
            'calfVaccination1' => 0,
            'calfColour1' => 'b&w',
            'calfEarTagNum1' => 0
        )));

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //DAM CALF
        $form->addFieldset('header')->setLabel('Cow Who Calved'); //set title
        $dam = $form->addElement('select', 'animalNumber', null, array('label' => 'Cow Animal Number', 'options' => $aniNumber));
        $dam->addRule("required", "Animal Number is required.");
        $dam->addRule('callback', 'ERROR: Cow is already entered as having calved.', array('callback' => array($this, 'checkAlreadyCalved'), 'arguments' => array('animalNumber')));
        $form->addElement('select', 'calfDeliveryMethod1', null, array('label' => 'Calf Delivery Method:', 'options' => $deliveryMethodArray))->addRule("required", "How the calf was delievered is required.");

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //FIRST CALF
        $form->addFieldset('header')->setLabel('Calf'); //set title
        $form->addElement('select', 'dateEvent1', null, array('options' => $dateArray, 'label' => 'Estimated Time of Birth'))->addRule('required', 'Date of calving is required');
        $form->addElement('select', 'calfMass1', null, array('label' => 'Calf Weight (kg)', 'options' => $massArray))->addRule('required', 'The weight of the calf is required');
        $form->addElement('select', 'calfSex1', null, array('label' => 'Calf Sex:', 'options' => $sexArray))->addRule('required', 'The sex of the calf is required');
        $form->addElement('select', 'calfDeadAlive1', null, array('label' => 'Calf Alive or Dead:', 'options' => $deadAliveArray))->addRule('required', 'Whether calf is Dead or Alive is required');
        //
        $form->addElement('select', 'calfEarTagNum1', null, array('label' => 'Calf Full Ear Tag Number','options' => $tagArray))->addRule('required', 'EarTag Number or Dead (no tag) is required.');
        $form->addElement('select', 'calfColour1', null, array('label' => 'Calf Hair Colour', 'options' => $colourArray))->addRule('required', 'Calf Hair Colour is required');
        $form->addElement('select', 'calfVaccination1', null, array('label' => 'Calf Vaccinated? (calf-guard and first defence)', 'options' => $vaccArray))->addRule('required', 'Calf vaccination status is required');
        $form->addElement('hidden', 'calfBreed1')->setValue('HO'); //don't need to choose because currently it is always holstein.
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // SECOND CALF
        if ($numberOfCalves == 2) {
            //defaults at run time for calf 2.
            $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
                'animalNumber' => 0,
                'calfDeliveryMethod1' => 5, //if its a twin it is always malpresentation, valacta rule.
                'calfDeliveryMethod2' => 0,
                'calfDeadAlive2' => 'alive',
                'calfVaccination2' => 0,
                'calfColour2' => 'b&w',
                 'calfEarTagNum1' => 0,
                 'calfEarTagNum2' => 0
            )));
            //SECOND CALF (Twin)
            $fieldset = $form->addFieldset('header')->setLabel('Calf 2'); //set title
            $form->addElement('select', 'dateEvent2', null, array('options' => $dateArray, 'label' => 'Estimated Time of Birth'))->addRule('required', 'Date of calving is required');
            $form->addElement('select', 'calfMass2', null, array('label' => 'Weight of Calf (kg)', 'options' => $massArray))->addRule('required', 'The weight of the calf is required');
            $form->addElement('select', 'calfSex2', null, array('label' => 'Calf Sex:', 'options' => $sexArray))->addRule('required', 'The sex of the calf is required');
            $form->addElement('select', 'calfDeadAlive2', null, array('label' => 'Calf Alive or Dead:', 'options' => $deadAliveArray))->addRule('required', 'Whether calf is Dead or Alive is required');
            $form->addElement('select', 'calfEarTagNum2', null, array('label' => 'Calf Full Ear Tag Number','options' => $tagArray))->addRule('required', 'EarTag Number or Dead (no tag) is required.');
            $form->addElement('select', 'calfColour2', null, array('label' => 'Calf Hair Colour', 'options' => $colourArray))->addRule('required', 'Calf Hair Colour is required');
            $form->addElement('select', 'calfVaccination2', null, array('label' => 'Calf Vaccinated? (calf-guard and first defence)', 'options' => $vaccArray))->addRule('required', 'Calf vaccination status is required');
            $form->addElement('hidden', 'calfBreed2', 'HO'); //don't need to choose because currently it is always holstein.  
           
        }//end twin
        //no code for triplets, too rare to code. Manually put in DB instead.

        return $form; //required       
    }

    function process($value) {
        
       
  $numberOfCalves = $this->mode;
        //get values
        $pageid = $value['pageid'];
        $bovine_id = $value['animalNumber'];
        
        $event_timeFixed = date('Y-m-d H:i:s', $value['dateEvent1']);  //traditional
        if ($value['calfTwin'] == 2) {
           
            $event_timeFixed2 = date('Y-m-d H:i:s', $value['dateEvent2']);  //traditional
        }


        //change dead calves for eartag to nulls.
         if ($value['calfEarTagNum1'] == 'dead') {
           $value['calfEarTagNum1']=null;
        }
         if ($value['calfEarTagNum2'] == 'dead') {
           $value['calfEarTagNum2']=null;
        }
        
        ///////////////////////////////////////
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            $last_lactation_id = self::addLactationRecord($bovine_id, $event_timeFixed);

            $calving_event_id1 = self::addCalvingRecord($event_timeFixed, $value['calfSex1'], $value['calfTwin'], $value['calfDeadAlive1'], $value['calfDeliveryMethod1'], $value['calfEarTagNum1'], $value['calfMass1'], $last_lactation_id);

            //twin (same delivery method)
            if ($numberOfCalves == 2) {
                $calving_event_id2 = self::addCalvingRecord($event_timeFixed2, $value['calfSex2'], $value['calfTwin'], $value['calfDeadAlive2'], $value['calfDeliveryMethod1'], $value['calfEarTagNum2'], $value['calfMass2'], $last_lactation_id);
            }
    
                        
            if (intval($value['calfTwin']) == 1) {
    
                if (($value['calfDeadAlive1'] == 'alive')) {
                    
                    $calf_bovine_id = self::addCalfToDB($bovine_id, $last_lactation_id, $value['calfEarTagNum1'], $value['calfColour1'], $value['calfBreed1'], $event_timeFixed, $value['calfSex1']);
                    
                    $this->addBovineIDToCalvingRecord($calf_bovine_id, $calving_event_id1);
                    if ($value['calfVaccination1'] == 'yes') {
                        self::addVaccinationInfo($calf_bovine_id, $event_timeFixed); //record whether female calf was vaccinated or not.
                    }
                }
               
            }
            //twin case //FIXME: does not handle mixed twins, if they are mixed we have to support a beef tag for the female.
            elseif (intval($value['calfTwin']) == 2) {

                if (($value['calfDeadAlive1'] == 'alive')) {
                    $calf_bovine_id1 = self::addCalfToDB($bovine_id, $last_lactation_id, $value['calfEarTagNum1'], $value['calfColour1'], $value['calfBreed1'], $event_timeFixed, $value['calfSex1']);
                    $this->addBovineIDToCalvingRecord($calf_bovine_id1, $calving_event_id1);
                    if ($value['calfVaccination1'] == 'yes') {
                        self::addVaccinationInfo($calf_bovine_id1, $event_timeFixed); //record whether female calf was vaccinated or not.
                    }
                }

                if (($value['calfDeadAlive2'] == 'alive')) {
                    $calf_bovine_id2 = self::addCalfToDB($bovine_id, $last_lactation_id, $value['calfEarTagNum2'], $value['calfColour2'], $value['calfBreed2'], $event_timeFixed2, $value['calfSex2']);
                    $this->addBovineIDToCalvingRecord($calf_bovine_id2, $calving_event_id2);
                    if ($value['calfVaccination2'] == 'yes') {
                        self::addVaccinationInfo($calf_bovine_id2, $event_timeFixed2); //record whether female calf was vaccinated or not.
                    }
                }
            }else {
                throw new Exception("Error Twin Must be value 1 or 2.");
            }

            // determine if the commit or rollback


            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
    }

    /////////////////// CUSTOM RULES FUNCTIONS/////////////////////////////////////

    

    //if a twin is entered throw up error message. Javascript.
    function checkTwin($value) {
        if ($value > 1) { //twins or triplets fail.
            return 0; //fail
        } else { //pass
            return 1;
        }
    }

    //if a the cow has not due date throw up and error message.
    function checkAlreadyCalved($value) {

        $bovine_id = $value;

        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.pregnant_view WHERE id=$bovine_id");
        if ($res->rowCount() >= 1) {
            $returnStatement = true; //means the cow is still pregnant.
        } else {
            $returnStatement = false; //means the cow is not pregnant and thus can't calve again.
        }

        return $returnStatement;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // INSERTS TO DB
    function addCalvingRecord($event_time, $calfSex, $twin, $calfDeadAlive, $calfDeliveryMethod, $calf_rfid_number, $calf_mass, $last_lactation_id) {

        //convert twin boolean to integer.
        if ($twin == TRUE) {
            $twin = 'true';
        } else {
            $twin = 'false';
        }


        //add calving data
        //don't put id value, uses next one in DB.
        $query = "INSERT INTO bovinemanagement.calving_event
(event_time   ,calf_sex ,alive_or_dead  ,calving_ease ,calf_rfid_number,calf_mass   ,lactation_id  ,userid) VALUES (:event_time,:calfSex,:calfDeadAlive,:calfDeliveryMethod,:calf_rfid_number, :calf_mass,:last_lactation_id,:userid);";

        $stmt = $GLOBALS['pdo']->prepare($query);
        $params = array
            (
            'event_time' => $event_time,
            'calfSex' => $calfSex,
            'calfDeadAlive' => $calfDeadAlive,
            'calfDeliveryMethod' => (int) $calfDeliveryMethod,
            'calf_rfid_number' => empty($calf_rfid_number) ? null : $calf_rfid_number,
            'calf_mass' => (int) $calf_mass,
            'last_lactation_id' => $last_lactation_id,
            'userid' => $GLOBALS['auth']->getUsername()
        );

        $stmt->execute($params);

        //get the last calving_event id.
        $last_calving_event_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.calving_event_id_seq');

        return $last_calving_event_id;
    }

    /*
     * we do an update here, for code readability. we don't know bovine_id when we do inital insert.
     */

    function addBovineIDToCalvingRecord($calf_bovine_id, $calving_event_id) {
        $query = "UPDATE bovinemanagement.calving_event SET calf_bovine_id=$calf_bovine_id WHERE calving_event.id=$calving_event_id";
        $stmt = $GLOBALS['pdo']->exec($query);
    }

    function addLactationRecord($bovine_id, $event_time) {


        //add lactation data
        //don't put id value, uses next one in DB.
        $query = "INSERT INTO bovinemanagement.lactation (bovine_id,fresh_date,dry_date,userid) VALUES ($bovine_id,'$event_time',NULL,'{$GLOBALS['auth']->getUsername()}');";

        $res = $GLOBALS['pdo']->exec($query);

        //get the last lactation id.
        $last_lactation_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.lactation_id_seq');

        return $last_lactation_id;
    }

    //add an alive calf with tag to DB
    function addCalfToDB($bovine_id, $last_lactation_id, $calf_rfid_number, $calfColour, $calfBreed, $event_time, $calfsex) {

        
        //RFID number should be valid, because it is from a select form and calf is alive. make sure it is.
 
        $res2 = $GLOBALS['pdo']->query("SELECT bovinemanagement.rfid_valid($calf_rfid_number::numeric)");
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        
        
        if ($row2['rfid_valid'] == false) {
            throw new Exception("Entered RFID number /registration number is not in a valid tag range. Either the tag was entered wrong or a new set of tags has not been entered as useable into the system.");
        }
        
        //tag is valid we know now, so get information on how to construct the local number.        
        $res3 = $GLOBALS['pdo']->query("SELECT  * from bovinemanagement.eartag_valid WHERE $calf_rfid_number::numeric(16,0) <@ rfid_range limit 1");
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);

        if ($calfsex == 'female') {
            $local_number = substr($calf_rfid_number, -1 * $row3['local_num_sig_digits'], $row3['local_num_sig_digits']);
            $local_number = $local_number + $row3['local_num_offset']; //correction factor because we didn't order tags correctly and local number doesn't match reg number.
            $calf_full_reg_number = TransitionCalving::createCanadianRegNumber('HO', 'F', $calf_rfid_number);
        } else { //male 
            $calf_full_reg_number = ''; //unregistered, so no number
            //unregistered calves never have a local num offset, thus just use how many are significant.
            $local_number = substr($calf_rfid_number, -1 * $row3['local_num_sig_digits'], $row3['local_num_sig_digits']);
        }

        
        //lookup what the dams full registration number is from the DB based on lactationid.
        $res = $GLOBALS['pdo']->query("SELECT full_reg_number FROM bovinemanagement.bovine WHERE bovine.id =$bovine_id LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $dam_full_reg_number = $row['full_reg_number'];

        //FIXME: code for recipients not implemented, need semen code to be written first.
        $recipient_full_reg_number = 'null';

        //create insert query for female calf into main db table for cows.
        $query = "INSERT INTO bovinemanagement.bovine (full_reg_number,full_name,colour,birth_date,death_date,sire_full_reg_number,dam_full_reg_number,local_number,rfid_number,recipient_full_reg_number,create_time)
VALUES (NULLIF('$calf_full_reg_number', ''),null,'b&w','$event_time',null,11111111111111111,'$dam_full_reg_number',$local_number,$calf_rfid_number,$recipient_full_reg_number,null)";

        $res = $GLOBALS['pdo']->exec($query);

        $calf_bovine_id = $GLOBALS['pdo']->lastinsertId('bovinemanagement.bovine_idx_seq');

        //give calf a location too, or it doesn't really exist.
        //Start is location 64, calving pen.
        $location_id = 64;
        $query = "INSERT INTO bovinemanagement.location_event (bovine_id, location_id, event_time, userid, transaction_id) VALUES ($calf_bovine_id,$location_id,'$event_time','{$GLOBALS['auth']->getUsername()}',null)";

        $res = $GLOBALS['pdo']->exec($query);


        return $calf_bovine_id;
    }

    function addVaccinationInfo($calf_bovine_id, $event_time) {

        //calf guard
        $medicine_index = 3; //hardcoded DB index, not good......
        $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,userid)
VALUES ($calf_bovine_id,$medicine_index,'$event_time',3,'ml','oral',null,'{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);

        //first defence
        $medicine_index = 91; //hardcoded DB index, not good......
        $query = "INSERT INTO bovinemanagement.medicine_administered (bovine_id,medicine_index,event_time,dosage,dosage_unit,location,note,userid)
VALUES ($calf_bovine_id,$medicine_index,'$event_time',1,'capsule','oral',null,'{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);
    }

    //called by HTML_QuickForm2_Rule sutomc class
    public function checkEarTagNumber($calfEarTagNum, $calfSex,$calfDeadAlive) {
        //defaults
        $shouldBeTrue1 = false;
        $shouldBeTrue2 = false;
        //check if true.
        $value = trim($calfEarTagNum); //trim off any spaces
        $shouldBeTrue1 = is_numeric($value); //check if it is a number.
        //check that the correct number of digits were entered.
        //male
        if (($calfSex == 'male') && (strlen($calfEarTagNum) == 15)) {

            $shouldBeTrue2 = true;
        }
        //female
        elseif (($calfSex == 'female') && (strlen($calfEarTagNum) == 8)) {
            $f = strlen($calfEarTagNum);
            $shouldBeTrue2 = true;
        } else {
            $shouldBeTrue2 = false;
        }

        //animal is dead, thus no number needed.
        if ($calfDeadAlive == 'dead') {
            return 1;
        } else if (($shouldBeTrue1 == true) && ($shouldBeTrue2 == true)) {
            return 1;
        } else {   //fail
            return 0;
        }
    }
    
    
}//end class
/*
 * complicated rule to check eartag number fir calf 1.
 * 
 */
class checkEarTagNumberCalf1 extends HTML_QuickForm2_Rule
{
    protected function validateOwner()
    {
        $arr=$this->owner->getValue();
        return QF2_CalvingEvent::checkEarTagNumber($arr['calfEarTagNum1'], $arr['calfSex1'],$arr['calfDeadAlive1']);
    }

    protected function setOwnerError()
    {
       //adding '-0', seems to be default way to name id. There also is not getElementByName method, so instead we get array and get first value. 
         $this->owner->getElementsByName('calfEarTagNum1')[0]->setError('ERROR: Improper number of digits (8 for female 4000XXXX, 15 for male, starting with 124XXXXXXXXXXX).');   
    }
    
}//end class

/*
 * complicated rule to check eartag number fir calf 2.
 * 
 */
class checkEarTagNumberCalf2 extends HTML_QuickForm2_Rule
{
    protected function validateOwner()
    {
        $arr=$this->owner->getValue();
        return QF2_CalvingEvent::checkEarTagNumber($arr['calfEarTagNum2'], $arr['calfSex2'],$arr['calfDeadAlive2']);
    }

    protected function setOwnerError()
    {
       //adding '-0', seems to be default way to name id. There also is not getElementByName method, so instead we get array and get first value. 
         $this->owner->getElementsByName('calfEarTagNum2')[0]->setError('ERROR: Improper number of digits (8 for female 4000XXXX, 15 for male, starting with 124XXXXXXXXXXX).');   
    }
    
}
//end class
?>
