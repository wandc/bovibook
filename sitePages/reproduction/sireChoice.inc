<?php 


class SireChoice extends BasePage {

    private $sire_reg;

    public function defaultDisplay() {

        $this->loadVars((isset($_REQUEST['sire_reg']) ? $_REQUEST['sire_reg'] : null), $_REQUEST ['pageid']);

         $param= new stdClass();
         $param->sire_reg= $this->sire_reg;   
         $c = new QF2_SireChoice((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', $param); //group

         $out[]=($c->renderBox('QF2_SireChoice', 'Select a Sire'));

         $out[]=$this->displayAllCowInbreedingForSire($this->sire_reg);
         
         print(implode($out));
    }

    private function loadVars($sire_reg = null, $pageid = null) {

        if ($sire_reg != null) {
            $this->sire_reg = $sire_reg;
        } elseif ((isset($_REQUEST['sire_reg']) == true) AND ( $_REQUEST['sire_reg'] != null)) { //fill class variables.
            $this->sire_reg = filter_var($_REQUEST['sire_reg'], FILTER_SANITIZE_STRING);
        }
    }

    

    public function displayAllCowInbreedingForSire($sire_reg) {
            
        $resultsOut=array();
            
        if (empty($sire_reg)) {
            $rowOut = array();
        } else {
            $sql = "SELECT  
cdn_progeny_potential_genetics.dam_full_reg_number,	cdn_progeny_potential_genetics.sire_full_reg_number,
            lpi,prodoll,	percent_inbreeding,local_number,full_name,bovinecurr.id as bovine_id,
            bovinemanagement.semen_code_to_short_name(primary_one) as sire_short_name_one,
            bovinemanagement.semen_code_to_short_name(primary_two) as sire_short_name_two,
            bovinemanagement.semen_code_to_short_name(primary_three) as sire_short_name_three,
            bovinemanagement.semen_code_to_short_name(secondary_one) as sire_short_name_sec_one,
            bovinemanagement.semen_code_to_short_name(secondary_two) as sire_short_name_sec_two,
            bovinemanagement.semen_code_to_short_name(secondary_three) as sire_short_name_sec_three
FROM batch.cdn_progeny_potential_genetics 
LEFT JOIN bovinemanagement.bovinecurr ON cdn_progeny_potential_genetics.dam_full_reg_number=full_reg_number
left JOIN bovinemanagement.service_picks_curr ON bovine_id=bovinecurr.id
WHERE cdn_progeny_potential_genetics.sire_full_reg_number='$sire_reg' AND bovine_id is not null AND bovinemanagement.due_date(bovinecurr.id) is null
order by prodoll";

            $statement = $GLOBALS['pdo']->prepare($sql);
            $statement->execute();
            //get data
            $results = $statement->fetchAll(PDO::FETCH_ASSOC);
//print_r($results);
            //change presentation
            foreach ($results as &$row) {
                $retArr = (new BovineQueryBreedingChoice)->nextBreedingAction($row['bovine_id'], false);


                $rowOut['Animal'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'], $row['local_number'], $row['full_name']);

                $rowOut['Pro$'] = $row['prodoll'];
                $rowOut['Selected Sire Inbreeding %'] = $row['percent_inbreeding'];
                $rowOut['Sire #1'] = $row['sire_short_name_one'];
                $rowOut['Sire #2'] = $row['sire_short_name_two'];
                $rowOut['Sire #3'] = $row['sire_short_name_three'];
                $rowOut['Sire #4'] = $row['sire_short_name_sec_one'];
                $rowOut['Sire #5'] = $row['sire_short_name_sec_two'];
                $rowOut['Sire #6'] = $row['sire_short_name_sec_three'];
                $rowOut['Next Repro Sire'] = $retArr['nextActionTxt'];

                $resultsOut[] = $rowOut;
        }}
            //output
            $dataArr = $resultsOut;

            $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Inbreeding For Sire '.$sire_reg, $dataArr));
            return implode($out);
        }

        

    

    function error() {
        
    }

}//end class

Class QF2_SireChoice extends QF2_Base {
    
    function definition($form) {

        
         $sireRegNumber = array();

        //get an array of all sires we have semen for in the tank. NOT THE SAME AS THE SEMEN INVENTORY CODE.
        $sql = "SELECT DISTINCT full_reg_number,short_name,full_name,invoice_cost
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE
GROUP BY full_reg_number,short_name,full_name,invoice_cost
ORDER BY short_name";

        $res = $GLOBALS['pdo']->query($sql);
        $sireRegNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //only show if we have a values for, some day we can take this out when cost is required.
            if ($row['invoice_cost'] != null) {
                $invoice_cost = ' $' . $row['invoice_cost'];
            } else {
                $invoice_cost = null;
            }
            $sireRegNumber[trim($row['full_reg_number'])] = $row['short_name'] . ' - ' . $row['full_name'] . $invoice_cost;
        }

        //javascript to allow submission on change of select element.
          $form->addElement('select', 'sireRegNum', array('onChange' => "javascript:location.href='?sire_reg='+this.options[this.selectedIndex].value+'&pageid={$_REQUEST['pageid']}';$('#example > ul').tabs('select', 0);"), array('options' => $sireRegNumber, 'label' => 'Sire'))->addRule("required", "Please specify your sire.");        
        
                 //defaults
        if ($this->param->sire_reg != null) {
             $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('sireRegNum' => $this->param->sire_reg))); //set current sire.
        } else {
            $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('sireRegNum' => 0))); //set default to no variety.
        }
       
     
        return $form; //required       
    }

    //removes the submit button by overriding. 
    function customSubmit($form) {
        return $form;
    }
    
    
    function process($value) {
        //javascript handles process
    }

}