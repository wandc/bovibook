<?php
class BulkTank extends BasePage {

     public function defaultDisplay() {
           $this->displayBulkTankState();
     }
     
      //who is currently in the feeder.
    public function displayBulkTankState() {
      
       $html = <<<HTML
        <div id='BulkTankATop'>No Data</div>
        <div id='BulkTankABottom'>No Data</div>     
        <div id='BulkTankBTop'>No Data</div>   
        <div id='BulkTankBBottom'>No Data</div> 
        <div id='Pasteurizer'>No Data</div>
HTML; 
       
 $js = <<<JS
  <script>
            $(document).ready(function() {
           (function worker() {
  $.ajax({
    url: {$GLOBALS['config']['HTTP']['URL_SHORT']}+'api/json/156/getBulkTankWhiskerSwitchState', 
    dataType : "html",
    success: function(data) {
         var objData = JSON.parse(data);  
         
         //erase the old data
         $( "#BulkTankATop").html('');
         $( "#BulkTankABottom").html('');
         $( "#BulkTankBTop").html('');
         $( "#BulkTankBBottom").html('');
         $( "#Pasteurizer").html('');
            
          /* write data to div*/      
 $( "#BulkTankATop").html(objData['input1state']);
 $( "#BulkTankBTop").html(objData['input2state']);
 $( "#BulkTankABottom").html(objData['input3state']);
 $( "#BulkTankBBottom").html(objData['input4state']);
 $( "#Pasteurizer").html(objData['input5state']);
 

    },
    complete: function() {
      // Schedule the next request when the current one's complete
      setTimeout(worker, 2000);
    }
  });
})();
});

        </script>
JS;       
        
  print($html.$js);
    }
     
      
    
     public function getBulkTankWhiskerSwitchState() {
        
        $ip = '192.168.9.181';

        $curl = curl_init();
// Set some options - we are passing in a useragent too here
        curl_setopt_array($curl, array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => "http://$ip/state.xml",
            CURLOPT_USERAGENT => 'Codular Sample cURL Request'
        ));
// Send the request & save response to $resp
        $resp = curl_exec($curl);
// Close request to clear up some resources
        curl_close($curl);

         $xml = simplexml_load_string($resp);
      

        return $xml;
    }
    
    
}//end class
?>