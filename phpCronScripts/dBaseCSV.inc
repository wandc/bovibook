<?php 
/*
* Run Various export functions that write files for dBase to read.
* //RUN FROM EXTERNAL SERVER ONLY
*/

//run the export functions whenever this file is accessed.
$_SERVER['DOCUMENT_ROOT']='/var/www/int/';
require_once('../global.php');



$x=new dbase();
$x->main();

?>