<?php 
// Lists straws of semen in our inventory


class CommodityPrices extends BasePage {
	
	
	public function defaultDisplay() {
		$this->plotCommodityPriceCharts ();
	}
	
	public function summaryDisplay() {
	
	}
	
	// full registration number of animal needed.
	function plotCommodityPriceCharts() {
		// set how far back to plot data.
		$plotStartDate = date ( 'Y-m-d', strtotime ( "-5 years" ) );
		
		// select the latest laction...
		$query = "SELECT event_time,ddgs_low,ddgs_high,barley,oats,wheat_low,soybeans_sorel,wheat_high,soybeans_sorel,soybeanmeal_hamilton_low,soybeanmeal_hamilton_high,corn_quebec,corn_west_ontario FROM batch.commodity_report WHERE event_time >= '$plotStartDate' ORDER BY event_time ASC";
		$res = $GLOBALS['pdo']->query ( $query );
		
		while ( $row = $res->fetch(PDO::FETCH_ASSOC) ) {
			
			// only plot data we actually have
			
			if ($row ['corn_quebec'] != 0) {
				
                                $cornArr[  strtotime ( $row ['event_time'] )]=$row ['corn_quebec'];
			}
                        
                        if ($row ['corn_west_ontario'] != 0) {
				
                                $cornArrOntario[  strtotime ( $row ['event_time'] )]=$row ['corn_west_ontario'];
			}
			
			if ($row ['soybeanmeal_hamilton_high'] != 0) {
				
                             $soybeanMealArr[  strtotime ( $row ['event_time'] )]=$row ['soybeanmeal_hamilton_high'];
			}
			
			if ($row ['barley'] != 0) {
				
                            $barleyArr[  strtotime ( $row ['event_time'] )]=$row ['barley'];
			}
			
			if ($row ['ddgs_high'] != 0) {
				
                             $ddgsArr[  strtotime ( $row ['event_time'] )]=$row ['ddgs_high'];
			}
                        
                        if ($row ['soybeans_sorel'] != 0) {
				
                            $soybeansSorelArr[  strtotime ( $row ['event_time'] )]=$row ['soybeans_sorel'];
			}
		}
		  
                print ("<h3>Corn (SW Quebec)</h3>") ;
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['Corn ($)'] = 'number';
                $x=new GoogleVisLine(null,$headerStrArr,$cornArr);
                print($x->toString());
                
                print ("<h3>Corn (Western Ontario Feed)</h3>") ;
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['Corn ($)'] = 'number';
                $x=new GoogleVisLine(null,$headerStrArr,$cornArrOntario);
                print($x->toString());
                
                //
                print ("<h3>Soybean Meal (Hamilton)</h3>") ;
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['Soybean Meal ($)'] = 'number';
                $x=new GoogleVisLine(null,$headerStrArr,$soybeanMealArr);
                print($x->toString());
                //
                print ("<h3>Barley (Bayport,ON)</h3>") ;
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['Barley ($)'] = 'number';
                $x=new GoogleVisLine(null,$headerStrArr,$barleyArr);
                print($x->toString());
                //
                print ("<h3>DDGS</h3>") ;
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['DDGS ($)'] = 'number';
                $x=new GoogleVisLine(null,$headerStrArr,$ddgsArr);
                print($x->toString());   
                //
                print ("<h3>Soybeans (Sorel)</h3>") ;
                $headerStrArr=array();
                $headerStrArr['xAxis'] = 'date'; //BACKWARDS
                $headerStrArr['Soybeans Sorel ($)'] = 'number';
                $x=new GoogleVisLine(null,$headerStrArr,$soybeansSorelArr);
                print($x->toString());
               

             
               
                
	} // end function
	
        
        	//shows error.
	function error() {
	 $error=array();
         
	////////////////////////
	//when commodity report has not beenupdated for 72 hours print error.
	$sql="
            With temp as (
SELECT  max(event_time) as one FROM batch.commodity_report 
)
SELECT 1 as two FROM temp WHERE temp.one >= (now() - interval '72 hours') 

	     ";
	     $res = $GLOBALS['pdo']->query($sql);
             if ($res->rowCount()==0) 
	     {
	      $error[]= new notifyObj(get_class($this),date('Y-m-d',strtotime("now")),2,'Commodity report is out of date.');
	     }
	     
	     
             
	 return $error;
	  
	} 
        
        
} //end class	