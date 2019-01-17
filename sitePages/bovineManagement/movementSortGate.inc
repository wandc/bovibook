<?php 
/**
 * Sets cows to be sorted out at certain times via the sort gate.
 */

/**
 * NOTE: ALpro has one boolean to maintain state of the sort gate. When it is true, 
 * the cow will be sorted the next time though the gate, it will then be set to false when that occurs.
 *  
 */
//location_sort is the postgres table.

class MovementSortGate extends TabPage {

    public $sortHelper;

    public function __construct() {
        parent::__construct();
        $this->sortHelper = new AlproSortHelper();
    }

    public function defaultDisplay() {
   
        

        /** Not used anymore, sync is called at 2 min intervals with alproCron script! */
        ////X    $this->sortHelper->sync(); //do an alpro sync on every refresh    

        $tabArray[1]['name'] = 'Form';
        $tabArray[2]['name'] = 'To Be Sorted';
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $tabArray[3]['name'] = 'Latest Log';
        }
        $this->renderTabs($tabArray);
    }

    function tab1() {
        
        print("<div class='row'>");
        
        print("<div class='col-sm-4 col-sm-push-8'>");
        
        
      
        ///
     
        print("<hr><br/>");
        ///
        
     
        $a=new QF2_SortGate((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('QF2_SortGate','Mark Animal(s) to be Sorted'));  
        print($a->formJS());
        
        $this->displaySyncStatus();
        print("<hr><br/>");        
        ///SSE button for syncing with alpro
        include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/misc/longProcesses.inc');
        $longProcesses = new LongProcesses();
        $longProcesses->showSSEButton(20007); //harcoded bad.
        ///
        print("<hr><br/>");
        //doesn't seem to handle errors well.
        $this->deleteOldSortsQuickForm();
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
        $this->displayCurrentSortEvents();
        print("</div>");
        print("</div>");

    }

    function tab2() {
        print('<div id="twoColumnLayout">');
        $this->displayCurrentlySetToBeSorted();
        print('</div>');
    }

    function tab3() {
        print('<div id="twoColumnLayout">');
        $this->displayAlrpoInstantaneousLog(1);
        $this->displayAlrpoInstantaneousLog(2);
        print('</div>');
    }

    /* */

    function lastSyncStatus() {
        $answer = '';
        $sql = "SELECT id,state,event_time FROM alpro.sync order by event_time DESC limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        //subtract 6 hrs
        if (($row['state'] == 1) && (new DateTime($row['event_time']) > (new DateTime('now'))->sub(new DateInterval('PT6H00M')))){
            $answer = ("<span style='background-color:green'>Last Sync: True " . $row['event_time'] . "</span>");
        } else {
            $answer = ("<span style='background-color:red'>Last Sync: False " . $row['event_time'] . "</span>");
        }

        $output['data'] = $answer;
        return ($output);
    }

    function displaySyncStatus() {
        ?>
        <div id='content123'></div>
        <script type="text/javascript">
            $(document).ready(function () {
                ajaxCall;
                setInterval(ajaxCall, 3000); //300000 MS == 5 minutes

                function ajaxCall() {
                    $.ajax({
                        contentType: 'application/json',
                        type: 'GET',
                        url: '<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?><?php  echo($this->pageid); ?>/lastSyncStatus/',
                        data: {get_param: 'data'},
                        success: function (data) {
                            var names = data
                            $('#content123').html(data);
                        }
                    });
                }
                ajaxCall();
            });




        </script>
        <?php 
    }

    
    function displayCurrentSortEvents() {
          print("<h2> Current Sorts</h2>");
        $dt = new JQueryDataTable();
        $options = '"paging": false,  "order": [[0,\'asc\']],';
        $columnsStr = <<<EX
             {
                            data: "estimated_sort_time_raw",
                            "visible": false,
                            "searchable": false
                        },
                        {
                            data: "local_number",
                            title: "Animal"
                        },
                        {
                            data: "actual_sort_time",
                            title: "Actual Sort Time"
                        },
                        {
                            data: "estimated_sort_time",
                            title: "Estimated Sort Time"
                        },
                        {
                            data: "location_name",
                            title: "Current Location"
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
        print($dt->start(105, 'currentSortEvents',null,$columnsStr,null,false,$options));     

       
    }


   

    public function currentSortEvents($request) {
        $timeTemp = $this->sortHelper->currentPeriodStartTime();

        // Setup your database connection
        $sql = "SELECT location_sort.id,location_sort.bovine_id,estimated_sort_time,actual_sort_time,userid,local_number,full_name,locationcurr.name as location_name
 FROM bovinemanagement.location_sort 
		       LEFT JOIN bovinemanagement.bovine ON bovine.id=location_sort.bovine_id
LEFT JOIN bovinemanagement.locationcurr ON bovine.id=locationcurr.bovine_id
		       WHERE actual_sort_time is null 
		       UNION
	               SELECT location_sort.id,location_sort.bovine_id,estimated_sort_time,actual_sort_time,userid,local_number,full_name,locationcurr.name as location_name
FROM bovinemanagement.location_sort 
		       LEFT JOIN bovinemanagement.bovine ON bovine.id=location_sort.bovine_id
LEFT JOIN bovinemanagement.locationcurr ON bovine.id=locationcurr.bovine_id
		       WHERE estimated_sort_time= (CURRENT_DATE + time '$timeTemp')  
		       ORDER BY estimated_sort_time";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $row['estimated_sort_time_raw'] = $row['estimated_sort_time'];
            $row['estimated_sort_time'] = ($row['estimated_sort_time'] ? date('D, M d Y A', strtotime($row['estimated_sort_time'])) : '');
            $row['actual_sort_time'] = ($row['actual_sort_time'] ? date('D, M d Y H:i', strtotime($row['actual_sort_time'])) : '');
            $row['action'] = $this->printActionButtons2($row['id'], "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    /*
     * according to alpro, cows who will be sorted next time through.
     *
     */

    function displayCurrentlySetToBeSorted() {

        $tArray = $this->sortHelper->getCowSortInfo(0);
        $fArray = array();
        //sort array out so only ones who are going to be sorted will be displayed.
        foreach ($tArray as $key => $value) {
            if (!empty($value[2])) {
            if ($value[2] == 1) {
                $fArray[$key] = $value;
            }}
        }

        print("<h2>Cows currently set to be sorted the next time through (according to Alpro).</h2>");
        $dg = new DataGridLRArray($fArray, 1000);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Cow #', '0', '0', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sort Next Time?', '2', '2', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sort to Area', '1', '1', array('' => ''), null, null));
        print($dg->render('datagridNarrow'));
        $dg->pager();
    }

   
    function printActionButtons2($location_sort_id, $callBackURL = null) {


        $currentUserid = $GLOBALS['auth']->getUsername();

        //admin user
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $sql = "SELECT id FROM bovinemanagement.location_sort WHERE actual_sort_time is null  ORDER BY update_time DESC ";
        } else { //normnal user
            $sql = "SELECT id FROM bovinemanagement.location_sort WHERE userid='$currentUserid' AND actual_sort_time is null  ORDER BY update_time DESC ";
        }

        $res = $GLOBALS['pdo']->query($sql);
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (($location_sort_id == $row['id'])) {
                $returnButtonsAsString = self::deleteQuickForm($row['id'], $callBackURL); //show delete button, with Breeding table id as index for delete.
            }
        }

        //print button
        return $returnButtonsAsString;
    }

    function deleteQuickForm($tableid, $callBackURL) {

        $form = new HTML_QuickForm("sortDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('submit', 'btnDelete', 'Delete');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');


            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();


            /** Not used anymore, sync is called at 2 min intervals with alproCron script! */
            //sync fixes all.
            //since this is called out of object context, we have to initate the class again.
            //$sortHelper=new AlproSortHelper();
            //$sortHelper->sync();
            //find out first if we are deleting something that is in the present or future.
            $sql = "SELECT location_sort.id,bovine_id,estimated_sort_time,actual_sort_time,userid,local_number,full_name FROM bovinemanagement.location_sort 
		       LEFT JOIN bovinemanagement.bovine ON bovine.id=location_sort.bovine_id
		       WHERE location_sort.id='$tableid' LIMIT 1";
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            /** Not used anymore, sync is called at 2 min intervals with alproCron script! */
            //  $timeTemp=$sortHelper->currentPeriodStartTime();
            //delete from alpro
            //set state back to 0 for this cow.
            $cowNumberArray[$row['local_number']]['local_number'] = $row['local_number'];
            $cowNumberArray[$row['local_number']]['sort_to_area'] = 1; //sort gate area to sort too. (always the sort pen) 
            $cowNumberArray[$row['local_number']]['yes_no'] = 0;
            $cowNumberArray[$row['local_number']]['notify_milking1'] = 0;
            $cowNumberArray[$row['local_number']]['notify_milking2'] = 0;

            /** Not used anymore, sync is called at 2 min intervals with alproCron script! */
            //  $output=$sortHelper->setCowSortInfo($ret='',$cowNumberArray);
            //delete the sort event if actual sort time is still null.
            if ($row['actual_sort_time'] == null) {
                $res = $GLOBALS['pdo']->exec("DELETE FROM bovinemanagement.location_sort WHERE id='$tableid'");
            }

            // determine if the commit or rollback
            
         
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            


            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    /** deletes all old sorts that are out of date and are not needed now */
    function deleteOldSortsQuickForm() {

        $form = new HTML_QuickForm("sortDeleteOldSortsButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('submit', 'btnDelete', 'Delete Historical Sorts');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');



            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();

            /** Not used anymore, sync is called at 2 min intervals with alproCron script! */
            //sync fixes all.
            //since this is called out of object context, we have to initate the class again.
            //$sortHelper=new AlproSortHelper();
            //$sortHelper->sync();
            //find out first if we are deleting something that is in the present or future.
            $sql = "SELECT location_sort.id,bovine_id,estimated_sort_time,actual_sort_time,userid,local_number,full_name FROM bovinemanagement.location_sort 
		       LEFT JOIN bovinemanagement.bovine ON bovine.id=location_sort.bovine_id
WHERE actual_sort_time is null AND estimated_sort_time < current_date";
            $res = $GLOBALS['pdo']->query($sql);
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

                //create an array to use for later deletion.
                //delete from alpro
                //set state back to 0 for this cow.
                $cowNumberArray[$row['local_number']]['local_number'] = $row['local_number'];
                $cowNumberArray[$row['local_number']]['sort_to_area'] = 1; //sort gate area to sort too. (always the sort pen) 
                $cowNumberArray[$row['local_number']]['yes_no'] = 0;
                $cowNumberArray[$row['local_number']]['notify_milking1'] = 0;
                $cowNumberArray[$row['local_number']]['notify_milking2'] = 0;


                if ($row['actual_sort_time'] == null) {
                    $res2 = $GLOBALS['pdo']->exec("DELETE FROM bovinemanagement.location_sort WHERE id='{$row['id']}'");
                }
            }

            /** Not used anymore, sync is called at 2 min intervals with alproCron script! */
            //run delete array through alpro all at once.
            // $timeTemp=$sortHelper->currentPeriodStartTime();
            // $output=$sortHelper->setCowSortInfo($ret='',$cowNumberArray);
            // determine if the commit or rollback
        
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            
            }




            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function displayAlrpoInstantaneousLog($which = 1) {

        if ($which == 1) {
            $sql = "SELECT *,one.name as home_location_name,two.name as sorted_location_name 
		       FROM alpro.sort_gate_log_curr
		       LEFT JOIN bovinemanagement.bovine ON sort_gate_log_curr.bovine_id=bovine.id
		       LEFT JOIN bovinemanagement.location as one ON one.id=sort_gate_log_curr.home_location_id
		       LEFT JOIN bovinemanagement.location as two ON two.id=sort_gate_log_curr.sorted_location_id 
		       ORDER BY local_number";
        } else {
            $sql = "SELECT *,one.name as home_location_name,two.name as sorted_location_name 
		       FROM alpro.sort_gate_log_curr
		       LEFT JOIN bovinemanagement.bovine ON sort_gate_log_curr.bovine_id=bovine.id
		       LEFT JOIN bovinemanagement.location as one ON one.id=sort_gate_log_curr.home_location_id
		       LEFT JOIN bovinemanagement.location as two ON two.id=sort_gate_log_curr.sorted_location_id 
                       WHERE event_time IS NOT NULL
		       ORDER BY event_time DESC";
        }

        $dg = new DataGridLR($sql, 500);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sorted Time', 'event_time', 'event_time', array('' => ''), null, array('DataGridHelper', 'printFormattedTimeGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sorted Location', 'sorted_location_name', 'sorted_location_name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Comment', 'Comment', 'Comment', array('' => ''), null, array('MovementSortGate', 'printGroupDifferent()')));



        if ($dg->datagrid->getRecordCount() > 0) {
            if ($which == 1) {
                print("<h3> Last Time Read at Sort Gate</h3>");
            } else {
                print("<span style='page-break-after: always;'></span>");
                print("<h3>Current Sorts by Time</h3>");
            }
            // Output table and paging links
            print($dg->render('datagridNarrow'));
            $dg->pager();
        } else {
            print("<h3>No Current Sorts.</h3>");
        }
    }

    function printGroupDifferent($params) {
        extract($params);

        //when the home location is High/Heif/Low/Sick and they haven't read in a while, that is an error.         
        if ($record['home_location_id'] == 39 || $record['home_location_id'] == 40 || $record['home_location_id'] == 41 || $record['home_location_id'] == 42) {

            if (strtotime($record['event_time']) < strtotime('-1 day')) {
                return "RFID not reading";
            }
        }
    }

    
    
   /*
    * gived an animal and time, return glyph if to be sorted, as well as time if they were sorted. 
    * just checks to a given day
    * breedings and heats uses this.
    */
  public static function checkToBeSorted($bovine_id,$event_time) {
      $str='';
      //we want to know if something is set to sort in the last 6 hours if youa re there in the am and the next 18 hours if you are the night before. 
      //so if we always say -6 +18, does that cover it?
      //NO is does not, since sort times are 00:00 and 12:00. 
     // $sql="SELECT id,actual_sort_time,estimated_sort_time FROM bovinemanagement.location_sort WHERE bovine_id=$bovine_id AND (estimated_sort_time = date_trunc('day', ('$event_time'::date)) OR estimated_sort_time = date_trunc('day', ('$event_time'::date + interval '1 day')) ) order by estimated_sort_time DESC limit 1";
     $sql="SELECT id,actual_sort_time,estimated_sort_time FROM bovinemanagement.location_sort WHERE bovine_id=$bovine_id AND  estimated_sort_time  >= (current_timestamp - interval '18' hour) AND estimated_sort_time  <= (current_timestamp + interval '18' hour)  order by estimated_sort_time DESC limit 1";
    
     // print($sql);
         $res3 = $GLOBALS['pdo']->query($sql);
            $rowCount = $res3->rowCount();
            $row = $res3->fetch(PDO::FETCH_ASSOC);
            
            $sortTime='';
            if ($row['actual_sort_time'] != null) {
               $sortTime=date('G:iA', strtotime($row['actual_sort_time']));
           }
           if ($rowCount >= 1) {
                 $str =  '<i class="glyphicon glyphicon-circle-arrow-right">'.$sortTime.'</i>';
           }
      
      return $str;
  }
    
    
    
    /* prints errors from alpro archive, doesn't really have anything to do with teh sort gate, but not better place to put it for now */

    public function error() {

        /* alpro error check */
        $error = array();
        include_once($_SERVER['DOCUMENT_ROOT'] . 'phpCronScripts/alproArchive.php');
        $aa = new AlproArchive();
        $error = $aa->errorAlproSync();

        $error2 = array();
        /* alpro iso individual errors */
        include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/alproCommands.php');
        $bb = new AlproCommands();
        $error2 = $bb->errorAlproSyncIndividual();

        return array_merge($error, $error2);
    }

     static function individualCowSortStatus($bovine_id) {
          $sql="SELECT * from bovinemanagement.location_sort where bovine_id=$bovine_id and estimated_sort_time >= (current_date - interval '48 hours')";
 $res = $GLOBALS['pdo']->query($sql);
 $ret='';           
 while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $ret=$ret.date('M d A',strtotime($row['estimated_sort_time'])) .' / '. date('M d h:m A',strtotime($row['actual_sort_time']));
            }
          return $ret;
     }

    
}//end class 

Class QF2_SortGate extends Quickform2Helper {

    function definition($form) {
       
        $aniNumber = Misc::createListOfAllAliveCows();        
        $dateArray = Misc::createDailyDatesForNext12DaysAtMidnightAndNoon(); //AM and PM for 12 days.
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
        if ($this->mode == 'group') {
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($aniNumber);
        } else {        
          
              $form->addElement('hidden', 'animalSelected')->setValue($this->param->bovine_id);     
        }
        
         $form->addSelect('dateEvent', array('multiple' => 'multiple', 'id' => 'selectEventsDate'))->loadOptions($dateArray);
        
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

var s3 = $("#selectEventsDate").select2({
    placeholder: "Choose Sort Times",
    tags: false
});     
        
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {
        if (is_array($value['animalSelected'])) { //check
            //group
            foreach ($value['animalSelected'] as $k => $v) {
                foreach ($value['dateEvent'] as $k2 => $v2) {
                    //now we insert all the animals that were checked.
                    $bovine_id = $v;
                    //and event times.
                    $event_time= date('Y-m-d H:i:sO', $v2); 
                                       
                    // Open a transaction
                    try {
                        $res = $GLOBALS['pdo']->beginTransaction();

                        $this->sortAnimal($bovine_id, $event_time);
              
                        $GLOBALS['pdo']->commit();
                    } catch (Exception $e) {
                        $GLOBALS['pdo']->rollBack();
                        echo "Failed: " . $e->getMessage();
                    }
                }
            }
        }
        //individual case 
        else {
            
            foreach ($value['dateEvent'] as $k2 => $v2) {
                    //now we insert all the animals that were checked.
                    $bovine_id = $value['animalSelected'];
                    //and event times.
                    $event_time= date('Y-m-d H:i:sO', $v2); 
                                       
                    // Open a transaction
                    try {
                        $res = $GLOBALS['pdo']->beginTransaction();

                        $this->sortAnimal($bovine_id, $event_time);
              
                        $GLOBALS['pdo']->commit();
                    } catch (Exception $e) {
                        $GLOBALS['pdo']->rollBack();
                        echo "Failed: " . $e->getMessage();
                    }
                }
            
        }
    }
    
    /*
     * can be called by external classes to say an animal should be sorted. 
     * NOTE: time portion will be converted to be at 00:00:00 or 12:00:00 for 2x milking.
     */
    public function sortAnimal($bovine_id, $event_time) {
        
        //convert 0 to 12 hours to 00:00:00 and 12 to 24 as 12:00:00
        $datetime=strtotime($event_time);
        if (date('a',$datetime) =='am') {
            $event_timeFinal=date('Y-m-d 00:00:00',$datetime);
        } else{
            $event_timeFinal=date('Y-m-d 12:00:00',$datetime); 
        }
        
        
         //check that we didn't already sort this cow in this period.
                $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.location_sort WHERE bovine_id=$bovine_id and estimated_sort_time='$event_time'");
                if ($res->rowCount() == 0) {

                    //insert intention for a sort to occur
                    $query = "INSERT INTO bovinemanagement.location_sort (bovine_id,estimated_sort_time,actual_sort_time,userid) VALUES ($bovine_id,'$event_time',null,'{$GLOBALS['auth']->getUsername()}')";
                    $res = $GLOBALS['pdo']->exec($query);
                }
        
    }

  

}//end class   
