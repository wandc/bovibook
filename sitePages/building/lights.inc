<?php

class Lights extends BasePage {

    public $barnIP;

    public function __construct() {
        $this->barnIP = '192.168.9.150'; //ip address of x-332 main barn lights
    }

    public function defaultDisplay() {
        $this->lightsBarnButtons();
      
    }

    //generate buttons to turn lights on/off
    public function lightsBarnButtons() {
        $top = <<<EOT1
<script type='text/javascript'>
            
             $(document).ready(function(){
         
         var objStart = null;
    
     $.ajax({
        url: '{$GLOBALS['config']['HTTP']['JSON_API']}/154/lightsBarnStateAll',
        type: 'get',
        dataType: 'html',
        async: false,
        success: function(data) {
            objStart =  jQuery.parseJSON(data);;
        } 
     });
EOT1;

        $bottom = <<<EOT1

   }); 
   </script>    
<!-- -->
EOT1;





        print($top);


        for ($i = 1; $i <= 16; $i++) {
            $relayNum = $i;

            $jsMiddle = <<<EOT

            
     
             function changeButtonColour$relayNum(obj) {
             
             if (obj.relay$relayNum==1) {
              $("#light$relayNum").css("background-color","green");
             }
             else if (obj.relay$relayNum==0) {
                $("#light$relayNum").css("background-color","red");
             }
             else { /*state unknown*/
              $("#light$relayNum").css("background-color","black");
             }
                 
             }
                             
             changeButtonColour$relayNum(objStart); /*only called once per button */
             
             console.log(objStart.relay1);
             
    $('#light$relayNum').click(function(){
             
        $.get('{$GLOBALS['config']['HTTP']['JSON_API']}/154/lightBarnChange/$relayNum', function(data, status){
             var obj = jQuery.parseJSON(data); /* get response for all relays*/
             
             changeButtonColour$relayNum(obj);
            
        });  });
            
       

EOT;

            print($jsMiddle);
        }



        print($bottom);


        for ($i = 1; $i <= 16; $i++) {
            $relayNum = $i;

            $html = <<<EOT

    
<button id="light$relayNum">Light $relayNum</button>

EOT;

            print($html);
        }
        //$obj=$this->lightsBarnStateAll();  
        //  print_r($obj);
    }

    public function lightsBarnStateAll() {
        $ip = $this->barnIP;

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

        $obj = $this->lightXMLProcess($resp);

        return $obj;
    }

    /*
      public function lightsBarnStateSingle() {
      if (!empty($request['item'])) {
      if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {
      $lightNum = trim($request['item']);
      $ip='192.168.9.150';

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

      $arr=(array)$this->lightXMLProcess($resp);

      return $arr['relay'.$lightNum]; //single state


      }} else{
      return null;
      }
      }
     */

    //takes xml light relay puts out and makes and object of it.
    private function lightXMLProcess($str) {


        $xml = simplexml_load_string($str);

        $relaystates = ($xml->relaystates);

        if (!is_numeric($relaystates)) {
            $tArr = str_split('X' . strrev($relaystates), 1); //works around string endiness and starting from 0 instead of 1.
            $obj = new stdClass();

            $obj->relay1 = $tArr[1];
            $obj->relay2 = $tArr[2];
            $obj->relay3 = $tArr[3];
            $obj->relay4 = $tArr[4];
            $obj->relay5 = $tArr[5];
            $obj->relay6 = $tArr[6];
            $obj->relay7 = $tArr[7];
            $obj->relay8 = $tArr[8];
            $obj->relay9 = $tArr[9];
            $obj->relay10 = $tArr[10];
            $obj->relay11 = $tArr[11];
            $obj->relay12 = $tArr[12];
            $obj->relay13 = $tArr[13];
            $obj->relay14 = $tArr[14];
            $obj->relay15 = $tArr[15];
            $obj->relay16 = $tArr[16];
        } else {
            throw new Exception("Error: no valid response from light relays.");
        }

        return $obj;
    }

    //flips state of specified relay
    public function lightBarnChange($request) {
        if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {
                $lightNum = trim($request['item']);

                $ip = '192.168.9.150';

                $currentStateArr = (array) $this->lightsBarnStateAll();
                $relay = $currentStateArr['relay' . $lightNum];
                if ($relay == 0) {
                    $state = 1;
                } else {
                    $state = 0;
                }

                $curl = curl_init();
// Set some options - we are passing in a useragent too here
                curl_setopt_array($curl, array(
                    CURLOPT_RETURNTRANSFER => 1,
                    CURLOPT_URL => "http://$ip/state.xml?relay$lightNum" . "State=" . $state,
                    CURLOPT_USERAGENT => 'Codular Sample cURL Request'
                ));
// Send the request & save response to $resp
                $resp = curl_exec($curl);
// Close request to clear up some resources
                curl_close($curl);

                $obj = $this->lightXMLProcess($resp);

                return $obj;
            }
        } else {
            return null;
        }
    }


}

//end class
?>