<?php 




class Sop extends BasePage{
  
  
  public function defaultDisplay() {

  $this->renderTabs();
  //$this->addNewSOPQuickform();  
  //$this->editNewSOPQuickform();  
  

  }
 
 /** **************************************************/
 /** Dynamically render tabs based on info from DB    */
 /** **************************************************/
 function renderTabs() {	

   
        print('<div id="jquery_tabs" class="flora"><ul>');     
         if ((in_array('owner',$GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin',$GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
         print("	<li><a title=\"x_0\" href=\"#x_0\"><span>Edit SOP</span></a></li>"."\n\r");
	 }
        
      //print titles for tab labels.
      $res = $GLOBALS['pdo']->query("SELECT id,title FROM misc.sop WHERE active='true' ORDER BY title");
	//$counter=1;
	while ($row=$res->fetch(PDO::FETCH_ASSOC)) {
	 print("	<li><a title=\"x_{$row['id']}\" href=\"#x_{$row['id']}\"><span>{$row['title']}</span></a></li>"."\n\r");
	// $counter++;
	} 
         print('</ul>');
        
         if ((in_array('owner',$GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin',$GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
        //add tab 0 which is the edit tab
        if (empty($_REQUEST['sop_id'])) { $sop_id=null; }
        else { $sop_id=$_REQUEST['sop_id'];}
        print("<div id='x_0'>"."\n\r"); 
        $this->addNewSOPQuickform(); 
        print('<hr/>');  
        $this->whichSOPQuickform(); 
        $this->editSOPQuickform($sop_id);      
        print('</div>'."\n\r");
	 }
   
     //print content in tabs
     $res = $GLOBALS['pdo']->query("SELECT *  FROM misc.sopcurr");
	while ($row=$res->fetch(PDO::FETCH_ASSOC)) {
	 print("<div id='x_{$row['id']}'>"."\n\r"); 
	 print("{$row['content']}"."\n\r");     
	 print('</div>'."\n\r");
	} 
    
    print('</div> <!--end of tabs div -->');
   
  }	
 
 
  /** ******************************** */
  /** Which SOP                        */
  /** ******************************** */
 function whichSOPQuickform() {
 
     //grab a list of sop titles.
      $res = $GLOBALS['pdo']->query("SELECT id,title FROM misc.sop ORDER BY title");
	$sopTitleList[null] = "Please Select one..."; //dummy value
	while ($row=$res->fetch(PDO::FETCH_ASSOC)) {
	  $sopTitleList[$row['id']] = $row['title'];
	}

      
        $form = new HTML_QuickForm("changeWhichSOPQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();
         $form->addElement('header', 'hdrQuickform','Edit SOP');
      //javascript to allow submission on change of select element.
		$attrs = array('onchange' => "javascript:location.href='?sop_id='+this.options[this.selectedIndex].value+'&pageid={$_REQUEST['pageid']}';$('#jquery_tabs > ul').tabs('select', 0);");
 $form->addElement('select', 'title', 'SOP', $sopTitleList,$attrs);
  // Try to validate a form
      if ($form->validate()) {
      
   } //end validation
    
    $form->accept($renderer);
        echo $renderer->toHtml();
  }
 
 
  /** ******************************** */
  /** Edit SOP                         */
  /** ******************************** */
 function editSOPQuickform($sop_id) {
 ?>
 <!--
<form method="post" action="somepage">
	<textarea name="content" style="width:100%">
	</textarea>
</form>
-->
<?php 
if ($sop_id!=null) {

        $res = $GLOBALS['pdo']->query("SELECT id,title,content FROM misc.sopcurr WHERE id=$sop_id");
	$row=$res->fetch(PDO::FETCH_ASSOC);
	$content=$row['content'];
	
	
	//has to be a seperate query because if it is a new sop, ther is no content yet.
	$res2 = $GLOBALS['pdo']->query("SELECT id,title FROM misc.sop WHERE id=$sop_id");
	$row2=$res2->fetch(PDO::FETCH_ASSOC);
	$title=$row2['title'];
    
     

      print("<h3>Editing: $title</h3>");
     
      $form = new HTML_QuickForm("editSOPQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
      
     
      $form->addElement('hidden','pageid',$_REQUEST['pageid']);
      $form->addElement('hidden','sop_id',$sop_id);
      
     
      
      $form->addElement('textarea',   'content', 'Content', array('cols'=>20, 'rows'=>2));
      $form->setDefaults(array('content' => "$content"));
         
      //submit buttons
      $submitGroup[] = $form->createElement('button', 'btnCancel','Cancel',array('onclick' =>"javascript:location.href='?pageid={$_REQUEST['pageid']}';$('#jquery_tabs > ul').tabs('select', 0);"));
      $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
      $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
      $form->addGroup($submitGroup, 'submitGroup');
      
      //rules
       $form->addRule('content', 'Some content is required', 'required');
      
      // Try to validate a form
      if ($form->validate()) {
	
	//get values
	$pageid=$form->exportValue('pageid');
	$sop_id=$form->exportValue('sop_id'); 
	$content=pg_escape_string(trim($form->exportValue('content')));
	$event_time=date('c',strtotime("now"));
	
	// Open a transaction
	try {$res = $GLOBALS['pdo']->beginTransaction();
	      
	//insert into sop table.
	$query = "INSERT INTO misc.sop_content (sop_id,content,event_time,userid) VALUES ($sop_id,'$content','$event_time','{$GLOBALS['auth']->getUsername()}')";
	$res=$GLOBALS['pdo']->exec($query);
	

	 $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
	
	header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
	exit();
	
    } //end validation
    
    $form->accept($renderer);
        echo $renderer->toHtml();
  }
  }
 
  /** ******************************** */
  /** Add a new SOP                    */
  /** ******************************** */
 function addNewSOPQuickform() {
    
      $form = new HTML_QuickForm("addNewSOPQuickform",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
      
      $form->addElement('header', 'hdrQuickform','Add New SOP');
      $form->addElement('hidden','pageid',$_REQUEST['pageid']);
      $form->addElement('text', 'title', 'Title');
         
      //submit buttons
      $submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
      $submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
      $form->addGroup($submitGroup, 'submitGroup');
      
      //rules
      $form->addRule('title', 'Title is required', 'required');
      
      // Try to validate a form
      if ($form->validate()) {
	
	//get values
	$pageid=$form->exportValue('pageid');
	
	$title=pg_escape_string(trim($form->exportValue('title')));
	
	// Open a transaction
	try {$res = $GLOBALS['pdo']->beginTransaction();
	      
	//insert into sop table.
	$query = "INSERT INTO misc.sop (title,userid) VALUES ('$title','{$GLOBALS['auth']->getUsername()}')";
	$res=$GLOBALS['pdo']->exec($query);
	$sop_id=$GLOBALS['pdo']->lastInsertId('misc.sop_id_seq');


	// determine if the commit or rollback
	 $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
	
	header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&editsopid=$sop_id");
	exit();
	
    } //end validation
    
    $form->accept($renderer);
        echo $renderer->toHtml();
  }
 
 
    
  function customJavascript() {
    ?>
    <script type="text/javascript" src="framework/composer/vendor/tinymce/tinymce/tinymce.min.js"></script>
<script type="text/javascript">
    
   tinymce.init({
    mode : "textareas",
    theme: 'modern',
    width: 1000,
    height: 500,
    plugins: [
      'advlist autolink link image lists charmap print preview hr anchor pagebreak spellchecker',
      'searchreplace wordcount visualblocks visualchars code fullscreen insertdatetime media nonbreaking',
      'save table contextmenu directionality emoticons template paste textcolor'
    ],
    toolbar: 'insertfile undo redo | styleselect | bold italic | alignleft aligncenter alignright alignjustify | bullist numlist outdent indent | link image | print preview media fullpage | forecolor backcolor emoticons'
  });
    
</script>
<?php 

 require_once($_SERVER['DOCUMENT_ROOT'] . 'template/tabPage.php');
     TabPage::customJavascript();



  }
  
  
} //end class  