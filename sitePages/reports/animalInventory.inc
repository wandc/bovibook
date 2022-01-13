<?php

/** records an actual cull from the herd and why */
class AnimalInventory extends BasePage {

    public function defaultDisplay() {
        print($this->displayInventoryByCohorts());

        print((new BootStrap)->infoNotify(''));

        $statsCan[]=(new BootStrap)->halfHalf($this->numberOfCalvesIn6Months(2), $this->numberOfCalvesIn6Months(1) );
        $statsCan[]=(new BootStrap)->halfHalf($this->numberOfCalvesIn6Months(4), $this->numberOfCalvesIn6Months(3));
        print( (new BootStrap)->plainCardCollapsed('Stats Canada Bi-Annual Survey: How many calves born alive in a semi-annual period?', implode($statsCan) ) );
        

       
        print( (new BootStrap)->plainCardCollapsed('All Herd Animals', $this->allHerdAnimals()) );
       
         
         
         
    }
    
    
    function allHerdAnimals() {
        
       
$sql = <<<SQL
SELECT *,bovinecurrall.id as animal_bovine_id,bovinecurrall.full_name as animal_full_name, bovinecurrall.local_number as animal_local_number,bovinecurrall.full_reg_number as animal_full_reg_number, sire.short_name as sire_short_name,pregnant_view.calculated_potential_due_date as due_date FROM bovinemanagement.bovinecurrall LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurrall.sire_full_reg_number LEFT JOIN bovinemanagement.pregnant_view on pregnant_view.id=bovinecurrall.id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
          
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLinkHolsteinCanada($row['animal_bovine_id'],$row['animal_local_number'],$row['animal_full_name'],null,null,$row['animal_full_reg_number']);     
            //$rowOut['Full Name'] = ($row['animal_full_name']);
            $rowOut['Reg #'] = ($row['animal_full_reg_number']);
            $rowOut['Sire'] = ($row['sire_short_name']);
            $rowOut['Birth Date'] = JQueryDataTable::formattedDate($row['birth_date']);
            $rowOut['Fresh'] = JQueryDataTable::formattedDate($row['fresh_date']);    
            $rowOut['Due'] = JQueryDataTable::formattedDate($row['due_date']);
            $rowOut['Location'] = ($row['location_name']);
          
          $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;
  
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Inventory', $dataArr,'paging: false')); 
         return implode($out);              
        
    }
    
    

    function numberAliveCalvesFirst6MonthsOfThisYear() {
        $sql = "select count(id) from bovinemanagement.calving_event where event_time >= date_trunc('year', now()) AND event_time <= '2019-06-30' AND alive_or_dead='alive'";
    }

    /*
     * stats canada survey always asked for number of calves in 6 months. 
     * 
     */

    function numberOfCalvesIn6Months($whichOne) {


        switch ($whichOne) {
            case 1:
                //1st two quarters of this year
                $sql = <<<SQL
        with temp as (
SELECT id,alive_or_dead FROM bovinemanagement.calving_event WHERE event_time between date_trunc('year', now()) AND (date_part('year', current_date)|| '-06-30')::date
)
SELECT distinct (Select count(temp.id) FROM temp where temp.alive_or_dead ='alive') as alive,
(Select count(temp.id) FROM temp where temp.alive_or_dead ='dead') as dead,
(Select count(temp.id) FROM temp) as total 
FROM temp
SQL;
                $title = 'Jan 1 to Jun 30 this year';
                break;
            case 2:
                //last two quarters of this year
                $sql = <<<SQL
        with temp as (
SELECT id,alive_or_dead FROM bovinemanagement.calving_event WHERE event_time between (date_part('year', current_date)|| '-07-01')::date AND (date_part('year', current_date)|| '-12-31')::date
)
SELECT distinct (Select count(temp.id) FROM temp where temp.alive_or_dead ='alive') as alive,
(Select count(temp.id) FROM temp where temp.alive_or_dead ='dead') as dead,
(Select count(temp.id) FROM temp) as total 
FROM temp
SQL;
                $title = 'July 1 to Dec 31 this year';
                break;
            case 3:
                //1st two quarters of last year
                $sql = <<<SQL
                 with temp as (
SELECT id,alive_or_dead FROM bovinemanagement.calving_event WHERE event_time between date_trunc('year', date_trunc('year', now() - interval '1 year')) AND (date_part('year', date_trunc('year', now() - interval '1 year'))|| '-06-30')::date
)
SELECT distinct (Select count(temp.id) FROM temp where temp.alive_or_dead ='alive') as alive,
(Select count(temp.id) FROM temp where temp.alive_or_dead ='dead') as dead,
(Select count(temp.id) FROM temp) as total 
FROM temp
SQL;
                $title = 'Jan 1 to Jun 30 last year';
                break;
            case 4:
                //last two quarters of last year  
                $sql = <<<SQL
      with temp as (
SELECT id,alive_or_dead FROM bovinemanagement.calving_event WHERE event_time between (date_part('year', date_trunc('year', now() - interval '1 year'))|| '-07-01')::date AND (date_part('year',  date_trunc('year', now() - interval '1 year'))|| '-12-31')::date
)
SELECT distinct (Select count(temp.id) FROM temp where temp.alive_or_dead ='alive') as alive,
(Select count(temp.id) FROM temp where temp.alive_or_dead ='dead') as dead,
(Select count(temp.id) FROM temp) as total 
FROM temp   
SQL;
                $title = 'July 1 to Dec 31 last year';
                break;
            default:
                throw new Exception("Error: pick a semi-anuual period.");
        }

        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC); //only 1 row
        $html = null;
        if (!empty($row)) {
        $html = <<<HTML
                
                <li>Alive: {$row['alive']}</li>
                 <li>Dead: {$row['dead']}</li>
                 <li>Total: {$row['total']}</li>
        HTML;
        }
        return (new BootStrap)->bootstrapSmallBox($title, $html);
    }

    function displayInventoryByCohorts($mode = 'default') {

        $sql = "with temp as (
Select bovinecurr.*,bovinemanagement.lactation_number(bovinecurr.id),bovinemanagement.last_breeding(bovinecurr.id) FROM bovinemanagement.bovinecurr
)
SELECT 'Milking' as cohort, count(id) FROM temp WHERE fresh_date is not null
UNION
SELECT 'Dry' as cohort, count(id) FROM temp WHERE fresh_date is null and lactation_number > 0
UNION
SELECT 'Bred Heifer' as cohort, count(id) FROM temp WHERE lactation_number = 0 AND last_breeding is not null
UNION
SELECT 'Young Stock' as cohort, count(id) FROM temp WHERE lactation_number = 0 AND last_breeding is null";



      
        $out[] = ( (new JQueryDataTable)->startBasicSql('Current Inventory', $sql));
        return implode($out);
    }

}

//end class    