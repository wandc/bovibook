<?php
include_once($_SERVER['DOCUMENT_ROOT'] .'/'. 'global.php');
 
/*
 * use to insert a packet from the things network into database table, based on 
 * which app is incoming. 
 *  useses TTN APPLICATION INTEGRATIONS with HTTP. 
 */

    //Receive from TTN
    $postdata = file_get_contents('php://input');

    $jsonArr=json_decode($postdata, true);

        
      //change to case statement as we add more apps.
    
   if ($jsonArr['app_id']=='fence_monitor') {

    //insert packet into DB.
    $exc = $GLOBALS['pdo']->prepare("INSERT INTO bas.ttn_fence_monitor_event( packet) VALUES ( :packet);");                               
    $exc->bindParam(':packet',$postdata, PDO::PARAM_STR);
    $exc->execute();
    

} else {
      header('HTTP/1.1 400 Bad Request');
      exit;  
}

  /*
    //DEBUG TO FILE
    file_put_contents('yourfile.txt',  $postdata . PHP_EOL, FILE_APPEND);
    echo 'ok';
   */
  
?>
