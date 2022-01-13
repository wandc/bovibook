<?php
class ReportsRFIDStats extends BasePage {

    public function defaultDisplay() {

        $this->showListOfCowsLast2DaysManualId();


        $this->displayAllCowsLastEvent();


        $this->displaylast25daysRFIDReadPercentage();
    }

    function last25daysRFIDReadPercentage() {
        /* starts at yesterday, only does milking number 1, assume time of group at 7:30am, does not include sick group */
        $sql = "
with temp as (
select date 
from generate_series(
  (current_date - interval '1 day')::date,
  (current_date - interval '45 day')::date,
  '-1 day'::interval
) date
), temp2 as (
SELECT date,bovinemanagement.location_total_at_timestamp (40, date::date)+bovinemanagement.location_total_at_timestamp (41, date::date)+bovinemanagement.location_total_at_timestamp (42, date::date) as possible_total,
(SELECT count(*) AS read_total FROM (SELECT distinct bovine_id from alpro.cow WHERE date=temp.date AND manualid=false AND milking_number=1 AND bovinemanagement.location_id_at_timestamp (bovine_id, (temp.date + '07:30'::time)::timestamp) IN(40,41,42))  s) 
 from temp
)
SELECT *,round((read_total::numeric/possible_total::numeric),3)*100 as read_percent from temp2

";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            //nothing
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function displaylast25daysRFIDReadPercentage() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                         
            {
                data: "date",
                title: "Date"
            },
            
            {
                data: "possible_total",
                title: "# Cows Milking"
            },
        {
                data: "read_percent",
                title: "Read %"
            }
           
EX;
        print("<h3>Last 45 days RFID Read Percentage</h3>");
        print($dt->start('Title CHANGE ME', $this->pageid, 'last25daysRFIDReadPercentage', null, $columnsStr));
    }

    function sortGateAndParlorLast4Days() {
        /* ignores sick group */
        $sql = "with temp as (
SELECT id as bovine_id, local_number, full_name,location_id ,
(SELECT COALESCE((extract(epoch from event_time))::int::bool,false) FROM alpro.sort_gate_log WHERE x.id=sort_gate_log.bovine_id AND event_time::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking1 )	limit 1) as sortgate_milking1,
(SELECT COALESCE((extract(epoch from event_time))::int::bool,false) FROM alpro.sort_gate_log WHERE x.id=sort_gate_log.bovine_id AND event_time::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking2 )	limit 1) as sortgate_milking2,
(SELECT COALESCE((extract(epoch from event_time))::int::bool,false) FROM alpro.sort_gate_log WHERE x.id=sort_gate_log.bovine_id AND event_time::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking3 )	limit 1) as sortgate_milking3,
(SELECT COALESCE((extract(epoch from event_time))::int::bool,false) FROM alpro.sort_gate_log WHERE x.id=sort_gate_log.bovine_id AND event_time::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking4 )	limit 1) as sortgate_milking4,
/* parlor */
(SELECT (NOT manualid) FROM alpro.cow WHERE x.id=cow.bovine_id AND milktime::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking1 )	limit 1) as parlor_milking1,
(SELECT (NOT manualid) FROM alpro.cow WHERE x.id=cow.bovine_id AND milktime::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking2 )	limit 1) as parlor_milking2,
(SELECT (NOT manualid) FROM alpro.cow WHERE x.id=cow.bovine_id AND milktime::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking3 )	limit 1) as parlor_milking3,
(SELECT (NOT manualid) FROM alpro.cow WHERE x.id=cow.bovine_id AND milktime::timestamptz <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking4 )	limit 1) as parlor_milking4

FROM bovinemanagement.bovinecurr x WHERE location_id in(40,41,42)
)
SELECT *
 from temp";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayAllCowsLastEvent() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                         
            {
                data: "local_number",
                title: "Animal"
            },
            
            {
                data: "sortgate_milking1",
                title: "sortgate_milking1"
            },
        {
                data: "sortgate_milking2",
                title: "sortgate_milking2"
            },
        {
                data: "sortgate_milking3",
                title: "sortgate_milking3"
            },
        {
                data: "sortgate_milking4",
                title: "sortgate_milking5"
            },
        {
                data: "parlor_milking1",
                title: "parlor_milking1"
            },
        {
                data: "parlor_milking2",
                title: "parlor_milking2"
            },
        {
                data: "parlor_milking3",
                title: "parlor_milking3"
            }
           
EX;
        print("<h3>Last 2 days Read: Parlor and Sort Gate</h3>");
        print($dt->start('Title CHANGE ME', $this->pageid, 'sortGateAndParlorLast4Days', null, $columnsStr));
    }

    function showListOfCowsLast2DaysManualId() {
   print("<h3>RFID didn't read the last two days and should of.</h3>");
        
        $sql = <<<SQL
with temp as (
SELECT bovinecurr.id as bovine_id,local_number,full_name,location_name
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -1 and milking_number=1) as t11
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -1 and milking_number=2) as t12
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -2  and milking_number=1) as t21 
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -2  and milking_number=2) as t22
FROM bovinemanagement.bovinecurr
 WHERE location_id IN(40,41,42) 
), temp2 as (
SELECT bovine_id,local_number,location_name,full_name ,sum(t11::int+t12::int+t21::int+t22::int)  as total_manual FROM temp group by bovine_id,local_number,full_name,location_name 
)
SELECT * from temp2 WHERE total_manual is null OR total_manual >=3 order by local_number   
SQL;
    
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $rowOut['Animal'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $rowOut['Total Manual'] = ($row['total_manual']);
            $rowOut['Location'] = ($row['location_name']);
            $output[]=$rowOut;
        }
           print( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $output));

    }

    //shows error.
    function error() {

        ////////////////////////
        //shows cows who yesterday and the day before didn't RFID read in the parlor
        //doesn't show when the tag has been on the order list for less than 2 weeks.
        $sql = "with temp as (
SELECT bovinecurr.id as bovine_id,local_number,full_name
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -1 and milking_number=1) as t11
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -1 and milking_number=2) as t12
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -2  and milking_number=1) as t21 
, (SELECT manualid FROM alpro.cow WHERE bovine_id=bovinecurr.id AND date=current_date -2  and milking_number=2) as t22
FROM bovinemanagement.bovinecurr
 WHERE 	(location_id=40 OR location_id=41 OR location_id=42 OR location_id=39) 
)
SELECT *,(SELECT id FROM bovinemanagement.eartag WHERE bovine_id=temp.bovine_id AND side='right' 
AND (ordered_event_time > (current_date - 14) OR ordered_event_time is null )) 
FROM temp WHERE  ( t11=true AND t12=true AND  t21=true AND t22=true) AND 
(SELECT id FROM bovinemanagement.eartag WHERE bovine_id=temp.bovine_id AND side='right' 
AND (ordered_event_time > (current_date - 14) OR ordered_event_time is null )) != NULL
	     ";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {



            $error["{$row['local_number']}"] = new notifyObj(get_class($this), date('r', strtotime("now")), 3, $row['local_number'] . ' needs RFID tagged ordered.');
        }
        if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
    }

}

//end class
?>