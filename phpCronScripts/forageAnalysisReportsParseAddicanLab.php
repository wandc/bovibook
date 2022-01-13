<?php

/**
  File to read Addican lab individual CSV files.
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
    $_SERVER['DOCUMENT_ROOT'] = dirname(__DIR__) . '/'; //when called from cli, command line define constant.
} else {
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
}

/*
 * This works from files manually copied into filePath directory. It was done this way because of how addican lab provides
 * many individual files. 
 * After its read into documents.forage_analysis_addican_lab table we then furth process to make it work with bag samples. 
 */

//lots of good forumelas here: http://www.agtest.com/articles/FEED%20AND%20FORAGES%20CALCULATIONS_new.pdf
class forageAnalysisReportsParseAddicanLab {

    public $filepath;

    public function __construct() {
        $this->filepath = "../local/dataFile/forageAnalysis/addicanLab/"; //hardcoded....bad.;
    }

    public function sse($request = null) {
        $this->main();
    }

    /*
     * process through files. 
     */

    public function main() {
        $this->one();
        $this->two();
    }

    /*
     * 
     */

    function one() {
        $arrayOfFiles = $this->readFileNames();
        $this->processFiles($arrayOfFiles);
    }

    private function readFileNames() {
    //load all the files in the directory.

        if ($handle = opendir($this->filepath)) {
            /* This is the correct way to loop over the directory. */
            while (false !== ($file = readdir($handle))) {
                //ignore stupid mac resource fork files. 
                if ((strpos($file, '.csv') !== false) AND ((substr($file, 0, 2) != '._' && $file != '.' && $file != '..'))) {

                    print("HI" . $file . " ");
                    $arrayOfFiles[] = $file;
                }
            }
            closedir($handle);
        }
        
        print_r($arrayOfFiles);

        return $arrayOfFiles;
    }

    //from: https://www.kodingmadesimple.com/2016/04/convert-csv-to-json-using-php.html
    function csvTojson($fileNameAndPath) {

        // open csv file
        if (!($fp = fopen($fileNameAndPath, 'r'))) {
            throw new Exception("Can't open file: $fileNameAndPath.");
        }

        //read csv headers
        $key = fgetcsv($fp, "8196", ",");

        // parse csv rows into array
        $json = array();
        ($row = fgetcsv($fp, "8196", ","));
        $json = array_combine($key, $row);

        $jsonStr = json_encode(($json));
        //$jsonStr=utf8_encode($jsonStr);
        //remove utf-8 BOM, terrible way to do it, tried the correct ways: mb_detect_encoding, etc. no luck.
        $jsonStr = str_replace('\ufeff', '', $jsonStr);

        return $jsonStr;
    }

    function processFiles($arrayOfFiles) {

        //read file
        foreach ($arrayOfFiles as $arrayOfFiles_num => $fileName) {

            $sql = "SELECT id,data,file_name FROM documents.forage_analysis_addican_lab WHERE file_name='$fileName'";
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if (empty($row['id'])) {
                //not in DB, do an insert of new data

                $dataJson = $this->csvTojson($this->filepath . $fileName);

                $query = "INSERT INTO documents.forage_analysis_addican_lab  (event_time,file_name,data,userid) VALUES ('now','$fileName','$dataJson','system')";
                $res = $GLOBALS['pdo']->exec($query);
            }
        }
    }

    //change from forage_analysis_addican_lab to 
    function two() {

        $sql = "SELECT id,data,file_name,data->>'Sample Number'::text as samplenum, data->>'Identification' as comment,data->>'Analysis Time' as sampletime FROM documents.forage_analysis_addican_lab ORDER BY file_name DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
        $stmt->execute();

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $data, PDO::PARAM_STR);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);

        SSE_Message::send_message(1, 'Starting.', 1); //sse

        $arrayCount = ($count = $stmt->rowCount());
        $i = 0;
        while ($lines = $stmt->fetch(PDO::FETCH_ASSOC)) {

            SSE_Message::send_message(6 + $i, 'Doing around ' . $fileName . $i . ' of ' . $arrayCount, 6 + round(($i / $arrayCount) * 100, 1)); //sse
            //Process a file. 



            $sql = "SELECT sample_number FROM nutrition.bag_analysis WHERE lab_name='addican' AND sample_number='{$lines['samplenum']}'";
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            if ($row == null) {  //ie not already in db, then proceed with insert
                //decode json to array....
                $json = json_decode($lines['data'], true);

                //do NRC 2001 template
                $rowArrayNRCBasic = $this->generateNRC2001ValuesFromRawValues($json);

                print("\n\r\n\r" . "Doing INSERT of " . $lines['samplenum'] . " " . $lines['comment'] . " \n\r\n\r");

                //var_dump($rowArray);
                //add things to raw report.
                $rowArrayBasic['comment'] = $json['Identification'];

                //create info template.
                $infoArr['DM'] = (float) $json['DM_AI'];
                // $infoArr['RFQ'] = (float) $json['RFQ'];
                // $infoArr['milk_per_tonne'] = (float) $json['milk per tonne from forage (kg/t)'];
                $infoArr['TDN'] = (float) $json['TDN'];
                $infoArr['CP'] = (float) $json['Protein'];
                $infoArr['Ca'] = (float) $json['Calcium'];
                $infoArr['P'] = (float) $json['Phosphorus'];
                $infoArr['K'] = (float) $json['Potassium'];
                $infoArr['ADF'] = (float) $json['ADF'];
                $infoArr['NDF'] = (float) $json['NDF'];
                $infoArr['lignin'] = (float) $json['LIGNIN'];
                $infoArr['ash'] = (float) $json['Ash'];
                $infoArr['starch'] = (float) $json['Starch'];
                //encode json to store in db. 
                $rawReportJson = json_encode($json, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $NRCJson = json_encode($rowArrayNRCBasic, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
                $infoJson = json_encode($infoArr, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);

                if (empty($json['Analysis Time'])) {
                    var_dump($json);
                    throw new Exception('File Has no valid analysis time, sample data is missing or corrupt in file name: ' . $fileNameAndPath);
                }


                //change the datetime inside the json to something sane. 
                //format is:  8/4/2021 12:14:32 PM
                //format is:  n/j/Y H:i:s A
                $date = DateTime::createFromFormat('n/j/Y H:i:s A', $json['Analysis Time']);
                if (($date) == false) {
                    //some files use date format of: 11/4/2021 17:08 instead
                    $date = DateTime::createFromFormat('n/j/Y H:i', $json['Analysis Time']);
                }
                if (($date) == false) {
                    throw new Exception('Error: Unknow date format.');
                }
                $json['Analysis Time2'] = $date->format('Y-m-d H:i:s'); //2015-03-26 11:39:59;

                $analysisTime = ($json["Analysis Time2"]);

                //now insert into db.
                $sql = "INSERT INTO nutrition.bag_analysis (bag_id,footage,sample_number,sample_date,userid, report_file, lab_name, raw_report	,nrc2001_template,info_template) VALUES (null,null,'{$json['Sample Number']}','{$analysisTime}','system',null,'addican','$rawReportJson','$NRCJson','$infoJson')";

                $res = $GLOBALS['pdo']->exec($sql);
            } else {
                print("\n\r\n\r" . "SKIPPING, already in: {$lines['samplenum']}" . " \n\r\n\r");
            }
        }
        $i++;

        SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
    }

//end function
//end function
    //turns parsed raw data into something nrc can use to overlay other forage types.
    function generateNRC2001ValuesFromRawValues($rowArray) {


        $outArray['TDN (%DM)'] = (float) $rowArray['TDN'];
        $outArray['DE (Mcal/kg)'] = (float) $rowArray['NEL']; //CHECK THIS. 
        $outArray['DM (%AF)'] = (float) $rowArray['DM_AI'];  //verified true with shurgain
        $outArray['NDF (%DM)'] = (float) $rowArray['NDF'];
        $outArray['ADF (%DM)'] = (float) $rowArray['ADF'];
        $outArray['Lignin (%DM)'] = (float) $rowArray['LIGNIN'];
        $outArray['CP (%DM)'] = (float) $rowArray['Protein'];

        $outArray['NDFIP (%DM)'] = $rowArray['NDICP (%CP)']; //NDF insolable protein, around 2. //TAREK: measures damaged and tightly bound protein that is not available for digestion and animal use. ADICP is defined as ADF bound protein.
        $outArray['ADFIP (%DM)'] = $rowArray['ADICP_AI']; //ADF insoluable protein, around 1.8 //Tarek: measures protein that is bound to the NDF fraction. //NOTE: migh be wet value, need to check, maybe mutiply by DM. 

        /* not sure what this is */
        $outArray['Prt-A (%CP)'] = null; //(float) $rowArray['A FRACTION'];  //Adican: needs to be done through wet chemistry. 
        $outArray['Prt-B (%CP)'] = null; //(float) round(( 100 - ($rowArray['A FRACTION'] + $rowArray['B FRACTION'])), 2);
        $outArray['Prt-C (%CP)'] = null; //(float) $rowArray['B FRACTION'];
        //
        $outArray['Kd (1/hr)'] = null; // (float) $rowArray['K2']; //addican knows what it is. 
        $outArray['RUP Digest (%)'] = (float) $rowArray['Protein'] - (float) $rowArray['SOL_PROTEIN']; //check if soluable protein is degradable protein. 
        $outArray['Fat (%DM)'] = (float) $rowArray['Fat'];
        $outArray['Ash (%DM)'] = (float) $rowArray['Ash'];
        //
        $outArray['Sodium'] = (float) $rowArray['Sodium'];
        $outArray['Calcium'] = (float) $rowArray['Calcium'];
        $outArray['Phosphorus'] = (float) $rowArray['Phosphorus'];
        $outArray['Magnesium'] = (float) $rowArray['Magnesium'];
        $outArray['Potassium'] = (float) $rowArray['Potassium'];
        $outArray['Sulphur'] = (float) $rowArray['Sulfur'];

        return $outArray;
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
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new forageAnalysisReportsParseAddicanLab();
    $cls->main();
} else {
    //nothing
}
?>