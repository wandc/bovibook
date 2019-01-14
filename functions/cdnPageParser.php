<?php 
require_once($_SERVER['DOCUMENT_ROOT'] . '/global.php'); 

class CDNPageParser {

//used when called from PHP
public function generateIframe($full_reg_number) {
    $str='';
    $str=$str.("<iframe id=\"cdnframe\" src=\"/functions/cdnPageParser.php?reg=$full_reg_number&iframe=0\" >");
    $str=$str.('<p>Your browser does not support iframes.</p>');
    $str=$str.('</iframe>'); 
    return $str;
}   
    
    
 function main() {    


//get reg variable and break it up into component parts.
$reg=$_REQUEST['reg'];


//if the iframe var is set to one, we callback this page with the iframe tags around it and go onto the else statement.
//this is needed because teh src element is required for the iframe. The reason we do this is to keep the little river pages css pure.
if ($_REQUEST['iframe']==1) {
	 print("<iframe id=\"cdnframe\" src=\"{$_SERVER['PHP_SELF']}?reg=$reg&iframe=0\" width=\"610px\" height=\"1024px\">");
     print('<p>Your browser does not support iframes.</p>');
	 print('</iframe>');
}
else {


//$reg='HOUSAM123066734';

$regComponentArray=$GLOBALS['MiscObj']->breakUpFullRegNumber($reg);


$url="https://www.cdn.ca/query/detail_ge.php?breed={$regComponentArray['breed']}&country={$regComponentArray['country']}&sex={$regComponentArray['sex']}&regnum={$regComponentArray['number']}";

 $ch = curl_init($url); 
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
 $cdnPage= curl_exec($ch);
 

//make links open up in the same window.
$cdnPage= str_replace('<a href','<a target="_self" href',$cdnPage);

 

 print('<base  HREF="https://www.cdn.ca/query/" TARGET="_top">'); //fixes all the relative url problems
 //print out cdn page
 print($cdnPage);

 //print('<style type="text/css"> * { font-size: 6px; }</style>'); //shrink all 
 
//override
print('
<style>
#PrintFooter {
display: none;
}

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


#PrintHeader {
display: none;
}

</style>
');
}//end else

}//end main

}//end class

 //if this is called from a url, then run it.
 if (isset($_REQUEST['reg'])) 
     {
    $a=new CDNPageParser();
    $a->main();
}  


?>