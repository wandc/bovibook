<?php 

class ReplacementPerformance extends BasePage {

    public function defaultDisplay() {
        print("helloWORLD");
      // print($this->displayPerformanceChart());
       $this->displayChart(); 
       
    }
//end function

    
    
    /*
     *paramaters for graph:
     *  1. animal number
     *  2. first lactation revenue
     *  3. number of days in a specific group
     * 
     */

     function displayPerformanceChart($mode = 'default') {
         
         /* remove things without a 100 days revenue, but keep things that were never in group 57. Many aniamls were sold, some good and some bad. */
  $sql = <<<SQL
          With temp as (
SELECT *,
bovinemanagement.location_daily_count_for_bovine_for_time_start_stop(bovine.id,57,'2012-01-01',current_date)  
FROM bovinemanagement.bovine 
WHERE birth_date >= '2012-01-01'
), temp2 as (
SELECT id,local_number,full_name,birth_date,location_daily_count_for_bovine_for_time_start_stop,(SELECT  fresh_date FROM bovinemanagement.lactation where bovine_id=temp.id order by fresh_date DESC limit 1)
from temp 
), temp3 as (SELECT *, batch.calculate_milk_revenue_per_cow_per_date(temp2.id, (fresh_date + interval '100 days')::date)
 FROM temp2 where fresh_date is not null
)
SELECT * FROM temp3 WHERE calculate_milk_revenue_per_cow_per_date is not null
order by location_daily_count_for_bovine_for_time_start_stop
SQL;     
       
        

         $out[]=("<h3>Current </h3>");
         $out[]=( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql)); 
         return implode($out); 

    }
    
    function displayChart() {

 $sql = <<<SQL
          With temp as (
SELECT *,
bovinemanagement.location_daily_count_for_bovine_for_time_start_stop(bovine.id,46,'2012-01-01',current_date)  
FROM bovinemanagement.bovine 
WHERE birth_date >= '2012-01-01'
), temp2 as (
SELECT id,local_number,full_name,birth_date,location_daily_count_for_bovine_for_time_start_stop,(SELECT  fresh_date FROM bovinemanagement.lactation where bovine_id=temp.id order by fresh_date DESC limit 1)
from temp 
), temp3 as (SELECT *, batch.calculate_milk_revenue_per_cow_per_date(temp2.id, (fresh_date + interval '100 days')::date)
 FROM temp2 where fresh_date is not null
)
SELECT * FROM temp3 WHERE calculate_milk_revenue_per_cow_per_date is not null
order by location_daily_count_for_bovine_for_time_start_stop
SQL;     
       
        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $dimArr[] = ($row2['location_daily_count_for_bovine_for_time_start_stop']);
            $munArr[] = round($row2['calculate_milk_revenue_per_cow_per_date'], 2);
        }

        $headerStrArr = array();
        $headerStrArr['number of days in group'] = 'number'; //BACKWARDS
        $headerStrArr['100 day milk revenue'] = 'number';
        $x = new GoogleVisualizationScatter(null, $headerStrArr, $dimArr, $munArr);
        print($x->toString());
    }     
        
        
        
    
    }
    
    
//end class
