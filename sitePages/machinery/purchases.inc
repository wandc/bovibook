<?php 
class Purchases extends TabPage {

    public function defaultDisplay() {
        //setup for tabs.
        $tabArray[1]['name'] = 'Current';
        $tabArray[2]['name'] = 'Log';
        $this->renderTabs($tabArray);
    }

    function tab1() {
          $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$this->purchaseRequestQuickform()}
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayListOfPurchases()}
        </div>
    </div>    
HTML;
     
         print($html);
        
    }

    function tab2() {
        print($this->displayListOfPurchasesLog());
    }

    public function purchaseCompletedRejected($purchases_id,$callBackURL) {
        $form = new HTML_QuickForm("purchaseCompletedRejectedQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        //$form->addElement('header', 'hdrQuickform','Purchase Completed or Rejected');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'purchases_id', $purchases_id);
        //
        $form->addElement('text', 'comment', 'Comment', 'size="45"');

        $form->addElement('checkbox', 'yesno', 'Yes or No', null, null);

        //submit buttons
        $form->addElement('submit', 'btnCompleted', 'Completed');

// Try to validate a form
        if ($form->validate()) {

            //get values
            $purchased = $form->exportValue('yesno');
            if ($purchased == 1) {
                $purchased = 'true';
            } else {
                $purchased = 'false';
            }


            $purchasing_comment = $form->exportValue('comment');
            //

            $purchases_id = $form->exportValue('purchases_id');
            $pageid = $form->exportValue('pageid');
            $purchasing_event_time = date('c', strtotime('now'));
            //
            $query = "UPDATE misc.purchases SET purchasing_event_time='$purchasing_event_time',purchased='$purchased',purchasing_comment='$purchasing_comment',purchasing_userid='{$GLOBALS['auth']->getUsername()}' WHERE id=$purchases_id";

            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header($callBackURL);
            exit();
        } //end validation

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function printActionButtons($record,$callBackURL) {
       

        $returnButtonsAsString = self::purchaseCompletedRejected($record['id'],$callBackURL); //show form
        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    /** used to enter purchase requests */
    public function purchaseRequestQuickform() {


        //grab a list of suppliers
        $res = $GLOBALS['pdo']->query("SELECT name FROM misc.purchase_suppliers WHERE active='true' ORDER BY name");

        $suppliersList[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            $suppliersList["$row[0]"] = $row[0];
        }

        //grab a list of cows with open medical cases.
        //$aniNumber=$this->grabListCowsWithOpenMedicalCase();       

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday();
        unset($timeArray[null]); //remove null value.
        //create array of celcius temps.
        $temperatureArray = array(null => 'Please Select One');
        for ($counter = 37.5; $counter <= 42.1; $counter += 0.10) {
            $farenheight = round(( (9 / 5) * ($counter) + 32), 1);
            $celcius = round($counter, 1);
            $temperatureArray["$celcius"] = "$celcius &#8451; &nbsp; / &nbsp; $farenheight &deg;F";
        }


        $form = new HTML_QuickForm("purchaseRequestQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Purchase Request');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        //
        $form->addElement('text', 'quantity', 'Quantity', 'size="5" maxlength="5" id="dosageval"');
        $form->addElement('textarea', 'description', 'description', array('cols' => 60, 'rows' => 4));

        //list of pre-canned suppliers
        $form->addElement('select', 'supplier1', 'Listed Supplier', $suppliersList);
        $form->addElement('text', 'supplier2', 'OR Enter a Supplier', 'size="35" ');


        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');


        //rules
        $form->addRule('quantity', 'Enter a valid number for quantity.', 'numeric');
        $form->addRule('quantity', 'Quantity is required.', 'required');
        $form->addRule('description', 'Description is required.', 'required');
        //
        $form->registerRule('supplierRequiredRule', 'function', 'supplierRequired', 'Purchases');
        $form->addRule(array('quantity', 'supplier1', 'supplier2'), 'A canned or custom supplier is required.', 'supplierRequiredRule', array('quantity', 'supplier1', 'supplier2'));

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $quantity = $form->exportValue('quantity');
            $description = $form->exportValue('description');
            $supplier1 = $form->exportValue('supplier1');
            $supplier2 = $form->exportValue('supplier2');
            //
            $pageid = $form->exportValue('pageid');
            $requesting_event_time = date('c', strtotime('now'));
            //
            //only choose one supplier, custom entry always overrides.
            if (strlen($supplier1) != 0) {
                $supplier = pg_escape_string($supplier1);
            } else {
                $supplier = pg_escape_string($supplier2);
            }

            $query = "INSERT INTO misc.purchases (requesting_event_time,requesting_userid,quantity,description,supplier) VALUES ('$requesting_event_time','{$GLOBALS['auth']->getUsername()}',$quantity,'$description','$supplier')";


            $res = $GLOBALS['pdo']->exec($query);

            //$_SESSION['selectedTab'] = 2;
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    //check at least one supplier was filled in.
    function supplierRequired($valueArr, $elementNameArr) {
        //generic loop to convert $form elements we want into a usuable array
        //I really hope this isn't how quickforms was designed.
        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData[$elementName] = $valueArr[$count];
            $count++;
        }

        //only return false when no supplier was entered or selected.
        if ((strlen(trim($formData['supplier1'])) != 0) || (strlen(trim($formData['supplier2'])) != 0)) {
            return true;
        } else {
            return false;
        }
    }

    public function displayListOfPurchases($mode = 'default') {
        
         $sql = <<<SQL
SELECT * FROM misc.purchases WHERE purchasing_userid is null ORDER BY requesting_event_time DESC
SQL;

        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
       
            $rowOut['Requesting Time'] = JQueryDataTable::formattedDate($row['requesting_event_time']);
            $rowOut['Quantity'] = ($row['quantity']);
            $rowOut['Description'] = ($row['description']);
            $rowOut['Requesting User'] = ($row['supplier']);
            $rowOut['Requesting User'] = ($row['requesting_userid']);
            $rowOut['Action'] = $this->printActionButtons2($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $rowOut['Completed?'] = $this->printActionButtons($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
             }
           

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Purchase</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'order: false')); 
         return implode($out); 
        
    }

    public function displayListOfPurchasesLog($mode = 'default') {

          $sql = <<<SQL
SELECT * FROM misc.purchases WHERE purchasing_userid != 'null' ORDER BY requesting_event_time DESC
SQL;

        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
       
            $rowOut['Requesting Time'] = JQueryDataTable::formattedDate($row['requesting_event_time']);
            $rowOut['Quantity'] = ($row['quantity']);
            $rowOut['Description'] = ($row['description']);
            $rowOut['Requesting User'] = ($row['supplier']);
            $rowOut['Requesting User'] = ($row['requesting_userid']);
            $rowOut['Purchasing Time'] = JQueryDataTable::formattedDate($row['purchasing_event_time']);
            $rowOut['Purchasing Comment'] = ($row['purchasing_comment']);
            $rowOut['Purchasing User'] = ($row['purchasing_userid']);
            $rowOut['ID'] = ($row['id']);
            $rowOut['Purchased?'] = ($row['purchased']);


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Purchase Log</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'order: false')); 
         return implode($out); 
         
 
    }

    function printActionButtons2($record,$callBackURL) {

        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM misc.purchases WHERE requesting_userid='$currentUserid' ORDER BY requesting_event_time DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if ($record['id'] == $row['id']) {
            $deleteButtonAsString = self::deleteQuickForm($record['id'],$callBackURL); //show delete button, with estrus table id as index for delete.
        }

        return $deleteButtonAsString;
    }

    function deleteQuickForm($purchasesID,$callBackURL) {

        $form = new HTML_QuickForm("purchasesDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'purchasesID', $purchasesID);
        $form->addElement('submit', 'btnDelete', 'Delete',array('class'=>'btn btn-warning'));


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $purchasesID = $form->getElementValue('purchasesID');


            $res = $GLOBALS['pdo']->query("DELETE FROM misc.purchases where id='$purchasesID'");



            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

   

}

//end class
?>