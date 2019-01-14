<?php





;

/* uses criteria to make a list of which cows should change groups */

class GroupChange extends BasePage {

    public function defaultDisplay() {


        //over 120 DIM.
        //calculate revnue of last 7 days when not in sick group.
       $this->displayAnywhereToMaternity();
     
        $this->displayHeiferToNewGroup();
       $this->displayHightToNewGroup();
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $this->displayLowToDryOff();
           $this->displayGeneralDryOffList();
        }
    }

    function anywhereToMaternity() {
        //criteria 1: any cow over 300 DIM and less than 10$ revenue
        $sql = "SELECT *,bovinecurr.id as bovine_id, 'Maternity Pack' as whereto
from bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
WHERE calculated_potential_due_date::date <= (current_date + interval '10 day') AND bovinecurr.location_id !=38
ORDER BY calculated_potential_due_date ASC
";
        
        
          $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    
    
    }

    function displayAnywhereToMaternity() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "calculated_potential_due_date",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            
               
          
            {
                data: "calculated_potential_due_date",
                title: "Days to Due"   
            },                      
            {
                data: "whereto",
                title: "Move To"
            }
            
EX;
       print ("<h3>Anywhere To Maternity</h3>");
        print($dt->start($this->pageid, 'anywhereToMaternity',  null, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'desc' ]],"));    
    }
    
    
    function lowToDryOff() {
        //criteria 1: any cow over 300 DIM and less than 10$ revenue
        $sql = "With temp as (
 SELECT bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.local_number,bovinecurr.location_id, (current_date - bovinemanagement.round_to_nearest_date(fresh_date)) as dim, preg_check_method,	calculated_potential_due_date,avg_7_day_revenue	,
 
(SELECT score FROM bovinemanagement.body_condition_score_event WHERE event_time=(SELECT max(event_time) FROM bovinemanagement.body_condition_score_event WHERE bovine_id=bovinecurr.id AND body_condition_score_event.event_time >= (bovinecurr.fresh_date + interval '45 days')::date) AND bovine_id=bovinecurr.id) as bcs 
FROM bovinemanagement.bovinecurr 
 LEFT JOIN batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
  LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
), temp2 as (
SELECT bovine_id,full_name,local_number,dim,bcs, avg_7_day_revenue as avg_rev,preg_check_method,calculated_potential_due_date-current_date as calculated_potential_due_date
FROM temp  WHERE location_id=41  AND (dim>= 300  OR dim is null)  )
SELECT temp2.*,'Dry Off' as whereto FROM temp2 WHERE calculated_potential_due_date is not null 
UNION
SELECT temp2.*,'Potential Cull' as whereto FROM temp2 WHERE calculated_potential_due_date is null
ORDER BY avg_rev ASC,DIM DESC
";
        //NOTE: we should add to this when we have a cull list and take that into account.
        
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    
    }
    

    function displayLowToDryOff() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "avg_rev",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "dim",
                title: "DIM"
            },
            {
                data: "avg_rev",
                title: "AVG 7 day Revenue ($)"
            },    
            {
                data: "bcs",
                title: "BCS"
            },
            {
                data: "calculated_potential_due_date",
                title: "Days to Due"   
            },                      
            {
                data: "whereto",
                title: "Move To"
            }
            
EX;
       print ("<h3>Low to Dry Off</h3>");
        print($dt->start($this->pageid, 'lowToDryOff',  null, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'asc' ]],"));    
    }
    
    function heiferToNewGroup() {
        $sql = "With temp as (
 SELECT bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.local_number,bovinecurr.location_id, (current_date - bovinemanagement.round_to_nearest_date(fresh_date)) as dim, calculated_potential_due_date, avg_7_day_revenue,
 (SELECT score FROM bovinemanagement.body_condition_score_event WHERE event_time=(SELECT max(event_time) FROM bovinemanagement.body_condition_score_event WHERE bovine_id=bovinecurr.id AND body_condition_score_event.event_time >= (bovinecurr.fresh_date + interval '45 days')::date) AND bovine_id=bovinecurr.id) as bcs
 FROM bovinemanagement.bovinecurr 
 LEFT JOIN batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
   LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
), temp2 as(
SELECT bovine_id,full_name,local_number,dim,calculated_potential_due_date-current_date as calculated_potential_due_date,bcs,avg_7_day_revenue as avg_rev FROM temp WHERE location_id=42 AND dim>= 60 AND calculated_potential_due_date is not null
)
SELECT temp2.*,'Low Group' as whereto FROM temp2 WHERE dim>= 120 AND avg_rev <20
UNION
SELECT temp2.*,'High Group' as whereto FROM temp2 WHERE dim< 120 AND avg_rev >=20
ORDER BY whereto DESC, avg_rev ASC 
";
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    
    }

     function displayHeiferToNewGroup() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "avg_rev",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "dim",
                title: "DIM"
            },
            {
                data: "avg_rev",
                title: "AVG 7 day Revenue ($)"
            },    
            {
                data: "bcs",
                title: "BCS"
            },
            {
                data: "calculated_potential_due_date",
                title: "Days to Due"   
            },                      
            {
                data: "whereto",
                title: "Move To"
            }
            
EX;
       print ("<h3>Heifer Group to Low</h3>");
        print($dt->start($this->pageid, 'heiferToNewGroup', null, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'asc' ]],"));    
    }
    
    
    function hightToNewGroup() {
        //any cow with body condition score over 2.9 past 45 days in milk goes to top of the list.

        $sql = "
        With temp as (
 SELECT bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.local_number,bovinecurr.location_id, (current_date - bovinemanagement.round_to_nearest_date(fresh_date)) as dim, calculated_potential_due_date, avg_7_day_revenue
 ,(SELECT score FROM bovinemanagement.body_condition_score_event WHERE event_time=(SELECT max(event_time) FROM bovinemanagement.body_condition_score_event WHERE bovine_id=bovinecurr.id AND body_condition_score_event.event_time >= (bovinecurr.fresh_date + interval '45 days')::date) AND bovine_id=bovinecurr.id) as bcs
 FROM bovinemanagement.bovinecurr 
  LEFT JOIN batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
   LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
), temp2 as (
SELECT bovine_id,full_name,local_number,dim,calculated_potential_due_date,avg_7_day_revenue as avg_rev,bcs,'Low Group' as whereto FROM temp  WHERE location_id=40  AND (dim>= 120 OR bcs >=2.9)  AND calculated_potential_due_date is not null ORDER BY bcs ASC,avg_rev ASC 
)
SELECT bovine_id,	full_name,	local_number,	dim,	calculated_potential_due_date-current_date as calculated_potential_due_date,avg_rev,	bcs,	whereto from temp2 WHERE avg_rev is not null
        ";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    
    function displayHightToNewGroup() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "avg_rev",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "dim",
                title: "DIM"
            },
            {
                data: "avg_rev",
                title: "AVG 7 day Revenue ($)"
            },    
            {
                data: "bcs",
                title: "BCS"
            },
            {
                data: "calculated_potential_due_date",
                title: "Days to Due"   
            },                      
            {
                data: "whereto",
                title: "Move To"
            }
            
EX;
       print ("<h3>High Group to Low</h3>");
        print($dt->start($this->pageid, 'hightToNewGroup', null, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'asc' ]],"));    
    }


    
    

    public function generalDryOffList($request) {
        $sql = "
        With temp as (
 SELECT bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.local_number,bovinecurr.location_id,location_name, (current_date - bovinemanagement.round_to_nearest_date(fresh_date)) as dim, calculated_potential_due_date, 
 avg_7_day_revenue
 FROM bovinemanagement.bovinecurr 
 LEFT JOIN batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
    LEFT OUTER JOIN  bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
WHERE bovinecurr.location_id  IN (39, 40, 41, 42)
), temp2 as (
SELECT bovine_id,full_name,local_number,location_name,dim,calculated_potential_due_date, avg_7_day_revenue as avg_rev FROM temp   ORDER BY calculated_potential_due_date ,avg_rev ASC 
)
SELECT bovine_id,	full_name,	local_number,	location_name,dim,	calculated_potential_due_date-current_date as calculated_potential_due_date,avg_rev,avg_rev as seven_day_rev from temp2 
        ";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayGeneralDryOffList() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "calculated_potential_due_date",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "calculated_potential_due_date",
                title: "Due Date"   
            },       
            {
                data: "dim",
                title: "DIM"
            },
             {
                data: "avg_rev",
                title: "AVG 7 day Revenue ($)"
            }
            
EX;
        print ("<h3>General Dry Off / Due List</h3>");
        print($dt->startCustom(116, 'generalDryOffList', null, $columnsStr, null, "\"order\": [[ 3, 'asc' ]]",false));     //change order to asc for due date
    }

}//end class
?>