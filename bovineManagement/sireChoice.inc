<?php 


include_once($_SERVER['DOCUMENT_ROOT'] . '/template/basePage.php');
include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryBreedingChoice.inc');

class SireChoice extends BasePage {

    private $sire_reg;

    public function defaultDisplay() {

        $this->loadVars((isset($_REQUEST['sire_reg']) ? $_REQUEST['sire_reg'] : null), $_REQUEST ['pageid']);

        $this->sireQuickForm();
        $this->main1();
    }

    private function loadVars($sire_reg = null, $pageid = null) {

        if ($sire_reg != null) {
            $this->sire_reg = $sire_reg;
        } elseif ((isset($_REQUEST['sire_reg']) == true) AND ( $_REQUEST['sire_reg'] != null)) { //fill class variables.
            $this->sire_reg = $_REQUEST['sire_reg'];
        }
    }

    function main1() {


        $this->displayAllCowInbreedingForSire($this->sire_reg);
    }

    public function SireInbreedingAllCows($request) {

        if ((isset($_REQUEST['item']) ? $_REQUEST['item'] : null) == null) {
            $results = array();
        } else {
            $sql = "SELECT  
cdn_progeny_potential_genetics.dam_full_reg_number,	cdn_progeny_potential_genetics.sire_full_reg_number,
            lpi,prodoll,	percent_inbreeding,local_number,full_name,bovinecurr.id as bovine_id,
            bovinemanagement.semen_code_to_short_name(primary_one) as sire_short_name_one,bovinemanagement.semen_code_to_short_name(primary_two) as sire_short_name_two,bovinemanagement.semen_code_to_short_name(primary_three) as sire_short_name_three,bovinemanagement.semen_code_to_short_name(secondary_one) as sire_short_name_sec_one
FROM batch.cdn_progeny_potential_genetics 
LEFT JOIN bovinemanagement.bovinecurr ON cdn_progeny_potential_genetics.dam_full_reg_number=full_reg_number
left JOIN bovinemanagement.service_picks_curr ON bovine_id=bovinecurr.id
WHERE cdn_progeny_potential_genetics.sire_full_reg_number='{$request['item']}' AND bovine_id is not null AND bovinemanagement.due_date(bovinecurr.id) is null
order by prodoll";

            $statement = $GLOBALS['pdo']->prepare($sql);
            $statement->execute();
            //get data
            $results = $statement->fetchAll(PDO::FETCH_ASSOC);
//print_r($results);
            //change presentation
            foreach ($results as &$row) {
                $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);

                $retArr = BovineQueryBreedingChoice::nextBreedingAction($row['bovine_id'], false);
                $row['breed_to'] = $retArr['nextActionTxt'];
            }
        }
        //output

        $output['data'] = $results;
        return ($output);
    }

    function displayAllCowInbreedingForSire($sire_reg) {
        ?>
        <table  class="display compact" id="displayAllCowInbreedingForSire"></table>
        <script type="text/javascript">
            "use strict";
            $(document).ready(function () {


                var displayAllCowInbreedingForSire = $('#displayAllCowInbreedingForSire').DataTable({
                    "ajax": "<?php  echo($GLOBALS['config']['HTTP']['JSON_API']); ?>136/SireInbreedingAllCows/<?php  echo($sire_reg) ?>",
                                "aaSorting": [[1, 'DESC']],
                                "columnDefs": [{type: 'natural', targets: [0]}], /*natual sorting on column 1*/
                                "iDisplayLength": 1000,
                                "columns": [
                                    {
                                        data: "local_number",
                                        title: "Animal"
                                    },
                                    {
                                        data: "prodoll",
                                        title: "Pro$"
                                    },
                                    {
                                        data: "lpi",
                                        title: "LPI"
                                    },
                                    {
                                        data: "percent_inbreeding",
                                        title: "Selected Sire Inbreeding %"
                                    }
                                    ,
                                    {
                                        data: "sire_short_name_one",
                                        title: "Sire #1"
                                    },
                                    {
                                        data: "sire_short_name_sec_one",
                                        title: "Sire #4"
                                    },
                                    {
                                        data: "breed_to",
                                        title: "Next Repro Sire"
                                    }
                                ]

                            });

                            setInterval(function () {
                                displayAllCowInbreedingForSire.ajax.reload(null, false); // user paging is not reset on reload
                            }, 30000);
                        });
        </script>

        <?php 
    }

    function sireQuickForm() {

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
        $attrs = array('onchange' => "javascript:location.href='?sire_reg='+this.options[this.selectedIndex].value+'&pageid={$_REQUEST['pageid']}';$('#example > ul').tabs('select', 0);");

        //javascript to allow submission when enter is pressed within select form
        //$attrs= array('onKeyPress' => "javascript:if(event.keyCode=='13'){document.frmTest.submit();}");

        $form = new HTML_QuickForm("frmSireQuery", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickformtest', 'Select a Sire');
        $form->addElement('select', 'sireRegNum', '', $sireRegNumber, $attrs);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //defaults
        if ($this->sire_reg != null) {
            $form->setDefaults(array('sireRegNum' => $this->sire_reg)); //set current sire.
        } else {
            $form->setDefaults(array('sireRegNum' => 0)); //set default to no variety.
        }


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $this->loadVars($form->exportValue('sireRegNum'), $form->exportValue('pageid'));
            $pageid = $form->exportValue('pageid');
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&sire_reg={$_REQUEST['sire_reg']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
        return $form->exportValue('sireRegNum'); // return what sire_reg was selected.
    }

    function error() {
        
    }

}

//end class
?>