<?php

class EstrusKamar extends BasePage {

    public function defaultDisplay() {
        
         $a = new QF2_EstrusKamar((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
        
        $left = <<<HTML
            {$this->displayLastAllKamarEvents()}
            {$this->displayNext2DaysScheduledKamars()}
        HTML; 
            
        $right = <<<HTML
            {$a->renderBox('qf2_kamarEntry', 'Record Kamar Event')}
        HTML; 
                
        $content=(new Bootstrap)->oneThirdTwoThirds($left,$right);                
        print($content);
        
    }

    
     function displayNext2DaysScheduledKamars($bovine_id=null) {
         $out=array();
            if (!empty($bovine_id)) {
                   $sql = "
SELECT * ,kamar_event.id as kamar_event_id
FROM bovinemanagement.kamar_event 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = kamar_event.bovine_id
WHERE  bovine_id=$bovine_id  AND scheduled_event_time <= (current_timestamp + interval '2 days') AND event_time is null
		ORDER BY scheduled_event_time ASC
   ";               
        }else {   
         $sql = "
SELECT * ,kamar_event.id as kamar_event_id
FROM bovinemanagement.kamar_event 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = kamar_event.bovine_id
WHERE  scheduled_event_time <= (current_timestamp + interval '2 days') AND event_time is null
		ORDER BY scheduled_event_time ASC
   "; 
         }
         
          $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

          $delButton=new QF2_DeleteButtonDecider('user_1only',(object) array('schemaTable'=>'bovinemanagement.kamar_event'));
        
          $resultsOut=array();
          
        //change presentation
        foreach ($results as &$row) {
        $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
        $rowOut['Scheduled Time'] = JQueryDataTable::formattedDateTimeDofW($row['scheduled_event_time']);
       
    
         $rowOut['Action'] = $delButton->output($row['id']);
         $rowOut['action2'] = $this->printActionButtonKamarScheduledComplete($row['kamar_event_id'],$row['bovine_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
     

            $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;
      
        $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Scheduled Kamars', $dataArr));
            return implode($out);

    }
    
    function printActionButtonKamarScheduledComplete($kamar_id, $bovine_id,$callBackURL = null) {
        //we dont need an if statement because we have a where above. 
                $a = new QF2_EstrusKamarScheduledComplete((object) array('callBackURL' => $callBackURL), 'individual', (object) array('kamarID' => $kamar_id));
            $returnButtonsAsString = '' . ($a->renderComplete($kamar_id,'Completed'));
            //print button
 
        return $returnButtonsAsString;
    }
    
    
    
    

    static function checkKamarStatus($valueArr, $elementNameArr) {
        //generic loop to convert $form elements we want into a usuable array
        //I really hope this isn't how quickforms was designed.
        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData[$elementName] = $valueArr[$count];
            $count++;
        }

        //ok now we have formData, use it.
        //TODO
        //print_r($formData);

        $bovine_id = $formData['animalNumber'];
        $lastKamarEvent = self::lookupLastKamarEventForAnimal($bovine_id);
        print("<h1>lastKamarEvent$lastKamarEvent</h1>");
        //print("lastKamarEvent:$lastKamarEvent</br>");

        $returnStatement = false; //default is to fail.

        switch ($formData['kamarEventType']) {
            case 'verifiedwhite':
                if (($lastKamarEvent == 'on') || ($lastKamarEvent == 'verifiedwhite') || ($lastKamarEvent == 'partialred')) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            case 'on':
                //print("Switch:on</br>");
                //only allowed if the last case was lost or removed.
                if (($lastKamarEvent == 'lost') || ($lastKamarEvent == 'removed') || ($lastKamarEvent == null)) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            case 'removed':
                //print("Switch:removed</br>");
                //allowed to be removed if on,red or patial red.
                if (($lastKamarEvent == 'on') || ($lastKamarEvent == 'red') || ($lastKamarEvent == 'partialred') || ($lastKamarEvent == 'verifiedwhite')) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            case 'lost':
                //print("Switch:lost</br>");
                //allowed to be lost if on,red or patial red.
                if (($lastKamarEvent == 'on') || ($lastKamarEvent == 'red') || ($lastKamarEvent == 'partialred')) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            case 'red':
                //print("Switch:red</br>");
                //allowed to be red if previously on,partialred.
                if (($lastKamarEvent == 'on') || ($lastKamarEvent == 'partialred') || ($lastKamarEvent == 'verifiedwhite')) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            case 'partialred':
                //print("Switch:partialred</br>");
                //allowed to be partialred if previously on
                if (($lastKamarEvent == 'on') || ($lastKamarEvent == 'verifiedwhite')) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            case 'verifiedwhite':
                //print("Switch:verifiedwhite</br>");
                //only has to be on, because sometimes, crew marks kamar red that is still white.
                if (($lastKamarEvent == 'on') || ($lastKamarEvent == 'verifiedwhite')) {
                    $returnStatement = true;
                } else {
                    $returnStatement = false;
                }
                break;
            default:
                $returnStatement = false;
        }

        return $returnStatement;
    }

    // verify only newer event times are being entered.
    // if not followed inconsistencies result.
    static function checkEventTimeIsNewer($valueArr, $elementNameArr) {
        //generic loop to convert $form elements we want into a usuable array
        //I really hope this isn't how quickforms was designed.
        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData[$elementName] = $valueArr[$count];
            $count++;
        }

        $bovine_id = $formData['animalNumber'];
        $res = $GLOBALS['pdo']->query("SELECT event_time FROM bovinemanagement.kamar_event WHERE bovine_id=$bovine_id ORDER BY event_time DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);

        if ($formData['dateEvent'] > strtotime($row['event_time'])) {
            return true;
        } else {
            return false;
        }
    }

    static function lookupLastKamarEventForAnimal($bovine_id) {
        $res = $GLOBALS['pdo']->query("SELECT trim(kamar_event_type) as kamar_event_type FROM bovinemanagement.kamar_event WHERE bovine_id=$bovine_id ORDER BY event_time DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        return $row['kamar_event_type']; //return last kamar event or null if none.
    }
    
     /*
    * used by breeding protocol to say watch for heat. Bit clunky, but so is kamar red.
    * 
    */
    public function protocolKamarDefaultScheduleInsert($bovine_id, $event_time, $kamar_event_type, $uuid)  {
         //don't put id value, uses next one in DB.
        $query = "INSERT INTO bovinemanagement.kamar_event (bovine_id,scheduled_event_time,kamar_event_type,scheduled_userid,protocol_uuid) VALUES ($bovine_id,'$event_time','$kamar_event_type','{$GLOBALS['auth']->getUsername()}','$uuid')";
        $res = $GLOBALS['pdo']->exec($query);       
    }
    

    /**
     * 
     * JSON KAMAR
     * 
     * 
     */
    //group


    public function AllKamarCurrent($request) {
        $bovine_id = Sanit::reqVal('item', false, 'INT');

        if ($bovine_id == true) {
            //show for specific cow
            $sql = "SELECT id,bovine_id,event_time,kamar_event_type,userid,local_number,full_name,location_name FROM bovinemanagement.kamar_event_curr WHERE bovine_id=$bovine_id limit 1";
            return $this->jsonKamarFormatter($sql);
        } else {
            //show for all cows.
            $sql = "SELECT id,bovine_id,event_time,kamar_event_type,userid,local_number,full_name,location_name FROM bovinemanagement.kamar_event_curr";
            return $this->jsonKamarFormatter($sql);
        }
    }

    //does the heavy lifting, needed for individual animal or group queries
    private function jsonKamarFormatter($sql) {

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

          $delButton=new QF2_DeleteButtonDecider('user_1only',(object) array('schemaTable'=>'bovinemanagement.kamar_event'));
        
        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] = date("l M d H:00", strtotime($row['event_time']));  //has to be at end.

            if (trim($row['kamar_event_type']) == 'red') {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            $row['action']=$delButton->output($row['id']); 
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    
    function displaySingleCowKamarStatusBox($bovine_id){

           $sql = "SELECT id,bovine_id,event_time,kamar_event_type,userid,local_number,full_name,location_name FROM bovinemanagement.kamar_event_curr WHERE bovine_id=$bovine_id limit 1";
           $statement = $GLOBALS['pdo']->prepare($sql);
           $statement->execute();
           $results = $statement->fetchAll(PDO::FETCH_ASSOC);
      
            if (empty($results[0])) {
            $colour = 'green';
            $text = '';
        } elseif ($results[0]['kamar_event_type'] == 'red') {
            $colour = 'red';
            $text = $results[0]['event_time'] . ' '. $results[0]['kamar_event_type'];
        } elseif (($results[0]['kamar_event_type'] == 'partialred') OR ( $results[0]['kamar_event_type'] == 'lost')) {
            $colour = 'yellow';
            $text = $results[0]['event_time'] . ' '. $results[0]['kamar_event_type'];
        } else {
            $colour = 'green';
            $text = $results[0]['event_time'] . ' '.$results[0]['kamar_event_type'];
        }


        return((new BootStrap)->bootstrapSmallBox('Kamar', $text , null, $colour, 'ion-pinpoint'));  
    }
    
    function displayLastAllKamarEvents($bovine_id = null) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
                 
            {
                data: "local_number",
                title: "Animal"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "kamar_event_type",
                title: "Kamar Status"
            },
            {
                data: "event_time",
                title: "Event Time"
            },
            {
                data: "userid",
                title: "Employee"
            },        
            {
                data: "action",
                title: "Action"
            } 
EX;
        return ($dt->start('Kamar Events', 31, 'AllKamarCurrent', $bovine_id, $columnsStr, 'highlight_important', ' paging: false'));
    }

}

//end class

Class QF2_EstrusKamarScheduledComplete extends QF2_Base {

    
    function definition($form) {

       //dummy, use directly passed paramaters. 

        return $form; //required       
    }

    function process($value) {
             
       
        
        if (empty($this->param->kamarID)){
            throw new Exception("Error: param->kamarID not provided!");
        }
        /*
             * This is a bit hackish, but as long as code is correct it should be fine. Not efficent for a prepared statement. 
             * 
             */
            $stmt = $GLOBALS['pdo']->prepare("UPDATE bovinemanagement.kamar_event SET event_time=now(),userid='{$GLOBALS['auth']->getUsername()}' WHERE id=:kamarID");
            $stmt->bindParam(':kamarID', $this->param->kamarID);
            $stmt->execute();
       
    }

}//end class

Class QF2_EstrusKamar extends QF2_Base {

    function definition($form) {

        //create list of kamar events. DB does check.
        $kamarEventsArray = array(null => 'Please Select One', 'on' => 'On', 'removed' => 'Removed', 'lost' => 'Lost', 'red' => 'Red', 'partialred' => 'Partially Red', 'verifiedwhite' => 'Verified White');


        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();


        ////
        // $fieldset = $form->addFieldset('header')->setLabel('Record Heat Event');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");
        } else {
            $form->addElement('hidden', 'bovine_id')->setValue($this->param->bovineID);
        }
        $form->addElement('select', 'kamarEventType', null, array('options' => $kamarEventsArray, 'label' => 'Type of Kamar Event'))->addRule("required", "Please specify your type of kamar state change.");
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of kamar event");

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);


        try {
            $res = $GLOBALS['pdo']->beginTransaction();


            $estrus_id = 'null'; //default state is that no estrus entry occurred.
            $kamar_event_id = 'null';


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.kamar_event (bovine_id,event_time,kamar_event_type,userid,estrus_id) VALUES ({$value['bovine_id']},'{$value['event_time']}','{$value['kamarEventType']}','{$GLOBALS['auth']->getUsername()}',$estrus_id)";

            $res = $GLOBALS['pdo']->exec($query); {
                print_r($res);
            }
            $kamar_event_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.kamar_event_id_seq');

            //if the patch is red we assume this is a heat event and have to put it in the estrus event too.
            if ($value['kamarEventType'] == 'red') {
                $estrus_type_id = 16; // this corresponds to 'Kamar Red'
                $query = "INSERT INTO bovinemanagement.estrus_event (bovine_id,event_time,estrus_type_id,userid,kamar_event_id) VALUES ({$value['bovine_id']},'{$value['event_time']}','$estrus_type_id','{$GLOBALS['auth']->getUsername()}',$kamar_event_id)";

                $res = $GLOBALS['pdo']->exec($query); {
                    print_r($res);
                }
                $estrus_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.estrus_event_id_seq');
            }


            //now do an update, to put the proper estrus id in the kamar_event table. Transaction keeps this safe.
            $query = "UPDATE bovinemanagement.kamar_event SET estrus_id=$estrus_id WHERE id=$kamar_event_id";

            $res = $GLOBALS['pdo']->exec($query);


            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

}

//end class
?>