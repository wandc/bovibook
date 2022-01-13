<?php

class FieldQueryHeader  {

    public $field_id;

    public function __construct($field_id) {
        $this->field_id=$field_id;
    }
    
    private function enclosingFaIcon($text,$icon) {
        
        return "<i class='fas $icon'>".$text.'</i>';
        
    }
    
   
    
    
    
    public function main() {
        $map=FieldQuery::displayCurrentFieldMap($this->field_id);
        $fieldName=$this->fieldName();
        $area= $this->enclosingFaIcon((new CroppingHelper)->fieldArea($this->field_id).' ha','fa-ruler-combined');
        $directions=$this->enclosingFaIcon("<a target='_blank'  href='".(new CroppingHelper)->directionsToField($this->field_id)."'>Directions<a/>",'fa-map-signs');
        $closestFields=$this->findClosestFields($this->field_id);
        $fieldParam=$this->displayFieldParamaters($this->field_id);
        $fieldMajorArea=$this->displayFieldMajorNameArea($this->field_id);
                
        $column1a = <<<HTML
            <span><h1 style='font-size:5.0rem;'><a href='{$GLOBALS['MiscObj']->hrefToFieldPage($this->field_id)}'>{$fieldName['alpha_numeric_id']}<a/></h1></span>
            <h6>{$fieldName['common_name']}</h6>
            
HTML;   
            
           
       $column1b = <<<HTML
          
            <h5>$area</h5>
            <h5>$directions</h5>
            <h5>{$this->field_id}</h5>   
          
HTML;         
       

       $column2a = <<<HTML
               
         $closestFields           
HTML;
           
        $column2b = <<<HTML
     
            $fieldParam
                
                $fieldMajorArea
HTML;    
      /*   
        $column3a = <<<HTML
            <h5>{$this->price()}</h5>
           
HTML;    
         $column3b = <<<HTML
     
            <h5> $genetics</h5>                
            <h5>{$this->classification()}</h5>
HTML;        
            
            
       
        $column4a = <<<HTML
            <h5>$damName</h5>
            <h5>$sireName</h5>             
HTML;    
         */ 
$finalOut = <<<HTML
        <div class="card card-widget widget-user shadow">                      
             <div class="d-flex justify-content-start">     
                <div class='headerBucket'>  
                    $map
                 </div>  
                <div class='headerBucket'>
                   $column1a
                </div>
                 <div class='headerBucket'>
                   $column1b
                </div>     
                  <div class='headerBucket'>
                   $column2a
                </div>        
                 <div class='headerBucket'>
                   $column2b
                </div>    
            </div>                         
                        
        </div>      
HTML;
        
        
      
return $finalOut;
  
    }
    
    
 //shows name
private function fieldName() {

        $sql = "
           SELECT * from cropping.field WHERE id=$this->field_id LIMIT 1
          ";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $ret['alpha_numeric_id'] = $row['alpha_numeric_id'];
        $ret['common_name'] = $row['common_name'];

        return $ret;
        }
        
        
        private function dd($field_id) {

        
                
        
    
    /*    
                   print($this->displayFieldParamaters($this->field_id));
            print($this->standAge($this->field_id));
            print ('</div>');
             
            print ('</div>');
          
            print();
      */          
            
        $str="";
     $str=$str.("<h3 style='float:right;font-size: inherit;'>Field ID: $field_id</h3>");
     $str=$str.("<h1 id='alphanumeric'>{$row['alpha_numeric_id']}</h1>");
     
     $str=$str.("<h1 id='fieldname'>{$row['common_name']}</h1>");
    
   
     
       $str=$str. ("<h3> </h3>");
     
       
       
       
       
     
     
 return $str;
 }
 
  function displayFieldParamaters($field_id) {
        $str = "";
        $sql = "
 	SELECT * from cropping.field_parameter WHERE field_id=$field_id ORDER BY event_time DESC LIMIT 1
 	";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() > 0) {
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $str = $str . "<li>Alfalfa suitable: {$row['alfalfa_suitable']}</li>";
            $str = $str . "<li>Corn suitable: {$row['corn_suitable']}</li>";
            $drainage_ScoreTxt=(new Field)->drainageScore($row['drainage_score']);
            $str = $str . "<li>Drainage: $drainage_ScoreTxt</li>";
        }

        return $str;
    }
    
    
    /*
     * shows area of major_name for active fields in that major_name at current borders
     */
    public function displayFieldMajorNameArea($field_id) {
        
        $sql=<<<SQL
            
                SELECT major_name,sum(cropping.field_border_area_at_timestamp(field.id,CURRENT_TIMESTAMP::TIMESTAMP WITHOUT TIME ZONE)) as full_area FROM cropping.field WHERE major_name=(SELECT major_name FROM cropping.field WHERE id=$field_id) and active = true group by major_name limit 1
                
        SQL;        
                
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);     
        
            return $row['major_name']. ' fields total area '. $row['full_area'] . ' ha';
            
    }
    
    
    
    
  /**
     * Find what year of an alfalfa stand or how many years corn on corn, etc.
     */
    function standAge($field_id) {
        $sql = "
   SELECT * FROM cropping.rotationcurr WHERE id=$field_id LIMIT 1
   ";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $age = date('Y', strtotime("now")) - date('Y', strtotime($row['event_time']));
        return ("<li>Rotation: {$row['specific_type']} " . $age . " years.</li>");
    }

     /**
     * lists the four closest fields to the given one.
     * @param type $field_id 
     */
    function findClosestFields($field_id) {
        $str = '';
        $sql = "SELECT 
id,alpha_numeric_id	,common_name, st_distance(border_geom,(SELECT border_geom FROM cropping.fieldcurr WHERE Id =$field_id)) as min_distance 
FROM cropping.fieldcurr 
WHERE Id !=$field_id ORDER BY min_distance LIMIT 4";

        //$str = $str . ("<h6>Nearby Fields:</h6>");
        $res = $GLOBALS['pdo']->query($sql);
        $str = $str . '<ul>';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $str = $str . ("<li><small><a href='/index.php?pageid=107&field_id={$row['id']}'>{$row['alpha_numeric_id']} - {$row['common_name']} </a></small></li>");
        }
        $str = $str . '</ul>';
        return $str;
    }
    
}//end class