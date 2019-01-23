<?php 
//Lists straws of semen in our inventory



class EstrusEmbryo extends TabPage{



	 public function defaultDisplay() { 
	 $tabArray[1]['name']='Implant';
         $tabArray[2]['name']='Inventory';
         $this->renderTabs($tabArray);
         }
         
         function tab1() {
             $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>    
             {$this->markCowToReceiveEmbryoQuickForm()}
             {$this->markCowAsImplantedQuickform()} 
        </div>
        <div class='col-sm-8 col-sm-pull-4'>
            {$this->displayListOfEmbryoImplantations()}
        </div> 
    </div>    
HTML;
          print($html);
         }
         
         
         function tab2() {
              $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>    
             {$this->addEmbryoStrawToInventory()}
             {$this->markFlushQuickForm()} 
        </div>
        <div class='col-sm-8 col-sm-pull-4'>
            {$this->displayListOfEmbryoStraws()}
        </div> 
    </div>    
HTML;
          print($html);
         }

         
         /* ******************************** */
	/* mark cow flushed	 */
	/* ******************************** */
	function markFlushQuickForm()
	{

		$dateArray=$GLOBALS['MiscObj']->createDailyDatesAtDailyIntervalsForNextNineAndSomePrevDays();//hourly array, with unix timestamp as index.

		//grab a list of cows.
		$res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number,full_reg_number from bovinemanagement.bovine WHERE death_date IS NULL ORDER BY local_number");
		
		$aniNumber[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch())) {
			//parse for last word of name and then make an array for the select list.
			$aniNumber[$row[0]] = $row[2] ." - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
		}

		


		$form = new HTML_QuickForm("markFlushQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

		$form->addElement('header', 'hdrQuickform','Mark a Flush');

		$form->addElement('select', 'animalNumber', 'Animal Number',$aniNumber);
                
		$form->addElement('select', 'DateFlush', 'Time of Flush', $dateArray);


		$form->addElement('hidden','pageid',$_REQUEST['pageid']);

		//submit buttons
		$submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
		$submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
		$form->addGroup($submitGroup, 'submitGroup');

		//defaults
		$form-> setDefaults(array('animalNumber' => 0)); //set default to no variety.
		
		$form-> setDefaults(array('DateFlush' => 0 ));

		//rules
		$form->addRule('animalNumber', 'Animal number is required', 'required');
		
		$form->addRule('DateFlush', 'Flush Date is required', 'required');

		// Try to validate a form
		if ($form->validate()) {

			//get values
			$bovine_id=$form->exportValue('animalNumber');
			
			$tempArray=$form->getElementValue('DateFlush');
			$dateFlush=$tempArray[0];
			$estimated_time=date('Y-m-d 12:00:00',$dateFlush);
			$pageid=$form->exportValue('pageid');

			$self::scheduleFlushDefaultInsert($bovine_id,$estimated_time);

			$host  = $_SERVER['HTTP_HOST'];
			header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
			exit();

		} //end validation

		$form->accept($renderer);
        return $renderer->toHtml();

	}
	
        //used when part of scheduled protocol for a flush
        public static function scheduleFlushDefaultInsert($bovine_id,$estimated_time,$uuid='null') {
            
            $res = $GLOBALS['pdo']->query("SELECT full_reg_number from bovinemanagement.bovine WHERE id=$bovine_id");
            $row = $res->fetch();
            $donor_dam_full_reg_number=$row[0];
            //
            $query = "INSERT INTO bovinemanagement.embryo_flush (donor_dam_full_reg_number,flush_date,userid,protocol_uuid) VALUES ('$donor_dam_full_reg_number','$estimated_time','{$GLOBALS['auth']->getUsername()}','$uuid')";
			
			$res=$GLOBALS['pdo']->exec($query);   
        }
        
         /**
     * used by breeding protocol to do deletes
     */
    public static function deleteCowToBeFlushed($uuid) {
        $query = "DELETE FROM bovinemanagement.embryo_flush WHERE protocol_uuid='$uuid'";
        $res = $GLOBALS['pdo']->exec($query);
    }
        
function addEmbryoStrawToInventory() {

                 $dateArray=$GLOBALS['MiscObj']->createDailyDatesAtDailyIntervalsForNextNineAndSomePrevDays();//hourly array, with unix timestamp as index.

		//grab a list of cows.
		$res = $GLOBALS['pdo']->query("SELECT embryo_flush.id,donor_dam_full_reg_number,flush_date,bovine.full_name FROM bovinemanagement.embryo_flush
                                     LEFT JOIN bovinemanagement.bovine ON bovine.full_reg_number=embryo_flush.donor_dam_full_reg_number
                                      ORDER BY flush_date DESC");
		
		$flushNumber[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$flushNumber[$row['id']] = $GLOBALS['MiscObj']->femaleShortName($row['full_name']).' - '.$row['donor_dam_full_reg_number'] ." - " .date('M d, Y',strtotime($row['flush_date']));
		}

		 //grab a list of all sires.
		$sql="SELECT full_reg_number,full_name,short_name FROM bovinemanagement.sire ORDER BY short_name ASC";
		$res = $GLOBALS['pdo']->query($sql);
		
		$sireList[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$sireList[$row['full_reg_number']] = $row['short_name'] ." ( " .$row['full_name'] . " ) ";
		}

                //generate list of possible straw numbers up to 50.
		$strawNumbers[null] = "Please Select one..."; //dummy value
		for ($i=1;$i <=50;$i++) {
		$strawNumbers[$i]="$i";
		}
                
                $qualityArray=array(null=>'Please Select one...',1=>1,2=>2);
		
		$numberOfBinArray=array(null=>'Please Select one...',1=>1,2=>2,3=>3,4=>4,5=>5,6=>6,99999=>'Bottom of Tank');


		$form = new HTML_QuickForm("addEmbryoStrawToInventoryQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

		$form->addElement('header', 'hdrQuickform','Add Embryo Straw to Inventory');

		$form->addElement('select', 'flushNumber', 'Flush',$flushNumber);
		
		$form->addElement('select', 'sireNumber', 'Sire',$sireList);

		$form->addElement('select', 'strawNumber', 'Straw Number',$strawNumbers);
                
                $form->addElement('select', 'quality', 'Quality',$qualityArray);

		$form->addElement('select', 'binNumber', 'Bin Number', $numberOfBinArray);
		
                $form->addElement('textarea', 'caneid', 'Cane ID');
                
                $form->addElement('textarea', 'comment2', 'Custom Comment', array('cols' => 40, 'rows' => 6));
                
		


		$form->addElement('hidden','pageid',$_REQUEST['pageid']);

		//submit buttons
		$submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
		$submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
		$form->addGroup($submitGroup, 'submitGroup');
                

		//defaults
		$form-> setDefaults(array('flushNumber' => 0)); //set default to no variety.
		$form-> setDefaults(array('sireNumber' => 0 ));
                $form-> setDefaults(array('quality' => 1 ));
		$form-> setDefaults(array('strawNumber' => 0 ));
		
		$form-> setDefaults(array('binNumber' => 0 ));

		//rules
		$form->addRule('flushNumber', 'Flush info is required', 'required');
		$form->addRule('sireNumber', 'Animal number is required', 'required');
		$form->addRule('strawNumber', 'Straw number is required', 'required');
		$form->addRule('quality', 'Quality is required', 'required');
		$form->addRule('binNumber', 'Bin Number is required', 'required');

		// Try to validate a form
		if ($form->validate()) {

			//get values
			$flush_id=$form->exportValue('flushNumber');
			$sire_reg=$form->exportValue('sireNumber');
			$straw_number=$form->exportValue('strawNumber');
                        $quality=$form->exportValue('quality');
			$bin=$form->exportValue('binNumber');
			
			$pageid=$form->exportValue('pageid');

                        
            $caneid = trim($form->exportValue('caneid'));  
            $caneid_esc = pg_escape_string($caneid); 
            $comment_esc = pg_escape_string($comment2);         
            $comment2 = trim($form->exportValue('comment2'));           
            $comment_esc = pg_escape_string($comment2);

            if ($caneid_esc =='') {
               $caneid_esc=null; 
            }
            
                        
                        

			//don't put id value, uses next one in DB.
			$query = "INSERT INTO bovinemanagement.embryo_straw (embryo_flush_id,straw_number,sire_full_reg_number,tank,bin,quality,cane,comment) VALUES ($flush_id,$straw_number,'$sire_reg','A',$bin,$quality,'$caneid_esc','$comment_esc')";
			
			$res=$GLOBALS['pdo']->exec($query);


			$host  = $_SERVER['HTTP_HOST'];
			header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
			exit();

		} //end validation

		$form->accept($renderer);
        return $renderer->toHtml();

	}



	function displayListOfEmbryoImplantations($mode='default') {

         
             $sql = <<<SQL
SELECT embryo_implant.id,bovine.id as bovine_id,bovine.local_number,bovine.full_name,event_time,implanter_userid,embryo_implant.userid as userid,estimated_event_time as event_time2,estimated_event_userid,embryo_implant_comment.comment,bin,straw_number,donor_dam_full_reg_number,embryo_straw.sire_full_reg_number
FROM bovinemanagement.embryo_implant
LEFT JOIN bovinemanagement.bovine ON embryo_implant.bovine_id = bovine.id
LEFT JOIN bovinemanagement.embryo_straw ON embryo_straw.id = embryo_implant.embryo_straw_id
LEFT JOIN bovinemanagement.embryo_implant_comment ON embryo_implant.comment_id=embryo_implant_comment.id
LEFT JOIN bovinemanagement.embryo_flush ON embryo_flush.id=embryo_straw.embryo_flush_id
ORDER BY event_time DESC
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);  
            $rowOut['Implanter'] = $row['implanter_userid'];
            $rowOut['Employee'] = $row['userid'];
            $rowOut['Est Time'] = JQueryDataTable::formattedDate($row['event_time2']);
            $rowOut['Est Event Employee'] = $row['estimated_event_userid'];
            $rowOut['Comment'] = $row['comment'];
            $rowOut['Bin'] = $row['bin'];
            $rowOut['Straw #'] = $row['straw_number'];
            $rowOut['Donor Dam Number'] = $row['donor_dam_full_reg_number'];
            $rowOut['Sire'] = $row['sire_full_reg_number'];
            if ($mode == 'default') {
                $rowOut['Action'] = $this->printActionButtons2($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            }


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
      

         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
         return implode($out); 
      
            
                
        }

	function printPartialName($params)
	{
		extract($params);
		$animalShortName=$GLOBALS['MiscObj']->femaleShortName($row['full_name']);
		return $animalShortName;
	}

        
        
         /** **********************************************************
	   * see whether to print completed button for scheduled medicine 
	   *********************************************************** */
   function printActionButtons2($record,$callBackURL)
	  {
	    
	    $currentUserid=$GLOBALS['auth']->getUsername();

	       $returnButtonsAsString=null;

	    if (((in_array('owner',$GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin',$GLOBALS['auth']->getAuthData('groups')) == TRUE)) AND ($record['event_time']==null))
	      {
		$returnButtonsAsString=self::deleteQuickForm($record['id'],'embryo_implant',$callBackURL); //show delete button, with Breeding table id as index for delete.
	      }
	  
	    
	    
	    return $returnButtonsAsString;
	  }
        
          /** *******************************************
	  * Delete Button Quickform
	  **********************************************/
	  function deleteQuickForm($tableid,$tablename,$callBackURL)
	  {
	    
	    $form = new HTML_QuickForm("medSchedDeleteButton",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
	    $form->addElement('hidden','pageid',$_REQUEST['pageid']);
	    $form->addElement('hidden','tableid',$tableid);
	    $form->addElement('hidden','tablename',$tablename);
	    $form->addElement('submit', 'btnDelete', 'Delete', array('id'=>'deleteSubmit','onclick' => "return confirm('Are you sure you want to delete?')")); //call javascript to confirm delete.
	    //FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY USER.
	    //DB hack works though, views need to support deletes before this can be fixed.
	    
	    // Try to validate a form
	    if ($form->validate()) {
	      
	      //get values
	      $pageid=$form->exportValue('pageid');
	      $tableid=$form->getElementValue('tableid');
	      $tablename=$form->getElementValue('tablename');
	      
	      // Open a transaction
	      try {$res = $GLOBALS['pdo']->beginTransaction();
	       
	   
	          
	      //delete the event
	      $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.$tablename where id='$tableid'");
	      
	      // determine if the commit or rollback
	       $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
	      
	      header($callBackURL);
	      exit();
	      
	      }
	      return $form->toHtml(); //pass on as html string to display later in the table
  
	      }    
	      
        

	/** ***********************************
	 Mark cow as embryo implanted
	 **/
	function markCowAsImplantedQuickform()
	{

		//$dateArray=$GLOBALS['MiscObj']->createHourlyDatesForLastTwoDays(); //hourly array, with unix timestamp as index.
		$dateArray=$GLOBALS['MiscObj']->createHourlyDatesForLastFourDays();

		//grab a list of cows who have ben chosen to be bred and have semen chosen.
		$sql="SELECT embryo_implant.id,bovine_id,local_number,full_name,estimated_event_time
FROM bovinemanagement.embryo_implant 
LEFT JOIN bovinemanagement.bovine ON embryo_implant.bovine_id = bovine.id
WHERE embryo_implant.event_time is null";
		$res = $GLOBALS['pdo']->query($sql);
		
		$aniNumber[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$aniNumber[$row['id']] = $row['local_number'] ." - " .$GLOBALS['MiscObj']->femaleShortName($row['full_name'])."&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;". date("D, M j", strtotime($row['estimated_event_time']));
		}

		//grab a list of  comments
		$res = $GLOBALS['pdo']->query("SELECT id,comment FROM bovinemanagement.embryo_implant_comment ORDER BY id");
		
		$embryoImplantComment['null'] = "Please Select one..."; //dummy value since it is optional and not required we can make it a null string for postgres.
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$embryoImplantComment[$row['id']] = $row['comment'];
		}

		//grab a list of people who do embryo implants.
		$sql="SELECT users.userid,firstname,lastname
FROM wcauthentication.users
LEFT JOIN wcauthentication.users_in_groups ON users_in_groups.userid = users.userid 
LEFT JOIN wcauthentication.groups ON groups.name = users_in_groups.groupid 
WHERE groups.name='embryo_implant'";
		$embryoImplanter['null'] = "Please Select one..."; //dummy value since it is optional and not required we can make it a null string for postgres.
		$res = $GLOBALS['pdo']->query($sql);
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$embryoImplanter[$row['userid']] = $row['firstname'] .' '. $row['lastname'];
		}

		$form = new HTML_QuickForm("markCowAsImplantedQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

		$form->addElement('header', 'hdrQuickform','Mark Cow as Embryo Implanted');

		$form->addElement('select', 'animalNumber', 'Animal Number',$aniNumber); //key is breeding_event id.

		$form->addElement('select', 'DateActualImplantation', 'Time of Actual Embryo Implantation', $dateArray);

		$form->addElement('select', 'implanterUserid', 'Implanter User', $embryoImplanter);

		$form->addElement('select', 'embryoImplantComment', 'Implantation Comment (Optional)', $embryoImplantComment);

		$form->addElement('hidden','pageid',$_REQUEST['pageid']);

		//submit buttons
		$submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
		$submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
		$form->addGroup($submitGroup, 'submitGroup');

		//defaults
		$form-> setDefaults(array('animalNumber' => 0)); //set default to no variety.
		$form-> setDefaults(array('DateActualImplantation' => 0 ));
		$form-> setDefaults(array('implanterUserid' => 0 ));

		//rules
		$form->addRule('animalNumber', 'Animal number is required', 'required');
		$form->addRule('DateActualImplantation', 'Date of Implantation is required', 'required');
		$form->addRule('implanterUserid', 'Who actually did the implantation is required', 'required');


		// Try to validate a form
		if ($form->validate()) {

			//get values
			$id=$form->exportValue('animalNumber');
			$tempArray=$form->getElementValue('DateActualImplantation');
			$DateActualImplantation=$tempArray[0];
			$actual_time=date('Y-m-d H:i:sO',$DateActualImplantation);
			$pageid=$form->exportValue('pageid');
			$comment_id=$form->exportValue('embryoImplantComment');
			$implanter_Userid=$form->exportValue('implanterUserid');

			//don't put id value, uses next one in DB.
			$query = "UPDATE bovinemanagement.embryo_implant SET (event_time,implanter_userid,userid,comment_id) = ('$actual_time','$implanter_Userid','{$GLOBALS['auth']->getUsername()}',$comment_id) WHERE id='$id'";
			
			$res=$GLOBALS['pdo']->exec($query);

			

			//forward to same page, ob_start() is key to making this work!
			$host  = $_SERVER['HTTP_HOST'];
			header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
			exit();

		} //end validation

		$form->accept($renderer);
        return $renderer->toHtml();


	}

	/* ******************************** */
	/* mark cow to receive embryo	 */
	/* ******************************** */
	function markCowToReceiveEmbryoQuickForm()
	{

		$dateArray=$GLOBALS['MiscObj']->createDailyDatesAtDailyIntervalsForNextNineAndSomePrevDays();//hourly array, with unix timestamp as index.

		//grab a list of cows.
		$res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovine WHERE death_date IS NULL ORDER BY local_number");
		
		$aniNumber[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch())) {
			//parse for last word of name and then make an array for the select list.
			$aniNumber[$row[0]] = $row[2] ." - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
		}

		//grab a list of embro straws.
		$sql="SELECT embryo_straw.id,embryo_flush.donor_dam_full_reg_number,embryo_flush.flush_date,embryo_straw.sire_full_reg_number,embryo_straw.straw_number,embryo_straw.bin,embryo_implant.event_time,sire.short_name as sire_short_name,bovine.full_name as full_name,bovine.local_number
FROM bovinemanagement.embryo_flush
LEFT JOIN bovinemanagement.embryo_straw ON embryo_flush.id = embryo_straw.embryo_flush_id
LEFT JOIN bovinemanagement.embryo_implant ON embryo_implant.embryo_straw_id = embryo_straw.id
LEFT JOIN bovinemanagement.sire ON embryo_straw.sire_full_reg_number = sire.full_reg_number
LEFT JOIN bovinemanagement.bovine ON embryo_flush.donor_dam_full_reg_number = bovine.full_reg_number
WHERE embryo_implant.estimated_event_userid IS NULL
ORDER BY flush_date DESC,straw_number ASC";
		$res = $GLOBALS['pdo']->query($sql);
		
		$embryoStraw[null] = "Please Select one..."; //dummy value
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
			//parse for last word of name and then make an array for the select list.
			$embryoStraw[$row['id']] = $GLOBALS['MiscObj']->femaleShortName($row['full_name']) ." - ".$row['donor_dam_full_reg_number'] ." - " . date('M d, Y',strtotime($row['flush_date'])) . ' to '. ''. $row['sire_short_name'] . ' straw # '.$row['straw_number'];
		}


		$form = new HTML_QuickForm("markCowToReceiveEmbryoQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

		$form->addElement('header', 'hdrQuickform','Mark Cow to be Receive Embryo');

		$form->addElement('select', 'animalNumber', 'Animal Number',$aniNumber);

		$form->addElement('select', 'embryo_straw_id', 'Embryo to Implant',$embryoStraw);

		$form->addElement('select', 'DateEstimateRecipientImplant', 'Time of Estimated Recipient Implantation', $dateArray);


		$form->addElement('hidden','pageid',$_REQUEST['pageid']);

		//submit buttons
		$submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
		$submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
		$form->addGroup($submitGroup, 'submitGroup');

		//defaults
		$form-> setDefaults(array('animalNumber' => 0)); //set default to no variety.
		$form-> setDefaults(array('embryo_straw_id' => 0 ));
		$form-> setDefaults(array('DateEstimateRecipientImplant' => 0 ));

		//rules
		$form->addRule('animalNumber', 'Animal number is required', 'required');
		$form->addRule('embryo_straw_id', 'An embryo choice is required', 'required');
		$form->addRule('DateEstimateRecipientImplant', 'Estimated Date of Implantation is required', 'required');

		// Try to validate a form
		if ($form->validate()) {

			//get values
			$bovine_id=$form->exportValue('animalNumber');
			$embryo_straw_id=$form->exportValue('embryo_straw_id');
			$tempArray=$form->getElementValue('DateEstimateRecipientImplant');
			$DateEstimateRecipientImplant=$tempArray[0];
			$estimated_time=date('Y-m-d 12:00:00',$DateEstimateRecipientImplant);
			$pageid=$form->exportValue('pageid');

			//don't put id value, uses next one in DB.
			$query = "INSERT INTO bovinemanagement.embryo_implant (bovine_id,embryo_straw_id,estimated_event_time,estimated_event_userid) VALUES ($bovine_id,$embryo_straw_id,'$estimated_time','{$GLOBALS['auth']->getUsername()}')";
			
			$res=$GLOBALS['pdo']->exec($query);

			

			$host  = $_SERVER['HTTP_HOST'];
			header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
			exit();

		} //end validation

		$form->accept($renderer);
        return $renderer->toHtml();

	}




	function displayListOfEmbryoStraws($mode='default') {
		
                
             $sql = <<<SQL
SELECT embryo_straw.id,embryo_straw.cane,embryo_straw.comment,embryo_flush.donor_dam_full_reg_number,bovine.local_number as donor_dam_local_number,embryo_flush.flush_date as event_time2,embryo_straw.sire_full_reg_number,embryo_straw.straw_number,embryo_straw.bin,embryo_implant.event_time, local_number,bovine.id as bovine_id, bovine.full_name,sire.short_name
FROM bovinemanagement.embryo_flush
LEFT JOIN bovinemanagement.embryo_straw ON embryo_flush.id = embryo_straw.embryo_flush_id
LEFT JOIN bovinemanagement.embryo_implant ON embryo_implant.embryo_straw_id = embryo_straw.id
LEFT JOIN bovinemanagement.bovine ON embryo_flush.donor_dam_full_reg_number = full_reg_number
LEFT JOIN bovinemanagement.sire ON embryo_straw.sire_full_reg_number = sire.full_reg_number
WHERE embryo_implant.event_time IS NULL
ORDER BY flush_date DESC,straw_number ASC
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Flush Date'] = JQueryDataTable::formattedDate($row['event_time2']);
            $rowOut['Donor Dam'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Donor Dam Number'] = $row['donor_dam_full_reg_number'];
            $rowOut['Sire Name'] = $row['short_name'];
            $rowOut['Sire Number'] = $row['sire_full_reg_number'];
            $rowOut['Bin #'] = $row['bin'];
            $rowOut['Cane #'] = $row['cane'];
            $rowOut['Straw #'] = $row['straw_number'];
            $rowOut['Comment'] = $row['comment'];

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
      
        $out[]="<h3>Embryo Inventory</h3>";
        $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
        return implode($out); 
      
	}

      

}//end class
?>
