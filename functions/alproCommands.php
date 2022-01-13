<?php 
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'].'functions/alproIsoLink.inc');

class AlproCommands {
public $lrDB;


/** connect to db **/
public function __construct() {

$this->lrDB = $GLOBALS['pdo'];
}
function main() {
 }
 
/**
* Changes cows groups. RUN THIS AFTER ADDING ALL COWS.
* Syncs locations in internal db to alpro.
*/ 
public function syncBovineMove() {
 
 $state='true';//asume state will work. 
 try {
 $moveArray=$this->findCowsToMoveGroup(); //looks at alpro and int db.
 $ret=$this->setMoveCowGroup(null,$moveArray);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncBovineMove Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','location_event',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
 /*
  print_r($alproIsoLink->inArray);
  print_r($alproIsoLink->outArray);
  print_r($alproIsoLink->logArray); */
}

/**
* Shows the Dump Milk light on MPC
*/ 
public function syncDumpMilk() {
 
 $state='true';//assume state will work.
 
 try {
 $dumpmilkArray=$this->findCowsToSetDumpMilk(); //looks at alpro and int db.
 $ret=$this->setDumpMilkDelayInDaysFromToday(null,$dumpmilkArray);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncDumpMilk Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','dump milk',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
}

/**
* Shows the Do Not Milk light on MPC
* Syncs locations in internal db to alpro.
*/ 
public function syncDoNotMilk() {
 
 $state='true';//assume state will work.
 
 try {
 $donotmilkArray=$this->findCowsToSetDoNotMilk(); //looks at alpro and int db.
 $ret=$this->setDontMilkDelayInDaysFromToday(null,$donotmilkArray);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncDoNotMilk Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','do not milk',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
}


/**
* Adds cows who have entered the herd to alpro (births usually).
* Syncs animals in internal db to alpro.
*/ 
public function syncBovineAlive() {
 
 $state='true';//asume state will work.
 
 try {
 $aliveArray=$this->findCowsToAdd();
 $ret=$this->setAddCow(null,$aliveArray);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;

 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncBovineAlive Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','bovine alive',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
 /*
  print_r($alproIsoLink->inArray);
  print_r($alproIsoLink->outArray);
  print_r($alproIsoLink->logArray); */
}

/**
* Removes cows from ALpro who have left the herd (deaths/sales usually).
* Syncs animals in internal db to alpro.
*/ 
public function syncBovineDead() {
 
 $state='true';//asume state will work.
 
 try {
 $deadArray=$this->findCowsToRemove();
 $ret=$this->setRemoveCow(null,$deadArray);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;

 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncBovineDead Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful

  print_r($alproIsoLink->inArray);
  print_r($alproIsoLink->outArray);
  print_r($alproIsoLink->logArray);
  }

    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','bovine dead',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
 
}

/**
* Marks as dried off in Alpro
* Syncs locations in internal db to alpro.
*/ 
public function syncBovineDriedOff() {
 
 $state='true';//assume state will work.
 
 try {
 $cowsToMarkDriedOffArray=$this->findCowsToMarkDriedOff(); //looks at alpro and int db.
 $ret=$this->setCowLatestDriedOff(null,$cowsToMarkDriedOffArray);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncBovineDriedOff Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','dried off',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
}

/**
* Marks calving date in Alpro
* Syncs locations in internal db to alpro.
*/ 
public function syncBovineCalving() {
 
 $state='true';//assume state will work.
 
 try {
 $findCowsLatestCalvingDate=$this->findCowsLatestCalvingDate(); //looks at alpro and int db.
 $ret=$this->setCowLatestCalvingDate(null,$findCowsLatestCalvingDate);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncBovineCalving Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','calving',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
}


/**
* Marks calving date in Alpro
* Syncs locations in internal db to alpro.
*/ 
public function syncBovineInsemination() {
 
 $state='true';//assume state will work.
 
 try {
 $findCowsLatestInseminations=$this->findCowsInseminations(); //looks at alpro and int db.
 $ret=$this->setCowInseminations(null,$findCowsLatestInseminations);
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  echo 'syncBovineInseminations Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  }
 
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink::writeSync('bovinemanagement','inseminations',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
}

//****************
// END OF SYNC FUNCTIONS
///////////////////

private function readCowDataFromAlpro($ret='') {

$ret=$ret.'CN Get cow number info:'."\r\n";  
$ret=$ret.'CN      |cow       |group no |birthdate | rfid'."\r\n";  
$ret=$ret.'RN39000200300001060003000030200030003308000303930150'."\r\n";  
//$rest = substr("abcdef", 0, -1);  
return $ret;
}


//assumes internal is master and alpro is slave
private function findCowsToMoveGroup() {
//get from ALPRO system
  $ret="";
  $ret=$ret.'CN Get cow number info:'."\r\n";  
  $ret=$ret.'CN      |cow       |group no |birthdate | rfid'."\r\n";  
  $ret=$ret.'RN39000200300001060003000030200030003308000303930150'."\r\n"; 
  $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 
  foreach ($alproIsoLink->outArray as $key => $value) {
   $cowNo=$value[0];
   $groupNo=$value[1];
   $dataArrayAlpro[$cowNo]=$groupNo; 
}
  //print_r($dataArrayAlpro);

//get from INTERNAL system
    $sql2="SELECT local_number,location_id FROM bovinemanagement.bovinecurr";
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
   $dataArrayInt[$row2['local_number']]=$row2['location_id']; //create cow move group array
 }
 
 //print_r($dataArrayInt);
 
 //find the array difference
 $cowsToMoveInAlproArray = array_diff_assoc($dataArrayInt, $dataArrayAlpro); //finds elements who are different
    
   print("\r\nCows with wrong groups in Alpro"."\r\n");
   print_r($cowsToMoveInAlproArray);

 return $cowsToMoveInAlproArray;
}

function setMoveCowGroup($ret='',$cowGroupArray) {

foreach ($cowGroupArray as $cowNum => $groupNum) {
    
//spacing is absolutely critical with this protocol. 
$cowNum=trim($cowNum);
$cowNumX=str_pad($cowNum, 6, ' ', STR_PAD_LEFT); 
$groupNum=trim($groupNum);
$groupNumX=str_pad($groupNum, 2, ' ', STR_PAD_LEFT); 

$ret=$ret."CN move cow number $cowNum"."\r\n";     
$ret=$ret.'DN3900020090007006000300003020'."\r\n";
$ret=$ret."VN390002{$cowNumX}{$groupNumX}"."\r\n";


} 
return $ret;
}

/**
* Adds cows who have entered the herd to alpro (births usually).
* Syncs animals in internal db to alpro.
*/ 
public function syncAlproProcessorDateTime() {
 
 $state='true';//asume state will work.
 
 try {
 $ret=$this->readAlproProcessDateTime();
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;

 //catch exception
 }
 catch(Exception $e)
  {
  echo 'readAlproProcessorDateTime Message: ' .$e->getMessage();
  $state='false'; //says sync was unsucessful
  } 
  
  //process output
  foreach ($alproIsoLink->outArray as $key => $value) {
   $dateTimeStr=$value[0] .' '.$value[1].':'.$value[2];   //convert date to one date string. 
   $retArray['dateTimeStr']=  date('c',strtotime($dateTimeStr));    
   
   //now compare with PHP time...then store in DB error? then show?
   
  }
    
 return $retArray;
  
  
}

/* finds the alpro system controller time and date and returns it */
private function readAlproProcessDateTime() {
//get processor date time from ALPRO system
//     308503  yymmddd 08 bytes 0 deci
//     308504  min     02 bytes 0 deci
//     308505  hour    02 bytes 0 deci

  $ret="";
  $ret=$ret.'CN Get alpro processor time and date:'."\r\n";  
  $ret=$ret.'CN      |date   |   hour|  min  '."\r\n";  
  $commandStr='RN 390050  00308503 080   00308504 020 00308505 020'."\r\n"; //V6.6 //command plus lemgth and deci from dictionary file   . 
  $ret=$ret.preg_replace('/\h+/', '', $commandStr); //replaces spaces and tabs, not line feeds, makes easier to read.

   return $ret;
  
}
/**
 * Returns an array of cows to add to alpro who have been born or added to the herd.
 */
private function findCowsToAdd() {
//get from ALPRO system
  $ret="";
  $ret=$ret.'CN Get cow number info:'."\r\n";  
  $ret=$ret.'CN      |cow       |group no |birthdate | rfid'."\r\n";  
  $ret=$ret.'RN39000200300001060003000030200030003308000303930150'."\r\n"; 
  $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 
  foreach ($alproIsoLink->outArray as $key => $value) {
   $cowNo=$value[0];
   $groupNo=$value[1];
   $dataArrayAlpro[$cowNo]=$cowNo; 
}

//get from INTERNAL system
    $sql2="SELECT local_number,location_id,birth_date,rfid_number FROM bovinemanagement.bovinecurr";
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    $dataArrayInt[$row2['local_number']]=$row2['local_number']; //create array for comparison
    //array to store data
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['location_id']=$row2['location_id'];
    $dataArrayInternal[$row2['local_number']]['birth_date']=$row2['birth_date'];
    $dataArrayInternal[$row2['local_number']]['rfid_number']=$row2['rfid_number'];
 }

 print("\r\nCows to Add to Alpro (born or added to herd)"."\r\n");
 $cowsWeWant = array_diff_assoc($dataArrayInt, $dataArrayAlpro); //finds elements who are different
 
 //now go through the array with the full amount of info and pull out who we want.
 $retArray=array();
 foreach ($cowsWeWant as $key => $value) {
    $retArray[$key]=$dataArrayInternal[$key];
 }  
 return $retArray;
}

private function findCowsToRemove() {
//get from ALPRO system
  $ret="";
  $ret=$ret.'CN Get cow number info:'."\r\n";  
  $ret=$ret.'CN      |cow       |group no |birthdate | rfid'."\r\n";  
  $ret=$ret.'RN39000200300001060003000030200030003308000303930150'."\r\n"; 
  $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 
  foreach ($alproIsoLink->outArray as $key => $value) {
   $cowNo=$value[0];
   $groupNo=$value[1];
   $dataArrayAlpro[$cowNo]=$cowNo; 
}

//get from INTERNAL system
    $sql2="SELECT local_number FROM bovinemanagement.bovinecurr";
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    $dataArrayInt[$row2['local_number']]=$row2['local_number']; //create array for comparison
    }
    
    print("\r\nCows to Remove from Alpro (died or left herd)"."\r\n");
    $cowsToRemoveFromAlproArray = array_diff_assoc($dataArrayAlpro, $dataArrayInt); //finds elements who are different
    print_r($cowsToRemoveFromAlproArray);   
    
    return $cowsToRemoveFromAlproArray;
}

/**
* Sets do not milk to number of days or blank for ok to milk for all current cows. 
*/
private function findCowsToSetDoNotMilk() {

    //get any cow who is dry and put them as do not milk for the next 14 days. 
    //assume this code gets run often enough so fresh cows get updated
    $sql2="SELECT local_number,14 as days,1 as milking1,1 as milking2 FROM bovinemanagement.bovinecurr WHERE fresh_date is null
           UNION
           SELECT local_number,0 as days,0 as milking1, 0 as milking2 FROM bovinemanagement.bovinecurr WHERE fresh_date is not null
    ";
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    //array to store data
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['days']=$row2['days'];
    $dataArrayInternal[$row2['local_number']]['milking1']=$row2['milking1'];
    $dataArrayInternal[$row2['local_number']]['milking2']=$row2['milking2'];
    }
    
    print("\r\nCows to Set Do Not Milk or no light (dry cows or milking)"."\r\n");
    print_r($dataArrayInternal);   
    
    return $dataArrayInternal;
}

/**
* Sets do dump milk to number of days or blank for ok to milk for all current cows. 
*/
private function findCowsToSetDumpMilk() {

    //look for any cows with a milk with holding due to treatment and set it to that number of days.
    //even if it is 5 minutes before the milk is good set it to one day. assume the code is run often so that this is not a problem.
    //ALSO set fresh cows to withhold milk 3 days from calving because of CQM.
    $sql2="SELECT local_number,(SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding(bovinecurr.id) where milk_withholding > current_timestamp) as milk_withholding,bovinecurr.fresh_date FROM bovinemanagement.bovinecurr"; 
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    
    $days1=null;
    $days2=null;
    
    //see if milk withholding is in the future.
    if ((strtotime($row2['milk_withholding']) > strtotime('now'))  && (strtotime($row2['milk_withholding']) != null)) {
     $days1=(strtotime($row2['milk_withholding'])-strtotime('now'))/(60*60*24);
     print("days1:$days1\r\n");
    }
    
    //withhold fresh milk for 3 days. 259200 seconds in 3 days.
    if (((strtotime($row2['fresh_date']) + 259200) > strtotime('now')) && (strtotime($row2['fresh_date']) != null)) {
     $days2=( strtotime($row2['fresh_date']) + 259200 - strtotime('now'))/(60*60*24);
     print("days2:$days2\r\n");
    }
    
    if (($days1 != null) || ($days2 != null)) {
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['days']=14;  //set to 14 days even if there is only 1 hour left.
    $dataArrayInternal[$row2['local_number']]['milking1']=1;
    $dataArrayInternal[$row2['local_number']]['milking2']=1; 
    }
    else {
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['days']=0;
    $dataArrayInternal[$row2['local_number']]['milking1']=0;
    $dataArrayInternal[$row2['local_number']]['milking2']=0;
    }
   
    }
    
    print("\r\nCows to Set Dump Milk or no light (treated cows or milking)"."\r\n");
    print_r($dataArrayInternal);   
    
    return $dataArrayInternal;
}

/* errors if any of the individual syncs have not occured for a while. For instance, dump milk isn't working, but the rest is.  */
/* called from movementSortGate.inc for now */
  public function errorAlproSyncIndividual() {
        $error = array();
        $error = array_merge($error, $this->errorAlproSyncIndividualQuery('location_sort'));
        $error = array_merge($error, $this->errorAlproSyncIndividualQuery('location_event'));
        $error = array_merge($error, $this->errorAlproSyncIndividualQuery('dump milk'));
        $error = array_merge($error, $this->errorAlproSyncIndividualQuery('do not milk'));
        $error = array_merge($error, $this->errorAlproSyncIndividualQuery('bovine alive'));
        $error = array_merge($error, $this->errorAlproSyncIndividualQuery('bovine dead'));
        return $error;
    }
       
//used to customize query for every type of sync.
private function errorAlproSyncIndividualQuery($table) {
 $error = array();

        //since these errors are so important, put the time to now and level to 99.
        $now = date('r', strtotime("now"));

        //check if sync didn't occur at all.  
        $query = "SELECT event_time,state FROM alpro.sync WHERE state=true AND db_table='$table' AND event_time >= (now() - interval '1 day') ORDER BY event_time DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($query);
        if ($res->rowCount() == 0) {

            $query2 = "SELECT event_time,state FROM alpro.sync WHERE state=true AND db_table='$table' ORDER BY event_time DESC LIMIT 1";
            $res2 = $GLOBALS['pdo']->query($query2);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $error[] = new notifyObj(get_class($this), $now, 1, " ERROR: Alpro ISO $table not synced since " . date('M d, Y G:i', strtotime($row2['event_time_max'])));
        }
        return $error;
}

  


/**
* adds an array of cows to alpro.
*/
function setAddCow($ret='',$cowAddArray) {

//go through each cow
foreach ($cowAddArray as $cowNum => $val) {
print_r($val);
$cowNum=$val['local_number'];
$groupNum=$val['location_id'];
$birthDate=$val['birth_date'];
$transponderNum=$val['rfid_number']; //must be string too big of number.


//spacing is absolutely critical with this protocol. 
$cowNum=trim($cowNum);
$cowNumX=str_pad($cowNum, 6, ' ', STR_PAD_LEFT); 
$groupNum=trim($groupNum);
$groupNumX=str_pad($groupNum, 2, ' ', STR_PAD_LEFT); 
$transponderNum=trim($transponderNum);
$transponderNumX=str_pad($transponderNum, 15, ' ', STR_PAD_LEFT);
$birthDateX=date('Ymd',strtotime($birthDate)); //always outputs 8 chars, so no trim needed.

$ret=$ret."CN Send and introduce a new animal ($cowNum)to Alpro and put her into correct breeding state"."\r\n";
$ret=$ret.'CN      |new cow   |group     |transp    |birthdate |calv date |lact num  |heat date |insem date|insem num |preg flag |dryied off|build-up'."\r\n";
$ret=$ret.'DN390001003039100600030000302000303930150003000330800030003508000300036020003000340800030035708000300037020003039110100030019408000303912010'."\r\n";
$ret=$ret."VN390001{$cowNumX}{$groupNumX}{$transponderNumX}{$birthDateX}??????????????????????????????????????"."\r\n"; //full string must be sent to add cows with ?'s.

}

return $ret;
}

/**
*Deletes a cow from alpro
*/
function setRemoveCow($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $cowNum2) {
    if (is_numeric($cowNum)==true) {
//spacing is absolutely critical with this protocol. 
$cowNum=trim($cowNum);
$cowNumX=str_pad($cowNum, 6, ' ', STR_PAD_LEFT);

$ret=$ret."CN Delete cow number $cowNum"."\r\n";
$ret=$ret.'DN39000200312007060'."\r\n";
$ret=$ret."VN390002{$cowNumX}"."\r\n";
}}
return $ret;
}

/**
*Marks a cow to not be milked on the parlor MPC for X number of days
*/
function setDontMilkDelayInDaysFromToday($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];
$dayNum=$val['days'];
$milking1=$val['milking1']; //if we don't set a milking number too, this does nothing.
$milking2=$val['milking1'];
//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);
$dayNumX=str_pad(trim($dayNum), 2, ' ', STR_PAD_LEFT);
$milking1X=str_pad(trim($milking1), 1, ' ', STR_PAD_LEFT);
$milking2X=str_pad(trim($milking2), 1, ' ', STR_PAD_LEFT);
//300066 DONT_MILK_DURATION
$ret=$ret."CN Do Not Milk In Days for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow #  |days |  milking_am | milking_pm'."\r\n";
$ret=$ret.'DN39000200900070060003000660200030005601000300057010'."\r\n";
$ret=$ret."VN390002{$cowNumX}{$dayNumX}{$milking1X}{$milking2X}"."\r\n";
}
return $ret;
}

/**
*Marks a cow to have milk dumped on the parlor MPC for X number of days
*/
function setDumpMilkDelayInDaysFromToday($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];
$dayNum=$val['days'];
$milking1=$val['milking1']; //if we don't set a milking number too, this does nothing.
$milking2=$val['milking1'];
//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);
$dayNumX=str_pad(trim($dayNum), 2, ' ', STR_PAD_LEFT);
$milking1X=str_pad(trim($milking1), 1, ' ', STR_PAD_LEFT);
$milking2X=str_pad(trim($milking2), 1, ' ', STR_PAD_LEFT);
$ret=$ret."CN Dump Milk In Days for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow #         |days  |  milking_am | milking_pm'."\r\n";
$ret=$ret.'DN39000200900070060003000780200030006801000300069010'."\r\n";
$ret=$ret."VN390002{$cowNumX}{$dayNumX}{$milking1X}{$milking2X}"."\r\n";
}
return $ret;
}

/**
 * NOT USED, just here in case we ever go this route.
 * This would be a way to get the milking data, but DeLaval cripples it.
 * They don't give as much accuracy for the milking stats as the DB holds.
 */
private function readMilkingDataForAllCowsTodayYest($ret='') {
//RN390010 milking data
//00300001060 cow chain number
//00300130042 today's milk yield 1
//00300133042 today's milk yield 2
//00300142042 yesterday's milk yield 1
//00300145042 yesterday's milk yield 2  
$ret=$ret.'CN Get cow number info:'."\r\n";  
$ret=$ret.'CN      |cow       |mlk tdy 1 |mlk tdy 2 | mlk yst 1'."\r\n";  
$ret=$ret.'RN3900100030000106000300130042003001330420030014204200300145042'."\r\n";  
//$rest = substr("abcdef", 0, -1);  
return $ret;
}



/**
* 
* Alpro has problems, so we only sync cows who are currently in lactation with calving event.
*/
function setCowLatestCalvingDate($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];

$calvingDateX=date('Ymd',strtotime($val['calving_date'])); //always outputs 8 chars, so no trim needed.
//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);

$calvingDateX=str_pad(trim($calvingDateX), 8, ' ', STR_PAD_LEFT);
$ret=$ret."CN Cows latest calving date for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow #|calving date '."\r\n";
$commandStr='DN 390006  00900070 060 00300035 080  '."\r\n";  //change breeding items state
$ret=$ret.preg_replace('/\h+/', '', $commandStr); //replaces spaces and tabs, not line feeds, makes easier to read.
$ret=$ret."VN390006{$cowNumX}{$calvingDateX}"."\r\n";
}
return $ret;
}

private function findCowsLatestCalvingDate() {


    //looks for any cows that are currenty in a dry off state in Little river system. We have to do it this way, because otherwise problems occur with alpro makring cows do not milk.
    $sql2 = <<<SQL
        
    SELECT local_number,fresh_date FROM bovinemanagement.bovinecurr WHERE fresh_date is not null

SQL;
    
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['calving_date']=$row2['fresh_date'];
       
    }
    
    print("\r\nCows who are currently in lacatation with calving date)"."\r\n");
    print_r($dataArrayInternal);   
    
    return $dataArrayInternal;
}



/**
* 
* Alpro has a convoluted way to add inseminations. We really only want to add 1 new insemination.
 * So we just take the latest inseminations and see if they are added and if not then add it. 
 * We then have to change the date of the insemination we just added to the date it actually happened. 
*/
function setCowInseminations($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];
$sireNameX=$val['service_sire_short_name'];
$insemDateX=date('Ymd',strtotime($val['event_time'])); //always outputs 8 chars, so no trim needed.

//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);
$sireNameX=str_pad(trim($sireNameX), 15, ' ', STR_PAD_LEFT);
$insemDateX=str_pad(trim($insemDateX), 8, ' ', STR_PAD_LEFT);
$insemNumberX=str_pad(trim($val['insem_num']), 2, ' ', STR_PAD_LEFT);

$ret=$ret."CN Cows latest insemination date for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow # '.$cowNumX.'|today|sirename|insem date '."\r\n";
$commandStr='DN 390006  00900070 060  00304054 010 00300374 150 00300037 020'."\r\n";  //mark insemination today,
$ret=$ret.preg_replace('/\h+/', '', $commandStr); //replaces spaces and tabs, not line feeds, makes easier to read.
$ret=$ret."VN390006{$cowNumX}1{$sireNameX}{$insemNumberX}"."\r\n";
$commandStr2='DN 390006  00900070 060  00300357 080  '."\r\n";  // then change date to when it actually happened. 
$ret=$ret.preg_replace('/\h+/', '', $commandStr2); //replaces spaces and tabs, not line feeds, makes easier to read.
$ret=$ret."VN390006{$cowNumX}{$insemDateX}"."\r\n";
}
print($ret); 
return $ret;
}

public function findCowsInseminations() {

   //get from INTERNAL system
    //gets the latest completed breeding or embryo for every cow.
    //NOTE: calculate_number_of_breeding_since_fresh only supports breedings, this should be ok, since no more embryo transfers in 2020.
    $sql2 = <<<SQL
        
SELECT recipient_local_number as local_number,service_sire_short_name,event_time,bovinemanagement.calculate_number_of_breeding_since_fresh(table_id)
from (
   SELECT a.*, row_number() over (partition by a.recipient_bovine_id ORDER BY event_time desc) r 
   FROM bovinemanagement.combined_breeding_embryo_view a WHERE event_time is not null AND recipient_bovine_id IN (SELECT id FROM bovinemanagement.bovinecurr) 
) as foo
WHERE r=1 limit 2


SQL;
    
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['service_sire_short_name']=$row2['service_sire_short_name'];
    $dataArrayInternal[$row2['local_number']]['event_time']=$row2['event_time'];
    $dataArrayInternal[$row2['local_number']]['num_insem']=$row2['calculate_number_of_breeding_since_fresh'];
       
    }
    
 //   print("\r\nCows latest insemination date.)"."\r\n");
 //   print_r($dataArrayInternal);   
    
  //  return $dataArrayInternal;
   /**
    * NOW GET COMPARISION LIST FROM ALPRO TO SEE WHAT IS ALREADY IN IT.
    * 
    */ 
    
    //get from ALPRO system
  $ret="";
  $ret=$ret.'CN Get cow last insemination info:'."\r\n";  
  $ret=$ret.'CN            |cow           |insem date     |sire name     |num of insem'."\r\n";  
  $commandStr='RN 390006  00900070 060    00300357 080    00300374 150   00300037 020'."\r\n"; 
  $ret=$ret.preg_replace('/\h+/', '', $commandStr); //replaces spaces and tabs, not line feeds, makes easier to read.
  $alproIsoLink=new AlproIsoLink($ret); //runs the commands on init
 
  foreach ($alproIsoLink->outArray as $key => $value) {
   $cowNo=$value[0];
   $date=$value[1];
   $sireName=$value[2]; 
   $numInsem=$value[3]; //use these for more accurate match in future if necessary
  
   $dataArrayAlpro[$cowNo]['local_number']=$cowNo;
   $dataArrayAlpro[$cowNo]['service_sire_short_name']=$sireName;
   $dataArrayAlpro[$cowNo]['event_time']=$date;
   $dataArrayAlpro[$cowNo]['num_insem']=$numInsem;
}
  print_r($dataArrayAlpro); //DEBUG
  print_r($dataArrayInternal); //DEBUG
     
 //multi dimesional array compare.
 $cowsToMoveInAlproArray = array_map('unserialize',array_diff(array_map('serialize', $dataArrayInternal), array_map('serialize', $dataArrayAlpro)));
 
   print("\r\nCows that need new insemintation inputted to Alpro"."\r\n");
   print_r($cowsToMoveInAlproArray);

   return $cowsToMoveInAlproArray;
    
    
}

/**
*Marks a cow dried off.
* Alpro has problems, so we only sync cows who are currently dried off.
*/
function setCowLatestDriedOff($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];
$breedState=4; //set to 4, dry off to be confirmed
$driedOffBool=1; //set dried off true
$dryDateX=date('Ymd',strtotime($val['dry_date'])); //always outputs 8 chars, so no trim needed.
//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);
$breedStateX=str_pad(trim($breedState), 2, ' ', STR_PAD_LEFT);
$driedOffBoolX=str_pad(trim($driedOffBool), 1, ' ', STR_PAD_LEFT);
$dryDateX=str_pad(trim($dryDateX), 8, ' ', STR_PAD_LEFT);
$ret=$ret."CN Cows is marked as dried off for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow #|dry date '."\r\n";
$commandStr='DN 390006  00900070 060 00300194 080  '."\r\n";  //change breeding items state
$ret=$ret.preg_replace('/\h+/', '', $commandStr); //replaces spaces and tabs, not line feeds, makes easier to read.
$ret=$ret."VN390006{$cowNumX}{$dryDateX}"."\r\n";
}
return $ret;
}

private function findCowsToMarkDriedOff() {


    //looks for any cows that are currenty in a dry off state in Little river system. We have to do it this way, because otherwise problems occur with alpro makring cows do not milk.
    $sql2 = <<<SQL
        
    SELECT * FROM bovinemanagement.drycurr 

SQL;
    
    $res2 = $this->lrDB->query($sql2);
    
    while($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
    
    $dataArrayInternal[$row2['local_number']]['local_number']=$row2['local_number'];
    $dataArrayInternal[$row2['local_number']]['dry_date']=$row2['dry_date'];
       
    }
    
    print("\r\nCows who are currently dry cows)"."\r\n");
    print_r($dataArrayInternal);   
    
    return $dataArrayInternal;
}


} //end class


/*
//comands to run for the class from another script, alproSync.
$xx=new AlproCommands();
//
$xx->syncBovineAlive();
$xx->syncBovineDead();
$xx->syncBovineMove();
$xx->syncDoNotMilk();
$xx->syncDumpMilk();
*/
?>
