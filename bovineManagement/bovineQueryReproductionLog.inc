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
       $sql = "SELECT * FROM  bovinemanagement.reproduction_events({$this->bovineID})";
       $res = $GLOBALS ['pdo']->query($sql);
       
       while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
           
           //var_dump($row['type']);
           //show semen code when we give list to buyers only.
          $extra='';
          if (($salesList==true) AND ($row['type']=='Breeding')) {
              $extra=" (Semen Code: ". $row['extra1'].')';
          }
           
           
           $time=strtotime($row['event_time']);
           $ftime = date('M j, Y', $time);
           $str = "<b>{$row['type']}:</b> " . Misc::daysOrHoursAgo($time) . " <b class=\"smallish\">($ftime)</b> {$row['text']} $extra<br /> ";
           $outArray = makeTimeString($outArray, $time, $str);
          
       }
       $queryPageHelper=new QueryPageHelper();
        $outArray = $queryPageHelper->calculateLactationLineBreaks($this->bovineID, $outArray);
        
        return ($queryPageHelper->sortOutArray($outArray)); // print out sorted array.
       
    }
    
    
    
      
    public function error(){}
    
}//end class
    ?>