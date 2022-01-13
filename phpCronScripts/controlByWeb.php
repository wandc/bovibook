<?php

/*
 * This is for control by web at feed tank 1 and grey barn.
 * This is also for Feed tank 4 raspberry pi, emulating control by web XML format
 */

//error_reporting(-1000);
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT'] = dirname(__DIR__) . '/';
}
include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');




/* this code downloads xml information from various control by web sources around the farm and inserts into db */

class ControlByWeb {
    /* meant to run by cron job on a timer every minute or so. */

    public function feedTankWeightMain() {
        $this->feedTankWeight(0, "http://192.168.9.192/state.xml"); //control by web (old type) (X-DAQ-8A5-I 00:0C:C8:03:56:A4)
        $this->feedTankWeight(1, "http://192.168.9.188/state.xml"); //control by web (old type)
        $this->feedTankWeight(2, "http://192.168.9.196/state.xml"); //control by web x418 (newer)
        $this->feedTankWeight(3, "http://192.168.8.251/state.xml"); //control by web x418 (newer) (CHANGE ME because ip will change after power out)
        $this->feedTankWeight(4, "http://192.168.9.191/index.php"); //ADC
    }

    /*
     * no longer used because of bug:
     * undefined constant CURL_HTTP_VERSION_0_9
     */

    private function curlMethod($url) {

        $ch = curl_init();    // initialize curl handle 
        curl_setopt($ch, CURLOPT_URL, $url); // set url  
        //curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_0_9); //needed or control by web will not work. //use socke method now, so ok. 
        curl_setopt($ch, CURLOPT_FAILONERROR, 1);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1); // allow redirects 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); // return into a variable 
        curl_setopt($ch, CURLOPT_TIMEOUT, 6); // times out after 4s 
        $result = curl_exec($ch); // run the whole process 
        if (curl_errno($ch)) {
            $error_msg = curl_error($ch);
        }

        curl_close($ch);
        if (isset($error_msg)) {
            throw new Exception("Error: Curl gave the following error when connecting to $url: " . $error_msg);
        }
        return $result;
    }

    /*
     * Curl no longer supports old versions of HTML on PHP easily, so since 
     * control by web is very basic and old we just connect directly. 
     *  $this->socketMethod($ip='192.168.9.188',$resource='state.xml');
     */

    private function socketMethod($url) {


        $ip = parse_url($url, PHP_URL_HOST);
        $resource = parse_url($url, PHP_URL_PATH);

        error_reporting(E_ALL);

        /* Get the port for the WWW service. */
        $service_port = getservbyname('www', 'tcp');

        /* Create a TCP/IP socket. */
        $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if ($socket === false) {
            throw newException("socket_create() failed:\nReason: " . socket_strerror(socket_last_error()) . "\n");
        }

        $result = socket_connect($socket, $ip, $service_port);
        if ($result === false) {
            throw newException("socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n");
        }

        $in = "HEAD / HTTP/1.1\r\n";
        $in .= "GET $resource HTTP/1.1";
        $in .= "Host: $ip\r\n";
        $in .= "Connection: Close\r\n\r\n";
        $out = '';

        $output = '';
        socket_write($socket, $in, strlen($in));

        //read response.
        if (false === ($bytes = socket_recv($socket, $buf, 2048, MSG_WAITALL))) {
            throw new \Exception( "socket_recv() failed; reason: " . socket_strerror(socket_last_error($socket)) . "\n");
        } 
        
        // print($buf); //DEBUG
        
         
            if (str_contains($buf, '400 Bad Request')) {
              throw new Exception("Bad URL, server doesn't know what you are trying to acccess.". "\n");   
            }
         
         
         if (str_contains($buf, '200 OK')) { //means there is a header, some devices have headers, some dont.
         
        list($header, $body) = preg_split("/\R\R/", $buf, 2); //strip raw http headers off.
         $output=$body;
         }
         else {
            $output=$buf;  
         }  
       
        socket_close($socket);

        return $output;
    }

    /**
     * 
     *   call directly to measure feed tankk weights.
     *   control by web or raspberry pi's used (that emulate control by web ).
     */
    private function feedTankWeight($tankNum, $url) {
        try {
            print("\n\rStart feedTankWeight for tank #:$tankNum\n");


            //older analog devices need different access method because of curl HTTP 0.9 limitation.
            switch ($tankNum) {
                case 0:
                    $result = $this->socketMethod($url);
                    break;
                case 1:
                    $result = $this->socketMethod($url);
                    break;
                case 2:
                    $result = $this->socketMethod($url);
                    break;
                case 3:
                    $result = $this->socketMethod($url);
                    break;
                case 4:
                    $result = $this->curlMethod($url);
                    break;
            }

            // print($result);

            $xml = new SimpleXMLElement($result);


            //HACKish, dynamically create sql, because one tanke it is inputs 1234 and next it is 5678.
            switch ($tankNum) {
                case 0:
                    $inputSQL = "{$xml->input1state},{$xml->input2state},{$xml->input3state},{$xml->input4state}";
                    break;
                case 1:
                    $inputSQL = "{$xml->input4state},{$xml->input5state},{$xml->input6state},{$xml->input7state}";
                    break;
                case 2:
                    $inputSQL = "{$xml->analogInput1},{$xml->analogInput2},{$xml->analogInput3},{$xml->analogInput4}";
                    break;
                case 3:
                    $inputSQL = "{$xml->analogInput1},{$xml->analogInput2},{$xml->analogInput3},{$xml->analogInput4}";
                    break;
                case 4:
                    $inputSQL = "{$xml->input0state},{$xml->input1state},{$xml->input2state},{$xml->input3state},{$xml->input4state},{$xml->input5state},{$xml->input6state},{$xml->input7state}";
                    break;
            }


            if ((    //older type of control by web
                    (is_numeric(trim($xml->input0state))) AND ( is_numeric(trim($xml->input1state))) AND ( is_numeric(trim($xml->input2state))) AND
                    ( is_numeric(trim($xml->input3state))) AND ( is_numeric(trim($xml->input4state))) AND ( is_numeric(trim($xml->input5state))) AND
                    ( is_numeric(trim($xml->input6state))) AND ( is_numeric(trim($xml->input7state)))
                    )
                    OR
                    (    //newer type of control by web x418
                    (is_numeric(trim($xml->analogInput1))) AND ( is_numeric(trim($xml->analogInput2))) AND ( is_numeric(trim($xml->analogInput3))) AND
                    ( is_numeric(trim($xml->analogInput4))) AND ( is_numeric(trim($xml->analogInput5))) AND ( is_numeric(trim($xml->analogInput6))) AND
                    ( is_numeric(trim($xml->analogInput7))) AND ( is_numeric(trim($xml->analogInput8)))
                    )) {

                print("INSERTING...Feed Tank#:$tankNum...with: $inputSQL\n\r"); //DEBUG
                $sql = "INSERT INTO bas.feed_bin_weight_event (tank_id,event_time,cells) VALUES ($tankNum,now(),ARRAY[$inputSQL]);";
                $res = $GLOBALS['pdo']->exec($sql);
                
            }

            //print_r($xml); //DEBUG
            print("Finish feedTankWeight for tank #:$tankNum\n");
            
        //Catch to show error for feed tank it will fail, but other tanks will work.    
        } catch (Exception $e) {
            echo 'Caught exception: ', $e->getMessage(), "\n";
            echo "SKIPPING TANK # $tankNum\n\r************************\n\r\n\r\n\r\n\r";
        }
    }

}//end class

/*
 * 
 * call from CLI like: php controlByWeb.php feedTankWeightMain
 */
if ((defined('STDIN')) AND empty($argv[1]) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    print("Error: Need to specify Params.\n\r");
} elseif ((defined('STDIN')) AND (!empty($argv[1]))AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    /* call different methods, based on first param */
    switch ($argv[1]) {
        case 'feedTankWeightMain':
            $cls = new ControlByWeb();
            $cls->feedTankWeightMain();
            break;
        case 'feedAugerUsageMain':
            $cls2 = new FeedAugerTimeLog();
            $cls2->feedAugerUsageMain();
            break;
        default:
            echo "Invalid parameter.\n";
    }
} else {
    //called form web, so nothing.
}


/*
 * feed auger reading control web dual relay III on/off state.
 * The data in DB will have on/off errors in it, so consider it fuzzy.
 * 
 */

class FeedAugerTimeLog {
    /* meant to run by cron job on a timer every minute or so. */

    public function feedAugerUsageMain() {

        $this->feedAugerDownloadLog('http://192.168.9.121/log.txt', 'Input01', 'I1', 11);
    }

    /* this looks for an ON event and if it find one, it look to the next event to be an OFF event, if
     * so and the date is greater then 2010, we store in DB. WAAAY simpler then other ways I tried.
     */

    private function feedAugerDownloadLog($url, $input_str, $input_str2, $tank_id) {
        //example $this->feedAugerDownloadLog('http://192.168.9.121/log.txt','Input01','I1');

        $ch = curl_init();    // initialize curl handle 
        curl_setopt($ch, CURLOPT_URL, $url); // set url  
        curl_setopt($ch, CURLOPT_FAILONERROR, 1);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1); // allow redirects 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); // return into a variable 
        curl_setopt($ch, CURLOPT_TIMEOUT, 30); // times out after 30s 
        $result = curl_exec($ch); // run the whole process 
        curl_close($ch);

        $unprossedArr = $GLOBALS['MiscObj']->parse_csv($result);

        //create an array where we only have input events, not NTP events, etc.
        $newArr = array();
        $count = 0;
        foreach ($unprossedArr as $key => $value) {
            if (strstr($value['Source'], $input_str) == true) {
                $newArr[$count] = $value;
                $count++;
            }
        }



        //stop 1 before end of array, because event in progress etc.
        for ($i = 0; $i < count($newArr) - 1; $i++) {

            //we have an ON event
            if (intval($newArr[$i]["$input_str2"]) == 1) {

                //look for matching OFF event as the next event
                if (intval($newArr[$i + 1]["$input_str2"]) == 0) {

                    //OK we have an ON/OFF pair, that's all we are interest in.
                    $tr1 = date('Y-m-d H:i:s', strtotime($newArr[$i]['Date Time']));
                    $tr2 = date('Y-m-d H:i:s', strtotime($newArr[$i + 1]['Date Time']));

                    //ignore bad 1970 data in log.
                    if (strtotime($tr1) > strtotime('2000-01-01')) {
                        $sql = "INSERT INTO bas.feed_auger_event_range (tank_id,event_range) VALUES ($tank_id,'[$tr1,$tr2]')  ON CONFLICT DO NOTHING";
                        $res = $GLOBALS['pdo']->exec($sql);
                    } else {
                        print("Ignoring 1970 data...\n");
                    }
                }
            }
        }
    }

    /** called by home page to see if feed auger was used today */
    public function todayFeedAugerUsage($tank_id) {
        $sql = "SELECT max(upper (event_range)) as auger_last_on,     extract('epoch' from sum(upper (event_range)-lower(event_range)) )      as auger_on_time_seconds FROM bas.feed_auger_event_range  WHERE upper (event_range) > current_date - interval '0 days' AND tank_id=$tank_id";
        $obj = $GLOBALS['pdo']->query($sql)->fetchAll(PDO::FETCH_OBJ);

        return $obj[0];
    }

    //shows error.
    public function error() {
        print("DONE"); //FIXME
        exit();
        ////////////////////////
        //see if auger has not been run in 2 days.
        $sql = "SELECT event_range FROM bas.feed_auger_event_range  WHERE upper (event_range) > current_date - interval '2 days'  LIMIT 1
	     ";
        $res = $GLOBALS['pdo']->query($sql);
        if (empty($row)) {

            $returnMessage = 'Feed Auger Sync has not been run in 2 days.';

            $error[] = new notifyObj(get_class($this), now(), 1, $returnMessage);
        }
        if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
    }

}

//end class
?>