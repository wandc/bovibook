<?php 
/** store various functions to use with google charts */
/** D Waddy April 1, 2009 */

class GoogleChartsHelper
{


 function javascriptDate($datIn) {
            //CHECK:off by zero bug for js??
                   $Y=date('Y',strtotime($datIn));
                   $M=date('m',strtotime($datIn));
                   $D=date('d',strtotime($datIn));
                    
                    
                return "new Date($Y,$M,$D)";
                }
                
  function javascriptDateTime($datTimeIn) {
            //CHECK:off by zero bug for js??
                   $Y=date('Y',strtotime($datTimeIn));
                   $M=date('m',strtotime($datTimeIn))-1; //months start at zero for js, dumbest thing ever. Javascript sucks!
                   $D=date('d',strtotime($datTimeIn));
                   $H=date('H',strtotime($datTimeIn));
                   $I=date('i',strtotime($datTimeIn));
                   $S=date('s',strtotime($datTimeIn));
                    
                    
                return "new Date($Y,$M,$D,$H,$I,$S)";
                }               

//allows values from 0 to 4095
//source:http://bendodson.com/blog/google-extended-encoding-made-easy/
function array_to_google_charts_extended_encoding($array)
{
    $characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.';

    $encoding = '';
    foreach ($array as $value) {
        $first = floor($value / 64);
        $second = $value % 64;
        
        $encoding .= $characters[(int)$first] . $characters[(int)$second];
    }

    return $encoding;
}
function google_charts_extended_encoding_to_array($string)
{
    $characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.';  

    for ($i=0;$i<strlen($characters);$i++) {
        $first[$characters[$i]] = $i * 64;
    }

    for ($i=0;$i<strlen($characters);$i++) {
        $second[$characters[$i]] = $i;
    }

    $pairs = str_split($string,2);
    foreach ($pairs as $pair) {
        $value = $first[$pair[0]];
        $value += $second[$pair[1]];
        $values[] = $value;
    }

    return $values;
}

//doesn't normalize, just converts it to a comma seprated list
function createGoogleChartsDataString($dataArray) {
    $dataStr='';
foreach ($dataArray as &$value) {
$dataStr=$dataStr.','. round($value,2);
}
$dataStr=substr($dataStr,1,strlen($dataStr)); //fix leading comma
return $dataStr;
}

//use this when you already have the data and just want to put | characters in it for label purposes.
function turnAnArrayIntoGoogleChartsLabelString($labelArray)
{
    $labelStr='';
    $dataStr=' ';
foreach ($labelArray as &$val) {
$labelStr=$labelStr.$val.'|';
}
$labelStr=substr($labelStr,0,strlen($dataStr)-1); //fix trailing |
return $labelStr;
}


//take in a label max and min values and linearly space an array of numbers between them of size numPoints.
//NOTE: $timeDateType='Y m' or similar for time data. AND labelMax and labelMin should to strtotime first.
function createGoogleChartsLabelXYString($labelMax,$labelMin,$numPoints,$timeDateType=false)
{
$labelStr='';
$dataStr=' ';
//avoid zero division and return imediately. maybe should be null or maybe should throw exception....
if ($labelMax==$labelMin) {
return null;
}

$spacing=(($labelMax-$labelMin)/$numPoints);

for ($i=$labelMin; $i<=$labelMax; $i=$i+$spacing)
{
$val=$i;

//if it is time data convert it to a readable format.
if ($timeDateType!=false) {
$val=date($timeDateType,$val);
}
$labelStr=$labelStr.$val.'|';
}

$labelStr=substr($labelStr,0,strlen($dataStr)-1); //fix trailing |

return $labelStr;

}

//Stupid google charts, wants data prescaled between 0 and 100.
//This function does it and returns the actual data and string too.
//default min and max, allow custom scaling of the data.
function normalizeGoogleChartData($dataArray,$customMin=null,$customMax=null,$timeDateType=false,$numberOfLabelPoints=20)
{
//only do if data actual exists.
if ($dataArray != null) {

if ($customMin==null)
{
$customMin=min($dataArray);
}
if ($customMax==null)
{
$customMax=max($dataArray);
}

//when they are the same the normaliztion forumla fails with divide by 0. use this as a work around, think about a better/more robust way.
if (($customMin==$customMax) && (count($dataArray)==1)){
$returnArray['labelMax']=$dataArray[0];
$returnArray['labelMin']=$dataArray[0];
$returnArray['normData'][0]=$dataArray[0];
$returnArray['normDataStr']="{$dataArray[0]}";
return $returnArray;
}

//else {
$returnArray['labelMax']=$customMax;
$returnArray['labelMin']=$customMin;

$returnArray['normDataStr']="";
$counter=0;
$extendedNumbersArray=null;

//find the last value in the dataArray for time series sparklines
$returnArray['labelLast']=$dataArray[sizeof($dataArray)-1];
$returnArray['dataCount']=sizeof($dataArray); //needed for sparkline labels


//create label string
$returnArray['linearPipeLabelStr']=self::createGoogleChartsLabelXYString($returnArray['labelMax'],$returnArray['labelMin'],$numberOfLabelPoints,$timeDateType);

foreach ($dataArray as &$value) {

/*
Suppose your numbers range from a to b, and you want to map them onto the interval from a' to b'. For a < x < b, find
x'=((b'-a')/(b-a))(x-a) + a'
*/
//avoids div by zero
if (($returnArray['labelMax']-$returnArray['labelMin']) == 0) {
$divZ=0;
}
else {
$divZ=(100-0)/($returnArray['labelMax']-$returnArray['labelMin']);
}
$tempValue1=($divZ)*($value-$returnArray['labelMin'])+0;
$tempValue=round($tempValue1,2);

//create text string
$returnArray['normData'][$counter]= $tempValue;
$returnArray['normDataStr']=$returnArray['normDataStr'].','. $tempValue;

//create numbers for extended string scale 0:100 to 0:4095
$extendedNumbersArray[$counter]=round($tempValue1*40.95,0);

$counter++;
}
$returnArray['normDataStr']=substr($returnArray['normDataStr'],1,strlen($returnArray['normDataStr'])); //fix leading comma

//create extended string.
//print_r($extendedNumbersArray);
$returnArray['extendedStr']=self::array_to_google_charts_extended_encoding($extendedNumbersArray);


}
return $returnArray;
}


//create google chart Tufte sparklines.
//https://chart.googleapis.com/chart
function sparkline($extendedStr,$dataCount,$labelLast,$bgColour='cccccc') {
$ret=("
<img class=\"sparkline\" src=\"/googleChartCache.php?cht=lc
&amp;chs=100x22
&amp;chd=e:$extendedStr
&amp;chco=336699
&amp;chls=1,1,0
&amp;chm=o,990000,0,$dataCount,4
&amp;chxt=r,x,y
&amp;chxs=0,990000,11,0,_|1,990000,1,0,_|2,990000,1,0,_
&amp;chxl=0:|$labelLast|1:||2:||
&amp;chxp=0,42
&amp;chf=bg,s,$bgColour
\" alt=\"$labelLast\" />
");
return $ret;
}




/**
 * takes a polygon returned from the db (postgres) and changes it into an array of lat/long
 */
//NOTE: DELETE THIS AFTER WE REMOVE ALL REFERENCES TO POLYGONS AND USE POSTGIS GEOM INSTEAD.
 function postgresPolygonToLatLongArray($polygon) {

	$x=explode('),(',$polygon);
	foreach ($x as $key => $value) {
		$value=str_replace(')','',$value);
		$value=str_replace('(','',$value);
		$t=null;
		$t=explode(',',$value);
		$y[$key]['lat']=$t[1];
		$y[$key]['lng']=$t[0];
	}
	return $y;
}

/**
 * takes a geom linestring returned from the db (postgres/postgis) and changes it into an array of lat/long
 * LINESTRING Z is 3D
 * LINESTRING is 2D
 */ 
function postgresLineStringToLatLongArray($linestring) {

      $linestring=str_replace('LINESTRING Z ','',$linestring);
      $linestring=str_replace(')','',$linestring);
      $linestring=str_replace('(','',$linestring);   
      $linestring=str_replace(',','|',$linestring);
      $linestring=str_replace(' ',',',$linestring);
    
	$x=explode('|',$linestring);
	foreach ($x as $key => $value) {
		$t=null;
		$t=explode(',',$value);
		$y[$key]['lat']=$t[1];
		$y[$key]['lng']=$t[0];
	}    
	return $y;
}


/**
 * takes an array of lat long points and converts it to a google maps standard polyline string.
 */
 function latLongArrayToGoogleMapsStandardPolyline($inArray) {
	$str=null;
	foreach ($inArray as $key => $value) {
		$str=$str."{$value['lat']},{$value['lng']}|";
	}
	//remove trailing |
	$str = substr($str, 0, -1);

	return $str;
}

/**
 * takes an array of lat long points and converts it to a google maps encoded polyline string.
 */
 function latLongArrayToGoogleMapsEncodedPolyline($inArray) {
	$points=null;
	foreach ($inArray as $key => $value) {
		$points[$key]=array($value['lat'],$value['lng']);
	}
	$encoder = new PolylineEncoder(18,  2, 0.00001,  true);
	$polyline = $encoder->encode($points);
	return "enc:". $polyline->points; //must return enc for the static map.
}

}//end class



// USE THIS FOR STATIC MAPS.
/**
 * PolylineEncoder based on Mark McClure's Javascript PolylineEncoder
 * and Jim Hribar's PHP version. All nicely melted into a proper PHP5 class.
 *
 * @package     Google Maps Helpers
 * @since       2008-12-02
 * @author      Matthias Bauer <matthias@pulpmedia.at>
 * @copyright	  2008, Pulpmedia Medientechnik und -design GmbH
 * @see 		    http://facstaff.unca.edu/mcmcclur/GoogleMaps/EncodePolyline/
 */
class PolylineEncoder
{
	private $numLevels = 18;
	private $zoomFactor = 2;
	private $verySmall = 0.000001;
	private $forceEndpoints = true;
	private $zoomLevelBreaks = array();

	/**
	 * All parameters are set with useful defaults.
	 * If you actually want to understand them, see Mark McClure's detailed description.
	 *
	 * @see	http://facstaff.unca.edu/mcmcclur/GoogleMaps/EncodePolyline/algorithm.html
	 */
	public function __construct($numLevels = 18, $zoomFactor = 2, $verySmall = 0.00001, $forceEndpoints = true)
	{
		$this->numLevels = $numLevels;
		$this->zoomFactor = $zoomFactor;
		$this->verySmall = $verySmall;
		$this->forceEndpoints = $forceEndpoints;

		for($i = 0; $i < $this->numLevels; $i++)
		{
			$this->zoomLevelBreaks[$i] = $this->verySmall*pow($this->zoomFactor, $this->numLevels-$i-1);
		}
	}

	/**
	 * Generates all values needed for the encoded Google Maps GPolyline.
	 *
	 * @param array		Multidimensional input array in the form of
	 * 					array(array(latitude, longitude), array(latitude, longitude),...)
	 *
	 * @return stdClass	Simple object containing three public parameter:
	 * 					- points: the points string with escaped backslashes
	 *          - levels: the encoded levels ready to use
	 *          - rawPoints: the points right out of the encoder
	 *          - numLevels: should be used for creating the polyline
	 *          - zoomFactor: should be used for creating the polyline
	 */
	public function encode($points)
	{
            $maxLoc=0;
            $absMaxDist=0;
		if(count($points) > 2)
		{
			$stack[] = array(0, count($points)-1);
			while(count($stack) > 0)
			{
				$current = array_pop($stack);
				$maxDist = 0;
				for($i = $current[0]+1; $i < $current[1]; $i++)
				{
					$temp = $this->distance($points[$i], $points[$current[0]], $points[$current[1]]);
					if($temp > $maxDist)
					{
					$maxDist = $temp;
					$maxLoc = $i;
					if($maxDist > $absMaxDist)
					{
					$absMaxDist = $maxDist;
			}
			}
			}
			if($maxDist > $this->verySmall)
			{
			$dists[$maxLoc] = $maxDist;
			array_push($stack, array($current[0], $maxLoc));
			array_push($stack, array($maxLoc, $current[1]));
	}
	}
	}

	$polyline = new stdClass();
	$polyline->rawPoints = $this->createEncodings($points, $dists);
	$polyline->levels = $this->encodeLevels($points, $dists, $absMaxDist);
	/* $polyline->points = str_replace("\\","\\\\", $polyline->rawPoints);  changed by david to fix bug */
	$polyline->points=$polyline->rawPoints; /* changed by david to fix bug */
	$polyline->numLevels = $this->numLevels;
	$polyline->zoomFactor = $this->zoomFactor;

	return $polyline;
}

private function computeLevel($dd)
{
	if($dd > $this->verySmall)
	{
	$lev = 0;
	while($dd < $this->zoomLevelBreaks[$lev])
	{
		$lev++;
	}
	}
	return $lev;
	}

	private function distance($p0, $p1, $p2)
	{
	if($p1[0] == $p2[0] && $p1[1] == $p2[1])
	{
	$out = sqrt(pow($p2[0]-$p0[0],2) + pow($p2[1]-$p0[1],2));
	}
	else
	{
	$u = (($p0[0]-$p1[0])*($p2[0]-$p1[0]) + ($p0[1]-$p1[1]) * ($p2[1]-$p1[1])) / (pow($p2[0]-$p1[0],2) + pow($p2[1]-$p1[1],2));
		if($u <= 0)
		{
		$out = sqrt(pow($p0[0] - $p1[0],2) + pow($p0[1] - $p1[1],2));
		}
			if($u >= 1)
			{
			$out = sqrt(pow($p0[0] - $p2[0],2) + pow($p0[1] - $p2[1],2));
	}
	if(0 < $u && $u < 1)
	{
	$out = sqrt(pow($p0[0]-$p1[0]-$u*($p2[0]-$p1[0]),2) + pow($p0[1]-$p1[1]-$u*($p2[1]-$p1[1]),2));
	}
	}
	return $out;
	}

	private function encodeSignedNumber($num)
	{
	$sgn_num = $num << 1;
		if ($num < 0)
		{
			$sgn_num = ~($sgn_num);
	}
	return $this->encodeNumber($sgn_num);
	}

	private function createEncodings($points, $dists)
	{
            $encoded_points=null;
            $plat=0;
            $plng=0;
            
	for($i=0; $i<count($points); $i++)
	{
			if(isset($dists[$i]) || $i == 0 || $i == count($points)-1)
			{
			$point = $points[$i];
			$lat = $point[0];
			$lng = $point[1];
			$late5 = floor($lat * 1e5);
			$lnge5 = floor($lng * 1e5);
			$dlat = $late5 - $plat;
			$dlng = $lnge5 - $plng;
			$plat = $late5;
			$plng = $lnge5;
			$encoded_points .= $this->encodeSignedNumber($dlat) . $this->encodeSignedNumber($dlng);
			}
			}
			return $encoded_points;
	}

	private function encodeLevels($points, $dists, $absMaxDist)
	{
            $encoded_levels=null;
	if($this->forceEndpoints)
	{
	$encoded_levels .= $this->encodeNumber($this->numLevels-1);
			}
				else
				{
					$encoded_levels .= $this->encodeNumber($this->numLevels-$this->computeLevel($absMaxDist)-1);
			}
			for($i=1; $i<count($points)-1; $i++)
			{
			if(isset($dists[$i]))
			{
			$encoded_levels .= $this->encodeNumber($this->numLevels-$this->computeLevel($dists[$i])-1);
			}
			}
			if($this->forceEndpoints)
			{
			$encoded_levels .= $this->encodeNumber($this->numLevels -1);
			}
			else
			{
			$encoded_levels .= $this->encodeNumber($this->numLevels-$this->computeLevel($absMaxDist)-1);
			}
			return $encoded_levels;
			}

			private function encodeNumber($num)
			{
                            $encodeString='';
				while($num >= 0x20)
				{
				$nextValue = (0x20 | ($num & 0x1f)) + 63;
				$encodeString .= chr($nextValue);
				$num >>= 5;
			}
			$finalValue = $num + 63;
			$encodeString .= chr($finalValue);
			return $encodeString;
			}
			}


			
?>