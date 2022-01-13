<?php

/*
 * this is great for deleting simple tables where 'id' describes what you want to complete. 
 * 
 * 
 * examplec call:
 * $a=new QF2_CompleteButtonGeneric((object) array('callBackURL' =>$callBackURL),'individual',(object) array('completeID'=>$estrus_id,'schemaTable'=>'bovinemanagement.estrus_event'));
             $completeButtonAsString=($a->renderComplete($estrus_id));    
 */       
abstract Class QF2_CompleteButtonGeneric extends QF2_Base {

    
    function definition($form) {

  $form->addElement('hidden', 'completeID')->setValue($this->param->completeID); //client could change this, we leave it here for debugging, but we don't actually use it. 

        return $form; //required       
    }

    function process($value) {
             
        
        if (empty($this->param->completeID)){
            throw new Exception("Error: param->completeID not provided!");
        }
        if (empty($this->param->schemaTable)){
            throw new Exception("Error: param->schemaTable not provided!");
        }
        
        if (empty($this->param->completeName)) {
            $this->param->completeName = 'id';
        }

       
       $this->action(); //main action of form after it is submitted. 
       
        //callback function to call after form is processed, this is used if 
            // there are more things to complete, for instance, in a complete form.
            if (!empty($this->param->formCompleteCallBack)) {
            call_user_func($this->param->formCompleteCallBack, $this->param);
            }
            
    }
    
   /*
    * seperates out the meat and potatoes of the DB insert/update code for a complete button, so a child class can implement it's own method for a custom complete button. 
    */
  abstract public function action();
    
    

}//end class
?>
