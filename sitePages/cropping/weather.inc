<?php 
//page to download infor from various sites (wunderground, weather network, enviorment canada) and make a meta page.

class Weather extends BasePage {
        
       

        public function defaultDisplay() {
            
$this->weatherUndergroundCurrent();
           ?>

<br/><br/><br/><br/><br/>
          
<img src="https://weather.gc.ca/data/radar/detailed/temp_image//XNC/XNC_PRECIP_RAIN_2018_09_29_14_10.GIF"/>
<br/><br/><br/><br/><br/><br/>
<!-- Begin WeatherLink Fragment -->
<iframe title="Environment Canada Weather" width="287px" height="191px" src="//weather.gc.ca/wxlink/wxlink.html?cityCode=nb-36&amp;lang=e" allowtransparency="true" frameborder="0"></iframe>
<!-- End WeatherLink Fragment -->
<?php 

 
        }
        
        
public function defaultDisplay2() {
$this->weatherUndergroundCurrent();
$this->echoRadarIMGTag();

$weatherNetworkForecastArray=$this->weatherNetworkForecast();
echo($weatherNetworkForecastArray[0]);
echo($weatherNetworkForecastArray[1]);
echo($weatherNetworkForecastArray[2]);
}

function weatherUndergroundCurrent() {

echo('<a href="https://www.wunderground.com/weatherstation/WXDailyHistory.asp?ID=INBCOLPI2"><img src="https://banners.wunderground.com/cgi-bin/banner/ban/wxBanner?bannertype=pws250_metric&weatherstationcount=INBCOLPI2" width="250" height="150" border="0" alt="Weather Underground PWS INBCOLPI2" /></a>');

}

function weatherNetworkForecast()
{

$ch = curl_init();
curl_setopt ($ch, CURLOPT_URL, "https://www.theweathernetwork.com/weather/canb0089/");
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
$result=curl_exec ($ch);
curl_close ($ch);

////////Current Conditions

$pattern = '/<!-- current conditions -->/';
preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$startIndex=$matches[0][1];

$pattern = '/<!-- forecast buttons -->/';
$dude=preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$finishIndex=$matches[0][1];

$newStr= substr($result, $startIndex, $finishIndex-$startIndex);     // bcdef
//tricky, has to replace the path correcty for images to show.
$newphrase = str_replace(array("/common/images/","common/images/"),array("common/images/","http://www.theweathernetwork.com/common/images/"),$newStr);
$returnCurrent=$newphrase;
//echo($newphrase);

////////Short Term

$pattern = '/<!-- short term forecast -->/';
preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$startIndex=$matches[0][1];

$pattern = '/<!------- newsfeature1 ------->/';
$dude=preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$finishIndex=$matches[0][1];

$newStr= substr($result, $startIndex, $finishIndex-$startIndex);     // bcdef
$newphrase = str_replace(array("/common/images/","common/images/"),array("common/images/","http://www.theweathernetwork.com/common/images/"),$newStr);
$returnShortTerm=$newphrase;
//echo($newphrase);

////////Long Term
$pattern = '/<!-- long term forecast -->/';
preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$startIndex=$matches[0][1];

$pattern = '/14 Day trend/';
$dude=preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$finishIndex=$matches[0][1];

$newStr= substr($result, $startIndex, $finishIndex-$startIndex);     // bcdef
$newphrase = str_replace(array("/common/images/","common/images/"),array("common/images/","http://www.theweathernetwork.com/common/images/"),$newStr);
$returnLongTerm=$newphrase;
//echo($newphrase);

//create final array
return array($returnCurrent,$returnShortTerm,$returnLongTerm);

}


///////////////////////////////// WEATHER OFFICE RADAR CODE ////////////////////////////////////
function echoRadarIMGTag()
{

$ch = curl_init();

curl_setopt ($ch, CURLOPT_URL, "https://www.weatheroffice.gc.ca/radar/index_e.html?id=XNC");
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
$result=curl_exec ($ch);
curl_close ($ch);
//echo($result);


$pattern = '/<!-- Image List -->/';
preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$startIndex=$matches[0][1];


$pattern = '/<!-- End Image List -->/';
$dude=preg_match($pattern, substr($result,0), $matches, PREG_OFFSET_CAPTURE);
$finishIndex=$matches[0][1];

$newStr= substr($result, $startIndex, $finishIndex-$startIndex);     // bcdef

//match something like this XNC_PRECIP_RAIN_2008_04_14_00_10.GIF
$pattern = '/XNC_PRECIP_.*\.GIF/';
$dude=preg_match_all($pattern, $newStr, $matches, PREG_OFFSET_CAPTURE);


//create full URL array for each radar gif
foreach ($matches[0] as $val) {
    $radarURL= "https://www.weatheroffice.gc.ca/data/radar/detailed/temp_image/XNC/" . $val[0] . "\n";
}

//TODO: combine images with image magik and make an animated gif.
//just grab the last image and display that.
$outStr= "<img class=\"radarImage\" src=" . $radarURL . "/>";
echo($outStr);
}
}//end class
?>