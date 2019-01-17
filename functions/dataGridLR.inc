<?php
/**
 
 * //extends datgrid class with specific logic to colour the line.
  class DataGridLR_Ext extends DataGridLR {
                     function colourLineCriteria($row) {
                     if ($row['completed']=='t') {  return true;}
                     else  {  return false;}
                     }
                }
   then call with:
   $dg1 = new DataGridLR_Ext($sql, 1000);
 * print($dg1->render('datagrid',true,'00FF00'));
  etc.....
 */


class DataGridLR {
      
      public $datagrid;
    

       public function __construct($sql,$numberPerPage=20) {
        require_once ('Structures/DataGrid.php');
        
        require_once ('Structures/DataGrid/DataSource/PDO.php');
        require_once ('HTML/Table.php');
        
        
        //run method to setup initial datagrid paramaters.
        $this->datagrid=$this->start($sql,$numberPerPage);
    
    }
    
    function start($sql,$numberPerPage) {
     
       
        // execute a row count on the sql query.
        $res = $GLOBALS ['pdo']->query($sql);
        
        $options = array('dbc' => $GLOBALS ['pdo'], 'count_query' => "SELECT {$res->rowCount()}");
       
        // Bind a basic SQL statement as datasource
        $datagrid = new Structures_DataGrid($numberPerPage);
        $datadriver = new Structures_DataGrid_DataSource_PDO();
        $datadriver->bind($sql, $options);
        $datagrid->bindDataSource($datadriver);
        
        return $datagrid;
       }
    
    
    function render($cssClass='datagrid',$altrows=true,$colourLine='#FF6666') {
    
        //COLUMN DEFINITIONS GO (ABOVE) HERE
      
        // Ask the DataGrid to fill the HTML_Table with data, using rendering options
        $tableAttribs = array('class' => $cssClass);        // Define the Look and Feel
        $table = new HTML_Table($tableAttribs);         // Create a HTML_Table
        $this->datagrid->fill($table);
       
        // MUST BE AFTER "fill"   
         if ($altrows ==true) {
        $headerAttribs = array('bgcolor' => '#CCCCCC');
        $evenRowAttribs = array('bgcolor' => '#FFFFFF');
        $oddRowAttribs = array('bgcolor' => '#EEEEEE');
        
        }
        //default behavior
        else {
         $headerAttribs = array('bgcolor' => '#CCCCCC');
        $evenRowAttribs = array('bgcolor' => '#FFFFFF');
        $oddRowAttribs = array('bgcolor' => '#FFFFFF');
        }
        
        $tableHeader = $table->getHeader();
        $tableBody = $table->getBody();
        $tableHeader->setRowAttributes(0, $headerAttribs); // Set attributes for the header row
        $tableBody->altRowAttributes(0, $evenRowAttribs, $oddRowAttribs, true);  // Set alternating row attributes
        //colour line a custom way code
         $this->colourLine($tableBody,$colourLine);
        
        
        
        // Output table as str
        return $table->toHtml(); 
       
     }

     /**
      * Method to change the colour of a row based on a specific criteria. 
      */
     private function colourLine($tableBody,$colourLine='#FF6666') {
       	
		//get an array of data in the datagrid
		$ds = $this->datagrid->getDataSource();
		$data = $ds->fetch();

		//go through each row and color it red if the cow hasn't been bred yet.
		$counter=0;
		foreach ($data as $row) {

			if ($this->colourLineCriteria($row)==true) {
				$tableBody->setRowAttributes($counter, array('bgcolor' => $colourLine));
			}

			$counter++;
		}


         
     }
     
     /** overirde this with logic to colour the lines */
     function colourLineCriteria($row) {
         
     return false; //default is to not colour anything.    
     }
     
     function  pager() {
     /////////////// PAGING ///////////////////////
        //might need sessionVar set for multiple pagers on the same page/
            $p_options=array( 'excludeVars' => array('callback') );  //remove callback variable or we will have problems on some redraws, see basepage for info.
            $this->datagrid->render('Pager', $p_options);    
    }

} //end class