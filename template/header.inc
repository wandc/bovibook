<?php 

/** Header at top of every normal secure page
 */

class Header {

   
    public function headerContent(){
        ?>

<!-- needed to allow bootstrap drop downs to work -->
        <script>
            $(document).ready(function() {
    $(".dropdown-toggle").dropdown();
});
            </script>
            
  <!-- Main Header -->
  <header class="main-header">

    <!-- Logo -->
    <a href="index.php" class="logo">
      <!-- mini logo for sidebar mini 50x50 pixels -->
      <span class="logo-mini"><b>L</b>R</span>
      <!-- logo for regular state and mobile devices -->
      <span class="logo-lg"><b>Little</b>River</span>
    </a>

    <!-- Header Navbar -->
    <nav class="navbar navbar-static-top" role="navigation">
        
        
        
        
      <!-- Sidebar toggle button-->
     <a href="#" class="sidebar-toggle" data-toggle="push-menu" role="button">
        <span class="sr-only">Toggle navigation</span>
      </a>
       <?php
       
         $hf=new HeaderForm();
         $animals=$hf->animals();
         $fields=$hf->fields();
         $machines=$hf->machines();
         
                 $html = <<<HTML
<div class="headerOneLine">
                        <span>
                        $animals
                        $fields
                        $machines
                        </span>
<!-- close div later -->
HTML;
         
         
                print($html);
            ?>
      <!-- Navbar Right Menu -->
      <div class="navbar-custom-menu">
        <ul class="nav navbar-nav">
          <!-- Messages: style can be found in dropdown.less-->
          
          
          
          
           <?php
       $str='';
                   $str=$str.('<div class="headerRight">'); 
        if ($GLOBALS['auth_scheme'] == 'oauth2') {
             $str=$str.self::getWhoIsLoggedIn(); //$str=$str. name of person logged in.
             $str=$str.Oauth2Security::logoutButton(); //show logout button
        } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
             $str=$str.self::getWhoIsLoggedIn(); //$str=$str. name of person logged in.
             $str=$str.Oauth2Local::logoutButton(); //show logout button
        } else {
            throw new Error("No auth scheme selected!");
        }

        $str=$str.('</div>' . "\n");
        print($str);
           ?>  
          
        <?php
          
        //show side bar.
        //only show to admin and owner. 
        if (((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) {
            $sideBarInfo = new SideBarInfo();
        } 

          ?>
          
          <li class="dropdown messages-menu">
            <!-- Menu toggle button -->
        
            <a href="#" class="dropdown-toggle" data-toggle="dropdown">
              <i class="fa fa-bell-o"></i>
              <span class="label label-warning"><?php print($sideBarInfo->warningCount);?></span>
            </a>
            
            <ul class="dropdown-menu">
                
                
             <?php  if (((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) { print($sideBarInfo->content(2));}  ?>     
           
            </ul>
          </li>
          <!-- /.messages-menu -->

          <!-- Notifications Menu -->
          <li class="dropdown notifications-menu">
            <!-- Menu toggle button -->
            <a href="#" class="dropdown-toggle" data-toggle="dropdown">
              <i class="fa fa-bell-o"></i>
              <span class="label label-danger"><?php print($sideBarInfo->dangerCount);?></span>
            </a>
            <ul class="dropdown-menu">
              <li class="header">You have 10 notifications</li>
              <li>
                <!-- Inner Menu: contains the notifications -->
                <ul class="menu">
                    <?php  if (((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) { print($sideBarInfo->content(1));}  ?>     
                       <?php  if (((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE))) { print($sideBarInfo->content(3));}  ?>     
                  <li><!-- start notification -->
                    <a href="#">
                      <i class="fa fa-users text-aqua"></i> 5 new members joined today
                    </a>
                  </li>
                  <!-- end notification -->
                </ul>
              </li>
              <li class="footer"><a href="#">View all</a></li>
            </ul>
          </li>
          <!-- Tasks Menu -->
          <li class="dropdown tasks-menu">
            <!-- Menu Toggle Button -->
            <a href="#" class="dropdown-toggle" data-toggle="dropdown">
              <i class="fa fa-flag-o"></i>
              <span class="label label-danger">9</span>
            </a>
            <ul class="dropdown-menu">
              <li class="header">You have 9 tasks</li>
              <li>
                <!-- Inner menu: contains the tasks -->
                <ul class="menu">
                  <li><!-- Task item -->
                    <a href="#">
                      <!-- Task title and progress text -->
                      <h3>
                        Design some buttons
                        <small class="pull-right">20%</small>
                      </h3>
                      <!-- The progress bar -->
                      <div class="progress xs">
                        <!-- Change the css width attribute to simulate progress -->
                        <div class="progress-bar progress-bar-aqua" style="width: 20%" role="progressbar" aria-valuenow="20" aria-valuemin="0" aria-valuemax="100">
                          <span class="sr-only">20% Complete</span>
                        </div>
                      </div>
                    </a>
                  </li>
                  <!-- end task item -->
                </ul>
              </li>
              <li class="footer">
                <a href="#">View all tasks</a>
              </li>
            </ul>
          </li>
    
          <!-- Control Sidebar Toggle Button -->
          <li>
            <a href="#" data-toggle="control-sidebar"><i class="fa fa-gears"></i></a>
          </li>
        </ul>
      </div>
    </nav>
  </header>

        <?php
    }
    

    /**
     * small function to $str=$str. who is logged in. 
     */
    function getWhoIsLoggedIn() {
        if ($GLOBALS['auth']->checkAuth()) {
            $loginName = ucwords($GLOBALS['auth']->getUsername()); //grab current user name and make first letter upper-case.
            return("<h2 class=\"loginNameBox\">$loginName</h2>");
        } else {
            return('<!--No One Logged In-->');
        }
    }

    /*
     * 
     * 
     * LOGOUT TIMER
     * FIXME: NOT USED.
     */
    public function untilLogoutTimerContent() {
          $expireTime=strtotime(Oauth2Local::findOutWhenSessionAccessTokenExpiresEpoch());
          $nowTime=strtotime("now");
          $diff=$expireTime-$nowTime;

          if ($diff<=200) {
          print('Warrning Logout in:'.$diff.'s');
           }
          
          if ($diff<=30) {
           ?>   
              <script type="text/javascript">
         $(document).ready(function(){ 
            function Redirect() {
               window.location=window.location.protocol + "//" + window.location.host + "?reset=1";
            }
             Redirect(); 
             }); 
      </script> 
              
           <?php    
          }
    }
    
    
     public  function displayUntilLogoutTimer($dom_id='timeUntilLogout') { 
         
        print("<div id='$dom_id'> <!--overwritten_by_header_ajax--> </div>" . "\n\r");
         $access_token=rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');
        ?>          
        <script type="text/javascript" language="javascript"> 
            
      $(document).ready(function(){ 
                               
                   $(function(){
                      function load2(){
                         $.ajax({
                  url: "/api/html/10006/untilLogoutTimerContent/<?php  echo($_REQUEST['pageid']); ?>",
                   headers: {"Authorization": "Bearer <?php  echo($access_token); ?>"}    
                  }).done(function(data) {
                     $('#<?php  echo($dom_id); ?>').html(data);
                });
                    }
                    load2();
                    setInterval(load2,10000);//10 seconds
                    });
                                       
             }); 
                    
            
        </script>
        <?php 

    }
  
}//end class

/*
 * custom forms for the top of the page header
 * for animals, crops, machinery
 */
Class HeaderForm {

   private function createSelectOption($arr) {

        foreach ($arr as $key => $value) {
            $out[] = '<option value="' . $key . '">' . $value . '</option>';
        }
        return implode($out);
    }

    public function animals() {
        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveCows(false);
        $options = $this->createSelectOption($aniNumber);

        $html = <<<HTML
  <select  class="form-control" id="bovineSelect" onchange="javascript:location.href='?bovine_id='+this.options[this.selectedIndex].value+'&pageid={$GLOBALS['config']['static_page_location']['bovineQuery']}';">
     $options
    </select>
HTML;
        return $html;
    }

    public function fields() {
        $aniNumber = CroppingHelper::listOfCurrentFields();
        $options = $this->createSelectOption($aniNumber);

        $html = <<<HTML
  <select  class="form-control" id="fieldSelect"  onchange="javascript:location.href='?field_id='+this.options[this.selectedIndex].value+'&amp;pageid={$GLOBALS['config']['static_page_location']['fieldQuery']}';">
     $options
    </select>
HTML;
        return $html;
    }
    
    public function machines() {
        $aniNumber =(new MachineQuery)->createListOfAllActiveMachines('default');
        $options = $this->createSelectOption($aniNumber);

        $html = <<<HTML
  <select  class="form-control" id="machineSelect" onchange="javascript:location.href='?machine_id='+this.options[this.selectedIndex].value+'&amp;pageid={$GLOBALS['config']['static_page_location']['machineQuery']}';">
     $options
    </select>
HTML;
        return $html;
    }
}
//end class