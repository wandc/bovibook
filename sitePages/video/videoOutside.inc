<?php 
/*
Page to show video camera feeds
*/

class VideoOutside extends TabPage {
 use videoPlumbing; 

 var $camera;

 //fill class variables.
  function __construct() {
      
        // 6 max per page
            //
      
            $camera[0]['host']='cam-axis-parlor-front';
            $camera[0]['security']='https';
            $camera[0]['name']='Parlor Front';
            $camera[0]['resVideo']='1280x800';
            $camera[0]['resStatic']='640x480';     
            $camera[0]['type']='newer';
             //
            $camera[1]['host']='cam-axis-parlor-outside-back';
            $camera[1]['security']='https';
            $camera[1]['name']='Parlor Outside Back';
            $camera[1]['resVideo']='1280x720';
            $camera[1]['resStatic']='640x360';
            $camera[1]['type']='newer';
             //
            $camera[2]['host']='cam-axis-silage-pad-pole';
            $camera[2]['security']='https';
            $camera[2]['name']='Silage Pad Pole';
            $camera[2]['resVideo']='2048x2048';
            $camera[2]['resStatic']='640x640';
            $camera[2]['type']='cropFishEyeLarge';
            //
            $camera[3]['host']='CAM-axis-electrical-hut';
            $camera[3]['security']='https';
            $camera[3]['name']='Electrical Hut';
            $camera[3]['resVideo']='1920x1080';
            $camera[3]['resStatic']='640x480';     
            $camera[3]['type']='newer';
            
            
            $this->camera=$camera;
      
  }
 
	public function defaultDisplay() {
            
            if ($GLOBALS['MiscObj']->isPublicAddress($_SERVER['REMOTE_ADDR']) == true) { //here from internet
            print($this->staticFeedsHTML($this->camera));
        } else {  // here from local address
            $tabArray[1]['name'] = 'All Cameras';
            $tabArray[2]['name'] = $this->camera[0]['name'];
            $tabArray[3]['name'] = $this->camera[1]['name'];
            $tabArray[4]['name'] = $this->camera[2]['name'];
            $tabArray[5]['name'] = $this->camera[3]['name'];
           
         
            $this->renderTabs($tabArray);
        }//end of local ip else 
    }//end function

    function tab1() {
        print($this->dynamicFeedsHTML($this->camera));
    }

    function tab2() {
        $obj = new VideoOutside();
        print($obj->tabIndividualVideo($obj->camera[0]));
    }

    function tab3() {
        $obj = new VideoOutside();
        print($obj->tabIndividualVideo($obj->camera[1]));
    }

    function tab4() {
        $obj = new VideoOutside();
        print($obj->tabIndividualVideo($obj->camera[2]));
    }

  

}//end class


  

?>