<?php

//script is apparently sloppy with memory/
ini_set("memory_limit", "1024M");
set_time_limit(30); //add 30 seconds to time out counter, to give time to script to complete. This allo
       
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
    $_SERVER['DOCUMENT_ROOT'] = dirname(__DIR__) . '/'; //when called from cli, command line define constant.
} else {
   include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php'); 
   
}
/*
 * Processes valcata files reading from DB.
 */

class ValactaDataFile {

    public $filepath = "../local/dataFile/valacta/"; //hardcoded....bad.

    
    public function sse($request = null) {
        $this->main();
    }
    
  

    //main function that processes old and new files.
    function main() {
         SSE_Message::send_message(1, 'Starting.', 1); //sse
       
       
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            //empty the adlic_table first
            $res = $GLOBALS['pdo']->exec("TRUNCATE batch.valacta_raw");

            //main part
            //do old first
                 SSE_Message::send_message(2, 'Doing old files.', 1); //sse
            $this->doAdlicProcessingOld();
            //then new
                  SSE_Message::send_message(3, 'Doing newer XML files.', 1); //sse
            $this->doValactaXmlProcessing();




            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            throw new Exception("Failed. Valcata Raw table not updated.");
        }

        print("<h4>Refreshing materialized view...</h4>");
        //now update materialized view.
        $sql4 = "REFRESH MATERIALIZED VIEW batch.valacta_data";
        $res4 = $GLOBALS['pdo']->exec($sql4);

         SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
        print("<h3>All done!</h3>");
    }

    //in 2017 we switched to XML files for valvacta old flat format, so use that from now on.
    function doValactaXmlProcessing() {
        print("Doing the following XML files:\n");

        $sql = "SELECT id,data::bytea,file_name FROM documents.valacta_xml ORDER BY file_name DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
       

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $data, PDO::PARAM_LOB);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);
         $stmt->execute();

        $arrayCount = $stmt->rowCount();    
        $i=0;
        while ($row = $stmt->fetch(PDO::FETCH_BOUND)) {
            
             SSE_Message::send_message(6 + $i, 'Doing around ' . $fileName . $i . ' of ' . $arrayCount, 6 + round(($i / $arrayCount) * 100, 1)); //sse

            print("XML File $fileName <br/>\n");

            //roundabout way, load from xml into json, then convert to array to work with.
            $xml = simplexml_load_string(pg_unescape_bytea(stream_get_contents($data)));
            $json = json_encode($xml);

            $array = json_decode($json, TRUE);

            //loop over all females in the specified file.
            for ($n = 0; $n < count($array['Females']); $n++) {


                if (isset($array['Females'][$n]['StartLactations'])) {
                    
                }

                //only do females that were actually tested and have a laction started, everything else in file gets ignored.
                if ((!empty($array['Females'][$n]['AnimalMilkCompositions']['@attributes']['SampleDate'])) AND ( isset($array['Females'][$n]['StartLactations'])) AND ( !empty($array['Females'][$n]['Productions']['@attributes']['Hr24Milk']))) {


                    //   print($array['Females'][$n]['AnimalMilkCompositions']['@attributes']['SampleDate']."\n\r"); 
                    //    print((int)$array['Females'][$n]['StartLactations'][0]['@attributes']['LactationNo']."\n\r");
                    //  print($array['Females'][$n]['@attributes']['VisibleIdNo']."\n\r"."\n\r");
                    /*
                      if ($array['Females'][$n]['StartLactations'][0]['@attributes']['LactationNo']==0)
                      {
                      var_dump($array['Females'][$n]);
                      }
                     */
                    $lineArray['HERD'] = $array['Herds']['@attributes']['HerdNo'];
                    $lineArray['CONTROL'] = $array['Females'][$n]['@attributes']['ComputerNo'];
                    $lineArray['REG'] = trim($array['Females'][$n]['@attributes']['BreedCd'] . $array['Females'][$n]['@attributes']['CountryCd'] . 'F' . $array['Females'][$n]['@attributes']['RegNo']); //F because we know it is female.
                    $lineArray['CHAIN'] = $array['Females'][$n]['@attributes']['VisibleIdNo'];
                    $lineArray['NAME'] = $array['Females'][$n]['@attributes']['BarnName'];
                    $lineArray['BIRTH_DATE'] = $array['Females'][$n]['@attributes']['BirthDate'];
                    $lineArray['STATUS'] = ($array['Females'][$n]['@attributes']['PdgreeStatus']);
                    $lineArray['TEST_DATE'] = $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['SampleDate'];
                    $lineArray['MILK_AM'] = (($array['Females'][$n]['Productions']['MilkWeights'][0]['@attributes']['Quantity'])) ?? ''; //use only total milk, these are not guranteed to be there.
                    $lineArray['MILK_PM'] = (($array['Females'][$n]['Productions']['MilkWeights'][1]['@attributes']['Quantity'])) ?? ''; //use only total milk, these are not guranteed to be there.
                    $lineArray['MILK_3X'] = (($array['Females'][$n]['Productions']['MilkWeights'][2]['@attributes']['Quantity'])) ?? ''; //use only total milk, these are not guranteed to be there.
                    $lineArray['TOTAL_MILK'] = (float) ($array['Females'][$n]['Productions']['@attributes']['Hr24Milk']);
                    $lineArray['FAT_PER'] = (float) $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['Fat'];
                    $lineArray['PROT_PER'] = (float) $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['CrudeProtein']; //crude protein
                    $lineArray['SSC'] = (float) $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['SCC'] * 1000;
                    $lineArray['MUN'] = (float) $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['MUN'];
                    $lineArray['BHB'] = (float) $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['BHB'];
                    $lineArray['LACTOSE_PER'] = (float) $array['Females'][$n]['AnimalMilkCompositions']['@attributes']['Lactose'];
                    $lineArray['LACT_NU'] = (int) ($array['Females'][$n]['StartLactations'][0]['@attributes']['LactationNo'] ?? $array['Females'][$n]['StartLactations']['@attributes']['LactationNo']); //there can be many StartLactations in file. The first one is the latest one.
                    $lineArray['FRESH'] = $array['Females'][$n]['StartLactations'][0]['@attributes']['StartDate'] ?? $array['Females'][$n]['StartLactations']['@attributes']['StartDate'];
                    $lineArray['EVENT_CODE'] = $array['Females'][$n]['StartLactations'][0]['@attributes']['StartLactationCd'] ?? $array['Females'][$n]['StartLactations']['@attributes']['StartLactationCd'];
                    $lineArray['LACT_END'] = '1900-01-01';
                    $lineArray['LACT_E_COD'] = $array['Females'][$n]['AnimalYields']['@attributes']['LerCd'] ?? ''; ////projection are in animal yieldsfor current lactation.
                    $lineArray['LEFT_HERD'] = '1900-01-01';
                    $lineArray['LH_REASON1'] = '';
                    $lineArray['LH_REASON2'] = '';
                    $lineArray['M_WEIGHT_S'] = '';
                    $lineArray['CONDITION1'] = '';
                    $lineArray['CONDITION2'] = '';
                    $lineArray['SAMPLE1'] = '';
                    $lineArray['SAMPLE2'] = '';
                    $lineArray['DAYS_IN_MI'] = (int) $array['Females'][$n]['AnimalYields']['@attributes']['Dim'];
                    $lineArray['LACT_MILK'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['MilkLactDateYld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['LACT_FAT'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['FatLactDateYld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['LACT_PROT'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['ProteinLactDateYld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['BCA_INDICT'] = '';
                    $lineArray['BCA_MILK'] = (float) $array['Females'][$n]['AnimalYields']['@attributes']['MilkBca'];
                    $lineArray['BCA_FAT'] = (float) $array['Females'][$n]['AnimalYields']['@attributes']['FatBca'];
                    $lineArray['BCA_PROT'] = (float) $array['Females'][$n]['AnimalYields']['@attributes']['ProteinBca'];
                    $lineArray['MILK305'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['MilkDay305Yld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['MILK_REL'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['MilkDay305Rlblty'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['FAT305'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['FatDay305Yld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['FAT_REL'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['FatDay305Rlblty'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['PROT305'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['ProteinDay305Yld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['PROT_REL'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['ProteinDay305Rlblty'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['MILK_DEV'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['MilkLactPrsstncy'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['FAT_DEV'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['FatLactPrsstncy'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['PROT_DEV'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['ProteinLactPrsstncy'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['M_INTV_BRE'] = 0;
                    $lineArray['F_INTV_BRE'] = 0;
                    $lineArray['P_INTV_BRE'] = 0;
                    $lineArray['PEAK_DIM'] = (int) ($array['Females'][$n]['AnimalYields']['@attributes']['PeakDim'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['PEAK_MILK'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['PeakMilkYld'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['MILK_STAND'] = 0;
                    $lineArray['FLAG_MILK'] = 0;
                    $lineArray['BREED_DAT1'] = '1900-01-01';
                    $lineArray['SERVICE1'] = 0;
                    $lineArray['B_CODE1'] = 0;
                    $lineArray['BREED_DAT2'] = '1900-01-01';
                    $lineArray['SERVICE2'] = 0;
                    $lineArray['B_CODE2'] = 0;
                    $lineArray['STOP'] = 0;
                    $lineArray['PREGNANT'] = 0;
                    $lineArray['LINEAR_SSC'] = (float) ($array['Females'][$n]['AnimalYields']['@attributes']['SccLinearScoreAvg'] ?? '0'); ////projection are in animal yieldsfor current lactation.
                    $lineArray['BODY_COND'] = 0;
                    $lineArray['BODY_WEIGH'] = (empty(($array['Females'][$n]['Weights']['@attributes']['Value']))) ? 0 : (float) ($array['Females'][$n]['Weights']['@attributes']['Value']);
                    $lineArray['PERSISTANC'] = 0;
                    $lineArray['DAYS_TO_FI'] = 0;
                    $lineArray['DAYS_TO_L'] = 0;
                    $lineArray['TIMES_BRED'] = 0;
                    $lineArray['DAYS_DRY'] = 0;
                    $lineArray['COW_WEIGHT'] = (empty(($array['Females'][$n]['Weights']['@attributes']['Value']))) ? 0 : (float) ($array['Females'][$n]['Weights']['@attributes']['Value']);
                    $lineArray['VERIFY'] = 0;




                    $this->doInsertJson($lineArray);
                } //end if.
                $i++;
            }//end for loop over females


            print("File $fileName XML is done.<br/><br/>\n");
        }
    }

    //old way to process files like *.n33
    function doAdlicProcessingOld() {


        print("Doing the following OLD files:\n");

        $sql = "SELECT id,data::bytea,file_name FROM documents.valacta_tip ORDER BY file_name DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
        $stmt->execute();

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $data, PDO::PARAM_LOB);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);


        while ($row = $stmt->fetch(PDO::FETCH_BOUND)) {

            print("File $fileName is string....<br/>\n");


            $lines = explode(PHP_EOL, pg_unescape_bytea(stream_get_contents($data)));



            foreach ($lines as $line_num => $line) {

                $lineArray['HERD'] = substr($line, $x = 2, $y = 5);
                $lineArray['CONTROL'] = substr($line, $x = $x + $y, $y = 5);
                $lineArray['REG'] = trim(substr($line, $x = $x + $y, $y = 18));
                $lineArray['CHAIN'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['NAME'] = substr($line, $x = $x + $y, $y = 10);
                $lineArray['BIRTH_DATE'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['STATUS'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['TEST_DATE'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['MILK_AM'] = trim(substr($line, $x = $x + $y, $y = 3) * 0.1); //WRONG
                $lineArray['MILK_PM'] = trim(substr($line, $x = $x + $y, $y = 3) * 0.1); //WRONG
                $lineArray['MILK_3X'] = trim(substr($line, $x = $x + $y, $y = 3) * 0.1);
                $lineArray['TOTAL_MILK'] = trim(substr($line, $x = $x + $y, $y = 4) * 0.1); //WRONG
                $lineArray['FAT_PER'] = substr($line, $x = $x + $y, $y = 4) * 0.01;
                $lineArray['PROT_PER'] = substr($line, $x = $x + $y, $y = 4) * 0.01;
                $lineArray['SSC'] = substr($line, $x = $x + $y, $y = 5) * 1000;
                $lineArray['MUN'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['LACT_NU'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['FRESH'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['EVENT_CODE'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['LACT_END'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['LACT_E_COD'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['LEFT_HERD'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['LH_REASON1'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['LH_REASON2'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['M_WEIGHT_S'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['CONDITION1'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['CONDITION2'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['SAMPLE1'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['SAMPLE2'] = substr($line, $x = $x + $y, $y = 2); //THIS LINE NOT TESTED
                $lineArray['DAYS_IN_MI'] = substr($line, $x = $x + $y, $y = 4);  //RIGHT!
                $lineArray['LACT_MILK'] = substr($line, $x = $x + $y, $y = 5) * 0.01;
                $lineArray['LACT_FAT'] = substr($line, $x = $x + $y, $y = 4) * 0.01;
                $lineArray['LACT_PROT'] = substr($line, $x = $x + $y, $y = 4) * 0.01;
                $lineArray['BCA_INDICT'] = substr($line, $x = $x + $y, $y = 1);
                $lineArray['BCA_MILK'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['BCA_FAT'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['BCA_PROT'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['MILK305'] = substr($line, $x = $x + $y, $y = 5);
                $lineArray['MILK_REL'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['FAT305'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['FAT_REL'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['PROT305'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['PROT_REL'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['MILK_DEV'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['FAT_DEV'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['PROT_DEV'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['M_INTV_BRE'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['F_INTV_BRE'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['P_INTV_BRE'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['PEAK_DIM'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['PEAK_MILK'] = substr($line, $x = $x + $y, $y = 5);
                $lineArray['MILK_STAND'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['FLAG_MILK'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['BREED_DAT1'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['SERVICE1'] = substr($line, $x = $x + $y, $y = 18);
                $lineArray['B_CODE1'] = substr($line, $x = $x + $y, $y = 1);
                $lineArray['BREED_DAT2'] = substr($line, $x = $x + $y, $y = 8);
                $lineArray['SERVICE2'] = substr($line, $x = $x + $y, $y = 18);
                $lineArray['B_CODE2'] = substr($line, $x = $x + $y, $y = 1);
                $lineArray['STOP'] = substr($line, $x = $x + $y, $y = 1);
                $lineArray['PREGNANT'] = substr($line, $x = $x + $y, $y = 1);
                $lineArray['LINEAR_SSC'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['BODY_COND'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['BODY_WEIGH'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['PERSISTANC'] = substr($line, $x = $x + $y, $y = 3);
                $lineArray['DAYS_TO_FI'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['DAYS_TO_L'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['TIMES_BRED'] = substr($line, $x = $x + $y, $y = 2);
                $lineArray['DAYS_DRY'] = substr($line, $x = $x + $y, $y = 4);
                $lineArray['COW_WEIGHT'] = substr($line, $x = $x + $y, $y = 1);
                $lineArray['VERIFY'] = substr($line, $x = $x + $y, $y = 1);



                /*
                  //for debugging: outputs the rest of the line as one string.
                  $lineArray['REST']= substr($line,$x=$x+$y,strlen($line) );
                 */
                //print_r($lineArray);
                //echo "Line #<b>{$line_num}</b> : " . htmlspecialchars($line) . "<br />\n";
                $fileArray[] = $lineArray;


                /* now do a correction to change some 0's to nulls;
                 */
                //$lineArray['MILK_AM'] = self::zeroToNull($lineArray['MILK_AM']);
                //$lineArray['MILK_PM'] = self::zeroToNull($lineArray['MILK_PM']);
                //$lineArray['MILK_3X'] = self::zeroToNull($lineArray['MILK_3X']);
                //$lineArray['TOTAL_MILK'] = self::zeroToNull($lineArray['TOTAL_MILK']);
                //$lineArray['FAT_PER'] = self::zeroToNull($lineArray['FAT_PER']);
                //$lineArray['PROT_PER'] = self::zeroToNull($lineArray['PROT_PER']);
                //$lineArray['SSC'] = self::zeroToNull($lineArray['SSC']);
                //$lineArray['MUN'] = self::zeroToNull($lineArray['MUN']);
                /*                 * ***************************** */



                //HACK CenTRAL LOOK AT LATER, WHY THIS IS NEEDED
                $lineArray['LACT_END'] = '19000101';
                $lineArray['LEFT_HERD'] = '19000101';
                $lineArray['BREED_DAT1'] = '19000101';
                $lineArray['BREED_DAT2'] = '19000101';
                $lineArray['COW_WEIGHT'] = 0;
                $lineArray['VERIFY'] = 0;
                $lineArray['DAYS_DRY'] = 0;
                $lineArray['TIMES_BRED'] = 0;
                $lineArray['BHB'] = '';

                $this->doInsertJson($lineArray);
            }
        }
    }

//end function
//


    function doInsertJson($lineArray) {
        $json = json_encode($lineArray);
        $data = $json;
        $test_date = $lineArray['TEST_DATE'];


        //print($test_date);
        //exit();

        if (!empty($lineArray['TEST_DATE'])) {

            $query = "INSERT INTO batch.valacta_raw (chain_number,test_date,data) VALUES ({$lineArray['CHAIN']},'$test_date','$data')";
            //print($query);
            // check that there wasn't an error with a cows data that was corrected on another test, we just throw it away. The newer data file data would be correct,
            // but not sure how to tell which is the newer file. maybe take an average of all the test_dates in file to find out if it is newer??
            $sql = "SELECT test_date FROM batch.valacta_raw WHERE test_date='{$lineArray['TEST_DATE']}' AND chain_number='{$lineArray['CHAIN']}' LIMIT 1";
            $res = $GLOBALS['pdo']->query($sql);

            // only do insert if there is no previous data.	
            if ($res->rowCount() == 0) {
                //insert array into DB table batch.valacta_data
                $res = $GLOBALS['pdo']->exec($query);
            } else {
                // don't do anything. 
            }
        }
    }

//function convert 0's to nulls.
    function zeroToNull($value) {
        if ($value == 0) {
            return 'null';
        } else {
            return $value;
        }
    }

}
//end class

//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new ValactaDataFile();
    $cls->main();
} else {
    //nothing
}




/*
 * every thing runs through this maerilied view that neeeds updated when file is run, (done above). 
 * 
 */

/*

  CREATE MATERIALIZED VIEW batch.valacta_data AS
  SELECT COALESCE(valacta_raw.data ->> 'HERD'::text, ''::text)::numeric AS herd,
  COALESCE(valacta_raw.data ->> 'CONTROL'::text, ''::text)::numeric AS control,
  trim(COALESCE(valacta_raw.data ->> 'REG'::text, ''::text)) AS reg,
  COALESCE(valacta_raw.data ->> 'CHAIN'::text, ''::text)::numeric AS chain,
  COALESCE(valacta_raw.data ->> 'NAME'::text, ''::text) AS name,
  COALESCE(valacta_raw.data ->> 'BIRTH_DATE'::text, ''::text)::date AS birth_date,
  COALESCE(valacta_raw.data ->> 'STATUS'::text, ''::text)::numeric AS status,
  COALESCE(valacta_raw.data ->> 'TEST_DATE'::text, ''::text)::date AS test_date,
  nullif(valacta_raw.data ->> 'MILK_AM'::text, ''::text)::numeric AS milk_am,
  nullif(valacta_raw.data ->> 'MILK_PM'::text, ''::text)::numeric AS milk_pm,
  nullif(valacta_raw.data ->> 'MILK_3X'::text, ''::text)::numeric AS milk_3x,
  COALESCE(valacta_raw.data ->> 'TOTAL_MILK'::text, ''::text)::numeric AS total_milk,
  COALESCE(valacta_raw.data ->> 'FAT_PER'::text, ''::text)::numeric AS fat_per,
  COALESCE(valacta_raw.data ->> 'PROT_PER'::text, ''::text)::numeric AS prot_per,
  nullif(valacta_raw.data ->> 'LACTOSE_PER'::text, ''::text)::numeric AS lactose_per,
  COALESCE(valacta_raw.data ->> 'SSC'::text, ''::text)::numeric AS ssc,
  COALESCE(valacta_raw.data ->> 'MUN'::text, ''::text)::numeric AS mun,
  nullif(valacta_raw.data ->> 'BHB'::text, ''::text)::numeric AS bhb,
  COALESCE(valacta_raw.data ->> 'LACT_NU'::text, ''::text)::numeric AS lact_nu,
  COALESCE(valacta_raw.data ->> 'FRESH'::text, NULL::text)::date AS fresh,
  COALESCE(valacta_raw.data ->> 'EVENT_CODE'::text, ''::text) AS event_code,
  COALESCE(valacta_raw.data ->> 'LACT_END'::text, NULL::text)::date AS lact_end,
  COALESCE(valacta_raw.data ->> 'LACT_E_COD'::text, ''::text) AS lact_e_cod,
  COALESCE(valacta_raw.data ->> 'LEFT_HERD'::text, NULL::text)::date AS left_herd,
  COALESCE(valacta_raw.data ->> 'LH_REASON1'::text, ''::text) AS lh_reason1,
  COALESCE(valacta_raw.data ->> 'LH_REASON2'::text, ''::text) AS lh_reason2,
  COALESCE(valacta_raw.data ->> 'M_WEIGHT_S'::text, ''::text) AS m_weight_s,
  COALESCE(valacta_raw.data ->> 'CONDITION1'::text, ''::text) AS condition1,
  COALESCE(valacta_raw.data ->> 'CONDITION2'::text, ''::text) AS condition2,
  COALESCE(valacta_raw.data ->> 'SAMPLE1'::text, ''::text) AS sample1,
  COALESCE(valacta_raw.data ->> 'SAMPLE2'::text, ''::text) AS sample2,
  COALESCE(valacta_raw.data ->> 'DAYS_IN_MI'::text, ''::text)::numeric AS days_in_mi,
  COALESCE(valacta_raw.data ->> 'LACT_MILK'::text, ''::text)::numeric AS lact_milk,
  COALESCE(valacta_raw.data ->> 'LACT_FAT'::text, ''::text)::numeric AS lact_fat,
  COALESCE(valacta_raw.data ->> 'LACT_PROT'::text, ''::text)::numeric AS lact_prot,
  COALESCE(valacta_raw.data ->> 'BCA_INDICT'::text, ''::text) AS bca_indict,
  COALESCE(valacta_raw.data ->> 'BCA_MILK'::text, ''::text)::numeric AS bca_milk,
  COALESCE(valacta_raw.data ->> 'BCA_FAT'::text, ''::text)::numeric AS bca_fat,
  COALESCE(valacta_raw.data ->> 'BCA_PROT'::text, ''::text)::numeric AS bca_prot,
  COALESCE(valacta_raw.data ->> 'MILK305'::text, ''::text)::numeric AS milk305,
  COALESCE(valacta_raw.data ->> 'MILK_REL'::text, ''::text)::numeric AS milk_rel,
  COALESCE(valacta_raw.data ->> 'FAT305'::text, ''::text)::numeric AS fat305,
  COALESCE(valacta_raw.data ->> 'FAT_REL'::text, ''::text)::numeric AS fat_rel,
  COALESCE(valacta_raw.data ->> 'PROT305'::text, ''::text)::numeric AS prot305,
  COALESCE(valacta_raw.data ->> 'PROT_REL'::text, ''::text)::numeric AS prot_rel,
  COALESCE(valacta_raw.data ->> 'MILK_DEV'::text, ''::text)::numeric AS milk_dev,
  COALESCE(valacta_raw.data ->> 'FAT_DEV'::text, ''::text)::numeric AS fat_dev,
  COALESCE(valacta_raw.data ->> 'PROT_DEV'::text, ''::text)::numeric AS prot_dev,
  COALESCE(valacta_raw.data ->> 'M_INTV_BRE'::text, ''::text)::numeric AS m_intv_bre,
  COALESCE(valacta_raw.data ->> 'F_INTV_BRE'::text, ''::text)::numeric AS f_intv_bre,
  COALESCE(valacta_raw.data ->> 'P_INTV_BRE'::text, ''::text)::numeric AS p_intv_bre,
  COALESCE(valacta_raw.data ->> 'PEAK_DIM'::text, ''::text)::numeric AS peak_dim,
  COALESCE(valacta_raw.data ->> 'PEAK_MILK'::text, ''::text)::numeric AS peak_milk,
  COALESCE(valacta_raw.data ->> 'MILK_STAND'::text, ''::text)::numeric AS milk_stand,
  COALESCE(valacta_raw.data ->> 'FLAG_MILK'::text, ''::text) AS flag_milk,
  COALESCE(valacta_raw.data ->> 'BREED_DAT1'::text, ''::text)::date AS breed_dat1,
  COALESCE(valacta_raw.data ->> 'SERVICE1'::text, ''::text) AS service1,
  COALESCE(valacta_raw.data ->> 'B_CODE1'::text, ''::text) AS b_code1,
  COALESCE(valacta_raw.data ->> 'BREED_DAT2'::text, ''::text)::date AS breed_dat2,
  COALESCE(valacta_raw.data ->> 'SERVICE2'::text, ''::text) AS service2,
  COALESCE(valacta_raw.data ->> 'B_CODE2'::text, ''::text) AS b_code2,
  COALESCE(valacta_raw.data ->> 'STOP'::text, ''::text) AS stop,
  COALESCE(valacta_raw.data ->> 'PREGNANT'::text, ''::text) AS pregnant,
  COALESCE(valacta_raw.data ->> 'LINEAR_SSC'::text, ''::text)::numeric AS linear_ssc,
  COALESCE(valacta_raw.data ->> 'BODY_COND'::text, ''::text)::numeric AS body_cond,
  COALESCE(valacta_raw.data ->> 'BODY_WEIGH'::text, ''::text)::numeric AS body_weigh,
  COALESCE(valacta_raw.data ->> 'PERSISTANC'::text, ''::text)::numeric AS persistanc,
  COALESCE(valacta_raw.data ->> 'DAYS_TO_FI'::text, ''::text)::numeric AS days_to_fi,
  COALESCE(valacta_raw.data ->> 'DAYS_TO_L'::text, ''::text)::numeric AS days_to_l,
  COALESCE(valacta_raw.data ->> 'TIMES_BRED'::text, ''::text)::numeric AS times_bred,
  COALESCE(valacta_raw.data ->> 'DAYS_DRY'::text, ''::text)::numeric AS days_dry,
  COALESCE(valacta_raw.data ->> 'COW_WEIGHT'::text, ''::text)::numeric AS cow_weight,
  COALESCE(valacta_raw.data ->> 'VERIFY'::text, ''::text)::numeric AS verify
  FROM valacta_raw;




 */
?>