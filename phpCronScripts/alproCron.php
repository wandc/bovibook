<?php 
/**
 *  FIXME: IS THIS FILE EVEN USED ANYMORE????? NEED
 * 
 * 
 * 
 * 
 */
if (defined('STDIN')) { //when called from cli, command line
$_SERVER['DOCUMENT_ROOT']='/var/www/int/'; 
}
require_once('../global.php');
require_once($_SERVER['DOCUMENT_ROOT'].'functions/alproCommands.php');

/**
 * Script that runs quite often, (every 2 minutes?) and keeps the int.littleriver.ca
 * system and the DeLaval Alpro system in sync. Dec 4, 2012
 * This was written to decouple the web gui with the sycing, too slow otherwise.
 */
class AlproCron {

    static public $scriptTimeoutSeconds=305;
    
    function main() {
        //DELETE any old entries so the db doesn't get away from us.
        $query = "DELETE FROM alpro.cron WHERE end_date <= (current_timestamp - interval '48 hours')";
        $res = $GLOBALS['pdo']->exec($query);
        
        //first check the db to see if this script is currently running or not.
        $res = $GLOBALS['pdo']->query("SELECT * FROM alpro.cron WHERE end_date is null");
        if ($res->RowCount() > 0) {
            //another script is currently running or has died.
            //so see if it was recently.
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                //if less than 5 minutes since script is running, just exit.
                //assume all is ok, just slow.
                if ((strtotime("now") - strtotime($row["start_time"])) <= $this->scriptTimeoutSeconds) {
                     print("WARNING: Another Alpro Cron script is running, but not for very long. Let it run, exiting..."."\n\r");
                    exit();
                }
                //the script is almost certainly hung.   
                else {
                    //kill script //careful... linux only code.
                    if (file_exists( "/proc/{$row['pid']}" )){
                    $retKill = posix_kill($row['pid']);
                    }
                    //already killed
                    else {
                      $retKill = true;  
                    }
                       
                    if ($retKill == true) {
                        //do insert to say script ended
                        $query = "UPDATE alpro.cron SET end_date=current_timestamp WHERE uuid='{$row['uuid']}'";
                        $res = $GLOBALS['pdo']->exec($query);
                        //exit script
                        exit();
                    }
                    //should never happen, but if it does abort script.
                    else {
                        throw new Exception("ERROR: Alpro Cron Process {$row['pid']} could not be killed." . "\n\r");
                        exit();
                    }
                }
            }
        } else {
            //all is normal so run actually code.
            self::cronSync();
        }
    

//end function
}

/**
 * Actually does the syncing.
 * 
 * 
 */
public static function cronSync() {
    //create uuid for db.
    $uuid=Misc::uuid_create();
    $pid=getmypid();
    //say we are starting.
    $query = "INSERT INTO alpro.cron (uuid,pid,start_time) VALUES ('$uuid',$pid,current_timestamp)";
    $res = $GLOBALS['pdo']->exec($query);
   
    //we can keep adding alpro features to the alproSync.php as we see fit.
    //any exceptions are not caught so this aborts, sloppy but it works.
    /** ******************** **/
    print("ALPRO CRON RUNNING."."\n\r");
    //include_once($_SERVER['DOCUMENT_ROOT'].'phpCronScripts/alproSync.php'); //too slow to run this every 2 minutes.
    include_once($_SERVER['DOCUMENT_ROOT'].'phpCronScripts/AlproSortAndMovementSync.inc');
    print("ALPRO CRON ENDING."."\n\r");
    /** ******************** **/

    //successfully ran
     $query = "UPDATE alpro.cron SET end_date=current_timestamp WHERE uuid='$uuid'";
     $res = $GLOBALS['pdo']->exec($query);
       
}


}//end class

$xx = new AlproCron();
$xx->main();
?>



