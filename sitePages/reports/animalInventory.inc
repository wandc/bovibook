<?php 






/** records an actual cull from the herd and why */
class AnimalInventory extends BasePage {

    public function defaultDisplay() {
        print($this->displayInventoryByCohorts());
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

        

         $out[]=("<h3>Current Inventory</h3>");
         $out[]=( (new JQueryDataTable)->startBasicSql($sql)); 
         return implode($out); 

    }

}//end class    