<?php

class Doors extends BasePage {

    public function defaultDisplay() {

        print($this->displayDoors());
    }

    public function displayDoors() {
        $width = 1500;
        $height = 1500;

        $svg = <<<SVG
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
        

SVG;

        //  $svg=($svg);

        $cssButton = <<<CSS
              <style>
    
.xxx {
    
                 height:300px;
                 }
                 
          
</style>

CSS;







        $out[] = ($cssButton);



        //print($svg);
        $html = <<<HTML
          
  
<div class="container">
  <div class="row">
   
    <div class="col-md-3">
     
    </div>
    
    <div class="col-md-3">
    </div>
      
    <div class="col-md-3">
     {$this->showDoorButton(2)}
    </div>
     <div class="col-md-3">
     {$this->showDoorButton(4)}
    </div>
    </div>
     
      <div class="row">
   
    <div class="col-md-3">
      {$this->showDoorButton(5)}
    </div>
    
    <div class="col-md-3">
    </div>
      
    <div class="col-md-3">
    
    </div>
     <div class="col-md-3">
   
    </div>
    </div> 
     
     
    <div class=" xxx row"> 
    </div>
     
     <div class="row">
    <div class="col-md-3">
      
    </div>
     <div class="col-md-3">
 
    </div>
    <div class="col-md-3">
     {$this->showDoorButton(1)}
    </div>
     <div class="col-md-3">
     {$this->showDoorButton(3)}
    </div>
  </div> 
     
     
  <div class="row">
    <div class="col-md-3">
    </div>
    <div class="col-md-3">
        {$this->showDoorButton(6)}
    </div>
    <div class="col-md-3">
    </div>
    <div class="col-md-3">
    </div>
  </div>
</div>   
          
HTML;




        $out[] = $html;





        return implode($out);
    }

    private function doorJS($num, $title) {

        $but = <<<JS
            <button type="button" class="btn btn-block btn-primary btn-lg" id="door{$num}">Door{$num} - {$title}</button>
                <script type='text/javascript'>
                    $(document).ready(function () {
                        $('#door{$num}').click(function () {
                            $.get('https://int.littleriver.ca/api/json/152/doorOpenClose/{$num}', function (data, status) {
                                  $("#door{$num}").removeClass("btn-primary");
                                  $("#door{$num}").addClass("btn-success");
                                  setTimeout(function() {    buttonReset{$num}(); }, 10000);  }); //wait then run function to reset button back to default                  
                            });      
                            
                        function buttonReset{$num} () {   
                               $("#door{$num}").removeClass("btn-success");
                               $("#door{$num}").addClass("btn-primary");  
                        };
                    
                    });

                </script> 
      
JS;
        return $but;
    }

    //show garage light open/close buttons
    public function showDoorButton($buttonNum) {


        switch ($buttonNum) {
            case 1:
                $but = $this->doorJS(1, 'Manure Pit');
                break;
            case 2:
                $but = $this->doorJS(2, 'Maternity');
                break;
            case 3:
                $but = $this->doorJS(3, 'High Group');
                break;
            case 4:
                $but = $this->doorJS(4, 'Dry Group');
                break;
            case 5:
                $but = $this->doorJS(5, 'Gator');
                break;
            case 6:
                $but = $this->doorJS(6, 'Shop');
                break;
            default:
                throw new Exception("Error unknown door button number.");
        }
        return($but);
    }

    private function doorNumberToIP($doorNum) {

        switch ($doorNum) {
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
            case 5:
                $ip = '192.168.8.14';
                break;
            case 6:
                $ip = '192.168.8.15';
                break;
            default:
                throw new Exception("Error unknow door number.");
        }
        return $ip;
    }

    /*
     * this does two curl cals to trigger a voltage pulse to open door.
     * 
     */

    public function doorOpenClose($request) {


        if (!empty($request['item'])) {
            if (!filter_var($request['item'], FILTER_VALIDATE_INT) === false) {
                $doorNum = trim($request['item']);

                $ip = $this->doorNumberToIP($doorNum);


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

    /*
     * used by error function just to see if door is up and returning something. 
     */

    private function checkDoorAlive($doorNum) {


        $ip = $this->doorNumberToIP($doorNum);

        $curl = curl_init();
        curl_setopt($curl, CURLOPT_HTTP09_ALLOWED, true); //IMPORTANT or will not work with this old control by web controller. 
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

        if (str_contains($resp, '<datavalues>')) {
            //all is well
            $error = null;
        } else {
            //error
            $error = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 3, " 99 Barn Garage Door controller # $doorNum is down.");
        }

        return $error;
    }

    
    public function error() {

        $errorSub = array();

        //check each of the doors for errors
        for ($i = 1; $i <= 6; $i++) {
            $ret = $this->checkDoorAlive($i);
            if (!empty($ret)) {
                $errorSub[] = $ret;
            }
        }

        return $errorSub;
    }

}

//end class
?>