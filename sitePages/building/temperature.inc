<?php

class Temperature extends BasePage {

    public $barnIP;
    public $parlourIP;

    public function __construct() {
        $this->barnIP = '192.168.9.150'; //ip address of x-332 main barn lights
        $this->parlourIP = '192.168.9.194'; //ip address of parlor x400
        $this->manurePitIP = '192.168.9.195'; //ip address of parlor x400
    }

    public function defaultDisplay() {
     
        print('<h3>The current barn temp is: ' . $this->getCurrentTemp_99Barn() . '</h3>');
        print('<h3>The current parlour temp is: ' . $this->getCurrentTemp_99Parlour() . '</h3>');
        print('<h3>The current manure pit temp is: ' . $this->getCurrentTemp_99ManurePit() . '</h3>');
        print('<hr>');
        print('<h3>The current hot water tank temp is: ' . $this->getCurrentTemp_99ParlourHotWaterTank() . '</h3>');
        print('<hr><br>');
        print($this->tempChartBarn());
        print($this->tempChartParlour());
        print($this->tempChartManurePit());
        print($this->waterLevelChartParlour());
        print($this->tempChartParlourHotWater());
    }

    
    public function tempChartBarn() {
        
        
         /* finds temp barn over time */
$sql = <<<SQL
SELECT event_time,(state->>'sensor1')::numeric as temperature
FROM bas.device_state_log 
WHERE device='x-332_99-barn' AND event_time >= (current_timestamp- interval '3 day') order by event_time DESC
SQL;
        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
               if ($row2['temperature']!='x.x'){ //it seems when control by web has an error it outputs x.x?
            $one[strtotime($row2['event_time'])] = ($row2['temperature']);         
               }
        }

        $out[]=("<h3>Dairy Barn Temperature</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Temp °C'] = 'number';
    

        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $one);
        $out[]=($x->toString());
        
        return implode($out);
    }
    
        public function tempChartParlour() {
        
        
         /* finds temp barn over time */
$sql = <<<SQL
SELECT event_time,(state->>'oneWireSensor2') as temperature
FROM bas.device_state_log 
WHERE device='x-400_99-parlour' AND event_time >= (current_timestamp- interval '3 day') order by event_time DESC
SQL;
        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            
            if ($row2['temperature']!='x.x'){ //it seems when control by web has an error it outputs x.x?
            $one[strtotime($row2['event_time'])] = ($row2['temperature']);     
            }    
        }

        $out[]=("<h3>Dairy Parlour Temperature</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Temp °C'] = 'number';
    

        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $one);
        $out[]=($x->toString());
        
        return implode($out);
    }
    
     public function tempChartParlourHotWater() {
        
        
         /* finds temp barn over time */
$sql = <<<SQL
SELECT event_time,(state->>'oneWireSensor1') as temperature
FROM bas.device_state_log 
WHERE device='x-400_99-parlour' AND event_time >= (current_timestamp- interval '3 day') order by event_time DESC
SQL;
        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            
            if ($row2['temperature']!='x.x'){ //it seems when control by web has an error it outputs x.x?
            $one[strtotime($row2['event_time'])] = ($row2['temperature']);     
            }    
        }

        $out[]=("<h3>Dairy Parlour Hot Water Tank Temperature</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Temp °C'] = 'number';
    

        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $one);
        $out[]=($x->toString());
        
        return implode($out);
    }
    
    
    public function tempChartManurePit() {
        
        
         /* finds temp barn over time */
$sql = <<<SQL
SELECT event_time,(state->>'oneWireSensor1') as temperature
FROM bas.device_state_log 
WHERE device='x-400_99-manurePit' AND event_time >= (current_timestamp- interval '3 day') order by event_time DESC
SQL;
        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            
            if ($row2['temperature']!='x.x'){ //it seems when control by web has an error it outputs x.x?
            $one[strtotime($row2['event_time'])] = ($row2['temperature']);     
            }    
        }

        $out[]=("<h3>Manure Pit Temperature</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Temp °C'] = 'number';
    

        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $one);
        $out[]=($x->toString());
        
        return implode($out);
    }
    
    
    public function waterLevelChartParlour() {
        
        
         /* finds temp barn over time */
$sql = <<<SQL
SELECT event_time,(state->>'analogInput1') as temperature
FROM bas.device_state_log 
WHERE device='x-400_99-parlour' AND event_time >= (current_timestamp- interval '3 day') order by event_time DESC
SQL;
        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
               if ($row2['temperature']!='x.x'){ //it seems when control by web has an error it outputs x.x?
            $one[strtotime($row2['event_time'])] = ($row2['temperature']);         
               }
        }

        $out[]=("<h3>Dairy Parlour Water Level</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Level'] = 'number';
    

        $x = new GoogleVisualizationLine( null, null, $headerStrArr, $one);
        $out[]=($x->toString());
        
        return implode($out);
    }
    
/*
 * control by web ip address.
 */
    public function getCurrentState($ip) {

        $ip = 'http://' . $ip . '/state.xml';

        $curl = curl_init();
// Set some options - we are passing in a useragent too here
        curl_setopt_array($curl, array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_URL => $ip,
            CURLOPT_USERAGENT => 'blahh'
        ));
        $resp = curl_exec($curl);

        curl_close($curl);

        $xml = simplexml_load_string($resp);
        $json = json_encode($xml);
        return $json;
    }

    public function getCurrentTemp_99Barn() {

        $json = $this->getCurrentState($this->barnIP);
        $data = json_decode($json);

        return $data->sensor1 . $data->units;
    }
    
    public function getCurrentTemp_99Parlour() {

        $json = $this->getCurrentState($this->parlourIP);
        $data = json_decode($json);

        return $data->oneWireSensor2 . '&#176;C';
    }
    
    public function getCurrentTemp_99ParlourHotWaterTank() {

        $json = $this->getCurrentState($this->parlourIP);
        $data = json_decode($json);

        return $data->oneWireSensor1 . '&#176;C';
    }
    
    public function getCurrentTemp_99ManurePit() {

        $json = $this->getCurrentState($this->manurePitIP);
        $data = json_decode($json);

        return $data->oneWireSensor1 . '&#176;C';
    }

}//end class