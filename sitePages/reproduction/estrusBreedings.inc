<?php 
class EstrusBreedings extends BasePage {

    public function defaultDisplay() {
        
        $boot=new BootStrap;
        $a=new QF2_EstrusBreedingsMarkCowToBeBred((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
        $c=new QF2_EstrusBreedingsChooseSemen((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');   
        $d=new QF2_EstrusBreedingsMarkBred((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');   
       
        $top=$boot->halfHalf($this->displayFutureBreedingEvents(),$this->displayBreedingRate());

      
        $left = <<<HTML
            {$top}
            {$this->displayLastBreedingEvents2()}
        HTML; 
            
        $right = <<<HTML
            {$a->renderBox('qf2_BreedingsCowToBeBred','Animal to be Bred')}
            {$c->renderBox('qf2_BreedingsChooseSemen','Choose Semen')}
            {$d->renderBox('qf2_BreedingsMarkBred','Mark as Bred')}
        HTML; 
                
        $content=$boot->oneThirdTwoThirds($left,$right);                
        print($content);
        
        
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
            },
             {
                data: "icon",
                title: ""
            }
EX;
    return ($dt->start('Breeding Events', 30,'breedings',null,$columnsStr,'highlight_important'));     
}     

    public function breedings() {
        $sql = "SELECT breeding_event.id, breeding_event.estimated_breeding_time as event_time, breeding_event.estimated_breeding_userid, breeding_event.actual_breeding_time as event_time2, 
            breeding_event.actual_breeding_userid, breeding_event.bovine_id, bovine.local_number, bovine.full_name , sire_semen_code.semen_code, sire.short_name as service_sire_short_name , sire.full_name as service_sire_full_name,kamar_event_type,
            sire.full_reg_number as sire_full_reg_number,semen_straw_choice_userid,sexed_semen,palpate_comment.comment as breeding_comment,locationcurr.name as location_name
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id 
LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code=semen_straw.semen_code
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id = breeding_event.comment_id
LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id=bovine.id
LEFT JOIN bovinemanagement.kamar_event_curr ON kamar_event_curr.bovine_id=bovine.id
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
                        
           $extra=$GLOBALS['MiscObj']->isSireBeefBullThenSymbol($row['sire_full_reg_number']); //see if beef symbol shown   
           $sexxed = ($row['sexed_semen'] == true) ? ' <i class="fas fa-venus"></i> ':''; 
           $sire_link= $GLOBALS['MiscObj']->createCDNLinkHref($row['service_sire_short_name'],$row['sire_full_reg_number']);
            
            $row['service_sire_full_name'] = $extra.$sire_link.$sexxed;
            $row['breeding_comment'] = $row['breeding_comment'];
            //$row['action'] = 1;
            //
            //  <i class="fas fa-clipboard-list"></i>
             $printButton=self::printBreedingReceiptForm2($row['id']);
            
              //find kamar status
              $kamarIcon = (!empty($row['kamar_event_type'])) ? '<i class="fas fa-capsules"></i>' : '';
             
              $row['icon']=$kamarIcon;
              
              $delButton=new QF2_DeleteButtonDecider('user_timeLimit',(object) array('dateTimeName'=> 'update_time', 'schemaTable'=>'bovinemanagement.breeding_event','useridColumnName'=>'estimated_breeding_userid','buttonClassName'=>'QF2_EstrusBreedingDeleteButton'));
              $row['action']=$delButton->output($row['id']).$printButton;
            
  
            
            $row['colour_row'] =  (empty($row['actual_breeding_userid']) ? 1 : 0);
            
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

  

    
    /**
     * small method to show if there are any breeding events scheduled in the future.
     */
    function displayFutureBreedingEvents() {
        $sql = "SELECT bovine_id,local_number,full_name,breeding_event.id, breeding_event.estimated_breeding_time,bovinemanagement.semen_code_to_short_name(semen_code)
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.semen_straw ON semen_straw_id	=semen_straw.id
LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id 
WHERE breeding_event.estimated_breeding_time >= (current_timestamp + interval '25 hours')::timestamp without time zone
ORDER BY estimated_breeding_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $inner = array();
        $rowCount = $res->rowCount();
        $inner[] = '<ul class="list-unstyled">';
        $prevTime = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time'] = date("l M d", strtotime($row['estimated_breeding_time']));
            if ($row['event_time'] != $prevTime) {
                $inner[] = '<h5>' . $row['event_time'] . '</h5>';
            }
            $prevTime = $row['event_time'];
            $inner[] = ("<li>{$row['local_number']} </li>");
        }
        $inner[] = '</ul>';
        $content = implode($inner);

        if ($rowCount >= 1) {
            return ( (new BootStrap)->generalInfoBox('ion-ios-timer-outline', 'bg-info', 'Future Breedings', $content));
        }
    }

    function displayBreedingRate() {
        
            //// breeding rate
       
        $breedingRateInterval = 30;
        $dataArray = (new ReproReport)->calculateTwentyOneDayBreedingAverage(1, $breedingRateInterval, 90); //slow part .03s.
        $out[]= ('<span>');
        $breedingrate=new GoogleVisualizationSparkLine();
        $out[]= $breedingrate->sparklineContainerVis('30d BR', $dataArray, end($dataArray));
        $out[]= ('</span>');

        return ( (new BootStrap)->generalInfoBox('ion-arrow-graph-up-right', 'bg-info', 'Breeding Rate', implode($out)));
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

                $checkToBeSorted = MovementSortGate::checkToBeSorted($bovine_id, $row['estimated_breeding_time']);
            }
        }

        //print breeding receipt button.

        $returnButtonsAsString = $returnButtonsAsString . self::printBreedingReceiptForm2($breeding_id);
        $returnButtonsAsString = $returnButtonsAsString . $checkToBeSorted;

        return $returnButtonsAsString;
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
        $form->addElement('submit', 'btnDelete', 'Delete',array('class'=>'btn btn-warning'));


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
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
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
        $res = $GLOBALS['pdo']->query("SELECT *,state_frozen,semen_straw_id,(SELECT local_number FROM bovinemanagement.bovine where bovine.id=breeding_event.bovine_id)  FROM bovinemanagement.breeding_event WHERE id=$breedingEventId LIMIT 1");
        {
            
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $buttonSecondary= ((integer) $row['state_frozen'] == '0') ? 'btn-primary' : 'btn-secondary'; 
        $iconSmallorLargeClass = ((integer) $row['state_frozen'] == '0') ? 'fa-2x' : '';
        $origOrReprint = ((integer) $row['state_frozen'] == '0') ? '' : '<i class="fas fa-redo "></i>';

        //call breeding slip 
        $cls = new BreedingSlipDialog($breedingEventId,$row['state_frozen'],$row['local_number']);
        $str = $str . $cls->js(); //output js functions
        $str = $str . $cls->main(); //output content of dialog


        ///TODO: onclick also add a state_frozen=TRUE
        //only print button when semen has actually been chosen.
        if ($row['semen_straw_id'] != null) {

            $strA = <<<HTML


                    <button type="button" class="btn $buttonSecondary   "  onClick="breeding_slip_func_{$breedingEventId}()">
  $origOrReprint <i class="fas fa-clipboard-list $iconSmallorLargeClass"></i>
</button>
                            
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
    public $state;
    public $localNumber;

    function __construct($breedingEventId,$state,$localNumber) {
        $this->breedingEventId = $breedingEventId;
        $this->state = $state;
        $this->localNumber=$localNumber;
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

        //print reprint or normal title. 
        if ($this->state==false) {
            $title="Breeding Slip #{$this->localNumber}";       
        }
        else {
            $title="Breeding Slip (reprint) #{$this->localNumber}";      
        }
        
        
        
        $str = <<<JS
<script>
      
   $( "#breeding_slip_{$this->breedingEventId}" ).dialog({ autoOpen: false, 
                            modal: false,
                            resizable: false,
                            draggable: true,
                            closeOnEscape: true,
                           
                            title: "$title", });   
 function breeding_slip_func_{$this->breedingEventId}() {
  
  $( "#breeding_slip_{$this->breedingEventId}" ).dialog("open");
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

          
        $symbol=$GLOBALS['MiscObj']->isSireBeefBullThenSymbol($row['sire_full_reg_number']); //see if beef symbol shown
           
        
        $str = $str . ("<h2>Breed: </h2>");
        $str = $str . ("<h3>{$row['local_number']} - {$row['cow_full_name']}</h3>");
         $str = $str . ("<h4>Location:{$row['location_name']}</h4>");
         $str = $str . ("<br>");
         $str = $str . ("<br>");
        $str = $str . ("<h2>To: </h2>");
        $str = $str . ("<h3>$symbol{$row['sire_full_name']} </h3>");
        $str = $str . ("<h3>{$row['sire_full_reg_number']} </h3>");
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

       
Class QF2_EstrusBreedingsChooseSemen extends QF2_Base {

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
            //$aniNumber[$row['id']] = $row['local_number'] ." - " .$GLOBALS['MiscObj']->femaleShortName($row['full_name'])."&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;". date("M d, Y", strtotime($row['estimated_breeding_time']));
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


       
Class QF2_EstrusBreedingsMarkBred extends QF2_Base {

    function definition($form) {
  
       

        
        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastTwoDays(); //hourly array, with unix timestamp as index.
        //grab a list of cows who have been chosen to be bred and have semen chosen.
        //doesn't allow you to mark cows who are bred until they are within 2 days of there scheduled breeding.
        $res = $GLOBALS['pdo']->query("SELECT breeding_event.id,breeding_event.estimated_breeding_time,breeding_event.bovine_id,bovine.local_number, bovine.full_name FROM bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id WHERE breeding_event.actual_breeding_time is null AND semen_straw_id is not null AND estimated_breeding_time::timestamp without time zone <= (current_timestamp + interval '2 day')::timestamp without time zone");
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row['id']] = $row['local_number'] . " - " . $GLOBALS['MiscObj']->femaleShortName($row['full_name']). "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . date("l @ H:00", strtotime($row['estimated_breeding_time']));
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


        
Class QF2_EstrusBreedingsMarkCowToBeBred extends QF2_Base {

    function definition($form) {
      
        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForNexttDay(); //hourly array, with unix timestamp as index.

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");        
        } else {     
                $form->addElement('hidden', 'bovine_id')->setValue( $this->param->bovineID);          
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

              
                $ret = (new BovineQueryBreedingChoice)->testBullOrProvenAlgorithm($value['bovine_id']);

                //when there is no error.
                if (($ret['text'] == null) AND ( $ret['semen_code'] != null)) {
                    $semen_code = $ret['semen_code'];

                    //select the semen straw.
                    self::semenStrawSelectionEngine($breeding_event_id, $semen_code);
                }
            }
            
            //when sort checked.
            if ( $value['sort'] == 1) {

                  (new MovementSortGate);//hack for class load
                  $a=new QF2_SortGate((object) array('forwardURLArr'=>array()),'individual'); //just so we can call sortAnimal. bit messy.
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
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }

        // Close the transaction
    }
    
}//end class

Class QF2_EstrusBreedingDeleteButton extends QF2_DeleteButtonGeneric{ 
    
    function action() {
        //$this->param should provide everything the form needs for paramaters.  
        
        
        
       $breeding_event_ID = $this->param->deleteID;

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
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction
     
    }
} //end class

Class QF2_BreedingBan extends QF2_Base {

    function definition($form) {

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/vetToCheck.inc');
        $dateArrayVetCheck= QF2_VetToCheckEvent::whatDayIsTheVetCheck();
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForNextSixtyDays(); 
        $dateArray2 = array_replace($dateArrayVetCheck,$dateArray);
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
       
        $eventTimeStart=$form->addElement('select', 'eventTimeStart', null, array('options' => $dateArray, 'label' => 'Time of Event Start'));
        $eventTimeEnd=$form->addElement('select', 'eventTimeEnd', null, array('options' => $dateArray2, 'label' => 'Time of Event End'));
        
       
          
        //defaults
        $today=strtotime("today");
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('eventTimeStart' => $today,'eventTimeEnd' => null)));
         
  
        /** RULES */
      
        //this doesn't really makes sense to me.
        $eventTimeStart->addRule('empty', '', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER)
            ->and_($eventTimeStart->createRule('empty'))
            ->or_($eventTimeEnd->createRule('gt', 'End time must be greater then start time.', $eventTimeStart));
        
        
         $eventTimeStart->addRule('required', 'Please specify your time',null,HTML_QuickForm2_Rule::SERVER | HTML_QuickForm2_Rule::CLIENT);
         $eventTimeEnd->addRule('required', 'Please specify your time',null,HTML_QuickForm2_Rule::SERVER | HTML_QuickForm2_Rule::CLIENT);
     
         
         /*
         //DEMO RULES 
         
         // registers a callback rule with a specific callback
        HTML_QuickForm2_Factory::registerRule('fooBar', 'HTML_QuickForm2_Rule_Callback', null,array('callback' => 'QF2_BreedingBan::chkTime'));
        $form->addRule('fooBar', 'wrong variable namedddDDDD!', array('two', 'three', 'four'));
         
        
        HTML_QuickForm2_Factory::registerRule('fooBar2', 'dan');
        $eventTimeEnd->addRule('fooBar2', 'wrongEEEEEE!');
         */
         
         
        return $form; //required       
    }

    //rule called by callback. 
    function chkTime($value,$two,$three) {
        //var_dump($two);
        //var_dump($three);
        return true; //always returns true, does nothing.
        
    }
    
    function process($value) {
            
            
        $value['bovine_id']=$value['animalSelected'];
        $value['event_time_start'] = date('Y-m-d 00:00:00', $value['eventTimeStart']);
        $value['event_time_end'] = date('Y-m-d 00:00:00', $value['eventTimeEnd']);

            
        self::insertBreedingBan($value['bovine_id'], $value['event_time_start'], $value['event_time_end']);
            
    }
     
    // dates need to be of Y-m-d 00:00:00.
    public static function insertBreedingBan($bovine_id,$event_time_start,$event_time_end) {
        
         //first chek that ranges do not overlap at all.
        $sql=<<<SQL
        SELECT id FROM bovinemanagement.breeding_ban WHERE bovine_id={$bovine_id} AND event_range && '[{$event_time_start},{$event_time_end}]'
SQL;
            $res2 = $GLOBALS['pdo']->query($sql);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            
            $query = "INSERT INTO bovinemanagement.breeding_ban (bovine_id,event_range,userid) VALUES ({$bovine_id},'[{$event_time_start},{$event_time_end}]','{$GLOBALS['auth']->getUsername()}')";
            
            //dont insert if intersects one in db for this animal. 
            if (empty($row2)) {    
             $res = $GLOBALS['pdo']->exec($query);
            }

        
    }
/*
 * if animal is currently banned from being bred return time it ends, ie true state or false.
 */    
      public static function isAnimalBreedingBanned($bovine_id) {
          
     $sql4=<<<SQL
              SELECT id,upper(event_range)::date as upper_event_range FROM bovinemanagement.breeding_ban WHERE bovine_id=$bovine_id AND event_range @> localtimestamp limit 1
      SQL;
        $res4 = $GLOBALS['pdo']->query($sql4);
        $row4 = $res4->fetch(PDO::FETCH_ASSOC);
        return (!empty($row4)) ? $row4['upper_event_range'] : false;
         
      }      
            
    

}//end class


/*
 * another way to do a rule. 
 */
class dan extends HTML_QuickForm2_Rule
{
    function validateOwner()
    {
        //var_dump($this->owner->getValue());
        return true;
    }
   
}

