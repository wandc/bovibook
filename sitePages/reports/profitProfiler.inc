<?php 


class ProfitProfiler extends BasePage {
	

	

	public function defaultDisplay() {
            
           $this->involuntaryCullRate(); 
            $this->cattleSales(); 
            
            
        }
        
        
        function cattleSales() {
             print("<h3>#56 Cattle Sales</h3>");
             
             
                        $sql = "	     
SELECT *,cull_event.id as id,bovinemanagement.lactation_number(bovine_id),age(death_date,birth_date) as age_at_cull 
FROM bovineManagement.cull_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id 
WHERE (date_trunc('year',event_time))   =  (date_trunc('year', CURRENT_DATE) - INTERVAL '1 year') 
AND reason='Sold'
ORDER BY event_time DESC;
";



        $dg = new DataGridLR($sql, 1000);

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
         $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''),null,array('DataGridHelper','printFormattedTime()'))); 
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('reason', 'reason', 'reason', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('comment', 'comment', 'comment', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Lactation #', 'lactation_number', 'lactation_number', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Age at Cull', 'age_at_cull', 'age_at_cull', array('' => ''), null, null));


        print($dg->render('datagrid datagridNarrow'));

        $dg->pager();
        
        }
         function involuntaryCullRate() {
             print("<h3>#37 Involuntary Cull Rate</h3>");
             
             //find who died in fisical year.
             $sql="	     
SELECT count(bovine_id) as num_died
FROM bovineManagement.cull_event
WHERE (date_trunc('year',event_time))   =  (date_trunc('year', CURRENT_DATE) - INTERVAL '1 year') 
AND reason='Died'
";         
             $res = $GLOBALS['pdo']->query($sql);
     	     $row = $res->fetch(PDO::FETCH_ASSOC);
             $row['num_died'];
             print("<h2>Died: {$row['num_died']} </h2>");
             
             ///////////
            
             //find everyone who got beefed and reason and number of lactations.

           
                        $sql = "	     
SELECT *,cull_event.id as id,bovinemanagement.lactation_number(bovine_id),age(death_date,birth_date) as age_at_cull 
FROM bovineManagement.cull_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id 
WHERE (date_trunc('year',event_time))   =  (date_trunc('year', CURRENT_DATE) - INTERVAL '1 year') 
AND reason='Beefed'
ORDER BY event_time DESC;
";



        $dg = new DataGridLR($sql, 1000);

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
  $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''),null,array('DataGridHelper','printFormattedTime()'))); 
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('reason', 'reason', 'reason', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('comment', 'comment', 'comment', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Lactation #', 'lactation_number', 'lactation_number', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Age at Cull', 'age_at_cull', 'age_at_cull', array('' => ''), null, null));


        print($dg->render('datagrid datagridNarrow'));

        $dg->pager();
            
            
        }
        
       
        
        
}//end class