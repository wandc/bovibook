<?php
/* class that shows various usefule info for developing */


/**
  Test page to try new things with.
 */
class Debug extends BasePage {

    function defaultDisplay() {
        
        echo $_SERVER["REMOTE_ADDR"];
        
        
        //var_dump($GLOBALS);
        
        print("<br/><h2>Oauth2 Info</h2>");
        $expireTime = strtotime(Oauth2Local::findOutWhenSessionAccessTokenExpiresEpoch())-strtotime("now");
        print("Access Token expireTime : $expireTime s<br/><br/>");
        $access_token = rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');
        print("Access Token: $access_token<br/>");

        
        
        //////////////
           print("<br/><h2>Cookies</h2>");
        print_r($_COOKIE);
        
//////////////////
        
         print("<br/><br/>SOAP:<br/><h2>check if soap extension is running...<h2>");

        if (extension_loaded('soap')) {
            print("<h2>soap extension is running! Yeah!<h2>");
        } else {
            print("<h2>soap extension is NOT running! Booo!<h2>");
        }

        //////
        
print("<br/><h2>PDO drivers</h2>");
print_r(PDO::getAvailableDrivers()); 

        print("<br/><h2>Session (all variables)</h2>");
           $maxlifetime = ini_get("session.gc_maxlifetime");
        print("Session Max Life: $maxlifetime<br/>");
        $cache_expire = session_cache_expire();
         print("Session cache_expire: $cache_expire<br/>");
        echo '<pre>';
        var_dump($_SESSION);
        echo '</pre>';

        //////////////////
        print("<br/><h2>PHP Info</h2>");
        phpinfo();
        xdebug_info();
    }

}

//end class

