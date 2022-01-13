<?php
/*
 * used to store a tempoary field list. 
 */
class FieldList extends BasePage {

    public function defaultDisplay() {
      
         print("<div class='row'>");
     
        print("<div class='col-sm-4 col-sm-push-8'>");
         $a=new QF2_FieldTempEntry((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('QF2_FieldTempEntry','Select Fields to Add to Custom Field List'));          
        print($a->formJS());
        
        $b=new QF2_DeleteTable((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($b->renderBox('QF2_DeleteTable','Delete Table'));          
        
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
        //table
            print($this->displayTempFieldList()); 
        
        print("</div>");
        
       print("</div>");
       
    }
    
    
    function displayTempFieldList() {
       
 $sql=<<<SQL
SELECT field_id,alpha_numeric_id,common_name,cropping.field_border_area_at_timestamp(field_id, CURRENT_TIMESTAMP::timestamp ) as "area" , comment
    FROM system.field_list_temp  
        LEFT JOIN cropping.field ON field.id=field_list_temp.field_id 
            ORDER BY cropping.field_short_name(field_id)
SQL;
 

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {
            
             $rowOut['alpha_numeric_id'] ="<a href=\"". $GLOBALS['MiscObj']->hrefToFieldPage($row['field_id'])."\"> <b>{$row['alpha_numeric_id']}</b> - ". $row['common_name'] ."</a>";   
             $rowOut['Area (ha)']= $row['area'];
             $rowOut['Comment']= $row['comment'];
             $rowOut['action']= (new JQueryDataTable())->actionDeleteButton($row['field_id'], 'system.field_list_temp', 'field_id',null,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}", null,true);
             $resultsOut[]=$rowOut;            
        }

      //  var_dump($resultsOut);
        
        //output
        $dataArr = $resultsOut;

         $out[]=( (new JQueryDataTable)->startBasicArrayNoSort('Field Temp List', $dataArr)); 
         return implode($out); 
       
    }
    
}//end class   


Class QF2_DeleteTable extends QF2_Base {

    
    function definition($form) {

 
        return $form; //required       
    }

    function process($value) {
             
         // empty table.
$sql = <<<SQL
Truncate system.field_list_temp
SQL;
                $res = $GLOBALS ['pdo']->exec($sql);
        
            
    }
}//end class

/*
 * allows entering many fields at once. 
 * 
 */
Class QF2_FieldTempEntry extends QF2_Base {

    function definition($form) {
 
        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        $combinedArr=Quickform2_GroupSelect::currentFieldsAndMajorFields(); 
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('field', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($combinedArr);
        } else {         
                $form->addElement('hidden', 'field')->setValue( $this->param->field_id);
        }
        
        
       $form->addElement('text', 'customComment', null, array('label' => 'Comment'));
         
  return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Fields",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

           $comment= ($value['customComment']); //FIXME: really should be PDO insert, but just for me. 

          // var_dump($value);
          // exit();

            if (is_array($value['field'])) {
                //group
                foreach ($value['field'] as $k => $v) {


                    // GROUP (LOCATION) MOVE
                    if (strstr($v, 'GROUP_') != false) { //when group or pen of animals
                        //remove the 'GROUP_' to find location_id.
                        $majorFieldID = preg_replace('/^GROUP_/', '', $v);
                        if (filter_var($majorFieldID, FILTER_SANITIZE_STRING)) {
                        } else {
                            throw new Exception("Error: bad data, major field id not a string, it is instead this: $majorFieldID");
                        }
                        $majorFieldID=filter_var($majorFieldID, FILTER_SANITIZE_STRING);
                        //now look up which fields are in the major field and we will loop through them. this is inside a transaction, so by doing this here we make it atomic. 
                        $sql2 = "SELECT id as field_id From cropping.fieldcurr WHERE major_name='$majorFieldID'";
                        $res2 = $GLOBALS['pdo']->query($sql2);
                        while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {

                             $this->insert($row2['field_id'],$value,$comment,$transaction_id);
                          
                        }
                        //NORMAL BUNCH OF Fields    
                    } else {
                        //now we insert all the fields that were checked.
                        $field_id = $v;
                        $this->insert($field_id,$value,$comment,$transaction_id);
                    }
                }
            }
            //individual case
            else {
              $this->insert($value['field_id'],$value,$comment,$transaction_id);
            }


            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

    
function insert($field_id,$value,$comment,$transaction_id) {
    
            if (empty($transaction_id)) {
                $transaction_id='null';
            }
            
         $userid = $GLOBALS['auth']->getUsername();//grab userid
               $type='';
                
 // do an insert into cropping field 
$sql = <<<SQL
INSERT INTO system.field_list_temp (field_id,type,comment,userid) VALUES  ($field_id,'$type','$comment','$userid')
SQL;
                $res = $GLOBALS ['pdo']->exec($sql);

           
            
    
}    
    
    

}//end class   




