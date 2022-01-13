<?php


/* uses criteria to make a list of which cows should change groups */

class GroupChange extends BasePage {

    
    
    public function defaultDisplay() {

        $out = array();
        //needed to know what revenue cutoff is for a group
        $cutOffArr = $this->populateRevenueCutOffVariables();

        
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $a = new QF2_GroupChangeRevenue((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'individual');
            $sql = 'SELECT location as "Group",cutoff as "Cut-Off ($)",userid as "Employee" FROM system.group_change_revenue_cutoff';
            $out[] = (new Bootstrap)->halfHalf((new JQueryDataTable)->startBasicSql('Current Group Change Cutoffs', $sql), $a->renderBox('qf2_GroupChangeRevenue', 'Change Revenue Cut-off for Group'));
        }

        //over 120 DIM.
        //calculate revnue of last 7 days when not in sick group.
        $out[] = $this->displayAnywhereToMaternity();
        $out[] = $this->displayHeiferToLowGroup($cutOffArr['Heifer Group to Low']);
        $out[] = $this->displayHightToLowGroup($cutOffArr['High Group to Low']);
        
        //only show these group changes to owners.
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $out[] = $this->displayLowToDryOff();
            $out[] = $this->displayGeneralDryOffList();
        }

        print(implode($out));
    }

    private function populateRevenueCutOffVariables() {
      
                 $sql = "SELECT * FROM system.group_change_revenue_cutoff";    
                 $statement = $GLOBALS['pdo']->prepare($sql);
                 $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $out=array();
        foreach ($results as $value) {
            $out[$value['location']]=$value['cutoff'];
        }
        
        return $out;
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
    
        return ($dt->start('Anywhere To Maternity', $this->pageid, 'anywhereToMaternity',  null, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'desc' ]],"));    
    }
    
    
    private function cutOffFilter($request) {
         if (!empty($request['item'])) {
        if (filter_var($request['item'], FILTER_VALIDATE_INT)) {
           $cutoff =$request['item'];
       } else {
            throw new Exception('Error: cutoff not valid format.');
        }
        } else {
             throw new Exception('Error: cutoff id specified');
        }
        return $cutoff;
    }
    
    
    function lowToDryOff($request) {
       
        
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
    

    function displayLowToDryOff($cutoff=null) {
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
       
        return ($dt->start('Low to Dry Off', $this->pageid, 'lowToDryOff',  $cutoff, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'asc' ]],"));    
    }
    
    function heiferToLowGroup($request) {
            $cutoff=$this->cutOffFilter($request);
        
        
        $sql = "With temp as (
 SELECT bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.local_number,bovinecurr.location_id, (current_date - bovinemanagement.round_to_nearest_date(fresh_date)) as dim, calculated_potential_due_date, avg_7_day_revenue,
 (SELECT score FROM bovinemanagement.body_condition_score_event WHERE event_time=(SELECT max(event_time) FROM bovinemanagement.body_condition_score_event WHERE bovine_id=bovinecurr.id AND body_condition_score_event.event_time >= (bovinecurr.fresh_date + interval '45 days')::date) AND bovine_id=bovinecurr.id) as bcs
 FROM bovinemanagement.bovinecurr 
 LEFT JOIN batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
   LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
), temp2 as(
SELECT bovine_id,full_name,local_number,dim,calculated_potential_due_date-current_date as calculated_potential_due_date,bcs,avg_7_day_revenue as avg_rev FROM temp WHERE location_id=42 AND dim>= 60 AND calculated_potential_due_date is not null
)
SELECT temp2.*,'Low Group' as whereto FROM temp2 WHERE dim>= 120 AND avg_rev < $cutoff
UNION
SELECT temp2.*,'High Group' as whereto FROM temp2 WHERE dim< 120 AND avg_rev >=$cutoff
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

     function displayHeiferToLowGroup($cutoff) {
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
    
       return ($dt->start('Heifer Group to Low', $this->pageid, 'heiferToLowGroup', $cutoff, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'asc' ]],"));    
    }
    
    
    function hightToLowGroup($request) {
         $cutoff=$this->cutOffFilter($request);
       
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
SELECT bovine_id,	full_name,	local_number,	dim,	calculated_potential_due_date-current_date as calculated_potential_due_date,avg_rev,	bcs,	whereto from temp2 WHERE avg_rev is not null AND  avg_rev <= $cutoff
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
    
    
    function displayHightToLowGroup($cutoff) {
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
     
        return ($dt->start('High Group to Low', $this->pageid, 'hightToLowGroup', $cutoff, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'asc' ]],"));    
    }


    
    

    public function generalDryOffList() {
       
        
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
    
        return ($dt->startCustom('General Dry Off / Due List', 116, 'generalDryOffList', null, $columnsStr, null, "\"order\": [[ 3, 'asc' ]]",false));     //change order to asc for due date
    }

}//end class

Class QF2_GroupChangeRevenue extends QF2_Base {

    function definition($form) {

      
        $revArr=array();
         $revArr[null] = "Please Select one..."; //dummy value
        for ($x = 1; $x <= 50; $x++) {
             $revArr[$x]=$x;
        }

        $listArr=array(null =>"Please Select one..." , "Heifer Group to Low" => "Heifer Group to Low", "High Group to Low" => "High Group to Low"); //removed , "Low to Dry Off" => "Low to Dry Off"

  
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        
        $form->addElement('select', 'group', null, array('options' => $listArr, 'label' => 'Which Group List'))->addRule("required", "Please specify your group list");
        $form->addElement('select', 'revenue', null, array('options' => $revArr, 'label' => 'Revenue Minimum Cut-off ($)'))->addRule("required", "Please specify your revenue cutoff");

        return $form; //required       
    }

    function process($value) {

        //don't put id value, uses next one in DB.
        $query = "INSERT INTO system.group_change_revenue_cutoff (cutoff,location,userid) VALUES ({$value['revenue']},'{$value['group']}','{$GLOBALS['auth']->getUsername()}') ON CONFLICT (location) DO UPDATE  SET cutoff = {$value['revenue']},  userid = '{$GLOBALS['auth']->getUsername()}';";
        $res = $GLOBALS['pdo']->exec($query);
    }

}//end class