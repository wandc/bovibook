<?php





require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
;

//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if (class_exists('DataGridLR_Ext') != true) { //FFDA7F
    //extends datgrid class with specific logic to colour the line.

    class DataGridLR_Ext extends DataGridLR {

        function colourLineCriteria($row) {
            /*
              $sql = "SELECT name FROM bovinemanagement.foot_event LEFT JOIN bovinemanagement.foot_type ON foot_type.id=foot_event.foot_type_id WHERE foot_event.id={$row['foot_event_id']}";
              $res2 = $GLOBALS['pdo']->query($sql);
              $row2 = $res2->fetch(PDO::FETCH_ASSOC);
             */
            if (($row['name'] == 'Lame') || ($row['name'] == 'Foot Rot') || ($row['name'] == 'Cut')) {
                return true;
            } else {
                return false;
            }
        }

    }

}

class Feet extends TabPage {

    public static $bovineID;
    public static $mode;

    public function __construct($bovineID = null) {
        parent::__construct();

        if ($bovineID != null) {
            self::$bovineID = $bovineID;
            self::$mode = 'individual';
        } else {
            self::$bovineID = null;
            self::$mode = 'group';
        }
    }

    public function defaultDisplay() {
        //setup for tabs.
        $tabArray[1]['name'] = 'Trim';
        $tabArray[2]['name'] = 'Log';
        $tabArray[3]['name'] = 'Milk2020';
        $tabArray[4]['name'] = 'Needs Trimmed';

        $this->renderTabs($tabArray);
    }

    protected function tab1() {
        print("<div class='row'>");

        print("<div class='col-sm-4 col-sm-push-8'>");
        $this->displayFootQuickForm(self::$mode, self::$bovineID);
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
        $this->displayFootActionList();
        print("</div>");
        print("</div>");
    }

    protected function tab2() {
        $this->displayFootCasesLog();
    }

    protected function tab3() {

        $this->displayMilk2020TrimEvents();
    }

    protected function tab4() {

        $this->displayNotTrimmedInLast6Months();
    }

    function displayFootActionList($mode = 'default') {
        $numberPerPage = 30;

        //complicated sql because we want to select by certain conditions and then order it by event_time
        //only show events in the last 90days. The union allows lame events to show longer if necessary.
        $sql = "
		with footcurrsingle as (
SELECT DISTINCT ON (footcurr.bovine_id) local_number,full_name,footcurr.bovine_id,footcurr.foot_event_id,foot,footcurr.name,foot_type_id,userid,footcurr.comment,footcurr.event_time as event_time,locationcurr.name as location_name FROM bovinemanagement.footcurr 
                LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id = footcurr.bovine_id
                WHERE footcurr.foot_type_id != 8
		ORDER BY footcurr.bovine_id,footcurr.event_time
)
SELECT * FROM footcurrsingle WHERE (name='Lame' OR name='Foot Rot' OR name='Cut') AND event_time >= (now() - interval '90 days')  
UNION
SELECT * FROM footcurrsingle WHERE (name!='Lame' OR name!='Foot Rot' OR name!='Cut') AND event_time >= (now() - interval '90 days')  ORDER BY event_time DESC ,local_number

		";


        $dg = new DataGridLR_Ext($sql, $numberPerPage);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Location', 'location_name', 'location_name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Foot Action', null, '', array('' => ''), null, array('Feet', 'printFootAction()')));

        print("<h3>Foot Action List</h3>");
        if ($mode == 'default') {
            print($dg->render('datagrid', true, $GLOBALS['config']['COLOUR']['important']));      //change line color to red for feet that have not been looked at.
            $dg->pager();
        }
    }

    /*     * ******************************* */
    /* Display list of all foot cases */
    /*     * ******************************* */

    function displayFootCasesLog($mode = 'default') {
        $numberPerPage = 30;

        $sql = "SELECT foot_event.id as foot_event_id,foot_event.bovine_id,foot_event.event_time,local_number,full_name,foot,foot_type.name as diagnosis,comment,userid,locationcurr.name FROM bovinemanagement.foot_event
		LEFT JOIN bovinemanagement.foot_type ON foot_event.foot_type_id = foot_type.id
		LEFT JOIN bovinemanagement.bovine ON bovine.id = foot_event.bovine_id
                LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id = bovine.id
		WHERE bovine.death_date is null
		ORDER BY foot_event.event_time DESC";

        $dg = new DataGridLR($sql, $numberPerPage);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedTime()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action/Diagnosis', 'diagnosis', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Which Foot', 'foot', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Comment', 'comment', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Location', 'name', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', '', array('' => ''), null, null));
        if ($mode == 'default') {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('Feet', 'printActionButtons()')));
        }


        print("<h3>Foot Log</h3>");
        print($dg->render());
        if ($mode == 'default') {
            $dg->pager();
        }
    }

    function displayFootQuickForm($mode = 'group', $bovine_id = null) {

        //grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovinecurr ORDER BY local_number");

        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . Misc::femaleShortName($row[1]);
        }

        //get different types of things that can happen to foot.
        $res = $GLOBALS['pdo']->query("SELECT id,name from bovinemanagement.foot_type");

        while (($row = $res->fetch())) {
            $actionArray[$row[0]] = $row[1];
        }

        //grab a list of Medicines'.
        $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit from bovinemanagement.medicine WHERE for_mastitis=true");

        while (($row = $res->fetch())) {
            $medArray[$row[0]] = $row[1];
        }

        $timeArray = Misc::createHourlyDatesForLastDay();
        unset($timeArray[null]); //remove null value.

        $form = new HTML_QuickForm("footQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'footQuickForm', "Foot Quick Treat");
        $form->addElement('select', 'animal', 'Animal', $aniNumber);
        $form->addElement('select', 'action', 'Action', $actionArray);
        $form->addElement('text', 'comment', 'Comment (optional)', $actionArray);

        $feetFront[] =  $form->createElement('advcheckbox', 'front left', 'LF');
        $feetFront[] =  $form->createElement('advcheckbox', 'front right', 'RF');
        $feetBack[] =  $form->createElement('advcheckbox', 'rear left', 'LR');
        $feetBack[] =  $form->createElement('advcheckbox', 'rear right', 'RR');
        $form->addGroup($feetFront, 'feetFront', 'Left Front', '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>Right Front</b>');
        $form->addGroup($feetBack, 'feetBack', 'Left Hind', '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>Right Hind</b>');

        $form->addElement('select', 'event_time', 'Event Time', $timeArray);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

// Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('animal');
            $foot_type_id = $form->exportValue('action');
            $comment = 'NULL'; //this will be replaced with real comment if there is one.
            $comment = $form->exportValue('comment');
            //

            $feetFront = $form->getElementValue('feetFront');
            $feetBack = $form->getElementValue('feetBack');

            $footArray['front left'] = $feetFront['front left'];
            $footArray['front right'] = $feetFront['front right'];
            $footArray['rear left'] = $feetBack['rear left'];
            $footArray['rear right'] = $feetBack['rear right'];

            //if no feet were selected assume it is for the cow in general.		
            if (array_sum($footArray) == 0) {
                $footArray['general'] = 1;
            }

            //
            $tempArray = $form->getElementValue('event_time');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);

            $pageid = $form->exportValue('pageid');

// Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //enter foot case.
                foreach ($footArray as $key => $value) {
                    if ($value == 1) { //only do if the value is true
                        $whichFoot = "$key";

                        $query = "INSERT INTO bovinemanagement.foot_event
				(bovine_id,event_time,foot_type_id,comment,foot,userid)
		                VALUES ($bovine_id,'$event_time',$foot_type_id,'$comment','$whichFoot','{$GLOBALS['auth']->getUsername()}')";
                        $res = $GLOBALS['pdo']->exec($query);
                        $medical_case_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.foot_diagnosis_event_id_seq');
                    }
                }

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id={$this->bovine_id}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

//datagrid customizations.....


    /* print the latest events for the different feet if the event is not cured, in which case nothing is printed. */
    function printFootAction($params) {
        extract($params);

        $sql = "SELECT * FROM bovinemanagement.footcurr WHERE bovine_id={$record['bovine_id']} ORDER BY event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $retStr = "";
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $date = date("M d", strtotime($row['event_time']));
            $comment = "";
            if ($record['comment'] != null) {
                $comment = ' - ' . $record['comment'];
            }
            $retStr = $retStr . "$date {$row['foot']} <b>{$row['name']} $comment</b> by {$row['userid']}<br/>";
        }
        return $retStr;
    }

    function printActionButtons($params) {
        extract($params);
        $returnButtonsAsString = '';
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        //print latest delete button (one) for current user
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.foot_event WHERE userid='$currentUserid' ORDER BY event_time DESC LIMIT 1");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($record['foot_event_id'] == $row['id']) {
                $returnButtonsAsString = self::deleteQuickForm($record['foot_event_id']); //show delete button, with Breeding table id as index for delete.
            }
        }

        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    function deleteQuickForm($footEventID) {
        $form = new HTML_QuickForm("footEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'footEventID', $footEventID);
        $form->addElement('submit', 'btnDelete', 'Delete');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $footEventID = $form->getElementValue('footEventID');

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //delete the dryoff_event
                $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.foot_event WHERE id='$footEventID'");

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }


            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    /**
     * Error notifier
     */
    public function error() {
        //cows that probably need to have there foot looked at.
        $sql = "with footcurrsingle as(
SELECT DISTINCT ON (footcurr.bovine_id) local_number,full_name,footcurr.bovine_id,footcurr.foot_event_id,foot,footcurr.name,foot_type_id,userid,footcurr.comment,footcurr.event_time as event_time,locationcurr.name as location_name FROM bovinemanagement.footcurr 
                LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id = footcurr.bovine_id
                WHERE footcurr.foot_type_id != 8
		ORDER BY footcurr.bovine_id,footcurr.event_time)
SELECT * FROM footcurrsingle WHERE (name='Lame' OR name='Foot Rot' OR name='Cut') AND event_time >= (now() - interval '30 days')";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error["{$row['local_number']}"] = new notifyObj(get_class($this), $row['event_time'], 2, $row['local_number'] . ' needs feet looked at.');
        }


        return $error;
    }

    function displayMilk2020TrimEvents() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
            {
                data: "event_time",
                title: "Trim Time",
                type: "date"
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "trimmer",
                title: "Trimmer",
                type: "text"
            },
        {
                data: "num_lesions",
                title: "num_lesions"
            },
        {
                data: "blocked",
                title: "blocked"
            },
            {
                data: "c",
                title: "c"
            },
        {
                data: "d",
                title: "d"
            },
         {
                data: "e",
                title: "e"
            }, 
        {
                data: "f",
                title: "f"
            }
EX;
        print($dt->start(63, 'milk2020TrimEvents', null, $columnsStr));
    }

    public function milk2020TrimEvents($request) {
        $sql = "SELECT  chain_number,trim_time,
data->'id'::text as trim_id,
COALESCE(data->>'c','') as c,
COALESCE(data->>'d','') as d,
COALESCE(data->>'e','') as e,
COALESCE(data->>'f','') as f,
COALESCE(data->>'g','') as g,
COALESCE(data->>'h','') as h,
COALESCE(data->>'i','') as i,
COALESCE(data->>'k','') as k,
COALESCE(data->>'t','') as t,
COALESCE(data->>'u','') as u,
COALESCE(data->>'v','') as v,
COALESCE(data->>'w','') as w,
COALESCE(data->>'x','') as x,
COALESCE(data->>'z','') as z,
data->'num_lesions' as num_lesions,
data->'blocked' as blocked,
data->'username' as trimmer,
data->'reg_num' as reg_number
 FROM batch.milk2020_hoof order by trim_time DESC ";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = $row['chain_number'];
            //     $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
            $row['event_time_raw'] = strtotime($row['trim_time']);
            //   $row['note'] = $this->reasonsNotToBreed($row['bovine_id'],$row['event_time'],$row['birth_date']);
            //  $row['action'] = $this->printActionButtons2($row['estrus_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['event_time'] = date("l M d Y H:00", strtotime($row['trim_time']));  //has to be at end.
            // if  ((($row['estrus_type_id'])==1) AND empty($row['note']))  {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    public function notTrimmedInLast6Months() {
        $sql = " with temp as (
SELECT * from bovinemanagement.bovinecurr where location_id IN (37,38,39,40,41,42)
)
SELECT *,current_date-fresh_date as dim from temp
LEFT JOIN 
(SELECT  chain_number,trim_time, data->'id'::text as trim_id,data->'username' as trimmer,
data->'reg_num' as reg_number
 FROM batch.milk2020_hoof 
WHERE trim_time >= (current_date - interval '6 months')) subquery
ON subquery.chain_number=temp.local_number
WHERE trim_id is null
ORDER BY location_id,local_number";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
       
        //change presentation
        foreach ($results as &$row) {
       $row['dim'] = round($row['dim']);     
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayNotTrimmedInLast6Months() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
        {
                data: "location_name",
                title: "location"
            },
                 {
                data: "dim",
                title: "DIM"
            }
EX;
        print("<h3>Not Trimmed in the last 6 Months</h3>");
        print($dt->start(63, 'notTrimmedInLast6Months', null, $columnsStr));
    }

}

//end class	
?>