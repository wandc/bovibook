<?php

//

//require_once($_SERVER['DOCUMENT_ROOT'].'/functions/croppingHelper.inc');
;

require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');

/**
  Test page to try new things with.
 */
class Building extends tabPage {

     public function defaultDisplay() {
        //double check to make sure only admin can see
       
            //setup for tabs.
            $tabArray[1]['name'] = 'Doors';
            $tabArray[2]['name'] = 'Lights';
            $tabArray[3]['name'] = 'Scales';
            $tabArray[4]['name'] = 'BulkTanks';

            $this->renderTabs($tabArray);
        
    }
    
    //bulk tanks
    function tab4() {
       $this->displayBulkTankState();
    }
    
    
    function getBulkTankWhiskerSwitchState() {
        
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
    
    
    
    
    
    //who is currently in the feeder.
    function displayBulkTankState() {
      
        
        print("<div id='BulkTankATop'>No Data</div>");
        print("<div id='BulkTankABottom'>No Data</div>");
        print("<div id='BulkTankBTop'>No Data</div>");
        print("<div id='BulkTankBBottom'>No Data</div>");
        print("<div id='Pasteurizer'>No Data</div>");
        
       ?> 
        
        <script>
            $(document).ready(function() {
           (function worker() {
  $.ajax({
    url: 'https://int.littleriver.ca/api/json/142/getBulkTankWhiskerSwitchState', 
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
        <?php
    }
    
    
    
    //scales
     function tab3() {
    
         
     }
    function tab1() {
           
   

       
         $cssButton = <<<EX
              <style>


#door1 {
    top: 490px;
    left: 369px;
    position: absolute;
    height: 6em;
    width: 10em;
                 }
                 
 #door2 {                
    top: 87px;
    left: 369px;
    position: absolute;
    height: 6em;
    width: 10em;
     }            

   #door3 {
    top: 490px;
    left: 558px;
    position: absolute;
    height: 6em;
    width: 10em;
                 }
                 
 #door4 {                
    top: 87px;
    left:  562px;
    position: absolute;
    height: 6em;
    width: 10em;
     }    
                 
</style>

EX;


        $width=1500;  
       $height=1500;
       
         $svg = <<<EX
        <svg width="$width" height="$height">
         <g transform="scale(3 1.5) rotate(90 110 123)">
    
        <!--main barn square -->
        <line x1='0' y1='0' x2='0' y2='123' stroke = 'black' stroke-width = '1'/>        
         <line x1='337' y1='0' x2='337' y2='123' stroke = 'black' stroke-width = '1'/>
         <line x1='0' y1='0' x2='337' y2='0' stroke = 'black' stroke-width = '1'/>
        
         <line x1='0' y1='123' x2='158' y2='123' stroke = 'black' stroke-width = '1'/>
         <line x1='172' y1='123' x2='337' y2='123' stroke = 'black' stroke-width = '1'/>
        
        <!-- divide up barn into squares -->
        
        <!--centre vertical -->
        <line x1='158' y1='0' x2='158' y2='123' stroke = 'grey' stroke-width = '1'/> 
        <line x1='172' y1='0' x2='172' y2='123' stroke = 'grey' stroke-width = '1'/> 
        
        <!--other vertical -->
         <line x1='82.5' y1='0' x2='82.5' y2='58.5' stroke = 'grey' stroke-width = '1'/> 
        <line x1='82.5' y1='61.5' x2='82.5' y2='123' stroke = 'grey' stroke-width = '1'/> 
        
        <line x1='247.5' y1='0' x2='247.5' y2='58.5' stroke = 'grey' stroke-width = '1'/> 
         <line x1='247.5' y1='61.5' x2='247.5' y2='123' stroke = 'grey' stroke-width = '1'/> 
        
        <!--horizontal -->
        <line x1='0' y1='58.5' x2='158' y2='58.5' stroke = 'grey' stroke-width = '1'/>    
        <line x1='172' y1='58.5' x2='337' y2='58.5' stroke = 'grey' stroke-width = '1'/>  
         <line x1='0' y1='61.5' x2='158' y2='61.5' stroke = 'grey' stroke-width = '1'/>    
        <line x1='172' y1='61.5' x2='337' y2='61.5' stroke = 'grey' stroke-width = '1'/>  
        
        <!-- breezeway -->
         <line x1='158' y1='123' x2='158' y2='186' stroke = 'black' stroke-width = '1'/>    
         <line x1='172' y1='123' x2='172' y2='233' stroke = 'black' stroke-width = '1'/>   
            <!-- parlor -->
         <line x1='158' y1='186' x2='50' y2='186' stroke = 'black' stroke-width = '1'/>   
         <line x1='50' y1='186' x2='50' y2='233' stroke = 'black' stroke-width = '1'/>   
         <line x1='50' y1='233' x2='172' y2='233' stroke = 'black' stroke-width = '1'/>   
     
                 </g>
     </svg>
        

EX;
         
         
         
 print($cssButton);
        
 print('<div id="buttonDiv">');
  print($svg);   
  
  
 print '<div id="buildingDoor1"/>'. $this->showDoorButton(1) . '</div>';
 print '<div id="buildingDoor2"/>'. $this->showDoorButton(2) . '</div>';
 print '<div id="buildingDoor3"/>'. $this->showDoorButton(3) . '</div>';
 print '<div id="buildingDoor4"/>'. $this->showDoorButton(4) . '</div>';
       
        print('</div>');

        
    
        
}
    
    
    function tab2() {

        // $LightsBarn=new LightsBarn();
        $this->lightsBarnButtons();


    }

    //generate buttons to turn lights on/off
    public function lightsBarnButtons() {
        $top = <<<EOT1
<script type='text/javascript'>
            
             $(document).ready(function(){
         
         var objStart = null;
    
     $.ajax({
        url: 'https://int.littleriver.ca/api/json/142/lightsBarnStateAll',
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
             
        $.get('https://int.littleriver.ca/api/json/142/lightBarnChange/$relayNum', function(data, status){
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
        $ip = '192.168.9.150';

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

    //show garage light open/close buttons
    public function showDoorButton($buttonNum) {


        switch ($buttonNum) {
            case 1:
                $but = <<<EX
                   <button class="btn btn-block btn-primary btn-lg" id="door1">Door1 - Manure Pit</button>
                <script type='text/javascript'>
                    $(document).ready(function () {
                        $('#door1').click(function () {
                            $.get('https://int.littleriver.ca/api/json/142/doorOpenClose/1', function (data, status) {
                                  $("#door1").removeClass("btn-primary");
                                  $("#door1").addClass("btn-success");
                                  setTimeout(function() {    buttonReset1(); }, 10000);  }); //wait then run function to reset button back to default                  
                            });      
                            
                        function buttonReset1 () {   
                               $("#door1").removeClass("btn-success");
                               $("#door1").addClass("btn-primary");  
                        };
                    
                    });
                    
                    
                    
                </script> 
EX;
                break;
            case 2:
                $but = <<<EX
                    <button id="door2" class="btn btn-block btn-primary btn-lg">Door2 - Maternity</button>
                <script type='text/javascript'>
                    $(document).ready(function () {
                        $('#door2').click(function () {
                            $.get('https://int.littleriver.ca/api/json/142/doorOpenClose/2', function (data, status) {
                                  $("#door2").removeClass("btn-primary");
                                  $("#door2").addClass("btn-success");
                                  setTimeout(function() {    buttonReset2(); }, 10000);  }); //wait then run function to reset button back to default                  
                            });      
                            
                        function buttonReset2 () {   
                               $("#door2").removeClass("btn-success");
                               $("#door2").addClass("btn-primary");  
                        };
                            });
                    
                </script> 
EX;
                break;
            case 3:
                $but = <<<EX
                    <button id="door3" class="btn btn-block btn-primary btn-lg">Door3 - High Group</button>
                <script type='text/javascript'>
                    $(document).ready(function () {
                        $('#door3').click(function () {
                            $.get('https://int.littleriver.ca/api/json/142/doorOpenClose/3', function (data, status) {         
                                  $("#door3").removeClass("btn-primary");
                                  $("#door3").addClass("btn-success");
                                  setTimeout(function() {    buttonReset3(); }, 10000);  }); //wait then run function to reset button back to default                  
                            });      
                            
                        function buttonReset3 () {   
                               $("#door3").removeClass("btn-success");
                               $("#door3").addClass("btn-primary");  
                        };
                            }); 
                    
                </script> 
EX;

                break;
            case 4:
                $but = <<<EX
                    <button id="door4" class="btn btn-block btn-primary btn-lg">Door4 - Dry Group</button>
                <script type='text/javascript'>
                    $(document).ready(function () {
                        $('#door4').click(function () {
                            $.get('https://int.littleriver.ca/api/json/142/doorOpenClose/4', function (data, status) {      
                                  $("#door4").removeClass("btn-primary");
                                  $("#door4").addClass("btn-success");
                                  setTimeout(function() {    buttonReset4(); }, 10000);  }); //wait then run function to reset button back to default                  
                            });      
                            
                        function buttonReset4 () {   
                               $("#door4").removeClass("btn-success");
                               $("#door4").addClass("btn-primary");  
                        };
                            });  
                  
                </script> 
EX;

                break;
            default:
                throw new Exception("Error unknown door button number.");
        }
        return($but);
    }

    /*
     * this does two curl cals to trigger a voltage pulse to open light.
     * 
     */

    public function doorOpenClose($request) {


        if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {
                $lightNum = trim($request['item']);


                switch ($lightNum) {
                    case 1:
                        $ip = '192.168.8.10';
                        break;
                    case 2:
                        $ip = '192.168.8.11';
                        break;
                    case 3:
                        $ip = '192.168.8.12';
                        break;
                    case 4:
                        $ip = '192.168.8.13';
                        break;
                    default:
                        throw new Exception("Error unknow light number.");
                }


                /*
                 * curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET "http://192.168.8.11/state.xml?relay1State=1";sleep .1;curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET "http://192.168.8.11/state.xml?relay1State=0"
                 */

                // Get cURL resource
                $curl = curl_init();
// Set some options - we are passing in a useragent too here
                curl_setopt_array($curl, array(
                    CURLOPT_RETURNTRANSFER => 1,
                    CURLOPT_URL => "http://$ip/state.xml?relay1State=1",
                    CURLOPT_USERAGENT => 'Codular Sample cURL Request'
                ));
// Send the request & save response to $resp
                $resp = curl_exec($curl);
// Close request to clear up some resources
                curl_close($curl);

                usleep(200000);

                // Get cURL resource
                $curl2 = curl_init();
// Set some options - we are passing in a useragent too here
                curl_setopt_array($curl2, array(
                    CURLOPT_RETURNTRANSFER => 1,
                    CURLOPT_URL => "http://$ip/state.xml?relay1State=0",
                    CURLOPT_USERAGENT => 'Codular Sample cURL Request'
                ));
// Send the request & save response to $resp
                $resp2 = curl_exec($curl2);
// Close request to clear up some resources
                curl_close($curl2);


                return "true";
            }
        }
    }

}

//end clas
?>