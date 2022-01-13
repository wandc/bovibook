<?php 
class UserQuery extends BasePage{
  
 
  


public function defaultDisplay() {
    
    
    $curretnUser=ucwords($GLOBALS['auth']->getUsername());
    
            
    print("<h2><i class='fas fa-user'></i> $curretnUser</h2>");      
            
      
    print("Not Implemented Yet");
    
    
    $sql=<<<SQL
            SELECT * FROM hr.skill_event
            LEFT JOIN hr.skill on skill_id=skill.id WHERE userid='david'
    SQL;        
    $res = $GLOBALS['pdo']->query($sql);
		
		
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
                    print_r($row);
                }
    
}

}//end class