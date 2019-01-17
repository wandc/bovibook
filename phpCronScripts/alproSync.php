<?php 
/**
* This file run the sync part of the sort gate code, so that if it is the middle of the night and no one has done a page refresh, a sync still occurs.
*
*/
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}

include_once($_SERVER['DOCUMENT_ROOT'].'functions/alproCommands.php');
echo("START: Alpro Commands Sync.\n\r");
$xx=new AlproCommands();
$xx->syncBovineAlive();
$xx->syncBovineDead();
$xx->syncBovineMove();
$xx->syncDoNotMilk();
$xx->syncDumpMilk();
//$xx->syncBovineDriedOff();
echo("END: Alpro Commands Sync.\n\r");
?>
