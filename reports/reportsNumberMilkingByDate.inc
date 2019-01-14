<?php 
/**
 Show number of cows milking on specific date for payroll purposes.
 */



class ReportsNumberMilkingByDate extends BasePage {

	
	public function defaultDisplay() {
		$this->displayNumberMilking();
	}


	function displayNumberMilking($mode = 'default') {
		

		$numberToshow=100;



		// Setup your database connection
		$sql ="SELECT date,number_of_cows FROM batch.daily_number_cows_milking ORDER by date DESC LIMIT 100";
       $dg = new DataGridLR($sql, $numberToshow);

		$dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''),null,array('ReportsNumberMilkingByDate','printFormattedDate()')));
		$dg->datagrid->addColumn(new Structures_DataGrid_Column('Number Milking', 'number_of_cows', '', array('' => ''), null, null));

		

    print("<h2>Number of Cows Milking on a Specific Date</h2>");
    // Output table and paging links
print($dg->render());
        if ($mode == 'default') {
            $dg->pager();
        }
     
	}

	function printFormattedDate($params)
	{
		extract($params);
		return date("l M d Y",strtotime($record['date']));
	}

}//end class