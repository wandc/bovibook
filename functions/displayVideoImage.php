<?php 
header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');
/**
Page to show axis camera camera video feed pictures for internet site visitors
*/

class displayVideoImage {

	public function __construct($ip) {
	  
	  
	 if ($this->validateIpAddress($ip) == true) {
	  //create URL to talk to axis camera.
	  $remoteImage='http://'.$ip.'/jpg/1/image.jpg?date=1&clock=1';	  
	  $this->savePhotoToStr($remoteImage);
	  }
	  else {
	  print("Invalid IPv4 address.<br/>");
          return -1;
	  }
		
	}



function savePhotoToStr($remoteImage) {
    $ch = curl_init();
    curl_setopt ($ch, CURLOPT_URL, $remoteImage);
    curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 0);
    $fileContents = curl_exec($ch);
    curl_close($ch);
    $newImg = imagecreatefromstring($fileContents);
    
    

    // Set the content type header - in this case image/jpeg
   header('Content-type: image/jpeg');

    // Output the image to browser
   imagejpeg($newImg);

    // Free up memory
  imagedestroy($newImg);
}

//from: http://roshanbh.com.np/2008/04/ip-address-validation-php.html
function validateIpAddress($ip_addr) {
$long = ip2long($ip_addr);
return !($long == -1 || $long === FALSE);
}


} //end class


new displayVideoImage($_REQUEST['ip']);


?>