<?php 
/**
 * Runs code to sync Alpro sort gate and movement with internal datatbase.
 *
 */
if (defined('STDIN')) { //when called from cli, command line
    //$_SERVER['DOCUMENT_ROOT']='/var/www/int/';
    require_once('../global.php');
    include_once('../functions/misc.inc');
    include_once('../functions/alproSortHelper.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/misc.inc');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/alproSortHelper.inc');
}

/*
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
*/

class AlproSortAndMovementSync {

    public function __construct() {
        
    }

    public function main() {
        echo("START: Sort Gate Sync.\n\r");
        $yyy = new AlproSortHelper();
        $yyy->sync();
        echo("END: Sort Gate Sync.\n\r");
    }

    public function sse() {
        $yyy = new AlproSortHelper();
        $yyy->sse();
    }
    
     /* returns datetime of last DB update for location_sort, thus assumes main function completed then */
    public function lastRun() {
        $sql = "SELECT event_time,state FROM alpro.sync WHERE state=true AND db_table='location_sort' ORDER BY event_time DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return strtotime($row['event_time']);
    }
    

}//end class

if ((defined('STDIN')) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new AlproSortAndMovementSync();
    $cls->main();
}
?>