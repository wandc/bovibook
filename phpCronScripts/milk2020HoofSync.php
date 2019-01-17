<?php

/*
 * This class contacts milk2020 and downlaod hoof data
 * 
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

class Milk2020HoofSync {

    function downloadData() {

        print("hello\n\r");

        $crl = curl_init();
        $url = "http://app.milk2020.ca/api/trim/farm?producer_num=" . $GLOBALS['config']['MILK2020_HOOF']['producerNumber'];
        curl_setopt($crl, CURLOPT_URL, $url);
        curl_setopt($crl, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($crl, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($crl, CURLOPT_POST, 1);
        $headers = array(
            'Content-Type:application/json',
            'Authorization: Basic ' . base64_encode($GLOBALS['config']['MILK2020_HOOF']['login'] . ":" . $GLOBALS['config']['MILK2020_HOOF']['password']) // <---
        );
        curl_setopt($crl, CURLOPT_HTTPHEADER, $headers);
        $result = curl_exec($crl);
        $headers = curl_getinfo($crl);
        curl_close($crl);
        json_decode($result); //used for json error check below
        if ((strpos($result, 'Little River') !== false) && (json_last_error() == JSON_ERROR_NONE)) {
            print("We have sucessfully logged in and downloaded some data!\n\r");



            $array = json_decode($result, TRUE);

            //print_r($array['trim_history']);

            foreach ($array['trim_history'] as &$value) {


                $sql1 = "SELECT trim_time FROM batch.milk2020_hoof WHERE chain_number={$value['cow_number']} AND trim_time='{$value['trim_datetime']}' ";
                $res1 = $GLOBALS['pdo']->query($sql1);

                //only insert if it has not been done before.
                if ($res1->rowCount() == 0) {

                    //print_r($value);
                    $json = null;
                    $json = json_encode($value);

                    $query = "INSERT INTO batch.milk2020_hoof  (chain_number,trim_time,data) VALUES ({$value['cow_number']},'{$value['trim_datetime']}','$json')";
                    $res = $GLOBALS['pdo']->exec($query);
                }
            }




            return true;
        } else {
            print("Login failed OR data download failed. Exiting!\n\r");
            exit();
        }
    }

}

//end class
//run from cli
$xx = new Milk2020HoofSync();
$xx->downloadData();
?>