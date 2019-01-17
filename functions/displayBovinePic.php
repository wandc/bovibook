<?php 
/* used to display pictures from picture db, based on bovine db*/
require_once('../global.php');


$bovine_id = filter_var($_REQUEST['bovine_id'], FILTER_SANITIZE_NUMBER_INT);

$res = $GLOBALS['pdo']->prepare("SELECT picture,octet_length(picture) FROM picture.bovine WHERE bovine_id=? AND event_time=(SELECT max(event_time) FROM picture.bovine WHERE bovine_id=?) limit 1");
$res->execute(array($bovine_id,$bovine_id));
$res->bindColumn(1, $lob, PDO::PARAM_LOB);
$res->bindColumn(2, $length, PDO::PARAM_INT);
$res->fetch(PDO::FETCH_BOUND);
if ($res->rowCount() == 1) {
header('Content-type: image/jpeg');
header("Content-Disposition: inline; filename=img$bovine_id.jpg");
  fpassthru($lob); //echo out data
}
else {

//show funny SVG when no image exists.    
header('Content-type: image/svg+xml');
$file = file_get_contents($_SERVER["DOCUMENT_ROOT"].'/images/big_eye_cow.svg', true);
echo $file;

}
?> 