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

        header('Content-Type: text/html; charset=utf-8'); //set encoding for all pages.
        
        require_once('auth/pageSecurityCheck.inc');



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
     * 
     */


    function unsecurePage() {
        ob_start();

        print('<html>' . "\n");
        print('<head>' . '<!-- Insecure Page-->' . "\n");
        $this->pageHeadContent();
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
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        print('</body>' . "\n");
        print('</html>' . "\n");
        ob_end_flush();
    }

    /** ansi shadwo http://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow&t=SECURE
      ███████╗███████╗ ██████╗██╗   ██╗██████╗ ███████╗
      ██╔════╝██╔════╝██╔════╝██║   ██║██╔══██╗██╔════╝
      ███████╗█████╗  ██║     ██║   ██║██████╔╝█████╗
      ╚════██║██╔══╝  ██║     ██║   ██║██╔══██╗██╔══╝
      ███████║███████╗╚██████╗╚██████╔╝██║  ██║███████╗
      ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
     */
    function securePage() {
        ob_start(); //this seems to be needed for security logout to work???
        //ob_Start, should be Turned off, apache2 uses defalte to compress pages.


        print('<html>' . "\n");
        print('<head>' . "\n");
        self::pageCSS();
        $this->allPageJavascript();
        $this->pageHeadJavascript(); //from page class
        $this->pageHeadContent(); //from page class
        $this->allPageHeadContent();
        print('<title>LR - ' . self::getPageTitleStr() . '</title>' . "\n");

        $this->customCSS();
        print('</head>' . "\n");
        //flush(); //suppose to be faster: http://developer.yahoo.com/performance/rules.html#expires



        print('<body class="hold-transition skin-purple  sidebar-mini">' . "\n");

        print('<div class="wrapper">' . "\n");


        /** Header */
        include_once('template/header.inc');
        $header = new Header();
        $header->headerContent();
        ///////////////////////////

        /** Side bar * */
        include_once('template/sideBar.inc');
        $sideBar = new SideBar();
        $sideBar->sideBarContent();
        ?>
        <!-- Content Wrapper. Contains page content -->
        <div class="content-wrapper">
            <!-- Content Header (Page header) -->
            <section class="content-header">
                <h1>
        <?php print(self::getPageTitleStr()); ?>
               <!--   <small>Optional description</small> -->
                </h1>
                <ol class="breadcrumb">
                    <li><a href="#"><i class="fa fa-dashboard"></i> Level</a></li>
                    <li class="active">Here</li>
                </ol>
            </section>

            <!-- Main content -->
            <section class="content">
        <?php
        $this->customBody();
        $this->defaultDisplay();
        ?>
            </section>
            <!-- /.content -->
        </div>
        <!-- /.content-wrapper -->



        <?php
       include_once('template/footer.inc');
        $footer = new Footer;
        $footer->footerContent();


        /** Side bar * */
        //goes after footer
        include_once('template/sideBarRight.inc');
        $sideBar = new SideBarRight();
        $sideBar->sideBarRightContent();

        print('
        </div>
<!-- ./wrapper -->
        ');


        $this->pageJavascript();
        $this->customJavascript();
        ?>


        <!-- AdminLTE App -->
        <script src="<?php echo($GLOBALS['config']['ADMINLTE']['path']); ?>dist/js/adminlte.min.js"></script>

        <!-- Optionally, you can add Slimscroll and FastClick plugins.
             Both of these plugins are recommended to enhance the
             user experience. Slimscroll is required when using the
             fixed layout. -->
        </body>
        </html>

        <?php
        ob_end_flush();
    }

    //basic secure page, no nav etc., used for one page apps, ie TMR MIXER.
    function basicSecurePage() {
        ob_start(); //this seems to be needed for security logout to work???
        //ob_Start, should be Turned off, apache2 uses defalte to compress pages.


        print('<html>' . "\n");

        print('<head>' . "\n");
        self::pageCSS();
        $this->allPageJavascript();
        $this->pageHeadJavascript();
        $this->pageHeadContent();
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
        $this->customJavascript();


        print('</html>' . "\n");
        ob_end_flush();
    }

    function pageHeadContent() {
        
    }

    function allPageHeadContent() {
        ?>


        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">

        <!-- Tell the browser to be responsive to screen width -->
        <meta content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no" name="viewport">



        <?php
    }

    static function pageCSS() {

        print(cssLinkGenerate('/vendor/twbs/bootstrap/dist/css/bootstrap.min.css', 'all'));
         //DO NOT ENABLE BOOTSTRAP THEME. ADMIN LTE IS THEME.
        
        //jquery
        print(cssLinkGenerate('/vendor/components/jqueryui/themes/blitzer/jquery-ui.css', 'all')); //modifed blitzer theme to match bootstrap.
        print(cssLinkGenerate('/css/jquery-ui.theme.css', 'all')); //modifed blitzer theme to match bootstrap.
        //jquery datatables
        print(cssLinkGenerate('/vendor/datatables/datatables/media/css/jquery.dataTables.css', 'all'));
        print(cssLinkGenerate('/jscript/select.dataTables.min.css', 'all')); //for tmr recipe new page.
        
        //select2 needs
        print(cssLinkGenerate('/vendor/select2/select2/dist/css/select2.min.css', 'all')); // <!-- Select2 CSS -->
        
        //adminLTE Needed CSS
        print(cssLinkGenerate('/vendor/fortawesome/font-awesome/css/font-awesome.min.css', 'all')); //  <!-- Font Awesome -->
        print(cssLinkGenerate('/vendor/driftyco/ionicons/css/ionicons.min.css', 'all')); // <!-- Ionicons -->
        

        print(cssLinkGenerate($GLOBALS['config']['ADMINLTE']['path'] . 'dist/css/AdminLTE.min.css', 'all'));
        print(cssLinkGenerate($GLOBALS['config']['ADMINLTE']['path'] . 'dist/css/skins/skin-purple.min.css', 'all'));

        //legacy
        print(cssLinkGenerate('/css/quickformTableless.css', 'all'));

        print(cssLinkGenerate('/css/header.css', 'all'));
        print(cssLinkGenerate('/css/generic.css', 'all'));
        print(cssLinkGenerate('/css/bovineQuery.css', 'all'));
        print(cssLinkGenerate('/css/fonts.css', 'all'));
        print(cssLinkGenerate('/css/print.css', 'print'));
    }

    /*
     * Detect if we have an iPhone or iPod touch. returns true if so.
     */

    public static function iPhoneDetection() {
        if ((strpos($_SERVER['HTTP_USER_AGENT'], "iPhone") == true ) || (strpos($_SERVER['HTTP_USER_AGENT'], "iPod") == true )) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    /*
     * Detect if we have an iPad. returns true if so.
     */

    public static function iPadDetection() {
        if (strpos($_SERVER['HTTP_USER_AGENT'], "iPad") == true) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    private function allPageJavascript() {
       
      print('<script type="text/javascript" src="vendor/components/jquery/jquery.min.js"></script>' . "\n");
      print('<script type="text/javascript" src="vendor/components/jqueryui/jquery-ui.min.js"></script>' . "\n"); //themes are here to with css
      print('<script type="text/javascript" src="vendor/datatables/datatables/media/js/jquery.dataTables.min.js"></script>' . "\n"); 
      print('<script type="text/javascript" src="vendor/twbs/bootstrap/dist/js/bootstrap.min.js"></script>' . "\n");
       
        //NOTE: adminLTE is loaded in header. 
        print('<script type="text/javascript" src="jscript/dataTables.select.min.js"</script>' . "\n"); //used on TMR recipe new page.
        print('<script type="text/javascript" src="vendor/drmonty/datatables-plugins/sorting/num-html.js"></script>' . "\n"); //datatable plugin for sorting this type, add other types here as necessary.
        print('<script type="text/javascript" src="vendor/moment/moment/min/moment.min.js"></script>' . "\n"); //datatable needs moment for moment plugin for dates.
        print('<script type="text/javascript" src="vendor/drmonty/datatables-plugins/sorting/datetime-moment.js"></script>' . "\n"); //datatable plugin for sorting this type, add other types here as necessary.    
        print('<script type="text/javascript" src="vendor/drmonty/datatables-responsive/js/dataTables.responsive.min.js"></script>' . "\n"); //responsive datatables, switch this too botrstrap responsive.bootstrap.min.js if we move to bootsreap tables.    
        print('<script type="text/javascript" src="vendor/select2/select2/dist/js/select2.min.js"></script>' . "\n"); //<!- no idea if this is used, could be for admin LTE fancy select buttons? ->
        //load for all pages, because it is easier
        include_once('functions/googleVisualization.inc');
        print(GoogleVisGlobal::loadVisualization());

     
    }

    function idleTimer() {
        require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/idleTimer.inc');
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
?>