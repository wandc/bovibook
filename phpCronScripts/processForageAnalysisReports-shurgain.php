<?php

/**
  File to read shurgain feed analysis reports and store in db
  Just copy the pdfs from shurgain into the forageAnalysis directory and go. This will ignore valacta and other reports.
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

/*
 * reads files from shurgain directory of forage sanple results. modified in 2018, nut used since 2015 or so before. changed so that
 * it looks for the word DAVID in the filename, skips others, this is so it only picks up newer files, can change back if necessary in future.
 * 
 * BIG REWRITE 2018
 * CHanged to json so if file format of report changes it is not a big deal. first job is to get a report machine readable and second one is use with NRC, so this 
 * script does both those jobs. This change breaks many things. 
 */

//lots of good forumelas here: http://www.agtest.com/articles/FEED%20AND%20FORAGES%20CALCULATIONS_new.pdf
class ForageAnalysisParseShurGain {

    public $filepath;
    
    public function __construct() {
         $this->filepath = "../local/dataFile/forageAnalysis/shurGain/"; //hardcoded....bad.
        //run main function
        $fileListArr=$this->readListOfFileNames();
         print("\n\r Doing the following files:\n");
         print_r($fileListArr);
        $this->doForageProcessing($fileListArr);
    }

    
    function readListOfFileNames() {
       
        
//load all the  files in the directory.
       
        if ($handle = opendir($this->filepath)) {
            /* This is the correct way to loop over the directory. */
            while (false !== ($file = readdir($handle))) {
                if ((strpos($file, '.pdf') !== false) AND (strpos($file, 'DAVID') !== false)) {
                    $arrayOfFiles[] = $file;
                }
            }
            closedir($handle);
        }
        return $arrayOfFiles;
    }
    
    function doForageProcessing($fileListArr) {

        //read file
        foreach ($fileListArr as $arrayOfFiles_num => $fileName) {

       //NOTE: There is one file from 2012, that has many tests in one PDF file. 
       //This code does not support that any more, because...well its a waste of time to code for shur-gain being stupid. 
       //Just break it up if need be in the future.      

            $pdfFileName =  $this->filepath . '"' . $fileName . '"';
            print("Doing File:$pdfFileName" . " ....<br>\n\r");
         //record report raw file data into db.
        $pdfFileName2 = str_replace('..', '', $pdfFileName);
        $pdfFileName2 = str_replace('"', '', $pdfFileName2);
        $filename = '/var/www/int' . $pdfFileName2; //hardcoded bad
            
//THERE can be multiple forage sample results in one file, one per page
//go through page numbers until we get a stop signal.
         

                try {
                    
                    $page=self::readOnePageToText($filename);
                    if (!empty($page)) {
                    
                        //parse page data and insert into DB.
                        self::parseSamplePage($page,$filename); 
                    }
                    else {
                        throw new Exception("NOTICE: Skipping file: $pdfFileName, no certificate number found, thus not a valid file for parsing.\n\r");
                    }
                  
                } catch (Exception $e) {
                    echo "SKIPPING PAGE (OR END OF FILE), Caught EXCEPTION for file $pdfFileName", $e->getMessage(), "\n";
                }

        }//filename loop.
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
        preg_match("/EA-740(?<sampleno>([\x20-\x7E]*))/", implode("\n\r", $output), $matches);
       
        //return null or page text
        if (!empty((isset($matches['sampleno'])))) {
            return $output;
        } else {
            return null;
        }
    }

    
    
    
    function parseSamplePage($page,$filename) {
       

     
       
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
        preg_match("/EA-740(?<sampleno>([\x20-\x7E]*))/", implode("\n\r", $page), $matches);
        $rowArray2['Sample no']='EA-740'.$matches['sampleno'];
        
        
//get sample date.
        preg_match("/Date sampled :\s*(?<date>([a-zA-Z]{3}\s\d\d,\s\d\d\d\d))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Sample Date'] = $matches['date'];
        }

//get received date.
        preg_match("/Date received :\s*(?<date>([a-zA-Z]{3}\s\d\d,\s\d\d\d\d))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Received Date'] = $matches['date'];
        }

//get reported date.
        preg_match("/Date reported :\s*(?<date>([a-zA-Z]{3}\s\d\d,\s\d\d\d\d))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Analysis date'] = $matches['date'];
        } else {
            throw new Exception("Reported date not found for page $startPage." . "\n\r");
        }


//get sample comment
        preg_match("/COMMENTS\s*(?<comment>([\x20-\x7E]*))\n\r(?<comment2>([\x20-\x7E]*))/", implode("\n\r", $page), $matches);
        $rowArray2['Identification']['answer'] = $matches['comment'] . ' ' . $matches['comment2'];

        
 
        //using template parse file for numbers for raw report
        $rowArray = $this->shurgainSilageTemplateRawReport();
        foreach ($rowArray as $key => $row) {
            $rowArrayBasic[$key]=self::getAnalysisNumbers($row['text'], implode("\n\r", $page), $row['unit'], $row['numberColumn'], $row['optional']); //key is text
        }
         
        //using raw parsed data generate values for NRC 2001 model.
       $rowArrayNRCBasic=$this->generateNRC2001ValuesFromRawValues($rowArrayBasic);

       
       
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
                $rowArrayBasic['comment']=   $rowArray2['Identification']['answer'];
                
                //create info template.
                $infoArr['DM']=(float)$rowArrayBasic['DRY MATTER'];
                $infoArr['RFQ']=(float)$rowArrayBasic['FORAGE QUALITY INDEX'];
                $infoArr['TDN']=(float)$rowArrayNRCBasic['TDN (%DM)'];
                $infoArr['CP']=(float)$rowArrayBasic['PROTEIN NIR'];
                $infoArr['Ca']=(float)$rowArrayBasic['*CALCIUM'];
                $infoArr['P']=(float)$rowArrayBasic['*PHOSPHORUS'];
                $infoArr['K']=(float)$rowArrayBasic['*POTASSIUM'];
                $infoArr['ADF']=(float)$rowArrayBasic['ACID DETERGENT FIBER NIR'];
                $infoArr['NDF']=(float)$rowArrayBasic['NDF(OM) NIR'];
                $infoArr['lignin']=(float)$rowArrayBasic['ACID DETERGENT FIBER LIGNIN NIR'];
                $infoArr['ash']=(float)$rowArrayBasic['ASH'];
                $infoArr['starch']=null;//$infoArr['starch']=$rowArrayBasic['starch'];//add later
    
                
                 //encode json to store in db. 
                $rawReportJson = json_encode($rowArrayBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $NRCJson= json_encode($rowArrayNRCBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $infoJson= json_encode($infoArr, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                
                //now insert into db.
                $sql = "INSERT INTO nutrition.bag_analysis (bag_id,footage,sample_number,sample_date,userid, report_file, lab_name, raw_report	,nrc2001_template,info_template) VALUES (null,null,'{$rowArray2['Sample no']}','{$rowArray2['Analysis date']}','system','{$rowArray2['pdf_file_report']['answer']}','shur-gain','$rawReportJson','$NRCJson','$infoJson')";
                     
                $res = $GLOBALS['pdo']->exec($sql);
            } else {
                print("\n\r\n\r" . "SKIPPING {$rowArray2['Sample no']}" ." \n\r\n\r");
            }


// determine if the commit or rollback

            $GLOBALS['pdo']->commit();
            //$GLOBALS['pdo']->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
// Close the transaction
    }

//end function

    
    //turns parsed raw data into something nrc can use to overlay other forage types.
    function generateNRC2001ValuesFromRawValues($rowArray) {
        
        
        $outArray['TDN (%DM)'] = (float) round( (($rowArray['NEL'] + .12) / 0.0245),2); //verified true uc davis. 
        $outArray['DE (Mcal/kg)'] = (float) round(($outArray['TDN (%DM)'] * 0.044),2); //verified true uc davis.   //FROM: http://animalsciencey.ucdavis.edu/java/LivestockSystemMgt/Conversion/energy.htm
        $outArray['DM (%AF)'] = (float) $rowArray['DRY MATTER'];  //verified true with shurgain
        $outArray['NDF (%DM)'] =  (float)$rowArray['NDF(OM) NIR'];
        $outArray['ADF (%DM)'] = (float) $rowArray['ACID DETERGENT FIBER NIR'];
        $outArray['Lignin (%DM)'] =  (float)$rowArray['ACID DETERGENT FIBER LIGNIN NIR'];
        $outArray['CP (%DM)'] = (float) $rowArray['PROTEIN NIR'];  //verified true with shurgain
        $outArray['NDFIP (%DM)'] = null; //not measured by shur-gain, book values for forages have small variance. don't worry use book value.
        $outArray['ADFIP (%DM)'] = null; //not measured by shur-gain, book values for forages have small variance. don't worry use book value.
        $outArray['Prt-A (%CP)'] =  (float)$rowArray['A FRACTION'];
        $outArray['Prt-B (%CP)'] =  (float)round(( 100 - ($rowArray['A FRACTION'] + $rowArray['B FRACTION']) ),2);
        $outArray['Prt-C (%CP)'] = (float) $rowArray['B FRACTION'];
        //
        $outArray['Kd (1/hr)'] = (float) $rowArray['K2']; //somewhat verified 
        $outArray['RUP Digest (%)'] = (float) $rowArray['POTENTIAL DIGESTIBILITY']; //somewhat verified, Not measured source:  John Metcalf
        $outArray['Fat (%DM)'] =  (float)$rowArray['CRUDE FAT NIR']; //verified true with shurgain
        $outArray['Ash (%DM)'] = (float) $rowArray['ASH']; //verified true with shurgain    
        //
        $outArray['Sodium'] =  (float)$rowArray['*SODIUM'];
        $outArray['Calcium'] =  (float)$rowArray['*CALCIUM'];
        $outArray['Phosphorus'] = (float) $rowArray['*PHOSPHORUS'];
        $outArray['Magnesium'] = (float) $rowArray['*MAGNESIUM'];
        $outArray['Potassium'] = (float) $rowArray['*POTASSIUM'];
        $outArray['Sulphur'] =  (float)$rowArray['SULFUR'];

        return $outArray;
    }
    

    
    
    function shurgainSilageTemplateRawReport() {
        $rowArray['ASH'] = array('text' => 'ASH', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
        $rowArray['DRY MATTER'] = array('text' => 'DRY MATTER', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
        $rowArray['PROTEIN NIR'] = array('text' => 'PROTEIN NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
        $rowArray['ADFN']  = array('text' => 'ADFN', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //ADF nitrogen portion??? ie crude protein portion?
      
        $rowArray['NEL'] = array('text' => 'NET ENERGY LACTATION', 'unit' => 'MCLKG', 'numberColumn' => 1, 'optional' => true);

        $rowArray['CRUDE FAT NIR'] = array('text' => 'CRUDE FAT NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //verified true with shurgain
         $rowArray['NDF(OM) NIR'] = array('text' => 'NDF(OM) NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //more accurate NDF I think.
        $rowArray['ACID DETERGENT FIBER NIR'] = array('text' => 'ACID DETERGENT FIBER NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); //more accurate ADF. I think.
        $rowArray['NON STRUCTURAL CARBOHYDRATE'] = array('text' => 'NON STRUCTURAL CARBOHYDRATE', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); 
        $rowArray['IF'] = array('text' => 'IF', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['POTENTIAL DIGESTIBILITY'] = array('text' => 'POTENTIAL DIGESTIBILITY', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);     
        $rowArray['*SODIUM'] = array('text' => '*SODIUM', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*CALCIUM'] = array('text' => '*CALCIUM', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*PHOSPHORUS'] = array('text' => '*PHOSPHORUS', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*MAGNESIUM'] = array('text' => '*MAGNESIUM', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['*POTASSIUM'] = array('text' => '*POTASSIUM', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['SULFUR'] = array('text' => 'SULFUR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['ADFN'] = array('text' => 'ADFN', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['GAIN ENERGY'] = array('text' => 'GAIN ENERGY', 'unit' => 'MJLKG', 'numberColumn' => 1, 'optional' => true);
        $rowArray['MAINT. ENERGY'] = array('text' => 'MAINT. ENERGY', 'unit' => 'MJLKG', 'numberColumn' => 1, 'optional' => true);
        $rowArray['FORAGE QUALITY INDEX'] = array('text' => 'FORAGE QUALITY INDEX', 'unit' => '', 'numberColumn' => 99, 'optional' => true);
        $rowArray['RELATIVE FEED VALUE'] = array('text' => 'RELATIVE FEED VALUE', 'unit' => '', 'numberColumn' => 99, 'optional' => true);
        $rowArray['VOMITOXINE'] = array('text' => 'VOMITOXINE', 'unit' => 'PPB', 'numberColumn' => 99, 'optional' => true);
        $rowArray['ZEARALENONE'] = array('text' => 'ZEARALENONE', 'unit' => 'PPB', 'numberColumn' => 99, 'optional' => true);   
        $rowArray['PERCENTAGE OF GRAIN'] = array('text' => 'PERCENTAGE OF GRAIN', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);  //test 2013 on use this.
        $rowArray['NDF-ADF SPREAD'] = array('text' => 'NDF-ADF SPREAD', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); 
        //
        //extra ones not used by shur-gain. use dummy value 123456789999 so that they are not found if needed.
        $rowArray['pH'] = array('text' => 'pH', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['SolubleProtein'] = array('text' => 'SolubleProtein', 'unit' => '','numberColumn' => 1, 'optional' => true);
        $rowArray['ADFCP'] = array('text' => 'ADFCP', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['NDFCP'] = array('text' => 'NDFCP', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['Lignin'] = array('text' => 'Lignin', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['DE'] = array('text' => 'DE', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        // new stuff 2018

        $rowArray['A FRACTION'] = array('text' => 'A FRACTION', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['B FRACTION'] = array('text' => 'B FRACTION', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['D FRACTION'] = array('text' => 'D FRACTION', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['G FRACTION'] = array('text' => 'G FRACTION', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['K2'] = array('text' => 'K2', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['K9'] = array('text' => 'K9', 'unit' => '', 'numberColumn' => 1, 'optional' => true);
        $rowArray['ACID DETERGENT FIBER LIGNIN NIR'] = array('text' => 'ACID DETERGENT FIBER LIGNIN NIR', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['LACTIC ACID'] = array('text' => 'LACTIC ACID', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        $rowArray['ACETIC ACID'] = array('text' => 'ACETIC ACID', 'unit' => '%', 'numberColumn' => 1, 'optional' => true);
        

        return $rowArray;
    }

    function getAnalysisNumbers($needleText, $haystackText, $unit, $numberOneOrTwo, $optional) {
//define 
        $matches['digit1'] = '';
        $matches['digit2'] = null;

//(\b[0-9]+\.([0-9]+\b)) matchs a floating point number.
//escape the string for slashes and brackets.
        $needleText = preg_quote($needleText, '/');

        if ($numberOneOrTwo == 1) {
            preg_match("/$needleText\s*$unit\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*/", $haystackText, $matches);

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
        } elseif ($numberOneOrTwo == 2) {
            preg_match("/$needleText\s*$unit\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*(?<digit2>(\b[0-9]+\.([0-9]+\b)))/", $haystackText, $matches);

            if ((isset($matches['digit2']) == true) && (is_numeric($matches['digit2']) == true)) {
                $num = $matches['digit2'];
            } elseif ($optional == true) {
                return null;
            } else {
                throw new Exception("No number read for row $needleText" . "\n\r");
            }

            return $num;
        } else {
            throw new Exception("Error: Number one or two not required for forage analysis row. None Found." . "\n\r");
        }
    }

}

//end class

new ForageAnalysisParseShurGain(); //run class
?>