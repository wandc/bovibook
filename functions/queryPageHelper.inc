<?php 
/* misc funcions that help out cropping query, bovine query, machien query etc.*/

class QueryPageHelper {

    
    //sorts array of events.
	//used for bovine query and field query
	public static function sortOutArray($outArray) {
		// ////////////////////////////////////////////////////////////////////
		// sort array by reverse date based on time key
            $ret='';
		if ($outArray != null) {
			krsort ( $outArray ); // reverse order
			foreach ( $outArray as $var ) {
				$ret=$ret. ($var) ;
			}
		}
		// ///////////////////////////////////////////////////////////////////
	return $ret;
	}
        
        /**
     * Find when laction starts to show a linebreak.
     * @param type $bovine_id
     * @param type $outArray
     * @return type 
     */
    public static  function calculateLactationLineBreaks($bovine_id, $outArray) {
        //find when lactations started and break up the list whenever that occurs
        $sql = "SELECT id,fresh_date FROM bovinemanagement.lactation WHERE bovine_id=$bovine_id ORDER BY fresh_date ASC";
        $res = $GLOBALS ['pdo']->query($sql);
        $counter = 1;
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $time = strtotime($row['fresh_date']);
            $str = ("<div class='hr_1'><div class='hr_2'><span class='hr_3'>&#8593; Lactation $counter &#8593;</span></div></div>");
             $outArray = makeTimeString($outArray, $time - 2, $str); // the minus 2 to display before calvinga nd lactation start
            $counter++;
        }
        return $outArray;
    }
    
    
    
}

?>
