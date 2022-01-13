<?php 
/*
Page to show video camera feeds
*/
class VideoInside extends TabPage {

 use videoPlumbing; 

 var $camera;

 //fill class variables.
  function __construct() {
      
        // 6 max per page
          
            //
            $camera[0]['host']='cam-axis-sort-pen';
            $camera[0]['security']='https';
            $camera[0]['name']='Sort Pen';
            $camera[0]['resVideo']='1280x800';
            $camera[0]['resStatic']='640x480';
            $camera[0]['type']='newer';
             //
            $camera[1]['host']='cam-axis-manure-auger';
            $camera[1]['security']='https';
            $camera[1]['name']='Cross Gutter';
            $camera[1]['resVideo']='1280x800';
            $camera[1]['resStatic']='640x400';
            $camera[1]['type']='m3204';         
            //
            $camera[2]['host']='cam-axis-maternityhigh';
            $camera[2]['security']='https';
            $camera[2]['name']='Maternity New';
            $camera[2]['resVideo']='1280x720';
            $camera[2]['resStatic']='640x360';
            $camera[2]['type']='newer';
            //
            $camera[3]['host']='cam-axis-99shop';
            $camera[3]['security']='https';
            $camera[3]['name']='Shop';
            $camera[3]['resVideo']='1600x1200';
            $camera[3]['resStatic']='640x360';
            $camera[3]['type']='newer';
            //
            $camera[4]['host']='cam-axis-sick-group';
            $camera[4]['security']='https';
            $camera[4]['name']='Sick Group';
            $camera[4]['resVideo']='1920x1080';
            $camera[4]['resStatic']='640x360';
            $camera[4]['type']='newer';
            
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
            $tabArray[6]['name'] = $this->camera[4]['name'];
         //   $tabArray[7]['name'] = $this->camera[5]['name'];
             
            $this->renderTabs($tabArray);
        }//end of local ip else 
    }//end function

 
   
    function tab1() {
        print($this->dynamicFeedsHTML($this->camera));
    }

    function tab2() {
        $obj = new VideoInside();
        print($obj->tabIndividualVideo($obj->camera[0]));
    }

    function tab3() {
        $obj = new VideoInside();
        print($obj->tabIndividualVideo($obj->camera[1]));
    }

    function tab4() {
        $obj = new VideoInside();
        print($obj->tabIndividualVideo($obj->camera[2]));
    }

    function tab5() {
        $obj = new VideoInside();
        print($obj->tabIndividualVideo($obj->camera[3]));
    }

    function tab6() {
        $obj = new VideoInside();
        print($obj->tabIndividualVideo($obj->camera[4]));
    }

    function tab7() {
        $obj = new VideoInside();
        print($obj->tabIndividualVideo($obj->camera[5]));
    }

}//end class