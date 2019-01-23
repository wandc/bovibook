<?php

/*
 * Misc functions used throught the website
 */


/* eventually move all misc functions to this class, so they are explicit. */

class Misc {

    //used to create a limited list of hours from the last day.
    function createHourlyDatesForLastDay() {
//create
//day of the week for today, yesterday, and use unix epoch as index..

        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j >= -23; $j--) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    public static function createListOfAllYoungCalves() {
        // grab a list of calves in calf barn location and calving pen
        $calfNumberArr[null] = "Please Select one..."; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovineall WHERE location_id IN(64,65,66,67,68) ORDER BY local_number");
        while (($row = $res->fetch())) {
            $calfNumberArr[$row[0]] = $row[2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
        }
        return $calfNumberArr;
    }

    //returns time offset from start time.
    //used by repro protocols
    public function timeOffset($start_time, $offset) {
//starttime is already in epoch formula
        $date = new DateTime($start_time);
        $date->modify($offset);
        return $date->format('c');
    }

    //turn csv string into array.
//from: http://ca.php.net/manual/en/function.str-getcsv.php
    public static function parse_csv($str, $options = null) {
        $res = null;
        $delimiter = empty($options['delimiter']) ? "," : $options['delimiter'];
        $to_object = empty($options['to_object']) ? false : true;
        $expr = "/,(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))/"; // added
        $lines = explode("\n", $str);
        $field_names = explode($delimiter, array_shift($lines));
        $field_names = array_map('trim', $field_names); //trim field names.
        foreach ($lines as $line) {
            // Skip the empty line
            if (empty($line))
                continue;
            $fields = preg_split($expr, trim($line)); // added
            $fields = preg_replace("/^\"(.*)\"$/", "$1", $fields); //added
            $_res = $to_object ? new stdClass : array();
            foreach ($field_names as $key => $f) {
                if ($to_object) {
                    $_res->{$f} = $fields[$key];
                } else {
                    $_res[$f] = $fields[$key];
                }
            }
            $res[] = $_res;
        }
        return $res;
    }

    //used to create a limited list of hours from the next 24 hours (1 day).
    function createHourlyDatesForNexttDay() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = 0; $j <= 23; $j++) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

//used to create a limited list of hours only from midnight
    function createHourlyDatesForOnlyToday() {
//start at midnight and go to the current time.
        $midnight = strtotime(date('Y-m-d 00:00:00', strtotime("now")));
        $now = strtotime(date('Y-m-d H:00:00', strtotime("now"))); //nearest hour
        $hours = round(($now - $midnight) / 3600);

//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }

        for ($j = $hours; $j >= 0; $j--) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours", $midnight)))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    function createHourlyDatesForLastFourDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j >= -95; $j--) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    function createHourlyDatesForLastSevenDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j >= -169; $j--) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    function createHourlyDatesForLastTwoDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j >= -47; $j--) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    function createHourlyDatesForNextSevenDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j <= 168; $j++) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    function createHourlyDatesForNextThirtyDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j <= 720; $j++) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    function createDailyDatesForLastandNextFewDays() {
//create

        for ($j = 2; $j >= -2; $j--) {
            $timeIndex = strtotime(date('Y-m-d 00:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l (M d)", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }
        return $dateArray;
    }

//used to create a list of dates going from today back three days.
    function createDailyDatesForLastFewDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j >= -7; $j--) {
            $timeIndex = strtotime(date('Y-m-d 00:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l (M d)", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

//used to create a list of hourly intervals going from today back seven days.
    function createDailyDatesForLastSevenDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) != TRUE)) { //don't have a null value for owners.
            $dateArray[null] = "Please Select one..."; //dummy value
        }
        for ($j = 0; $j >= -166; $j--) {
            $timeIndex = strtotime(date('Y-m-d H:00:00', date(strtotime("$j hours")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l - d @ H:00", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

//used to create a list of daily intervals going from today back seven days.
    function createDailyDatesAtDailyIntervalsForLastSevenDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = 0; $j >= -8; $j--) {
            $timeIndex = strtotime(date('Y-m-d 12:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l, M j", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

//used to create a list of daily intervals going forward and back for 8 days.
    function createDailyDatesAtDailyIntervalsForNextNineAndSomePrevDays() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = -3; $j <= 9; $j++) {
            $timeIndex = strtotime(date('Y-m-d 12:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("l, M j", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

//used to create a list of daily dates going back a year. The time is set to noon. Usually used for entering old data.
    function createDailyDatesForLastYearAtNoon() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = 0; $j >= -1365; $j--) {
            $timeIndex = strtotime(date('Y-m-d 12:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("M d, Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

    //used to create a list of daily dates going back a year. The time is set to midnight. Usually used for entering old data.
    function createDailyDatesForLastYearAtMidnight() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = 0; $j >= -1365; $j--) {
            $timeIndex = strtotime(date('Y-m-d 0:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("M d, Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }
    
//used to create a list of daily dates for the last 6 months and next 6 months
    function createDailyDatesForNextNineMonthsPrevSixMonthsAtNoon() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = -180; $j <= 275; $j++) {
            $timeIndex = strtotime(date('Y-m-d 12:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("M d, Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }


        return $dateArray;
    }

//used to create a list of daily datesfor past 7 and next 14 days
    function createDailyDatesForPast7Next14DaysAtNoon() {
//create
//day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = -7; $j <= 14; $j++) {
            $timeIndex = strtotime(date('Y-m-d 12:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay = date("M d, Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }

        return $dateArray;
    }

//used to create a list of daily dates for next 12 days at midnight and noon. used for the sort gate.
//used for sort gate
    function createDailyDatesForNext12DaysAtMidnightAndNoon() {
//create day of the week for today, yesterday, and use unix epoch as index..
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($j = 0; $j <= 12; $j++) {
            $timeIndex1x = strtotime(date('Y-m-d 00:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay1x = date("l - M d A", $timeIndex1x);
            $dateArray[$timeIndex1x] = $timeDisplay1x;

            $timeIndex2x = strtotime(date('Y-m-d 12:00:00', date(strtotime("$j days")))); //rounds timeIndex to nearest hour.
            $timeDisplay2x = date("l - M d A", $timeIndex2x);
            $dateArray[$timeIndex2x] = $timeDisplay2x;
        }

        return $dateArray;
    }

//create 1st of the month dates for the previous 12 months
    function createMonthlyDatesForPrev12Months() {
        $current_month = date('n');
        $dateArray[null] = "Please Select one..."; //dummy value
        $numMonths = 140;
        for ($m = $numMonths; $m >= 1; $m--) {
            $display_month = $m - $numMonths + $current_month;
            $timeIndex = mktime(0, 0, 0, $display_month, 1, date("Y"));
            $timeDisplay = date("M Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }
        return $dateArray;
    }

//create 1st of the month dates for the previous 56 months
    function createMonthlyDatesForPrev56Months() {
        $current_month = date('n');
        $dateArray[null] = "Please Select one..."; //dummy value
        $numMonths = 56;
        for ($m = $numMonths; $m >= 1; $m--) {
            $display_month = $m - $numMonths + $current_month;
            $timeIndex = mktime(0, 0, 0, $display_month, 1, date("Y"));
            $timeDisplay = date("M Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }
        return $dateArray;
    }

//create 1st of the month dates for the previous 2 months and next 2 months.   
//This is written with mktime because there seems to be a bug in +1 month if it is towards the end of the month (ie 30 days), doesn't work correctly and skips months.
    function createMonthlyDatesForNextAndPrevFewMonths() {
        $current_month = date('n');
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($m = -2; $m < 3; $m++) {
            $display_month = $m + 1 + $current_month;
            $timeIndex = mktime(0, 0, 0, $display_month, 1, date("Y"));
            $timeDisplay = date("M jS, Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }
        return $dateArray;
    }

    function createMonthlyDatesForNextAndPrev12Months() {
        $current_month = date('n');
        $dateArray[null] = "Please Select one..."; //dummy value
        for ($m = -12; $m < 14; $m++) {
            $display_month = $m + 1 + $current_month;
            $timeIndex = mktime(0, 0, 0, $display_month, 1, date("Y"));
            $timeDisplay = date("M jS, Y", $timeIndex);
            $dateArray[$timeIndex] = $timeDisplay;
        }
        return $dateArray;
    }

//last day of the month
    function createMonthlyDatesLastDayMonthForNextAndPrevManyMonths() {

        $dateArray[null] = "Please Select one..."; //dummy value
        for ($m = 1; $m > -15; $m--) {
            $timeDisplay = date("M t, Y", strtotime("$m month"));
            $timeIndex = strtotime($timeDisplay);
            $dateArray[$timeIndex] = $timeDisplay;
        }
        return $dateArray;
    }

    /*
     * lists of cows
     * 
     */

    function createListOfAllAliveCows($animalQuery = false) {
        $aniNumber[null] = "Please Select one..."; //dummy value
        if ($animalQuery == true) {
            $aniNumber[null] = null;
        } //special case for at top of page.
//grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovinecurrall ORDER BY local_number");

        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $preg = preg_split("/[\s,]+/", $row[1]);
            $aniNumber[$row[0]] = $row[2] . " - " . end($preg);
        }
        return $aniNumber;
    }

    function createListOfAllAliveCowsFemale($animalQuery = false) {
        $aniNumber[null] = "Please Select one..."; //dummy value
        if ($animalQuery == true) {
            $aniNumber[null] = null;
        } //special case for at top of page.
//grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovinecurr ORDER BY local_number");

        while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
            $preg = preg_split("/[\s,]+/", $row[1]);
            $aniNumber[$row[0]] = $row[2] . " - " . end($preg);
        }
        return $aniNumber;
    }

    function createListOfAllCowsMilking($sireRegNum) {

        //break up the reg number.
        $regArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($sireRegNum);
        //NOTE: do not change & to &amp; here, breaks other things.
        return "https://www.cdn.ca/query/detail_ge.php?breed={$regArray['breed']}&country={$regArray['country']}&sex={$regArray['sex']}&regnum={$regArray['number']}";
    }

//break up a registration number.
    function breakUpFullRegNumber($full_reg_number) {

        $full_reg_number = trim($full_reg_number); //clear any excess spaces

        $full_reg_number = strtoupper($full_reg_number); //make sure it is capitlaized
//break into component parts:
        $returnArray['full_reg_number'] = $full_reg_number;
        $returnArray['breed'] = substr($full_reg_number, 0, 2);
        $returnArray['country'] = substr($full_reg_number, 2, 3);
        $returnArray['sex'] = substr($full_reg_number, 5, 1);
        $returnArray['number'] = substr($full_reg_number, 6, strlen($full_reg_number));

        $status = true; //default is true
//AY Ayrshire
//BS Brown Swiss
//CN Canadienne
//GU Guernsey
//HO Holstein
//JE Jersey
//MS Milking Shorthorn
//NR Norwegian Red
        if ($returnArray['breed'] != ('AY' || 'BS' || 'CN' || 'GU' || 'HO' || 'JE' || 'MS' || 'NR')) {
            $status = false;
        }
        /*
          CAN Canada
          USA United States
          840 United States
          ARG Argentina
          AUS Australia
          AUT Austria
          BEL Belgium
          BGR Bulgaria
          HRV Croatia
          CZE Czech Republic
          DNK Denmark
          EST Estonia
          FIN Finland
          FRA France
          DEU Germany
          GRC Greece
          HUN Hungary
          IRL Ireland
          IJE Isle of Jersey
          ISR Israel
          ITA Italy
          JPN Japan
          LVA Latvia
          LTU Lithuania
          LUX Luxembourg
          MEX Mexico
          NLD Netherlands
          NZL New Zealand
          NOR Norway
          POL Poland
          PRT Portugal
          ROM Romania
          RUS Russia
          SVK Slovak Republic
          SVN Slovenia
          ZAF South Africa
          ESP Spain
          SWE Sweden
          CHE Switzerland
          GBR United Kingdom
          YUG Yugoslavia
         */
        if ($returnArray['country'] != ('CAN' || 'USA' || '840' || 'ARG' || 'AUS' || 'AUT' || 'BEL' || 'BGR' || 'HRV' || 'CZE' || 'DNK' || 'EST' || 'FIN' || 'FRA' || 'DEU' || 'GRC' || 'HUN' || 'IRL' || 'IJE' || 'ISR' || 'ITA' || 'JPN' || 'LVA' || 'LTU' || 'LUX' || 'MEX' || 'NLD' || 'NZL' || 'NOR' || 'POL' || 'PRT' || 'ROM' || 'RUS' || 'SVK' || 'SVN' || 'ZAF' || 'ESP' || 'SWE' || 'CHE' || 'GBR' || 'YUG')) {
            $status = false;
        }

        if ($returnArray['sex'] != ('M' || 'F')) {
            $status = false;
        }

        if ((strlen($returnArray['number']) < 3) || (strlen($returnArray['number']) > 12) || (is_numeric($returnArray['number']) == false)) {
            $status = false;
        }


//if we have an error return false, else return array.
        if ($status == false) {
            return false;
        } else {
            return $returnArray;
        }
    }

//generates a somewhat random number mostly based on the current time. This is used to create a hopefully 
// unique transaction id for the DB, so it can be later tracked and undone if necessary.
    function generatePseudoRandomTransactionInteger() {
        $epoch = time();
        $random = rand(1000, 9999);
        $returnSting = $epoch . $random;
        return $returnSting;
    }

    /** generates uuid according to RFC4122. 
      /** If we did this alot, use postgres extension.
      /** There are faster functions in php too. */
    function uuid_create() {
        return sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x', mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0x0fff) | 0x4000, mt_rand(0, 0x3fff) | 0x8000, mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff));
    }

    function femaleShortName($longName) {
        $tt = preg_split("/[\s,]+/", $longName);
        return end($tt);
    }

//get some stats on how many cows are milking right now.
    function milkingCowCount() {

        $returnArray['countHigh'] = null;
        $returnArray['countLow'] = null;
        $returnArray['countHeifer'] = null;
        $returnArray['countFresh'] = null;
        $returnArray['countTotal'] = null;
        $returnArray['dimMiklkingTotal'] = null;

        //use locations_total view to get group counts.
        $res = $GLOBALS['pdo']->query('SELECT location_id,location_total,dim FROM bovinemanagement.location_stats');
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            switch ($row['location_id']) {
                case 40:
                    $returnArray['countHigh'] = $row['location_total'];
                    $dim_high = $row['dim'];
                    break;
                case 41:
                    $returnArray['countLow'] = $row['location_total'];
                    $dim_low = $row['dim'];
                    break;
                case 42:
                    $returnArray['countHeifer'] = $row['location_total'];
                    $dim_heifer = $row['dim'];
                    break;
                case 39:
                    $returnArray['countFresh'] = $row['location_total'];
                    break;
            }
        }

        $returnArray['countTotal'] = $returnArray['countHigh'] + $returnArray['countLow'] + $returnArray['countHeifer']; //all milk in tank groups.
        //milking dim total
        $returnArray['dimMiklkingTotal'] = $dim_high / $returnArray['countHigh'] + $dim_low / $returnArray['countLow'] + $dim_heifer / $returnArray['countHeifer'];

        return $returnArray;
    }

//really simply function to return the href part of a link to a cows record. This makes it easy to only change the page # in one spot.
    function hrefToBovinePage($bovineid) {
        return "/index.php?pageid=6&bovine_id=$bovineid";
    }
    
    function hrefToBovinePageFullName($bovine_id,$local_number,$full_name) {
        $t=preg_split("/[\s,]+/", $full_name); // this must be this way for pass by reference error. limnitation of the lanbguage. 
    return "<a href=\"" . self::hrefToBovinePage($bovine_id) . "\"> <b>$local_number</b> - " . end($t) . "</a>";
    }

    function hrefToFieldPage($field_id) {
        return "/index.php?pageid=107&field_id=$field_id";
    }

//same as from gis.littleriver
    /*
      calculates area from x,y,z x,y,z corrdinates.
     */
    function areaLatLonInHectares($perimeterCoordinates) {
//echo("<p>$perimeterCoordinates</p>");
//split up lat/lon into x and y arrays of points.
        $xy = preg_split('/,0 /', $perimeterCoordinates);
        $count = 0;
        foreach ($xy as $val) {
            list($xTemp, $yTemp) = explode(",", $val);
            $x[$count] = $xTemp;
            $y[$count] = $yTemp;
            $count++;
        }
//print_r($x);
//print_r($y);
//normalize each to meters, assumes 46 degrees north.
//Length Of A Degree Of Latitude In Meters 111151.29
//Length Of A Degree Of Longitude In Meters 77463.27
//list($xy) = split(",0", $perimeterCoordinates,-1);
//print_r($xy);
//echo "<br>size of array = ".sizeof($xy)."<br>";
//Gauss-Green Formula 
//http://www.ma.iup.edu/projects/CalcDEMma/Green/Green.html
        $sum = 0;
        for ($i = 0; $i <= count($x); $i++) {
            $tempEq = 0.5 * ($x[$i + 1] + $x[$i]) * 77463.27 * ($y[$i + 1] - $y[$i]) * 111151.29 - 0.5 * ($y[$i + 1] + $y[$i]) * 77463.27 * ($x[$i + 1] - $x[$i]) * 111151.29;
//echo("<br/>yes: $tempEq");
            $sum = $sum + $tempEq;
        }
        $final = 0.5 * $sum * .0001; //convert to hectares from meters (.0001 factor)
//echo("<p>Area in hectares $final</p>");
        return $final;
    }

//usually used for postgres because of what it returns. //NULL False
    function convertTFtoTrueFalse($var) {
        if ($var == 't') {
            return 'true';
        } elseif ($var == 'f') {
            return '';
        } else {
            return null;
        }
    }

    /** converts time to either days ago or hours ago if it is that day */
    function daysOrHoursAgo($time) {
        if (date('Y-m-d', strtotime('now')) == date('Y-m-d', $time)) { //then it is todays date
            $hours = round((strtotime('now') - $time) / 3600);
            if ($hours < 0) { // it is in the futre)
                $str = "<b id='boldBlue'>" . '+' . abs($hours) . "hours</b>";
            } else {
                $str = "<b id='boldRed'>$hours hours</b>";
            }
        } else { //it is some other date in the past (OR future);
            $tt = round(( strtotime(date('Y-m-d', strtotime('now'))) - strtotime(date('Y-m-d', $time)) ) / 86400);

            if ($tt < 0) { // it is in the futre)
                $str = '<b id="boldBlue">' . '+' . abs($tt) . " days</b>";
            } else {
                $str = $tt . " days";
            }
        }
        return $str;
    }

//from: http://stackoverflow.com/questions/13818064/check-if-an-ip-address-is-private
    function isPublicAddress($ip) {

        //Private ranges...
        //http://www.iana.org/assignments/iana-ipv4-special-registry/
        $networks = array('10.0.0.0' => '255.0.0.0', //LAN.
            '172.16.0.0' => '255.240.0.0', //LAN.
            '192.168.0.0' => '255.255.0.0', //LAN.
            '127.0.0.0' => '255.0.0.0', //Loopback.
            '169.254.0.0' => '255.255.0.0', //Link-local.
            '100.64.0.0' => '255.192.0.0', //Carrier.
            '192.0.2.0' => '255.255.255.0', //Testing.
            '198.18.0.0' => '255.254.0.0', //Testing.
            '198.51.100.0' => '255.255.255.0', //Testing.
            '203.0.113.0' => '255.255.255.0', //Testing.
            '192.0.0.0' => '255.255.255.0', //Reserved.
            '224.0.0.0' => '224.0.0.0', //Reserved.
            '0.0.0.0' => '255.0.0.0');       //Reserved.
        //inet_pton.
        $ip = @inet_pton($ip);
        if (strlen($ip) !== 4) {
            return false;
        }

        //Is the IP in a private range?
        foreach ($networks as $network_address => $network_mask) {
            $network_address = inet_pton($network_address);
            $network_mask = inet_pton($network_mask);
            assert(strlen($network_address) === 4);
            assert(strlen($network_mask) === 4);
            if (($ip & $network_mask) === $network_address)
                return false;
        }

        //Success!
        return true;
    }

}//end class


//
////
//
//
//



// adds a time and string to an array.
//used for bovine query and field query
function makeTimeString($outArray, $time, $str) {

    if (empty($time)) {
        throw new Exception('Error: empty time .');
    }

    if (array_key_exists($time, $outArray) == true) {
        $time = $time + 1; // create new array key and try again
        $outArray = makeTimeString($outArray, $time, $str); // recursive
        // call until events
        // don't have the same
        // time.
    } else {
        $outArray [$time] = $str; // add to array
    }


    return $outArray;
}

//used when bovine pic is uploaded.
/** taken from php.net docs for gd, requires gd */

/** apparently this is super slow and can be sped up */
function resizeJPEG($filename) {
// Set a maximum height and width
    $width = 1280;
    $height = 1280;

// Content type
//header('Content-type: image/jpeg');
// Get new dimensions
    list($width_orig, $height_orig) = getimagesize($filename);

    $ratio_orig = $width_orig / $height_orig;

    if ($width / $height > $ratio_orig) {
        $width = $height * $ratio_orig;
    } else {
        $height = $width / $ratio_orig;
    }

// Resample
    $image_p = imagecreatetruecolor($width, $height);
    $image = imagecreatefromjpeg($filename);
    imagecopyresampled($image_p, $image, 0, 0, 0, 0, $width, $height, $width_orig, $height_orig);

// Output, kind of hachish
    ob_start();
    imagejpeg($image_p, null, 100);
    $stringdata = ob_get_contents(); // read from buffer
    ob_end_clean(); // delete buffer
//return 
    return $stringdata;
}

//sometimes called from another class, ie sidebar, thus we need to dyniamically find the page_id.
//Feed this to it in calling class, ie findFileNameOfClass(strtolower(get_class($this)));
/** REPLACE ALL THE HARD CODED pageid=6, etc. with this function. * */
function findFileNameOfClass($className) {
    $sql = "SELECT pageid FROM intwebsite.page WHERE lower(filename)='$className' || '.inc' OR lower(filename)='$className' || '.php' LIMIT 1";
    $res = $GLOBALS['pdo']->query($sql);
    $row = $res->fetch(PDO::FETCH_ASSOC);
    return $row['pageid'];
}

/* returns true when internet (google) is up */

function is_online() {
    //NOTE: Google.com seems to drop too many dns requests, so try yahoo.
    if ((checkdnsrr('www.yahoo.com', 'ANY') ) == true) {
        return true;
    } else {
        return false;
    }
}

/* generates a css loading line with cache fixed. */

function cssLinkGenerate($file, $mediaStr) {
    return ('<link rel="stylesheet" type="text/css" media="' . $mediaStr . '" href="' . $file . '?v=' . filemtime($_SERVER["DOCUMENT_ROOT"] . $file) . '" />' . "\n");
}

/* this is a single method  classthat any long running class will need for SSE calls
 * put this in its own class in misc file. Maybe move to its own file later
 */

class SSE_Message {

//used to send messages in basic json format back to webpage for updates of long loops
    static function send_message($id, $message, $progress, $mute = false) {
        if ($mute == false) {
            $d = array('message' => $message, 'progress' => $progress);

            echo "id: $id" . PHP_EOL;
            echo "data: " . json_encode($d) . PHP_EOL;
            echo PHP_EOL;

            //ob_flush();
            flush();
        }
    }

}//end class


/* //simple sanitize of request/get/post variables.
 * FROM: http://stackoverflow.com/questions/12933550/better-way-for-checking-request-variable
 * $p = reqVal( 'p', 0 );
 * $p = getVal( 'p', 'something', false );
 * $p = postVal( 'p' ); // or just forget the 2nd and 3rd parameter
 */

class Sanit {

    public static function reqVal($val, $default = "", $filter = 'INT', $no_sql = true) {
        $var = isset($_REQUEST[$val]) ? $_REQUEST[$val] : $default;
        $var = $no_sql ? Sanit::nosql($var, $filter) : $var;
        return $var;
    }

    public static function getVal($val, $default = "", $no_sql = true) {
        $var = isset($_GET[$val]) ? $_GET[$val] : $default;
        $var = $no_sql ? Sanit::nosql($var, $filter) : $var;
        return $var;
    }

    public static function postVal($val, $default = "", $no_sql = true) {
        $var = isset($_POST[$val]) ? $_POST[$val] : $default;
        $var = $no_sql ? Sanit::nosql($var, $filter) : $var;
        return $var;
    }

    private static function nosql($var, $filter) {
        switch ($filter) {
            case 'INT':
                $var = filter_var($var, FILTER_VALIDATE_INT);
                break;
            case 'STR':
                $var = filter_var($var, FILTER_SANITIZE_STRING);
                break;
            default:
                throw New Exception("Error: Didn't supply filter sanitizer value.");
        }

        return $var;
    }

}//end class


/**
 * Object used to pass notifications (errors) around
 *
 */
class notifyObj {

    public function __destruct() {
        
    }

    private $callingClass;
    private $time;
    private $level;
    private $text;

    public function __construct($callingClass, $time, $level, $text) {

        //do some basic sanity checks.

        $this->callingClass = $callingClass;

        if (strtotime($time) != false) {
            $this->time = $time;
        } else {
            throw new exception("notifyObj time ($time) not valid!");
        }


        if (is_integer($level) == true) {
            $this->level = $level;
        } else {
            throw new exception("notifyObj level ($level) not a valid integer!");
        }




        $this->text = $text;
    }

    public function __toString() {
        return $this->get();
    }

    public function get_array() {
        $ret = array();
        $ret['callingClass'] = $this->callingClass;
        $ret['time'] = $this->time;
        $ret['level'] = $this->level;
        $ret['text'] = $this->text;
        return $ret;
    }

    public function get() {
        return (" callingClass {$this->callingClass} time {$this->time}  level {$this->level} text {$this->text}");
    }

}//end class
?>