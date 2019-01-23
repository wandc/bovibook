<?php 

//tracks who we took DNA test from. 





;
require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');

/* uses criteria to make a list of which cows should change groups */

class DnaTest extends TabPage {

   

    public function defaultDisplay() {
  //setup for tabs.
        $tabArray[1]['name'] = 'Test Performed';
        $tabArray[2]['name'] = 'Test Needed';
       
         $this->renderTabs($tabArray);
    }

    
    function tab1() {
        print("<div class='row'>");
       
        print("<div class='col-sm-4 col-sm-push-8'>");      
		
        $a=new QF2_DNATest((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('QF2_DNATest','Mark Animal(s) That Have had DNA test taken'));  
        print($a->formJS());
                  
        print("</div>");
         print("<div class='col-sm-8 col-sm-pull-4'>");
      $this->displayAllDNATestEvents();
        print("</div>");
        print("</div>");     
    }
    
    function tab2() {
        print($this->displayAliveCowsWithNoDNATest());
        print($this->currentAnimalsWithNoGenoTestFromCDN());
    }
    
    /*
     * designed to be called via ajax.
     */

   /*
    * catchall in case one was missed, ie entered as tested and not really tested, etc.
    * 
    */
    function currentAnimalsWithNoGenoTestFromCDN() {
    
$sql = <<<SQL
    SELECT *
FROM bovinemanagement.bovinecurr 
LEFT JOIN batch.aggregate_view_curr  ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
where geno_test is null  
SQL;


 $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           //  $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.      
               $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               $rowOut['Location']=$row['location_name'];
                                      
          
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]='<h3>CDN Has No Genotest</h3>';
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
         return implode($out); 
    }
    
    
    
       function aliveCowsWithNoDNATestSubmitted() {

        $sql = "
SELECT 
bovinecurr.id as bovine_id,local_number,bovinecurr.full_name,bovinecurr.full_reg_number,location_name,prodoll,geno_test,(SELECT donotbreed FROM bovinemanagement.service_picks WHERE bovine_id=bovinecurr.id ORDER BY event_time DESC LIMIT 1)
FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.dnatest_event ON bovinecurr.id=bovine_id  
LEFT JOIN batch.aggregate_view_curr  ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
WHERE dnatest_type is null
ORDER BY location_name DESC,local_number DESC ";
        
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
           $row['donotbreed']=($row['donotbreed'] ? $row['donotbreed'] : '');        
        }

        //output
        $output['data'] = $results;
        return ($output);
     
    }
    
     function displayAliveCowsWithNoDNATest() {
        
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
                          
            {
                data: "local_number",
                title: "Animal",
                 type: "num-html"
            },
        {
                data: "location_name",
                title: "Location"
            },
        
        {
                data: "prodoll",
                title: "Pro$"
            },
        {
                data: "geno_test",
                title: "CDN Geno Test"
            },
        {
                data: "donotbreed",
                title: "donotbreed"
            }
        
        
            
EX;
    

         $out[]='<h3>Test Needed to be Taken</h3>';
         $out[]=($dt->start(126,'aliveCowsWithNoDNATestSubmitted',null,$columnsStr)); 
         return implode($out);     
}     
    
    
    
    function allDNATestsEvents() {

        $sql = "SELECT 
dnatest_event.id,bovine.id as bovine_id,bovine.local_number,bovine.full_name,bovine.full_reg_number, dnatest_type,event_time,userid,	geno_test,location_name
FROM bovinemanagement.dnatest_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id=bovine_id    
LEFT JOIN batch.aggregate_view_curr  ON aggregate_view_curr.full_reg_number = bovine.full_reg_number
ORDER BY event_time DESC,local_number DESC  ";
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
           $row['event_time_raw']=strtotime($row['event_time']);        
          $row['event_time']= date("M d Y",strtotime($row['event_time']));  //has to be at end.
         
        }

        //output
        $output['data'] = $results;
        return ($output);
     
    }

    function displayAllDNATestEvents() {
        
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
        {
                data: "full_reg_number",
                title: "Reg #"
            },
        {
                data: "location_name",
                title: "Current Location"
            },
        {
                data: "dnatest_type",
                title: "Test Type"
            },
        {
                data: "event_time",
                title: "Time",
                type: "date"
            },
        {
                data: "userid",
                title: "Employee"
            },
        {
                data: "geno_test",
                title: "Results"
            },
        
        
            
EX;
    print($dt->start(126,'allDNATestsEvents',null,$columnsStr));     
}     
    
}//end class

/*
 * Herd Cull animal form.
 * 
 */
Class QF2_DNATest extends Quickform2Helper {

    function definition($form) {
       
        //only show if userid is admin or owner group.
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //for admin only.   
        }else {
           $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastFewDays(); 
        } 

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveCows();
        
         $dnaTestTypes = array('LD Panel' => 'LD Panel', '50K Panel' => '50K Panel');

        ////
        //$fieldset = $form->addFieldset('header')->setLabel('Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($aniNumber);
        } else {
                $form->addElement('hidden', 'animalSelected')->setValue( $this->param->bovine_id);       
        }
              
        $form->addElement('select', 'dnaTestType', null, array('options' => $dnaTestTypes, 'label' => 'Type of DNA test'))->addRule("required", "Please specify your type of DNA test.");   
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time.");
        
        
         // Set defaults for the form elements
$form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('event_time' => 1)));
        
  return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);

        $bovineIdArr=parent::processAnimalsGroupIndividiual($value['animalSelected']);
        
        try {
            $res = $GLOBALS['pdo']->beginTransaction();
   
          foreach ($bovineIdArr as $key => $bovineId) {
              
             //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.dnaTest_event (bovine_id,event_time,dnaTest_type,userid) VALUES ($bovineId,'{$value['event_time']}','{$value['dnaTestType']}','{$GLOBALS['auth']->getUsername()}')";
           

            $res = $GLOBALS['pdo']->exec($query);
                           
          }
            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
    }

   

}//end class   
?>