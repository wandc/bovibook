<?php 
require_once('../../functions/misc.inc');

//get reg variable and brek it up into component parts.
$reg=$_REQUEST['reg'];

//$reg='HOUSAM123066734';

$regComponentArray=$GLOBALS['MiscObj']->breakUpFullRegNumber($reg);


$url="https://www.cdn.ca/query/detail_ge.php?breed={$regComponentArray['breed']}&country={$regComponentArray['country']}&sex={$regComponentArray['sex']}&regnum={$regComponentArray['number']}";

 $ch = curl_init($url); 
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
 $cdnPage= curl_exec($ch);
 
 
 //replace the red graphing image links with one that works.
$cdnPage= str_replace('../images/bg_hilite_tile.gif','https://www.cdn.ca/images/bg_hilite_tile.gif',$cdnPage);
 
 //print out cdn page
 print($cdnPage);
 
//print('<link href="tnone.css" rel="stylesheet" type="text/css"> ');

//over
print('
<style>

#HeaderArea {
display: none;
}

#FooterArea {
  display: none;
}


#HorizMenu {
display: none;
}

#SignInArea {
  display: none;
}

#VMenu {
display: none;
}

.tab_bar_baseline 
{
display: none;
}

#VertMenu {
display: none;
}

#ContentArea {
position: absolute;
width:570px;
border: solid 1px #000;
}

</style>
');

?>