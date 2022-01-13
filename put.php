<?php

/** code to process any put requests from building automation */
/** there is no security with this, although we do json decode, that helps only a little (or really not at all).

  /** code to put BLE tag data in DB */
include_once($_SERVER['DOCUMENT_ROOT'] .'/'. 'global.php');

//only allow local LAN addresses to connect. Not really that secure, but gets rid of 98%.
$ip=$_SERVER['REMOTE_ADDR'];
if ((new Misc)->isPublicAddress($ip) != true)
{
    header('HTTP/1.1 403 Forbidden');
    exit;
}
    

$json = file_get_contents('php://input');
$objArr = json_decode($json, true);

//error_log("BAS json:" . ($json));

foreach ($objArr as $val) {
//error_log("BAS:" . print_R($val, TRUE));
//so we know where to take it.
    if (!empty($val['packet'])) {
    if ($val['packet'] == 'ble_rtls') {
        processBleRTLS($val);
    } elseif ($val['packet'] == 'calf_barn_scale') {
        processCalfBarnScale($val);
    } elseif ($val['packet'] == 'tmr_mixer') {
        processTMRMixer($val);    
    } elseif ($val['packet'] == 'tmr_mixer_recipe_item_feed_log') {
        processTMRMixerReceipeItemFeedLog($val);       
    } elseif ($val['packet'] == 'access_control_log') {
        processAccessControlLog($val);            
    } else {
        //unknown packet do nothing.
    }
}}


/* but estrus detetct stuff in db */

function processBleRTLS($val) {

    //error_log("BAS OBJ:". print_r($objArr,true));
//error_log("BAS JSON:". $json);
//for debuging.
//error_log("BAS:". print_R($val,TRUE));
//do DB insert.
    $query = "INSERT INTO bas.ble_tag_event (event_time, base_id, tag_id, rssi) VALUES ('{$val['time']}','{$val['base']}','{$val['tag']}',{$val['rssi']})";
    $res = $GLOBALS['pdo']->exec($query);
}

/* put calf barn scale info into db */
//example command line
//curl  --header 'content-type:application/json' -X POST --data '[{"scale_id": "D", "timestamp_epoch": 1516154433, "units": "kg", "weight": 37.30800000000018, "temp": "18.81", "packet": "calf_barn_scale"}]' https://int.littleriver.ca/put.php
function processCalfBarnScale($val) {

    error_log("calf OBJ:". print_r($objArr,true));
//error_log("calf JSON:". $json);
//do DB insert.
     $query = "INSERT INTO bas.calf_barn_scale_event_raw (event_time, scale_id, mass, temp) VALUES (to_timestamp({$val['timestamp_epoch']}),'{$val['scale_id']}','{$val['weight']}',{$val['temp']})";
     $res = $GLOBALS['pdo']->exec($query);
}


/*
 * tmr mixer puts a weight out every second. I do not thing it is very efficent to write it to DB every second and then read it back, but the calf feeder does the same thing.....
 * 
 * To set system to Read-Write use
sudo mount -o remount,rw /
and to set it back to Read-Only
sudo mount -o remount,ro /
 * 
 */
function processTMRMixer($val) {
    
    // error_log("tmr OBJ:". print_r($val,true)); //debug
     
         //does gps is not added yet.
       $query =  "INSERT INTO bas.tmr_event (event_time,display_weight,gross_weight,lat,lon,hdop) VALUES ('now'::text::timestamp without time zone,{$val['display_weight']},{$val['gross_weight']},NULL,NULL,NULL)";
     $res = $GLOBALS['pdo']->exec($query);
}


/*
 * 
 * 
 */
function processTMRMixerReceipeItemFeedLog($val) {
    
    // error_log("tmr OBJ:". print_r($val,true)); //debug
     
         //does gps is not added yet.
        $sql= <<<SQL
      INSERT INTO nutrition.nrc_recipe_item_feed_log (start_mass,end_mass,userid,nrc_recipe_item_id) VALUES ({$val['start_mass']},{$val['end_mass']},'{$val['userid']}',{$val['nrc_recipe_item_id']}) 
      ON CONFLICT (nrc_recipe_item_id) 
      DO UPDATE SET (start_mass,end_mass,userid,nrc_recipe_item_id) = ({$val['start_mass']},{$val['end_mass']},'{$val['userid']}',{$val['nrc_recipe_item_id']}) 
SQL;

    $res = $GLOBALS['pdo']->exec($sql);
}

function processAccessControlLog($val) {
    
    //need to look up device id from mac.
    
    /*
      $sql= <<<SQL
      INSERT INTO bas.access_control_log(start_mass,end_mass,userid,nrc_recipe_item_id) VALUES ({$val['start_mass']},{$val['end_mass']},'{$val['userid']}',{$val['nrc_recipe_item_id']}) 
      ON CONFLICT (nrc_recipe_item_id) 
      DO UPDATE SET (start_mass,end_mass,userid,nrc_recipe_item_id) = ({$val['start_mass']},{$val['end_mass']},'{$val['userid']}',{$val['nrc_recipe_item_id']}) 
SQL;
     
     */
}



?>