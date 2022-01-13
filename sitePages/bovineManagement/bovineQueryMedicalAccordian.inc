<?php

class bovineQueryMedicalAccordian {
    /*
     * different, used for mdeical case page.
     */

    public function showMedicalCaseAccordion() {

        $accordionArray[1]['name'] = 'General Treatment';
        $accordionArray[2]['name'] = 'Mastitis Treatment';
        $accordionArray[3]['name'] = 'Temperature';
        $accordionArray[4]['name'] = 'Ketone';
        $accordionArray[5]['name'] = 'Magnet';
        $accordionArray[6]['name'] = 'Comment';
        $accordionArray[7]['name'] = 'Vet Comment';
        $accordionArray[8]['name'] = 'Vet Check';
        $accordionArray[9]['name'] = 'Schedule Med';
        $accordionArray[10]['name'] = 'Schedule Action';
        $accordionArray[11]['name'] = 'Protocol';
        $accordionArray[12]['name'] = 'DeHorn';
        $accordionArray[13]['name'] = 'Diagnosis';
        $accordionArray[14]['name'] = 'Open/Close Case';
        $accordionArray[15]['name'] = 'CMT';


        $accordion = new AccordionImplementationMedicalQuickforms(null, 'list');
        $accordion->setCSS('accordionNarrow');
        return $accordion->render($accordionArray);
    }

    public static function showAccordion($bovine_id, $extra) {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');

        //NOTE: array numbers matter, don't change without changing tab functions.
        $accordionArray[1]['name'] = 'General Treatment';
        $accordionArray[2]['name'] = 'Mastitis Treatment';
        $accordionArray[3]['name'] = 'Temperature';
        $accordionArray[4]['name'] = 'Ketone';
        $accordionArray[5]['name'] = 'Magnet';
        $accordionArray[6]['name'] = 'Comment';
        $accordionArray[7]['name'] = 'Vet Comment';
        $accordionArray[8]['name'] = 'Vet Check';
        $accordionArray[9]['name'] = 'Schedule Med';
        $accordionArray[10]['name'] = 'Schedule Action';
        $accordionArray[11]['name'] = 'Protocol';
        $accordionArray[12]['name'] = 'DeHorn';
        $accordionArray[13]['name'] = 'Diagnosis';
        $accordionArray[14]['name'] = 'Open/Close Case';
        $accordionArray[15]['name'] = 'CMT';

        //$accordionArray[10]['name'] = 'Body Cond. Score'; //FIXME, make seperate accordian.
        //$accordionArray[11]['name'] = 'Feet'; //FIXME, make seperate accordian.
        //$accordionArray[12]['name'] = 'Ear Tag'; //FIXME, make seperate accordian.

        $accordion = new AccordionImplementationMedicalQuickforms($bovine_id, 'individual', $extra);
        $accordion->setCSS('accordionNarrowBovineQuery');
        return $accordion->render($accordionArray);
    }

    public function error() {
        
    }

}

//end class


/* supports both individual and group */

class AccordionImplementationMedicalQuickforms extends AccordionSub {

    public $bovineID;
    public $mode;
    public $paramLocal; //stores variables for QF2 forms.
    public $extra;

    /* in case of individual cow treatments, we use bovine_id. */

    public function __construct($bovineID = null, $mode, $extra = null) {

        new MedicalCase(); //this is called here so temp table is created.

        $this->extra = $extra;
        // parent::__construct(); ddoes not work?

        $this->paramLocal = new stdClass(); //sets up QF2 group forms to only allow animals who have a medical case open to be enetered. OR an individual specific bovine on query page.
        //setup paramaters based on which mode the accordian is. 
        switch ($mode) {
            case 'individual':
                if (empty($bovineID)) {
                    throw new Exception('Error bovineID cannnot be undefined.');
                }
                $this->bovineID = $bovineID;
                $this->mode = 'individual';
                $this->paramLocal->aniNumber = null;
                $this->paramLocal->bovineID = $bovineID;
                break;
            case 'list':
                $this->bovineID = null;
                $this->mode = 'list';
                $this->paramLocal->bovineID = null;
                $this->paramLocal->aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase(); //pass along list of animals to show for group. this case open medical cases
                break;
            case 'group':
                throw new Exception('Group mode not supported for this accordion.');
                break;
            case 'calf':        //NOT USED AS OF YET. would be for calf group page.
                $this->bovineID = null;
                $this->mode = 'calf';
                $this->paramLocal->bovineID = null;
                $this->paramLocal->aniNumber = $GLOBALS['MiscObj']->createListOfAllYoungCalves();
                break;
            default:
                throw new Exception('Mode Not known, likely spelled incorrectly or undefined.');
                break;
        }
    }

    function tab1() {

        $a = new QF2_GeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return($a->renderBox('QF2_GeneralTreatment', 'Select Animal to Treat'));
    }

    function tab2() {

        $a = new QF2_MastitisQuickTreat((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return ($a->renderBox('QF2_MastitisQuickTreat', 'Mastitis Event'));
    }

    function tab3() {

        $a = new QF2_MedicalTemperature((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return $a->renderBox('QF2_MedicalTemperature', 'Record Animals Temperature');
    }

    function tab4() {

        $a = new QF2_Ketone((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return $a->renderBox('QF2_Ketone', 'Record Animals Ketone Level');
    }

    function tab5() {

        $a = new QF2_Magnet((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return $a->renderBox('QF2_Magnet', 'Record Magnet Bolus');
    }

    function tab6() {

        $a = new QF2_MedicalComment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return $a->renderBox('QF2_MedicalComment', 'Medical Comment');
    }

    //repro vet comment
    function tab7() {
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/customComment.inc');
        /* XXX */
        /*  $param= new stdClass();

          $param->bovineID= self::$bovineID;
          $param->aniNumber = null; */
        $this->paramLocal->type = 'medical_vet';
        $a = new QF2_CustomComment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return($a->renderBox('QF2_Comment_Medical_Vet', 'Medical Vet Comment'));
    }

    function tab8() {

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/vetToCheck.inc');
        $a = new QF2_VetToCheckEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return ($a->renderBox('qf2_VetToCheckEvent', 'Vet To Check'));
    }

    function tab9() {
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicineScheduled.inc');
            $a = new QF2_ScheduleSingleGeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
            return $a->renderBox('QF2_ScheduleSingleGeneralTreatment', 'Schedule Medicine');
        }
    }

    function tab10() {
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicineScheduled.inc');
            $a = new QF2_ScheduleAction((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
            return $a->renderBox('QF2_ScheduleAction', 'Schedule Medical Action');
        }
    }

    function tab11() {
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusProtocol.inc');

            if ($this->extra == 'replacement_animal') {

                $this->paramLocal->type = 'calf';
                $a = new QF2_Protocol((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
                return $a->renderBox('QF2_Protocol', 'Calf Medical Protocol');
            } else {
                $this->paramLocal->type = 'medical';
                $a = new QF2_Protocol((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
                return $a->renderBox('QF2_Protocol', 'Medical Protocol');
            }
        }
    }

    function tab12() {

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/groupTreatment.inc');
        $d = new QF2_Dehorn((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return($d->renderBox('qf2_Dehorn', 'Select Animals / Groups to Dehorn'));
    }

    function tab13() {


        $a = new QF2_MedicalDiagnosis((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return $a->renderBox('QF2_MedicalDiagnosis', 'Record New Medical Case Diagnosis');
    }

    function tab14() {
        //OPEN OR CLOSE medical case
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');



        $this->paramLocal->aniNumber = null;
        $openIndiv = new QF2_OpenMedicalCase((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        $closeIndiv = new QF2_CloseMedicalCase((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        //       
        //open
        $this->paramLocalOtherOpen = new stdClass();
        $this->paramLocalOtherOpen->aniNumber = MedicalCase::lookupAnimalsWhoDontHaveOpenMedicalCase();
        $this->paramLocalOtherOpen->bovineID = null;
        $openOther = new QF2_OpenMedicalCase((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocalOtherOpen);
        //close
        $this->paramLocal->aniNumber = MedicalCase::grabListCowsWithOpenMedicalCase(); //pass along list of animals to show for group. this case open medical cases
        $closeOther = new QF2_CloseMedicalCase((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);


        if ($this->mode == 'individual') {

            //open case
            if (is_numeric(MedicalCase::lookupOpenMedicalCaseForAnimal($this->bovineID, date('r', strtotime('now'))))) {
                return $closeIndiv->renderBox('QF2_CloseMedicalCase', 'Close Medical Case');
            }
            //close case
            else {

                return $openIndiv->renderBox('QF2_OpenMedicalCase', 'Open New Medical Case');
            }
        } else {

            return $openOther->renderBox('QF2_OpenMedicalCase', 'Open New Medical Case') . $closeOther->renderBox('QF2_CloseMedicalCase', 'Close Medical Case');
        }


        /*
          //group only
          $out[] = (new MedicalCase)->openNewMedicalCaseQuickForm();
          $out[] = ('<br/>' . "\n\r");
          $out[] = (new MedicalCase)->closeMedicalCaseQuickForm();
          return (implode($out));
         */
    }

    function tab15() {
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
        $a = new QF2_CMT((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->bovineID)), $this->mode, $this->paramLocal);
        return $a->renderBox('QF2_CMT', 'Record Animals Udder CMT');
    }

}

//end class