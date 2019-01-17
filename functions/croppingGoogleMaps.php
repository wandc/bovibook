<?php  
/**
 *  this has its own independent DB connection for some reason. Dec 2015. 
 *  Change to use global. FIXME: IS THIS file EVEN USED?
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/misc.inc');
    require_once($_SERVER['DOCUMENT_ROOT'].'/functions/croppingHelper.inc');
}

header("Content-Transfer-Encoding: binary");
header("Pragma: no-cache");
header("Expires: 0");



 $outputType='html'; //choose kml or kmz or html

if ($outputType=='kml') {
header("Content-disposition: attachment; filename=area.kml");
header("Content-Type: application/vnd.google-earth.kml+xml kml; charset=utf8");
$kmlData=main();
print($kmlData);
}
elseif ($outputType=='kmz') {
$kmlData=main();
header("Content-disposition: attachment; filename=area.kmz");
header("Content-type: application/vnd.google-earth.kmz");
$KMZfile = new KMZfile();  
$KMZfile -> add_file($kmlData, "doc.kml");
echo $KMZfile->file();//how to output the object?
}
else if ($outputType=='html') {
$kmlData=main();
print($kmlData); 
}  
else {
print("ERROR MUST CHOOSE KML OR KMZ OR HTML<br>\n\r");
}


function main() {

// the limit 1 is hackish. should be db constraints in.
$query = 
"
SELECT *,gis.ST_AsText(gis.ST_Centroid(seed_event_geom)) as placemark_coordinates, gis.ST_AsText(gis.ST_Boundary(seed_event_geom)) AS linestring,gis.ST_AREA(gis.ST_Transform(seed_event_geom, 2036))/10000 as area,
( SELECT table_id
                   FROM cropping.all_event all_event
                  WHERE all_event.field_id = fieldcurr.id AND all_event.event_time = (( SELECT max(all_event.event_time) AS max
                           FROM cropping.all_event all_event
                          WHERE all_event.field_id = fieldcurr.id AND all_event.event_time <= 'now'::text::date)) LIMIT 1) AS last_event_type
FROM cropping.fieldcurr
LEFT JOIN cropping.seed_event ON seed_event.id=fieldcurr.seed_event_id
LEFT JOIN cropping.seed ON seed_event.seed_id=seed.id 
LEFT JOIN cropping.seed_category ON seed.seed_category_id=seed_category.id 
LIMIT 1000
";
$res = $GLOBALS['pdo']->query($query);



$placeMarkText='';

$outputStylePlacemarkStr=null;
$outputStylePlacemarkStr=outputStylePlacemark(); //output all the styles for placemarks just once.

while ($line = $res->fetch(PDO::FETCH_ASSOC)) {

/////
/* based on last_event_type change the placemark icon via its style */
$placemarkStyleNumStr=null;
 switch ($line['last_event_type']) {		  
    case 'seed_event':
          $marker_number=3;
        break;
    case 'seed_event_scheduled':
          $marker_number=4;
        break;    
    case 'manure_event':
         $marker_number=5;
        break;
    case 'spray_event':
         $marker_number=6;
        break;    
    case 'tillage_event':
         $marker_number=7;
        break;        
     default:   
         $marker_number=0;
    }
    
     $placemarkStyleNumStr="#placemark_number_$marker_number";
/////

////
/* based on ???? change the border colour */
$line['google_polygon_outline_colour']='ff0000ff'; //red
$line['google_polygon_outline_colour']='ff00ff00'; //greed


//if the crop needs planted every year, show brown colour instead.
if ( ($line['perennial'] =='f') && (strtotime($line['event_time']) < strtotime(date('Y',strtotime('now')) . '-01-01 00:00:00'))) {
$line['google_polygon_fill_colour']='502878B4';
}

//outline and fill pastures differently
if ($line['pasture'] =='t') {
$line['google_polygon_outline_colour']='50000000';
$line['google_polygon_fill_colour']='5014F0B4';
}


//for postgis linestring
$line['linestring']=str_replace('LINESTRING(','',$line['linestring']);
$line['linestring']=str_replace(')','',$line['linestring']);
$line['linestring']=str_replace(' ','|',$line['linestring']);
$line['linestring']=str_replace(',',',0 ',$line['linestring']);
$line['linestring']=str_replace('|',',',$line['linestring']);
  
  $line['perimeter_coordinates']=$line['linestring'];
  
   $line['placemark_coordinates']=str_replace('POINT(','',$line['placemark_coordinates']);
   $line['placemark_coordinates']=str_replace(')','',$line['placemark_coordinates']);
   $line['placemark_coordinates']=str_replace(' ',',',$line['placemark_coordinates']);   
  
//calculate area.
$areaHa2=$line['areaha']; //probbaly wrong.
$areaHa=($line['area']);

$style="{$line['google_polygon_fill_colour']}{$line['google_polygon_outline_colour']}";
$extra=(CroppingHelper::displayFieldInfo($line['field_id'])); //new description from field query
$oldDescription="{$line['common_name']} <br/> Field ID: {$line['field_id']} <br/> Crop: {$line['general_type']} <br/> Subtype: {$line['specific_type']} <br/> Land Size $areaHa (ha) or $areaHa2<br/>Tillable Size $areaHa (ha)<br/>";
$descriptionHTMLStr=$extra;

$textO='';
$textO=outputStylePolygon($style,$line['google_polygon_outline_colour'],$line['google_polygon_fill_colour']); //paints fields different colours
$textO=$textO.outputPlacemarkPoint($line['alpha_numeric_id'],"$placemarkStyleNumStr",$descriptionHTMLStr,$line['placemark_coordinates'],'');
$textO=$textO.outputPlacemarkPolygon($line['alpha_numeric_id']."-polygon",$style,'',$line['perimeter_coordinates'],'');
$placeMarkText=$placeMarkText.$textO;

} //end query loop

$encapsulatedText=$placeMarkText;
$encapsulatedText=$outputStylePlacemarkStr.$encapsulatedText;
$encapsulatedText=outputDocument("Little River Map Items",$encapsulatedText);
$encapsulatedText=outputKML($encapsulatedText);


return $encapsulatedText;
}


function outputKML($encapsulatedText)
{
$outputStr='';
$outputStr=$outputStr . '<?php xml version="1.0" encoding="UTF-8"?>'."\n\r";
$outputStr=$outputStr . '<kml xmlns="http://earth.google.com/kml/2.2">'."\n\r";
$outputStr=$outputStr . $encapsulatedText;
$outputStr=$outputStr . '</kml>'."\n\r";
return $outputStr;
}

function outputDocument($name,$encapsulatedText)
{
$outputStr='';
$outputStr=$outputStr . "\t".'<Document>'."\n\r";
$outputStr=$outputStr . "\t"."\t"."<name>$name</name>"."\n\r";
$outputStr=$outputStr . $encapsulatedText;
$outputStr=$outputStr . "\t".'</Document>'."\n\r";
return $outputStr;
}

function outputFolder($name,$timeSpanStart,$timeSpanEnd,$encapsulatedText)
{
$outputStr='';
$outputStr=$outputStr.'<Folder>'."\n\r";
$outputStr=$outputStr.'<name>folder action</name>'."\n\r";
$outputStr=$outputStr.'<open>1</open>'."\n\r";
$outputStr=$outputStr.'<TimeSpan>'."\n\r";
$outputStr=$outputStr."<begin>$timeSpanStart</begin>"."\n\r";
$outputStr=$outputStr."<end>$timeSpanEnd</end>"."\n\r";
$outputStr=$outputStr.'</TimeSpan>'."\n\r";
$outputStr=$outputStr . $encapsulatedText;
$outputStr=$outputStr.'</Folder>'."\n\r";
return $outputStr;
}

function outputPlacemarkPoint($name,$styleURL,$descriptionHTMLStr,$coordinatesHTMLStr,$addonText)
{
$outputStr='';
$outputStr=$outputStr ."\t"."\t"."\t".("<Placemark>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<name>$name</name>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<description>$descriptionHTMLStr</description>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<styleUrl>$styleURL</styleUrl>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<Point>")."\n\r";
		$outputStr=$outputStr ."\t"."\t"."\t"."\t".("<coordinates>$coordinatesHTMLStr</coordinates>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("</Point>")."\n\r";
$outputStr=$outputStr ."\t"."\t"."\t".("</Placemark>")."\n\r\n\r";
//most inner element, so tack on $addonText
$outputStr=$outputStr.$addonText;
return $outputStr;
}

function outputPlacemarkPolygon($name,$styleURL,$descriptionHTMLStr,$coordinatesHTMLStr,$addonText)
{
$outputStr='';
$outputStr=$outputStr ."\t"."\t"."\t".("<Placemark>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<name>XXX$name</name>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<description>YYY$descriptionHTMLStr</description>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<styleUrl>#$styleURL</styleUrl>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("<Polygon>")."\n\r";
		$outputStr=$outputStr ."\t"."\t"."\t".("<tessellate>1</tessellate>")."\n\r";	
		$outputStr=$outputStr ."\t"."\t"."\t".("<outerBoundaryIs>")."\n\r";
			$outputStr=$outputStr ."\t"."\t"."\t".("<LinearRing>")."\n\r";
				$outputStr=$outputStr ."\t"."\t"."\t"."\t".("<coordinates>$coordinatesHTMLStr</coordinates>")."\n\r";
			$outputStr=$outputStr ."\t"."\t"."\t".("</LinearRing>")."\n\r";
		$outputStr=$outputStr ."\t"."\t"."\t".("</outerBoundaryIs>")."\n\r";
	$outputStr=$outputStr ."\t"."\t"."\t".("</Polygon>")."\n\r";


$outputStr=$outputStr ."\t"."\t"."\t".("</Placemark>")."\n\r\n\r";
//most inner element, so tack on $addonText
//$outputStr=$outputStr.$addonText;
return $outputStr;
}

/* output all the styles, but only once */
function outputStylePlacemark()
{

///define styles  
$out[1]['number']=1;  
$out[1]['icon']='http://maps.google.com/mapfiles/kml/pal4/icon56.png';
$out[1]['scale']=0.8;
//
$out[2]['number']=2;  
$out[2]['icon']='http://maps.google.com/mapfiles/kml/paddle/red-stars-lv.png';
$out[2]['scale']=0.8;
//
$out[3]['number']=3;  
$out[3]['icon']='http://int.littleriver.ca/images/googlemaps/plant.png';
$out[3]['scale']=0.8;
//
$out[4]['number']=4;  
$out[4]['icon']='http://int.littleriver.ca/images/googlemaps/calendar.png';
$out[4]['scale']=0.8;
//
$out[5]['number']=5;  
$out[5]['icon']='http://int.littleriver.ca/images/googlemaps/manure.png';
$out[5]['scale']=0.8;
//
$out[6]['number']=6;  
$out[6]['icon']='http://int.littleriver.ca/images/googlemaps/spray.png';
$out[6]['scale']=0.8;
//
$out[7]['number']=7;  
$out[7]['icon']='http://int.littleriver.ca/images/googlemaps/tillage.png';
$out[7]['scale']=0.8;
//
$out[8]['number']=8;  
$out[8]['icon']='http://maps.google.com/mapfiles/kml/paddle/blu-circle_maps.png';
$out[8]['scale']=0.8;

$str1='';
foreach ($out as $key => $value) {
    $str1=$str1 . placemarkStyle($value['number'],$value['icon'],$value['scale']);
}

return $str1;
}

function placemarkStyle($style,$icon,$scale) {
$outputStr='';
$outputStr=$outputStr ."\r\n"."\r\n"."<Style id=\"placemark_number_$style\">"."\r\n";
$outputStr=$outputStr .'		<IconStyle>'."\r\n";
$outputStr=$outputStr ."		<scale>$scale</scale>"."\r\n";
$outputStr=$outputStr .'			<Icon>'."\r\n";
$outputStr=$outputStr ."				<href>$icon</href>"."\r\n";
$outputStr=$outputStr .'	                </Icon>'."\r\n";
$outputStr=$outputStr .'	                <hotSpot x="0" y="0" xunits="pixels" yunits="pixels"/>'."\r\n";
$outputStr=$outputStr .'	        </IconStyle>'."\r\n";
$outputStr=$outputStr .'	<LabelStyle>'."\r\n";
$outputStr=$outputStr ."			<scale>$scale</scale>"."\r\n";
$outputStr=$outputStr .'	</LabelStyle>'."\r\n";
$outputStr=$outputStr .'  <ListStyle>'."\r\n";
$outputStr=$outputStr .'             <ItemIcon>'."\r\n";
$outputStr=$outputStr ."                      <href>$icon</href>"."\r\n";
$outputStr=$outputStr .'             </ItemIcon>'."\r\n";
$outputStr=$outputStr .'  </ListStyle>'."\r\n";
$outputStr=$outputStr .'</Style>'."\r\n";
return $outputStr;
}

function outputStylePolygon($name,$lineColour8hex,$fillColour8hex)
{
$outputStr='';
$outputStr=$outputStr ."<Style id=\"$name\">"."\n\r";	
$outputStr=$outputStr .'		<LineStyle>'."\n\r";	
$outputStr=$outputStr ."			<color>$lineColour8hex</color>"."\n\r";	
$outputStr=$outputStr .'			<width>1.2</width>'."\n\r";	
$outputStr=$outputStr .'		</LineStyle>'."\n\r";
$outputStr=$outputStr .'		<PolyStyle>'."\n\r";	
$outputStr=$outputStr ."			<color>$fillColour8hex</color>"."\n\r";	
$outputStr=$outputStr .'		</PolyStyle>'."\n\r";	
$outputStr=$outputStr .'	</Style>'."\n\r";	
return $outputStr;
}



/* class below is based on http://www.nearby.org.uk/google/zip.class.phps */
/* use this to take kml and output kmz */
 class KMZfile  
{
	var $datasec = array();
	var $ctrl_dir = array();
	var $eof_ctrl_dir = "\x50\x4b\x05\x06\x00\x00\x00\x00";
	var $old_offset = 0;

	function add_dir($name)   
	{  
		$fr = "\x50\x4b\x03\x04";
		$fr .= "\x0a\x00";  
		$fr .= "\x00\x00";  
		$fr .= "\x00\x00";  
		$fr .= "\x00\x00\x00\x00";
		$fr .= pack("V",0);
		$fr .= pack("V",0);
		$fr .= pack("V",0);
		$fr .= pack("v", strlen($name) );
		$fr .= pack("v", 0 );
		$fr .= $name; 
		$fr .= pack("V",$crc);
		$fr .= pack("V",$c_len);
		$fr .= pack("V",$unc_len);
		$this -> datasec[] = $fr;
		$new_offset = strlen(implode("", $this->datasec));
		$cdrec = "\x50\x4b\x01\x02";
		$cdrec .="\x00\x00";
		$cdrec .="\x0a\x00";
		$cdrec .="\x00\x00";
		$cdrec .="\x00\x00";
		$cdrec .="\x00\x00\x00\x00";
		$cdrec .= pack("V",0);
		$cdrec .= pack("V",0);
		$cdrec .= pack("V",0);
		$cdrec .= pack("v", strlen($name) );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("v", 0 );
		$ext = "\x00\x00\x10\x00";
		$ext = "\xff\xff\xff\xff";
		$cdrec .= pack("V", 16 );
		$cdrec .= pack("V", $this -> old_offset );
		$this -> old_offset = $new_offset;
		$cdrec .= $name;
		$this -> ctrl_dir[] = $cdrec;
	}

	function add_file($data, $name)   
	{  
		$fr = "\x50\x4b\x03\x04";
		$fr .= "\x14\x00";
		$fr .= "\x00\x00";
		$fr .= "\x08\x00";
		$fr .= "\x00\x00\x00\x00";
		$unc_len = strlen($data);
		$crc = crc32($data);
		$zdata = gzcompress($data);
		$zdata = substr( substr($zdata, 0, strlen($zdata) - 4), 2);
		$c_len = strlen($zdata);
		$fr .= pack("V",$crc);
		$fr .= pack("V",$c_len);
		$fr .= pack("V",$unc_len);
		$fr .= pack("v", strlen($name) );
		$fr .= pack("v", 0 );
		$fr .= $name;
		$fr .= $zdata;
		$fr .= pack("V",$crc);
		$fr .= pack("V",$c_len);
		$fr .= pack("V",$unc_len);
		$this -> datasec[] = $fr;
		$new_offset = strlen(implode("", $this->datasec));
		$cdrec = "\x50\x4b\x01\x02";
		$cdrec .="\x00\x00";
		$cdrec .="\x14\x00";
		$cdrec .="\x00\x00";
		$cdrec .="\x08\x00";
		$cdrec .="\x00\x00\x00\x00";
		$cdrec .= pack("V",$crc);
		$cdrec .= pack("V",$c_len);
		$cdrec .= pack("V",$unc_len);
		$cdrec .= pack("v", strlen($name) );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("v", 0 );
		$cdrec .= pack("V", 32 );
		$cdrec .= pack("V", $this -> old_offset );
		$this -> old_offset = $new_offset;
		$cdrec .= $name;
		$this -> ctrl_dir[] = $cdrec;
	}

	function file() { 
		$data = implode("", $this -> datasec);
		$ctrldir = implode("", $this -> ctrl_dir);
		return $data.
			$ctrldir.
			$this -> eof_ctrl_dir.
			pack("v", sizeof($this -> ctrl_dir)).
			pack("v", sizeof($this -> ctrl_dir)).
			pack("V", strlen($ctrldir)).
			pack("V", strlen($data)).
			"\x00\x00";
	}
}  


?>