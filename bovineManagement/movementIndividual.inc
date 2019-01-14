<?php 
require_once "HTML/QuickForm.php";

require_once($_SERVER['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
;
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc'; 
//This relies on alrpo cron running successfully for alpro sync.
class MovementIndividual extends BasePage {

    public function defaultDisplay() {
           
         print("<div class='row'>");
     
        print("<div class='col-sm-4 col-sm-push-8'>");
            
         $a=new QF2_MoveAnimal((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('qf2_MoveAnimal','Select Animal(s) to Move Between Groups'));  
        print($a->formJS());
        
          print("<hr><br/>");    
           print("<br/><br/><br/>");print("<h3>Force Sort Gate Sync Now</h3>"); 
           print("<h4>Note: press only once.</h4>"); 
        ///SSE button for syncing with alpro
        include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/misc/longProcesses.inc');
        $longProcesses = new LongProcesses();
        $longProcesses->showSSEButton(20007); //harcoded bad.
        ///
        print("<hr><br/>");
          
        print("</div>");
           print("<div class='col-sm-8 col-sm-pull-4'>");
         $this->displayAllMovements();
        print("</div>");
        
        print("</div>");
        
    }

   
    
  
    
    
    public function allMovements($request) {
           $sql = "SELECT bovine.local_number,bovine.full_name,bovine.id as bovine_id,location.name,location_event.transaction_id, location_event.event_time,location_event.userid,location_event.id 
FROM bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location.id=location_event.location_id 
LEFT JOIN bovinemanagement.bovine ON bovine.id=location_event.bovine_id 
WHERE event_time > current_date - interval '1 year'
ORDER BY location_event.event_time DESC ";
           
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['local_number'] =JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']);
           $row['event_time_raw']=strtotime($row['event_time']);
           $row['location'] = $row['name'];
           $row['event_time']= date("l M d Y, H:00",strtotime($row['event_time']));  
           $row['event_time_raw']= strtotime($row['event_time']);  
           $row['userid'] = $row['userid'];
           $row['action'] = $this->printActionButtons($row['id'],$row['transaction_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
  
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    
    function displayAllMovements() {
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
                data: "event_time",
                title: "Event Time",
                type: "date",
        order: 2
            },
            {
                data: "location",
                title: "Location"
        },
        {
                data: "userid",
                title: "Employee"
        },
        {
                data: "action",
                title: "action"
        }
           
EX;
    print("<h3>Recent Moves</h3>");
    print($dt->start($this->pageid,'allMovements',null,$columnsStr));     
} 
  
    function printActionButtons($id,$transaction_id,$callBackURL=null) {
        $retStr='';
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id,transaction_id FROM bovinemanagement.location_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if ($id == $row['id']) {
            
            $a=new QF2_GenericDeleteButton((object) array('callBackURL' =>$callBackURL),'individual',(object) array('deleteID'=>$id,'schemaTable'=>'bovinemanagement.location_event'));
             $retStr=($a->renderDelete($id,'Delete'));    
           
        }

        if (($transaction_id == $row['transaction_id'])) {
            
             $b=new QF2_GenericDeleteButton((object) array('callBackURL' =>$callBackURL),'individual',(object) array('deleteID'=>$transaction_id,'deleteName'=>'transaction_id','schemaTable'=>'bovinemanagement.location_event'));
             $retStr = $retStr.($b->renderDelete($transaction_id,'Delete Trans.'));  
            
        }
        
        return $retStr;
    }

    

//shows a small summary of the latest info.
    public function smallSummaryDisplay() {
        $sql = "SELECT bovine.local_number,bovine.full_name,bovine.id as bovine_id,location.name, location_event.event_time,location_event.userid,location_event.id FROM bovinemanagement.location_event LEFT JOIN bovinemanagement.location ON location.id=location_event.location_id LEFT JOIN bovinemanagement.bovine ON bovine.id=location_event.bovine_id WHERE event_time >= localtimestamp - interval '36 hours' ORDER BY location_event.event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $retStr = '';

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $retStr = $retStr . $row['local_number'] . ' - ' . $row['name'] . "<br>";
        }

        return $retStr;
    }

    //passes along error info.
    public function error() {
        $error = array();

        $error = self::checkCalfDiedInHeiferBarnAndNotEntered();
        $error = array_merge($error, self::checkCalfDiedInHutchAndNotEntered());

        return $error;
    }

    /* check calf didn't die in heifer barn and was not enteterd */

    function checkCalfDiedInHeiferBarnAndNotEntered() {
        $errorSub = array();

        $sql = "SELECT * FROM bovinemanagement.bovinecurr WHERE location_id=46 and birth_date <=(current_date - interval '265 days') ";

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $str = "Born: {$row['birth_date']}  &nbsp;.";

            $errorSub[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 3, $row['local_number'] . " Calf At Location (Heifer Barn) To Long: $str");
        }

        return $errorSub;
    }

    /* check if calf died and was not entered */

    function checkCalfDiedInHutchAndNotEntered() {
        $errorSub = array();

        $sql = "SELECT * FROM bovinemanagement.bovinecurr WHERE location_id=45 and birth_date <=(current_date - interval '120 days') ";

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $str = "Born: {$row['birth_date']}  &nbsp;.";

            $errorSub[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 3, $row['local_number'] . " Calf At Location (Hutch) To Long: $str");
        }

        return $errorSub;
    }

}//end classs


/*
 * move animal form.
 * 
 */
Class QF2_MoveAnimal extends Quickform2Helper {

    function definition($form) {

        $dateArray = Misc::createHourlyDatesForLastDay();
        $location = $this->listActiveLocationsAndCounts();  
       $combinedArr=parent::currentAniamlsAndGroups(); 

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($combinedArr);
        } else {         
                $form->addElement('hidden', 'animalSelected')->setValue( $this->param->bovine_id);
        }
        
        $form->addElement('select', 'newLocation', null, array('options' => $location, 'label' => 'Location Moving to'))->addRule("required", "Please specify your location");
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");
        
  return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            $transaction_id = Misc::generatePseudoRandomTransactionInteger();

            $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);


            if (is_array($value['animalSelected'])) {
                //group
                foreach ($value['animalSelected'] as $k => $v) {


                    // GROUP (LOCATION) MOVE
                    if (strstr($v, 'GROUP_') != false) { //when group or pen of animals
                        //remove the 'GROUP_' to find location_id.
                        $integerLocationID = preg_replace('/^GROUP_/', '', $v);
                        if (filter_var($integerLocationID, FILTER_VALIDATE_INT)) {
                            $locationID = $integerLocationID;
                        } else {
                            throw new Exception("Error: bad data, location id not an integer");
                        }

                        //now look up which cows are in that location and we will loop through them. this is inside a transaction, so by doing this here we make it atomic. 
                        $sql2 = "SELECT bovine_id From bovinemanagement.locationcurr WHERE location_id=$integerLocationID";
                        $res2 = $GLOBALS['pdo']->query($sql2);
                        while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {

                            $query = "/*GROUP*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ({$row2['bovine_id']},'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                            $res = $GLOBALS['pdo']->exec($query);
                        }
                        //NORMAL BUNCH OF ANIMALS    
                    } else {

                        //now we insert all the animals that were checked.
                        $bovine_id = $v;
                        $query = "/*ANIMAL*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ($bovine_id,'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                        $res = $GLOBALS['pdo']->exec($query);
                    }
                }
            }
            //individual case
            else {
                $query = "/*SINGLE ANIMAL*/ INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ({$value['animalSelected']},'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                $res = $GLOBALS['pdo']->exec($query);
            }


            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
    }

    //used for animals movements
function listActiveLocationsAndCounts(){
    //grab a list of locations with pen counts// SQL SMALL BUG ON COUNT	
		$res = $GLOBALS['pdo']->query("SELECT DISTINCT ON (location.name) location.name,location.id,COUNT(*) as count FROM bovinemanagement.location LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.location_id=location.id   WHERE location.active=true GROUP BY location.name,location.id ORDER BY location.name");
		
		
		
		$activeLocations[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//bug in above sql, prints count of 1 when the pen is empty.
			if ($row['count'] ==1) {
			$row['count'] =null;
			$activeLocations[$row['id']] = $row['name'];
			}
			else {
			$activeLocations[$row['id']] = $row['name'].' ('.$row['count'].')';
			}
		}
return $activeLocations;
}

}//end class   
?>