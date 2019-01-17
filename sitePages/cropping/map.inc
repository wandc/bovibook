<?php

require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/croppingHelper.inc');

class Map extends BasePage {

    public function defaultDisplay() {
        $this->drawMap2();
    }

    public function drawMap2() {
        ?>
        

        


            <div class="row">
                <div class="col-sm-6"> 



                    <p>
                        <label for="slider-manure-amount">Manure previous -X weeks:</label>
                        <input type="text" id="slider-manure-amount" readonly style="border:0; color:#f6931f; font-weight:bold;">
                    </p>
                    <div id="slider-manure"></div>



                    <p>
                        <label for="slider-fertilizer-amount">Fertilizer previous -X weeks:</label>
                        <input type="text" id="slider-fertilizer-amount" readonly style="border:0; color:#f6931f; font-weight:bold;">
                    </p>
                    <div id="slider-fertilizer"></div>

                    <label><input type="checkbox" id="spfhLayer" checked="checked" onclick="check()" />SPFH Fields</label>&nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="balerLayer" onclick="check()" />Baler Fields</label>&nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="pastureLayer" onclick="check()" />Pasture Fields</label>&nbsp;&nbsp;&nbsp;&nbsp;
                    <label>&nbsp;|&nbsp; </label>
                    <label><input type="checkbox" id="spfhLimeLayer" onclick="check()" />SPFH Fields: Needs Lime</label>&nbsp;&nbsp;&nbsp;&nbsp;

                    <label><input type="checkbox" id="spfhAlfalfaLayer" onclick="check()" />SPFH: Alfalfa Currently</label>&nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="spfhCloverLayer" onclick="check()" />SPFH: Clover Currently</label>&nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="spfhOtherForageLayer" onclick="check()" />SPFH: Other Recent Forage</label>    &nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="spfhCornLayer" onclick="check()" />SPFH: Corn Currently</label>    &nbsp;&nbsp;&nbsp;&nbsp;

                    <label><input type="checkbox" id="spfhCornPlanLayer" onclick="check()" />SPFH: Corn Plan</label>    &nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="spfhAlfalfaPlanLayer" onclick="check()" />SPFH: Alfalfa Plan</label>    &nbsp;&nbsp;&nbsp;&nbsp;
                     <label><input type="checkbox" id="spfhOtherForagePlanLayer" onclick="check()" />SPFH: Other Forage Plan</label>    &nbsp;&nbsp;&nbsp;&nbsp;

                    <label><input type="checkbox" id="allFertCurrLayer" onclick="check()" />All: Fertilizer This Year </label>    &nbsp;&nbsp;&nbsp;&nbsp;
                    <label><input type="checkbox" id="allSprayCurrLayer" onclick="check()" />All: Spray This Year </label>    &nbsp;&nbsp;&nbsp;&nbsp;

                    </div>

                    </div><!-- /mapControl stuff -->

                      <div class="row">
                <div class="col-sm-12"> 
                    <div class="fieldMapBig" id="map"></div>
                     </div>

                    </div><!-- /end map stuff -->
                 
        <script>
            /*slider funtion */
  $( function() {
    $( "#slider-manure" ).slider({
      range: "min",
      value: 0,
      min: 0,
      max: 104,
      
            slide: function( event, ui ) {
                //show gui chnaginga mount as you slide
        $( "#slider-manure-amount" ).val( ui.value );
      }
    });
    $( "#slider-manure-amount" ).val( $( "#slider-manure" ).slider( "value" ) );
  } );
  
  $( function() {
    $( "#slider-fertilizer" ).slider({
      range: "min",
      value: 0,
      min: 0,
      max: 104,
      
            slide: function( event, ui ) {
                //show gui chnaginga mount as you slide
        $( "#slider-fertilizer-amount" ).val( ui.value );
      }
    });
    $( "#slider-fertilizer-amount" ).val( $( "#slider-fertilizer" ).slider( "value" ) );
  } );
  

  </script>
        
        <script>
            var map;

            /* have to be here for scope reasons */
            var spfhLayer;
            var balerLayer;
            var pastureLayer;
            var spfhLimeLayer;

            var spfhAlfalfaLayer;
            var spfhCornLayer;
            var spfhCloverLayer;
            var spfhOtherForageLayer;
            var spfhCornPlanLayer;
            var spfhAlfalfaPlanLayer;
            var spfhOtherForagePlanLayer;
            var allFertCurrLayer;
             var allSprayCurrLayer;
             var manureLayer;
             var fertilizerLayer;

            function initMap() {
                map = new google.maps.Map(document.getElementById('map'), {
                    center: {lat: 45.9788769346638, lng: -64.9880610086718},
                    zoom: 16,
                    mapTypeId: google.maps.MapTypeId.ROADMAP
                });


              

                initMapExtras(); /* setup estra stuff */
                check(); /*checkbox to torun layers on and off */
                sliderAction();

            }
        </script>
        <script src="https://maps.googleapis.com/maps/api/js?key=<?php echo($GLOBALS['config']['GOOGLE_MAPS']['API_KEY']);?>&callback=initMap"
        async defer></script>    

        <script type="text/javascript">
            /* extra stuff */
            function initMapExtras() {

                spfhLayer = new google.maps.Data();
                balerLayer = new google.maps.Data();
                pastureLayer = new google.maps.Data();
                spfhLimeLayer = new google.maps.Data();

                spfhAlfalfaLayer = new google.maps.Data();
                spfhCornLayer = new google.maps.Data();
                spfhCloverLayer = new google.maps.Data();
                spfhOtherForageLayer = new google.maps.Data();
                spfhCornPlanLayer = new google.maps.Data();
                spfhAlfalfaPlanLayer = new google.maps.Data();
                spfhOtherForagePlanLayer = new google.maps.Data();
                allFertCurrLayer = new  google.maps.Data();
                 allSprayCurrLayer = new  google.maps.Data();
                 manureLayer = new google.maps.Data();
                 fertilizerLayer = new google.maps.Data();
                 
                 
               
                 
                /*
                 map.data.addListener('click', function (event) {
                 
                 event.feature.setProperty('isColorful', true);
                 
                 infoWindow.setContent('<div>test</div>');
                 
                 var anchor = new google.maps.MVCObject();
                 anchor.setValues({ //position of the point
                 position: event.latLng,
                 anchorPoint: new google.maps.Point(0, -40)
                 });
                 
                 infoWindow.open(map, anchor);
                 
                 });
                 
                 // defined in the function passed to setStyle()
                 map.data.addListener('mouseover', function (event) {
                 map.data.revertStyle();
                 map.data.overrideStyle(event.feature, {
                 strokeWeight: 8
                 });
                 });
                 
                 map.data.addListener('mouseout', function (event) {
                 map.data.revertStyle();
                 });
                 
                 
                 */
                /*circles for distance from farm */
                var circle1km = new google.maps.Circle({
                    strokeColor: '#FF0000',
                    strokeOpacity: 0.4,
                    strokeWeight: 1.33,
                    fillOpacity: 0,
                    map: map,
                    center: {lat: 45.980775, lng: -64.996611},
                    radius: 1000
                });

                var circle3km = new google.maps.Circle({
                    strokeColor: '#FF0000',
                    strokeOpacity: 0.2,
                    strokeWeight: 1,
                    fillOpacity: 0,
                    map: map,
                    center: {lat: 45.980775, lng: -64.996611},
                    radius: 3000
                });






//one function do to customization of layer 
function doLayerStuff(layerObj,geoJsonUrl,colour,opacity,borderWeight=1) {
    
   layerObj.loadGeoJson(geoJsonUrl);  //load geo json url for object, before we use it.
    
     layerObj.setStyle({
                    strokeColor: 'grey',
                    strokeWeight: borderWeight,
                    fillColor: colour,
                    fillOpacity: opacity
                });
    
}

doLayerStuff(spfhLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh','#FF0000',.05);
doLayerStuff(balerLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/baler','black',.05);
doLayerStuff(pastureLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/pasture','grean',.05);
doLayerStuff(spfhLimeLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_NeedLime','blue',.5);

doLayerStuff(spfhAlfalfaLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_Alfalfa','#68f442',.9);
doLayerStuff(spfhCornLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_Corn','#FFFF00',.9);
doLayerStuff(spfhCloverLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_Clover','#f44197',.9);
doLayerStuff(spfhOtherForageLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_OtherForage','#7f41f4',.9);
//
doLayerStuff(spfhCornPlanLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_Corn_Plan','#FFFF00',.9,3);
doLayerStuff(spfhAlfalfaPlanLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_Alfalfa_Plan','#68f442',.9,3);
doLayerStuff(spfhOtherForagePlanLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/spfh_OtherForage_Plan','#7f41f4',.9,3);
//
doLayerStuff(allFertCurrLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/all_Fert_Curr','#42cef4',.9,3);
doLayerStuff(allSprayCurrLayer,'https://int.littleriver.ca/api/json/14/SpecificCroppingGISData/all_Spray_Curr','#f4b642',.9,3);
//

                //turn on spfh base layer as default...maybe
                spfhLayer.setMap(map);


            }
            ;

            /* watched checkbox divs and when clicked, turns on and off map layers */
            function check()
            {
              
              // turn layer on and off.          
                function checkIf(layerObj,layerName) {
                 if (document.getElementById(layerName).checked)
                {
                    layerObj.setMap(map);
                } 
                else
                {
                    layerObj.setMap(null);
                }   
                }
                
               checkIf(spfhLayer,"spfhLayer");
               checkIf(balerLayer,"balerLayer");
               checkIf(pastureLayer,"pastureLayer");
               checkIf(spfhLimeLayer,"spfhLimeLayer");

               checkIf(spfhAlfalfaLayer,"spfhAlfalfaLayer");
               checkIf(spfhCornLayer,"spfhCornLayer");
               checkIf(spfhCloverLayer,"spfhCloverLayer");
               checkIf(spfhOtherForageLayer,"spfhOtherForageLayer");
               // 
 checkIf(spfhCornPlanLayer,"spfhCornPlanLayer");
 checkIf(spfhAlfalfaPlanLayer,"spfhAlfalfaPlanLayer");
  checkIf(spfhOtherForagePlanLayer,"spfhOtherForagePlanLayer");
 
//
 checkIf(allFertCurrLayer,"allFertCurrLayer");
  checkIf(allSprayCurrLayer,"allSprayCurrLayer");

            };
            
                          /*
            * 
            * @type String
            * this is used to pass along what fields we want to filter, we will need something to decode it on the other end.
            */
            function createFieldFilter() {
    
                var filter;
                //
                var spfhFilter;  
                 var balerFilter; 
                    var pastureFilter;   
              
               filter='';
              
                if (document.getElementById('spfhLayer').checked) {
                   spfhFilter='1';  
                }
                 else {
                     spfhFilter='0'; 
                }
                //
                             
                if (document.getElementById('balerLayer').checked) {
                   balerFilter='1';  
                }
                 else {
                     balerFilter='0'; 
                }
                //
                         
                if (document.getElementById('pastureLayer').checked) {
                   pastureFilter='1';  
                }
                 else {
                     pastureFilter='0'; 
                }
                filter=spfhFilter+balerFilter+pastureFilter;  
                return filter;
            };
            
            
            
            
            
            
              /*run whenever the specified slider changes. */
                          function sliderAction() {
                
           
         
                
                
                
                $( "#slider-manure" ).slider({                                           
                       stop: function( event, ui ) {
                           //when you stop sliding, show on map                    
                          if (ui.value == null)  
                                  {
                                  manureLayer.setMap(null); 
                                  }
                                  else {
                                      var obj = {type:"manure", filter:createFieldFilter(), time:ui.value};                  
                                                 manureLayer.forEach(function(feature) {           manureLayer.remove(feature);   }); //remove all data from data layer, before we add more data.
                                                 manureLayer.loadGeoJson('https://int.littleriver.ca/api/json/14/slider/'+encodeURIComponent(JSON.stringify(obj)));  //load geo json url for object, before we use it.
                                                 manureLayer.setMap(map);                          
                                        }                  
                      }
                 });
                    
                    /* **** */
                     $( "#slider-fertilizer" ).slider({                                           
                       stop: function( event, ui ) {
                  
                           //when you stop sliding, show on map                    
                          if (ui.value == null)  
                                  {
                                  fertilizerLayer.setMap(null); 
                                  }
                                  else {
                                      var obj = {type:"fertilizer", filter:createFieldFilter(), time:ui.value};                                     
                                                 fertilizerLayer.forEach(function(feature) {           fertilizerLayer.remove(feature);   }); //remove all data from data layer, before we add more data.
                                                 fertilizerLayer.loadGeoJson('https://int.littleriver.ca/api/json/14/slider/'+encodeURIComponent(JSON.stringify(obj)));  //load geo json url for object, before we use it.
                                                 fertilizerLayer.setMap(map);                          
                                        }                  
                      }
                 });
                    
                    
                    
                    
                    
                                        
                            }
                         

        </script>
        <?php
    }

    public function SpecificCroppingGISData($request) {
        if (!empty($request['item'])) {
            if (is_string($request['item'])) {
                $item = filter_var($request['item'], FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_HIGH);
            }
        } else {
            header("HTTP/1.1 400 Error");
            throw new Exception('Error: no resource specified');
        }

//forage fields with spfh          
        $sqlSpfh_All = <<<EOD
SELECT *,gis.ST_AsGeoJSON (border_geom) as border_geojson, gis.ST_AsGeoJSON(gis.ST_Centroid(border_geom)) as point_geojson, 'purple' as fillcolor
FROM cropping.foragecurr 
WHERE spfh_suitable = true
EOD;

//forage fields, but not spfh fields, thus BALER fields.
        $sqlBaler_All = <<<EOD
  SELECT *,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor
  FROM cropping.foragecurr WHERE spfh_suitable = false
EOD;
        
        //forage fields, but not spfh fields, thus BALER fields.
        $sqlPasture_All = <<<EOD
  SELECT *,ST_AsGeoJSON (border_geom) as border_geojson, '89f441' as fillcolor
  FROM cropping.pasturecurr
EOD;


//SPFH and pH below 6 and there hasn't been lime put on since soil test.
        $sqlSpfh_NeedLime = <<<EOD
        with temp as (
SELECT soil_sample_event.pH as ph,soil_sample_event.event_time as soil_sample_event_time,(SELECT event_time FROM cropping.lime_event WHERE id=cropping.lime_event_id_at_timestamp (foragecurr.id, now()::timestamp)) as lime_event_time,foragecurr.*,ST_AsGeoJSON (foragecurr.border_geom) as border_geojson, '68f442' as fillcolor
FROM cropping.foragecurr 
LEFT JOIN cropping.soil_sample_event ON soil_sample_event.id=cropping.soil_sample_event_id_at_timestamp (foragecurr.id, now()::timestamp)
)
    SELECT * from temp WHERE spfh_suitable = true AND ph<6 AND soil_sample_event_time > lime_event_time
EOD;




//Alfalfa planted in last three years.
        $sqlSpfh_Alfalfa = <<<EOD
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='Alfalfa' AND seed_event.event_time  >= (current_date-interval '3 year') AND spfh_suitable=true
EOD;

//Corn planted in last 14 months.
        $sqlSpfh_Corn = <<<EOD
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor 
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='Corn' AND seed_event.event_time  >= (current_date-interval '9 months') AND spfh_suitable=true
UNION
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson, 'e6e54b' as fillcolor 
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='BMR Corn' AND seed_event.event_time  >= (current_date-interval '9 months') AND spfh_suitable=true
EOD;

//Clover planted in last 2 years.
        $sqlSpfh_Clover = <<<EOD
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson , '68f442' as fillcolor
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='Clover' AND seed_event.event_time  >= (current_date-interval '2 years') AND spfh_suitable=true
EOD;

//other forage planted in last 2 years.
        $sqlSpfh_OtherForage = <<<EOD
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson , '68f442' as fillcolor
FROM cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE  specific_type NOT IN ('Clover', 'BMR Corn','Corn', 'Alfalfa') AND seed_event.event_time  >= (current_date-interval '2 years') AND spfh_suitable=true
EOD;

/* PLANNING FOR COMING YEAR */
        
//corn for coming year.
        $sqlSpfh_Corn_Planning = <<<EOD
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >='2019-01-01' AND seed_event_scheduled.event_time < '2019-12-31'  
AND specific_type='Corn' AND spfh_suitable=true
UNION
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, 'e6e54b' as fillcolor
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >='2019-01-01' AND seed_event_scheduled.event_time < '2019-12-31'  
AND specific_type='BMR Corn' AND spfh_suitable=true
EOD;
        
         $sqlSpfh_Alfalfa_Planning = <<<EOD
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >='2019-01-01' AND seed_event_scheduled.event_time < '2019-12-31'  
AND specific_type='Alfalfa' AND spfh_suitable=true
EOD;
         
$sqlSpfh_OtherForage_Planning     = <<<EOD
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >='2019-01-01' AND seed_event_scheduled.event_time < '2019-12-31'  AND
   specific_type NOT IN ( 'BMR Corn','Corn', 'Alfalfa') AND spfh_suitable=true
EOD;
         
/* Fertilizer put on this year for spfh fields */
         $sqlAll_Fertilizer_currentYear = <<<EOD
SELECT  distinct(fertilizer_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, '42cef4' as fillcolor
FROM cropping.fertilizer_event 
JOIN cropping.fieldcurr on fieldcurr.id=fertilizer_event.field_id
WHERE all_covered=true and date_part('year', event_time) = date_part('year', CURRENT_DATE) 
EOD;
   
/* Spray put on this year for spfh fields */
         $sqlAll_Spray_currentYear = <<<EOD
SELECT  distinct(spray_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, 'f4b642' as fillcolor
FROM cropping.spray_event 
JOIN cropping.fieldcurr on fieldcurr.id=spray_event.field_id
WHERE  date_part('year', event_time) = date_part('year', CURRENT_DATE) 
EOD;
         
        

        /* decide which sql code to run and thus what data to return */
        switch ($item) {
            case 'spfh':
                $sqlToRun = $sqlSpfh_All;
                break;
            case 'baler':
                $sqlToRun = $sqlBaler_All;
                break;
             case 'pasture':
                $sqlToRun = $sqlPasture_All;
                break;
            case 'spfh_NeedLime':
                $sqlToRun = $sqlSpfh_NeedLime;
                break;
            case 'spfh_Alfalfa':
                $sqlToRun = $sqlSpfh_Alfalfa;
                break;
            case 'spfh_Corn':
                $sqlToRun = $sqlSpfh_Corn;
                break;
            case 'spfh_Clover':
                $sqlToRun = $sqlSpfh_Clover;
                break;
            case 'spfh_OtherForage':
                $sqlToRun = $sqlSpfh_OtherForage;
                break;
            case 'spfh_Corn_Plan':
                $sqlToRun = $sqlSpfh_Corn_Planning;
                break;
             case 'spfh_Alfalfa_Plan':
                $sqlToRun = $sqlSpfh_Alfalfa_Planning;
                break;         
             case 'spfh_OtherForage_Plan':
                $sqlToRun = $sqlSpfh_OtherForage_Planning;
                break;                
            case 'all_Fert_Curr':
                $sqlToRun = $sqlAll_Fertilizer_currentYear;
                break;     
             case 'all_Spray_Curr':
                $sqlToRun = $sqlAll_Spray_currentYear;
                break;   
                 
            default:
                header("HTTP/1.1 400 Error");
                throw new Exception('Error: no resource specified');
        }

        $statement = $GLOBALS['pdo']->prepare($sqlToRun);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);


        // Build GeoJSON feature collection array
        $geojson = array(
            'type' => 'FeatureCollection',
            'features' => array()
        );


# Loop through rows to build feature arrays
        foreach ($results as $key => $value) {

            /* add polygon of fields */
            $feature = array(
                'type' => 'Feature',
                'properties' => $value,
                'id' => $value['id'] . '_1',
                'geometry' => json_decode($value['border_geojson']),
                'fillcolor' => $value['fillcolor']
            );
            // Add feature arrays to feature collection array
            array_push($geojson['features'], $feature);

            /*
              if (!empty($value['point_geojson'])) {
              // add a placemark
              $feature2 = array(
              'id' => $value['id'].'_2',
              'type' => 'Feature',
              'geometry' => json_decode($value['point_geojson']),
              'properties' => $value
              );

              // Add feature arrays to feature collection array
              array_push($geojson['features'], $feature2);
              } */
        }


        return ($geojson);
    }

    
    
    //changes '100' code back into usefule info
    private function decodeFieldlFilter($fieldFilter) {
      //filter fields back into code, uses 1 for true and 0 for false
        $str='';
        
        if (substr($fieldFilter, 0,1) ==true) {
             $sql0 = <<<EOD
SELECT id from cropping.foragecurr WHERE spfh_suitable is true
EOD;
  $statement0 = $GLOBALS['pdo']->prepare($sql0);
        $statement0->execute();
        $results0 = $statement0->fetchAll(PDO::FETCH_COLUMN, 0);
        $str=$str.= implode (", ", $results0);
        }
        
        if (substr($fieldFilter, 1,1) ==true) {
             $sql1 = <<<EOD
SELECT id from cropping.foragecurr WHERE spfh_suitable is false
EOD;
  $statement1 = $GLOBALS['pdo']->prepare($sql1);
        $statement1->execute();
        $results1 = $statement1->fetchAll(PDO::FETCH_COLUMN, 0);
        if ($str!=null) {$str=$str.',';}
        $str=$str.= implode (", ", $results1);
        }
        
        if (substr($fieldFilter, 2,1) ==true) {
             $sql2 = <<<EOD
SELECT id from cropping.pasturecurr
EOD;
  $statement2 = $GLOBALS['pdo']->prepare($sql2);
        $statement2->execute();
        $results2 = $statement2->fetchAll(PDO::FETCH_COLUMN, 0);
         if ($str!=null) {$str=$str.',';}
        $str=$str.= implode (", ", $results2);
        }
        
        //sql needs this to be null when no entries or does not work.
         if ($fieldFilter =='000') {
             $str='null';  
         }
         return $str;
    }
    
    
    //handles when slider calls for geojson (slider stopped)
    public function slider($request) {
   
        if (!empty($request['item'])) {
            if (is_string($request['item'])) {
                //devoce passed json string and sanitize so we can use.
                $item = filter_var($request['item'], FILTER_SANITIZE_URL);
                $decoded = json_decode($item, true);
                $type = filter_var($decoded['type'], FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_HIGH);
                $time = filter_var($decoded['time'], FILTER_SANITIZE_NUMBER_INT, FILTER_FLAG_STRIP_HIGH);
                $fieldFilter = filter_var($decoded['filter'], FILTER_SANITIZE_NUMBER_INT, FILTER_FLAG_STRIP_HIGH);
            }
        } else {
            header("HTTP/1.1 400 Error");
            throw new Exception('Error: no resource specified');
        }

   
        switch ($type) {
            case 'manure':
//manure vairable length of time in past
                $sql = <<<EOD
SELECT  distinct(manure_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor
FROM cropping.manure_event 
JOIN cropping.foragecurr on foragecurr.id=manure_event.field_id
WHERE all_covered=true and event_time > now() - interval '$time week' AND field_id IN ({$this->decodeFieldlFilter($fieldFilter)})
EOD;
                break;
            case 'fertilizer':
//manure vairable length of time in past
                $sql = <<<EOD
SELECT  distinct(fertilizer_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, 'e5b285' as fillcolor
FROM cropping.fertilizer_event 
JOIN cropping.fieldcurr on fieldcurr.id=fertilizer_event.field_id
WHERE all_covered=true and event_time > now() - interval '$time week' AND field_id IN ({$this->decodeFieldlFilter($fieldFilter)})
EOD;
                break;
            default:
                header("HTTP/1.1 400 Error");
                throw new Exception('Error: no type specified, ie manure');
        }

    
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);


        // Build GeoJSON feature collection array
        $geojson = array(
            'type' => 'FeatureCollection',
            'features' => array()
        );


# Loop through rows to build feature arrays
        foreach ($results as $key => $value) {

            /* add polygon of fields */
            $feature = array(
                'type' => 'Feature',
                'properties' => $value,
                'id' => $value['id'] . '_1',
                'geometry' => json_decode($value['border_geojson']),
                'fillcolor' => $value['fillcolor']
            );
            // Add feature arrays to feature collection array
            array_push($geojson['features'], $feature);
        }

        return ($geojson);
    }


}
//end class
?>