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
        $outArray = $this->processFootEventsLocal($outArray);
         $outArray = $this->processFootEventsMilk2020HoofApp($outArray);
        $outArray = $this->processMedEvents($outArray);
          $str=$str.(QueryPageHelper::sortOutArray($outArray));
          return $str;
    }
    
    /*
     * extra medical type things.
     */
    public function auxiliary() {
        
      
        $withholding=$this->showMilkandBeefWithHolding($this->bovineID);
        if (!empty($withholding)) { $out[]= ''.$withholding.''; }   
           
           $out[]=($this->magenetStatus($this->bovineID));
           $out[]=(BodyConditionScore::currentBCSforAnimal($this->bovineID));
        ////FIXME: (addme) $str=$str.("<b>Vaccination Status:</b>");
         
     return $out;
    }
    
    
    
     private function processFootEventsLocal($outArray) {
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
            $str = "<span class='text-custom-foot'>Foot</span>" . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> <b>{$row2['foot']}</b> with action/diagnois of <b>{$row2['diagnosis']}</b> and comment \"{$row2['comment']}\" by {$row2['userid']}<br/>";
            
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }
    
    
    private function milk2020Severity($row2,$name) {
        
         if ($row2[$name] >0)
       {
         return  "Area $name serverity ".$row2[$name]. '.';
       }
       else {return null;}
    }
    
    private function processFootEventsMilk2020HoofApp($outArray) {
        
       $reg_number=substr($this->bovineFullRegNumber,6); //milk2020 doesn't store full number.
        

        // ////////////////////////////////////////////////////////////////
        // do all foot events.
        $sql = "SELECT  *, split_part(trimmer, '@', 1) as userid,wcauthentication.initials(split_part(trimmer, '@', 1)) as initials
            FROM batch.milk2020_hoof_all 
WHERE reg_number::text='$reg_number'::text
ORDER BY trim_time DESC 
		";
        $res2 = $GLOBALS ['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {

       
       //the milk2020 username doesn't necessarily match our system, so if initals is not null assume it did.
       if (!empty($row2['initals'])) {
      $trimmer_initials=$row2['initals'];
       }
       else {
         $trimmer_initials=$row2['userid'];   
       }

       //if we don't know what foot it is. 
       $row2['foot'] = empty($row2['foot']) ? 'unknown': $row2['foot'];
       
       
       if ($row2['num_lesions'] > 0) {
           $num_lesions=$row2['num_lesions'].' lesions';
       }
       
       if ($row2['blocked'] > 0) {
           $blocks=$row2['blocked'].' blocks';
       }
       
       $lesions='';
       $out[]=$this->milk2020Severity($row2,'c');
       $out[]=$this->milk2020Severity($row2,'d');
       $out[]=$this->milk2020Severity($row2,'e');
       $out[]=$this->milk2020Severity($row2,'f');
       $out[]=$this->milk2020Severity($row2,'g');
       $out[]=$this->milk2020Severity($row2,'h');
       $out[]=$this->milk2020Severity($row2,'i');
       $out[]=$this->milk2020Severity($row2,'k');
       $out[]=$this->milk2020Severity($row2,'t');
       $out[]=$this->milk2020Severity($row2,'u');
       $out[]=$this->milk2020Severity($row2,'v');
       $out[]=$this->milk2020Severity($row2,'w');
       $out[]=$this->milk2020Severity($row2,'x');
       $out[]=$this->milk2020Severity($row2,'z');
       $lesions=implode($out);
     
                $num_lesions = empty($num_lesions) ? '' : $num_lesions;
                $blocks = empty($blocks) ? '' : $blocks;
                $lesions = empty($lesions) ? '' : $lesions;
                
            $time = strtotime($row2 ['trim_time']);
            $ftime = date('M j, Y', $time);
            $str = "<ul class='list'><span class='badge label-info label-custom-foot'>" ."Foot".'</span> ' . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> ". str_replace(' ', '', strtolower($row2['foot'])) ." $num_lesions $blocks $lesions ".(new Bootstrap)->initials($trimmer_initials) ." </ul>";         
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, $time, $str);
        }
        return $outArray;
    }
    

    //add extra markup to medical log
    public function medEventsMarkup($tablename,$text,$raw_value) {
        
        switch ($tablename) {
            
            case 'medical_cmt':
                 $out= $this->cmtTable($raw_value); 
                break;
            case 'medical_ketone':
                 $out=(new MedicalCase)->colourKetoneOrNot($raw_value,true);
                break;
            
            case 'medical_temperature':
                $out=(new MedicalCase)->colourTempNormalOrNot($raw_value,true);
                break;
            
            default:
                $out=$text;
                break;
        }
        
        return $out;
    }
   
    //to display CMT reuslts. 
    private function cmtTable($json) {
        
        $jsonArr=json_decode($json,true);
        
        foreach ($jsonArr as $key => $value) {
            switch ($value) {
                case 'N':
                   $jsonArr[$key]=  (new BootStrap)->successBadge($value);
                    break;
                case 'T':
                   $jsonArr[$key]=  (new BootStrap)->successBadge($value);
                    break;
                case '1':
                   $jsonArr[$key]=  (new BootStrap)->warningBadge($value);
                    break;
                 case '2':
                   $jsonArr[$key]=  (new BootStrap)->dangerBadge($value);
                    break;
                 case '3':
                   $jsonArr[$key]=  (new BootStrap)->dangerBadge($value);
                    break;
            }
        }
        
        
        $html=<<<HTML
                <table class="table inlineSmallTable">
  
  <tbody>
    <tr>
      <td>{$jsonArr['lf']}</td>
      <td>{$jsonArr['rf']}</td>
    </tr>
     <tr>
      <td>{$jsonArr['lr']}</td>
      <td>{$jsonArr['rr']}</td>
    </tr>            
      </tbody>
         </table>  
HTML;
        return $html;
    }
   
    
     private function processMedEvents($outArray) {

       

        // /////////////////////////////////////////////////////////////////
        // do all repro hormone therapies
        $sql = "SELECT id,'bovinemanagement' as schema,tablename,type,event_time,text,userid,initials,raw_value FROM bovinemanagement.medical_summary 
		WHERE bovine_id={$this->bovineID} 
		ORDER BY medical_summary.event_time DESC";

        $res10 = $GLOBALS ['pdo']->query($sql);

        while ($row10 = $res10->fetch(PDO::FETCH_ASSOC)) {
            
             $deleteButton=$this->medicalEventsDeleteButton($row10['type'],$row10['id'],$row10['tablename'],$row10['schema'],$this->bovineID);
            
            //this modifies the text output to add extra display info, I do not likle this at all, but it's a compromise.
            $text=$this->medEventsMarkup($row10['tablename'],$row10['text'],$row10['raw_value']);
 
            $outArray = $GLOBALS['MiscObj']->makeTimeString($outArray, strtotime($row10['event_time']),  $this->medEventListItem($row10['type'],$row10['event_time'],$text,$row10['initials'],$deleteButton));
        }
        return $outArray;
    }
    
    
    /*
     * medical schedule is complicated, so it has its own call. 
     */
    public function medicalEventsDeleteButton($type,$id,$tablename,$schema,$bovine_id) {
        
      
        $out[] = '';
        
        //scheduled meds and actions are in the future, so myt be handles differentlet. 
        if ($type == 'Scheduled Meds')  {  //this does medical event in the future
            include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicineScheduled.inc');
            $delButton=new QF2_DeleteButtonDecider('user_timeFuture',(object) array('dateTimeName'=>'scheduled_event_time','useridColumnName'=>'scheduled_userid','buttonClassName'=>'QF2_ScheduleSingleGeneralTreatmentDeleteButton','schemaTable'=>'bovinemanagement.medicine_administered')); 
            $out[] = $delButton->output($id);
            
        } 
        
        if ($type == 'Scheduled Action') {  //this does medical events in the future //QF2_ScheduleAction
            $delButton=new QF2_DeleteButtonDecider('user_timeFuture',(object) array('dateTimeName'=>'scheduled_event_time','useridColumnName'=>'scheduled_userid','buttonClassName'=>'QF2_ScheduleSingleMedicalActionDeleteButton','schemaTable'=>'bovinemanagement.medical_action')); 
            $out[] = $delButton->output($id);
            
        } 
        
        
             //the generic delete button works for all cases with normal 'id' and 'userid' columns
            $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'schemaTable' => $schema . '.' . $tablename));
            $out[] = $delButton->output($id);
        
        
        return implode($out);
    }
    
    
    
   
    public function medEventListItem($type,$event_time,$text,$initials,$deleteButton) {
      
        $time = strtotime($event_time);
        $ftime = date('M j, Y', $time);
        $initialsStr=(new Bootstrap)->initials($initials);

        return "<ul class='list'><span class='badge label-info label-custom-". str_replace(' ', '', strtolower($type))   ."'>" .$type.'</span> ' . $GLOBALS['MiscObj']->daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> $text $initialsStr $deleteButton</ul>";
        
    }
    
    
    private function showMilkandBeefWithHolding($bovineID) {
        $str = '';
        
          
        if (self::allMilkWithHolding($bovineID) != null) {

            $txt = 'milk withhold until: ' . date('D M j/Y ga', self::allMilkWithHolding($bovineID));

            $str = $str . (new BootStrap)->dangerAlert('Milk', $txt);
        }
        
        if (self::allBeefWithHolding($bovineID) != null) {

            $txt2 = 'beef withhold until: ' . date('D M j/Y ga', self::allBeefWithHolding($bovineID));

            $str = $str . (new BootStrap)->dangerAlert('Beef', $txt2);
        }
       
        
         if (self::beefWithHolding($bovineID) != null) {

            $txt2 = 'beef withhold until: ' . date('D M j/Y ga', self::beefWithHolding($bovineID));

            $str = $str . (new BootStrap)->dangerAlert('Beef (old)', $txt2);
        }
        
        
        
         return ($str);
    }

    //fresh and medical reasons
     public static function allMilkWithHolding($bovineID) {
         $sql = "SELECT * FROM bovinemanagement.milk_beef_fresh_withholding($bovineID) as withholding";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
       
        $jsonArr=json_decode($row['withholding'],true);
        $milkArr= !empty($jsonArr['milk']) ? ($jsonArr['milk'][0]['jsonb_build_object']) : null;
        $beefArr= !empty($jsonArr['beef']) ?($jsonArr['beef'][0]['jsonb_build_object']) : null;
        $freshArr=!empty($jsonArr['post_fresh']) ?($jsonArr['post_fresh'][0]['jsonb_build_object']) : null;
         
        $milk=null;
        $fresh=null;
        
        if (!empty($milkArr)) {
            if ((strtotime($milkArr['withholding']) > strtotime("now"))) {
                $milk = strtotime($milkArr['withholding']);
            } else {
                $milk = null;
            }
        } else {
            $milk = null;
        }

        if (!empty($freshArr)) {
            if (strtotime($freshArr['withholding']) > strtotime("now")) {
                $fresh = strtotime($milkArr['withholding']);
            } else {
                $fresh = null;
            }
        } else {
            $fresh = null;
        }

        //if either fresh or milk are true, then milk needs withheld. 
        if (empty($milk) AND empty($fresh)) {
            return null;
        } elseif ($milk > $fresh) {
            return $milk;
        } else {
            return $fresh;
        }
    }
    
    
    //beef reasons
     public static function allBeefWithHolding($bovineID) {
         $sql = "SELECT * FROM bovinemanagement.milk_beef_fresh_withholding($bovineID) as withholding";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
       
        $jsonArr=json_decode($row['withholding'],true);
        $milkArr= !empty($jsonArr['milk']) ? ($jsonArr['milk'][0]['jsonb_build_object']) : null;
        $beefArr= !empty($jsonArr['beef']) ?($jsonArr['beef'][0]['jsonb_build_object']) : null;
        $freshArr=!empty($jsonArr['post_fresh']) ?($jsonArr['post_fresh'][0]['jsonb_build_object']) : null;
         
        $milk=null;
        $fresh=null;
        if (!empty($beefArr)) {
            if ((strtotime($beefArr['withholding']) > strtotime("now"))) {
                return strtotime($beefArr['withholding']);
            } else {
                return null;
            }
        } else {
            return null;
        }
    }
    
    public static function milkWithHolding($bovineID) {
        $sql = "SELECT * FROM bovinemanagement.milk_beef_fresh_withholding($bovineID) as withholding limit 1"; //only return 1 row anyway
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $jsonArr = json_decode($row['withholding'], true);

        //array decoding json is a bit messy with nulls. 
        $milkArr = empty($jsonArr['milk']) ? null : ($jsonArr['milk'][0]['jsonb_build_object']);
        $beefArr = empty($jsonArr['beef']) ? null : ($jsonArr['beef'][0]['jsonb_build_object']);
        $freshArr = empty($jsonArr['post_fresh']) ? null : ($jsonArr['post_fresh'][0]['jsonb_build_object']);

        foreach ($jsonArr as $key => $value) {
            
        }

        if ((!empty($milkArr['withholding'])) AND (strtotime($milkArr['withholding']) > strtotime("now"))) {
            return strtotime($milkArr['withholding']);
        } else {
            return null;
        }
    }

    private static function beefWithHolding($bovineID) {
    $sql = "SELECT beef_withholding FROM bovinemanagement.milk_and_beef_withholding($bovineID) as beef_withholding";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (strtotime($row ['beef_withholding']) > strtotime("now")) {
            return strtotime($row ['beef_withholding']);
        } else {
            return null;
        }
    }
    
     private static function magenetStatus($bovine_id) {
        $sql="SELECT magnet FROM bovinemanagement.medical_magnet WHERE bovine_id=$bovine_id";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (empty($row)) {
            $message = "<b>Reticulum Magnet:</b> No";
        } else {
            if ($row['magnet'] == 't') {
                $message = "<b>Reticulum Magnet:</b> Yes";
            } else {
                $message = "<b>Reticulum Magnet:</b> No";
            }
        }

        return (new BootStrap)->successAlert('Magnet', $message);
    }
   
    
    public function error(){}
    
}//end class

Class QF2_medicalEventsDeleteButton extends QF2_DeleteButtonGeneric{ 
    
    function action() {
        //$this-param should provide everything the form needs for paramaters.  
           
         BovineQueryMedicalLog::protocolMedicineDefaultDeleteByID('medicine_administered',$this->param->deleteID); 
     
    }
}