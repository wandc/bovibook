<?php 
/**
 * All Classes use this code...
 * 
 */

$iniFile = 'local/config/config.ini'; //load custom config


require_once('HTML/QuickForm.php'); //one off loader for old deprecated pear class. remove forms then delete this. FIXME
require_once 'HTML/QuickForm/Renderer/Tableless.php'; //one off loader for old deprecated pear class. remove forms then delete this. FIXME
new IntAutoLoader(); //bovibook loader
require_once('vendor/autoload.php'); // autoloads all composer files.

/*
 * Initiate Misc Class and make global
 * Used for helper functions across the site.
 */

if (defined('STDIN')) { //when called from cli, command line
  require_once(dirname(__FILE__).'/functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
    include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
}
$GLOBALS['MiscObj']= new Misc();

////
try {
$GLOBALS['config']= parse_ini_file($iniFile, true);
} catch (Exception $e) {
	die('Missing INI file: ' . $iniFile);
}

//date time
date_default_timezone_set($GLOBALS['config']['timezone']['default']);

// CONNECTS TO DB USING PHP PDO
 try {
    $pdoDsn = ($GLOBALS['config']['PDO']['dsnMain']); //local socket
    $pdo = new PDO($pdoDsn);
    //$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES,false); //is this slow? no idea
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); //DEBUG:: turns on DB debugging site wide.
    $GLOBALS['pdo']=$pdo; //set db handle global
} catch (PDOException $Exception) {
    print("Exception: ".$Exception->getMessage());
    throw new PDOException($Exception->getMessage(), $Exception->getCode());
}



//DEBUG: turn this on to see all sql queries.
//Explain_Queries::main();

   /*
    * CHOOSE AUTH SCHEME
    * 
    */
//choose which authentication to use by IP address.
//home mac

if (php_sapi_name() != 'cli') {
//home mac    
if ($_SERVER['REMOTE_ADDR'] == '192.168.8.51') {
   $GLOBALS['auth_scheme']='oauth2Local'; 
  //$GLOBALS['auth_scheme']='oauth2'; 
}
//work mac
elseif ($_SERVER['REMOTE_ADDR'] == '192.168.8.28') {
  $GLOBALS['auth_scheme']='oauth2Local'; 
}
//fred pc
elseif ($_SERVER['REMOTE_ADDR'] == '192.168.8.24') {
   $GLOBALS['auth_scheme']='oauth2Local'; 
}
else {
   $GLOBALS['auth_scheme']='oauth2Local'; 
}
}
//CLI
else {
    // no autentication used for cli scripts
}



//from: http://stackoverflow.com/questions/9557374/custom-autoload-vs-default
//supports camelcase classes, default doesn't.
//put in all the directories to look to load the classes needed. 
class IntAutoLoader {

    private $_directoriesToLook;

    
    private function customAutoloader($class_name) {
      
        //already loaded, check first
           if (class_exists($class_name)) {
            return true;
           }

        foreach ($this->_directoriesToLook as $directory) {
            $files = scandir($directory);
           // print_r($files);
           //print_r2($class_name);
             //lcfirst for camelcase file names, vs capitialized class names. 
            if (in_array(lcfirst($class_name) . ".php", $files)) {
                require($directory . lcfirst($class_name) . ".php");
                return true;
            }
            elseif (in_array(lcfirst($class_name) . ".inc", $files)) {
                require($directory . lcfirst($class_name) . ".inc");
                return true;
            }
        }
        
        return false;
    }

    function __construct() {
                        
        $this->_directoriesToLook= preg_filter('/^/', $_SERVER['DOCUMENT_ROOT'], array("auth/","sitePages/bovineManagement/",  "sitePages/cropping/",  "sitePages/heifer/",  "sitePages/hr/",  "sitePages/machinery/",  "sitePages/medical/",  "sitePages/misc/",  "sitePages/nutrition/",  "sitePages/parlor/",  "sitePages/reports/",  "sitePages/reproduction/",  "sitePages/structure/", "sitePages/transition/", "sitePages/", "functions/", "template/", "phpCronScripts/"));
       // echo(get_include_path() . PATH_SEPARATOR . implode(PATH_SEPARATOR, $this->_directoriesToLook));
     // print_r($this->_directoriesToLook);
       $a= set_include_path(get_include_path() . PATH_SEPARATOR . implode(PATH_SEPARATOR, $this->_directoriesToLook));
      //print_r(get_include_path());
        spl_autoload_extensions(".php, .inc");
        //spl_autoload_register();
         spl_autoload_register(array($this, "customAutoloader"));
       // print_r(get_declared_classes());
    }

}

?>