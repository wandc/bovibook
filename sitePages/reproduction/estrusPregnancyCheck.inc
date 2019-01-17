<?php 
//keeps track of any cows who have a pregancy check done.
class EstrusPregnancyCheck extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Preg Check Log';
        $tabArray[2]['name'] = 'Due Dates';
        $tabArray[3]['name'] = 'Preg Chk needed > 29';
        $tabArray[4]['name'] = 'Preg Chk needed > 75';
        $tabArray[5]['name'] = 'Needs Bred';
        $tabArray[6]['name'] = 'Post Fresh Check';
        $tabArray[7]['name'] = '120 Days and No Vet Check';
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $tabArray[8]['name'] = 'Insem Score';
        }
        $this->renderTabs($tabArray);
    }

    function tab1() {
        print("<div class='row'>"."\n\r");
       
        print("<div class='col-sm-4 col-sm-push-8'>"."\n\r");
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $this->addPreganacyCheckEvenQuickForm();   //quickform page only show for owners and admins
            // $a=new QF2_PregCheckEvent((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
            // print($a->renderBox('QF2_PregCheckEvent','Record Preg Check'));       
        }
        print("</div>"."\n\r");
         print("<div class='col-sm-8 col-sm-pull-4'>"."\n\r");
       //$this->displayListOfLastPregnancyChecks('default');
        $this->displayPregnancyCheckLog('default');
        print("</div>"."\n\r");
        print("</div>"."\n\r");
   
    }

    function tab2() {
        $this->displayListOfConfirmedPregnant('default');
    }

    function tab3() {
      
         $this->displayListOfAnimalsNeedingPregnancyCheckDataTable();
    }

    function tab4() {
        print($this->displayListOfAnimalsNeedingADueDate('default'));
    }

    function tab5() {
        print($this->displayListOfAnimalsWithMostOpenDays('default'));
    }

    function tab6() {
        $this->displayListOfAnimalsPreVWP('default');
    }
    
    
     function tab7() {
        $this->displayListOfAnimalsNotChecked('default');
    }

    function tab8() {
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $this->inseminationScore();
        }
    }

    public function summaryDisplay() {

       
    }

    /* calculated insemination score for fred and david only */

    public function inseminationScore() {
        $res = $GLOBALS['pdo']->query("WITH temp as (
SELECT 'breeding|'::text || breeding_event.id AS id, 'breeding' AS type, breeding_event.id AS table_id,  bovinemanagement.round_to_nearest_date(breeding_event.actual_breeding_time + '280 days 12:00:00'::interval) AS calculated_potential_due_date,preg_check_event.preg_check_result,breeding_event.actual_breeding_userid,breeding_event.actual_breeding_time
           FROM bovinemanagement.breeding_event
      LEFT JOIN bovinemanagement.bovinecurr ON breeding_event.bovine_id = bovinecurr.id
   LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id
   LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code
   LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number
   LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id = breeding_event.comment_id
LEFT JOIN bovinemanagement.preg_check_event ON estimated_breeding_embryo_event_id=('breeding|'::text || breeding_event.id)
), temp2 as(
SELECT  DISTINCT ON(id) * FROM temp
)
 SELECT *, (SELECT count(temp2.id) FROM temp2 WHERE preg_check_result='pregnant' AND actual_breeding_userid='david') as david_num
,(SELECT count(temp2.id) FROM temp2 WHERE actual_breeding_userid='david') as david_den
,(SELECT count(temp2.id) FROM temp2 WHERE preg_check_result='pregnant' AND actual_breeding_userid='fred') as fred_num
,(SELECT count(temp2.id) FROM temp2 WHERE actual_breeding_userid='fred') as fred_den
 FROM temp2 
LIMIT 1
 ");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        print("<h2>Percent pregnant over all breedings by that user</h2>");
        print("David: " . round($row['david_num'] / $row['david_den'] * 100, 2) . "%<br/>\n\r");
        print("Fred: " . round($row['fred_num'] / $row['fred_den'] * 100, 2) . "%<br/>\n\r");
    }

 
    public function displayPregnancyCheckLog($mode='default') {
        
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
              {
                data: "event_time",
                title: "Event Time"
            },   
            {
                data: "local_number",
                title: "Animal"
            },   
            {
                data: "test_full_name",
                title: "Test"
            },   
            {
                data: "preg_check_result",
                title: "Result"
            },   
            {
                data: "comment_combined",
                title: "Comment"
            },   
            {
                data: "selected_event",
                title: "Selected Event"
            },   
            {
                data: "vet_userid",
                title: "Vet"
            },   
            {
                data: "userid",
                title: "Employee"
            },   
            {
                data: "action",
                title: "Action"
            }
       
EX;
    print($dt->start(35,'pregnancyCheckLog',$mode,$columnsStr,'highlight_important',' paging: false'));     
}     


 public function pregnancyCheckLog($request) {
       $mode = Sanit::reqVal('item', false, 'STR');
       
      $sql = "SELECT preg_check_event.id,preg_check_event.event_time as event_time,bovine.local_number,bovine.full_name,
		combined_breeding_embryo_view.event_time as breeding_embryo_event_time,combined_breeding_embryo_view.id as breeding_embryo_id,combined_breeding_embryo_view.calculated_potential_due_date, combined_breeding_embryo_view.service_sire_short_name, combined_breeding_embryo_view.type, preg_check_event.preg_check_result,preg_check_event.userid,preg_check_type.test_full_name,preg_check_comment_custom,comment,bovine.id as bovine_id,vet_userid
FROM bovinemanagement.preg_check_event
LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id=preg_check_event.estimated_breeding_embryo_event_id
JOIN bovinemanagement.preg_check_type ON preg_check_event.preg_check_type_id = preg_check_type.id
JOIN bovinemanagement.bovine ON bovine.id=preg_check_event.bovine_id
LEFT JOIN bovinemanagement.preg_check_comment ON preg_check_comment.id = preg_check_event.preg_check_comment_id
ORDER BY preg_check_event.create_time DESC limit 1000";
      
  $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time']= date("M d, Y",strtotime($row['event_time']));  //has to be at end.

           $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
           $row['event_time_raw']=strtotime($row['event_time']);
           $row['comment_combined']=$this->printCommnet($row['comment'],$row['preg_check_comment_custom']);
              
           $row['selected_event']= $this->printSelectedBreedingEmbryoEvent($row['type'],$row['breeding_embryo_id'],$row['breeding_embryo_event_time'],$row['service_sire_short_name']);
           if ($mode == 'default') {
             $row['action'] = $this->printActionButtons($row['id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }
           
        }
         

        //output
        $output['data'] = $results;
        return ($output);
    
    
 }

    function printCommnet($comment,$preg_check_comment_custom) {
     
        //combine comments into one field.
        if ($preg_check_comment_custom == null) {
            $commentOut = $comment;
        } elseif ($comment == null) {
            $commentOut = $preg_check_comment_custom;
        } else {
            $commentOut = $comment . ' & ' . $preg_check_comment_custom;
        }
        return $commentOut;
    }

    //datagrid customizations.....
  

    function printSelectedBreedingEmbryoEvent($type,$breeding_embryo_id,$breeding_embryo_event_time,$service_sire_short_name) {
       
        if ($breeding_embryo_id != null) {
            return $type . ' : ' . date("M d Y", strtotime($breeding_embryo_event_time)) . ' to ' . $service_sire_short_name;
        } else {
            return null;
        }
    }

    function printActionButtons($id,$callBackURL) {
      
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.preg_check_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if ($id == $row['id']) {
            $deleteButtonAsString = self::deleteQuickForm($id,$callBackURL); //show delete button, with estrus table id as index for delete.
        }

        return $deleteButtonAsString;
    }

    function deleteQuickForm($preg_check_eventID,$callBackURL) {
        $form = new HTML_QuickForm("estrusPregCheckDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtablelessButton'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'preg_check_eventID', $preg_check_eventID);
        $form->addElement('submit', 'btnDelete', 'Delete');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $preg_check_eventID = $form->getElementValue('preg_check_eventID');

            $query = ("DELETE FROM bovinemanagement.preg_check_event where id='$preg_check_eventID'");
            $res = $GLOBALS['pdo']->exec($query);

            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

 

    
    function displayListOfConfirmedPregnant($mode) {
                print("<h3>Pregnant w/ Confirmed Due Date</h3>");
        $dt=new JQueryDataTable();
         $options = '"paging": false,  "order": [[5,\'asc\']],';
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },   
              {  
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_current_name",
                title: "Current Location"
            },
            {
                data: "preg_check_method",
                title: "Method"
        
            },
     
            {
                data: "due_date",
                title: "Due Date",
               type: "date"
            },
             {
                data: "service_sire_short_name",
                title: "Service Sire"
            },
          {
                data: "offspring_pro_doll",
                title: "Fetus Pro$"
            },
        
        
        
EX;
    print($dt->start(35,'listOfConfirmedPregnant',null,$columnsStr,null, false, $options));     
}
    
    public function listOfConfirmedPregnant($mode) {
        // Setup your database connection
        $sql = "with temp as(
SELECT id as bovine_id,calculated_potential_due_date as event_time2,* 

from bovinemanagement.pregnant_view

)
/* find PA pro doll for offspring */
SELECT * , round(((SELECT prodoll FROM batch.aggregate_view_all WHERE full_reg_number=(SELECT full_reg_number FROM bovinemanagement.bovine WHERE id=temp.bovine_id)) +
(SELECT prodoll FROM batch.aggregate_view_all WHERE full_reg_number=service_sire_full_reg_number))/2) as offspring_pro_doll
from temp";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
             $row['event_time_raw']=strtotime($row['event_time']);
           $row['event_time']=date("M d,Y",strtotime($row['event_time']));
           $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
          //location_current_name
          //preg_check_method 
            $row['due_date']=date("M d, Y",strtotime($row['calculated_potential_due_date']));
         //service_sire_short_name
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
        
    
    //return JSON
       public function listOfAnimalsNeedingPregnancyCheck($request) {
       $sql = "SELECT combined_breeding_embryo_view.recipient_bovine_id  as bovine_id, combined_breeding_embryo_view.recipient_local_number as local_number, combined_breeding_embryo_view.event_time, donotbreed,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,
preg_check_event.event_time as preg_check_time, date_trunc('days',now()-combined_breeding_embryo_view.event_time) as days_ago, bovinecurr.full_name, bovinecurr.location_name,preg_check_event.estimated_breeding_embryo_event_id,date_trunc('days',now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id)) as days_volun
   FROM bovinemanagement.bovinecurr

   LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id = 
        (SELECT id FROM bovinemanagement.combined_breeding_embryo_view 
         WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id AND event_time = 
             (SELECT max(event_time) FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id))

   LEFT JOIN bovinemanagement.preg_check_event ON preg_check_event.id = 
        ((SELECT preg_check_event.id FROM bovinemanagement.preg_check_event
        WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric AND preg_check_event.event_time = 
             (( SELECT max(preg_check_event.event_time) AS max FROM bovinemanagement.preg_check_event WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric))))
 
LEFT JOIN bovinemanagement.service_picks_curr ON service_picks_curr.bovine_id=bovinecurr.id

WHERE combined_breeding_embryo_view.event_time IS NOT NULL
AND ( preg_check_event.event_time IS NULL OR preg_check_event.preg_check_result = 'recheck' OR preg_check_event.event_time <=  combined_breeding_embryo_view.event_time)
AND combined_breeding_embryo_view.event_time + interval '29 days' <= now()
ORDER BY days_ago DESC;";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        /*
         * 
         * 
         * 
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Last Breeding (days)', 'days_ago', 'days_ago', array('' => ''), null, null));
       OK    $dg->datagrid->addColumn(new Structures_DataGrid_Column('Location', 'location_name', 'location_name', array('' => ''), null, null));
              $dg->datagrid->addColumn(new Structures_DataGrid_Column('Days Open', 'days_volun', 'days_volun', array('' => ''), null, null));
              $dg->datagrid->addColumn(new Structures_DataGrid_Column('DIM', 'dim', 'dim', array('' => ''), null, null));
               $dg->datagrid->addColumn(new Structures_DataGrid_Column('Do Not Breed?', 'donotbreed', 'donotbreed', array('' => ''), null, array('DataGridHelper', 'printTrueFalseGenericTrueOnly()')));
                $dg->datagrid->addColumn(new Structures_DataGrid_Column('Note', null, '', array('' => ''), null, array('EstrusPregnancyCheck', 'printWarning()')));

         */
        
        
        //change presentation
        foreach ($results as &$row) {

           $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
           $row['event_time_raw']=($row['days_ago']);
           $row['note'] = $this->printWarning($row['bovine_id']);
         //  $row['note'] = 'h';
           if ($row['days_ago'] >= 80) {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
            if ($row['days_ago'] >= 80) {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
            $row['donotbreed2']=($row['donotbreed'] == 1 ? 'true' : '');
          // $row['action'] = $this->printActionButtons2($row['estrus_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
          // $row['event_time']= date("l M d H:00",strtotime($row['event_time']));  //has to be at end.
          // if  ((($row['estrus_type_id'])==1) AND empty($row['note']))  {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    
    
     function displayListOfAnimalsNeedingPregnancyCheckDataTable($bovine_id=null) {
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
                 
            {
                data: "local_number",
                title: "Animal"
            },
         {
                data: "days_ago",
                title: "Last Breeding (days)"
            },
         {
                data: "location_name",
                title: "Current Location"
            },
        {
                data: "days_volun",
                title: "Days Open"
            },
         {
                data: "dim",
                title: "DIM"
            },
 
         {
                data: "donotbreed2",
                title: "donotbreed2"
            },
        {
                data: "note",
                title: "note"
            }
           
EX;
    print($dt->start(35,'listOfAnimalsNeedingPregnancyCheck',$bovine_id,$columnsStr,'highlight_important'));     
}     
    
    
   
//check if 29D conception test works.
    function printWarning($bovine_id) {
      
        //lookup if it has been less than 100 days from calving. If so, print warning that 29D conception does not work.
        $sql = "SELECT event_time + interval '100 days' as check_date FROM bovinemanagement.calving_event WHERE lactation_id = (SELECT lactation.id FROM bovinemanagement.lactation WHERE lactation.bovine_id=$bovine_id ORDER BY fresh_date DESC LIMIT 1)";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (strtotime($row['check_date']) > strtotime("now")) {
            return "Warning: 29D Conception test ineffective until 100 DIM.";
        } else {
            return null;
        }
    }

    public function displayListOfAnimalsNeedingADueDate($mode = 'default') {


        // Setup your database connection
        //This query works thanks to Jamie. The main key was putting on a combined unqiue constraint for bovine_id/actual_breeding_time on breeding_event and bovine_id/event_time on preg_check_event
        //this allows the select max part to work and not return more than one row in the inner select statements.
        $sql = "with temp as (
SELECT combined_breeding_embryo_view.recipient_bovine_id  as bovine_id, combined_breeding_embryo_view.recipient_local_number as local_number, combined_breeding_embryo_view.event_time,  donotbreed,
preg_check_event.event_time as preg_check_time, date_trunc('days',now()-combined_breeding_embryo_view.event_time) as days_ago, bovinecurr.full_name, bovinecurr.location_name,preg_check_event.estimated_breeding_embryo_event_id,date_trunc('days',now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id)) as days_volun
   FROM bovinemanagement.bovinecurr

   LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id = 
        (SELECT id FROM bovinemanagement.combined_breeding_embryo_view 
         WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id AND event_time = 
             (SELECT max(event_time) FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id))

LEFT JOIN bovinemanagement.service_picks ON service_picks.bovine_id=bovinecurr.id

   LEFT JOIN bovinemanagement.preg_check_event ON preg_check_event.id = 
        ((SELECT preg_check_event.id FROM bovinemanagement.preg_check_event
        WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric AND preg_check_event.event_time = 
             (( SELECT max(preg_check_event.event_time) AS max FROM bovinemanagement.preg_check_event WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric))))
 
WHERE combined_breeding_embryo_view.event_time IS NOT NULL
AND ( preg_check_event.event_time IS NULL OR preg_check_event.preg_check_result = 'recheck' OR preg_check_event.event_time <=  combined_breeding_embryo_view.event_time OR preg_check_event.estimated_breeding_embryo_event_id IS NULL )
AND combined_breeding_embryo_view.event_time + interval '75 days' <= now()
ORDER BY days_ago DESC
)
SELECT distinct(bovine_id),* from temp";


 $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
 
               $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               $rowOut['Last Breeding (days)']=$row['days_ago'];
               $rowOut['Days Open']=$row['days_volun'];   
               $rowOut['Location'] = $row['location_name'];          
               $rowOut['Do Not Breed?']=$row['donotbreed'];   
               $rowOut['Estimated Breeding/Embryo ID']=(empty($row['estimated_breeding_embryo_event_id'])) ? '' : $row['estimated_breeding_embryo_event_id'];   


          
           $resultsOut[]=$rowOut;  
        }
            
        //output
         $dataArr = $resultsOut;

         $out[]=("<h3>Preg Check Needed To Confirm Due Date</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr)); 
         
         return implode($out); 

    }

    public function displayListOfAnimalsWithMostOpenDays($mode = 'default') {

        // Setup your database connection
        //display cows who have not been bred or had embryo put in for 21 days after voluntary waiting period.
        //the 60 day term is a bit of a hack to make sure we catch cows we breed before voluntary waiting period.
        $sql = "SELECT bovinecurr.local_number, bovinecurr.full_name, bovinecurr.id as bovine_id, bovinecurr.location_name, combined_breeding_embryo_view.event_time as breeding_embryo_event_time,	date_trunc('day',justify_hours(now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id))) as days_open,service_picks_curr.donotbreed as donotbreed
 FROM bovinemanagement.bovinecurr
  LEFT JOIN bovinemanagement.service_picks_curr ON service_picks_curr.bovine_id=bovinecurr.id
 LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id = 
        (SELECT id FROM bovinemanagement.combined_breeding_embryo_view 
         WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id AND event_time = 
             (SELECT max(event_time) FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id))
WHERE (combined_breeding_embryo_view.event_time < bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id) - interval '60 days' OR combined_breeding_embryo_view.event_time IS null)
      AND bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id) + interval '21 days' <= now() AND donotbreed=false
ORDER BY days_open DESC
";
        
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           //  $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.      
               $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               $rowOut['Past Waiting Period']=$row['days_open'];
               $rowOut['Location']=$row['location_name'];   
                                     
          
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
         $out[]=("<h3>Cows >21 Days Past Waiting Period with No Breedings and Eligibil to Breed</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
         return implode($out); 
        
    }

    public function displayListOfAnimalsPreVWP($type) {
        ?>
        <script type="text/javascript">
            "use strict";
            $(document).ready(function () {
                $('#freshCowCheckBeforeVWP2').DataTable({
                    "ajax": "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/freshCowCheckBeforeVWP2/",
                    "columnDefs": [{type: 'natural', targets: [0]}], /*natual sorting on column 1*/
                    columns: [
                        {
                            data: "local_number",
                            title: "local_number"
                        },
                        {
                            data: "location_name",
                            title: "location_name"
                        },
                        {
                            data: "fresh_date",
                            title: "fresh_date"
                        },
                        {
                            data: "calculate_breeding_voluntary_waiting_period",
                            title: "calculate_breeding_voluntary_waiting_period"
                        }]

                });
            });
        </script>
        <table  class="display compact" id="freshCowCheckBeforeVWP2"></table>
        <?php 
    }

    
     public function displayListOfAnimalsNotChecked($type) {
         
        ?>
        <script type="text/javascript">
            "use strict";
            $(document).ready(function () {
                $('#animalsNotChecked').DataTable({
                    "ajax": "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/animalsNotChecked/",
                    "columnDefs": [{type: 'natural', targets: [0]}], /*natual sorting on column 1*/
                    columns: [
                        {
                            data: "local_number",
                            title: "local_number"
                        },
                        {
                            data: "location_name",
                            title: "location_name"
                        },
                        {
                            data: "fresh_date",
                            title: "fresh_date"
                        },
                    {
                            data: "checkToBeSorted",
                            title: "To Be Sorted"
                        }]

                });
            });
        </script>
        <table  class="display compact" id="animalsNotChecked"></table>
        <?php 
    }

      public function animalsNotChecked($request) {
        $sql = "with temp as (
SELECT * ,
(SELECT event_time from bovinemanagement.preg_check_event where bovine_id=bovinecurr.id order by event_time desc limit 1) as peg_event_time
from bovinemanagement.bovinecurr 
  LEFT JOIN bovinemanagement.service_picks_curr ON service_picks_curr.bovine_id=bovinecurr.id
where fresh_date is not null and dry_date is null
)
SELECT * from temp where (peg_event_time <= fresh_date OR peg_event_time is null) AND current_date >= fresh_date + (interval '120 days') AND  donotbreed=false";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $row['local_number'] =  $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            
             //sort gate
        require_once $_SERVER['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/movementSortGate.inc'; 
         $row['checkToBeSorted'] = MovementSortGate::checkToBeSorted($row['bovine_id'], $row['event_time']);
            
        }
        //output
        $output['data'] = $results;
        return ($output);
    }
    
    
    
    public function freshCowCheckBeforeVWP2($request) {
        $sql = "with temp as (
SELECT *,id as bovine_id,bovinemanagement.calculate_breeding_voluntary_waiting_period (id),bovinemanagement.calculate_number_of_breeding_since_fresh (id),bovinemanagement.donotbreed_at_timestamp (id, current_date::timestamp) from bovinemanagement.bovinecurr
)
SELECT * from temp where calculate_number_of_breeding_since_fresh=0 and donotbreed_at_timestamp=false AND calculate_breeding_voluntary_waiting_period >= current_date";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
         
            $row['local_number'] =  $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

//passes along error info.
    public function error() {
        $error = array();

        //find who has not been moved to maternity pack a week from calving. TODO: We maybe should have a lower grade warning at 2 weeks before.
        $res = $GLOBALS['pdo']->query("SELECT local_number,calculated_potential_due_date,location_current_name FROM bovinemanagement.pregnant_view WHERE (calculated_potential_due_date - interval '7 days') <= current_timestamp AND location_id!=38 ORDER BY calculated_potential_due_date DESC,local_number");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error[] = new notifyObj(get_class($this), $row['calculated_potential_due_date'], 1, $row['local_number'] . ' needs to be moved to Maternity Pack.');
        }



        return $error;
    }
   /*     * ******************************* */
    /* Add Pregnancy Check Event to DB  */
    /*     * ******************************* */

    public function addPreganacyCheckEvenQuickForm($mode = 'group', $bovine_id = null) {


        $dateArray = Misc::createDailyDatesAtDailyIntervalsForLastSevenDays(); //daily dates rray, with unix timestamp as index.
        $dateArray = Misc::createDailyDatesForLastYearAtNoon();
$aniNumber=null;
   $aniNumberBreedings  =null;   
        if ($mode == 'group') {
            //grab a list of cows
            //show any current cows who have been bred since there last lactation or for heifers since birth....
            $res = $GLOBALS['pdo']->query("SELECT 
		combined_breeding_embryo_view.id as combined_breeding_embryo_id, combined_breeding_embryo_view.recipient_bovine_id as bovine_id, combined_breeding_embryo_view.event_time as event_time, bovinecurr.local_number, bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.dry_date,
	  date_part('day',(localtimestamp - combined_breeding_embryo_view.event_time)) as days_since_breeding, combined_breeding_embryo_view.service_sire_full_name as sire_full_name, combined_breeding_embryo_view.service_sire_short_name as sire_short_name
,(SELECT preg_check_result FROM bovinemanagement.preg_check_event where combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id AND preg_check_result != 'open' AND event_time=(SELECT max(event_time) FROM bovinemanagement.preg_check_event WHERE bovine_id=bovinecurr.id))
FROM bovinemanagement.combined_breeding_embryo_view
JOIN bovinemanagement.bovinecurr ON bovinecurr.id = combined_breeding_embryo_view.recipient_bovine_id
WHERE combined_breeding_embryo_view.event_time IS NOT NULL
ORDER BY local_number,event_time DESC");
            $aniNumber[null] = "Please Select one..."; //dummy value
        } elseif ($mode == 'individual') {

            $res = $GLOBALS['pdo']->query("SELECT 
		combined_breeding_embryo_view.id as combined_breeding_embryo_id, combined_breeding_embryo_view.recipient_bovine_id as bovine_id, combined_breeding_embryo_view.event_time as event_time, bovinecurr.local_number, bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.dry_date,
	  date_part('day',(localtimestamp - combined_breeding_embryo_view.event_time)) as days_since_breeding, combined_breeding_embryo_view.service_sire_full_name as sire_full_name, combined_breeding_embryo_view.service_sire_short_name as sire_short_name
,(SELECT preg_check_result FROM bovinemanagement.preg_check_event where combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id AND preg_check_result != 'open' AND event_time=(SELECT max(event_time) FROM bovinemanagement.preg_check_event WHERE bovine_id=$bovine_id))
                    FROM bovinemanagement.combined_breeding_embryo_view
JOIN bovinemanagement.bovinecurr ON bovinecurr.id = combined_breeding_embryo_view.recipient_bovine_id
WHERE combined_breeding_embryo_view.event_time IS NOT NULL AND bovinecurr.id=$bovine_id
ORDER BY local_number,event_time DESC");
        } else {
            throw new Exception("Error unkown addPreganacyCheckEvenQuickForm mode<br/>.");
        }

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //parse for last word of name and then make an array for the select list.
            $a=preg_split("/[\s,]+/", $row['full_name']);
            $aniNumber[$row['bovine_id']] = $row['local_number'] . " - " . end($a);
            $preg_check_result = null;
            if (trim($row['preg_check_result']) == 'pregnant') {
                $preg_check_result = ' (marked ' . $row['preg_check_result'] . ')';
            }
            $aniNumberBreedings[$row['bovine_id']][$row['combined_breeding_embryo_id']] = $row['days_since_breeding'] . " days ago to " . $row['sire_short_name'] . $preg_check_result;
        }


        //grab a list of pregnancy check tests
        $res = $GLOBALS['pdo']->query("SELECT id,test_full_name FROM bovinemanagement.preg_check_type ORDER BY id");

        $typeOfPregnacyCheck[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $typeOfPregnacyCheck[$row['id']] = $row['test_full_name'];
        }

        //grab a list of pregnancy check comments
        $res = $GLOBALS['pdo']->query("SELECT id,comment FROM bovinemanagement.preg_check_comment ORDER BY id");

        $pregnancyCheckComment[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $pregnancyCheckComment[$row['id']] = $row['comment'];
        }

        //grab a list of vets for optional record of who the vet was for preg check.
        $sqlVet = "SELECT * from wcauthentication.users_in_groups
LEFT JOIN wcauthentication.users ON users.userid=users_in_groups.userid
WHERE groupid='veterinary' and active=true";
        $resVet = $GLOBALS['pdo']->query($sqlVet);
       
        $vet[null] = "Please Select one..."; //dummy value
        while (($row = $resVet->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $vet[$row['userid']] = $row['firstname'] . ' ' . $row['lastname'];
        }



        //would be nice to be able to grab this from db's row constraint, but don't know how to do it.
        $pregnancyCheckResult = array(null => 'Please Select one...', 'pregnant' => 'Pregnant', 'open' => 'Open', 'recheck' => 'Recheck');

        $form = new HTML_QuickForm("addPreganacyCheckEvenQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickform', 'Add Pregnancy Check to List');

        //Hierarchical select list
        $opts[] = $aniNumber;
        $opts[] = $aniNumberBreedings;
        $hs = $form->addElement('hierselect', 'whichBreeding', 'Animal & Which Breeding', array('style' => 'width: 20em;'), '<br />');
        $hs->setOptions($opts);

        $form->addElement('select', 'dateEvent', 'Time of Check', $dateArray);
        $form->addElement('select', 'typeOfPregnacyCheck', 'Type of Check', $typeOfPregnacyCheck);
        $form->addElement('select', 'pregnancyCheckResult', 'Result of Check', $pregnancyCheckResult);
        $form->addElement('select', 'vet_userid', 'Vet (Optional)', $vet);
        $form->addElement('select', 'pregnancyCheckComment', 'Comment (Optional)', $pregnancyCheckComment);
        $form->addElement('text', 'customComment', 'Custom Comment (Optional):');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //set palpate to default.
        $form->setDefaults(array('typeOfPregnacyCheck' => '1'));
        $form->setDefaults(array('dateEvent' => strtotime(date('Y-m-d 12:00:00', strtotime("now"))))); // set default to today
        //rules
        $form->addRule('whichBreeding', 'Animal number and breeding is required', 'required');
        $form->addRule('dateEvent', 'Date of pregnancy check is required', 'required');
        $form->addRule('typeOfPregnacyCheck', 'Type of pregnancy check is required', 'required');
        $form->addRule('pregnancyCheckResult', 'Result of pregnancy check is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            //get values

            $whichBreeding = $form->exportValue('whichBreeding');
            $bovine_id = $whichBreeding[0];
            $breeding_event_id = $whichBreeding[1];
            $tempArray = $form->getElementValue('dateEvent');
            $dateEvent = $tempArray[0];
            $event_time = date('Y-m-d H:i:sO', $dateEvent);
            $vetUserid = $form->exportValue('vet_userid');
            $typeOfPregnacyCheck = $form->exportValue('typeOfPregnacyCheck');
            $pregnancyCheckResult = $form->exportValue('pregnancyCheckResult');
            $pregnancyCheckComment = $form->exportValue('pregnancyCheckComment');
            $customComment = $form->exportValue('customComment');
            $pageid = $form->exportValue('pageid');

            //fill optional fields with null.
            if (trim($pregnancyCheckComment) == null) {
                $pregnancyCheckComment = 'NULL';
            }
            if (trim($customComment) == null) {
                $customComment = '';
            }


            if ((trim($vetUserid) == null) || (trim($vetUserid) == '')) {
                $vetUserid = 'null';
            }


            //if a cow is open it doesn't really make sense to link it to a breeding.
            if ($pregnancyCheckResult == 'open' || $pregnancyCheckResult == 'recheck') {
                $breeding_event_id = 'NULL';
            }


            print("whichBreeding::{$whichBreeding[0]}::{$whichBreeding[1]}<br>");
            print("typeOfPregnacyCheck::$typeOfPregnacyCheck<br>");
            print("pregnancyCheckComment::$pregnancyCheckComment<br>");


            //insert into DB here!!!!!!!!!!!!!1

            $query = "INSERT INTO bovinemanagement.preg_check_event (event_time,preg_check_type_id,preg_check_comment_id,preg_check_comment_custom,userid,estimated_breeding_embryo_event_id,preg_check_result,bovine_id,vet_userid) VALUES ('$event_time',$typeOfPregnacyCheck,$pregnancyCheckComment,'$customComment','{$GLOBALS['auth']->getUsername()}','$breeding_event_id','$pregnancyCheckResult',$bovine_id,'$vetUserid')";
            
            $res = $GLOBALS['pdo']->exec($query);
            {
                print_r($res);
              
            }
            $preg_check_event_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.preg_check_event_id_seq');


            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id=$bovine_id");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }
}

//end class


require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_PregCheckEvent extends Quickform2Helper {

    function definition($form) {

       $dateArray = Misc::createDailyDatesAtDailyIntervalsForLastSevenDays(); //daily dates rray, with unix timestamp as index.
       //$dateArray = Misc::createDailyDatesForLastYearAtNoon(); //used when we have a problem. should add an if admin clause. 

       $aniNumberBreedingsStd=$this->bredAnimalList($this->mode); 
       
        //grab a list of pregnancy check tests
        $res = $GLOBALS['pdo']->query("SELECT id,test_full_name FROM bovinemanagement.preg_check_type ORDER BY id");

        $typeOfPregnacyCheck[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $typeOfPregnacyCheck[$row['id']] = $row['test_full_name'];
        }
        ///

        //grab a list of pregnancy check comments
        $res = $GLOBALS['pdo']->query("SELECT id,comment FROM bovinemanagement.preg_check_comment ORDER BY id");

        $pregnancyCheckComment[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $pregnancyCheckComment[$row['id']] = $row['comment'];
        }
        ///

        //grab a list of vets for optional record of who the vet was for preg check.
        $sqlVet = "SELECT * from wcauthentication.users_in_groups
LEFT JOIN wcauthentication.users ON users.userid=users_in_groups.userid
WHERE groupid='veterinary' and active=true";
        $resVet = $GLOBALS['pdo']->query($sqlVet);
       
        $vet[null] = "Please Select one..."; //dummy value
        while (($row = $resVet->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $vet[$row['userid']] = $row['firstname'] . ' ' . $row['lastname'];
        }
        ///
       
        //preg result types
        $pregnancyCheckResult = array(null => 'Please Select one...', 'pregnant' => 'Pregnant', 'open' => 'Open', 'recheck' => 'Recheck');
       ///
       
  
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
             $form->addElement('hierselect', 'animal')->setLabel('Choose Animal:')->loadOptions(array($aniNumberBreedingsStd->aniNumber, $aniNumberBreedingsStd->aniNumberBreedings));
             
            
        } else {
            print("FIXME");
            $form->addElement('hidden', 'bovine_id')->setValue($this->param->bovine_id);
             $form->addElement('select', 'breeding', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");
        }
        
        
        
        
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Preg Check'))->addRule("required", "Please specify your time of preg check.");
        $form->addElement('select', 'typeOfPregnacyCheck', null, array('options' => $typeOfPregnacyCheck, 'label' => 'Type of Check'))->addRule("required", "Please specify your type of check.");
        $form->addElement('select', 'pregnancyCheckResult', null, array('options' => $pregnancyCheckResult, 'label' => 'Result of Check'))->addRule("required", "Please specify your result of preg check.");
        $form->addElement('select', 'vet_userid', null, array('options' => $vet, 'label' => 'Vet (Optional)'));
        $form->addElement('select', 'pregnancyCheckComment', null, array('options' => $pregnancyCheckComment, 'label' => 'Comment (Optional)'));  
        $form->addElement('text', 'customComment', null, array('label' => 'Custom Comment (Optional):'));

        return $form; //required       
    }

    function process($value) {
       // $value['event_time'] = date('Y-m-d H:i:sO', $value['dateHeat']);
       // $value['estrus_type_id'] = $value['heatType'];

        
        $query = "INSERT INTO bovinemanagement.preg_check_event (event_time,preg_check_type_id,preg_check_comment_id,preg_check_comment_custom,userid,estimated_breeding_embryo_event_id,preg_check_result,bovine_id,vet_userid) VALUES ('$event_time',$typeOfPregnacyCheck,$pregnancyCheckComment,'$customComment','{$GLOBALS['auth']->getUsername()}','$breeding_event_id','$pregnancyCheckResult',$bovine_id,'$vetUserid')";
            
        
       
        $res = $GLOBALS['pdo']->exec($query);
    }

    //used to generate arrays for hier select. 
    function bredAnimalList($mode) {
        
$aniNumber=null;
   $aniNumberBreedings  =null;   
        if ($mode == 'group') {
            //grab a list of cows
            //show any current cows who have been bred since there last lactation or for heifers since birth....
            $res = $GLOBALS['pdo']->query("SELECT 
		combined_breeding_embryo_view.id as combined_breeding_embryo_id, combined_breeding_embryo_view.recipient_bovine_id as bovine_id, combined_breeding_embryo_view.event_time as event_time, bovinecurr.local_number, bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.dry_date,
	  date_part('day',(localtimestamp - combined_breeding_embryo_view.event_time)) as days_since_breeding, combined_breeding_embryo_view.service_sire_full_name as sire_full_name, combined_breeding_embryo_view.service_sire_short_name as sire_short_name
,(SELECT preg_check_result FROM bovinemanagement.preg_check_event where combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id AND preg_check_result != 'open' AND event_time=(SELECT max(event_time) FROM bovinemanagement.preg_check_event WHERE bovine_id=bovinecurr.id))
FROM bovinemanagement.combined_breeding_embryo_view
JOIN bovinemanagement.bovinecurr ON bovinecurr.id = combined_breeding_embryo_view.recipient_bovine_id
WHERE combined_breeding_embryo_view.event_time IS NOT NULL
ORDER BY local_number,event_time DESC");
            $aniNumber[null] = "Please Select one..."; //dummy value
        } elseif ($mode == 'individual') {

            $res = $GLOBALS['pdo']->query("SELECT 
		combined_breeding_embryo_view.id as combined_breeding_embryo_id, combined_breeding_embryo_view.recipient_bovine_id as bovine_id, combined_breeding_embryo_view.event_time as event_time, bovinecurr.local_number, bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.dry_date,
	  date_part('day',(localtimestamp - combined_breeding_embryo_view.event_time)) as days_since_breeding, combined_breeding_embryo_view.service_sire_full_name as sire_full_name, combined_breeding_embryo_view.service_sire_short_name as sire_short_name
,(SELECT preg_check_result FROM bovinemanagement.preg_check_event where combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id AND preg_check_result != 'open' AND event_time=(SELECT max(event_time) FROM bovinemanagement.preg_check_event WHERE bovine_id=$bovine_id))
                    FROM bovinemanagement.combined_breeding_embryo_view
JOIN bovinemanagement.bovinecurr ON bovinecurr.id = combined_breeding_embryo_view.recipient_bovine_id
WHERE combined_breeding_embryo_view.event_time IS NOT NULL AND bovinecurr.id=$bovine_id
ORDER BY local_number,event_time DESC");
        } else {
            throw new Exception("Error unkown addPreganacyCheckEvenQuickForm mode<br/>.");
        }

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //parse for last word of name and then make an array for the select list.
            $a=preg_split("/[\s,]+/", $row['full_name']);
            $aniNumber[$row['bovine_id']] = $row['local_number'] . " - " . end($a);
            $preg_check_result = null;
            if (trim($row['preg_check_result']) == 'pregnant') {
                $preg_check_result = ' (marked ' . $row['preg_check_result'] . ')';
            }
            $aniNumberBreedings[$row['bovine_id']][$row['combined_breeding_embryo_id']] = $row['days_since_breeding'] . " days ago to " . $row['sire_short_name'] . $preg_check_result;
        }
        
        //return
        $ret=new stdClass();
        $ret->aniNumber=$aniNumber;
        $ret->aniNumberBreedings=$aniNumberBreedings;
     return $ret;   
    }
    
    
    
}//end class
?>