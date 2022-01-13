<?php 

/*
 * when a search is run on the header bar of the web page. This shows the results 
 * and holds the logic.
 */


class Search Extends BasePage {

    const pageid = 122; //kind of hackish, change if the pages id ever changes.

   
    
    /* search content */
    /**
     * returns a JSON array of search content with text search key and a url
     * USED directly now as array, no ajax.
     * 
     */
    public function content() {
        
       $sql = "SELECT id,textid,type  FROM system.search_view";
       $res = $GLOBALS['pdo']->query($sql);
       $allArr = $res->fetchAll(PDO::FETCH_ASSOC);  
       
        $out[]=array('searchText'=>'SEARCH','partialUrl'=>'');
        
       foreach ($allArr as $key => $value) {
           
           switch ($value['type']) {
                case 'bovineid':
                    $pageid = 6;
                    $type = 'bovine_id';
                    break;
                case 'fieldid':
                    $pageid = 107;
                    $type = 'field_id';
                    break;
                case 'pageid':
                    $pageid = $value['id'];
                    $type = 'null';
                    $id = 'null';
                    break;
                case 'machineid':
                    $pageid = 95;
                    $type = 'machine_id';
                    break;
                default:       //this should maybe be an error page. For now it is the home page.
                    $pageid = 1;
                    break;
            }
            
           if (!empty($type)) {   
           $out[]=array('searchText'=>$value['textid'],'partialUrl'=>'?pageid='.$pageid.'&'.$type.'='.$value['id'],'url'=>$GLOBALS['config']['HTTP']['URL'].'?pageid='.$pageid.'&'.$type.'='.$value['id']);
           }
       }
       
        return ($out);
        //return json_encode($out);
    }
    
    
   
    /** does not currently run from page */
    public function defaultDisplay() {
        
    }

    


}//end class