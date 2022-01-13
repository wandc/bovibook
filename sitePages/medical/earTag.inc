<?php 
class EarTag extends TabPage {

    public $leftTagType = 'Ultraflex XL Panel';
    public $rightTagType = 'Ultraflex RFID Button';

   
    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Missing Tags';
        $tabArray[2]['name'] = 'Ear Tag Log';
               if ($GLOBALS['auth']->getOwnerAccess() == 1) {
        $tabArray[3]['name'] = 'Valid Tag Range';
                }
        $this->renderTabs($tabArray);
    }

    function tab1() {

      

        print("<div class='row'>");
        print("<div class='col-sm-4 col-sm-push-8'>");
       
        $a=new QF2_OrderEarTag((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('QF2_OrderEarTag','Mark Animal(s) Needing an Ear Tag'));  
       
        
     if ($GLOBALS['auth']->getOwnerAccess() == 1) {   
      $this->displayTagsToOrderEmail();
      print($this->markAllOrderedQuickForm());
      }
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
      
           $this->displayMissingTags();
        print("</div>");
        print("</div>");
        
        
      
    }

    function tab2() {
        $this->displayTagLog();
    }
    
       function tab3() {
           
           
        print("<div class='row'>");
        print("<div class='col-sm-4 col-sm-push-8'>");
       
     
     if ($GLOBALS['auth']->getOwnerAccess() == 1) {   
        $a=new QF2_EarTagRangeEntry((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'individual');
        print($a->renderBox('qf2_EarTagRangeEntry','Record Ear Tag Range'));       
      }
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
      
           $this->displayTagRanges();
        print("</div>");
        print("</div>");
        
           
         
  
    }
    

    public function tagRanges() {

$sql = <<<SQL
with temp as (
                     SELECT *, lower(rfid_range),upper(rfid_range), 
                         (SELECT max(rfid_number) as max_used FROM bovinemanagement.bovine WHERE bovine.rfid_number <@ eartag_valid.rfid_range)
                     FROM bovinemanagement.eartag_valid order by event_time
)
SELECT *, upper-max_used as remaining_tags_in_series FROM temp
SQL;

        


        

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           if ($GLOBALS['auth']->getOwnerAccess() == 1) {
           $row['action']=$this->deleteButton($row['update_time'],$row['rfid_range']);
            }
        }

        //output
        $output['data'] = $results;
        return ($output);
    }  
    
    private function deleteButton($update_time,$id) { //update_time 
         if (strtotime($update_time) >= strtotime('-1 week')) {
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
                $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $id,'deleteName'=>'rfid_range', 'schemaTable' => 'bovinemanagement.eartag_valid'));
                $deleteButtonAsString =''. ($a->renderDelete('tag_range_delete' . $id));
            } else {
                $deleteButtonAsString = '';
            }
            return $deleteButtonAsString;
    }
    
    
     function displayTagRanges() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
             {
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
                {
                data: "purebred",
                title: "purebred"
        },
            {
                data: "rfid_range",
                title: "rfid_range",       
            },
            {
                data: "local_num_sig_digits",
                title: "Sig Digits",       
            },
            
        {
                data: "local_num_offset",
                title: "Num Offset"
        },
                {
                data: "remaining_tags_in_series",
                title: "Remaining Tags in Series"
        },
                
                   {
                data: "action",
                title: "Action"
        }
           
EX;
    
        print($dt->start('RFID valid tag ranges', 13, 'tagRanges', null, $columnsStr));
    }

    

    function printActionButtons($id,$callBackURL = null) {
       

        $currentUserid = $GLOBALS['auth']->getUsername();
        $returnButtonsAsString = null;

      
     
      

            $res = $GLOBALS['pdo']->query("SELECT id,ordered_event_time FROM bovinemanagement.eartag WHERE userid='$currentUserid' AND ordered_event_time is null ORDER BY update_time DESC LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
              if (($id == $row['id'])) {
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'bovinemanagement.eartag'));
            $returnButtonsAsString = ($a->renderDelete('eartag' . $row['id']));
        } elseif ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {

            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $id, 'schemaTable' => 'bovinemanagement.eartag'));
            $returnButtonsAsString = ($a->renderDelete('eartag' . $id));
        }



        //print button
        return $returnButtonsAsString;
    }

   

    /**
      Button that is clicked after all tags are ordered
     * */
    function markAllOrderedQuickForm() {

        $form = new HTML_QuickForm("eartagOrderedButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('submit', 'btnSubmit', 'Mark Tags Ordered', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure the tag order email been sent?')")); //call javascript to confirm submit.	   
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');


            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            //mark all tags who are currently not ordered as ordered.

            $res = $GLOBALS['pdo']->exec("UPDATE bovinemanagement.eartag set ordered_event_time='" . date('r', strtotime("now")) . "' WHERE ordered_event_time IS NULL");

            // determine if the commit or rollback
           
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            
            }


            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

   
    
    public function missingTags() {
         $sql = "SELECT eartag.id,bovineall.id as bovine_id,bovineall.local_number,bovineall.full_name,eartag.side,eartag.event_time ,userid
		FROM bovinemanagement.eartag 
		LEFT JOIN bovinemanagement.bovineall ON bovineall.id=eartag.bovine_id 
		WHERE ordered_event_time is null
		GROUP BY eartag.event_time,bovineall.local_number,bovineall.full_name,bovineall.id ,eartag.side,eartag.id 
		ORDER BY event_time DESC";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['action'] = $this->printActionButtons($row['id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['event_time'] = date("M d Y", strtotime($row['event_time']));  //has to be at end.
            
        }

        //output
        $output['data'] = $results;
        return ($output);
    }  
    
    
    
    
     function displayMissingTags() {
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
                data: "side",
                title: "Side"
            },
                {
                data: "event_time",
                title: "Event Time"
            },
            {
                data: "userid",
                title: "Employee",       
            },
            {
                data: "action",
                title: "Action",       
            }
            
EX;
        print("<h3>Missing Tag List</h3>");
        print($dt->start('Title CHANGE ME', 13, 'missingTags', null, $columnsStr));
    }
    
    

    function displayTagsToOrderEmail() {

        print("<h3>Email To Send</h3>");

        $str = "";
        $str = $str . "<p>To:  order@nlid.org</p></br>";
        $str = $str . "<p>Subject: ".$GLOBALS['config']['HOLSTEIN_CANADA']['prefix']." Tag Order (" . date('M d, Y', strtotime("now")) . ")</p></br>";
        $str = $str . "<p>Can I have a RUSH order shipped purolator express? I need replacement tags for the following <b>".$GLOBALS['config']['HOLSTEIN_CANADA']['prefix']."</b> animal(s):</p></br>";

        $sql = "SELECT eartag.id,bovineall.id as bovine_id,bovineall.local_number,bovineall.full_name,eartag.side,eartag.event_time,bovineall.full_reg_number
		FROM bovinemanagement.eartag 
		LEFT JOIN bovinemanagement.bovineall ON bovineall.id=eartag.bovine_id 
		WHERE ordered_event_time is null
		GROUP BY eartag.event_time,bovineall.local_number,bovineall.full_name,bovineall.id ,eartag.side,eartag.id,bovineall.full_reg_number
		ORDER BY event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $eartagType = null;
            if (trim($row['side']) == 'left') {
                $eartagType = $this->leftTagType;
            } elseif (trim($row['side']) == 'right') {
                $eartagType = $this->rightTagType;
            } else {
                throw new Exception("Ear tag side not specified for Animal # {$row['local_number']}.</br>");
            }

            $str = $str . $row['local_number'] . " " . $row['full_reg_number'] . " " . $eartagType . "</br>";
        }

        $str = $str . "</br><p>Note, the tag type specified is not necessarily what was originally on the animal. All tag(s) were lost.</p>";


        print("$str");
    }

    



    
    public function taglog() {
          $sql = "SELECT eartag.id,bovineall.id as bovine_id,bovineall.local_number,bovineall.full_name,eartag.side,eartag.event_time,eartag.ordered_event_time as event_time2,locationcurr.name as location_name
		FROM bovinemanagement.eartag 
		LEFT JOIN bovinemanagement.bovineall ON bovineall.id=eartag.bovine_id 
                LEFT JOIN bovinemanagement.locationcurr ON bovineall.id=locationcurr.bovine_id 
		GROUP BY eartag.event_time,bovineall.local_number,bovineall.full_name,bovineall.id ,eartag.side,eartag.id,locationcurr.name 
		ORDER BY event_time DESC";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] =  empty($row['event_time']) ? null : date("M d Y", strtotime($row['event_time']));
            $row['event_time2'] =  empty($row['event_time2']) ? null : date("M d Y", strtotime($row['event_time2']));
            
           
           
        }

        //output
        $output['data'] = $results;
        return ($output);
    }  
    
    
    
    
     function displayTagLog() {
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
                data: "side",
                title: "Side"
            },
                {
                data: "event_time",
                title: "Submitted Time"
            },
             {
                data: "event_time2",
                title: "Ordered Time"
            },
                {
                data: "location_name",
                title: "Location"       
            }
            
EX;
        print("<h3>Ear Tag Log</h3>");
          print($dt->start('Title CHANGE ME', 13, 'taglog',  null, $columnsStr, null, false," \"pageLength\": 100,"));
    }
    
     public function error() {
         $a1=$this->errorTooManyTagsinQueueOrderMore();
         $a2=$this->errorLocalNumberAndRFIDDoNotMatch();
         $a3=$this->errorAlmostOutOfTagsInCurrentSeries();
         $error=array_merge($a1,$a2,$a3);
         return $error; 
     }
    //passes along error info.
    public function errorTooManyTagsinQueueOrderMore() {
        $error = array();


        //if two or more right side tags need to be ordered then notify
        $res = $GLOBALS['pdo']->query("SELECT Distinct  (side) side,count(eartag.id)
		FROM bovinemanagement.eartag 
		LEFT JOIN bovinemanagement.bovineall ON bovineall.id=eartag.bovine_id 
		WHERE ordered_event_time is null
		GROUP BY eartag.side");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if (($row['side'] == 'right') && ($row['count'] >= 2)) {
                $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 2, $row['count'] . ' ' . $row['side'] . ' side eartags need to ordered.');
            } elseif (($row['side'] == 'left') && ($row['count'] >= 6)) {
                $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 3, $row['count'] . ' ' . $row['side'] . ' side eartags need to ordered.');
            }
        }


        return $error;
    }
    
    public function errorAlmostOutOfTagsInCurrentSeries() {
        $error = array();
 
        
        //pure bred
        $sql = <<<SQL
 SELECT upper(rfid_range)-rfid_number as from_end_of_range,rfid_range, rfid_number, purebred,  right(rfid_number::varchar,local_num_sig_digits)::numeric as local_number
                 FROM bovinemanagement.bovine
                 LEFT JOIN bovinemanagement.eartag_valid ON rfid_range @> rfid_number
                 WHERE purebred=true
                 ORDER BY birth_date desc,rfid_number desc
                 LIMIT 1
SQL;
       
        //if two or more right side tags need to be ordered then notify
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
      if ($row['from_end_of_range']<=10) {
          
         $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 1, $row['from_end_of_range'] . ' tags left, pure bred tags need to be ordered! It takes 3 weeks.');  
      }

      //beef tags
              $sql = <<<SQL
 SELECT upper(rfid_range)-rfid_number as from_end_of_range,rfid_range, rfid_number, purebred,  right(rfid_number::varchar,local_num_sig_digits)::numeric as local_number
                 FROM bovinemanagement.bovine
                 LEFT JOIN bovinemanagement.eartag_valid ON rfid_range @> rfid_number
                 WHERE purebred=false
                 ORDER BY birth_date desc,rfid_number desc
                 LIMIT 1
SQL;
       
        //if two or more right side tags need to be ordered then notify
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
      if ($row['from_end_of_range']<=10) {
          
         $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 1, $row['from_end_of_range'] . ' tags left, beef tags need to be ordered! It takes 3 weeks.');  
      }
      
      
    

        return $error;
    }
    
    
   public function errorLocalNumberAndRFIDDoNotMatch() {
          $error = array();
        $sql = <<<SQL
 SELECT  local_number,rfid_number,bovinemanagement.rfid_purebred( rfid_number),bovinemanagement.rfid_to_local_number (rfid_number) as rfid_to_local_number
from bovinemanagement.bovine where bovinemanagement.rfid_to_local_number (rfid_number) != local_number 
SQL;
          $res = $GLOBALS['pdo']->query($sql);
    while (($row = $res->fetch(PDO::FETCH_ASSOC))) {      
        
    
      $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 2, $row['local_number'] . 'The RFID#: ' . $row['rfid_number'] .' and the DB generated local#: '. $row['rfid_to_local_number'] . ' do not match. This usually means the RFID number was changed for the urban feeder to work.');
      
    }
    return $error;
}
}
//end class


        
Class QF2_EarTagRangeEntry extends QF2_Base {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //last year.
 
        // create array significant digits
        $sigArr = array(null => 'Please Select One');
        for ($counter = 3; $counter <= 8; $counter +=1) {
            $sigArr ["$counter"] = "$counter";
        }

        //->addRule
         $range1=$form->addElement('text', 'range1', null, array( 'label' => 'Start of Tag Range'));
         $range1->addRule("required", "Please specify your start range.");
         $range1->addRule("length",'Needs to be 15 digits exactly.',array('min' => 15, 'max' => 15) );
         $range1->addRule('regex', 'Needs to be numbers only.', '/^\d{15,15}$/');
         
         $range2=$form->addElement('text', 'range2', null, array( 'label' => 'End of Tag Range'));
         $range2->addRule("required", "Please specify your end range.");
         $range2->addRule("length",'Needs to be 15 digits exactly.',array('min' => 15, 'max' => 15) );
         $range2->addRule('regex', 'Needs to be numbers only.', '/^\d{15,15}$/');

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
       
        $form->addElement('select', 'sigdigits', null, array('options' => $sigArr, 'label' => '# of Digits (Large Number)'))->addRule("required", "Please specify your number of digits.");
        $form->addElement('select', 'purebred', null, array('options' => array(null => 'Please Select One','yes'=>'yes','no'=>'no'), 'label' => 'Purebred Tag'))->addRule("required", "Please specify if your tag is purebred or not.");
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Date of First Use'))->addRule("required", "Please specify when tag range first added to herd.");

        
        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);
        $value['estrus_type_id'] = $value['heatType'];

        if ($value['purebred']=='yes') {
             $value['purebred']='true';
        }
        else {
             $value['purebred']='false';
        }
            
        //don't put id value, uses next one in DB.
       $query =  "INSERT INTO bovinemanagement.eartag_valid (rfid_range,local_num_sig_digits,local_num_offset,event_time,purebred) VALUES ( '[{$value['range1']}, {$value['range2']}]',{$value['sigdigits']},0,'{$value['event_time']}',{$value['purebred']})"; 
        $res = $GLOBALS['pdo']->exec($query);
    }

}//end class   

Class QF2_OrderEarTag extends QF2_Base {

    function definition($form) {
       
       
      
       
        $earTagSide = array(null => 'Please Select one...','left' => 'Left', 'right' => 'Right (RFID)');       //sides


        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
       if ($this->mode == 'group') {
            $form=(new Quickform2_GroupSelect())->definitionAnimalGroup($form);
        }
        else {
            $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form);
        }
              
        $form->addElement('select', 'earTagSide', null, array('options' => $earTagSide, 'label' => 'Ear Tag Side'))->addRule("required", "Please specify your ear tag side.");   
       
        
         // Set defaults for the form elements
$form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('event_time' => 1)));
        
  return $form; //required   
    }

    
   
    
    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', strtotime("now"));

        $bovineIdArr=(new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
        
        try {
            $res = $GLOBALS['pdo']->beginTransaction();
   
          foreach ($bovineIdArr as $key => $bovineId) {
            
  
   $query = "INSERT INTO bovinemanagement.eartag (event_time, bovine_id, side, userid) VALUES ('{$value['event_time']}',$bovineId,'{$value['earTagSide']}','{$GLOBALS['auth']->getUsername()}') ON CONFLICT DO NOTHING";
              
            
            $res = $GLOBALS['pdo']->exec($query);
                           
          }
            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }


}//end class   