<?php 


  
class ClassificationDay extends TabPage {

	

	public function defaultDisplay() {
            
             //setup for tabs.
        $tabArray[1]['name'] = 'Heifer Group';
        $tabArray[2]['name'] = 'High Group';
        $tabArray[3]['name'] = 'Other';
        $tabArray[4]['name'] = 'Cow Chooser';
        
        $this->renderTabs($tabArray);
            
            
		
	}
        
    protected function tab1() {

		print($this->displayClassificationList(42));
             
    }
    
    protected function tab2() {

		print($this->displayClassificationList(40));
             
    }
    
    protected function tab3() {

		print($this->displayClassificationList(-1));
             
    }
    

    protected function tab4() {
        $formStr='';
        $a=new QF2_Classification((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
        $formStr.=($a->renderBox('qf2_Classification','Select Animals to Add to Classification List'));  
       
	$formStr.=($this-> deleteButton());
        //   
        $warning=(new BootStrap)->warningCallout('NOTE: Do not use this form during classification or results will be lost.');
        
$html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>    
            <p>$warning</p>
            $formStr
        </div>
        <div class='col-sm-8 col-sm-pull-4'>
        <h3></h3>
            {$this->displayCowsToClassifyList()}
        </div> 
    </div>    
HTML;

          print($html);
       
    }
        
    
    
    
    public function displayCowsToClassifyList() {
$sql = <<<SQL
SELECT bovinecurr.id as bovine_id,local_number,bovinecurr.full_name,location_name,bovinecurr.birth_date,
    bovinemanagement.round_to_nearest_date (fresh_date) as fresh_date,score,short_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,
        (SELECT checked FROM system.classificationreport_temp WHERE bovine_id=bovinecurr.id)
                FROM
                bovinemanagement.bovinecurr 
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
WHERE bovinecurr.location_id in (40,41,39,42)
ORDER BY location_name,local_number ASC
SQL;
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        
        //change presentation
        foreach ($results as &$row) {

              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
            $rowOut['Current Location'] = ($row['location_name']);
            
            $rowOut['Fresh Date'] = JQueryDataTable::formattedDate($row['fresh_date']);
            $rowOut['DIM'] = ($row['dim']);
            $rowOut['Classification Score'] = ($row['score']);
            $rowOut['Birth Date'] =JQueryDataTable::formattedDate($row['birth_date']);
            $rowOut['colour_row'] = $row['checked'] ? 1:0;
            
            
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayColour('List to Help Choose Who To Classify', $dataArr, 'paging: false,order:true','highlight_attention'));
        return implode($out);
  
        
    }
    
      
    
    /*
     * used when classifying to check off what is done.
     */
    public function CCdisplayClassificationList($location) {
         $dt=new JQueryDataTable();
$columnsStr= <<<EX
           
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
                data: "fresh_date",
                title: "Fresh Date",
                type: "date",
            },
     
            {
                data: "dim",
                title: "DIM"
            },
        {
                data: "score",
                title: "Classificatuon Score"
            },{
                data: "birth_date",
                title: "Birth Date",
                type: "date",
            },
        {
                data: "action",
                title: "Action"
            }
EX;
    return($dt->start('Title CHANGE ME', $this->pageid,'classificationList',$location,$columnsStr,'highlight_okay',false,'searching: false,paging: false,ordering: false,info: false,'));     
    }
    
        
	public function displayClassificationList($groupShortName) {
         
            /* hard coded groups. no real way to avoid this without getting fancy. */
            switch ($groupShortName) {
                case '42':
                  $location_idTxt=' location_id=42';

                    break;
                case '40':
                     $location_idTxt=' location_id=40';

                    break;
                default: //everything else
                     $location_idTxt=" NOT location_id = ANY ('{42,40}'::int[])";
                    
                    break;
            }

$sql = <<<SQL
SELECT bovinecurr.id as bovine_id,local_number,bovinecurr.full_name,location_name,bovinecurr.birth_date,classificationreport_temp.completed,
    bovinemanagement.round_to_nearest_date (fresh_date) as fresh_date,score,short_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim 
                FROM system.classificationreport_temp
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id=classificationreport_temp.bovine_id
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
WHERE classificationreport_temp.checked=true AND $location_idTxt
ORDER BY completed ,local_number
SQL;

         
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']);   
            $rowOut['Fresh'] = JQueryDataTable::formattedDate($row['fresh_date']);
            $rowOut['DIM'] = ($row['dim']);
            $rowOut['Score'] = ($row['score']);
            $rowOut['Birth Date'] =JQueryDataTable::formattedDate($row['birth_date']);
            $rowOut[' '] = $this->printActionButtons2($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
                
            $rowOut['colour_row'] = $row['completed'] ? 1:0;
            
            
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayColour('', $dataArr, 'paging: false,order:true','highlight_attention'));
        return implode($out);   
        }
        
        
        
          /** **********************************************************
	   * see whether to print completed button for classification
	   *********************************************************** */
   function printActionButtons2($record,$callBackURL)
	  {

	
	      //only show complete button for those that are not completed and will occur in the next 10 hours OR PAST.      
              if ($record['completed']=='1') {
                  $returnButtonsAsString=self::completedClassificationQuickForm($record['bovine_id'],'undo',$callBackURL); //show undo button
              }
              else {
                  $returnButtonsAsString=self::completedClassificationQuickForm($record['bovine_id'],'completed',$callBackURL); //show complete button
              }

	    
	    //print button
	    //$returnButtonsAsString=$returnButtonsAsString;
	    
	    return $returnButtonsAsString;
	  }
          
	  
	   /** **********************************************************
	   * completed button for classification
	   *********************************************************** */
	   function completedClassificationQuickForm($bovine_id,$button_type,$callBackURL)
	  {
	    //$uuid= uniqid();
	    $form = new HTML_QuickForm("completedButtonQuickForm",  'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
	     $form->addElement('hidden','bovine_id',$bovine_id);
             if ($button_type =='completed') {
               $form->addElement('hidden','completed','true');
                 $form->addElement('submit', 'btnCompleted','Completed');
             }
             else {
                 $form->addElement('hidden','completed','false');
                 $form->addElement('submit', 'btnCompleted','Undo');
             }
                 
               
	  
	    
	    //FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY USER.
	    //DB hack works though, views need to support deletes before this can be fixed.
	    
	    // Try to validate a form
	    if ($form->validate()) {
	  
	      
	      $bovine_id=$form->getElementValue('bovine_id'); 
              $completed=$form->getElementValue('completed'); 
	     
	      
	      // Open a transaction
	      try {$res = $GLOBALS['pdo']->beginTransaction();
	      
              if ($completed =='true') {
	     //update event to say it is completed.
	      $res = $GLOBALS['pdo']->query("Update system.classificationreport_temp SET completed='true' WHERE bovine_id='$bovine_id'");
              }
              else {
                  $res = $GLOBALS['pdo']->query("Update system.classificationreport_temp SET completed='false' WHERE bovine_id='$bovine_id'");
              } 
              $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
	      
	      
	      header($callBackURL);
	      exit();
	      
	      }
	      return $form->toHtml(); //pass on as html string to display later in the table
  
	      }    
        

        
   function printDateInClassifierFormat($params)
	{
		extract($params);
		//use the fieldname to be generic.
		return date("d-m-Y",strtotime($record[$fieldName]));
	}

 
	
	
	

	
 function deleteButton() {
        $form = new HTML_QuickForm("classificatioanDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);      
        $form->addElement('submit', 'btnDelete', 'Delete', array('class'=>'btn btn-warning','id' => 'btnDelete', 'onclick' => "return confirm('Are you sure you want to delete all classification data')")); //call javascript to confirm submit.
        // Try to validate a form
        if ($form->validate()) {

            //get values
         
                        // delete everything in the temp storage table.
		        $res=$GLOBALS['pdo']->exec("TRUNCATE system.classificationreport_temp"); //erase contents of temp db table.	
                        

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }
}//end class

Class QF2_Classification extends QF2_Base {

    function definition($form) {


        $mode = 'group';

       
        
        //get list of fresh cows
        $sql="SELECT id,local_number,bovinecurr.full_name,location_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,checked ,score FROM bovinemanagement.bovinecurr 
LEFT JOIN system.classificationreport_temp ON classificationreport_temp.bovine_id=bovinecurr.id
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
WHERE fresh_date is not null ORDER BY location_name,local_number";
        
        $res = $GLOBALS['pdo']->query($sql);

while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
$aniNumber[$row[0]] = $row[1] ." - " . $GLOBALS['MiscObj']->femaleShortName($row[2]);
}

        

        ////
        //$fieldset = $form->addFieldset('header')->setLabel('Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form=(new Quickform2_GroupSelect())->definitionAnimalGroup($form,$aniNumber);
        }
        else {
            $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form,$aniNumber);
        }

        //read animals that are currently marked as slected and add them as defaults to the form
        $query="SELECT bovine_id FROM system.classificationreport_temp where checked=true";
        $statement = $GLOBALS['pdo']->prepare($query);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_COLUMN); //fetch column

        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
    'animalSelected' =>  $results
)));
  return $form; //required   
    }

   

    
 
    function process($value) {
        
      
        
        // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

          
        $res=$GLOBALS['pdo']->exec("TRUNCATE system.classificationreport_temp"); //erase contents of temp db table.	
      
        foreach ($value['animalSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.classificationreport_temp (bovine_id,checked) VALUES ($v,true) ON CONFLICT (bovine_id) DO  UPDATE SET checked =true");
              $res = $GLOBALS['pdo']->exec($query);
            }

               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            
            }
            // Close the transaction
       
       
    }

}//end class   
?>