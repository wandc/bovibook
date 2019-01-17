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
          $this->insertComponentsData($componentsArr);
          $this->insertHealthData($healthArr);
          SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
          //DEBUG
        //print_r($data);
        //print_r($healthArr);
        //print_r($componentsArr);
    }

    function downloadData() {

        print("helllo\n\r");
        $crl = curl_init();
        $url ="https://www.milkquality.ca/mqm.d7/?q=node/1&amp;destination=node/1";
        curl_setopt($crl, CURLOPT_URL, $url);
        curl_setopt($crl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt");
        curl_setopt($crl, CURLOPT_COOKIEJAR, "/tmp/cookie.txt");
        curl_setopt($crl, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($crl, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($crl, CURLOPT_POST, 1);
        $postdata = array(
            "name" => $GLOBALS['config']['MARITIME_QUALITY_MILK']['login'],
            "pass" => $GLOBALS['config']['MARITIME_QUALITY_MILK']['password'],
            "form_id" => "user_login_block",
            "op" => "Log in",
        );
        curl_setopt($crl, CURLOPT_POSTFIELDS, $postdata);
        $result = curl_exec($crl);
        $headers = curl_getinfo($crl);
        //curl_close($crl);
        if (strpos($result, 'Log out') !== false) {
            print("We have sucessfully logged in!\n\r");
            print("Now see if we can download form of MQM component data...\n\r");


            curl_setopt($crl, CURLOPT_URL, 'https://www.milkquality.ca/?q=mqm_data_tbl/form'); // set url for next request

            $result2 = curl_exec($crl); // make request on the same handle with the current login session
          
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
        } else {
            print("Login failed. Exiting!\n\r");
            exit();
        }
    }

    function insertComponentsData($componentsArr) {


        $BulkTankTestPeriods = new BulkTankTestPeriods();



        foreach ($componentsArr as &$row) {
            $month = date('M', strtotime($row['date']));
            $year = date('Y', strtotime($row['date']));
            $dateMonthYearUnix = strtotime("1-{$month}-{$year} 00:00:00"); //return seconds since unix epoch to start of month

            $testData = $BulkTankTestPeriods->generateTestDateRange($row['period'], $dateMonthYearUnix);

            $sql1 = "SELECT test_time_start FROM batch.nb_bulk_tank_sample WHERE test_time_start='{$testData['startTime']}'";
            $res1 = $GLOBALS['pdo']->query($sql1);

            //only insert if it has not been done before.
            if ($res1->rowCount() == 0) {
                $query = "INSERT INTO batch.nb_bulk_tank_sample 
(test_time_start,test_time_finish,average_test_time, fat, protein, lactose, scc,test_period) VALUES ('{$testData['startTime']}','{$testData['endTime']}','{$testData['testDate']}',{$row['fat']},{$row['prot']},{$row['los']},{$row['scc']},{$row['period']});";

                $res = $GLOBALS['pdo']->exec($query);
            }
        }
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

    function processComponents($data) {
        /*
         * matches the every 4 times a month component pattern
         * 
         */
        $holderArr = array();
        foreach ($data as &$Row) {
            //from mqm drupal table
            //"A,2016-07-07,1,,,  4.11,  3.53,  5.81, 13.45, 140000,9.34,2.27,"

            preg_match("/(?<tank_number>(A|B)),(?<date>([0-9]{4})-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])),(?<period>(1|2|3|4)),,,\s*(?<fat>([0-9.]+)),\s*(?<prot>([0-9.]+)),\s*(?<los>([0-9.]+)),\s*(?<tos>([0-9.]+)),\s*(?<scc>([0-9,]+)),\s*(?<snf>([0-9.,]+)),\s*(?<snf_bf>([0-9.,]+)),/", $Row, $matches);

            if (!empty($matches)) {
                $out = array();
                //$out['farm_number'] = $matches['farm_number'];
                $out['period'] = $matches['period'];
                $out['date'] = $matches['date'];
                $out['fat'] = $matches['fat'];
                $out['prot'] = $matches['prot'];
                $out['los'] = $matches['los'];
                $out['tos'] = $matches['tos'];
                $out['scc'] = str_replace(',', '', $matches['scc']);
                $out['snf'] = $matches['snf'];
                $out['snf_bf'] = $matches['snf_bf'];


                $holderArr[] = $out;
            }
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

}

//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new MQMParser();
    $cls->main();
} else {
//nothing.
}

/**
 * used to create date ranges for bulk tank test.
 */
class BulkTankTestPeriods {

    function generateTestDateRange($period, $dateMonthYearUnix) {
//period 1	1-7   
//period 2	8-15
//period 3	16-23
//period 4	24-31
//Don't use the above method....
//create date. based on dviding the year into 48 periods, 7.06 days each. evenly distributing.
        $testDateUnix = ($period * 657450 - 328725) + $dateMonthYearUnix;
        $testDate = date('Y-m-d H:i:s', $testDateUnix);
//do range.
        $testDateRangeArray = $this->createTestDateRange($period, $dateMonthYearUnix);
        $testDateRangeArray['testDate'] = $testDate;
        $testDateRangeArray['period'] = $period;

        return $testDateRangeArray;
    }

    function createTestDateRange($period, $dateMonthYearUnix) {
//create start time based on first day of period.
//create end time based on last day of period.
//period 1	1-7   
//period 2	8-15
//period 3	16-23
//period 4	24-31

        switch ($period) {
            case 1:
                $one = 1;
                $two = 7;
                break;
            case 2:
                $one = 8;
                $two = 15;
                break;
            case 3:
                $one = 16;
                $two = 23;
                break;
            case 4:
                $one = 24;
                $two = $this->GetLastDayofMonth(date('Y', $dateMonthYearUnix), date('m', $dateMonthYearUnix)); //tricky need to get last day of month, not just 31.
                break;
            default:
                throw new Exception("Test period is not valid." . "\n\r");
        }

        $startTime = date("Y-m-$one 00:00:00", $dateMonthYearUnix);
        $endTime = date("Y-m-$two 23:59:59", $dateMonthYearUnix);

//print("StartTime:$startTime");
//print("EndTime:$endTime");


        $ret['startTime'] = $startTime;
        $ret['endTime'] = $endTime;
        return $ret;
    }

// This is a simple function that will get the last day of the month.
//FROM:http://php.net/manual/en/function.checkdate.php
    function GetLastDayofMonth($year, $month) {
        for ($day = 31; $day >= 28; $day--) {
            if (checkdate($month, $day, $year)) {
                return $day;
            }
        }
    }

}

//end class
?>
