<?php 
//tracks who we took DNA test from. 
class DnaTest extends TabPage {

   

    public function defaultDisplay() {
  //setup for tabs.
        $tabArray[1]['name'] = 'Test Performed';
        $tabArray[2]['name'] = 'Test Needed';
       
         $this->renderTabs($tabArray);
    }

    
    function tab1() {
       
        $a=new QF2_DNATest((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
      
        
        $left= $this->displayAllDNATestEvents();
        $right=$a->renderBox('QF2_DNATest','Mark Animal(s) That Have had DNA test taken');
        print((new BootStrap)->oneThirdTwoThirds($left, $right));
    }
    
    function tab2() {
        $left=($this->displayAliveCowsWithNoDNATest());
        $right=($this->currentAnimalsWithNoGenoTestFromCDN());
        
          print((new BootStrap)->halfHalf($left, $right));
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
      
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('CDN Has No Genotest', $dataArr,'')); 
         return implode($out); 
    }
    
    
    
       function aliveCowsWithNoDNATestSubmitted() {

        $sql = "
SELECT 
bovinecurr.id as bovine_id,local_number,bovinecurr.full_name,bovinecurr.full_reg_number,location_name,prodoll,geno_test,(SELECT donotbreed FROM bovinemanagement.service_picks WHERE bovine_id=bovinecurr.id ORDER BY event_time DESC LIMIT 1)
,(SELECT calf_chimeric FROM bovinemanagement.calving_event WHERE calving_event.calf_bovine_id=bovinecurr.id   limit 1) 
,(SELECT calf_breed FROM bovinemanagement.calving_event WHERE calving_event.calf_bovine_id=bovinecurr.id   limit 1) 
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
           $row['icons'] = $GLOBALS['MiscObj']->iconCalfChimeric($row['calf_chimeric']) . $GLOBALS['MiscObj']->iconNotHolstein($row['calf_breed']);         
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
            },
         {
                data: "icons",
                title: ""
            }
        
        
            
EX;
    

         $out[]=($dt->start('Test Needed to be Taken', 126,'aliveCowsWithNoDNATestSubmitted',null,$columnsStr)); 
         return implode($out);     
}     
    
    
    
    function allDNATestsEvents() {

        $sql = "SELECT local_reg_status,
dnatest_event.id,bovine.id as bovine_id,bovine.local_number,bovine.full_name,bovine.full_reg_number, dnatest_type,event_time,userid,	geno_test,location_name
FROM bovinemanagement.dnatest_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id=bovine_id    
LEFT JOIN batch.aggregate_view_curr  ON aggregate_view_curr.full_reg_number = bovine.full_reg_number
LEFT JOIN batch.holstein_canada_registered ON holstein_canada_registered.bovine_id=bovine.id
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
                data: "local_reg_status",
                title: "Reg Status"
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
    return($dt->start('DNA Test Log', 126,'allDNATestsEvents',null,$columnsStr));     
}     
    
}//end class

/*
 * Herd Cull animal form.
 * 
 */
Class QF2_DNATest extends QF2_Base {

    function definition($form) {
       
        //only show if userid in owner group.
      if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //for admin only.   
        }else {
           $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastFewDays(); 
        } 

        
        
         $dnaTestTypes = array('LD Panel' => 'LD Panel', '50K Panel' => '50K Panel');

        if ($this->mode == 'group') {
            $form=(new Quickform2_GroupSelect())->definitionAnimalGroup($form);
        }
        else {
            $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form);
        }
              
        $form->addElement('select', 'dnaTestType', null, array('options' => $dnaTestTypes, 'label' => 'Type of DNA test'))->addRule("required", "Please specify your type of DNA test.");   
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time.");
        
        
         // Set defaults for the form elements
$form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('event_time' => 1)));
        
  return $form; //required   
    }

   
    
    
    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['event_time']);

        $bovineIdArr=(new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
        
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
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

   

}//end class   
?>