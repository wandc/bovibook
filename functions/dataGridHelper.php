<?php 
/** helper functions for datagrid formatting */
/** and little river custom datagrid */
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');
include_once($_SERVER['DOCUMENT_ROOT'].'/functions/debug.php');

/**
* this class is to encourage code reuse for most datagrid types.
*
* call like:
*
           include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
	   $sql="SELECT * FROM  bovinemanagement.medicine_administered LIMIT 100 DESC";
           $dg=new DataGridLR($sql,10);
           $dg->datagrid->addColumn(new Structures_DataGrid_Column('Case #', 'full_name','', array('' => ''), null, null)); 
           print($dg->render());
           $dg->pager();
*
*/
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



/**
* Used for Arrays:

        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
        $dg = new DataGridLRArray($mineral, 1000);
        //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Name', 'trade_name', '', array('' => ''), null, null));
        print($dg->render());
        $dg->pager();
*/
class DataGridLRArray {
      
      public $datagrid;
    

       public function __construct($array,$numberPerPage=20) {
        require_once('Structures/DataGrid.php');
        require_once('Structures/DataGrid/DataSource/Array.php');
        require_once('HTML/Table.php');
        
        
        //run method to setup initial datagrid paramaters.
        $this->datagrid=$this->start($array,$numberPerPage);
    
    }
    
    function start($array,$numberPerPage) {
     
        //no row count? Maybe not needed for this driver.
        $options = array();
       
        // Bind a basic SQL statement as datasource
        $datagrid = new Structures_DataGrid($numberPerPage);
        $datadriver = new Structures_DataGrid_DataSource_Array();
        $datadriver->bind($array, $options);
        $datagrid->bindDataSource($datadriver);
        
        return $datagrid;
       }
    
    
    function render($cssClass='datagrid',$altrows=true) {
    
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
        $tableHeader = $table->getHeader();
        $tableBody = $table->getBody();
        $tableHeader->setRowAttributes(0, $headerAttribs); // Set attributes for the header row
        $tableBody->altRowAttributes(0, $evenRowAttribs, $oddRowAttribs, true);  // Set alternating row attributes
        }
        // Output table as str
        return $table->toHtml(); 
       
     }

     function  pager() {
     /////////////// PAGING ///////////////////////
        //might need sessionVar set for multiple pagers on the same page/
            $p_options=array( 'excludeVars' => array('callback') );  //remove callback variable or we will have problems on some redraws, see basepage for info.
            $this->datagrid->render('Pager', $p_options);    
    }

} //end class



class DataGridHelper {


/**
*shows a formatted cow local_number and short name
*requires:
* bovine_id,local_number,full_name
*
*/
	function printFormattedBovineNameAndNumberLarge($params)
	{
		extract($params);
		$number='<div id=\'largish\'>'. $GLOBALS['MiscObj']->hrefToBovinePageFullName($record['bovine_id'],$record['local_number'],$record['full_name']).'</div>';
		return $number;
	}
	
	function printFormattedBovineNameAndNumber($params)
	{
		extract($params);
		$number= $GLOBALS['MiscObj']->hrefToBovinePageFullName($record['bovine_id'],$record['local_number'],$record['full_name']);
		return $number;
	}

	function printCDNRegNumberLink($params)
	{
		extract($params);
		//create a link to CDN website, so when user clicks they go there for the sire
		$linkStr=Misc::createListOfAllCowsMilking($record['full_reg_number']);
		return  "<a href=\"$linkStr\">{$record['full_reg_number']}</a>";
	}
	
        function printShortRegNum($params)
	{
                //just returns the number portion of the reg number (used for herd cull)
		extract($params);
		$returnArray=$GLOBALS['MiscObj']->breakUpFullRegNumber($record['full_reg_number']);
		return  $returnArray['number'];
	}
        
	
        function printFormattedTime($params)
	{
		extract($params);
		if ($record['event_time'] ==null) { return ''; }
		else {
		return date("M d Y",strtotime($record['event_time']));
		}
	}
	
	//sometimes there are two time fields in one datagrid.
	function printFormattedTime2($params)
	{
		extract($params);
		if ($record['event_time2'] ==null) { return ''; }
		else {
		return date("M d Y",strtotime($record['event_time2']));
		}
	}
	
	function printFormattedDateTime($params)
	{
		extract($params);
		if ($record['event_time'] ==null) { return ''; }
		else {
		return date("M d Y H:i",strtotime($record['event_time']));
		}
	}
	
	function printFormattedDateTime2($params)
	{
		extract($params);
		if ($record['event_time2'] ==null) { return ''; }
		else {
		return date("M d Y H:i",strtotime($record['event_time2']));
		}
	}
	
	function printFormattedDateTimeDay($params)
	{
		extract($params);
		if ($record['event_time'] ==null) { return ''; }
		else {
		return date("D, M d Y H:i",strtotime($record['event_time']));
		}
	}
	
	function printFormattedDateTimeDay2($params)
	{
		extract($params);
		if ($record['event_time2'] ==null) { return ''; }
		else {
		return date("D, M d Y H:i",strtotime($record['event_time2']));
		}
	}
	
        function printFormattedDayDate($params)
	{
		extract($params);
		if ($record['event_time'] ==null) { return ''; }
		else {
		return date("D, M d Y",strtotime($record['event_time']));
		}
	}
        
        
/** 
 * Generic function to print this based on a fieldname, supplied at time of calling (2nd param). 
 */	
function printFormattedDateGeneric($params)
	{
		extract($params); //extracts everything in array.
		//fieldName is the 2nd param in addcolumn, needed to make this generic.
		if (empty($record["$fieldName"])) { return null;}
		else {	return date("M d Y",strtotime($record["$fieldName"])); }
	}
	
/** 
 * Generic function to print this based on a fieldname, supplied at time of calling (2nd param). 
 */	
function printFormattedTimeGeneric($params)
	{
		extract($params);
		if ($record["$fieldName"]=='') { return null;}
		else {	return date("M d Y H:i",strtotime($record["$fieldName"])); }
	}
        
        /** 
 * Generic function to print this based on a fieldname, supplied at time of calling (2nd param). 
 */	
function printFormattedTimeDayGeneric($params)
	{
		extract($params);
		if ($record["$fieldName"]=='') { return null;}
		else {	return date("D, M d Y H:i",strtotime($record["$fieldName"])); }
	}
        

/** 
 * Generic function to print this based on a fieldname, supplied at time of calling (2nd param). 
 */	
function printFormattedYearGeneric($params)
	{
		extract($params); //extracts everything in array.
		//fieldName is the 2nd param in addcolumn, needed to make this generic.
		if ($record["$fieldName"]=='') { return null;}
		else {	return date("Y",strtotime($record["$fieldName"])); }
	}
/** 
 * Generic function to print this based on a fieldname, supplied at time of calling (2nd param). 
 */	
function printFormattedYearMonthGeneric($params)
	{
		extract($params); //extracts everything in array.
		//fieldName is the 2nd param in addcolumn, needed to make this generic.
		if ($record["$fieldName"]=='') { return null;}
		else {	return date("M Y",strtotime($record["$fieldName"])); }
	}	
/**
generic function to convert db true and false to readable.
*/
function printTrueFalseGeneric($params)
	  {
	    extract($params);
	    //second param in add column.
	    if ($record["$fieldName"]=='t') { return 'True';}
            elseif ($record["$fieldName"]=='f') { return 'False';}
            else { return ''; }
	  }
/**
generic function to convert db true and false to readable.
*/
function printTrueFalseGenericEmphasis($params)
	  {
	    extract($params);
	    //second param in add column.
	    if ($record["$fieldName"]=='t') { return '<b id="boldRed">True</b>';}
            elseif ($record["$fieldName"]=='f') { return 'False';}
            else { return ''; }
	  }	  

/**
generic function to convert db true and false to readable.
*/
function printTrueFalseGenericTrueOnly($params)
	  {
	    extract($params);
	    //second param in add column.
	    if ($record["$fieldName"]=='t') { return '<b id="boldRed">True</b>';}
            elseif ($record["$fieldName"]=='f') { return '';}
            else { return ''; }
	  }
          
	  /**
	   * print link to field query page
	   */
	  function printFormattedFieldAlphaNumeric($params)
	  {
	  	extract($params);
	  	$number="<a href=\"". "/index.php?pageid=107&field_id={$record['field_id']}"."\"> <b>{$record['alpha_numeric_id']}</b> " ."</a>";
	  	return $number;
	  }	  
	  
          /************************************************
     * Delete Button Quickform
     * ******************************************** */
    public static function deleteQuickForm($tableid, $tablename,$customHeaderLocation=null) {

        $form = new HTML_QuickForm('genericDataGridDeleteButton_'.$tablename.'_'.$tableid, 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete?')")); //call javascript to confirm delete.
        //FIXME:HUGE SECURITY HOLE HERE BECAUSE TABLE NAME CAN BE MODIFIED BY USER.
        //DB hack works though, views need to support deletes before this can be fixed.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');

            //delete the event
            $res = $GLOBALS['pdo']->exec("DELETE FROM bovinemanagement.$tablename where id='$tableid'");

            if ($customHeaderLocation==null) {
	      header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
              }
              else {
               header("Location: ". $customHeaderLocation);   
              }
	      exit();
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }
          
          
          
}//end class

?>