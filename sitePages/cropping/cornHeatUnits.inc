<?php

class CornHeatUnits extends BasePage {

    public function defaultDisplay() {
        $this->run();
    }

    function run() {

       //  print_r($this->enviormentCanadaDailyData(50309,2018));
        // exit();


//weather underground station id
        $stationID = 'INBCOLPI2'; //colpitts settlement
        
//$stationID='KMEMARSH2';//Mars Hill, ME (plaster rock)
//$stationID='INBMONCT4'; //moncton
//$stationID='INOVASCO6'; //annapolis valley
//$stationID='IQCSAINT11';//quebec corn area
//$stationID='IONTARIO265';//ott airport ontario corn area



        print("<h2>Station: $stationID</h2>");
        print("<h2>FIXME: data is missing, thus totals are lower then they should be for years with crooked line graph.</h2>");
        $this->dbTableSetup($stationID);
        $this->cornLastXYears();
        print("<br/>");
        $this->alfalfaLastXYears();



        //data makes no sense before April 1.
        if (strtotime("now") <= strtotime((date('Y', time()) . "-04-31"))) {
            $startYear = (date('Y', strtotime("-1 year")));
        } else {
            $startYear = date('Y', time()); //assume the current year, unless before April 2.
        }
/*
        Print("<h3>Station: $stationID</h3>");
        Print("Start Year:$startYear");

        $this->alfalfaChuForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, true, true);
        $startYear = (date('Y', strtotime("-1 year")));


        $this->alfalfaChuForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, true, true);

        //2 years ago
        $startYear = (date('Y', strtotime("-2 year")));
        $this->alfalfaChuForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, true, true);

        //3 years ago
        $startYear = (date('Y', strtotime("-3 year")));
        $this->alfalfaChuForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, true, true);

        //4 years ago
        $startYear = (date('Y', strtotime("-4 year")));
        $this->alfalfaChuForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, false);
        $this->plotChartForGivenWeatherSationAndYear($stationID, $startYear, true, true);
 *
 */
    }

    /* easier to handle data in a DB table, so do it that way, downloads every time from wunderground */

    function dbTableSetup($stationID) {
        $sqlX = "
CREATE  TEMP TABLE IF NOT EXISTS weather_data (
    date        date CONSTRAINT firstkey PRIMARY KEY,
    tmax       numeric NOT NULL,
    tmin        numeric NOT NULL,
    chu     numeric NOT NULL,
    GDD5         numeric NOT NULL
  
);         
";
        $sqlX2 = "TRUNCATE weather_data;";

        $resX = $GLOBALS['pdo']->query($sqlX);
        $resX = $GLOBALS['pdo']->query($sqlX2);

        //dates of interest
        $startDay = 1;
        $startMonth = 4;

        $endDay = 15; //defaults to Oct 15, this might be late....
        $endMonth = 10;



        //check if we are between Jan 1 and April 30, if so use previous year.    
        if (strtotime("now") >= strtotime((date('Y', time()) . "-05-01"))) {
            $startPoint = 0;
        } else {
            $startPoint = 1;
        }


        for ($i = $startPoint; $i <= 5; $i++) {

            $startYear = (date('Y', strtotime("-$i year")));
            $endYear = $startYear;

            //Station Of Interest.
            $rawData = $this->getDataFromWeb($stationID, $startDay, $startMonth, $startYear, $endDay, $endMonth, $endYear);
            //print_r($rawData);
            $dataArr = $this->processWebDataToArray($rawData);

            //Backup Station. used when data is missing from station of interest. Should be a close very reliable station. 
            //we use envorment canada in moncton in our case. 
            $dataArrBackup = $this->enviormentCanadaDailyData(50309,$startYear);
            //var_dump($dataArrBackup);

            $loopStartDate = ("$startYear-$startMonth-$startDay");
            $loopEndDate = ("$endYear-$endMonth-$endDay");
            //array of dates per day
            $period = new DatePeriod(
                    new DateTime($loopStartDate), new DateInterval('P1D'), new DateTime($loopEndDate)
            );


            /*
              //redo array so date is key field.
              $dataArr2 = array();
              foreach ($dataArr as $key => $v) {
              $dataArr2[trim($v['date'])] = $v;
              }
             */

            // var_dump($dataArr);

            foreach ($period as $key => $value) {
                $k = ($value->format('Y-n-j') );

                if (isset($dataArr[$k])) {
                    $v = $dataArr[$k];
                  //  print_r($v);
                    $res = $GLOBALS['pdo']->exec("INSERT INTO weather_data (date,tmax,tmin,chu,GDD5) VALUES ('{$v['date']}',{$v['Tmax']},{$v['Tmin']},{$v['Chu']},{$v['GDD5']})");
                    //$dataArr 
                    //print_r($v);
                //
            } else { //date missing weather info. REliable station for when main sation does not work, use it instead. 
                    //print("For $stationID and date $k data is missing.<br>");
               
                    if (isset($dataArrBackup[$k])) {
                        $v = $dataArrBackup[$k];
                        $res = $GLOBALS['pdo']->exec("INSERT INTO weather_data (date,tmax,tmin,chu,GDD5) VALUES ('{$v['date']}',{$v['Tmax']},{$v['Tmin']},{$v['Chu']},{$v['GDD5']})");
                    } else {
                        print("No backup stationd data for date $k, data is missing for  orginal station $stationID as well.<br>");
                    }
                }
            }
        }
    }

    
    //curl --remote-header-name / -J "http://climate.weather.gc.ca/climate_data/bulk_data_e.html?format=csv&stationID=50309&Year=2018&Month=8&Day=1&timeframe=2&submit=Download+Data"
    //dowloads data from enviorment canada for specified year and station.
    function enviormentCanadaDailyData($stationID, $year) {
        //create curl URL 
        $url = "http://climate.weather.gc.ca/climate_data/bulk_data_e.html?format=csv&stationID=$stationID&Year=$year&Month=1&Day=1&timeframe=2&submit=Download+Data";
        // create curl resource 
        $ch = curl_init();
        // set url 
        curl_setopt($ch, CURLOPT_URL, $url);
        //return the transfer as a string 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        // $output contains the output string 
        $output = curl_exec($ch);

        // close curl resource to free up system resources 
        curl_close($ch);

        //break up by lines
        $pieces = preg_split('/\r\n|\n|\r/', $output);
        //print_r($pieces);
        $outArr = array();

        foreach ($pieces as &$line) {

            //explode it and remove quotes
            $csvLine = explode(',', $line);
            foreach ($csvLine as $key => $value) {
                $csvLine[$key] = str_replace('"', "", $value);
            }

 if ((!empty($csvLine[1])) AND (checkdate($csvLine[2],$csvLine[3],$csvLine[1]))) {
            if ((is_numeric($csvLine[1])) AND (!empty($csvLine[5]))){ //simple check to see if data is somewhat valid.

                //print_r($csvLine);print("<br><br>"); //DEBUG
                //format
                //change date format to match wunderground
                $csvLine[0]=date('Y-n-j',strtotime($csvLine[0]));
                
                
                $newKey = trim($csvLine[0]);
                $outArr[$newKey]['date'] = trim($csvLine[0]);
                $outArr[$newKey]['Tmax'] = $csvLine[5];
                $outArr[$newKey]['Tavg'] = $csvLine[9];
                $outArr[$newKey]['Tmin'] = $csvLine[7];
                $outArr[$newKey]['Chu'] = self::calculateDailyChu($outArr[$newKey]['Tmax'], $outArr[$newKey]['Tmin']);
                $outArr[$newKey]['GDD5'] = self::calculateDailyGDD5($outArr[$newKey]['Tmax'], $outArr[$newKey]['Tmin']);
 }}
        }


        return $outArr;
    }

    function cornLastXYears() {


        $dataStr = " ['Time', 'All Years'],";


        //does running some for each year independently. 
        //a bit hackish, the date_part days to take dates bewen may and octber.
        $sql = "With temp as (
SELECT *
FROM weather_data 
WHERE date_part('month', date) >=5 AND date_part('month', date) <=10
)
SELECT *,  sum(chu)  OVER w  as running_chu
             FROM temp
             WINDOW w AS (PARTITION BY extract(year from date) ORDER BY date)";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $dataStr = $dataStr . "[new Date('" . date('m/d/Y', strtotime($row['date'])) . "'),{$row['running_chu']}],";
        }
        ?>
        <div id="chart_div_yearChu" ></div>


        <script type="text/javascript">

            setTimeout(drawChart_yearChu, 200); //firefox workaround.  
            google.charts.setOnLoadCallback(drawChart_yearChu, true);
            $(window).resize(function () {
                google.charts.setOnLoadCallback(drawChart_yearChu);
            });
            function drawChart_yearChu() {

                var dataChu = google.visualization.arrayToDataTable([
        <?php echo($dataStr); ?>
                ]);


                var optionsChu = {
                    title: 'Accumulated Chu Last 5 Years',
                    interpolateNulls: true,
                    height: 600,
                    width: 1200,
                    hAxis: {title: 'Time', titleTextStyle: {color: '#000'}},
                    vAxis: {title: 'Chu', titleTextStyle: {color: '#000'}},
                    legend: {textStyle: {color: '#000', fontSize: 10}}

                };


                var chartChu = new google.visualization.ScatterChart(document.getElementById('chart_div_yearChu'));
                chartChu.draw(dataChu, optionsChu);
            }
        </script>


        <?php
    }

    function alfalfaLastXYears() {


        $dataStr = " ['Time', 'All Years'],";


        //does running some for each year independently. 
        //a bit hackish, the date_part days to take dates bewen april and june.
        $sql = "With temp as (
SELECT *
FROM weather_data 
WHERE date_part('month', date) >=4 AND date_part('month', date) <=6
)
SELECT *,  sum(GDD5)  OVER w  as running_GDD5
             FROM temp
             WINDOW w AS (PARTITION BY extract(year from date) ORDER BY date)";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //  print_r($row);
            $dataStr = $dataStr . "[new Date('" . date('m/d/Y', strtotime($row['date'])) . "'),{$row['running_gdd5']}],";
        }
        ?>
        <div id="chart_div_yearGDD5"></div>


        <script type="text/javascript">

            setTimeout(drawChart_yearGDD5, 200); //firefox workaround.   
            google.charts.setOnLoadCallback(drawChart_yearGDD5, true);
            $(window).resize(function () {
                google.charts.setOnLoadCallback(drawChart_yearGDD5);
            });

            function drawChart_yearGDD5() {

                var data = google.visualization.arrayToDataTable([
        <?php echo($dataStr); ?>
                ]);


                var options = {
                    title: 'Accumulated GDD5 Last 5 Years',
                    interpolateNulls: true,
                    height: 600,
                    width: 1200,
                    hAxis: {title: 'Time', titleTextStyle: {color: '#000'}},
                    vAxis: {title: 'GDD5', titleTextStyle: {color: '#000'}},
                    legend: {textStyle: {color: '#000', fontSize: 10}}

                };


                var chart = new google.visualization.ScatterChart(document.getElementById('chart_div_yearGDD5'));
                chart.draw(data, options);
            }
        </script>


        <?php
    }

    function cornRunningSumChuForOneYear($stationID, $startYear) {
        //Chu redefined to start May 1 by OMAFRA.
        $startDay = 1;
        $startMonth = 5;

        $endDay = 15; //defaults to Oct 15, this might be late....
        $endMonth = 10;
        $endYear = $startYear;

        //execute major functions.
        $rawData = $this->getDataFromWeb($stationID, $startDay, $startMonth, $startYear, $endDay, $endMonth, $endYear);
        $dataArr = $this->processWebDataToArray($rawData);
        $dataArr = $this->checkDataArrayIsInDateRange($dataArr, $startDay, $startMonth, $endDay, $endMonth, $startYear, $endYear);


        //create a running sum array.
        $tsum = 0;
        $tArr = array();
        $tArr2 = array();

        print("$startYear<br>");
        //print_r($dataArr);
        foreach ($dataArr as $d) {
            $tArr[date('M-d', strtotime($d['date']))] = $d['Chu'];
        }

        $tArr2 = $this->calculateRunningSumArray($tArr);


        return $tArr2;
    }

    function alfalfaChuForGivenWeatherSationAndYear($stationID, $startYear, $calculateChuRunningTotal = false) {
        //The Chu



        print("<h2>Alfalfa $startYear</h2>");
        $startDay = 1; //defaults to first of month.
        $startMonth = 4;  //start looking in april for the start of the planting year.
        $endDay = 15; //defaults to June 15
        $endMonth = 06;
        $endYear = $startYear;

        $rawData = $this->getDataFromWeb($stationID, $startDay, $startMonth, $startYear, $endDay, $endMonth, $endYear);
        $dataArr = $this->processWebDataToArray($rawData);
        $dataArr = $this->checkDataArrayIsInDateRange($dataArr, $startDay, $startMonth, $endDay, $endMonth, $startYear, $endYear);


        $this->plotAlfalfaGDD5($dataArr);
    }

    function plotChartForGivenWeatherSationAndYear($stationID, $startYear, $calculateChuRunningTotal = false, $useFixedStartDate = false) {

//station and year are supplied.
//get dates to use.
        $startDay = 20; //defaults to first of month.
        $startMonth = 4;  //start looking in april for the start of the planting year.

        $endDay = 15; //defaults to Oct 15
        $endMonth = 10;
        $endYear = $startYear;


//execute major functions.

        $rawData = $this->getDataFromWeb($stationID, $startDay, $startMonth, $startYear, $endDay, $endMonth, $endYear);
        $dataArr = $this->processWebDataToArray($rawData);
        $dataArr = $this->checkDataArrayIsInDateRange($dataArr, $startDay, $startMonth, $endDay, $endMonth, $startYear, $endYear);
        if ($useFixedStartDate == false) {
            $startDateData = $this->findStartDate($dataArr, $stationID, false);
        } else {
            $startDateData = $this->findStartDate($dataArr, $stationID, true);
        }

        $startDate = $startDateData['$startDate'];
        $startDateKey = $startDateData['$startDateKey'];

        if ($startDate == null) {
            print("<h2>Corn Crop Heat Units has not start for this year yet</h2>");
        } else {
            if ($useFixedStartDate == false) {
                $endDate = $this->findEndDate($dataArr, $startDateKey, false);
            } else {
                $endDate = $this->findEndDate($dataArr, $startDateKey, true);
            }

            $dailyChuArr = $this->findDataSubset($dataArr, $startDate, $endDate);
            $totalChu = $this->calculateTotalChu($dailyChuArr);

            if ($calculateChuRunningTotal == true) {
                $dailyChuArr = $this->calculateRunningSumArray($dailyChuArr);
                $this->plotGraph($dailyChuArr);
            } else {
                $this->plotGraph($dailyChuArr);
            }
        }

        return $totalChu;
    }

//when we want to create as sum of heat units throught the year.
//do running total and output as array.
    function calculateRunningSumArray($arrayIn) {
        for ($i = 1; $i <= count($arrayIn); $i++) {
            $key = array_keys($arrayIn);
            $arrayOut[$key[$i - 1]] = self::array_partial_sum($arrayIn, 0, $i);
        }

        return $arrayOut;
    }

    /* sum partial array from: http://www.php.net/manual/en/function.array-sum.php#50497 */

    private function array_partial_sum($array, $start, $length) {
        $new_array = array_slice($array, $start, $length);
        return array_sum($new_array);
    }

    function getDataFromWeb($stationID, $startDay, $startMonth, $startYear, $endDay, $endMonth, $endYear) {

//grab the weather data for the farm weather station from weather underground


        $ch = curl_init("https://www.wunderground.com/weatherstation/WXDailyHistory.asp?ID=$stationID&graphspan=custom&month=$startMonth&day=$startDay&year=$startYear&monthend=$endMonth&dayend=$endDay&yearend=$endYear&format=1");
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

// We'll be returning this transfer, and the data is binary
// so we don't want to NULL terminate
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

// Grab the page and save the contents in the $data variable
        $rawData = curl_exec($ch);
// close the connection
        curl_close($ch);

        return $rawData;
    }

    //date is array key value
    function processWebDataToArray($data) {

//break up by lines
        $pieces = explode('<br>', $data);
        $outArr=array();
        $data = null;
       
        foreach ($pieces as &$line) {

//break up a line of csv
            $csvLine = explode(',', $line);

//check that we have a valid line of data and not the header or footer.
            if (!empty($csvLine[1])) {
                if (is_numeric($csvLine[1])) {
//Tmax is 1 and Tmin is 3.
                    $newKey=trim($csvLine[0]);
                    $outArr[$newKey]['date'] = trim($csvLine[0]);
                    $outArr[$newKey]['Tmax'] = $csvLine[1];
                    $outArr[$newKey]['Tavg'] = $csvLine[2];
                    $outArr[$newKey]['Tmin'] = $csvLine[3];
                    $outArr[$newKey]['Chu'] = self::calculateDailyChu( $outArr[$newKey]['Tmax'],  $outArr[$newKey]['Tmin']);
                    $outArr[$newKey]['GDD5'] = self::calculateDailyGDD5( $outArr[$newKey]['Tmax'],  $outArr[$newKey]['Tmin']);
                   
                }
            }
        }

//DEBUG
//print_r($outArr);

        return $outArr; //return array of data.
    }

//for some reason weatherunderground doesn't return the proper date range, so use this function to make sure array is within range.
    function checkDataArrayIsInDateRange($dataArr, $startDay, $startMonth, $endDay, $endMonth, $startYear, $endYear) {

        foreach ($dataArr as $key => $line) {

//convert to seconds from unix epoch
            $startT = strtotime("$startDay-$startMonth-$startYear");
            $endT = strtotime("$endDay-$endMonth-$endYear");
            $currentT = strtotime($line['date']);

//delete array values if they are out of accepted date range.
            if ($currentT < $startT) {
                unset($dataArr[$key]);
            } elseif ($currentT > $endT) {
                unset($dataArr[$key]);
            }
        }
        $dataArr = array_merge($dataArr); //re-indexes array starting at 0, because we deleted elements.
        return $dataArr;
    }

    function findStartDate($dataArr, $stationID, $override = false) {

        //to compare year to year, we need a fixed start date, we use May 1.
        if ($override == true) {
            //find what year we are in.
            $year = date('Y', strtotime($dataArr[0]['date']));
            $fixedDate = "$year-5-1";
            $startDateData['$startDate'] = $fixedDate;
            foreach ($dataArr as $key => $value) {
                if (strtotime($value['date']) == strtotime($fixedDate)) {
                    $startDateData['$startDateKey'] = $key;
                    break;
                }
            }
            if ($startDateData['$startDateKey'] == null) {
                throw new Exception("Start date of $fixedDate not found!!!");
            }
        } else {

            $startDateData = null;
            $startDate = null;

            $sizeArr = sizeof($dataArr); //find array size
//look for three consecutive days above 12.8 degrees C average temp.
//this date is also suppose to be greater than the 30 year average >= mean temp of 10 degrees C.
//this is not checked for because I don't have this info.
            for ($x = 2; $x <= $sizeArr - 2; $x++) {
                if (($dataArr[$x - 2]['Tavg'] >= 12.8) && ($dataArr[$x - 1]['Tavg'] >= 12.8) && ($dataArr[$x]['Tavg'] >= 12.8)) {
                    $startDate = $dataArr[$x]['date'];
                    $startDateData['$startDate'] = $startDate;
                    $startDateData['$startDateKey'] = $x;
                    break;
                }
            }
        }

        return $startDateData;
    }

    function findEndDate($dataArr, $startDateKey, $override = false) {

        $endDate = null;
        $sizeArr = sizeof($dataArr); //find array size
//to compare year to year, we need a fixed end date, we use Oct 15.
        if ($override == true) {
            $year = date('Y', strtotime($dataArr[0]['date']));
            $fixedDate = "$year-10-15";
            $fixedDate = "$year-" . date('m-d', strtotime("now"));
            for ($x = $startDateKey; $x <= $sizeArr; $x++) {
                if (strtotime($dataArr[$x]['date']) == strtotime($fixedDate)) {
                    $endDate = $dataArr[$x]['date'];
                    break;
                }
            }
        } else {


//find first occurence of -2 degrees C after start date.

            for ($x = $startDateKey; $x <= $sizeArr; $x++) {
                if ($dataArr[$x]['Tmin'] <= -2) {
                    $endDate = $dataArr[$x]['date'];
                    break;
                }
            }
        }
        return $endDate;
    }

//only return data between start and end.
    function findDataSubset($dataArr, $startDate, $endDate) {

        $ChuArr = null;

        if ($startDate == null) {
//don't return anything because season hasn't started yet.
            return null;
        } else {
            $startDateEpoch = strtotime($startDate);
        }

        if ($endDate == null) {
//return today's date because season is ungoing.
            $endDateEpoch = strtotime('today');
        } else {
            $endDateEpoch = strtotime($endDate);
        }


        $sizeArr = sizeof($dataArr); //find array size

        for ($x = 0; $x <= $sizeArr; $x++) {
            $xDate = strtotime($dataArr[$x]['date']);

            if (($xDate >= $startDateEpoch) && ($xDate < $endDateEpoch)) {
                $ChuArr["$xDate"] = $dataArr[$x]['Chu'];
            }
        }

//print_r($ChuArr);
        return $ChuArr;
    }

    function calculateTotalChu($ChuArr) {

//print out Chu Total
        $sum = round(array_sum($ChuArr));

//find start date of Chu calculation.
        $startTimeEpoch = key($ChuArr);
        $startDate = date('M j Y', $startTimeEpoch);

//find the last date of the Chu calculation
        end($ChuArr); //put array pointer on last element.
        $endTimeEpoch = key($ChuArr);
        $endDate = date('M j Y', $endTimeEpoch);

        print("<h2>Total Chu:$sum from $startDate to $endDate</h2>");
    }

    function plotGraph($ChuArr) {

//create a comma seperated string of data for google charts.
        foreach ($ChuArr as &$elem) {
            $outStr = "$outStr" . "$elem,";
        }
        $outStr = substr($outStr, 0, -1); //erase the last ,
        $maxDataValue = max($ChuArr);
        $numberOfDays = count($ChuArr);


        print("
<img src=\"
https://chart.googleapis.com/chart
?chs=300x225
&chd=t:$outStr
&chds=0,$maxDataValue
&cht=lc
&chxt=x,y,r,x
&chxr=0,0,$numberOfDays|1,0,$maxDataValue
&chxl=3:|   |Time from Planting (days)|   |2:|  |Crop Heat Units|  |
\"
/>");
    }

    /*
      //Chu calulcation
      //from: http://www.omafra.gov.on.ca/english/crops/pub811/1ChuChu.htm#Chu
      Daily Chu = (Ymax + Ymin) ÷ 2
      where:
      Ymax = (3.33 x (Tmax-10.0))-(0.084 x (Tmax-10.0)2) (If values are negative, set to 0)
      Tmax = Daily maximum air temperature (°C) (measured from midnight to midnight) (Accuracy should be <0.25°C)
      Ymin = (1.8 x (Tmin-4.4)) (If values are negative, set to 0)
      Tmin = Daily minimum temperature (°C)
     */

    function calculateDailyChu($Tmax, $Tmin) {


        $Ymax = (3.33 * ($Tmax - 10.0)) - (0.084 * ($Tmax - 10.0) * ($Tmax - 10.0));
        if ($Ymax <= 0) {
            $Ymax = 0;
        }

        $Ymin = (1.8 * ($Tmin - 4.4));
        if ($Ymin <= 0) {
            $Ymin = 0;
        }

        $DailyChu = ($Ymax + $Ymin) / 2;

        /* DEBUG
          print("Tmax=$Tmax,Tmin=$Tmin<br>");
          print("Ymax=$Ymax,Ymin=$Ymin<br>");
          print("Daily Chu:$DailyChu<br>");
         */

        return $DailyChu;
    }

    //base 5 deg C Chu
    //from alpril 1 to 300 Chu C, we cut alfalfa. 
    //GDD5 = {(daily max. temperature + daily min. temperature) ÷ 2} - base temperature5 
    function calculateDailyGDD5($Tmax, $Tmin) {

        $t = ($Tmax + $Tmin) / 2 - 5;
        if ($t > 0) {
            return $t;
        } else {
            return 0;
        }
    }

    function plotAlfalfaGDD5($dataArr) {


        $runningSum = 0;
        foreach ($dataArr as $d) {
            $data = $data . "[new date('{$d['date']}'), {$d['GDD5']}],";
            $runningSum = $runningSum + $d['GDD5'];
            $milkSoldArray[strtotime($d['date'])] = $runningSum;
        }


        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['GDD5 Accumulation'] = 'number';

        $opt = "                  
                        series: {
            0:{ lineWidth: 2, targetAxisIndex: 0 }
                                },
                        chartArea: {width: '90%', height: '80%'},
                        legend: {position: 'in'}";


        $x = new GoogleVisualizationLine($opt, $headerStrArr, $milkSoldArray);
        print($x->toString());
    }

}

//end class
?>