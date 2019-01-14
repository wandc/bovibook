<?php 
require_once($_SERVER['DOCUMENT_ROOT'].'/global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/holsteinCanadaHelper.inc');

class HolsteinCanadaPageParser {
  
  
  private $width;
  private $height;

	public function __construct($width=850,$height=1024) {
	 //get reg variable and brek it up into component parts.
        
         $this->width=$width;
         $this->height=$height;
	 

	}

	// this will be called automatically at the end of scope
	public function __destruct() {
		//nothing
	}




public function generateIframe($reg) {    
    $str='';
    $str=$str.("<iframe id=\"holframe\" src=\"/functions/holsteinCanadaPageParser.php?reg=$reg&iframe=0\" width=\"$this->width px\" height=\"$this->height px\">");
    $str=$str.('<p>Your browser does not support iframes.</p>');
    $str=$str.('</iframe>'); 
    return $str;
}



function main() {

    $reg=$_REQUEST['reg'];
    
//if the iframe var is set to one, we callback this page with the iframe tags around it and go onto the else statement.
//this is needed because teh src element is required for the iframe. The reason we do this is to keep the little river pages css pure.
if ($_REQUEST['iframe']==1) {
	print("<iframe id=\"holframe\" src=\"{$_SERVER['PHP_SELF']}?reg=$reg&iframe=0\" width=\"$this->width px\" height=\"$this->height px\"><p>Your browser does not support iframes.</p></iframe>");
     print('<p>Your browser does not support iframes.</p>');
	 print('</iframe>');
}
else {

    $regComponentArray=$GLOBALS['MiscObj']->breakUpFullRegNumber($reg);
    $url=HolsteinCanadaHelper::createHolsteinCanadaQuery($reg); //create string to query holstein Canada from reg number

 $ch = curl_init($url); 
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch,CURLOPT_USERAGENT,'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0) Gecko/20100101 Firefox/40.1'); //it seems holstein canada displays site only to browsers it recognizes.

 $hosPage= curl_exec($ch);
 //remove nav junk.
 $hosPage = preg_replace('~<nav(.*?)</nav>~Usi', "", $hosPage);

//make links open up in the same window.
$hosPage= str_replace('href','target="_self" href',$hosPage);
$hosPage= str_replace('TARGET="_top"','TARGET="_self"',$hosPage);
$hosPage= str_replace('target="_top"','TARGET="_self"',$hosPage);

print('<base  HREF="http://www.holstein.ca/" TARGET="_top">'); //fixes all the relative url problems
 //print out holstein page
 print($hosPage);

 //print('<style type="text/css"> * { font-size: 6px; }</style>'); //shrink all 
 /*
//override
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

  */
 
   }//end else
 

}//end main




}//end class


//only run class if being called via url with reg.
if ((isset($_REQUEST['reg']))) {
    $xx=new HolsteinCanadaPageParser(850,1024);
$xx->main();
}


?>