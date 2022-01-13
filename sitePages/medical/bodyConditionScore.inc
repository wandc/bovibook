<?php

/**
 * 
 */
class BodyConditionScore extends BasePage {

    
     public static $bovineID;
    public static $mode;

    /* in case of individual cow treatments, we use bovine_id. */

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
     
           $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$this->displayBodyCondQuickForm(self::$bovineID)}
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayBodyCondActionList()}
            {$this->displayBodyCondCasesLog()}
        </div>
    </div>    
HTML;
    
        print($html);
    }

    public static function currentBCSforAnimal($bovineID) {
        $sql = "SELECT event_time,score,comment,userid FROM bovinemanagement.body_condition_score_curr WHERE bovine_id=$bovineID";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($res->rowCount() == 1) {
            return '<b>Body Con. Score:</b> ' . $row['score'] . ' <span class="smallish">(' . date('M d, Y', strtotime($row['event_time'])) . ')</span>';
        } else {
            return '<b>Body Con. Score: </b>???';
        }
    }

    function displayBodyCondQuickForm($bovine_id=null) {
        //only do this for milking cows.
        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines();

        $conditionScores = array();
        //condition scores at 0.1 intervals
        for ($i = 0; $i <= 40; $i++) {
            $num = 1 + $i * 0.1;
            $conditionScores["$num"] = $num;
        }

        $timeArray = $GLOBALS['MiscObj']->createDailyDatesForLastSevenDays();
        unset($timeArray[null]); //remove null value.

        $form = new HTML_QuickForm("bodyCondQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'bodyCondQuickForm', "Body Condition Score");
        $form->addElement('select', 'animalNumber', 'Animal', $aniNumber);
        $form->addElement('select', 'event_time', 'Event Time', $timeArray);
        $form->addElement('select', 'score', 'Score', $conditionScores);
        $form->addElement('text', 'comment', 'Comment (optional)');
        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id', $bovine_id);
        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');
        //
        $form->addRule('animalNumber', 'Animal number is required', 'required');
        $form->addRule('event_time', 'Date is required', 'required');
        $form->addRule('score', 'Score is required', 'required');
        $form->addRule('score', 'Score must be a number', 'numeric');


// Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('animalNumber');
            $score = $form->exportValue('score');
            $comment = 'NULL'; //this will be replaced with real comment if there is one.
            $comment = $form->exportValue('comment');
            //
            //
			$tempArray = $form->getElementValue('event_time');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);

            $pageid = $form->exportValue('pageid');

// Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();


            $query = "INSERT INTO bovinemanagement.body_condition_score_event
				(bovine_id,event_time,score,comment,userid)
		                VALUES ($bovine_id,'$event_time',$score,'$comment','{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);
            $medical_case_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.medical_generic_id_seq');


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
        return $renderer->toHtml();
    }

    //list any cows who have not been body scored after being 60 DIM.
    function displayBodyCondActionList($mode = 'default') {


      

        //animals over 60 days in milk who have not been scored.
        //if animale is scored past fresh date, then not on list.
        //takes the most recent score into acount after fresh date, not
        //needed for this, but use query for a who's done list in future.
        $sql = "
		With temp as (
SELECT bovinecurr.id,bovinecurr.full_name,bovinecurr.local_number,age(fresh_date) ,location_name,location_id,
(SELECT body_condition_score_event.score FROM bovinemanagement.body_condition_score_event WHERE body_condition_score_event.bovine_id=bovinecurr.id AND body_condition_score_event.event_time=(SELECT max(event_time) FROM bovinemanagement.body_condition_score_event WHERE body_condition_score_event.bovine_id=bovinecurr.id AND body_condition_score_event.event_time> fresh_date)) as current_BCS
FROM bovinemanagement.bovinecurr
WHERE fresh_date IS NOT null AND (age(fresh_date) >= interval '60 days') 
)
SELECT *,temp.id as bovine_id FROM temp WHERE current_BCS is null ORDER BY location_id DESC,local_number ASC
		";

            $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            
              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
              $rowOut['Location'] = ($row['location_name']);
             
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Need to be BC Scored</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'order: false')); 
         return implode($out); 
        
    }

    /*     * ******************************* */
    /* Display list of all bodyCond cases */
    /*     * ******************************* */

    function displayBodyCondCasesLog($mode = 'default') {

        $sql = "SELECT body_condition_score_event.id as body_condition_score_event_id,body_condition_score_event.bovine_id,body_condition_score_event.event_time,body_condition_score_event.score,local_number,full_name,comment,userid,locationcurr.name  as location_name 
                FROM bovinemanagement.body_condition_score_event		
		LEFT JOIN bovinemanagement.bovine ON bovine.id = body_condition_score_event.bovine_id
                LEFT JOIN bovinemanagement.locationcurr ON locationcurr.bovine_id = bovine.id
		WHERE bovine.death_date is null
		ORDER BY body_condition_score_event.event_time DESC";

        
          $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
 
            
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Location'] = ($row['location_name']);
            $rowOut['Score'] = ($row['score']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
         //var_dump($dataArr);
         $out[]=("<h3>Body Condition Score Log</h3>");
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'order: false')); 
         return implode($out); 
        
    }

    function printActionButtons($record,$callBackURL) {
    
        $currentUserid = $GLOBALS ['auth']->getUsername();


        $res = $GLOBALS ['pdo']->query("SELECT id,'body_condition_score_event' as tablename FROM bovinemanagement.body_condition_score_event WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
$returnButtonsAsString='';
        if (($record ['body_condition_score_event_id'] == $row ['id'])) {
            $returnButtonsAsString = self::deleteQuickForm($row ['id'], $row ['tablename'],$callBackURL); // show
            //delete button, with _____ table id as index for delete.
        }

        return $returnButtonsAsString;
    }

    function deleteQuickForm($tableid, $tablename,$callBackURL) {

        $form = new HTML_QuickForm("genericEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete',array('class'=>'btn btn-warning'));

        // FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY
        // USER.
        // DB hack works though, views need to support deletes before this can
        // be fixed.
        // Try to validate a form
        if ($form->validate()) {

            // get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');

            // Open a transaction
            try{$res = $GLOBALS ['pdo']->beginTransaction();

            // delete the dryoff_event
            $res = $GLOBALS ['pdo']->query("DELETE FROM bovinemanagement.$tablename where id='$tableid'");

            // determine if the commit or rollback
             $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }

            header($callBackURL);
            exit();
        }
        $form->accept($renderer);
        return ($renderer->toHtml()); // pass on as html string to display later in the
        // table
    }

    /**
     * Error notifier
     */
    public function error() {
        
    }

}//end class	
?>