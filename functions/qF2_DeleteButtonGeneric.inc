<?php

/*
 * this is great for deleting simple tables where 'id' describes what you want to delete. 
 * 
 * 
 * examplec call:
 * $a=new QF2_DeleteButtonGeneric((object) array('callBackURL' =>$callBackURL),'individual',(object) array('deleteID'=>$estrus_id,'schemaTable'=>'bovinemanagement.estrus_event'));
             $deleteButtonAsString=($a->renderDelete($estrus_id));    
 */       
Class QF2_DeleteButtonGeneric extends QF2_Base {

    
    function definition($form) {

  $form->addElement('hidden', 'deleteID')->setValue($this->param->deleteID); //client could change this, we leave it here for debugging, but we don't actually use it. 

        return $form; //required       
    }

    function process($value) {
             
        
        if (empty($this->param->deleteID)){
            throw new Exception("Error: param->deleteID not provided!");
        }
        if (empty($this->param->schemaTable)){
            throw new Exception("Error: param->schemaTable not provided!");
        }
        
        if (empty($this->param->deleteName)) {
            $this->param->deleteName = 'id';
        }

       
       $this->action(); //main action of form after it is submitted. 
       
        //callback function to call after form is processed, this is used if 
            // there are more things to delete, for instance, in a delete form.
            if (!empty($this->param->formCompleteCallBack)) {
            call_user_func($this->param->formCompleteCallBack, $this->param);
            }
            
    }
    
   /*
    * seperates out the meat and potatoes of the DB insert/update code for a delete button, so a child class can implement it's own method for a custom delete button. 
    */
   public function action() {
        //$this-param should provide everything the form needs for paramaters.  
       
         /*
             * This is a bit hackish, but as long as code is correct it should be fine. Not efficent for a prepared statement. 
             * 
             */
        
        
        (DEBUG) ?: $GLOBALS['debug']->writeLine("DELETE FROM {$this->param->schemaTable} where {$this->param->deleteName}={$this->param->deleteID}");
        
            $stmt = $GLOBALS['pdo']->prepare("DELETE FROM {$this->param->schemaTable} where {$this->param->deleteName}=:deleteID");
            $stmt->bindParam(':deleteID', $this->param->deleteID);
            $stmt->execute();
            
    }
    
    

}//end class
?>