<?php 
/*
Page to show video camera feeds
*/
class VideoOther extends TabPage {

 use videoPlumbing; 

 var $camera;

 //fill class variables.
  function __construct() {
      
        // 6 max per page
            //
      
            $camera[0]['host']='cam-axis-151Bann';
            $camera[0]['security']='https';
            $camera[0]['name']='151 Bann';
            $camera[0]['resVideo']='1280x720';
            $camera[0]['resStatic']='640x360';
            $camera[0]['type']='m3204'; 
      
            $this->camera=$camera;
  }
 
	public function defaultDisplay() {
            
            if ($GLOBALS['MiscObj']->isPublicAddress($_SERVER['REMOTE_ADDR']) == true) { //here from internet
            print($this->staticFeedsHTML($this->camera));
        } else {  // here from local address
            $tabArray[1]['name'] = 'All Cameras';
            $tabArray[2]['name'] = $this->camera[0]['name'];
          //  $tabArray[3]['name'] = $this->camera[1]['name'];
          //  $tabArray[4]['name'] = $this->camera[2]['name'];
            $this->renderTabs($tabArray);
        }//end of local ip else 
    }//end function

    function tab1() {
        print($this->dynamicFeedsHTML($this->camera));
    }

    function tab2() {
        $obj = new VideoOther();
        print($obj->tabIndividualVideo($obj->camera[0]));
    }
    
    function tab3() {
        $obj = new VideoOther();
       // print($obj->tabIndividualVideo($obj->camera[1]));
    }
    
    function tab4() {
        $obj = new VideoOther();
      //  print($obj->tabIndividualVideo($obj->camera[2]));
    }
    
}//end class