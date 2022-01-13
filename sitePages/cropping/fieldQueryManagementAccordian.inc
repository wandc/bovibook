<?php

class fieldQueryManagementAccordian {
    
    public static function showAccordion($field_id) {
        
        //NOTE: array numbers matter, don't change without changing tab functions.
        $accordionArray[1]['name'] = 'Add New';
        $accordionArray[1]['name'] = 'Field Paramater';
        $accordionArray[2]['name'] = 'Field Border';
        
        
      
        $accordion = new AccordionImplementationFieldQuerySurfaceForms($field_id,'individual');
        $accordion->setCSS('accordionNarrowBovineQuery');
        return $accordion->render($accordionArray);
    }

}

//end class


/* supports both individual and group */

class AccordionImplementationFieldQueryManagementForms extends AccordionSub {

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

    function tab1() {

        //cover both group and individual case. 

        return (new Field())->addNewFieldParamQuickform('default', $this->field_id);
        
       // $a = new QF2_GeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
       // return($a->renderBox('QF2_GeneralTreatment', 'Select Animal to Treat'));
    }

    function tab2() {
        
         {$x->addNewFieldBorderQuickform('default', $this->field_id)}
                <hr/>
              
        //group only 
        //$a = new QF2_MastitisQuickTreat((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
        //return ($a->renderBox('QF2_MastitisQuickTreat', 'Mastitis Event'));
    }

     function tab3() {
        //group only 
        //$a = new QF2_MastitisQuickTreat((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'field_id' => self::$field_id)), self::$mode, $this->param); //group
        //return ($a->renderBox('QF2_MastitisQuickTreat', 'Mastitis Event'));
    }
}

//end class

