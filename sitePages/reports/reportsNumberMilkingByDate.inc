<?php 
/**
 Show number of cows milking on specific date for payroll purposes.
 */
//FIXME move this somewhere else. waste of a page. add chart too?


class ReportsNumberMilkingByDate extends BasePage {

	
	public function defaultDisplay() {
		print($this->displayNumberMilking());
	}


	function displayNumberMilking($mode = 'default') {
		
                
$sql = <<<SQL
SELECT date,number_of_cows FROM batch.daily_number_cows_milking ORDER by date DESC LIMIT 100
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
          $rowOut['date']=JQueryDataTable::formattedDate($row['date']);        
          $rowOut['Number of Cows']=  $row['number_of_cows'];
          $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]='<h3>Number of Cows Milking on a Specific Date</h3>';
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr)); 
         return implode($out); 
            
            
        }
	

}//end class