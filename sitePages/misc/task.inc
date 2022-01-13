<?php 



class Task extends TabPage {

    public function defaultDisplay() {
        $this->displayScore();

        //setup for tabs.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $tabArray[1]['name'] = 'Tasks Needing Completion';

if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $tabArray[2]['name'] = 'All Users Task List';
            $tabArray[3]['name'] = 'Log';
            $tabArray[4]['name'] = 'Scores';
}
        }
        $this->renderTabs($tabArray);
    }

    function tab1() {
        $this->displayListOfTasksToDo('single');
    }

    function tab2() {
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $this->displayListOfTasksToDo('all');
        }
    }

    function tab3() {
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $this->displayTaskLog();
        }
    }

    function tab4() {
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $this->displayScoreLog();
        }
    }

    function displayScore() {

        //calculates the total time spent on tasks per quarter, limits it to only looking at the current year and then breaks it down so it only returns the current quarter.
        $sql = "
  WITH temp as (
SELECT count(task_completed.create_time) as total_events, sum(extract(epoch from time_to_complete)/60) as total_time,EXTRACT(QUARTER FROM task_completed.event_time) as quarter
FROM misc.task_completed 
LEFT JOIN misc.task ON task_completed.task_id=task.id 
WHERE  userid='{$GLOBALS['auth']->getUsername()}' AND task_completed.event_time >= (extract(year from current_date) || '-01-01')::date
GROUP BY quarter
)
SELECT * FROM temp  WHERE quarter=(extract(quarter from current_date)) LIMIT 1;

";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $score = $row['total_time'];
        print("Your task score this quarter: <b>$score</b>.");
    }

//this sql shows the total scores for everyone for the current year.
    function displayScoreLog($mode = 'default') {



        $sql = "
	               WITH temp as (
SELECT userid,count(task_completed.create_time) as total_events, sum(extract(epoch from time_to_complete)/60) as total_time,EXTRACT(QUARTER FROM task_completed.event_time) as quarter
FROM misc.task_completed 
LEFT JOIN misc.task ON task_completed.task_id=task.id 
WHERE  task_completed.event_time >= (extract(year from current_date) || '-01-01')::date
GROUP BY userid,quarter ORDER BY quarter,userid
)
SELECT * FROM temp ;
		";
          print( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql)); 
        
    }

    private function displayTaskLog($mode = 'default') {


        $sql = "
	              SELECT * FROM misc.task_completed LEFT JOIN misc.task ON task.id=task_completed.task_id ORDER BY event_time DESC,name
		";

        //Name  Userid   Event Time Comment
                print( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql)); 
       
    }

    private function displayListOfTasksToDo($mode = 'single') {

        if ($mode == 'single') {
            $sql = "
	SELECT *,extract(epoch from interval)::numeric/extract(epoch from last_completion_elapsed_time)::numeric as last_completion_to_interval_ratio FROM misc.taskcurr
        WHERE assigned_userid='{$GLOBALS['auth']->getUsername()}'
		ORDER BY assigned_userid,last_completion_to_interval_ratio DESC
                ";
        } elseif ($mode = 'all') {
            $sql = "
	SELECT *,extract(epoch from interval)::numeric/extract(epoch from last_completion_elapsed_time)::numeric as last_completion_to_interval_ratio FROM misc.taskcurr
		ORDER BY assigned_userid,last_completion_to_interval_ratio DESC
                ";
        }

        // Name Completion Time How Often Details Last Completed Completed? COLOUR ROWS
          print( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql)); 
        
    }

    function printLastCompleted($params) {
        extract($params);

        $comment = '';

        if (($record['last_completion_comment'] != null) && ($record['last_completion_comment'] != 'null')) {
            $comment = ' comment: ' . $record['last_completion_comment'];
        }

        if ($record['last_completion_event_time'] != null) {


            $event_time = date('D M d', strtotime($record['last_completion_event_time']));

            $userid = $record['last_completion_userid'];

            $returnString = $event_time . ' by ' . $userid . $comment;
        } else {
            $returnString = 'NEVER';
        }

        //if it's bveen more than double the time since we were suppose to complete the task, blink it.
        if (($record['last_completion_to_interval_ratio'] <= .5) AND ( $record['last_completion_to_interval_ratio'] > 0)) {

            $returnString = '<b id="boldBlue"><b id="boldBlink">' . $returnString . '</b></b>';
        }


        return $returnString;
    }

    function printActionButtons($params) {
        extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();

        $returnButtonsAsString = self::taskCompletedQuickForm($record['id']); //show form
        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    function taskCompletedQuickForm($task_id) {

        $timeArray = $GLOBALS['MiscObj']->createHourlyDatesForOnlyToday();
        unset($timeArray[null]); //remove null value.
        $comment2 = '';
        $form = new HTML_QuickForm("taskCompletedQuickFormButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'task_id', $task_id);
        $form->addElement('select', 'event_time', 'Event Time', $timeArray);
        $form->addElement('text', 'comment2', 'Comment', $comment2);

        $form->addElement('submit', 'btnCompleted', 'Completed');



        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $task_id = $form->exportValue('task_id');
            $tempArray = $form->getElementValue('event_time');
            $event_time = date('Y-m-d H:i:sO', $tempArray[0]);
            $comment2 = trim($form->getElementValue('comment2'));

            if (($comment2 == null) || ($comment2 == '')) {
                $commentFinal = 'null';
            } else {
                $commentFinal = $comment2;
            }

            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

                //delete the dryoff_event
                $query = "INSERT INTO misc.task_completed (task_id,userid,event_time,comment) VALUES ($task_id,'{$GLOBALS['auth']->getUsername()}','$event_time','$commentFinal')";
                $res = $GLOBALS['pdo']->query($query);



                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }



            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

}

//end class
?>