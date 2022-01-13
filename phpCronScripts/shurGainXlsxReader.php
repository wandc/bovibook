<?php
/*
 * shurgain sends us a spreadsheet with 3 sheets in it. We only need 2nd and 3rd. They also put a total at bottom we need to ignore. 
 * The two sheets correlate by invoice number, but sort that out in the database later. 
 * 
 * copy new files into ../local/dataFile/shurGainInvoice then run script from CLI and it will insert into db. then look at shurgain view.
 * 
 */



if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
    $_SERVER['DOCUMENT_ROOT'] = dirname(__DIR__) . '/'; //when called from cli, command line define constant.
} else {
   include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php'); 
}


class ShurGainXlsxReader {

 
    public function sse($request = null) {
        $this->main();
    }
    
    
    function main() {
         SSE_Message::send_message(1, 'Starting.', 1); //sse
        $sql = "SELECT id,data::bytea,file_name FROM documents.shur_gain_flock ORDER BY file_name DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
       

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $data, PDO::PARAM_LOB);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);
        $stmt->execute();

  
        $arrayCount = $stmt->rowCount();    
        $i=0;
        while ($lines = $stmt->fetch(PDO::FETCH_BOUND)) {

            
             SSE_Message::send_message(6 + $i, 'Doing around ' . $fileName . $i . ' of ' . $arrayCount, 6 + round(($i / $arrayCount) * 100, 1)); //sse

            
            $fileStr = ( ((stream_get_contents($data)))); //note this is binary datat, no escaping when it was inserting or now when selecting.

            $temp = tmpfile(); //create temp file
            $path = stream_get_meta_data($temp)['uri']; //get full path

            fwrite($temp, $fileStr); //write string to file
            fseek($temp, 0); //set pointer to 0 position.

            print("\n\r\n\r\n\r\n\r *** DOING $fileName ***\n\r");
            //print(" ||| DOING $fileName Sheet 1 |||\n\r\n\r\n\r");
            //insert both sheet 1st
            $rowsArr = $this->loadSpreadSheetSheet($path, 1);
            $this->DBUpsert_sheet1($rowsArr);

            //insert both sheet 2nd 
            //  print("\n\r\n\r\n\r\n\r ||| DOING $fileName Sheet 2 |||\n\r\n\r\n\r");
            $rowsArr2 = $this->loadSpreadSheetSheet($path, 2);
            $this->DBUpsert_sheet2($rowsArr2);

            fclose($temp); // this removes the file
            $i++; //counter
        }
           SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
    }

//end main

    function DBUpsert_sheet1($rowsArr) {

        //pop firs value off the array.
        $ColumnNamesArr = array_shift($rowsArr);


        //print('Count' . count($rowsArr)); //DEBUG
        //loop through each row on sheet
        foreach ($rowsArr as $key => $value) {

            //change to json
            $a = $this->zipEncode($ColumnNamesArr, $value);
            
            //trim JSON by decoding and encoding again. 
            $decoded=json_decode($a, true);
            $b=json_encode($decoded,JSON_HEX_APOS);

            //check if it is not the last row where they add a total. so always check for  
            $checkArr = json_decode($a, true);
            if (is_numeric($checkArr['Item Number'])) {

               // var_dump($a);
               // print('\n\r');


                //write valid line to DB with upsert.
                $query = "INSERT INTO batch.shurgain_invoice_sheet1 (data) VALUES ('$b') ON CONFLICT DO NOTHING;";
                $statement = $GLOBALS['pdo']->prepare($query);
                $inserted = $statement->execute();
            }
        }
    }

    function DBUpsert_sheet2($rowsArr) {

        //pop firs value off the array.
        $ColumnNamesArr = array_shift($rowsArr);

        //loop through each row on sheet
        foreach ($rowsArr as $key => $value) {

            //change to json
            $a = $this->zipEncode($ColumnNamesArr, $value);
            
             //trim JSON by decoding and encoding again. 
            $decoded=json_decode($a, true);
            $b=json_encode($decoded,JSON_HEX_APOS);
            
            //check if it is not the last row where they add a total. so always check for  
            $checkArr = json_decode($a, true);
            if (is_numeric($checkArr['Item #'])) {

               // var_dump($a);
               // print('\n\r');


                //write valid line to DB with upsert.
                $query = "INSERT INTO batch.shurgain_invoice_sheet2 (data) VALUES ('$b') ON CONFLICT DO NOTHING;";
                $statement = $GLOBALS['pdo']->prepare($query);
                $inserted = $statement->execute();
            }
        }
    }

//PhpSpreadsheet
    function loadSpreadSheetSheet($inputFileName, $sheetNum) {

        require '/var/www/int/vendor/autoload.php';

        /** Create a new Xls Reader  * */
        $reader = new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
        $reader->setReadDataOnly(true);
        $reader->setLoadAllSheets();

        $spreadsheet = $reader->load($inputFileName);
        $worksheet = $spreadsheet->getSheet($sheetNum);
        $rowsArr = $worksheet->toArray();

        return $rowsArr;
    }

//two arrays to json, one header, one data
    function zipEncode($a, $b) {
        $twoDArr = (array_map(null, $a, $b));

        //flatten array
        $outArr = array();
        foreach ($twoDArr as $key => $value) {
            $outArr[$value[0]] = $value[1];
        }

        return json_encode($outArr, JSON_HEX_APOS);
    }

    
      /* returns datetime of last DB update, thus assumes main function completed then */
    public function lastRun() {
        $sql = "SELECT max(update_time) as max_update_time FROM batch.shurgain_invoice_sheet1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return strtotime($row['max_update_time']);
    }
    
}
//end class

//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new ShurGainXlsxReader();
    $cls->main();
} else {
    //nothing
}