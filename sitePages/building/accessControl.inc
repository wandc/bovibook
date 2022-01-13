<?php
class AccessControl extends BasePage {

     public function defaultDisplay() {

      print("Das");
    }
    
    
    //arduino at door requestes it's list of valid cards.
    //This is set to query at regular interervals, say every 10 mins. 
    //if arduino can't connect to INT server it just lets anything in,
    //unless security flag is set and only master card will allow it to be opened.
    //{$GLOBALS['config']['HTTP']['JSON_API']}/166/allowedCards
     public function allowedCards($request) {
         
         
         
         /*
        if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {
                $lightNum = trim($request['item']);
    
            }
        }
          * 
          */
                     $sql = <<<SQL
SELECT card_token from bas.access_control_all where device_mac='ff:aa:ff:aa:ff:aa'
SQL;
    $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);       
        $out=json_encode($results, JSON_HEX_TAG); 
        
        return $out;
        
         
            }
            
}//end class
        