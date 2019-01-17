<?php 

/**
 * search trimble data dir for any shape files that can be imported and then 
 * proceeds to import them.
 *  
 */
$_SERVER['DOCUMENT_ROOT'] = '/var/www/int/'; //run from command line only.
require_once($_SERVER['DOCUMENT_ROOT'] . '/global.php');


$path = realpath('../local/dataFile/trimbleGPS');

coverageParser($path);
//rtfSummaryParser($path);


/*
 * 
 * grabs all the gps swaths and inserts into db.
 */

function coverageParser($path) {


    $objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path), RecursiveIteratorIterator::SELF_FIRST);
//now we have a list of everything recursively
    foreach ($objects as $name => $object) {
        $last_line = '';
        //look for shape files.
        if (strstr($name, 'Coverage.shp') == true) {  //change to .shp to get all data
            print("Name:$name\n");
            
            //get the parent directtory name, we will use this as the key for db events.
            $pArr=null;
            $keyValue=null;
            $pArr=(explode('/', $name, -1));
            $keyValue=$pArr[count($pArr)-2];
            //print("Key: $keyValue\n");
            ////////
            
           
            //now generate sql query for swaths
            $command = "/usr/bin/shp2pgsql -a $name";
            $last_line = shell_exec($command);

            //using the key value from above insert it into the sql query. 
            //this is not robust at all and if the format ever changes this will have to be changed.
            $last_line=str_replace('"version","gps_status"','"event","version","gps_status"',$last_line);
            $last_line=str_replace(') VALUES (',") VALUES ('$keyValue',",$last_line);
            
            
            
            //replace text to inset schema name
            // $last_line=str_replace('swaths', 'gis.swaths', $last_line);
            //show sql query.
            print($last_line);
            print("\n\r" . "\n\r" . "\n\r" . "--------------------------------------------" . "\n\r" . "\n\r" . "\n\r" . "\n\r");


            //execure sql query
            //import into db.
            
            $res = $GLOBALS['pdo']->exec($last_line);
        }
    }
}

/**
 * main function grab data out of summary file that we want and insert into db. 
 */
function rtfSummaryParser($path) {

    //drop everything from db table.
    $sql = "TRUNCATE gis.summary";
    $res = $GLOBALS['pdo']->exec($sql);


    $objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path), RecursiveIteratorIterator::SELF_FIRST);
//now we have a list of everything recursively
    foreach ($objects as $name => $object) {
        $last_line = '';
        //look for shape files.
        if (strstr($name, 'EventSummary.rtf') == true) {
            print("Name: $name\n");
            //read in file
            $rtfStr = file_get_contents($name, true);
            //print("File: $rtfStr\n");
            //get lat/long of file
            $latLongArr = matchLatLong($rtfStr);
            //print("LAT/LONG:\n");
            //print_R($latLongArr);


            //get everything else
            $matchArr = matchEverything($rtfStr);
            $outArr = array();
            foreach ($matchArr[1] as $key => $value) {
                $oArr["$value"] = $matchArr[2][$key];
            }

            //print_r($oArr);
//now insert into db table.
            //total data available
            /*
              Client Name] => Default_Client
              [Farm Name] => Default_Farm
              [Field Name] => 051413_0002_EZ06543
              [Event Name] => Event_051413_0001_EZ06543
              [Operator] =>
              [Applied Material] =>
              [Event Created] =>  05 14 13:36:19 2013
              [Summary Created] =>  05 14 15:13:35 2013
              [Field Area] => 2.3258 ha
              [Productive Area] => 2.3258 ha
              [Total Time] => 01h 37m 16.0s
              [Operator EPA license] =>
              [Harvest Year] =>
              [Farm Location] =>
              [Crop] =>
              [Target Pests] =>
              [Application Method] =>
              [Start Time] =>  05 14 13:36:21 2013
              [End Time] =>  05 14 15:11:15 2013
              [Coverage Time] => 01h 27m 11.53s
              [Coverage Area] => 2.4404 ha
              [Applied Volume] => N/A
              [Vehicle] =>
              [Implement] =>
              [Implement Width] => 3.048 m
              [Number of Sections] => N/A
              [Soil Conditions] =>
              [Soil Type] =>
              [Temperature] => 0.0 \'b0C
              [Humidity] => 0%
              [Wind Speed] => 0.0 kph
              [Wind Direction] =>
              [Wind Gust Speed] => 0.0 kph
              [Sky Conditions] =>
              [Custom1] =>
              [Custom2] =>
              [Custom3] =>
              [Custom4] =>
             */


            //data we store
            /*
              ] => Default_Client
              [] => Default_Farm
              [] => 051413_0002_EZ06543
              [] => Event_051413_0001_EZ06543
              [] =>  05 14 13:36:19 2013
              [] =>  05 14 15:13:35 2013
              [] => 2.3258 ha
              [] => 2.3258 ha
              [] => 01h 37m 16.0s
              [] =>  05 14 13:36:21 2013
              [] =>  05 14 15:11:15 2013
              [] => 01h 27m 11.53s
              [] => 2.4404 ha
              [] => 3.048 m
             */

            $oArr['Client Name'] = pg_escape_string($oArr['Client Name']);
            $oArr['Farm Name'] = pg_escape_string($oArr['Farm Name']);
            $oArr['Field Name'] = pg_escape_string($oArr['Field Name']);
            $oArr['Event Name'] = pg_escape_string($oArr['Event Name']);
            $oArr['Event Created'] = strtotimeNull($oArr['Event Created']);
            $oArr['Summary Created'] = strtotimeNull($oArr['Summary Created']);
            $oArr['Field Area'] = $oArr['Field Area'] + 0;
            $oArr['Productive Area'] = $oArr['Productive Area'] + 0;
            $oArr['Total Time'] = strtotimeNull($oArr['Total Time']);
            $oArr['Start Time'] = strtotimeNull($oArr['Start Time']);
            $oArr['End Time'] = strtotimeNull($oArr['End Time']);
            $oArr['Coverage Time'] = strtotimeNull($oArr['Coverage Time']);
            $oArr['Coverage Area'] = $oArr['Coverage Area'] + 0;
            $oArr['Implement Width'] = $oArr['Implement Width'] + 0;

            // print_r($oArr);

            $sql = "INSERT INTO gis.summary (client_name,farm_name,field_name,event_name,event_created,summary_created,field_area,productive_area,total_time,start_time,end_time,coverage_time,coverage_area,implement_width,geo_point) VALUES ('{$oArr['Client Name']}','{$oArr['Farm Name']}','{$oArr['Field Name']}','{$oArr['Event Name']}','{$oArr['Event Created']}','{$oArr['Summary Created']}',{$oArr['Field Area']},{$oArr['Productive Area']},'{$oArr['Total Time']}','{$oArr['Start Time']}','{$oArr['End Time']}','{$oArr['Coverage Time']}',{$oArr['Coverage Area']},{$oArr['Implement Width']},gis.ST_SetSRID(gis.ST_MakePoint({$latLongArr['lon']}, {$latLongArr['lat']}), 4326) )";
            print("\n" . $sql . "\n");

            $res = $GLOBALS['pdo']->exec($sql);
            {
                echo $res->getMessage();
            }
        }
    }
}

//match the field from the rtf file, they all follow same format.
function matchEverything($messageBody) {

    //four backslahses used to reprensent one backslash.
    $a = preg_match_all("/{\\\\f0\\\\fs20\s(?<field>[\w\s]*)}\n\\\\cell\n\\\\pard\\\\intbl\n{\\\\f0\\\\fs20\s(?<field2>.*)}\n/", $messageBody, $matches);

//print_r($matches);

    return $matches;
}

//this is a special case, so needs its won method to get lat/lon
function matchLatLong($messageBody) {
    $oArr = array();
    /*
      {\f0\fs20 Field Latitude/Longitude}
      \cell
      \pard\intbl
      {\f0\fs20 45\'b059'7.33"N 64\'b058'34.25"W}
     */
    preg_match("/{\\\\f0\\\\fs20\sField\sLatitude\/Longitude}\n\\\\cell\n\\\\pard\\\\intbl\n{\\\\f0\\\\fs20\s(?<lat1>[\d][\d])\\\\'b0(?<lat2>[\d][\d])'(?<lat3>-?[0-9]+(?:\.[0-9]+)?)\"(?<lat4>[N|S])\s(?<lon1>[\d]{2,3})\\\\'b0(?<lon2>[\d][\d])'(?<lon3>-?[0-9]+(?:\.[0-9]+)?)\"(?<lon4>[E|W])}\n/", $messageBody, $matches);

    //when nothing was found, make it 0.
    if ($matches==null) {
        $oArr['lat'] =0; 
         $oArr['lon'] =0;
    }
    else {
    
    if ($matches['lat4'] == 'N') {
        $latSign = 1;
    } elseif ($matches['lat4'] == 'S') {
        $latSign = -1;
    } else {
        throw new Exception("Error: Can't convert lat, no N or S direction.\n");
    }

    if ($matches['lon4'] == 'E') {
        $lonSign = 1;
    } elseif ($matches['lon4'] == 'W') {
        $lonSign = -1;
    } else {
        throw new Exception("Error: Can't convert lat, no N or S direction.\n");
    }

    //convert to decimal
    //Decimal Degrees = Degrees + minutes/60 + seconds/3600
    $oArr['lat'] = ($matches['lat1'] + $matches['lat2'] / 60 + $matches['lat3'] / 3600) * $latSign;
    $oArr['lon'] = ($matches['lon1'] + $matches['lon2'] / 60 + $matches['lon3'] / 3600) * $lonSign;
    }
      
    return $oArr;
}

//fix trimbles stupid timestamp format to something we can read
function strtotimeNull($timestamp) {
    $out = null;
    //convert stupid trimble timestamps to real ones.
    //05 02 12:46:39 2010
    preg_match("/(?<month>[\d][\d])\s(?<day>[\d][\d])\s(?<time>(([0-1][0-9])|([2][0-3])):([0-5][0-9]):([0-5][0-9]))\s(?<year>[\d][\d][\d][\d])/", trim($timestamp), $matches);
    if ($matches != null) {
        $out = $matches['day'] . '-' . $matches['month'] . '-' . $matches['year'] . ' ' . $matches['time'];
        $out = date('c', strtotime($out));
        return $out;
    }

    //03h 56m 37.0s
    preg_match("/(?<hour>[\d][\d])h\s(?<minute>[\d][\d])m\s(?<second>-?[0-9]+(?:\.[0-9]+)?)s/", trim($timestamp), $matches);
    if ($matches != null) {
        $out = $matches['hour'] . ':' . $matches['minute'] . ':' . $matches['second'];
        return $out;
    }

    //do null check 
    if (($timestamp == null) OR ($timestamp == '')) {
        $out = null;
        return $out;
    }

    throw new Exception("Out is not null for timestamp, should never get here...$timestamp.\n\r");
}

?>
