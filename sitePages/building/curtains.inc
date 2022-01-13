<?php

class Curtains extends BasePage {

    public function defaultDisplay() {

    //  print  (new NinetyNineParlour)->defaultDisplay();
        
      
 $out[]=<<<HTML
         {$this->updateButtonState()}
         
         receiver heater
  {$this->changeRelayButton('parlour_receiver_heater')}     
  parlor pit lights
 {$this->changeRelayButton('parlour_pit_lights')}           
  
   {$this->curtainCards('Low Group','lowGroup')}
   {$this->curtainCards('high Group','highGroup')}
   {$this->curtainCards('First Lact. Group','firstGroup')} 
HTML;
   
            
       print(implode($out));
       
    }

private function curtainCards($title,$baseCommand) {
    
$in[]=<<<HTML
                  
        {$this->curtainButton($baseCommand.'_Up')}
        {$this->curtainButton($baseCommand.'_Dw')}
                        
HTML;
        
        $temp=(new BootStrap)->plainCard($title,implode($in));
        
              $out[]=<<<HTML
        <div class="w-50 p-3" style="background-color: #eee;">          
  $temp
          </div>      
HTML;
           
       return implode($out);
}    
    
//$command=lowGroup_Up
private function curtainButton($command) {

//based on command change the text
$upDownTxt=strstr($command,'Up') ? '<i class="fas fa-arrow-up"></i>' : '<i class="fas fa-arrow-down"></i>';


$out[]=<<<JS

   <button  class="$command btn btn-primary" enabled>
  $upDownTxt
</button>


<script>


/*
 * shows a loading for the button while long JSON call is running. 
 * 
 */
  $(document).ready(function () {
 
 $(".$command").click(function(e) {
     
      var loadingText = '<i class="fas fa-sync fa-spin"></i> ';
     
  $.ajax({

     type: 'POST',
     dataType: 'JSON',
     url: '{$GLOBALS['config']['HTTP']['JSON_API']}/170/curtainChange/$command', 

  beforeSend: function() {
    if ( $(".$command").html() !== loadingText) {
      $(".$command").data('original-text',  $(".$command").html());
      $(".$command").html(loadingText);
    }
    }
  ,  
  complete: function() {
          $(".$command").html('$upDownTxt');
  }

  });
  });  
   });  
    </script>
JS;
 


return implode($out);
        
    }

    
    
    //raw actuate relay
    private function changeRelayRaw($curl, $ip, $relay, $state) {
        curl_setopt_array($curl, array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => "http://$ip/state.xml?relay$relay" . "State=" . $state,
            CURLOPT_USERAGENT => 'Codular Sample cURL Request'
        ));
        $resp = curl_exec($curl);
    }

    //raw curtain go up or down for a specific amount of time. based on relay
    private function curtainOnDirection($relay, $time_us = 1000000) {
       
                //turns relay on and then off. 
                $curl = curl_init();
                $ip = '192.168.9.199';
                $this->changeRelayRaw($curl, $ip, $relay, 1);
                usleep($time_us); //1s default
                $this->changeRelayRaw($curl, $ip, $relay, 0);
                curl_close($curl);  // Close request to clear up some resources
            
        
    }

    //runs commands from ajax. 
    public function curtainChange($request) {
        if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_SANITIZE_STRING) === false) {
                $command = trim($request['item']);

                $relay = null;
                switch ($command) {
                    case 'lowGroup_Up':
                        $relay = 1;
                        break;
                    case 'lowGroup_Dw':
                        $relay = 2;
                        break;
                    case 'highGroup_Up':
                        $relay = 3;
                        break;
                    case 'highGroup_Dw':
                        $relay = 4;
                        break;
                    case 'firstGroup_Up':
                        $relay = 5;
                        break;
                    case 'firstGroup_Dw':
                        $relay = 6;
                        break;
                    default:
                        throw new Exception("Error Not a valid curtain command!");
                }


                $time_us = 3000000;
                $this->curtainOnDirection($relay, $time_us);
                return true;
        }}
        return false;
    }
    
    /*
     * JS that runs all the time changing the button state to the right colour. 
     */
    private function updateButtonState() {
       
        //$searchStringToColour='foo';
        $uuid = uniqid();
        $javascript = <<<EOT
<script type="text/javascript">
  $(document).ready(function() {
           (function updateState_$uuid() {
  $.ajax({
    url: '{$GLOBALS['config']['HTTP']['JSON_API']}/170/getState', 
    dataType : "html",
    success: function(data) {
         var objData = JSON.parse(data);  
             
    
    for (var key in objData) {
    if (objData.hasOwnProperty(key)) {
        console.log(key + " -> " + objData[key]);
    }
}
    
       
                

    },
    complete: function() {
      // update every 10 seconds.
      setTimeout(updateState_$uuid, 10000);
    }
  });
})();
});
      
</script>
EOT;
    return $javascript;
    }
    
    
    private function getState() {

        $curl = curl_init();
        $ip = '192.168.9.194';
        $file = '/state.json';

        curl_setopt_array($curl, array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => "http://$ip$file",
            CURLOPT_USERAGENT => 'cURL Request- JSON Control by web'
        ));
        $resp = curl_exec($curl);
        curl_close($curl);  // Close request to clear up some resources
          
        $jsonArr = json_decode($resp);
        if (json_last_error() === JSON_ERROR_NONE) {
            return (array)$jsonArr;
        } else {
            throw new Exception('Error non valid JSON returned from: '.$ip);
        }
    }

    private function changeRelayButton($command) {

//based on command change the text
$upDownTxt=strstr($command,'Up') ? '<i class="fab fa-hotjar"></i>' : '<i class="fab fa-hotjar"></i>';


$out[]=<<<JS

   <button  class="$command btn btn-lg btn-success" enabled>
  $upDownTxt
</button>


<script>


/*
 * shows a loading for the button while long JSON call is running. 
 * 
 */
  $(document).ready(function () {
 
 $(".$command").click(function(e) {
     
      var loadingText = '<i class="fas fa-sync fa-spin"></i> ';
     
  $.ajax({

     type: 'POST',
     dataType: 'JSON',
     url: '{$GLOBALS['config']['HTTP']['JSON_API']}/170/relayChange/$command', 

  beforeSend: function() {
    if ( $(".$command").html() !== loadingText) {
      $(".$command").data('original-text',  $(".$command").html());
      $(".$command").html(loadingText);
    }
    }
  ,  
  complete: function() {
          $(".$command").html('$upDownTxt');
  }

  });
  });  
   });  
    </script>
JS;
 


return implode($out);
        
    }
    
    
    public function relayChange($request) {
        if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_SANITIZE_STRING) === false) {
                $command = trim($request['item']);

                $relay = null;
                switch ($command) {
                    case 'parlour_receiver_heater':
                        $relay = 1;
                        break;
                    case 'parlour_pit_heat_left':
                        $relay = 2;
                        break;
                    case 'parlour_pit_heat_right':
                        $relay = 3;
                        break;
                    case 'parlour_pit_lights':
                        $relay = 5;
                        break;
                    case 'parlour_overhead_lights':
                        $relay = 6;
                        break;
                    
                        
                    default:
                        throw new Exception("Error Not a valid curtain command!");
                }

                
                //get current state, so we know which way to change state.
                $jsonArr=$this->getState();
                $state = $jsonArr['relay'.$relay] ? 0 : 1; //change state to be opposite.
                
                
        $curl = curl_init();
        $ip = '192.168.9.194';

                $this->changeRelayRaw($curl, $ip, $relay, $state);
        
                return true;
        }}
        return false;
    } 
    
}

/*
 * probably should extend a control by web base class with raw controls for things. 
 */
class NinetyNineParlour {
   
    public function defaultDisplay() {

   var_dump($this->getState());
   $in[]=<<<HTML
           
        {$this->changeRelayButton('parlour_receiver_heater')}                    
HTML;
  
       print(implode($in));
       
    }
    
    
    //raw actuate relay
    private function changeRelayRaw($curl, $ip, $relay, $state) {
        curl_setopt_array($curl, array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => "http://$ip/state.xml?relay$relay" . "State=" . $state,
            CURLOPT_USERAGENT => 'Codular Sample cURL Request'
        ));
        $resp = curl_exec($curl);
    }
    
   
}//end class