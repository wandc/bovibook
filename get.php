<?php

/** Used by basic devices (arduino) to get information OUT of the system */
/** can only be accessed over TLS SSL. */
/** We then only need to verify the request is actually coming from who we think it is, thus we need a non-guessable static indentifier. */
/** mac address and some pre-shared token with the device. */

include_once($_SERVER['DOCUMENT_ROOT'] .'/'. 'global.php');
/*
//only allow local LAN addresses to connect. Not really that secure, but gets rid of 98%.
$ip=$_SERVER['REMOTE_ADDR'];
if ((new Misc)->isPublicAddress($ip) != true)
{
    header('HTTP/1.1 403 Forbidden');
    exit;
}
  */  
main();

function main(){


//grab URL contents. 

$urlDataField = $_GET['data'];


$dataArr = json_decode($urlDataField,true);
$val=$dataArr;



//json was not valid
if (empty($val)) {
   header('HTTP/1.1 400 Bad Request');
         exit;  
}

$response='';
//so we know where to take it.
    if (!empty($val['packet'])) {
    if ($val['packet'] == 'access_control_download') {
        $response=processAccessControlDownload($val);         
    } else {
        //unknown packet do nothing.      
         header('HTTP/1.1 400 Bad Request');
         exit;
    }
    
   
 /* send response */
        header('Content-Type: application/json');
        echo json_encode($response);
        die();

    }
}

/*used by door controller to download information */
function processAccessControlDownload($val) {
    $out=array();

    //check basic security, need a changing token
    if ($val['token']!='h3j3LkQr9Vxn3SKSX2B3U8XbPTzjrrGA') {
        header('HTTP/1.1 401 Unauthorized');
         exit;
    }
    
   //check mac is valid
    if (false === filter_var($val['mac'], FILTER_VALIDATE_MAC)) {
     header('HTTP/1.1 400 Bad Request');
      exit;
    }
    
                    $sql = <<<SQL
SELECT card_token from bas.access_control_all where device_mac='{$val['mac']}'
SQL;
    $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);  
        
    error_log("Access Control OBJ:". print_r($val,true));

    $out['info']="cards this reader allows in.";
    $out['data']=$results;
    
   return $out;
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