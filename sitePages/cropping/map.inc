<?php


class Map extends BasePage {

    
    private $basicOjectsArr;
    
    
    //need an array of objects 
    //each object has name, text name, sql. 
    function basicElementsConstructor() {
        
        $outArr=array();
        ///////////////////////////
        
        
         /* double crop corn plan.  june 1 or later estiamted plating date*/
        $obj=new stdClass();
        $obj->name='spfhCornPlanDoubleCropLayer';
        $obj->desc='SPFH: Corn Plan Double Crop';
        $obj->sql=<<<SQL
with temp as (
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >=  (extract(year from current_date) || '-06-01')::date AND seed_event_scheduled.event_time <  (extract(year from current_date) || '-12-31')::date  
AND specific_type='Corn' AND spfh_suitable=true
UNION
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, 'e6e54b' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >= (extract(year from current_date) || '-06-01')::date AND seed_event_scheduled.event_time <  (extract(year from current_date) || '-12-31')::date  
AND specific_type='BMR Corn' AND spfh_suitable=true
) 
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#aaa500';
        $obj->opacity=.5;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        
        /**/
        $obj=new stdClass();
        $obj->name='spfhLayer';
        $obj->desc='SPFH Fields';
        $obj->sql=<<<SQL
with temp as (
SELECT ST_Area(border_geom,true)/10000 as area_calc,*,gis.ST_AsGeoJSON (border_geom) as border_geojson, gis.ST_AsGeoJSON(gis.ST_Centroid(border_geom)) as point_geojson, 'purple' as fillcolor
FROM cropping.foragecurr 
WHERE spfh_suitable = true
)
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#FF0000';
        $obj->opacity=.05;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        /**/ //forage fields, but not spfh fields, thus BALER fields.
        $obj=new stdClass();
        $obj->name='balerLayer';
        $obj->desc='Baler Fields';
        $obj->sql=<<<SQL
                with temp as (
  SELECT ST_Area(border_geom,true)/10000 as area_calc,*,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor
  FROM cropping.foragecurr WHERE spfh_suitable = false
                )
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='black';
        $obj->opacity=.05;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////

        /**/    //forage fields, but not spfh fields, thus BALER fields.
        $obj=new stdClass();
        $obj->name='pastureLayer';
        $obj->desc='Pasture Fields';
        $obj->sql=<<<SQL
                  with temp as (
  SELECT ST_Area(border_geom,true)/10000 as area_calc,*,ST_AsGeoJSON (border_geom) as border_geojson, '89f441' as fillcolor
  FROM cropping.pasturecurr
                   )
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='green';
        $obj->opacity=.05;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        /**/ //SPFH and pH below 6 and there hasn't been lime put on since soil test.
        $obj=new stdClass();
        $obj->name='spfhLimeLayer';
        $obj->desc='SPFH Fields: Needs Lime';
        $obj->sql=<<<SQL
        with temp as (
SELECT soil_sample_event.pH as ph,soil_sample_event.event_time as soil_sample_event_time,(SELECT event_time FROM cropping.lime_event WHERE id=cropping.lime_event_id_at_timestamp (foragecurr.id, now()::timestamp)) as lime_event_time,foragecurr.*,ST_AsGeoJSON (foragecurr.border_geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
FROM cropping.foragecurr 
LEFT JOIN cropping.soil_sample_event ON soil_sample_event.id=cropping.soil_sample_event_id_at_timestamp (foragecurr.id, now()::timestamp)
), temp2 as (
    SELECT *  from temp WHERE spfh_suitable = true AND ph<6 AND soil_sample_event_time > lime_event_time
                )
                SELECT *,(SELECT sum(area_calc) as total_area from temp2) FROM temp2   
SQL;
        $obj->colour='blue';
        $obj->opacity=.5;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        
         /**/ //SPFH and pH below 6.5 and there hasn't been lime put on since soil test.
        $obj=new stdClass();
        $obj->name='spfhLimeLayerAlfalfa';
        $obj->desc='SPFH Fields: Needs Lime (for Alfafla) <6.5';
        $obj->sql=<<<SQL
        with temp as (
SELECT soil_sample_event.pH as ph,soil_sample_event.event_time as soil_sample_event_time,(SELECT event_time FROM cropping.lime_event WHERE id=cropping.lime_event_id_at_timestamp (foragecurr.id, now()::timestamp)) as lime_event_time,foragecurr.*,ST_AsGeoJSON (foragecurr.border_geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
FROM cropping.foragecurr 
LEFT JOIN cropping.soil_sample_event ON soil_sample_event.id=cropping.soil_sample_event_id_at_timestamp (foragecurr.id, now()::timestamp)
), temp2 as (
    SELECT *  from temp WHERE spfh_suitable = true AND ph<6.5 AND soil_sample_event_time > lime_event_time
                )
                SELECT *,(SELECT sum(area_calc) as total_area from temp2) FROM temp2   
SQL;
        $obj->colour='blue';
        $obj->opacity=.5;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        
         /**/ //Alfalfa planted in last three years.
        $obj=new stdClass();
        $obj->name='spfhAlfalfaLayer';
        $obj->desc='SPFH: Alfalfa Currently';
        $obj->sql=<<<SQL
                  with temp as (
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='Alfalfa' AND seed_event.event_time  >= (current_date-interval '3 year') AND spfh_suitable=true
                   )
                SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#68f442';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ //Clover planted in last 2 years.
        $obj=new stdClass();
        $obj->name='spfhCloverLayer';
        $obj->desc='SPFH: Clover Currently';
        $obj->sql=<<<SQL
                  with temp as (
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson , '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='Clover' AND seed_event.event_time  >= (current_date-interval '2 years') AND spfh_suitable=true
                   )
                SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#f44197';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ //Clover (or other seed) frost seeded this season
        $obj=new stdClass();
        $obj->name='allFrostSeedingLayer';
        $obj->desc='All: Frost Seeding this year';
        $obj->sql=<<<SQL
                  with temp as (
SELECT fieldcurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson , '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
from cropping.fieldcurr
LEFT JOIN cropping.seed_event ON seed_event.id=fieldcurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE seed_event.event_time  >=  (extract(year from current_date) || '-01-01')::date AND  seed_event.event_time  <=  (extract(year from current_date) || '-04-10')::date      
                   )
                SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#d44197';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
              /**/ 
        $obj=new stdClass();
        $obj->name='spfhOtherForageLayer';
        $obj->desc='SPFH: Other Recent Forage';
        $obj->sql=<<<SQL
      with temp as (
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson , '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
FROM cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE  specific_type NOT IN ('Clover', 'BMR Corn','Corn', 'Alfalfa') AND seed_event.event_time  >= (current_date-interval '2 years') AND spfh_suitable=true
                       )
                SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#7f41f4';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ //Corn planted in last 14 months.
        $obj=new stdClass();
        $obj->name='spfhCornLayer';
        $obj->desc='SPFH: Corn Currently';
        $obj->sql=<<<SQL
                with temp as (
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor ,ST_Area(border_geom,true)/10000 as area_calc
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='Corn' AND seed_event.event_time  >= (current_date-interval '9 months') AND spfh_suitable=true
UNION
SELECT foragecurr.*,specific_type,seed_event.event_time as seed_event_time,ST_AsGeoJSON (border_geom) as border_geojson, 'e6e54b' as fillcolor ,ST_Area(border_geom,true)/10000 as area_calc
from cropping.foragecurr
LEFT JOIN cropping.seed_event ON seed_event.id=foragecurr.seed_event_id
LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id
LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
WHERE specific_type='BMR Corn' AND seed_event.event_time  >= (current_date-interval '9 months') AND spfh_suitable=true
                )
                SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#FFFF00';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ //corn for coming year.
        $obj=new stdClass();
        $obj->name='spfhCornPlanLayer';
        $obj->desc='SPFH: Corn Plan';
        
        //what years data to show for crop planning? If it's past Sept 1? Show next year?
        if (date('n') >= 9) {
            $obj->year = date('Y', strtotime('+1 year'));
        } else {
            $obj->year = date("Y");
        }
      
        $obj->sql=<<<SQL
                with temp as (
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >=  ({$obj->year} || '-01-01')::date  AND seed_event_scheduled.event_time <=  ({$obj->year} || '-05-31')::date
AND specific_type='Corn' AND spfh_suitable=true
UNION
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, 'e6e54b' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >= ({$obj->year} || '-01-01')::date  AND seed_event_scheduled.event_time <= ({$obj->year} || '-05-31')::date 
AND specific_type='BMR Corn' AND spfh_suitable=true
                ) 
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#FFFF00';
        $obj->opacity=.9;
        $obj->borderWeight=3;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ 
        $obj=new stdClass();
        $obj->name='spfhAlfalfaPlanLayer';
        $obj->desc='SPFH: Alfalfa Plan';
        
         //what years data to show for crop planning? If it's past Sept 1? Show next year?
        if (date('n') >= 9) {
            $obj->year = date('Y', strtotime('+1 year'));
        } else {
            $obj->year = date("Y");
        }
        
        $obj->sql=<<<SQL
                   with temp as (
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >= ({$obj->year} || '-01-01')::date  AND seed_event_scheduled.event_time <  ({$obj->year} || '-12-31')::date 
AND specific_type='Alfalfa' AND spfh_suitable=true
                                ) 
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#68f442';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ //other forage planted in last 2 years.
        $obj=new stdClass();
        $obj->name='spfhOtherForagePlanLayer';
        $obj->desc='SPFH: Other Forage Plan';
        $obj->sql=<<<SQL
                with temp as (
SELECT field.id as id,field.id as field_id,datum.id as datum_id,ST_AsGeoJSON (geom) as border_geojson, '68f442' as fillcolor,ST_Area(border_geom,true)/10000 as area_calc
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
 LEFT JOIN cropping.foragecurr ON foragecurr.id =field.id
        WHERE  seed_event_scheduled.event_time >='2020-01-01' AND seed_event_scheduled.event_time < '2020-12-31'  AND
   specific_type NOT IN ( 'BMR Corn','Corn', 'Alfalfa') AND spfh_suitable=true
                          ) 
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp 
SQL;
        $obj->colour='#7f41f4';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ /* Fertilizer put on this year for spfh fields */
        $obj=new stdClass();
        $obj->name='allFertCurrLayer';
        $obj->desc='All: Fertilizer This Year';
        $obj->sql=<<<SQL
SELECT  distinct(fertilizer_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, '42cef4' as fillcolor
FROM cropping.fertilizer_event 
JOIN cropping.fieldcurr on fieldcurr.id=fertilizer_event.field_id
WHERE all_covered=true and date_part('year', fertilizer_event.event_time) = date_part('year', CURRENT_DATE) 
SQL;
        $obj->colour='#42cef4';
        $obj->opacity=.9;
        $obj->borderWeight=3;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
         /**/ /* Spray put on this year for spfh fields */
        $obj=new stdClass();
        $obj->name='allSprayCurrLayer';
        $obj->desc='All: Spray This Year';
        $obj->sql=<<<SQL
SELECT  distinct(spray_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, 'f4b642' as fillcolor
FROM cropping.spray_event 
JOIN cropping.fieldcurr on fieldcurr.id=spray_event.field_id
WHERE  date_part('year', spray_event.event_time) = date_part('year', CURRENT_DATE) 
SQL;
        $obj->colour='#f4b642';
        $obj->opacity=.9;
        $obj->borderWeight=3;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        
        
         /**/ 
        $obj=new stdClass();
        $obj->name='potassium';
        $obj->desc='SPFH: Potassium Deficient (<99 ppm)';
        $obj->sql=<<<SQL
        with temp as (
SELECT soil_sample_event.*,soil_sample_event.event_time as soil_sample_event_time,(SELECT event_time FROM cropping.lime_event WHERE id=cropping.lime_event_id_at_timestamp (foragecurr.id, now()::timestamp)) as lime_event_time,foragecurr.*,ST_AsGeoJSON (foragecurr.border_geom) as border_geojson, '68f442' as fillcolor
FROM cropping.foragecurr 
LEFT JOIN cropping.soil_sample_event ON soil_sample_event.id=cropping.soil_sample_event_id_at_timestamp (foragecurr.id, now()::timestamp)
)
    SELECT * from temp WHERE potassium <=99 AND spfh_suitable=true
SQL;
        $obj->colour='purple';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        $obj=new stdClass();
        $obj->name='boron';
        $obj->desc='SPFH: Boron Deficient (<1 ppm)'; //https://www.plant.uoguelph.ca/sites/plant.uoguelph.ca/files/forages/documents/A6-Boron%20fertilization%20of%20alfalfa-July1986.pdf
        $obj->sql=<<<SQL
        with temp as (
SELECT soil_sample_event.*,soil_sample_event.event_time as soil_sample_event_time,(SELECT event_time FROM cropping.lime_event WHERE id=cropping.lime_event_id_at_timestamp (foragecurr.id, now()::timestamp)) as lime_event_time,foragecurr.*,ST_AsGeoJSON (foragecurr.border_geom) as border_geojson, '68f442' as fillcolor
FROM cropping.foragecurr 
LEFT JOIN cropping.soil_sample_event ON soil_sample_event.id=cropping.soil_sample_event_id_at_timestamp (foragecurr.id, now()::timestamp)
)
    SELECT * from temp WHERE boron <=1 AND spfh_suitable=true
SQL;
        $obj->colour='pink';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        
         $obj=new stdClass();
        $obj->name='phosphorus';
        $obj->desc='SPFH: Phosphorus Excessive';
        $obj->sql=<<<SQL
        with temp as (
SELECT soil_sample_event.*,soil_sample_event.event_time as soil_sample_event_time,(SELECT event_time FROM cropping.lime_event WHERE id=cropping.lime_event_id_at_timestamp (foragecurr.id, now()::timestamp)) as lime_event_time,foragecurr.*,ST_AsGeoJSON (foragecurr.border_geom) as border_geojson, '68f442' as fillcolor
FROM cropping.foragecurr 
LEFT JOIN cropping.soil_sample_event ON soil_sample_event.id=cropping.soil_sample_event_id_at_timestamp (foragecurr.id, now()::timestamp)
)
    SELECT * from temp WHERE phosphorus >=157 AND spfh_suitable=true
SQL;
        $obj->colour='purple';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        $obj=new stdClass();
        $obj->name='drainage';
        $obj->desc='SPFH: Drainage(Systematically only)';
        $obj->sql=<<<SQL
        with temp as (
SELECT ST_Area(border_geom,true)/10000 as area_calc,*,gis.ST_AsGeoJSON (border_geom) as border_geojson, gis.ST_AsGeoJSON(gis.ST_Centroid(border_geom)) as point_geojson, 'purple' as fillcolor
FROM cropping.foragecurr 
WHERE spfh_suitable = true AND drainage_score =1
)
SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp
SQL;
        $obj->colour='pink';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        
        
        
         //////////////////////////
        
        $obj=new stdClass();
        $obj->name='nitrogen';
        $obj->desc='All: Nitorgen this year >= 40 units';
        $obj->sql=<<<SQL



/* find total units of nitrogen this year so far on each field */
with temp as (

SELECT field_id as id,gis.ST_Area(border_geom,true)/10000 as area_calc,gis.ST_AsGeoJSON (border_geom) as border_geojson, gis.ST_AsGeoJSON(gis.ST_Centroid(border_geom)) as point_geojson, 'purple' as fillcolor
,(SELECT sum((amount/100)*n) as units_of_n_this_year FROM cropping.fertilizer_event 
LEFT JOIN cropping.fertilizer ON fertilizer.id=fertilizer_event.fertilizer_id
WHERE fertilizer_event.field_id= fieldcurr.id and event_time >= DATE_TRUNC('year', current_date)) as units_of_n_this_year
FROM cropping.fieldcurr 

), temp2 as (

/* get rid of null fields, with no nitrogen this year */
SELECT * FROM temp WHERE units_of_n_this_year is not null AND units_of_n_this_year >=40
)

/* so total is correct */
SELECT *,(SELECT sum(area_calc) as total_area from temp2) FROM temp2 
SQL;
        $obj->colour='red';
        $obj->opacity=.6;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
                     
         $obj=new stdClass();
        $obj->name='tempFieldList1';
        $obj->desc='Temp Field List';
        $obj->sql=<<<SQL
                with temp as (
SELECT  ST_Area(border_geom,true)/10000 as area_calc,field_list_temp.field_id, *,ST_AsGeoJSON (border_geom) as border_geojson, '42cef4' as fillcolor
FROM system.field_list_temp 
JOIN cropping.fieldcurr on fieldcurr.id=field_list_temp.field_id)
                SELECT *,(SELECT sum(area_calc) as total_area from temp) FROM temp
SQL;
        $obj->colour='brown';
        $obj->opacity=.9;
        $obj->borderWeight=1;
        $outArr[$obj->name]=$obj;
        unset($obj);
        //////////////////////////
        
        
        
        //set class object
        $this->basicOjectsArr=$outArr;
    }
    
    
    //dynamic js of $basicOjectsArr
    function inputOnOff($objArr) {
        $out=array();
        $count=0;
        foreach ($objArr as $obj) {
                $out[]= <<<JS
 <label><input type="checkbox" id="$obj->name" onclick="check()" />$obj->desc</label>    &nbsp;&nbsp;&nbsp;&nbsp;
JS;
                if ($count==2) {  $out[]=' <label>&nbsp;|&nbsp; &nbsp;</label>';} //draw spacer, depends on order of config.
          $count++;     
        }

        return implode($out);
    }
    
   //dynamic js of $basicOjectsArr
     function jsVarOnOff($objArr) {
        $out=array();
        foreach ($objArr as $obj) {
                $out[]= <<<JS
         var $obj->name; \n\r
JS;
                
        }
        return implode($out);
    }
    
   //dynamic js of $basicOjectsArr
    function jsMapExtraOnOff($objArr) {
        $out=array();
        foreach ($objArr as $obj) {
                $out[]= <<<JS
        $obj->name = new google.maps.Data(); \n\r
JS;
                
        }
        return implode($out);
    }
    
    //dynamic js of $basicOjectsArr
       function jsDoLayerStuffOnOff($objArr) {
        $out=array();
        foreach ($objArr as $obj) {
                $out[]= <<<JS
       doLayerStuff($obj->name,'{$GLOBALS['config']['HTTP']['JSON_API']}/14/SpecificCroppingGISData/$obj->name','$obj->colour', $obj->opacity,$obj->borderWeight);\n\r
                        
JS;
                
        }
        return implode($out);
    }
    
  
    
    function jsCheckIfOnOff($objArr) {
        $out=array();
        foreach ($objArr as $obj) {
                $out[]= <<<JS
         checkIf($obj->name,"$obj->name");\n\r
JS;
                
        }
        return implode($out);
    }
    
    
    
    public function defaultDisplay() {
        //setup
     
        $this->basicElementsConstructor(); 
        
      
        
        print($this->drawMap2());
    }

    public function drawMap2() {
        
        $out[]=$this->mapHTML();
        $out[]=$this->mapJavaScript();
        return implode($out);
    }
    
    
    public function mapHTML() {
           
        
        $sliderManure=<<<HTML
                
                 
                        <label for="slider-manure-amount">Manure previous -X weeks:
                            <input type="text" id="slider-manure-amount" readonly style="border:0; color:#f6931f; font-weight:bold;">                        
                        </label>
                       
                    
                    <div id="slider-manure"></div>
                
        HTML;        
        
         $sliderFertilizer=<<<HTML
                
                 
                  
                        <label for="slider-fertilizer-amount">Fertilizer previous -X weeks:
                             <input type="text" id="slider-fertilizer-amount" readonly style="border:0; color:#f6931f; font-weight:bold;">
                        </label>
                       
                  
                    <div id="slider-fertilizer"></div>

                
        HTML;    
        
         $sliderSprayed=<<<HTML
                
                  
                        <label for="slider-sprayed-amount">Sprayed previous -X weeks:
                            <input type="text" id="slider-sprayed-amount" readonly style="border:0; color:#f6931f; font-weight:bold;">    
                        </label>
                        
                  
                    <div id="slider-sprayed"></div>
                
        HTML;    

      $sliderManureBox=(new BootStrap())->plainCard('Manure', $sliderManure);
      $sliderFertilizerBox=(new BootStrap())->plainCard('Fertilizer', $sliderFertilizer);
      $sliderSprayedBox=(new BootStrap())->plainCard('Sprayed', $sliderSprayed);
         
            $allBox=(new BootStrap())->plainCard('All', $this->inputOnOff($this->basicOjectsArr));
            
    $left=<<<HTML
        {$sliderManureBox}
        {$sliderFertilizerBox}
        {$sliderSprayedBox}
            
        {$allBox}
          <div id="checked-area"></div>    
    HTML;        
                    
           
              $right= ' <div class="fieldMapBig" id="map"></div>';
                    
                    
                    
              return      (new BootStrap())->oneFifthFourFifth($left, $right);
                    

    
    }
    
    
    public function mapJavaScript() {
    ?>
                 
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
  
  $( function() {
    $( "#slider-sprayed" ).slider({
      range: "min",
      value: 0,
      min: 0,
      max: 104,
      
            slide: function( event, ui ) {
                //show gui chnaginga mount as you slide
        $( "#slider-sprayed-amount" ).val( ui.value );
      }
    });
    $( "#slider-sprayed-amount" ).val( $( "#slider-sprayed" ).slider( "value" ) );
  } );
  
  

  </script>
        
        <script>
            var map;

            /* have to be here for scope reasons */
         
           
           
            var manureLayer;
            var fertilizerLayer;
             var sprayedLayer;
            
              <?php echo($this->jsVarOnOff($this->basicOjectsArr));?>

            function initMap() {
                map = new google.maps.Map(document.getElementById('map'), {
                    center: {lat: 45.9788769346638, lng: -64.9880610086718},
                    zoom: 14,
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

            
            
                manureLayer = new google.maps.Data();
                fertilizerLayer = new google.maps.Data();
                sprayedLayer = new google.maps.Data();
                 
                   <?php echo($this->jsMapExtraOnOff($this->basicOjectsArr));?>
               
                 
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


  <?php echo($this->jsDoLayerStuffOnOff($this->basicOjectsArr));?>
      
             //turn on spfh base layer as default...maybe
                    spfhLayer.setMap(map); //depends on config
              

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
                    
                     
                     /* request to get layers area in hectares*/
                     $.ajax({
  url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?>/14/SpecificCroppingTotalArea/"+layerName,
  cache: false
})
  .done(function( html ) {
    $( "#checked-area" ).append( layerName+': '+(html)+' ha. <br/>' );
  });
                } 
                else
                {
                    layerObj.setMap(null);
                     $('#checked-area').text('');
                }   
                }
                
             
              
                
                  <?php echo($this->jsCheckIfOnOff($this->basicOjectsArr));?>

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

                    /* ** */
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
                                                //load geo json url for object, before we use it.
                                                  $.when(  manureLayer.loadGeoJson('<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?>/14/slider/'+encodeURIComponent(JSON.stringify(obj)))).done(function() {
                                                       manureLayer.setMap(map); 
                                                  })
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
                                                 //load geo json url for object, before we use it.
                                                  $.when(  fertilizerLayer.loadGeoJson('<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?>/14/slider/'+encodeURIComponent(JSON.stringify(obj)))).done(function() {
                                                       fertilizerLayer.setMap(map); 
                                                  })                        
                                        }                  
                      }
                 });
                    
                    
                     /* **** */
                     $( "#slider-sprayed" ).slider({                                           
                       stop: function( event, ui ) {
                  
                           //when you stop sliding, show on map                    
                          if (ui.value == null)  
                                  {
                                  sprayedLayer.setMap(null); 
                                  }
                                  else {
                                      var obj = {type:"sprayed", filter:createFieldFilter(), time:ui.value};                                     
                                                 sprayedLayer.forEach(function(feature) {           sprayedLayer.remove(feature);   }); //remove all data from data layer, before we add more data.
                                                 //load geo json url for object, before we use it.
                                                  $.when(  sprayedLayer.loadGeoJson('<?php echo($GLOBALS['config']['HTTP']['JSON_API']);?>/14/slider/'+encodeURIComponent(JSON.stringify(obj)))).done(function() {
                                                       sprayedLayer.setMap(map); 
                                                  })                          
                                        }                  
                      }
                 });
                    
                    
                                        
                            }
                       
                         
        </script>
        <?php
    }

    /*
     * returns the total area only
     * 
     */
    public function SpecificCroppingTotalArea($request) {
        if (!empty($request['item'])) {
            if (is_string($request['item'])) {
                $item = filter_var($request['item'], FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_HIGH);
            }
        } else {
            header("HTTP/1.1 400 Error");
            throw new Exception('Error: no resource specified');
        }
        
          $this->basicElementsConstructor(); //run constructor, because class called via REST. hackish, maps should be seperate class. 
        
  
       if (empty($this->basicOjectsArr[$item])) {
            header("HTTP/1.1 400 Error");
            throw new Exception("Error item $item does not exist.");
        }
        
         $obj=$this->basicOjectsArr[$item];
         $sqlToRun=$obj->sql;
        
              
          $statement = $GLOBALS['pdo']->prepare($sqlToRun);
        $statement->execute();
        $row =$statement->fetch(PDO::FETCH_ASSOC);
         
        return round($row['total_area'],1);
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

       $this->basicElementsConstructor(); //run constructor, because class called via REST. hackish, maps should be seperate class. 
        
  
       if (empty($this->basicOjectsArr[$item])) {
            header("HTTP/1.1 400 Error");
            throw new Exception("Error item $item does not exist.");
        }
        
         $obj=$this->basicOjectsArr[$item];
         $sqlToRun=$obj->sql;
        
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
   
        //var_dump($fieldFilter);
   
        switch ($type) {
            case 'manure':
//manure vairable length of time in past
                $sql = <<<EOD
SELECT  distinct(manure_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, '68f442' as fillcolor
FROM cropping.manure_event 
JOIN cropping.foragecurr on foragecurr.id=manure_event.field_id
WHERE all_covered=true and manure_event.event_time > now() - interval '$time week' AND manure_event.field_id IN ({$this->decodeFieldlFilter($fieldFilter)})
EOD;
                break;
            case 'fertilizer':
//manure vairable length of time in past
                $sql = <<<EOD
SELECT  distinct(fertilizer_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, 'e5b285' as fillcolor
FROM cropping.fertilizer_event 
JOIN cropping.fieldcurr on fieldcurr.id=fertilizer_event.field_id
WHERE all_covered=true and fertilizer_event.event_time > now() - interval '$time week' AND fertilizer_event.field_id IN ({$this->decodeFieldlFilter($fieldFilter)})
EOD;
                break;
               case 'sprayed':
//sprayed vairable length of time in past
                $sql = <<<EOD
SELECT  distinct(spray_event.field_id), *,ST_AsGeoJSON (border_geom) as border_geojson, '5386fc' as fillcolor
FROM cropping.spray_event 
JOIN cropping.fieldcurr on fieldcurr.id=spray_event.field_id
WHERE spray_event.event_time > now() - interval '$time week' AND spray_event.field_id IN ({$this->decodeFieldlFilter($fieldFilter)})
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

    //red to blue gradient
//from: https://stackoverflow.com/questions/20579351/assigning-color-values-in-a-range-based-on-numberic-values
    function colorGradient($value,$max=100,$min=0) {
$position=($value-$min)/($max-$min);
$highColor=hexdec("FF0000"); //converts to decimal for multiplication
$lowColor=hexdec("00FF00");
$newColor=intval($position*($highColor-$lowColor)+$lowColor);
$newColorStr=dechex($newColor);
return $newColorStr;
    }
    
}
//end class
?>