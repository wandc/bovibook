<?php

header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");
require_once('../global.php');

/**
  Page to show axis camera camera video feed pictures for internet site visitors
 */
/*
 * Call this directly to see if there are errors:
 * {$GLOBALS['config']['HTTP']['URL_SHORT']}functions/displayVideoImage.php?host=cam-axis-calf-pend&security=https 
 * 
 * 
 */
class displayVideoImage {

    public function __construct() {

        //inital check it's not junk 
        if (filter_input(INPUT_GET, 'link', FILTER_SANITIZE_URL) == true) {
            print("Invalid URL.<br/>");
            error_log("Invalid URL", 0);
            return -1;
        }

        //filter to make safe
        $safePost = filter_input_array(INPUT_GET, [
            "host" => FILTER_SANITIZE_STRING,
            "security" => FILTER_SANITIZE_STRING
        ]);
        //create a camera URL based of of host name. 
        $finalDNS = $safePost['host'] . '.' . $GLOBALS['config']['HTTP']['ROOT_DNS'];

        $remoteImage = $safePost['security'] . '://' . $finalDNS . '/jpg/1/image.jpg?date=1&clock=1';
        $this->savePhotoToStr($remoteImage, $safePost['host']);

        //error_log("-----------------DEBUG IMAGE URL $remoteImage", 0);
    }

    private function savePhotoToStr($remoteImage, $host) {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $remoteImage);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 0);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false); //makes the connection insecure again..... local site connection...we don't care and it also just shows an image.
        curl_setopt($ch, CURLOPT_TIMEOUT, 3);
        $fileContents = curl_exec($ch);
        if (curl_exec($ch) === false) {
            error_log('Curl error: ' . curl_error($ch));
        }
        curl_close($ch);

        //nothing found...camera is down
        //make an error image.
        if (empty($fileContents)) {


            //set up image
            $height = 200;
            $width = 200;

            $im = imagecreatetruecolor($width, $height);
            $white = imagecolorallocate($im, 255, 255, 255);
            $blue = imagecolorallocate($im, 0, 0, 255);

            //draw on image
            imagefill($im, 0, 0, $blue);
            imageline($im, 0, 0, $width, $height, $white);
            imagestring($im, 2, 50, 150, 'Not Working', $white); //ERROR Text
            imagestring($im, 2, 2, 15, $host, $white);

            //output image
            Header('Content-type: image/jpg');
            imagejpeg($im);

            //clean up
            imagedestroy($im);
        } else {

            //show the image

            $newImg = imagecreatefromstring($fileContents);

            // Set the content type header - in this case image/jpeg
            header('Content-type: image/jpeg');

            // Output the image to browser
            imagejpeg($newImg);

            // Free up memory
            imagedestroy($newImg);
        }
    }

}

//end class

new displayVideoImage();