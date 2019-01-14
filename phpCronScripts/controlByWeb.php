<?php 

/*
 * This is for control by web at feed tank 1 and grey barn.
 * This is also for Feed tank 4 raspberry pi, emulating control by web XML format
 */

//error_reporting(-1000);
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}



/* this code downloads xml information from various control by web sources around the farm and inserts into db */

class ControlByWeb {

    
    /* meant to run by cron job on a timer every minute or so. */

    public function feedTankWeightMain() {
        $this-> feedTankWeight(1, "http://192.168.9.188/state.xml"); //control by web
        $this-> feedTankWeight(4, "http://192.168.9.191/index.php"); //ADC
        $this-> feedTankWeight(0, "http://192.168.9.192/index.php"); //open scale
    }
    
    /**
     * 
     *   call directly to measure feed tankk weights.
     *   control by web or raspberry pi's used (that emulate control by web ).
     */
    private function feedTankWeight($tankNum, $url) {
  print("Start feedTankWeight for tank #:$tankNum\n");

  
  
  
    
//$sxml = simplexml_load_file($url);
//var_dump($sxml);
//$xml = new SimpleXMLElement(file_get_contents($url));
// then you can do
//var_dump($xml);



        $ch = curl_init();    // initialize curl handle 
        curl_setopt($ch, CURLOPT_URL, $url); // set url  
        curl_setopt($ch, CURLOPT_FAILONERROR, 1);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1); // allow redirects 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); // return into a variable 
        curl_setopt($ch, CURLOPT_TIMEOUT, 3); // times out after 4s 
        $result = curl_exec($ch); // run the whole process 
        curl_close($ch);
print($result);

        $xml = new SimpleXMLElement($result);

        
        //HACKish, dynamically create sql, because one tanke it is inputs 1234 and next it is 5678.
        switch ($tankNum) {
   case 0:
         $inputSQL= "{$xml->input0state}";
         break;
   case 1:
         $inputSQL= "{$xml->input4state},{$xml->input5state},{$xml->input6state},{$xml->input7state}";
         break;
   case 4:
         $inputSQL= "{$xml->input0state},{$xml->input1state},{$xml->input2state},{$xml->input3state},{$xml->input4state},{$xml->input5state},{$xml->input6state},{$xml->input7state}";
         break;
}
        
        
        if ((is_numeric(trim($xml->input0state))) AND ( is_numeric(trim($xml->input1state))) AND ( is_numeric(trim($xml->input2state))) AND ( is_numeric(trim($xml->input3state))) AND ( is_numeric(trim($xml->input4state)))  AND ( is_numeric(trim($xml->input5state))) AND ( is_numeric(trim($xml->input6state))) AND ( is_numeric(trim($xml->input7state)))  ) {

            $sql = "INSERT INTO bas.feed_bin_weight_event (tank_id,event_time,cells)
VALUES ($tankNum,now(),ARRAY[$inputSQL]);";
            $res = $GLOBALS['pdo']->exec($sql);
            {
                print_r($res);
              
            }
        }

        //print_r($xml); //DEBUG
        print("Finish feedTankWeight for tank #:$tankNum\n");
    }

}//end class

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
    
       $this->feedAugerDownloadLog('http://192.168.9.121/log.txt', 'Input01', 'I1',11);
 
    }
    
    /* this looks for an ON event and if it find one, it look to the next event to be an OFF event, if
     * so and the date is greater then 2010, we store in DB. WAAAY simpler then other ways I tried.
     */
    private function feedAugerDownloadLog($url, $input_str, $input_str2,$tank_id) {
        //example $this->feedAugerDownloadLog('http://192.168.9.121/log.txt','Input01','I1');

        $ch = curl_init();    // initialize curl handle 
        curl_setopt($ch, CURLOPT_URL, $url); // set url  
        curl_setopt($ch, CURLOPT_FAILONERROR, 1);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1); // allow redirects 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); // return into a variable 
        curl_setopt($ch, CURLOPT_TIMEOUT, 30); // times out after 30s 
        $result = curl_exec($ch); // run the whole process 
        curl_close($ch);

        $unprossedArr = Misc::parse_csv($result);
        
        //create an array where we only have input events, not NTP events, etc.
        $newArr=array();
        $count=0;
          foreach ($unprossedArr as $key => $value) {
      if (strstr($value['Source'], $input_str) == true) {       
          $newArr[$count]=$value;
          $count++;
          }}
        
        

        //stop 1 before end of array, because event in progress etc.
        for ($i = 0; $i < count($newArr) - 1; $i++) {

            //we have an ON event
            if (intval($newArr[$i]["$input_str2"]) == 1) {

                //look for matching OFF event as the next event
                if (intval($newArr[$i + 1]["$input_str2"]) == 0) {

                    //OK we have an ON/OFF pair, that's all we are interest in.
                    $tr1 = date('Y-m-d H:i:s', strtotime($newArr[$i]['Date Time']));
                    $tr2 = date('Y-m-d H:i:s', strtotime($newArr[$i+1]['Date Time']));

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
        $sql="SELECT max(upper (event_range)) as auger_last_on,     extract('epoch' from sum(upper (event_range)-lower(event_range)) )      as auger_on_time_seconds FROM bas.feed_auger_event_range  WHERE upper (event_range) > current_date - interval '0 days' AND tank_id=$tank_id";
        $obj=$GLOBALS['pdo']->query($sql)->fetchAll(PDO::FETCH_OBJ);
       
        return $obj[0];      
    }
    
    //shows error.
    public function error() {
        print("DONE");//FIXME
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
    
}//end class

?>