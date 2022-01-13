<?php

/**
 * Base Page class that all other classes that want to display something should extend.
 * Dec 1, 2008
 *
 */
abstract Class BasePage {

    //PATH TO ADMIN LTE

    public $pageid;

    // public $app;  //slim app.
    //api and main act as constructors.
    public function __construct() {

        if (php_sapi_name() == 'cli') {  //if called directly from command line, pageid doesn't really mean anything.
            $this->pageid = -1;
        }
        //do nothing else
    }

    //renader normal page flow.
    function main($pageid) {

        $this->pageid = $pageid; //constructor portion

        //header('Content-Type: text/html; charset=utf-8'); //set encoding for all pages.

        $PageSecurityCheckInstance = new PageSecurityCheck();

        $userid = $GLOBALS['auth']->getUsername();


        /* TAB PAGE */
        //check if this is a callback to the same page, 
        // ie a tab wants to load content, but not the whole sidebar, etc. 
        if ((isset($_REQUEST['callback']) ) && ($_REQUEST['callback'] == 1)) {

            //VERY IMPORTANT
            //HACK: rewrite the url (from php's point of view) to take out callback variable
            // There must be a more correct way to do this. 
            // We need to do this so that child classes don't put the callback variable in url's 
            // for form submits and table paging, or it will just draw the form without the rest
            // of the page.
            $_SERVER ["REQUEST_URI"] = str_replace('&callback=1', '', $_SERVER ["REQUEST_URI"]);

            if ($GLOBALS['auth_scheme'] == 'oauth2') {
                Oauth2Security::checkAuth();
            } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
                Oauth2Local::checkAuth();
            } else {
                throw new Exception("Error:No Auth scheme selected.");
            }

            $this->callBack(); //calls child method to actually deal with callback.
        }
        //this is an ajax callback, take appropriate action in child class.
        else {  //normal page  
            /* NORMAL PAGE */
            if ($GLOBALS['auth_scheme'] == 'oauth2') {
                Oauth2Security::checkAuth();
            } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
                Oauth2Local::checkAuth();
            } else {
                throw new Exception("Error:No Auth scheme selected.");
            }



            //user is not allowed to see the page.
            if (($PageSecurityCheckInstance->canUserSeePage($_REQUEST['pageid'], $userid) == false) AND ( $PageSecurityCheckInstance->isUserInAnyGroups($userid) == true)) {

                if ($GLOBALS['auth_scheme'] == 'oauth2') {
                    Oauth2Security::loginButton();
                } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
                    Oauth2Local::checkAuth();
                } else {
                    throw new Exception("Error:No Auth scheme selected.");
                }
            } elseif (($GLOBALS['auth']->checkAuth()) AND ( $PageSecurityCheckInstance->isUserInAnyGroups($userid) == false)) {
                print('ERROR: User is valid, but not in any security groups. Add ?reset=1 to URL to logout.');
            } elseif ($GLOBALS['auth']->checkAuth()) {
                //sometimes for tabs we want to skip the whole page load and go right to defaultDisplay.
                if ((isset($_REQUEST['callback']) ) && ($_REQUEST['skipptocore'] == true)) {
                    $this->defaultDisplay();
                } elseif (in_array('secure_basic', $GLOBALS['auth']->getAuthData('groups')) == true) {
                    $this->basicSecurePage();
                } else {
                    $this->securePage();
                }
            } else {  //everyone can see
                if ($GLOBALS['auth_scheme'] == 'oauth2') {
                    Oauth2Security::loginButton();
                } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
                    Oauth2Local::checkAuth();
                } else {
                    throw new Exception("Error:No Auth scheme selected.");
                }
            }
        }//end callback else
        //FIXME: should have automatic logout here, javascript doesn't work as it just keeps looping, need callback, too hackish.
    }

//end function

    /*
     * There are 3 types of pages that the site serves (plus ajax). secure, unsecure, and securebasic
     * securebasic is used by the TMR mixer, has no nav, etc. The user is aiuthenticated though. This should be locked to a single device and sing auth group, so they can see no other pages. 


      ██╗   ██╗███╗   ██╗███████╗███████╗ ██████╗██╗   ██╗██████╗ ███████╗
      ██║   ██║████╗  ██║██╔════╝██╔════╝██╔════╝██║   ██║██╔══██╗██╔════╝
      ██║   ██║██╔██╗ ██║███████╗█████╗  ██║     ██║   ██║██████╔╝█████╗
      ██║   ██║██║╚██╗██║╚════██║██╔══╝  ██║     ██║   ██║██╔══██╗██╔══╝
      ╚██████╔╝██║ ╚████║███████║███████╗╚██████╗╚██████╔╝██║  ██║███████╗
      ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
     */
    function unsecurePage() {
        ob_start();

        print('<html>' . "\n");
        print('<head>' . '<!-- Insecure Page-->' . "\n");

        print('<title>Insecure Page</title>' . "\n");
        self::pageCSS();
        $this->customCSS();
        print('</head>' . "\n");
        print('<body>' . "\n");

        //print('<div class="globalTopContainer">'."\n");
        print("       <a href=\"{$_SERVER['PHP_SELF']}?pageid=1\"></a>"); //clickable logo
        security::doSecurity2();
        //if we are now logged in we have to get back to the secure page.
        if ($GLOBALS['auth']->checkAuth()) {
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        print('</body>' . "\n");
        print('</html>' . "\n");
        ob_end_flush();
    }

    /** ansi shadow http://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow&t=SECURE
      ███████╗███████╗ ██████╗██╗   ██╗██████╗ ███████╗
      ██╔════╝██╔════╝██╔════╝██║   ██║██╔══██╗██╔════╝
      ███████╗█████╗  ██║     ██║   ██║██████╔╝█████╗
      ╚════██║██╔══╝  ██║     ██║   ██║██╔══██╗██╔══╝
      ███████║███████╗╚██████╗╚██████╔╝██║  ██║███████╗
      ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
     */
    function securePage() {
       ob_start();
        /*
        IMPORTANT
        ob_Start buffers the output. if multiple headers are passed it takes care to put them together. 
        If this isn't used headers used to forward in form submits don't work. 
        A way to get around this and possibly speed things up, is to handle forms at the top of pages before everything else. 
       */
        
        $allPageHeadContent=$this->allPageHeadContent(); 

        $contentHeader=(new Header)->headerContent();
        $sideBar = (new SideBar())->sideBarContent();
        $pageLevel=self::getPageLevel();
        $pageTitle=self::getPageTitleStr();       
        $footer = (new Footer)->footerContent();

        
        /*
         * Using three heredocs so that we can still output 
         */
        
        
$securePageTop = <<<HTML
<html>
            <head>
                    {$this->pageCSS()}
                    {$this->allPageJavascript()}
                    {$this->pageHeadJavascript()}                   
                    {$allPageHeadContent}
                    <title>LR - {$pageTitle} </title>
                    {$this->customCSS()}
            </head>
            <body class="sidebar-mini layout-fixed layout-navbar-fixed">   
                <div class="wrapper">
                  
                    <div class="content-wrapper" style="min-height: 733px;">
                        <div class="content-header">
                          
                            <div class="container-fluid">
                    
                    {$contentHeader}
                    {$sideBar}
                    
                                <div class="row mb-2">
                                    <div class="col-sm-6">
                                        <h1 class="m-0 text-dark">
                                           $pageTitle
                                    
                                        </h1>
                                    </div>
                                    <div class="col-sm-6">
                                      <ol class="breadcrumb float-sm-right">
                                            <li class="breadcrumb-item"><a href="#">$pageTitle</a></li>
                                            <li class="breadcrumb-item active">$pageLevel</li>
                                      </ol>
                                    </div>
                                </div>
                            </div>
                        </div>    
                                         <!-- PAGE CONTENT GOES HERE -->
                        <div class="mainContainer">    
HTML;                     
                    
$securePageMiddle = <<<HTML
                        </div>
                           
                            
                      {$footer}             
                    </div>
                   
                     
                </div>
                <!-- /.content-wrapper -->    
                
                                   
                {$this->pageJavascript()}
                {$this->customJavascript()}
 HTML;   
 $securePageBottom = <<<HTML
                 <script src="/vendor/almasaeed2010/adminlte/dist/js/adminlte.min.js"></script>  <!-- AdminLTE App -->
            </body>
</html>                   
HTML;

    
    //has to be written this way to allow tag output....
   // maybe it is not worth supporting. 
    print($securePageTop);
     $this->customBody();
     $this->defaultDisplay();
     print($securePageMiddle . $securePageBottom);   
    }

    //basic secure page, no nav etc., used for one page apps, ie TMR MIXER.

    /**
      ██████╗  █████╗ ███████╗██╗ ██████╗
      ██╔══██╗██╔══██╗██╔════╝██║██╔════╝
      ██████╔╝███████║███████╗██║██║
      ██╔══██╗██╔══██║╚════██║██║██║
      ██████╔╝██║  ██║███████║██║╚██████╗
      ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝ ╚═════╝
     */
    function basicSecurePage() {
        ob_start(); //neaded if header forwarding is used. 


        print('<html>' . "\n");

        print('<head>' . "\n");
        self::pageCSS();
        $this->allPageJavascript();
        $this->pageHeadJavascript();

        print('<title>LR - ' . self::getPageTitleStr() . '</title>' . "\n");

        //$this->customCSS();
        print('</head>' . "\n");
        //flush(); //suppose to be faster: http://developer.yahoo.com/performance/rules.html#expires
        print('<body>' . "\n");
        print('<div class="bodyMost">' . "\n");

        ///////////////////////////

        print('<div class="allContainer">' . "\n");

        ///////////////////////////
        print('<div class="mainContainer">' . "\n");
        $this->customBody();
        $this->defaultDisplay();  //for compatibility for pre dec 20 2008 way of doing things.
        print('</div>' . "\n");
        print('</div> <!-- End AllContaier -->' . "\n");
        /** Footer */
        print('</div> <!--/bodyMost-->' . "\n");

        print('</body>' . "\n");
        //place at end for faster responsiveness.
        //some code might need to be placed in head???

        $this->pageJavascript();
        print($this->customJavascript());


        print('</html>' . "\n");
        //ob_end_flush();
    }

    function allPageHeadContent() {
         $html = <<<HTML
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
             <!-- Tell the browser to be responsive to screen width -->
        <meta content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no" name="viewport">
HTML;
         
         return $html;
    }

    static function pageCSS() {

        $out[]=(cssLinkGenerate('/vendor/twbs/bootstrap/dist/css/bootstrap.min.css', 'all'));
         //DO NOT ENABLE BOOTSTRAP THEME. ADMIN LTE IS THEME.
        
        //jquery
        $out[]=(cssLinkGenerate('/vendor/components/jqueryui/themes/blitzer/jquery-ui.css', 'all')); //modifed blitzer theme to match bootstrap.
        $out[]=(cssLinkGenerate('/css/jquery-ui.theme.css', 'all')); //modifed blitzer theme to match bootstrap.
        //jquery datatables
        
        $out[]=(cssLinkGenerate('/vendor/peekleon/datatables-all/media/css/jquery.dataTables.css', 'all'));
        $out[]=(cssLinkGenerate('/vendor/peekleon/datatables-all/media/css/dataTables.bootstrap4.min.css', 'all'));
             // $out[]=(cssLinkGenerate('/vendor/peekleon/datatables-all/media/css/responsive.bootstrap4.min.css', 'all'));
             // $out[]=(cssLinkGenerate('/vendor/peekleon/datatables-all/media/css/buttons.bootstrap4.min.css', 'all'));

        
        $out[]=(cssLinkGenerate('/jscript/select.dataTables.min.css', 'all')); //for tmr recipe new page.
        //datatable buttons   
        // $out[]=('<script type="text/css" src="/vendor/peekleon/datatables-all/extensions/Buttons/css/buttons.dataTables.min.css"></script>' . "\n");
        // $out[]=('<script type="text/css" src="/vendor/peekleon/datatables-all/extensions/Buttons/css/buttons.bootstrap.min.css"></script>' . "\n");
        
        //select2 needs
        $out[]=(cssLinkGenerate('/vendor/select2/select2/dist/css/select2.min.css', 'all')); // <!-- Select2 CSS -->
        
        //adminLTE 3 Needed CSS
        $out[]=(cssLinkGenerate('/vendor/components/font-awesome/css/all.min.css','all'));  //Font Awesome Icons 
        $out[]=(cssLinkGenerate('/vendor/driftyco/ionicons/css/ionicons.min.css', 'all')); // <!-- Ionicons -->
        
        //must be added this way because it includes child style sheets with print media, etc. 
        $out[]='<link rel="stylesheet" href="/vendor/almasaeed2010/adminlte/dist/css/adminlte.min.css">'; //theme syle adminelte 3
        
        $out[]=('<script type="text/css" src="https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,400i,700"></script>' . "\n"); //adminlte3 fonts
        
        //legacy
        $out[]=(cssLinkGenerate('/css/quickformTableless.css', 'all'));

        $out[]=(cssLinkGenerate('/css/header.css', 'all'));
        $out[]=(cssLinkGenerate('/css/generic.css', 'all'));
        $out[]=(cssLinkGenerate('/css/bovineQuery.css', 'all'));
        $out[]=(cssLinkGenerate('/css/fonts.css', 'all'));
        $out[]=(cssLinkGenerate('/css/print.css', 'print'));
        
        return implode($out);
    }

  
   

    private function allPageJavascript() {
       
      $out[]=('<script type="text/javascript" src="/vendor/components/jquery/jquery.min.js"></script>' . "\n");
      $out[]=('<script type="text/javascript" src="/vendor/components/jqueryui/jquery-ui.min.js"></script>' . "\n"); //themes are here to with css
      $out[]=('<script type="text/javascript" src="/vendor/peekleon/datatables-all/media/js/jquery.dataTables.min.js"></script>' . "\n"); 
      
       $out[]=('<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.11.0/umd/popper.min.js"></script>' . "\n");  //need for tooltips.
      
      $out[]=('<script type="text/javascript" src="/vendor/twbs/bootstrap/dist/js/bootstrap.min.js"></script>' . "\n");
       
        //NOTE: adminLTE is loaded in header. 
      
      
        $out[]=('<script type="text/javascript" src="/vendor/peekleon/datatables-all/extensions/Buttons/js/buttons.print.min.js"</script>' . "\n"); //print button
        $out[]=('<script type="text/javascript" src="/vendor/peekleon/datatables-all/extensions/Buttons/js/buttons.bootstrap4.min.js"</script>' . "\n"); //bootstrap 4 buttons.
       
        $out[]=('<script type="text/javascript" src="/vendor/peekleon/datatables-all/extensions/Buttons/js/dataTables.buttons.min.js"</script>' . "\n"); //CSV button
        $out[]=('<script type="text/javascript" src="/vendor/stuk/jszip/dist/jszip.min.js"</script>' . "\n"); //CSV button 
        $out[]=('<script type="text/javascript" src="/vendor/peekleon/datatables-all/extensions/Buttons/js/buttons.html5.min.js"</script>' . "\n"); //CSV button 
              
        $out[]=('<script type="text/javascript" src="/jscript/dataTables.select.min.js"</script>' . "\n"); //used on TMR recipe new page.
        $out[]=('<script type="text/javascript" src="/vendor/drmonty/datatables-plugins/sorting/num-html.js"></script>' . "\n"); //datatable plugin for sorting this type, add other types here as necessary.
        $out[]=('<script type="text/javascript" src="/vendor/moment/moment/min/moment.min.js"></script>' . "\n"); //datatable needs moment for moment plugin for dates.
        $out[]=('<script type="text/javascript" src="/vendor/drmonty/datatables-plugins/sorting/datetime-moment.js"></script>' . "\n"); //datatable plugin for sorting this type, add other types here as necessary.    
        $out[]=('<script type="text/javascript" src="/vendor/drmonty/datatables-responsive/js/dataTables.responsive.min.js"></script>' . "\n"); //responsive datatables, switch this too botrstrap responsive.bootstrap.min.js if we move to bootsreap tables.    
        $out[]=('<script type="text/javascript" src="/vendor/select2/select2/dist/js/select2.min.js"></script>' . "\n"); //<!- no idea if this is used, could be for admin LTE fancy select buttons? ->
        
       //load charts for all pages, because it is easier
       $out[]=(GoogleVisualization::loadVisualization());
       
        return implode($out);
    }

    function idleTimer() {
     
        IdleTimer::main();
    }

    /* pageid can be passed (ajax) or read directly with request */

    static public function getPageTitleStr($pageid = null) {
        if ($pageid != null) {
            $pageidreal = $pageid;
        } elseif ($_REQUEST['pageid'] != null) {
            $pageidreal = $_REQUEST['pageid'];
        }

        $sql = "Select page.title FROM intWebsite.page WHERE page.pageid=$pageidreal LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        return $row['title'];
    }

       static public function getPageLevel($pageid = null) {
        if ($pageid != null) {
            $pageidreal = $pageid;
        } elseif ($_REQUEST['pageid'] != null) {
            $pageidreal = $_REQUEST['pageid'];
        }

        $sql="SELECT title from intwebsite.page where pageid=(SELECT parent_id from intwebsite.page where pageid=$pageidreal) limit 1";
        
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);     
        return empty($row) ? '' : $row['title'];
    }
    
    
    
    /*
      Override in child class.
     */

    function pageJavascript() {
        
    }

    function customBody() {
        
    }

    function defaultDisplay() {
        
    }

    function customJavascript() {
        
    }

    function pageHeadJavascript() {
        
    }

    function customCSS() {
        
    }

    function callBack() {
        
    }

    function api() {
        
    }

    function error() {
        
    }

    protected function customSQL() {
        
    }

}
//end class