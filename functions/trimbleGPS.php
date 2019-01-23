<?php 
/**
 * Shows trimble GPS data on field query (or wherever else) 
 *
 */
class TrimbleGPS {

    public static $geom_to_intersect;

    public function __construct($geom_to_intersect) {
        if (($geom_to_intersect != null)) {
            self::$geom_to_intersect = $geom_to_intersect;
        } else {
            throw new Exception("Need an intersecting lat and long for field data");
        }
    }

    function main() {
        self::loadData2();
    }

    /*
     * returns a json object of all gps events for a field.
     */

    static function gpsEvents($field_id) {
        $sql = "SELECT border_geom FROM cropping.fieldcurr WHERE id=$field_id";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $geom_to_intersect = $row['border_geom'];


        $sql = "
        select array_to_json(array_agg(row_to_json(t)))
    from (

   With temp as (SELECT DISTINCT ON (event) *,(dateclosed || ' ' ||  timeclosed )::timestamp as close_timestamp FROM gis.coverage 
WHERE st_intersects( gis.ST_SetSRID(('$geom_to_intersect'::geometry),4326),(gis.ST_SetSRID(the_geom,4326)))
 )
 SELECT close_timestamp,event,total_time,coverage_area,implement_width,start_time,end_time,coverage_time	 
 FROM temp 
  JOIN  gis.summary ON summary.field_name=temp.event
ORDER BY close_timestamp DESC,event DESC

    ) t           
    ";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch();

        return ($row[0]);
    }

    static function eventList($field_id) {
        ?>
        <style>
            .ui-widget {
                font-size: 10px;
            }
        </style>

        <script type="text/javascript">
            "use strict";
            $(document).ready(function () {
                $(document).ready(function () {
                    $('#example').dataTable({
                        "iDisplayLength": 5,
                        "bJQueryUI": true,
                        "bProcessing": true,
                        "ajax": "<?php echo($GLOBALS['config']['HTTP']['HTML_API']);?>107/gpsEvents/<?php  echo($field_id); ?>",
                        "aaSorting": [[0, "desc"]],
                        "aoColumnDefs": [
                            {"sTitle": "close_timestamp", "aTargets": [0]},
                            {"sTitle": "event", "aTargets": [1]},
                            {"sTitle": "total_time", "aTargets": [2]},
                            {"sTitle": "coverage_area", "aTargets": [3]},
                            {"sTitle": "implement width", "aTargets": [4]},
                            {"sTitle": "start_time", "aTargets": [5]},
                            {"sTitle": "end_time", "aTargets": [6]},
                            {"sTitle": "coverage_time", "aTargets": [7]},
                        ],
                        "aoColumns": [
                            {"mData": "close_timestamp"},
                            {"mData": "event"},
                            {"mData": "total_time"},
                            {"mData": "coverage_area"},
                            {"mData": "implement_width"},
                            {"mData": "start_time"},
                            {"mData": "end_time"},
                            {"mData": "coverage_time"},
                        ]
                    });
                });
            });
        </script>
        <table id="example"></table>
        <?php 
    }

    /**
     * NOT USED YET
     * test code to convert all the multilinestirngs to one big polygon
     *  
     */
    public function loadData() {
        $geom_to_intersect = self::$geom_to_intersect;

        //find center of geom to center map.
        $sql2 = "SELECT gis.ST_AsText(gis.ST_Centroid(gis.ST_SetSRID(('$geom_to_intersect'::geometry),4326))) as center";
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        $centerPoint = self::convertPostgisPointStringToLanLngPoint($row2['center']);




        $sql1 = "SELECT DISTINCT ON (summary_created) event_name,coverage_area,implement_width,start_time,end_time,coverage_time,geo_point FROM gis.summary 
WHERE st_intersects( gis.ST_SetSRID(('$geom_to_intersect'::geometry),4326),(gis.ST_SetSRID(geo_point,4326)))
ORDER BY summary_created DESC";
        print( (new JQueryDataTable)->startBasicSql($sql1)); 
        print("<br/>");


        $sql = "
   CREATE TEMP TABLE temp1 as
  (
     SELECT dateclosed,0 as height,gis.ST_AsText(gis.ST_SetSRID(the_geom,4326)) AS multilinestring FROM gis.coverage 
WHERE st_intersects( gis.ST_SetSRID((SELECT border_geom FROM cropping.fieldcurr WHERE id=3235),4326),(gis.ST_SetSRID(the_geom,4326)))
ORDER BY dateclosed DESC,timeclosed DESC
  );
UPDATE temp1
  SET multilinestring=gis.ST_SimplifyPreserveTopology(multilinestring, 0.0001)
  WHERE gis.ST_IsValid(multilinestring) = false;

with one AS (
SELECT dateclosed, gis.ST_AsText(gis.ST_SetSRID(gis.ST_Union(multilinestring),4326)) as geom  from temp1 where gis.ST_IsValid(multilinestring) =true GROUP BY dateclosed
)
SELECT gis.ST_Area(gis.ST_Transform(gis.ST_Setsrid(geom, 4326), 2036))/10000 as area,*  FROM one

    ";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("File Date:{$row['dateclosed']}<br/>");
        }
        print("<br/>");


        //use last row....



        $outStr = '';
        $outStr = $outStr . self::latLngArrayToGoogleMapsJavascriptPolygon($this->convertPostgisPolygonToLanLngArray($row['geom'], '50'));
        //print("$outStr");
        self::showMap($outStr, $centerPoint);
    }

    /* shows files and caoverages */

    public function loadData2() {
        $geom_to_intersect = self::$geom_to_intersect;

        $sql1 = "With temp as (SELECT DISTINCT ON (event) *,(dateclosed || ' ' ||  timeclosed )::timestamp as close_timestamp FROM gis.coverage 
WHERE st_intersects( gis.ST_SetSRID(('$geom_to_intersect'::geometry),4326),(gis.ST_SetSRID(the_geom,4326)))
 )
 SELECT close_timestamp,event,total_time,coverage_area,implement_width,start_time,end_time,coverage_time	 
 FROM temp 
  JOIN  gis.summary ON summary.field_name=temp.event
ORDER BY close_timestamp DESC,event DESC limit 1 offset 2";
        $res1 = $GLOBALS['pdo']->query($sql1);
        $outStr = ''; //init
        while ($row1 = $res1->fetch(PDO::FETCH_ASSOC)) {
            //print_r($row1);
            //print("Date: {$row1['close_timestamp']} Name: {$row1['event']} {$row1['total_time']} {$row1['coverage_area']} ha {$row1['implement_width']} {$row1['start_time']}  {$row1['end_time']} {$row1['coverage_time']}<br/>");

            $outStr = $outStr . self::createPolylineArrayOfSingleEvent($row1['event']);
        }
        print("<br/>");

        //find center of geom to center map.
        $sql2 = "SELECT gis.ST_AsText(gis.ST_Centroid(gis.ST_SetSRID(('$geom_to_intersect'::geometry),4326))) as center";
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        $centerPoint = self::convertPostgisPointStringToLanLngPoint($row2['center']);




        self::showMap($outStr, $centerPoint);
    }

    /* this assumes and event name is unique */
    /* returns one whole event as a polyline array */

    private function createPolylineArrayOfSingleEvent($event) {
        $outStr = ''; //init
        $count = 0;

        $sql = "SELECT event,dateclosed,0 as height,gis.ST_AsText(gis.ST_SetSRID(the_geom,4326)) AS multilinestring 
                         FROM gis.coverage 
                         WHERE event='$event'";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() > 0) {



            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                if ($count == 0) {
                    //header of polineArr
                    $varName = md5($row['event']);
                    $outStr = $outStr . "var polylineArr$varName = new Array();";
                }

                $outStr = $outStr . self::latLngArrayToGoogleMapsJavascriptPolyline($this->convertPostgisMultiPolygonToLanLngArray($row['multilinestring'], $row['height']), $row['event']);
                $outStr = $outStr . "polylineArr$varName.push(line); ";
                $count++;
            }
        }

        //footer of polylineArr
        $outStr = $outStr . "
 for (var i=0; i<polylineArr$varName.length; i++) {
     
    polylineArr{$varName}[i].setMap(map);
}
 ";

        return $outStr;
    }

//POINT(-64.9601102202882 45.9816123758737) to new google.maps.LatLng(45.98, -64.96)
    private function convertPostgisPointStringToLanLngPoint($point) {
        $str = $point;
        $str = str_replace('POINT(', '', $str);
        $str = str_replace(')', '', $str);
        $tArr = (explode(' ', $str));
        return 'new google.maps.LatLng(' . $tArr[1] . ', ' . $tArr[0] . ')';
    }

    private function convertPostgisMultiLineStringToLanLngArray($multistring) {
        $str = $multistring;
        $str = str_replace('MULTILINESTRING((', '', $str);
        $str = str_replace('))', '', $str);
        $str = str_replace(',', '|', $str);
        $str = trim($str);
        $str = str_replace(' ', ',', $str);

        $outArray = array();
        //tokenize by lat,lng
        $strArray = (explode('|', $str));
        foreach ($strArray as $key => $value) {
            //seperate lat and lng
            $tArr = (explode(',', $value));
            $outArray[$key]['lat'] = $tArr[1];
            $outArray[$key]['lng'] = $tArr[0];
            $outArray[$key]['alt'] = 0; //height is 0.
        }
        return $outArray;
    }

    private function convertPostgisMultiPolygonToLanLngArray($multistring, $height) {
        $str = $multistring;
        $str = str_replace('MULTIPOLYGON(((', '', $str);
        $str = str_replace(')))', '', $str);
        $str = str_replace(',', '|', $str);
        $str = trim($str);
        $str = str_replace(' ', ',', $str);

        $outArray = array();
        //tokenize by lat,lng
        $strArray = (explode('|', $str));
        foreach ($strArray as $key => $value) {
            //seperate lat and lng
            $tArr = (explode(',', $value));
            $outArray[$key]['lat'] = $tArr[1];
            $outArray[$key]['lng'] = $tArr[0];
            $outArray[$key]['alt'] = $height; //height is 0.
        }
        return $outArray;
    }

    private function convertPostgisPolygonToLanLngArray($multistring, $height) {
        $str = $multistring;
        $str = str_replace('POLYGON((', '', $str);
        $str = str_replace('))', '', $str);
        $str = str_replace(',', '|', $str);
        $str = str_replace('(', '', $str);
        $str = str_replace(')', '', $str);
        $str = trim($str);
        $str = str_replace(' ', ',', $str);

        $outArray = array();
        //tokenize by lat,lng
        $strArray = (explode('|', $str));
        foreach ($strArray as $key => $value) {
            //seperate lat and lng
            $tArr = (explode(',', $value));
            $outArray[$key]['lat'] = $tArr[1];
            $outArray[$key]['lng'] = $tArr[0];
            $outArray[$key]['alt'] = $height; //height is 0.
        }
        return $outArray;
    }

//NOTE: duplicated from croppingHelper, fix to be generic
    static public function latlangArrayToGoogleMapsJavascriptStringPath($latlngArray) {

        //now start creating google maps polgon array
        $str = '';
        $str = $str . 'var data = [';

        //now take the array and do each point.
        foreach ($latlngArray as $val) {

            $lat = $val['lat'];
            $lng = $val['lng'];

            $str = $str . "new google.maps.LatLng(" . $lat . "," . $lng . ")," . "\n\r";
        }

        $str = substr($str, 0, strlen($str) - 3); //remove trailing ,

        $str = $str . '];' . "\n\r"; //all done.....


        return $str;
    }

    public function latLngArrayToGoogleMapsJavascriptPolyline($latlngArray) {
        $str = '';
        $str = $str . self::latlangArrayToGoogleMapsJavascriptStringPath($latlngArray);



        $str = $str . "     var line = new google.maps.Polyline({
          path: data,
          strokeColor: '#ff0000',
          strokeOpacity: 1.0,
          strokeWeight: 3
        });
        //line.setMap(map);
       
           
           
      ";
        return $str;
    }

    public function latLngArrayToGoogleMapsJavascriptPolygon() {
        $str = '';
        $str = $str . self::latlangArrayToGoogleMapsJavascriptStringPath($latlngArray);

        $str = $str . "     var line = new google.maps.Polygon({
          paths: data,
    strokeColor: '#000000',
    strokeOpacity: 0.8,
    strokeWeight: 0.05,
    fillColor: '#D7DF01',
    fillOpacity: 0.80
        });
        line.setMap(map);
      ";
        return $str;
    }

    function showMap($extraCode, $centerPoint) {
        /*
         * google maps inside a jquery tab solution:
         * http://stackoverflow.com/questions/10197128/google-maps-api-v3-not-rendering-competely-on-tabbed-page-using-twitters-bootst
         * I wonder if this will need a longer timeout on a slow connection.
         */
        ?>
        <div style="width:100%; height:600px;  margin: 0; padding: 0;" id="map_canvas"></div>
        <div id="panel">
            <input onclick="removeLine();" type=button value="Remove line">
            <input onclick="addLine();" type=button value="Restore line">
        </div>
        <script>
            $(document).ready(function () {
                console.log("ddddd");
                setTimeout(loadScript, 80); //critical, has to have time for tab to load first.

            });


            var flightPath;
            var map; //needs to be here, so we can call map from other functions so it is in scope.

            function initialize() {
                var title = "Title";
                var lat = 50;
                var lng = 50;

                var myOptions = {
                    title: title,
                    zoom: 17,
                    center: <?php  echo($centerPoint); ?>,
                    mapTypeId: google.maps.MapTypeId.ROADMAP
                }
                map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);


                var flightPlanCoordinates = [
                    new google.maps.LatLng(37.772323, -122.214897),
                    new google.maps.LatLng(21.291982, -157.821856),
                    new google.maps.LatLng(-18.142599, 178.431),
                    new google.maps.LatLng(-27.46758, 153.027892)
                ];

                flightPath = new google.maps.Polyline({
                    path: flightPlanCoordinates,
                    strokeColor: '#FF0000',
                    strokeOpacity: 1.0,
                    strokeWeight: 2
                });

                addLine();


        <?php  echo($extraCode); ?>
            }

            function loadScript() {
                var script = document.createElement("script");
                script.type = "text/javascript";
                script.src = "https://maps.google.com/maps/api/js?sensor=false&key=<?php echo($GLOBALS['config']['GOOGLE_MAPS']['API_KEY']);?>&callback=initialize";
                document.body.appendChild(script);
            }

            function addLine() {
                flightPath.setMap(map);
            }

            function removeLine() {
                flightPath.setMap(null);
            }


        </script>

        <?php 
    }

}

//end class
?>