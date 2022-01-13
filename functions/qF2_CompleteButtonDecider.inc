<?php

/**
  Used to add complete buttons to datatables.
  This should handle almost all but the most custom cases.

  Called like this:
  //initate outside loop at top
  $delButton=new QF2_CompleteButtonDecider('user_1only', (object) array('completeName' =>'id','dateTimeName'=> 'event_time', 'dateTimePeriodDays' => '7', 'schemaTable'=>'schema.table', 'callBackURL'=>"{$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}"));

  // for each row
  $rowOut['Action']=$delButton->output($row['id']);

  //Basic example for simplest case with single user
  $delButton=new QF2_CompleteButtonDecider('user_1only',(object) array('schemaTable'=>'schema.table'));
  $rowOut['Action']=$delButton->output($row['id']);


  //another example for time case for single user where key field is "id" but we use update_time
  $delButton=new QF2_CompleteButtonDecider('user_timeLimit',(object) array('dateTimeName'=> 'update_time', 'schemaTable'=>'schema.table'));
  $rowOut['Action']=$delButton->output($row['id']);

 */
Class QF2_CompleteButtonDecider {

    private $completeName;    //id is default, but can use anything.
    private $dateTimeName;  //event_time is default, but can use update_time, etc. instead
    private $dateTimePeriodDays; //7 days is the default, but can use anything.
    private $schemaTable; //MUST be specified. 
    private $callBackURL; //one will be generated that will work in most cases, or supple one MINUS 'location :'
    private $useridColumnName; //defaults to userid, but might be shceduled_userid, etc. instead.
    private $buttonClassName; //string name of class the is the actual complete button class, defaults to QF2_CompleteButtonGeneric; MUST BE A CHILD OF QF2_CompleteButtonGeneric OTHERWISE.
    
    
    /*
     * Different cases this supports:
     * 1. admin - can complete anything, in effect always
     * AND
     * A. user 1 only - can complete last id only. 
     * B. user time only - can complete all last id's for a time period for that user (usually 7 days). Also includes all future dates. 
     * C. any user 1 only - can complete last entered id, that was entered by anyone.
     * D. any user time only - can complete all last id's for a time period for any user (usually 7 days). Also includes all future dates. 
     * 
     * A: need to provide id to search for. 
     * B: need to provide dateTimeName field to use as paramater.
     * C: need to provide id to search for. 
     * D: need to provide dateTimeName field to use as paramater. 
     * 
     * mode=user_1only $completeID
     * mode=user_timeLimit
     * mode=anyuser_1only
     * mode=anyuser_timeLimit
     */

    //call this at top outside of loop to first find out which "key id"(s) are eligible to complete      
    public function __construct($mode = 'user_1only', object $optionsObj) {

        $resultArr = array();
        $currentUserid = $GLOBALS['auth']->getUsername(); //get current user
        //setup defaults
        $this->completeName = empty($optionsObj->completeName) ? 'id' : $optionsObj->completeName;
        $this->dateTimeName = empty($optionsObj->dateTimeName) ? 'event_time' : $optionsObj->dateTimeName;
        $this->dateTimePeriodDays = empty($optionsObj->dateTimePeriodDays) ? '7' : $optionsObj->dateTimePeriodDays;
        $this->completeIDArr = array();
        $this->useridColumnName = empty($optionsObj->useridColumnName) ? 'userid' : $optionsObj->useridColumnName;
        $this->buttonClassName = empty($optionsObj->buttonClassName) ? 'QF2_CompleteButtonGeneric' : $optionsObj->buttonClassName;
        
        if (empty($optionsObj->schemaTable)) {
            throw new Exception('Error schemaTable cannot be empty or there is nothing to do' . '/n/r');
        }
        $this->schemaTable=$optionsObj->schemaTable;
        
        //adds the location part always. if empty it sends a generic URL, should work in most cases.
        if (empty($_REQUEST['pageid'])) {
            throw new Excetion('Error page id cant be empty.');
        }
        //optional fields
        $bovine_id_URL = empty($_REQUEST['bovine_id']) ? '' : "&bovine_id={$_REQUEST['bovine_id']}";
        $field_id_URL = empty($_REQUEST['field_id']) ? '' : "&field_id={$_REQUEST['field_id']}";
        $machine_id_URL = empty($_REQUEST['machine_id']) ? '' : "&machine_id={$_REQUEST['machine_id']}";
        //generate
        $this->callBackURL = empty($optionsObj->callBackURL) ? "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}$bovine_id_URL$field_id_URL$machine_id_URL" : "Location: " . $optionsObj->callBackURL;



        /* since the SQL will fail if any of these are missing, no real point in checking for errors */

        /* for complete we really only need to worry about WHEN to show the button, not to WHO. We only want to show it +/- so many hours from when it can be seen. */

        switch ($mode) {
    
            case "anyuser_timeLimit":

                $sql = <<<SQL
    SELECT {$this->completeName} FROM {$this->schemaTable} WHERE current_timestamp BETWEEN (scheduled_event_time - interval '4 hours ') AND (scheduled_event_time + interval '12 hours ') ORDER BY {$this->dateTimeName} DESC 
SQL;
                break;
            default:
                throw new Exception('Error wrong mode used. Instead we got:' . $mode . '\n\r');
        }

        //var_dump($sql);
        //run the SQL and get a resulting array of key values to use to compare for complete later.      
        $res = $GLOBALS['pdo']->query($sql);
        $resultArr = $res->fetchAll(PDO::FETCH_COLUMN, 0);
        $this->completeIDArr=$resultArr;
        
        return true; //completed
    }

//this generates the button (or not) after the setup is run. 
//we only need to pass the id, because everything else is defined in the setup once. 
    //overrideCompleteButtonQF2 used when the generic button can't do the job.
    public function output($completeIDForCurrentRow,$overrideCompleteButtonQF2=null,$icon=null) {
        $completeButtonAsString = '';
        $showButton = false;

       //cant use because sometimes there is nothing to complete.. 
      //  if (empty($this->completeIDArr)) {
     //       throw new Exception('Error completeIDArr is empty, did you forget to initialize with setup?\n\r');
     //   }


        //if admin is on we always show a button.
        if ($GLOBALS['auth']->getAdminMode() == 1) {
            $showButton = true;
        }

        //if matches we show a button. 
        if (in_array($completeIDForCurrentRow, $this->completeIDArr) == true) {
            $showButton = true;
        }

        
        if ($showButton == true) {

            if (!empty($overrideCompleteButtonQF2)) {
                $completeButtonAsString = $overrideCompleteButtonQF2; //ouputs
            } else {
                //create button
                $buttonClassName=$this->buttonClassName;
                $a = new $buttonClassName((object) array('callBackURL' => $this->callBackURL), 'individual', (object) array('completeID' => $completeIDForCurrentRow, 'completeName' => $this->completeName, 'schemaTable' => $this->schemaTable));
                $completeButtonAsString = ($a->renderComplete($completeIDForCurrentRow,null,$icon));
            }
        }
        return $completeButtonAsString;
    }

}

//end class
?>