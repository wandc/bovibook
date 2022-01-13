<?php 
require_once($_SERVER['DOCUMENT_ROOT'].'/global.php');

/**
 * This file holds methods that are used by various cropping classes.
 * 
 * 
 * 
 * 
 */

class CroppingHelper {

    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
    
    
    /** get fields id,name,common name and return as array */
    function allActiveFieldsArray() {
    $sql = "SELECT field.id,field.common_name,field.alpha_numeric_id FROM cropping.field WHERE active=true ORDER BY alpha_numeric_id";
        $res = $GLOBALS ['pdo']->query($sql);
        $fieldArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $fieldArray [$row ['id']] = $row ['alpha_numeric_id'] .' - '. $row ['common_name'];
        }
     return $fieldArray;
    }
    
    
    // rule to check it is valid kml lat/long/alt input.
    function checkAreaIsValid($valueArr, $elementNameArr) {
        // generic loop to convert $form elements we want into a usuable array
        // I really hope this isn't how quickforms was designed.
        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData [$elementName] = $valueArr [$count];
            $count++;
        }

        //choose which type of string we have
        $area=self::convertLineStringORNumNumNum($formData['polygon']);

        
        //check all is well or not...
        if ($area == - 1) {
            return false;
        } else {
            return true;
        }
    }

    /**
    *  Used to chose which type of string we have
    */
    function convertLineStringORNumNumNum($strToConvert) {

   //=== needed to work right
    if (stripos(trim($strToConvert),'LINESTRING Z ') === 0) {  
    //for postgis format
        $area=self::convertLineStringToLatLngArray($strToConvert);
        }
        else {
        //for kml format
        $area = self::convertLatLongHeightStrToArray($strToConvert);
        }
    return $area;
    }
    
    
    
    
     /* takes a string of the form: LINESTRING(-64.9859932840402 45.9772531297788,-64.98 ... ) */
     /* converts to lat/lng array */
     function convertLineStringToLatLngArray($str) {
   
     //initial check for line string that the text is found at the start.
     if (stripos(trim($str),'LINESTRING Z ') != 0) {
     return -1;
     }
     
     // LINESTRING(-64.9859932840402 45.9772531297788,-64.9831590847232 45.9777573906762,-64.983600970827 45.9792913290286,-64.9864813034962 45.9788012867401,-64.9859932840402 45.9772531297788)
      $str = str_replace('LINESTRING Z (', '', $str);
      $str = str_replace(')', '', $str);
      $str = str_replace(',', '|', $str);
      $str = trim($str);
      $str = str_replace(' ', ',', $str);
    
        $outArray=array();
        //tokenize by lat,lng
        $strArray = (explode('|', $str));
        foreach ($strArray as $key => $value) {
            //seperate lat and lng
           $tArr = (explode(',', $value));
           
           //check for coordinate reversal, NOTE::only works for local reagion!!
           if (($tArr[1]>= 44) AND  ($tArr[1]<= 47)) {
           $outArray[$key]['lat']=$tArr[1];
           $outArray[$key]['lng']=$tArr[0];
           }
           else {
           $outArray[$key]['lat']=$tArr[0];
           $outArray[$key]['lng']=$tArr[1];   
               
           }
           $outArray[$key]['alt']=0; //height is 0.
           if ((is_numeric($outArray[$key]['lat']) == false) ||   (is_numeric($outArray[$key]['lng']))==false) {
                return -1;
            }
     }
     
     //check for closed or not.
     if (self::checkLatLngArrayIsClosed($outArray) ==-1) { return -1; }
     
     return $outArray;
     
     }
    
     //takes an array of lat/lng/heights and 
     //converts to -64.9604387335924,45.98314879896523,0
     //Used to put array as kml back into google earth.
    function ConvertArrayToLatLongHeightStr($inArr){      
        $str='';
      foreach ($inArr as $key => $value) {
           $str=$str.$value['lng'].','.$value['lat'].','.$value['alt'].' ';  
    }
    return $str;
    } 
     
     
    /**
    * check first lat long is equal to last lat long.
    */
    function checkLatLngArrayIsClosed($inArray) {
        
    $startSum=$inArray[0]['lat']+ $inArray[0]['lng']+ $inArray[0]['alt'];
    $endElem=end($inArray);
    $finishSum=$endElem['lat']+ $endElem['lng']+ $endElem['alt'];
    
    //if sums are equal the polygon is closed. (simple definition).
    if ($startSum != $finishSum) {
    return -1;
    }  
    else  {
    return $inArray;
    }
    }
    
    
    
    /*
     * Takes in a string of:
     * -64.9604387335924,45.98314879896523,0 and converts it to a cleaned up
     * array. *******************************
     */
    function convertLatLongHeightStrToArray($areaRawStr) {

        $areaRawStr = trim($areaRawStr); // trim
        // initial sanity check. see if it is numbers.
        $sanityTest = str_replace(' ', '|', $areaRawStr);
        $sanityTest = str_replace(',', '|', $sanityTest);

        //now tokenize based on ,.
        $sanityTestArr = (explode('|', $sanityTest));
        foreach ($sanityTestArr as $key => $value) {
            if (is_numeric($value) == false) {
                return - 1;
            }
        }
        // take string and break it up based on spaces.
        $areaArr = (explode(' ', $areaRawStr));
        // get rid of empty spaces.
        $areaArrNew = array_filter($areaArr);
          $outArray=array();
            
        // now check what is left is in number,number,number format.
        foreach ($areaArrNew as $key => $value) {
           $tArr = explode(',', $value);
           $outArray[$key]['lat']=$tArr[1];
           $outArray[$key]['lng']=$tArr[0];
           $outArray[$key]['alt']=0; //height is 0.
           
           if ((is_numeric($outArray[$key]['lat']) == false) ||   (is_numeric($outArray[$key]['lng']))==false) {
                return -1;
            }
            
         }

        //check for closed or not.
        if (self::checkLatLngArrayIsClosed($outArray) ==-1) { return -1; }
        
        return $outArray;
    }
    
    
    /**
     * *******************************************************
     * Takes an area array and converts it to polygon string *
     * NOTE: polygon only supports 2d, so we strip height out.
     * *
     * ********************************************************
     */
    function kmlArrayToLinestringStr($kmlArr) {
        $str = '';
        foreach ($kmlArr as $key => $value) {
           
            $str = $str . ", {$value['lng']} {$value['lat']} {$value['alt']}"; //must be 3d for datum DB table
        }
        // remove first comma and return;
        $ret = '' . substr($str, 1) . '';

        return $ret;
    }

    /**
     * *******************************************************
     * Takes an area array and converts it to polygon string *
     * NOTE: polygon only supports 2d, so we strip height out.
     * *
     * ********************************************************
     */
    function kmlArrayToPolygonStr($kmlArr) {
        $str = '';
        foreach ($kmlArr as $k => $v) {
            $onelineArr = explode(',', $v);
            $str = $str . ",({$onelineArr[0]},{$onelineArr[1]})";
        }
        // remove first comma and add brackets and return;
        $ret = '( ' . substr($str, 1) . ' )';

        return $ret;
    }

    ////////////////////////////////////////////////////////////////////////////////
   
    
    /* takes an array of the form $array[$key]['lat'] and outputs a javascript string */

    static public function latlangArrayToGoogleMapsJavascriptString($latlngArray) {

        //now start creating google maps polgon array
        $str = '';
        $str = $str . 'var polygonPoints = [';

        //now take the array and do each point.
        foreach ($latlngArray as $val) {
          
            $lat=$val['lat'];
            $lng=$val['lng'];

            $str = $str . "new google.maps.LatLng(" . $lat . "," . $lng . "),";
        }

        $str = substr($str, 0, strlen($str)); //remove trailing ,

        $str = $str . '];'; //all done.....

     
        return $str;
    }

    /** return a javascript polgon variable for google maps from a field. */
    static public function fieldBorderToGoogleMapsJavascriptVar($field_id) {

        $sql = "SELECT gis.ST_AsText(gis.ST_Boundary(border_geom)) AS linestring FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $latlngArray = self::convertLineStringToLatLngArray($row['linestring']); 
        
        return self::latlangArrayToGoogleMapsJavascriptString($latlngArray);  
    }
    
     /** return a javascript point  variable for the centroids from a field. */
    static public function fieldBorderCentroidToGoogleMapsJavascriptPoint($field_id,$basic=false) {

        $sql = "SELECT gis.ST_AsText(gis.ST_Centroid(border_geom)) AS centroid FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $row['centroid']=trim($row['centroid']);
        $row['centroid']=str_replace('POINT(','', $row['centroid']);
        $row['centroid']=str_replace(')','', $row['centroid']);
        $row['centroid']=str_replace(' ','|', $row['centroid']);
        list($lng,$lat) = explode('|',$row['centroid']);
        //basic used for google maps javascript interactive maps.
        if ($basic==true) {
        return "$lat,$lng";
        }
        else {
        return "mapCenterPoint=new google.maps.LatLng($lat,$lng);";
        }
    }
    
    
//////////////////

    static public function PolygonStrToKmlStr($polygon)
{
$polygon=str_replace('),(',',0 ', $polygon);
$polygon=str_replace('((','', $polygon);
$polygon=str_replace('))','', $polygon);
return $polygon;
}

  /** returns an array of currently active fields */
static public function listOfCurrentFields($mode='default') {
     
  $sql="SELECT id,alpha_numeric_id,common_name FROM cropping.fieldcurr";
	    $res = $GLOBALS['pdo']->query($sql);
            if ($mode=='compact') {
	     $fieldNameArray[null] = null; //dummy value
            }
            else {
                $fieldNameArray[null] = "Please select a field..."; //dummy value
            }
	      while($row=$res->fetch(PDO::FETCH_ASSOC)) { 	
                  $fieldNameArray[$row['id']]=$row['alpha_numeric_id'] .' - '. $row['common_name'];
             } 
  
  return $fieldNameArray;
}  





 
 /**
  * google maps directions to field 
  * ie URL: https://www.google.com/maps/search/?api=1&query=47.5951518,-122.3316393
  */
 public function directionsToField($field_id) {
     
   $sql="SELECT gis.ST_AsText(gis.ST_Centroid(border_geom)) as centre FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1";
 	$res = $GLOBALS['pdo']->query($sql);
 	
 	if ($res->rowCount() > 0) {
 		$row=$res->fetch(PDO::FETCH_ASSOC);
                 $centerPoint = self::convertPostgisPointStringToLanLngPointString($row['centre']);
            
 		
 		return 'https://www.google.com/maps/search/?api=1&map_action=satellite&query='.round($centerPoint[1],5).','.round($centerPoint[0],5);
 	}
 	else {
 	return null; //no area for retired fields. 
 	}  
        
      
 }
 
 public function convertPostgisPointStringToLanLngPointString($point) {
        $str = $point;
        $str = str_replace('POINT(', '', $str);
        $str = str_replace(')', '', $str);
        $tArr = (explode(' ', $str));
        return $tArr;
    }
 
 
 /**
 * returns the field area in hectares via postgis.
 */
 public function fieldArea($field_id)
 {
 	//we have to do a transform to NB corrdinate system first, because it is orignally in degrees and we have to localize for our part of the world.
        $sql=" 	SELECT gis.ST_AREA(gis.ST_Transform(border_geom, 2036))/10000 as area FROM cropping.fieldcurr WHERE id=$field_id LIMIT 1	";
 	$res = $GLOBALS['pdo']->query($sql);
 	
 	if ($res->rowCount() > 0) {
 		$row=$res->fetch(PDO::FETCH_ASSOC);
 		$x=round(($row['area']),2);
 		return $x;
 	}
 	else {
 	return null; //no area for retired fields. 
 	}
 	
 }
 
 
 
 //No longer needed with postgis....
static public function PolygonareaLatLonInHectares($polygon)
{
$kmlStr=(self::PolygonStrToKmlStr($polygon)); 
//should not need to abs, but for some polygons we get a negative area, this could mean a bug somehwere deeper.
return abs($GLOBALS['MiscObj']->areaLatLonInHectares($kmlStr));
}

/**
     * calculates area from x,y,z x,y,z corrdinates.
     */
    static function areaLatLonInHectares($perimeterCoordinates) {
        //echo("<p>$perimeterCoordinates</p>");
        if ($perimeterCoordinates != 0) {
//split up lat/lon into x and y arrays of points.
            $xy = preg_split('/,0 /', $perimeterCoordinates);
            $count = 0;
            foreach ($xy as $val) {
                if (trim($val) == 0) {
                    list($xTemp, $yTemp) = array(0, 0);
                } else {
                    list($xTemp, $yTemp) = explode(",", $val);
                }
                $x[$count] = $xTemp;
                $y[$count] = $yTemp;
                $count++;
            }
            print_r($x);
            print_r($y);
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
        } else {
            return 0;
        }
    }

}

//end class
?>