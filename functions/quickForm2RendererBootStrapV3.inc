<?php
/**
 * 
 * //this is heavily based off of:http://mansion.im/tests/quickform2-bootstrap.php?step=2
 * free to use AFAIK.
 * feb 2018.
 * 
 * just initate and use as renderer for quickform2 to output a more bootstrap friendly renderer.
 */
class QuickForm2_Renderer_BootStrapV3 {
    
    /*
     * see http://pear.php.net/manual/en/package.html.html-quickform2.renderers.default.php for templates replaced. 
     * 
     */
    
    /**
     *  parent HTML_QuickForm2_Element OR 
     *  parent HTML_QuickForm2_Container
     
         //list of elements
        'button'       HTML_QuickForm2_Element_Button
        'checkbox'     HTML_QuickForm2_Element_InputCheckbox
        'date'         HTML_QuickForm2_Element_Date
        'fieldset'     HTML_QuickForm2_Container_Fieldset
        'group'        HTML_QuickForm2_Container_Group
        'file'         HTML_QuickForm2_Element_InputFile
        'hidden'       HTML_QuickForm2_Element_InputHidden
        'hierselect'   HTML_QuickForm2_Element_Hierselect
        'image'        HTML_QuickForm2_Element_InputImage
        'inputbutton'  HTML_QuickForm2_Element_InputButton
        'password'     HTML_QuickForm2_Element_InputPassword
        'radio'        HTML_QuickForm2_Element_InputRadio
        'repeat'       HTML_QuickForm2_Container_Repeat
        'reset'        HTML_QuickForm2_Element_InputReset
        'script'       HTML_QuickForm2_Element_Script
        'select'       HTML_QuickForm2_Element_Select
        'static'       HTML_QuickForm2_Element_Static
        'submit'       HTML_QuickForm2_Element_InputSubmit
        'text'         HTML_QuickForm2_Element_InputText
        'textarea'     HTML_QuickForm2_Element_Textarea
     
     */

function renderer() {
  $renderer = HTML_QuickForm2_Renderer::factory('callback');
   
  
  
  //LITTLE RIVER
  //select object
  $renderer->setCallbackForClass('HTML_QuickForm2_Element_Select', function($renderer, $element) {
      $error = $element->getError();
      $errorSpan='';
            if ($error) {
                 $html[]='<div class="form-group form-inline has-error">';            
                $errorSpan= "<span class='help-block'>$error</span>";
            } else {
                 $html[]='<div class="form-group form-inline">';
            }   
            $element->addClass('form-control');          
        $html[] ='<label for="'.$element->getId().'">'.$element->getLabel().'</label>';
        $html[] = $element;   
        $html[]=$errorSpan.'</div>';
          return implode('', $html);
  });
  
    //LITTLE RIVER
  //text object AND ALL OTHER INPUTS.
  $renderer->setCallbackForClass('HTML_QuickForm2_Element_Input', function($renderer, $element) {
      $error = $element->getError();
      $errorSpan='';
            if ($error) {
                 $html[]='<div class="form-group form-inline has-error">';            
                $errorSpan= "<span class='help-block'>$error</span>";
            } else {
                 $html[]='<div class="form-group form-inline">';
            }   
            $element->addClass('form-control');          
        $html[] ='<label for="'.$element->getId().'">'.$element->getLabel().'</label>';
        $html[] = $element;   
        $html[]=$errorSpan.'</div>';
          return implode('', $html);
  });
  
  
  //LITTLE RIVER
  //submit object 
  //note these have to stay inputs AFAIK and can't be changed to buttons.
   $renderer->setCallbackForClass('HTML_QuickForm2_Element_InputSubmit', function($renderer, $element) {
       $element->addClass('btn btn-default');
       $html[]=$element;
          return implode('', $html);
  });
  
  $renderer->setCallbackForClass('HTML_QuickForm2_Element_InputCheckbox', function($renderer, $element) {
   $error = $element->getError();
      $errorSpan='';
            if ($error) {
                 $html[]='<div class="form-group form-inline has-error">';            
                $errorSpan= "<span class='help-block'>$error</span>";
            } else {
                 $html[]='<div class="form-group form-inline">';
            }   
            //$element->addClass('form-control');          
        $html[] ='<label for="">'.$element->getLabel().'</label>';
        $html[] = $element;   
        $html[]=$errorSpan.'</div>';
          return implode('', $html);
});
  
  /*
  
       // Global callback for all the elements

$renderer->setCallbackForClass('HTML_QuickForm2_Element', function($renderer, $element) {
   //print_r2($element);
    $error = $element->getError();
    if ($error) {
        $html[] = '<div class="form-group has-error">';
        $renderer->errors[] = $error;
    } else {
        $html[] = '<div class="form-group">';
    }
     //remove  <p></p> //
    $temp=$renderer->renderLabel($element);
    $html[] = str_replace('</p>',"",str_replace('<p class="label">',"",$temp)); //removing <p></p> tags, not necessary with bootstrap.
    //                 //
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

// Callback for the error message

$renderer->setErrorGroupCallback(function($renderer, $form) {
    $html = array();
    if (!empty($renderer->errors)) {
        $html[] = '<div class="alert-message has-error">';
        $html[] = '<a class="close" href="#">x</a>';
        $html[] = '<p>' . $renderer->getOption('errors_prefix') . '</p>';
        $html[] = '</div>';
    }
    return implode("", $html);
});

// Callback for groups of checkboxes and radios

$renderer->setCallbackForClass('InputList', function($renderer, $group) {
    $error = $group->getError();
    if ($error) {
        $html[] = '<div class="form-group">';
        $renderer->errors[] = $error;
    } else {
        $html[] = '<div class="form-group">';
    }
    $html[] = $renderer->renderLabel($group);
    $html[] = '<div class="input">';
    $html[] = '<ul class="input-group"><li>';
    $elements  = array_pop($renderer->html);
    $html[] = implode('</li><li>', $elements);
    $html[] = '</li></ul>';
    if ($error) {
        $html[] = '<span class="help-inline">'.$error.'</span>';
    } else {
        $label = $group->getLabel();
        if (is_array($label) && !empty($label[1])) {
            $html[] = '<span class="help-block">'.$label[1].'</span>';
        }
    }
    $html[] = '</div>';
    $html[] = '</div>';
    return implode('', $html);
});

// Callback for checkboxes and radios inside a group

$renderer->setElementCallbackForGroupClass('InputList',
    'HTML_QuickForm2_Element_InputCheckable', function($renderer, $element) {
    $html[] = '<label>';
    $html[] = '<input' . $element->getAttributes(true) . ' /> ';
    $html[] = $element->getContent();
    $html[] = '</label>';
    return implode('', $html);
});

// Callback for groups of inline inputs

$renderer->setCallbackForClass('InlineInputs', function($renderer, $group) {
    $error = $group->getError();
    if ($error) {
        $html[] = '<div class="form-group has-error">';
        $renderer->errors[] = $error;
    } else {
        $html[] = '<div class="form-group">';
    }
    $html[] = $renderer->renderLabel($group);
    $html[] = '<div class="input">';
    $html[] = '<div class="input-group">';

    $separator = $group->getSeparator();
    $elements  = array_pop($renderer->html);
    if (!is_array($separator)) {
        $content = implode((string)$separator, $elements);
    } else {
        $content    = '';
        $cSeparator = count($separator);
        for ($i = 0, $count = count($elements); $i < $count; $i++) {
            $content .= (0 == $i? '': $separator[($i - 1) % $cSeparator]) .
                        $elements[$i];
        }
    }
    $html[] = $content;

    if ($error) {
        $html[] = '<span class="help-inline">'.$error.'</span>';
    } else {
        $label = $group->getLabel();
        if (is_array($label) && !empty($label[1])) {
            $html[] = '<span class="help-block">'.$label[1].'</span>';
        }
    }
    $html[] = '</div>';
    $html[] = '</div>';
    $html[] = '</div>';
    return implode('', $html);
});

// Callback for elements inside an inline inputs group

function renderInlineElement($renderer, $element) {
    return (string)$element;
}

$renderer->setElementCallbackForGroupClass('InlineInputs',
    'HTML_QuickForm2_Element', 'renderInlineElement');

// Callback for action inputs

$renderer->setCallbackForClass('ActionInputs', function($renderer, $group) {
    $html[] = '<div class="actions">';
    $elements = array_pop($renderer->html);
    $html[] = implode(' ', $elements);
    $html[] = '</div>';
    return implode('', $html);
});

// Callback for elements inside an action inputs group
// Note : same as InlineInputs

$renderer->setElementCallbackForGroupClass('ActionInputs',
    'HTML_QuickForm2_Element', 'renderInlineElement');

*/
// Set some options

$renderer->setOption(array(
    'group_hiddens' => true,
    'group_errors'  => false,
    'required_note' => '<div class="note"><strong>Note:</strong> <span class="required">✱</span> denotes required fields</div>',
    'errors_prefix' => '<strong>Alert !</strong> There are errors in your form.',
    'errors_suffix' => '',
));
       
      return $renderer; 
    }

}//end class


/*
 * 
 * 
 * 
 */
/*


print("<h1>Hello</h1>");
$form = new HTML_QuickForm2('uploads');
$fieldset = $form->addElement('fieldset');
$name = $fieldset->addElement('text', 'name', array('size' => 50, 'maxlength' => 255))->setLabel('Enter your name:');
$fieldset->addElement('submit', null, array('value' => 'Send!'));
$name->addFilter('trim');
$name->addRule('required', 'Please enter your name', null, HTML_QuickForm2_Rule::CLIENT_SERVER);
if ($form->validate()) {
echo '<h1>Hello, ' . htmlspecialchars($name->getValue()) . '!</h1>';
exit;
}
//$renderer = HTML_QuickForm2_Renderer::factory('default');

$quickForm2_Renderer_BootStrapV3 = new QuickForm2_Renderer_BootStrapV3();
echo $form->render($quickForm2_Renderer_BootStrapV3->renderer());

*/


?>