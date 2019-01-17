<?php 

/*
 * Given a dam list and a sire list this using two for loops sends both numbers
 * to CDN who then calculates the inbreeding percent and other componenents such 
 * as LPI for potential offspring. If the data is less then a week old for a certain 
 * bull then it skips it. This is fine except when bull proofs are released by CDN 
 * then we need a forced update. 
 * 
 * 
 */

if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

/*
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(-1);
*/

//
// Contact the CDN website and get progeny inbreeding and LPI info by giving the Dam and Sire Reg Number
//
class CDNProgenyInbreedingMulti {
    
     const CDN_NUMBER_CONNECTIONS_PARALLEL = 50;
     private $uuid;
     private $pairArr; //this is the main array of sires and dams. nneds to be here for exception modification
     
     
    public function __construct() {
        $this->uuid=Misc::uuid_create();
    }

    public function sse($request = null) {
        $this->main();
    }

//run this when you want to empty the db table first.     
    function resetRun() {
        $this->tableReset();
        $this->main();
    }

    function tableReset() {

//empty the table.
        $query = "TRUNCATE batch.cdn_progeny_potential_genetics";
        
        $res = $GLOBALS['pdo']->exec($query);
    }

    function deleteOldData() {
        //does an inverse search for any sires that do not belong (do not have a straw in tank or do not have projection) and deletes them from table.
          $query = "DELETE FROM batch.cdn_progeny_potential_genetics o
WHERE o.sire_full_reg_number NOT IN (SELECT sire_full_reg_number FROM bovinemanagement.semen_straw_curr_summary
LEFT JOIN bovinemanagement.sire_semen_code ON semen_straw_curr_summary.semen_code=sire_semen_code.semen_code)";              
          $res = $GLOBALS['pdo']->exec($query);
    }
    
    /*
     * THIS NORMALLY RUNS....
     * 
     */
    function main() {
        print('<h1>Started processing progeny inbreeding from CDN website.</h1>\n');

        SSE_Message::send_message(1, 'Deleteing out of date data.', 1); //sse
        $this->deleteOldData();
        SSE_Message::send_message(2, 'Started Doing findDamSirePairs Portion.', 2); //sse
        $this->pairArr = $this->findDamSirePairs();
        SSE_Message::send_message(4, 'Finished Doing findDamSirePairs Portion.', 4); //sse
        SSE_Message::send_message(5, 'Started doing CDN Loop.', 5); //sse
        //SLOW CDN LOOP
        $csvStringArr = array();

        $arrayCount = count($this->pairArr);
        for ($i = 0; $i < $arrayCount; $i = $i + self::CDN_NUMBER_CONNECTIONS_PARALLEL) {
            print('loop:' . $i . "\n");

            $pairArrSubset = array_slice($this->pairArr, $i, self::CDN_NUMBER_CONNECTIONS_PARALLEL);
       
            //get data from website.
            //update SSE message
            if (!empty($pairArrSubset[0]->damReg)) {
            SSE_Message::send_message(6 + $i, 'Doing around ' . $pairArrSubset[0]->damReg . ' on CDN iteration ' . $i . ' of ' . $arrayCount, 6 + round(($i / $arrayCount) * 100, 1)); //sse
            }
       
          
            $dataArrMulti = $this->contactCDNGetRawInbreedingInfoNEW($pairArrSubset);
            
       
            foreach ($dataArrMulti as $key => $value) {
                try {
                     if (empty($value)) {
                     throw new Exception("No Data for this key: $key.\n\r"); }
                $this->inbreedingInsertDB($value);
                } catch (CDNNoDataException $e) {
                    echo "CDN has no data for dam {$value['dam']} and sire {$value['sire']} skipping. \n";
                    //so lets remove the rest of teh instances of this dam from the pair array and redo pair array count.
                    $this->deleteDamFromPairArray($value['dam']);
           }                  
                    
                   
                   
                
                  
            
            }
        }

        SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
        print('<h1>Finished processing progeny inbreeding from CDN website.</h1>\n');



        //catch exceptions here to skip the rest of the sire for that dam, because there is probably a problem with the dam
        //ie. she was just registered and CDN doesn't have any data on here, so thus they will have no data for all sires.
    }

    //delete specific dam from array
    private function deleteDamFromPairArray($damReg) {
          foreach ($this->pairArr as $key => $value) {
             if ($value->damReg==$damReg) {
               unset($this->pairArr[$key]); 
             }
          }  
    }
    
    
    /* goes through all cows and looks for out of date ones and makes an array of objects of dam sire pairs */

    function findDamSirePairs() {

//select data to use.
//get an array of all sires we have semen for in the tank. NOT THE SAME AS THE SEMEN INVENTORY CODE.
        $sql = "SELECT DISTINCT full_reg_number,short_name,full_name
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE
GROUP BY full_reg_number,short_name,full_name
ORDER BY short_name";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        while ($row = $statement->fetch(PDO::FETCH_ASSOC)) {
            $sireRegArray[] = trim($row['full_reg_number']);
        }

//get an array of all current female bovines reg numbers who are of not waaaaay to youn to breed.
        $sql = "SELECT full_reg_number FROM bovinemanagement.bovinecurr WHERE (birth_date + interval '8 months') < current_date ORDER BY local_number";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $damTotalCount = $statement->rowCount();

        while ($row = $statement->fetch(PDO::FETCH_ASSOC)) {
            $damRegArray[] = trim($row['full_reg_number']);
        }

        print("Total possible combinations of sire/dam: ". count($sireRegArray)*count($damRegArray) . ".\n");
        
        
//go through array for each cow and sire.
        $damLoopCount = 0;
        $pairArr = array();
        foreach ($damRegArray as &$damReg) {

            //go through array for each cow and sire.
            foreach ($sireRegArray as &$sireReg) {
                $pairObj = new stdClass(); //create object
//first see if we need to bother updating the little river db.
//if the update time is less than a week ago, skip it.
                $sql = "SELECT update_time FROM batch.cdn_progeny_potential_genetics WHERE dam_full_reg_number='$damReg' AND sire_full_reg_number='$sireReg'";
                $statement2 = $GLOBALS['pdo']->prepare($sql);
                $statement2->execute();
                $row2 = $statement2->fetch(PDO::FETCH_ASSOC);

                if ((strtotime("-1 week") > strtotime($row2['update_time'])) OR (empty($row2['update_time']))) {
                    $pairObj->damReg = $damReg;
                    $pairObj->sireReg = $sireReg;
                    $pairArr[] = $pairObj;
                } else {
                    //print('DEBUG: No update necessary, data less than a week old.' . "\n");
                }
            }

            $damLoopCount++;
        }

        return $pairArr;
    }

//end function

    
    /*
      Contacts CDN and get the progeny inbreeding info as an output array.
     * This runs in parrallel for the request porion, so we don't wnat to have to big
     * of a $pairArrSmall, say 10 max??, or it hits the CDN site too hard.
     */
    function contactCDNGetRawInbreedingInfoNEW($pairArrSmall) {

        $ch = array();
        // build the multi-curl handle, adding all $ch[$key] to it.
        $mh = curl_multi_init();

     

        //go through each sire/dam mating pair.
        foreach ($pairArrSmall as $key => $value) {

//breakup reg numbers into component parts.
            $damArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($value->damReg);
            $sireArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($value->sireReg);

//first send the post to display a page as html.
            $url = "https://www.cdn.ca/inbreeding/choosemate.php";

            $post = Array();
            $post['_postback_'] = urlencode('1');
            $post['_formname_'] = urlencode('selectList');
            $post['q_sex'] = urlencode('M');
            $post['femaleMode'] = urlencode('individual');
            $post['femaleBreed'] = urlencode($damArray['breed']);
            $post['femaleCountry'] = urlencode($damArray['country']);
            $post['femaleReg'] = urlencode($damArray['number']);
            $post['maleMode'] = urlencode('individual');
            $post['q_breed'] = urlencode($sireArray['breed']);
            $post['maleCountry'] = urlencode($sireArray['country']);
            $post['maleReg'] = urlencode($sireArray['number']);
            $post['inbreedAnimals'] = urlencode('Continue');
            $post['_returnurl_'] = urlencode('https://www.cdn.ca/inbreeding/selectlist.php');

            // build the individual requests as above, but do not execute them
            $ch[$key] = curl_init($url);
            curl_setopt($ch[$key], CURLOPT_COOKIEJAR, "/tmp/curl_cookie_{$this->uuid}_$key");
            curl_setopt($ch[$key], CURLOPT_COOKIEFILE, "/tmp/curl_cookie_{$this->uuid}_$key");
            curl_setopt($ch[$key], CURLOPT_POSTFIELDS, $post);
            curl_setopt($ch[$key], CURLOPT_FOLLOWLOCATION, true);
            curl_setopt($ch[$key], CURLOPT_RETURNTRANSFER, true);
            //add handle
            curl_multi_add_handle($mh, $ch[$key]);

           

          
        } //end foreach

        // with curl_multi, you only have to wait for the longest-running request  
        // execute all queries simultaneously, and continue when all are complete
        //PAGE 1, only do one page
        $running = null;
        do {
            curl_multi_exec($mh, $running);
        } while ($running);

        //page 1 content
        $page1ContentArr=array();
        foreach ($ch as $key => $value) {
            $page1ContentArr[$key] = curl_multi_getcontent($value);        
        }

        $dataArr=array();
         foreach ($page1ContentArr as $key => $value) {
       $dataArr[$key]=$this->parseCDNInbreedingPageDataXPath($value, $pairArrSmall[$key]->damReg,$pairArrSmall[$key]->sireReg);
         }
        
        return $dataArr;
    }
    
/*
 * use xpath to parse table data, old way was to use CSV, but not reliable because of CDN bugs
 * 
 */    
private function parseCDNInbreedingPageDataXPath($pageContent, $dam,$potential_sire) {
        $dom = new DOMDocument();
        libxml_use_internal_errors(true);
        $dom->loadHTML($pageContent);
        //
        libxml_clear_errors();
        $xpath = new DOMXpath($dom);

       $dataArr=array();
       $dataArr["dam"]=$dam;
       $dataArr["sire"]=$potential_sire;
       
       for ($index = 1; $index < 17; $index++) {
           
       
        $rowHead = $xpath->query('//table[@class="list_table"]/tr[@class="list_head"][2]/td'."[$index]"); //head of progeny
        $rowData = $xpath->query('//table[@class="list_table"]/tr[@class="list_row_alt"][2]/td'."[$index]");
         if (!empty($rowData[0])) {
         $dataArr[$rowHead[0]->textContent]=trim($rowData[0]->textContent); //pu what we found into an array, with head as key and data as value
         }
       /*
        //DEBUG 
        print("Head\n");
        print($rowHead[0]->textContent);
        print("\nData\n");
        print($rowData[0]->textContent);
        print("\n------------------------------\n\r\n\r\n\r\n\r\n\r");
        */
       
         
         //remove positive signs from %F and %P.
         if (!empty($dataArr['%F'])) { $dataArr['%F']=str_replace('+','',$dataArr['%F']);}
           if (!empty($dataArr['%P'])) {$dataArr['%P']=str_replace('+','',$dataArr['%P']);}     
               
       }  
  return $dataArr; //return data array
    }

    

    /*
      Takes a xpath array and put what we need in the DB.
     */

    function inbreedingInsertDB($dataArr) {   
        
        //Do data validity checks. see if pro$ inbvreeding are numbers. if so , we assume valid.
         if ((empty($dataArr['%INB'])==true) AND (empty($dataArr['Pro$'])==true)) {
        throw new CDNNoDataException("CDN has no data for this dam/sire combination."); //we catch this later
             
         }
        
        if ((is_numeric($dataArr['%INB'])==true) AND (is_numeric($dataArr['Pro$'])==true)) {


// Open a transaction
           
                try {
                    $res = $GLOBALS['pdo']->beginTransaction();

//delete any old records (only one allowed).
                $sql = "DELETE FROM batch.cdn_progeny_potential_genetics WHERE dam_full_reg_number='{$dataArr['dam']}' AND sire_full_reg_number='{$dataArr['sire']}'";
                $statement = $GLOBALS['pdo']->prepare($sql);
                $statement->execute();

//insert collected info into the db.
                $sql="INSERT INTO batch.cdn_progeny_potential_genetics (dam_full_reg_number, sire_full_reg_number, lpi_code, lpi, prodoll, percent_inbreeding, milk, fat, protein, percent_fat, percent_protein, scs, conf, ms, f_and_l, ds, rp) VALUES ('{$dataArr['dam']}', '{$dataArr['sire']}', '{$dataArr['LPI Code']}', {$dataArr['LPI']}, {$dataArr['Pro$']}, {$dataArr['%INB']}, {$dataArr['MILK']}, {$dataArr['FAT']}, {$dataArr['PROT']}, {$dataArr['%F']}, {$dataArr['%P']}, {$dataArr['SCS']}, {$dataArr['Conf']}, {$dataArr['MS']}, {$dataArr['F&L']}, {$dataArr['DS']}, {$dataArr['RP']})";
             
                $statement = $GLOBALS['pdo']->prepare($sql);
                $statement->execute();

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
        } else {
            throw new CDNNoDataException("CDN has no data for this dam/sire combination."); //we catch this later
        }
    }

//function to clean-up the db table of cows that are no longer current and bulls we no longer stock.
    function dbTableCleanup($damRegArray, $sireRegArray) {

// do dams.
        $damLongWhere = null;
        foreach ($damRegArray as &$damReg) {
            $damLongWhere = $damLongWhere . 'dam_full_reg_number != ' . "'$damReg'" . ' AND ';
        }
        $damLongWhere = $damLongWhere . '1 != 2'; //smartass hack to correct for last floating AND.
//
        $sqlDam = 'DELETE FROM batch.cdn_progeny_potential_genetics WHERE ' . $damLongWhere;
        $statement = $GLOBALS['pdo']->prepare($sqlDam);
        $statement->execute();


//do sires.
        $sireLongWhere = null;
        foreach ($sireRegArray as &$sireReg) {
            $sireLongWhere = $sireLongWhere . 'sire_full_reg_number != ' . "'$sireReg'" . ' AND ';
        }
        $sireLongWhere = $sireLongWhere . '1 != 2'; //smartass hack to correct for last floating AND.
//
        $sqlSire = 'DELETE FROM batch.cdn_progeny_potential_genetics WHERE ' . $sireLongWhere;
        $statement = $GLOBALS['pdo']->prepare($sqlSire);
        $statement->execute();
    }

     /* returns datetime of last DB update, thus assumes main function completed then */
    public function lastRun() {
        $sql = "SELECT max(update_time) as max_update_time FROM batch.cdn_progeny_potential_genetics";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return strtotime($row['max_update_time']);
    }
    
    
}

//end class

/* custom class for when the dam has no data in CDN, we catch it agian upstream */

class CDNNoDataException extends Exception {

    public function __construct($message, $code = 0, Exception $previous = null) {
        $trace = $this->getTrace();
        $cls = $trace[0]['class'];
        $message = $cls . ": $message";
        parent::__construct($message, $code, $previous);
    }

}

//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new CDNProgenyInbreedingMulti();
    $cls->main();
} else {
    //nothing
}
?>