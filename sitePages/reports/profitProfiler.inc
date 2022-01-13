<?php 


class ProfitProfiler extends BasePage {
	

	

	public function defaultDisplay() {
            print("<h3>Average Milk Price and Expenses for a Year</h3>");
            print($this->yearlyMilkPrice());
            print($this->involuntaryCullRate()); 
            print($this->cattleSales()); 
            print($this->whoBeefed());
            
        }
        
        function yearlyMilkPrice() {
                $sql = <<<SQL
SELECT distinct date_trunc('year',date)::date as date
,round((avg(butterfat)  OVER W),4) as avg_butterfat
,round((avg(protein) OVER w),4)  as avg_protein
,round((avg(lactose) OVER w),4)  as avg_lactose
,round((avg(transport) OVER w),4)  as avg_transport
,round((avg(promotion) OVER w),4)  as avg_promotion
,round((avg(admin) OVER w),4)  as avg_admin
,round((avg(lab) OVER w),4)  as avg_lab
,round((avg(research) OVER w),4) as avg_research
 from batch.milk_statement
WINDOW w AS (PARTITION BY  date_trunc('year',date)) order by date desc;
SQL;
                

            return ( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql)); 
        }
        
        
        function cattleSales() {
           
             
             $sql = <<<SQL
SELECT *,cull_event.id as id,bovinemanagement.lactation_number(bovine_id),age(death_date,birth_date) as age_at_cull 
FROM bovineManagement.cull_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id 
WHERE (date_trunc('year',event_time))   =  (date_trunc('year', CURRENT_DATE) - INTERVAL '1 year') 
AND reason='Sold'
ORDER BY event_time DESC;
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
          $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
          $rowOut['Date']=JQueryDataTable::formattedDate($row['event_time']);        
          $rowOut['Reason']=  $row['reason'];
          $rowOut['Comment']=  $row['comment'];
          $rowOut['Lactation #']=  $row['lactation_number'];
          $rowOut['Age at Cull']=  $row['age_at_cull'];
          $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;

         $out[]='<h3>#56 Cattle Sales</h3>';
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr)); 
         return implode($out); 
        
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
             return("<h2>Died: {$row['num_died']} </h2>");
        }
        
        
        
        
         function whoBeefed() {
       
             $sql = <<<SQL
SELECT *,cull_event.id as id,bovinemanagement.lactation_number(bovine_id),age(death_date,birth_date) as age_at_cull 
FROM bovineManagement.cull_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id 
WHERE (date_trunc('year',event_time))   =  (date_trunc('year', CURRENT_DATE) - INTERVAL '1 year') 
AND reason='Beefed'
ORDER BY event_time DESC;
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
          $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
          $rowOut['Date']=JQueryDataTable::formattedDate($row['event_time']);        
          $rowOut['Reason']=  $row['reason'];
          $rowOut['Comment']=  $row['comment'];
          $rowOut['laction #']=  $row['lactation_number'];
          $rowOut['Age at Cull']=  $row['age_at_cull'];
          $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]='<h3>#?? Who Beefed</h3>';
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr)); 
         return implode($out); 
   
        }
        
       
        
        
}//end class