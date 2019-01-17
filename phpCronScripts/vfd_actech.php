<?php

/*
 * This is for accessing Letnz AC Tech VFD web interfaces
 */

//error_reporting(-1000);
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');

class Hammermill_VFD {

    //run by cronscript every minute or so.  
    public function log() {
        $address = 520;
        $vfd_ham = new VFD_ACTech('http://192.168.9.187/GSP.htm');
        $answer = $vfd_ham->queryAddress($address);
        $sql = "INSERT INTO bas.vfd_event (vfd_id,event_time,address_number,address_value) VALUES ('hamm',now(),$address,$answer)";
        $res = $GLOBALS['pdo']->exec($sql);
    }

    //info for a web page to display 
    public function hammermillRealtimeInfo() {
        $hammerMillRunning = new VFD_ACTech('http://192.168.9.187/GSP.htm');

           print("<h3>INFO:</h3>");
         $ans = $hammerMillRunning->queryAddress(507);
        print("<h4>Auger Motor Load %: $ans</h4>");
        
        $ans = $hammerMillRunning->queryAddress(520);
        print("<h4>520: 0-10 VDC Input: $ans</h4>");
        //
        $ans = $hammerMillRunning->queryAddress(527);
        print("<h4>527: Actual Output Frequency: $ans</h4>");
        //
       
        print("<hr>");
         print("<h3>DEBUG INFO:</h3>");
         
        print("<h4>Set Point 231: ".$hammerMillRunning->queryAddress(231)." Amps of 51 max</h4>");
        $ans207 = $hammerMillRunning->queryAddress(207);
        $ans208 = $hammerMillRunning->queryAddress(208);
        $ans209 = $hammerMillRunning->queryAddress(209);
        print("<h4>207/208/209: PID: $ans207 / $ans208 / $ans209</h4>");
        print("<br>");
        //
        $ans101 = $hammerMillRunning->queryAddress(101);
        $ans102 = $hammerMillRunning->queryAddress(102);
        $ans104 = $hammerMillRunning->queryAddress(104);
        $ans105 = $hammerMillRunning->queryAddress(105);
        $ans108 = $hammerMillRunning->queryAddress(108);
        $ans110 = $hammerMillRunning->queryAddress(110);
        $ans121 = $hammerMillRunning->queryAddress(121);
        print("<h4>101/102/104: PID: $ans101 / $ans102 / $ans104</h4>");
        print("<h4>105/108/110: PID: $ans105 / $ans108 / $ans110</h4>");
        print("<h4>121: PID: $ans121 </h4>");
        print("<hr>");
    }

}

//end class

/** allows querying of ACTECH VFD DRIVE. */
class VFD_ACTech {

    public $url;

    function __construct($url) {
        $this->url = $url;
    }

    //MAIN
    public function queryAddress($address) {
        $pageData = $this->curlCalltoSMVVector($this->url, $address);
        $answer = $this->parseSMVVectorForm($pageData, $address);

        //print("Answer is: $answer<br> "); //DEBUG
        return $answer;
    }

    private function curlCalltoSMVVector($url, $address) {
        $curl = curl_init();

        curl_setopt_array($curl, array(
            CURLOPT_URL => $url,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_ENCODING => "",
            CURLOPT_MAXREDIRS => 10,
            CURLOPT_TIMEOUT => 30,
            CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
            CURLOPT_CUSTOMREQUEST => "POST",
            CURLOPT_POSTFIELDS => "D900=$address&D901=0&FGSP=Read",
            CURLOPT_HTTPHEADER => array(
                "cache-control: no-cache",
                "content-type: application/x-www-form-urlencoded",
                "d900: $address",
                "d901: 0",
                "fgsp: Read",
                "extra-token: 00000000000000000000000000000000000000000000"
            ),
        ));

        $response = curl_exec($curl);
        $err = curl_error($curl);

        curl_close($curl);

        if ($err) {
            echo "cURL Error #:" . $err;
        } else {
            return $response;
        }
    }

    private function parseSMVVectorForm($pageContent, $address) {
        $dom = new DOMDocument();
        libxml_use_internal_errors(true);
        $dom->loadHTML($pageContent);
        //
        libxml_clear_errors();
        $xpath = new DOMXpath($dom);

        $dataArr = array();

        //search for form values with answer  
        $rowAddressArr = ($xpath->query('//input[@name="D900"]/@value[1]'));
        $rowAnswerArr = ($xpath->query('//input[@name="D901"]/@value[1]'));

        $rowAddress = trim($rowAddressArr[0]->textContent);
        $rowAnswer = trim($rowAnswerArr[0]->textContent);


        if ($rowAddress == $address) {
            return $rowAnswer;
        } else {
            throw new Exception("Adress not returned. Data invalid.");
        }
    }

}

//end class



if ((defined('STDIN')) AND empty($argv[1]) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    print("Error: Need to specify Params.\n\r");
} elseif ((defined('STDIN')) AND ( !empty($argv[1]))AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    /* call different methods, based on first param */
    switch ($argv[1]) {
        case 'vfdMain':
            $cls = new Hammermill_VFD();
            $cls->log();
            break;
        default:
            echo "Invalid parameter.\n";
    }
} else {
    //called form web, so nothing.
}
?>
