<?php

class fieldQuerySoilAccordian {
   
    public static function showAccordion($field_id) {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');

        $accordionArray[1]['name'] = 'Manure';
        $accordionArray[2]['name'] = 'Fertilizer';
        $accordionArray[3]['name'] = 'Lime';
        $accordionArray[4]['name'] = 'Soil Sample';
        $accordionArray[5]['name'] = 'Tissue Sample';
        $accordionArray[6]['name'] = 'Tile ';
      
        $accordion = new AccordionImplementationFieldQuerySoilForms($field_id,'individual');
        $accordion->setCSS('accordionNarrowBovineQuery');
        return $accordion->render($accordionArray);
    }

}

//end class


/* supports both individual and group */

class AccordionImplementationFieldQuerySoilForms extends AccordionSub {

    public static $field_id;
    public static $mode;
    public $param; //stores variables for QF2 forms.

    /* in case of individual cow treatments, we use field_id. */

    public function __construct($field_id = null,$mode) {

        
        
        // parent::__construct(); ddoes not work?

          $this->param = new stdClass(); //sets up QF2 group forms to only allow animals who have a medical case open to be enetered. OR an individual specific bovine on query page.
        
          //setup paramaters based on which mode the accordian is. 
          switch ($mode) {
    case 'individual':
            if (empty($field_id)) {   throw new Exception('Error field_id cannnot be undefined.'); }
            self::$field_id = $field_id;
            self::$mode = 'individual';
            $this->param->aniNumber = null;
            $this->param->field_id = $field_id;
        break;
    case 'list':
            self::$field_id = null;
            self::$mode = 'list';
            $this->param->field_id = null;
            $this->param->aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase(); //pass along list of animals to show for group. this case open medical cases
        break;
    case 'group':
        throw new Exception('Group mode not supported for this accordion.');
        break;
    default:
        throw new Exception('Mode Not known, likely spelled incorrectly or undefined.');
        break;
   
}
            
       
    }

    //manure
    function tab1() {

        return (new Manure())->addNewManureLoadsEventQuickform('default', self::$field_id);
        
        //cover both group and individual case. 

      //  $a = new QF2_GeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
      //  return($a->renderBox('QF2_GeneralTreatment', 'Select Animal to Treat'));
    }

    //fertilzier 
    function tab2() {
        
        
        return (new Field())->addNewFertilizerEventQuickform('default', self::$field_id);
        
        //group only 
       // $a = new QF2_MastitisQuickTreat((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
       // return ($a->renderBox('QF2_MastitisQuickTreat', 'Mastitis Event'));
    }

    function tab3() {

        $lime = new QF2_LimeEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
         $out[]=  $lime->renderBox('QF2_LimeEvent', 'Enter Field(s) Lime Events');
          $out[]= $lime->formJS();
          return implode($out);
       
        
        //$a = new QF2_MedicalTemperature((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
        //return $a->renderBox('QF2_MedicalTemperature', 'Record Animals Temperature');
    }

    function tab4() {

        //$a = new QF2_Ketone((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
        //return $a->renderBox('QF2_Ketone', 'Record Animals Ketone Level');
    }

    function tab5() {

        //$a = new QF2_Magnet((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
        //return $a->renderBox('QF2_Magnet', 'Record Magnet Bolus');
    }

    function tab6() {

        //$a = new QF2_MedicalComment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
        //return $a->renderBox('QF2_MedicalComment', 'Medical Comment');
    }

   
}//end class