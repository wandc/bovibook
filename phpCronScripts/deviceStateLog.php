<?php
/* used for logging state from control by web */
/* this script runs quite often */

/* run from cron */
$_SERVER['DOCUMENT_ROOT'] = '/var/www/int/';
require_once('../global.php');



//do dairy barn state
$x = new Temperature();
$json = $x->getCurrentState($x->barnIP);
$query = "INSERT INTO bas.device_state_log (event_time,device,state) VALUES (CURRENT_TIMESTAMP,'x-332_99-barn','$json')";
$res = $GLOBALS ['pdo']->exec($query);

//do dairy parlor state
$x = new Temperature();
$json = $x->getCurrentState($x->parlourIP);
$query = "INSERT INTO bas.device_state_log (event_time,device,state) VALUES (CURRENT_TIMESTAMP,'x-400_99-parlour','$json')";
$res = $GLOBALS ['pdo']->exec($query);

//do manure pit state
$x = new Temperature();
$json = $x->getCurrentState($x->manurePitIP);
$query = "INSERT INTO bas.device_state_log (event_time,device,state) VALUES (CURRENT_TIMESTAMP,'x-400_99-manurePit','$json')";
$res = $GLOBALS ['pdo']->exec($query);

?>
