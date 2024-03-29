<?php
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
        print($this->displayFootActionList());
        print("</div>");
        print("</div>");
    }

    protected function tab2() {
        print($this->displayFootCasesLog());
    }

    protected function tab3() {

        $this->displayMilk2020TrimEvents();
    }

    protected function tab4() {

        $this->displayNotTrimmedInLast6Months();
    }

    function displayFootActionList($mode = 'default') {
        
        
        
        //complicated sql because we want to select by certain conditions and then order it by event_time
        //only show events in the last 90days. The union allows lame events to show longer if necessary.
        
        $sql = <<<SQL
	with footcurrsingle as (
SELECT DISTINCT ON (footcurr.bovine_id) local_number,full_name,footcurr.bovine_id,footcurr.foot_event_id,foot,footcurr.name,foot_type_id,userid,footcurr.comment,footcurr.event_time as event_time,locationcurr.name as location_name FROM bovinemanagement.footcurr 
                LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id = footcurr.bovine_id
                WHERE footcurr.foot_type_id != 8
		ORDER BY footcurr.bovine_id,footcurr.event_time
)
SELECT * FROM footcurrsingle WHERE (name='Lame' OR name='Foot Rot' OR name='Cut') AND event_time >= (now() - interval '90 days')  
UNION
SELECT * FROM footcurrsingle WHERE (name!='Lame' OR name!='Foot Rot' OR name!='Cut') AND event_time >= (now() - interval '90 days')  ORDER BY event_time DESC ,local_number

SQL;

        
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {
          
             $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
             $rowOut['Location'] = ($row['location_name']);
             $rowOut['Foot Action'] = $this->printFootAction($row);
             $rowOut['colour_row'] =  (!empty($row['foot_type_id']) ? 1 : 0); //this really doesn't do anything.
          
          $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;
    
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Foot Action List', $dataArr)); 
         return implode($out); 
      
    }

    /*     * ******************************* */
    /* Display list of all foot cases */
    /*     * ******************************* */

    function displayFootCasesLog($mode = 'default') {
        
        
         $sql = <<<SQL
SELECT foot_event.id as foot_event_id,foot_event.bovine_id,foot_event.event_time,local_number,full_name,foot,foot_type.name as diagnosis,comment,userid,locationcurr.name FROM bovinemanagement.foot_event
		LEFT JOIN bovinemanagement.foot_type ON foot_event.foot_type_id = foot_type.id
		LEFT JOIN bovinemanagement.bovine ON bovine.id = foot_event.bovine_id
                LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id = bovine.id
		WHERE bovine.death_date is null
		ORDER BY foot_event.event_time DESC
SQL;

        
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Action/Diagnosis'] = ($row['diagnosis']);
            $rowOut['Which Foot'] = ($row['foot']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Location'] = ($row['name']);
            $rowOut['Employee'] = ($row['userid']);

            if ($mode == 'default') {
                $rowOut['Action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            }

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
    
         $out[]=("<h3>Foot Log</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr)); 
         return implode($out); 
        
    }

    function displayFootQuickForm($mode = 'group', $bovine_id = null) {

        //grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovinecurr ORDER BY local_number");

        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
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

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay();
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
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&bovine_id={$this->bovine_id}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

//customizations.....


    /* print the latest events for the different feet if the event is not cured, in which case nothing is printed. */
    function printFootAction($record) {
      

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

    function printActionButtons($record,$callBackURL) {

        $returnButtonsAsString = '';
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        //print latest delete button (one) for current user
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.foot_event WHERE userid='$currentUserid' ORDER BY event_time DESC LIMIT 1");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($record['foot_event_id'] == $row['id']) {
                $returnButtonsAsString = self::deleteQuickForm($record['foot_event_id'],$callBackURL); //show delete button, with Breeding table id as index for delete.
            }
        }

        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    function deleteQuickForm($footEventID,$callBackURL) {
        $form = new HTML_QuickForm("footEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'footEventID', $footEventID);
        $form->addElement('submit', 'btnDelete', 'Delete',array('class'=>'btn btn-warning'));

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
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }


            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

    /**
     * Error notifier
     */
    public function error() {
        //cows that probably need to have there foot looked at.
        $error = array();
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
        print($dt->start('Title CHANGE ME', 63, 'milk2020TrimEvents', null, $columnsStr));
    }

    public function milk2020TrimEvents($request) {
        $sql = "SELECT  * FROM batch.milk2020_hoof_all order by trim_time DESC ";

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

    public function notTrimmedInLast9Months() {
        $sql = " with temp as (
SELECT * from bovinemanagement.bovinecurr where location_id IN (37,38,39,40,41,42)
)
SELECT *,current_date-fresh_date as dim from temp
LEFT JOIN 
(SELECT  chain_number,trim_time, data->'id'::text as trim_id,data->'username' as trimmer,
data->'reg_num' as reg_number
 FROM batch.milk2020_hoof 
WHERE trim_time >= (current_date - interval '9 months')) subquery
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
        print("<h3>Not Trimmed in the last 9 Months</h3>");
        print($dt->start('Title CHANGE ME', 63, 'notTrimmedInLast9Months', null, $columnsStr));
    }

}

//end class	
?>