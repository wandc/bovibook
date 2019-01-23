<?php 
class PreCalving extends BasePage {

   

    public function defaultDisplay() {
        print('<h2> MU-SE three weeks max before calving.</h2>');
        print('<h2> CRC Bolus three weeks max before calving.</h2>');

            $this->displayListOfPreCalvingTasks();
            
      
    }

    //;
    //used by home page to display what needs done for CRC boluses. 
    public static function crcBolusList() {
        //anything in dry pack sick gets bolus starting at 3 weeks before calving. If they make it to the milking line, then this will not ctach them. 
        // has to have more than 1 lactation.
        $sql2="
with temp as (
SELECT *
,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE bovine_id=x.id AND medicine_index=41  AND event_time >= x.calculated_potential_due_date::date - interval '4 weeks') as bolus
 FROM bovinemanagement.pregnant_view  as x 
WHERE bovinemanagement.lactation_number(id) >1 
 AND location_id = ANY ('{37,38,39}'::int[])
 AND (x.calculated_potential_due_date::date - interval '3 weeks') <= current_date
) SELECT * from temp where bolus is null  order by calculated_potential_due_date 
 ";
        $res2 = $GLOBALS['pdo']->query($sql2);
       //get data
        $results = $res2->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
             $row['home_page']=$GLOBALS['MiscObj']->hrefToBovinePageFullName($row['id'],$row['local_number'],$row['location_current_name']);
          }
          
        //output
        $output['data'] = $results;
        return ($output); 
        
    }
    
    
    
     public function listOfPreCalvingTasks() {
         
     
      $sql = "
   SELECT x.id as bovine_id,*,calculated_potential_due_date as event_time2,bovine.full_name,preg_check_comment,
(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE bovine_id=x.id AND medicine_index=35  AND event_time >= x.calculated_potential_due_date::date - interval '3 weeks') as muse,
(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE bovine_id=x.id AND medicine_index=41  AND event_time >= x.calculated_potential_due_date::date - interval '3 weeks') as bolus
FROM bovinemanagement.pregnant_view x
LEFT JOIN bovinemanagement.bovine ON bovine.id=x.id
WHERE calculated_potential_due_date <= current_Date + interval '10 weeks'
    ";
    
      
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
              $row['number_name'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
           $row['event_time_raw']=strtotime($row['event_time2']);
  
           $row['event_time2']= date("M d, Y",strtotime($row['event_time2']));  //has to be at end.
           
           if (empty($row['muse']) OR empty($row['bolus'])) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            
           
        }
        //output
        $output['data'] = $results;
        return ($output);
    }
      
     function displayListOfPreCalvingTasks() {
        $dt = new JQueryDataTable();
        
        
        $columnsStr = <<<EX
               
            {
                data: "event_time_raw",
                visible: false,
                searchable: false
            },
            {
                data: "number_name",
                title: "Animal",
                type: "num-html"
            },          
            {
                data: "event_time2",
                title: "Due Date",
                type: "date"
            },
            {
                data: "preg_check_comment",
                title: "Preg Check Comment"
            },
            {
                data: "location_current_name",
                title: "Location"
            },
            {
                data: "muse",
                title: "Muse"
            },
            {
                data: "bolus",
                title: "Bolus"
            }
           
EX;
        print("<h3>Last 45 days Pre-Calving Tasks</h3>");
        print($dt->start($this->pageid, 'listOfPreCalvingTasks', null, $columnsStr,'highlight_important',null));
    } 
      
     
    

    public function error() {
        //throw an error if the treatments are overtime. 
    }

}//end class
?>