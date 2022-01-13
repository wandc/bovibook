<?php

/*
  Page to show video camera feeds
 */

class VideoCalfBarn extends TabPage {

    use videoPlumbing;

    var $camera;

    //fill class variables.
    function __construct() {

        // 6 max per page
        $camera[0]['host'] = 'cam-axis-calf-pend';
        $camera[0]['name'] = 'Calf Pen D';
        $camera[0]['security'] = 'https';
        $camera[0]['resVideo'] = '1280x1024';
        $camera[0]['resStatic'] = '640x480';
        $camera[0]['type'] = 'm3044';
        //
        $camera[1]['host'] = 'cam-axis-calf-penc';
        $camera[1]['name'] = 'Calf Pen C';
        $camera[1]['security'] = 'https';
        $camera[1]['resVideo'] = '1280x1024';
        $camera[1]['resStatic'] = '640x480';
        $camera[1]['type'] = 'm3044';
        //
        $camera[2]['host'] = 'cam-axis-calf-penb';
        $camera[2]['name'] = 'Calf Pen B';
        $camera[2]['security'] = 'https';
        $camera[2]['resVideo'] = '1280x1024';
        $camera[2]['resStatic'] = '640x480';
        $camera[2]['type'] = 'm3044';
        //
        $camera[3]['host'] = 'cam-axis-calf-pena';
        $camera[3]['name'] = 'Calf Pen A';
        $camera[3]['security'] = 'https';
        $camera[3]['resVideo'] = '1280x1024';
        $camera[3]['resStatic'] = '640x480';
        $camera[3]['type'] = 'm3044';
        //
        $camera[4]['host'] = 'cam-axis-99heifer-smallcalves';
        $camera[4]['name'] = '99 Heifer - Small Calves';
        $camera[4]['resVideo'] = '1280x1024';
        $camera[4]['security'] = 'https';
        $camera[4]['resStatic'] = '640x480';
        $camera[4]['type'] = 'm3044';
        //
        $camera[5]['host'] = 'cam-axis-99heifer-largecalves';
        $camera[5]['name'] = '99 Heifer - Large Calves';
        $camera[5]['resVideo'] = '1280x1024';
        $camera[5]['security'] = 'https';
        $camera[5]['resStatic'] = '640x480';
        $camera[5]['type'] = 'm3044';
        //


        $this->camera = $camera;
    }

    public function defaultDisplay() {



        if ($GLOBALS['MiscObj']->isPublicAddress($_SERVER['REMOTE_ADDR']) == true) { //here from internet
            print($this->staticFeedsHTML($this->camera));
        } else {  // here from local addres
            //local network show video. 
            $tabArray[1]['name'] = 'All Cameras';
            $tabArray[2]['name'] = $this->camera[0]['name'];
            $tabArray[3]['name'] = $this->camera[1]['name'];
            $tabArray[4]['name'] = $this->camera[2]['name'];
            $tabArray[5]['name'] = $this->camera[3]['name'];
            $tabArray[6]['name'] = $this->camera[4]['name'];
            $tabArray[7]['name'] = $this->camera[5]['name'];

            $this->renderTabs($tabArray);
        }//end of local ip else 
    }

//end function

    function tab1() {
        print($this->dynamicFeedsHTML($this->camera));
        //print(self::showVideoFeeds());
    }

    function tab2() {
        $obj = new VideoCalfBarn();
        print($obj->tabIndividualVideo($obj->camera[0]));
    }

    function tab3() {
        $obj = new VideoCalfBarn();
        print($obj->tabIndividualVideo($obj->camera[1]));
    }

    function tab4() {
        $obj = new VideoCalfBarn();
        print($obj->tabIndividualVideo($obj->camera[2]));
    }

    function tab5() {
        $obj = new VideoCalfBarn();
        print($obj->tabIndividualVideo($obj->camera[3]));
    }

    function tab6() {
        $obj = new VideoCalfBarn();
        print($obj->tabIndividualVideo($obj->camera[4]));
    }

    function tab7() {
        $obj = new VideoCalfBarn();
        print($obj->tabIndividualVideo($obj->camera[5]));
    }

}

//end class