<?php

require_once('../global.php');

/**
 * Reads the RPC and Lacatanet milk sample emails sent by the milk board.
 * Checks a special email address where RPC emails are forwarded to.
 * This was used from June to Aug 2020, then the NB milkboard switched to MQM again.
 * THIS IS NO LONGER USED.
 */
class ProcessNBBulkTankTest {

    public function sse($request = null) {
        $this->main();
    }

    function main() {

        echo("<h1>Started processing email messages.</h1>\n");

        SSE_Message::send_message(1, 'Starting.', 1); //sse
        SSE_Message::send_message(10, 'Downloading data...', 10); //sse
        $data = $this->downloadData();
        SSE_Message::send_message(90, 'Insering into DB...', 90); //sse
        $this->storeInfoInDB($data);
        SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse  

        echo("<h1>Finished processing email messages.</h1>\n");
    }

    function downloadData() {

        $mbox = imap_open("{imap.gmail.com:993/imap/ssl/novalidate-cert}INBOX", $GLOBALS['config']['RPC_BULK_TANK']['LOGIN'], $GLOBALS['config']['RPC_BULK_TANK']['PASSWORD'])
                or die("can't connect: " . imap_last_error());

        $headers = imap_headers($mbox);
        if ($headers == false) {
            echo "IMAP call failed! <br/>\n";
        } else {

            $totalHeaders = count($headers);

            print(count($headers) . " is the number of headers in the mailbox.\n\r");
            $count = 0;
            foreach ($headers as $val) {

                $count++;
                //print("Doing hearder #$count " . $val . " \n\r"); //DEBUG

                if (($count) >= $totalHeaders - 20) { //ONLY LOOK AT LAST 20 MESSAGES.
                    //echo $val . "<br />\n";
                    $body = imap_body($mbox, $count);
                    // usleep(100000); //wait .1s, seems to fix a probelm with imap.
                    // print_r($body); //DEBUG

                    $header = imap_header($mbox, $count); // get header so we know message date.
                    $emailDate = $header->udate;
                    $infoArray[$count] = $this->getAllInformationFromEmail($body); //grab info from email and store in array
                    $infoArray[$count]['email_date'] = date('r', $emailDate);
                    //if ($count==15) {break;} //DEBUG
                } else {
                    //print("skipping, too old count:" . $count . " \n\r"); //DEBUG
                }
            }
        }


       // var_dump($infoArray); //DEBUG

        imap_close($mbox);
        return $infoArray;
    }

    function getDateOfReport($messageBody) {
        preg_match("/Date:\s*(?<dateStr>.*)/", $messageBody, $matches);

        if (strtotime($matches['dateStr']) == false) {
            throw new exception("Bulk Tank report date not valid");
        }

        $returnArray['test_date'] = date('r', strtotime($matches['dateStr']));
        return $returnArray;
    }

    function getTankNumber($messageBody) {
        preg_match("/Tank # \/ Citerne #:\s*(?<tankStr>\d{1})/", $messageBody, $matches);

        if (($matches['tankStr'] != '1') AND ($matches['tankStr'] != '2')) {
            throw new exception("Tank number not read, instead we got: " . "|" . $matches['tankStr'] . "|");
        }

        $matches['tankStr'] = $matches['tankStr'] == 1 ? 'A' : 'B'; //convert to how tanks are recorded locally. 

        $returnArray['tank_num'] = $matches['tankStr'];
        return $returnArray;
    }

    function validTest($messageBody) {

        preg_match("/Valid Test\? \/ Test Valide\?:\s(?<validTestStr>.*)\s\//", $messageBody, $matches);

        if (($matches['validTestStr'] != 'Yes') AND ($matches['validTestStr'] != 'No')) {

            throw new exception("Valid Test (Yes or No) not read, instead we got: " . $matches['validTestStr']);
        }


        $returnArray['validTest'] = $matches['validTestStr'];
        return $returnArray;
    }

    function butterFat($messageBody) {

        preg_match("/BF \/ MG:\s(?<validNumber>\d{1}.\d{2})/", $messageBody, $matches);

        if (!is_numeric($matches['validNumber'])) {
            throw new exception("Valid Butterfat number not read, instead we got: " . $matches['validNumber']);
        }

        $returnArray['butterfat'] = $matches['validNumber'];
        return $returnArray;
    }

    function protein($messageBody) {

        preg_match("/Protein \/ Prot=E9ine:\s(?<validNumber>\d{1}.\d{2})/", $messageBody, $matches);

        if (!is_numeric($matches['validNumber'])) {
            throw new exception("Valid Protein number not read, instead we got: " . $matches['validNumber']);
        }

        $returnArray['protein'] = $matches['validNumber'];
        return $returnArray;
    }

    function lactose($messageBody) {

        preg_match("/LOS \/ LAS:\s(?<validNumber>\d{1}.\d{2})/", $messageBody, $matches);

        if (!is_numeric($matches['validNumber'])) {
            throw new exception("Valid LOS number not read, instead we got: " . $matches['validNumber']);
        }

        $returnArray['lactose'] = $matches['validNumber'];
        return $returnArray;
    }

    function scc($messageBody) {

        preg_match("/SCC \/ CCC:\s(?<validNumber>\d*)/", $messageBody, $matches);

        if (!is_numeric($matches['validNumber'])) {
            throw new exception("Valid SCC number not read, instead we got: " . $matches['validNumber']);
        }

        $returnArray['scc'] = $matches['validNumber'];
        return $returnArray;
    }


    function mun($messageBody) {

        preg_match("/MUN \/ LUA:\s(?<validNumber>\d{1,2}.\d{2})/", $messageBody, $matches);

        if ((empty($matches['validNumber'])) OR ($matches['validNumber']==0)) {
            $returnArray['mun'] = null; //MUN is optional, so if not there, place a null. 
        } else {
            if (!is_numeric($matches['validNumber'])) {
                throw new exception("Valid MUN number not read properly, instead we got: " . $matches['validNumber']);
            } else {
                $returnArray['mun'] = $matches['validNumber'];
            }
        }

        return $returnArray;
    }

    function getAllInformationFromEmail($messageBody) {


//print($body);
//assume if we find this string that it is an actual commodity report and thus we must continue, ignore other emails.
        if (strstr($messageBody, 'Component Test Results') == true) {


//keep appending output to array.
            $ret = array();
            $ret = array_merge($this->getDateOfReport($messageBody), $ret);
            $ret = array_merge($this->getTankNumber($messageBody), $ret);
            $ret = array_merge($this->validTest($messageBody), $ret);
            $ret = array_merge($this->butterFat($messageBody), $ret);
            $ret = array_merge($this->protein($messageBody), $ret);
            $ret = array_merge($this->lactose($messageBody), $ret);
            $ret = array_merge($this->scc($messageBody), $ret);
            $ret = array_merge($this->mun($messageBody), $ret);
// $ret = array_merge($this->mun($messageBody), $ret);
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


            foreach ($infoArray as $data) {

                //print_r($data); //DEBUG
//only try to insert non null array elements.
                if ((is_array($data) == true) && (array_key_exists('butterfat', $data) && ($data['validTest'] == 'Yes'))) {
 $query = <<<SQL
                INSERT INTO batch.nb_bulk_tank_sample_every
                               (fat,protein,lactose,scc,tank_num,test_collection_date,test_reporting_date,mun)
                        VALUES (:butterfat,:protein,:lactose,:scc,:tank_num,:test_date,:email_date,:mun)
                        ON CONFLICT DO NOTHING
SQL;                    
                    
                    
                    $statement = $GLOBALS['pdo']->prepare($query);
                    $statement->bindValue(':butterfat', $data['butterfat'], PDO::PARAM_STR);
                    $statement->bindValue(':protein', $data['protein'], PDO::PARAM_STR);
                    $statement->bindValue(':lactose', $data['lactose'], PDO::PARAM_STR);
                    $statement->bindValue(':scc', $data['scc'], PDO::PARAM_INT);
                    $statement->bindValue(':tank_num', $data['tank_num'], PDO::PARAM_STR);
                    $statement->bindValue(':test_date', $data['test_date'], PDO::PARAM_STR);
                    $statement->bindValue(':email_date', $data['email_date'], PDO::PARAM_STR);
                    $statement->bindValue(':mun',  $data['mun'], PDO::PARAM_STR);
                    $statement->execute();
                }
            }

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }



        return null;
    }

}

//end class

$xx = new ProcessNBBulkTankTest();
$xx->main();
?>