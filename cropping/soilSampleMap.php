<?php 
require_once($_SERVER['DOCUMENT_ROOT'].'/global.php');
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');

function main() {

 //grab a list of fields.	
	    $sql="SELECT field.id as field_id, field.alpha_numeric_id,field.common_name,field.placemark_coordinates,field.perimeter_coordinates,field.begin_time as field_begin_time ,field.end_time as field_end_time,seed.google_polygon_outline_colour, seed.google_polygon_fill_colour,seed.general_type,seed.specific_type, seeding.begin_time as seeding_begin_time,seeding.end_time as seeding_end_time, seeding.id as seeding_id,perennial,ph,cec,soil_index 
FROM landmanagement.field
LEFT JOIN landmanagement.seeding ON field.id = seeding.field_id
LEFT JOIN landmanagement.seed ON seeding.seed_id=seed.id 
LEFT JOIN landmanagement.soil_test ON soil_test.field_id = field.id
ORDER BY field.alpha_numeric_id LIMIT 99";

//select all fields for this year.
$sql="SELECT DISTINCT ON (field.alpha_numeric_id)  field.id as field_id, field.alpha_numeric_id,field.common_name,field.placemark_coordinates,field.perimeter_coordinates,field.begin_time as field_begin_time ,field.end_time as field_end_time,seed.google_polygon_outline_colour, seed.google_polygon_fill_colour,seed.general_type,seed.specific_type, seeding.begin_time as seeding_begin_time,seeding.end_time as seeding_end_time, seeding.id as seeding_id,perennial,ph,cec,soil_index 
FROM landmanagement.field
LEFT JOIN landmanagement.seeding ON field.id = seeding.field_id
LEFT JOIN landmanagement.seed ON seeding.seed_id=seed.id 
LEFT JOIN landmanagement.soil_test ON soil_test.field_id = field.id
WHERE field.begin_time <= '2009-01-01 00:00:00' AND  field.end_time >= '2009-12-31 23:59:59'
ORDER BY field.alpha_numeric_id LIMIT 1000";

            $res = $GLOBALS['pdo']->query($sql);
	   
	   //generate new colour gradient array of hex numbers.
	   $cGrad=new ColourGradient();
	   $gradArray=$cGrad->main(0xB7B7FF,0x0000FF,31);
	   //print_r($gradArray);
	   

	    $textLoop='';
	    $textLoop=$textLoop.outputStyle(); //output style info at top.
	    
            while (($line = $res->fetch(PDO::FETCH_ASSOC))) {
	    //print_r($line);
	    
	     $style="{$line['google_polygon_fill_colour']}{$line['google_polygon_outline_colour']}";
            $descriptionHTMLStr="{$line['common_name']} <br/> Crop: {$line['general_type']} <br/> Subtype: {$line['specific_type']} <br/> Land Size $areaHa (ha)<br/>Tillable Size $areaHa (ha)<br/>";
	    
	    
	    //$rColour='ff'.$gradArray[rand(1, 32)]; //create randome colour gradient for map.
	    $rColour=$line['google_polygon_fill_colour']; //use field colour
	    
	    
	    $textO='';
$textO=outputStylePolygon($style,$line['google_polygon_outline_colour'],$rColour); //paints fields different colours
//$textO=$textO.outputPlacemarkPoint($line['alpha_numeric_id'],"sn_5_copy3",$descriptionHTMLStr,$line['placemark_coordinates'],'');
$textO=$textO.outputPlacemarkPolygon($line['alpha_numeric_id']."-polygon",$style,$descriptionHTMLStr,$line['perimeter_coordinates'],'');
	    $textLoop=$textLoop.$textO;
            }
return $textLoop;
}

function outputPlacemarkPoint($name,$styleURL,$descriptionHTMLStr,$coordinatesHTMLStr,$addonText)
{
$outputStr='';
$outputStr=$outputStr ."\t"."\t"."\t".("<Placemark>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<name>$name</name>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<description>$descriptionHTMLStr</description>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<styleUrl>#$styleURL</styleUrl>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<Point>")."\r";
		$outputStr=$outputStr ."\t"."\t"."\t"."\t".("<coordinates>$coordinatesHTMLStr</coordinates>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("</Point>")."\r";
$outputStr=$outputStr ."\t"."\t"."\t".("</Placemark>")."\r";
//most inner element, so tack on $addonText
$outputStr=$outputStr.$addonText;
return $outputStr;
}

function outputPlacemarkPolygon($name,$styleURL,$descriptionHTMLStr,$coordinatesHTMLStr,$addonText)
{
$outputStr='';
$outputStr=$outputStr ."\t"."\t"."\t".("<Placemark>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<name>$name</name>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<description>$descriptionHTMLStr</description>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<styleUrl>#$styleURL</styleUrl>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<Polygon>")."\r";
		$outputStr=$outputStr ."\t"."\t"."\t".("<tessellate>1</tessellate>")."\r";	
		$outputStr=$outputStr ."\t"."\t"."\t".("<outerBoundaryIs>")."\r";
			$outputStr=$outputStr ."\t"."\t"."\t".("<LinearRing>")."\r";
				$outputStr=$outputStr ."\t"."\t"."\t"."\t".("<coordinates>$coordinatesHTMLStr</coordinates>")."\r";
			$outputStr=$outputStr ."\t"."\t"."\t".("</LinearRing>")."\r";
		$outputStr=$outputStr ."\t"."\t"."\t".("</outerBoundaryIs>")."\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("</Polygon>")."\r";


$outputStr=$outputStr ."\t"."\t"."\t".("</Placemark>")."\r";
//most inner element, so tack on $addonText
$outputStr=$outputStr.$addonText;
return $outputStr;
}

function outputStyle()
{

//no icon style.
$outputStr='';
$outputStr=$outputStr ."\r".'	<Style id="sn_5_copy3">'."\r";
$outputStr=$outputStr .'		<IconStyle>'."\r";
$outputStr=$outputStr .'			<Icon>'."\r";
$outputStr=$outputStr .'			</Icon>'."\r";
$outputStr=$outputStr .'		</IconStyle>'."\r";
$outputStr=$outputStr .'		<ListStyle>'."\r";
$outputStr=$outputStr .'		</ListStyle>'."\r";
$outputStr=$outputStr .'	</Style>'."\r";

/*
$outputStr='';
$outputStr=$outputStr .'<Style id="sn_5_copy3">';
$outputStr=$outputStr .'		<IconStyle>';
$outputStr=$outputStr .'		<scale>0.8</scale>';
$outputStr=$outputStr .'			<Icon>';
$outputStr=$outputStr .'				<href>http://maps.google.com/mapfiles/kml/paddle/red-stars.png</href>';
$outputStr=$outputStr .'	</Icon>';
$outputStr=$outputStr .'	<hotSpot x="32" y="1" xunits="pixels" yunits="pixels"/>';
$outputStr=$outputStr .'	</IconStyle>';
$outputStr=$outputStr .'	<LabelStyle>';
$outputStr=$outputStr .'			<scale>0.8</scale>';
$outputStr=$outputStr .'	</LabelStyle>';
$outputStr=$outputStr .'.<ListStyle>';
$outputStr=$outputStr .'<ItemIcon>';
$outputStr=$outputStr .'<href>http://maps.google.com/mapfiles/kml/paddle/red-stars-lv.png</href>';
$outputStr=$outputStr .'</ItemIcon>';
$outputStr=$outputStr .'</ListStyle>';
$outputStr=$outputStr .'</Style>';
*/
return $outputStr;
}

function outputStylePolygon($name,$lineColour8hex,$fillColour8hex)
{
$outputStr='';
$outputStr=$outputStr ."<Style id=\"$name\">"."\r";	
$outputStr=$outputStr .'		<LineStyle>'."\r";	
$outputStr=$outputStr ."			<color>$lineColour8hex</color>"."\r";	
$outputStr=$outputStr .'			<width>1.2</width>'."\r";	
$outputStr=$outputStr .'		</LineStyle>'."\r";
$outputStr=$outputStr .'		<PolyStyle>'."\r";	
$outputStr=$outputStr ."			<color>$fillColour8hex</color>"."\r";	
$outputStr=$outputStr .'		</PolyStyle>'."\r";	
$outputStr=$outputStr .'	</Style>'."\r";	
return $outputStr;
}
 
 
 //print out everything we generated with kml headers.
function finalOutput($contentText) {


header("Expires: 5");
header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
header("Cache-Control: private",false); 
header("Content-Type: application/vnd.google-earth.kml+xml;charset=UTF-8");
header("Content-Disposition: attachment; filename=\"mapsandstuff1.kml\"");



$start='<kml xmlns="http://earth.google.com/kml/2.0">';
$end='</kml>';

print($start.$contentText.$end);
}
 
 //
$textLoop=main(); 
finalOutput($textLoop);



class ColourGradient {


public function main($colourBegin,$colourEnd,$steps) {
return  $this->generateGradient($colourBegin,$colourEnd,$steps);
}


private function generateGradient($colourBegin,$colourEnd,$steps) {
//based on: http://www.herethere.net/~samson/php/color_gradient/color_gradient_generator.php.txt
//format of colours is hex, ie 0xffffff. NO QUOTES, it's not a string.
//0xB7B7FF to 0x0000FF will give a nice blue gradient


//break into rgb
 $theR0 = ($colourBegin & 0xff0000) >> 16;
 $theG0 = ($colourBegin & 0x00ff00) >> 8;
 $theB0 = ($colourBegin & 0x0000ff) >> 0;

 $theR1 = ($colourEnd & 0xff0000) >> 16;
 $theG1 = ($colourEnd & 0x00ff00) >> 8;
 $theB1 = ($colourEnd & 0x0000ff) >> 0;

// go through main loop to create gradient of colours
$gradArray=null;
 for ($i = 0; $i <= $steps; $i++) {
    $theR = $this->interpolate($theR0, $theR1, $i, $steps);
    $theG = $this->interpolate($theG0, $theG1, $i, $steps);
    $theB = $this->interpolate($theB0, $theB1, $i, $steps);

    $theVal = ((($theR << 8) | $theG) << 8) | $theB;
    
//convert from hex to string.
$gradArray[$i]=sprintf("%06X", $theVal);

}

//return array
return $gradArray;
}

  // return the interpolated value between pBegin and pEnd
private function interpolate($pBegin, $pEnd, $pStep, $pMax) {
    if ($pBegin < $pEnd) {
      return (($pEnd - $pBegin) * ($pStep / $pMax)) + $pBegin;
    } else {
      return (($pBegin - $pEnd) * (1 - ($pStep / $pMax))) + $pEnd;
    }
}


}//end class

?>