<?php 
/**
* This file mostly exists so we can seperate the core sort gate code to do with syncing, so it then can be called from the command line.
*
*/
if (defined('STDIN')) { //when called from cli, command line
$_SERVER['DOCUMENT_ROOT']='/var/www/int/';
}
include_once($_SERVER['DOCUMENT_ROOT'].'functions/alproIsoLink.inc');
include_once($_SERVER['DOCUMENT_ROOT'].'functions/alproODBCLink.php');

class AlproSortHelper {
  
  
//called by phpcronscript calss alprosortandmovementsync  
 public function sse($request=null) {
   $this->sync(false); 
}    
  
 /**
  * This code is run as a cron script to keep alpro and postgres in sync.
  * NOTE: normally mute the output of sse messages.
  */
  public function sync($sseMute=true) {
     
   SSE_Message::send_message(5, 'Contacting Alpro to get state.' , 5,$sseMute); //sse    
  //first read the alpro state for all cows to find any who have been sorted.
  $outAlpro=self::getCowSortInfo(0);
  SSE_Message::send_message(50, 'Alpro done.' , 50,$sseMute); //sse   
  $dataArrayAlpro=self::processAlproSortReadData($outAlpro);

  //now read the state of postgresql for the current am or pm period.
  $dataArrayPostgres=self::findAnimalsSetToSort();
  
  //now get data from ODBC link on actual time cow walked through sort gate
  //and what happened when it did.
    $sortGateArray=$this->getAlproSortGateState(); //get data via odbc
  
  SSE_Message::send_message(95, 'Process Animals.' , 95,$sseMute); //sse   
  if (is_array($dataArrayPostgres)==true) {
  foreach ($dataArrayPostgres as $key => $value) {
  //DEBUG//print("Doing Animal: ".$value['local_number']."\n");
        $alproStateOut=$this->findIfCowSortedInPeriod($value['local_number'],$value['event_time'],$sortGateArray);
    
    //if a date was returned (thus it is in the right period, update it 
    if ($alproStateOut != false)
    {
        $res = $GLOBALS['pdo']->exec("UPDATE bovinemanagement.location_sort set actual_sort_time='$alproStateOut' WHERE bovine_id='{$value['bovine_id']}' AND estimated_sort_time='{$value['event_time']}'");
        $value['yes_no']=0; //sorted completed so set to no.
    }
    
    //when true set the cow to be sorted.
    if ($value['yes_no'] ==1) {
      
     
	
     $cowNumberArray=null;
     $cowNumberArray[$value['local_number']]['local_number']=$value['local_number'];
     $cowNumberArray[$value['local_number']]['sort_to_area']=1; //sort gate area to sort too. (always the sort pen) 
     $cowNumberArray[$value['local_number']]['yes_no']=1;
     if (date('a',strtotime($value['event_time']))=='am') {
	$cowNumberArray[$value['local_number']]['notify_milking1']=1;
	$cowNumberArray[$value['local_number']]['notify_milking2']=0;
      }
      else {
	$cowNumberArray[$value['local_number']]['notify_milking1']=0;
	$cowNumberArray[$value['local_number']]['notify_milking2']=1;
      }
     
     
     $output=self::setCowSortInfo($ret='',$cowNumberArray);
      
    }
    
    
  }//end foreach
  }//end is_array
   
   SSE_Message::send_message('CLOSE', 'Process complete',100,$sseMute);//sse
  } //end function

   /*
   * find if cow sorted or not, if so return the date.
   */
  function findIfCowSortedInPeriod($local_number,$periodDate,$sortGateArray) {
    
  
    
    //pull out the data for the local_number of interest.
    if (array_key_exists($local_number, $sortGateArray)) {
      $cowArray=$sortGateArray[$local_number];
    }
    //couldn't find data so throw an error
    else {
    throw new Exception("The cow $local_number does not exist in the odbc array returned from the sort gate.</br>");
    }  
    
    //now look in array for sort time in this period, 
    if ( ((date('a',strtotime($periodDate)) == 'am') && (date('a',strtotime($cowArray['event_time']))== 'am')) OR ((date('a',strtotime($periodDate)) == 'pm') && (date('a',strtotime($cowArray['event_time']))== 'pm')) )
    {    
    if ((date('Y-m-d',strtotime($periodDate))) == (date('Y-m-d',strtotime($cowArray['event_time'])))) { return $cowArray['event_time']; }
    else { return false; } //no sort occured in period
    }
    else { return false; }  //no sort occured in period
  }  
  
   protected function findAnimalsSetToSort()
  {
  //find all the animals who are set to be sorted who are IN THIS PERIOD.
    //if it is 12 hours past when the estimated_sort_time was suppose to occur ignore it.
    // if it is 12 hours more in the future ignore it.
    // we basically just want to find if we are currently in the am or pm period and set those cows.
    
    //find what period we are right now.
    if (self::currentPeriod()=='am')
    {
    //am
    $str="(estimated_sort_time between (CURRENT_DATE + time '00:00:00')  AND (CURRENT_DATE + time '11:59:59'))";    
    }
    else {
    //pm
     $str="(estimated_sort_time between (CURRENT_DATE + time '12:00:00') AND (CURRENT_DATE + time '23:59:59'))";    
    }
    
 
    $sql="SELECT location_sort.id,bovine_id,estimated_sort_time,actual_sort_time,userid,local_number FROM bovinemanagement.location_sort
   LEFT JOIN bovinemanagement.bovine ON bovine.id=location_sort.bovine_id
   WHERE $str 
   ORDER BY location_sort.estimated_sort_time DESC";
   
  $res = $GLOBALS['pdo']->query($sql);
  
  $dataArrayInternal=null;
   while($row = $res->fetch(PDO::FETCH_ASSOC)) {
      $dataArrayInternal[$row['local_number']]['bovine_id']=$row['bovine_id'];
      $dataArrayInternal[$row['local_number']]['local_number']=$row['local_number'];
      $dataArrayInternal[$row['local_number']]['sort_to_area']=1; //sort gate area to sort too. (always the sort pen) 
      $dataArrayInternal[$row['local_number']]['event_time']=$row['estimated_sort_time'];
      
      
      if ($row['actual_sort_time']==null)  {
      $dataArrayInternal[$row['local_number']]['yes_no']=1; //set to one to sort and 0 to say not to sort
      }
      else {
      $dataArrayInternal[$row['local_number']]['yes_no']=0; //set to one to sort and 0 to say not to sort
      }
      
      }
 return $dataArrayInternal;   
  }
  
  //return am or pm
  public function currentPeriod() {
  return date('a',strtotime("now"));
  }
  public function currentPeriodStartTime() {
   if (self::currentPeriod()=='am')
	        {  $timeTemp='00:00:00'; }
	        else
	         {  $timeTemp='12:00:00'; }
   return $timeTemp;
  }
  
    protected function processAlproSortReadData($arrayIn) {
        $dataArrayAlpro=array();
  foreach ($arrayIn as $key => $value) {
      if (!empty($value[0])) {
   $dataArrayAlpro[$value[0]]['local_number']=$value[0];
   $dataArrayAlpro[$value[0]]['yes_no']=$value[2];
  }}
   return $dataArrayAlpro;
  }
  
/** Marks a cow to be sorted once only at next milking
*/
/***
Sort Areas in Alpro:
1 Sort Pen
2 Fresh Sick
3 Heifer
4 High
5 Low
*/
public function setCowSortInfo($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];
$areaNum=$val['sort_to_area'];
$sortOnce=$val['yes_no']; //0=no 1=yes.
//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);
$areaNumX=str_pad(trim($areaNum), 2, ' ', STR_PAD_LEFT);
$sortOnceX=str_pad(trim($sortOnce), 1, ' ', STR_PAD_LEFT);
$ret=$ret."CN Sort once next milking for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow #   |   area # | sort once| '."\r\n";
$ret=$ret.'DN390045009000700600030002402000300096010'."\r\n";
$ret=$ret."VN390045{$cowNumX}{$areaNumX}{$sortOnceX}"."\r\n";
$ret=self::setSortStatusOnMPC($ret,$cowNumberArray);
}
$outArray=self::readWriteAlpro($ret,'bovinemanagement','location_sort');
return $outArray;
}

//assumes internal is master and alpro is slave
public function getCowSortInfo($cowNum) {
//get from ALPRO system
  $ret="";
  $ret=$ret."CN Get sort infor for cow number $cowNum"."\r\n";
  $ret=$ret.'CN      |cow #   |   area # | sort once| '."\r\n";
  $ret=$ret.'RN390045009000700600030002402000300096010'."\r\n";
  $outArray=self::readWriteAlpro($ret,'bovinemanagement','location_sort');
  return $outArray;
}



/**
*Marks a cow to be sorted on parlor MPC
*/
function setSortStatusOnMPC($ret='',$cowNumberArray) {
foreach ($cowNumberArray as $cowNum => $val) {
$cowNum=$val['local_number'];
$milking1=$val['notify_milking1']; //if we don't set a milking number too, this does nothing.
$milking2=$val['notify_milking2'];
//spacing is absolutely critical with this protocol. 
$cowNumX=str_pad(trim($cowNum), 6, ' ', STR_PAD_LEFT);
$milking1X=str_pad(trim($milking1), 1, ' ', STR_PAD_LEFT);
$milking2X=str_pad(trim($milking2), 1, ' ', STR_PAD_LEFT);
$ret=$ret."CN Display Sort Cow on MPC for cow number $cowNum"."\r\n";
$ret=$ret.'CN      |cow #         |days  |  milking_am | milking_pm'."\r\n";
$ret=$ret.'DN390012009000700600030004701000300048010'."\r\n";
$ret=$ret."VN390012{$cowNumX}{$milking1X}{$milking2X}"."\r\n";
}
return $ret;
}

/** /////////////////////////////////////////////////////////////////////////////////// **/
/**
* Generic function to read or write to alpro
*/
public function readWriteAlpro($inFileText,$syncSchema,$syncTable) {
$state='true';//assume state will work.
 
 try {
 $alproIsoLink=null;
 $alproIsoLink=new AlproIsoLink($inFileText); //runs the commands on init
 $logResult=$alproIsoLink->logArray; //get the results from the WRITE!
 $outResult=$alproIsoLink->outArray;
 $inResult=$alproIsoLink->inArray;
 //catch exception
 }
 catch(Exception $e)
  {
  print('Message: ' . $e->getMessage());
  $state='false'; //says sync was unsucessful
  }
    //finally write results to table in db that says if we are synced or not.
  $inResult="";
  $alproIsoLink->writeSync($syncSchema,$syncTable,$state,$alproIsoLink->inArray,$alproIsoLink->outArray,$alproIsoLink->logArray,$GLOBALS['pdo']);

return $alproIsoLink->outArray;
}
/** /////////////////////////////////////////////////////////////////////////////////// **
 */


/*
 * contact alpro though odbc to get current sort gate state.
 */
public function getAlproSortGateState() {
 $x=new AlproODBCLink();
 $arrayOut=$x->sortGateQuery();
 $x->storeSortGateLogInDB($arrayOut); //store in postgres.
 asort($arrayOut); //sorts array on output by key, because sql sort not supported?
return $arrayOut;
}  


}//end class
?>