<?php 


require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/croppingHelper.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/cropping/field.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/cropping/fieldQuery.inc');
;
require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
class Manure  extends TabPage {

    private $field_id;
    private $page_id;

    
     public function defaultDisplay() {
        $tabArray[1]['name'] = 'Manure Field';
        $tabArray[2]['name'] = 'Log';
   
     
        $this->renderTabs($tabArray);
    }
    
    
     public function tab1() {

         self::addNewManureLoadsEventQuickform();
         self::displayJSPortionofajaxReturnFieldPicture();

        
     }
      public function tab2() {
      
         self::displayAllManureEventsLast1000(null);
     }
     
     
     
     // public function displayCurrentFieldMap($field_id) {
     /*
      * returns a pic of field_id, ie changes with select choice.
      */
     public function ajaxReturnFieldPicture($request) {
         if (!empty($request['item'])) {
       if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {  

         return FieldQuery::displayCurrentFieldMap($request['item'],16); 
         }}
     }
     
     //returns a datable of latest results for selcted field.
     public function ajaxReturnFieldTable($request) {
       if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {  

        return    self::displayAllManureEventsLast1000($request['item']); //show events only for this field.
           
     }
    
     }
     
    private function findLastManureEventFieldForUser() {
             $sql = "Select field_id FROM cropping.manure_Event WHERE userid='{$GLOBALS['auth']->getUsername()}' ORDER BY update_time DESC LIMIT 1";
                  $res = $GLOBALS ['pdo']->query($sql);
             $row = $res->fetch(PDO::FETCH_ASSOC);
             if (!empty($row['field_id'])) {
                 return $row['field_id'];
             }
      else {
          return null;
      }
    }
     
     
     public function displayJSPortionofajaxReturnFieldPicture() {
        /*
         * Dynamically chnage field picture with select change
         */ //style="height:100px;width:500px;
           print('<div id="drawFieldGoogleStaticPictureDiv" >  </div>'); //where picture gets drawn, on select change     
            print('<div id="drawFieldTable"> Picture </div>'); //drawFieldTable, on select change   
         
?>
                <script type="text/javascript" language="javascript"> 
   
         function drawFieldGoogleStaticPicture(id) {
        $.ajax({
         type: "GET",
         url: "<?php  echo($GLOBALS['config']['HTTP']['HTML_API']); ?>101/ajaxReturnFieldPicture/"+id,
         success: function(result) {
             $("#drawFieldGoogleStaticPictureDiv").html(result);
         }
        });
        
        $.ajax({
         type: "GET",
         url: "<?php  echo($GLOBALS['config']['HTTP']['HTML_API']); ?>101/ajaxReturnFieldTable/"+id,
         success: function(result) {
             $("#drawFieldTable").html(result);
         }
        });
        
      
        };
       
        //run above function onload too.
    $( document ).ready(function() {
    drawFieldGoogleStaticPicture(<?php echo(self::findLastManureEventFieldForUser());?>);
      
    });
       
        </script>
<?php   
     }
     
     
       /**
     * Manure by number of loads
     */
    function addNewManureLoadsEventQuickform($type = 'default', $field_id = null) {

        
       
        
        $fieldArray = CroppingHelper::allActiveFieldsArray();

        // select active people in manure group to say who spread.
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='manure' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $manureUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $manureUsers [$row ['userid']] = $row ['userid'];
        }



        $manureAmount = array(null => 'Please Select one...');
        for ($i = 1; $i <= 100; $i++) {
            $manureAmount[$i] = "$i loads";
        }

        $spreaderType = array(null => 'Please Select one...', '1' => 'Nuhn 4000 - liquid', '2' => 'Gehl Scavenger - semi-solid');

        $form = new HTML_QuickForm("addNewManureEventQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); 
       
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'addNewManureEventQuickform', 'Add a New Manure Load Application Event');
        if ($field_id == null) {
            $form->addElement('select', 'field', 'Field', $fieldArray, array('onchange' => 'drawFieldGoogleStaticPicture(this.value);'));
        } else {
            $form->addElement('hidden', 'field', $field_id);
        }
           $form->addElement('select', 'date_start', 'Date of Report', $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon());
                    $form->addElement('select', 'spreader_type', 'Spreader Type', $spreaderType);
        $form->addElement('select', 'manure_amount', 'Manure Amount (loads)', $manureAmount);
         $form->addElement('checkbox', 'all_covered', 'Field All covered?');
       //only allow admin to do a custom field size
        if ((in_array('owner', $GLOBALS ['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS ['auth']->getAuthData('groups')) == TRUE)) {
            $form->addElement('select', 'spreader_userid', 'Who Spread it', $manureUsers);
        }
        else {
             $form->addElement('hidden', 'spreader_userid', "{$GLOBALS['auth']->getUsername()}");
        }
        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));



       

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('date_start' => 'now')); // set default to
        $form->setDefaults(array('all_covered' =>  true));
        // today.
        
        $form->setDefaults(array('field' => self::findLastManureEventFieldForUser())); // set
        $form->setDefaults(array('spreader_userid' => "{$GLOBALS['auth']->getUsername()}")); // set
        $form->setDefaults(array('spreader_type' => "1")); // set to nuhn as default
        $form->setDefaults(array('use_current_border_polygon' => '1')); // set
        // default to
        // today.
        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Date Start is required', 'required');
        $form->addRule('manure_amount', 'Manure Amount is required', 'required');
        $form->addRule('spreader_userid', 'Who spread the manure is required', 'required');
        $form->addRule('spreader_type', 'Manure Spreader type is required', 'required');

        //custom rule for if over 9 hours of work they need an explanation.
        $form->registerRule('checkExplanationForFieldNotCovered', 'function', 'checkExplanationForFieldNotCovered');
        $form->addRule(array('all_covered','comment'), 'ERROR: No Explanation for overtime worked.', 'checkExplanationForFieldNotCovered', array('all_covered','comment'));

function checkExplanationForFieldNotCovered($junk, $valueArr, $elementNameArr) {
            //generic loop to convert $form elements we want into a usuable array
            //I really hope this isn't how quickforms was designed.
            $count = 0;
            foreach ($elementNameArr as $elementName) {
                $formData[$elementName] = $valueArr[$count];
                $count++;
            }

            
            //when workTime is ovetime and textbox is blank...
            if ((trim($formData['comment']) == '') AND ($formData['all_covered'] ==0)) {
                $retValue = 0; //error
            } else {
                $retValue = 1; //ok
            }

            return $retValue;
        }
        
        // Try to validate a form
        if ($form->validate()) {

            $field_id = $form->exportValue('field');

                 $t1Array = $form->getElementValue('date_start');           
            $event_time = date('Y-m-d H:i:sO', $t1Array [0]);
        
            $manure_amount_loads = $form->exportValue('manure_amount');
            $all_covered=$form->exportValue('all_covered')  ? 'true' : 'false';

           
            $spreader_userid = $form->exportValue('spreader_userid');
            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = null;
            }

            switch ($form->exportValue('spreader_type')) {
                case 1 :
                    $manureType = 'liquid';
                    $tankerCapacityLiters = 15141.6471; //4000 US gallons in liters

                    break;
                case 2 :
                    $manureType = 'semi-solid';
                    $tankerCapacityLiters = 5110.30591; //1350 US gallons (in liters) might not be right, but close.
                    break;
            }
            //we know oads and tanker capacity, find out how manyliters spread.
            $manure_amount_overall = $manure_amount_loads * $tankerCapacityLiters;

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();
                
                //get the datum id as the current field datum id
                 $sql2 = "SELECT border_datum_id FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1";
                    $res2 = $GLOBALS['pdo']->query($sql2);
                    $row2 = $res2->fetch(PDO::FETCH_ASSOC);
                    $last_datum_id = ($row2['border_datum_id']);
                
         

                

                //now find out the area of the polygon manure was spread on, so we can calculate amount per hectare.
                $sql = "
 	SELECT gis.ST_Area(gis.ST_Transform(geom, 2036))/10000 as area FROM cropping.datum WHERE id=$last_datum_id LIMIT 1";
                $res = $GLOBALS['pdo']->query($sql);
                if ($res->rowCount() > 0) {
                    $row = $res->fetch(PDO::FETCH_ASSOC);
                    $fieldArea = ($row['area']);
                    $manure_amount = $manure_amount_overall / $fieldArea;
                } else {
                    throw new Exception("ERROR: no field polygon selected to calculate area for manure amount.");
                }


                
                // do an insert into cropping field and datum of data.
                $query = "INSERT INTO cropping.manure_event (field_id,datum_id,event_time,manure_amount,userid,spreader_userid,comment,manure_type,all_covered) VALUES ($field_id,$last_datum_id,'$event_time',$manure_amount,'{$GLOBALS['auth']->getUsername()}','$spreader_userid','$comment','$manureType',$all_covered)";
                $res = $GLOBALS ['pdo']->exec($query);

           
                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction

            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&field_id=$field_id");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

     
   
    function printActionButtons2($manureEventID,$callBackURL=null)
	{
		
		$currentUserid=$GLOBALS['auth']->getUsername();

		//find the last entry for the current user and use that as a basais to display the delete button or not.
		//We only want the delete button shown once for the latest entry.
		$res = $GLOBALS['pdo']->query("SELECT id FROM cropping.manure_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");
		
		$row = $res->fetch(PDO::FETCH_ASSOC);
                $deleteButtonAsString=null;
		if ($manureEventID==$row['id'])
		{
			$deleteButtonAsString=self::deleteQuickForm2($manureEventID,$callBackURL); //show delete button, with estrus table id as index for delete.
		}

		return $deleteButtonAsString;
	}
         function deleteQuickForm2($manureEventID,$callBackURL)
	{
		
		$form = new HTML_QuickForm("manureEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtablelessButton'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
   
		$form->addElement('hidden','pageid',$_REQUEST['pageid']);
		$form->addElement('hidden','manureEventID',$manureEventID);
		$form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete this event?')")); //call javascript to confirm delete.
                

		// Try to validate a form
		if ($form->validate()) {

			//get values
			$pageid=$form->exportValue('pageid');
			$manureEventID=$form->getElementValue('manureEventID');

			//print("<br>DELETING $estrusID !!!!<br>");
			$res = $GLOBALS['pdo']->query("DELETE FROM cropping.manure_event where id='$manureEventID'");
			

			//print("DELETING succesful");
			header($callBackURL);
			exit();

		}
		$form->accept($renderer);
                return $renderer->toHtml();
	
	}
    
      public function AllManureEventsLast1000($request) {
         $sql = "SELECT manure_event.*,manure_event.id as manure_event_id,field.*,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.manure_event 
        LEFT JOIN cropping.field ON field.id=manure_event.field_id
        LEFT JOIN cropping.datum ON manure_event.datum_id = datum.id
        ORDER BY event_time DESC,alpha_numeric_id limit 1000";
	
         
   
         //when specific field id is sent.
         if (!empty($request['item'])) {
           if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {  
               
            $sql = "SELECT manure_event.*,manure_event.id as manure_event_id,field.*,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.manure_event 
        LEFT JOIN cropping.field ON field.id=manure_event.field_id
        LEFT JOIN cropping.datum ON manure_event.datum_id = datum.id
        WHERE field.id={$request['item']} 
        ORDER BY event_time DESC,alpha_numeric_id limit 1000";    
               
         }}

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
                 $row['event_time_raw']=strtotime($row['event_time']);
                  $row['manure_amount']=round($row['manure_amount']);
           $row['alpha_numeric_id'] ="<a href=\"". $GLOBALS['MiscObj']->hrefToFieldPage($row['field_id'])."\"> <b>{$row['alpha_numeric_id']}</b> - ". $row['common_name'] ."</a>";     
           $row['event_time']= date("l, M d Y",strtotime($row['event_time']));  //has to be at end.
           $row['action'] = $this->printActionButtons2($row['manure_event_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$row['field_id']}");
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
      function displayAllManureEventsLast1000($field_id=null) {
          
          if (!empty($field_id)) {
            print ("<h3>Latest Field Manure Events</h3>");
          }
          else{
                 print ("<h3>All Manure Events</h3>");
          }
            
            
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
                 
        {
                data: "alpha_numeric_id",
                title: "Field"
            },
        {
                data: "manure_amount",
                title: "Amount (l)"
            },
         {
                data: "comment",
                title: "comment"
            },
        {
                data: "area",
                title: "Area (ha)"
            },
        
            {
                data: "event_time",
                title: "Event Time"
            },
            {
                data: "spreader_userid",
                title: "Employee"
            },  
                {
                data: "all_covered",
                title: "all_covered"
            },  
            {
                data: "action",
                title: "Action"
            } 
EX;
    print($dt->start(101,'AllManureEventsLast1000',$field_id,$columnsStr));     
}     
    
    
    

    /**
     * converts liters/heactare to a descriptive word
     */
    function printManureAmountString($params) {
        extract($params);
        switch ($record ['manure_amount']) {
            case 20000 :
                $str = 'light';
                break;
            case 40000 :
                $str = 'medium';
                break;
            case 60000 :
                $str = 'heavy';
                break;
            default :
                $str = $record ['manure_amount'];
        }

        return $str;
    }


}//end class 
