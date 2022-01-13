<?php

/**
  File to read shurgain feed analysis reports and store in db
  Just copy the pdfs from shurgain into the forageAnalysis directory and go. This will ignore valacta and other reports.
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
    $_SERVER['DOCUMENT_ROOT'] = dirname(__DIR__) . '/'; //when called from cli, command line define constant.
} else {
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
}

/*
 * reads files from shurgain directory of forage sanple results. modified in 2018, nut used since 2015 or so before. changed so that
 * it looks for the word DAVID in the filename, skips others, this is so it only picks up newer files, can change back if necessary in future.
 * 
 * BIG REWRITE 2018
 * CHanged to json so if file format of report changes it is not a big deal. first job is to get a report machine readable and second one is use with NRC, so this 
 * script does both those jobs. This change breaks many things. 
 * 
 * 2019
 * Shurgain changed how the report looks, so thus need to rewrite again. 
 */

//lots of good forumelas here: http://www.agtest.com/articles/FEED%20AND%20FORAGES%20CALCULATIONS_new.pdf
class forageAnalysisReportsParseShurgain {

    public $filepath;

    public function sse($request = null) {
        $this->main();
    }

    /*
     * process through files. 
     */

    function main() {

        $sql = "SELECT id,data::bytea,file_name FROM documents.forage_analysis_shur_gain ORDER BY file_name DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
        $stmt->execute();

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $data, PDO::PARAM_LOB);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);


        SSE_Message::send_message(1, 'Starting.', 1); //sse

        $arrayCount = ($count = $stmt->rowCount());
        $i = 0;
        while ($lines = $stmt->fetch(PDO::FETCH_BOUND)) {

            SSE_Message::send_message(6 + $i, 'Doing around ' . $fileName . $i . ' of ' . $arrayCount, 6 + round(($i / $arrayCount) * 100, 1)); //sse

            $fileStr = ( ((stream_get_contents($data)))); //note this is binary datat, no escaping when it was inserting or now when selecting.

            $temp = tmpfile(); //create temp file
            $path = stream_get_meta_data($temp)['uri']; //get full path

            fwrite($temp, $fileStr); //write string to file
            fseek($temp, 0); //set pointer to 0 position.
            //NOTE: There is one file from 2012, that has many tests in one PDF file. 
            //This code does not support that any more, because...well its a waste of time to code for shur-gain being stupid. 
            //Just break it up if need be in the future.      

            $filename = $fileName;
            print("Doing File:$fileName" . " ....<br>\n\r");


            try {

                $page = self::readOnePageToText($path);
                if (!empty($page)) {

                    //parse page data and insert into DB.
                    self::parseSamplePage($page, $path);
                } else {
                    throw new Exception("NOTICE: Skipping file: $filename, no certificate number found, thus not a valid file for parsing.\n\r");
                }
            } catch (Exception $e) {
                echo "SKIPPING PAGE (OR END OF FILE), Caught EXCEPTION for file $filename", $e->getMessage(), "\n";
            }
            $i++;         
        }//filename loop.
          SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
    }

//end function

    function readOnePageToText($filename) {
        $rowArray2 = array();



//now run unix poppler utility to convert pdf to text. http://poppler.freedesktop.org
        $retVal = null;
        $output = null;
        $retVal = exec("pdftotext '$filename' -layout -", $output);

        //DEBUG
        //print_r(implode("\n\r", $output));
        //look for sample no to see if this is a valid feed analysis page. if it is, exit and tell function to stop
        $matches = null;
        //look for 
        preg_match("/Lab sample ID\s*(?<sampleno>(E\d*))\s/", implode("\n\r", $output), $matches);

        //return null or page text
        if (!empty((isset($matches['sampleno'])))) {
            return $output;
        } else {
            return null;
        }
    }

    function parseSamplePage($page, $filename) {




        if (file_exists($filename)) {
            // Get the file size 
            $file_size = filesize($filename);
            // Open the file 
            $fh = fopen($filename, "r");
            $contents = fread($fh, filesize($filename));
            fclose($fh);
            $rowArray2['pdf_file_report']['answer'] = pg_escape_bytea($contents);
        } else {
            throw new Exception("ERROR: Cannot read forage anaylisis file for some reason, which should not happen becuase it was just read to get this far. EXITING.");
            exit();
        }


        //already found above, so we are sure it is here.
        $matches = null;
        preg_match("/Lab sample ID\s*(?<sampleno>(E\d*))\s/", implode("\n\r", $page), $matches);
        $rowArray2['Sample no'] = '' . $matches['sampleno'];

//get sample date.
        preg_match("/Date sampled\s*(?<date>(\d\d\d\d-\d\d-\d\d))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            $rowArray2['Sample Date'] = $matches['date'];
        }


//get received date.
        preg_match("/Date received\s*(?<date>(\d\d\d\d-\d\d-\d\d))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Received Date'] = $matches['date'];
        }

//get reported date.
        preg_match("/Date reported\s*(?<date>(\d\d\d\d-\d\d-\d\d))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Analysis date'] = $matches['date'];
        } else {
            throw new Exception("Reported date not found for page $startPage." . "\n\r");
        }


//get sample comment
        preg_match("/Comments\s*(?<comment>([\x20-\x7E]*))\n\r(?<comment2>([\x20-\x7E]*))/", implode("\n\r", $page), $matches);
        $rowArray2['Identification']['answer'] = $matches['comment'] . ' ' . $matches['comment2'];


        //using template parse file for numbers for raw report
        $rowArray = $this->shurgainSilageTemplateRawReport();
        foreach ($rowArray as $key => $row) {
            $rowArrayBasic[$key] = self::getAnalysisNumbers($row['text'], implode("\n\r", $page), $row['unit'], $row['numberColumn'], $row['optional']); //key is text
        }



        //using raw parsed data generate values for NRC 2001 model.
        $rowArrayNRCBasic = $this->generateNRC2001ValuesFromRawValues($rowArrayBasic);



//fix analysis date
        $rowArray2['Analysis date'] = date('r', strtotime($rowArray2['Analysis date']));


// Open a transaction
        try {
            //$GLOBALS['pdo']->setAttribute(PDO::ATTR_AUTOCOMMIT, 0);
            $res = $GLOBALS['pdo']->beginTransaction();

//use sample number to see if this forage anaylsis has already been inserted into the db, if so just skip it.
            $sql = "SELECT sample_number FROM nutrition.bag_analysis WHERE sample_number='{$rowArray2['Sample no']}'";
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            if ($row == null) {  //ie not already in db, then proceed with insert
                $comment = pg_escape_string($rowArray2['Identification']['answer']);

                print("\n\r\n\r" . "Doing INSERT of {$rowArray2['Sample no']} " . "$comment" . " \n\r\n\r");

                //var_dump($rowArray);
                //add things to raw report.
                $rowArrayBasic['comment'] = $rowArray2['Identification']['answer'];

                //create info template.
                $infoArr['DM'] = (float) $rowArrayBasic['DRY MATTER'];
                $infoArr['RFQ'] = (float) $rowArrayBasic['FORAGE QUALITY INDEX'];
                $infoArr['TDN'] = (float) $rowArrayNRCBasic['TDN (%DM)'];
                $infoArr['CP'] = (float) $rowArrayBasic['PROTEIN NIR'];
                $infoArr['Ca'] = (float) $rowArrayBasic['*CALCIUM'];
                $infoArr['P'] = (float) $rowArrayBasic['*PHOSPHORUS'];
                $infoArr['K'] = (float) $rowArrayBasic['*POTASSIUM'];
                $infoArr['ADF'] = (float) $rowArrayBasic['ACID DETERGENT FIBER NIR'];
                $infoArr['NDF'] = (float) $rowArrayBasic['NDF(OM) NIR'];
                $infoArr['lignin'] = (float) $rowArrayBasic['ACID DETERGENT FIBER LIGNIN NIR'];
                $infoArr['ash'] = (float) $rowArrayBasic['ASH'];
                $infoArr['starch'] = null; //$infoArr['starch']=$rowArrayBasic['starch'];//add later
                //encode json to store in db. 
                $rawReportJson = json_encode($rowArrayBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $NRCJson = json_encode($rowArrayNRCBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $infoJson = json_encode($infoArr, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);

                //now insert into db.
                $sql = "INSERT INTO nutrition.bag_analysis (bag_id,footage,sample_number,sample_date,userid, report_file, lab_name, raw_report	,nrc2001_template,info_template) VALUES (null,null,'{$rowArray2['Sample no']}','{$rowArray2['Analysis date']}','system','{$rowArray2['pdf_file_report']['answer']}','shur-gain','$rawReportJson','$NRCJson','$infoJson')";

                $res = $GLOBALS['pdo']->exec($sql);
            } else {
                print("\n\r\n\r" . "SKIPPING, already in: {$rowArray2['Sample no']}" . " \n\r\n\r");
            }


// determine if the commit or rollback

            $GLOBALS['pdo']->commit();
            //$GLOBALS['pdo']->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
// Close the transaction
    }

//end function
    //turns parsed raw data into something nrc can use to overlay other forage types.
    function generateNRC2001ValuesFromRawValues($rowArray) {


        !empty($rowArray['NEL']) ? $nel = $rowArray['NEL'] : $nel = $rowArray['NEL2']; //pick the NEL that it actuall found. 

        $outArray['TDN (%DM)'] = (float) round((($nel + .12) / 0.0245), 2); //verified true uc davis. 
        $outArray['DE (Mcal/kg)'] = (float) round(($outArray['TDN (%DM)'] * 0.044), 2); //verified true uc davis.   //FROM: http://animalsciencey.ucdavis.edu/java/LivestockSystemMgt/Conversion/energy.htm
        $outArray['DM (%AF)'] = (float) $rowArray['DRY MATTER'];  //verified true with shurgain
        $outArray['NDF (%DM)'] = (float) $rowArray['NDF(OM) NIR'];
        $outArray['ADF (%DM)'] = (float) $rowArray['ACID DETERGENT FIBER NIR'];
        $outArray['Lignin (%DM)'] = (float) $rowArray['ACID DETERGENT FIBER LIGNIN NIR'];
        $outArray['CP (%DM)'] = (float) $rowArray['PROTEIN NIR'];  //verified true with shurgain
        $outArray['NDFIP (%DM)'] = null; //not measured by shur-gain, book values for forages have small variance. don't worry use book value.
        $outArray['ADFIP (%DM)'] = null; //not measured by shur-gain, book values for forages have small variance. don't worry use book value.
        $outArray['Prt-A (%CP)'] = (float) $rowArray['A FRACTION'];
        $outArray['Prt-B (%CP)'] = (float) round(( 100 - ($rowArray['A FRACTION'] + $rowArray['B FRACTION'])), 2);
        $outArray['Prt-C (%CP)'] = (float) $rowArray['B FRACTION'];
        //
        $outArray['Kd (1/hr)'] = (float) $rowArray['K2']; //somewhat verified 
        $outArray['RUP Digest (%)'] = (float) $rowArray['POTENTIAL DIGESTIBILITY']; //somewhat verified, Not measured source:  John Metcalf
        $outArray['Fat (%DM)'] = (float) $rowArray['CRUDE FAT NIR']; //verified true with shurgain
        $outArray['Ash (%DM)'] = (float) $rowArray['ASH']; //verified true with shurgain    
        //
        $outArray['Sodium'] = (float) $rowArray['*SODIUM'];
        $outArray['Calcium'] = (float) $rowArray['*CALCIUM'];
        $outArray['Phosphorus'] = (float) $rowArray['*PHOSPHORUS'];
        $outArray['Magnesium'] = (float) $rowArray['*MAGNESIUM'];
        $outArray['Potassium'] = (float) $rowArray['*POTASSIUM'];
        $outArray['Sulphur'] = (float) $rowArray['SULFUR'];

        return $outArray;
    }

    /*
     * this function is the template of how to do the preg match row by row of the file. chnage the text portion when needed.
     */

    function shurgainSilageTemplateRawReport() {
        $rowArray['ASH'] = array('text' => '*Ash', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
        $rowArray['DRY MATTER'] = array('text' => 'Dry Matter', 'unit' => '%', 'numberColumn' => 3, 'optional' => true); //verified true with shurgain
        $rowArray['PROTEIN NIR'] = array('text' => 'Protein NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
        $rowArray['ADFN'] = array('text' => 'ADFN NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //ADF nitrogen portion??? ie crude protein portion?

        $rowArray['NEL'] = array('text' => 'Net Energy Lactation', 'unit' => 'Mcal\/kg', 'numberColumn' => 1, 'optional' => true);
        $rowArray['NEL2'] = array('text' => 'Net Energy Lactation Mcal/kg', 'unit' => 'Mcal\/kg', 'numberColumn' => 1, 'optional' => true); //shurgain changed how they displayed NEL, sometims with units and sometimes without in text.        

        $rowArray['CRUDE FAT NIR'] = array('text' => 'Crude Fat NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
        $rowArray['NDF(OM) NIR'] = array('text' => 'NDF(OM) NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //more accurate NDF I think.
        $rowArray['ACID DETERGENT FIBER NIR'] = array('text' => 'Acid Detergent Fiber NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //more accurate ADF. I think.
        $rowArray['NON STRUCTURAL CARBOHYDRATE'] = array('text' => 'Non Structural Carbohydrates', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['IF'] = array('text' => 'IF', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['POTENTIAL DIGESTIBILITY'] = array('text' => 'Potential Digestibility', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*SODIUM'] = array('text' => '*Sodium', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*CALCIUM'] = array('text' => '*Calcium', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*PHOSPHORUS'] = array('text' => '*Phosphorus', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*MAGNESIUM'] = array('text' => '*Magnesium', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*POTASSIUM'] = array('text' => '*Potassium', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['SULFUR'] = array('text' => 'Sulfur', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['GAIN ENERGY'] = array('text' => 'Gaine Energy', 'unit' => 'MJ\/kg', 'numberColumn' => 1, 'optional' => true);
        $rowArray['MAINT. ENERGY'] = array('text' => 'Maint. Energy', 'unit' => 'MJ\/kg', 'numberColumn' => 1, 'optional' => true);
        $rowArray['FORAGE QUALITY INDEX'] = array('text' => 'Forage Quality Index', 'unit' => '', 'numberColumn' => 99, 'optional' => true);
        $rowArray['RELATIVE FEED VALUE'] = array('text' => 'Relative Feed Value', 'unit' => '', 'numberColumn' => 99, 'optional' => true);
        $rowArray['VOMITOXINE'] = array('text' => 'Vomitoxine', 'unit' => 'ppb', 'numberColumn' => 99, 'optional' => true);
        $rowArray['ZEARALENONE'] = array('text' => 'Zearalenone', 'unit' => 'ppb', 'numberColumn' => 99, 'optional' => true);
        $rowArray['PERCENTAGE OF GRAIN'] = array('text' => 'Precentage Of Grain', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);  //test 2013 on use this.
        $rowArray['NDF-ADF SPREAD'] = array('text' => 'NDF-ADF Spread', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        //
        //extra ones not used by shur-gain. use dummy value 123456789999 so that they are not found if needed.
        $rowArray['pH'] = array('text' => 'pH', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['SolubleProtein'] = array('text' => 'SolubleProtein', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['ADFCP'] = array('text' => 'ADFCP', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['NDFCP'] = array('text' => 'NDFCP', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['Lignin'] = array('text' => 'Lignin', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['DE'] = array('text' => 'DE', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        // new stuff 2018

        $rowArray['A FRACTION'] = array('text' => 'A Fraction', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['B FRACTION'] = array('text' => 'B Fraction', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['D FRACTION'] = array('text' => 'D Fraction', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['G FRACTION'] = array('text' => 'G Fraction', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['K2'] = array('text' => 'K2', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['K9'] = array('text' => 'K9', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['ACID DETERGENT FIBER LIGNIN NIR'] = array('text' => 'Acid Detergent Fiber Lignine NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['LACTIC ACID'] = array('text' => 'Lactic Acid', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['ACETIC ACID'] = array('text' => 'Acetic Acid', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);


        return $rowArray;
    }

    function getAnalysisNumbers($needleText, $haystackText, $unit, $numberOneOrTwo, $optional) {
//define 
        $matches['digit1'] = '';
        $matches['digit2'] = null;

//(\b[0-9]+\.([0-9]+\b)) matchs a floating point number.
//escape the string for slashes and brackets.
        $needleText = preg_quote($needleText, '/');


        //does most of them
        if ($numberOneOrTwo == 1) {
            preg_match("/$needleText\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*(?<digit2>(\b[0-9]+\.([0-9]+\b)))\s*$unit\s*/", $haystackText, $matches);

            if ((isset($matches['digit1']) == true) && (is_numeric($matches['digit1']) == true)) {
                $num = $matches['digit1'];
            } elseif ($optional == true) {
                return null;
            } else {
                throw new Exception("\n\r\n\r" . "ERROR:No number read for row $needleText. Should this be optinonal?" . "\n\r");
            }

            return $num;
        }

        //custom one for dry matter, no second digit. 
        if ($numberOneOrTwo == 3) {
            preg_match("/$needleText\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*$unit\s*/", $haystackText, $matches);

            if ((isset($matches['digit1']) == true) && (is_numeric($matches['digit1']) == true)) {
                $num = $matches['digit1'];
            } elseif ($optional == true) {
                return null;
            } else {
                throw new Exception("\n\r\n\r" . "ERROR:No number read for row $needleText. Should this be optinonal?" . "\n\r");
            }

            return $num;
        }

//for matching ppb with integer numbers
        if ($numberOneOrTwo == 99) {
            preg_match("/$needleText\s*$unit\s*(?<digit1>([0-9.]+))\s*/", $haystackText, $matches);


            if ((isset($matches['digit1']) == true) && (is_numeric($matches['digit1']) == true)) {
                $num = $matches['digit1'];
            } elseif ($optional == true) {
                return null;
            } else {
                throw new Exception("\n\r\n\r" . "ERROR:No number read for row $needleText. Should this be optinonal?" . "\n\r");
            }

            return $num;
        }
    }

      // returns datetime of last DB update, assumes shurgainf orage test was last one, not say cumberland
    public function lastRun() {
        $sql = "SELECT max(update_time) as max_update_time FROM nutrition.bag_analysis";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return strtotime($row['max_update_time']);
    }
    
}
//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new forageAnalysisReportsParseShurgain();
    $cls->main();
} else {
    //nothing
}
?>