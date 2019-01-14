<?php 
require_once('../global.php');



/** This porgram processes valacta's componetns for each group and stores in db, NOTE: these are not mass corrected values */
class AlproProductionForGroups {

  public function __construct() {
		self::main();
	    }

function main() {

print('<h1>Started processing Alpro milk production per group.</h1>'."\n\r");

// Open a transaction
try {$res = $GLOBALS['pdo']->beginTransaction();

//empty table to start
$query = "TRUNCATE batch.alpro_group_production";
$res=$GLOBALS['pdo']->exec($query);


//Big while loop to go through days.
$sinceEpoch=strtotime("2010-02-03 00:00:00"); //date we moved into new barn
while ($sinceEpoch < strtotime("now")) {

//look up number of cow milking for date.
$timeToUseStr = date('Y-m-d 00:00:00',($sinceEpoch));

self::readAndWriteToDB(40,$timeToUseStr);
self::readAndWriteToDB(41,$timeToUseStr);
self::readAndWriteToDB(42,$timeToUseStr);

//incremenet +1 day
$sinceEpoch=strtotime("$timeToUseStr + 1 day");
}

// determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }


print('<h1>Finished processing Alpro milk production per group.</h1>'."\n\r");
}


function readAndWriteToDB($location_id,$timeToUseStr) 
{
 //now do a component lookup for this date and specific group.
$sql="SELECT * FROM alpro.alpro_production_for_group_at_date ($location_id,'$timeToUseStr'::timestamp without time zone) LIMIT 1";
$res = $GLOBALS['pdo']->query($sql);
$row = $res->fetch(PDO::FETCH_ASSOC);

if (is_null($row['total_milk_production']) == true)
{
  $row['avg_milk_production']=0;
  $row['total_milk_production']=0;
}  

//now do an insert
//print("XX: , {$row['alpro_production_for_group_at_date']}, $location_id, $timeToUseStr"."<br/>\n\r");

$sql2="INSERT INTO batch.alpro_group_production (event_time,location_id,avg_milk_production,total_milk_production) VALUES ('$timeToUseStr', $location_id,{$row['avg_milk_production']},{$row['total_milk_production']})";
$res2 = $GLOBALS['pdo']->exec($sql2); 
  
  
}

} //end class

//run it
new AlproProductionForGroups();


?>
