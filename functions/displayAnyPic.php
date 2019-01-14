<?php 
/* used to display pictures from picture db, based on bovine db*/
/*all the picture schema tables use the same index. So we can just join the tables and this can serve all the picture for the site */
/* this is very dumb, it will just serve the id given to it */
require_once('../global.php');


$picture_id = filter_var($_REQUEST['picture_id'], FILTER_SANITIZE_NUMBER_INT);
$sql="";


$res = $GLOBALS['pdo']->prepare("SELECT picture,octet_length(picture) FROM picture.picture_combined WHERE id=:id limit 1");
$res->execute(array('id'=>$picture_id));
$res->bindColumn(1, $lob, PDO::PARAM_LOB);
$res->bindColumn(2, $length, PDO::PARAM_INT);
$res->fetch(PDO::FETCH_BOUND);
if ($res->rowCount() == 1) {
header('Content-type: image/jpeg');
header("Content-Disposition: inline; filename=picture_{$picture_id}.jpg");
  fpassthru($lob); //echo out data
}
else {
 print("<h1>Error JPG  comvined picture id does not exist! picture_id:{$picture_id}</h1>");

}
?> 
