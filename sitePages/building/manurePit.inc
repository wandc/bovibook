<?php
class ManurePit extends BasePage {

    public $error = array();
    
    
     public function defaultDisplay() {
       
       $csvArr=$this->getControlByWebLog();
       
      // var_dump($csvArr);
       
       $last3HoursArr=$this->processCSVLog($csvArr);
       $errorArr=$this->logic($last3HoursArr);
       
        print_r($this->error);
        print_r($errorArr);
       
       
     }
     
     //get something meaninful out of the data from the last 3 hours on the state of the manue pit.
     public function logic($last3HoursArr) {
          $error=array();
          
         //Error 1:
         $latestValue=end($last3HoursArr);
         if (!is_array($latestValue)) {
              $error[]='Error 99 Manure Pit Controller has not reported in a long time. Check that it has power and internet.';
         } else {
         $latestTime=str_replace("DST", "", $latestValue['Date Time']);   
         if ((strtotime($latestTime)) <= (strtotime("-3 hour"))) {
             $error[]='Error 99 Manure Pit Controller has not reported in the last 3 hours. Check that it has power and internet. ';
         }
         } 
         //Error 2:
         $sum=0;
         $columnArr = array_column($last3HoursArr, 'input3 ShallowFloatState');
         foreach ($columnArr as $keyE2 => $valueE2) {
             $sum=$sum+$valueE2;
         }
             if ($sum >=1) {
                 $error[]='Error 99 Manure Pit Shallow Float has tripped in the last 3 hours, This should not happen. Pit is not emptying fast enough.';
             }     
     
         
        //Error 3:
         $sum=0;
         $allGood=false;
         $columnArr = array_column($last3HoursArr, 'input4 DeepFloatState');
         foreach ($columnArr as $keyE3 => $valueE3) {
             if ($valueE3==0){
              //good then we were below deep float once.    
                 $allGood=true;
             }
         }
             if ($allGood ==false) {
                 $error[]='Error 99 Manure Pit Deep Float has not tripped in the last 3 hours, This should not happen. Pit is not pumping down.';
             }     
         
        //Error 4:
         $sumInput=0;
          $columnArr = array_column($last3HoursArr, 'input1 PumpState');
         foreach ($columnArr as $keyE4 => $valueE4) {
            $sumInput=$sumInput+$valueE4;  
         } 
         $sumRelay=0;
          $columnArr2 = array_column($last3HoursArr, 'Relay 1 PumpOnOff');
         foreach ($columnArr2 as $keyE42 => $valueE42) {
            $sumRelay=$sumRelay+$valueE42;  
         } 
         
          if (($sumInput > 0) AND  ($sumRelay > 0)){
               //all normal relay and input both running on auto for pump
             }
          elseif (($sumInput =0) AND  ($sumRelay > 0)){
            
              $error[]='Error 99 Manure Pit Pump has not run in last 3 hours.';
             
          }   
        
             
        //Error 5:
         $sumInput=0;
         $columnArr=array();
           $columnArr = array_column($last3HoursArr, 'input2 AgitatorState');
         foreach ($columnArr as $keyE5 => $valueE5) {
            $sumInput=$sumInput+$valueE5;  
         } 
         $sumRelay=0;
           $columnArr2=array();
           $columnArr2 = array_column($last3HoursArr, 'Relay 2 AgitatorOnOff');
         foreach ($columnArr2 as $keyE52 => $valueE52) {
            $sumRelay=$sumRelay+$valueE52;  
         } 
         
          if (($sumInput > 0) AND  ($sumRelay > 0)){
               //all normal relay and input both running on auto for agitator
             }
          elseif (($sumInput =0) AND  ($sumRelay > 0)){
            
              $error[]='Error 99 Manure Pit Agitator has not run in last 3 hours.';
             
          }   
        
          return $error;
     }
     
      //go through array looking for value greater than 3 hours ago and slice it out. 
     public function processCSVLog($csvArr) {
         
         
         
         //the first array element is the key names.
         //pop off top of array
         $columnNames=array_shift($csvArr);
         
         foreach ($csvArr as $key => $value) {
             
             //control by web uses a non standard date  
              $value[0]=str_replace("DST", "", $value[0]); 
              $value[0]=str_replace("STD", "", $value[0]); 
              $value[0]=trim($value[0]);
   
             foreach ($value as $key2 => $value2) {
             $out[$columnNames[$key2]]=$value2;    
             }
             
             //only store if time value is valid
             if (strtotime($value[0])) {       
             $final[]=$out;
             }
         }
         //var_dump($final);
         
        return $final;         
         
     }
     
        public function getControlByWebLog() {
        
        $ip = '192.168.9.195';

        $curl = curl_init();
// Set some options - we are passing in a useragent too here
        curl_setopt_array($curl, array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => "http://$ip/log.txt",
            CURLOPT_USERAGENT => 'Codular Sample cURL Request'
        ));
// Send the request & save response to $resp
        $resp = curl_exec($curl);
        if (curl_errno($curl)) {
            $error_msg = curl_error($curl);
            $this->error[]='Curl Error, cannot connect to device properly: '.$error_msg;
        }
// Close request to clear up some resources
        curl_close($curl);

        
        $csvArr = array_map('str_getcsv', explode("\n", $resp));
        // $csvArr = str_getcsv($resp);
      

        return $csvArr;
    }
     
    
}//end class

?>