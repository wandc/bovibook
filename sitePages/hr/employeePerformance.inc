<?php 
include_once($_SERVER['DOCUMENT_ROOT'].'functions/debug.php');
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');
include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');

class EmployeePerformance extends BasePage{
  
 
function defaultDisplay() {
 $this->milkingParlorStats();
}

function milkingParlorStats() {
    
    $sql="with temp as (
SELECT Distinct(cow.date,cow.milking_number),min(milktime) as first_attach,max(milktime) as last_attach,userid,cow.date,cow.milking_number 
FROM alpro.cow 
LEFT JOIN batch.employee_shift ON (employee_shift.date,employee_shift.milking_number) = (cow.date,cow.milking_number)
WHERE cow.date >= (current_date - interval '6 month')  AND cow.date < current_date
GROUP BY cow.date,cow.milking_number,userid
)
SELECT userid as employee,date,milking_number,first_attach,last_attach
FROM temp
ORDER BY employee,milking_number,date DESC";
    
    
        $dg = new DataGridLR($sql, 200);
        /*
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Field Number', 'alpha_numeric_id', 'alpha_numeric_id', array('' => ''), null, array('DataGridHelper', 'printFormattedFieldAlphaNumeric()')));
          $dg->datagrid->addColumn(new Structures_DataGrid_Column('Name', 'common_name', '', array('' => ''), null, null));
         */
        
        print ("<h3>Miking Parlor Stats</h3>");
        print($dg->render());
        $dg->pager();
    }




}//end class
?>