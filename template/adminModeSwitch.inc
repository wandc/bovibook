<?php 

/** Header at top of every normal secure page
 */
class AdminModeSwitch {
    
    
    public function main() {
        
        if ($GLOBALS['auth']->getAdminMode() === 1) {
            $onOff = 1; //print("<h1>ADMIN MODE ON</h1>");
        } else {
            $onOff = 0; //print("<h1>ADMIN MODE OFF</h1>");
        }
        
      return   $this->SideBarRightContent($onOff);
    }
    
    //shows locked icon when on. 
    public function statusOfSwitch() {
             
    $html = <<<HTML
     
      <li class="nav-item d-none d-sm-inline-block">
        <a href="#" class="nav-link">  <i class="fas fa fa-lock mr-2"></i></a>
      </li>
        
    HTML;    
    
        $out = ($GLOBALS['auth']->getAdminMode() === 1) ? $html : '';

     return $out;   
    }
    
    
    
    private function js() {
         $access_token = rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '='); //session is serverside only, js can't access directly. 
   /* need somethng to set state on page load */
$str= <<<JS
        
    <script type="text/javascript" language="javascript">        
        $(document).ready(function () {     
            $('.flipOnOffSwitch input').on('change', function() {
        
                /* set to admin mode, user checked button */
                if(this.checked)  {
                    console.log( 'checked' );
 
                    $.ajax({
                        url: '/api/html/10007/processSwitch&mode=1',
                        headers: {"Authorization": "Bearer $access_token"},
                        success: function(data){ $('#searchOverwriteX').html(data);},
                        error: function(data, textStatus, jqXHR) { console.log(data.error);}
                    });
                     setTimeout('', 500);
                    location = location;
                }
                else{
                     /* set to normal mode, user unchecked. */
                    console.log( 'normal mode' );
        
                    $.ajax({
                        url: '/api/html/10007/processSwitch&mode=0',
                        headers: {"Authorization": "Bearer $access_token"},
                        success: function(data){ $('#searchOverwriteX').html(data);},
                        error: function(data, textStatus, jqXHR) { console.log(data.error);}
                    });
                     setTimeout('', 500);
                    location = location;
                }                
            });
        });
    </script>
JS;

return($str);
        
        
    }
    
    /*
     * turn admin mode on and off
     */
    public function processSwitch() {      
        $inputStr= filter_var($_REQUEST['mode'], FILTER_SANITIZE_NUMBER_INT);
        if ($inputStr==1) {
          $GLOBALS['auth']->setAdminMode(1);   
        }
        else {
         $GLOBALS['auth']->setAdminMode(0);   
        }
    }
    
    
 public function SideBarRightContent($onOff) {
     
     $switch=(new BootStrap)->flipSwitchOnOff($onOff);
     $switchJS=$this->js();

return  $switch.$switchJS;
     
 }


    
}

//end class
