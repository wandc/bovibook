<?php
class Fences extends BasePage {

     public function defaultDisplay() {
        
         //$this->displayFencer();
         $this->test();
     }
     
     function test() {
            /* DEBUG */
       $one=new WeMo('192.168.8.153'); //192.168.8.153	 // 192.168.9.160 192.168.9.161
      echo $one->getState();
      echo $one->getSignalStrength();
      $one->setState(1);
     
     }
     
     
      function displayFencer() {

for ($i = 1; $i <= 2; $i++) {    
    $relayNum=$i;
    
          $buttonActionCode = <<<JS
        /* when button pressed. set state */
                $('#fencer$relayNum').click(function(){
             
                $.ajax({
                    url: '{$GLOBALS['config']['HTTP']['JSON_API']}/153/fencerChangeState/$relayNum',
                    dataType: 'json',
                    async: false,
                    success: function(data) {
                     //when we changed state, then get state to be sure and change colour.
                  
                     /* get the current state */
                   $.getJSON( "{$GLOBALS['config']['HTTP']['JSON_API']}/153/fencerStateAll", function( data2,status2 ) {  
                       
                         var obj = jQuery.parseJSON(data2); /* get response for all relays*/
                         //console.log(obj);
                         changeFencerButtonColour$relayNum(obj);                  
                     });  
   
                             
                }});
            });
                  
          function changeFencerButtonColour$relayNum(obj) {
                  
            //console.log(obj);
                  
             if (obj.relay$relayNum==1) {
              $("#fencer$relayNum").css("background-color","green");
             }
             else if (obj.relay$relayNum==0) {
                $("#fencer$relayNum").css("background-color","red");
             }
             else { /*state unknown*/
              $("#fencer$relayNum").css("background-color","black");
             }
                 
             }
                             
             changeFencerButtonColour$relayNum(objFencerState); /*only called once per button */         
                  
JS;

          $out[]=$buttonActionCode;
}

$bottomJS=implode($out);
          
        $js = <<<JS
<script type='text/javascript'>
            
             $(document).ready(function(){
         
         var objFencerState = null;
    
     $.ajax({
        url: '{$GLOBALS['config']['HTTP']['JSON_API']}/153/fencerStateAll',
        type: 'get',
        dataType: 'json',
        async: false,
        success: function(data) {
            objFencerState =  jQuery.parseJSON(data);   
                //console.log( objFencerState.relay1);
        } 
               
        });
          
                $bottomJS
     });          
                  </script> 
JS;

        


   
        
print($js);

    for ($i = 1; $i <= 2; $i++) {
            $relayNum = $i;

            $html = <<<EOT

    
<button id="fencer$relayNum">Fencer $relayNum</button>

EOT;

            print($html);
        
    }}
    
    //usually ajax called.
    public function fencerStateAll() {
     $one=new WeMo('192.168.9.160');
     $two=new WeMo('192.168.9.161');
  
     $arr = array('relay1' => $one->getState(), 'relay2' =>   $two->getState(),'relay3' => 1,'relay4' => -1 );
     
     return json_encode($arr);
    }
    
    //lookup current state and if on, then turn off or opposite. 
    public function fencerChangeState($request) {
         if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {
                $relayNum = trim($request['item']);
         }}
         else {
             header("HTTP/1.0 404 Not Found");
             die();
         }

         switch ($relayNum) {
             case 1:
                $obj=new WeMo('192.168.9.160');  
                 break;
               case 2:
                $obj=new WeMo('192.168.9.161');  
                 break;
             default:
                header("HTTP/1.0 404 Not Found");
                die();
         }             
      
       $state=$obj->getState();
       if ($state==1) {
         $obj->setState(0);  
       } 
       else {
         $obj->setState(1);    
       }
        
        
    }
     
     
}//end class
/*
 * Belkin weMo insight plug control
 *  based off of:
 * http://moderntoil.com/?p=839
 */
class WeMo {
        
public $ip;
public $port;
    
function __construct($ip) {
//find the port to use for the device. seems to be 2 possible.

        $ch = curl_init(); 
        curl_setopt($ch, CURLOPT_URL, $ip.":49153"); 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
        $output = curl_exec($ch); 
        curl_close($ch);   
       // Print("First".$output);
        if (strpos($output, '404') == true) {
            $this->port = '49153';
        } else {

            $ch = curl_init();
            curl_setopt($ch, CURLOPT_URL, $ip.":49152");
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
            $output2 = curl_exec($ch);
            curl_close($ch);
            // Print("Second".$output);
            if (strpos($output2, '404') == true) {
                $this->port = '49152';
            }
        }

        if (empty($this->port)) {
            throw new Exception('Error: Can\'t find the port for ip: '.$ip.'. Device is probably down.');
            exit();
        }
             
   $this->ip=  $ip;
   
   //print("IP: ".$this->ip.'<br/>Port:'.$this->port); ///DEBUG
   
   return true;
}    

//on ==1
//off==0
public function setState($value=0) {
    
    $action="SetBinaryState";
    $body= "<BinaryState>" . $value . "</BinaryState>";
    
   $output=$this->runCommand($action,$body);
   return ($output);
}

/* is plug on?
 * return 1 for on, 0 for off, -1 for error.
 */
public function getState() {
 $action="GetBinaryState";
 $body= "<BinaryState>" . '1' . "</BinaryState>";
    
  $output=$this->runCommand($action,$body);

  try {
            if (strpos($output, '<BinaryState>0</BinaryState>') == true) {
                return 0;
            } elseif (strpos($output, '<BinaryState>8</BinaryState>') == true) {
                return 1;
            } else {
                throw new Exception('Error: getState returned unknown state: ' . $output);
            }
        } catch (Exception $e) {
            return -1;
        }
    }


    /*
     * return wifi signal strength, I think between 0 and 100. 
     */
public function getSignalStrength() {
 $action="GetSignalStrength";
 $body= "<GetSignalStrength>0</GetSignalStrength>";

$output=$this->runCommand($action,$body);
preg_match('/<SignalStrength>(\d+)<\/SignalStrength>/', $output, $match);

  try {
            if (($match) == true) {
                return $match[1];
            } else {
                throw new Exception('Error: Signal Strength returned unknown state: ' . $output);
            }
        } catch (Exception $e) {
            return -1;
        }
    }
    
private function runCommand($action,$body){
     $data = '<?xml version="1.0" encoding="utf-8"?>'
        . '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">'
        . '<s:Body><u:' . $action . ' xmlns:u="urn:Belkin:service:basicevent:1">' . $body . '</u:' . $action . '></s:Body></s:Envelope>';

   
        $header = "SOAPACTION: \"urn:Belkin:service:basicevent:1#" . $action . "\"";
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
        curl_setopt($ch, CURLOPT_USERAGENT, "");
        curl_setopt($ch, CURLOPT_TIMEOUT, 1000);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch, CURLOPT_URL, "http://" . $this->ip . ":" . $this->port . "/upnp/control/basicevent1");
        curl_setopt($ch, CURLOPT_HTTPHEADER, array (
            "Accept: ",
            "Content-type: text/xml; charset=\"utf-8\"",
            $header
        ));  
         $output = curl_exec($ch);
         curl_close($ch);
      return($output);
    
}



}
?>