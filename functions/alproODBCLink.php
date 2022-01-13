<?php 


include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
/** USED by sort gate only */
/** This is a hack class if the ISO protocol was complete this would not be needed, hopefully we can get rid of it in the future. */

/**
 * Used currently for sort gate, but could be used for other unsupported things.
 * 
 * This class establishes an odbc link by using the opensource mdbtools. This is a read only link from the point of view of mdbtools.
 * File locking of the db files is a real problem, so the time is checked to see if the the file is up to date.
 * After that, any queries can be run, currently only sort gate.
 */
class AlproODBCLink {

    public $dsn = "odbc:///alpro2tmprename"; // opensource driver "odbc:///alpro2" //commerical driver: "odbc:///alpro2easysoftdriver" //use alpro2tmp to point to /tmp/waddy.aph
    public $odbc;
    public $dbFileSource = '/mnt/share2/waddy.apw'; //set to share2 because we need read only access or we get an oplocks problem and don't get the latest data.
    public $dbFileDestination = '/tmp/tmp_waddy.apw'; //must be a different name, because we have other cron scripts that conflict if you use waddy.apw
    public $dbFileAge;
    public $sortAreaArray;

    public function __construct() {


        //change this if you ever change the settings in alpro sort area to location_id
        $this->sortAreaArray[1] = 59;
        $this->sortAreaArray[2] = 39;
        $this->sortAreaArray[3] = 42;
        $this->sortAreaArray[4] = 40;
        $this->sortAreaArray[5] = 41;


        $this->copyAlproDB();
        $this->makeAlproDBConnection();
        $this->dbFileAge = $this->getAlproDBDate();
        //print("</br>DEBUG: Time of db file: {$this->dbFileAge}.</br>");

        if (abs(strtotime($this->dbFileAge) - strtotime("now")) > 300) {
            exec('umount /mnt/share2;mount /mnt/share2'); //this is a super hack to reset oplocks on samba share.
            //try again before throwing exception.
            $this->copyAlproDB();
            $this->makeAlproDBConnection();
            $this->dbFileAge = $this->getAlproDBDate();


            if (abs(strtotime($this->dbFileAge) - strtotime("now")) > 300) {
                throw new exception("<b>ERROR: ODBC DB file out of date (more than 5 minutes). Check Alpro processor clock is correct, if still out of date, probably a file oplocks problem. Alpro time is: {$this->dbFileAge} and server time is " . date('Y-m-d H:i:s', strtotime("now")) . " </b></br>");
            }
        }
    }

//end construct

    /** delete temp file when done */
    public function __destruct() {
        try {
            unlink($this->dbFileDestination);
        } catch (Exception $e) {
            throw new Exception("Failed to delete {$this->dbFileDestination}. Should never happen.<\br>\n\r");
        }
    }

//end destruct
//////////////////////// FUNCTIONS FOR THE CONSTRUCTOR ////////////////////////////////////

    /**
     * Get how old the data is from the alpro db.
     */
    private function getAlproDBDate() {
        //do a test read of the file to see if it is up to date, ie if you have oplocks on you can get an old version of the file.
        $sql2 = "SELECT RealDate,RealHour,RealMinute FROM TblSystem";
        $res2 = $this->odbc->query($sql2);
        $row2 = $res2->fetch();
        $alproTime = date('Y-m-d', strtotime($row2[0])) . ' ' . $row2[1] . ':' . $row2[2];
        return $alproTime;
    }

    private function copyAlproDB() {
        /** first we must copy the db file to a temporary file or we can't read it with the mdbtools odbc driver */
        if (!copy($this->dbFileSource, $this->dbFileDestination)) {
            throw new Exception("Failed to copy {$this->dbFileSource}. This could mean that the /mnt/share directory is down or permssions problems.<\br>\n\r");
        }
    }

    private function makeAlproDBConnection() {
        /** now point the odbc.ini to the temporary location of waddy.aph DB file */
        // Create a new PDO connection object and connect to the ODBC database.
       
        $mdb_file = $this->dbFileSource;
        $uname = explode(" ", php_uname());
        $os = $uname[0];
        switch ($os) {
            case 'Windows':
                $driver = '{Microsoft Access Driver (*.mdb)}';
                break;
            case 'Linux':
                $driver = 'MDBTools';
                break;
            default:
                exit("Unknown OS, please add support.");
        }
        $dataSourceName = "odbc:Driver=$driver;DBQ=$mdb_file;";

 
        try {
             $this->odbc = new PDO($dataSourceName);
             $this->odbc->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); //DEBUG:: turns on DB debugging 
        } catch (PDOException $Exception) {
            print("Unable to connect to ODBC database.");
            throw new PDOException($Exception->getMessage(), $Exception->getCode());
        }
    }

//////////////////////// END FUNCTIONS FOR THE CONSTRUCTOR ////////////////////////////////////

    /**
     * returns a log of who went thought the sort gate last
     */
    public function sortGateQuery() {

        /*
         * Example code: 
          $sql ="SELECT TblCow.CowNo,TblCow.LastCutDate,TblCow.GroupNo,TblCow.LastCutDate,TblCow.LastCutTime,QryAreaName.CutAreaIdName,QryCowSorting2.CutReasonText FROM ((TblCow LEFT JOIN QryAreaName ON (TblCow.CowNo = QryAreaName.CowNo AND TblCow.HerdNo = QryAreaName.HerdNo)) LEFT JOIN QryCowSorting2 ON (TblCow.CowNo = QryCowSorting2.CowNo AND TblCow.HerdNo = QryCowSorting2.HerdNo)) WHERE ( TblCow.GroupNo > 0 ) ORDER BY 1 ASC";
         */

        $sql = "SELECT CowNo,GroupNo,LastCutDate,LastCutTime,CutArea2 FROM TblCow";

        $res = $this->odbc->query($sql);
        $outArray = null;
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            //convert to timestamp

            if ('1969-12-31' == date('Y-m-d', strtotime($row['LastCutDate']))) {
                $event_time = null;
            } else {
                $event_time = date('Y-m-d', strtotime($row['LastCutDate'])) . ' ' . date('H:i:s', strtotime($row['LastCutTime']));
            }

            $outArray[$row['CowNo']]['local_number'] = $row['CowNo'];
            $outArray[$row['CowNo']]['location_id'] = $row['GroupNo'];
            $outArray[$row['CowNo']]['event_time'] = $event_time;
            $outArray[$row['CowNo']]['sort_gate_area'] = $row['CutArea2'];
        }

        return $outArray;
    }

//end function

    /**
     * Stores the latest log of the sort gate in postgres for easier manipulation
     */
    public function storeSortGateLogInDB($dataArray) {



        
            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();


            //NEVER EMPTY LOG
            //empty current table data.
            //$statement = $GLOBALS['pdo']->prepare("TRUNCATE alpro.sort_gate_log");
            //$statement->execute();



            foreach ($dataArray as $key => $value) {
                //print_r($value);//DEBUG//
                //get bovine id
                $sql = "SELECT id FROM bovinemanagement.bovine WHERE local_number={$value['local_number']} LIMIT 1";
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $bovine_id = $row['id'];

                $data_time = date('r', strtotime($this->dbFileAge));
                if ($value['sort_gate_area'] == null) {
                    $sorted_location_id = null;
                } else {
                    $sorted_location_id = $this->sortAreaArray[$value['sort_gate_area']];
                }

                $home_location_id = $value['location_id'];


                //This code is all quote ugly because we are using event_time and bovine_id 
                //together as the primary key and event_time can be null. This creates a mess of code.
                //take care of blank dates.
                if (( date('Y', strtotime($value['event_time'])) == 1969) OR (trim($value['event_time']) == '')) {
                    $event_time2 = "null::timestamp without time zone";
                    $sql = "SELECT bovine_id,event_time FROM alpro.sort_gate_log WHERE bovine_id=$bovine_id AND event_time is null";
                } else {
                    $event_time2 = "'" . date('Y-m-d H:i:s', strtotime($value['event_time'])) . "'";
                    $sql = "SELECT bovine_id,event_time FROM alpro.sort_gate_log WHERE bovine_id=$bovine_id AND event_time=$event_time2";
                }

                //take care of situation where the cow has never been sorted.
                if ($sorted_location_id == null) {
                    $sorted_location_id = 'null';
                }

                //only insert data if bovine_id and event_time are unique.
                //RUN sql...
                $res = $GLOBALS['pdo']->query($sql);
                //print($sql."\n"); //DEBUG
                if ($res->rowCount() == 0) {
                    $query = "INSERT INTO alpro.sort_gate_log (bovine_id,home_location_id,sorted_location_id,event_time,data_time) VALUES ($bovine_id,$home_location_id,$sorted_location_id,$event_time2,'$data_time')";
                   //print("Inserting:: $bovine_id || $event_time2 || {$value['event_time']} ||.\n $query\n\n");  //DEBUG//
                    $statement = $GLOBALS['pdo']->prepare($query);
                    $statement->execute();
                }
            }//end foreach 
            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

}

//end class 

/*
  $x=new AlproODBCLink();
  $x->sortGateQuery();
 */
?>
