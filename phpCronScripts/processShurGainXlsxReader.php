<?php

/*
 * shurgain sends us a spreadsheet with 3 sheets in it. We only need 2nd and 3rd. They also put a total at bottom we need to ignore. 
 * The two sheets correlate by invoice number, but sort that out in the database later. 
 * 
 * copy new files into ../local/dataFile/shurGainInvoice then run script from CLI and it will insert into db. then look at shurgain view.
 * 
 */
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');

class ShurGainXlsxReader {

    public function __construct() {
        $this->main();
    }

    function main() {
        
        $filepath="../local/dataFile/shurGainInvoice/"; //hardcoded....bad.
		if ($handle = opendir($filepath)) {
			/* This is the correct way to loop over the directory. */
			while (false !== ($file = readdir($handle))) {
				if (strpos($file,'.xlsx') !==false) {
					$arrayOfFiles[]=$file;
				}
			}
			closedir($handle);
		}

		print("Doing the following files:\n");
		print_r($arrayOfFiles);

		//read file
		foreach ($arrayOfFiles as $arrayOfFiles_num => $fileName)
		{
        
        
        
        
        $inputFileName = '../local/dataFile/shurGainInvoice/' .$fileName;
       
        print("\n\r\n\r\n\r\n\r *** DOING $fileName ***\n\r\n\r\n\r\n\r");
        
         //insert both sheet 1st
         $rowsArr = $this->loadSpreadSheetSheet($inputFileName, 1);
         $this->DBUpsert_sheet1($rowsArr);
          
        //insert both sheet 2nd 
         $rowsArr2 = $this->loadSpreadSheetSheet($inputFileName, 2);
         $this->DBUpsert_sheet2($rowsArr2);
                }
        
    }

//end main

    function DBUpsert_sheet1($rowsArr) {

        //pop firs value off the array.
        $ColumnNamesArr = array_shift($rowsArr);

        //loop through each row on sheet
        foreach ($rowsArr as $key => $value) {

            //change to json
            $a = $this->zipEncode($ColumnNamesArr, $value);

            //check if it is not the last row where they add a total. so always check for  
            $checkArr = json_decode($a, true);
            if (is_numeric($checkArr['Item Number'])) {

                var_dump($a);
                print('\n\r');


                //write valid line to DB with upsert.
                $query = "INSERT INTO batch.shurgain_invoice_sheet1 (data) VALUES ('$a') ON CONFLICT DO NOTHING;";
                $res = $GLOBALS['pdo']->exec($query);
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

            //check if it is not the last row where they add a total. so always check for  
            $checkArr = json_decode($a, true);
            if (is_numeric($checkArr['Item #'])) {

                var_dump($a);
                print('\n\r');


                //write valid line to DB with upsert.
                $query = "INSERT INTO batch.shurgain_invoice_sheet2 (data) VALUES ('$a') ON CONFLICT DO NOTHING;";
               
                $res = $GLOBALS['pdo']->exec($query);
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
        $twoDArr=(array_map(null, $a, $b));
        
        //flatten array
        $outArr=array();
        foreach ($twoDArr as $key => $value) {
           $outArr[$value[0]]=$value[1]; 
        }
        
        return json_encode($outArr,JSON_HEX_APOS);
    }

 
    
    
}//end class

new ShurGainXlsxReader();
?>
