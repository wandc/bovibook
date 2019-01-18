<?php 

/*
 * The only way to get a refresh token is through: "Authorizaton Code or User Credentials grant types".
 * This means for us that we can only get a refresh token when we are using a username and password, ie login.
 * So we need to always get a refresh token at login and if we ever detect the refresh token is expired we should 
 * send them back to a login prompt and get another one.
 * We should probably have some JS code running so when the refresh token expires, you see a login screen instead of multiple 
 * access denied messages from every ajax call running.

 */


/* main function that checks security */

class Oauth2Local {

    public $gClient;

  
    private  $google_client_id;
    private  $google_client_secret;
    private  $google_redirect_url;
    private  $google_developer_key;
    private static $google_scopes = array('basic');

    //called on class initiation (start of scope)
    public function __construct() {

        $this->google_client_id=$GLOBALS['config']['OAUTH2_LOCAL']['google_client_id'];
        $this->google_client_secret=$GLOBALS['config']['OAUTH2_LOCAL']['google_client_secret'];
        $this->google_redirect_url=$GLOBALS['config']['OAUTH2_LOCAL']['google_redirect_url'];
        $this->google_developer_key=$GLOBALS['config']['OAUTH2_LOCAL']['google_developer_key'];
        
        $this->gClient = new Google_Client();
        $this->gClient->setApplicationName('Login to '.$GLOBALS['config']['HTTP']['URL']);
        $this->gClient->setClientId($this->google_client_id);
        $this->gClient->setClientSecret($this->google_client_secret);
        $this->gClient->setRedirectUri($this->google_redirect_url);
        $this->gClient->setDeveloperKey($this->google_developer_key);
        $this->gClient->setScopes(self::$google_scopes); //this gets us the users name, email, picture, etc. 
        $this->gClient->setApprovalPrompt('auto'); //auto //if this is not here, it will ask for approval=force everytime. Something to do with offline vs online access. 
        $this->gClient->setAccessType('offline'); //verify we need this??      
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        //nothing
    }

    private static function isJson($string) {
        json_decode($string);
        return (json_last_error() == JSON_ERROR_NONE);
    }

    
    
    public function main() {

        
            
    
  //does not currently work with fastcgi in 2017, this can probbaly be removed in future.  
if (!function_exists('getallheaders')) 
{ 
    function getallheaders() 
    { 
           $headers = ''; 
       foreach ($_SERVER as $name => $value) 
       { 
           if (substr($name, 0, 5) == 'HTTP_') 
           { 
               $headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value; 
           } 
       } 
       return $headers; 
    } 
}
        
        
        //start session if not started.
        if (session_status() == PHP_SESSION_NONE) {
            session_start();
            //this is for ios, because it tends to lose things when page reloaded.
            // Extend cookie life time by an amount of your liking
$cookieLifetime = 365 * 24 * 60 * 60; // A year in seconds
setcookie(session_name(),session_id(),time()+$cookieLifetime);
        }


        // used for ajax requests with access token in the URL.
        if (isset($_REQUEST['access_token'])) {
            $ttt = json_decode(base64_decode(strtr($_REQUEST['access_token'], '-_', '+/'))) . PHP_EOL;
            $_SESSION['access_token'] = $ttt;
        }

        // used for ajax requestes for acess token is passed in the header as Authorization Bearer xxxxxx
        //this probably only works with apache because of the header command.       
        $headers = getallheaders();
        if (isset($headers['Authorization'])) {
            $authHeader = $headers['Authorization'];
            if (strstr($authHeader, 'Bearer ') == true) {
                $authHeader = str_replace('Bearer ', '', $authHeader);
                $ttt2 = json_decode(base64_decode(strtr($authHeader, '-_', '+/'))) . PHP_EOL;
                $_SESSION['access_token'] = $ttt2;
            }
        }




        //If user wishes to log out, we just unset Session variable and forward to a new page
        if (isset($_REQUEST['resetauth'])) {
            $this->logout();
            exit();
        }


        if ((isset($_SESSION['access_token']) == false) OR ( trim($_SESSION['access_token']) == '')) {


            //login form submmitted.
            if ((isset($_REQUEST['username'])) AND ( isset($_REQUEST['password']))) {

                //put code to detect mobile devices here. If it is a phone, then allow long tokens.
               $extra='';
               if ((isset($_REQUEST['longtoken'])) AND $_REQUEST['longtoken']==1) {
                   $extra='longtoken=1';
               }
                
                //md5 and salt login here.
                $username = filter_var($_REQUEST['username'], FILTER_SANITIZE_EMAIL);
                $password = md5($_REQUEST['password']); //since we md5 it, it doesn't need filtered.
                //$password=($_REQUEST['password']);
                // curl -u david:MD5ofPassword https://int.littleriver.ca/auth/token.php -d 'grant_type=client_credentials'               
                $login = curl_init($GLOBALS['config']['HTTP']['URL_SHORT'].'auth/token.php');
                curl_setopt($login, CURLOPT_COOKIESESSION, true);
                curl_setopt($login, CURLOPT_RETURNTRANSFER, 1);
                curl_setopt($login, CURLOPT_FOLLOWLOCATION, 1);
                curl_setopt($login, CURLOPT_SSL_VERIFYPEER, 0);
                curl_setopt($login, CURLOPT_SSL_VERIFYHOST, 2);
                curl_setopt($login, CURLOPT_UNRESTRICTED_AUTH, 1);
                curl_setopt($login, CURLOPT_USERPWD, "$username:$password");
                curl_setopt($login, CURLOPT_POSTFIELDS, $extra.'grant_type=client_credentials');
                $loginStr = curl_exec($login);
                 
                //without this curl silently fails.
                if($loginStr === false)
                {
                    throw new Exceiption('LOGIN Curl error: ' . curl_error($login));
                }
                
                curl_close($login);

                if (self::isJson($loginStr)) {
                    //so we know we have json. still coud be an error
                    $tok = json_decode($loginStr);
                    //var_dump($tok);
                    if (isset($tok->access_token)) {

                        //lookup information in DB to encode into the session for later retrivel.
                        //UPDATE: there is no point in doing this unless we sign the information or it can't be trusted because the client can theoretically change it.
                        //
                      // $tok->id_token='not_secure without being signed';
                        //
                     //just use a server side global variable instead.
                            
                        $_SESSION['access_token'] = json_encode($tok);
                        $GLOBALS['Oauth2']['userArr']['groups'] = self::findOutWhatGroupsTheUserIsIn($username);
                        $GLOBALS['Oauth2']['userArr']['userid'] = $username;
                        $GLOBALS['auth'] = $this;
                        $this->loginLog($username);
                        //store in DB log that someone logged in.
                        
                        //set global
                        // print_r2($GLOBALS['Oauth2']);
                    } else {
                        //show inccorect login page
                        $this->incorrectPage();
                    }
                }
            } else {
                //this has to be a whole complete page with body,headers, etc......
                //show login page
                $this->loginPage();
            }
        }



        //we are already logged in. Is token valid?? check?
        if ((isset($_SESSION['access_token'])) && (trim($_SESSION['access_token']) != '')) {

            $tok = json_decode($_SESSION['access_token']);


            //take access token and lookup userid
            $sql4 = "SELECT client_id FROM wcauthentication.oauth_access_tokens WHERE access_token='{$tok->access_token}' AND expires < now()";
            $res4 = $GLOBALS['pdo']->query($sql4);
            $sql5 = "SELECT client_id FROM wcauthentication.oauth_access_tokens WHERE access_token='{$tok->access_token}' AND expires > now()";
            $res5 = $GLOBALS['pdo']->query($sql5);

            if ($res5->rowCount() > 0) {
                $row5 = $res5->fetch(PDO::FETCH_ASSOC);
                $username = $row5['client_id'];
                $GLOBALS['Oauth2']['userArr']['groups'] = self::findOutWhatGroupsTheUserIsIn($username);
                $GLOBALS['Oauth2']['userArr']['userid'] = $username;
                $GLOBALS['auth'] = $this;
            }
            //expired token
            elseif ($res4->rowCount() > 0) {

                $row4 = $res4->fetch(PDO::FETCH_ASSOC);
                $username = $row4['client_id'];
                //lets get a new token
                //first lookup the refresh token in db.
                $sql3 = "SELECT refresh_token FROM wcauthentication.oauth_refresh_tokens WHERE client_id='$username' AND expires > now()";
                $res3 = $GLOBALS['pdo']->query($sql3);
                if ($res3->rowCount() == 0) {
                    //print("<h1>Error:No refresh token found in db. You need to authenticate!</h1>");
                    $this->logout(); //log them out so they can reauthenticate.
                    exit();
                } else {
                    $row3 = $res3->fetch(PDO::FETCH_ASSOC);


                    $login2 = curl_init($GLOBALS['config']['HTTP']['URL_SHORT'].'auth/token.php');
                    curl_setopt($login, CURLOPT_COOKIESESSION, true);
                    curl_setopt($login, CURLOPT_RETURNTRANSFER, 1);
                    curl_setopt($login, CURLOPT_FOLLOWLOCATION, 1);
                    curl_setopt($login, CURLOPT_SSL_VERIFYPEER, 0);
                    curl_setopt($login, CURLOPT_SSL_VERIFYHOST, 2);
                    curl_setopt($login, CURLOPT_UNRESTRICTED_AUTH, 1);
                    curl_setopt($login, CURLOPT_USERPWD, "$username:$password");
                    curl_setopt($login, CURLOPT_POSTFIELDS, "grant_type=refresh_token&refresh_token={$row3['refresh_token']}");
                    $loginStr2 = curl_exec($login2);
                    curl_close($login2);

                    if (self::isJson($loginStr2)) {
                        //so we know we have json. still coud be an error
                        $tok2 = json_decode($loginStr2);
                        if (isset($tok2->access_token)) {
                            $_SESSION['access_token'] = $tok2;
                            $GLOBALS['Oauth2']['userArr']['groups'] = self::findOutWhatGroupsTheUserIsIn($username);
                            $GLOBALS['Oauth2']['userArr']['userid'] = $username;
                            $GLOBALS['auth'] = $this;
                        }
                    }
                }
            } else {
                //access token is invaid.
                //print_r($_SESSION['access_token']);
                print("<h1>Token not valid.</h1>");
            }
        } else {
            // print("no access token set."); //debug
        }

        //close of the session after all writes are done so that we free the 
        // file lock on session file and otehr tab scripts or ajax calls can run.
        session_write_close();

        if ($this->checkAuth()) {
            
        }
        /*
          //DEBUG
          if ($this->checkAuth()) {
          //  print_r2($_SESSION);
          self::logoutJustIntButton();
          print('<b style="position:absolute">Oauth2: logged in.</b><br/>');
          } else {
          print('<b style="position:absolute">Oauth2: NOT logged in.</b><br/>');
          }
         */
    }

//end main function 



    /* this should be enough to say if we are actually logged in or not */

    public static function checkAuth() {
        if (isset($_SESSION['access_token'])) {
            return true;
        } else {
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
        if ($array = 'groups') {
            return $GLOBALS['Oauth2']['userArr']['groups'];
        } else {
            return null;
        }
    }

    private function logout() {
        unset($_SESSION['access_token']);
        unset($GLOBALS['Oauth2']);
        $this->gClient->revokeToken();
        header('Location: ' . filter_var($this->google_redirect_url, FILTER_SANITIZE_URL));
        exit();
    }

    //what is shown when incorrect login inforamtion is enetered
    private function incorrectPage() {
        print("<div><h3>Incorrect login! Please Try Again.</h3></div>");
        $js = <<<JS
                                <script type="text/javascript">
                                /* forwards page after a few seconds to go back to login */
                                doStuff();
                                function doStuff()
                                {
                                setTimeout(continueExecution, 2000) //wait 2 seconds before continuing
                                }
                                function continueExecution()
                                {
                                  window.location.replace("https://{$_SERVER['HTTP_HOST']}");
                                }
                          
                                </script>
JS;
        print($js);
    }

    private function loginPage() {

        //basePage::doctype();
        print('<head>' . '<!-- Login Page-->' . "\n");
        basePage::allPageHeadContent();
        print('<title>Login Page</title>' . "\n");
        basePage::pageCSS();
        print('</head>' . "\n");
        print('<body>' . "\n");

        $str2 = <<<HTML
            <div class="loginFormImage" src="/images/LittleRiver_logo_grayscale_background.jpg">
    <form class="loginForm" method="post" action="https://{$_SERVER['HTTP_HOST']}">
    <p><b>Username</b><input class="loginFormInput" type="text" name="username" onChange="javascript:this.value=this.value.toLowerCase();"/></p>
    <p><b>Password</b><input class="loginFormInput" type="password" name="password"/></p>
    <p><input class="loginFormSubmit" type="submit" value="Login"/></p>
    </form>
    </div>
HTML;
        print $str2;

        print('</body>' . "\n");
        print('</html>' . "\n");
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
            if (($userArr['refreshToken'] != null) AND ( $userArr['refreshToken'] != '')) {
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
            } else {
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
        $groupArr = $res2->fetchAll();
        $userArr['groups'] = $groupArr;

        return $userArr;
    }

    //print login button.  (called directly from basepage logic)
    public static function loginButton($authUrl = null) {
        if (isset($authUrl) == false) {
            $authUrl = $GLOBALS['Oauth2']['authUrl'];
        }
        print('<div class="loginFormImage" src="/images/LittleRiver_logo_grayscale_background.jpg">');
        print("");
        print('<a class="loginGoogleButton" href="' . $authUrl . '"><h1>Please Login</h1><img src="/images/google-login-button.png" /></a>');
        print("</div>");

        //used to load CSS files.
        include_once($_SERVER['DOCUMENT_ROOT'] . '/template/basePage.php');
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

        $str = <<<HTML
     <form class="logoutForm" method="post" action="https://{$_SERVER['HTTP_HOST']}/index.php?resetauth=1">
     <input id="logoutButton" type="submit" title="Logout Int Only XXX" value=""/>   
     </form>
HTML;

        return $str;
    }

    /* logs out of google then Int site */

    public static function logoutGoogleIntButton() {
        $str = '';
        /**
         * this js code logs you out of everything to do with google and then out of local site
         */
        $str = $str . <<<JS
  <script type="text/javascript" language="javascript">   
                     $(document).ready(function(){ 
                         $('.logoutForm').on('submit', function () {
                             var newURL = window.location.protocol + "//" + window.location.host + "/" + window.location.pathname;
                             document.location.href = "https://www.google.com/accounts/Logout?continue=https://appengine.google.com/_ah/logout?continue=" + newURL + "?resetauth=1";
                             return false;
                         });
                     }); 
                 </script>
JS;
//  <--this action is overridden by the above javascript.-->    
        $str = $str . <<<HTML
                
<form class="logoutForm" method="post" action="https://{$_SERVER['HTTP_HOST']}/index.php?resetauth=1">
<input id="logoutButton" type="submit" title="Logout Int & Google" value=""/>
</form>
HTML;

        return $str;
    }

    //look up in DB what groups a user is in and return that as an array.
    public static function findOutWhatGroupsTheUserIsIn($userid) {
        $res = $GLOBALS['pdo']->query("SELECT groupid FROM wcauthentication.users_in_groups WHERE userid='$userid'");
        $groupArray = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $groupArray[] = $row['groupid'];
        }
        return $groupArray;
    }

    
    /**
     * 
     * find out when session access token expires
     * this is meant to be called by javascript quite often.
     */
    public static function findOutWhenSessionAccessTokenExpiresEpoch() {

        $accessTokenObj = json_decode($_SESSION['access_token']);

        $query = "
        SELECT  expires FROM wcauthentication.oauth_access_tokens WHERE access_token='{$accessTokenObj->access_token}' limit 1";
        $res = $GLOBALS ['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return($row['expires']); //since unix epoch
    }


    /**
     * 
     * used by ajax calls (ie slim) to see if acess token is valid.
     * this doesn't need session to work. 
     * returns all the userinfo needed.
     */
    public static function accessTokenCheck($access_token) {

        $query = "
        SELECT client_id,expires FROM wcauthentication.oauth_access_tokens WHERE access_token='$access_token' AND expires > now() limit 1";
        $res = $GLOBALS ['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row['client_id'])) {
            $userid = $row['client_id'];

            //now look up what security groups that userid is in.
            $groupArr = self::findOutWhatGroupsTheUserIsIn($userid);

              //create a fancy object to return with all info needed on the security side.
            $obj=new stdClass();
            $obj->userid=$userid;
            $obj->groups=$groupArr;
            $obj->expires=$row['expires'];
            $obj->access_token=$access_token;
            
            return($obj); //since unix epoch   
        } else {

            return false; //security failed.
        }
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
class JWTX {

    /**
     * @param string      $jwt    The JWT
     * @param string|null $key    The secret key
     * @param bool        $verify Don't skip verification process 
     *
     * @return object The JWT's payload as a PHP object
     */
    public static function decode($jwt, $key = null, $verify = true) {
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
    public static function encode($payload, $key, $algo = 'HS256') {
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
    public static function sign($msg, $key, $method = 'HS256') {
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
    public static function jsonDecode($input) {
        $obj = json_decode($input);
        if (function_exists('json_last_error') && $errno = json_last_error()) {
            JWT::handleJsonError($errno);
        } else if ($obj === null && $input !== 'null') {
            throw new DomainException('Null result with non-null input');
        }
        return $obj;
    }

    /**
     * @param object|array $input A PHP object or array
     *
     * @return string JSON representation of the PHP object or array
     */
    public static function jsonEncode($input) {
        $json = json_encode($input);
        if (function_exists('json_last_error') && $errno = json_last_error()) {
            JWT::handleJsonError($errno);
        } else if ($json === 'null' && $input !== null) {
            throw new DomainException('Null result with non-null input');
        }
        return $json;
    }

    /**
     * @param string $input A base64 encoded string
     *
     * @return string A decoded string
     */
    public static function urlsafeB64Decode($input) {
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
    public static function urlsafeB64Encode($input) {
        return str_replace('=', '', strtr(base64_encode($input), '+/', '-_'));
    }

    /**
     * @param int $errno An error number from json_last_error()
     *
     * @return void
     */
    private static function handleJsonError($errno) {
        $messages = array(
            JSON_ERROR_DEPTH => 'Maximum stack depth exceeded',
            JSON_ERROR_CTRL_CHAR => 'Unexpected control character found',
            JSON_ERROR_SYNTAX => 'Syntax error, malformed JSON'
        );
        throw new DomainException(isset($messages[$errno]) ? $messages[$errno] : 'Unknown JSON error: ' . $errno
        );
    }

}
?>