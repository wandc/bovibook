<?php

/**
  File to read irving oil chignecto soil and crop pricing information from PDFs
 * 
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
    $_SERVER['DOCUMENT_ROOT'] = dirname(__DIR__) . '/'; //when called from cli, command line define constant.
} else {
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
}



class irvingOilPricePDFParse {

    
    public $filepath = "../local/dataFile/irvingOilPricing/"; //hardcoded....bad.
     
    public function sse($request = null) {
        $this->main();
    }

    /*
     * process through files. 
     */

    
    function mainNEW() {
     
        $filesArr = scandir($this->filepath);
        //var_dump($filesArr);
       
   //
   //$filesArr = array_diff(scandir(__DIR__ .$this->$filepath), array('.', '..'));   
        
foreach ($filesArr as $key => $value) {
    $ret=array();
    $output=$this->readPDFToText($value);
    if (!empty($output)) {
    $matchedArr=$this->parseForMatches($output);
    $this->insertDB($matchedArr);
    }
  
}
    }

function insertDB($matchedArr) {
    
    
    if (empty($matchedArr['date'])) {
        return null;
    }
  
    
    $date=date('Y-m-d',strtotime($matchedArr['date']));
    
  
    
     $query = "INSERT INTO batch.irving_oil_price (date,uls_diesel,furnace_oil,reg_gasoline) VALUES ('$date',{$matchedArr['diesel']},{$matchedArr['furnace']},{$matchedArr['gas']}) ON CONFLICT DO NOTHING";
            
                $res = $GLOBALS['pdo']->exec($query);
    
    
}


function parseForMatches($pdfPageStr) {
    $ret=array();
    
    //var_dump($pdfPageStr);
         $matches = null;
    preg_match('/Effective\s(?<date>(Jan(uary)?|Feb(ruary)?|Mar(ch)?|Apr(il)?|May|Jun(e)?|Jul(y)?|Aug(ust)?|Sep(tember)?|Oct(ober)?|Nov(ember)?|Dec(ember)?)\s+\d{1,2},\s+\d{4})/', implode("\n\r", $pdfPageStr), $matches);
    $ret['date']=empty($matches['date'])?null:$matches['date'];
    
    //now look for strings
     $matches = null;
    preg_match('/ULS Diesel\s*\$\s(?<diesel>(\d.*))/', implode("\n\r", $pdfPageStr), $matches);
    $ret['diesel']=empty($matches['diesel'])?null:$matches['diesel'];
    
     $matches = null;
    preg_match('/Furnace Oil\s*\$\s(?<furnace>(\d.*))/', implode("\n\r", $pdfPageStr), $matches);
    $ret['furnace']=empty($matches['furnace'])?null:$matches['furnace'];
    
     $matches = null;
    preg_match('/Regular Gasoline\s*\$\s(?<gas>(\d.*))/', implode("\n\r", $pdfPageStr), $matches);
    $ret['gas']=empty($matches['gas'])?null:$matches['gas'];
    
      // var_dump($ret);

  return $ret;
    
}




        
    
    
    function readPDFToText($filename) {
        $rowArray2 = array();

        $fileNamePath=$this->filepath .'/'.$filename;

//now run unix poppler utility to convert pdf to text. http://poppler.freedesktop.org
        $retVal = null;
        $output = null;
        $retVal = exec("pdftotext '$fileNamePath' -layout -", $output);

        //DEBUG
        // print_r(implode("\n\r", $output));
        //look for sample no to see if this is a valid feed analysis page. if it is, exit and tell function to stop
        $matches = null;
        //look for 
        preg_match("/ULS Diesel\s*(?<sampleno>(\d*))\s/", implode("\n\r", $output), $matches);

     
        //return null or page text
        if (!empty((isset($matches['sampleno'])))) {
            return $output;
        } else {
            return null;
        }
    }
    
    
    function main() {

        $sql = "SELECT date FROM batch.irving_oil_price ORDER BY date DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
        $stmt->execute();

        /* Bind by column number */
        $stmt->bindColumn(1, $date, PDO::PARAM_STR);


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
        preg_match("/SAMPLE NUMBER:\s*(?<sampleno>(\d*))\s/", implode("\n\r", $output), $matches);

     
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
        preg_match("/SAMPLE NUMBER:\s*(?<sampleno>(\d*))\s/", implode("\n\r", $page), $matches);
       // var_dump($matches);
        $rowArray2['Sample no'] = '' . $matches['sampleno'];

//get sample date.
        preg_match("/DATE PRINTED:\s*(?<date>(\d{2}\/\d{2}\/\d{2}))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            $rowArray2['Sample Date'] = $matches['date'];
        }

       

//get received date. (same date)
      preg_match("/DATE PRINTED:\s*(?<date>(\d{2}\/\d{2}\/\d{2}))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Received Date'] = $matches['date'];
        }

//get reported date. (same date)
       preg_match("/DATE PRINTED:\s*(?<date>(\d{2}\/\d{2}\/\d{2}))/", implode("\n\r", $page), $matches);
        if (strtotime($matches['date']) != false) {
            strtotime($matches['date']);
            $rowArray2['Analysis date'] = $matches['date'];
        } else {
            throw new Exception("Reported date not found for page $startPage." . "\n\r");
        }


//get sample comment
        preg_match("/DESCRIPTION:\s*(?<comment>([\x20-\x7E]*))\n\r(?<comment2>([\x20-\x7E]*))/", implode("\n\r", $page), $matches);
        $rowArray2['Identification']['answer'] = $matches['comment']. ' '. ltrim($matches['comment2']);

        
        //using template parse file for numbers for raw report
        $rowArray = $this->agriAnalyseSilageTemplateRawReport();
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
          
            if (empty($rowArray2['Sample no'])) {
                throw new Exception("We do not even have data to insert for file: $filename. Probably parsing failed.");
            }
            
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
                $infoArr['RFQ'] = (float) $rowArrayBasic['RFQ'];
                $infoArr['milk_per_tonne'] = (float) $rowArrayBasic['milk per tonne from forage (kg/t)'];
                $infoArr['TDN'] = (float) $rowArrayNRCBasic['TDN (%DM)'];
                $infoArr['CP'] = (float) $rowArrayBasic['CRUDE PROTEIN'];
                $infoArr['Ca'] = (float) $rowArrayBasic['CALCIUM'];
                $infoArr['P'] = (float) $rowArrayBasic['PHOSPHORUS'];
                $infoArr['K'] = (float) $rowArrayBasic['POTASSIUM'];
                $infoArr['ADF'] = (float) $rowArrayBasic['ACID DETERGENT FIBER'];
                $infoArr['NDF'] = (float) $rowArrayBasic['NEUTRAL DETERGENT FIBER'];
                $infoArr['lignin'] = (float) $rowArrayBasic['LIGNIN'];
                $infoArr['ash'] = (float) $rowArrayBasic['ASH'];
                $infoArr['starch'] = null; //$infoArr['starch']=$rowArrayBasic['starch'];//add later
                //encode json to store in db. 
                $rawReportJson = json_encode($rowArrayBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $NRCJson = json_encode($rowArrayNRCBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $infoJson = json_encode($infoArr, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);

                //now insert into db.
                $sql = "INSERT INTO nutrition.bag_analysis (bag_id,footage,sample_number,sample_date,userid, report_file, lab_name, raw_report	,nrc2001_template,info_template) VALUES (null,null,'{$rowArray2['Sample no']}','{$rowArray2['Analysis date']}','system','{$rowArray2['pdf_file_report']['answer']}','agri-analyse','$rawReportJson','$NRCJson','$infoJson')";

               
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


        $outArray['TDN (%DM)'] = (float) round((($rowArray['NEL ADF'] + .12) / 0.0245), 2); //verified true uc davis. checked again, this is metric.
        $outArray['DE (Mcal/kg)'] = (float) round(($outArray['TDN (%DM)'] * 0.044), 2); //verified true uc davis. checked again, this is metric.   //FROM: http://animalsciencey.ucdavis.edu/java/LivestockSystemMgt/Conversion/energy.htm
        $outArray['DM (%AF)'] = (float) $rowArray['DRY MATTER'];  //verified true with shurgain
        $outArray['NDF (%DM)'] = (float) $rowArray['NEUTRAL DETERGENT FIBER'];
        $outArray['ADF (%DM)'] = (float) $rowArray['ACID DETERGENT FIBER'];
        $outArray['Lignin (%DM)'] = (float) $rowArray['LIGNIN'];
        $outArray['CP (%DM)'] = (float) $rowArray['CRUDE PROTEIN'];  
        
        $outArray['NDFIP (%DM)'] = null; //NDF insolable protein, around 2.
        $outArray['ADFIP (%DM)'] = null; //ADF insoluable protein, around 1.8
        
        /* not sure what this is */
        $outArray['Prt-A (%CP)'] = null; //(float) $rowArray['A FRACTION'];  //Adican: needs to be done through wet chemistry. 
        $outArray['Prt-B (%CP)'] = null; //(float) round(( 100 - ($rowArray['A FRACTION'] + $rowArray['B FRACTION'])), 2);
        $outArray['Prt-C (%CP)'] = null; //(float) $rowArray['B FRACTION'];
        //
        $outArray['Kd (1/hr)'] = null; // (float) $rowArray['K2']; //addican knows what it is. 
        $outArray['RUP Digest (%)'] = (float) $rowArray['CRUDE PROTEIN'] - (float) $rowArray['DEGRADABLE PROTEIN %CP']; //seems to match numbers from nrc in DB. 
        $outArray['Fat (%DM)'] = (float) $rowArray['FAT']; //verified true with shurgain
        $outArray['Ash (%DM)'] = (float) $rowArray['ASH']; //verified true with shurgain    
        //
        $outArray['Sodium'] = (float) $rowArray['SODIUM'];
        $outArray['Calcium'] = (float) $rowArray['CALCIUM'];
        $outArray['Phosphorus'] = (float) $rowArray['PHOSPHORUS'];
        $outArray['Magnesium'] = (float) $rowArray['MAGNESIUM'];
        $outArray['Potassium'] = (float) $rowArray['POTASSIUM'];
        $outArray['Sulphur'] = (float) $rowArray['SULFUR'];

        return $outArray;
    }

    /*
     * this function is the template of how to do the preg match row by row of the file. chnage the text portion when needed.
     */

    function agriAnalyseSilageTemplateRawReport() {
        
        $rowArray['DRY MATTER'] = array('text' => 'DRY MATTER %', 'unit' => '%', 'numberColumn' => 1, 'optional' => true); 
       
         //protein
         $rowArray['CRUDE PROTEIN'] = array('text' => 'CRUDE PROTEIN %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true); 
        $rowArray['ADFN'] = array('text' => 'ADF-N %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true); //ADF nitrogen portion??? ie crude protein portion?
        $rowArray['ADFN %CP'] = array('text' => 'ADF-N (%CP) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true); //ADF nitrogen portion??? ie crude protein portion?
        $rowArray['AVAILABLE PROTEIN'] = array('text' => 'AVAILABLE PROTEIN %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
        $rowArray['SOLUBLE PROTEIN %CP'] = array('text' => 'SOLUBLE PROTEIN (%CP)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
        $rowArray['DEGRADABLE PROTEIN %CP'] = array('text' => 'DEGRADABLE PROTEIN (% CP)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
          $rowArray['NDICP'] = array('text' => 'NDICP %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['NDICP %CP'] = array('text' => 'NDICP (%CP) %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
          //fiber
          $rowArray['ACID DETERGENT FIBER'] = array('text' => 'ACID DET. FIBER %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true); //more accurate ADF. I think. 
          $rowArray['NEUTRAL DETERGENT FIBER'] = array('text' => 'NEUTRAL DET. FIBER %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true); //more accurate ADF. I think.      
          $rowArray['NDFD 30h %NDF'] = array('text' => 'NDFD30 (%NDF) %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
          $rowArray['NDFD 48h %NDF'] = array('text' => 'NDFD48 (%NDF) %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['LIGNIN'] = array('text' => 'LIGNIN %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['LIGNIN %NDF'] = array('text' => 'LIGNIN (%NDF) %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           //
           $rowArray['NFC'] = array('text' => 'NFC %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['NSC'] = array('text' => 'NSC %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['DRY MATTER INTAKE (% BODY WT.)'] = array('text' => 'DRY MATTER INTAKE (% BODY WT.)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['FAT'] = array('text' => 'FAT %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['STARCH'] = array('text' => 'STARCH %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['DIGEST. STARCH 7HR (% STARCH) %'] = array('text' => 'DIGEST. STARCH 7HR (% STARCH) %', 'unit' => '', 'numberColumn' => 2, 'optional' => true); //70 to 98, avg 84.
           $rowArray['SUGARS'] = array('text' => 'SUGARS %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['ASH'] = array('text' => 'ASH %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           //Minerals  
        $rowArray['CALCIUM'] = array('text' => 'CALCIUM (Ca) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        $rowArray['PHOSPHORUS'] = array('text' => 'PHOSPHORUS (P) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        $rowArray['POTASSIUM'] = array('text' => 'POTASSIUM (K) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        $rowArray['MAGNESIUM'] = array('text' => 'MAGNESIUM (Mg) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        $rowArray['SULFUR'] = array('text' => 'SULFUR (S) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        $rowArray['CHLORIDE'] = array('text' => 'CHLORIDE (CL) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        $rowArray['SODIUM'] = array('text' => 'SODIUM (NA) %', 'unit' => '%', 'numberColumn' => 2, 'optional' => true);
        //Energy
         $rowArray['TDN ADF'] = array('text' => 'TDN % [ADF]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['NEL ADF'] = array('text' => 'NEL, MCAL/KG [ADF]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['NEM ADF'] = array('text' => 'NEM, MCAL/KG [ADF]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['NEG ADF'] = array('text' => 'NEG, MCAL/KG [ADF]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['TDN SSCSE'] = array('text' => 'TDN % [SSCSE]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['NEL SSCSE'] = array('text' => 'NEL (Mcal/Kg) [SSCSE]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['NEM SSCSE'] = array('text' => 'NEM, MCAL/KG [SSCSE]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);
         $rowArray['NEG SSCSE'] = array('text' => 'NEG, MCAL/KG [SSCSE]', 'unit' => 'Mcal\/kg', 'numberColumn' => 2, 'optional' => true);   
        //VFA
         $rowArray['pH %'] = array('text' => 'pH %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
         $rowArray['LACTIC'] = array('text' => 'LACTIC %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
         $rowArray['ACETIC'] = array('text' => 'ACETIC %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
         $rowArray['AMMONIA'] = array('text' => 'AMMONIA %', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
        
        //EXTRA  
           $rowArray['milk per day from forage (kg)'] = array('text' => 'Milk Per Day From Forage (Kg)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['milk per tonne from forage (kg/t)'] = array('text' => 'Milk Per Metric Ton From Forage (Kg/MT)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['DCAD mEq/Kg'] = array('text' => 'DCAD, mEq/Kg', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
          
         //grass sheet has extra
           $rowArray['DRY MATTER INTAKE (% BODY WT.) 2'] = array('text' => 'DM INTAKE (% BODY WT.)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['RFV'] = array('text' => 'RELATIVE FEED VALUE (RFV)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           $rowArray['RFQ'] = array('text' => 'RELATIVE FORAGE QUALITY (RFQ)', 'unit' => '', 'numberColumn' => 2, 'optional' => true);
           
           
        return $rowArray;
    }

    function getAnalysisNumbers($needleText, $haystackText, $unit, $numberOneOrTwo, $optional) {
        //unit is not used. 
//define 
        $matches['digit1'] = '';
        $matches['digit2'] = null;

//(\b[0-9]+\.([0-9]+\b)) matchs a floating point number.
//escape the string for slashes and brackets.
        $needleText = preg_quote($needleText, '/');


        //does most of them
        if ($numberOneOrTwo == 1) {
            preg_match("/$needleText\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*(?<digit2>(\b[0-9]+\.([0-9]+\b)))\s*/", $haystackText, $matches);

            //always take 2nd digit as it is dry matter based one.
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
        if ($numberOneOrTwo == 2) {
            preg_match("/$needleText\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*(?<digit2>(\b[0-9]+\.([0-9]+\b)))\s*/", $haystackText, $matches);

            //always take 2nd digit as it is dry matter based one.
            if ((isset($matches['digit2']) == true) && (is_numeric($matches['digit2']) == true)) {
                $num = $matches['digit2'];
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
    $cls = new irvingOilPricePDFParse();
    $cls->mainNEW();
} else {
    //nothing
}
?>