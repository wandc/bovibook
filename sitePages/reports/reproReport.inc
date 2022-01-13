<?php 
class ReproReport extends BasePage {



	public function defaultDisplay() {
	
	//do 21d breeding rate
	$timeWindow=21;
	$skipDays=2;
	$retArray=$this->calculateTwentyOneDayBreedingAverage($skipDays,$timeWindow);
	$chartTitle="$timeWindow Day Breeding Rate";
	$this->plotChart($retArray,$timeWindow,$chartTitle);
	

	//do preg estimate
	$retArray=$this->calculatePregnancyEstimateFromBreedingRate(1,21,300,$conceptionProbability=.42);
        $chartTitle="$timeWindow d Pregnancy Rate PROJECTION $conceptionProbability";
        
	$this->plotChart($retArray,$timeWindow=21,$chartTitle);
	
        print($this->cidrSyncBenchmark());
        
	}

public function calculateTwentyOneDayBreedingAverage($skipDays=5,$timeWindow=21,$dataPoints=500) {

//we seem to be able to have 150 data points or so. 
//so calculate backwards from todays date based on number of skip days.
$numberOfDaysBack=$dataPoints*$skipDays;

$startDate=date('Y-m-d 00:00:00',strtotime("-$numberOfDaysBack days"));
$stopDate=date('Y-m-d 00:00:00',strtotime("now"));

$i = 0;
$stop=false;
while ($stop!=true) {

$indexDate = date( 'Y-m-d 00:00:00', strtotime( "+$i day", strtotime($startDate) ) );
$windowEndDate= date( 'Y-m-d 00:00:00', strtotime( "+$i day +$timeWindow day", strtotime($startDate) ) );

$sql="WITH temp_table AS (
SELECT bovine_id,actual_breeding_time FROM bovinemanagement.breeding_event WHERE actual_breeding_time >= (timestamp '$startDate'::timestamp  + interval '$i days') AND actual_breeding_time < (timestamp '$startDate' + interval '$i days' + interval '$timeWindow days')
)
SELECT (timestamp '$startDate'::timestamp  + interval '$i days') as date,count(temp_table.bovine_id) as twenty_one_day_breeding FROM temp_table LIMIT 1";
$res = $GLOBALS['pdo']->query($sql);
$row = $res->fetch(PDO::FETCH_ASSOC);

//correct date for timewindow period,
$correctedDate=strtotime(date('Y-m-d 00:00:00',strtotime($row['date']) + $timeWindow*86400));

$retArray[$correctedDate]=$row['twenty_one_day_breeding'];

if ($windowEndDate>=$stopDate) {$stop=true;}
$i=$i+$skipDays;
}

return $retArray;
}


public function calculatePregnancyEstimateFromBreedingRate($skipDays=2,$timeWindow=21,$dataPoints=500,$conceptionProbability=.42) {
    $newArray=array();
//calculate breeding rate.
$retArray=$this->calculateTwentyOneDayBreedingAverage($skipDays,$timeWindow);

foreach ($retArray as $key => $value) { 
$newKey = $key + 279 * 86400; //add gestation period
$newValue=round($value*$conceptionProbability,1); //take a guess at conception probability.
$newArray[$newKey]=$newValue;
}
return $newArray;
}



function plotChart($dataArray,$timeWindow,$chartTitle) {
               
               		
	        print('<div class="lactationCurve">');
               
		print("<h3>$chartTitle</h3>");
                 
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['Preg Rate'] = 'number';
                $x=new GoogleVisualizationLine( null, null,$headerStrArr,$dataArray);
                print($x->toString());
		
                
		print('</div> <!-- end LactationCurve -->');

               
 }

 /*
  * shows how well we are doing with CIDR sync
  */
 function  cidrSyncBenchmark() {
     $sql=<<<SQL
             with temp as (
SELECT protocol_event.id, local_number,date_start as protocol_start_date, name as protocol_name, (date_start+ interval '10 days')::date as conception_date,
 bovinemanagement.pregnant_at_timestamp(bovine_id, date_start+ interval '14 days') as due_date
FROM bovinemanagement.protocol_event
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN bovinemanagement.protocol_type ON protocol_event.protocol_type_id=protocol_type.id 
WHERE name LIKE 'CIDR%' 
) 
SELECT temp.*, daterange( (due_date::date-interval '278 days')::date,(due_date::date) ) as preg_range
FROM temp
SQL;
     
     return ( (new JQueryDataTable)->startBasicSql('CIDR Sync Benchmark', $sql)); 
 }
 
 
}//end class

/**
 
 generate a 21 day series 
with temp as (
SELECT date_trunc('day', dd):: timestamp as series
FROM generate_series
        ((current_date-interval '2 years'+ interval '21 days')::timestamp 
        , current_date::timestamp
        , '21 day'::interval) dd
 )
 count distinct animals bred in period, assumes they were  eligible to breed 
, temp2 as (
SELECT series
, (SELECT count(DISTINCT  recipient_bovine_id) from  combined_breeding_embryo_view WHERE series > event_time AND (series -interval '21 days') <= event_time) as distinct_bred_count
,(SELECT count(pregnant_at_timestamp (recipient_bovine_id, series)) FROM combined_breeding_embryo_view WHERE series > event_time AND (series -interval '21 days') <= event_time) as xxx
 FROM temp
)
SELECT * from temp2

 **/

?>