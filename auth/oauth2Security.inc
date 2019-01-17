<?php 
/*
//DEBUG
ini_set('display_startup_errors',1);
ini_set('display_errors',1);
error_reporting(-1);
*/

require_once($_SERVER['DOCUMENT_ROOT'] . 'global.php'); //loads db code.
//include google api files
set_include_path($_SERVER['DOCUMENT_ROOT'] . 'framework/google-api-php-client/src/' . PATH_SEPARATOR . get_include_path());
require_once($_SERVER['DOCUMENT_ROOT'] . 'framework/google-api-php-client/src/Google/Client.php');
require_once($_SERVER['DOCUMENT_ROOT'] . 'framework/google-api-php-client/src/Google/Auth/OAuth2.php');
require_once($_SERVER['DOCUMENT_ROOT'] . 'framework/google-api-php-client/src/Google/Service/Oauth2.php');



/* main function that checks security */
class Oauth2Security {
    public $gClient;
    
    ########## Google Settings.. Client ID, Client Secret #############
    // change settings here: https://code.google.com/apis/console/?pli=1#project:830648150001:access

    private static $google_client_id = '830648150001.apps.googleusercontent.com';
    private static $google_client_secret = 'Pekn5Z-vXW4Zr0zgQyTNuqNJ';
    private static $google_redirect_url = 'https://int.littleriver.ca/';
    private static $google_developer_key = '830648150001@developer.gserviceaccount.com';
    private static $google_scopes = array('https://www.googleapis.com/auth/userinfo.email','https://www.googleapis.com/auth/userinfo.profile','https://mail.google.com/');

/*
    private static $google_client_id = 'testclient';
    private static $google_client_secret = 'testpass';
    private static $google_redirect_url = 'https://int.littleriver.ca/index.php';
    private static $google_developer_key = '830648150001@developer.gserviceaccount.com';
    private static $google_scopes = array('basic');
 */

    //called on class initiation (start of scope)
    public function __construct() {


        $this->gClient = new Google_Client();
        $this->gClient->setApplicationName('Login to int.littleriver.ca');
        $this->gClient->setClientId(self::$google_client_id);
        $this->gClient->setClientSecret(self::$google_client_secret);
        $this->gClient->setRedirectUri(self::$google_redirect_url);
        $this->gClient->setDeveloperKey(self::$google_developer_key);
        $this->gClient->setScopes(self::$google_scopes); //this gets us the users name, email, picture, etc. 
        $this->gClient->setApprovalPrompt('auto');//auto //if this is not here, it will ask for approval=force everytime. Something to do with offline vs online access. 
       $this->gClient->setAccessType('offline'); //verify we need this??      
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        //nothing
    }

    public function main() {
  
         //start session if not started.
        if (session_status() == PHP_SESSION_NONE) {
            session_start();
                        //this is for ios, because it tends to lose things when page reloaded.
            // Extend cookie life time by an amount of your liking
$cookieLifetime = 365 * 24 * 60 * 60; // A year in seconds
setcookie(session_name(),session_id(),time()+$cookieLifetime);
            
        }

        //////// erase me
        /*
             
        //print($viewTokenObj->id_token);
       ini_set('display_startup_errors',1);
ini_set('display_errors',1);
error_reporting(-1);
        $viewTokenObj = json_decode($_SESSION['access_token']);
        $id_token=$viewTokenObj->id_token;
        $aa=JWT::decode($id_token,null,null);
        $bb= get_object_vars($aa);
        $bb['email'];
        /////////
        
         */
        
        // used for ajax requests with access token in the URL.
        if (isset($_REQUEST['access_token'])) {
            $ttt = json_decode(base64_decode(strtr($_REQUEST['access_token'], '-_', '+/'))) . PHP_EOL;
            $this->gClient->setAccessToken($ttt);
            $_SESSION['access_token'] = $this->gClient->getAccessToken();
        }
       
        // used for ajax requestes for acess token is passed in the header as Authorization Bearer xxxxxx
        //this probably only works with apache because of the header command.       
        $headers = getallheaders();
        if (isset($headers['Authorization'])) {
        $authHeader = $headers['Authorization'];
        if (strstr($authHeader, 'Bearer ') == true) {
            $authHeader = str_replace('Bearer ', '', $authHeader);
            $ttt2 = json_decode(base64_decode(strtr($authHeader, '-_', '+/'))) . PHP_EOL;
            $this->gClient->setAccessToken($ttt2);
            $_SESSION['access_token'] = $this->gClient->getAccessToken();
        }}

        
        //If user wishes to log out, we just unset Session variable
        if (isset($_REQUEST['resetauth'])) {
            unset($_SESSION['access_token']);
            unset($GLOBALS["Oauth2"]);
            $this->gClient->revokeToken();
            
           
            header('Location: ' . filter_var(self::$google_redirect_url, FILTER_SANITIZE_URL));
 
        }
        
        // Step 2: The user accepted your access now you need to exchange it.
        if (isset($_GET['code'])) {
            $this->gClient->authenticate($_GET['code']);
            $_SESSION['access_token'] = $this->gClient->getAccessToken();
            header('Location: ' . filter_var(self::$google_redirect_url, FILTER_SANITIZE_URL));
            return;
        }

       
        

         // Step 3: We have access we can now create our service
        //else we make a authurl to use for login button later.
        if (isset($_SESSION['access_token']) && $_SESSION['access_token']) {
  $this->gClient->setAccessToken($_SESSION['access_token']);
} else {
  $authUrl =  $this->gClient->createAuthUrl();
}
        
      

    


        // The access token expires every 3600 seconds, so if session is still valid, then we allow a refresh token 
        //from: http://stackoverflow.com/questions/15770674/how-can-i-get-an-updated-access-token-using-stored-refresh-token
        // Google only sends the refresh token once when you first grant access to the app. To allow it to be sent again, 
        // both force and offline have to be set above. 
        //so I guess proper procedure is to store the refresh token in the DB and when you need a new access token you use it. 
        //the refresh tokens never expire unless the user revokes access to the little river app.
   


        if ($this->gClient->getAccessToken()) {
          
            //Get user details if user is logged in
            $_SESSION['access_token'] = $this->gClient->getAccessToken();
            //print_r($_SESSION['access_token']);
         
            
    //service (based on profile set above) to get user info.
    $oauth2Service = new Google_Service_Oauth2($this->gClient);
   
    //if token is expired it shows up here because we are trying to use a service. 
    try {
    $userInfo = $oauth2Service->userinfo->get();
    }
    //assume this is the error: The OAuth 2.0 access token has expired, and a refresh token is not available. Refresh tokens are not returned for responses that were auto-approved.
    catch(Google_Auth_Exception $exception) { 
        
        /*** this code causes a login to be shown instead of refresh token. HACK */
        //show login when token is expired. (FIXME: NOT THE RIGHT WAY, login every hour with this????)
       /* $authUrl =  $this->gClient->createAuthUrl();
        $this->loginButton($authUrl);
        exit(); */
        /** ****************************** */
            /*
             print($exception);
                ini_set('display_startup_errors', 1);
                ini_set('display_errors', 1);
                error_reporting(-1);
*/
               
            //look at the access token expire time. if it is expired
           //generate a new token from the DB stored refresh token  
           //apparently you can't get a new token before it is expired or you get an invalid grant error. 
            $viewTokenObj = json_decode($_SESSION['access_token']);
          
            
            if (($viewTokenObj->expires_in + $viewTokenObj->created) <= (time() + 0)) {
                
                //find tokens email address.
                    $viewTokenObj = json_decode($_SESSION['access_token']);
                    $id_token = $viewTokenObj->id_token;
                    $aa = JWT::decode($id_token, null, null);
                    $bb = get_object_vars($aa);
        
                //take google user id from token and lookup in db for match to get refresh token from db.
                $sql11 = "SELECT google_refresh_token FROM wcauthentication.users_google WHERE google_id='{$bb['id']}' limit 1";
                $res11 = $GLOBALS['pdo']->query($sql11);
                // they exist in DB already (as they should)
               if ($res11->rowCount() > 0) {
                  $row11 = $res11->fetch(PDO::FETCH_ASSOC);
                     $this->gClient->refreshToken($row11['google_refresh_token']);
                     $_SESSION['access_token'] = $this->gClient->getAccessToken();
                     //do 2nd attempt to get userinfo....
                     $userInfo = $oauth2Service->userinfo->get();
                }
            }

            
    } //end exception catch
    
    
            $userArr['user_id'] = $userInfo['id'];
            $userArr['user_name'] = filter_var($userInfo['name'], FILTER_SANITIZE_SPECIAL_CHARS);
            $userArr['email'] = filter_var($userInfo['email'], FILTER_SANITIZE_EMAIL);
            $userArr['profile_url'] = filter_var($userInfo['link'], FILTER_VALIDATE_URL);
            $userArr['profile_image_url'] = filter_var($userInfo['picture'], FILTER_VALIDATE_URL);
            $userArr['personMarkup'] = "{$userArr['email']}<div><img src='{$userArr['profile_image_url']}?sz=50'></div>";

          
            
                  
            //obtain refresh token (only will be here right after inintial authorization and never again.
            $_SESSION['access_token'] = $this->gClient->getAccessToken();
            $authObj = json_decode($_SESSION['access_token']);
            if (isset($authObj->refresh_token)) {
            $refreshToken = $authObj->refresh_token;
            $userArr['refreshToken']=$refreshToken;
            }
            else {
               $userArr['refreshToken']=null;  
            }
        
            $this->loginLog($userArr['user_name']);
             //now do local DB login stuff
             $this->dbLoginStuff($userArr);
             //now do db get userinfo stuff
             $userArr=$this->dbUserInfoStuff($userArr);
            
            
            //global the user array
            $GLOBALS['Oauth2']['userArr']=$userArr;
      

            //we are now fully logged in, but silently end.
           
        } else {
 
            //not logged in at all, but also silently end.
            $GLOBALS['Oauth2']['authUrl'] = $this->gClient->createAuthUrl();
        }

        
        //close of the session after all writes are done so that we free the 
        // file lock on session file and otehr tab scripts or ajax calls can run.
        session_write_close();
        
      /*
        //DEBUG
        if ($this->checkAuth()) {
            print('<b style="position:absolute">Oauth2: logged in.</b><br/>');
        } else {
            print('<b style="position:absolute">Oauth2: NOT logged in.</b><br/>');
        }
        */
        
    }//end main function 

   
    
    /* this should be enough to say if we are actually logged in or not */
    public static function checkAuth() {
        if (isset($_SESSION['access_token'])) {
            return true;
        }
        else {
            return false;
    }
    } 
    
    public static function getUsername() {
        if (isset($GLOBALS['Oauth2']['userArr']['userid'])) {
            return $GLOBALS['Oauth2']['userArr']['userid'];
        } else {
            return null;
        }
    }
    
    
    public static function getAuthData($array) {
        if ($array ='groups')
        {   
        return $GLOBALS['Oauth2']['userArr']['groups'];
        }
        else {
            return null;
        }
    }
    
    
    


    /*
     * stores info in db when authenticated.
     */
    private function dbLoginStuff($userArr) {
        
        //check if we are already in DB
        $sql = "SELECT google_id FROM wcauthentication.users_google WHERE google_id='{$userArr['user_id']}'";
        $res = $GLOBALS['pdo']->query($sql);
        /* they exist in system already */
        if ($res->rowCount() > 0) {
            //echo 'Welcome back ' . $userArr['user_name'] . '!';
            //update the refresh token, might not have to do this if it wern't for it always asking to verify teh permission we are giving.
            if (($userArr['refreshToken'] !=null) AND ($userArr['refreshToken'] !='')) {
            $sql = ("UPDATE wcauthentication.users_google SET google_refresh_token = '{$userArr['refreshToken']}' WHERE google_id={$userArr['user_id']}");
            $res = $GLOBALS['pdo']->exec($sql);
            }
            return 'returning';
        } else { //user is new
   
            // first check if the google_email matches an internet email to prevent someome out on the internet who is not valid to login.
            $sql5 = "SELECT userid FROM wcauthentication.users WHERE email='{$userArr['email']}'";
            $res5 = $GLOBALS['pdo']->query($sql5);
            if ($res5->rowCount() > 0) {
            
            //echo 'Hi ' . $userArr['user_name'] . ', Thanks for Registering!';
            $sql = ("INSERT INTO wcauthentication.users_google (google_id, google_name, google_email, google_link, google_picture_link, google_refresh_token) VALUES ('{$userArr['user_id']}', '{$userArr['user_name']}','{$userArr['email']}','{$userArr['profile_url']}','{$userArr['profile_image_url']}','{$userArr['refreshToken']}')");
            $res = $GLOBALS['pdo']->exec($sql);
            return 'new';
            }
            else {
              return 'invalid user';   
            }
        }
        
    }
    
    private function dbUserInfoStuff($userArr) {
        
        //find the userid without @littleriver.ca
        $sql = "SELECT users.userid as short_userid FROM wcauthentication.users_google LEFT JOIN wcauthentication.users ON users.email=users_google.google_email WHERE google_id='{$userArr['user_id']}' limit 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $userArr['userid'] = $row['short_userid'];
      
        //finds the groups array.
        $res2 = $GLOBALS['pdo']->query("SELECT groupid FROM wcauthentication.users_in_groups WHERE userid='{$userArr['userid']}'");
        $groupArr = $res2->fetchCol(0);    
        $userArr['groups']=$groupArr;
        
        return $userArr;
    }
    
    
    

    
    //print login button.  (called directly from basepage logic)
    public static function loginButton($authUrl=null) { 
        if (isset($authUrl) == false) { 
      $authUrl=$GLOBALS['Oauth2']['authUrl'];
        }
        print('<div class="loginFormImage" src="/images/LittleRiver_logo_grayscale_background.jpg">');
        print("");
        print('<a class="loginGoogleButton" href="' . $authUrl . '"><h1>Please Login</h1><img src="/images/google-login-button.png" /></a>');
        print("</div>");
        
        //used to load CSS files.
        include_once($_SERVER['DOCUMENT_ROOT'].'template/basePage.php');
        BasePage::pageCSS();
    }

    
    
     /* choose which type of logout button to show based on ip address. Private computers would only want to logout of INT site and not google too. */
     public static function logoutButton() {
        //home mac, work mac, fred pc 
        if (($_SERVER['REMOTE_ADDR'] == '192.168.8.51') || ($_SERVER['REMOTE_ADDR'] == '192.168.8.28') || ($_SERVER['REMOTE_ADDR'] == '192.168.8.24')) {
        return self::logoutJustIntButton();
        } else {
        return self::logoutGoogleIntButton();
        }
    }

    /* this logs out of just little river int site. Used for PC's that are secure and single user */
    public static function logoutJustIntButton() {
        $str = '';
        $str = $str . ("<form class=\"logoutForm\" method=\"post\" action=\"{$_SERVER['PHP_SELF']}/index.php?resetauth=1\">");
        $str = $str . ("<input id=\"logoutButton\" type=\"submit\" title=\"Logout Int Only\" value=\"\"/>");
        $str = $str . ("</form>");
        return $str;
    }

    /* logs out of google then Int site */
    public static function logoutGoogleIntButton() {
        /**
         * this js code logs you out of everything to do with google and then out of int.littleriver.ca
         */
        ?>
                 <script type="text/javascript" language="javascript">   
                     $(document).ready(function(){ 
                         $('.logoutForm').on('submit', function () {
                             var site = location.href;
                             document.location.href = "https://www.google.com/accounts/Logout?continue=https://appengine.google.com/_ah/logout?continue=" + site + "index.php?resetauth=1";
                             return false;
                         });
                     }); 
                 </script>
                     <?php 
        //this action is overridden by the above javascript.             
        print("<form class=\"logoutForm\" method=\"post\" action=\"{$_SERVER['PHP_SELF']}/index.php?resetauth=1\">");
        print("<input id=\"logoutButton\" type=\"submit\" title=\"Logout Int & Google\" value=\"\"/>");
        print("</form>");
    }
    
 /** store whoever logs in to a table in db,
      /** this should only be called once per login. */
    function loginLog($username) {
        $uuid = self::uuid();
        $userid = $username;
        $event_time = date('r', strtotime("now"));
        $user_agent = '';
        $user_agent = pg_escape_string($_SERVER['HTTP_USER_AGENT']);
        $ip = $_SERVER['REMOTE_ADDR'];
//run insert
        $query = "INSERT INTO system.login_log (uuid,userid,event_time,user_agent,ip) VALUES ('$uuid','$userid','$event_time','$user_agent','$ip')";
        $res = $GLOBALS ['pdo']->exec($query);
    }

    /** generates uuid according to RFC4122.
      /** If we did this alot, use postgres extension.
      /** There are faster functions in php too. */
    function uuid() {
        return sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x', mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0x0fff) | 0x4000, mt_rand(0, 0x3fff) | 0x8000, mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff));
    }

    
}

//end class

/*
 * DO NOT CALL HERE, CALL FROM INDEX FILE
$auth = new Oauth2Security();
$auth->main();
global $auth;
 */


/**
 * JSON Web Token implementation
 *
 * Minimum implementation used by Realtime auth, based on this spec:
 * http://self-issued.info/docs/draft-jones-json-web-token-01.html.
 *
 * @author Neuman Vong <neuman@twilio.com>
 */
class JWT
{
    /**
     * @param string      $jwt    The JWT
     * @param string|null $key    The secret key
     * @param bool        $verify Don't skip verification process 
     *
     * @return object The JWT's payload as a PHP object
     */
    public static function decode($jwt, $key = null, $verify = true)
    {
        $tks = explode('.', $jwt);
        if (count($tks) != 3) {
            throw new UnexpectedValueException('Wrong number of segments');
        }
        list($headb64, $payloadb64, $cryptob64) = $tks;
        if (null === ($header = JWT::jsonDecode(JWT::urlsafeB64Decode($headb64)))
        ) {
            throw new UnexpectedValueException('Invalid segment encoding');
        }
        if (null === $payload = JWT::jsonDecode(JWT::urlsafeB64Decode($payloadb64))
        ) {
            throw new UnexpectedValueException('Invalid segment encoding');
        }
        $sig = JWT::urlsafeB64Decode($cryptob64);
        if ($verify) {
            if (empty($header->alg)) {
                throw new DomainException('Empty algorithm');
            }
            if ($sig != JWT::sign("$headb64.$payloadb64", $key, $header->alg)) {
                throw new UnexpectedValueException('Signature verification failed');
            }
        }
        return $payload;
    }

    /**
     * @param object|array $payload PHP object or array
     * @param string       $key     The secret key
     * @param string       $algo    The signing algorithm
     *
     * @return string A JWT
     */
    public static function encode($payload, $key, $algo = 'HS256')
    {
        $header = array('typ' => 'JWT', 'alg' => $algo);

        $segments = array();
        $segments[] = JWT::urlsafeB64Encode(JWT::jsonEncode($header));
        $segments[] = JWT::urlsafeB64Encode(JWT::jsonEncode($payload));
        $signing_input = implode('.', $segments);

        $signature = JWT::sign($signing_input, $key, $algo);
        $segments[] = JWT::urlsafeB64Encode($signature);

        return implode('.', $segments);
    }

    /**
     * @param string $msg    The message to sign
     * @param string $key    The secret key
     * @param string $method The signing algorithm
     *
     * @return string An encrypted message
     */
    public static function sign($msg, $key, $method = 'HS256')
    {
        $methods = array(
            'HS256' => 'sha256',
            'HS384' => 'sha384',
            'HS512' => 'sha512',
        );
        if (empty($methods[$method])) {
            throw new DomainException('Algorithm not supported');
        }
        return hash_hmac($methods[$method], $msg, $key, true);
    }

    /**
     * @param string $input JSON string
     *
     * @return object Object representation of JSON string
     */
    public static function jsonDecode($input)
    {
        $obj = json_decode($input);
        if (function_exists('json_last_error') && $errno = json_last_error()) {
            JWT::handleJsonError($errno);
        }
        else if ($obj === null && $input !== 'null') {
            throw new DomainException('Null result with non-null input');
        }
        return $obj;
    }

    /**
     * @param object|array $input A PHP object or array
     *
     * @return string JSON representation of the PHP object or array
     */
    public static function jsonEncode($input)
    {
        $json = json_encode($input);
        if (function_exists('json_last_error') && $errno = json_last_error()) {
            JWT::handleJsonError($errno);
        }
        else if ($json === 'null' && $input !== null) {
            throw new DomainException('Null result with non-null input');
        }
        return $json;
    }

    /**
     * @param string $input A base64 encoded string
     *
     * @return string A decoded string
     */
    public static function urlsafeB64Decode($input)
    {
        $remainder = strlen($input) % 4;
        if ($remainder) {
            $padlen = 4 - $remainder;
            $input .= str_repeat('=', $padlen);
        }
        return base64_decode(strtr($input, '-_', '+/'));
    }

    /**
     * @param string $input Anything really
     *
     * @return string The base64 encode of what you passed in
     */
    public static function urlsafeB64Encode($input)
    {
        return str_replace('=', '', strtr(base64_encode($input), '+/', '-_'));
    }

    /**
     * @param int $errno An error number from json_last_error()
     *
     * @return void
     */
    private static function handleJsonError($errno)
    {
        $messages = array(
            JSON_ERROR_DEPTH => 'Maximum stack depth exceeded',
            JSON_ERROR_CTRL_CHAR => 'Unexpected control character found',
            JSON_ERROR_SYNTAX => 'Syntax error, malformed JSON'
        );
        throw new DomainException(isset($messages[$errno])
            ? $messages[$errno]
            : 'Unknown JSON error: ' . $errno
        );
    }

}

?>