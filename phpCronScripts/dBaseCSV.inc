<?php 
/*
* Run Various export functions that write files for dBase to read.
* //RUN FROM EXTERNAL SERVER ONLY
*/

//run the export functions whenever this file is accessed.
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . '/global.php');
}


$x=new dbase();
$x->main();

?>