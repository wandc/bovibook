<?php 
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');

error_reporting(E_ALL);
ini_set('display_errors', 1);

/**
 * Reads through odbc link to access db information from alrpo.
 * NOTE DB password is: 61016622 , not needed here, but for debugging.
 * NOTE2: the milking start times are only accurate to nearest minute. Alpro 
 * windows has them to the nearest second. I can't find where they are actually stored in the db.
 */
class AlproArchive {

    function main() {


        exec('mount /mnt/share2'); //must be run as root, but just an extra check that the file is mounted.
//fist copy file to tmp to get over stupid cant alloc file bug...
//FIXME change this whole thing to iso protocol. We lose info if we do....
        $myFile1 = '/tmp/waddy.apw';
        if (file_exists($myFile1) == true) {
            unlink($myFile1);
        }
        copy('/mnt/share2/waddy.apw', $myFile1);


//Now uses PDO ODBC connection, stilled uses MDBTools
        try {
            $mdb_file = '/mnt/share2/waddy.apw';
            $driver = 'MDBTools';
            $dsn = "odbc:Driver=$driver;DBQ=$mdb_file;";
            $dbh = new PDO($dsn);
            $lastUpdateTime = self::getLastDBUpdateTime($dbh); //get alpro db last write time.
        } catch (PDOException $e) {
            $dbh = null;
            print "Alpro ODBC PDO Connection Error!: " . $e->getMessage() . "<br/>";
            die();
        }

//check that we are not +/- 5 mins from midnight, which is the alpro db dayshift time, because if we are then, thet data might not be valid.
        if ((self::midnight_seconds($lastUpdateTime) <= 300) || (self::midnight_seconds($lastUpdateTime) >= (86400 - 300))) {
            print("Within +/-5 mins midnight, doing nothing for safety sake.   <br/>\n\r");
        } else {

//get data from "tblcowb", this contatins mostly auxillary milking data.
            $sql = ("SELECT CowNo,
ManualDetatchToday1, ManualKeyToday1,OverrideKeyToday1,ReattatchToday1,IDTimeTodayMM1,IDTimeTodaySS1 , MilkTimeTodaySS1, MilkFlow0_15_Today1,MilkFlow15_30_Today1, 
MilkFlow30_60_Today1,MilkFlow60_120Today1,LowMilkFloPercToday1,TakeOffFlowToday1,BatchNoToday1,NoMilkTodaySession1,
ManualDetatchToday2, ManualKeyToday2,OverrideKeyToday2,ReattatchToday2,IDTimeTodayMM2, IDTimeTodaySS2 , MilkTimeTodaySS2,MilkFlow0_15_Today2 , MilkFlow15_30_Today2,
 MilkFlow30_60_Today2, MilkFlow60_120Today2,LowMilkFloPercToday2, TakeOffFlowToday2, BatchNoToday2, NoMilkTodaySession2,
ManualDetatchYester1, ManualKeyYester1,OverrideKeyYester1,ReattatchYester1, IDTimeYesterMM1, IDTimeYesterSS1, MilkTimeYesterSS1, MilkFlow0_15_Yester1, MilkFlow15_30_Yester1,
 MilkFlow30_60_Yester1, MilkFlow60_120Yester1,  LowMilkFloPercYester1, TakeOffFlowYester1, BatchNoYesterday1, NoMilkYestSession1,
ManualDetatchYester2 ,ManualKeyYester2,OverrideKeyYester2,ReattatchYester2,IDTimeYesterMM2, IDTimeYesterSS2 ,MilkTimeYesterSS2, MilkFlow0_15_Yester2 , MilkFlow15_30_Yester2,
 MilkFlow30_60_Yester2 , MilkFlow60_120Yester2 , LowMilkFloPercYester2, TakeOffFlowYester2 , BatchNoYesterday2 , NoMilkYestSession2 
 FROM tblcowb
 ");

            try {
                $statement = $dbh->prepare($sql);
                $statement->execute();
                $dataArr = $statement->fetchAll(PDO::FETCH_GROUP | PDO::FETCH_ASSOC);
                $statement = null;
                printf("Result array of tblcowb BBBBB contains %d rows\n", count($dataArr));
            } catch (Exception $e) {
                die("SELECT failed for tblcowb: " . $e);
            }


//get data from main table "tblcow"
            $sql222 = ("SELECT CowNo,MilkToday1,MilkToday2,MilkYesterday1,MilkYesterday2,Duration1,Duration2,DurationY1,DurationY2,
MilkTimeToday1,MilkTimeToday2,MilkTimeYesterd1,MilkTimeYesterd2,
PeakFlow1, AverFlow1, Duration1, MPCToday1, ManualIDToday1, 
PeakFlow2,  AverFlow2,  Duration2, MPCToday2, ManualIDToday2,
PeakFlowY1, AverFlowY1, DurationY1, MPCYesterday1, ManualIDYest1, 
PeakFlowY2, AverFlowY2, DurationY2, MPCYesterday2,  ManualIDYest2  
 FROM tblcow
 ");

            try {
                $statement = $dbh->prepare($sql222);
                $statement->execute();
                $dataArr2 = $statement->fetchAll(PDO::FETCH_GROUP | PDO::FETCH_ASSOC); //this is solely here to to a row count on next line, must be better way?
                printf("Result array of tblcow COWCOW contains %d rows\n", count($dataArr2));
            } catch (Exception $e) {
                die("SELECT failed for tblcow: " . $e);
            }
            if (count($dataArr2) == 0) {
                throw new Exception("Table is returning 0 rows. Check if Alpro Sync program down.");
            }


            $statement->execute();
            while ($row = $statement->fetch(PDO::FETCH_ASSOC)) {
                $cowno = $row['CowNo'];
                print("#" . $cowno . " ");
                //print_r($dataArr[$cowno][0]); //for debugging.
                //print_r($row); //for debugging.
                //

 
 //find out the bovine_id for the current cow
                $sql = "SELECT id FROM bovineManagement.bovinecurr WHERE local_number={$row['CowNo']} LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);

                if (($row2 != null)) { //only go through this if cow number exists.
                    //get data
                    $bovine_id = $row2['id'];


                    //try and insert data for today first milking
                    $date = date("Y-m-d", strtotime($lastUpdateTime));
                    try {
                        self::insertMilkData($bovine_id, $date, 1, $row['MilkToday1'], $row['Duration1'], $row['MilkTimeToday1'], $row['PeakFlow1'], $row['AverFlow1'], $row['MPCToday1'], $row['ManualIDToday1'], $dataArr[$cowno][0]['ManualDetatchToday1'], $dataArr[$cowno][0]['ManualKeyToday1'], $dataArr[$cowno][0]['OverrideKeyToday1'], $dataArr[$cowno][0]['ReattatchToday1'], $dataArr[$cowno][0]['IDTimeTodayMM1'], $dataArr[$cowno][0]['IDTimeTodaySS1'], $dataArr[$cowno][0]['MilkTimeTodaySS1'], $dataArr[$cowno][0]['MilkFlow0_15_Today1'], $dataArr[$cowno][0]['MilkFlow15_30_Today1'], $dataArr[$cowno][0]['MilkFlow30_60_Today1'], $dataArr[$cowno][0]['MilkFlow60_120Today1'], $dataArr[$cowno][0]['LowMilkFloPercToday1'], $dataArr[$cowno][0]['TakeOffFlowToday1'], $dataArr[$cowno][0]['BatchNoToday1'], $dataArr[$cowno][0]['NoMilkTodaySession1']); //do an insert of the 
                    } catch (Exception $e) {
                        echo "\n\nSkipping Insert 1 for $cowno Caught exception: ", $e->getMessage(), "\n\n";
                    }
//
                    try {
                        self::insertMilkData($bovine_id, $date, 2, $row['MilkToday2'], $row['Duration2'], $row['MilkTimeToday2'], $row['PeakFlow2'], $row['AverFlow2'], $row['MPCToday2'], $row['ManualIDToday2'], $dataArr[$cowno][0]['ManualDetatchToday2'], $dataArr[$cowno][0]['ManualKeyToday2'], $dataArr[$cowno][0]['OverrideKeyToday2'], $dataArr[$cowno][0]['ReattatchToday2'], $dataArr[$cowno][0]['IDTimeTodayMM2'], $dataArr[$cowno][0]['IDTimeTodaySS2'], $dataArr[$cowno][0]['MilkTimeTodaySS2'], $dataArr[$cowno][0]['MilkFlow0_15_Today2'], $dataArr[$cowno][0]['MilkFlow15_30_Today2'], $dataArr[$cowno][0]['MilkFlow30_60_Today2'], $dataArr[$cowno][0]['MilkFlow60_120Today2'], $dataArr[$cowno][0]['LowMilkFloPercToday2'], $dataArr[$cowno][0]['TakeOffFlowToday2'], $dataArr[$cowno][0]['BatchNoToday2'], $dataArr[$cowno][0]['NoMilkTodaySession2']); //do an insert of the 
                    } catch (Exception $e) {
                        echo "\n\nSkipping Insert 2 for $cowno Caught exception: ", $e->getMessage(), "\n\n";
                    }
//////////////////////////////////////// 
                    //try and insert data for yesterday
                    try {
                        $dateY = date("Y-m-d", strtotime("$lastUpdateTime -1 day"));
                        self::insertMilkData($bovine_id, $dateY, 1, $row['MilkYesterday1'], $row['DurationY1'], $row['MilkTimeYesterd1'], $row['PeakFlowY1'], $row['AverFlowY1'], $row['MPCYesterday1'], $row['ManualIDYest1'], $dataArr[$cowno][0]['ManualDetatchYester1'], $dataArr[$cowno][0]['ManualKeyYester1'], $dataArr[$cowno][0]['OverrideKeyYester1'], $dataArr[$cowno][0]['ReattatchYester1'], $dataArr[$cowno][0]['IDTimeYesterMM1'], $dataArr[$cowno][0]['IDTimeYesterSS1'], $dataArr[$cowno][0]['MilkTimeYesterSS1'], $dataArr[$cowno][0]['MilkFlow0_15_Yester1'], $dataArr[$cowno][0]['MilkFlow15_30_Yester1'], $dataArr[$cowno][0]['MilkFlow30_60_Yester1'], $dataArr[$cowno][0]['MilkFlow60_120Yester1'], $dataArr[$cowno][0]['LowMilkFloPercYester1'], $dataArr[$cowno][0]['TakeOffFlowYester1'], $dataArr[$cowno][0]['BatchNoYesterday1'], $dataArr[$cowno][0]['NoMilkYestSession1']); //do an insert of the milking data for the current cow.
                    } catch (Exception $e) {
                        echo "\n\nSkipping Insert 3 for $cowno Caught exception: ", $e->getMessage(), "\n\n";
                    }
                    //
                    try {
                        self::insertMilkData($bovine_id, $dateY, 2, $row['MilkYesterday2'], $row['DurationY2'], $row['MilkTimeYesterd2'], $row['PeakFlowY2'], $row['AverFlowY2'], $row['MPCYesterday2'], $row['ManualIDYest2'], $dataArr[$cowno][0]['ManualDetatchYester2'], $dataArr[$cowno][0]['ManualKeyYester2'], $dataArr[$cowno][0]['OverrideKeyYester2'], $dataArr[$cowno][0]['ReattatchYester2'], $dataArr[$cowno][0]['IDTimeYesterMM2'], $dataArr[$cowno][0]['IDTimeYesterSS2'], $dataArr[$cowno][0]['MilkTimeYesterSS2'], $dataArr[$cowno][0]['MilkFlow0_15_Yester2'], $dataArr[$cowno][0]['MilkFlow15_30_Yester2'], $dataArr[$cowno][0]['MilkFlow30_60_Yester2'], $dataArr[$cowno][0]['MilkFlow60_120Yester2'], $dataArr[$cowno][0]['LowMilkFloPercYester2'], $dataArr[$cowno][0]['TakeOffFlowYester2'], $dataArr[$cowno][0]['BatchNoYesterday2'], $dataArr[$cowno][0]['NoMilkYestSession2']);
                    } catch (Exception $e) {
                        echo "\n\nSkipping Insert 4 for $cowno Caught exception: ", $e->getMessage(), "\n\n";
                    }

                    //self::insertMilkData($bovine_id,$dateY,2,$row['MilkYesterday2'],$row['DurationY2'],$row['MilkTimeYesterd2'],$row['PeakFlowY2'], $row['AverFlowY2'], $row['MPCYesterday2'],  $row['ManualIDYest2']  ); //do an insert of the milking data for the current cow.
                }
            }
        }//end midnight else
        print("Script Finished." . "<br>");
    }

//end function

    /**
     * Only insert milking data when some milking data exists and only when a record doesn't previously exist.
     */
    function insertMilkData($bovine_id, $date, $milking_number, $milkyield, $duration, $milkTime, $peakFlow, $averFlow, $MPCnumber, $ManualID, $ManualDetatch, $ManualKey, $OverrideKey, $Reattatch, $IDTimeMM, $IDTimeSS, $milkTimeSS, $MilkFlow0_15, $MilkFlow15_30, $MilkFlow30_60, $MilkFlow60_120, $LowMilkFloPerc, $TakeOffFlow, $BatchNo, $NoMilk) {

        if ((is_numeric($milkyield) == TRUE) && ($milkyield != '') && ($milkyield != 0)) { //if we have a milk yield try and insert the data.
//correct the duration to be a time interval. MDB files seem to use 12/30/99 for this?????
            if ($duration != '') {
                $durationTimeInt = date("H:i:s", strtotime($duration)); //returns the time interval (ignores date portion).
            } else { //sometimes there is no duration for some reason....
                $durationTimeInt = '00:00:00';
            }


//convert from int to boolean
            $ManualID = self::convertToBoolean($ManualID);
            $ManualDetatch = self::convertToBoolean($ManualDetatch);
            $ManualKey = self::convertToBoolean($ManualKey);
            $OverrideKey = self::convertToBoolean($OverrideKey);
            $Reattatch = self::convertToBoolean($Reattatch);

//convert values to zero

            if ($averFlow == '') {
                $averFlow = 0;
            }
            if ($MilkFlow0_15 == '') {
                $MilkFlow0_15 = 0;
            }
            if ($MilkFlow15_30 == '') {
                $MilkFlow15_30 = 0;
            }
            if ($MilkFlow30_60 == '') {
                $MilkFlow30_60 = 0;
            }
            if ($MilkFlow60_120 == '') {
                $MilkFlow60_120 = 0;
            }
            if ($LowMilkFloPerc == '') {
                $LowMilkFloPerc = 0;
            }
            if ($TakeOffFlow == '') {
                $TakeOffFlow = 0;
            }
            if ($BatchNo == '') {
                $BatchNo = 0;
            }
            if ($NoMilk == '') {
                $NoMilk = 0;
            }
            if ($IDTimeSS == '') {
                $IDTimeSS = 0;
            } //why is this zero???
            if ($milkTimeSS == '') {
                $milkTimeSS = 0;
            } //why is this zero???
//correct the milking time to the proper date.
            $milkTimeDate = date('c', mktime(date("H", strtotime($milkTime)), date("i", strtotime($milkTime)), $milkTimeSS, date("n", strtotime($date)), date("j", strtotime($date)), date("Y", strtotime($date))));
            $IDTimeMMFixed = date('c', mktime(date("H", strtotime($IDTimeMM)), date("i", strtotime($IDTimeMM)), $IDTimeSS, date("n", strtotime($date)), date("j", strtotime($date)), date("Y", strtotime($date))));


//do a better check, we seem to have problems with data from today being the same as yesterday.
//so make the assumption if yield and duration and milking start time are the same as the day before that something is wrong.
// statistical chance is very low of a false positive.
            $yesterdayDate = date('Y-m-d', strtotime("$date - 1 day"));
            $sql4 = "SELECT milkyield,duration,peakflow,mpcnumber FROM alpro.cow WHERE bovine_id=$bovine_id AND date='$yesterdayDate'  AND milking_number=$milking_number LIMIT 1";
            $res4 = $GLOBALS['pdo']->query($sql4);
            $row4 = $res4->fetch(PDO::FETCH_ASSOC);
            if (($row4['milkyield'] == $milkyield) AND ($row4['duration'] == $durationTimeInt) AND ($row4['peakflow'] == $peakFlow) AND ($row4['mpcnumber'] == $MPCnumber)) {
                throw new Exception("Current data for $bovine_id is equal to yesterday's data for that milking, error.");
            }


            //look to see if data was already put in.
            $sql = "SELECT bovine_id FROM alpro.cow WHERE bovine_id=$bovine_id AND date='$date' AND milking_number=$milking_number LIMIT 1";
            $res3 = $GLOBALS['pdo']->query($sql);
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);
            if ($row3 == null) { //then no records exist and we can do an insert
                $query = "INSERT INTO alpro.cow (bovine_id,date,milking_number,milkyield,duration,milktime,peakflow,averflow,mpcnumber,manualid,"
                        . "manualdetatch,manualkey,overridekey,reattatch,idtimemm,idtimess,milktimess,milkflow0_15,milkflow15_30,milkflow30_60,milkflow60_120,lowmilkfloperc,takeoffflow,batchno,nomilk) VALUES "
                        . "($bovine_id,'$date',$milking_number,$milkyield,'$durationTimeInt','$milkTimeDate',$peakFlow,$averFlow,$MPCnumber,$ManualID,
 $ManualDetatch,$ManualKey,$OverrideKey,$Reattatch,'$IDTimeMMFixed',$IDTimeSS,$milkTimeSS,$MilkFlow0_15,$MilkFlow15_30,$MilkFlow30_60,$MilkFlow60_120,$LowMilkFloPerc,$TakeOffFlow,$BatchNo,$NoMilk)";
                unset($statement);
                $statement = $GLOBALS['pdo']->prepare($query);
                $statement->execute();
                return 1;
            } else {
                return 0; //didn't do anything
            }
        } else {
            return 0; //didn't do anything
        }
    }

//convert int to boolean
    function convertToBoolean($potBoolean) {
        if ($potBoolean == 1) {
            return 'TRUE';
        } else {
            return 'FALSE';
        }
    }

    /** method to get when the alpro db was last written to *
      /* This is used to as our date basis */
    function getLastDBUpdateTime($dbh) {
//get data from "tbldatamgr", this contatins data on the most recent update to db.
        /** NOTE: THe dayshift time is assumed to be 0:00 (midnight). */
        $sql = ("SELECT UpdatedDate,UpdatedTime FROM tbldatamgr");
        $statement = $dbh->prepare($sql);
        $statement->execute();
        $row = $statement->fetch(PDO::FETCH_ASSOC);
        $lastUpdateTime = date('Y-m-d', strtotime($row['UpdatedDate'])) . ' ' . date('H:i:s', strtotime($row['UpdatedTime']));
        return $lastUpdateTime;
    }

//from: http://www.perturb.org/display/Perlfunc__Seconds_Since_Midnight.html
    function midnight_seconds($lastUpdateTime) {
        $secs = (date("G", strtotime($lastUpdateTime)) * 3600) + (date("i", strtotime($lastUpdateTime)) * 60) + date("s", strtotime($lastUpdateTime));
        return $secs;
    }

    /**
     * prints error if alpro sync has not occured for one day.
     * Called from movement sort gate for now, no better place.    
     */
    function errorAlproSync() {

        $error = array();

        //since these errors are so important, put the time to now and level to 99.
        $now = date('r', strtotime("now"));

//check if sync didn't occur at all.  
        $query = "SELECT event_time FROM alpro.sync WHERE event_time >= (now() - interval '1 day')";
        $res = $GLOBALS['pdo']->query($query);
        if ($res->rowCount() == 0) {

            $query2 = "SELECT max(event_time) as event_time_max FROM alpro.sync";
            $res2 = $GLOBALS['pdo']->query($query2);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $error[] = new notifyObj(get_class($this), $now, 1, " ERROR: Alpro ISO data not synced since " . date('M d, Y G:i', strtotime($row2['event_time_max'])));
        }

//check if sync tried to occur but wasn't sucessful.
        $query3 = "SELECT event_time,state FROM alpro.sync WHERE event_time = (SELECT max(event_time) FROM alpro.sync) LIMIT 1";
        $res3 = $GLOBALS['pdo']->query($query3);
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);
        if ($row3['state'] != 't') {
            $error[] = new notifyObj(get_class($this), $now, 1, "ERROR: Alpro tried to ISO sync, but was unsuccessful since " . date('M d, Y G:i', strtotime($row3['event_time'])));
        }

//check if milk data download occured.
        $query4 = "SELECT max(date) as max_date FROM alpro.cow LIMIT 1";
        $res4 = $GLOBALS['pdo']->query($query4);
        $row4 = $res4->fetch(PDO::FETCH_ASSOC);
        if (strtotime($row4['max_date']) < strtotime("now - 26 hours")) {
            $error[] = new notifyObj(get_class($this), $now, 1, " ERROR: Alpro milking ODBC data is out of sync since " . date('M d, Y G:i', strtotime($row4['max_date'])));
        }

        return $error;
    }

//end function
}

//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new AlproArchive();
    $cls->main();
} else {
    /* don't do anything, has be called manually */
}
?>