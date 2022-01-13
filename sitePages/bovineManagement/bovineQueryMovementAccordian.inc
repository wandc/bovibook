<?php

class bovineQueryMovementAccordian {
    
     function showAccordion($bovine_id) {

        $accordionArray[1]['name'] = 'Movement';
        $accordionArray[2]['name'] = 'Sort';
        $accordionArray[3]['name'] = 'Potential Cull';
        $accordionArray[4]['name'] = 'Feet';
        $accordionArray[5]['name'] = 'Ear Tag';
        $accordionArray[6]['name'] = 'Picture';
        $accordionArray[7]['name'] = 'Schedule Dry Off';

        $accordion = new AccordionImplementationMovement($bovine_id);
        $accordion->setCSS('accordionNarrow');
        return $accordion->render($accordionArray);
    }
    
        public function error(){}
}


class AccordionImplementationMovement extends AccordionSub {

    public static $bovineID;
    public static $mode;

    /* in case of individual cow items, we use bovine_id. */

    public function __construct($bovineID = null) {

        if ($bovineID != null) {
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
        } else {
            self::$bovineID = null;
            self::$mode = 'group';
        }
    }

    function tab1() {
        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        $a = new QF2_MoveAnimal((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $_REQUEST['bovine_id'])), self::$mode, $param);
        return ($a->renderBox('qf2_MoveAnimal', 'Move Animal to new Group'));
       
    }

    function tab2() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementSortGate.inc');

        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        $a = new QF2_SortGate((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $_REQUEST['bovine_id'])), self::$mode, $param);
        return ($a->renderBox('QF2_SortGate', 'Mark Animal to be Sorted'));
      
    }

    function tab3() {

        return("<h2>TODO:Put potential cull here</h2>");
    }

    function tab4() {

        return("<h2>TODO:Put Feet here</h2>");
    }

    function tab5() {

        return("<h2>TODO:Put Ear Tags</h2>");
    }
    
     function tab6() {

        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reports/bovinePicture.inc');
        $a = new QF2_UploadPicture((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'default',$param);
        return ($a->renderBox('QF2_UploadPicture', 'Upload Animal Photo'));
       
    }
    
     function tab7() {
          require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/transition/transitionDryoff.inc');
        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        $param->aniNumber = null;
        $QF2_DryOffSchedule = new QF2_DryOffSchedule((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $param->bovineID)), 'individual', $param); //group
        return $QF2_DryOffSchedule->renderBox('QF2_DryOffSchedule', 'Schedule Cow to Start Dy Off Protocol');
       
    }
    
    
}

//end class