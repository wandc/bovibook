<?php 

/** records an actual cull from the herd and why */


class BovinePicture extends TabPage{
  
  
  
  public function defaultDisplay() {
    //setup for tabs.
        $tabArray[1]['name'] = 'High Group';     
        $tabArray[2]['name'] = 'Low Group';
        $tabArray[3]['name'] = 'Heifer Group';
        $tabArray[4]['name'] = 'Sick Group';
        $tabArray[5]['name'] = 'Dry Group';
        $this->renderTabs($tabArray);
  }
  
   
    function tab1() {
        $this->displayListOfBovinePictures(40);
        }
    function tab2() {
        $this->displayListOfBovinePictures(41);
        }
    function tab3() {
        $this->displayListOfBovinePictures(42);
        }
    function tab4() {
       $this->displayListOfBovinePictures(39);
        }
    function tab5() {
       $this->displayListOfBovinePictures(37);
        }


/**
 * shows small pictures of all the cows in a group 
 **/
function displayListOfBovinePictures($group_id) {
  
  $query="SELECT DISTINCT ON (local_number) bovinecurr.id as bovine_id,bovinecurr.full_name, bovinecurr.location_name,bovinecurr.local_number,bovinecurr.location_id 
          FROM bovinemanagement.bovinecurr LEFT JOIN picture.bovine ON bovine.bovine_id=bovinecurr.id WHERE location_id ='$group_id' ORDER BY local_number";
		$res = $GLOBALS['pdo']->query($query);
		
   print("<div id='fiveColumnLayout'>");		
   while($row=$res->fetch(PDO::FETCH_ASSOC)) {
   		  
   
   print("<div id='bovinePic'>");
   print("<b>{$row['local_number']}</b><br/>");
   print("<a href=\"".$GLOBALS['MiscObj']->hrefToBovinePage($row['bovine_id'])."\"><img width=\"160px\" height=\"120px\" src='/functions/displayBovinePic.php?bovine_id={$row['bovine_id']}'/></a>");
   print('</div>');
   print('<br/>');
   }
   print("</div>");
		
  
}  
  
  
} //end class  