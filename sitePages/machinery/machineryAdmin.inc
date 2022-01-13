<?php 

class machineryAdmin extends TabPage {


    
     public function defaultDisplay() {
        $tabArray[1]['name'] = 'Entry';
        $tabArray[2]['name'] = 'Machine List';
        $tabArray[3]['name'] = 'Filter List';
        $tabArray[4]['name'] = 'xxx';
        $tabArray[5]['name'] = 'yyy';
        $this->renderTabs($tabArray);
    }

   
    
    public function tab1() {
        //print("<h2>Machine Actions</h2>");
        print($this->checkMachineHoursHaveNotBeenEnteredIn3Months());
        $a=new QF2_UploadMachinePicture((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'default');
        print($a->renderBox('QF2_MachineUploadPicture','Upload Machine Photo'));  
        
        
         $b=new QF2_Hours((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'default');
         print($b->renderBox('QF2_Hours','Enter Machine Hours'));  
        
        $c=new QF2_AddSeriveItemToMachine((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'default');
        print($c->renderBox('QF2_AddSeriveItemToMachine','Enter Service Item'));  
        
         
        //$this->uploadPicture();
        
        
    }
    
    public function tab2() {
        print('<hr/>');
        $this->addNewMachine();
         print('<hr/>');
        $this->displayMachineListAll();
    }
    
    public function tab3() {
        
       $this->displayFiltersList();
    }

    public function tab4() {
        print('<h1>JD filters spring order, check every year or fix this to be proper.</h1>');
        
        
        $sql=<<<SQL
              SELECT machine.name ,service_item.name as "Part Name",extract(hour FROM hours) as Hours,concat(part_num, ' ',part_num2) as "Part #" , service_item.id as "Service Item ID"
FROM machinery.service_item 
LEFT JOIN machinery.machine ON machine.id=machine_id
WHERE machine_id = ANY ('{85,95,67,78,96,59,76,68,96}'::int[]) ORDER BY machine.name ASC  
SQL;        
       print((new JQueryDataTable)->startSql($sql));         
    }
    
    public function tab5() {
      print('ttt');
    }
    
    
   function displayFiltersList() {
 $sql = <<<SQL
       SELECT concat(part_num, ' ',part_num2) as "Part #",napa_part_num,machine.name as machine_name,service_item.name as filter_info
from machinery.service_item
LEFT JOIN machinery.machine on machine_id=machine.id where is_filter=true and machine.active =true
order by machine_name,part_num
SQL;
 

      print((new JQueryDataTable)->startSql($sql));       
   }
    
     function displayMachineListAll() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                        
            {
                data: "name",
                title: "name"
              
            },          
            {
                data: "id",
                title: "id" 
            },
            {
                data: "serial_num",
                title: "serial_num"
        },
        {
                data: "active",
                title: "active"
        },
        {
                data: "picture",
                title: "picture"
        }
           
EX;
    
        print($dt->start('List of All Machines', 117, 'machineListAll',  null, $columnsStr, null, false," \"pageLength\": 100,"));
    }
    
    public function machineListAll() {
       $sql = "SELECT machine.id,machine.name,machine.serial_num,machine.active,pic.id::boolean as picture FROM machinery.machine LEFT JOIN picture.machine as pic ON pic.machine_id=machine.id ORDER BY name";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            /*
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['note'] = $this->reasonsNotToBreed($row['bovine_id'], $row['event_time'], $row['birth_date']);
            $row['action'] = $this->printActionButtons2($row['bovine_id'], $row['event_time'],$row['estrus_id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['event_time'] = date("l M d H:00", strtotime($row['event_time']));  //has to be at end.
            */
          
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    

    /*     * ***************************************************** */
    /* Adds a new machine to db                 */
    /*     * *************************************************** */

    public function addNewMachine() {
       

        $form = new HTML_QuickForm("addNewMachineQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);   $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Add New Machine');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        $form->addElement('text', 'name', 'Name');
        $form->addElement('text', 'serial', 'Serial Number');


        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('name', 'Name is required', 'required');
        $form->addRule('serial', 'Serial is required', 'required');


        // Try to validate a form
        if ($form->validate()) {




            //get values
            $name = pg_escape_string(trim($form->exportValue('name')));
            $serial = pg_escape_string(trim($form->exportValue('serial')));



            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            //do insert
            //delete the dryoff_event
            $res = $GLOBALS['pdo']->query("INSERT INTO machinery.machine (name,serial_num,active) VALUES ('$name','$serial','true')");

            // determine if the commit or rollback
   
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            


            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    public function uploadPicture() {
     

      
        $machineNumber = MachineQuery::createListOfAllActiveMachines('default'); //input compact here to see small version


        PEAR::setErrorHandling(PEAR_ERROR_TRIGGER); //why here????      


        $form = new HTML_QuickForm("uploadPhotoQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Add Machine Photo');
      


        $form->setMaxFileSize('5120000');
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        $form->addElement('select', 'item1', 'Machine', $machineNumber);
        $form->addElement('file', 'imageName1', 'Image 1:');
       


        //RULES //NOTE checck php.ini upload file size if you have problems.
        $form->addRule('item1', 'Machine is required', 'required');
        $form->addRule('imageName1', 'Photo 1 is required', 'required', null);
        $form->addRule('imageName1', 'The file is larger than 18MB', 'maxfilesize', 18120000);
        $form->addRule('imageName1', 'Photo Needs to be a jpg', 'mimetype', 'image/jpeg');




        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');




        // Try to validate a form
        if ($form->validate()) {
            $form->freeze();

            $photoArray = array();

            //make an array from the form data.
            if (($form->getElement('item1') != '') && ($form->getElement('imageName1') != '')) {
                $photoArray[1]['item'] = $form->getElement('item1');
                $photoArray[1]['imageName'] = $form->getElement('imageName1');
            }


            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();



            foreach ($photoArray as $key => $value) {
                $item_id = null;
                $escaped_data = null;
                $event_time = null;

                if ($value['imageName']->_value['tmp_name'] != null) {
                    print("KEY:$key<br>");
                    $event_time = date('r', strtotime("now"));
                    $item_id = $value['item']->_values[0];
                    $tmpFileName = ($value['imageName']->_value['tmp_name']);
                    $data = resizeJPEG($tmpFileName);  //change pic size to smaller
                    $escaped_data = pg_escape_bytea($data); //escape data
                    $type = 'machine';

                    $sqlInsert = "INSERT INTO picture.machine (machine_id,event_time,picture) VALUES ($item_id,'$event_time','$escaped_data'::bytea)";

                   

                    $res = $GLOBALS['pdo']->exec($sqlInsert); //insert
                }
            }


            // determine if the commit or rollback
     
    
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    
    
function checkMachineHoursHaveNotBeenEnteredIn3Months() {
    // $txt = "Error: " . round($value, $roundValue) . " out of range: (" . round($referenceValue - 2 * $safeLowerBound, $roundValue) . " to " . round($referenceValue + 2 * $safeUpperBound, $roundValue) . "), rebalance!";
    //            $msg = $msg . (new BootStrap)->errorNotify($txt);
    $sql = "with temp  as (
SELECT *, (SELECT event_time from machinery.hours_log WHERE event_time >= (current_date - interval '90 days') AND machine_id=machine.id order by event_time DESC limit 1) as event_time
FROM machinery.machine WHERE hours_log = true and active = true 
)
select * from temp where event_time is null order by name";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        
        $out=array();
        
        if (!empty($results)) {
             $out[]='<h3>Need hours Checked</h3>';
         $out[]='<ul>';
     foreach ($results as &$row) {
         
         $out[]='<li>'.$row['name'].'</li>';
         
     }
      $out[]='</ul>';
  
     return ((new BootStrap)->errorNotify(implode($out))); 
        }
        else {
            return null;
        }
} 
    
    
}//end class 
         
Class QF2_UploadMachinePicture extends QF2_Base {

    function definition($form) {

        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
      


           require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/machinery/machineQuery.inc');
        $machineNumber = MachineQuery::createListOfAllActiveMachines('default'); //input compact here to see small version



        ////
       // $fieldset = $form->addFieldset('header')->setLabel('Record Heat Event');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

            $form->addElement('select', 'machine_id', null, array('options' => $machineNumber, 'label' => 'Machine'))->addRule("required", "Please specify your Machine.");
            
      $fsUpload = $form->addElement('fieldset')->setLabel('Upload picture');
$upload = $fsUpload->addElement('file', 'imageName', array('style' => 'width: auto'))
                   ->setLabel('Picture (jpg, &lt;=20MB):');
// no longer using special 'uploadedfile' rule for uploads, allow client-side validation
$upload->addRule('required', 'Please upload picture', null,
                 HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
// no longer using 'filename' rule for uploads, allow client-side validation
$upload->addRule('regex', 'Allowed extensions:  .jp(e)g', '/\\.(jpe?g)$/i',
                 HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
// these don't work client-side, for obvious reasons
$upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an image',
                 array('image/jpeg'));
$upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);
            
         
        
        return $form; //required       
    }

    function process($value) {
   
        
        $value['event_time'] =date('r',strtotime("now"));  
        
        if (is_uploaded_file($_FILES['imageName']['tmp_name'])) {

        $tmpFileName=$_FILES['imageName']['tmp_name'];
        }
        else {
            throw new Exception("Error possible upload attack.");
        }
        
                 
                       
                         $data=resizeJPEG($tmpFileName);  //change pic size to smaller
                         $escaped_data =  pg_escape_bytea($data); //escape data
                            
                         
        $query = "INSERT INTO picture.machine (machine_id,event_time,picture,userid) VALUES ({$value['machine_id']},'{$value['event_time']}','$escaped_data'::bytea,'{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);
    }

    
    
    
    
}//end class

class QF2_AddSeriveItemToMachine extends QF2_Base {

    public function generateServiceHoursArray() {
        
                  //hours array
              $hoursArray = array(null => 'Please Select One');
        for ($counter = 50; $counter <= 250; $counter += 50) {
            $hoursArray[$counter] = "$counter hours";
        }
        for ($counter = 250; $counter <= 6500; $counter += 250) {
            $hoursArray[$counter] = "$counter hours";
        }
        
        return $hoursArray;
    }
    
    
    function definition($form) {

        $machineNumber = MachineQuery::createListOfAllActiveMachines('default'); //input compact here to see small version
        $form->addElement('select', 'machine_id', null, array('options' => $machineNumber, 'label' => 'Machine'))->addRule("required", "Please specify your Machine.");
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        $form->addElement('textarea', 'name', null, array('label' =>'Item Action'))->addRule("required", "Please specify your item.");
        $form->addElement('text', 'part_num', null, array('label' =>'Part Number (offical)'))->addRule("required", "Please specify your part number.");;
        $form->addElement('text', 'napa_part_num', null, array('label' =>'NAPA Part Number (optional)'));
        
        $hoursArray=$this->generateServiceHoursArray();
        $form->addElement('select', 'hours', null, array('options' => $hoursArray, 'label' => 'Number of Hours'))->addRule("required", "Please specify your service interval in hours.");
        
        $form->addElement('checkbox', 'is_filter', array('id' => 'boxId', 'value' => 'yes'))->setContent('Is this a filter?');
        
        //look up previoud machine done and make that the defualt. //a bit hackish, depends on id order, this is not really guranteed. 
        $sql = "SELECT machine_id FROM machinery.service_item ORDER BY id DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('machine_id'=>$row['machine_id'])));
        
        return $form;
    }

    function process($value) {
      
        if (empty($value['is_filter'])) {
            $value['is_filter']=false;
        }
        
          //we have hours entry so add it.   
            $query = "INSERT INTO machinery.service_item (machine_id,name,part_num,hours,napa_part_num,is_filter) VALUES (:machine_id,:name,:part_num,:hours,:napa_part_num,:is_filter)";
            $res = $GLOBALS['pdo']->prepare($query);
            $res->bindValue(':machine_id', $value['machine_id'], PDO::PARAM_INT);
            $res->bindValue(':name', $value['name'], PDO::PARAM_STR);
            $res->bindValue(':part_num', $value['part_num'], PDO::PARAM_STR);
            $res->bindValue(':hours', $value['hours'] . ' hours', PDO::PARAM_STR);
            $res->bindValue(':napa_part_num', $value['napa_part_num'], PDO::PARAM_STR);
            $res->bindValue(':is_filter', $value['is_filter'], PDO::PARAM_BOOL);
            $res->execute();
        }
    

}//end class