<?php 
require_once('../global.php');


/**
* This class runs long queries in btach scheme of DB. should be run every few hours by cron script.
*
*/
class NumberMilking {

  public function __construct() {
		self::mainNumbermilkingPerDay();
	    }
  
  
function mainNumbermilkingPerDay() {
print('<h1>Started processing number of cows milking per day.</h1>\n');

// Open a transaction
try {$res = $GLOBALS['pdo']->beginTransaction();

//empty the table.
$query = "TRUNCATE batch.daily_number_cows_milking";
 
$res=$GLOBALS['pdo']->exec($query);


//Big while loop to go through days.
$sinceEpoch=strtotime("2009-01-01 00:00:00"); //june 1 2008 is arbitrary...might get too long in the future because every query takes 1 sec.
while ($sinceEpoch < strtotime("now")) {

//look up number of cow milking for date.
$timeToUseStr = date('Y-m-d 00:00:00',($sinceEpoch));
$retArray=self::findNumberOfMilkingCowsOnSpecificDate($timeToUseStr);


//print("date0:$timeToUseStr<br>");

//insert the date and number of cows into DB for taht paticular day.
self::insertIntoDB($timeToUseStr,$retArray);

//incremenet +1 day
$sinceEpoch=strtotime("$timeToUseStr + 1 day");
}

// determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }

print('<h1>Finished processing number of cows milking per day.</h1>\n');
}

function insertIntoDB($date,$retArray)
{
print("date:$date<br>");
//don't put id value, uses next one in DB.
$query = "INSERT INTO batch.daily_number_cows_milking (date,number_of_cows,average_DIM,total_milking_high,total_milking_low,total_milking_heifer) VALUES ('$date',{$retArray['numberOfCowsMilking']},{$retArray['averageDIM']},{$retArray['total_milking_high']},{$retArray['total_milking_low']},{$retArray['total_milking_heifer']})";
 
$res=$GLOBALS['pdo']->exec($query);

}

/* TODO:: 2015 Dec: tried to do this with DB functions, too slow. Maybe in the future?
 *  
  select batch.bovine_milking_number_for_date(date::date) ,date 
from generate_series(
  '2015-02-01'::date,
  '2015-03-01'::date,
  '1 day'::interval
) date;
 * 
 * 
 * 
 * 
 * 
 * 
 */




function findNumberOfMilkingCowsOnSpecificDate($dateOfInterest)
{
//crazy query to give a list of milking cows on a specific date
//and when there fresh date was.
$query="
    with temp as (
    SELECT local_number,d1.event_time,name,milking_location,location.id as loc_id,date_part('day','$dateOfInterest'-fresh_date) as dim from bovinemanagement.location_event d1 
JOIN bovinemanagement.location ON d1.location_id = location.id
JOIN bovinemanagement.bovine ON d1.bovine_id=bovine.id
JOIN bovinemanagement.lactation ON d1.bovine_id=lactation.bovine_id
WHERE d1.id in
(select id FROM bovinemanagement.location_event d2 WHERE d2.bovine_id = d1.bovine_id AND event_time <= '$dateOfInterest' ORDER BY event_time DESC LIMIT 1) 
AND fresh_date = (select fresh_date FROM bovinemanagement.lactation d3 WHERE d3.bovine_id = d1.bovine_id AND fresh_date <= '$dateOfInterest' ORDER BY fresh_date DESC LIMIT 1) 
AND (bovine.death_date IS NULL OR bovine.death_date > '$dateOfInterest')
AND milking_location=true 
ORDER BY d1.bovine_id asc
) 
SELECT sum(dim) as sum_dim, count(local_number) as total_milking,
(SELECT count FROM (SELECT count(temp.local_number) ,temp.loc_id FROM temp group by temp.loc_id) x40 WHERE loc_id=40) as total_milking_high,
(SELECT count FROM (SELECT count(temp.local_number) ,temp.loc_id FROM temp group by temp.loc_id) x41 WHERE loc_id=41) as total_milking_low,
(SELECT count FROM (SELECT count(temp.local_number) ,temp.loc_id FROM temp group by temp.loc_id) x42 WHERE loc_id=42)as total_milking_heifer
FROM temp  limit 1  
    
    ";
$res = $GLOBALS['pdo']->query($query);

//find number of cows milking
$numberOfCowsMilking = $res->rowCount();
$numberOfCowsMilkingShipping=0;

$row = $res->fetch(PDO::FETCH_ASSOC);


//find average days in milk
$averageDIM=$row['sum_dim'] / $row['total_milking'];

$retArray['numberOfCowsMilking']=$row['total_milking'];
$retArray['numberOfCowsMilkingShipping']=$numberOfCowsMilkingShipping;
$retArray['averageDIM']=$averageDIM;

//when there are none in the group pass a 0 to the db
if ($row['total_milking_high']==null) {$row['total_milking_high']=0;}
if ($row['total_milking_low']==null) {$row['total_milking_low']=0;}
if ($row['total_milking_heifer']==null) {$row['total_milking_heifer']=0;}

$retArray['total_milking_high']=$row['total_milking_high'];
$retArray['total_milking_low']=$row['total_milking_low'];
$retArray['total_milking_heifer']=$row['total_milking_heifer'];

//return array
return $retArray;
}

} //end class

//run it
new NumberMilking();

?>