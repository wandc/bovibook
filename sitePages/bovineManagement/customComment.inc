<?php 
/** so far this is only used in bovinequery */
class CustomComment extends BasePage{
  
  
  public function defaultDisplay() {
  $this->newReproCommentQuickForm();
  }
  
    public function summaryDisplay() {
    //nothing
  }

    
  
  
   // displays info on potential offspring from cdn website.
    function displayCustomComment($mode='group',$bovine_id=null,$type=null) {
        
          
        if ($mode != 'group') {
            $sql = "SELECT id,type,bovine_id,comment,userid,event_time, date_trunc('days',now()-event_time) as days_ago FROM bovinemanagement.comment_custom WHERE type='$type' AND comment_custom.bovine_id=$bovine_id ";
        } else {
            $sql = "SELECT id,type,bovine_id,comment,userid,event_time, date_trunc('days',now()-event_time) as days_ago FROM bovinemanagement.comment_custom WHERE type='$type' ";
        }

        
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
          if ($mode=='group') {     
          $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
          }
          $rowOut['Time']=JQueryDataTable::formattedDate($row['event_time']);        
          $rowOut['Comment']=  $row['comment'];
          $rowOut['Employee']=  $row['userid'];
          $rowOut['Action']=  self::printActionButtons($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
          $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
        
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr)); 
         print implode($out); 
   
    }

    function printActionButtons($record,$callBackURL) {
      
        
        $currentUserid = $GLOBALS['auth']->getUsername();
        $type = $record['type'];

        //admin user
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $sql = "SELECT id FROM bovinemanagement.comment_custom WHERE update_time=(SELECT max(update_time) FROM bovinemanagement.comment_custom WHERE type='$type') AND type='$type'";
        } else { //normnal user
            $sql = "SELECT id FROM bovinemanagement.comment_custom WHERE update_time=(SELECT max(update_time) FROM bovinemanagement.comment_custom WHERE userid='$currentUserid' AND type='$type') AND userid='$currentUserid' AND type='$type'";
        }

        $res = $GLOBALS['pdo']->query($sql);
       
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (($record['id'] == $row['id'])) {
                  $a = new QF2_GenericDeleteButton((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'bovinemanagement.comment_custom'));
                $returnButtonsAsString = ($a->renderDelete('customcomment' . $row['id']));
            }
        }

        //print button
        return $returnButtonsAsString;
    }

    /* ******************************** */
    /* General Custom Comment           */
    /* ******************************** */
    function newCustomCommentQuickForm($mode='group',$bovine_id=null,$type=null)
    {
        
        //for different types of comments, keep adding.
        switch ($type) {
    case 'reproductive':
        $cannedComment=array(null=>'Please Select one...','Cloudy Vaginal Discharge.'=>'Cloudy Vaginal Discharge.','Thick Bloody Vaginal Disharge.'=>'Thick Bloody Vaginal Disharge.','Aborted.'=>'Aborted.' );
        $header='Repro Comment';  
        break;
    
     case 'trait':
        $cannedComment=null;
        $header='Trait Comment';
        break;
    
    case 'genotest':
        $cannedComment=null;
        $header='Genomic Test';
        break;
    
    default:
        throw new Exception("Custom Comment has no type. Aborting.<br />\n\r");
    }
        
        
        
      
       //grab a list of cows.
		$res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovinecurr ORDER BY local_number");
		
		$aniNumber[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch())) {
			//parse for last word of name and then make an array for the select list.
                    $tt=preg_split("/[\s,]+/", $row[1]);
			$aniNumber[$row[0]] = $row[2] ." - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
		}
      
     $timeArray=$GLOBALS['MiscObj']->createHourlyDatesForLastDay();
     unset($timeArray[null]); //remove null value.
      
      //list of pre-canned comments
      

      //the action=request_uri is the key for this to work with callbacks and tabs. 
      $form = new HTML_QuickForm("newCustomCommentQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();   
     
      $form->addElement('header', 'newCustomCommentQuickFormHdr',$header);
      
      //code to turn off fields if we are accessing this from the bovine query page.
      if ($mode=='group') {
      $form->addElement('select', 'bovine_id', 'Animal Number',$aniNumber);
      }
      else {
      $form->addElement('hidden','bovine_id',$bovine_id);
      }
      $form->addElement('hidden','pageid',$_REQUEST['pageid']);
      
      $form->addElement('select', 'dateEvent', 'Time of Check', $timeArray);
      
      
      if (empty($cannedComment) ==false) {
      $form->addElement('select', 'comment1', 'Canned Comment', $cannedComment);
      }
      else {
          $form->addElement('hidden','comment1',''); 
      }
      
      $form->addElement('textarea',   'comment2', 'OR Custom Comment', array('cols'=>40, 'rows'=>2));
      
      //submit buttons
      $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
      $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
      $form->addGroup($submitGroup, 'submitGroup');
      
      //defaults
        if ($mode=='group') {
        $form-> setDefaults(array('bovine_id' => 0)); //set default to no variety.
	}
	
      //rules
      if ($mode=='group') {
      $form->addRule('bovine_id', 'Animal number is required', 'required');
       }
       
      $form->addRule('dateEvent', 'Event Time is required', 'required');
      //custom rule for teats (at least one needs to be selected)
      $form->registerRule('commentRequiredRule','function','commentRequired','CustomComment');
      $form->addRule(array('bovine_id','comment1','comment2'),'A canned or custom comment is required.','commentRequiredRule',array('bovine_id','comment1','comment2'));
	
      
      
      // Try to validate a form
      if ($form->validate()) {
	
	//get values
	$bovine_id=$form->exportValue('bovine_id');
	$tempArray=$form->getElementValue('dateEvent');
	$event_time=date('Y-m-d H:i:sO',$tempArray[0]);
	$pageid=$form->exportValue('pageid');
	$comment1=trim($form->exportValue('comment1'));
	$comment2=trim($form->exportValue('comment2'));
	
	print("comment1:$comment1<br>");
	print("comment2:$comment2<br>");
	
	
	//concatenate comments if they are both entered.
	if (strlen($comment1) == 0) {
	$comment=$comment2;
	}
	else {
	$comment=$comment1.' '.$comment2;
	}  
	
	$comment=pg_escape_string($comment);
	
	
	//don't put id value, uses next one in DB.
	$query = "INSERT INTO bovinemanagement.comment_custom (bovine_id,event_time,comment,userid,type) VALUES ($bovine_id,'$event_time','$comment','{$GLOBALS['auth']->getUsername()}','$type')";
	
	
	$res=$GLOBALS['pdo']->exec($query);
	
	 //forward to same page, ob_start() is key to making this work!
         header("Location: https://" . $_SERVER['HTTP_HOST'] . $_SERVER['PHP_SELF'] . "?pageid={$_REQUEST['pageid']}&bovine_id=$bovine_id");
         exit();
	
     } //end validation
    
     $form->accept($renderer);
        echo $renderer->toHtml();
  }
  
      //check at least one comment was filled in.
      function commentRequired($valueArr,$elementNameArr) {
	//generic loop to convert $form elements we want into a usuable array
	//I really hope this isn't how quickforms was designed.
	$count=0;
	foreach ($elementNameArr as $elementName) {
	  $formData[$elementName]=$valueArr[$count];
	  $count++;
	}
	
	//only return false when no comment was entered or selected.
	if ((strlen(trim($formData['comment1']))!=0) || (strlen(trim($formData['comment2']))!=0)) 
	{
	  return true;
	}
	else 
	{
	  return false;  
	}
	
      }
  
  } //end class
?>