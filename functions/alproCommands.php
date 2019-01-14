<?php 
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
    $_SERVER['DOCUMENT_ROOT']='/var/www/int/';
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/misc.inc');
}
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
 $ret=$this->setCowDriedOff(null,$cowsToMarkDriedOffArray);
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
  $alproIsoLink::writeSync('bovinemanagement','lactation',$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$this->lrDB);
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
    $sql2="SELECT local_number,(SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding(bovinecurr.id)) as milk_withholding,bovinecurr.fresh_date FROM bovinemanagement.bovinecurr"; 
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
*Marks a cow dried off.
*/
function setCowDriedOff($ret='',$cowNumberArray) {
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
$ret=$ret.'CN      |cow #|stat|bool |date '."\r\n";
$ret=$ret.'DN39000690007030032304061'."\r\n";
$ret=$ret."VN390006{$cowNumX}{$breedStateX}{$driedOffBoolX}"."\r\n";
}
return $ret;
}

private function findCowsToMarkDriedOff() {

    //looks for any cows that are currenty in a dry off state in Little river system.
    $sql2="SELECT * FROM bovinemanagement.drycurr"; 
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
//comands to run for the class from another script.
$xx=new AlproCommands();
//
$xx->syncBovineAlive();
$xx->syncBovineDead();
$xx->syncBovineMove();
$xx->syncDoNotMilk();
$xx->syncDumpMilk();
*/
?>
