<?php 
require_once($_SERVER['DOCUMENT_ROOT'] . 'functions/queryPageHelper.inc');
class BovineQueryReproductionLog {
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

    
   
    
    /*
     * new way, uses a giant SQL query, no faster as it turns out.
     */
    public function main($salesList=false) {
         $outArray=array();
        /*
         * repro event log is all concatenated by sql, much faster */
       $sql = "SELECT *,wcauthentication.initials(userid) as initials FROM bovinemanagement.reproduction_events({$this->bovineID})";
       $res = $GLOBALS ['pdo']->query($sql);
       
       $reproArr=array();
       
       while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
           
           
           
           /*
            * shows delete button. 
            * really not a great way to do this. every form entry class should probably have a delete function and we just call that, we could have a default one that is like this. 
            */
           $deleteButton = '';
           //null table name means we really shouldn't delete this. 
            if ($row['tablename'] != 'null') {
                $useridColumnName = 'userid';
                $dateTimeName = 'event_time';
                $mode='user_timeLimit';

                //ovverrides defaults for special cases.
                switch ($row['type']) {
                    case 'Breeding Ban':
                        $dateTimeName = 'create_time';
                        break;
                    case 'Breeding':
                        $useridColumnName = 'actual_breeding_userid';
                        $dateTimeName = 'actual_breeding_time';
                        break;
                    case 'Breeding Sexed':
                        $useridColumnName = 'actual_breeding_userid';
                        $dateTimeName = 'actual_breeding_time';
                        break;
                    case 'Protocol':
                        $dateTimeName = 'date_start';
                        break;
                    case 'Breeding Scheduled':
                        $useridColumnName = 'estimated_breeding_userid';
                        $dateTimeName = 'estimated_breeding_time';
                        $mode='user_timeFuture';
                        print("dsadsasa");
                         break;
                }
                DEBUG ?: var_dump($row['type']);
                $delButton = new QF2_DeleteButtonDecider($mode, (object) array('dateTimePeriodDays' => '1', 'dateTimeName' => $dateTimeName, 'useridColumnName' => $useridColumnName, 'schemaTable' => 'bovinemanagement' . '.' . $row['tablename']));
                $deleteButton = $delButton->output($row['type_id']);
            }

    
            
            //var_dump($row['type']);
           //show semen code when we give list to buyers only.
          $extra='';
          if (($salesList==true) AND ($row['type']=='Breeding')) {
              $extra=" (Semen Code: ". $row['extra1'].')';
          }
           
           if (($row['type']=='Preg Check') AND ($row['extra1']=='true')) {
              $extra="<b>Twins</b>";
          }
          
          //use json as a bit of a hack to pass info we need. 
          if (($row['type']=='Calf Female') OR ($row['type']=='Calf Male')) {
              $jsonArr=json_decode($row['extra1']);
              $out=null;
              $out[]=$GLOBALS['MiscObj']->hrefToBovinePageFullName($jsonArr[0],$jsonArr[1],$jsonArr[2]);
              if ($jsonArr[3]=='AN') {
                  $out[]='<i class="fab fa-github-alt"></i>';//show angus symbol                
              }
              $row['text']=implode($out);
          }
          elseif (($row['type']=='Vet Comment') AND (!empty($row['extra1'])) ) {
          $jsonArr=json_decode($row['extra1']);
           //var_dump($jsonArr->vet_userid);
              $row['text']='Vet '. $jsonArr->vet_userid .': '. $row['text'];//vet userid 
          }
          
           
           $time=strtotime($row['event_time']);
           $ftime = date('M j, Y', $time);
           
 
           $logArr=array();
           //create log table format....
           $logArr[]='<div class="timeline-item">';
           $logArr[]='<td>'. "<span class='badge label-info label-custom-". str_replace(' ', '', strtolower($row['type']))   ."'>" .$row['type'].'</span> '   .'</td>'; //badge
           $logArr[]='<td>'. $GLOBALS['MiscObj']->daysOrHoursAgo($time)    .'</td>'; //daysago              
           $logArr[]='<td>'. '<b class=\"smallish\">'.$ftime.'</b>'.'</td>';   //date
           $logArr[]='<td>'. $row['text'] ; //text
           $logArr[]= "<div class='circle'><span class='initials'>{$row['initials']}</span></div>" ; //initials              
           $logArr[]= $extra . $deleteButton .'</td>'; //buttons
              $logArr[]='</div>';
            $str= '<tr>'. implode($logArr) .'</tr>'; //row
          // print('<table>'.$str.'</table>');
            
            
          
        
           
           $tempArr=array();
           $tempArr['time']=$time;
           $tempArr['ftime']=$ftime;
           $tempArr['type']=$row['type'];
           $tempArr['text']=$row['text'];
           $tempArr['initials']=$row['initials'];
           $tempArr['buttons']=$extra . $deleteButton ;
           //store in array    
           // var_dump($tempArr);
            
            //find time for array key. 
            
            
           $reproArr[]=array('time'=>$time,'data'=>$this->assembleTimeLineItem($tempArr));
       }

       //add today label.
          $reproArr[]=array('time'=>strtotime('now'),'data'=> $this->assembleTimeLineLabel($class='bg-green','Today'));
         

        $lactArr = $this->calculateLactationSplits($this->bovineID);
        
        
           
        
         
               $queryPageHelper=new QueryPageHelper();   
         $header=<<<HEADER
                 <script>
                 $(function () {
  $('[data-toggle="tooltip"]').tooltip()
})
                 </script>
                 
                <ul class="timeline"><!-- timeline time label -->
 
HEADER;
      
         $mergedArr=($reproArr+$lactArr);    
         
         //now sort out array based on time element.
         function cmp($a, $b) {
            return $b['time'] - $a['time']; //numbers only
        }

        usort($mergedArr,"cmp");
        
        //var_dump($mergedArr);
        
        //now make one string. 
        $doneArr=array();
        foreach ($mergedArr as $key => $value) {
            $doneArr[]=$value['data'];
        }
        
        
         
        $middle= implode($doneArr); 
       $footer=<<<FOOTER
        </ul>       
       FOOTER;        
        
        
        return $header.$middle.$footer;
       /*
       
       
       $queryPageHelper=new QueryPageHelper();
        $outArray = $queryPageHelper->calculateLactationLineBreaks($this->bovineID, $outArray);
        
        return ($queryPageHelper->sortOutArray($outArray)); // print out sorted array.
   */    
    }
    
    
    
     public  function calculateLactationSplits($bovine_id) {
        //find when lactations started and break up the list whenever that occurs
        $sql = "SELECT id,fresh_date FROM bovinemanagement.lactation WHERE bovine_id=$bovine_id ORDER BY fresh_date ASC";
        $res = $GLOBALS ['pdo']->query($sql);
        $counter = 1;
        $outArray=array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['fresh_date']);
            
             $outArray[]=array('time'=>$time,'data'=>$this->assembleTimeLineLabel('bg-green','Lactation #'.$counter));
            $counter++;
        }
        return $outArray;
    }
    
    public function assembleTimeLineLabel($class='bg-green',$label)
    {
    $html=<<<HTML
            
        <div class="time-label">
        <span class="$class">
           $label
        </span>
    </div> 
        
    HTML;
    return $html;
    
    }
    
    
    public function assembleTimeLineItem(array $arrItem) {
    
        $typeClass=str_replace(' ', '', strtolower($arrItem['type']));
        $daysAgo=$GLOBALS['MiscObj']->daysOrHoursAgo($arrItem['time']);
        $html=<<<HTML
       
        <div class="timeline-ContainerCustom">        
          <i class='badge label-info label-custom-$typeClass'>{$arrItem['type']}</i>  
              
             <div class="timeline-itemCustom">   
              
             
          
               
               {$arrItem['text']}
               <div class='circle'><span class='initials'>{$arrItem['initials']}</span></div>
                  <a href="#" data-toggle="tooltip" title="" data-original-title="{$arrItem['ftime']}"> {$daysAgo} </a>
               {$arrItem['buttons']}
             </div>   
               
        </div>       
        HTML;        
      
        
        return $html;
    }
    
    
    
      
    public function error(){}
    
}//end class