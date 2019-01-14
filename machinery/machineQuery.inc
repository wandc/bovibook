<?php

//Lists straws of semen in our inventory



require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');

require_once($_SERVER['DOCUMENT_ROOT'] . 'functions/queryPageHelper.inc');
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';


//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if (class_exists('DataGridLR_Ext') != true) {

    //extends datgrid class with specific logic to colour the line.
    //use print($dg1->render('datagrid',true,'00FF00')); to set colour
    class DataGridLR_Ext extends DataGridLR {

        function colourLineCriteria($row) {
            if (strlen($row['last_service_time']) == 0) {
                return true;
            }
            //if it hasn't been serviced in 6 months.
            elseif (strtotime($row['last_service_time']) <= strtotime('now - 6 months')) {
                return true;
            } else {
                return false;
            }
        }

    }

}

/**
  This class tracks machinery that needs serviced by number of hours */
class MachineQuery extends BasePage {

    public $machine_id;
    private $page_id;
    private $timeEpochNow;

    public function __construct() {
        parent::__construct();
        //nothing
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

    public function defaultDisplay() {

         print("<div class='row'>" . "\n\r");
         print($this->displayMachinePicture());
         
         
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/machinery/documents.inc');
        $docs=Documents::displayeListOfDocuments('machine', $this->machine_id); 
        print($this->machineryBootstrapBox('ion-ios-book-outline','bg-aqua',$docs));
          
        
           $partsList=$this->displayPartsList();   
        print($this->machineryBootstrapBox('ion-ios-book-outline','bg-aqua',implode($partsList)));
          
        print("</div>" . "\n\r");
        
        
        print("<div class='row'>" . "\n\r");
        print("<div class='col-sm-4 col-sm-push-8'>" . "\n\r");



       
        print($this->serviceQuickform($this->machine_id, false));

      
        $this->displayMachineHours();
        $this->displayServiceAdministeredForMachine();
        
        
        
     
         
        
        // only show hours log for equipment that supports it.
        $sql = "SELECT hours_log FROM machinery.machine WHERE id={$this->machine_id} limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch();
        
        if ($row['hours_log'] == 1) {
        $a=new QF2_MachineHours((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'],'machine_id'=>$_REQUEST['machine_id'])),'group',(object) array('machine_id'=>$this->machine_id));
        $qf2=($a->renderBox('qf_machienHours','Record Machine Hours'));       
          print($this->machineryBootstrapBox('ion-ios-book-outline','bg-aqua',$qf2));
        }
        

        print("</div>" . "\n\r");

        print("<div class='col-sm-8 col-sm-pull-4'>" . "\n\r");

        print(implode($this->generateTimeline()));

        print("</div>" . "\n\r");
        print("</div>" . "\n\r");
    }


   

    
    //call like $this->machineryBootstrapBox('ion-ios-book-outline','bg-aqua','XX');
    function machineryBootstrapBox($icon,$colour,$content) {
        
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
    
    
    
    
     function displayPartsList() {
         $out=array();
        $sql = "SELECT name,part_num,napa_part_num FROM machinery.service_item WHERE machine_id={$this->machine_id} ORDER BY name DESC";
        $res = $GLOBALS['pdo']->query($sql);
//some things don't have hours. 
        if ($res->rowCount() > 0) {
            
       
            
            
               $out[]=('<h2>Common Parts List</h2>');
            $dg = new DataGridLR($sql, 1000);

            $dg->datagrid->addColumn(new Structures_DataGrid_Column('name', 'name', 'name', array('' => ''), null, null));
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('part_num', 'part_num', 'part_num', array('' => ''), null, null));
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('napa_part_num', 'napa_part_num', 'napa_part_num', array('' => ''), null, null));

        $out[]=($dg->render('datagrid datagridSummary'));

        }
        
        return $out;
    }
    
    
    
    /**
     * Small form that selects the machine.
     */
    public function machineSelectQuickForm($mode = 'default') {

        if ($mode == 'default') {
            $style = 'machineSelectPage';
        } else {
            $style = 'machineSelect';
        }

        $machineNumber = self::createListOfAllActiveMachines($mode); //input compact here to see small version
        //javascript to allow submission on change of select element.
        $attrs = array('id' => "$style", 'onchange' => "javascript:location.href='?machine_id='+this.options[this.selectedIndex].value+'&pageid={$this->pageid}';$('#example > ul').tabs('select', 0);");

        //javascript to allow submission when enter is pressed within select form
        //$attrs= array('onKeyPress' => "javascript:if(event.keyCode=='13'){document.frmTest.submit();}");



        $form = new HTML_QuickForm("frmMachineQuickSelect", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('select', 'machineNumber', '', $machineNumber, $attrs);
        $form->addElement('hidden', 'pageid', $this->pageid);

        //defaults
        if ($this->machine_id != null) {
            $form->setDefaults(array('machineNumber' => $this->machine_id)); //set to current machine
        } else {
            $form->setDefaults(array('machineNumber' => 0)); //set default to no.  
        }


        // Try to validate a form
        if ($form->validate()) {

            //get value
            $this->machine_id = $form->exportValue('machineNumber');
            $this->pageid = $form->exportValue('pageid');
        } //end validation
        $form->accept($renderer);
        return $renderer->toHtml();
    }

    public function createListOfAllActiveMachines($mode = 'default') {
        $machineNumber[null] = "Please Select one..."; //dummy value
        if ($mode == 'default') {
            $machineNumber[null] = "Please Selct one...";
        } //special case for at top of page.
        else {
            $machineNumber[null] = null;
        }
//grab a list of machines.
        $res = $GLOBALS['pdo']->query("SELECT id,name from machinery.machine WHERE active=true ORDER BY name ");
        {
            
        } // Always check that result is not an error
        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $machineNumber[$row[0]] = $row[1];
        }
        return $machineNumber;
    }

   

    function displayMachineHours($mode = 'default') {


        $sql = "SELECT id,event_time,userid,extract(hour FROM hours) as hours FROM machinery.hours_log WHERE machine_id={$this->machine_id} ORDER BY event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
//some things don't have hours. 
        if ($res->rowCount() > 0) {
            print('<h2>Hours Log</h2>');
            $dg = new DataGridLR($sql, 1000);

            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Reserved', null, '', array('' => ''), null, array('DatagridHelper', 'printFormattedTime()')));
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Hours', 'hours', 'hours', array('' => ''), null, null));
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', 'userid', array('' => ''), null, null));
            if ($mode == 'default') {
                $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', 'Action', 'Action', array('' => ''), null, array('MachineQuery', 'printActionButtons2()')));
            }

            print($dg->render('datagrid datagridSummary'));
            $dg->pager();
        }
    }

    /*     * *****************************************
     * Show Delete Button.
     * ******************************************* */

    function printActionButtons2($params) {
        extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();
        $returnButtonsAsString = '';
        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $sql = "SELECT id,'hours_log' as tablename FROM machinery.hours_log ORDER BY update_time DESC LIMIT 10";
        } else { //only allow the last one to be deleted for normal users.
            $sql = "SELECT id,'hours_log' as tablename FROM machinery.hours_log WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1";
        }
        $res = $GLOBALS['pdo']->query($sql);

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if (($record['id'] == $row['id'])) {
                $returnButtonsAsString = self::deleteQuickForm($row['id'], $row['tablename']); //show delete button, with Breeding table id as index for delete.
            }
        }

        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    private function displayMachinePicture() {
        $res = $GLOBALS['pdo']->query("SELECT id FROM picture.machine WHERE machine_id={$this->machine_id} order by event_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);

       
        $res2 = $GLOBALS['pdo']->query("SELECT id,name,serial_num from machinery.machine WHERE id={$this->machine_id} LIMIT 1");
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
       
        
$html= <<<HTML

<div class="col-md-3">

  <div class="box box-primary">
            <div class="box-body box-profile">
              <img class="profile-user-img img-responsive img-circle" src='/functions/displayAnyPic.php?picture_id={$row['id']}' alt="machine picture">

              <h3 class="profile-username text-center">{$row2['name']}</h3>

              <p class="text-muted text-center">Serial #: {$row2['serial_num']} </p>

             

             
            </div>
            <!-- /.box-body -->
          </div>      
</div>      
                
        
HTML;


        
        return $html;
        
    }

  

    function displayServiceAdministeredForMachine($mode = 'default') {

        print('<h2>Service Log</h2>');
        $sql = "SELECT service_administered.id,service_administered.event_time,service_administered.userid,service_administered.comment,part_num,service_item.name,extract(hour FROM hours_log.hours) as hours "
                . "FROM machinery.service_administered "
                . "LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id "
                . "LEFT JOIN machinery.machine ON machine.id=service_item.machine_id "
                . "LEFT JOIN machinery.hours_log ON hours_log.id=hours_log_id "
                . "WHERE machine.id={$this->machine_id} "
                . "ORDER BY service_administered.event_time DESC";

        $dg = new DataGridLR($sql, 1000);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', 'event_time', 'event_time', array('' => ''), null, array('DatagridHelper', 'printFormattedTime()')));

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Item', 'name', 'name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Hours', 'hours', 'hours', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Comment', 'comment', 'comment', array('' => ''), null, array('MachineQuery', 'printComment()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', 'userid', array('' => ''), null, null));
        if ($mode == 'default') {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', 'Action', 'Action', array('' => ''), null, array('MachineQuery', 'printActionButtons()')));
        }
        print($dg->render('datagrid datagridSummary'));
        $dg->pager();
    }

    /*     * *****************************************
     * Show Delete Button.
     * ******************************************* */

    function printActionButtons($params) {
        extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();
        $returnButtonsAsString = '';
        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $sql = "SELECT id,'service_administered' as tablename FROM machinery.service_administered ORDER BY update_time DESC LIMIT 10";
        } else { //only allow the last one to be deleted for normal users.
            $sql = "SELECT id,'service_administered' as tablename FROM machinery.service_administered WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1";
        }
        $res = $GLOBALS['pdo']->query($sql);

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if (($record['id'] == $row['id'])) {
                $returnButtonsAsString = self::deleteQuickForm($row['id'], $row['tablename']); //show delete button, with Breeding table id as index for delete.
            }
        }

        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    /*     * ******************************************
     * Delete Button Quickform
     * ******************************************** */

    function deleteQuickForm($tableid, $tablename) {

        $form = new HTML_QuickForm("medSchedDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete?')")); //call javascript to confirm delete.
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
            }



            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&machine_id={$_REQUEST['machine_id']}");
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


        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
        $sql = "SELECT id,machine_id,name,part_num,extract(hour FROM hours)
    ,(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id) as last_service_time   
    ,(SELECT userid  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_userid
    ,extract(hour FROM (SELECT hours_log.hours  FROM machinery.service_administered LEFT JOIN machinery.hours_log ON service_administered.hours_log_id = hours_log.id WHERE service_item_id=service_item.id AND service_administered.event_time=(SELECT max(service_administered.event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id))) as last_service_hours  
    ,(SELECT comment  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_comment
    FROM machinery.service_item WHERE machine_id=$machine_id ORDER BY name";

        $dg = new DataGridLR($sql, 100);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('name', 'name', 'name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('part_num', 'part_num', 'part_num', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('hours', 'hours', 'hours', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Service Action', null, '', array('' => ''), null, array('MachineryService', 'printLastServiceInfo()')));

        print($dg->render());
        $dg->pager();
    }

    function machineryHistoricalLog($machine_id) {
        $sql = "SELECT *,extract(hour FROM hours) as hours FROM machinery.service_administered LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id LEFT JOIN machinery.machine ON machine.id=service_item.machine_id WHERE machine_id=$machine_id ORDER BY service_administered.event_time DESC";
        $dg = new DataGridLR($sql, 100);

        print($dg->render());
        $dg->pager();
    }

    /**
     * not actually a quickform
     */
    function machineryQueryQuickForm() {


        $machine = $this->createListOfAllCurrentMachinery(false);


        // custom select input
        // with javascript to allow submission on change of select element.
        print ("<select $style onchange=\"javascript:location.href='?pageid=95&amp;machine_id='+this.options[this.selectedIndex].value;$('#example &gt; ul').tabs('select', 0);\" name=\"animaXXXlNumber\">");
        foreach ($machine as $key => $value) {
            print ("<option value=\"$key\">$value</option>" . "\n\r");
        }
        print ('</select>');
    }

    function createListOfAllCurrentMachinery() {
        $machineNumber[null] = "Please Select one..."; //dummy value
//grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,name from machinery.machine WHERE active=true ORDER BY name ");
        {
            
        } // Always check that result is not an error
        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $machineNumber[$row[0]] = $row[1];
        }
        return $machineNumber;
    }

    function serviceQuickform($machine_id, $inline = true) {


        $timeArray = Misc::createHourlyDatesForOnlyToday();
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
        $form->addRule('service_item', 'Service Item is required', 'required');
        //check that the hours of the machine were recorded in the last 7 days or throw an error. 
        $form->registerRule('checkHoursRecentlyRecordedRule', 'function', 'checkHoursRecentlyRecorded', 'MachineQuery');
        $form->addRule(array('service_item', 'machine_id'), 'ERROR: Record the current hours on machine first, not been done in the last day.', 'checkHoursRecentlyRecordedRule', array('service_item', 'machine_id'));


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
            }



            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid=$pageid&machine_id=$machine_id");
            exit();
        }

        if (empty($serviceItemArray)) {
            return null;
        } else {

            return $form->toHtml(); //pass on as html string to display later in the table
        }
    }

    function printServiceActionForm($params) {
        extract($params);
        $machine_id = $record['machine_id'];
        $this->serviceQuickform($machine_id);
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

        $numberPerPage = 20;

        // Setup your database connection
        $sql = "SELECT *,id as machine_id,extract(hour FROM (SELECT max(hours) FROM machinery.hours_log WHERE machine_id=machine.id)) as latest_hours
		,(SELECT event_time FROM machinery.hours_log WHERE machine_id=machine.id AND hours=(SELECT max(hours) FROM machinery.hours_log WHERE machine_id=machine.id)) as latest_hours_time
		FROM machinery.machine WHERE active=true ORDER BY name";
        $dg = new DataGridLR($sql, $numberPerPage);

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('ID', 'id', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Name', null, '', array('' => ''), null, array('MachineryService', 'printName()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Service Items', null, '', array('' => ''), null, array('MachineryService', 'printServiceItemsForMachine()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Service Action', null, '', array('' => ''), null, array('MachineryService', 'printServiceActionForm()')));

        print($dg->render());
        if ($mode == 'default') {
            $dg->pager();
        }
    }

    /** NESTED  TABLE* */
    function printServiceItemsForMachine($params) {
        extract($params);


        $numberPerPage = 1000; //set stupid high so data is never clipped.
        // Setup your database connection
        $sql = "SELECT id,machine_id,name,part_num,extract(hour FROM hours)
    ,(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id) as last_service_time   
    ,(SELECT userid  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_userid
    ,extract(hour FROM (SELECT hours_log.hours  FROM machinery.service_administered LEFT JOIN machinery.hours_log ON service_administered.hours_log_id = hours_log.id WHERE service_item_id=service_item.id AND service_administered.event_time=(SELECT max(service_administered.event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id))) as last_service_hours  
    ,(SELECT comment  FROM machinery.service_administered WHERE service_item_id=service_item.id AND event_time=(SELECT max(event_time)  FROM machinery.service_administered WHERE service_item_id=service_item.id)) as last_service_comment
    FROM machinery.service_item WHERE machine_id={$record['id']} ORDER BY name";

        $dg = new DataGridLR_Ext($sql, $numberPerPage);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('name', 'name', 'name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('part_num', 'part_num', 'part_num', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('hours', 'hours', 'hours', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Service Action', null, '', array('' => ''), null, array('MachineryService', 'printLastServiceInfo()')));


        return ($dg->render('datagrid', true, $GLOBALS['config']['COLOUR']['attention']));
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

   
    function displayHoursLog() {

        $numberPerPage = 40;

        // Setup your database connection
        $sql = "SELECT *,hours_log.id as hours_id, extract(hour FROM hours) FROM machinery.hours_log LEFT JOIN machinery.machine ON machine.id=hours_log.machine_id ORDER BY event_time DESC,machine.name";
        $dg = new DataGridLR($sql, $numberPerPage);

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''), null, array('MachineryService', 'printFormattedTime()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Name', 'name', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Hours (or KM)', 'hours', 'hours', array('' => ''), null, array('MachineryService', 'printHours()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Hours id', 'hours_id', 'hours_id', array('' => ''), null, null));
        if ($mode == 'default') {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', 'Action', 'Action', array('' => ''), null, array('MachineQuery', 'printActionButtons2()')));
        }

        print("<h3>Machine Hours Log</h3>");
        print($dg->render());
        if ($mode == 'default') {
            $dg->pager();
        }
    }

//datagrid customizations.....
    function printHours($params) {
        extract($params);
        $number = substr($record['hours'], 0, strpos($record['hours'], ':00:00'));
        return $number;
    }

    function printFormattedTime($params) {
        extract($params);
        return date("M d Y", strtotime($record['event_time']));
    }

    function printName($params) {
        extract($params);
        $ser = ''; //printer serial number if not null.
        if ($record['serial_num'] != null) {
            $ser = '<br/><br/>ser#: ' . $record['serial_num'];
        }

        return '<div id=\'largish\'>' . $record['name'] . '</div>' . '' . ($record['latest_hours']) . 'hrs @ ' . date("M d, Y", strtotime($record['latest_hours_time'])) . $ser;
    }

    function printLastServiceInfo($params) {
        extract($params);

        $comment = '';
        if (($record['last_service_comment'] != null) && ($record['last_service_comment'] != 'null')) {
            $comment = ' ' . $record['last_service_comment'];
        }

        if ($record['last_service_time'] == null) {
            $date = '';
        } else {
            $ftime = date("M d, Y", strtotime($record['last_service_time']));
            $time = strtotime($record['last_service_time']);
            $str = "<b>Serviced</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$record['last_service_hours']} hours as done by {$record['last_service_userid']} &nbsp; &nbsp;$comment";
            return $str;
        }
    }

    function displayServiceAdministeredLog($mode = 'default') {

        $numberPerPage = 40;

        // Setup your database connection
        $sql = "SELECT *,extract(hour FROM hours) as hours FROM machinery.service_administered LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id LEFT JOIN machinery.machine ON machine.id=service_item.machine_id ORDER BY service_administered.event_time DESC";
        $dg = new DataGridLR($sql, $numberPerPage);
        print("<h3>Service Completion Log</h3>");
        print($dg->render());
        if ($mode == 'default') {
            $dg->pager();
        }
    }

  

    private function generateTimeline() {
        $outArr = array();

        $outArr[(string) ( $this->timeEpochNow + 4)] = '<h1>Timeline</h1>';

        $outArr[(string) ( $this->timeEpochNow + 3)] = '<ul class="timeline">'; //put on second in future, so always first.


        $a = new QF2_MachineryComment((object) array(forwardURLArr => array('pageid' => $_REQUEST['pageid'], 'machine_id' => $this->machine_id)), 'group');
        $form = ($a->render('qf_machineryEntry', ''));
        // {$a->render('qf_machineryEntry','')}

        $outArr[(string) ( $this->timeEpochNow + 1 )] = <<< HTMLTOP
    <li>
        <i class="fa fa-laptop bg-aqua"></i>
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> now</span>
            <h3 class="timeline-header no-border"><a href="#">{$GLOBALS['auth']->getUsername()}</a>$form</h3>
        </div>
    </li>
HTMLTOP;



        $commentsArr = $this->processCommentEvents();
        $hoursArr = $this->processHourEvents();
        $serviceArr = $this->processServiceEvents();
        $pictureArr = $this->processPictureEvents();

        //
        if (is_array($commentsArr)) {
            $outArr = ($outArr + $commentsArr);
        }
        //
        if (is_array($hoursArr)) {
            $outArr = ($outArr + $hoursArr);
        }
        //
        if (is_array($serviceArr)) {
            $outArr = ($outArr + $serviceArr);
        }
        //
        if (is_array($pictureArr)) {
            $outArr = ($outArr + $pictureArr);
        }


        //finally add year tags, by finding max and min year. 
        $maxYear = date('Y', (max(array_keys($outArr))));
        $minYear = date('Y', (min(array_keys($outArr))));
        $outArr = ($outArr + $this->timelineYear($minYear, $maxYear));


        $outArr[(string) strtotime('Jan 1 1970')] = '</ul>'; //epoch

        krsort($outArr); // reverse order

        return $outArr;
    }

    private function timelineYear($minYear, $maxYear) {

        for ($x = $minYear; $x <= $maxYear; $x++) {
            $time = strtotime('Jan 1 ' . $x);
            $year = $x - 1;
            $outArr[$time] = <<< HTML
<!-- timeline time label -->
    <li class="time-label">
        <span class="bg-red">
          $year
        </span>
    </li>
<!-- /.timeline-label -->
HTML;
        }

//now do one extra in green for today's date. 
        $timeNow =  $this->timeEpochNow + 2; //now plus 1 second
        $timeNowStr = date('d M Y', $timeNow);

        $outArr[$timeNow] = <<< HTML
<!-- timeline time label -->
    <li class="time-label">
        <span class="bg-green">
           $timeNowStr
        </span>
    </li>
<!-- /.timeline-label -->
HTML;


        return $outArr;
    }

    private function processServiceEvents() {
        $sql = "SELECT service_administered.id,service_administered.event_time,service_administered.userid,service_administered.comment,part_num,service_item.name as name,extract(hour FROM hours_log.hours)::integer as hours "
                . "FROM machinery.service_administered "
                . "LEFT JOIN machinery.service_item ON service_item.id=service_administered.service_item_id "
                . "LEFT JOIN machinery.machine ON machine.id=service_item.machine_id "
                . "LEFT JOIN machinery.hours_log ON hours_log.id=hours_log_id "
                . "WHERE machine.id={$this->machine_id} "
                . "ORDER BY service_administered.event_time DESC";

        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['event_time']);
            $ago = Misc::daysOrHoursAgo($time);
            $userid = ucfirst($row['userid']);

            //bootstrap timeline
            $outArr[(string) $time] = <<< HTML
<li>
        <i class="fa fa-wrench bg-aqua"></i>
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> $ago ago</span>
            <h3 class="timeline-header no-border"><a href="#">$userid</a> {$row['name']} hours</h3>
        </div>
    </li>
HTML;
        }

        return $outArr;
    }

    private function processPictureEvents() {

        $sql = "
        SELECT id,event_time,userid FROM picture.machine_comment WHERE machine_id={$this->machine_id}
              ";
        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['event_time']) + 1;
            $ago = Misc::daysOrHoursAgo($time);
            $userid = ucfirst($row['userid']);
            $pic = ("<img id='machinePic' style='max-height: 300px; max-width: 300px;' src='/functions/displayAnyPic.php?picture_id={$row['id']}'/>");

            if ($time >= strtotime('-1 week')) {
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&machine_id=$this->machine_id";
                $a = new QF2_GenericDeleteButton((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'picture.machine_comment'));
                $deleteButtonAsString = ($a->renderDelete('machinerypicture' . $row['id']));
            } else {
                $deleteButtonAsString = '';
            }


            //bootstrap timeline
            $outArr[(string) $time] = <<< HTML
<li>
      <i class="fa fa-camera bg-purple"></i>     
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> $ago ago</span>
            <h3 class="timeline-header no-border"><a href="#">$userid</a> $pic $deleteButtonAsString</h3>
        </div>
    </li>
HTML;
        }

        return $outArr;
    }

    private function processHourEvents() {
        $sql = "
        SELECT id,event_time,userid,(extract(hour FROM hours))::integer as hours FROM machinery.hours_log WHERE machine_id={$this->machine_id} ORDER BY event_time DESC
              ";
        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['event_time']);
            $ago = Misc::daysOrHoursAgo($time);
            $userid = ucfirst($row['userid']);

            //show delete button for a week.
            if ($time >= strtotime('-1 week')) {
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&machine_id=$this->machine_id";
                $a = new QF2_GenericDeleteButton((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'machinery.hours_log'));
                $deleteButtonAsString = ($a->renderDelete('machineryhour' . $row['id']));
            } else {
                $deleteButtonAsString = '';
            }





            //bootstrap timeline
            $outArr[(string) $time] = <<< HTML
<li>
        <i class="fa fa-hourglass bg-green"></i>
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> $ago ago</span>
            <h3 class="timeline-header no-border"><a href="#">$userid</a> {$row['hours']} hours $deleteButtonAsString</h3>
        </div>
    </li>
HTML;
        }

        return $outArr;
    }

    private function processCommentEvents() {
        $sql = "
           SELECT * from machinery.comment WHERE machine_id={$this->machine_id} ORDER BY event_time DESC
          ";
        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row ['event_time']);
            $ago = Misc::daysOrHoursAgo($time);
            $userid = ucfirst($row['userid']);

            //show delete button for a week.
            if (strtotime($row ['update_time']) >= strtotime('-1 week')) {
                $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&machine_id=$this->machine_id";
                $a = new QF2_GenericDeleteButton((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'machinery.comment'));
                $deleteButtonAsString = ($a->renderDelete('machinerycomment' . $row['id']));
            } else {
                $deleteButtonAsString = '';
            }

            //bootstrap timeline
            $outArr[(string) $time] = <<< HTML
<li>
        <i class="fa fa-comments bg-yellow"></i>
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> $ago ago</span>
            <h3 class="timeline-header no-border"><a href="#">$userid</a> {$row['comment']} $deleteButtonAsString</h3>
        </div>
    </li>
HTML;
        }

        return $outArr;
    }

}

//end class



Class QF2_MachineryComment extends Quickform2Helper {

    function definition($form) {

             $dateArray=Misc::createDailyDatesForLastYearAtMidnight();
             unset($dateArray[null]);//remove deault so default is today.
             
             //
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form->addElement('hidden', 'machine_id')->setValue($_REQUEST['machine_id']);

         if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
          $form->addElement('select', 'event_time', null, array('options' => $dateArray, 'label' => 'Time of Event (optional)'));
         }
         
        $upload = $form->addElement('file', 'imageName', array('style' => 'width: 300px')); // no longer using special 'uploadedfile' rule for uploads, allow client-side validation
        $upload->addRule('regex', 'Allowed extensions:  .jp(e)g', '/\\.(jpe?g)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER); // these don't work client-side, for obvious reasons
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an image', array('image/jpeg'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        $form->addElement('textarea', 'comment', array('rows' => 6, 'style' => 'width: 98%'))->setLabel("Comments : ");


        return $form; //required       
    }

    function process($value) {
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();


            //allow owner to set any time for comment. 
            if (!empty($value['event_time'])) {
                $value['event_time'] = date('r', ($value['event_time']));
            } else {
                $value['event_time'] = date('r',  $this->timeEpochNow);
            }
                     
            if (!empty($value['comment'])) {
                //we have a comment, so insert it. 
                $query = "INSERT INTO machinery.comment (machine_id,event_time,comment,userid) VALUES (:machine_id,:event_time,:comment,:userid)";
                $res = $GLOBALS['pdo']->prepare($query);
                $res->bindValue(':machine_id', $value['machine_id'], PDO::PARAM_INT);
                $res->bindValue(':event_time', $value['event_time'],PDO::PARAM_STR);
                $res->bindValue(':comment', $value['comment'], PDO::PARAM_STR);
                $res->bindValue(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
                $res->execute();
               
            }

            if (is_uploaded_file($_FILES['imageName']['tmp_name'])) {

                $tmpFileName = $_FILES['imageName']['tmp_name'];
                $data = resizeJPEG($tmpFileName);  //change pic size to smaller
                $escaped_data = pg_escape_bytea($data); //escape data

                $query2 = "INSERT INTO picture.machine_comment (machine_id,event_time,picture,userid) VALUES ({$value['machine_id']},'{$value['event_time']}','$escaped_data'::bytea,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS['pdo']->exec($query2);
            } else {
                //do nothing, not uploaded.
            }





            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
        // Close the transaction
    }

}

//end class

require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';         
Class QF2_MachineHours extends Quickform2Helper {

    function definition($form) {

      
         $sql = "SELECT id,name FROM machinery.machine WHERE id={$this->param->machine_id} ORDER BY name";
        //grab a list of machines
        $res = $GLOBALS['pdo']->query($sql);
        $machineArray = array(null => 'Please Select One');
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $machineArray[$row['id']] = $row['name'];
        }


        $dateArray=Misc::createDailyDatesForLastFewDays();
        
        $hoursArray = array(null => 'Please Select One');
        for ($counter = 10; $counter <= 15000; $counter += 10) {
            $hoursArray[$counter] = "$counter hours";
        }
        for ($counter = 15100; $counter <= 100000; $counter += 100) {
            $hoursArray[$counter] = "$counter hours";
        }
     


       $fieldset = $form->addFieldset('header')->setLabel('Record Machine Hours Event');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
       
            $form->addElement('hidden', 'machine_id')->setValue($this->param->machine_id);
       
        $form->addElement('select', 'hours', null, array('options' => $hoursArray, 'label' => 'Number of Hours'))->addRule("required", "Please specify your number of hours.");
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of hours entry.");

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);

        //don't put id value, uses next one in DB.
          $query = "INSERT INTO machinery.hours_log (machine_id,hours,event_time,userid) VALUES ({$value['machine_id']},'{$value['hours']} hours','{$value['event_time']}','{$GLOBALS['auth']->getUsername()}')";

             $res = $GLOBALS['pdo']->exec($query);
    }

}//end class


?>