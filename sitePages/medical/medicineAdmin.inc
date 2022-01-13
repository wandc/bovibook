<?php 


class MedicineAdmin extends BasePage {

    public function defaultDisplay() {
        
         $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$this->addMedicineQuickform()}
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayMedicineList()}
        </div>
    </div>    
HTML;
     
         print($html);
       
    }

    /**     * **************************** */
    /**    Display medicine list    * */
    /**     * **************************** */
    function displayMedicineList($mode = 'default') {
        $sql = "SELECT * FROM bovinemanagement.medicine ORDER BY trade_name ASC";
       return( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql,'order: false')); 

    }

    /**     * **************************** */
    /**    Add medicine to list    * */

    /**     * **************************** */
    function addMedicineQuickform() {

        $default_body_location = (new MedicalCase)->medicalBodyLocations(true);

        $form = new HTML_QuickForm("addMedicineQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'addMedicineQuickform', "Add Medicine");
        $form->addElement('text', 'trade_name', 'Trade Name', 'size="50" maxlength="256"');
        $form->addElement('text', 'latin_name', 'Latin Name', 'size="50" maxlength="256"');
        $form->addElement('textarea', 'official_instructions', 'Label Instructions', array('cols' => 80, 'rows' => 6));
        $form->addElement('textarea', 'local_instructions', 'Local Instructions', array('cols' => 80, 'rows' => 6));
        $form->addElement('text', 'medicine_class', 'Medicine Class (antibiotic,vitamin,etc.)', 'size="20" maxlength="64"');
        $form->addElement('text', 'milk_withholding', 'Milk Withholding (hours)', 'size="4" maxlength="4"');
        $form->addElement('text', 'beef_withholding', 'Beef Withholding (days)', 'size="3" maxlength="3"');
        $form->addElement('text', 'milk_post_calving_withholding', 'Milk Post Calving Withholding (hours)', 'size="3" maxlength="3"');
        $form->addElement('text', 'default_dosage', 'Default Dosage', 'size="5" maxlength="5"');
        $form->addElement('text', 'default_dosage_unit', 'Default Dosage Units', 'size="15" maxlength="15"');
        $form->addElement('select', 'default_dosage_location', 'Default Body Location', $default_body_location);
        $form->addElement('advcheckbox', 'for_mastitis', 'Exclusively For Mastitis Treatment (yes/no)');
        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules
        $form->addRule('trade_name', 'Trade Name is required', 'required');
        $form->addRule('latin_name', 'Latin Name is required', 'required');
        $form->addRule('official_instructions', 'Official Instructions is required', 'required');
        $form->addRule('local_instructions', 'Local Instructions is required', 'required');
        $form->addRule('medicine_class', 'Medicine Class is required', 'required');
        $form->addRule('milk_withholding', 'Milk Withholding is required', 'required');
        $form->addRule('beef_withholding', 'Beef Withholding is required', 'required');
        $form->addRule('milk_post_calving_withholding', 'Milk Post Calving Withholding is required', 'required');
        $form->addRule('default_dosage', 'Default Dosage is required', 'required');
        $form->addRule('default_dosage_location', 'Default Location is required', 'required');
        $form->addRule('default_dosage_unit', 'Default Dosage Units is required', 'required');
        //
        $form->addRule('default_dosage', 'Enter a valid number for default dosage.', 'numeric');
        $form->addRule('milk_withholding', 'Enter a valid number for milk withholding.', 'numeric');
        $form->addRule('beef_withholding', 'Enter a valid number for beef withholding.', 'numeric');
        $form->addRule('milk_post_calving_withholding', 'Enter a valid number for milk post calving withholding.', 'numeric');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $trade_name = $form->exportValue('trade_name');
            $latin_name = $form->exportValue('latin_name');
            $official_instructions = pg_escape_string($form->exportValue('official_instructions'));
            $local_instructions = pg_escape_string($form->exportValue('local_instructions'));
            $medicine_class = $form->exportValue('medicine_class');
            $milk_withholding = $form->exportValue('milk_withholding');
            $beef_withholding = $form->exportValue('beef_withholding');
            $milk_post_calving_withholding = $form->exportValue('milk_post_calving_withholding');
            $default_dosage = $form->exportValue('default_dosage');
            $default_dosage_unit = $form->exportValue('default_dosage_unit');
            $default_body_location = $form->exportValue('default_dosage_location');
            $for_mastitis = $form->exportValue('for_mastitis');
            // 
            $pageid = $form->exportValue('pageid');

            if ($for_mastitis == 1) {
                $for_mastitis = 'true';
            } else {
                $for_mastitis = 'false';
            }
            
            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            //add drug
            $query = "INSERT INTO bovinemanagement.medicine (trade_name,latin_name,official_instructions,local_instructions,medicine_class,milk_withholding,beef_withholding,milk_post_calving_withholding,default_dosage,default_dosage_unit,default_body_location, for_mastitis)
	  VALUES ('$trade_name','$latin_name','$official_instructions','$local_instructions','$medicine_class','$milk_withholding','$beef_withholding','$milk_post_calving_withholding','$default_dosage','$default_dosage_unit','$default_body_location','$for_mastitis')";
            $res = $GLOBALS['pdo']->exec($query);

            // determine if the commit or rollback
    
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

}//end class
?>