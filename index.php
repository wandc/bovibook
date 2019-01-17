<?php

//namespace BoviBook;

require_once("global.php"); 

error_reporting(E_ALL ^ E_DEPRECATED ^ E_WARNING ^ E_NOTICE);



//auth scheme set in globals file, so that ajax.php and other ways into the
//site know which scheme we are using.
//start oauth2
if ($GLOBALS['auth_scheme'] == 'oauth2') {

    $auth = new Oauth2Security();
    $auth->main();
    global $auth;
} elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
    $auth = new Oauth2Local();
    $auth->main();
    global $auth;
}
//start 
else {
    throw new Exception("No auth_scheme chosen, ie oauth2.");
}

/**
 * This class is the start of everything. It looks up a pages content and class name by its pageid number. 
 * When it finds it, it creates the the page content class dynamically. 
 * The page content class extends a page template class which holds all the boiler plate html.
  D Waddy Dec 30, 2008. updated March 2016.
 * */
Class Index {

    public $pageid;
    public $app;

    public function __construct() {



        $xxx = $this;


        //get pageid
        if ((isset($_REQUEST['pageid']) == false) || ($_REQUEST['pageid'] == null)) {
            $xxx->pageid = 1; //goto home page.
            $_REQUEST['pageid'] = 1; //just to be safe.
            //now call function to load page object
            $xxx->callPageObject($xxx->pageid);
        } elseif (!filter_var($_REQUEST['pageid'], FILTER_VALIDATE_INT) === false) {
            $xxx->pageid = $_REQUEST['pageid'];
            //now call function to load page object
            $xxx->callPageObject($xxx->pageid);
        } else {
            throw new Exception('Error: Page id is request variable is not an interger.');
        }
    }

    //factory
    function callPageObject($pageid, $app = null) {
        $filePathArray = $this->getPageFilePath($pageid);

        if ($filePathArray != null) {
          //  require_once($filePathArray['filePath']); //load the page code
            //now create the object based on the file name, instantiate dynamically.
            $classname = $filePathArray['objectName'];
            $mainObj = new $classname(); //autoload object
            if (!empty($app)) {
                $mainObj->mainApi($pageid, $this->app); //call basepage api method
            } else {
                $mainObj->main($pageid);     //call basepage pageid main method
            }
        } else { //pageid is not valid.
            http_response_code(404);

            $html = <<<HTML
  <div class="error-page">
        <h2 class="headline text-yellow"> 404</h2>

        <div class="error-content">
          <h3><i class="fa fa-warning text-yellow"></i> Oops! Page not found.</h3>

          <p>
            We could not find the page you were looking for.
            Meanwhile, you may <a href="../../index.php">return to dashboard</a>.
          </p>
        </div>
           </div>
        
HTML;
            print($html);
        }
    }

//get the file path for the page.
    function getPageFilePath($pageid) {
        if ($pageid == null) {
            throw new Exception('Error no pageid to lookup file name.');
        }
        $returnArray = array();
//find information about page clicked
        $res = $GLOBALS['pdo']->query("Select trim(page.filename) as filename,trim(page.filesubdir) as filesubdir FROM intWebsite.page WHERE page.pageid={$pageid} AND active=true LIMIT 1");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

//create a file path for the page from while loop above. SLOPPY!
            $filePath = 'sitePages/' . $row['filesubdir'] . '/' . $row['filename'];
            $returnArray['filePath'] = 'sitePages/' . $row['filesubdir'] . '/' . $row['filename'];

//now create the name of the main object to call
            $objName = $row['filename'];
            $objName = str_ireplace('.php', '', $objName);
            $objName = str_ireplace('.inc', '', $objName);
            $returnArray['objectName'] = $objName;
        }
        return $returnArray;
    }

}//end class

new Index(); //the start of it all!!!
?>