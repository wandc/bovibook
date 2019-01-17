<?php

/*
 * This is meant to be used a cron script to sync the urban feeder with the INT system
 * The urban feeder needs birthday information sent to it.
 * The urban feeder automatically registeres new calves, so we wait to see who is in the urban system and then we send it the birthdate.
 * The scales need to know when a calf was in the feeder, so we get that information
 */

/**

  We used the postgres_fdw extension so we could access the foreign postgres db
  --------
  CREATE EXTENSION postgres_fdw;

  CREATE SERVER foreign_server_urban_feeder
  FOREIGN DATA WRAPPER postgres_fdw
  OPTIONS (host '192.168.9.6', port '5432', dbname 'u46_1_kunde');

  CREATE USER MAPPING FOR david
  SERVER foreign_server_urban_feeder
  OPTIONS (user 'waddy', password '******');

  CREATE USER MAPPING FOR r2d2
  SERVER foreign_server_urban_feeder
  OPTIONS (user 'waddy', password '******');

  IMPORT FOREIGN SCHEMA tiere LIMIT TO (tabelle_trinkgeschwindigkeit, tabelle_stationsbesuch,	identifikation, view_mh_aktuelle_tier_bewertungen, tageswerte_verbrauch_milch)
  FROM SERVER foreign_server_urban_feeder INTO urban_feeder_foreign_tiere;
  --------
 * 
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

/*
  ini_set('display_errors', 1);
  ini_set('display_startup_errors', 1);
  error_reporting(-1);
 */

class UrbanFeederSync {

    // const CDN_NUMBER_CONNECTIONS_PARALLEL = 50;
    // private $uuid;

    var $pdoUrban;

    function __construct() {
        //try and connect to calf feeder
        try {
            $pdoDsnUrbanCalf = ($GLOBALS['config']['PDO']['dsnUrbanCalf']); //local socket
            $pdoUrban = new PDO($pdoDsnUrbanCalf);
            //$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES,false); //is this slow? no idea
            $pdoUrban->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); //DEBUG:: turns on DB debugging site wide.
            $this->pdoUrban = $pdoUrban;
        } catch (PDOException $Exception) {
            print("Exception: " . $Exception->getMessage());
            throw new PDOException($Exception->getMessage(), $Exception->getCode());
        }
    }

    //main method to run
    public function main() {
        print("Doing Birthdays" . "\n\r");
        $this->syncUrbanBrithdaysFromINT();
        print("Doing Culled/Dead calves" . "\n\r");
        $this->syncUrbanCavlesFomINTCulls();
         print("Doing Movement Locations for calves" . "\n\r");
         $this->syncUrbanFeederStallWithINTLocation();
    }

    //not sure how to do this yet.
    public function removedWeanedHeiferBarnCalvesFromUrban() {
        
    }

    //when a calf as at a certain urbanfeeder put a movement event into the INT computer
    public function syncUrbanFeederStallWithINTLocation() {

        //first find all calves in urban feeder
        $sql = "    with temp as (
select distinct on(responder_nr) responder_nr as partial_rfid_number,tiere_id,
(SELECT terminal || '.' || automat || '.' || station || '.' ||  box as feedstall from urban_feeder_foreign_tiere.tabelle_stationsbesuch where tiere_id=identifikation.tiere_id order by letzte_erkennung DESC limit 1) as feedstall
FROM  urban_feeder_foreign_tiere.identifikation  WHERE substring(responder_nr from 1 for 3) != 'DEL'
)
select tiere_id,id as bovine_id,local_number,bovineall.location_id,location_name,feedstall as urban_feedstall,location_urban_feedstall.feedstall_id as int_feedstall
FROM temp
LEFT JOIN bovinemanagement.bovineall ON partial_rfid_number::text=(bovinemanagement.rfid_number_to_partial(rfid_number))::text
LEFT JOIN bovinemanagement.location_urban_feedstall on bovineall.location_id=location_urban_feedstall.location_id";
        $res = $GLOBALS['pdo']->query("$sql");
        
 $transaction_id = Misc::generatePseudoRandomTransactionInteger(); //for location insert.
 
        //go through each calf in urban feeder
        while (($row = $res->fetch())) {

            /* calf has been moved to heifer barn, but he/she still is in urban feeder. we need to delete them from urban feeder */
            if ($row['location_id'] == 46) {

                print("Deleteing from feeder:" . $row['local_number']) . "\n\r";
                $sqlDELETE = "SELECT * FROM tiere.funk_mh_tier_zum_loeschen_makieren({$row['tiere_id']})"; //this functions does the delete in urban feeder. DANGEROUS, NO GOING BACK
                $resUrban = $this->pdoUrban->exec($sqlDELETE);
            }
            //for everything not moved to heifer barn we assume there INT location is wrong to begin with.
            else {
                //not yet moved to calf barn pen in int system, so lets do an insert.

                $sqlInner = "SELECT location_id FROM bovinemanagement.location_urban_feedstall WHERE feedstall_id='{$row['urban_feedstall']}' limit 1"; //find int location based on urban db's feedstall
                $rowInner = $GLOBALS['pdo']->query("$sqlInner")->fetch();
                //when they are not the same update the location in int system.
                if ($rowInner['location_id'] != $row['location_id']) {
                    
                    $sqlInsert = "INSERT INTO bovinemanagement.location_event (bovine_id,location_id, event_time,userid,transaction_id) VALUES ({$row['bovine_id']},{$rowInner['location_id']},date_trunc('hour',now()),'system','$transaction_id')";
                    $stmt = $GLOBALS['pdo']->exec($sqlInsert);
                }
            }
        }
    }

    /* WARNING: DO NOT CULL THE WRONG CULLS OR THEIR INFORMATION WILL BE LOST.
     * the urban feeder needs to be told when we cull a bob calf or dead heifer. This is dangerous because if you cull the wrong animal, there seems to be no way to reverse it.
     */

    public function syncUrbanCavlesFomINTCulls() {


//find all the urban tiere_id of animals who are in urban system, but have a death_date in little river int, ie they are culled.
//ignores calves that have been recently deleted.
        $sql = "
    with temp as (
select distinct on(responder_nr) responder_nr as partial_rfid_number,tiere_id
FROM  urban_feeder_foreign_tiere.identifikation WHERE substring(responder_nr from 1 for 3) != 'DEL'
)
select id as bovine_id,local_number,current_date - birth_date::date as age_days,temp.*
FROM temp
LEFT JOIN bovinemanagement.bovine ON partial_rfid_number::text=(bovinemanagement.rfid_number_to_partial(rfid_number))::text
where death_date is not null
ORDER BY birth_date DESC
";

        $res = $GLOBALS['pdo']->query("$sql");
        while (($row = $res->fetch())) {
            
            print("Deleteing from feeder:" . $row['local_number']) . "\n\r";
            $sqlDELETE = "SELECT * FROM tiere.funk_mh_tier_zum_loeschen_makieren({$row['tiere_id']})"; //this functions does the delete in urban feeder. DANGEROUS, NO GOING BACK
            $resUrban = $this->pdoUrban->exec($sqlDELETE);
        }
    }

    //the urban feeder has no idea what a calfs birthday is unless we tell it.
    public function syncUrbanBrithdaysFromINT() {
        $sql0 = "SELECT local_number,rfid_number,birth_date, (regexp_matches(rfid_number::text,	E'([0-9]{3}[0]*)([0-9]*)') )[2] as partial_rfid_number
FROM bovinemanagement.bovine 
WHERE birth_date >= current_date  - interval '6 months'";
        $statement = $GLOBALS['pdo']->prepare($sql0);
        $statement->execute();
        $results0 = $statement->fetchAll(PDO::FETCH_ASSOC);

        //convert to key value of partial rfid and birthday. //one in 54 bit chance of collision with another country code.
        $intArr = array();
        foreach ($results0 as $k => $v) {
            $intArr[$v['partial_rfid_number']] = $v['birth_date'];
        }

        //get birthday and cow list from urban
        $sql = "SELECT geburtsdatum.tiere_id,responder_nr,geburtsdatum.geburtsdatum,herkunft
FROM tiere.identifikation
LEFT JOIN tiere.geburtsdatum ON identifikation.tiere_id=geburtsdatum.tiere_id ";
        $res = $this->pdoUrban->query($sql);

        //Note that this ignores herkunft=30 that the feeder enters when a calf shows up for the first time. I assume herkunft=50 will always supersede it. Thus we only insert to 50 
        //if the script hasn't been run yet since calf was added it will have a herkunft=30 entry only.
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //match partial rfid numbers between int and urban
            if (empty($intArr[$row['responder_nr']]) == false) {
                //print_r($row);
                if ($row['herkunft'] == 30) {

                    //insert if a herkunft=50 does not exist. this is postgres 9.1,so no upsert.
                    //do an insert because we know birth date has never been put in urban system.
                    $sql10 = "SELECT tiere_id FROM tiere.geburtsdatum  WHERE tiere_id={$row['tiere_id']} AND herkunft=50";
                    $statement10 = $this->pdoUrban->prepare($sql10);
                    $statement10->execute();
                    $results10 = $statement10->fetchAll(PDO::FETCH_ASSOC);

                    if (count($results10) == 0) {


                        $sqlInsert = "INSERT INTO tiere.geburtsdatum  (tiere_id,geburtsdatum, herkunft) VALUES ({$row['tiere_id']},'{$intArr[$row['responder_nr']]}',50)";
                        print("insert {$row['responder_nr']}<br>");
                        try {
                            $stmt = $this->pdoUrban->exec($sqlInsert);
                        } catch (Exception $e) {
                            echo 'Exception -> ';
                            var_dump($e->getMessage());
                        }
                    }
                } elseif ($row['herkunft'] == 50) {
                    //check birthdate is correct, if not update.
                    if ($row['geburtsdatum'] != $intArr[$row['responder_nr']]) {
                        //birthdates don't match, thus update urban feeder.

                        $sqlUpdate = "UPDATE tiere.geburtsdatum SET geburtsdatum='{$intArr[$row['responder_nr']]}' WHERE tiere_id={$row['tiere_id']} AND herkunft=50";
                        print("update {$row['responder_nr']}<br>");
                        try {
                            $stmt = $this->pdoUrban->exec($sqlUpdate);
                        } catch (Exception $e) {
                            echo 'Exception -> ';
                            var_dump($e->getMessage());
                        }
                    }
                } else {
                    print("Error Did not do anything. Should not be called.<br>");
                }
            }
        }
    }

}

//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new UrbanFeederSync();
    $cls->main();
} else {
    //nothing
}
?>