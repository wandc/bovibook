<?php 
/** this is an entry point to the site like ajax and index pages. 
 * thus this needs to do a security check before serving content.
 * 
 */
require_once("global.php");

/*
//very basic seuctiry check.
if (empty($GLOBALS['auth']->getUsername())) {
     header('400 Bad Request');
     exit;
} 
*/

/* make sure we have a valid resource or exit */
if (!isset($_GET['sseid'])) {
    header('400 Bad Request');
    exit;
}    
try {
   header('Content-Type: text/event-stream');
   header('Cache-Control: no-cache'); //prevent caching of event data.
    $sse=new SSE();
    $sse->main($_GET['sseid']);
} catch (Exception $e) {
    header('404 Not Found');
    print("404 Not Found"."\n\r");
    exit;
}






/*
 * used for generic functions needed to make server side events work.
 */
class SSE {

function main($sseid) {
   
    //do security check by checking token.
    //if token valid, then go ahead..
    
    $this->sseCall($sseid); 
     
  /*
    //LONG RUNNING TASK TEST
for($i = 1; $i <= 9; $i++) {
    send_message($i, $sseid.'on iteration ' . $i . ' of 10' , $i*10/1);

    sleep(1);
}

send_message('CLOSE', 'Process complete',100);
   */
    
    
}


//loads the file, then the class and calls the sse method. 
function   sseCall($sseid) {
    
    $res = $GLOBALS['pdo']->query("Select sse.filename,sse.filesubdir FROM intWebsite.sse WHERE sse.id={$sseid} LIMIT 1");

$row = $res->fetch(PDO::FETCH_ASSOC);

//find file path
$returnArray['filePath']='/'. $row['filesubdir'] .'/'. $row['filename'];

//object name from file name
$objName=$row['filename'];
$objName = str_ireplace('.php', '', $objName);
$objName = str_ireplace('.inc', '', $objName);
$returnArray['objectName']=$objName; 

    //load 
    require_once(__DIR__ . '/' . $returnArray['filePath']);
    $controller = new $returnArray['objectName'];
    $result = $controller->sse($_REQUEST); //replace ajax with resource.  
    return   $result;

   
}


/* this is a single method that any long running class will need for SSE calls
 * put this in its own class in misc file. Maybe move to its own file later
 */
static function send_message($id, $message, $progress) {
    $d = array('message' => $message , 'progress' => $progress);

    echo "id: $id" . PHP_EOL;
    echo "data: " . json_encode($d) . PHP_EOL;
    echo PHP_EOL;

    ob_flush();
    flush();
}
}//end class
?>