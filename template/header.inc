<?php 

/** Header at top of every normal secure page
 */

class Header {

    
    
    private function loginHeaderInfo() {
             
   
  
        if ($GLOBALS['auth_scheme'] == 'oauth2') {
             $out[]= self::getWhoIsLoggedIn(); //  name of person logged in.
             $out[]= Oauth2Security::logoutButton(); //show logout button
        } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
             $out[]= self::getWhoIsLoggedIn(); // name of person logged in.
             $out[]= Oauth2Local::logoutButton(); //show logout button
        } else {
            throw new Error("No auth scheme selected!");
        }

       
        
        return implode($out);
    }

    
    function corn() {
        $html = <<<HTML
        <svg aria-hidden="true" focusable="false" data-prefix="far" data-icon="corn" role="img" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" class="svg-inline--fa fa-corn fa-w-16 fa-3x"><path fill="currentColor" d="M441.79.32c-2.07-.2-4.57-.32-7.04-.32-12.1 0-23.73 2.82-34.13 8.01-3.53-.5-7.11-.75-10.72-.75-7.27 0-14.52 1.05-21.53 3.12a76.524 76.524 0 0 0-25.45 12.79c-9.17.42-18.11 2.45-26.63 6.07-8.9 4.01-16.77 9.4-23.38 15.86a76.438 76.438 0 0 0-26.06 9.58c-7.6 4.3-14.32 9.96-19.91 16.61-8.69 2.35-16.93 6.25-24.4 11.58-5.01 3.53-8.85 8.24-12.81 12.8-4.79-5.68-9.5-11.39-14.94-16.82L152.63 36.7c-9.01-9.01-24.43-4.34-26.93 8.15L98.51 180.72l-61.48 61.46c-47.25 47.23-49.15 122.14-6.51 171.46l60.7 61.97c27.39 27.38 65.59 40.41 105.41 35.3 29.25-3.75 55.89-18.9 76.75-39.75l57.83-57.81 135.93-27.18c12.5-2.5 17.17-17.91 8.15-26.92l-42.16-42.15c-5.32-5.32-10.89-10.28-16.62-14.96 3.61-3.4 7.61-6.35 10.55-10.46 5.29-7.4 9.13-15.55 11.49-24.04a76.011 76.011 0 0 0 17.39-20.81c4.6-7.74 7.69-16.19 9.24-24.91a76.238 76.238 0 0 0 15.55-22.55c3.97-8.82 6.19-18.12 6.68-27.45a77.528 77.528 0 0 0 12.88-24.7 78.082 78.082 0 0 0 2.61-31.7 77.244 77.244 0 0 0 8.93-32.94l.04-1v-1c2.4-41.1-28.34-76.61-70.08-80.26zM64.38 379.79c-24.07-30.62-21.7-75.58 6.5-103.78l71.73-71.7 18.33-91.64c45.96 45.94 62.27 102.09 48.95 155.23-33.79 12.01-65.48 31.89-92.77 59.17l-52.74 52.72zm243.24-10.53l-71.73 71.7c-30.38 30.37-80.44 31.18-110.82.81l-27.5-27.49 53.39-53.37c37.92-37.91 86.06-58.39 133.48-58.39 41.21 0 81.88 15.47 114.84 48.42l-91.66 18.32zM447.95 105.4c6.45 7.13 9.43 17.47 6.45 27.58-3 9.65-10.59 16.78-19.55 19.53 5.05 7.82 6.21 17.93 2.07 27.12-4.14 9.19-12.42 14.94-21.61 16.55 4.14 8.27 3.91 18.38-1.14 26.89-4.84 8.51-13.57 13.56-22.77 14.25 3.22 8.5 2.3 18.38-3.44 26.43-4.12 5.77-10.05 8.77-16.29 10.44-27.19-12.65-56.74-19.53-87.22-19.53-7.91 0-15.79.47-23.62 1.32 4.84-38.96-2.36-78.56-21.29-115.62 1.36-7.19 4.66-13.99 10.93-18.41 9.49-6.78 20.02-5.96 26.66-3.46.69-8.96 5.75-17.7 14.26-22.53 13.58-8.05 26.54-1.31 26.9-1.14 1.61-8.97 7.59-17.24 16.78-21.38 11.56-4.9 21.83-1.13 27.13 2.3 2.76-9.19 10.11-16.54 19.77-19.3 2.58-.76 16.01-4.31 27.59 6.89 23.37-44.68 85.4 17.95 38.39 42.07z" class=""></path></svg>
HTML;
        return $html;
        
    }
    
    
    public function headerContent() {
        
            $hf=new HeaderForm();
        $adminModeSwitch= new AdminModeSwitch;
       
   $s = new Search();
        
        
$html = <<<HTML
        
        
        <nav class="main-header navbar navbar-expand navbar-white navbar-light">
    <!-- Left navbar links -->
    <ul class="navbar-nav">
      <li class="nav-item">
        <a class="nav-link" data-widget="pushmenu" href="#" role="button"><i class="fas fa-bars"></i></a>
      </li>
      <li class="nav-item d-none d-sm-inline-block">
         
      {$hf->animals()}       
 
      </li>
<li class="nav-item d-none d-sm-inline-block">
{$hf->fields()}
      </li>
<li class="nav-item d-none d-sm-inline-block">
{$hf->machines()}
      </li>
  </li>
<li class="nav-item d-none d-sm-inline-block">
{$this->corn()}
      </li>
     

<li class="nav-item">
   <a class="nav-link" href="index.php?pageid=172" role="button"><span style="color: #bebebe;"><i class="fas fa-user-tie fa-2x "style=" line-height: 15px;"></i></span></a>

  </li>

    </ul>


  {$this->searchForm()}

  
 
    <!-- Right navbar links -->
    <ul class="navbar-nav ml-auto">
      {$this->messagesDropdownMenu()}
      {$this->notificationsDropdownMenu()}

      {$adminModeSwitch->statusOfSwitch()}
        {$this->settings()}
      
        
        
       <li class="nav-item d-none d-sm-inline-block">
        <a href="{$GLOBALS['config']['HTTP']['URL_SHORT']}/index.php?resetauth=1" class="nav-link">  <i class="fas fa fa-sign-in-alt mr-2"></i></a>
      </li>
   
   </ul>
  </nav>
        
HTML;
    return ($html);
        
    }
    
    
    private function settings() {
        
         //show side bar.
        //only show to admin and owner. 
        $adminStrOut=null;
        $adminArr=array();
        $adminStr=array();
          if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $adminModeSwitch= new AdminModeSwitch();
            $adminStr[]=$adminModeSwitch->main();
            
            
                //this will not work because form process function never gets called because code is in ajax tab.
              $c = new QF2_AdminParams((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', null);
              $form=$c->renderBox('QF2_AdminParams', 'Admin Paramaters');
              $adminArr[]=$form;
                 
        } 
        $adminStrOut=implode($adminStr);
        
       
        
$html = <<<HTML
                  
    <li class="nav-item dropdown">
        
         <a class="nav-link" data-toggle="dropdown" href="#">
          <i class="fas fa-th-large"></i>
        </a>
        
   
        
        <div class="dropdown-menu dropdown-menu-lg dropdown-menu-right">
        
          <span class="dropdown-item dropdown-header">12 Notifications</span>
         
          {$adminStrOut}
          
        
           <span class="dropdown-item dropdown-header">10 Notifications</span>          
          <div class="dropdown-divider"></div>
        
          <a href="#" class="dropdown-item">
            <i class="fas fa-envelope mr-2"></i> 4 new messages
            <span class="float-right text-muted text-sm">3 mins</span>
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <i class="fas fa-users mr-2"></i> 8 friend requests
            <span class="float-right text-muted text-sm">12 hours</span>
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <i class="fas fa-file mr-2"></i> 3 new reports
            <span class="float-right text-muted text-sm">2 days</span>
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item dropdown-footer">See All Notifications</a>
        </div>
    </li>    
                  
HTML;

    
    return $html;
        
    }
    
    
    private function searchForm() {
        
        $searchArr=(new Search)->content();
        
        foreach($searchArr as $value) {
            $out[]= " <option value='{$value['partialUrl']}'>{$value['searchText']}</option>";   
        }
        $str=implode($out);
        
    $html = <<<HTML
            
           
            
             <form class="navSearchForm form-inline ml-3">
      <div class="input-group input-group-sm">
            <select  style='width:300px' class="js-example-matcher-start form-control "   placeholder="Search" aria-label="Search" >
        $str
                        <options>!</options>
                        <options>Singapore</options>
                    </select>
        <div class="input-group-append">
          <button id='navSearchFormBtn' class="btn btn-navbar" type="submit">
            <i class="fas fa-search"></i>
          </button>
        </div>
      </div>
    </form>
            
               <script>    
            
       $( document ).ready(function() {       
           
            //when enter or mouse click is pressed on select
            $('.js-example-matcher-start').change(function(){
  var text = '{$GLOBALS['config']['HTTP']['URL']}'+ $(this).val();
             window.location.href = text;
});
            
            
   //when submit is clicked.
$('#navSearchFormBtn').change(function(){
            comsole.log('dsadas');
       var text =    $('.js-example-matcher-start').val();  
                window.location.href = '{$GLOBALS['config']['HTTP']['URL']}'+ text;

});
            
        $(".js-example-matcher-start").focus();     

            
           /* 
 // Populate search with data via ajax.
             const url = '{$GLOBALS['config']['HTTP']['HTML_API']}/10003/content/';
$.getJSON(url, function (data) {
  $.each(data, function (key, entry) {
            //console.log( entry );
   $( ".js-example-matcher-start" ).append($('<option></option>').attr('value', entry.url).text(entry.searchText));
  })
});    

   */
         
                    
                        
$(".js-example-matcher-start").select2({
 minimumInputLength: 2
});
            
            });  
            
             
               
  
       
             /* put keyboard focus on search box */
            $(".select2-selection__rendered").focus(); /* does not really work */
           
             
          
</script>
       
HTML;
    
    return $html;
    }
       
    private function searchFor2m() {
        
        
    $html = <<<HTML
            
            
      <!-- SEARCH FORM -->
    
            
            <form class="form-inline ml-3">
      <div class="input-group input-group-sm">
            <select id="mainSearchSelect2" class="form-control " id="mainSearchSelect"  placeholder="Search" aria-label="Search" >
         <options>Chinaxxxxxxxxxxxxxxxxxxxxxxx</options>
                        <options>Malaysia</options>
                        <options>Singapore</options>
                    </select>
        <div class="input-group-append">
          <button class="btn btn-navbar" type="submit">
            <i class="fas fa-search"></i>
          </button>
        </div>
      </div>
    </form>
           
   
            
            
             <script>       
            
            
            
            
            $( document ).ready(function() {
    console.log( "ready!" );
            
                   
            
            
            
            // Populate search with data via ajax.
             const url = '{$GLOBALS['config']['HTTP']['HTML_API']}/10003/content/';
$.getJSON(url, function (data) {
  $.each(data, function (key, entry) {
            //console.log( entry );
   $( "#mainSearchSelect2" ).append($('<option></option>').attr('value', entry.url).text(entry.searchText));
  })
});     
        /*
            $('#mainSearchSelect').change(function() {
    //Use $option (with the "$") to see that the variable is a jQuery object
    var option = $(this).find('option:selected');
    //Added with the EDIT
    var value = option.val();//to get content of "value" attrib
    var text = option.text();//to get <option>Text</option> content
            console.log(value);
});
          */  
          
            $('#navSearchForm').mousedown(function() { 
            console.log('mousedonw');
            var option = $(this).find('option:selected');
             var value = option.val();
             console.log('mouse down' + value);       
            
                }); 
            
            
            //overide submit and goto url when submitted. 
            //NOT WORKING
            $('#navSearchForm').on("submit", function(event) { 
  event.preventDefault(); // prevent the default event handler, which reloads the page
            console.log('FIRED');
            
            console.log($('#mainSearchSelect').val());
             console.log('FIRED');
            
  window.location.href = ($('#mainSearchSelect').val());
});
            
            
});
            
            
    
            
            
var s2 = $("#mainSearchSelect").select2({
    placeholder: "Choose SEARCH",
    tags: false
});   
      </script>
            
HTML;    
    
    return $html;
        
    }
    
    
    private function messagesDropdownMenu() {
        
        
         if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $sideBarInfo = new SideBarInfo(); 
            
      
        } 
      
        $content1=($GLOBALS['auth']->getOwnerAccess() == 1) ? $sideBarInfo->content(1) :'';
        $content2=($GLOBALS['auth']->getOwnerAccess() == 1) ? $sideBarInfo->content(2) :'';
        $content3=($GLOBALS['auth']->getOwnerAccess() == 1) ? $sideBarInfo->content(3) :'';
        $warningCount=($GLOBALS['auth']->getOwnerAccess() == 1) ? $sideBarInfo->warningCount :'';
        $dangerCount=($GLOBALS['auth']->getOwnerAccess() == 1) ? $sideBarInfo->dangerCount :'';
        
        
        
        
        
      $html = <<<HTML
              
         <!-- Messages Dropdown Menu -->
      <li class="nav-item dropdown">
        <a class="nav-link" data-toggle="dropdown" href="#">
          <i class="far fa-comments"></i>
          <span class="badge badge-danger navbar-badge">3</span>
        </a>
        <div class="dropdown-menu dropdown-menu-lg dropdown-menu-right">
          <a href="#" class="dropdown-item">
            <!-- Message Start -->
            <div class="media">
             
              <div class="media-body">
                <h3 class="dropdown-item-title">
                  Brad Diesel
                  <span class="float-right text-sm text-danger"><i class="fas fa-star"></i></span>
                </h3>
                <p class="text-sm">Call me whenever you can...</p>
                <p class="text-sm text-muted"><i class="far fa-clock mr-1"></i> 4 Hours Ago</p>
              </div>
            </div>
            <!-- Message End -->
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <!-- Message Start -->
            <div class="media">
           
              <div class="media-body">
                <h3 class="dropdown-item-title">
                  John Pierce
                  <span class="float-right text-sm text-muted"><i class="fas fa-star"></i></span>
                </h3>
                <p class="text-sm">I got your message bro</p>
                <p class="text-sm text-muted"><i class="far fa-clock mr-1"></i> 4 Hours Ago</p>
              </div>
            </div>
            <!-- Message End -->
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <!-- Message Start -->
            <div class="media">
           
              <div class="media-body">
                <h3 class="dropdown-item-title">
                  Nora Silvester
                  <span class="float-right text-sm text-warning"><i class="fas fa-star"></i></span>
                </h3>
                <p class="text-sm">The subject goes here</p>
                <p class="text-sm text-muted"><i class="far fa-clock mr-1"></i> 4 Hours Ago</p>
              </div>
            </div>
            <!-- Message End -->
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item dropdown-footer">See All Messages</a>
        </div>
      </li>
        
   HTML; 
      
        return $html;
    }
    
    
    
    private function notificationsDropdownMenu() {
        
  $html = <<<HTML
        
        <!-- Notifications Dropdown Menu -->
      <li class="nav-item dropdown">
        <a class="nav-link" data-toggle="dropdown" href="#">
          <i class="far fa-bell"></i>
          <span class="badge badge-warning navbar-badge">15</span>
        </a>
        <div class="dropdown-menu dropdown-menu-lg dropdown-menu-right">
          <span class="dropdown-item dropdown-header">15 Notifications</span>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <i class="fas fa-envelope mr-2"></i> 4 new messages
            <span class="float-right text-muted text-sm">3 mins</span>
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <i class="fas fa-users mr-2"></i> 8 friend requests
            <span class="float-right text-muted text-sm">12 hours</span>
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item">
            <i class="fas fa-file mr-2"></i> 3 new reports
            <span class="float-right text-muted text-sm">2 days</span>
          </a>
          <div class="dropdown-divider"></div>
          <a href="#" class="dropdown-item dropdown-footer">See All Notifications</a>
        </div>
      </li>
        
    HTML;    
        
       return $html; 
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
        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovines(false);
        $options = $this->createSelectOption($aniNumber);

        $html = <<<HTML
    <select  class="nav-link form-control" id="bovineSelect"  onchange= "javascript:location.href='?pageid={$GLOBALS['config']['static_page_location']['bovineQuery']}' + '&amp;bovine_id=' + this.options[this.selectedIndex].value;">
     $options
    </select>
HTML;
        return $html;
    }

    public function fields() {
        $aniNumber = (new CroppingHelper)->listOfCurrentFields();
        $options = $this->createSelectOption($aniNumber);

        $html = <<<HTML
  <select  class="form-control" id="fieldSelect"  onchange= "javascript:location.href='?pageid={$GLOBALS['config']['static_page_location']['fieldQuery']}' + '&amp;field_id=' + this.options[this.selectedIndex].value;">
     $options
    </select>
HTML;
        return $html;
    }
    
    public function machines() {
        $aniNumber =(new MachineQuery)->createListOfAllActiveMachines('default');
        $options = $this->createSelectOption($aniNumber);

        $html = <<<HTML
                 <select  class="form-control" id="machineSelect"  onchange= "javascript:location.href='?pageid={$GLOBALS['config']['static_page_location']['machineQuery']}' + '&amp;machine_id=' + this.options[this.selectedIndex].value;">
     $options
    </select>
HTML;
        return $html;
    }
}
//end class

/*
 * paramaters for admin:
 *      debug on/off
 *      admin mode on/off
 */
Class QF2_AdminParams extends QF2_Base {

    function definition($form) {
       
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        //not used?
        $html=<<<HTML
                    <div class="custom-control custom-switch custom-switch-off-danger custom-switch-on-success">
                      <input type="checkbox" class="custom-control-input" id="customSwitch3">
                      <label class="custom-control-label" for="customSwitch3">Toggle this custom switch element with custom colors danger/success</label>
                    </div>        
        HTML;
       
        $adminModeDefault=($GLOBALS['auth']->getAdminMode() === 1) ? 1 : 0;
        $debugModeDefault=($GLOBALS['auth']->getDebugMode() === 1) ? 1 : 0;
        
        $form->addElement('checkbox', 'adminMode', array('class'=>'','id' => 'boxId', 'value' => '1'))->setContent('Admin Mode');
        $form->addElement('checkbox', 'debugMode', array('class'=>'','id' => 'boxId', 'value' => '1'))->setContent('Debug Mode');
        
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('adminMode' => $adminModeDefault,'debugMode' => $debugModeDefault))); //set defaults

       
        return $form; //required       
    }

    function process($value) {
           
        $GLOBALS['auth']->setDebugMode($value['debugMode']);
        var_dump($value);
        
          if ($GLOBALS['auth']->getAdminMode() === 1) {
            $inputStr = filter_var($value['adminMode'], FILTER_SANITIZE_NUMBER_INT); //super check.
            if ($inputStr == 1) {
                $GLOBALS['auth']->setAdminMode(1);
            } else {
                $GLOBALS['auth']->setAdminMode(0);
            }
        }
    }

}//end class
