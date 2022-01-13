<?php

class bovineQueryReproductionAccordian {
    
    
    function showAccordion($bovine_id) {

        $accordionArray[1]['name'] = 'Heat';
        $accordionArray[2]['name'] = 'Hormone';
        $accordionArray[3]['name'] = 'To Breed';
        $accordionArray[4]['name'] = 'Comment';
        $accordionArray[5]['name'] = 'Vet Comment';
        $accordionArray[6]['name'] = 'Kamar';
        $accordionArray[7]['name'] = 'Preg Check';
        $accordionArray[8]['name'] = 'Protocol';
        $accordionArray[9]['name'] = 'Breeding Ban';

        $accordion = new AccordionImplementationReproduction($bovine_id,'individual');
        $accordion->setCSS('accordionNarrow');
        return $accordion->render($accordionArray);
    }
    
    public function error(){}
    
    }//end class
    
    /** shows reproduction quickforms * */
class AccordionImplementationReproduction extends AccordionSub {

    public static $bovineID;
    public static $mode;
    public $param; //stores variables for QF2 forms.

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct($bovineID = null,$mode) {
        
        
          $this->param = new stdClass(); //sets up QF2 group forms to only allow animals who have a medical case open to be enetered. OR an individual specific bovine on query page.
        
          //setup paramaters based on which mode the accordian is. 
          switch ($mode) {
    case 'individual':
            if (empty($bovineID)) {   throw new Exception('Error bovineID cannnot be undefined.'); }
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
            $this->param->aniNumber = null;
            $this->param->bovineID = $bovineID;
        break;
    case 'group':   //NOT USED
            self::$bovineID = null;
            self::$mode = 'group';
            $this->param->bovineID = null;
            $this->param->aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines(); 
        break;
    default:
        throw new Exception('ReproAccordian Mode Not known, likely spelled incorrectly or undefined.');
        break;
          } 
        
    }

    function tab1() {

        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusHeats.inc');
        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        $param->aniNumber = null;
        $a = new QF2_EstrusHeats((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $_REQUEST['bovine_id'])), 'individual', $param);
        return($a->renderBox('qf_heatsEntry', 'Record Heat Event'));
    }

    function tab2() {

        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusHormone.inc');

        $a = new QF2_HormoneInjection((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => self::$bovineID)), self::$mode, $this->param); //group
        return ($a->renderBox('QF2_HormoneInjection', 'Repro Hormone Injection'));
    }

    function tab3() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusBreedings.inc');
        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        $param->aniNumber = null;
        $a = new QF2_EstrusBreedingsMarkCowToBeBred((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $_REQUEST['bovine_id'])), 'individual', $param);
        return($a->renderBox('qf2_BreedingsCowToBeBred', 'Animal to be Bred'));
    }

    //repro comment
    function tab4() {
       require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/customComment.inc');
        /* XXX */
        $param= new stdClass();
        $param->type= 'reproductive';
        $param->bovineID= self::$bovineID;
        $param->aniNumber = null;
        $a=new QF2_CustomComment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' =>  self::$bovineID)), 'individual', $param);
        return($a->renderBox('QF2_Comment_Repro','Repro Comment'));  
    }
    
    //repro vet comment
     function tab5() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/customComment.inc');
        /* XXX */
        $param= new stdClass();
        $param->type= 'reproductive_vet';
        $param->bovineID= self::$bovineID;
        $param->aniNumber = null;
        $a=new QF2_CustomComment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' =>  self::$bovineID)), 'individual', $param);
        return($a->renderBox('QF2_Comment_Repro_Vet','Repro Vet Comment'));  
        
        
    }

    function tab6() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusKamar.inc');
        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        $k = new QF2_EstrusKamar((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $_REQUEST['bovine_id'])), 'individual', $param);
        return($k->renderBox('qf2_kamarEntry', 'Record Kamar Event'));
    }

    function tab7() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusPregnancyCheck.inc');
        
         $a = new QF2_PregCheckEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => self::$bovineID)), self::$mode, $this->param); //group
        return ($a->renderBox('QF2_PregCheckEvent', 'Add Preg Check Event'));
        
    }
    
    function tab8() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusProtocol.inc');
        $param= new stdClass();
        $param->type= 'reproductive';
        $param->bovineID= self::$bovineID;
        $param->aniNumber = null;
        $a=new QF2_Protocol((object) array('forwardURLArr'=>(new Quickform2Misc)->forwardURLDefaults()),'individual',$param);
        return($a->renderBox('QF2_Protocol_Repro','Repro Protocol'));  

    }

    function tab9() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusBreedings.inc');
        $param = new stdClass();
        $param->bovineID = self::$bovineID;
        $param->aniNumber = null;
        $a = new QF2_BreedingBan((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $_REQUEST['bovine_id'])), 'individual', $param);
        return($a->renderBox('qf2_QF2_BreedingBan', 'Ban From Breeding'));
    }
    
    
}
//end class