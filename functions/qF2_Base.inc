<?php

/**
 * 
 * 
 */
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickForm2RendererBootStrapV3.inc';


abstract class QF2_Base {

    public $mode;
    public $forwardURLQuery;
    public $param;
    public $uuid;

    /**
     * 
     * @param object option array $forwardURLArr URL to forward to after form completes.
     * @param type $mode individual or group
     * @param object $param parameters to pass on, ie bovine_id or breeding_id, etc. stdClass
     * @throws Exception
     * 
     */
    public function __construct(object $options, $mode = 'group', object $param = null) {

        /*
         * use either forwardURLArr params or full callBackURL
         * 
         */

        //create a uuid
        $this->uuid = $GLOBALS['MiscObj']->uuid_create();

        //check ok
        if (isset($options)) {
            if ($options instanceof stdClass) {
                $this->options = $options;
            } else {
                throw new Exception("Error QF2 Options is not object of type stdClass.");
            }
        } else {
            throw new Exception("Error QF2 Options is not object of type stdClass.");
        }

        if (!empty($this->options->callBackURL)) {
            //deal with at bottom in header.   
        } elseif (is_array($this->options->forwardURLArr)) {
            $this->options->forwardURLArr = http_build_query($this->options->forwardURLArr);
        } else {
            throw new Exception("Error: QF2 forwardURLArr must be an array OR must provided a callbackURL.");
        }

        if ($mode == ('group'OR'individual'OR'list')) {
            $this->mode = $mode; //individual or group, can then be used in other methods.
        } else {
            throw new Exception("Error: QF2 mode must be group or individual or list.");
        }

        if (isset($param)) {
            if ($param instanceof stdClass) {
                $this->param = $param;
            } else {
                throw new Exception("Error QF2 Param is not object of type stdClass.");
            }
        }
    }

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
       
           //throw new Exception("0000000");
            if (!empty($this->options->callBackURL)) {    
            
                //print('CALLBACK URL');
                header($this->options->callBackURL);
            } else {
                // print('FORWARD URL');
                // print("Location: {$GLOBALS['config']['HTTP']['URL']}?" . $this->options->forwardURLArr);
                header("Location: {$GLOBALS['config']['HTTP']['URL']}?" . $this->options->forwardURLArr);
            }
            exit();
        } else {
            //display form
            $quickForm2_Renderer_BootStrapV3 = new QuickForm2_Renderer_BootStrapV3();
            
            //$quickForm2_Renderer_BootStrapV3->renderer()->getJavascriptBuilder()->getLibraries(true, true); //maybe need echoed see http://pear.php.net/manual/en/package.html.html-quickform2.javascript.php
            //Couldn't get the fancy javascript caller to work, so just call manually on all pages. not efficent. 
            $jsStr[]= '<script type="text/javascript" src="/vendor/pear/html_quickform2/data/js/min/quickform.js"></script>';
            $jsStr[]= '<script type="text/javascript" src="/vendor/pear/html_quickform2/data/js/min/quickform-hierselect.js"></script>';
            
            return implode($jsStr).$form->render($quickForm2_Renderer_BootStrapV3->renderer());
         
         
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
    $html[] = str_replace('</p>',"",str_replace('<p class="badge">',"",$temp));
    
    
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
     * single buttons must display:inline
     * callbackURL only
     */
        function renderDelete($formidSeed,$buttonValue='<b>Delete</b>',$icon=null) {
       
          $icon= empty($icon) ? "<i class='fa fa-fw fa-trash'></i>" : $icon; //force it default if it arrives empty.
            
            
            //if we have many deletes on the page we need each to have a unique name.
            $formid= 'QF2_Del_'.md5('give unique name to the form'.$formidSeed);
  
        $form = new HTML_QuickForm2($formid, 'post', array('action' => $_SERVER['REQUEST_URI']),true);
        
        //because this is a single button, add a class to later style. 
        $form->removeAttribute('name');
        $form->setAttribute('class','QF2_InlineButton');
        
        $form = $this->definition($form);
        $form->addElement('button', 'aButton', array('class' => 'btn btn-danger btn-sm' ,'type' => 'submit', 'id' => 'buttonId')     )->setContent($icon) . "\n";
     
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
    
        
         function renderComplete($formidSeed,$buttonValue='<b>Complete</b>',$icon=null) {
             
              $icon= empty($icon) ? "<i class='far fa-check-circle'></i>" : $icon; //force it default if it arrives empty.
         
       
            //if we have many deletes on the page we need each to have a unique name.
            $formid= 'QF2_Comp_'.md5('give unique name to the form'.$formidSeed);
  
        $form = new HTML_QuickForm2($formid, 'post', array('action' => $_SERVER['REQUEST_URI']),true);

        //because this is a single button, add a class to later style. 
        $form->removeAttribute('name');
        $form->setAttribute('class','QF2_InlineButton');
        
        $form = $this->definition($form);
        $form->addElement('button', 'aButton', array('class' => 'btn btn-success btn-sm' ,'type' => 'submit', 'id' => 'buttonId')     )->setContent($icon) . "\n";
     
        // Apply the trim() function recursively on all form children elements
        $form->addRecursiveFilter('trim');

        if ($form->validate()) {
            $value = $form->getValue(); //get values to array.

            $this->process($value);

            if (!empty($this->options->callBackURL)) {              
                header($this->options->callBackURL);
            } else {
                throw new Exception("Error: callBackURL required for complete button.");
            }
            exit();
        } else {
            //display form
            $quickForm2_Renderer_BootStrapV3 = new QuickForm2_Renderer_BootStrapV3();
            $quickForm2_Renderer_BootStrapV3->renderer()->getJavascriptBuilder()->getLibraries(true, true); //maybe need echoed see http://pear.php.net/manual/en/package.html.html-quickform2.javascript.php
            return (string) $form->render($quickForm2_Renderer_BootStrapV3->renderer());
        }}
        

    //EXTEND: //input arrays and addelements, etc. !!!!no submit button, use custom.
    abstract public function definition($form);


    
    //EXTEND: //sql insert into db code.
    abstract public function process($value);
        
    

    //overload this with your own submit if necessary.
    public function customSubmit($form) {
        // Add the submit button to our form (outside of our fieldset)
        $form->addReset('reset', array('value' => "Clear"));
        $form->addSubmit('submit', array('value' => "Submit"));
        return $form;
    }

    function renderBox($formid,$header) {
        return    (new BootStrap)->plainCardPrimarySolid($header, $this->render($formid));
    }

 

}//end class

class Quickform2_GroupSelect {
    
     private $uuid;
     private $bovine_id;
    
     /*
      * set a bovine id to individual, defaults to group. 
      */
    public function __construct($bovine_id=null) {
        $this->uuid = uniqid(); 
        $this->bovine_id=$bovine_id;
}
    


/*
 * this covers all three cases of animal select:
 * individual bovine where there is no animal entry
 * list where we can select only one bovine from long list
 * group where we can select multiple bovines. 
 */
//call like:  $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 
public function definitionAnimalAll($form,$aniNumberIn=null,$mode=null) {
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        //bovine id supplied implies individual mode. 
        if (!(empty($this->bovine_id))) {  $mode='individual'; }
        //so mode should be defined from now on.
        if ((empty($mode))) { throw new Exception('Mode Not selected, likely called incorrectly.'); }
        
        switch ($mode) {
    case 'individual':
         $form->addElement('hidden', 'animalSelected')->setValue($this->bovine_id);
        break;
    case 'list':
         $aniNumber = empty($aniNumberIn) ? $GLOBALS['MiscObj']->createListOfAllAliveBovines() : $aniNumberIn;
         $form->addSelect('animalSelected', array('id' => 'selectEvents_'.$this->uuid))->loadOptions($aniNumber)->addRule("required", "Animal Required");
        break;
    case 'group':
         $combinedArr=$this->currentAnimalsAndGroups($aniNumberIn);    
         $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents_'.$this->uuid))->loadOptions($combinedArr)->addRule("required", "Animal Required");
         $form->addElement('static', 'script_definitionAnimal')->setValue($this->groupsJS());
        break;
    default:
        throw new Exception('Mode Not known, likely spelled incorrectly.');
        break;
   
}
        
         return $form;
      
}

     
/*
  * EXTRA HELPER FUNCTION FOR FORMS. 
  * 
  */ 
    //call like this or add animalNumber to use custom list. 
    /*
       if ($this->mode == 'group') {
            $form=(new Quickform2_GroupSelect())->definitionAnimalGroup($form);
        }
        else {
            $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form);
        }
     * 
     */
//aniNumber ovverrides default all alive animal list
    public function definitionAnimalGroup($form,$aniNumber=null) {
        
        $combinedArr=$this->currentAnimalsAndGroups($aniNumber); 
      
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if (empty($this->bovine_id)) { //group
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents_'.$this->uuid))->loadOptions($combinedArr)->addRule("required", "Animal Required");
        } else {                       //individual
                $form->addElement('hidden', 'animalSelected')->setValue($this->bovine_id);
        }
        $form->addElement('static', 'script_definitionAnimal')->setValue($this->groupsJS());
        
        return $form;
}


/*
 * Used for the case where we don't want group select. We just allow individual animal or individual animal from a list of animals. 
 * 
 */
 public function definitionAnimalBasic($form,$aniNumberIn=null) {
        
     //custom or default list of animals. 
        if (!empty($aniNumberIn)) {
            $aniNumber=$aniNumberIn;
        }else {
              //animals
    
        }
     
       
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if (empty($this->bovine_id)) { //group
          
            $form->addSelect('animalSelected', array('id' => 'selectEvents_'.$this->uuid))->loadOptions($aniNumber)->addRule("required", "Animal Required");
        } else {                       //individual
                $form->addElement('hidden', 'animalSelected')->setValue($this->bovine_id);
        }
        
        return $form;
}
    
    
    public function processAnimal($form) {
        
    }

    
    /*
     * used for select form to include groups+animals
     */
    private function currentAnimalsAndGroups($aniNumberIn) {
   
        //custom or default list of animals. 
        if (!empty($aniNumberIn)) {
            $aniNumber=$aniNumberIn;
        }else {
              //animals
        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();
        }

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
    
    
    
    //js needed for select to work....
    private function groupsJS() {
$jscript = <<<JSCRIPT
            
           
 <script>           
var s2 = $("#selectEvents_{$this->uuid}").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>
JSCRIPT;
    

    return $jscript;
    }
    
    
     /*
     * used for select form to include major fields (ie, all MG, all CL) + plus fields.
     */
    function currentFieldsAndMajorFields() {
        //animals
        $fields = (new CroppingHelper)->allActiveFieldsArray();

        //find locations with animals in it, we will call a group.
        $sql = "SELECT distinct (major_name) as major_name FROM cropping.fieldcurr";
        $res = $GLOBALS['pdo']->query($sql);
        $groupArr = array();
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $groupArr['GROUP_' . $row['major_name']] = 'GROUP: ' . $row['major_name'];
        }
        $combinedArr = ($fields + $groupArr);
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
       
} //end class


/*
 * 
 * Misc functions
 * 
 */
class Quickform2Misc {
    
    /*
     * generates a standard forward URL for the site. 
     * Questionable if this is stupid way to this, maybe we should just add it every time. 
     * (new Quickform2Trait)->forwardURLDefaults();
     */
    public function forwardURLDefaults() {
        
        $out=array();
        
     if (!empty($_REQUEST['pageid'])  && is_numeric($_REQUEST['pageid'])) {
        $out['pageid']= $_REQUEST['pageid'];
     }   
        
     if (!empty($_REQUEST['bovine_id'])  && is_numeric($_REQUEST['bovine_id'])) {
        $out['bovine_id']= $_REQUEST['bovine_id'];
     }
        
     
     if (!empty($_REQUEST['machine_id'])  && is_numeric($_REQUEST['machine_id'])) {
        $out['machine_id']= $_REQUEST['machine_id'];
     }
     
     if (!empty($_REQUEST['field_id'])  && is_numeric($_REQUEST['field_id'])) {
        $out['field_id']= $_REQUEST['field_id'];
     }
     
        return $out;
    }
    
    
    
    
} //end class