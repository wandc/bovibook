<?php 
require_once("global.php");
header('Content-Type: text/html; charset=utf-8'); //send with everything
/**
 * This acts as a controller for ajax requests and sends them
 *  to the designated file and makes an instance of the class and calls
 * ajax method.
 * from: http://codereview.stackexchange.com/questions/25987/best-way-to-structure-php-ajax-handlers
 *  called like this with mod_rewrite enabled in apache https://int.littleriver.ca/api/55/ajax/
 *    RewriteRule ^/api/(xml|json|html|jsonp)/([^/.]+)/([^/.]+)/?$ /ajax.php?ajaxtype=$1&pageid=$2&resource=$3 [QSA,L]
 *    RewriteRule ^/api/(xml|json|html|jsonp)/([^/.]+)/([^/.]+)/([^/.]+)/?$ /ajax.php?ajaxtype=$1&pageid=$2&resource=$3&item=$4 [QSA,L]
 */
if ((!isset($_GET['pageid'])) || (!is_numeric($_GET['pageid'])) || (!isset($_GET['resource']))) {
    header('400 Bad Request');
    exit;
}    

ini_set('memory_limit', '256M'); //needed for large charts of data.
try {

  
    //get url paramaters.
    $ajaxtype = $_GET['ajaxtype'];
    $pageid = $_GET['pageid'];
    $resource = $_GET['resource'];

   
    //Decided whether we have a direct ajax call to a function class or a call to a page class
    //use this range of numbers of sitePages
    if ( ($pageid >=1) AND ($pageid <=9999) ) {
         $result=pageCall($pageid,$resource);    
    }
    //use this range of numbers for ajax calls
    elseif  ( ($pageid >=10000) AND ($pageid <=19999) ) {      
        if (ajaxSecurity($pageid) == true) {
            $result = ajaxCall($pageid, $resource);
        } else {
            header('403 Forbidden');
            exit;
        }
    }
    else {
    header('400 Bad Request');
    exit;   
    }
     
    //bases on what type of file format was requested we return the resource.
        switch ($ajaxtype) {
        case 'xml':
            header("Content-type: text/xml");
            echo($result); //should be escaped somehow, maybe???
            break;
        case 'json':
            header('Content-Type: application/json; charset=utf-8');
            echo(json_encode($result, JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT));
            break;
        case 'html':
            //header("Content-Type: text/html"); //this causes problems for some reason. Default is this anyway, so don't use.
            echo(($result)); //should be escaped somehow?? htmlspecialchars disallows javascript from running in jquery, so maybe just pass it through.!!!
            break;
        case 'jsonp':
            header('content-type: application/json; charset=utf-8');
            echo(json_encode($result, JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT));
            break;
        default:
            throw new Exception('Unknown Ajax type!');
    }
    
} catch (Exception $e) {
    //print($e); //DEBUG Enable this to see error messages from ajax requests.
    header('404 Not Found');
    print("404 Not Found"."\n\r");
    exit;
}

function   ajaxCall($pageid,$resource) {
    
    $res = $GLOBALS['pdo']->query("Select ajax.filename,ajax.filesubdir FROM intWebsite.ajax WHERE ajax.id={$pageid} LIMIT 1");

$row = $res->fetch(PDO::FETCH_ASSOC);

//find file path
$returnArray['filePath']='/'. $row['filesubdir'] .'/'. $row['filename'];

//object name from file name
$objName=$row['filename'];
$objName = str_ireplace('.php', '', $objName);
$objName = str_ireplace('.inc', '', $objName);
$returnArray['objectName']=$objName; 

    //load 
    require_once(__DIR__ . '/' . $returnArray['filePath']);
    $controller = new $returnArray['objectName'];
    $result = $controller->$resource($_REQUEST); //replace ajax with resource.  
    return   $result;

   
}


//chooses which security method to use.
function getUserid() {
 
   if ($GLOBALS['auth_scheme'] == 'oauth2') {
        require_once($_SERVER['DOCUMENT_ROOT'] . '/auth/oauth2Security.inc');

        $auth = new Oauth2Security();
        $auth->main();
        $GLOBALS['auth'] = $auth;
        $userid = $auth->getUsername(); //grab userid
    } elseif ($GLOBALS['auth_scheme'] == 'oauth2Local') {
            
        include_once($_SERVER['DOCUMENT_ROOT'] . '/auth/oauth2Local.php');
   
   $auth = new Oauth2Local();
 
   $auth->main();
      $userid = $auth->getUsername(); //grab userid
   global $auth;

     
   
    } else {
        throw new Error("Unknown Auth Scheme.");
    }


    return $userid;
}



function ajaxSecurity($pageid) {
     $userid = getUserid();   
    
    $res = $GLOBALS['pdo']->query("Select ajax_id FROM intWebsite.ajax_security WHERE ajax_security.ajax_id=$pageid AND ajax_security.group = any(array(SELECT groupid FROM wcauthentication.users_in_groups WHERE userid='$userid')) LIMIT 1");  
   
    if ($res->rowCount() >= 1) {
        return true;
    }
    else {
        return false; 
    }
    
}

function   pageCall($pageid,$resource) {
    //do security to see if resource is allowed to be fetched.
    //////////
    $userid = getUserid();  
    require_once('auth/pageSecurityCheck.inc');
     $PageSecurityCheckInstance=new PageSecurityCheck();
    if ($PageSecurityCheckInstance->canUserSeePage($_REQUEST['pageid'], $userid) == false) {
        throw new Exception("Security failed, User:$userid not allowed to use resource!");
    }
    //////////
      
    //try to load php class 
    $ret = pageClassLookup($pageid);

    require_once("template/basePage.php"); //need an autoßloader..
    require_once(__DIR__ . '/' . $ret['filePath']);

    $controller = new $ret['objectName'];

    $result = $controller->$resource($_REQUEST); //replace ajax with resource.  
    return   $result;
}






/* FIXME this is very similar to the index page class, combine the two */
function pageClassLookup($pageid) {
    //find information about page clicked
$res = $GLOBALS['pdo']->query("Select page.filename,page.filesubdir FROM intWebsite.page WHERE page.pageid={$pageid} LIMIT 1");

$row = $res->fetch(PDO::FETCH_ASSOC);

//create a file path for the page from while loop above. SLOPPY!
$returnArray['filePath']='sitePages/'. $row['filesubdir'] .'/'. $row['filename'];

//now create the name of the main object to call
$objName=$row['filename'];
$objName = str_ireplace('.php', '', $objName);
$objName = str_ireplace('.inc', '', $objName);
$returnArray['objectName']=$objName;

return $returnArray;
}
?>