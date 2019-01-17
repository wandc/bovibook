<?php 


//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if(class_exists('DataGridLR_Ext') != true) {
  //extends datgrid class with specific logic to colour the line.
    //use print($dg1->render('datagrid',true,'00FF00')); to set colour
  class DataGridLR_Ext extends DataGridLR {
                     function colourLineCriteria($row) {
                     if ($row['completed']=='t') {  return true;}
                     else  {  return false;}
                     }
                }
}
  
class ClassificationDay extends TabPage {

	

	public function defaultDisplay() {
            
             //setup for tabs.
        $tabArray[1]['name'] = 'List';
        $tabArray[2]['name'] = 'Cow Chooser';
        
        $this->renderTabs($tabArray);
            
            
		
	}

	protected function tab1() {
      print("<style>form.quickformtableless input {width: 200px;}</style>"); //tp fix stupid quickforms bug.
		$this->displayClassificationList();
             
    }

    protected function tab2() {
     
        print("<h3>NOTE: Do not use this form during classification or results will be lost.</h3>");
        $a=new QF2_Classification((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('qf2_Classification','Select Animals to Add to Classification List'));  
        print($a->formJS());
	print($this-> deleteButton());
        
          print("<h3>List to Help Choose Who To Classify</h3>".$this->pageid);
        $this->displayCowsToClassifyList();
    }
        
    
    
    
    public function toClassifyList() {
        $sql = "SELECT bovinecurr.id as bovine_id,local_number,bovinecurr.full_name,location_name,bovinecurr.birth_date,bovinemanagement.round_to_nearest_date (fresh_date) as fresh_date,score,short_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim 
                FROM
                bovinemanagement.bovinecurr 
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
WHERE bovinecurr.location_id in (40,41,39,42)
ORDER BY location_name,local_number ASC";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        
   
        
        //change presentation
        foreach ($results as &$row) {
          
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
            $row['current_location'] = strtotime($row['location_name']);
           
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    
      function displayCowsToClassifyList() {
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
EX;
    print($dt->start($this->pageid,'toClassifyList',null,$columnsStr,null,false,'"paging": false,'));     
}     

    
    
    
    
    
        
	public function displayClassificationList() {
		
include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');

		// Setup your database connection
		$sql="
		SELECT bovinecurr.id as bovine_id,local_number,bovinecurr.full_name,location_name,bovinecurr.birth_date,classificationreport_temp.completed,bovinemanagement.round_to_nearest_date (fresh_date) as fresh_date,score,short_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim 
                FROM system.classificationreport_temp
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id=classificationreport_temp.bovine_id
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number

WHERE classificationreport_temp.checked=true
ORDER BY location_name,local_number
";
             
                $dg1 = new DataGridLR_Ext($sql, 1000);
               
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''),null,array('DataGridHelper','printFormattedBovineNameAndNumber()'))); 
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Full Name', 'full_name', '', array('' => ''), null, null));
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Location', 'location_name', '', array('' => ''), null, null));
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('DIM   ', 'dim', '', array('' => ''), null, null));
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Fresh Date', 'fresh_date', 'fresh_date', array('' => ''),null,array('ClassificationDay','printDateInClassifierFormat()')));
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Classification Score', 'score', '', array('' => ''), null, null));
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Birth Date', 'birth_date', 'birth_date', array('' => ''),null,array('ClassificationDay','printDateInClassifierFormat()')));
               $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('ClassificationDay', 'printActionButtons2()')));      
                    
                        
                       
                print($dg1->render('datagrid',true,$GLOBALS['config']['COLOUR']['okay']));
                $dg1->pager(); 
        }     
	
        
        
        
          /** **********************************************************
	   * see whether to print completed button for classification
	   *********************************************************** */
   function printActionButtons2($params)
	  {
       extract($params);
	
	      //only show complete button for those that are not completed and will occur in the next 10 hours OR PAST.      
              if ($record['completed']=='1') {
                  $returnButtonsAsString=self::completedClassificationQuickForm($record['bovine_id'],'undo'); //show undo button
              }
              else {
                  $returnButtonsAsString=self::completedClassificationQuickForm($record['bovine_id'],'completed'); //show complete button
              }

	    
	    //print button
	    //$returnButtonsAsString=$returnButtonsAsString;
	    
	    return $returnButtonsAsString;
	  }
          
	  
	   /** **********************************************************
	   * completed button for classification
	   *********************************************************** */
	   function completedClassificationQuickForm($bovine_id,$button_type)
	  {
	    
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
                echo "Failed: " . $e->getMessage();
            }
	      
	      
	      header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
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
        $form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'btnDelete', 'onclick' => "return confirm('Are you sure you want to delete all classification data')")); //call javascript to confirm submit.
        // Try to validate a form
        if ($form->validate()) {

            //get values
         
                        // delete everything in the temp storage table.
		        $res=$GLOBALS['pdo']->exec("TRUNCATE system.classificationreport_temp"); //erase contents of temp db table.	
                        

            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }
}//end class

Class QF2_Classification extends Quickform2Helper {

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
$aniNumber[$row[0]] = $row[1] ." - " . Misc::femaleShortName($row[2]);
}

        

        ////
        //$fieldset = $form->addFieldset('header')->setLabel('Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($mode == 'group') {
            $form->addSelect('aniamlSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($aniNumber);
        } else {
            $form->addElement('hidden', 'aniamlSelected')->setValue($bovine_id);
        }

        //read animals that are currently marked as slected and add them as defaults to the form
        $query="SELECT bovine_id FROM system.classificationreport_temp where checked=true";
        $statement = $GLOBALS['pdo']->prepare($query);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_COLUMN); //fetch column

        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
    'aniamlSelected' =>  $results
)));
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
            try {$res = $GLOBALS['pdo']->beginTransaction();

          
        $res=$GLOBALS['pdo']->exec("TRUNCATE system.classificationreport_temp"); //erase contents of temp db table.	
      
        foreach ($value['aniamlSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.classificationreport_temp (bovine_id,checked) VALUES ($v,true) ON CONFLICT (bovine_id) DO  UPDATE SET checked =true");
              $res = $GLOBALS['pdo']->exec($query);
            }

               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            
            }
            // Close the transaction
       
       
    }

}//end class   
?>