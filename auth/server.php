<?php 
// error reporting (this is a demo, after all!)
ini_set('display_errors',1);error_reporting(E_ALL);

if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
}

// Autoloading (composer is preferred, but for this example let's just do this)
require_once('/var/www/int/vendor/bshaffer/oauth2-server-php/src/OAuth2/Autoloader.php');
OAuth2\Autoloader::register();





// $dsn is the Data Source Name for your database, for exmaple "mysql:dbname=my_oauth2_db;host=localhost"
//we use user c3po because we needed to set DEFAULT schema to wcauthentication, framework does not support schemas directly.
$storage = new OAuth2\Storage\Pdo($GLOBALS['config']['PDO']['dsnAuthServerLocal']);


//if we are setting a mobile device, allow extra long tokens.
if ((isset($_REQUEST['longtoken'])) AND $_REQUEST['longtoken']==1) {
   $refresh_token_lifetime= 157784630; /*5 years */
}else {
    $refresh_token_lifetime= 1209600; /*14 days */ 
}


// Pass a storage object or array of storage objects to the OAuth2 server class
// create the server, and configure it to allow implicit
$config=array(
    'allow_implicit' => true,
    'access_token_lifetime'         => 120900, 
    'refresh_token_lifetime'         => $refresh_token_lifetime,
     'always_issue_new_refresh_token' => true,  /* default is false */
);

/**
 // create some users in memory
    $users = array('bshaffer' => array('password' => 'brent123', 'first_name' => 'Brent', 'last_name' => 'Shaffer'));

    // HERE IS THE NEW PART
    $clients = array('TestClient' => array('client_secret' => 'TestSecret'));

    // create a storage object
    // ALSO NEW: pass "client_credentials" in with the Memory object

   // $storage = new OAuth2\Storage\Memory(array('user_credentials' => $users, 'client_credentials' => $clients));

    // create the grant type
    $grantType = new OAuth2\GrantType\UserCredentials($storage);

    // Pass a storage object or array of storage objects to the OAuth2 server class
    $server = new OAuth2\Server($storage);

    // add the grant type to your OAuth server
    $server->addGrantType($grantType);
 
**/




$server = new OAuth2\Server($storage, $config);
    $server->setConfig('access_lifetime', 3600 * 24 * 7); //SO VERRRY IMPORTANT. Finally works. 
    
/*
//change token lifetime based on client. Mobile make really long. Office make 2 weeks Barn make 6 hours.
$useragent = $_SERVER['HTTP_USER_AGENT'];
//from stackexchange
if (preg_match('/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i', $useragent) || preg_match('/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i', substr($useragent, 0, 4))) {
    $server->setConfig('access_lifetime', 3600 * 24 * 60); //SO VERRRY IMPORTANT. Finally works. 
} else {
    $server->setConfig('access_lifetime', 3600 * 24 * 7); //SO VERRRY IMPORTANT. Finally works. 
}
*/



// Add the "Client Credentials" grant type (it is the simplest of the grant types), ie LOGIN & PASSWORD 
$server->addGrantType(new OAuth2\GrantType\ClientCredentials($storage));



/*
// Add the "Authorization Code" grant type (this is where the oauth magic happens)
$server->addGrantType(new OAuth2\GrantType\AuthorizationCode($storage));

// create refresh token grant type (added by D Waddy Aug 11 2014)
$server->addGrantType(new OAuth2\GrantType\RefreshToken($storage));
*/



//var_dump($server->getGrantTypes());





?>