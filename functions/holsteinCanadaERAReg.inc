<?php 

/** used to generate an Holstein Canada ERA file 
  //Based on the data format at:
  https://www.holstein.ca/Content/Pdfs/Registration/HelpfulTips/English/era_file_layout.pdf

 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/misc.inc');
}

  class HolsteinCanadaERA_NoConceptionEventException extends Exception {}

      class HolsteinCanadaERA {

    public function __construct($bovine_id) {

        ///run main function
        //$bovine_id = 1388; use for testing. 

        $this->main($bovine_id);
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        //nothing
    }

    //main function....
    public function main($bovine_id) {
        $strFile = $this->generateERAFileForGivenAnimal($bovine_id);
        //print($strFile); //DEBUG
        $this->processAndUploadERAFile($bovine_id, $strFile, true);
    }

    /**
     * This is called after we have the file contents generated in string format.
     * It then creates txt(era) and zip files. Then uploads the zip.
     * @param type $strFile
     * @return type 
     */
    private function processAndUploadERAFile($bovine_id, $strFile, $ftp = true) {
        //find local number
        $sql = "SELECT local_number FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $uuidShort = self::writeOutERAFile($strFile, $row['local_number']);
                print("Deciding to ftp $uuidShort<br/>");
        //ftp the file when told to.
        if ($ftp == true) {

            $sql3 = "SELECT bovine_id,short_uuid,event_time_reg_sent FROM batch.holstein_canada_registered WHERE bovine_id=$bovine_id LIMIT 1";
            $res3 = $GLOBALS ['pdo']->query($sql3);
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);

            if ($row3['bovine_id'] != $bovine_id) { //no rows found in db, so insert one. 

                $query = "INSERT INTO batch.holstein_canada_registered (bovine_id,local_reg_status,parentage_verified) VALUES ($bovine_id,'false','true')"; //so insert one.
                $res5 = $GLOBALS['pdo']->exec($query);
            }   
            
            //now look again
            $sql3 = "SELECT bovine_id,short_uuid,event_time_reg_sent FROM batch.holstein_canada_registered WHERE bovine_id=$bovine_id LIMIT 1";
            $res3 = $GLOBALS ['pdo']->query($sql3);
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);
            
            print_r($row3);
            //if not submitted via ftp before, it must have a blank short_uuid and event_time_reg_sent in DB for that bovines entry.
            if ((empty($row3['short_uuid']) ) AND ($res3->rowCount() == 1)) {

                
            
               
                //ftp file to holstein canada
                $uuidShort2 = self::ftpERAtoHolsteinCanada($uuidShort, $row['local_number']);

                //now do a db update to say we were successful sending it in..  
                $sql2 = "UPDATE batch.holstein_canada_registered SET local_reg_status='true',short_uuid='$uuidShort2',event_time_reg_sent='now()' WHERE bovine_id=$bovine_id";
                $res2 = $GLOBALS ['pdo']->exec($sql2);
             
            } else {
                throw new Exception("ERROR: ERA file already submitted for bovine_id=$bovine_id and file partial name: $uuidShort.<br />\n\r");
            }
        } else {
            print("<br />Skipping ftp of file $uuidShort, as directed.<br />" . "\n\r");
        }


        return $uuidShort2; //on sucess the short uuid is returned.
    }

    public function generateERAFileForGivenAnimal($bovine_id) {

        //if it doesn't check out abort.
        try {
            $eraFormatArr = self::checkERAFormat(self::ERAFormat());
        } catch (Exception $e) {
            echo 'Caught exception: ', $e->getMessage(), "\n";
            exit();
        }

        //now get to the main program.
        print("OK, let's go. \n\r");

        $answerArr = self::singleCowRegGenerator($eraFormatArr, $bovine_id);

        // print_r2($answerArr);
        //check that everything that is mandatory has an answer.
        foreach ($answerArr as $key => $val) {
            if (($val['required'] == 'M') AND ($val['answer'] == null)) {
                // print("$key holstein is probably required <br>");
            }
            if (($val['lr_required'] == 'T') AND ($val['answer'] == null)) {
                //  print("$key is LR required <br>");
            }

            if (strlen($val['answer']) > $val['length']) {
                //  print("$key is too long! <br>");
            }
        }

        //calculate how many lines for 01DEND -> No. of data fields sent for the animal including 01DEND record
        foreach ($answerArr as $key => $val) {

            if ($key == '01HEND') {
                $answerArr['01HEND']['answer'] = '';
            }

            //search through array and count al 01D lines.
            if ($key == '01DEND') {
                $counter = 0;
                foreach ($answerArr as $key1 => $val1) {
                    if ((strstr($key1, '01D') == true) AND ($val1['lr_required'] == 'T')) {
                        $counter++;
                    }
                }


                $answerArr['01DEND']['answer'] = "$counter";
            }

            //this would have to be changed dynamically for registering more than one animal.
            if ($key == '01EEND') {
                $answerArr['01EEND']['answer'] = '3'; //mnumber of animals to reg + 2.
            }
        }


        //now generate array of valid data
        $outArray = array();
        foreach ($answerArr as $key => $val) {
            if (($val['lr_required'] == 'T')) {
                $outArray[] = $val['field_no'] . trim($val['answer']);
            }
        }



        //print out for web page
        foreach ($outArray as $key => $val) {
            print($val . "<br/>");
        }

        //print out for file
        $strFile = '';
        foreach ($outArray as $key => $val) {
            $strFile = $strFile . ($val . "\n");
        }

        return $strFile;
    }

//end function

    /**
     * Uploads the era zip file to holstein Canada.
     * 
     * @param type $uuidShort
     * @param type $local_number
     * @return boolean
     * @throws Exception 
     */
    function ftpERAtoHolsteinCanada($uuidShort, $local_number) {
        //use the $uuid to create the file location again.
        $fileName2 = $GLOBALS['config']['HOLSTEIN_CANADA']['era_file_prefix'].'_' . $local_number . '_' . $uuidShort . '.zip';
        $pathLocal = '/tmp/';
        $pathRemote = '/pub/ERA/';
        //login credentials
        $ftp_server = 'ftp.holstein.ca';
        $ftp_user_name = $GLOBALS['config']['HOLSTEIN_CANADA']['era_username']; //from config file
        $ftp_user_pass = $GLOBALS['config']['HOLSTEIN_CANADA']['era_password']; //from config file
        // set up basic connection
        $conn_id = ftp_connect($ftp_server);

        // login with username and password
        $login_result = ftp_login($conn_id, $ftp_user_name, $ftp_user_pass);

        // check connection
        if ((!$conn_id) || (!$login_result)) {
            throw new Exception("ERROR: ftp connection failed to $ftp_server for file $fileName2." . "<br />\n\r");
        } else {
            echo "<br />\n\r" . "Connected to $ftp_server, for user $ftp_user_name" . "<br />\n\r";
        }

        // upload the file
        ftp_pasv($conn_id, true); 
        $upload = ftp_put($conn_id, $pathRemote . $fileName2, $pathLocal . $fileName2, FTP_BINARY);

        // check upload status
        if (!$upload) {
            throw new Exception("ERROR: ftp file upload failed to $ftp_server for file $fileName2." . "<br />\n\r");
        } else {
            echo "Uploaded file to $ftp_server as $fileName2." . "<br />\n\r";
        }

        // close the FTP stream 
        ftp_close($conn_id);

        return $uuidShort;
    }

    /*
     * Takes an string of ERA data and writes it out to a file.
     */

    function writeOutERAFile($strIn, $local_number) {

        $tempDir = '/tmp/';


        //create a unique id for the file, which was too longf or holstein canada, 
        //so now we use a shorter one, that maybe isn't unique. 
        $uuidShort = substr(Misc::uuid_create(), 19, 32);

        //create a fileName prefix.
        $fileName1 = $GLOBALS['config']['HOLSTEIN_CANADA']['era_file_prefix'].'_' . $local_number . '_' . $uuidShort . '.era';
        $fileName2 = $GLOBALS['config']['HOLSTEIN_CANADA']['era_file_prefix'].'_' . $local_number . '_' . $uuidShort . '.zip';

        //write out era file.
        $fp = fopen($tempDir . $fileName1, 'w');
        fwrite($fp, $strIn);
        fclose($fp);
        
        //php has awhole zip class, but didn't seem to work, this is easier, but less secure.
        $output = shell_exec("cd /tmp/; zip $fileName2 $fileName1");
        if (!$output) {
            throw new Exception("ERROR: zipping file failed $fileName2. Try apt-get install zip" . "<br />\n\r");
        } else {
            echo "zipped file $fileName2 successfully." . "<br />\n\r";
        }

        //return links to the files.
        print('<br />');
        print('<br />');
        print('<a href="file://' . $tempDir . $fileName1 . '">' . $local_number . ' ERA file</a>');
        print('<br />');
        print('<a href="file://' . $tempDir . $fileName2 . '">' . $local_number . ' ZIP file</a>');



//pass along 
        return $uuidShort;
    }

    /* giant decision maker */

    private function singleCowRegGenerator($eraFormatArr, $bovine_id) {

        //contstants
        $numRegistrations = 1;
        $numSequence = 0; //really starts at one with code.
        $prefix = strtoupper($GLOBALS['config']['HOLSTEIN_CANADA']['prefix']);


        //general info about animal to register
        $sql = "SELECT * FROM bovinemanagement.bovine WHERE id=$bovine_id LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        //information on calving and twin.
        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['full_reg_number']);
        $sql2 =
                "SELECT id,event_time,lactation_id,calf_mass,(SELECT count(lactation_id) FROM bovinemanagement.calving_event where lactation_id=xxx.lactation_id) as twin_count
FROM bovinemanagement.calving_event xxx
WHERE calf_rfid_number = '{$ret['number']}' LIMIT 1";
        $res2 = $GLOBALS ['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);

        if ($res2->rowCount() == 0) {
            throw new Exception("Error: Calving information not found for bovineID $bovine_id." . " </br>\n\r");
        }

        $twin_sex=null;
        if ($row2['twin_count'] >= 2) {
            //we have twins ( or more), look up sex of the another one
            //NOTE: the data format daoesn't seem to support triplets, I assume they catch it based on birth date and raise all kinds of flags.
            //      So it doesn't matter which calf of the triplets we choose the sex from.        
            //NOTE2: because one twin could have died we could have null partial reg number and thus "distince from" is needed
            $sql3 = "SELECT calf_sex FROM bovinemanagement.calving_event WHERE lactation_id={$row2['lactation_id']} AND calf_rfid_number::integer IS DISTINCT FROM '{$ret['number']}'::integer LIMIT 1";
            $res3 = $GLOBALS ['pdo']->query($sql3);
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);

            
            //holstein canada needs a Y or N answer for this.
            if (trim($row3['calf_sex']) == 'male') {
                $twin_sex = 'Y';
            } elseif (trim($row3['calf_sex']) == 'female') {
                $twin_sex = 'N';
            } else {
                throw new Exception("Error: We have a twin situation for bovineID $bovine_id, but sex of twin cannot be determined." . " </br>\n\r");
            }
        }//end twin stuff
        //general info about dam
        $sql4 = "SELECT * FROM bovinemanagement.bovine WHERE full_reg_number='{$row['dam_full_reg_number']}' LIMIT 1";
        $res4 = $GLOBALS ['pdo']->query($sql4);
        $row_dam = $res4->fetch(PDO::FETCH_ASSOC);

        //sire and conception information, from the dam or recipient (whoever had the calf).
        $conceptionArr = self::listMostLikelyConceptionEvents($row2['event_time'], $row_dam['id'], true);
        //print_r2($conceptionArr);

        //find the actual name of the calf from the calf nameing db table.
        $sql5 = "SELECT potential_name FROM bovinemanagement.calf_potential_name WHERE bovine_id=$bovine_id LIMIT 1";
        $res5 = $GLOBALS ['pdo']->query($sql5);
        $row_name = $res5->fetch(PDO::FETCH_ASSOC);

        //array for answers w/ previous info from case statement actions.
        $eraAnswerArr = $eraFormatArr;

        //loop through each field generator a string if need be.
        foreach ($eraFormatArr as $key => $val) {
            $ans = null; //clear answer for each loop.
            $req = null; //clear required for each loop.

            switch ($key) {
                /* Breed */
                case '01H009':
                    //always assume holstein
                    $ans = 'HO';
                    $req = 'T';
                    break;
                /* prefix */
                case '01H001':
                    //always assume default prefix
                    $ans = $prefix;
                    $req = 'T';
                    break;
                /* submission date */
                case '01H002':
                    //always assume today
                    $ans = date('Ymd', strtotime("now"));
                    $req = 'T';
                    break;
                case '01H003':
                    //assume only one registration 
                    $ans = $numRegistrations;
                    $req = 'T';
                    break;
                case '01H004':
                    //what record number we start at, always start at one.
                    $ans = $numSequence + 1;
                    $req = 'T';
                    break;
                //skip credit card, not used.
                case '01H012':
                    //assume never rush
                    $ans = 'N';
                    $req = 'T';
                    break;
                case '01H020':
                    //assume no certificates
                    $ans = 'N';
                    $req = 'T';
                    break;
                //skip 01H013 to 17, never need certificates
                case '01H021':
                    //always assume default prefix
                    $ans = $prefix;
                    $req = 'T';
                    break;
                //skip 01H022 only one emploee submits.
                /* DUMMY VALUE, need to overwrtie this later */
                case '01HEND':
                    $ans = '-1';
                    $req = 'T';
                    break;
                /////////////////////////// Animal ID ///////////////////////////
                case '01D000':
                    //start with sequence number specified and keep adding one for each reacord.
                    $numSequence = $numSequence + 1;
                    $ans = $numSequence;
                    $req = 'T';
                    break;
                case '01D001':
                    //get breed of animal we are registering.
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['full_reg_number']);
                    if (($ret['breed'] == 'HO') OR ($val['breed'] == 'BS') OR ($val['breed'] == 'CN') OR ($val['breed'] == 'GU') OR ($val['breed'] == 'JE') OR ($val['breed'] == 'SH')) {
                        $ans = $ret['breed'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                case '01D014':
                    //animal name    
                    if (empty($conceptionArr[0]['service_sire_short_name'])) {
                        throw new Exception("Error: key: $key for bovineID $bovine_id has no sire short name for animal name." . " </br>\n\r");
                    }
                    $aniNamePot = $conceptionArr[0]['service_sire_short_name'] . ' ' . $row_name['potential_name'];
                    
                      //automaticaally truncate the sire name if it is over 30 characters.
                    if (strlen($prefix . ' ' . $aniNamePot) > 30) {
                        //this will leave at least one letter for sire name and truncate up to the rest. This can still not be short enough and if so it will cause an exception below.
                        $aniNamePot = substr($conceptionArr[0]['service_sire_short_name'], 0, 1) . substr($conceptionArr[0]['service_sire_short_name'], 1, (strlen($conceptionArr[0]['service_sire_short_name']) - 1) - (strlen($prefix . ' ' . $aniNamePot) - 30)) . ' ' . $row_name['potential_name'];
                    }


                    //check that the name has a space in it, ie two words, sloppy.
                    if ($aniNamePot == null) {
                        throw new Exception("Error: key: $key for bovineID $bovine_id has no Animal Name." . " </br>\n\r");
                    }
                    if (stristr($aniNamePot, ' ') == false) {
                        throw new Exception("Error: key: $key for bovineID $bovine_id does not have at least two names in it." . " </br>\n\r");
                    }
           
                    if (strlen($aniNamePot) > 30) {
                         throw new Exception("Error: key: $key for bovineID $bovine_id full name longer then 30 character limit." . " </br>\n\r");
                    }
                    //we need to put the potential name, and the potenital first sire short name         
                    $ans = $aniNamePot;
                    $req = 'T';
                    break;
                case '01D008':
                    //birth date
                    $ans = date('Ymd', strtotime($row['birth_date']));
                    $req = 'T';
                    break;
                case '01D009':
                    //get sex of animal we are registering.     
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['full_reg_number']);
                    if (($ret['sex'] == 'M') OR ($ret['sex'] == 'F')) {
                        $ans = $ret['sex'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['sex']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                case '01D002':
                    //always assume purebred.
                    $ans = 'PB';
                    $req = 'T';
                    break;
                case '01D170':
                    //remove country code from rfid number.
                    if (stristr($row['rfid_number'], '124') == false) {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is not a Canadian RFID code for animal." . " </br>\n\r");
                    }
                    $shortRFIDNum = str_ireplace('124', '', $row['rfid_number']);
                    $ans = $shortRFIDNum;
                    $req = 'T';
                    break;
                //skip 01D003, no longer used???
                case '01D010':
                    //twin or not?
                    $ans = $row2['twin_count'];
                    $req = 'T';
                    break;
                case '01D011':
                    //sex of twin?
                    $ans = $twin_sex;
                    $req = 'T';
                    break;
                case '01D012':
                    //embryo transfer???? if calf resulted from Embryo Transfer.
                    if (trim($conceptionArr[0]['type']) == 'embryo7day') {
                        $ans = 'Y';
                    } elseif (trim($conceptionArr[0]['type']) == 'breeding') {
                        $ans = 'N';
                    } else {
                        print_r($conceptionArr);
                        throw new Exception("Error: key: $key for bovineID $bovine_id does not have a embryo or breeding type: {$conceptionArr[0]['type']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                case '01D230':
                    //size of calf.
                    if ($row2['calf_mass'] <= 30) {
                        $ans = 'S';
                    } elseif (($row2['calf_mass'] > 30) AND ($row2['calf_mass'] <= 40)) {
                        $ans = 'N';
                    } elseif ($row2['calf_mass'] > 40) {
                        $ans = 'L';
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id Calf does not have a valid size." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                //skip 01D133 premature/late birth. //we could code this, but why bother?
                case '01D013':
                    //calf colour
                    //W=Black&White, RW=Red&White, BR=Black/Red, AW=All White, AB=All Black, AR=All Red, IC=Irregular Colour
                    if ($row['colour'] == 'b&w') {
                        $ans = 'BW';
                    } elseif ($row['colour'] == 'r&w') {
                        $ans = 'RW';
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id calf colour is wrong format." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                case '01D004':
                    //management number (local_number)
                    if (is_numeric($row['local_number']) == true) {
                        $ans = $row['local_number'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id invalid local number: {$row['local_number']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                //skip 01D005 01D006 secondary tag and provincial tag not used.
                case '01D231':
                    //order pedigree is always no.
                    $ans = 'N';
                    $req = 'T';
                    break;
                /* Skip 01D234 Comment */
                ///////
                ////// Dam Identification
                //////
                /* dam breed */
                case '01D015':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['dam_full_reg_number']);
                    if (($ret['breed'] == 'HO') OR ($val['breed'] == 'BS') OR ($val['breed'] == 'CN') OR ($val['breed'] == 'GU') OR ($val['breed'] == 'JE') OR ($val['breed'] == 'SH')) {
                        $ans = $ret['breed'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* dam country */
                case '01D016':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['dam_full_reg_number']);
                    if (($ret['country'] == 'CAN') OR ($ret['country'] == 'USA')) {
                        $ans = $ret['country'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* dam sex */
                case '01D017':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['dam_full_reg_number']);
                    if (($ret['sex'] == 'F')) {
                        $ans = $ret['sex'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['sex']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* skip 01D018 dam reg prefix */
                /* dam reg number */
                case '01D019':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['dam_full_reg_number']);
                    if (is_numeric($ret['number']) == true) {
                        $ans = $ret['number'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['number']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* dam reg siffix */
                // skip 01D020
                //if we had a sam from another coutry with this, it would error before this point
                /* dam birth date */
                case '01D021':
                    $ans = date('Ymd', strtotime($row_dam['birth_date']));
                    $req = 'T';
                    break;
                /* dam full name with prefix */
                case '01D110':
                    $ans = $row_dam['full_name'];
                    $req = 'T';
                    break;
                /* skip dam provincial number */
                /* skip dam ear tag numberm, since we use rfid */
                /* dam rfid number */
                case '01D171':
                    //remove country code from rfid number.
                    if (stristr($row_dam['rfid_number'], '124') == false) {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is not a Canadian RFID code for dam." . " </br>\n\r");
                    }
                    $shortRFIDNum = str_ireplace('124', '', $row_dam['rfid_number']);
                    $ans = $shortRFIDNum;
                    $req = 'T';
                    break;
                /* 01D025 skip dam electronic id */
                //////////////
                //// SIRE #1 Info
                //////////////////
                /* service number */
                case '01D026':
                    $ans = '1';
                    $req = 'T';
                    break;
                /* who did inseminating */
                case '01D027':
                    $ans = 'H'; //always owner.
                    $req = 'T';
                    break;
                /* sire breed */
                case '01D028':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[0]['service_sire_full_reg_number']);
                    if (($ret['breed'] == 'HO') OR ($val['breed'] == 'BS') OR ($val['breed'] == 'CN') OR ($val['breed'] == 'GU') OR ($val['breed'] == 'JE') OR ($val['breed'] == 'SH')) {
                        $ans = $ret['breed'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* sire country */
                case '01D029':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[0]['service_sire_full_reg_number']);
                    $ans = $ret['country']; //accuracy not checked, so hopefully right
                    $req = 'T';
                    break;
                /* sire sex */
                case '01D030':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[0]['service_sire_full_reg_number']);
                    if (($ret['sex'] == 'M')) {
                        $ans = $ret['sex'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['sex']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* skip 01D111 sire reg number prefix */
                /* sire reg number */
                case '01D031':
                    $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[0]['service_sire_full_reg_number']);
                    if (is_numeric($ret['number']) == true) {
                        $ans = $ret['number'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['number']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* skip 01D112 sire reg number suffix */
                /* sire semen code */
                case '01D032':
                    //look it up, this will not differentiate between normal and sexed, always take standard code.
                    $sql_sire1 = "SELECT full_name,semen_code,(SELECT supplier_code FROM bovinemanagement.semen_straw where semen_code=semen_code limit 1) as supplier_code
                      FROM bovinemanagement.sire_semen_code 
                      LEFT JOIN bovinemanagement.sire ON sire_full_reg_number=sire.full_reg_number
                      WHERE sexed_semen=false AND sire_full_reg_number='{$conceptionArr[0]['service_sire_full_reg_number']}' LIMIT 1";

                    $res_sire1 = $GLOBALS ['pdo']->query($sql_sire1);
                    $row_sire1 = $res_sire1->fetch(PDO::FETCH_ASSOC);
                    if (!empty($row_sire1['semen_code'])) {
                        $ans = $row_sire1['semen_code'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire1['semen_code']}." . " </br>\n\r");
                    }

                    /* skip 01D033 semen freeze date */
                    /* skip 01D034 semen freeze code */
                    /* skip 01D035 sire birth date, always use ai, so not needed */
                    /* sire name */
                    $req = 'T';
                    break;
                case '01D113':
                    if (!empty($row_sire1['full_name'])) {
                        $ans = $row_sire1['full_name'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire1['full_name']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* breeding date */
                case '01D036':
                    $ans = date('Ymd', strtotime($conceptionArr[0]['event_time']));
                    $req = 'T';
                    break;
                /* skip 01D114 bull pasture not used */
                /* skip 01D037 ai tech not used */
                /* semen supplier code */
                case '01D038':
                    if (!empty($row_sire1['supplier_code'])) {
                        $ans = $row_sire1['supplier_code'];
                    } else {
                        throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire1['full_name']}." . " </br>\n\r");
                    }
                    $req = 'T';
                    break;
                /* sire on farm code not sure hwat this is, but fred has it. */
                case '01D039':
                    $ans = 'NBOF079'; //always owner.
                    $req = 'T';
                    break;
                /* skip 01D172 do not need sire rfid number for purchased semen */
                /* skip 01D116 Sire Electronic ID */
                /* skip 01D142 A.I. Breeding Slip Number */
                /* skip 01D143 Technician (Inseminator) Number */
            } //end main case
            //////////////
            //// SIRE #2 Info OPTIONAL
            //////////////////  
            //only run if there is another sire
           if (!empty($conceptionArr[1]['type']))  { 

                switch ($key) {

                    /* service number */
                    case '01D040':
                        $ans = '2';
                        $req = 'T';
                        break;
                    /* who did inseminating */
                    case '01D041':
                        $ans = 'H'; //always owner.
                        $req = 'T';
                        break;
                    /* sire breed */
                    case '01D042':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[1]['service_sire_full_reg_number']);
                        if (($ret['breed'] == 'HO') OR ($val['breed'] == 'BS') OR ($val['breed'] == 'CN') OR ($val['breed'] == 'GU') OR ($val['breed'] == 'JE') OR ($val['breed'] == 'SH')) {
                            $ans = $ret['breed'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* sire country */
                    case '01D043':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[1]['service_sire_full_reg_number']);
                        $ans = $ret['country']; //accuracy not checked, so hopefully right
                        $req = 'T';
                        break;
                    /* sire sex */
                    case '01D044':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[1]['service_sire_full_reg_number']);
                        if (($ret['sex'] == 'M')) {
                            $ans = $ret['sex'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['sex']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* skip 01D117 sire reg number prefix */
                    /* sire reg number */
                    case '01D045':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[1]['service_sire_full_reg_number']);
                        if (is_numeric($ret['number']) == true) {
                            $ans = $ret['number'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['number']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* skip 01D118 sire reg number suffix */
                    /* sire semen code */
                    case '01D046':
                        //look it up, this will not differentiate between normal and sexed, always take standard code.
                        $sql_sire2 = "SELECT full_name,semen_code,(SELECT supplier_code FROM bovinemanagement.semen_straw where semen_code=semen_code limit 1) as supplier_code
                      FROM bovinemanagement.sire_semen_code 
                      LEFT JOIN bovinemanagement.sire ON sire_full_reg_number=sire.full_reg_number
                      WHERE sexed_semen=false AND sire_full_reg_number='{$conceptionArr[1]['service_sire_full_reg_number']}' LIMIT 1";

                        $res_sire2 = $GLOBALS ['pdo']->query($sql_sire2);
                        $row_sire2 = $res_sire2->fetch(PDO::FETCH_ASSOC);
                        if (!empty($row_sire2['semen_code'])) {
                            $ans = $row_sire2['semen_code'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire2['semen_code']}." . " </br>\n\r");
                        }

                        /* skip 01D047 semen freeze date */
                        /* skip 01D048 semen freeze code */
                        /* skip 01D049 sire birth date, always use ai, so not needed */
                        /* sire name */
                        $req = 'T';
                        break;
                    case '01D119':
                        if (!empty($row_sire2['full_name'])) {
                            $ans = $row_sire2['full_name'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire2['full_name']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* breeding date */
                    case '01D050':
                        $ans = date('Ymd', strtotime($conceptionArr[1]['event_time']));
                        $req = 'T';
                        break;
                    /* skip 01D120 bull pasture not used */
                    /* skip 01D051 ai tech not used */
                    /* semen supplier code */
                    case '01D052':
                        if (!empty($row_sire2['supplier_code'])) {
                            $ans = $row_sire2['supplier_code'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire2['full_name']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* sire on farm code not sure hwat this is, but fred has it. */
                    case '01D053':
                        $ans = 'NBOF079'; //always owner.
                        $req = 'T';
                        break;
                    /* skip 01D173 do not need sire rfid number for purchased semen */
                    /* skip 01D122 Sire Electronic ID */
                    /* skip 01D144 A.I. Breeding Slip Number */
                    /* skip 01D145 Technician (Inseminator) Number */
                } //end case
            } //end sire 1 loop.
            //////////////
            //// SIRE #3 Info OPTIONAL
            //////////////////  
            //only run if there is another sire
            if (!empty($conceptionArr[2]['type'])) {

                switch ($key) {
                    /* service number */
                    case '01D054':
                        $ans = '3';
                        $req = 'T';
                        break;
                    /* who did inseminating */
                    case '01D055':
                        $ans = 'H'; //always owner.
                        $req = 'T';
                        break;
                    /* sire breed */
                    case '01D056':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[2]['service_sire_full_reg_number']);
                        if (($ret['breed'] == 'HO') OR ($val['breed'] == 'BS') OR ($val['breed'] == 'CN') OR ($val['breed'] == 'GU') OR ($val['breed'] == 'JE') OR ($val['breed'] == 'SH')) {
                            $ans = $ret['breed'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* sire country */
                    case '01D057':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[2]['service_sire_full_reg_number']);
                        $ans = $ret['country']; //accuracy not checked, so hopefully right
                        $req = 'T';
                        break;
                    /* sire sex */
                    case '01D058':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[2]['service_sire_full_reg_number']);
                        if (($ret['sex'] == 'M')) {
                            $ans = $ret['sex'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['sex']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* skip 01D123 sire reg number prefix */
                    /* sire reg number */
                    case '01D059':
                        $ret = $GLOBALS['MiscObj']->breakUpFullRegNumber($conceptionArr[2]['service_sire_full_reg_number']);
                        if (is_numeric($ret['number']) == true) {
                            $ans = $ret['number'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$ret['number']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* skip 01D124 sire reg number suffix */
                    /* sire semen code */
                    case '01D060':
                        //look it up, this will not differentiate between normal and sexed, always take standard code.
                        $sql_sire3 = "SELECT full_name,semen_code,(SELECT supplier_code FROM bovinemanagement.semen_straw where semen_code=semen_code limit 1) as supplier_code
                      FROM bovinemanagement.sire_semen_code 
                      LEFT JOIN bovinemanagement.sire ON sire_full_reg_number=sire.full_reg_number
                      WHERE sexed_semen=false AND sire_full_reg_number='{$conceptionArr[2]['service_sire_full_reg_number']}' LIMIT 1";

                        $res_sire3 = $GLOBALS ['pdo']->query($sql_sire3);
                        $row_sire3 = $res_sire3->fetch(PDO::FETCH_ASSOC);
                        if (!empty($row_sire3['semen_code'])) {
                            $ans = $row_sire3['semen_code'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire3['semen_code']}." . " </br>\n\r");
                        }

                        /* skip 01D061 semen freeze date */
                        /* skip 01D062 semen freeze code */
                        /* skip 01D063 sire birth date, always use ai, so not needed */
                        /* sire name */
                        $req = 'T';
                        break;
                    case '01D125':
                        if (!empty($row_sire3['full_name'])) {
                            $ans = $row_sire3['full_name'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire3['full_name']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* breeding date */
                    case '01D064':
                        $ans = date('Ymd', strtotime($conceptionArr[2]['event_time']));
                        $req = 'T';
                        break;
                    /* skip 01D126 bull pasture not used */
                    /* skip 01D065 ai tech not used */
                    /* semen supplier code */
                    case '01D066':
                        if (!empty($row_sire3['supplier_code'])) {
                            $ans = $row_sire3['supplier_code'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format: {$row_sire3['full_name']}." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* sire on farm code not sure hwat this is, but fred has it. */
                    case '01D067':
                        $ans = 'NBOF079'; //always owner.
                        $req = 'T';
                        break;
                    /* skip 01D174 do not need sire rfid number for purchased semen */
                    /* skip 01D128 Sire Electronic ID */
                    /* skip 01D146 A.I. Breeding Slip Number */
                    /* skip 01D147 Technician (Inseminator) Number */
                } //end case
            } //end sire 2 loop.
            /////////////////
            ///// EMBRYO TRANSFER
            /////////////////
            //if the first concenption event was embryo, then assume embryo transfer
            $conceptionArr[0]['type']=empty($conceptionArr[0]['type']) ? ''  : $conceptionArr[0]['type'];
            if (trim($conceptionArr[0]['type']) == 'embryo7day') {


                //lookup embryo.
                $sql_embryo = "SELECT flush_date,donor_dam_full_reg_number,embryo_straw_id,event_time,bovine_id,full_reg_number as recipient_full_reg_number
                        FROM bovinemanagement.embryo_implant 
                      LEFT JOIN bovinemanagement.embryo_straw ON embryo_straw_id=embryo_straw.id
                      LEFT JOIN bovinemanagement.embryo_flush ON embryo_flush_id=embryo_flush.id
                      LEFT JOIN   bovinemanagement.bovine ON embryo_implant.bovine_id=bovine.id
                      WHERE embryo_implant.id={$conceptionArr[0]['table_id']} LIMIT 1";
                $res_embryo = $GLOBALS ['pdo']->query($sql_embryo);
                $row_embryo = $res_embryo->fetch(PDO::FETCH_ASSOC);

                if ($res_embryo->rowCount() != 1) {
                    throw new Exception("Error: Embryo info not found for bovineID $bovine_id and embryo table_id {$conceptionArr[0]['table_id']}." . " </br>\n\r");
                }

                switch ($key) {

                    /* recovery date of embryo */
                    case '01D068':
                        if (!empty($row_embryo['flush_date'])) {
                            $ans = date('Ymd', strtotime($row_embryo['flush_date']));
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format. Embryo recovery date (flush date)." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* embryo manipulation type */
                    case '01D069':
                        $ans = 'R'; //only supports regular embryos
                        $req = 'T';
                        break;
                    /* embryo id */
                    case '01D070':
                        if (!empty($row_embryo['donor_dam_full_reg_number']) AND  (!empty($row_embryo['embryo_straw_id']))) {
                            $ans = $row_embryo['donor_dam_full_reg_number'] . date('Ymd', strtotime($row_embryo['flush_date'])) . $row_embryo['embryo_straw_id']; //just a concatenation of info. Maybe not specific enough?
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* embryo recipient */
                    case '01D071':
                        if (!empty($row_embryo['recipient_full_reg_number'])) {
                            $ans = $row_embryo['recipient_full_reg_number'];
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                    /* embryo transplant date */
                    case '01D072':
                        if (!empty($row_embryo['event_time'])) {
                            $ans = date('Ymd', strtotime($row_embryo['event_time']));
                        } else {
                            throw new Exception("Error: key: $key for bovineID $bovine_id is wrong format." . " </br>\n\r");
                        }
                        $req = 'T';
                        break;
                } //end case
            }//end embryo
            ////////////////////
            ///////// OWNER INFO
            //////////////////////

            switch ($key) {
                /* Prefix/ClientID */
                case '01D076':
                    $ans = $prefix;
                    $req = 'T';
                    break;
                /* Phone # */
                case '01D077':
                    $ans = '506-372-4564';
                    $req = 'T';
                    break;
                /* Milk Recording Program Herd No. */
                case '01D078':
                    $ans = '97593'; //valcata number
                    $req = 'T';
                    break;
                ////////////////////
                ///////// BREEDER INFO
                //////////////////////
                /* Prefix Breeder */
                case '01D130':
                    $ans = $prefix;
                    $req = 'T';
                    break;
                /* DUMMY 01DEND, need to ovewrite this later, calculated field */
                case '01DEND':
                    $ans = '-1';
                    $req = 'T';
                    break;
                /* DUMMY 01EEND, need to ovewrite this later */
                case '01EEND':
                    $ans = '-1';
                    $req = 'T';
                    break;
            }//end case

            $eraAnswerArr["$key"]['answer'] = $ans;
            $eraAnswerArr["$key"]['lr_required'] = $req;
        }//end field for loop
        //now return array
        return $eraAnswerArr;
    }

//end function
    //checks for basic errors in syntax of the data format
    public function checkERAFormat($eraFormatArr) {


        foreach ($eraFormatArr as $key => $val) {

            //set the required field to N for everything. We will change this later
            //dynamically if teh field is actually reauired.
            $eraFormatArr[$key]['lr_required'] = 'F';


            //check length is an integer
            if (is_int($val['length']) != true) {
                throw new Exception("ERA format key: $key length is wrong." . " </br>\n\r");
            }

            //check to see if field type is formatted correctly
            if (($val['field_type'] == 'A') OR ($val['field_type'] == 'N') OR ($val['field_type'] == 'A/N')) {
                //all good
            } else {
                throw new Exception("ERA format key: $key field type is wrong. {$val['field_type']}" . " </br>\n\r");
            }

            //check to see if field type is formatted correctly
            if (($val['required'] == 'M') OR ($val['required'] == 'O') OR ($val['required'] == 'X')) {
                //all good
            } else {
                throw new Exception("ERA format key: $key required is wrong." . " </br>\n\r");
            }
        }
        return $eraFormatArr;
    }

//function end

    private function ERAFormat() {

        $eraFormatArr['01H009']['field_no'] = '01H009';
        $eraFormatArr['01H009']['field_name'] = 'Breed';
        $eraFormatArr['01H009']['length'] = 2;
        $eraFormatArr['01H009']['field_type'] = 'A';
        $eraFormatArr['01H009']['example'] = 'HO';
        $eraFormatArr['01H009']['domain'] = 'HO';
        $eraFormatArr['01H009']['required'] = 'M';
        //
        $eraFormatArr['01H001']['field_no'] = '01H001';
        $eraFormatArr['01H001']['field_name'] = 'Prefix/Client ID';
        $eraFormatArr['01H001']['length'] = 25;
        $eraFormatArr['01H001']['field_type'] = 'A/N';
        $eraFormatArr['01H001']['example'] = 'BRANTCROFT';
        $eraFormatArr['01H001']['domain'] = null;
        $eraFormatArr['01H001']['required'] = 'M';
        //
        $eraFormatArr['01H002']['field_no'] = '01H002';
        $eraFormatArr['01H002']['field_name'] = 'Submission Date';
        $eraFormatArr['01H002']['length'] = 8;
        $eraFormatArr['01H002']['field_type'] = 'N';
        $eraFormatArr['01H002']['example'] = '20001231';
        $eraFormatArr['01H002']['domain'] = null;
        $eraFormatArr['01H002']['required'] = 'M';
        //
        $eraFormatArr['01H003']['field_no'] = '01H003';
        $eraFormatArr['01H003']['field_name'] = 'No. of Registrations';
        $eraFormatArr['01H003']['length'] = 3;
        $eraFormatArr['01H003']['field_type'] = 'N';
        $eraFormatArr['01H003']['example'] = '2';
        $eraFormatArr['01H003']['domain'] = '1-999';
        $eraFormatArr['01H003']['required'] = 'M';
        //
        $eraFormatArr['01H004']['field_no'] = '01H004';
        $eraFormatArr['01H004']['field_name'] = 'Starting Sequence No.';
        $eraFormatArr['01H004']['length'] = 3;
        $eraFormatArr['01H004']['field_type'] = 'N';
        $eraFormatArr['01H004']['example'] = '11';
        $eraFormatArr['01H004']['domain'] = '1-999';
        $eraFormatArr['01H004']['required'] = 'M';
        //
        $eraFormatArr['01H005']['field_no'] = '01H005';
        $eraFormatArr['01H005']['field_name'] = 'Credit Card Type';
        $eraFormatArr['01H005']['length'] = 3;
        $eraFormatArr['01H005']['field_type'] = 'A';
        $eraFormatArr['01H005']['example'] = 'VIS-VISA';
        $eraFormatArr['01H005']['domain'] = 'VIS-VISA, MAS - MasterCard';
        $eraFormatArr['01H005']['required'] = 'X';
        //
        $eraFormatArr['01H006']['field_no'] = '01H006';
        $eraFormatArr['01H006']['field_name'] = 'Credit Card Number';
        $eraFormatArr['01H006']['length'] = 19;
        $eraFormatArr['01H006']['field_type'] = 'A/N';
        $eraFormatArr['01H006']['example'] = '1234-5678-9012- 3456';
        $eraFormatArr['01H006']['domain'] = null;
        $eraFormatArr['01H006']['required'] = 'X';
        //
        $eraFormatArr['01H007']['field_no'] = '01H007';
        $eraFormatArr['01H007']['field_name'] = 'Credit Card Expiry Date';
        $eraFormatArr['01H007']['length'] = 6;
        $eraFormatArr['01H007']['field_type'] = 'N';
        $eraFormatArr['01H007']['example'] = '0107 (yymm)';
        $eraFormatArr['01H007']['domain'] = null;
        $eraFormatArr['01H007']['required'] = 'X';
        //
        $eraFormatArr['01H008']['field_no'] = '01H008';
        $eraFormatArr['01H008']['field_name'] = 'Credit Card Name';
        $eraFormatArr['01H008']['length'] = 50;
        $eraFormatArr['01H008']['field_type'] = 'A/N';
        $eraFormatArr['01H008']['example'] = 'John Hanover';
        $eraFormatArr['01H008']['domain'] = null;
        $eraFormatArr['01H008']['required'] = 'X';
        //
        $eraFormatArr['01H010']['field_no'] = '01H010';
        $eraFormatArr['01H010']['field_name'] = 'Credit Card Maximum Charge';
        $eraFormatArr['01H010']['length'] = 8;
        $eraFormatArr['01H010']['field_type'] = 'N';
        $eraFormatArr['01H010']['example'] = '99999.99';
        $eraFormatArr['01H010']['domain'] = null;
        $eraFormatArr['01H010']['required'] = 'O';
        //
        $eraFormatArr['01H011']['field_no'] = '01H011';
        $eraFormatArr['01H011']['field_name'] = 'Shipping Method';
        $eraFormatArr['01H011']['length'] = 1;
        $eraFormatArr['01H011']['field_type'] = 'A';
        $eraFormatArr['01H011']['example'] = 'C - Courier,F - First Class, H-Personal Pick-up';
        $eraFormatArr['01H011']['domain'] = 'C,F,H';
        $eraFormatArr['01H011']['required'] = 'X';
        //
        $eraFormatArr['01H012']['field_no'] = '01H012';
        $eraFormatArr['01H012']['field_name'] = 'Rush Service Requested';
        $eraFormatArr['01H012']['length'] = 1;
        $eraFormatArr['01H012']['field_type'] = 'A';
        $eraFormatArr['01H012']['example'] = 'Y,N';
        $eraFormatArr['01H012']['domain'] = 'Y,N';
        $eraFormatArr['01H012']['required'] = 'M';
        //
        $eraFormatArr['01H020']['field_no'] = '01H020';
        $eraFormatArr['01H020']['field_name'] = 'Certificates of Registry Requested';
        $eraFormatArr['01H020']['length'] = 1;
        $eraFormatArr['01H020']['field_type'] = 'A';
        $eraFormatArr['01H020']['example'] = 'Y,N';
        $eraFormatArr['01H020']['domain'] = 'Y,N';
        $eraFormatArr['01H020']['required'] = 'M';
        //
        $eraFormatArr['01H013']['field_no'] = '01H013';
        $eraFormatArr['01H013']['field_name'] = 'Date Certificates Required by';
        $eraFormatArr['01H013']['length'] = 8;
        $eraFormatArr['01H013']['field_type'] = 'N';
        $eraFormatArr['01H013']['example'] = '20010127';
        $eraFormatArr['01H013']['domain'] = null;
        $eraFormatArr['01H013']['required'] = 'O';
        //
        $eraFormatArr['01H014']['field_no'] = '01H014';
        $eraFormatArr['01H014']['field_name'] = 'Ship to (Prefix/Client ID)';
        $eraFormatArr['01H014']['length'] = 25;
        $eraFormatArr['01H014']['field_type'] = 'A/N';
        $eraFormatArr['01H014']['example'] = 'BRANTBRAE';
        $eraFormatArr['01H014']['domain'] = null;
        $eraFormatArr['01H014']['required'] = 'X';
        //
        $eraFormatArr['01H015']['field_no'] = '01H015';
        $eraFormatArr['01H015']['field_name'] = 'Ship to (Name)';
        $eraFormatArr['01H015']['length'] = 50;
        $eraFormatArr['01H015']['field_type'] = 'A/N';
        $eraFormatArr['01H015']['example'] = 'Jim Brant';
        $eraFormatArr['01H015']['domain'] = null;
        $eraFormatArr['01H015']['required'] = 'X';
        //
        $eraFormatArr['01H016']['field_no'] = '01H016';
        $eraFormatArr['01H016']['field_name'] = 'Ship to (CHIN - Telephone No.)';
        $eraFormatArr['01H016']['length'] = 12;
        $eraFormatArr['01H016']['field_type'] = 'A/N';
        $eraFormatArr['01H016']['example'] = '514-999-9999';
        $eraFormatArr['01H016']['domain'] = null;
        $eraFormatArr['01H016']['required'] = 'O';
        //
        $eraFormatArr['01H017']['field_no'] = '01H017';
        $eraFormatArr['01H017']['field_name'] = 'Ship to (Milk Recording Program Herd No.)';
        $eraFormatArr['01H017']['length'] = 5;
        $eraFormatArr['01H017']['field_type'] = 'N';
        $eraFormatArr['01H017']['example'] = '';
        $eraFormatArr['01H017']['domain'] = '1-99999';
        $eraFormatArr['01H017']['required'] = 'O';
        //
        $eraFormatArr['01H021']['field_no'] = '01H021';
        $eraFormatArr['01H021']['field_name'] = 'Submitter Prefix/Client ID';
        $eraFormatArr['01H021']['length'] = 25;
        $eraFormatArr['01H021']['field_type'] = 'A/N';
        $eraFormatArr['01H021']['example'] = 'Prefix/Client ID or Accreditation Code';
        $eraFormatArr['01H021']['domain'] = '[A-Z][0-99999]';
        $eraFormatArr['01H021']['required'] = 'M';
        //
        $eraFormatArr['01H022']['field_no'] = '01H022';
        $eraFormatArr['01H022']['field_name'] = 'Submitter Employee No.';
        $eraFormatArr['01H022']['length'] = 10;
        $eraFormatArr['01H022']['field_type'] = 'N';
        $eraFormatArr['01H022']['example'] = '1234';
        $eraFormatArr['01H022']['domain'] = null;
        $eraFormatArr['01H022']['required'] = 'O';
        //
        $eraFormatArr['01HEND']['field_no'] = '01HEND';
        $eraFormatArr['01HEND']['field_name'] = 'Header Record End';
        $eraFormatArr['01HEND']['length'] = 0;
        $eraFormatArr['01HEND']['field_type'] = 'N';
        $eraFormatArr['01HEND']['example'] = '';
        $eraFormatArr['01HEND']['domain'] = null;
        $eraFormatArr['01HEND']['required'] = 'M';
        //
        // ANIMAL ID
        //
       $eraFormatArr['01D000']['field_no'] = '01D000';
        $eraFormatArr['01D000']['field_name'] = 'Sequence Number';
        $eraFormatArr['01D000']['length'] = 3;
        $eraFormatArr['01D000']['field_type'] = 'N';
        $eraFormatArr['01D000']['example'] = '11';
        $eraFormatArr['01D000']['domain'] = '1-999';
        $eraFormatArr['01D000']['required'] = 'M';
        //
        $eraFormatArr['01D001']['field_no'] = '01D001';
        $eraFormatArr['01D001']['field_name'] = 'Breed';
        $eraFormatArr['01D001']['length'] = 2;
        $eraFormatArr['01D001']['field_type'] = 'A';
        $eraFormatArr['01D001']['example'] = 'HO';
        $eraFormatArr['01D001']['domain'] = 'HO,BS,CN,GU,JE,SH';
        $eraFormatArr['01D001']['required'] = 'M';
        //
        $eraFormatArr['01D014']['field_no'] = '01D014';
        $eraFormatArr['01D014']['field_name'] = 'Animal Name';
        $eraFormatArr['01D014']['length'] = 30;
        $eraFormatArr['01D014']['field_type'] = 'A/N';
        $eraFormatArr['01D014']['example'] = 'Prelude Bonnie';
        $eraFormatArr['01D014']['domain'] = 'A-Z,0-9';
        $eraFormatArr['01D014']['required'] = 'M';
        //
        $eraFormatArr['01D008']['field_no'] = '01D008';
        $eraFormatArr['01D008']['field_name'] = 'Birth Date';
        $eraFormatArr['01D008']['length'] = 8;
        $eraFormatArr['01D008']['field_type'] = 'N';
        $eraFormatArr['01D008']['example'] = '19920120';
        $eraFormatArr['01D008']['domain'] = null;
        $eraFormatArr['01D008']['required'] = 'X';
        //
        $eraFormatArr['01D009']['field_no'] = '01D009';
        $eraFormatArr['01D009']['field_name'] = 'Sex';
        $eraFormatArr['01D009']['length'] = 1;
        $eraFormatArr['01D009']['field_type'] = 'A';
        $eraFormatArr['01D009']['example'] = 'F=Female, M=Male';
        $eraFormatArr['01D009']['domain'] = 'F,M';
        $eraFormatArr['01D009']['required'] = 'M';
        //
        $eraFormatArr['01D002']['field_no'] = '01D002';
        $eraFormatArr['01D002']['field_name'] = 'Registration Type';
        $eraFormatArr['01D002']['length'] = 2;
        $eraFormatArr['01D002']['field_type'] = 'A';
        $eraFormatArr['01D002']['example'] = 'PB=Purebred, PC=Percentage, RD=Recorded';
        $eraFormatArr['01D002']['domain'] = 'PB,PC,RD';
        $eraFormatArr['01D002']['required'] = 'M';
        //
        $eraFormatArr['01D170']['field_no'] = '01D170';
        $eraFormatArr['01D170']['field_name'] = 'EZE-IR Ear Tag';
        $eraFormatArr['01D170']['length'] = 12;
        $eraFormatArr['01D170']['field_type'] = 'N';
        $eraFormatArr['01D170']['example'] = '12345678';
        $eraFormatArr['01D170']['domain'] = '1-99999999';
        $eraFormatArr['01D170']['required'] = 'M';
        //
        $eraFormatArr['01D003']['field_no'] = '01D003';
        $eraFormatArr['01D003']['field_name'] = 'Microchip ID';
        $eraFormatArr['01D003']['length'] = 16;
        $eraFormatArr['01D003']['field_type'] = 'A/N';
        $eraFormatArr['01D003']['example'] = '7F7D28D58';
        $eraFormatArr['01D003']['domain'] = 'A-Z,0-9';
        $eraFormatArr['01D003']['required'] = 'X';
        //
        $eraFormatArr['01D010']['field_no'] = '01D010';
        $eraFormatArr['01D010']['field_name'] = 'Birth Type';
        $eraFormatArr['01D010']['length'] = 1;
        $eraFormatArr['01D010']['field_type'] = 'N';
        $eraFormatArr['01D010']['example'] = '1=Single 2=Twin, etc.';
        $eraFormatArr['01D010']['domain'] = '1-9';
        $eraFormatArr['01D010']['required'] = 'M';
        //
        $eraFormatArr['01D011']['field_no'] = '01D011';
        $eraFormatArr['01D011']['field_name'] = 'Sex of Twin';
        $eraFormatArr['01D011']['length'] = 1;
        $eraFormatArr['01D011']['field_type'] = 'A';
        $eraFormatArr['01D011']['example'] = 'Y=Male N=Female';
        $eraFormatArr['01D011']['domain'] = 'Y,N';
        $eraFormatArr['01D011']['required'] = 'X';
        //
        $eraFormatArr['01D012']['field_no'] = '01D012';
        $eraFormatArr['01D012']['field_name'] = 'Embryo Transfer';
        $eraFormatArr['01D012']['length'] = 2;
        $eraFormatArr['01D012']['field_type'] = 'A';
        $eraFormatArr['01D012']['example'] = 'Y,N';
        $eraFormatArr['01D012']['domain'] = 'Y,N';
        $eraFormatArr['01D012']['required'] = 'M';
        //
        $eraFormatArr['01D230']['field_no'] = '01D230';
        $eraFormatArr['01D230']['field_name'] = 'Size';
        $eraFormatArr['01D230']['length'] = 1;
        $eraFormatArr['01D230']['field_type'] = 'A';
        $eraFormatArr['01D230']['example'] = 'S=Small,N=Normal, L=Large';
        $eraFormatArr['01D230']['domain'] = 'S,N,L';
        $eraFormatArr['01D230']['required'] = 'O';
        //
        $eraFormatArr['01D133']['field_no'] = '01D133';
        $eraFormatArr['01D133']['field_name'] = 'Premature/Late Birth';
        $eraFormatArr['01D133']['length'] = 1;
        $eraFormatArr['01D133']['field_type'] = 'A';
        $eraFormatArr['01D133']['example'] = 'P=Premature, L=Late';
        $eraFormatArr['01D133']['domain'] = 'P,L';
        $eraFormatArr['01D133']['required'] = 'O';
        //
        $eraFormatArr['01D013']['field_no'] = '01D013';
        $eraFormatArr['01D013']['field_name'] = 'Colour';
        $eraFormatArr['01D013']['length'] = 2;
        $eraFormatArr['01D013']['field_type'] = 'A';
        $eraFormatArr['01D013']['example'] = 'BW=Black&White, RW=Red&White, BR=Black/Red, AW=All White, AB=All Black, AR=All Red, IC=Irregular Colour';
        $eraFormatArr['01D013']['domain'] = 'BW,RW,BR,AW,AB,AR,IC';
        $eraFormatArr['01D013']['required'] = 'M';
        //
        $eraFormatArr['01D004']['field_no'] = '01D004';
        $eraFormatArr['01D004']['field_name'] = 'Within Herd Tag';
        $eraFormatArr['01D004']['length'] = 10;
        $eraFormatArr['01D004']['field_type'] = 'A/N';
        $eraFormatArr['01D004']['example'] = 'B7826';
        $eraFormatArr['01D004']['domain'] = null;
        $eraFormatArr['01D004']['required'] = 'M';
        //
        $eraFormatArr['01D005']['field_no'] = '01D005';
        $eraFormatArr['01D005']['field_name'] = 'Secondary Tag/ID';
        $eraFormatArr['01D005']['length'] = 10;
        $eraFormatArr['01D005']['field_type'] = 'A/N';
        $eraFormatArr['01D005']['example'] = 'CD4286';
        $eraFormatArr['01D005']['domain'] = null;
        $eraFormatArr['01D005']['required'] = 'O';
        //
        $eraFormatArr['01D006']['field_no'] = '01D006';
        $eraFormatArr['01D006']['field_name'] = 'Provincial Number (other animal identifier)';
        $eraFormatArr['01D006']['length'] = 10;
        $eraFormatArr['01D006']['field_type'] = 'A/N';
        $eraFormatArr['01D006']['example'] = 'B381426';
        $eraFormatArr['01D006']['domain'] = null;
        $eraFormatArr['01D006']['required'] = 'O';
        //
        $eraFormatArr['01D231']['field_no'] = '01D231';
        $eraFormatArr['01D231']['field_name'] = 'Pedigree';
        $eraFormatArr['01D231']['length'] = 1;
        $eraFormatArr['01D231']['field_type'] = 'A';
        $eraFormatArr['01D231']['example'] = 'Y/N';
        $eraFormatArr['01D231']['domain'] = 'Y,N';
        $eraFormatArr['01D231']['required'] = 'O';
        //
        $eraFormatArr['01D232']['field_no'] = '01D232';
        $eraFormatArr['01D232']['field_name'] = 'Image Type';
        $eraFormatArr['01D232']['length'] = 1;
        $eraFormatArr['01D232']['field_type'] = 'A';
        $eraFormatArr['01D232']['example'] = 'P-Photo S-Sketch, N-None';
        $eraFormatArr['01D232']['domain'] = 'P,S,N';
        $eraFormatArr['01D232']['required'] = 'X';
        //
        $eraFormatArr['01D233']['field_no'] = '01D233';
        $eraFormatArr['01D233']['field_name'] = 'Image Identifier';
        $eraFormatArr['01D233']['length'] = 80;
        $eraFormatArr['01D233']['field_type'] = 'A/N';
        $eraFormatArr['01D233']['example'] = '12345678.jpg';
        $eraFormatArr['01D233']['domain'] = null;
        $eraFormatArr['01D233']['required'] = 'X';
        //
        $eraFormatArr['01D234']['field_no'] = '01D234';
        $eraFormatArr['01D234']['field_name'] = 'Comment';
        $eraFormatArr['01D234']['length'] = 80;
        $eraFormatArr['01D234']['field_type'] = 'A/N';
        $eraFormatArr['01D234']['example'] = 'no idea?';
        $eraFormatArr['01D234']['domain'] = null;
        $eraFormatArr['01D234']['required'] = 'O';
        //
        // DAM
        //
       $eraFormatArr['01D015']['field_no'] = '01D015';
        $eraFormatArr['01D015']['field_name'] = 'Dam Breed';
        $eraFormatArr['01D015']['length'] = 2;
        $eraFormatArr['01D015']['field_type'] = 'A';
        $eraFormatArr['01D015']['example'] = 'HO';
        $eraFormatArr['01D015']['domain'] = 'HO,BS,CN,GU,JE,SH';
        $eraFormatArr['01D015']['required'] = 'M';
        //
        $eraFormatArr['01D016']['field_no'] = '01D016';
        $eraFormatArr['01D016']['field_name'] = 'Dam Country';
        $eraFormatArr['01D016']['length'] = 3;
        $eraFormatArr['01D016']['field_type'] = 'A';
        $eraFormatArr['01D016']['example'] = 'CAN';
        $eraFormatArr['01D016']['domain'] = 'A-Z';
        $eraFormatArr['01D016']['required'] = 'X';
        //
        $eraFormatArr['01D017']['field_no'] = '01D017';
        $eraFormatArr['01D017']['field_name'] = 'Dam Sex';
        $eraFormatArr['01D017']['length'] = 1;
        $eraFormatArr['01D017']['field_type'] = 'A';
        $eraFormatArr['01D017']['example'] = 'F';
        $eraFormatArr['01D017']['domain'] = 'F';
        $eraFormatArr['01D017']['required'] = 'M';
        //
        $eraFormatArr['01D018']['field_no'] = '01D018';
        $eraFormatArr['01D018']['field_name'] = 'Dam Reg. No. Prefix';
        $eraFormatArr['01D018']['length'] = 4;
        $eraFormatArr['01D018']['field_type'] = 'A/N';
        $eraFormatArr['01D018']['example'] = 'DR';
        $eraFormatArr['01D018']['domain'] = null;
        $eraFormatArr['01D018']['required'] = 'X';
        //
        $eraFormatArr['01D019']['field_no'] = '01D019';
        $eraFormatArr['01D019']['field_name'] = 'Dam Registration Number';
        $eraFormatArr['01D019']['length'] = 12;
        $eraFormatArr['01D019']['field_type'] = 'A/N';
        $eraFormatArr['01D019']['example'] = '6789012';
        $eraFormatArr['01D019']['domain'] = null;
        $eraFormatArr['01D019']['required'] = 'X';
        //
        $eraFormatArr['01D020']['field_no'] = '01D020';
        $eraFormatArr['01D020']['field_name'] = 'Dam Reg. No. Suffix';
        $eraFormatArr['01D020']['length'] = 3;
        $eraFormatArr['01D020']['field_type'] = 'A/N';
        $eraFormatArr['01D020']['example'] = 'X';
        $eraFormatArr['01D020']['domain'] = null;
        $eraFormatArr['01D020']['required'] = 'X';
        //
        $eraFormatArr['01D021']['field_no'] = '01D021';
        $eraFormatArr['01D021']['field_name'] = 'Dam Birth Date';
        $eraFormatArr['01D021']['length'] = 8;
        $eraFormatArr['01D021']['field_type'] = 'N';
        $eraFormatArr['01D021']['example'] = '19880917';
        $eraFormatArr['01D021']['domain'] = null;
        $eraFormatArr['01D021']['required'] = 'X';
        //
        $eraFormatArr['01D110']['field_no'] = '01D110';
        $eraFormatArr['01D110']['field_name'] = 'Dam Name';
        $eraFormatArr['01D110']['length'] = 40;
        $eraFormatArr['01D110']['field_type'] = 'A/N';
        $eraFormatArr['01D110']['example'] = 'BRANTCROFT STARBUCK BESSY';
        $eraFormatArr['01D110']['domain'] = null;
        $eraFormatArr['01D110']['required'] = 'X';
        //
        $eraFormatArr['01D022']['field_no'] = '01D022';
        $eraFormatArr['01D022']['field_name'] = 'Dam Provincial Number';
        $eraFormatArr['01D022']['length'] = 10;
        $eraFormatArr['01D022']['field_type'] = 'A/N';
        $eraFormatArr['01D022']['example'] = 'BC81426';
        $eraFormatArr['01D022']['domain'] = null;
        $eraFormatArr['01D022']['required'] = 'X';
        //
        $eraFormatArr['01D023']['field_no'] = '01D023';
        $eraFormatArr['01D023']['field_name'] = 'Dam Ear Tag';
        $eraFormatArr['01D023']['length'] = 10;
        $eraFormatArr['01D023']['field_type'] = 'A/N';
        $eraFormatArr['01D023']['example'] = 'N4236';
        $eraFormatArr['01D023']['domain'] = null;
        $eraFormatArr['01D023']['required'] = 'X';
        //
        $eraFormatArr['01D171']['field_no'] = '01D171';
        $eraFormatArr['01D171']['field_name'] = 'Dam EZE-IR Ear Tag';
        $eraFormatArr['01D171']['length'] = 12;
        $eraFormatArr['01D171']['field_type'] = 'N';
        $eraFormatArr['01D171']['example'] = '12345678';
        $eraFormatArr['01D171']['domain'] = '1-99999999';
        $eraFormatArr['01D171']['required'] = 'X';
        //
        $eraFormatArr['01D025']['field_no'] = '01D025';
        $eraFormatArr['01D025']['field_name'] = 'Dam Electronic ID';
        $eraFormatArr['01D025']['length'] = 16;
        $eraFormatArr['01D025']['field_type'] = 'A/N';
        $eraFormatArr['01D025']['example'] = '7F7D28D57';
        $eraFormatArr['01D025']['domain'] = 'A-Z,0-9';
        $eraFormatArr['01D025']['required'] = 'X';
        //
        // Sire Service #0 (first)
        //
       $eraFormatArr['01D026']['field_no'] = '01D026';
        $eraFormatArr['01D026']['field_name'] = 'Service Number';
        $eraFormatArr['01D026']['length'] = 1;
        $eraFormatArr['01D026']['field_type'] = 'N';
        $eraFormatArr['01D026']['example'] = '1';
        $eraFormatArr['01D026']['domain'] = '1';
        $eraFormatArr['01D026']['required'] = 'M';
        //
        $eraFormatArr['01D027']['field_no'] = '01D027';
        $eraFormatArr['01D027']['field_name'] = 'Service Type';
        $eraFormatArr['01D027']['length'] = 1;
        $eraFormatArr['01D027']['field_type'] = 'A';
        $eraFormatArr['01D027']['example'] = 'H=Owner Inseminated(AI),T= Technician Inseminated (AI), N= Natural, P = Pasture/Paddock, I = Invitro';
        $eraFormatArr['01D027']['domain'] = 'H,T,N,P,I';
        $eraFormatArr['01D027']['required'] = 'M';
        //
        $eraFormatArr['01D028']['field_no'] = '01D028';
        $eraFormatArr['01D028']['field_name'] = 'Sire Breed';
        $eraFormatArr['01D028']['length'] = 2;
        $eraFormatArr['01D028']['field_type'] = 'A';
        $eraFormatArr['01D028']['example'] = 'HO';
        $eraFormatArr['01D028']['domain'] = 'A-Z';
        $eraFormatArr['01D028']['required'] = 'M';
        //
        $eraFormatArr['01D029']['field_no'] = '01D029';
        $eraFormatArr['01D029']['field_name'] = 'Sire Country';
        $eraFormatArr['01D029']['length'] = 3;
        $eraFormatArr['01D029']['field_type'] = 'A';
        $eraFormatArr['01D029']['example'] = 'CAN';
        $eraFormatArr['01D029']['domain'] = null;
        $eraFormatArr['01D029']['required'] = 'X';
        //
        $eraFormatArr['01D030']['field_no'] = '01D030';
        $eraFormatArr['01D030']['field_name'] = 'Sire Sex';
        $eraFormatArr['01D030']['length'] = 1;
        $eraFormatArr['01D030']['field_type'] = 'A';
        $eraFormatArr['01D030']['example'] = 'M';
        $eraFormatArr['01D030']['domain'] = 'M';
        $eraFormatArr['01D030']['required'] = 'M';
        //
        $eraFormatArr['01D111']['field_no'] = '01D111';
        $eraFormatArr['01D111']['field_name'] = 'Sire Reg. No. Prefix';
        $eraFormatArr['01D111']['length'] = 4;
        $eraFormatArr['01D111']['field_type'] = 'A/N';
        $eraFormatArr['01D111']['example'] = 'DR';
        $eraFormatArr['01D111']['domain'] = 'A-Z';
        $eraFormatArr['01D111']['required'] = 'X';
        //
        $eraFormatArr['01D031']['field_no'] = '01D031';
        $eraFormatArr['01D031']['field_name'] = 'Sire Reg. No.';
        $eraFormatArr['01D031']['length'] = 12;
        $eraFormatArr['01D031']['field_type'] = 'A/N';
        $eraFormatArr['01D031']['example'] = '1903838';
        $eraFormatArr['01D031']['domain'] = '1-99999999';
        $eraFormatArr['01D031']['required'] = 'X';
        //
        $eraFormatArr['01D112']['field_no'] = '01D112';
        $eraFormatArr['01D112']['field_name'] = 'Sire Reg. No. Suffix';
        $eraFormatArr['01D112']['length'] = 3;
        $eraFormatArr['01D112']['field_type'] = 'A/N';
        $eraFormatArr['01D112']['example'] = 'X';
        $eraFormatArr['01D112']['domain'] = 'A-Z';
        $eraFormatArr['01D112']['required'] = 'X';
        //
        $eraFormatArr['01D032']['field_no'] = '01D032';
        $eraFormatArr['01D032']['field_name'] = 'Sire Code';
        $eraFormatArr['01D032']['length'] = 11;
        $eraFormatArr['01D032']['field_type'] = 'A/N';
        $eraFormatArr['01D032']['example'] = '0072HO01024';
        $eraFormatArr['01D032']['domain'] = null;
        $eraFormatArr['01D032']['required'] = 'X';
        //
        $eraFormatArr['01D033']['field_no'] = '01D033';
        $eraFormatArr['01D033']['field_name'] = 'Semen Freeze Date';
        $eraFormatArr['01D033']['length'] = 8;
        $eraFormatArr['01D033']['field_type'] = 'N';
        $eraFormatArr['01D033']['example'] = '19910623';
        $eraFormatArr['01D033']['domain'] = null;
        $eraFormatArr['01D033']['required'] = 'O';
        //
        $eraFormatArr['01D034']['field_no'] = '01D034';
        $eraFormatArr['01D034']['field_name'] = 'Semen Freeze Code';
        $eraFormatArr['01D034']['length'] = 9;
        $eraFormatArr['01D034']['field_type'] = 'A/N';
        $eraFormatArr['01D034']['example'] = '14637';
        $eraFormatArr['01D034']['domain'] = null;
        $eraFormatArr['01D034']['required'] = 'O';
        //
        $eraFormatArr['01D035']['field_no'] = '01D035';
        $eraFormatArr['01D035']['field_name'] = 'Sire Birth Date';
        $eraFormatArr['01D035']['length'] = 8;
        $eraFormatArr['01D035']['field_type'] = 'N';
        $eraFormatArr['01D035']['example'] = '19860820';
        $eraFormatArr['01D035']['domain'] = null;
        $eraFormatArr['01D035']['required'] = 'X';
        //
        $eraFormatArr['01D113']['field_no'] = '01D113';
        $eraFormatArr['01D113']['field_name'] = 'Sire Name';
        $eraFormatArr['01D113']['length'] = 40;
        $eraFormatArr['01D113']['field_type'] = 'A/N';
        $eraFormatArr['01D113']['example'] = '';
        $eraFormatArr['01D113']['domain'] = null;
        $eraFormatArr['01D113']['required'] = 'X';
        //
        $eraFormatArr['01D036']['field_no'] = '01D036';
        $eraFormatArr['01D036']['field_name'] = 'Breeding/ Service Date/Starting Pasture Service Date';
        $eraFormatArr['01D036']['length'] = 8;
        $eraFormatArr['01D036']['field_type'] = 'N';
        $eraFormatArr['01D036']['example'] = '19910511';
        $eraFormatArr['01D036']['domain'] = null;
        $eraFormatArr['01D036']['required'] = 'M';
        //
        $eraFormatArr['01D114']['field_no'] = '01D114';
        $eraFormatArr['01D114']['field_name'] = 'Ending Pasture Date';
        $eraFormatArr['01D114']['length'] = 8;
        $eraFormatArr['01D114']['field_type'] = 'N';
        $eraFormatArr['01D114']['example'] = '19911231';
        $eraFormatArr['01D114']['domain'] = null;
        $eraFormatArr['01D114']['required'] = 'X';
        //
        $eraFormatArr['01D037']['field_no'] = '01D037';
        $eraFormatArr['01D037']['field_name'] = 'AI Insemination Code';
        $eraFormatArr['01D037']['length'] = 8;
        $eraFormatArr['01D037']['field_type'] = 'A/N';
        $eraFormatArr['01D037']['example'] = 'ONAI0070';
        $eraFormatArr['01D037']['domain'] = null;
        $eraFormatArr['01D037']['required'] = 'X';
        //
        $eraFormatArr['01D038']['field_no'] = '01D038';
        $eraFormatArr['01D038']['field_name'] = 'Semen Supplier Code';
        $eraFormatArr['01D038']['length'] = 8;
        $eraFormatArr['01D038']['field_type'] = 'A/N';
        $eraFormatArr['01D038']['example'] = 'ONAI0038';
        $eraFormatArr['01D038']['domain'] = null;
        $eraFormatArr['01D038']['required'] = 'O';
        //
        $eraFormatArr['01D039']['field_no'] = '01D039';
        $eraFormatArr['01D039']['field_name'] = 'On-Farm Code';
        $eraFormatArr['01D039']['length'] = 8;
        $eraFormatArr['01D039']['field_type'] = 'A/N';
        $eraFormatArr['01D039']['example'] = 'ONOF0039';
        $eraFormatArr['01D039']['domain'] = null;
        $eraFormatArr['01D039']['required'] = 'X';
        //
        $eraFormatArr['01D172']['field_no'] = '01D172';
        $eraFormatArr['01D172']['field_name'] = 'Sire EZE-IR Ear Tag';
        $eraFormatArr['01D172']['length'] = 12;
        $eraFormatArr['01D172']['field_type'] = 'N';
        $eraFormatArr['01D172']['example'] = '12345678';
        $eraFormatArr['01D172']['domain'] = '1-99999999';
        $eraFormatArr['01D172']['required'] = 'X';
        //
        $eraFormatArr['01D116']['field_no'] = '01D116';
        $eraFormatArr['01D116']['field_name'] = 'Sire Electronic ID';
        $eraFormatArr['01D116']['length'] = 16;
        $eraFormatArr['01D116']['field_type'] = 'A/N';
        $eraFormatArr['01D116']['example'] = '7F7D28D57';
        $eraFormatArr['01D116']['domain'] = 'A-Z,0-9';
        $eraFormatArr['01D116']['required'] = 'X';
        //
        $eraFormatArr['01D142']['field_no'] = '01D142';
        $eraFormatArr['01D142']['field_name'] = 'A.I. Breeding Slip Number';
        $eraFormatArr['01D142']['length'] = 9;
        $eraFormatArr['01D142']['field_type'] = 'A/N';
        $eraFormatArr['01D142']['example'] = '22';
        $eraFormatArr['01D142']['domain'] = null;
        $eraFormatArr['01D142']['required'] = 'O';
        //
        $eraFormatArr['01D143']['field_no'] = '01D143';
        $eraFormatArr['01D143']['field_name'] = 'Technician (Inseminator) Number';
        $eraFormatArr['01D143']['length'] = 5;
        $eraFormatArr['01D143']['field_type'] = 'A/N';
        $eraFormatArr['01D143']['example'] = '106';
        $eraFormatArr['01D143']['domain'] = null;
        $eraFormatArr['01D143']['required'] = 'O';
        //
        // Sire Service #1 (second)
        //
       $eraFormatArr['01D040']['field_no'] = '01D040';
        $eraFormatArr['01D040']['field_name'] = 'Service Number';
        $eraFormatArr['01D040']['length'] = 1;
        $eraFormatArr['01D040']['field_type'] = 'N';
        $eraFormatArr['01D040']['example'] = '1';
        $eraFormatArr['01D040']['domain'] = '1';
        $eraFormatArr['01D040']['required'] = 'M';
        //
        $eraFormatArr['01D041']['field_no'] = '01D041';
        $eraFormatArr['01D041']['field_name'] = 'Service Type';
        $eraFormatArr['01D041']['length'] = 1;
        $eraFormatArr['01D041']['field_type'] = 'A';
        $eraFormatArr['01D041']['example'] = 'H=Owner Inseminated(AI),T= Technician Inseminated (AI), N= Natural, P = Pasture/Paddock, I = Invitro';
        $eraFormatArr['01D041']['domain'] = 'H,T,N,P,I';
        $eraFormatArr['01D041']['required'] = 'M';
        //
        $eraFormatArr['01D042']['field_no'] = '01D042';
        $eraFormatArr['01D042']['field_name'] = 'Sire Breed';
        $eraFormatArr['01D042']['length'] = 2;
        $eraFormatArr['01D042']['field_type'] = 'A';
        $eraFormatArr['01D042']['example'] = 'HO';
        $eraFormatArr['01D042']['domain'] = 'A-Z';
        $eraFormatArr['01D042']['required'] = 'M';
        //
        $eraFormatArr['01D043']['field_no'] = '01D043';
        $eraFormatArr['01D043']['field_name'] = 'Sire Country';
        $eraFormatArr['01D043']['length'] = 3;
        $eraFormatArr['01D043']['field_type'] = 'A';
        $eraFormatArr['01D043']['example'] = 'CAN';
        $eraFormatArr['01D043']['domain'] = null;
        $eraFormatArr['01D043']['required'] = 'X';
        //
        $eraFormatArr['01D044']['field_no'] = '01D044';
        $eraFormatArr['01D044']['field_name'] = 'Sire Sex';
        $eraFormatArr['01D044']['length'] = 1;
        $eraFormatArr['01D044']['field_type'] = 'A';
        $eraFormatArr['01D044']['example'] = 'M';
        $eraFormatArr['01D044']['domain'] = 'M';
        $eraFormatArr['01D044']['required'] = 'M';
        //
        $eraFormatArr['01D117']['field_no'] = '01D117';
        $eraFormatArr['01D117']['field_name'] = 'Sire Reg. No. Prefix';
        $eraFormatArr['01D117']['length'] = 4;
        $eraFormatArr['01D117']['field_type'] = 'A/N';
        $eraFormatArr['01D117']['example'] = 'DR';
        $eraFormatArr['01D117']['domain'] = 'A-Z';
        $eraFormatArr['01D117']['required'] = 'X';
        //
        $eraFormatArr['01D045']['field_no'] = '01D045';
        $eraFormatArr['01D045']['field_name'] = 'Sire Reg. No.';
        $eraFormatArr['01D045']['length'] = 12;
        $eraFormatArr['01D045']['field_type'] = 'A/N';
        $eraFormatArr['01D045']['example'] = '1903838';
        $eraFormatArr['01D045']['domain'] = '1-99999999';
        $eraFormatArr['01D045']['required'] = 'X';
        //
        $eraFormatArr['01D118']['field_no'] = '01D118';
        $eraFormatArr['01D118']['field_name'] = 'Sire Reg. No. Suffix';
        $eraFormatArr['01D118']['length'] = 3;
        $eraFormatArr['01D118']['field_type'] = 'A/N';
        $eraFormatArr['01D118']['example'] = 'X';
        $eraFormatArr['01D118']['domain'] = 'A-Z';
        $eraFormatArr['01D118']['required'] = 'X';
        //
        $eraFormatArr['01D046']['field_no'] = '01D046';
        $eraFormatArr['01D046']['field_name'] = 'Sire Code';
        $eraFormatArr['01D046']['length'] = 11;
        $eraFormatArr['01D046']['field_type'] = 'A/N';
        $eraFormatArr['01D046']['example'] = '0072HO01024';
        $eraFormatArr['01D046']['domain'] = null;
        $eraFormatArr['01D046']['required'] = 'X';
        //
        $eraFormatArr['01D047']['field_no'] = '01D047';
        $eraFormatArr['01D047']['field_name'] = 'Semen Freeze Date';
        $eraFormatArr['01D047']['length'] = 8;
        $eraFormatArr['01D047']['field_type'] = 'N';
        $eraFormatArr['01D047']['example'] = '19910623';
        $eraFormatArr['01D047']['domain'] = null;
        $eraFormatArr['01D047']['required'] = 'O';
        //
        $eraFormatArr['01D048']['field_no'] = '01D048';
        $eraFormatArr['01D048']['field_name'] = 'Semen Freeze Code';
        $eraFormatArr['01D048']['length'] = 9;
        $eraFormatArr['01D048']['field_type'] = 'A/N';
        $eraFormatArr['01D048']['example'] = '14637';
        $eraFormatArr['01D048']['domain'] = null;
        $eraFormatArr['01D048']['required'] = 'O';
        //
        $eraFormatArr['01D049']['field_no'] = '01D049';
        $eraFormatArr['01D049']['field_name'] = 'Sire Birth Date';
        $eraFormatArr['01D049']['length'] = 8;
        $eraFormatArr['01D049']['field_type'] = 'N';
        $eraFormatArr['01D049']['example'] = '19860820';
        $eraFormatArr['01D049']['domain'] = null;
        $eraFormatArr['01D049']['required'] = 'X';
        //
        $eraFormatArr['01D119']['field_no'] = '01D119';
        $eraFormatArr['01D119']['field_name'] = 'Sire Name';
        $eraFormatArr['01D119']['length'] = 40;
        $eraFormatArr['01D119']['field_type'] = 'A/N';
        $eraFormatArr['01D119']['example'] = '';
        $eraFormatArr['01D119']['domain'] = null;
        $eraFormatArr['01D119']['required'] = 'X';
        //
        $eraFormatArr['01D050']['field_no'] = '01D050';
        $eraFormatArr['01D050']['field_name'] = 'Breeding/ Service Date/Starting Pasture Service Date';
        $eraFormatArr['01D050']['length'] = 8;
        $eraFormatArr['01D050']['field_type'] = 'N';
        $eraFormatArr['01D050']['example'] = '19910511';
        $eraFormatArr['01D050']['domain'] = null;
        $eraFormatArr['01D050']['required'] = 'M';
        //
        $eraFormatArr['01D120']['field_no'] = '01D120';
        $eraFormatArr['01D120']['field_name'] = 'Ending Pasture Date';
        $eraFormatArr['01D120']['length'] = 8;
        $eraFormatArr['01D120']['field_type'] = 'N';
        $eraFormatArr['01D120']['example'] = '19911231';
        $eraFormatArr['01D120']['domain'] = null;
        $eraFormatArr['01D120']['required'] = 'X';
        //
        $eraFormatArr['01D051']['field_no'] = '01D051';
        $eraFormatArr['01D051']['field_name'] = 'AI Insemination Code';
        $eraFormatArr['01D051']['length'] = 8;
        $eraFormatArr['01D051']['field_type'] = 'A/N';
        $eraFormatArr['01D051']['example'] = 'ONAI0070';
        $eraFormatArr['01D051']['domain'] = null;
        $eraFormatArr['01D051']['required'] = 'X';
        //
        $eraFormatArr['01D052']['field_no'] = '01D052';
        $eraFormatArr['01D052']['field_name'] = 'Semen Supplier Code';
        $eraFormatArr['01D052']['length'] = 8;
        $eraFormatArr['01D052']['field_type'] = 'A/N';
        $eraFormatArr['01D052']['example'] = 'ONAI0038';
        $eraFormatArr['01D052']['domain'] = null;
        $eraFormatArr['01D052']['required'] = 'O';
        //
        $eraFormatArr['01D053']['field_no'] = '01D053';
        $eraFormatArr['01D053']['field_name'] = 'On-Farm Code';
        $eraFormatArr['01D053']['length'] = 8;
        $eraFormatArr['01D053']['field_type'] = 'A/N';
        $eraFormatArr['01D053']['example'] = 'ONOF0039';
        $eraFormatArr['01D053']['domain'] = null;
        $eraFormatArr['01D053']['required'] = 'X';
        //
        $eraFormatArr['01D173']['field_no'] = '01D173';
        $eraFormatArr['01D173']['field_name'] = 'Sire EZE-IR Ear Tag';
        $eraFormatArr['01D173']['length'] = 12;
        $eraFormatArr['01D173']['field_type'] = 'N';
        $eraFormatArr['01D173']['example'] = '12345678';
        $eraFormatArr['01D173']['domain'] = '1-99999999';
        $eraFormatArr['01D173']['required'] = 'X';
        //
        $eraFormatArr['01D122']['field_no'] = '01D122';
        $eraFormatArr['01D122']['field_name'] = 'Sire Electronic ID';
        $eraFormatArr['01D122']['length'] = 16;
        $eraFormatArr['01D122']['field_type'] = 'A/N';
        $eraFormatArr['01D122']['example'] = '7F7D28D57';
        $eraFormatArr['01D122']['domain'] = 'A-Z,0-9';
        $eraFormatArr['01D122']['required'] = 'X';
        //
        $eraFormatArr['01D144']['field_no'] = '01D144';
        $eraFormatArr['01D144']['field_name'] = 'A.I. Breeding Slip Number';
        $eraFormatArr['01D144']['length'] = 9;
        $eraFormatArr['01D144']['field_type'] = 'A/N';
        $eraFormatArr['01D144']['example'] = '22';
        $eraFormatArr['01D144']['domain'] = null;
        $eraFormatArr['01D144']['required'] = 'O';
        //
        $eraFormatArr['01D145']['field_no'] = '01D145';
        $eraFormatArr['01D145']['field_name'] = 'Technician (Inseminator) Number';
        $eraFormatArr['01D145']['length'] = 5;
        $eraFormatArr['01D145']['field_type'] = 'A/N';
        $eraFormatArr['01D145']['example'] = '106';
        $eraFormatArr['01D145']['domain'] = null;
        $eraFormatArr['01D145']['required'] = 'O';
        //////////////////
        //
       // Sire Service #2 (third)
        //
       $eraFormatArr['01D054']['field_no'] = '01D054';
        $eraFormatArr['01D054']['field_name'] = 'Service Number';
        $eraFormatArr['01D054']['length'] = 1;
        $eraFormatArr['01D054']['field_type'] = 'N';
        $eraFormatArr['01D054']['example'] = '1';
        $eraFormatArr['01D054']['domain'] = '1';
        $eraFormatArr['01D054']['required'] = 'M';
        //
        $eraFormatArr['01D055']['field_no'] = '01D055';
        $eraFormatArr['01D055']['field_name'] = 'Service Type';
        $eraFormatArr['01D055']['length'] = 1;
        $eraFormatArr['01D055']['field_type'] = 'A';
        $eraFormatArr['01D055']['example'] = 'H=Owner Inseminated(AI),T= Technician Inseminated (AI), N= Natural, P = Pasture/Paddock, I = Invitro';
        $eraFormatArr['01D055']['domain'] = 'H,T,N,P,I';
        $eraFormatArr['01D055']['required'] = 'M';
        //
        $eraFormatArr['01D056']['field_no'] = '01D056';
        $eraFormatArr['01D056']['field_name'] = 'Sire Breed';
        $eraFormatArr['01D056']['length'] = 2;
        $eraFormatArr['01D056']['field_type'] = 'A';
        $eraFormatArr['01D056']['example'] = 'HO';
        $eraFormatArr['01D056']['domain'] = 'A-Z';
        $eraFormatArr['01D056']['required'] = 'M';
        //
        $eraFormatArr['01D057']['field_no'] = '01D057';
        $eraFormatArr['01D057']['field_name'] = 'Sire Country';
        $eraFormatArr['01D057']['length'] = 3;
        $eraFormatArr['01D057']['field_type'] = 'A';
        $eraFormatArr['01D057']['example'] = 'CAN';
        $eraFormatArr['01D057']['domain'] = null;
        $eraFormatArr['01D057']['required'] = 'X';
        //
        $eraFormatArr['01D058']['field_no'] = '01D058';
        $eraFormatArr['01D058']['field_name'] = 'Sire Sex';
        $eraFormatArr['01D058']['length'] = 1;
        $eraFormatArr['01D058']['field_type'] = 'A';
        $eraFormatArr['01D058']['example'] = 'M';
        $eraFormatArr['01D058']['domain'] = 'M';
        $eraFormatArr['01D058']['required'] = 'M';
        //
        $eraFormatArr['01D123']['field_no'] = '01D123';
        $eraFormatArr['01D123']['field_name'] = 'Sire Reg. No. Prefix';
        $eraFormatArr['01D123']['length'] = 4;
        $eraFormatArr['01D123']['field_type'] = 'A/N';
        $eraFormatArr['01D123']['example'] = 'DR';
        $eraFormatArr['01D123']['domain'] = 'A-Z';
        $eraFormatArr['01D123']['required'] = 'X';
        //
        $eraFormatArr['01D059']['field_no'] = '01D059';
        $eraFormatArr['01D059']['field_name'] = 'Sire Reg. No.';
        $eraFormatArr['01D059']['length'] = 12;
        $eraFormatArr['01D059']['field_type'] = 'A/N';
        $eraFormatArr['01D059']['example'] = '1903838';
        $eraFormatArr['01D059']['domain'] = '1-99999999';
        $eraFormatArr['01D059']['required'] = 'X';
        //
        $eraFormatArr['01D124']['field_no'] = '01D124';
        $eraFormatArr['01D124']['field_name'] = 'Sire Reg. No. Suffix';
        $eraFormatArr['01D124']['length'] = 3;
        $eraFormatArr['01D124']['field_type'] = 'A/N';
        $eraFormatArr['01D124']['example'] = 'X';
        $eraFormatArr['01D124']['domain'] = 'A-Z';
        $eraFormatArr['01D124']['required'] = 'X';
        //
        $eraFormatArr['01D060']['field_no'] = '01D060';
        $eraFormatArr['01D060']['field_name'] = 'Sire Code';
        $eraFormatArr['01D060']['length'] = 11;
        $eraFormatArr['01D060']['field_type'] = 'A/N';
        $eraFormatArr['01D060']['example'] = '0072HO01024';
        $eraFormatArr['01D060']['domain'] = null;
        $eraFormatArr['01D060']['required'] = 'X';
        //
        $eraFormatArr['01D061']['field_no'] = '01D061';
        $eraFormatArr['01D061']['field_name'] = 'Semen Freeze Date';
        $eraFormatArr['01D061']['length'] = 8;
        $eraFormatArr['01D061']['field_type'] = 'N';
        $eraFormatArr['01D061']['example'] = '19910623';
        $eraFormatArr['01D061']['domain'] = null;
        $eraFormatArr['01D061']['required'] = 'O';
        //
        $eraFormatArr['01D062']['field_no'] = '01D062';
        $eraFormatArr['01D062']['field_name'] = 'Semen Freeze Code';
        $eraFormatArr['01D062']['length'] = 9;
        $eraFormatArr['01D062']['field_type'] = 'A/N';
        $eraFormatArr['01D062']['example'] = '14637';
        $eraFormatArr['01D062']['domain'] = null;
        $eraFormatArr['01D062']['required'] = 'O';
        //
        $eraFormatArr['01D063']['field_no'] = '01D063';
        $eraFormatArr['01D063']['field_name'] = 'Sire Birth Date';
        $eraFormatArr['01D063']['length'] = 8;
        $eraFormatArr['01D063']['field_type'] = 'N';
        $eraFormatArr['01D063']['example'] = '19860820';
        $eraFormatArr['01D063']['domain'] = null;
        $eraFormatArr['01D063']['required'] = 'X';
        //
        $eraFormatArr['01D125']['field_no'] = '01D125';
        $eraFormatArr['01D125']['field_name'] = 'Sire Name';
        $eraFormatArr['01D125']['length'] = 40;
        $eraFormatArr['01D125']['field_type'] = 'A/N';
        $eraFormatArr['01D125']['example'] = '';
        $eraFormatArr['01D125']['domain'] = null;
        $eraFormatArr['01D125']['required'] = 'X';
        //
        $eraFormatArr['01D064']['field_no'] = '01D064';
        $eraFormatArr['01D064']['field_name'] = 'Breeding/ Service Date/Starting Pasture Service Date';
        $eraFormatArr['01D064']['length'] = 8;
        $eraFormatArr['01D064']['field_type'] = 'N';
        $eraFormatArr['01D064']['example'] = '19910511';
        $eraFormatArr['01D064']['domain'] = null;
        $eraFormatArr['01D064']['required'] = 'M';
        //
        $eraFormatArr['01D126']['field_no'] = '01D126';
        $eraFormatArr['01D126']['field_name'] = 'Ending Pasture Date';
        $eraFormatArr['01D126']['length'] = 8;
        $eraFormatArr['01D126']['field_type'] = 'N';
        $eraFormatArr['01D126']['example'] = '19911231';
        $eraFormatArr['01D126']['domain'] = null;
        $eraFormatArr['01D126']['required'] = 'X';
        //
        $eraFormatArr['01D065']['field_no'] = '01D065';
        $eraFormatArr['01D065']['field_name'] = 'AI Insemination Code';
        $eraFormatArr['01D065']['length'] = 8;
        $eraFormatArr['01D065']['field_type'] = 'A/N';
        $eraFormatArr['01D065']['example'] = 'ONAI0070';
        $eraFormatArr['01D065']['domain'] = null;
        $eraFormatArr['01D065']['required'] = 'X';
        //
        $eraFormatArr['01D066']['field_no'] = '01D066';
        $eraFormatArr['01D066']['field_name'] = 'Semen Supplier Code';
        $eraFormatArr['01D066']['length'] = 8;
        $eraFormatArr['01D066']['field_type'] = 'A/N';
        $eraFormatArr['01D066']['example'] = 'ONAI0038';
        $eraFormatArr['01D066']['domain'] = null;
        $eraFormatArr['01D066']['required'] = 'O';
        //
        $eraFormatArr['01D067']['field_no'] = '01D067';
        $eraFormatArr['01D067']['field_name'] = 'On-Farm Code';
        $eraFormatArr['01D067']['length'] = 8;
        $eraFormatArr['01D067']['field_type'] = 'A/N';
        $eraFormatArr['01D067']['example'] = 'ONOF0039';
        $eraFormatArr['01D067']['domain'] = null;
        $eraFormatArr['01D067']['required'] = 'X';
        //
        $eraFormatArr['01D174']['field_no'] = '01D174';
        $eraFormatArr['01D174']['field_name'] = 'Sire EZE-IR Ear Tag';
        $eraFormatArr['01D174']['length'] = 12;
        $eraFormatArr['01D174']['field_type'] = 'N';
        $eraFormatArr['01D174']['example'] = '12345678';
        $eraFormatArr['01D174']['domain'] = '1-99999999';
        $eraFormatArr['01D174']['required'] = 'X';
        //
        $eraFormatArr['01D128']['field_no'] = '01D128';
        $eraFormatArr['01D128']['field_name'] = 'Sire Electronic ID';
        $eraFormatArr['01D128']['length'] = 16;
        $eraFormatArr['01D128']['field_type'] = 'A/N';
        $eraFormatArr['01D128']['example'] = '7F7D28D57';
        $eraFormatArr['01D128']['domain'] = 'A-Z,0-9';
        $eraFormatArr['01D128']['required'] = 'X';
        //
        $eraFormatArr['01D146']['field_no'] = '01D146';
        $eraFormatArr['01D146']['field_name'] = 'A.I. Breeding Slip Number';
        $eraFormatArr['01D146']['length'] = 9;
        $eraFormatArr['01D146']['field_type'] = 'A/N';
        $eraFormatArr['01D146']['example'] = '22';
        $eraFormatArr['01D146']['domain'] = null;
        $eraFormatArr['01D146']['required'] = 'O';
        //
        $eraFormatArr['01D147']['field_no'] = '01D147';
        $eraFormatArr['01D147']['field_name'] = 'Technician (Inseminator) Number';
        $eraFormatArr['01D147']['length'] = 5;
        $eraFormatArr['01D147']['field_type'] = 'A/N';
        $eraFormatArr['01D147']['example'] = '106';
        $eraFormatArr['01D147']['domain'] = null;
        $eraFormatArr['01D147']['required'] = 'O';
        // 
        // Animal Embryo Transfer Calf
        //
        $eraFormatArr['01D068']['field_no'] = '01D068';
        $eraFormatArr['01D068']['field_name'] = 'Recovery Date of Embryo';
        $eraFormatArr['01D068']['length'] = 8;
        $eraFormatArr['01D068']['field_type'] = 'N';
        $eraFormatArr['01D068']['example'] = '19910812';
        $eraFormatArr['01D068']['domain'] = null;
        $eraFormatArr['01D068']['required'] = 'X';
        //
        $eraFormatArr['01D069']['field_no'] = '01D069';
        $eraFormatArr['01D069']['field_name'] = 'Embryo Manipulation Type';
        $eraFormatArr['01D069']['length'] = 1;
        $eraFormatArr['01D069']['field_type'] = 'A';
        $eraFormatArr['01D069']['example'] = 'R=Regular S=Split C=Clone N=Nuclear Transfer D=Divided B=Bisected';
        $eraFormatArr['01D069']['domain'] = 'R,S,C,N,D,B';
        $eraFormatArr['01D069']['required'] = 'X';
        //
        $eraFormatArr['01D070']['field_no'] = '01D070';
        $eraFormatArr['01D070']['field_name'] = 'Individual Embryo Identification';
        $eraFormatArr['01D070']['length'] = 30;
        $eraFormatArr['01D070']['field_type'] = 'A/N';
        $eraFormatArr['01D070']['example'] = 'CANF540132719910 323123';
        $eraFormatArr['01D070']['domain'] = null;
        $eraFormatArr['01D070']['required'] = 'X';
        //
        $eraFormatArr['01D071']['field_no'] = '01D071';
        $eraFormatArr['01D071']['field_name'] = 'Recipient Dam Identification';
        $eraFormatArr['01D071']['length'] = 30;
        $eraFormatArr['01D071']['field_type'] = 'A/N';
        $eraFormatArr['01D071']['example'] = 'CANF12345678';
        $eraFormatArr['01D071']['domain'] = 'A-Z,1- 99999999';
        $eraFormatArr['01D071']['required'] = 'X';
        //
        $eraFormatArr['01D072']['field_no'] = '01D072';
        $eraFormatArr['01D072']['field_name'] = 'Embryo Transplant Date (into recipient)';
        $eraFormatArr['01D072']['length'] = 8;
        $eraFormatArr['01D072']['field_type'] = 'N';
        $eraFormatArr['01D072']['example'] = '19910827';
        $eraFormatArr['01D072']['domain'] = null;
        $eraFormatArr['01D072']['required'] = 'X';
        //
        //OWNER ID
        //
       $eraFormatArr['01D076']['field_no'] = '01D076';
        $eraFormatArr['01D076']['field_name'] = 'Prefix/ClientID';
        $eraFormatArr['01D076']['length'] = 25;
        $eraFormatArr['01D076']['field_type'] = 'A/N';
        $eraFormatArr['01D076']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D076']['domain'] = null;
        $eraFormatArr['01D076']['required'] = 'M';
        //
        $eraFormatArr['01D077']['field_no'] = '01D077';
        $eraFormatArr['01D077']['field_name'] = 'CHIN No.(Telephone No.)';
        $eraFormatArr['01D077']['length'] = 12;
        $eraFormatArr['01D077']['field_type'] = 'A/N';
        $eraFormatArr['01D077']['example'] = '416-253-4724';
        $eraFormatArr['01D077']['domain'] = null;
        $eraFormatArr['01D077']['required'] = 'O';
        //
        $eraFormatArr['01D078']['field_no'] = '01D078';
        $eraFormatArr['01D078']['field_name'] = 'Milk Recording Program Herd No.';
        $eraFormatArr['01D078']['length'] = 5;
        $eraFormatArr['01D078']['field_type'] = 'N';
        $eraFormatArr['01D078']['example'] = '54321';
        $eraFormatArr['01D078']['domain'] = null;
        $eraFormatArr['01D078']['required'] = 'O';
        //
        // #2
        //
       $eraFormatArr['01D215']['field_no'] = '01D215';
        $eraFormatArr['01D215']['field_name'] = 'Prefix/ClientID 2';
        $eraFormatArr['01D215']['length'] = 25;
        $eraFormatArr['01D215']['field_type'] = 'A/N';
        $eraFormatArr['01D215']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D215']['domain'] = null;
        $eraFormatArr['01D215']['required'] = 'X';
        //
        $eraFormatArr['01D216']['field_no'] = '01D216';
        $eraFormatArr['01D216']['field_name'] = 'CHIN No.(Telephone No.) 2';
        $eraFormatArr['01D216']['length'] = 12;
        $eraFormatArr['01D216']['field_type'] = 'A/N';
        $eraFormatArr['01D216']['example'] = '416-253-4724';
        $eraFormatArr['01D216']['domain'] = null;
        $eraFormatArr['01D216']['required'] = 'O';
        //
        $eraFormatArr['01D217']['field_no'] = '01D217';
        $eraFormatArr['01D217']['field_name'] = 'Milk Recording Program Herd No. 2';
        $eraFormatArr['01D217']['length'] = 5;
        $eraFormatArr['01D217']['field_type'] = 'N';
        $eraFormatArr['01D217']['example'] = '54321';
        $eraFormatArr['01D217']['domain'] = null;
        $eraFormatArr['01D217']['required'] = 'O';
        //
        // #3
        //
       $eraFormatArr['01D218']['field_no'] = '01D218';
        $eraFormatArr['01D218']['field_name'] = 'Prefix/ClientID 3';
        $eraFormatArr['01D218']['length'] = 25;
        $eraFormatArr['01D218']['field_type'] = 'A/N';
        $eraFormatArr['01D218']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D218']['domain'] = null;
        $eraFormatArr['01D218']['required'] = 'X';
        //
        $eraFormatArr['01D219']['field_no'] = '01D219';
        $eraFormatArr['01D219']['field_name'] = 'CHIN No.(Telephone No.) 3';
        $eraFormatArr['01D219']['length'] = 12;
        $eraFormatArr['01D219']['field_type'] = 'A/N';
        $eraFormatArr['01D219']['example'] = '416-253-4724';
        $eraFormatArr['01D219']['domain'] = null;
        $eraFormatArr['01D219']['required'] = 'O';
        //
        $eraFormatArr['01D220']['field_no'] = '01D220';
        $eraFormatArr['01D220']['field_name'] = 'Milk Recording Program Herd No. 3';
        $eraFormatArr['01D220']['length'] = 5;
        $eraFormatArr['01D220']['field_type'] = 'N';
        $eraFormatArr['01D220']['example'] = '54321';
        $eraFormatArr['01D220']['domain'] = null;
        $eraFormatArr['01D220']['required'] = 'O';
        //
        // #4
        //
       $eraFormatArr['01D221']['field_no'] = '01D221';
        $eraFormatArr['01D221']['field_name'] = 'Prefix/ClientID 4';
        $eraFormatArr['01D221']['length'] = 25;
        $eraFormatArr['01D221']['field_type'] = 'A/N';
        $eraFormatArr['01D221']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D221']['domain'] = null;
        $eraFormatArr['01D221']['required'] = 'X';
        //
        $eraFormatArr['01D222']['field_no'] = '01D222';
        $eraFormatArr['01D222']['field_name'] = 'CHIN No.(Telephone No.) 4';
        $eraFormatArr['01D222']['length'] = 12;
        $eraFormatArr['01D222']['field_type'] = 'A/N';
        $eraFormatArr['01D222']['example'] = '416-253-4724';
        $eraFormatArr['01D222']['domain'] = null;
        $eraFormatArr['01D222']['required'] = 'O';
        //
        $eraFormatArr['01D223']['field_no'] = '01D223';
        $eraFormatArr['01D223']['field_name'] = 'Milk Recording Program Herd No. 4';
        $eraFormatArr['01D223']['length'] = 5;
        $eraFormatArr['01D223']['field_type'] = 'N';
        $eraFormatArr['01D223']['example'] = '54321';
        $eraFormatArr['01D223']['domain'] = null;
        $eraFormatArr['01D223']['required'] = 'O';
        //
        // #5
        //
       $eraFormatArr['01D224']['field_no'] = '01D224';
        $eraFormatArr['01D224']['field_name'] = 'Prefix/ClientID 5';
        $eraFormatArr['01D224']['length'] = 25;
        $eraFormatArr['01D224']['field_type'] = 'A/N';
        $eraFormatArr['01D224']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D224']['domain'] = null;
        $eraFormatArr['01D224']['required'] = 'X';
        //
        $eraFormatArr['01D225']['field_no'] = '01D225';
        $eraFormatArr['01D225']['field_name'] = 'CHIN No.(Telephone No.) 5';
        $eraFormatArr['01D225']['length'] = 12;
        $eraFormatArr['01D225']['field_type'] = 'A/N';
        $eraFormatArr['01D225']['example'] = '416-253-4724';
        $eraFormatArr['01D225']['domain'] = null;
        $eraFormatArr['01D225']['required'] = 'O';
        //
        $eraFormatArr['01D226']['field_no'] = '01D226';
        $eraFormatArr['01D226']['field_name'] = 'Milk Recording Program Herd No. 5';
        $eraFormatArr['01D226']['length'] = 5;
        $eraFormatArr['01D226']['field_type'] = 'N';
        $eraFormatArr['01D226']['example'] = '54321';
        $eraFormatArr['01D226']['domain'] = null;
        $eraFormatArr['01D226']['required'] = 'O';
        //
        // #6
        //
       $eraFormatArr['01D227']['field_no'] = '01D227';
        $eraFormatArr['01D227']['field_name'] = 'Prefix/ClientID 6';
        $eraFormatArr['01D227']['length'] = 25;
        $eraFormatArr['01D227']['field_type'] = 'A/N';
        $eraFormatArr['01D227']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D227']['domain'] = null;
        $eraFormatArr['01D227']['required'] = 'X';
        //
        $eraFormatArr['01D228']['field_no'] = '01D228';
        $eraFormatArr['01D228']['field_name'] = 'CHIN No.(Telephone No.) 6';
        $eraFormatArr['01D228']['length'] = 12;
        $eraFormatArr['01D228']['field_type'] = 'A/N';
        $eraFormatArr['01D228']['example'] = '416-253-4724';
        $eraFormatArr['01D228']['domain'] = null;
        $eraFormatArr['01D228']['required'] = 'O';
        //
        $eraFormatArr['01D229']['field_no'] = '01D229';
        $eraFormatArr['01D229']['field_name'] = 'Milk Recording Program Herd No. 6';
        $eraFormatArr['01D229']['length'] = 5;
        $eraFormatArr['01D229']['field_type'] = 'N';
        $eraFormatArr['01D229']['example'] = '54321';
        $eraFormatArr['01D229']['domain'] = null;
        $eraFormatArr['01D229']['required'] = 'O';
        //
        // END STUFF
        //      
        $eraFormatArr['01D130']['field_no'] = '01D130';
        $eraFormatArr['01D130']['field_name'] = 'Breeder Prefix/Client ID';
        $eraFormatArr['01D130']['length'] = 25;
        $eraFormatArr['01D130']['field_type'] = 'A/N';
        $eraFormatArr['01D130']['example'] = 'BRANTCROFT';
        $eraFormatArr['01D130']['domain'] = null;
        $eraFormatArr['01D130']['required'] = 'M';
        //
        $eraFormatArr['01DEND']['field_no'] = '01DEND';
        $eraFormatArr['01DEND']['field_name'] = 'No. of data fields sent for the animal including 01DEND record';
        $eraFormatArr['01DEND']['length'] = 3;
        $eraFormatArr['01DEND']['field_type'] = 'N';
        $eraFormatArr['01DEND']['example'] = '999';
        $eraFormatArr['01DEND']['domain'] = null;
        $eraFormatArr['01DEND']['required'] = 'M';
        //
        $eraFormatArr['01EEND']['field_no'] = '01EEND';
        $eraFormatArr['01EEND']['field_name'] = 'Total No. of Data records sent. Num animals sent + 2';
        $eraFormatArr['01EEND']['length'] = 3;
        $eraFormatArr['01EEND']['field_type'] = 'N';
        $eraFormatArr['01EEND']['example'] = '999';
        $eraFormatArr['01EEND']['domain'] = null;
        $eraFormatArr['01EEND']['required'] = 'M';
        //
        return $eraFormatArr;
    }

    /**
     * 
     * 
     * used for holstein canada registration and here to find most likely conception event.
     */
    public static function listMostLikelyConceptionEvents($calvingTime, $bovine_id, $orderByTime = false,$calvingIntervalRange=21) {
        //calvingIntervalRange days plus or minus the calving date to look for breedings (minus 280 days).
        
           
        // Open a transaction
        try {$res = $GLOBALS['pdo']->beginTransaction();

        //temp table is used so we can sort it two different ways.  
        $sql = "
          CREATE OR REPLACE TEMP VIEW temp_conc_events_$bovine_id AS
            SELECT combined_breeding_embryo_view.id as com_id,table_id, type,event_time,recipient_bovine_id,service_sire_full_name,service_sire_short_name,service_sire_full_reg_number,donor_dam_bovine_id,donor_dam_local_number,donor_dam_full_name,bovinemanagement.short_name(donor_dam_full_name) as donor_dam_short_name, donor_dam_full_reg_number,calculated_potential_due_date,
	date_trunc('days','$calvingTime'- ( calculated_potential_due_date - interval '280 days')) as days_ago, abs(calculated_potential_due_date::date - '$calvingTime'::date) as time_since
 FROM bovinemanagement.combined_breeding_embryo_view 
WHERE recipient_bovine_id=$bovine_id AND calculated_potential_due_date >= (date '$calvingTime'- interval '$calvingIntervalRange days') AND calculated_potential_due_date <= (date '$calvingTime'+ interval '$calvingIntervalRange days') AND type='breeding'
            UNION
            SELECT combined_breeding_embryo_view.id as com_id,table_id, type,event_time,recipient_bovine_id,service_sire_full_name,service_sire_short_name,service_sire_full_reg_number,donor_dam_bovine_id,donor_dam_local_number,donor_dam_full_name,bovinemanagement.short_name(donor_dam_full_name) as donor_dam_short_name, donor_dam_full_reg_number,calculated_potential_due_date,
	date_trunc('days','$calvingTime'- ( calculated_potential_due_date - interval '273 days')) as days_ago, abs(calculated_potential_due_date::date - '$calvingTime'::date) as time_since
 FROM bovinemanagement.combined_breeding_embryo_view 
WHERE recipient_bovine_id=$bovine_id AND calculated_potential_due_date >= (date '$calvingTime'- interval '$calvingIntervalRange days') AND calculated_potential_due_date <= (date '$calvingTime'+ interval '$calvingIntervalRange days') AND type='embryo7day'
       
            ";
        $res = $GLOBALS['pdo']->query($sql);

        //orders by closest to calving date + or -.
        if ($orderByTime == false) {
            $sql2 = " SELECT * FROM temp_conc_events_$bovine_id ORDER BY time_since ASC;";
        }
        //orders from most current to least current holstein canada wants it this way.
        else {
            $sql2 = "With temp as (SELECT * FROM temp_conc_events_$bovine_id ORDER BY time_since ASC LIMIT 3) SELECT * FROM temp ORDER BY event_time DESC;";
        }


        $res2 = $GLOBALS['pdo']->query($sql2);

        $outArr = array();
        $counter=0;
        while ($row = $res2->fetch(PDO::FETCH_ASSOC)) {
            
            //Alta Genetics has a tenency to name there bull things like ALTAIOTA.
            //we don't like marketing for them, so replace this and just leave whatever is left.
            $row['service_sire_short_name']=str_replace ('ALTA' , '', $row['service_sire_short_name'] );
            
            $i = $counter;//start index at zero and continue higher.
            $outArr[$i]['com_id'] = $row['com_id'];
            $outArr[$i]['type'] = $row['type'];
            $outArr[$i]['event_time'] = $row['event_time'];
            $outArr[$i]['table_id'] = $row['table_id'];
            $outArr[$i]['days_ago'] = $row['days_ago'];
            $outArr[$i]['service_sire_short_name'] = $row['service_sire_short_name'];
            $outArr[$i]['service_sire_full_reg_number'] = $row['service_sire_full_reg_number'];
            $outArr[$i]['donor_dam_local_number'] = $row['donor_dam_local_number'];
            $outArr[$i]['donor_dam_full_name'] = $row['donor_dam_full_name'];
            $outArr[$i]['donor_dam_short_name'] = $row['donor_dam_short_name'];
            $counter++;
        }

        //drop temp table.
        $sql3 = " DROP view temp_conc_events_$bovine_id ;";
        $res3 = $GLOBALS['pdo']->query($sql3);


        // determine if the commit or rollback

           $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            
            
        //emergency stop, this would get triggered when abortions are put in and we milk the cow, so thus they need a laction and to have calved.
          if (($calvingIntervalRange>=180))    {      
           throw new HolsteinCanadaERA_NoConceptionEventException("Error: No conception events for dam (bovine_id=$bovine_id) to regsiter calf going back $calvingIntervalRange days.". "</br>\n\r");
           exit();
       }
       
       //does a RECURSIVE search, looking for the breeding if necessary.
       if (count($outArr)==0)     {
           //run it again recursively until it finds a conception event.
           $calvingIntervalRange=$calvingIntervalRange+11;
           $outArr = self::listMostLikelyConceptionEvents($calvingTime, $bovine_id, $orderByTime,$calvingIntervalRange);
          }

            
           
        return $outArr;
    }

}

//end class
?>