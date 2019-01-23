<?php

/**
 * 
 * 
 */
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickForm2RendererBootStrapV3.inc';


class Quickform2Helper {

    public $mode;
    public $forwardURLQuery;
    public $param;

    /**
     * 
     * @param object option array $forwardURLArr URL to forward to after form completes.
     * @param type $mode individual or group
     * @param object $param parameters to pass on, ie bovine_id or breeding_id, etc. stdClass
     * @throws Exception
     * 
     */
    public function __construct(object $options, $mode = 'group', object $param=null) {
   
        /*
         * use either forwardURLArr params or full callBackURL
         * 
         */
        

        //check ok
     if (isset($options)) {
         if ($options instanceof stdClass) {
            $this->options = $options;
        } else {
            throw new Exception("Error QF2 Options is not object of type stdClass.");
     }}else {
           throw new Exception("Error QF2 Options is not object of type stdClass."); 
     }
        
        if (!empty($this->options->callBackURL)) {
         //deal with at bottom in header.   
        } 
        elseif (is_array($this->options->forwardURLArr)) {
            $this->options->forwardURLArr = http_build_query($this->options->forwardURLArr);
        } else {
            throw new Exception("Error: QF2 forwardURLArr must be an array OR must provided a callbackURL.");
        }

        if ($mode == ('group'OR'individual')) {
            $this->mode = $mode; //individual or group, can then be used in other methods.
        } else {
            throw new Exception("Error: QF2 mode must be group or individual.");
        }
        
        if (isset($param)) {
         if ($param instanceof stdClass) {
            $this->param = $param;
        } else {
            throw new Exception("Error QF2 Param is not object of type stdClass.");
        }
    }}

    /*
     * main form function, not to be confused with HTML_QuickForm2 render
     */
    function render($formid) {
        if (empty($formid)) {
            throw new Exception("Error render needs an internal name for form. set it: render(name).");
        }
     //'htts HTTP_HOST  REQUEST_URI
        $form = new HTML_QuickForm2($formid, 'post', array('action' => $_SERVER['REQUEST_URI']),true);


        $form = $this->definition($form);
        $form = $this->customSubmit($form);

        // Apply the trim() function recursively on all form children elements
        $form->addRecursiveFilter('trim');

        if ($form->validate()) {
            $value = $form->getValue(); //get values to array.

            $this->process($value);

            if (!empty($this->options->callBackURL)) {
                
                header($this->options->callBackURL);
            } else {
                header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?" . $this->options->forwardURLArr);
            }
            exit();
        } else {
            //display form
            $quickForm2_Renderer_BootStrapV3 = new QuickForm2_Renderer_BootStrapV3();
            //$quickForm2_Renderer_BootStrapV3->renderer()->getJavascriptBuilder()->getLibraries(true, true); //maybe need echoed see http://pear.php.net/manual/en/package.html.html-quickform2.javascript.php
            return $form->render($quickForm2_Renderer_BootStrapV3->renderer());
         
         
             // $renderer2 = HTML_QuickForm2_Renderer::factory('callback');
         /*   
          
    ->setOption(array(
        'group_hiddens' => true,
        'group_errors'  => true,
        'required_note' => '<strong>Note:</strong> Required fields are marked with an asterisk (<em>*</em>).'
    ))
    ->setTemplateForId('submit', '<div class="element">{element} or <a href="/">Cancel</a></div>')
    ->setTemplateForClass(
        'HTML_QuickForm2_Element_Input',
        '<div class="btn btn-default<qf:error> error</qf:error>"><qf:error>{error}</qf:error>' .
        '<label for="{id}" class="qf-label<qf:required> required</qf:required>">{label}</label>' .
        '{element}' .
        '<qf:label_2><div class="qf-label-1">{label_2}</div></qf:label_2></div>'
    );
         echo $form->render($renderer2);   
      */   
          /*
        $renderer2->setCallbackForClass('HTML_QuickForm2_Element', function($renderer, $element) {
            print($renderer->renderLabel($element));
            //exit();
    $error = $element->getError();
    if ($error) {
        $html[] = '<div class="form-group error">';
        $element->addClass('form-control');
        $renderer->errors[] = $error;
    } else {
        $html[] = '<div class="form-group">';
    }
    //remove  </p>
    $temp=$renderer->renderLabel($element);
    $html[] = str_replace('</p>',"",str_replace('<p class="label">',"",$temp));
    
    
    $html[] = '<div class="input">';
    if ($element->toggleFrozen()) {
        $html[] = '<span class="uneditable-input">';
    }
    $html[] = $element;
    if ($error) {
        $html[] = '<span class="help-inline">'.$error.'</span>';
    } else {
        $label = $element->getLabel();
        if (is_array($label) && !empty($label[1])) {
            $html[] = '<span class="help-block">'.$label[1].'</span>';
        }
    }
    if ($element->toggleFrozen()) {
        $html[] = '</span>';
    }
    $html[] = '</div>';
    $html[] = '</div>';
    return implode('', $html);
});      
            */  
       /*       
        $renderer2
        ->setTemplateForClass(
        'HTML_QuickForm2_Element',
        '<div type="text" placeholder="Default input" class="form-control <qf:error> error</qf:error>"><qf:error>{error}</qf:error>' .
        '<label for="{id}" class="qf-label<qf:required> required</qf:required>">{label}</label>' .
        '{element}' .
        '<qf:label_2><div class="qf-label-1">{label_2}</div></qf:label_2></div>'
    );
        */
         // echo $form->render($renderer2);      
            
        }
    }
    
    
    /*
     * 
     * used as single button for delete. 
     * callbackURL only
     */
        function renderDelete($formidSeed,$buttonValue='Delete') {
       
            //if we have many deletes on the page we need each to have a unique name.
            $formid= 'QF2_Del_'.md5('give unique name to the form'.$formidSeed);
  
        $form = new HTML_QuickForm2($formid, 'post', array('action' => $_SERVER['REQUEST_URI']),true);


        $form = $this->definition($form);
        $form->addSubmit('submit', array('class' => 'glyphicon glyphicon-asterisk' ,'value' => $buttonValue));

        // Apply the trim() function recursively on all form children elements
        $form->addRecursiveFilter('trim');

        if ($form->validate()) {
            $value = $form->getValue(); //get values to array.

            $this->process($value);

            if (!empty($this->options->callBackURL)) {              
                header($this->options->callBackURL);
            } else {
                throw new Exception("Error: callBackURL required for delete button.");
            }
            exit();
        } else {
            //display form
            $quickForm2_Renderer_BootStrapV3 = new QuickForm2_Renderer_BootStrapV3();
            //$quickForm2_Renderer_BootStrapV3->renderer()->getJavascriptBuilder()->getLibraries(true, true); //maybe need echoed see http://pear.php.net/manual/en/package.html.html-quickform2.javascript.php
            return (string) $form->render($quickForm2_Renderer_BootStrapV3->renderer());
        }}
    

    //EXTEND: //input arrays and addelements, etc. !!!!no submit button, use custom.
    public function definition($form) {
        
    }


    
    //EXTEND: //sql insert into db code.
    public function process($value) {
        
    }

    //overload this with your own submit if necessary.
    public function customSubmit($form) {
        // Add the submit button to our form (outside of our fieldset)
        $form->addSubmit('reset', array('value' => "Clear"));
        $form->addSubmit('submit', array('value' => "Submit"));
        return $form;
    }

    function renderBox($formid,$header) {
$html = <<<HTML
   <div class="box box-default">
            <div class="box-header with-border">
              <h3 class="box-title">$header</h3>
         </div>
            <!-- /.box-header -->
        <div class="box-body">
{$this->render($formid)}
        </div>
        </div>
            <!-- /.box-body -->
HTML;

        return $html;
    }

 /*
  * EXTRA HELPER FUNCTION FOR FORMS. 
  * 
  */ 
    
    /*
     * used for select form to include groups+animals
     */
    function currentAniamlsAndGroups() {
        //animals
        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveCows();

        //find locations with animals in it, we will call a group.
        $sql = "SELECT DISTINCT ON (location_id) location_id,location.name,COUNT(*) as count from bovinemanagement.bovinecurrall 
LEFT JOIN bovinemanagement.location ON bovinecurrall.location_id=location.id
GROUP BY location_id  ,location.name";
        $res = $GLOBALS['pdo']->query($sql);
        $groupArr = array();
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $groupArr['GROUP_' . $row['location_id']] = 'GROUP: ' . $row['name'] . ' ' . '(' . $row['count'] . ')';
        }
        $combinedArr = ($aniNumber + $groupArr);
        return ($combinedArr);
    }
    
    /*
     * used to process select form that includes group+animals.
     */
    function processAnimalsGroupIndividiual($animalGroupList) {

        $outArr = array();

        if (is_array($animalGroupList)) {
            //group
            foreach ($animalGroupList as $k => $v) {


                // GROUP (LOCATION) MOVE
                if (strstr($v, 'GROUP_') != false) { //when group or pen of animals
                    //remove the 'GROUP_' to find location_id.
                    $integerLocationID = preg_replace('/^GROUP_/', '', $v);
                    if (filter_var($integerLocationID, FILTER_VALIDATE_INT)) {
                        $locationID = $integerLocationID;
                    } else {
                        throw new Exception("Error: bad data, location id not an integer");
                    }

                    //now look up which cows are in that location and we will loop through them. this is inside a transaction, so by doing this here we make it atomic. 
                    $sql2 = "SELECT bovine_id From bovinemanagement.locationcurr WHERE location_id=$integerLocationID";
                    $res2 = $GLOBALS['pdo']->query($sql2);
                    while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {

                        $outArr[] = $row2['bovine_id'];
                    }
                    //NORMAL BUNCH OF ANIMALS    
                } else {

                    $outArr[] = $v;
                }
            }
        } else {
            $outArr[] = $animalGroupList; //single
        }
        return $outArr;
    }

}//end class



/*
 * this is great for deleting simple tables where 'id' describes what you want to delete. 
 * 
 * 
 * examplec call:
 * $a=new QF2_GenericDeleteButton((object) array('callBackURL' =>$callBackURL),'individual',(object) array('deleteID'=>$estrus_id,'schemaTable'=>'bovinemanagement.estrus_event'));
             $deleteButtonAsString=($a->renderDelete($estrus_id));    
 */       
Class QF2_GenericDeleteButton extends Quickform2Helper {

    
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

        /*
             * This is a bit hackish, but as long as code is correct it should be fine. Not efficent for a prepared statement. 
             * 
             */
            $stmt = $GLOBALS['pdo']->prepare("DELETE FROM {$this->param->schemaTable} where {$this->param->deleteName}=:deleteID");
            $stmt->bindParam(':deleteID', $this->param->deleteID);
            $stmt->execute();
       
       
    }

}//end class
?>