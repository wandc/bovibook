<?php 
/** 
 * shows notifications and errors from the site. */

include_once($_SERVER['DOCUMENT_ROOT'].'global.php'); //need to include here for javascript refresh.
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');




class SideBarInfo {
    /*
Info! This alert box indicates a neutral informative change or action.
×Warning! This alert box indicates a warning that might need attention.
×Danger! This alert box indicates a dangerous or potentially negative action.
     */
          public $infoCount=0; //level 3
          public $warningCount=0; //level 2
          public $dangerCount=0; //level 1


    //api and main act as constructors.
    public function __construct() {
      
        $res2 = $GLOBALS['pdo']->query("select level as alert_level, count(level) AS number_occurences from  batch.error_curr group by level");
         while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {

        switch ($row2['alert_level']) {
    case 1:
        $this->dangerCount=$row2['number_occurences'];
        break;
    case 2:
         $this->warningCount=$row2['number_occurences'];
         break;
    case 3:
        $this->infoCount=$row2['number_occurences'];
        break;
}
                
                
         }
       
    }
    
    //level is 1,2,3 
  public function content($level=1) {
    
        $content='';
        $res2 = $GLOBALS['pdo']->query("SELECT event_time,level,detail,extract ('epoch' from (SELECT current_timestamp - max(update_time) FROM batch.error_curr )) as estimated_last_update FROM batch.error_curr WHERE level=$level ORDER BY level DESC ,event_time");
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $errorToAdd=null;
            
            $errorToAdd='<li class="smallish">'.'<b>'.date('M j, ga',strtotime($row2['event_time'])) .'</b>'.': '. $row2['detail'].'</li>';
            
            /*
            //when it's level 0, make it red. super important error.
            if ($row2['level']==99) {
            $errorToAdd='<b id="boldRed">'.$errorToAdd.'</b>';
            }
            
             if ($row2['level']==95) {
            $errorToAdd='<b id="lightRed">'.$errorToAdd.'</b>';
            }
            */
            $content=$content. $errorToAdd;
            $estimated_last_update=$row2['estimated_last_update'];
        }
        
        //check to see if the db table hasn't been updated in a while.
       if ($estimated_last_update >= 2000) {
          $content='<li class="smallish">'.'<b>'.'NOTE: Data Probably Out of Date. (fix error checking script)' .'</b>'.'</li>'.$content;  //put at top so we see it.
       }
        
      return $content;   
    }
  
}//end class