<?php 

require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');



class CullList extends BasePage{

	private $bovine_id;


	public function defaultDisplay() {

            if (empty($_REQUEST['bovine_id'])) {
                $_REQUEST['bovine_id']=null;
            }
            if (!filter_var( $_REQUEST['bovine_id'], FILTER_VALIDATE_INT) === false) {
            $this->bovine_id = $_REQUEST['bovine_id'];
        } else {
             $this->bovine_id =null;
        }
            
		

		$this->animalQueryQuickForm();

		if ($this->bovine_id != null) {
			$this->cullEventQuickForm();
		}

	}


	function cullEventQuickForm()
	{
		//grab name/number info on current bovine
		$sql="SELECT id as bovine_id,full_name,local_number FROM bovinemanagement.bovine WHERE id={$this->bovine_id}";
		$res = $GLOBALS['pdo']->query($sql);
		$row = $res->fetch(PDO::FETCH_ASSOC);

		$animalText= $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
		 


		print('<h1>FIXME: DOES NOT WORK YET.</h1>');

			

		$form = new HTML_QuickForm("cullEventQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

		$form->addElement('header', 'cullEventQuickform',"Record Cull Choice for $animalText");
		 
		$form->addElement('hidden','pageid',$this->pageid);
		 
		$form->addElement('radio', 'cull_status', 'Cull Status:', 'Do Not Cull', 1);
		$form->addElement('radio', 'cull_status', '', 'Cull', 2);
		$form->addElement('radio', 'cull_status', '', 'Potential Cull', 3);

		//submit buttons
		$submitGroup[] = $form->createElement('reset',  'btnClear', 'Clear');
		$submitGroup[] = $form->createElement('submit', 'btnSubmit','Submit');
		$form->addGroup($submitGroup, 'submitGroup');

		//rules
		$form->addRule('cull_status', 'Please choose a culling status.', 'required');


		//check to see if the cow is already in the DB and if it is, use the default values already in.
		$row=null;
		$sql="SELECT bovine_id,cull_status FROM bovinemanagement.cull_list_event WHERE bovine_id={$this->bovine_id} ORDER BY create_time LIMIT 1";
		$res = $GLOBALS['pdo']->query($sql);
		$row = $res->fetch(PDO::FETCH_ASSOC);
			
		if ($row['cull_status']=='do_not_cull')
			
		switch ($row['cull_status']) {
			case null:
				$form-> setDefaults(array('cull_status' => null));
				break;
			case 'do_not_cull':
				$form-> setDefaults(array('cull_status' => 1));
				break;
			case 'potential_cull':
				$form-> setDefaults(array('cull_status' => 2));
				break;
			case 'cull':
				$form-> setDefaults(array('cull_status' => 3));
				break;
		}
			
			

		// Try to validate a form
		if ($form->validate()) {

			print("XXXXXXXXXXXXXXXXXXXX");
			exit();


			//get values
			$bovine_id=$form->exportValue('bovine_id');
			$pageid=$form->exportValue('pageid');
			$cull_status=null;

			switch ($form->exportValue('cull_status')) {
				case 1:
					$cull_status='do_not_cull';
					break;
				case 2:
					$cull_status='potential_cull';
					break;
				case 3:
					$cull_status='cull';
					break;
				default:
					exception('Error Cull Status is undefined.');
			}


			// Open a transaction
			try {$res = $GLOBALS['pdo']->beginTransaction();

			$query = "INSERT INTO bovinemanagement.cull_list_event (bovine_id,cull_status,userid)
		VALUES ($bovine_id,'$cull_status','{$GLOBALS['auth']->getUsername()}')";        
			$res=$GLOBALS['pdo']->exec($query);

			
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
			// Close the transaction


			//forward to same page, ob_start() is key to making this work!
			$host  = $_SERVER['HTTP_HOST'];
			header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id={$this->bovine_id}");
			exit();

		} //end validation

		$form->accept($renderer);
        echo $renderer->toHtml();
	}

	function animalQueryQuickForm()
	{

		$aniNumber=$GLOBALS['MiscObj']->createListOfAllAliveCows();

		//javascript to allow submission on change of select element.
		$attrs = array('onchange' => "javascript:location.href='?bovine_id='+this.options[this.selectedIndex].value+'&pageid={$_REQUEST['pageid']}';$('#example > ul').tabs('select', 0);");

		$form = new HTML_QuickForm("frmBovineQuery",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

		$form->addElement('header', 'hdrQuickformtest','Select an Animal');
		$form->addElement('select', 'animalNumber', '',$aniNumber,$attrs);
		$form->addElement('hidden','pageid',$_REQUEST['pageid']);

		//defaults
		if ($this->bovine_id != null) {
			$form-> setDefaults(array('animalNumber' => $this->bovine_id)); //set current bovine.
		}
		else {
			$form-> setDefaults(array('animalNumber' => 0)); //set default to no variety.
		}


		// Try to validate a form
		if ($form->validate()) {

			//get values
			__construct($form->exportValue('animalNumber'));
			$pageid=$form->exportValue('pageid');

		} //end validation

		$form->accept($renderer);
        echo $renderer->toHtml();


	}


}//end class