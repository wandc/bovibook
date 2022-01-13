<?php

/**
  This class tracks machinery that needs serviced by number of hours */
class MachineQuery extends TabPage {

    public $machine_id;
    private $page_id;
    private $timeEpochNow;

    public function __construct() {
        parent::__construct();
        //nothing
        $this->loadVars();
    }

    //constructor called class variables.
     private function loadVars() {

        if (empty($this->machine_id)) {
          
        //set the class vars from url vars
        if ((isset($_REQUEST['machine_id']) ) && ($_REQUEST['machine_id'] != null)) {
            $this->machine_id = $_REQUEST['machine_id'];
        }
        if ((isset($_REQUEST['pageid']) ) && ($_REQUEST['pageid'] != null)) {
            //sometimes called from another class, ie sidebar, thus we need to dyniamically find the page_id.
            $this->pageid = findFileNameOfClass(strtolower(get_class($this)));
        }

       
        //find for this timezone, timeline needs.
        $dt = new DateTime("now", new DateTimeZone($GLOBALS['config']['timezone']['default']));
        $this->timeEpochNow = $dt->getTimestamp();
          
     }
     
 
      //now check machine_id is valid for non owner group user, if not throw exception to keep them from seeing page. FIXME: add security violation handler/pretty display.
        if (($GLOBALS['auth']->getOwnerAccess() != 1) AND isset($this->machine_id)) {
              $res = $GLOBALS['pdo']->query("SELECT owner_only from machinery.machine WHERE id={$this->machine_id} limit 1");
              $row = $res->fetch();
              if ($row['owner_only'] == true) {
                  throw new Exception("Security Vioaltion for resource: ".$_REQUEST['machine_id']);
              }
       }
        
        }             

    
     public function defaultDisplay() {
         
         
         $docs = (new Documents)->displayeListOfDocuments('machine', $this->machine_id);
         
$html = <<<HTML
<div class="row">
  <div class="col-md-4">
  {$this->displayMachinePicture()}      
  </div>
  <div class="col-md-8">
      {$this->machineryBootstrapBox('ion-ios-book-outline', 'bg-info', $docs)}  
   </div>     
 </div>        

HTML;
 
        print($html);

         
        $tabArray[1]['name'] = 'TimeLine';
        $tabArray[2]['name'] = 'Service Info';
        $tabArray[3]['name'] = 'Financial Info';
                
        $tabArray[2]['extra_callback_param']="&machine_id={$this->machine_id}";
        $tabArray[3]['extra_callback_param']="&machine_id={$this->machine_id}";
        
        
        $this->renderTabs($tabArray);
    }

    /* time line */
   public function tab1() {

        $timeLine=new Machinery_Timeline($this->page_id,$this->machine_id,$this->timeEpochNow);
        $left= implode($timeLine->generateTimeline());
       
        $a = new QF2_MachineryService((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'machine_id' => $this->machine_id)), 'individual');
       $right=$a->renderBox('qf2_MachineryService', 'Service');
       
       
       
      print( (new BootStrap)->oneThirdTwoThirds($left, $right) );
    }
    
      /* service */
   public function tab2() {


            print($this->serviceTimeLineChart());
       
        $partsList = $this->displayPartsList();
        print($partsList);
        print($this->machineryBootstrapBox('ion-ios-book-outline', 'bg-green', ($partsList)));
        print('</div>' . "\n\r");

   }
   
   /* financial */
   public function tab3() {
   
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
  print($this->displayFinanceInfo());
        }
        print($this->displayHoursChart());
   } 
    
    public function serviceTimeLineChart() {
        $sql = <<<SQL
        SELECT name,service_administered_id, EXTRACT(epoch FROM hours)/3600 as hours, EXTRACT(epoch FROM expired_hours)/3600 as expired_hours,event_time FROM machinery.service_log where machine_id={$this->machine_id} ORDER BY valid_hours,name
SQL;

        $res = $GLOBALS['pdo']->query($sql);


        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = date('M d Y', strtotime($row['event_time']));

            $arr[] = "[ '{$row['name']}', '$time @ {$row['hours']}h',  new Date({$row['hours']}) ,   new Date({$row['expired_hours']}) ],\n\r";
            ; //format of  // $arr[]="[ 't', 'XX',  new Date(0) ,   new Date(6002) ],\n\r";     ;
        }

        //if there is data to display...
        if (!empty($arr)) {

            //since we are going to show the chart now, let's find the current hours. 
            $sql = <<<SQL
                      SELECT EXTRACT(epoch FROM hours)/3600 as hours, EXTRACT(epoch FROM last_service_hours)/3600 as last_service_hours  FROM machinery.machinecurr WHERE id={$this->machine_id} 
SQL;
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            if ($row['last_service_hours'] > $row['hours']) {
                $hoursStr = "[ 'Hours', 'Current Hours',  new Date({$row['last_service_hours']}) ,   new Date({$row['last_service_hours']}+1) ],\n\r";
            } else {
                $hoursStr = "[ 'Hours', 'Current Hours',  new Date({$row['hours']}) ,   new Date({$row['hours']}+.0011) ],\n\r";
            }
            array_unshift($arr, $hoursStr); //put at top of array.





            $js1 = <<<JS1
 <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>

<script type="text/javascript">
  google.charts.load("current", {packages:["timeline"]});
  google.charts.setOnLoadCallback(drawChart);
  function drawChart() {

    var container = document.getElementById('example3.1');
    var chart = new google.visualization.Timeline(container);
    var dataTable = new google.visualization.DataTable();
    dataTable.addColumn({ type: 'string', id: 'Position' });
    dataTable.addColumn({ type: 'string', id: 'Name' });
    dataTable.addColumn({ type: 'date', id: 'Start' });
    dataTable.addColumn({ type: 'date', id: 'End' });
    dataTable.addRows([
JS1;
//put data here
            $js2 = <<<JS2
  
    ]);

      var options = {
           title: 'GDP of selected countries, in US ',
          
          
           legend: 'none',
           bar: {groupWidth: '95%'},
           vAxis: { label: 'gg', gridlines: { count: 4 } }
         };  
        
    chart.draw(dataTable,options);
  }
</script>

<div id="example3.1" style="height: 1400px;"></div>
                
JS2;

            $content = ($js1 . implode($arr) . $js2);
        } else {
            $content = null;
        }


        $colour = 'bg-info';

        $icon = 'ion-ios-cog';
        $html = <<<HTML
<div class="info-box" data-widget="collapse">
            <span class="info-box-icon $colour"><i class="ion $icon"></i></span>
            <div class="info-box-content">
                <span class="info-box-text">Service Intervals History</span>
                $content
            </div>
            <!-- /.info-box-content -->
          </div>
        
HTML;

        return $html;
    }

    

    //call like $this->machineryBootstrapBox('ion-ios-book-outline','bg-info','XX');
    function machineryBootstrapBox($icon, $colour, $content) {

        $html = <<<HTML
<div class="info-box">
            <span class="info-box-icon $colour"><i class="ion $icon"></i></span>
            <div class="info-box-content">
                $content
            </div>
            <!-- /.info-box-content -->
          </div>
        
HTML;

        return $html;
    }

    function displayHoursChart() {
        $sql = "SELECT event_time,EXTRACT(epoch FROM hours)/3600 as hours,userid FROM machinery.hours_combined WHERE machine_id={$this->machine_id} ORDER BY hours";
        $res2 = $GLOBALS['pdo']->query($sql);
           while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $t = strtotime($row2['event_time']);
            $actual[$t] = round($row2['hours']);
        }
        
        $out[] = ("<h3>Hours Through Time</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Hours'] = 'number';
     
        $x = new GoogleVisualizationLine(null, null, $headerStrArr, $actual);
        $out[] = ($x->toString());

        return implode($out);
        
    }
    
    
    function displayFinanceChartLease() {
        ///////////////


        $sql = "SELECT event_type,event_time_start,event_time_finish,residual,hour_overage,yearly_hour_limit,hours_start FROM machinery.finance_event WHERE machine_id={$this->machine_id} AND event_type='lease' ORDER BY event_time_start DESC limit 1";
        $res2 = $GLOBALS['pdo']->query($sql);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);

        // find number of months. 
        $d1 = new DateTime($row2['event_time_start']);
        $d2 = new DateTime($row2['event_time_finish']);
        //$d2->modify('+1 day'); // dont do this. we need to be one month less for hours to be correct. 

        $finance_months = ($d1->diff($d2)->m + ($d1->diff($d2)->y * 12)); // int(8)
        //find monthy number of hours.        
        $monthly_hour_limit = $row2['yearly_hour_limit'] / 12;

        //generate monthly date series and then fill in with cum sum hours, so we can chart it as our hourly lease limit.     
        $period = new DatePeriod($d1, new DateInterval('P1M'), $d2);

        $startHours = $row2['hours_start'];
        $loopCounter = 0;
        $arr = array();
        foreach ($period as $key => $value) {
            $date = $value->format('Y-m-d');

            $hours = $startHours + $monthly_hour_limit * $loopCounter;

            $arr[strtotime($date)] = $hours;
            $loopCounter++;
        }

        //now find all the hour logs entered into the computer between the two dates. 
        $sql3 = "SELECT event_time::date,EXTRACT(epoch FROM hours)/3600   as hours FROM machinery.hours_log WHERE machine_id={$this->machine_id} AND event_time BETWEEN '{$row2['event_time_start']}' AND '{$row2['event_time_finish']}'";
        $res3 = $GLOBALS['pdo']->query($sql3);

        while ($row3 = $res3->fetch(PDO::FETCH_ASSOC)) {
            $t = strtotime($row3['event_time']);
            $actual[$t] = round($row3['hours']);
        }


        $out[] = ("<h3>Lease Hours Limit</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Lease Requirement'] = 'number';
        $headerStrArr['Actual'] = 'number';

        $x = new GoogleVisualizationLine(null, null, $headerStrArr, $arr, $actual);
        $out[] = ($x->toString());

        return implode($out);
    }

    function displayFinanceInfo() {
        $sql = "SELECT event_type,event_time_start,event_time_finish,residual,hour_overage,yearly_hour_limit FROM machinery.finance_event WHERE machine_id={$this->machine_id} ORDER BY event_time_start DESC";

        //only return if there is info to show. 
        $res = $GLOBALS['pdo']->query($sql);

        if ($res->rowCount() > 0) {

            $out[] = ( (new JQueryDataTable)->startBasicSql('Lease/Finance Info', $sql));
            $out[] = $this->displayFinanceChartLease();
            $content = implode($out);
        } else {
            $content = '';
        }

        $colour = 'bg-info';

        $icon = 'ion-ios-cog';
        $html = <<<HTML
<div class="info-box" data-widget="collapse">
            <span class="info-box-icon $colour"><i class="ion $icon"></i></span>
            <div class="info-box-content">
                <span class="info-box-text">Lease Info</span>
                $content
            </div>
            <!-- /.info-box-content -->
          </div>
        
HTML;

        return $html;
    }

    function displayPartsList() {
$sql = <<<SQL
SELECT name as "Part Name",extract(hour FROM hours) as Hours,concat(part_num, ' ',part_num2) as "Part #" ,napa_part_num as "NAPA Part #" FROM machinery.service_item WHERE machine_id={$this->machine_id} ORDER BY hours,name ASC
SQL;

      

        $out[] = ( (new JQueryDataTable)->startBasicSql('Common Parts List', $sql));

        return implode($out);
    }

    public static function createListOfAllActiveMachines($mode = 'default') {
        $machineNumber[null] = "Please Select one..."; //dummy value
        if ($mode == 'default') {
            $machineNumber[null] = "Please Selct one...";
        } //special case for at top of page.
        else {
            $machineNumber[null] = null;
        }
        
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
              $res = $GLOBALS['pdo']->query("SELECT id,name from machinery.machine WHERE active=true ORDER BY name ");
       }
       else {
             $res = $GLOBALS['pdo']->query("SELECT id,name from machinery.machine WHERE active=true and owner_only=false ORDER BY name "); 
       }
        
        
//grab a list of machines.
      {
            
        } // Always check that result is not an error
        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $machineNumber[$row[0]] = $row[1];
        }
        return $machineNumber;
    }

    public static function createListOfAllActiveMachinesWithHours($mode = 'default') {
        $machineNumber[null] = "Please Select one..."; //dummy value
        if ($mode == 'default') {
            $machineNumber[null] = "Please Selct one...";
        } //special case for at top of page.
        else {
            $machineNumber[null] = null;
        }
//grab a list of machines.
        $res = $GLOBALS['pdo']->query("SELECT id,name,hours from machinery.machineCurr WHERE  hours_log=true ORDER BY name "); {
            
        } // Always check that result is not an error
        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $machineNumber[$row[0]] = $row[1] . ' ' . $row[2] . ' hours';
        }
        return $machineNumber;
    }

   

    private function displayMachinePicture() {
        $res = $GLOBALS['pdo']->query("SELECT id FROM picture.machine WHERE machine_id={$this->machine_id} order by event_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);


        $res2 = $GLOBALS['pdo']->query("SELECT id,name,serial_num from machinery.machine WHERE id={$this->machine_id} LIMIT 1");
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);

              $id=empty($row['id']) ? '' : $row['id'];
              $name=empty($row2['name'])? '' : $row2['name'];     
              $serial=empty($row2['serial_num'])? '' : $row2['serial_num'];   
              
        $html = <<<HTML

  <div class="box box-primary">
            <div class="box-body box-profile">
              <img class="profile-user-img img-responsive img-circle" src='/functions/displayAnyPic.php?picture_id={$id}' alt="machine picture">

              <h3 class="profile-username text-center">{$name}</h3>

              <p class="text-muted text-center">Serial #: {$serial} </p>

            </div>
            <!-- /.box-body -->
          </div>      
       
HTML;

        return $html;
    }

    function displayServiceAdministeredForMachine($mode = 'default') {

        $sql = <<<SQL
SELECT service_administered.id,service_administered.event_time,service_administered.userid,service_administered.comment,part_num,service_item.name,extract(hour FROM hours_log.hours) as hours
FROM machinery.service_administered
LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id
LEFT JOIN machinery.machine ON machine.id=service_item.machine_id  
LEFT JOIN machinery.hours_log ON hours_log.id=hours_log_id
WHERE machine.id={$this->machine_id}  
ORDER BY service_administered.event_time DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

         $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'schemaTable' => 'system.service_administered'));
         
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Item'] = ($row['name']);
            $rowOut['Hours'] = ($row['hours']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Employee'] = ($row['userid']);
            if ($mode == 'default') {
                $rowOut['Action'] = $delButton->output($row['id']);
            }
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Service Log', $dataArr, 'ordering: false'));
        return implode($out);
    }

   
    /*     * ******************************************
     * Delete Button Quickform
     * ******************************************** */

    function deleteQuickForm($tableid, $tablename, $callBackURL) {

        $form = new HTML_QuickForm("medSchedDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete', array('class' => 'btn btn-warning', 'id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete?')")); //call javascript to confirm delete.
        //FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY USER.
        //DB hack works though, views need to support deletes before this can be fixed.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();


                //delete the event
                $res = $GLOBALS['pdo']->query("DELETE FROM machinery.$tablename where id='$tableid'");

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }



            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function printComment($params) {
        extract($params);
        if ($record['comment'] == 'null')
            return '';
        else {
            return $record['comment'];
        }
    }

    function machineryHeader($machine_id) {
        $sql = "SELECT * FROM machinery.machine WHERE id = $machine_id LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        print("<h1>{$row['name']}</h1>");
    }

    function machineryDetailed($machine_id) {

        $sql = <<<SQL
SELECT id,machine_id,name,part_num,extract(hour FROM hours)
    ,(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id) as last_service_time   
    ,(SELECT userid  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_userid
    ,extract(hour FROM (SELECT hours_log.hours  FROM machinery.service_administered LEFT JOIN machinery.hours_log ON service_administered.hours_log_id = hours_log.id WHERE service_item_id=service_item.id AND service_administered.event_time=(SELECT max(service_administered.event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id))) as last_service_hours  
    ,(SELECT comment  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_comment
    FROM machinery.service_item WHERE machine_id=$machine_id ORDER BY name
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Name'] = ($row['name']);
            $rowOut['Part #'] = ($row['part_num']);
            $rowOut['Hours'] = ($row['hours']);
            $rowOut['Action'] = $this->printLastServiceInfo($row);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Service Parts Detail', $dataArr, 'order: false'));
        return implode($out);
    }

    function machineryHistoricalLog($machine_id) {
        $sql = "SELECT *,extract(hour FROM hours) as hours FROM machinery.service_administered LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id LEFT JOIN machinery.machine ON machine.id=service_item.machine_id WHERE machine_id=$machine_id ORDER BY service_administered.event_time DESC";
        return (new JQueryDataTable)->startBasicArrayOpt('Service Hsitorical Log', $sql);
    }

    /**
     * not actually a quickform
     */
    function machineryQueryQuickForm() {


        $machine = $this->createListOfAllCurrentMachinery(false);


        // custom select input
        // with javascript to allow submission on change of select element.
        print ("<select $style onchange=\"javascript:location.href='?pageid=95&amp;machine_id='+this.options[this.selectedIndex].value;$('#example &gt; ul').tabs('select', 0);\" name=\"animalNumber\">");
        foreach ($machine as $key => $value) {
            print ("<option value=\"$key\">$value</option>" . "\n\r");
        }
        print ('</select>');
    }

    function createListOfAllCurrentMachinery() {
        $machineNumber[null] = "Please Select one..."; //dummy value
//grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,name from machinery.machine WHERE active=true ORDER BY name "); {
            
        } // Always check that result is not an error
        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $machineNumber[$row[0]] = $row[1];
        }
        return $machineNumber;
    }

    function serviceQuickform($machine_id, $inline = true) {


        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday();
        unset($timeArray[null]); //remove null value.

        $sql = "SELECT id,name FROM machinery.service_item WHERE machine_id=$machine_id ORDER BY name";
        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $serviceItemArray[$row['id']] = $row['name'];
        }



        $form = new HTML_QuickForm("serviceCompletedQuickFormButton$machine_id", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        if ($inline != true) {
            $form->addElement('header', 'serviceCompletedQuickFormHeader', 'Service Peformed');
        }
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'machine_id', $machine_id);


        //create a checkbox for each
        if (empty($serviceItemArray)) {
            ///no service items
        } else {
            foreach ($serviceItemArray as $k => $v) {
                //  $checkGroup = array();
                $checkGroup[] = $form->createElement('text', 'service_item_comment_' . $k);
                $checkGroup[] = $form->createElement('checkbox', 'service_item_check_' . $k, '', "$v <br/>");
            }
            $form->addGroup($checkGroup, 'checkGroup', 'Comment / Item');
        }




        $form->addElement('submit', 'btnCompleted', 'Completed');

        //rules
//        $form->addRule('checkGroup', 'Service Item is required', 'required');
        //check that the hours of the machine were recorded in the last 7 days or throw an error. 
        //    $form->registerRule('checkHoursRecentlyRecordedRule', 'function', 'checkHoursRecentlyRecorded', 'MachineQuery');
        //    $form->addRule(array('checkGroup', 'machine_id'), 'ERROR: Record the current hours on machine first, not been done in the last day.', 'checkHoursRecentlyRecordedRule', array('checkGroup', 'machine_id'));
        // Try to validate a form
        if ($form->validate()) {


            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //will alway find one because of the rule above.
                $sql = "SELECT id FROM machinery.hours_log WHERE machine_id=$machine_id ORDER BY event_time DESC LIMIT 1";
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $hours_log_id = $row['id'];

                //get values
                $pageid = $form->exportValue('pageid');
                $machine_id = $form->exportValue('machine_id');
                $checkArray = $form->exportValue('checkGroup');


                //since there is so many checkboxes, we insert each one individually
                foreach ($checkArray as $k => $v) {

                    //bit of a hack.
                    //grep for word service_item_check_ and then get the service_item number.
                    //then look for a correspoding comment.
                    //this ignores the comment indexes in the array otherwise.

                    $service_item_id = null;
                    $comment = '';
                    preg_match("/service_item_check_(?P<service_item>[0-9]*)/", $k, $matches);

                    if ((is_numeric($matches['service_item']) == true) AND ( $v == 1)) {
                        $service_item_id = $matches['service_item'];

                        //now check for a comment for that service_id
                        if ($checkArray['service_item_comment_' . $service_item_id] != null) {
                            $comment = trim($checkArray['service_item_comment_' . $service_item_id]);
                        }

                        //do insert
                        $res = $GLOBALS['pdo']->exec("INSERT INTO machinery.service_administered (service_item_id,event_time,comment,hours_log_id,userid) VALUES ($service_item_id,'now','$comment',$hours_log_id,'{$GLOBALS['auth']->getUsername()}')");
                    }
                }


                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }



            header($callbackURL);
            exit();
        }

        if (empty($serviceItemArray)) {
            return null;
        } else {

            return $form->toHtml(); //pass on as html string to display later in the table
        }
    }

    function printServiceActionForm($record, $callBackURL) {
        $machine_id = $record['machine_id'];
        $this->serviceQuickform($machine_id, null, $callBackURL);
    }

    //throw error if hours have not been recorded recently when we try and do a service administered insert.
    function checkHoursRecentlyRecorded($valueArr, $elementNameArr) {
        //this has only 2 paramaters because it is outside calling function (ie in the class)
        //generic loop to convert $form elements we want into a usuable array
        //I really hope this isn't how quickforms was designed.

        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData[$elementName] = $valueArr[$count];
            $count++;
        }

        //ok now we have formData, use it.
        //TODO
        //$serviceItemID=$formData['service_item'];
        //find 


        $res = $GLOBALS['pdo']->query("SELECT event_time FROM machinery.hours_log WHERE machine_id={$formData['machine_id']} AND event_time >= (now() - interval '7 day')");
        if ($res->rowCount() >= 1) {
            $returnStatement = true; //means the hours was recently recorded
        } else {
            $returnStatement = false;  //means the hours was NOT recently recorded
        }

        return $returnStatement;
    }

    function displayMachines() {


        $sql = <<<SQL
SELECT *,id as machine_id,extract(hour FROM (SELECT max(hours) FROM machinery.hours_log WHERE machine_id=machine.id)) as latest_hours
		,(SELECT event_time FROM machinery.hours_log WHERE machine_id=machine.id AND hours=(SELECT max(hours) FROM machinery.hours_log WHERE machine_id=machine.id)) as latest_hours_time
		FROM machinery.machine WHERE active=true ORDER BY name
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['ID'] = ($row['id']);
            $rowOut['Name'] = $this->printName($row);
            $rowOut['Service Items'] = $this->printServiceItemsForMachine($row);
            $rowOut['Service Action'] = $this->printServiceActionForm($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&machine_id=$machine_id");

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Service Actions', $dataArr, 'order: false'));
        return implode($out);
    }

    /** NESTED  TABLE* */
    function printServiceItemsForMachine($record) {



        $numberPerPage = 1000; //set stupid high so data is never clipped.
        // Setup your database connection
        $sql = "SELECT id,machine_id,name,part_num,extract(hour FROM hours)
    ,(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id) as last_service_time   
    ,(SELECT userid  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_userid
    ,extract(hour FROM (SELECT hours_log.hours  FROM machinery.service_administered LEFT JOIN machinery.hours_log ON service_administered.hours_log_id = hours_log.id WHERE service_item_id=service_item.id AND service_administered.event_time=(SELECT max(service_administered.event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id))) as last_service_hours  
    ,(SELECT comment  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_comment
    FROM machinery.service_item WHERE machine_id={$record['id']} ORDER BY name";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Name'] = ($row['name']);
            $rowOut['Part #'] = ($row['part_num']);
            $rowOut['Hours'] = ($row['hours']);
            $rowOut['Name'] = $this->printName($row);
            $rowOut['Service Action'] = $this->printLastServiceInfo($row);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
 
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Service Items', $dataArr, 'order: false'));
        return implode($out);
    }

    function colorRowsOlderThan6Months($datagrid, $tableBody) {
        //get an array of data in the datagrid
        $ds = $datagrid->getDataSource();
        $data = $ds->fetch();

        //go through each row and color it if it is the first one
        $counter = 0;
        foreach ($data as $row) {
            //if it has never been serviced.
            if (strlen($row['last_service_time']) == 0) {
                $tableBody->setRowAttributes($counter, array('id' => 'rowAlert'));
            }
            //if it hasn't been serviced in 6 months.
            elseif (strtotime($row['last_service_time']) <= strtotime('now - 6 months')) {
                $tableBody->setRowAttributes($counter, array('id' => 'rowAlert'));
            }
            $counter++;
        }
    }

    function printName($record) {

        $ser = ''; //printer serial number if not null.
        if ($record['serial_num'] != null) {
            $ser = '<br/><br/>ser#: ' . $record['serial_num'];
        }

        return '<div id=\'largish\'>' . $record['name'] . '</div>' . '' . ($record['latest_hours']) . 'hrs @ ' . date("M d, Y", strtotime($record['latest_hours_time'])) . $ser;
    }

    function printLastServiceInfo($record) {

        $comment = '';
        if (($record['last_service_comment'] != null) && ($record['last_service_comment'] != 'null')) {
            $comment = ' ' . $record['last_service_comment'];
        }

        if ($record['last_service_time'] == null) {
            $date = '';
        } else {
            $ftime = date("M d, Y", strtotime($record['last_service_time']));
            $time = strtotime($record['last_service_time']);
            $str = "<b>Serviced</b>: " . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$record['last_service_hours']} hours as done by {$record['last_service_userid']} &nbsp; &nbsp;$comment";
            return $str;
        }
    }

    function displayServiceAdministeredLog($mode = 'default') {

        $numberPerPage = 40;

        // Setup your database connection
        $sql = "SELECT *,extract(hour FROM hours) as hours FROM machinery.service_administered LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id LEFT JOIN machinery.machine ON machine.id=service_item.machine_id ORDER BY service_administered.event_time DESC";
        $out[] = ("<h3>Service Completion Log</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicSql('Service Administered', $sql, ''));

        return implode($out);
    }

    
}

//end class


/*
 * 
 * machinery timeline
 */

class Machinery_Timeline {
    
    public $machine_id;
    private $page_id;
    private $timeEpochNow;
    
    
     public function __construct($page_id,$machine_id,$timeEpochNow) {
        
       $this->page_id=$page_id;  
       $this->machine_id=$machine_id;
       $this->timeEpochNow=$timeEpochNow;
    }
    
    /*
     * main method
     */
   public function generateTimeline() {
        $outArr = array();

        $x=array();
        $x['date']= $this->timeEpochNow + 4;  
        $x['stuff']='<h1>Timeline</h1>';
        $outArr[]=$x;
        
        $x=array();
        $x['date']= $this->timeEpochNow + 3;  
        $x['stuff']='<ul class="timeline">'; //put on second in future, so always first.
        $outArr[]=$x;

        /* timeline entry form */
        $a = new QF2_MachineryComment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'machine_id' => $this->machine_id)), 'group');
        $form = ($a->render('qf_machineryEntry', ''));
        $useridUC = ucfirst($GLOBALS['auth']->getUsername());        
        $x=array();
        $x['date']= $this->timeEpochNow + 1;    
        $x['stuff']=$this->timeLineBox('fa fa-laptop bg-info','now',$useridUC,' entry',$form);
       
        $outArr[]=$x;
    
        $commentsArr = $this->processCommentEvents();
        $hoursArr = $this->processHourEvents();
        $serviceArr = $this->processServiceEvents();
        $pictureArr = $this->processPictureEvents();
        $commentPictureArr = $this->processCommentPictureEvents();

      
        
        
        //
        if (is_array($commentsArr)) {
            $outArr = array_merge($outArr,$commentsArr);
        }
        //
        if (is_array($hoursArr)) {
            $outArr = array_merge($outArr , $hoursArr);
        }
        //
        if (is_array($serviceArr)) {
            $outArr = array_merge($outArr , $serviceArr);
        }
        //
        if (is_array($pictureArr)) {
            $outArr = array_merge($outArr , $pictureArr);
        }
        //
        if (is_array($commentPictureArr)) {
            $outArr = array_merge($outArr , $commentPictureArr);
        }


        //finally add year tags, by finding max and min year. 
        $maxYear = date('Y', (max(( array_column($outArr, 'date')))));
        $minYear = date('Y', (min(( array_column($outArr, 'date')))));
       
        $a=$this->timelineYear($minYear, $maxYear);
       
         $outArr =array_merge($outArr , $this->timelineYear($minYear, $maxYear));
        
        $x=array();
        $x['date']= strtotime('Jan 1 1970');  
        $x['stuff']='</ul>'; //epoch
        $outArr[]=$x;
        
        arsort($outArr); // reverse order

        //order maintained, go through and get the element we want out. 
        foreach($outArr as $key => &$value) {
  $out[]=$value['stuff'];
}
    
        return $out;
    }

    /*
     * creates an array of years for the timeline.
     */
    private function timelineYear($minYear, $maxYear) {

        for ($y = $minYear; $y <= $maxYear; $y++) {
            $time = strtotime('Jan 1 ' . $y);
            $year = $y - 1;
         
        $t=array();
        $t['date']= $time;
        $t['stuff']= <<< HTML
<!-- timeline time label -->
    <div class="time-label">
        <span class="bg-red">
          $year
        </span>
    </div>
<!-- /.timeline-label -->
HTML;
        $outArr[]=$t;
     
        }
   

//now do one extra in green for today's date. 
        $timeNow = $this->timeEpochNow + 2; //now plus 1 second
        $timeNowStr = date('d M Y', $timeNow);
  
         $x=array();
        $x['date']= $timeNow; 
        $x['stuff']=<<< HTML
<!-- timeline time label -->
    <div class="time-label">
        <span class="bg-green">
           $timeNowStr
        </span>
    </div>
<!-- /.timeline-label -->
HTML;
        $outArr[]=$x;

        return $outArr;
    }

    /*
     *      $time = strtotime($row['event_time']);       
         $outArr[(string) $time]=$this->timeLineBox('fa fa-wrench bg-info',$row['event_time'],$row['userid'],$title,$content);
     */
    private function timeLineBox($class,$event_time,$userid,$title,$content) {
        
            $time = date('M d Y',strtotime($event_time));
            $ago = $GLOBALS['MiscObj']->daysOrHoursAgo(strtotime($event_time));
            $userid = ucfirst($userid); 
        
            
            
                   $str = <<< HTML
<div>
        <i class="$class"></i>
        <div class="timeline-item">
            <span class="time" style="font-size:12px color: #999" data-widget="tooltip" title="$time"><i class="fa fa-clock-o"></i> $ago ago</span>
           
            <h3 class="timeline-header no-border"><a href="#">$userid</a>$title</h3>
       
 
            <div class="timeline-body"> $content  </div>
                           
        </div>                      
</div>
HTML;
       

        return $str; 
        
    }
    
    //from https://stackoverflow.com/questions/23739215/format-my-json-string-into-an-ol-ordered-list-in-php
    //recursive build html list from array.
    function build_list($array) {

        $list = '<ul>';
        foreach ($array as $key => $value) {

            if (is_array($value)) {
                $list .= "<strong>$key</strong>: " . build_list($value);
            } else {
                $list .= "<li><strong>$key</strong>: $value</li>";
            }
        }
        $list .= '</ul>';
        return $list;
    }
       //show delete button.
    private function deleteEvent($itemEventTime,$deleteIDName,$schemaTable) {
       
          //show delete button for a week or if admin all events. 
            if (((strtotime($itemEventTime ) >= strtotime('-1 week')) AND ($row['userid']== $GLOBALS['auth']->getUsername())) OR ($GLOBALS['auth']->getAdminMode() == 1)){
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&machine_id=$this->machine_id";
                $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $deleteIDName, 'schemaTable' => $schemaTable ));
                $deleteButtonAsString = ($a->renderDelete($schemaTable . $deleteIDName));
            } else {
                $deleteButtonAsString = '';
            }
            
        return $deleteButtonAsString;
        
    }
    
    private function processHourEvents() {
        $outArr=array();
        $sql = "
        SELECT id,event_time,userid,(extract(hour FROM hours))::integer as hours FROM machinery.hours_log WHERE machine_id={$this->machine_id} ORDER BY event_time DESC
              ";
        $res = $GLOBALS['pdo']->query($sql);

        
         $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'schemaTable' => 'machinery.hours_log'));
        
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            
            $deleteButtonAsString =$delButton->output($row['id']);
            
                     $title='hours recorded';
$content = <<< HTML
          <p id='dsds'>   {$row['hours']} hours $deleteButtonAsString  </p><!-- hi -->
HTML;        
                  
        $x['date']= strtotime($row['event_time']);   
        $x['stuff']=$this->timeLineBox('fa fa-hourglass bg-green',$row['event_time'],$row['userid'],$title,$content);
        $outArr[]=$x;
                    
        }

        return $outArr;
    }

    private function processPictureEvents() {
        $outArr=array();
        $sql = "
        SELECT id,event_time,userid FROM picture.machine_comment WHERE machine_id={$this->machine_id} AND machinery_comment_id IS NULL
              ";
        $res = $GLOBALS['pdo']->query($sql);

          $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimeName' => 'event_time','dateTimePeriodDays' => '1', 'schemaTable' => 'picture.machine_comment'));
        
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            $pic = ("<img id='machinePic' style='max-height: 300px; max-width: 300px;' src='/functions/displayAnyPic.php?picture_id={$row['id']}'/>");

           $deleteButtonAsString =$delButton->output($row['id']);
            
            $title="shared picture";
$content = <<< HTML
         $pic $deleteButtonAsString
HTML;        
        
         
        $x['date']= strtotime($row['event_time']);   
        $x['stuff']=$this->timeLineBox('fas fa-camera bg-purple',$row['event_time'],$row['userid'],$title,$content);
        $outArr[]=$x;
            
        }

        return $outArr;
    }

    private function processCommentEvents() {
         $outArr=array();
        $sql = "
           SELECT comment.* from machinery.comment
LEFT JOIN picture.machine_comment ON machine_comment.machinery_comment_id=comment.id WHERE comment.machine_id={$this->machine_id}  AND machine_comment.id is null ORDER BY event_time DESC
          ";
        $res = $GLOBALS['pdo']->query($sql);

           $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimeName' => 'update_time','dateTimePeriodDays' => '1', 'schemaTable' => 'machinery.comment'));

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

                   $deleteButtonAsString =$delButton->output($row['id']);
             
            $title="commented";
$content = <<< HTML
        {$row['comment']} $deleteButtonAsString      
HTML;        
        
        $x['date']= strtotime($row['event_time']);   
        $x['stuff']=$this->timeLineBox('fas fa-comments bg-yellow',$row['event_time'],$row['userid'],$title,$content);
        $outArr[]=$x;

        }

        return $outArr;
    }

    private function processCommentPictureEvents() {
        $outArr=array();
        $sql = "
          SELECT comment.*,machine_comment.*, machine_comment.id as picture_id, comment.id as comment_id from machinery.comment
LEFT JOIN picture.machine_comment ON machine_comment.machinery_comment_id=comment.id WHERE comment.machine_id={$this->machine_id}  AND machine_comment.id IS NOT NULL ORDER BY comment.event_time DESC
          ";
        $res = $GLOBALS['pdo']->query($sql);

         $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimeName' => 'update_time','dateTimePeriodDays' => '1', 'schemaTable' => 'machinery.comment'));

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
         
            $pic = ("<img id='machinePic' style='max-height: 300px; max-width: 300px;' src='/functions/displayAnyPic.php?picture_id={$row['picture_id']}'/>");

              $deleteButtonAsString =$delButton->output($row['comment_id']);
              
            $title="shared picture and comment";
$content = <<< HTML
        $pic <p>{$row['comment']}</p> $deleteButtonAsString 
HTML;        
        

        $x['date']= strtotime($row['event_time']);   
        $x['stuff']=$this->timeLineBox('fas fa-camera bg-purple',$row['event_time'],$row['userid'],$title,$content);
        $outArr[]=$x;
            
            
        }

        return $outArr;
    }
 
       private function processServiceEvents() {
            $outArr=array();
        $sql = <<<SQL
      with temp as (
SELECT distinct service_administered_id service_administered_id,event_time,update_time,  EXTRACT(epoch FROM hours)/3600 as hours,userid, comment,(array_agg(name)) as service_name FROM machinery.service_log  WHERE machine_id={$this->machine_id}  GROUP BY  service_administered_id, event_time, update_time,hours,userid, comment ORDER BY event_time
)
SELECT  service_administered_id,event_time,hours,userid,update_time, comment,array_to_json(service_name) as service_name FROM temp     

SQL;

        $res = $GLOBALS['pdo']->query($sql);

         $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('deleteName'=>'id','dateTimeName' => 'update_time','dateTimePeriodDays' => '1', 'schemaTable' => 'machinery.service_administered'));

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            //since ordered by service administered id we can look for a change in that and group things together that way

            //convert json array to list.
            $build_list = $this->build_list(json_decode($row['service_name'], true));

              $deleteButtonAsString =$delButton->output($row['service_administered_id']);
            
            $title="servcied performed";
$content = <<< HTML
{$row['comment']}
            <h4> Serviced: </h4>  $deleteButtonAsString
            $build_list    
HTML;        
        
    
        $x['date']= strtotime($row['event_time']);   
        $x['stuff']=$this->timeLineBox('fas fa-wrench bg-info',$row['event_time'],$row['userid'],$title,$content);
        $outArr[]=$x;
            
        }

        return $outArr;

    }
}//end class


/*
 * general classes used by machinery quickforms.
 */
class Machine_Helper {
    /*
     * insert hours, used by timeline too. 
     */

    public function insertHours($machineId, $hours, $event_time) {

        if (!empty($hours) AND ! empty($machineId) AND ! empty($event_time)) {
            //we have hours entry so add it.   
            $query = "INSERT INTO machinery.hours_log (machine_id,hours,event_time,userid) VALUES (:machine_id,:hours,:event_time,:userid)";
            $res = $GLOBALS['pdo']->prepare($query);
            $res->bindValue(':machine_id', $machineId, PDO::PARAM_INT);
            $res->bindValue(':hours', $hours . ' hours', PDO::PARAM_STR);
            $res->bindValue(':event_time', $event_time, PDO::PARAM_STR);
            $res->bindValue(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
            $res->execute();
        }
    }

    public function generateHoursArray() {

        //hours array
        $hoursArray = array(null => 'Please Select One');
        for ($counter = 10; $counter <= 15000; $counter += 10) {
            $hoursArray[$counter] = "$counter hours";
        }
        for ($counter = 15100; $counter <= 100000; $counter += 100) {
            $hoursArray[$counter] = "$counter hours";
        }

        return $hoursArray;
    }

}

/*
 * basically only used by admin page. 
 */

class QF2_Hours extends QF2_Base {

    function definition($form) {

        $machineNumber = MachineQuery::createListOfAllActiveMachinesWithHours('default'); //input compact here to see small version
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtMidnight();
        unset($dateArray[null]); //remove deault so default is today.
        $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event (optional)'));

        $form->addElement('select', 'machine_id', null, array('options' => $machineNumber, 'label' => 'Machine'))->addRule("required", "Please specify your Machine.");
        $hoursArray = (new Machine_Helper)->generateHoursArray();
        $form->addElement('select', 'hours', null, array('options' => $hoursArray, 'label' => 'Number of Hours (optional)'));

        return $form;
    }

    function process($value) {
        (new Machine_Helper)->insertHours($value['machine_id'], $value['hours'], $value['event_time']);
    }

}

//end class

Class QF2_MachineryComment extends QF2_Base {

    function definition($form) {

        //see if machine has hours log
        // only show hours log for equipment that supports it.
        $sql = "SELECT hours_log FROM machinery.machine WHERE id={$_REQUEST['machine_id']} limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch();
        $hoursLogShow = $row['hours_log'];
        if ($hoursLogShow == 1) {
            $hoursArray = (new Machine_Helper)->generateHoursArray();
        }


        //
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form->addElement('hidden', 'machine_id')->setValue($_REQUEST['machine_id']);

        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtMidnight();
            unset($dateArray[null]); //remove deault so default is today.
            $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event (optional)'));
        } else {
            $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastSevenDays();
            unset($dateArray[null]); //remove deault so default is today.
            $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event (optional)'));
        }

        if ($hoursLogShow == 1) {
            $form->addElement('select', 'hours', null, array('options' => $hoursArray, 'label' => 'Number of Hours (optional)'));
        }

        $upload = $form->addElement('file', 'imageName', array('style' => 'width: 300px'))->setLabel("Picture (optional)"); // no longer using special 'uploadedfile' rule for uploads, allow client-side validation
        $upload->addRule('regex', 'Allowed extensions:  .jp(e)g or png', '/\\.(jpe?g|png)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER); // these don't work client-side, for obvious reasons
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an image', array('image/jpeg','image/png')); //ADD //image/heic when imagick supports it in php.
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        $form->addElement('textarea', 'comment', array('rows' => 6, 'style' => 'width: 98%'))->setLabel("Comments (optional)");


        return $form; //required       
    }

    function process($value) {
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();


            //allow owner to set any date for comment. 
            if (!empty($value['event_time'])) {
                $value['event_time'] = date('r', ($value['event_time']));
            } else {
                $value['event_time'] = date('r', $this->timeEpochNow);
            }

            //use the current time of day with the date givien above to give (almost) unique time.
            //not perfect, there still could be collisons.
            $value['event_time'] = (date("Y-m-d", strtotime($value['event_time']))) . ' ' . date("H:i:s.v");

            $machine_comment_id = null;
            if (!empty($value['comment'])) {
                //we have a comment, so insert it. 
                $query = "INSERT INTO machinery.comment (machine_id,event_time,comment,userid) VALUES (:machine_id,:event_time,:comment,:userid)";
                $res = $GLOBALS['pdo']->prepare($query);
                $res->bindValue(':machine_id', $value['machine_id'], PDO::PARAM_INT);
                $res->bindValue(':event_time', $value['event_time'], PDO::PARAM_STR);
                $res->bindValue(':comment', $value['comment'], PDO::PARAM_STR);
                $res->bindValue(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);

                $res->execute();
                $machine_comment_id = $GLOBALS['pdo']->lastInsertId(); //used to link pictures and comments.  
            }


            if (is_uploaded_file($_FILES['imageName']['tmp_name'])) {

                $tmpFileName = $_FILES['imageName']['tmp_name'];
                
                //convert any file to jpg format.
                $data=$this->convertImage($tmpFileName);

                $escaped_data = pg_escape_bytea($data); //escape data

                $query2 = "INSERT INTO picture.machine_comment (machine_id,event_time,picture,userid,machinery_comment_id) VALUES ({$value['machine_id']},'{$value['event_time']}','$escaped_data'::bytea,'{$GLOBALS['auth']->getUsername()}',$machine_comment_id)";
                $res = $GLOBALS['pdo']->exec($query2);
            } else {
                //do nothing, not uploaded.
            }


            if (!empty($value['hours'])) {
                //we have hours entry so add it.   
                (new Machine_Helper)->insertHours($value['machine_id'], $value['hours'], $value['event_time']);
            }

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
        // Close the transaction
    }
    

/*
 * use image magic to convert file to picture jpg. ADD heic when supported. Not supported yet 2021 in this compiled version of imagik module, look at phpinfo for "heic"
 */    
    private function convertImage($originalImageFileName) {

        if (!extension_loaded('imagick')) {
            throw new Exception('Image Magix modules imagick is not installed. check phpinfo.');
        }
        
        $im = new imagick($originalImageFileName);

        $im->setImageFormat('jpg');   // convert to jpg
        $im->scaleImage(800, 0);    //scale to 800 pixels wide and automatic height.     
        
        $outStr = $im->getImageBlob();
        $im->clear();
        $im->destroy();

        return $outStr;
    }

}

//end class

Class QF2_MachineryService extends QF2_Base {

    function definition($form) {

        $machine_id = $_REQUEST['machine_id']; //we should pass this instead.

        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //for admin only.   
        } else {
            $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastFewDays();
        }

       

        $sql = "SELECT id,name,extract(hour from hours) as hours FROM machinery.service_item WHERE machine_id=$machine_id ORDER BY hours,name";
        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $serviceItemArray[$row['id']] = array('name' => $row['name'], 'hours' => $row['hours']);
        }

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form->addElement('hidden', 'machine_id')->setValue($machine_id);

        if (!empty($serviceItemArray)) {
        foreach ($serviceItemArray as $k => $v) {

            $form->addElement('checkbox', 'service_item_check_' . $k, array('class' => 'serviceCheckBox', 'id' => 'boxId', 'value' => 'yes'))->setContent($v['name'] . ' ' . (new BootStrap)->dangerCallout($v['hours'] . ' h'));
        }}
        $form->addElement('textarea', 'service_comment', null, array('label' => 'Comment'));
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");

        
          $sql2 = "SELECT id FROM machinery.machine WHERE id=$machine_id AND hours_log=true";
          $res2 = $GLOBALS['pdo']->query($sql2);
           if ($res2->rowCount() >= 1) {
        $hoursArray = (new Machine_Helper)->generateHoursArray();
        $form->addElement('select', 'hours', null, array('options' => $hoursArray, 'label' => 'Number of Hours at Service'))->addRule("required", "Number of Hours is required.");
        //  $form->addRule('callback', 'ERROR: Hours Entered Makes no Sense. Check it\'s not less than a currenly entered hours.', array('callback' => array($this, 'checkHoursEntryMakesSense'), 'arguments' => array('hours','dateEvent')));
           } else {
             $form->addElement('hidden', 'hours')->setValue('');   
           }
           
           
        $js = <<<JS
                 $('#checkAllButton').on('click', function(event) {
                 console.log('dsds');
 $( '#checkAllButton' ).click( function () {
                  console.log('eee');
    $( 'input#boxId.serviceCheckBox' ).prop('checked', this.checked)
  })
});
 
JS;
        $form->addElement('static', 'checkAllButton')->setContent('<button type="button" id="checkAllButton" class="btn btn-primary">Check All</button>');
        $form->addElement('static', 'checkAllButtonJS')->setContent("<script>$js</script>");


        return $form; //required       
    }

    /*
     * if hours entered is less than latest hours for certain, throw an error
     */

    function checkHoursEntryMakesSense($value) {

        $bovine_id = $value;

        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.pregnant_view WHERE id=$bovine_id");
        if ($res->rowCount() >= 1) {
            $returnStatement = true; //means the cow is still pregnant.
        } else {
            $returnStatement = false; //means the cow is not pregnant and thus can't calve again.
        }

        return $returnStatement;
    }

    function process($value) {

        $event_time = date('Y-m-d H:i:sO', $value['dateEvent']);


        //pulls out the keys from string. not secure. should be some type of hash function that user can't interact with. 
        $serviceItemArr = array();
        foreach ($value as $k => $v) {
            $x = (explode('service_item_check_', $k));
            if (is_numeric($x[1])) {
                $serviceItemArr[$x[1]] = true;
            }
        }

        $res = $GLOBALS['pdo']->beginTransaction();
        try {


            //insert parent entry once.    
            $query = "INSERT INTO machinery.service_administered (event_time,hours,machine_id,comment,userid) VALUES (:event_time,:hours,:machine_id,:comment,:userid)";
            $res = $GLOBALS['pdo']->prepare($query);
            $res->bindValue(':event_time', $event_time, PDO::PARAM_STR);
            $res->bindValue(':hours', $value['hours'] . ':00:00', PDO::PARAM_STR);
            $res->bindValue(':machine_id', $value['machine_id'], PDO::PARAM_INT);
            $res->bindValue(':comment', $value['service_comment'], PDO::PARAM_STR);
            $res->bindValue(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
            $res->execute();

            //get last insertid 
            $service_administered_id = $GLOBALS['pdo']->lastInsertId('machinery.service_administered_id_seq1');

            //now insert each service item(s)
            foreach ($serviceItemArr as $k => $v) {
                $query = "INSERT INTO machinery.service_item_administered (service_administered_id,service_item_id) VALUES (:service_administered_id,:service_item_id)";
                $res = $GLOBALS['pdo']->prepare($query);
                $res->bindValue(':service_administered_id', $service_administered_id, PDO::PARAM_INT);
                $res->bindValue(':service_item_id', $k, PDO::PARAM_INT);
                $res->execute();
            }


            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
        // Close the transaction
    }

}

//end class
?>