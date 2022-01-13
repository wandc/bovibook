<?php

//code that just has to do with heats
//July 26, 2008
//big changes aug 2018 to QF2
//FIXME: change to visual heats page, then use views to tie in auto heats and kamars.
class EstrusHeats extends BasePage {

    public function defaultDisplay() {
     
    $a=new QF2_EstrusHeats((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
         
            
        $left = <<<HTML
            {$this->displayLast100DaysEstrusEvents()}
        HTML; 
           
        $right = <<<HTML
            {$a->renderBox('qf2_heatsEntry','Record Heat Event')}   
            {$this->displayFutureHeatEvents()}
        HTML; 
               
        $content=(new Bootstrap)->oneThirdTwoThirds($left,$right);                
        print($content);
    }

    /**
     * for ajax calls to get heats.
     * @param type $request
     * @return type
     */
    public function heats($request) {
        $sql = "Select estrus_event.id as estrus_id,estrus_event.bovine_id,local_number,full_name,estrus_type.name as event_name,estrus_event.event_time as event_time,birth_date,estrus_event.userid,locationcurr.name as location_name FROM bovinemanagement.estrus_event
	    LEFT JOIN bovinemanagement.bovine ON bovine.id=estrus_event.bovine_id 
	    LEFT JOIN bovinemanagement.estrus_type ON estrus_event.estrus_type_id=estrus_type.id  
	    LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id=bovine.id
             WHERE estrus_event.event_time >=  CURRENT_DATE - interval '100 days'
	     ORDER BY estrus_event.event_time DESC ";

        $res = & $GLOBALS['pdo']->query($sql);
        $obj = $res->fetchAll(PDO::FETCH_CLASS);
        $output['aaData'] = $obj;
        return $output;
    }

   
   

   
    function printNotBreedingReason($params) {

        extract($params);
        $returnMessage = null;
        $returnMessage = self::reasonsNotToBreed($record['bovine_id'], $record['event_time'], $record['birth_date']);
        return $returnMessage;
    }

    function reasonsNotToBreed($bovine_id, $event_time, $birth_date) {
        $returnMessage = '';
        //do all reason with respect to event time not todays date incase we look back in time.
        //if animal is too young.
        //use volunatry breeding period instead.
        //if animal is already bred, then write that.

        //uses order of presedence. first reason hit is the one that gets displayed. 
        
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.breeding_event WHERE bovine_id = $bovine_id AND actual_breeding_time > timestamp '$event_time' - interval '2 days'");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
           return 'Already Bred.';
        }

        //see if animal has reached vountary waiting period yet.
        $query = "SELECT bovinemanagement.calculate_breeding_voluntary_waiting_period(id),date_trunc('day',now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(id)) from bovinemanagement.bovinecurr WHERE id=$bovine_id";
        $res = $GLOBALS['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row['calculate_breeding_voluntary_waiting_period'])) {
        if (strtotime($row['calculate_breeding_voluntary_waiting_period']) >= strtotime("now")) {
           return "Voluntary waiting period not reached until {$row['calculate_breeding_voluntary_waiting_period']}.";
        }}

        //if animal is on do not breed list write that.
        $res = $GLOBALS['pdo']->query("SELECT bovine_id,donotbreed FROM bovinemanagement.service_picks WHERE bovine_id= $bovine_id ORDER BY event_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($res->rowCount() > 0) {
        if ($row['donotbreed'] == true) {
           return 'On Do Not Breed List.';
        }}


        //if animal was preg check positive in the last 14 days, assume they are definately pregnant.
        $res = $GLOBALS['pdo']->query("SELECT event_time FROM bovinemanagement.preg_check_event WHERE bovine_id= $bovine_id  AND preg_check_result='pregnant' AND event_time >= ('$event_time'::date - interval '14 days') ORDER BY event_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
           return "Marked Pregnant on " . date('M d', strtotime($row['event_time'])) . ".";
        }

        //if animal is less than 180 days until they are due. 
        $res = $GLOBALS['pdo']->query("SELECT (calculated_potential_due_date - '$event_time') as days FROM bovinemanagement.combined_breeding_embryo_view WHERE recipient_bovine_id=$bovine_id ORDER BY event_time DESC limit 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
            if (($row['days'] <= 180) && ($row['days'] > 0))
           return "Marked Pregnant, only {$row['days']} days till due.";
        }

            //if animal is to be bred way in the furture.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.breeding_event WHERE bovine_id = $bovine_id AND actual_breeding_time is null AND estimated_breeding_time > ('$event_time'::date  + interval '1.5 day')");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
           return 'Marked to breed in the future.';
        }
        

        //if animal is to be bred.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.breeding_event WHERE bovine_id = $bovine_id AND actual_breeding_time is null AND estimated_breeding_time > timestamp '$event_time' - interval '1 days'");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
           return '<b id="boldRed">Marked to Breed.</b>';
        }

      
        
        //if animal is marked for embryo transfer.
        $res = $GLOBALS['pdo']->query("SELECT id,estimated_event_time FROM bovinemanagement.embryo_implant WHERE bovine_id = $bovine_id AND estimated_event_time > timestamp '$event_time' - interval '4 days'");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row != null) {
            $embryoTransferTime = date("l M d", strtotime($row['estimated_event_time']));
           return "Marked for Embryo Transfer on $embryoTransferTime.";
        }

        //check if breeding banned
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusBreedings.inc');
        $breedingBan=QF2_BreedingBan::isAnimalBreedingBanned($bovine_id);
         if (!empty($breedingBan)) {   
         return "Breeding banned until $breedingBan.";
         }
        
        //print_r($record);
        return $returnMessage;
    }

    

    

    //shows error.
    function error() {

        ////////////////////////
        //looks if cow is on heat list with score above 60 and not marked to breed or other reason.
        $sql = "Select estrus_event.id as estrus_id,estrus_event.bovine_id,local_number,full_name,estrus_type.name as event_name,estrus_event.event_time as event_time,birth_date,estrus_event.userid,locationcurr.name as location_name
            FROM bovinemanagement.estrus_event
	    LEFT JOIN bovinemanagement.bovine ON bovine.id=estrus_event.bovine_id 
	    LEFT JOIN bovinemanagement.estrus_type ON estrus_event.estrus_type_id=estrus_type.id  
	    LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id=bovine.id
            WHERE breeding_rating >= 60 AND estrus_event.event_time >= (now() - interval '36 hours') 
	     ORDER BY estrus_event.event_time DESC
	     ";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //find a reason to not breed this cow.
            $returnMessage = self::reasonsNotToBreed($row['bovine_id'], $row['event_time'], $row['birth_date']);


            if ($returnMessage == null) {
                $error["{$row['local_number']}"] = new notifyObj(get_class($this), $row['event_time'], 1, $row['local_number'] . ' probably needs to be marked to breed.');
            }
        }
        if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
    }

    
  
    public function displayFutureHeatEvents() {
        $arr=$this->watchForFutureHeatsEvents();
        $out=array();
        $prevTime=null;
          foreach ($arr as &$row) {
              if ($row['event_time']!=$prevTime) {
                    $out[]='<h3>&#xb1; '.$row['event_time'].'</h3>';
              }
              $prevTime=$row['event_time'];
            
              $out[]='<li>'.$row['local_number'].'</li>';
          }
        
      
        $content=(implode($out));
     return(    (new BootStrap)->generalInfoBox('ion-ios-eye-outline','bg-info','Watch For Heats',$content));
    }
    
    
    public function watchForFutureHeatsEvents() {
        
        /* this shows breeding protocol watch for heat, but not animals who were just bred. */
        $sql = <<<SQL
Select estrus_event.id as estrus_id,estrus_event.bovine_id,local_number,full_name,estrus_type.name as event_name,estrus_event.event_time as event_time,birth_date,estrus_event.userid,locationcurr.name as location_name,estrus_type.id as estrus_type_id,bovinemanagement.last_breeding(bovine.id)
 FROM bovinemanagement.estrus_event
	    LEFT JOIN bovinemanagement.bovine ON bovine.id=estrus_event.bovine_id 
	    LEFT JOIN bovinemanagement.estrus_type ON estrus_event.estrus_type_id=estrus_type.id  
	    LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id=bovine.id
             WHERE 
    estrus_event.event_time >=  CURRENT_DATE - interval '2 days' 
AND estrus_event.event_time <  CURRENT_DATE + interval '2 days' 
AND system_entry = true
AND (bovinemanagement.last_breeding(bovine.id) IS NULL OR bovinemanagement.last_breeding(bovine.id) <=  (CURRENT_DATE - interval '2 days') )
ORDER BY estrus_event.event_time DESC 
SQL;
    
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {
            $rowOut['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['event_time'] = date("l M d", strtotime($row['event_time']));  //has to be at end.
             $resultsOut[] = $rowOut; 
        }
          
        return $resultsOut;
    }
   

    public function AllHeats7Days($request) {
        
         $sql = <<<SQL
Select estrus_event.id as estrus_id,estrus_event.bovine_id,local_number,full_name,estrus_type.name as event_name,estrus_event.event_time as event_time,birth_date,estrus_event.userid,locationcurr.name as location_name,estrus_type.id as estrus_type_id
 FROM bovinemanagement.estrus_event
	    LEFT JOIN bovinemanagement.bovine ON bovine.id=estrus_event.bovine_id 
	    LEFT JOIN bovinemanagement.estrus_type ON estrus_event.estrus_type_id=estrus_type.id  
	    LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id=bovine.id
             WHERE estrus_event.event_time >=  CURRENT_DATE - interval '7 days' AND system_entry = false
	     ORDER BY estrus_event.event_time DESC 
SQL;
         
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

         $delButton=new QF2_DeleteButtonDecider('user_1only',(object) array('schemaTable'=>'bovinemanagement.estrus_event'));
        
        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['note'] = $this->reasonsNotToBreed($row['bovine_id'], $row['event_time'], $row['birth_date']);
            $row['action']=$delButton->output($row['estrus_id']) .  MovementSortGate::checkToBeSorted($row['bovine_id'], $row['event_time']);     
            $row['event_time'] = date("l M d H:00", strtotime($row['event_time']));  //has to be at end.
            
            if ((($row['estrus_type_id']) == 1) AND empty($row['note'])) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayEstrusDetectorHighProbabilityHeats() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                        
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
           
            
            {
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "sum_var",
                title: "Variance"
        },
        {
                data: "tag_id",
                title: "Tag #"
        }
           
EX;

        $str.=($dt->start('Estrus Detector Last 2 Days', 140, 'highProbabilityHeats', null, $columnsStr));
        return  $str;
    }

    function displayLast100DaysEstrusEvents() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
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
                data: "event_name",
                title: "Event"
        
            },
            {
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "userid",
                title: "Employee"
            },
             {
                data: "note",
                title: "Note"
            },
            {
                data: "action",
                title: "Action"
            }   
EX;
        return($dt->start('Heat Events', 28, 'AllHeats7Days', null, $columnsStr));

       
    }

    /*
    * used by breeding protocol to say watch for heat. Bit clunky, but so is kamar red.
    * 
    */
    public function protocolEstrusDefaultInsert($bovine_id, $event_time, $estrus_type_id, $uuid)  {
         //don't put id value, uses next one in DB.
        $query = "INSERT INTO bovinemanagement.estrus_event (bovine_id,event_time,estrus_type_id,userid,protocol_uuid) VALUES ($bovine_id,'$event_time',$estrus_type_id,'{$GLOBALS['auth']->getUsername()}','$uuid')";
        $res = $GLOBALS['pdo']->exec($query);       
    }
    

}//end class



       
Class QF2_EstrusHeats extends QF2_Base {

    function definition($form) {

      

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();
        //grab a list of heat events.
        $res = $GLOBALS['pdo']->query("SELECT id,name from bovinemanagement.estrus_type WHERE display_order > 0 AND system_entry = false ORDER BY display_order");

        $heatEvents[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $heatEvents[$row[0]] = $row[1];
        }


        ////
       // $fieldset = $form->addFieldset('header')->setLabel('Record Heat Event');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");
            
        } else {
            $form->addElement('hidden', 'bovine_id')->setValue($this->param->bovineID);
        }
        $form->addElement('select', 'heatType', null, array('options' => $heatEvents, 'label' => 'Type of Heat Event'))->addRule("required", "Please specify your type of heat");
        $form->addElement('select', 'dateHeat', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of heat");

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['dateHeat']);
        $value['estrus_type_id'] = $value['heatType'];

        //don't put id value, uses next one in DB.
        $query = "INSERT INTO bovinemanagement.estrus_event (bovine_id,event_time,estrus_type_id,userid) VALUES ({$value['bovine_id']},'{$value['event_time']}',{$value['estrus_type_id']},'{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);
    }

}//end class