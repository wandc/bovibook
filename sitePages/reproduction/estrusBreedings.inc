<?php 
class EstrusBreedings extends BasePage {

    public function defaultDisplay() {
        print("<div class='row'>"."\n\r");
       
        print("<div class='col-sm-4 col-sm-push-8'>"."\n\r");
        //$this->markCowToBeBredQuickForm();
       // $this->chooseSemenForBreedingQuickForm();
       //$this->markCowAsBredQuickForm();
        
        
        
        $a=new QF2_EstrusBreedingsMarkCowToBeBred((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('qf2_BreedingsCowToBeBred','Animal to be Bred'));     
        //
         $c=new QF2_EstrusBreedingsChooseSemen((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($c->renderBox('qf2_BreedingsChooseSemen','Choose Semen'));     
        //
        $c=new QF2_EstrusBreedingsMarkBred((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($c->renderBox('qf2_BreedingsMarkBred','Mark as Bred'));     
        
        print("</div>"."\n\r");
         print("<div class='col-sm-8 col-sm-pull-4'>"."\n\r");
        $this->displayFutureBreedingEvents();
        //$this->displayLastBreedingEvents();
        $this->displayLastBreedingEvents2();
        print("</div>"."\n\r");
        print("</div>"."\n\r");
    }

    public function summaryDisplay() {

        $this->displayLastBreedingEvents('summary');
    }

    public function milkTestInputsDisplay() {

        $this->displayLastBreedingEvents('milkTestInputs');
    }
    // Animal	Current Location	Estimated Breeding Time	Employee	Semen Selector	Sire Choice	Actual Breeding Time	Comment	AI Technician	Action
        
    function displayLastBreedingEvents2() {
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            }, 
               {
                data: "local_number",
                title: "Animal",
                type: "num-html"
        },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "event_time",
                title: "Breeding Time (or Scheduled Time)",
                type: "date",
            },
     
            {
                data: "semen_straw_choice_userid",
                title: "Semen Selector"
            },
        {
                data: "service_sire_full_name",
                title: "Service Sire"
            },
        {
                data: "actual_breeding_userid",
                title: "AI Technition"
            },
             {
                data: "breeding_comment",
                title: "Comment"
            },
            {
                data: "action",
                title: "Action"
            }
EX;
    print($dt->start(30,'breedings',null,$columnsStr,'highlight_important'));     
}     

    public function breedings() {
        $sql = "SELECT breeding_event.id, breeding_event.estimated_breeding_time as event_time, breeding_event.estimated_breeding_userid, breeding_event.actual_breeding_time as event_time2, 
            breeding_event.actual_breeding_userid, breeding_event.bovine_id, bovine.local_number, bovine.full_name , sire_semen_code.semen_code, sire.full_name as service_sire_full_name,
            sire.full_reg_number as sire_full_reg_number,semen_straw_choice_userid,sexed_semen,palpate_comment.comment as breeding_comment,locationcurr.name as location_name
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id 
LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code=semen_straw.semen_code
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id = breeding_event.comment_id
LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id=bovine.id
WHERE breeding_event.estimated_breeding_time <= (current_timestamp + interval '25 hours')::timestamp without time zone
ORDER BY actual_breeding_time DESC ,estimated_breeding_time ASC, local_number limit 50";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        
   
        
        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['event_time']);     
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
            $row['current_location'] = strtotime($row['location_name']);
            //show actual or estiamted breeding time, actual takes presedence. 
            if (empty($row['event_time2'])) {
                  $row['event_time'] = date("l M d Y, H:00", strtotime($row['event_time'])); 
            }
            else {
                   $row['event_time'] = date("l M d Y, H:00", strtotime($row['event_time2'])); 
            }
            $row['estimated_breeding_userid'] = $row['estimated_breeding_userid'];
             $row['semen_straw_choice_userid'] = $row['semen_straw_choice_userid'];
            
            $row['actual_breeding_userid'] = $row['actual_breeding_userid'];
            $row['service_sire_full_name'] = $row['service_sire_full_name'];
            $row['breeding_comment'] = $row['breeding_comment'];
            //$row['action'] = 1;
            //
            //  
            $row['action'] = $this->printActionButtons2($row['id'],$row['bovine_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            if (empty($row['actual_breeding_userid'])) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    /**
     * small method to show if there are any breeding events scheduled in the future.
     */
    function displayFutureBreedingEvents() {
        $sql = "SELECT breeding_event.id, breeding_event.estimated_breeding_time
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id 
WHERE breeding_event.estimated_breeding_time >= (current_timestamp + interval '25 hours')::timestamp without time zone
ORDER BY estimated_breeding_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $rowCount = $res->rowCount();
        if ($rowCount >= 1) {
            print("<br/><b>Note: $rowCount future breedings scheduled.</b><br/>");
        }
    }



  

  

  function printActionButtons2($breeding_id, $bovine_id, $callBackURL = null) {
        // extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();
        $checkToBeSorted = '';
        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        //we only show it if the cow hasn't been bred yet.
        if (array_search('manager', $GLOBALS['auth']->getAuthData('groups')) == true) {
            //print delete button for all
            $res = $GLOBALS['pdo']->query("SELECT id,estimated_breeding_time FROM bovinemanagement.breeding_event WHERE actual_breeding_time is null ORDER BY id DESC");
        } else {
            //print latest delete button (one) for current user
            $res = $GLOBALS['pdo']->query("SELECT id,estimated_breeding_time FROM bovinemanagement.breeding_event WHERE estimated_breeding_userid='$currentUserid' AND actual_breeding_time is null ORDER BY id DESC LIMIT 1");
        }
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($breeding_id == $row['id']) {
                $returnButtonsAsString = self::deleteBreedingForm($breeding_id, $callBackURL); //show delete button, with Breeding table id as index for delete.
                require_once $_SERVER['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/movementSortGate.inc'; 
                $checkToBeSorted = MovementSortGate::checkToBeSorted($bovine_id, $row['estimated_breeding_time']);
            }
        }

        //print breeding receipt button.

        $returnButtonsAsString = $returnButtonsAsString . self::printBreedingReceiptForm2($breeding_id);
        $returnButtonsAsString = $returnButtonsAsString . $checkToBeSorted;

        return $returnButtonsAsString;
    }

    //indicate if sire is sexed by modifying the name.
    function printSireChoice($params) {

        extract($params);

        //create a link to CDN website, so when user clicks they go there for the sire
        $linkStr = Misc::createListOfAllCowsMilking($record['sire_full_reg_number']);

        if ($record['sexed_semen'] == 't') {
            $returnValue = "<a href=\"$linkStr\">{$record['service_sire_full_name']} - sexed</a>";
        } else {
            $returnValue = "<a href=\"$linkStr\">{$record['service_sire_full_name']}</a>";
        }
        return $returnValue;
    }

    
    /**
     * used by breeding protocol to do inserts
     */
    function insertCowToBeBred($bovine_id, $event_time, $uuid) {
        $query = "INSERT INTO bovinemanagement.breeding_event (bovine_id,estimated_breeding_time,estimated_breeding_userid,protocol_uuid) VALUES ($bovine_id,'$event_time','{$GLOBALS['auth']->getUsername()}','$uuid')";
        $res = $GLOBALS['pdo']->exec($query);
    }

    /**
     * used by breeding protocol to do deletes
     */
    function deleteCowToBeBred($uuid) {
        $query = "DELETE FROM bovinemanagement.breeding_event WHERE state_frozen=false AND protocols_uuid='$uuid'";
        $res = $GLOBALS['pdo']->exec($query);
    }

    

  

    /*     * ******************************* */
    /* Delete breeding Form */
    /*     * ******************************* */

    function deleteBreedingForm($breeding_event_ID,$callBackURL) {

        $form = new HTML_QuickForm("formBreedingDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'breeding_event_ID', $breeding_event_ID);
        $form->addElement('submit', 'btnDelete', 'Delete');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $breeding_event_ID = $form->getElementValue('breeding_event_ID');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //erase the reference in semen_straw.
                $res = $GLOBALS['pdo']->query("SELECT semen_straw_id FROM bovinemanagement.breeding_event WHERE id='$breeding_event_ID'");
                {
                    
                } // Always check that result is not an error
                $row = $res->fetch(PDO::FETCH_ASSOC);
                if ($row['semen_straw_id'] != null) {
                    $query = "UPDATE bovinemanagement.semen_straw SET breeding_event_id = (NULL) WHERE id='{$row['semen_straw_id']}'";
                    $res = $GLOBALS['pdo']->exec($query);
                    {
                        
                    } // Always check that result is not an error
                }

                $query = "DELETE FROM bovinemanagement.breeding_event where id='$breeding_event_ID' AND state_frozen = FALSE";
                $res = $GLOBALS['pdo']->exec($query);
                {
                    
                } // Always check that result is not an error
                // determine to commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction

           header($callBackURL);
	   exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

//end delete form


    /*
     * button for breeding slip popup
     */
    function printBreedingReceiptForm2($breedingEventId) {
        $str = '';
        //find out if this is the first time printing the breeding slip or not.
        $res = $GLOBALS['pdo']->query("SELECT state_frozen,semen_straw_id  FROM bovinemanagement.breeding_event WHERE id='$breedingEventId' LIMIT 1");
        {
            
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $origOrReprint = ((integer) $row['state_frozen'] == '0') ? 'Print NEW' : 'Reprint NEW';

        //call breeding slip 
        $cls = new BreedingSlipDialog($breedingEventId);
        $str = $str . $cls->js(); //output js functions
        $str = $str . $cls->main(); //output content of dialog


        ///TODO: onclick also add a state_frozen=TRUE
        //only print button when semen has actually been chosen.
        if ($row['semen_straw_id'] != null) {

            $strA = <<<HTML
<form>
<input type=button value="$origOrReprint Breeding Slip" onClick="breeding_slip_func_{$breedingEventId}()">
</form>
HTML;
            $str = $str . $strA;
        }
        return $str;
    }

   

//passes along error info.
    public function error() {
        $error = array();

        //find who needs semen selected
        $res = $GLOBALS['pdo']->query("SELECT breeding_event.id,breeding_event.estimated_breeding_time,breeding_event.bovine_id,bovine.local_number, bovine.full_name FROM bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id WHERE semen_straw_id is null ORDER BY bovine.local_number,actual_breeding_time DESC");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error[] = new notifyObj(get_class($this), $row['estimated_breeding_time'], 3, $row['local_number'] . ' needs a sire selected for breeding #' . $row['id'] . '.');
        }

        //find who is past there estimated breeding time.
        $res = $GLOBALS['pdo']->query("SELECT breeding_event.id,breeding_event.estimated_breeding_time,breeding_event.bovine_id,bovine.local_number, bovine.full_name, ((estimated_breeding_time - current_timestamp)) as past_due
 FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id 
WHERE actual_breeding_time is null AND estimated_breeding_time < current_timestamp
ORDER BY local_number");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 2, $row['local_number'] . ' is overdue to be bred by ' . $row['past_due'] . '.');
        }


        return $error;
    }

}

/**
 *  shows information to get semen out of tank
 * 
 */
class BreedingSlipDialog {

    public $breedingEventId;

    function __construct($breedingEventId) {
        $this->breedingEventId = $breedingEventId;
    }

    //content of dialog
    public function main() {
        $obj = $this->breedingSlip();
        $str = '';
        $str = $str . "<div class='jquery_dialog' id='breeding_slip_{$this->breedingEventId}' title='{$obj->title}'>";
        $str = $str . $obj->content;
        $str = $str . "</div>";
        return $str;
    }

    //javascript needed for dialog
    public function js() {

        $str = <<<JS
<script>
       $(function() {
   $( "#breeding_slip_{$this->breedingEventId}" ).hide();
  });
       
 function breeding_slip_func_{$this->breedingEventId}() {
   
  $( "#breeding_slip_{$this->breedingEventId}" ).dialog({width: 'auto',
  position: { my: "left top", at: "left bottom", of: "button" }
});
   }
  </script>
JS;

        return $str;
    }

    private function breedingSlip() {
        $str = '';


        $res = $GLOBALS['pdo']->query("
          With temp as (
        SELECT sire_semen_code.sire_full_reg_number,breeding_event.id as breeding_event_id, short_name as sire_short_name,sire.full_name as sire_full_name,sire.full_reg_number as sire_full_reg_mumber, semen_straw.semen_code,tank,bin,freeze_date,sexed_semen,bovinecurr.local_number,bovinecurr.full_name as cow_full_name,bovinecurr.full_reg_number as cow_full_reg_number,bovinecurr.location_name,state_frozen,bovinecurr.location_name
FROM bovinemanagement.breeding_event
LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN bovinemanagement.bovinecurr ON breeding_event.bovine_id = bovinecurr.id
WHERE breeding_event.id= $this->breedingEventId LIMIT 1
)
SELECT temp.*,  (SELECT count(id) FROM bovinemanagement.semen_straw WHERE discarded=false AND breeding_event_id is null AND freeze_date=temp.freeze_date AND bin=temp.bin AND tank=temp.tank AND semen_code=temp.semen_code) as straws_in_cane_left FROM temp
     
        ");

        $row = $res->fetch(PDO::FETCH_ASSOC);

        If ($row['state_frozen'] == 'f') {
            $origOrReprint = 'Original';
        } else {
            $origOrReprint = 'Reprint';
        }

        $str = $str . ("<h1>Breeding Slip ($origOrReprint)</h1>");
        $str = $str . ("<br>");
         $str = $str . ("<br>");
        $str = $str . ("<h2>Breed: </h2>");
        $str = $str . ("<h3>{$row['local_number']} - {$row['cow_full_name']}</h3>");
         $str = $str . ("<h3>Location:{$row['location_name']}</h3>");
         $str = $str . ("<br>");
         $str = $str . ("<br>");
        $str = $str . ("<h2>To: </h2>");
        $str = $str . ("<h3>{$row['sire_full_name']} </h3>");
        $str = $str . ("<h3>{$row['sire_full_reg_number']} </h3>");
        $str = $str . ("<br>");


        $str = $str . ("<br>");
        //$str = $str . ("<h4>Tank: A</h4>");
        $str = $str . ("<br>");
        $str = $str . ("<br>");
        $str = $str . ("<h1>Bin: {$row['bin']}</h1>");
         $str = $str . ("<h1> {$row['sire_short_name']}</h1>");
        $str = $str . ("<h1> {$row['semen_code']}</h1>");
          $str = $str . ("<br>");
        $str = $str . ("<br>");
        $str = $str . ("<h4>Freeze Date: {$row['freeze_date']}</h4>");

        $str = $str . ("<h4>Straws Left in Cane After This One: {$row['straws_in_cane_left']}</h4>");
        $str = $str . ("<h4>Breeding unique id: {$row['breeding_event_id']}</h4>");

        //return object.
        $ret = new stdClass();
        $ret->content = $str;
        $ret->title = $row['local_number'] . " - " . $row['cow_full_name'] . " to " . $row['sire_short_name'];
        return $ret;
    }

}

//end class

require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_EstrusBreedingsChooseSemen extends Quickform2Helper {

    function definition($form) {
  
      

        //grab a list of cows who have are chosen to be bred, but do not have semen chosen yet.
        //$sql="SELECT breeding_event.id,breeding_event.estimated_breeding_time,breeding_event.bovine_id,bovine.local_number, bovine.full_name FROM bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id WHERE breeding_event.actual_breeding_time is null";
        $sql = "SELECT breeding_event.id,breeding_event.estimated_breeding_time,breeding_event.bovine_id,bovine.local_number, bovine.full_name FROM bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id WHERE semen_straw_id is null ORDER BY bovine.local_number,actual_breeding_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        {
            
        } // Always check that result is not an error
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $t=preg_split("/[\s,]+/", $row['full_name']);
            $aniNumber[$row['id']] = $row['local_number'] . " - " . end($t) . "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . date("l @ H:00", strtotime($row['estimated_breeding_time']));
            //$aniNumber[$row['id']] = $row['local_number'] ." - " .Misc::femaleShortName($row['full_name'])."&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;". date("M d, Y", strtotime($row['estimated_breeding_time']));
        }

        //use semen code as index, allocate actual straw when we do the insert. will cause an error if straw is already taken, but that is better then allocating a straw we don't have?
        $query = "SELECT sire.id,short_name,full_name,sire_full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,sexed_semen,ownerid, count(*)
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE AND semen_straw.bin IS NOT NULL AND semen_straw.discarded IS FALSE AND semen_straw.reserved IS FALSE
GROUP BY sire.id,short_name,full_name,sire_full_reg_number,semen_straw.semen_code,tank,bin,freeze_date,sexed_semen,ownerid
ORDER BY short_name,semen_code,freeze_date ASC;";
        $res = $GLOBALS['pdo']->query($query);
        {
            
        } // Always check that result is not an error
        $selectedSire[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['sexed_semen'] == 't') {
                $selectedSire[$row['semen_code']] = '<b>' . $row['short_name'] . '</b>' . ' - ' . $row['full_name'] . ' - sexed';
            } else {
                $selectedSire[$row['semen_code']] = '<b>' . $row['short_name'] . '</b>' . ' - ' . $row['full_name'];
            }
            //ownerid //FIXME: does not support semen owned by W&C and others, does not differentiate here.
            if ($row['ownerid'] != 'W&C') {
                $selectedSire[$row['semen_code']] = $selectedSire[$row['semen_code']] . ' (' . $row['ownerid'] . ')';
            }
        }

        
        
      

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");        
        } else {
            $form->addElement('hidden', 'bovine_id')->setValue($bovine_id);
        }
        
      
        
        $form->addElement('select', 'selectedSire', null, array('options' => $selectedSire, 'label' => 'Semen'));
       
        return $form; //required       
    }

    function process($value) {
           
            $breeding_event_id = $value['bovine_id']; //note not really bovine_id, breedinge vent id, is stand in.
            $semen_code = $value['selectedSire'];

            QF2_EstrusBreedingsMarkCowToBeBred::semenStrawSelectionEngine($breeding_event_id, $semen_code);

       
    }

}//end class


require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_EstrusBreedingsMarkBred extends Quickform2Helper {

    function definition($form) {
  
       

        
        $dateArray = Misc::createHourlyDatesForLastTwoDays(); //hourly array, with unix timestamp as index.
        //grab a list of cows who have been chosen to be bred and have semen chosen.
        //doesn't allow you to mark cows who are bred until they are within 2 days of there scheduled breeding.
        $res = $GLOBALS['pdo']->query("SELECT breeding_event.id,breeding_event.estimated_breeding_time,breeding_event.bovine_id,bovine.local_number, bovine.full_name FROM bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id WHERE breeding_event.actual_breeding_time is null AND semen_straw_id is not null AND estimated_breeding_time::timestamp without time zone <= (current_timestamp + interval '2 day')::timestamp without time zone");
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row['id']] = $row['local_number'] . " - " . Misc::femaleShortName($row['full_name']). "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . date("l @ H:00", strtotime($row['estimated_breeding_time']));
        }

        //grab a list of  comments
        $res = $GLOBALS['pdo']->query("SELECT id,comment FROM bovinemanagement.palpate_comment ORDER BY id");
        $breedingComment['null'] = "Please Select one..."; //dummy value since it is optional and not required we can make it a null string for postgres.
        while (($row = $res->fetch((PDO::FETCH_ASSOC)))) {
            //parse for last word of name and then make an array for the select list.
            $breedingComment[$row['id']] = $row['comment'];
        }


        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
      if ($this->mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");        
        } else {
            $form->addElement('hidden', 'bovine_id')->setValue($bovine_id);
        }
        
      
        
        $form->addElement('select', 'breedingComment', null, array('options' => $breedingComment, 'label' => 'Breeding Comment (Optional)'));
        $form->addElement('select', 'DateActualBreeding', null, array('options' => $dateArray, 'label' => 'Time of Actual Breeding'))->addRule("required", "Please specify your time of animal was bred.");

        return $form; //required       
    }

    function process($value) {
        $value['actual_time'] = date('Y-m-d H:i:sO', $value['DateActualBreeding']);
     

        
          $query = "UPDATE bovinemanagement.breeding_event SET (actual_breeding_time,actual_breeding_userid,state_frozen,comment_id) = ('{$value['actual_time']}','{$GLOBALS['auth']->getUsername()}','TRUE',{$value['breedingComment']}) WHERE id='{$value['bovine_id']}'";

        
       
        $res = $GLOBALS['pdo']->exec($query);
    }

}//end class


require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_EstrusBreedingsMarkCowToBeBred extends Quickform2Helper {

    function definition($form) {
      
        $dateArray = Misc::createHourlyDatesForNexttDay(); //hourly array, with unix timestamp as index.

        $aniNumber = Misc::createListOfAllAliveCows();

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");        
        } else {     
                $form->addElement('hidden', 'bovine_id')->setValue( $this->param->bovine_id);          
        }

        $form->addElement('select', 'DateEstimatedBreeding', null, array('options' => $dateArray, 'label' => 'Time of Estiamted Breeding'))->addRule("required", "Please specify your time of breeding.");

        $form->addElement('checkbox', 'autoselect', array(), array('label'=> 'Auto Select Semen'));
        $form->addElement('checkbox', 'sort', array(), array('label'=> 'Sort At milking'));
        
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('autoselect' => true,'sort' => false))); //set defaults
 
        
        return $form; //required       
    }

    function process($value) {
        $value['estimated_time'] = date('Y-m-d H:i:sO', $value['DateEstimatedBreeding']);
     
        //checkboxs not clicked do not exist. add to be complete. 
        if (!isset($value['autoselect'])) {$value['autoselect']=0;}
        
        //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.breeding_event (bovine_id,estimated_breeding_time,estimated_breeding_userid) VALUES ({$value['bovine_id']},'{$value['estimated_time']}','{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);
            $breeding_event_id = $GLOBALS['pdo']->lastInsertID('bovinemanagement.breeding_event_id_seq');

            //when true auto selct a semen straw, if possible.
            
            if ( $value['autoselect'] == 1) {

               // require_once ($_SERVER ['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/bovineQueryBreedingChoice.inc');
                $ret = BovineQueryBreedingChoice::testBullOrProvenAlgorithm($value['bovine_id']);

                //when there is no error.
                if (($ret['text'] == null) AND ( $ret['semen_code'] != null)) {
                    $semen_code = $ret['semen_code'];

                    //select the semen straw.
                    self::semenStrawSelectionEngine($breeding_event_id, $semen_code);
                }
            }
            
            //when sort checked.
            if ( $value['sort'] == 1) {

                  require_once ($_SERVER ['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/movementSortGate.inc');
                  $a=new QF2_SortGate((object) array(forwardURLArr=>array()),'individual'); //just so we can call sortAnimal. bit messy.
                  $a->sortAnimal($value['bovine_id'], $value['estimated_time']);
            }

        
    }

    
    /*     * *
     * Takes a breeding event id and semen code and marks the semen straw to be used for a breeding.
     */

    public function semenStrawSelectionEngine($breeding_event_id, $semen_code) {

        if (($breeding_event_id == null) OR ( $semen_code == null)) {
            throw new Exception("ERROR both breeding_event_id:$breeding_event_id and semen_code:$semen_code  are required" . "</br>\n\r");
        }

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            //first check that we already didn't select a semen straw for this breeding, if so we need to remove the breeding_id for that straw.

            $res = $GLOBALS['pdo']->query("SELECT semen_straw.id FROM bovinemanagement.semen_straw WHERE breeding_event_id=$breeding_event_id LIMIT 1");
            {
                
            } // Always check that result is not an error
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $semen_straw_id = $row['id'];
            //we need to erase the values.
            if ($semen_straw_id != null) {
                $query = "UPDATE bovinemanagement.breeding_event SET (semen_straw_id,semen_straw_choice_userid) = (NULL,NULL) WHERE id='$breeding_event_id'";

                $res = $GLOBALS['pdo']->exec($query);
                //
                $query = "UPDATE bovinemanagement.semen_straw SET breeding_event_id = (NULL) WHERE id='$semen_straw_id'";

                $res = $GLOBALS['pdo']->exec($query);
            }



            //find an available straw of semen.
            $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.semen_straw WHERE semen_code='$semen_code' AND semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE AND semen_straw.bin IS NOT NULL AND semen_straw.discarded IS FALSE AND semen_straw.reserved IS FALSE ORDER BY freeze_date ASC");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if ($row['id'] == null) {
                throw new Exception("ERROR: No semen sraw for semen code $semen_code, can be found." . "</br>\n\r");
            }
            $semen_straw_id = $row['id'];
            //now update the semen_straw with the breeding_event_id.
            $query = "UPDATE bovinemanagement.breeding_event SET (semen_straw_id,semen_straw_choice_userid) = ($semen_straw_id,'{$GLOBALS['auth']->getUsername()}') WHERE id=$breeding_event_id";

            $res = $GLOBALS['pdo']->exec($query);

            //now update the breeding_event with the semen_straw_id
            $query = "UPDATE bovinemanagement.semen_straw SET breeding_event_id = $breeding_event_id WHERE id=$semen_straw_id";

            $res = $GLOBALS['pdo']->exec($query);

            // determine to commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }

        // Close the transaction
    }
    
}//end class

?>