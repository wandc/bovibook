<?php 

require_once('../global.php');

/**
 * Reads the daily ontario commodity reports and stores them in the db.
 */
class CommodityReport {

    function main() {

        echo("<h1>Started processing Commodity email messages.</h1>\n");


        
//login to imap server
        $mbox = imap_open("{imap.gmail.com:993/imap/ssl/novalidate-cert}INBOX", $GLOBALS['config']['COMMODITY_REPORT']['LOGIN'], $GLOBALS['config']['COMMODITY_REPORT']['PASSWORD'])
                or die("can't connect: " . imap_last_error());




//get list of email headers.
        $headers = imap_headers($mbox);
        if ($headers == false) {
            echo "Call failed<br />\n";
        } else {
            print(count($headers) . " is the number of headers in the mailbox.\n\r");
            $count = 0;
            foreach ($headers as $val) {

                $count++;
                print("Doing hearder #$count " . $val . " \n\r");
                //echo $val . "<br />\n";
                $body = imap_body($mbox, $count);
                // usleep(100000); //wait .1s, seems to fix a probelm with imap.
                //print_r($body);

                $header = imap_header($mbox, $count); // get header so we know message date.
                $emailDate = $header->udate;
                $infoArray[$count] = $this->getAllInformationFromEmail($body); //grab info from email and store in array
                //if ($count==15) {break;} //DEBUG
            }
        }

//print_r($infoArray);
//store in DB.
        $this->storeInfoInDB($infoArray);


        echo("<h1>Finished processing email messages.</h1>\n");
//close connection
        imap_close($mbox);


///////////////////////////////////////////////////////
    }

    function getSoybeansSorelQuebec($messageBody) {
        //Note: matches when some info N/A.  
        preg_match("/(Sorel QC|Sorel QC      N\/A)\s*(?<spot>([-]?[0-9]+(\.[0-9]+)))\s*(?<bushel>([-]?[0-9]+(\.[0-9]+)))\s*(?<tonne>([-]?[0-9]+(\.[0-9]+)))\s*/", $messageBody, $matches);

        if (((is_numeric($matches['spot']) != true) || (is_numeric($matches['bushel']) != true) || (is_numeric($matches['tonne']) != true)) AND ( (is_null($matches['spot']) != true) || (is_null($matches['bushel']) != true) || (is_null($matches['tonne']) != true))) {
            throw new exception("Soybeans Sorel Quebec price not valid");
        }

        $returnArray['soybeansSorelQuebec'] = $matches['tonne'];
        return $returnArray;
    }

    function getCornSWQuebec($messageBody) {
        //Note: matches when some info N/A.  
        preg_match("/(FOB SW Que|FOB SW Que  N\/A)\s*(?<spot>([-]?[0-9]+(\.[0-9]+)))\s*(?<bushel>([-]?[0-9]+(\.[0-9]+)))\s*(?<tonne>([-]?[0-9]+(\.[0-9]+)))\s*/", $messageBody, $matches);

        if (((is_numeric($matches['spot']) != true) || (is_numeric($matches['bushel']) != true) || (is_numeric($matches['tonne']) != true)) AND ( (is_null($matches['spot']) != true) || (is_null($matches['bushel']) != true) || (is_null($matches['tonne']) != true))) {
            throw new exception("Corn SQ Quebec price not valid");
        }

        $returnArray['cornSWQuebec'] = $matches['tonne'];
        return $returnArray;
    }
    
    function getCornWesternOntarioFeed($messageBody) {
        
        //not match: W O Feed     1.16                   0.74 4.91 193.10
        //macthing: W O Feed     1.36                        4.85 191.11
        //Note: matches when some info N/A.  
        preg_match("/(W O Feed|W O Feed  N\/A)\s*(?<spot>([-]?[0-9]+(\.[0-9]+)))\s*(?<bushel>([-]?[0-9]+(\.[0-9]+)))\s*(?<tonne>([-]?[0-9]+(\.[0-9]+)))\s*/", $messageBody, $matches);

        if (((is_numeric($matches['spot']) != true) || (is_numeric($matches['bushel']) != true) || (is_numeric($matches['tonne']) != true)) AND ( (is_null($matches['spot']) != true) || (is_null($matches['bushel']) != true) || (is_null($matches['tonne']) != true))) {
            throw new exception("W O Feed Corn price not valid");
        }

        $returnArray['cornWesternOntario'] = $matches['tonne'];
        return $returnArray;
        
        
        
        
    }
    
    

    function getFeedWheat($messageBody) {
        preg_match("/Bayport Wheat\s*(?<priceLow>\d{3}.\d{2})\s-\s(?<priceHigh>\d{3}.\d{2})/", $messageBody, $matches);

        if (((is_numeric($matches['priceLow']) != true) || (is_numeric($matches['priceHigh']) != true)) AND ( (is_null($matches['priceLow']) != true) || (is_null($matches['priceHigh']) != true))) {
            throw new exception("Bayport Wheat price not valid: priceLow:{$matches['priceLow']}  priceHigh:{$matches['priceHigh']}");
        }

        $returnArray['wheatLow'] = $matches['priceLow'];
        $returnArray['wheatHigh'] = $matches['priceHigh'];
        return $returnArray;
    }

    function getFeedOats($messageBody) {
        preg_match("/Bayport Oats\s*(?<priceLow>\d{3}.\d{2})\s*/", $messageBody, $matches);

        if (((is_numeric($matches['priceLow']) != true)) AND ( (is_null($matches['priceLow']) != true))) {
            throw new exception("Bayport Oats price not valid");
        }

        $returnArray['oatsLow'] = $matches['priceLow'];
//$returnArray['oatsHigh']=$matches['priceHigh'];
        return $returnArray;
    }

    function getFeedBarley($messageBody) {
        preg_match("/Bayport Barley\s*(?<priceLow>\d{3}.\d{2})\s*/", $messageBody, $matches);

        if (((is_numeric($matches['priceLow']) != true)) AND ( (is_null($matches['priceLow']) != true))) {
            throw new exception("Barley Oats price not valid");
        }

        $returnArray['barleyLow'] = $matches['priceLow'];
//$returnArray['barleyHigh']=$matches['priceHigh'];
        return $returnArray;
    }

//parse hamiltion soybean meal pricing.
    function getSoybeanMealHamiltion($messageBody) {
        preg_match("/FOB Hm Soymeal\s*(?<priceLow>\d{3}.\d{2})\s-\s(?<priceHigh>\d{3}.\d{2})/", $messageBody, $matches);

        if (((is_numeric($matches['priceLow']) != true) || (is_numeric($matches['priceHigh']) != true)) AND ( (is_null($matches['priceLow']) != true) || (is_null($matches['priceHigh']) != true))) {
            throw new exception("Hamilton Soybean price not valid");
        }

        $returnArray['soybeanMealHamiltonLow'] = $matches['priceLow'];
        $returnArray['soybeanMealHamiltonHigh'] = $matches['priceHigh'];
        return $returnArray;
    }

    function getDDG($messageBody) {
//preg_match("/DDG's FOB Srn, Wndr, Aylm\s*(?<priceLow>\d{3}.\d{2})\s-\s(?<priceHigh>\d{3}.\d{2})/",$messageBody,$matches);
        //needs to match the old way OR new way teh emails were written.
        preg_match("/DDG's FOB (Srn, Wndr, Aylm|Srn & Aylm)\s*(?<priceLow>\d{3}.\d{2})\s-\s(?<priceHigh>\d{3}.\d{2})/", $messageBody, $matches);

        if (((is_numeric($matches['priceLow']) != true) || (is_numeric($matches['priceHigh']) != true)) AND ( (is_null($matches['priceLow']) != true) || (is_null($matches['priceHigh']) != true))) {
            throw new exception("DDG price not valid");
        }

        $returnArray['DDGLow'] = $matches['priceLow'];
        $returnArray['DDGHigh'] = $matches['priceHigh'];
        return $returnArray;
    }

    function getDateOfReport($messageBody) {
        preg_match("/Date:\s*(?<dateStr>.*)/", $messageBody, $matches);

        if (strtotime($matches['dateStr']) == false) {
            throw new exception("Commodity report date not valid");
        }

        $returnArray['date'] = date('r', strtotime($matches['dateStr']));
        return $returnArray;
    }

    function getAllInformationFromEmail($messageBody) {


//print($body);
//assume if we find this string that it is an actual commodity report and thus we must continue, ignore other emails.
        if (strstr($messageBody, 'FARM MARKET NEWS - ONTARIO COMMODITY REPORT FOR') == true) {


//keep appending output to array.
            $ret = array();
            $ret = array_merge($this->getDateOfReport($messageBody), $ret);
            $ret = array_merge($this->getCornSWQuebec($messageBody), $ret);
            $ret = array_merge($this->getCornWesternOntarioFeed($messageBody), $ret);
            $ret = array_merge($this->getSoybeanMealHamiltion($messageBody), $ret);
            $ret = array_merge($this->getSoybeansSorelQuebec($messageBody), $ret);
            $ret = array_merge($this->getFeedWheat($messageBody), $ret);
            $ret = array_merge($this->getFeedOats($messageBody), $ret);
            $ret = array_merge($this->getFeedBarley($messageBody), $ret);
            $ret = array_merge($this->getDDG($messageBody), $ret);

//check array for null values, if any are found, convert to zero.
            foreach ($ret as $key => $value) {
                if ($key != 'date') {
                    if (is_null($value) == true) {
                        $ret["$key"] = '0';
                    }
                }
            }


            return $ret;
        }
    }

//store info in DB.
    function storeInfoInDB($infoArray) {
// Open a DB transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

//empty the table.
            $query = "TRUNCATE batch.commodity_report";
            
            $res = $GLOBALS['pdo']->exec($query);

            foreach ($infoArray as $data) {

                print_r($data);


//only try to insert non null array elements.
                if ((is_array($data) == true) && (array_key_exists('DDGLow', $data))) {

                    $query = "INSERT INTO batch.commodity_report 
(event_time,ddgs_low,ddgs_high,barley,oats,wheat_low,wheat_high,soybeans_sorel,soybeanmeal_hamilton_low,soybeanmeal_hamilton_high,corn_quebec,corn_west_ontario) VALUES ('{$data['date']}','{$data['DDGLow']}','{$data['DDGHigh']}','{$data['barleyLow']}','{$data['oatsLow']}','{$data['wheatLow']}','{$data['wheatHigh']}','{$data['soybeansSorelQuebec']}','{$data['soybeanMealHamiltonLow']}','{$data['soybeanMealHamiltonHigh']}','{$data['cornSWQuebec']}','{$data['cornWesternOntario']}');";
                    
                    $res = $GLOBALS['pdo']->exec($query);
                }
            }

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }



        return null;
    }

}

//end class

$xx = new CommodityReport();
$xx->main();
?>