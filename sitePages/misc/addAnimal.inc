<?php 
class AddAnimal extends BasePage{


public function defaultDisplay() {
    
    print("<h3>NOTE: This should only be used for animals bought from outside the farm or adding a herd to bovibook for the first time.XXX</h3>");
    print('<h3>NOTE2: Needs to be a CSV list of purebred animals, ie. "HOCANF11111111, HOCANM44444444, HOCANF32323232" </h3>');
    
    $milk=100*40/12*$milk;
    $cow=100*$milk;
    print("Milk Cow: $cow");
    
    
    
 $a = new QF2_AddAnimals((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');
        print($a->renderBox('QF2_AddAnimals', 'Add Animals'));
        //$this->displayValactaXMLList();
       
}

 public function displayValactaXMLList() {
        print("<h3>Uploaded Valacta XML Files</h3>");

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.valacta_xml
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {
            
            
        $rowOut['Date']=JQueryDataTable::formattedDate($row['event_time']);    
        $rowOut['File Name']=  $row['file_name'];
        $rowOut['Employee']=$row['userid'];
        
        //delete button
        $callBackURL= "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
        $a=new QF2_DeleteButtonGeneric((object) array('callBackURL' =>$callBackURL),'individual',(object) array('deleteID'=>$row['id'],'schemaTable'=>'documents.valacta_xml'));
        $rowOut['Action']=($a->renderDelete($estrus_id));  
                         
                 $output[] = $rowOut;
        }
        print( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $output, 'searching: true,paging: false,ordering: false,info: true'));
    }


    
} //end class
Class QF2_AddAnimals extends QF2_Base {

    function definition($form) {

      
        
        
        
        $fieldset = $form->addFieldset('header')->setLabel('CSV list of animals to add');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form->addElement('textarea', 'animals', array('style' => 'width: 100%;', 'cols' => 50, 'rows' => 7), array('label' => 'Test Textarea:'))->setLabel('Animals to add:'); 
     
         
 

        return $form; //required       
    }

    function process($value) {
        $value['animals'];
        
        
        $areaRawStr = trim($areaRawStr); // trim
        // initial sanity check. see if it is numbers.
        $sanityTest = str_replace(' ', '|', $areaRawStr);
        $sanityTest = str_replace(',', '|', $sanityTest);
        
        
        
        
        $value['event_time'] = date('Y-m-d 9:00:00', $value['event_time']);

        //don't put id value, uses next one in DB.
        $query = "INSERT INTO XXX (userid) VALUES ('{$GLOBALS['auth']->getUsername()}')";

        $res = $GLOBALS['pdo']->exec($query);
    }

   

}

//end class
?>
