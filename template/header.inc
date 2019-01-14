<?php 

/** Header at top of every normal secure page
 */
include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php'); //need to include here for javascript refresh.

require_once($_SERVER['DOCUMENT_ROOT'] . '/template/basePage.php');
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
             $str ='';
            $str = $str . ('<div class="headerOneLine">');
                $str = $str . ('<span>');
                include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQuery.php');
                $bovineQ = new BovineQuery();
                $str = $str .$bovineQ->bovineSelectQuickForm('compact');
                $str = $str . ('</span>');

                $str = $str . ('<span>');
                include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/cropping/fieldQuery.inc');
                $fieldQ = new FieldQuery();
                $str = $str .$fieldQ->fieldSelectQuickForm('compact');
                $str = $str . ('</span>');

                $str = $str . ('<span>');
                include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/machinery/machineQuery.inc');
                $machineQ = new MachineQuery();
                $str = $str .$machineQ->machineSelectQuickForm('compact');
                $str = $str . ('</span>');
         
                print($str);
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
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/sideBarInfo.inc');
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
    
    //OLD WAY
    public function original() { 
        
        //runs a timer until access token expires and logs the user out.
        //if we end up using refresh tokens, this could do the refreshing.
        if ($GLOBALS['auth_scheme'] == 'oauth2Local') {
        $this->displayUntilLogoutTimer();
        }
        
        print("<div id='headerReplace'> <!--overwritten with Header AJAX --></div>" . "\n\r");
         $access_token=rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');
        ?>          
        <script type="text/javascript" language="javascript"> 
            
             $(document).ready(function(){ 
               
                        $(function(){
      function load(){
         $.ajax({
  url: "/api/html/10005/content/<?php echo($_REQUEST['pageid']);?>",
   headers: {"Authorization": "Bearer <?php echo($access_token);?>"}    
  }).done(function(data) {
     $('#headerReplace').html(data);
});
    }
    load();
    setTimeout(load,300000);//300 seconds
    });
                       
                    }); 
            
            
        </script>
        <?php 

    }

    public function content() {
        $pageid=$_REQUEST['item'];//ajax pass of pageid
        
           return ($this->main(BasePage::getPageTitleStr($pageid)));
          
    }
    
    
    

     public function main($pageTitleStr) {
        $str = '';

        if (BasePage::iPhoneDetection() == true) {
            $str = $str . self::mobileDisplay($pageTitleStr);
        } else {
            $str = $str . self::desktopDisplay($pageTitleStr);
        }
        
        return $str;
    }
    
    
    public function mobileDisplay($pageTitleStr) {
        $str = '';

        //when mobile device logs in directly with key, this isn't set, so error thrown. FIXME.
        if ($GLOBALS['auth']->getAuthData('groups') != null) {
            if ((in_array('employee', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {

                $str = $str . ('<div class="headerOneLine">');
                $str = $str . ('<span>');
                include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQuery.php');
                $bovineQ = new BovineQuery();
                $str = $str .$bovineQ->bovineSelectQuickForm('compact');
                $str = $str . ('</span>');

                $str = $str . ('<span>');
                include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/cropping/fieldQuery.inc');
                $fieldQ = new FieldQuery();
                $str = $str .$fieldQ->fieldSelectQuickForm('compact');
                $str = $str . ('</span>');

                $str = $str . ('<span>');
                include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/machinery/machineQuery.inc');
                $machineQ = new MachineQuery();
                $str = $str .$machineQ->machineSelectQuickForm('compact');
                $str = $str . ('</span>');

                $str = $str . ('<span>');
                require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/structure/search.inc');
                $s = new Search();
                $str = $str .$s->searchForm();
                $str = $str . ('</span>');

                $str = $str . ('</div">');
            }
        }
        return $str;
    }

    public function desktopDisplay($pageTitleStr) {
 $str = '';
 
        $str=$str.('<div class="globalTopContainer">' . "\n");
        $str=$str.('<div class="headerOneLine">' . "\n");

        $str=$str.('<span>' . "\n");
        $str=$str.("<a class='globalTopContainerCrest' href=\"index.php?pageid=1\"></a>" . "\n"); //clickable logo
        $str=$str.('</span>' . "\n");
                 $str = $str .($_SERVER['REMOTE_ADDR']);
        $str=$str.("<h1>$pageTitleStr</h1>" . "\n");

        //show icons only to internal employees.
        if ((in_array('employee', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {

            include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQuery.php');
            $bovineQ = new BovineQuery();
              $str=$str.$bovineQ->bovineSelectQuickForm('compact');
            
              $str=$str.('<h4 class="iconSpacer">' . '</h4>' . "\n");

            include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/cropping/fieldQuery.inc');
            $fieldQ = new FieldQuery();
              $str=$str.$fieldQ->fieldSelectQuickForm('compact');

            $str=$str.('<h4 class="iconSpacer">' . '</h4>' . "\n");

            include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/machinery/machineQuery.inc');
            $machineQ = new MachineQuery();
             $str=$str.$machineQ->machineSelectQuickForm('compact');
             
               $str=$str.('<h4 class="iconSpacer">' . '</h4>' . "\n");
        }


        require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/structure/search.inc');
        $s = new Search();
        $str=$str.$s->searchForm();

        /* opens gmail in new tab when clicked, user must be autheroized */
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {


            $str=$str.('<h4 class="iconSpacer">' . '</h4>' . "\n");

            //shows gmail count.
            $str=$str.('<span>');
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/googleMailSite.inc');
            $mail = new GoogleMailSite();
             $str=$str.$mail->mailAnchorLink();
            // $mail->original();  //FIXME? what does this do here?
            $mail->originalNewMessages();
            $str=$str.('</span>');
        }

        $str=$str.('<div class="headerRight">'); 
        if ($GLOBALS['auth_scheme'] == 'oauth2') {
             $str=$str.self::getWhoIsLoggedIn(); //$str=$str. name of person logged in.
             $str=$str.Oauth2Security::logoutButton(); //show logout button
        } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
             $str=$str.self::getWhoIsLoggedIn(); //$str=$str. name of person logged in.
             $str=$str.Oauth2Local::logoutButton(); //show logout button
        } elseif ($GLOBALS['auth_scheme'] == 'auth') {
             $str=$str.self::getWhoIsLoggedIn(); //$str=$str. name of person logged in.
             $str=$str.security::doSecurity2(); //show logout button
        } else {
            throw new Error("No auth scheme selected!");
        }

        $str=$str.('</div>' . "\n");
 
       $str=$str.('</div>' . "\n");
        $str=$str.('<!--/headerOneLine-->' . "\n");


        $str=$str.('</div>' . "\n");
        $str=$str.('<!--/globalTopContainer-->' . "\n");
        
        return $str;
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
     * 
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
    
    
    
    
}

//end class