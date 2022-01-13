<?php 

class Cqm extends TabPage {

    public function defaultDisplay() {
        //setup for tabs.
        $tabArray[1]['name'] = 'Record 16: Emergency Plans';
        $tabArray[2]['name'] = 'Record 17: Corrective Action';
        $tabArray[3]['name'] = 'Record 13: Milking Equipment Sanitation Record';
        $tabArray[4]['name'] = 'Record 9: Chemicals Used on Livestock';
        $tabArray[5]['name'] = 'Record 8: Veterinary Prescriptions';
        $this->renderTabs($tabArray);
    }

//end function

    function tab1() {
        print(self::record16());
    }

    function tab2() {
        print(self::record17());
    }

    function tab3() {
        self::record13();
    }

    function tab4() {
        self::record9();
    }

    function tab5() {
        self::record8();
    }

    function record8() {
        print('<p> See Black Binder in vet room.</p>');
    }

    function record16() {
      

        $row1 = array('Area of Concern' => 'Cooling and storage of milk',
            'Specific Incidence' => "Milk is not cooled to between 1 &#176; C to 4&#176;C within the acceptable cooling period",
            'Corrective Action to be Taken' => 'Call DeLaval 24/7 support at 506-432-6944 or Ross Refridgeration 24/7 support at 902-597-3933',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );
        $row2 = array('Area of Concern' => 'Equipment sanitation',
            'Specific Incidence' => "Visible milk residue buil-up on milk contact surfaces",
            'Corrective Action to be Taken' => 'Check all chemical lines and containers. Rewash system.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );
        $row3 = array('Area of Concern' => 'Equipment sanitation',
            'Specific Incidence' => "Improper water temperature",
            'Corrective Action to be Taken' => 'Rewash system and check temperture on top of hot water tank and on TTR to try and find problem.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );
        $row4 = array('Area of Concern' => 'Use of water for cleaing of milk contact surfaces',
            'Specific Incidence' => "Water test result reveals a form of contanimation (e.g. high bacteria)",
            'Corrective Action to be Taken' => 'Shock the dug well and holding tank. Switch to back-up drilled well and re-test the water.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );
        $row5 = array('Area of Concern' => 'Medicines or chemicals used on livestock',
            'Specific Incidence' => "Improper administrion of livestock medicines or chemicals",
            'Corrective Action to be Taken' => 'Record a medical note on the cows file. Place cow in sick group.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );
        $row6 = array('Area of Concern' => 'Milking Treated Animals',
            'Specific Incidence' => "Milk from treated animals enters the bulk tank",
            'Corrective Action to be Taken' => 'Place a note on the tank for it to not be picked up. Agitate tank for 5 minutes and take a milk sample. Take sample to dairytown in Sussex. Dump milk or remove sign, depending on result.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );
        $row7 = array('Area of Concern' => 'Shipping Animals',
            'Specific Incidence' => "Animal is shipped with a chemical residue (e.g. antibiotics) or broen needle in it and the next buyer is not informed.",
            'Corrective Action to be Taken' => 'Contact the buyer.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255) or Sussex CO-OP Sarah 432-1824'
        );

        $row8 = array('Area of Concern' => 'Downer Cows',
            'Specific Incidence' => "Animal is down in barn and will not get up.",
            'Corrective Action to be Taken' => 'Using the down cow platform and 204k will pallet forks, roll the down cow unto the platform. In summer put the down cow outside and in winter put them in the calving pen.',
            'Contact Person' => 'Management (David 372-4564, Fred 372-8812/9255)'
        );

      
        $contentArr = array($row1, $row2, $row3, $row4, $row5, $row6, $row8);

        $str[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $contentArr));
       return implode($str);
        
    }

    function record9() {
        print($this->record9Display());
    }

    function record17() {
       
        $right=($this->record17Quickform());
        $left=($this->record17Display());
        
      return  (new BootStrap)->oneThirdTwoThirds($left, $right);
        
    }

    function record13() {
      
        $right=$this->record13Quickform();
        $left=$this->record13Display();

        
        print((new BootStrap)->fourFifthOneFifth($left, $right));
      
    }

    function record13Quickform() {

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForLastFourDays();
        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.
        //list of pre-canned comments
        $cannedComment = array(null => 'Please Select one...', 'Cleaned Dirty Parts.' => 'Cleaned Dirty Parts.');

        $form = new HTML_QuickForm("record17Quickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Milking Equipment Sanitation Record');
        $form->addElement('hidden', 'pageid', $this->pageid);


        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);

        $form->addElement('advcheckbox', 'tanka_paddle', 'Tank A Paddle');
        $form->addElement('advcheckbox', 'tanka_dipstick', 'Tank A Dipstick');
        $form->addElement('advcheckbox', 'tanka_surface', 'Tank A Surface');
        $form->addElement('advcheckbox', 'tanka_outlet', 'Tank A Outlet');
        $form->addElement('advcheckbox', 'tanka_valve', 'Tank A Valve');
        $form->addElement('advcheckbox', 'tanka_gaskets', 'Tank A Gaskets');
        //
        $form->addElement('advcheckbox', 'tankb_paddle', 'Tank B Paddle');
        $form->addElement('advcheckbox', 'tankb_dipstick', 'Tank B Dipstick');
        $form->addElement('advcheckbox', 'tankb_surface', 'Tank B Surface');
        $form->addElement('advcheckbox', 'tankb_outlet', 'Tank B Outlet');
        $form->addElement('advcheckbox', 'tankb_valve', 'Tank B Valve');
        $form->addElement('advcheckbox', 'tankb_gaskets', 'Tank B Gaskets');
        //
        $form->addElement('advcheckbox', 'parlor_receiverjar', 'Parlor Receiver Jar');
        $form->addElement('advcheckbox', 'parlor_pipelineinlets', 'Parlor Pipeline Inlets');
        $form->addElement('advcheckbox', 'parlor_inflations', 'Parlor Inflations');
        $form->addElement('advcheckbox', 'parlor_milkhoses', 'Parlor Milk Hoses');
        $form->addElement('advcheckbox', 'parlor_claws', 'Parlor Claws');
        $form->addElement('advcheckbox', 'parlor_meters', 'Parlor Meters');
        $form->addElement('advcheckbox', 'parlor_gaskets', 'Parlor Gaskets');
        $form->addElement('advcheckbox', 'parlor_filtercoil', 'Parlor Filter Coil');
        $form->addElement('advcheckbox', 'parlor_buckets', 'Parlor Buckets');
        $form->addElement('advcheckbox', 'parlor_sanitarytrap', 'Parlor Sanitary Trap');
        //
        $form->addElement('advcheckbox', 'parlor_hotwatertemp', 'Hot Water Temperature');
        //
        $form->addElement('select', 'comment1', 'Corrective Action', $cannedComment);
        $form->addElement('textarea', 'comment2', 'comment', array('cols' => 40, 'rows' => 6));

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        $form->addRule('dateEvent', 'Event Time is required', 'required');
        //$form->addRule('corrective_action', 'Corrective Action is required', 'required');
        //custom rule for teats (at least one needs to be selected)
        $form->registerRule('commentRequiredRule', 'function', 'commentRequired', 'MedicalCase');
        // $form->addRule(array('animalNumber','comment1','comment2'),'A canned or custom comment is required.','commentRequiredRule',array('animalNumber','comment1','comment2'));
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');
            $comment1 = trim($form->exportValue('comment1'));
            $comment2 = trim($form->exportValue('comment2'));

            //concatenate comments if they are both entered.
            if (strlen($comment1) == 0) {
                $comment = $comment2;
            } else {
                $comment = $comment1 . ' ' . $comment2;
            }


            //do postgres escape.
            $corrective_action_esc = pg_escape_string($comment);

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO misc.cqm_record13 (event_time,corrective_action,userid,
	tanka_paddle,
	tanka_dipstick,
	tanka_surface,
	tanka_outlet,
	tanka_valve,
	tanka_gaskets,
	
	tankb_paddle,
	tankb_dipstick,
	tankb_surface,
	tankb_outlet,
	tankb_valve,
	tankb_gaskets,
	
	parlor_receiverjar,
	parlor_pipelineinlets,
	parlor_inflations,
	parlor_milkhoses,
	parlor_claws,
	parlor_meters,
	parlor_gaskets,
	parlor_filtercoil,
	parlor_buckets,
	parlor_sanitarytrap,
        parlor_hotwatertemp
	) VALUES ('$event_time','$corrective_action_esc','{$GLOBALS['auth']->getUsername()}',
	" . $this->truefalse($form->exportValue('tanka_paddle')) . ",
	" . $this->truefalse($form->exportValue('tanka_dipstick')) . ",
	" . $this->truefalse($form->exportValue('tanka_surface')) . ",
	" . $this->truefalse($form->exportValue('tanka_outlet')) . ",
	" . $this->truefalse($form->exportValue('tanka_valve')) . ",
	" . $this->truefalse($form->exportValue('tanka_gaskets')) . ",
	
	" . $this->truefalse($form->exportValue('tankb_paddle')) . ",
	" . $this->truefalse($form->exportValue('tankb_dipstick')) . ",
	" . $this->truefalse($form->exportValue('tankb_surface')) . ",
	" . $this->truefalse($form->exportValue('tankb_outlet')) . ",
	" . $this->truefalse($form->exportValue('tankb_valve')) . ",
	" . $this->truefalse($form->exportValue('tankb_gaskets')) . ",
	
	" . $this->truefalse($form->exportValue('parlor_receiverjar')) . ",
	" . $this->truefalse($form->exportValue('parlor_pipelineinlets')) . ",
	" . $this->truefalse($form->exportValue('parlor_inflations')) . ",
	" . $this->truefalse($form->exportValue('parlor_milkhoses')) . ",
	" . $this->truefalse($form->exportValue('parlor_claws')) . ",
	" . $this->truefalse($form->exportValue('parlor_meters')) . ",
	" . $this->truefalse($form->exportValue('parlor_gaskets')) . ",
	" . $this->truefalse($form->exportValue('parlor_filtercoil')) . ",
	" . $this->truefalse($form->exportValue('parlor_buckets')) . ",
        " . $this->truefalse($form->exportValue('parlor_sanitarytrap')) . ",    
	" . $this->truefalse($form->exportValue('parlor_hotwatertemp')) . "
	)";


            $res = $GLOBALS['pdo']->exec($query);

            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    private function truefalse($var) {
        if ($var == '1') {
            return 'true';
        } else {
            return 'false';
        }
    }

    public function record13Display($mode = 'default') {

      

        // Setup your database connection
        $sql = "	     
SELECT * FROM misc.cqm_record13 ORDER BY event_time DESC;
";

        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {    
               $rowOut[''] = ($row['event_time']);  //has to be at end.      
               $rowOut['Date'] = date("M d, Y", strtotime($row['event_time']));  //has to be at end.      
               $rowOut['Employee']=$row['userid'];
               $rowOut['Tank A Paddle']=$row['tanka_paddle'];   
               $rowOut['Tank A Dipstick']=$row['tanka_dipstick'];   
               $rowOut['Tank A Surface']=$row['tanka_surface'];   
               $rowOut['Tank A Outlet']=$row['tanka_outlet'];   
               $rowOut['Tank A Valve']=$row['tanka_valve'];   
               $rowOut['Tank A Gaskets']=$row['tanka_gaskets'];   
               $rowOut['Tank B Paddle']=$row['tankb_paddle'];   
               $rowOut['Tank B Dipstick']=$row['tankb_dipstick'];   
               $rowOut['Tank B Surface']=$row['tankb_surface'];   
               $rowOut['Tank B Outlet']=$row['tankb_outlet'];   
               $rowOut['Tank B Valve']=$row['tankb_valve'];   
               $rowOut['Tank B Gaskets']=$row['tankb_gaskets'];   
               $rowOut['Parlor Receiver Jar']=$row['parlor_receiverjar'];   
               $rowOut['Parlor Pipeline Inlets']=$row['parlor_pipelineinlets'];   
               $rowOut['Parlor Inflations']=$row['parlor_inflations'];   
               $rowOut['Parlor Milk Hoses']=$row['parlor_milkhoses'];   
               $rowOut['Parlor Claws']=$row['parlor_claws'];   
               $rowOut['Parlor Meters']=$row['parlor_meters'];   
               $rowOut['Parlor Gaskets']=$row['parlor_gaskets'];   
               $rowOut['Parlor Filter Coil']=$row['parlor_filtercoil'];
               $rowOut['Parlor Buckets']=$row['parlor_buckets'];
               $rowOut['Parlor Sanitary Trap']=$row['parlor_sanitarytrap'];
               $rowOut['Parlor Hot Water Temp']=$row['parlor_hotwatertemp'];
  
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Record 13', $dataArr)); 
         return implode($out); 
        
       
    }

//end function

    function record17Quickform() {

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForLastFourDays();
        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon();
        unset($timeArray[null]); //remove null value.
        //list of pre-canned comments
        $cannedComment = array(null => 'Please Select one...', 'Tank A improperly washed.' => 'Tank A improperly washed.', 'Tank B improperly washed.' => 'Tank B improperly washed.', 'Treated Milk in Tank A.' => 'Treated Milk in Tank A.', 'Treated Milk in Tank B.' => 'Treated Milk in Tank B.');

        $form = new HTML_QuickForm("record17Quickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Problem & Corrective Action');
        $form->addElement('hidden', 'pageid', $this->pageid);

        $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);

        $form->addElement('select', 'comment1', 'Canned Problem', $cannedComment);
        $form->addElement('textarea', 'comment2', 'OR Custom Problem', array('cols' => 40, 'rows' => 6));
        $form->addElement('textarea', 'corrective_action', 'Corrective Action', array('cols' => 40, 'rows' => 6));

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');



        $form->addRule('dateEvent', 'Event Time is required', 'required');
        $form->addRule('corrective_action', 'Corrective Action is required', 'required');
        //custom rule for teats (at least one needs to be selected)
        $form->registerRule('commentRequiredRule', 'function', 'commentRequired', 'MedicalCase');
        // $form->addRule(array('animalNumber','comment1','comment2'),'A canned or custom comment is required.','commentRequiredRule',array('animalNumber','comment1','comment2'));
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $tempArray = $form->getElementValue('dateEvent');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $pageid = $form->exportValue('pageid');
            $comment1 = trim($form->exportValue('comment1'));
            $comment2 = trim($form->exportValue('comment2'));
            $corrective_action = trim($form->exportValue('corrective_action'));

            /*
              print("bovine_id:$bovine_id<br>");
              print("comment1:$comment1<br>");
              print("comment2:$comment2<br>");
              print("pageid:$pageid<br>");
             */

            //concatenate comments if they are both entered.
            if (strlen($comment1) == 0) {
                $comment = $comment2;
            } else {
                $comment = $comment1 . ' ' . $comment2;
            }

            //do postgres escape.
            $comment_esc = pg_escape_string($comment);
            $corrective_action_esc = pg_escape_string($corrective_action);

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO misc.cqm_record17 (event_time,problem,corrective_action,userid) VALUES ('$event_time','$comment_esc','$corrective_action_esc','{$GLOBALS['auth']->getUsername()}')";


            $res = $GLOBALS['pdo']->exec($query);

            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    public function record17Display($mode = 'default') {

    
        $sql = "	     
SELECT * FROM misc.cqm_record17 ORDER BY event_time DESC;
";
       $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {    
              $rowOut[''] = ( strtotime($row['event_time']));  //has to be at end.      
               $rowOut['Date'] = date("M d, Y ", strtotime($row['event_time']));  //has to be at end.      
               $rowOut['Problem']=$row['problem'];
               $rowOut['Corrective Action']=$row['corrective_action'];   
               $rowOut['Employee'] = $row['userid'];                
          
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
       $out[]=(new BootStrap)->infoAlertBasic('TTR alarms due to no milk in tank because too much time has elapsed can be safely ignored, but everything else must be recorded and rectified.');
         $out[]=( (new JQueryDataTable)->startBasicArrayNoSort('Corrective Action', $dataArr)); 
         return implode($out);  
        
       
    }

    public function record9Display($mode = 'default') {

       

        $sql = "	     
SELECT * FROM misc.cqm_record9 ORDER BY event_time DESC;
";
       $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {    
               $rowOut['Date'] = date("M d, Y ", strtotime($row['event_time']));  //has to be at end.      
               $rowOut['Name']=$row['name'];
               $rowOut['Approved for Dairy']=$row['approved_for_dairy'];   
               $rowOut['Stored According to Label']=$row['stored_according_label'];   
               $rowOut['DIN #']=$row['DIN'];   
               $rowOut['Employee'] = $row['userid'];                
          
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr)); 
         return implode($out);  
        
    }

//end function
}

//end class
?>