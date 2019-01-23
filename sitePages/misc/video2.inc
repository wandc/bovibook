<?php 
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');

require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
/*
Page to show video camera feeds
*/

class Video2 extends TabPage {

	

	public function defaultDisplay() {

   
	  //need the http in front to make it a url.
if ($GLOBALS['MiscObj']->isPublicAddress($_SERVER['REMOTE_ADDR']) ==true) {
print("<h3>You are here from the internet and thus only get static pictures, no video, to save bandwidth.</h3>");
  
          $str = <<<EOD
    <div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.80" /></div>
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.83" /></div>
    </div>
                  
     <div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.84" /></div>
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.88" /></div>
    </div>
                 
     <div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.89" /></div>
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.93" /></div>
    </div>
                  
     <div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.94" /></div>
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.95" /></div>
    </div>
                  
    <div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage" src="/functions/displayVideoImage.php?ip=192.168.9.96" /></div>
      <div class="col-md-6"><img class="img-responsive" id="staticVideoImage"  /></div>
    </div>
EOD;
print($str);
}
else 
  
    {
 //local network show video. 
      
    $tabArray[1]['name'] = 'All Cameras';
    $tabArray[2]['name'] = 'Cross Gutter';   
    $tabArray[3]['name'] = 'Parlor Entrance';
    $tabArray[4]['name'] = 'Parlor Pit';
    $tabArray[5]['name'] = 'Heifer Barn';
    $tabArray[6]['name'] = 'Parlor Front';
    $tabArray[7]['name'] = 'Silage Pad North';
    $tabArray[8]['name'] = 'Red Barn Inside';
    $tabArray[9]['name'] = 'Grey Barn CL-10';
    $tabArray[10]['name'] = 'Silage Pad South';
    $this->renderTabs($tabArray);

   
   }//end of local ip else 

	
}//end function


 
   
    function tab1() {
        self::showVideoFeeds();
        }
    function tab2() {
        print('<img src="http://192.168.9.80/axis-cgi/mjpg/video.cgi?resolution=1280x1024&dummy='.strtotime("now").'"/>'."\n\r");
        }  
    function tab3() {
          print('<img src="http://192.168.9.83/axis-cgi/mjpg/video.cgi?resolution=1280x1024&dummy='.strtotime("now").'"/>'."\n\r");
        }
    function tab4() {
          print('<img src="http://192.168.9.84/axis-cgi/mjpg/video.cgi?resolution=1280x1024&dummy='.strtotime("now").'"/>'."\n\r");
        }  
     function tab5() {
       	 print('<img src="http://192.168.9.88/axis-cgi/mjpg/video.cgi?resolution=1280x800&dummy='.strtotime("now").'"/>'."\n\r");   //Axis M3004-V
      }
     function tab6() {
       	 print('<img src="http://192.168.9.89/axis-cgi/mjpg/video.cgi?resolution=1280x800&dummy='.strtotime("now").'"/>'."\n\r");   //Axis M3004-V
      }	 	  
      function tab7() {
       	 print('<img src="http://192.168.9.93/mjpg/1/video.mjpg?timestamp='.strtotime("now").'"/>'."\n\r");   //Axis M3044-V
      }	
      function tab8() {
       	 print('<img src="http://192.168.9.94/mjpg/1/video.mjpg?timestamp='.strtotime("now").'"/>'."\n\r");   //Axis M3044-V
      }	    
      function tab9() {
       	 print('<img src="http://192.168.9.95/mjpg/1/video.mjpg?timestamp='.strtotime("now").'"/>'."\n\r");   //Axis M3044-V
      }	
      function tab10() {
       	 print('<img src="http://192.168.9.96/mjpg/1/video.mjpg?timestamp='.strtotime("now").'"/>'."\n\r");   //Axis M3044-V
      }	




/**
 * show all feeds on one page.
 */
function showVideoFeeds() {
        $time=strtotime("now");
    $str = <<<EOD
<div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" src="http://192.168.9.80/axis-cgi/mjpg/video.cgi?resolution=640x480&dummy=$time"/></div>
      <div class="col-md-6"> <img class="img-responsive" src="http://192.168.9.83/axis-cgi/mjpg/video.cgi?resolution=640x480&dummy=$time"/></div>
</div>
            
<div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" src="http://192.168.9.84/axis-cgi/mjpg/video.cgi?resolution=640x480&dummy=$time"/></div>
      <div class="col-md-6"> <img class="img-responsive" src="http://192.168.9.88/axis-cgi/mjpg/video.cgi?resolution=640x480&dummy=$time"/></div>
</div>
     
<div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" src="http://192.168.89/axis-cgi/mjpg/video.cgi?resolution=640x480&dummy=$time"/></div>
      <div class="col-md-6"> <img class="img-responsive" src="http://192.168.9.93/axis-cgi/mjpg/video.cgi?resolution=640x360&dummy=$time"/></div>
</div>
            
<div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" src="http://192.168.94/axis-cgi/mjpg/video.cgi?resolution=640x360&dummy=$time"/></div>
      <div class="col-md-6"> <img class="img-responsive" src="http://192.168.9.95/axis-cgi/mjpg/video.cgi?resolution=640x360&dummy=$time"/></div>
</div>
            
<div class="row show-grid">
      <div class="col-md-6"><img class="img-responsive" src="http://192.168.96/axis-cgi/mjpg/video.cgi?resolution=640x360&dummy=$time"/></div>  
      <div class="col-md-6"><img class="img-responsive" /></div>  
</div>
EOD;
print($str);
}//end function




/**
 * display axis 209 MFD camera according to official method.
 * We don't use this as it seems to screw up the tabs.
 */
function showAxis209MFDFeed($IPv4,$width=640,$height=480) {
?>
<SCRIPT LANGUAGE="JavaScript">
// Set the BaseURL to the URL of your camera
var BaseURL = "http://<?php echo($IPv4);?>/";

// DisplayWidth & DisplayHeight specifies the displayed width & height of the image.
// You may change these numbers, the effect will be a stretched or a shrunk image
var DisplayWidth = "640";
var DisplayHeight = "480";

// This is the path to the image generating file inside the camera itself
var File = "axis-cgi/mjpg/video.cgi?resolution=<?php echo($width);?>x<?php echo($height);?>";
// No changes required below this point
var output = "";
if ((navigator.appName == "Microsoft Internet Explorer") &&
   (navigator.platform != "MacPPC") && (navigator.platform != "Mac68k"))
{
  // If Internet Explorer under Windows then use ActiveX 
  output  = '<OBJECT ID="Player" width='
  output += DisplayWidth;
  output += ' height=';
  output += DisplayHeight;
  output += ' CLASSID="CLSID:DE625294-70E6-45ED-B895-CFFA13AEB044" ';
  output += 'CODEBASE="';
  output += BaseURL;
  output += 'activex/AMC.cab#version=5,4,17,5">';
  output += '<PARAM NAME="MediaURL" VALUE="';
  output += BaseURL;
  output += File + '">';
  output += '<param name="MediaType" value="mjpeg-unicast">';
  output += '<param name="ShowStatusBar" value="0">';
  output += '<param name="ShowToolbar" value="0">';
  output += '<param name="AutoStart" value="1">';
  output += '<param name="StretchToFit" value="1">';
  // Remove the // for the ptz settings below to use the code for click-in-image. 
     //  output += '<param name="PTZControlURL" value="';
     //  output += BaseURL;
     //  output += '/axis-cgi/com/ptz.cgi?camera=1">';
     //  output += '<param name="UIMode" value="ptz-relative">'; // or "ptz-absolute"
  output += '<BR><B>Axis Media Control</B><BR>';
  output += 'The AXIS Media Control, which enables you ';
  output += 'to view live image streams in Microsoft Internet';
  output += ' Explorer, could not be registered on your computer.';
  output += '<BR></OBJECT>';
} else {
  // If not IE for Windows use the browser itself to display
  theDate = new Date();
  output  = '<IMG SRC="';
  output += BaseURL;
  output += File;
  output += '&dummy=' + theDate.getTime().toString(10);
  output += '" HEIGHT="';
  output += DisplayHeight;
  output += '" WIDTH="';
  output += DisplayWidth;
  output += '" ALT="Camera Image">';
}
document.write(output);
document.Player.ToolbarConfiguration = "play,+snapshot,+fullscreen"

// Remove the // below to use the code for Motion Detection. 
  // document.Player.UIMode = "MDConfig";
  // document.Player.MotionConfigURL = "/axis-cgi/operator/param.cgi?ImageSource=0"
  // document.Player.MotionDataURL = "/axis-cgi/motion/motiondata.cgi";
</SCRIPT>
<?php 
}//end function







}//end class
?>
