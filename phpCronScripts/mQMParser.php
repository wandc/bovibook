<?php

/*
 * This class contacts maritime quality milk and gets the last 3 years of data. 
 * It then takes a hash of each line of the data and if it is different then what is in DB, it inserts it. OR just try and do insert and ignore duplicates.
 * 
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

class MQMParser {

    public function sse($request = null) {
        $this->main();
    }

    function main() {
        SSE_Message::send_message(1, 'Starting.', 1); //sse
        SSE_Message::send_message(10, 'Downloading data.', 10); //sse
        $data = $this->downloadData();


        SSE_Message::send_message(80, 'Started Processing data.', 80); //sse
        $componentsArr = $this->processComponents($data);
        $healthArr = $this->processHealth($data);
        //now insert only new data in database.
        SSE_Message::send_message(90, 'Insering into DB.', 90); //sse
        $this->storeInfoInDBComponents($componentsArr);
        $this->insertHealthData($healthArr);
        SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
        //DEBUG
       // print_r($data);
       // print_r($healthArr);
       // print_r($componentsArr);
    }

    function downloadData() {

        //first we use JSON drupal method to login...GREAT!
        $ch = curl_init();

        $login = $GLOBALS['config']['MARITIME_QUALITY_MILK']['login'];
        $pass = $GLOBALS['config']['MARITIME_QUALITY_MILK']['password'];


        curl_setopt($ch, CURLOPT_URL, 'https://www.milkquality.ca/user/login?_format=json');
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_COOKIEFILE, "/tmp/cookie.txt");
        curl_setopt($ch, CURLOPT_COOKIEJAR, "/tmp/cookie.txt");
        curl_setopt($ch, CURLOPT_POSTFIELDS, "{\"name\":\"$login\", \"pass\":\"$pass\"}");

        $headers = array();
        $headers[] = 'Content-Type: application/json';
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

        $json = curl_exec($ch);
        if (curl_errno($ch)) {
            echo 'Error:' . curl_error($ch);
        }


        json_decode($json);
        if (json_last_error() == JSON_ERROR_NONE) {
            //all good! Getting token.
            //TOKEN not needed for anything. Using cookies instead it seems. 
            $arr = (json_decode($json, true));
            //var_dump($arr); //DEBUG
            //$token=($arr['csrf_token']); //maybe only gets the first time we log in?
        } else {
            print("Login failed. JSON login did not work for some reason. $json . Exiting!\n\r");
            exit();
        }

        //now we are logged in and have a token to use for queries. 

        print("We have sucessfully logged in!\n\r");
        print("Now see if we can download form of MQM component data...\n\r");


        curl_setopt($ch, CURLOPT_URL, 'https://www.milkquality.ca/mqm-data-viewer'); // set url for next request

        $result2 = curl_exec($ch); // make request on the same handle with the current login session
        //print($result2);
        if (empty($result2)) {
            throw new Exception("Error: results page returned from curl is empty, ie we have wrong url or didn't post correctly.");
        }

        $doc = new DOMDocument();
        libxml_use_internal_errors(true); //suppress tag errors, because MQM does not know how to write html....probably.
        $doc->loadHTML($result2);
        libxml_use_internal_errors(false);  //enable tag errors again
        $xpath = new DOMXpath($doc);

        //get the table rows
        $tableComponents = $xpath->query('//*[@id="div-ajax-mqm-dt"]/table/thead/tr');


        $tableData = array();
        foreach ($tableComponents as $row) {
            $cells = $row->getElementsByTagName('td');


            $cellData = [];
            foreach ($cells as $cell) {
                $cellData[] = $cell->nodeValue;
            }
            //make table row look like csv, for later processing
            $tableData[] = (implode(",", $cellData) );
        }
        //print_r($tableData);
        return $tableData;
    }

    /*
     * Tank	PeriodDate	Period	SPC	LPC	BF	PROT	LOS	TOS	SCC	SNF	SNF/BF	FREZ
     */

    //store info in DB.
    function storeInfoInDBComponents($infoArray) {
// Open a DB transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            foreach ($infoArray as $data) {
                
                //onluy do daily tests, 4x month no longer supported. only insert non null
                if (!empty($data)) {
                if  (($data['type'] == 'daily') && (array_key_exists('butterfat', $data))) {
                
$query = <<<SQL
                INSERT INTO batch.nb_bulk_tank_sample_every
                               (fat,protein,lactose,scc,tank_num,test_collection_date,test_reporting_date,mun)
                        VALUES (:butterfat,:protein,:lactose,:scc,:tank_num,:test_date,:email_date,:mun)
                        ON CONFLICT DO NOTHING
SQL;


                    //print_r($data);

                    $statement = $GLOBALS['pdo']->prepare($query);
                    $statement->bindValue(':butterfat', $data['butterfat'], PDO::PARAM_STR);
                    $statement->bindValue(':protein', $data['protein'], PDO::PARAM_STR);
                    $statement->bindValue(':lactose', $data['lactose'], PDO::PARAM_STR);
                    $statement->bindValue(':scc', $data['scc'], PDO::PARAM_INT);
                    $statement->bindValue(':tank_num', $data['tank_num'], PDO::PARAM_STR);
                    $statement->bindValue(':test_date', $data['test_date'], PDO::PARAM_STR);
                    $statement->bindValue(':email_date', date("Y-m-d"), PDO::PARAM_STR);
                    $statement->bindValue(':mun', null, PDO::PARAM_NULL);
                    $statement->execute();
                
            }}}

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }



        return null;
    }

    function insertHealthData($healthArr) {


        foreach ($healthArr as &$row) {


            $sql1 = "SELECT event_time FROM batch.nb_bulk_tank_health WHERE event_time='{$row['date']}' AND tank='{$row['tank_number']}'";
            $res1 = $GLOBALS['pdo']->query($sql1);

            //only insert if it has not been done before.
            if ($res1->rowCount() == 0) {
                $query = "INSERT INTO batch.nb_bulk_tank_health 
(event_time,tank,spc, lpc) VALUES ('{$row['date']}','{$row['tank_number']}','{$row['spc']}',{$row['lpc']});";

                $res = $GLOBALS['pdo']->exec($query);
            }
        }
    }

    /*
     * old tests that were 4x per month. 
     * 
     */

    private function processComponents4xMonth($Row) {

        preg_match("/(?<tank_number>(A|B)),(?<date>([0-9]{4})-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])),(?<period>(1|2|3|4)),,,\s*(?<fat>([0-9.]+)),\s*(?<prot>([0-9.]+)),\s*(?<los>([0-9.]+)),\s*(?<tos>([0-9.]+)),\s*(?<scc>([0-9,]+)),\s*(?<snf>([0-9.,]+)),\s*(?<snf_bf>([0-9.,]+)),/", $Row, $matches);

        if (!empty($matches)) {

            //print_r($matches);
            $out = array();
            $out['type'] = '4xMonth';
            $out['period'] = $matches['period'];
            $out['date'] = $matches['date'];
            $out['fat'] = $matches['fat'];
            $out['prot'] = $matches['prot'];
            $out['los'] = $matches['los'];
            $out['tos'] = $matches['tos'];
            $out['scc'] = str_replace(',', '', $matches['scc']);
            $out['snf'] = $matches['snf'];
            $out['snf_bf'] = $matches['snf_bf'];


            return $out;
        }
    }

    /*
     * daily tests used after August 2020. 
     * Tanks changed to numierc from letters
     */

    private function processComponentsDaily($Row) {

        preg_match("/(?<tank_number>(1|2)),(?<date>([0-9]{4})-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])),,,,\s*(?<fat>([0-9.]+)),\s*(?<prot>([0-9.]+)),\s*(?<los>([0-9.]+)),\s*(?<tos>([0-9.]+)),\s*(?<scc>([0-9,]+)),\s*(?<snf>([0-9.,]+)),\s*(?<snf_bf>([0-9.,]+)),/", $Row, $matches);

        if (!empty($matches)) {

            //print_r($matches);
            $out = array();
            $out['type'] = 'daily';
            $out['test_date'] = $matches['date'];
            $out['butterfat'] = $matches['fat'];
            $out['protein'] = $matches['prot'];
            $out['lactose'] = $matches['los'];
            $out['tos'] = $matches['tos'];

            //the SCC has some weird leading zeros thing going on.
            $out['scc'] = ltrim($matches['scc'], '0');
            //they have rounding errors
            $out['snf'] = round($matches['snf'], 3);
            $out['snf_bf'] = $matches['snf_bf'];

            //tank number needs to be in letter format.
           
            if ($matches['tank_number']==1) {
                 $out['tank_num'] = 'A'; 
            }
            elseif ($matches['tank_number']==2) {
                 $out['tank_num'] = 'B'; 
            }else {
                throw new Exception("Error Tank number does not match 1 or 2.");
            }

            return $out;
        }
    }

    function processComponents($data) {
        /*
         * process all rows based on daily or 4x per month. 
         * 
         */
        $holderArr = array();
        foreach ($data as &$Row) {

            $holderArr[] = $this->processComponents4xMonth($Row);
            $holderArr[] = $this->processComponentsDaily($Row);
        }
        return $holderArr;
    }

    function processHealth($data) {
        /*
         * matches the health dept test
         * 
         */
        $holderArr2 = array();
        foreach ($data as &$Row) {

            //mqm drupal table with my csv
            //B,2017-03-07,,1300,30,,,,,,,,
            preg_match("/(?<tank_number>(A|B|)),(?<date>([0-9]{4})-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])),,(?<spc>([0-9]+)),(?<lpc>([0-9]+)),,,,,,,,/", $Row, $matches);


            //print_r($matches);
            if (!empty($matches)) {
                $out = array();

                $out['tank_number'] = $matches['tank_number'];
                $out['date'] = $matches['date'];
                $out['spc'] = str_replace(',', '', $matches['spc']);
                $out['lpc'] = str_replace(',', '', $matches['lpc']);
                $holderArr2[] = $out;
            }
        }

        return $holderArr2;
    }

    // returns datetime of last DB update
    public function lastRun() {
        $sql = "SELECT max(create_time) as max_update_time FROM batch.nb_bulk_tank_sample";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return strtotime($row['max_update_time']);
    }

}

//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new MQMParser();
    $cls->main();
} else {
//nothing.
}
?>