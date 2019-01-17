<?php 


class CroppingNutrientRequirements {

    
     public $field_id;
      public $cropyear;
    
    public function __construct($field_id,$cropyear) {
       $this->field_id=$field_id;
       $this->cropyear=$cropyear;
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    
    
    function manureCredits() {
        
        $creditArr['N']=0;
        $creditArr['P']=0;
        $creditArr['K']=0;
       
        //manure test, use book values. get from db in future.
        //get new values, these are guesses.
        $n_per_liter=.00079;
        $p_per_liter=.00005;
        $k_per_liter=.0017;
        
        //find all relevant manure events.
        $date=($this->cropyear - 1) .'-08-15';
        $sql = "SELECT * FROM cropping.manure_event WHERE field_id={$this->field_id} AND event_time>='$date'";
        $res = $GLOBALS['pdo']->query($sql);
        { 
       
    } 
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
          print($row['manure_amount']."<br/>");	  
          
         //TODO: was manure incorporated??
         $creditArr['N']=$creditArr['N']+$row['manure_amount']*$n_per_liter;
         $creditArr['P']=$creditArr['P']+$row['manure_amount']*$p_per_liter;
         $creditArr['K']=$creditArr['K']+$row['manure_amount']*$k_per_liter;
         
        }
        
        return $creditArr;
    }
    
    
    // you can put 40 kg/ha less N if you sidedress it. For us, it is cheaper to just
    //put on the extra N and assume it will be lost and not side-dress unless PSNT says to.
    function fertilizerCredits() {
        
        $creditArr['N']=0;
        $creditArr['P']=0;
        $creditArr['K']=0;
        
        $date=($this->cropyear - 1) .'-08-15';
        //find all fertilizer events.
        $sql = "SELECT * FROM cropping.fertilizer_event 
        LEFT JOIN cropping.fertilizer ON fertilizer_id=fertilizer.id
        WHERE field_id={$this->field_id} AND event_time>='$date'";
        $res = $GLOBALS['pdo']->query($sql);
       
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
             $creditArr['N']=$creditArr['N']+$row['amount']*$row['n'];
             $creditArr['P']=$creditArr['P']+$row['amount']*$row['p'];
             $creditArr['K']=$creditArr['K']+$row['amount']*$row['k'];      
         
          
        }
         
         return $creditArr;
    }
    
    function prevCropCredits() {
       
        $creditArr['N']=0;
        $creditArr['P']=0;
        $creditArr['K']=0;
         
       return $creditArr;  
    }
    
    function soilCredits() {
       
        //always get the latest soil test.
         $sql = "SELECT * FROM cropping.soil_sample_event
        WHERE field_id={$this->field_id} AND event_time=(select max(event_time) FROM cropping.soil_sample_event WHERE field_id={$this->field_id})";
        $res = $GLOBALS['pdo']->query($sql);
        
        //the first thing to do is make a virtual soil sample.
        
        if ($res->rowCount()==0) {
            //never had a soil test. 
            //there are very few fields without a soil test, usually poor fields, so assume they are low in everything.          
            $soilArr['N']=0;
            $soilArr['P']=30;
            $soilArr['K']=50;
            
        }else {
            //we have a soil test.
             $row = $res->fetch(PDO::FETCH_ASSOC);
            //if it is less than a year old, use it as is.
            if (strtotime($row['event_time'] ) < strtotime('12 months ago')) {
               
                 $soilArr['N']=0;
            	 $soilArr['P']=$row['phosphorus']; 
                 $soilArr['K']=$row['potassium'];		       
        } elseif  (strtotime($row['event_time'] ) < strtotime('24 months ago')) {
            
            $soilArr['N']=0;
            $soilArr['P']=$row['phosphorus']*.9; 
            $soilArr['K']=$row['potassium']*.9;
        
        } elseif  (strtotime($row['event_time'] ) < strtotime('36 months ago')) {
            
            $soilArr['N']=0;
            $soilArr['P']=$row['phosphorus']*.8; 
            $soilArr['K']=$row['potassium']*.8;
        
        } elseif  (strtotime($row['event_time'] ) < strtotime('48 months ago')) {
            
            $soilArr['N']=0;
            $soilArr['P']=$row['phosphorus']*.7; 
            $soilArr['K']=$row['potassium']*.7;
        
        } else
            $soilArr['N']=0;
            $soilArr['P']=$row['phosphorus']*.5; 
            $soilArr['K']=$row['potassium']*.5;
        }
        
        //so we now have some type of soil information in ppm.
        
        //now we need to determine what is available for the crop and put it into kg/ha.
        
   
       return $soilArr;  
    }
    
    
    function cropRequirements($specific_type) {
        
        $reqArr['N']=0;
        $reqArr['P']=0;
        $reqArr['K']=0;
        
        
        switch ($specific_type) {
          
    case 'Corn':
        //when corn always assume corn silage, because we don't know how it will be harvested.
        //corn silage requires N-P-K 150-30-30 per hectare.          
        $reqArr['N']=150;
        $reqArr['P']=30;
        $reqArr['K']=30;
        break;
 
    default:
        $reqArr['N']=0;
        $reqArr['P']=0;
        $reqArr['K']=0;
        break;
     }      
        
     return $reqArr;
    }
    
    
    /*
     * calculates what select crops need for fertilizer
     * 
     * 
     */
    function main($specific_type) {
        
      
           
        //now show all credits
        
        
        //calculate deficit
        
        //suggest two scenarios:
        // 1) med coat manure + fertilizer
        // 2) just fertilizer
        
        
        
        $reqArr=$this->cropRequirements($specific_type);
        $creditArr0=$this->prevCropCredits();
        $creditArr1=$this->soilCredits();
        $creditArr2=$this->manureCredits();
        $creditArr3=$this->fertilizerCredits();
        
        print("<p>Requirements for <b>$specific_type</b>:</p>");
        print_r($reqArr);
        
        print("<p>Prev Crop Credit:</p>");
        print_r($creditArr0);

         print("<p>Soil Credit:</p>");
        print_r($creditArr1);
        
        
        print("<p>Manure Credit:</p>");
        print_r($creditArr2);
        
        
         print("<p>Fertilizer Credit:</p>");
         print_r($creditArr3);
        
        
        print("<p>Nutrient Balance:</p>");
        
        
        print("<p>Suggested Fertilizer:</p>");
        
        
        print("<p>Suggested Fertilizer and Manure:</p>");
        
    }
    
}//end class

?>

