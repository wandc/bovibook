<?php 
require_once($_SERVER['DOCUMENT_ROOT'] . 'functions/queryPageHelper.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/bodyConditionScore.inc');
class BovineQueryMedicalLog {
/* does what it says on the tin */

    private $bovineID;
    private $bovineFullRegNumber;

    

    public function __construct($bovineID=null,$bovineFullRegNumber=null) {
      $this->bovineID=$bovineID;
      $this->bovineFullRegNumber=$bovineFullRegNumber;
    }

// end constructor
    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    
     public function main() {
        $str='';
         // do med events
         $outArray = array();
        $outArray = queryPageHelper::calculateLactationLineBreaks($this->bovineID, $outArray);
        $outArray = $this->processFootEvents($outArray);
        $outArray = $this->processMedEvents($outArray);
          $str=$str. ('<div id="reproTabInfo">');
          $str=$str. $this->showMilkandBeefWithHolding($this->bovineID);
        $str=$str.('<p>'.$this->magenetStatus($this->bovineID).'</p>');
         $str=$str.('<p>'.BodyConditionScore::currentBCSforAnimal($this->bovineID).'</p>');
        ////FIXME: (addme) $str=$str.("<b>Vaccination Status:</b>");
         
        $str=$str. ('</div><!--End Repro Tab Info -->');
          $str=$str.(QueryPageHelper::sortOutArray($outArray));
          return $str;
    }
    
    
     private function processFootEvents($outArray) {
        // ////////////////////////////////////////////////////////////////
        // do all foot events.
        $sql = "SELECT bovine_id,event_time,local_number,full_name,foot,foot_type.name as diagnosis,comment,userid,date_trunc('days',now()-event_time) as days_ago FROM bovinemanagement.foot_event
		LEFT JOIN bovinemanagement.foot_type ON foot_event.foot_type_id = foot_type.id
		LEFT JOIN bovinemanagement.bovine ON bovine.id = foot_event.bovine_id
		WHERE bovine_id={$this->bovineID} ORDER BY event_time DESC";
        $res2 = $GLOBALS ['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row2 ['event_time']);
            $ftime = date('M j, Y', $time);
            $str = "<b>Foot</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> <b>{$row2['foot']}</b> with action/diagnois of <b>{$row2['diagnosis']}</b> and comment \"{$row2['comment']}\" by {$row2['userid']}<br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }

     private function processMedEvents($outArray) {

        //find the latest medical_summary_id for this cow and this user in the last 24 hours.
        //used to find where to show a delete button.
        $currentUserid = $GLOBALS ['auth']->getUsername();
        $sql2 = "SELECT id FROM bovinemanagement.medical_summary WHERE userid='$currentUserid' AND bovine_id={$this->bovineID} AND event_time >= (current_timestamp - interval '1 day') ORDER BY id DESC  LIMIT 1";
        $res88 = $GLOBALS ['pdo']->query($sql2);
        $row88 = $res88->fetch(PDO::FETCH_ASSOC);
        $latestInsertedMedicalId = $row88['id'];

        // /////////////////////////////////////////////////////////////////
        // do all repro hormone therapies
        $sql = "SELECT id,tablename,type,event_time,text,userid FROM bovinemanagement.medical_summary 
		WHERE bovine_id={$this->bovineID} 
		ORDER BY medical_summary.event_time DESC";

        $res10 = $GLOBALS ['pdo']->query($sql);

        while ($row10 = $res10->fetch(PDO::FETCH_ASSOC)) {
            $deleteButton = '';
            if ($latestInsertedMedicalId == $row10['id']) {
                
                //we have to check that this doesn't have a scheduled event with it for medicine adminisutered case
                if ($row10['tablename'] == 'medicine_administered') {

                    $sqlY = "SELECT calendar_event_id FROM bovinemanagement.medicine_administered WHERE id={$row10['id']}";
                    $resY = $GLOBALS ['pdo']->query($sqlY);
                    $rowY = $resY->fetch(PDO::FETCH_ASSOC);
                    if ($rowY['calendar_event_id'] != null) {
                         include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicineScheduled.inc');
                           $callBackURL=("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id={$this->bovineID}");
                         $deleteButton = MedicineScheduled::deleteQuickForm($row10['id'], $row10['tablename'],$callBackURL); // show 
                    }
                }
                
                if ($deleteButton=='') {
                include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
                $deleteButton = MedicalCase::deleteQuickForm($row10['id'], $row10['tablename']); // show
            }}
            //
            $time = strtotime($row10 ['event_time']);
            $ftime = date('M j, Y', $time);
            $str = "<span><b>{$row10['type']}</b>: " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row10['text']} </b>  by {$row10['userid']}. $deleteButton</span><br/>";
            $outArray = makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }
    
     private function showMilkandBeefWithHolding($bovineID) {
     $str='';  
        if (self::milkWithHolding($bovineID) !=null) {
            $str = $str . '<span id=\'colourBack\'>milk withhold until: ' . date('D M j/Y ga', self::milkWithHolding($bovineID)) . '</span><br/>';
        }
       if (self::beefWithHolding($bovineID) !=null) {
            $str = $str . '<span id=\'colourBack\'>beef withhold until: ' . date('D M j/Y ga', self::beefWithHolding($bovineID)) . '</span><br/>';
        }
        return ($str);
    }

    public function milkWithHolding($bovineID) {
        $sql = "SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding($bovineID) as milk_withholding";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (strtotime($row ['milk_withholding']) > strtotime("now")) {
            return strtotime($row ['milk_withholding']);
        } else {
            return null;
        }
    }

    private function beefWithHolding($bovineID) {
    $sql = "SELECT beef_withholding FROM bovinemanagement.milk_and_beef_withholding($bovineID) as beef_withholding";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (strtotime($row ['beef_withholding']) > strtotime("now")) {
            return strtotime($row ['beef_withholding']);
        } else {
            return null;
        }
    }
    
     private function magenetStatus($bovine_id) {
        $sql="SELECT magnet FROM bovinemanagement.medical_magnet WHERE bovine_id=$bovine_id";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row['magnet']=='t') {
            return "<b>Rumen Magnet:</b> Yes";
        }
        else {
             return "<b>Rumen Magnet:</b> No";
        }
        
    }
    
    
    
    
    
    
    
    
    
    
    public function error(){}
    
}//end class
    ?>