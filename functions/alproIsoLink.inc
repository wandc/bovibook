<?php 
include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
/**
 * This class is responsible for writing files to the alpro windows computer and reading files back. The alrpo sync program has to 
 * setup so that when a file is automatically copied to its directory it is read and sent to the processor and then renamed by adding an "X".
 * This class should do all the error checking to make sure that any errors coming back over the link are caught. ie that the data didn't make it into the alpro processor. This class should not be parsing the commands coming in to see if they are valid, or very basic validation.
 */
class AlproIsoLink {

    const ISO_LOG_FILE = '/mnt/share/IsoLog.txt';
    const ISO_IN_FILE = '/mnt/share/xxx.txt';
    const ISO_OUT_FILE = '/mnt/share/isoout.txt';
    const ISO_ROOT_DIR_PATH = '/mnt/share';
    const ISO_EXE_FILE = 'Iso32.exe'; //no slashes
    const FILE_LOOP_WAIT_US = 100000; //micro seconds, so .12 seconds  //can't seem to be shoter because of upgrade to alpro 7.0, sync runs slower.
    const FILE_LOOP_MAX_NUM = 300; //times to loop before timoute.

    //this is all that is visible when we are done.

    public $logArray = false;
    public $outArray = false;
    public $inArray = false;

    public function __construct($isoCommandStr) {

        //run alpro stuff
        $this->checkFilePathMounted(); //check that the nfs mount is up
        $this->deleteIsoLinkFiles(); //delete files from previous runs.
        $ret = "";
        $ret = $this->createIsoLinkHeader($ret);
        $ret = $this->createCommand($ret, $isoCommandStr);
        $ret = $this->createIsoLinkFooter($ret);
        $this->inArray = preg_split('/\n/', $ret); //store as array
        $this->writeToIsoLink($ret);    //write STRING to outfile.
        $this->logArray = $this->readIsoLogFile(); //read this one first to see if there is an error.
        //only look for outfile if RN command was sent, else skip it.
        if ($this->countNumberOfTwoLetterIsoCommands($isoCommandStr, 'RN') >= 1) {
            $this->outArray = $this->readIsoOutFile();
        }
    }

    public function __destruct() {
        $this->deleteIsoLinkFiles(); //delete files from this run.
    }

    /**
     * checks to make sure file path is mounted to alpro dir
     */
    private function checkFilePathMounted() {

        $filename = self::ISO_ROOT_DIR_PATH . '/' . self::ISO_EXE_FILE;

        if (!file_exists($filename)) {
            throw new Exception('Fatal Error: NFS share of alpro directory is not running OR ISO32.exe file does not exist OR you don\'t have permission to access the directory.' . "\r\n");
        }
        return 1;
    }

    /**
     * checks the incoming command for a bunch of things that could cause errors.
     */
    private function createCommand($ret, $isoCommandStr) {

        $counterRN = $this->countNumberOfTwoLetterIsoCommands($isoCommandStr, 'RN');
        if ($counterRN >= 2) {
            throw new Exception('multiple RN lines in input file. Class only supports one at this time.');
        }

        return $ret . $isoCommandStr;
    }

//count the number of two letter commands in an iso infile. this allows us to usally look for number of RN commands.
    private function countNumberOfTwoLetterIsoCommands($isoCommandStr, $twoLetterCommand = 'RN') {
//convert the command string into an array of lines.
        $commandArr = preg_split('/\n|\r/', $isoCommandStr);

//check that 'RN' only occurs once, this does not support more than one occurence at the current time.
        $counter = 0;
        foreach ($commandArr as $key => $value) {
            if (substr($value, 0, 2) == $twoLetterCommand) {
                $counter++;
            }
        }
        return $counter;
    }

    private function createIsoLinkHeader($ret = "") {
//maybe a different data dictionary is needed in the future.
        $ret = $ret . 'DH990001000000000800090000624000900008080009000111000090000208000900012080009000120800090000308000900004060' . "\r\n";
        $ret = $ret . 'VH990001DD      DeLaval International AB664     ALPRO     2000    ALPRO_DD660     20071116141741' . "\r\n";
        $ret = $ret . 'CN-----START----------' . "\r\n";
        return $ret;
    }

    private function createIsoLinkFooter($ret) {
        $ret = $ret . 'CN-----END-----------' . "\r\n";
        $ret = $ret . 'ZN' . "\r\n";
        return $ret;
    }

    private function deleteIsoLinkFiles() {
        $myFile1 = self::ISO_LOG_FILE;
        $myFile2 = self::ISO_IN_FILE;
        $myFile3 = self::ISO_IN_FILE . "X"; //when file is completed and X is added
        $myFile4 = self::ISO_OUT_FILE;
        if (file_exists($myFile1) == true) {
            unlink($myFile1);
        }
// wait for 2 seconds
        usleep(500000);
        if (is_file($myFile2) == true) {
            unlink($myFile2);
        }
        if (file_exists($myFile3) == true) {
            unlink($myFile3);
        }
        if (file_exists($myFile4) == true) {
            unlink($myFile4);
        }
    }

    private function writeToIsoLink($ret) {
        $myFile = self::ISO_IN_FILE;
        $fh = fopen($myFile, 'w') or die("can't open file");
        fwrite($fh, $ret);
        fclose($fh);
    }

    private function readIsoOutFile() {
        $myFile = self::ISO_OUT_FILE;

//check if file exists
        $fileContents = false;
        $loopMaxNum = self::FILE_LOOP_MAX_NUM;
        $loopNum = 0;
        while ($fileContents == false & $loopNum <= $loopMaxNum) {
            if (file_exists($myFile) == true) {
//print("DEBUG: Out file loop\r\n");
//check to see if file is bust, if it is not continue on.
                if (!@file($myFile)) {
                    //do nothing here, supresses the error that the file is busy, stored in: $php_errormsg 
                } else {
                    //read file
                    $file_array = file($myFile);

                    //since excel mode is active we need to breakup the tab seperated lines into different 2d array components.
                    foreach ($file_array as $key => $value) {
                        $file_array[$key] = explode("\t", $value, -1); //don't return the last token, this will be blank because there is an extra \t if we did return it.
                        //fix numbers so they lose leeding zero.
                        foreach ($file_array[$key] as $key2 => $value2) {
                            if (is_numeric($value2) == true) {
                                $file_array[$key][$key2] = $value2 + 0; //hack to trim off leading 0's
                            }
                        }
                    }

                    return $file_array;  //return $file_array; //good result
                }
            }
            $loopNum++; //increase counter
// wait for .1 seconds
//print("Read Out File Wait Loop #$loopNum.   <br/>"."\n\r"); //DEBUG
            usleep(self::FILE_LOOP_WAIT_US);
        }
        throw new Exception('ISO out file not read.');
        
    }

    private function readIsoLogFile() {
        $myFile = self::ISO_LOG_FILE;

//check if file exists
        $fileContents = false;
        $loopMaxNum = self::FILE_LOOP_MAX_NUM;
        $loopNum = 0;
        while ($fileContents == false & $loopNum <= $loopMaxNum) {
            if (file_exists($myFile) == true) {
//print("DEBUG: Log file loop\r\n");
//check to see if file is bust, if it is not continue on.
                if (!@file($myFile)) {
                    //do nothing here, supresses the error that the file is busy, stored in: $php_errormsg 
                } else {
                    $file_array = file($myFile);
                    //DEBUG: print_r($file_array); //DEBUG
                    //search log for the word "Error"
                    if (strstr(implode("\r\n", $file_array), 'Error') != false) {
                        throw new Exception('Error found to be recorded in iso log file.');
                    }

                    return $file_array; //good result
                }
            }
            $loopNum++; //increase counter
// wait for .1 seconds
//this is needed, because it seems the windows iso32 program doesn't see that the file was written.
//The looking for a new file loop time of the alpro sync program should happen more often, seems to be every 30 seconds or so now.
//this got worse with version 7.
//print("Read Log File Wait Loop #$loopNum.   <br/>"."\n\r"); //DEBUG
            usleep(self::FILE_LOOP_WAIT_US);
        }
        throw new Exception('ISO log file not read.' . "\n\r");
        return 0; //failed
    }

    /**
     * This method writes to the postgres db table what tables are synced with alpro.
     * even if alpro command failed, this is still used to log
     */
    public static function writeSync($schema, $table, $state, $inResult, $outResult, $logResult, $db) {


        //fix the strings for inserts into db.
        if ($logResult != null) {
            $logResult = pg_escape_string(utf8_decode(implode($logResult))); //convert array to string.
        }
        if ($outResult != null) {
            $outFlat = self::array_flatten_sep('|', $outResult);
            $outResult = pg_escape_string(utf8_decode(implode($outFlat))); //convert array to string.
        }
        if ($inResult != null) {
            $inResult = pg_escape_string(utf8_decode(implode($inResult))); //convert array to string.
        }
        try {
            //do an insert into sync table
            $query = "INSERT INTO alpro.sync (db_schema,db_table,event_time,state,debug_in,debug_out,debug_log) VALUES ('$schema','$table','now',$state,'$inResult','$outResult','$logResult')";
            $res = $GLOBALS['pdo']->exec($query);
        } catch (PDOException $e) {
            print $e->getMessage();
        }
    }

    function handlePearError($err) {
        echo("<b>PEAR error</b><br>message: <i>"
        . $err->getMessage()
        . "</i><br>user info: <i>"
        . $err->getUserInfo()
        . "</i><br>backtrace: <i><br>");
        $arr = $err->backtrace;
        foreach ($arr as &$value) {
            print($value['line'] . ' - ' . $value['file'] . '<br>');
        }
        print('<br>');
//print the pretty error.
//print_r($err);
        echo("</i><br>");

        exit;
        die();
    }

//from http://php.net/manual/en/function.array-values.php
    function array_flatten_sep($sep, $array) {
        $result = array();
        $stack = array();
        array_push($stack, array("", $array));

        while (count($stack) > 0) {
            list($prefix, $array) = array_pop($stack);

            foreach ($array as $key => $value) {
                $new_key = $prefix . strval($key);

                if (is_array($value))
                    array_push($stack, array($new_key . $sep, $value));
                else
                    $result[$new_key] = $value;
            }
        }

        return $result;
    }

}

//end class
?>