<?php
/*
 * SELECT gis.ST_Difference((SELECT geom FROM cropping.datum WHERE id=4157),(SELECT geom FROM cropping.datum WHERE id=3246))
 */

class Field extends TabPage {

    public function defaultDisplay() {

        //setup for tabs.
        $tabArray[1]['name'] = 'Field';
        $tabArray[2]['name'] = 'Border';
        $tabArray[3]['name'] = 'Seeding';
        $tabArray[4]['name'] = 'Seed Types';
        $tabArray[5]['name'] = 'Crop Planner';
        $tabArray[6]['name'] = 'Seed Inventory';
        $tabArray[7]['name'] = 'Tillage';
        $tabArray[8]['name'] = 'Manure';
        $tabArray[9]['name'] = 'Spray';
        $tabArray[10]['name'] = 'Fertilizer';
        $tabArray[11]['name'] = 'Fence';
        $tabArray[12]['name'] = 'Lime';
        $tabArray[13]['name'] = 'Param';
        $tabArray[14]['name'] = 'Rotation';
        $tabArray[15]['name'] = 'Yield';
        $this->renderTabs($tabArray);
    }

    protected function tab1() {
        $this->addNewFieldQuickform();
        print($this->displayAllActiveFieldsList());
    }

    protected function tab2() {
        $this->addNewFieldBorderQuickform();
    }

    protected function tab3() {
        print($this->displaySeedingInfo());
        $this->addNewSeedingToAFieldQuickform('default', null);
        $this->addNewCornSoybeanSeedingQuickform('default', null);
        print($this->displaySeedEventLog());
    }

    protected function tab4() {
        $this->addNewSeedTypeQuickform();
        print($this->displayListOfSeeds());
    }

    protected function tab5() {
        
            $left = ($this->addNewSeedingToAFieldQuickform('potential', null));
        $year = date('Y', strtotime('+ 1 year'));
         $right= $this->displayFieldsForCurrentSeasonPlanting($year) . $this->findPotentialSeedingSums($year);
        $content = (new Bootstrap)->oneThirdTwoThirds($right,$left);
        print($content);

        print("<hr/>");
        $year = date('Y', strtotime('now'));
        print($this->displayFieldsForCurrentSeasonPlanting($year));
        $this->findPotentialSeedingSums($year);
        print("<hr/>");
        $year = date('Y', strtotime('- 1 year'));
        print($this->displayFieldsForCurrentSeasonPlanting($year));
        $this->findPotentialSeedingSums($year);
    }

    protected function tab6() {
        $this->addNewSeedInventoryQuickform();
        print($this->displayCurrentSeedInventory());
    }

    protected function tab7() {
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $this->addNewTillageEventQuickform();
        }
        print($this->displayTillageEvents());
    }

    protected function tab8() {
        //FIXME add quickform2
        /*
 

         if ($GLOBALS['auth']->getOwnerAccess() == 1) {
          Manure::addNewManureLoadsEventQuickform();
          Manure::addNewManureEventQuickform();
          }
         * 
         */
        (new Manure)->displayAllManureEventsLast1000();
    }

    protected function tab9() {
        $out[]=
         $out[]=($this->displaySprayEvents());
        $out[]= $this->displaySprayInfo();
            $out[]=($this->displaySprayLog());
            
        $left=implode($out);
       // $this->addSprayEventQuickform();
        $a = new QF2_SprayEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
         $out2[]=($a->renderBox('QF2_SprayEvent', 'Enter Field(s) Spray Events'));
         $out2[]=($a->formJS());
     
         
         $right=implode($out2);
         
         print(new BootStrap)->oneThirdTwoThirds($left, $right);
         
        
    }

    protected function tab10() {
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $this->addNewFertilizerEventQuickform('default');
            $this->addNewFertilizerEventQuickform('scheduled');
        }
        print($this->displayFertilizerEventScheduled());
        print($this->displayFertilizerEvent());
    }

    protected function tab11() {
        
    }

    protected function tab12() {
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            print("<div class='row'>");

            print("<div class='col-sm-4 col-sm-push-8'>");
            $a = new QF2_LimeEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
            print($a->renderBox('QF2_LimeEvent', 'Enter Field(s) Lime Events'));
            print($a->formJS());
            print("</div>");
            
            print("<div class='col-sm-8 col-sm-pull-4'>");
            print($this->displayLimeEvent());
            print("</div>");

            print("</div>");
        }
    }

    protected function tab13() {
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $this->addNewFieldParamQuickform();
            $this->addNewGeneralCommentQuickform('default');
            print($this->displayFieldParamEvent());
        }
    }

    protected function tab14() {
        $this->displayCurrentYearCropRotation();
    }

    protected function tab15() {
        print($this->displayCornSnaplangeYieldInfo(date("Y", strtotime("0 year"))));
        print($this->displayCornSnaplangeYieldInfo(date("Y", strtotime("-1 year"))));
        print($this->displayCornSnaplangeYieldInfo(date("Y", strtotime("-2 year"))));
        print("<br/>");
        print($this->displayBaleYieldInfo(date("Y", strtotime("0 year"))));
        print($this->displayBaleYieldInfo(date("Y", strtotime("-1 year"))));
        print($this->displayBaleYieldInfo(date("Y", strtotime("-2 year"))));
        print("<br/>");
        print($this->displayCornYieldChart(date("Y", strtotime("0 year"))));
        print($this->displayCornYieldChart(date("Y", strtotime("-1 year"))));
        //print($this->displayCornYieldChart(date("Y", strtotime("-2 year"))));
        print("<br/>");
        $this->plotCornYield(date("Y", strtotime("-1 year")));
        // $this->plotCornYield(date("Y", strtotime("-1 year")));
        // $this->plotCornYield(date("Y", strtotime("-2 year")));
        print("<br/><h3>All Years Corn Yields</h3>");
        $this->plotAllYearsCornYield();
    }

    function plotAllYearsCornYield() {
        $sql = "
(with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '2012-04-01' AND event_time <= '2012-12-31'  AND specific_type = 'Corn'
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year (2012)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE feed_type_name = 'Corn Silage' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,2012 as year ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha)
    UNION
(with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '2013-04-01' AND event_time <= '2013-12-31'  AND specific_type = 'Corn'
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year (2013)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE feed_type_name = 'Corn Silage' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,2013 as year ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha)

UNION
(with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '2014-04-01' AND event_time <= '2014-12-31'  AND specific_type = 'Corn'
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year (2014)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE (feed_type_name = 'Corn Silage' OR feed_type_name = 'Corn Silage (BMR)')
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,2014 as year ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha)


UNION
(with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '2011-04-01' AND event_time <= '2011-12-31'  AND specific_type = 'Corn'
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year (2011)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE feed_type_name = 'Corn Silage' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,2011 as year ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha)
 UNION
(with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '2010-04-01' AND event_time <= '2010-12-31'  AND specific_type = 'Corn'
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year (2010)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE feed_type_name = 'Corn Silage' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,2010 as year ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha)
";
        ?>
        <script type="text/javascript">


            setTimeout(drawChart, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart, true);
            $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart);
            });
            function drawChart() {
            var data = google.visualization.arrayToDataTable([
            ['alpha_numeric_id', 'dm_tonne_per_ha', 'year', 'name', 'field_ha'],
            <?php
            $res = $GLOBALS ['pdo']->query($sql);
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                print("['{$row['alpha_numeric_id']}', {$row['dm_tonne_per_ha']},	{$row['year']},   '{$row['name']}', {$row['field_ha']}],");
            }
            ?>


            ]);

            var options = {
            title: 'Corn Silage Yields',
            hAxis: {title: 'Relative DM Tonne/ha'},
            vAxis: {title: 'Year'},
            bubble: {textStyle: {fontSize: 11}},
            vAxis: {
            title: 'Year',
            viewWindowMode: 'explicit',
            viewWindow: {
            max: 2015,
            min: 2009
            }},
            width: 1400,
            height: 800

            };

            var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all'));
            chart.draw(data, options);
            }
        </script>




        <?php
        print("<div id='chart_divv_all' ></div>");
    }

    function plotCornYield($year) {
        print("<h3>Corn Yields $year</h3>");
        $sql = "

with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '$year-04-01' AND event_time <= '$year-12-31'  AND (specific_type = 'Corn' OR specific_type = 'BMR Corn')
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year ($year)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE feed_type_name = 'Corn Silage' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Corn Silage (BMR)' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha
    

";
        ?>
        <script type="text/javascript">
            var year = <?php echo($year); ?>;

            setTimeout(drawChart<?php echo("_cornplot_$year"); ?>, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart<?php echo("_cornplot_$year"); ?>, true);
            $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart<?php echo("_cornplot_$year"); ?>);
            });
            function drawChart<?php echo("_cornplot_$year"); ?>() {
            var data = google.visualization.arrayToDataTable([
            ['alpha_numeric_id', 'dm_tonne_per_ha', 'chu', 'name', 'field_ha'],
            <?php
            $res = $GLOBALS ['pdo']->query($sql);
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                print("['{$row['alpha_numeric_id']}', {$row['dm_tonne_per_ha']},	{$row['chu']},   '{$row['name']}', {$row['field_ha']}],");
            }
            ?>


            ]);

            var options = {
            title: 'Corn Yields',
            hAxis: {title: 'dm_tonne_per_ha'},
            vAxis: {title: 'chu'},
            bubble: {textStyle: {fontSize: 11}}
            };

            var chart<?php echo("_cornplot_$year"); ?> = new google.visualization.BubbleChart(document.getElementById('chart_divv' + year));
            chart<?php echo("_cornplot_$year"); ?>.draw(data, options);
            }
        </script>




        <?php
        print("<div id='chart_divv$year' ></div>");
    }

    //the 1.3 is a guess for snaplage ratio to corn silage, density already taken into account. fixme.
    function displayCornYieldChart($year) {
        $sql = "

with temp as (
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '$year-04-01' AND event_time <= '$year-12-31' AND (specific_type = 'Corn' OR specific_type = 'BMR Corn')
), temp2 as (
SELECT field_crop_yield_year.*,seed_id,company,name,chu FROM cropping.field_crop_yield_year ($year)
LEFT JOIN temp ON temp.field_id = field_crop_yield_year.field_id 
),tempx as (
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)  as corrected_linear_yield  
FROM temp2
WHERE feed_type_name = 'Corn Silage' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Corn Silage (BMR)' 
UNION
SELECT temp2.*,cropping.calculate_dm_linear_yield (diameter_foot, yield_bag_feet, feed_type_name)*1.3 as corrected_linear_yield  
FROM temp2
WHERE  feed_type_name = 'Snaplage' 
)
SELECT distinct(field_id),field_id, alpha_numeric_id ,company,name,chu, field_ha,sum(corrected_linear_yield) OVER (partition BY field_id),sum(corrected_linear_yield) OVER (partition BY field_id)/field_ha/1000 as dm_tonne_per_ha  from tempx ORDER BY dm_tonne_per_ha
    

";


        return ( (new JQueryDataTable)->startBasicSql("Corn Yield $year", $sql, 'order: false'));

    }

    function displayBaleYieldInfo($year = 2014) {
        $sql = <<<SQL
With temp as(
SELECT *,
round((gis.ST_Area(gis.ST_Transform(border_geom, 2036))/10000)::numeric,2) as area2,
extract(year from yield_event.event_time) as year_of_interest
 FROM cropping.yield_event 
LEFT JOIN cropping.fieldcurr ON yield_event.field_id=fieldcurr.id
LEFT JOIN cropping.yield_type ON yield_type.id = yield_type_id WHERE extract(year from yield_event.event_time)=$year
)
SELECT *,amount/temp.area2 as yield,
(SELECT sum(amount) from temp group by temp.year_of_interest) as amount_per_year,
(SELECT sum(area) from temp group by  temp.year_of_interest) as area_per_year
FROM temp 
ORDER BY alpha_numeric_id;
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Name'] = ($row['name']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Yield'] = ($row['yield']);
            $rowOut['Amount'] = ($row['amount']);
            $rowOut['area'] = ($row['area']);
            $rowOut['Total Amount / year'] = ($row['amount_per_year']);
            $rowOut['Total Amount / area /year'] = ($row['area_per_year']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Bale Yield $year</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    function displayCornSnaplangeYieldInfo($year = 2014) {

        $sqlX1 = "
/* this table is used to make a table of what corn seed was planted inw hat field for a year */
CREATE  TEMP TABLE temp_seed_type_$year AS
SELECT seed_id,field_id,company,name,chu from cropping.seed_event 
LEFT JOIN cropping.seed ON seed_id=seed.id
LEFT JOIN cropping.seed_category ON seed_category_id=seed_category.id
WHERE event_time >= '$year-04-01' AND event_time <= '$year-12-31'  AND specific_type = 'Corn';
";

        $sqlX2 = "         
/* this table calculates yield numbers */
CREATE  TEMP TABLE temp_bag_yield_current_$year AS 
            With temp as(
SELECT event_time,field.id as field_id,alpha_numeric_id ,abs(footage_finish-footage_start) as footage_yield, diameter_foot, nutrition.bag_feed_name_at_footage(bag.id::integer, bag_field.footage_start::integer)::text as feed_type_name,
 ( SELECT border.datum_id
                   FROM cropping.border_event border
                  WHERE border.field_id = field.id AND border.event_time = (( SELECT max(border.event_time) AS max
                           FROM cropping.border_event border
                          WHERE border.field_id = field.id AND border.event_time <= 'now'::text::date))) AS datum_id

 FROM nutrition.bag
LEFT JOIN nutrition.bag_field ON bag_id=bag.id 
LEFT JOIN cropping.field on field_id=field.id
WHERE event_time >'$year-04-01' AND event_time <= '$year-12-31' 
), temp2 as(
SELECT *,sum(footage_yield) OVER (partition BY field_id, feed_type_name),geom as border_geom
 FROM temp
LEFT JOIN cropping.datum ON temp.datum_id=datum.id
), temp3 as (
SELECT DISTINCT(field_id,feed_type_name) event_time,field_id,alpha_numeric_id, diameter_foot,feed_type_name,sum as season_yield,gis.ST_Area(gis.ST_Transform(border_geom, 2036))/10000 as field_ha 
FROM temp2  WHERE field_id is not null ORDER BY alpha_numeric_id	
)
SELECT field_id,alpha_numeric_id,feed_type_name,season_yield/field_ha as yield_per_ha, (season_yield/field_ha)*pi()*(diameter_foot/2)*(diameter_foot/2)*13*0.453592 as dm_kg_yield_per_linear_foot
FROM temp3 ORDER BY feed_type_name,alpha_numeric_id;
";

        //used by printYieldByCropType
        $sqlX3 = "
/* this combines the two temp tables together */
SELECT * FROM temp_bag_yield_current_$year
LEFT JOIN temp_seed_type_$year ON temp_seed_type_$year.field_id = temp_bag_yield_current_$year.field_id 
WHERE feed_type_name = 'Corn Silage' OR feed_type_name = 'Snaplage' ORDER by yield_per_ha

";

        $sql = "SELECT DISTINCT(field_id) field_id,alpha_numeric_id,$year as year FROM temp_bag_yield_current_$year ORDER BY alpha_numeric_id";

        $resX1 = $GLOBALS['pdo']->prepare($sqlX1);
        $resX1->execute();
        $resX2 = $GLOBALS['pdo']->prepare($sqlX2);
        $resX2->execute();
        $resX = $GLOBALS['pdo']->query($sqlX3);     //used by printYieldByCropType


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Yield'] = $this->printYieldByCropType($row);
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        return ( (new JQueryDataTable)->startBasicArrayOpt("Corn & Snaplage Yield for $year", $dataArr, 'order: false'));
      
    }

    /**
     * NESTED Information
     */
    function printYieldByCropType($record) {

        /*
         * NESTED table
         */
        $year = $record['year'];
        
        if ($record['field_id'] != null) {
            $sql = "SELECT feed_type_name,round(yield_per_ha::numeric,2) as yield_per_ha,round((dm_kg_yield_per_linear_foot/1000)::numeric,2) as dm_tonne_yield_per_ha FROM   temp_bag_yield_current_$year WHERE field_id={$record['field_id']} ORDER BY feed_type_name";
            return (new JQueryDataTable(false))->startBasicSql(null, $sql, 'order: false');
        } else {
            print_r2($record);
            throw new Exception("Error, no field_id provided for yield by crop.</br>");
        }
    }

    function printYieldDetails($record) {

        return $record['feed_type_name'] . '&nbsp;' . $record['yield_per_ha'] . '(bag ft/ha) &nbsp;' . $record['dm_tonne_yield_per_ha'] . '(DM tonne/ha) &nbsp;';
    }

    /* list of current fields  for quickform */
    public function currentFieldsArr() {
        //grab a list of fields.
        $res = $GLOBALS['pdo']->query("SELECT id,alpha_numeric_id,common_name FROM cropping.fieldcurr ORDER BY alpha_numeric_id ASC");

        $listOfFields[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $listOfFields[$row['id']] = $row['alpha_numeric_id'] . ' - ' . $row['common_name'];
        }
        
        return $listOfFields;
    }
    
    
    
    /* FORM portion */

    function quickformLinestringFORM($form) {
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/google/googleMapsDrawing.php');
        $field_id = $form->exportValue('field');

        //we need some javascript to find out what field is currently seleced 
        //from the drop down list to know which field_id to put in here.


        $gmd = new googleMapsDrawing();
        $gmd->colorBoxMap();

        /*
          $colorString = "<script type='text/javascript' language='javascript'>
          $(document).ready(function(){

          $('#colorBoxToggle-map').toggle(); //hide element when page loads.

          $('#field').change(function(){

          var str='/functions/googleMapsDrawing.php?openpage=true&field_id=' + ($('#field').val()) ;
          $('#colorBoxToggle-map').show(); //show
          $('#colorBoxToggle-map').attr('href', str);
          })
          });
          </script>
          ";
          print($colorString);
         */



        $form->addElement('textarea', 'polygon', 'Overide Border With Custom Area (KML or Linestring)', array('cols' => 40, 'rows' => 2));
        $form->addElement('html', "\n" . "<br/><div >");
        $form->addElement('html', "<a id=\"colorBoxToggle-map\" href=\"#\">Select Via Map</a>\n");
        $form->addElement('html', "\n" . '</div>');

        //custom rule to check polygon input is of the correct format
        $form->registerRule('areaValidRule', 'function', 'checkAreaIsValid', 'CroppingHelper');
        $form->addRule(array('polygon'), 'ERROR: Area has to be of form "num,num,num OR linestring AND closed loop.".', 'areaValidRule', array('polygon'));

        return $form;
    }

    /* VALIDATE portion */
    /* NOTE: Always use withing a DB transaction, to make it atomic */

    public function quickformLinestringVALIDATE($form, $event_time, $border = false) {
        // do insert of new polygon data and get the datum_id for it after
        // insert.
        //first off find the datum of the current border at that event_time.
        // the borders could be different from the current time.
        $sql = "SELECT border_event.datum_id
				FROM cropping.border_event
				WHERE border_event.field_id = {$form->exportValue('field')} AND border_event.event_time =
				(( SELECT max(border_event.event_time) AS max
				FROM cropping.border_event
				WHERE border_event.field_id = {$form->exportValue('field')} AND border_event.event_time <= '$event_time'::date))
				";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $last_border_datum_id = $row ['datum_id'];


        if ($form->getElementValue('polygon') != null) {

            $lineString = (new CroppingHelper)->kmlArrayToLinestringStr((new CroppingHelper)->convertLineStringORNumNumNum($form->getElementValue('polygon')));

            //when we are adding a new border, we don't want to check that it overlaps the current border.
            if ($border == false) {
                //using the current border and the enetered lineString
                //return the part that overlaps, FIXME: if they don't overlap, errors out.
                $sql2 = "SELECT (gis.ST_Intersection((SELECT geom FROM cropping.datum WHERE id=$last_border_datum_id), gis.ST_GeomFromText('POLYGON(($lineString))',4326) )) as inter";
                $res2 = $GLOBALS ['pdo']->query($sql2);
                $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            } else {

                $row2['inter'] = "POLYGON(($lineString))";
            }


            // do an insert of the lat/long data for the field first and
            // then return a datum id.
            $query3 = "INSERT INTO cropping.datum (geom) VALUES (gis.ST_GeomFromText('{$row2['inter']}',4326))";
            $res3 = $GLOBALS ['pdo']->exec($query3);
            $last_datum_id = $GLOBALS['pdo']->lastInsertId('cropping.cropping_id_seq');
        } else { //asume we are using current default borer.
            $last_datum_id = $last_border_datum_id; //use border at time.
        }
        return $last_datum_id;
    }

    /**
     * turns longer crop rotations shows text in different shades of red to warn user.
     */
    function printWarningForLongCropRotation($stand_age) {


        if ($stand_age == 4) {
            $str = '<b id="lightRed">' . $stand_age . '</b>';
        } elseif ($stand_age > 4) {
            $str = '<b id="boldRed" class="boldBlink">' . $stand_age . '</b>';
        } else {
            $str = $stand_age;
        }

        return $str;
    }

    /**
     * displays the crop roation stat
     */
    public function currentYearCropRotation($request) {
        $sql = "SELECT *,id as field_id,event_time,
                extract('year' FROM age(date_trunc('year',event_time)))+1 as stand_age
                FROM cropping.rotationcurr
		";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['field_number'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] = date("M d Y", strtotime($row['event_time']));  //has to be at end.
            $row['stand_age'] = $this->printWarningForLongCropRotation($row['stand_age']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayCurrentYearCropRotation() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                        
            {
                data: "field_number",
                title: "Field",
                type: "num-html"
            },
            {
                data: "specific_type",
                title: "Crop"
        },
            
            {
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "stand_age",
                title: "Stand Age"
        }
       
           
EX;
        print("<h3>Crop Rotation</h3>");
        print($dt->startCustom('Title Custom CHANGE ME', 4, 'currentYearCropRotation', null, $columnsStr));
    }

    /**
     * General Comment quickform
     */
    function addNewGeneralCommentQuickform($type = 'default', $field_id = null) {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();


        $form = new HTML_QuickForm("addNewGeneralCommentQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->removeAttribute('name');

        $form->addElement('header', 'addNewGeneralCommentQuickform', 'Add a New General Commnet');
        if ($field_id != null) {
            $form->addElement('hidden', 'field', $field_id);
        } else {
            $form->addElement('select', 'field', 'Field', $fieldArray);
        }
        $form->addElement('date', 'date_start', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));

        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));
        $form = self::quickformLinestringFORM($form);
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('date_start' => 'now')); // set default to	
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Date Start is required', 'required');

        // Try to validate a form
        if ($form->validate()) {




            $field_id = $form->exportValue('field');
            $tempArray = $form->getElementValue('date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));
            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = null;
            }

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);






                // do an insert into cropping field and datum of data.
                $query = "INSERT INTO cropping.general_comment_event (field_id,datum_id,event_time,userid,comment) VALUES ($field_id,$last_datum_id,'$event_time','{$GLOBALS['auth']->getUsername()}','$comment')";
                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    /**
     * Displays spray events for corn and soybeans.
     */
    function displayFieldParamEvent($mode = 'default') {
        $sql = <<<SQL
SELECT *
		FROM cropping.field_parameter
		LEFT JOIN cropping.field ON field.id=field_parameter.field_id
		ORDER BY event_time DESC,alpha_numeric_id
SQL;


        $out[] = ("<h3>Field Paramaters</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql, 'order: false'));
        return implode($out);
    }

    //returns text represenation of drainage score. 
    public function drainageScore($number) {
        $arr=$this->drainageScoreArr();
        return $arr[$number];
    }
    
    
     public function drainageScoreArr() {
        
        return array('1'=>'Tiled (Systematically)','2'=>'Tiled (Some)', '3'=>'Interval (Flood Plain)', '4'=>'Upland (Well Drained)', '5'=>'Normal', '6'=>'Wet (Ruts Up)' );
    }
    
    
    /**
     * Adds a new type of seed.
     */
    function addNewFieldParamQuickform($type = 'default', $field_id = null) {



        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        $form = new HTML_QuickForm("addNewFieldParamQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'addNewFieldParamQuickform', 'Edit a Field Paramater');

        if ($field_id != null) {
            $form->addElement('hidden', 'field', $field_id);
        } else {
            $form->addElement('select', 'field', 'Field', $fieldArray);
        }
        $form->addElement('date', 'event_time', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
        $form->addElement('advcheckbox', 'pasture', 'pasture');
        $form->addElement('advcheckbox', 'not_normally_farmed', 'not_normally_farmed');
        $form->addElement('advcheckbox', 'alfalfa_suitable', 'alfalfa_suitable');
        $form->addElement('advcheckbox', 'corn_suitable', 'corn_suitable');
        $form->addElement('advcheckbox', 'trefoil_suitable', 'trefoil_suitable');
        $form->addElement('advcheckbox', 'spfh_suitable', 'spfh_suitable');
        $form->addElement('select', 'drainage_score', 'drainage_score', $this->drainageScoreArr());

        // defaults
        // rules
        $form->setDefaults(array('event_time' => 'now'));


        //FIELD PARAM IS TEMPORAL, load last values as default.
        //load latest defaults
        if (!empty($field_id)) {
            $sql = "SELECT * FROM cropping.field_parameter WHERE id=cropping.field_parameter_id_at_timestamp ($field_id, now()::timestamp)";
            $res = $GLOBALS ['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $form->setDefaults(array("pasture" => $row['pasture']));
            $form->setDefaults(array("not_normally_farmed" => $row['not_normally_farmed']));
            $form->setDefaults(array("alfalfa_suitable" => $row['alfalfa_suitable']));
            $form->setDefaults(array("corn_suitable" => $row['corn_suitable']));
            $form->setDefaults(array("trefoil_suitable" => $row['trefoil_suitable']));
            $form->setDefaults(array("spfh_suitable" => $row['spfh_suitable']));
            $form->setDefaults(array("drainage_score" => $row['drainage_score']));
            //TODO
        } else {
            print("TODO: does not load previous parameter default values, be careful.");
        }


        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('event_time', 'Date is required', 'required');

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $tempArray = $form->exportValue('event_time');
            $event_time = date('r', strtotime($tempArray['Y'] . '-' . $tempArray['m'] . '-' . $tempArray['d']));


            $field_id = $form->exportValue('field');
            $pasture = $form->exportValue('pasture');
            $not_normally_farmed = $form->exportValue('not_normally_farmed');
            $alfalfa_suitable = $form->exportValue('alfalfa_suitable');
            $corn_suitable = $form->exportValue('corn_suitable');
            $trefoil_suitable = $form->exportValue('trefoil_suitable');
            $spfh_suitable = $form->exportValue('spfh_suitable');
             $drainage_score = $form->exportValue('drainage_score');

            function trueOrFalse($val) {
                if ($val == 1) {
                    return 'TRUE';
                } else {
                    return 'FALSE';
                }
            }

            $pasture = trueOrFalse($pasture);
            $not_normally_farmed = trueOrFalse($not_normally_farmed);
            $alfalfa_suitable = trueOrFalse($alfalfa_suitable);
            $corn_suitable = trueOrFalse($corn_suitable);
            $trefoil_suitable = trueOrFalse($trefoil_suitable);
            $spfh_suitable = trueOrFalse($spfh_suitable);

            //FIXME, if you do the same event_time, this will fail.           
            // 
            $query = "INSERT INTO cropping.field_parameter (field_id,event_time,pasture,not_normally_farmed,userid,alfalfa_suitable,corn_suitable,trefoil_suitable,spfh_suitable,drainage_score) VALUES ($field_id,'$event_time','$pasture','$not_normally_farmed','{$GLOBALS['auth']->getUsername()}','$alfalfa_suitable','$corn_suitable','$trefoil_suitable','$spfh_suitable',$drainage_score) ON CONFLICT DO NOTHING";
            $res = $GLOBALS ['pdo']->exec($query);


            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation


        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    /**
     * Tillage performed on a field.
     */
    function addSprayEventQuickform() {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        $sql = "SELECT id,name FROM cropping.spray ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $typeArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $typeArray [$row ['id']] = $row ['name'];
        }


        // select active people in manure group to say who can spray, change in future to a seeder group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='manure' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $sprayUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $sprayUsers [$row ['userid']] = $row ['userid'];
        }


        $form = new HTML_QuickForm("addNewSprayEventQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'addNewSprayEventQuickform', 'Add a New spray Event');
        $form->addElement('select', 'field', 'Field', $fieldArray);
        $form->addElement('date', 'date_start', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
        $form->addElement('select', 'spray_type', 'spray Type', $typeArray);
        $form->addElement('text', 'spray_amount', 'Chemical Amount (l/hectare)');
        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));

        //only show the choice on who did the spray for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'spray_userid', 'Who did the spray', $sprayUsers);
            $form->addRule('spray_userid', 'spray User is required', 'required');
        } else {
            $form->addElement('hidden', 'spray_userid', $GLOBALS['auth']->getUsername());
        }


        $form = self::quickformLinestringFORM($form);

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('date_start' => 'now')); // set default to
        // today.
        $form->setDefaults(array('use_current_border_polygon' => '1')); // set
        // default to
        // today.
        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Seeding Date Start is required', 'required');
        $form->addRule('spray_type', 'spray Type is required', 'required');
        $form->addRule('spray_amount', 'spray Amount is required', 'required');
        $form->addRule('spray_amount', 'spray Amount must be a number', 'numeric');


        // Try to validate a form
        if ($form->validate()) {

            $field_id = $form->exportValue('field');

            $tempArray = $form->getElementValue('date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));
            $spray_id = $form->exportValue('spray_type');
            $spray_userid = $form->exportValue('spray_userid');
            $spray_amount = $form->exportValue('spray_amount');
            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = null;
            }

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);

                // do an insert into cropping field and datum of data.
                $query = "INSERT INTO cropping.spray_event (field_id,datum_id,event_time,spray_id,userid,comment,sprayer_userid,amount) VALUES ($field_id,$last_datum_id,'$event_time',$spray_id,'{$GLOBALS['auth']->getUsername()}','$comment','$spray_userid',$spray_amount)";
                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    function displaySprayInfo() {
        $out[]=('<br/>');
        $out[]=("OLD<br>");
        $out[]=("Marksman 6.0 liters per tank. High 2 gear on 5030, 1500 rpm. 40psi spray rate.");
        $out[]=('<br/>');
        $out[]=("Roundup 2.2 liters per tank. High 2 gear on 5030, 1500 rpm. 40psi spray rate.");
        $out[]=('<br/>');


        $out[]=("<a href='http://www.monsanto.ca/products/Documents/roundup_transorb_en.pdf'>Roundup Transorb HC Label</a>");
        $out[]=('<br/>');
        $out[]=("<a href='https://agro.basf.ca/basf/agprocan/agsolutions/WebASProduct.nsf/WebDocCodeEast/Marksman/Marksman.pdf'>Marksman</a>");
        $out[]=('<br/>');
        $out[]=("<a href='http://www.al.gov.bc.ca/pesticides/ipco-24d-label.pdf'>2,4-D Amine 600 Label</a>");
        $out[]=('<br/>');
        $out[]=("<a href='http://www2.dupont.com/Crop_Protection/en_CA/assets/downloads/Banvel-II.pdf'>Banvel II Label</a>");
        $out[]=('<br/>');
        $out[]=("<a href='http://www.syngentafarm.ca/pdf/labels/FLEXSTAR_GT_30412_en_pamphlet.pdf'>Flexstar GT (soybeans)</a>");
        $out[]=('<br/>');
        $out[]=("<a href='http://www.bayercropscience.ca/English/LabelMSDS/273/File.ashx'>Select (pure Alfalfa)</a>");
        $out[]=('<br/>');

        $out[]=('<img src="/images/sprayerNozzleCatalogue.svg" width="1000"/>');
        $out[]=('<br/>');
        $out[]=('<br/>');
        $out[]=('<br/>');

        $out[]= ("<h3> Totals</h3>");
        $year = date('Y', strtotime("now"));
         $out[]= $this->findPotentialSprayingTraitSums($year);
         return implode($out);
    }

    function displaySprayLog() {
        $sql = <<<SQL
SELECT *,spray_event.id as spray_event_id FROM cropping.spray_event 
LEFT JOIN cropping.field ON field.id=spray_event.field_id        
LEFT JOIN cropping.spray ON spray_event.spray_id=spray.id
ORDER by event_time DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        
           $delButton=new QF2_DeleteButtonDecider('user_timeLimit',(object) array('dateTimePeriodDays' =>'1','schemaTable'=>'cropping.spray_event'));
        
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Event Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Chemical'] = ($row['name']);
            $rowOut['l/ha'] = ($row['amount']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Sprayer Employee'] = ($row['sprayer_userid']);
            $rowOut['action']=  $delButton->output($row['spray_event_id']);     
            
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Spray Log</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    /**
     * Displays spray events for corn and soybeans.
     */
    function displaySprayEvents() {

        $sql = <<<SQL
With temp as (
SELECT seed_event .id as seed_event_id,seed_event.event_time as seed_event_time, field.id as field_id,alpha_numeric_id,common_name, specific_type,trait_rr,chu,company || ' ' || name as seed_name,population,comment,userid,event_time,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.seed_event 
        LEFT JOIN cropping.seed ON seed_event.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event.field_id
        LEFT JOIN cropping.datum ON seed_event.datum_id = datum.id
        WHERE seed_event.event_time >= (date_part('year', current_date) || '-01-01')::date AND seed_event.event_time < (date_part('year', current_date) || '-12-31 23:59:59')::timestamp AND (seed_category.specific_type = 'Soybean' OR seed_category.specific_type ='Corn')
ORDER BY alpha_numeric_id
) SELECT *,(SELECT true as current_spray FROM cropping.spray_event WHERE field_id=temp.field_id AND event_time >  (temp.seed_event_time + interval '7 day') AND event_time < (date_part('year', current_date) || '-12-31 23:59:59')::timestamp AND spray_event.event_time=(SELECT max(event_time) FROM cropping.spray_event WHERE field_id=temp.field_id AND event_time > (temp.seed_event_time + interval '7 day') AND event_time < (date_part('year', current_date) || '-12-31 23:59:59')::timestamp))

as current_spray FROM temp
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = empty($row['name']) ? '': $row['name'];
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['Planting Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Crop'] = ($row['specific_type']);
            $rowOut['Seed'] = ($row['seed_name']);
            $rowOut['Roundup?'] = ($row['trait_rr']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Sprayed?'] = ($row['current_spray']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Spray to do List (> 7days after planting)</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }


     function printActionButtonsDeleteLime($record,$callBackURL=null) {
        

        $currentUserid = $GLOBALS['auth']->getUsername();
          
         //normnal user
                    $sql = <<<SQL
SELECT lime_event.id as id
        FROM cropping.lime_event 
       WHERE userid='{$GLOBALS['auth']->getUsername()}' ORDER by event_time DESC limit 1
SQL;
        $res = $GLOBALS['pdo']->query($sql);
     
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
           
            if (($record['lime_event_id'] == $row['id'])) {
                 $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'cropping.lime_event'));
                 $returnButtonsAsString = ($a->renderDelete('salePrice' . $row['id']));                
            }
        }

        //print button
        return $returnButtonsAsString;
    }
    
    function displayLimeEvent($mode = 'default') {
        
        $sql = <<<SQL
SELECT *,lime_event.id as lime_event_id,   lime_event.id as delete_table_id, 'lime_event' as delete_table_name,lime.name as lime_type,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.lime_event 
        LEFT JOIN cropping.lime ON lime_event.lime_id=lime.id
        LEFT JOIN cropping.field ON field.id=lime_event.field_id
        LEFT JOIN cropping.datum ON lime_event.datum_id = datum.id
        ORDER BY event_time DESC,alpha_numeric_id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Amount'] = ($row['lime_amount']);
            $rowOut['Type'] = ($row['lime_type']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Employee'] = ($row['userid']); 
            $rowOut['Action'] = $this->printActionButtonsDeleteLime($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
   

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Lime Log</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    /**
     * **********************************************************
     * completed button.
     * **********************************************************
     */
    function printActionButtons2($record, $callBackURL) {

        $currentUserid = $GLOBALS ['auth']->getUsername();
        $returnButtonsAsString = '';

        // This sees if there is a fertilizer event between when it was
        // scheduled and the current time. This assumes this list is updated
        // every so often.
        $res = $GLOBALS ['pdo']->query("SELECT id,'fertilizer_event' as tablename  FROM cropping.fertilizer_event WHERE fertilizer_id={$record['fertilizer_id']} AND field_id={$record['field_id']} AND event_time <= now()  AND event_time >= '{$record['event_time']}'");
        //

        if ($res->rowCount() == 0) {
            // so this says a fertilizer event has not occured.
            // so when no fertilizer events were found we pass along the id of
            // the fertilizer scheduled event and the name of the table to then
            // insert to: 'fertilizer_event'
            $returnButtonsAsString = self::completedScheduledFertilzierButtonQuickForm($record ['fertilizer_event_scheduled_id'], 'fertilizer_event', $callBackURL); // show
            // complete
            // button
        }

        // print button
        // $returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    /**
     * **********************************************************
     * completed button fertilizer scheduled
     * **********************************************************
     */
    function completedScheduledFertilzierButtonQuickForm($tableid, $tablename, $callBackURL) {

        $fertilizer_users = self::whoCanSpreadFertilizer();

        $form = new HTML_QuickForm("completedButtonQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        //only show the choice on who did the fertilizer for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'fertilizer_userid', 'Who did the spreading?', $fertilizer_users);
            $form->addRule('fertilizer_userid', 'Fertilizer User is required', 'required');
        } else {
            $form->addElement('hidden', 'fertilizer_userid', $GLOBALS['auth']->getUsername());
        }
        $form->addElement('submit', 'btnCompleted', 'Completed');

        // FIXME:HUGE SECURITY HOLE HERE.
        // Try to validate a form
        if ($form->validate()) {

            // get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');
            $fertilizer_userid = $form->exportValue('fertilizer_userid');

            // use current time and userid for values.
            $event_time = date('r', strtotime("now")); // this has to be now because
            // we assume that is when the event
            // actually occurred.

            $userid = $GLOBALS ['auth']->getUsername();

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // lookup scheduled fertilizer and insert that info into fertilizer
                // event.
                $res = $GLOBALS ['pdo']->query("SELECT * FROM cropping.fertilizer_event_scheduled WHERE id=$tableid LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);

                $row ['comment'] = 'null'; // FIXME: add comment later
                // now do insert into fertilizer_event, with the info.
                $res = $GLOBALS ['pdo']->exec("INSERT INTO cropping.fertilizer_event (field_id,datum_id,event_time,fertilizer_id,userid,comment,fertilizer_application_method_id,amount,fertilizer_userid) VALUES ({$row['field_id']},{$row['datum_id']},' $event_time',{$row['fertilizer_id']},'{$GLOBALS['auth']->getUsername()}','{$row['comment']}',{$row['fertilizer_application_method_id']},{$row['amount']},'fertilizer_userid')");

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }

            header($callBackURL);
            exit();
        }
        return $form->toHtml(); // pass on as html string to display later in the
        // table
    }

    function displayFertilizerEventScheduled($mode = 'default') {


        $sql = <<<SQL
SELECT *,fertilizer_event_scheduled.id as fertilizer_event_scheduled_id,   fertilizer_event_scheduled.id as delete_table_id, 'fertilizer_event_scheduled' as delete_table_name,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.fertilizer_event_scheduled 
        LEFT JOIN cropping.fertilizer ON fertilizer_event_scheduled.fertilizer_id=fertilizer.id
        LEFT JOIN cropping.fertilizer_application_method ON fertilizer_application_method.id=fertilizer_event_scheduled.fertilizer_application_method_id
        LEFT JOIN cropping.field ON field.id=fertilizer_event_scheduled.field_id
        LEFT JOIN cropping.datum ON fertilizer_event_scheduled.datum_id = datum.id
        WHERE fertilizer_event_scheduled.event_time >=  (date_part('year', current_date) || '-01-01')::date AND fertilizer_event_scheduled.event_time < '2021-01-01'  ORDER BY alpha_numeric_id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Type'] = $this->printFertilzierType($row);
            $rowOut['Amount (kg/ha)'] = ($row['amount']);
            $rowOut['Method'] = ($row['name']);
            $rowOut['Crop Year'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['Employee'] = ($row['userid']);

           if ($GLOBALS['auth']->getOwnerAccess() == 1) {
                $rowOut['Action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            }
            $rowOut['Action2'] = $this->printActionButtons2($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");



            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Fertilizer To Do List</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    // area in hectares
    function printFertilzierType($record) {

        $boron = '';
        $n_source = '';
        if ($record['b'] != 0) {
            $boron = '-' . $record ['b'] . 'b';
        }
        if ($record['n_source'] == 'ammonium nitrate') {
            $n_source = 'AN';
        }
        return $record ['n'] . '-' . $record ['p'] . '-' . $record ['k'] . $boron . ' ' . $n_source;
        ;
    }

    function displayFertilizerEvent($mode = 'default') {


        $sql = <<<SQL
SELECT *,fertilizer_event.id as fertilizer_event_scheduled_id,   fertilizer_event.id as delete_table_id, 'fertilizer_event' as delete_table_name,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.fertilizer_event 
        LEFT JOIN cropping.fertilizer ON fertilizer_event.fertilizer_id=fertilizer.id
        LEFT JOIN cropping.fertilizer_application_method ON fertilizer_application_method.id=fertilizer_event.fertilizer_application_method_id
        LEFT JOIN cropping.field ON field.id=fertilizer_event.field_id
        LEFT JOIN cropping.datum ON fertilizer_event.datum_id = datum.id
        ORDER BY event_time DESC,alpha_numeric_id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Type'] = $this->printFertilzierType($row);
            $rowOut['Amount (kg/ha)'] = ($row['amount']);
            $rowOut['Method'] = ($row['name']);
            $rowOut['Crop Year'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['Employee'] = ($row['userid']);


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Fertilizer Lo</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    private function yieldTypes() {

        $sql = "SELECT id,name FROM cropping.yield_type ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $typeArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $typeArray [$row ['id']] = $row ['name'];
        }
        return $typeArray;
    }

    private function fertilizerTypes() {

        $sql = "SELECT id,n,p,k,b,n_source,s,zn FROM cropping.fertilizer ORDER BY n";
        $res = $GLOBALS ['pdo']->query($sql);
        $typeArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $boron = '';
            $sulphur = '';
            $zinc = '';
            $n_source = '';
            if ($row['b'] != 0) {
                $boron = '-' . $row ['b'] . 'b';
            }
            if ($row['s'] != 0) {
                $sulphur = '-' . $row ['s'] . 'S';
            }
            if ($row['zn'] != 0) {
                $zinc = '-' . $row ['zn'] . 'Zn';
            }
            if ($row['n_source'] == 'ammonium nitrate') {
                $n_source = 'AN';
            }

            $typeArray [$row ['id']] = $row ['n'] . '-' . $row ['p'] . '-' . $row ['k'] . $boron . $sulphur . $zinc . ' ' . $n_source;
        }
        return $typeArray;
    }

    // select active people in manure group to say who can spread manure, change in future to a fertilizer group if necessary
    function whoCanSpreadFertilizer() {

        // select active people in manure group to say who can spread manure, change in future to a fertilizer group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='manure' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $fertilizer_users [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $fertilizer_users [$row ['userid']] = $row ['userid'];
        }
        return $fertilizer_users;
    }

    function addNewYieldEventQuickform($type = 'default', $field_id = null) {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();


        $typeArray = self::yieldTypes();


        //FIXME: change to something differenty in future
        $yield_users = self::whoCanSpreadFertilizer();

        $form = new HTML_QuickForm("addNewYieldEventQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'addNewYieldEventQuickform', 'Add a New Yield Event');
        if ($field_id == null) {
            $form->addElement('select', 'field', 'Field', $fieldArray);
        } else {
            $form->addElement('hidden', 'field', $field_id);
        }
        $form->addElement('date', 'date_start', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
        $form->addElement('select', 'yield_type', 'Type', $typeArray);

        $form->addElement('text', 'amount', 'Amount (units)');
        //only show the choice on who did the yield for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'yield_userid', 'Who did the spreading?', $yield_users);
            $form->addRule('yield_userid', 'Yield User is required', 'required');
        } else {
            $form->addElement('hidden', 'yield_userid', $GLOBALS['auth']->getUsername());
        }

        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));
        //


        $form = self::quickformLinestringFORM($form);
        //
        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        // defaults
        $form->setDefaults(array('date_start' => 'now')); // set default to
        // today.
        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Seeding Date Start is required', 'required');
        $form->addRule('yield_type', 'Yield type is required', 'required');

        $form->addRule('amount', 'Amount is required', 'required');
        $form->addRule('amount', 'amount must be a number', 'numeric');

        // Try to validate a form
        if ($form->validate()) {

            // always the first of the month for scheduled.

            $tempArray = $form->getElementValue('date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));


            $field_id = $form->exportValue('field');
            $yield_id = $form->exportValue('yield_type');
            $yield_userid = $form->exportValue('yield_userid');

            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = null;
            }

            $amount = $form->exportValue('amount');

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);


                // do an insert into yield
                $query = "INSERT INTO cropping.yield_event (field_id,event_time,yield_type_id,comment,amount,userid) VALUES ($field_id,'$event_time',$yield_id,'$comment',$amount,'$yield_userid')";


                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    function addNewFertilizerEventQuickform($type = 'default', $field_id = null) {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();


        $typeArray = self::fertilizerTypes();



        $fertilizer_users = self::whoCanSpreadFertilizer();


        $sql = "SELECT id,name FROM cropping.fertilizer_application_method ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $application_methodArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $application_methodArray [$row ['id']] = $row ['name'];
        }

        if ($type == 'scheduled') {
            $form = new HTML_QuickForm("addNewFertilizerScheduledEventQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
            $renderer = new HTML_QuickForm_Renderer_Tableless();

            $form->addElement('header', 'addNewFertilizerScheduledEventQuickform', 'Add a New Scheduled Fertilizer Event');
            if ($field_id == null) {
                $form->addElement('select', 'field', 'Field', $fieldArray);
            } else {
                $form->addElement('hidden', 'field', $field_id);
            }
            $form->addElement('date', 'date_start', 'Date', array('format' => 'Y-m', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
            $form->addElement('select', 'fertilizer_type', 'Type', $typeArray);
            $form->addElement('select', 'application_method', 'Application Method', $application_methodArray);
            $form->addElement('text', 'amount', 'Amount (kg/ha)');
            //
            $form = self::quickformLinestringFORM($form);
            $form->addElement('hidden', 'comment', '');
            //
            $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
            $form->addElement('hidden', 'fertilizer_userid', $GLOBALS['auth']->getUsername());
            //
            // defaults
            $form->setDefaults(array('date_start' => 'now')); // set default to
            // today.
            $form->setDefaults(array('application_method' => '3830')); // set
            // default to
            // boradcast
            $form->setDefaults(array('fertilizer_type' => '3868')); // set default
            // to 10-0-40
            $form->setDefaults(array('date_start' => array('d' => '1', 'm' => '5', 'Y' => date('Y', strtotime('now')))));
            $form->setDefaults(array('amount' => '75'));
        } else {
            $form = new HTML_QuickForm("addNewFertilizerEventQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
            $renderer = new HTML_QuickForm_Renderer_Tableless();

            $form->addElement('header', 'addNewFertilizerEventQuickform', 'Add a New Fertilizer Event');
            if ($field_id == null) {
                $form->addElement('select', 'field', 'Field', $fieldArray);
            } else {
                $form->addElement('hidden', 'field', $field_id);
            }
            $form->addElement('date', 'date_start', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
            $form->addElement('select', 'fertilizer_type', 'Type', $typeArray);
            $form->addElement('select', 'application_method', 'Application Method', $application_methodArray);
            $form->addElement('text', 'amount', 'Amount (kg/ha)');
            //only show the choice on who did the fertilizer for admin users.
           if ($GLOBALS['auth']->getOwnerAccess() == 1) {
                $form->addElement('select', 'fertilizer_userid', 'Who did the spreading?', $fertilizer_users);
                $form->addRule('fertilizer_userid', 'Fertilizer User is required', 'required');
            } else {
                $form->addElement('hidden', 'fertilizer_userid', $GLOBALS['auth']->getUsername());
            }

            $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));
            //


            $form = self::quickformLinestringFORM($form);
            //
            $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
            // defaults
            $form->setDefaults(array('date_start' => 'now')); // set default to
            // today.
        }

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Seeding Date Start is required', 'required');
        $form->addRule('fertilizer_type', 'Fertilzier type is required', 'required');
        $form->addRule('application_method', 'Application method is required', 'required');
        $form->addRule('amount', 'Amount is required', 'required');
        $form->addRule('amount', 'amount must be a number', 'numeric');

        // Try to validate a form
        if ($form->validate()) {

            // always the first of the month for scheduled.
            if ($type == 'scheduled') {
                $tempArray = $form->getElementValue('date_start');
                $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . '1'));
            } else {
                $tempArray = $form->getElementValue('date_start');
                $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));
            }

            $field_id = $form->exportValue('field');
            $fertilizer_id = $form->exportValue('fertilizer_type');
            $fertilizer_userid = $form->exportValue('fertilizer_userid');

            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = null;
            }
            $application_method_id = $form->exportValue('application_method');
            $amount = $form->exportValue('amount');

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);

                if ($type == 'scheduled') {
                    // do an insert into fertilizer SCHEDULED
                    $query = "INSERT INTO cropping.fertilizer_event_scheduled (field_id,datum_id,event_time,fertilizer_id,userid,comment,fertilizer_application_method_id,amount) VALUES ($field_id,$last_datum_id,'$event_time',$fertilizer_id,'{$GLOBALS['auth']->getUsername()}','$comment',$application_method_id,$amount)";
                } else {
                    // do an insert into fertilizer
                    $query = "INSERT INTO cropping.fertilizer_event (field_id,datum_id,event_time,fertilizer_id,userid,comment,fertilizer_application_method_id,amount,fertilizer_userid) VALUES ($field_id,$last_datum_id,'$event_time',$fertilizer_id,'{$GLOBALS['auth']->getUsername()}','$comment',$application_method_id,$amount,'$fertilizer_userid')";
                }

                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        return ($renderer->toHtml());
    }

    function displayTillageEvents($mode = 'default') {
        $sql = <<<SQL
SELECT tillage_event.*,field.*,tillage.*,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.tillage_event 
        LEFT JOIN cropping.field ON field.id=tillage_event.field_id
        LEFT JOIN cropping.datum ON tillage_event.datum_id = datum.id
        LEFT JOIN cropping.tillage ON tillage_event.tillage_id= tillage.id
        ORDER BY event_time DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Type'] = ($row['name']);
            $rowOut['Employee'] = ($row['userid']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Tillage Events</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    /**
     * Tillage performed on a field.
     */
    function addNewTillageEventQuickform() {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        $sql = "SELECT id,name FROM cropping.tillage ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $typeArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $typeArray [$row ['id']] = $row ['name'];
        }


        // select active people in manure group to say who can till, change in future to a seeder group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='manure' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $tillageUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $tillageUsers [$row ['userid']] = $row ['userid'];
        }

        $form = new HTML_QuickForm("addNewTillageEventQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'addNewTillageEventQuickform', 'Add a New Tillage Event');
        $form->addElement('select', 'field', 'Field', $fieldArray);
        $form->addElement('date', 'date_start', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
        $form->addElement('select', 'tillage_type', 'Tillage Type', $typeArray);
        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));

        //only show the choice on who did the tillage for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'tillage_userid', 'Who did the tillage', $tillageUsers);
            $form->addRule('tillage_userid', 'Tillage User is required', 'required');
        } else {
            $form->addElement('hidden', 'tillage_userid', $GLOBALS['auth']->getUsername());
        }


        $form = self::quickformLinestringFORM($form);

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('date_start' => 'now')); // set default to
        // today.
        $form->setDefaults(array('use_current_border_polygon' => '1')); // set
        // default to
        // today.
        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Seeding Date Start is required', 'required');
        $form->addRule('tillage_type', 'Tillage Type is required', 'required');



        // Try to validate a form
        if ($form->validate()) {

            $field_id = $form->exportValue('field');

            $tempArray = $form->getElementValue('date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));
            $tillage_id = $form->exportValue('tillage_type');
            $tillage_userid = $form->exportValue('tillage_userid');
            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = null;
            }

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);

                // do an insert into cropping field and datum of data.
                $query = "INSERT INTO cropping.tillage_event (field_id,datum_id,event_time,tillage_id,userid,comment,tillage_userid) VALUES ($field_id,$last_datum_id,'$event_time',$tillage_id,'{$GLOBALS['auth']->getUsername()}','$comment','$tillage_userid')";
                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    function displayCurrentSeedInventory($mode = 'default') {

        $sql = "SELECT *
        FROM cropping.seed_inventory 
        LEFT JOIN cropping.seed ON seed_inventory.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
       ";

        $out[] = ("<h3>Current Seed Inventory</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql, 'order: false'));
        return implode($out);
    }

    function addNewSeedInventoryQuickform() {
        $sql = "SELECT *,seed.id as seed_id FROM cropping.seed LEFT JOIN cropping.seed_category ON seed_category.id = seed.seed_category_id  ORDER BY general_type,specific_type";
        $res = $GLOBALS ['pdo']->query($sql);
        $seedArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $seedArray [$row ['seed_id']] = $row ['specific_type'] . ' - ' . $row ['name'] . ' - (' . $row ['company'] . ')';
        }

        $numberOfBagsArray = array(null => 'Please Select one...', 1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5, 6 => 6, 7 => 7, 8 => 8, 9 => 9, 10 => 10, 15 => 15, 20 => 20);

        $form = new HTML_QuickForm("addNewSeedInventoryQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'addNewSeedTypeQuickform', 'Add Bag of Seed to Inventory');

        $form->addElement('select', 'seed_id', 'Seed', $seedArray);
        $form->addElement('select', 'number_of_bags', 'Number of Bags', $numberOfBagsArray);
        $form->addElement('date', 'date_bag', 'Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));

        // defaults
        $form->setDefaults(array('date_bag' => 'now')); // set default to today.
        // rules
        $form->addRule('seed_id', 'Seed is required', 'required');
        $form->addRule('number_of_bags', 'Number of bags is required', 'required');
        $form->addRule('date_bag', 'Date is required', 'required');

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $seed_id = $form->exportValue('seed_id');
            $number_of_bags = ($form->exportValue('number_of_bags'));
            $tempArray = $form->getElementValue('date_bag');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do an insert of the lat/long data for the field first and then
                // return a datum id.
                $query = "INSERT INTO cropping.seed_inventory (seed_id,event_time,bag_count,userid) VALUES ($seed_id,'$event_time',$number_of_bags,'{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS ['pdo']->exec($query);
                $last_datum_id = $GLOBALS ['pdo']->lastInsertId('cropping.cropping_id_seq');

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    /**
     * list all seed events
     */
    function displaySeedEventLog($mode = 'default') {

        $sql = <<<SQL
SELECT *,seed_event.id as seed_event_scheduled_id,comment ,name,seed_event.id as delete_table_id, 'seed_event' as delete_table_name, round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id, amount, population  
        FROM cropping.seed_event
        LEFT JOIN cropping.seed ON seed_event.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event.field_id
        LEFT JOIN cropping.datum ON seed_event.datum_id = datum.id
        ORDER BY event_time DESC,alpha_numeric_id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['General Type'] = ($row['general_type']);
            $rowOut['Specific Type'] = ($row['specific_type']);
            $rowOut['Seed Name'] = ($row['name']);
            $rowOut['Crop Year'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Amount'] = ($row['amount']);
            $rowOut['Population'] = ($row['population']);
            $rowOut['Planting Comments'] = ($row['comment']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Seed Event Log</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    /**
     * year must be specified
     * 
     */
    public function displayFieldsForCurrentSeasonPlanting($year, $fieldID = null) {

        if ((($year > 1975) == false) || (($year < 2050) == false)) {
            throw new Exception("Invalid year ($year) for crop planning.");
        }


        //all seeds
        if ($fieldID == null) {
            $sql = <<<SQL
SELECT *,field.id as field_id,seed_event_scheduled.id as seed_event_scheduled_id, seed_event_scheduled.id as delete_table_id, 'seed_event_scheduled' as delete_table_name,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id,event_time,comment,(Select extract('year' from event_time) FROM cropping.seed_event WHERE id=cropping.rotation_establishment_event (field.id, $year)) as rotation,(Select specific_type FROM cropping.seed_event LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id WHERE seed_event.id=cropping.rotation_establishment_event (field.id, $year)) as rotation_crop, seed_event_scheduled.event_time as seed_event_scheduled_event_time
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
        WHERE  seed_event_scheduled.event_time >='$year-01-01' AND seed_event_scheduled.event_time < '$year-12-31'  ORDER BY specific_type,alpha_numeric_id
SQL;
        } else {

            $sql = <<<SQL
SELECT *,field.id as field_id,seed_event_scheduled.id as seed_event_scheduled_id, seed_event_scheduled.id as delete_table_id, 'seed_event_scheduled' as delete_table_name,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id,event_time,comment,(Select extract('year' from event_time) FROM cropping.seed_event WHERE id=cropping.rotation_establishment_event (field.id, $year)) as rotation,(Select specific_type FROM cropping.seed_event LEFT JOIN cropping.seed ON seed.id=seed_event.seed_id WHERE seed_event.id=cropping.rotation_establishment_event (field.id, $year)) as rotation_crop, seed_event_scheduled.event_time as seed_event_scheduled_event_time
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
        WHERE  seed_event_scheduled.event_time >='$year-01-01' AND seed_event_scheduled.event_time < '$year-12-31' AND field.id=$fieldID
SQL;
        }

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['General Type'] = ($row['general_type']);
            $rowOut['Specific Type'] = ($row['specific_type']);
            $rowOut['Crop Year'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Double Crope'] =  ((date('m',strtotime($row['seed_event_scheduled_event_time']))) >=6) ? 'double' : '';
            $rowOut['Rotation'] = ($row['rotation']);
            $rowOut['Rotation Crop'] = ($row['rotation_crop']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Completed?'] = $this->printCompleted($row);
            $rowOut['Action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Planning List for '.$year, $dataArr, 'paging:false,order: false'));
        return implode($out);
    }

    /* print total amount of acres for specifc seed types */

    function findPotentialSeedingSums($year) {
        $sql = "
            With temp as (
SELECT field.id,specific_type,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id
        ,(SELECT true as completed FROM cropping.seed_event WHERE field_id=field.id AND event_time between '$year-01-01' AND '$year-12-31' LIMIT 1) as completed
        FROM cropping.seed_event_scheduled 
        LEFT JOIN cropping.seed ON seed_event_scheduled.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event_scheduled.field_id
        LEFT JOIN cropping.datum ON seed_event_scheduled.datum_id = datum.id
        WHERE  seed_event_scheduled.event_time >='$year-01-01' AND seed_event_scheduled.event_time < '$year-12-31'  ORDER BY alpha_numeric_id
), temp2 as  (
SELECT *,(SELECT temp.area FROM temp xxx WHERE completed=true AND xxx.id=temp.id limit 1) as completed_area FROM temp 
)
Select distinct(specific_type) ,sum(area),sum(completed_area) as completed FROM temp2 GROUP BY specific_type
";
        $res = $GLOBALS ['pdo']->query($sql);

        $str = '';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            if ($row['specific_type'] == 'BMR Corn') {
                $bagNo = round($row['sum'] * (84601 / 80000), 1); // populaton per ha / seed per bag.
                $str = $str . $row['specific_type'] . ': ' . $row['sum'] . ' ha. Total Bags: ' . $bagNo . '. (@ 34k seeds/acre)' . " {$row['completed']} ha done." . ' <br/>';
            } elseif ($row['specific_type'] == 'Corn') {
                $bagNo = round($row['sum'] * (84601 / 80000), 1); // populaton per ha / seed per bag.
                $str = $str . $row['specific_type'] . ': ' . $row['sum'] . ' ha. Total Bags: ' . $bagNo . '. (@ 34k seeds/acre)' . " {$row['completed']} ha done." . ' <br/>';
            } elseif ($row['specific_type'] == 'Alfalfa') {
                $bagNo = round($row['sum'] * (13.5 / 25), 1); // populaton per ha / seed per bag.
                $str = $str . $row['specific_type'] . ': ' . $row['sum'] . ' ha. Total Bags: ' . $bagNo . '. (@ 13.5 kg/ha)' . " {$row['completed']} ha done." . ' <br/>';
            } elseif ($row['specific_type'] == 'Soybean') {
                $bagNo = round($row['sum'] * (370658 / 140000), 1); // (150,000 per acare) populaton per ha / seed per bag.
                $str = $str . $row['specific_type'] . ': ' . $row['sum'] . ' ha. Total Bags: ' . $bagNo . '. (@ 150k seeds/acre)' . " {$row['completed']} ha done." . ' <br/>';
            } elseif ($row['specific_type'] == 'Birdsfoot Trefoil') {
                $bagNo = round($row['sum'] * (11.2 / 25), 1); // (10 lb per acre) populaton per ha / seed per bag.
                $str = $str . $row['specific_type'] . ': ' . $row['sum'] . ' ha. Total Bags: ' . $bagNo . '. (@ 11.2 kg/ha)' . " {$row['completed']} ha done." . ' <br/>';
            } else {
                $str = $str . $row['specific_type'] . ': ' . $row['sum'] . ' ha.<br/>';
            }
        }
        print("<h3>Totals</h3>");
        print($str);
    }

    /* print total amount of different types of spray needed, based on acres and genetic trait,
      assumptions are made omn the chemical used and rate. CORN or SOYBEANS only.
     */

    function findPotentialSprayingTraitSums($year) {


        $sql = "
            With temp as (
SELECT field.id,specific_type,datum.id as datum_id,trait_rr,
round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,(SELECT true as completed FROM cropping.spray_event WHERE field_id=field.id AND event_time between '$year-01-01' AND '$year-12-31' LIMIT 1) as completed
        FROM cropping.seed_event
        LEFT JOIN cropping.seed ON seed_event.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event.field_id
        LEFT JOIN cropping.datum ON seed_event.datum_id = datum.id
        WHERE  seed_event.event_time >='$year-01-01' AND seed_event.event_time < '$year-12-31'  AND (specific_type='Corn' OR specific_type='Soybean') ORDER BY alpha_numeric_id
), temp2 as  (
SELECT *,(SELECT temp.area FROM temp xxx WHERE completed=true AND xxx.id=temp.id limit 1) as completed_area FROM temp 
)
Select distinct(trait_rr),sum(area),sum(completed_area) as completed FROM temp2 GROUP BY trait_rr
";
        $res = $GLOBALS ['pdo']->query($sql);



        $str = '';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            if ($row['trait_rr'] == 't') {
                //Glphosate 540 is 1.67 l/ha rate
                $numJugsTotal = round($row['sum'] * 1.67 / 10, 1);
                $numJugsTotalLeft = ($row['sum'] - $row['completed']) * 1.67 / 10;
                $str = $str . 'Round-up 540 REGISTRATION NO. 27487 ' . $row['sum'] . ' ha. Total Jugs: ' . $numJugsTotal . '. (@ 1.67 l/ha).&nbsp;&nbsp;&nbsp;' . " {$row['completed']} ha done." . ' <br/>';
            } else { //false conventional corn or soybeans, assume never plant conventional soybeans.
                //marksmen for corn
                $numJugsTotal = round($row['sum'] * 4.5 / 10, 1);
                $numJugsTotalLeft = ($row['sum'] - $row['completed']) * 4.5 / 10;
                $str = $str . 'Marksman REGISTRATION NO. 19349 ' . $row['sum'] . ' ha. Total Jugs: ' . $numJugsTotal . '. (@ 4.5 l/ha).&nbsp;&nbsp;&nbsp;' . " {$row['completed']} ha done." . ' <br/>';
            }
        }
        return($str);
    }

    function printFieldArea($record) {

        $res = $GLOBALS ['pdo']->query("SELECT datum_id FROM cropping.border_event WHERE field_id={$record['field_id']} AND event_time <='{$record['event_time']}' ORDER BY event_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row)) {
            if ($row['datum_id'] != $record['datum_id']) {
                return "<b id='boldGreen'>Partial {$record['area']}</b>";
            } else {
                return $record['area'];
            }
        } else {
            return null;
        }
    }

    function printCompleted($record) {


        $yearOfInterest = date('Y', strtotime($record['event_time']));
        $res = $GLOBALS ['pdo']->query("SELECT seed_event.id FROM cropping.seed_event WHERE field_id={$record['field_id']} AND event_time between '$yearOfInterest-01-01' AND '$yearOfInterest-12-31'");
        // $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($res->rowCount() >= 1) {
            return '<b id="boldRed">true</b>';
        } else {
            return 'false';
        }
    }

    // show delete button
    function printActionButtons($record, $callBackURL) {

        $currentUserid = $GLOBALS ['auth']->getUsername();
        $returnButtonsAsString = self::deleteQuickForm($record ['delete_table_id'], $record ['delete_table_name'], $callBackURL); // show
        // delete
        // button
        return $returnButtonsAsString;
    }

    // passing the table name without checking it is a bad idea.trusting the
    // users.....
    function deleteQuickForm($tableid, $tablename, $callBackURL) {

        $form = new HTML_QuickForm("CullEventDeleteButton", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('hidden', 'tablename', $tablename);
        $form->addElement('submit', 'btnDelete', 'Delete',array('class'=>'btn btn-warning'));

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');
            $tablename = $form->getElementValue('tablename');

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // lookup datum and try and delete it.
                $res = $GLOBALS ['pdo']->query("SELECT datum_id FROM cropping.$tablename WHERE id='$tableid' LIMIT 1");
                $row = $res->fetch(PDO::FETCH_ASSOC);
                $datum_id = $row ['datum_id'];
                // now look in border, should be the only place it also is. if it is
                // not there then delete it. //really should be more robust, with
                // triggers, etc.
                $res = $GLOBALS ['pdo']->query("SELECT datum_id FROM cropping.border_event WHERE datum_id='$datum_id' LIMIT 1");
                $row = null;
                $row = $res->fetch(PDO::FETCH_ASSOC);
                if ($row == null) {
                    $res = $GLOBALS ['pdo']->exec("DELETE FROM cropping.datum WHERE id=$datum_id");
                }

                // delete the seeding event.
                $res = $GLOBALS ['pdo']->query("DELETE FROM cropping.$tablename where id='$tableid'");

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }

            header($callBackURL);
            exit();
        }
        return $form->toHtml(); // pass on as html string to display later in the
        // table
    }

    function displayListOfSeeds($mode = 'default') {

        $sql = "SELECT seed.id as seed_id,* FROM cropping.seed LEFT JOIN cropping.seed_category ON seed.seed_category_id = seed_category.id WHERE seed.active=true ORDER BY general_type,specific_type,name";
        $out[] = ("<h3>Seed List</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql, 'order: false'));
        return implode($out);
    }

    /**
     * Adds a new type of seed.
     */
    function addNewSeedTypeQuickform() {

        $sql = "SELECT * FROM cropping.seed_category ORDER BY general_type,specific_type";
        $res = $GLOBALS ['pdo']->query($sql);
        $seedCatArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $seedCatArray [$row ['id']] = $row ['general_type'] . ' - ' . $row ['specific_type'];
        }

        $form = new HTML_QuickForm("addNewSeedTypeQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'addNewSeedTypeQuickform', 'Add a Type of Seed');

        $form->addElement('select', 'seed_category', 'Seed Category', $seedCatArray);
        $form->addElement('text', 'seed_name', 'Seed Name');
        $form->addElement('text', 'company', 'Seed Company');
        $form->addElement('text', 'chu', 'Crop Heat Units');
        $form->addElement('advcheckbox', 'trait_rr', 'RR Trait');
        $form->addElement('advcheckbox', 'trait_bt', 'BT Trait');
        $form->addElement('textarea', 'note', 'Note', array('cols' => 40, 'rows' => 2));

        // defaults
        // rules
        $form->addRule('seed_category', 'Seed Category is required', 'required');
        $form->addRule('seed_name', 'Seed Name is required', 'required');
        $form->addRule('company', 'Company is required', 'required');
        $form->addRule('chu', 'CHU must be a number', 'numeric');

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $seed_category_id = $form->exportValue('seed_category');
            $seed_name = pg_escape_string(trim($form->exportValue('seed_name')));
            $company = pg_escape_string(trim($form->exportValue('company')));
            $chu = pg_escape_string(trim($form->exportValue('chu')));
            $trait_rr = $form->exportValue('trait_rr');
            $trait_bt = $form->exportValue('trait_bt');
            $note = $form->exportValue('note');

            if ($chu == '') {
                $chu = 'null';
            }
            if ($note == '') {
                $note = 'null';
            }

            if ($trait_rr == 't') {
                $trait_rr = 'true';
            } else {
                $trait_rr = 'false';
            }
            if ($trait_bt == 't') {
                $trait_bt = 'true';
            } else {
                $trait_bt = 'false';
            }

            $active = 'true';

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do an insert of the lat/long data for the field first and then
                // return a datum id.
                $query = "INSERT INTO cropping.seed (seed_category_id,name,company,chu,note,active,trait_rr,trait_bt) VALUES ($seed_category_id,'$seed_name','$company',$chu,'$note',$active,$trait_rr,$trait_bt)";
                $res = $GLOBALS ['pdo']->exec($query);
                $last_datum_id = $GLOBALS ['pdo']->lastInsertId('cropping.cropping_id_seq');

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    /**
     * Used for corn/soybean planting, allows planter fertilizer to be entered
     * @param type $type
     * @param type $field_id 
     */
    function addNewCornSoybeanSeedingQuickform($type = 'default', $field_id = null) {
        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        // select active people in manure group to say who can seed, change in future to a seeder group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='manure' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $seederUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $seederUsers [$row ['userid']] = $row ['userid'];
        }

        $sql = "SELECT seed.id as seed_id,* FROM cropping.seed LEFT JOIN cropping.seed_category ON seed.seed_category_id = seed_category.id WHERE seed.active=true AND (seed_category_id=12 OR seed_category_id=2 OR seed_category_id=6760 ) ORDER BY general_type,specific_type,name";
        $res = $GLOBALS ['pdo']->query($sql);
        $seedArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $seedArray [$row ['seed_id']] = $row ['specific_type'] . ': ' . $row ['company'] . '-' . $row ['name'];
        }


        $form = new HTML_QuickForm("addNewCornSoybeanSeedingQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'addNewCornSoybeanSeedingQuickform', 'Add a New Corn/Soybean Seeding');
        if ($field_id != null) {
            $form->addElement('hidden', 'field', $field_id);
        } else {
            $form->addElement('select', 'field', 'Field', $fieldArray);
        }
        $form->addElement('date', 'date_start', 'Seeding Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2030, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
        $form->addElement('select', 'seed_type', 'Seed Type', $seedArray);
        $form->addElement('text', 'seed_amount_pop', 'Seed Population (#/HA)');

        //only show the choice on who did the seeding for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'seeder_userid', 'Who did the seeding', $seederUsers);
        } else {
            $form->addElement('hidden', 'seeder_userid', $GLOBALS['auth']->getUsername());
        }

        //fertilizer
        $typeArray = self::fertilizerTypes();
        $sql = "SELECT id,name FROM cropping.fertilizer_application_method WHERE name='band 2x2' ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $application_methodArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $application_methodArray [$row ['id']] = $row ['name'];
        }
        $form->addElement('select', 'fertilizer_type', 'Type', $typeArray);
        $form->addElement('select', 'application_method', 'Application Method', $application_methodArray);
        $form->addElement('text', 'fertilizer_amount', 'Amount (kg/ha)');



        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));
        $form = self::quickformLinestringFORM($form);

        $form->setDefaults(array('date_start' => 'now', 'use_current_border_polygon' => '1')); // defaults set default to today


        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');


        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Seeding Date Start is required', 'required');

        $form->addRule('seeder_userid', 'Seed User is required', 'required');
        $form->addRule('seed_type', 'Seed Type is required', 'required');
        $form->addRule('seed_amount_pop', 'Seed Seed Population is required', 'required');
        //
        //fertilizer is optional.
        $form->setDefaults(array('fertilizer_type' => 8294, 'application_method' => 3832, 'seed_amount_pop' => 81000, 'fertilizer_amount' => 99));



        // Try to validate a form
        if ($form->validate()) {



            $field_id = $form->exportValue('field');

            $tempArray = $form->getElementValue('date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));

            $seeder_userid = $form->exportValue('seeder_userid');

            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = '';
            }

            $seed_id = $form->exportValue('seed_type');

            $population = round($form->exportValue('seed_amount_pop') * 0.404686); //convert to #/acre for DB store
            //fert 
            $fertilizer_id = $form->exportValue('fertilizer_type');
            $fertilizer_comment = '';
            $application_method_id = $form->exportValue('application_method');
            $fertilizer_amount = $form->exportValue('fertilizer_amount');


            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);

                // do an insert into cropping field and datum of data.
                //insert fertilizer (if not null)
                if ($fertilizer_id != null) {
                    $query = "INSERT INTO cropping.fertilizer_event (field_id,datum_id,event_time,fertilizer_id,userid,comment,fertilizer_application_method_id,amount,fertilizer_userid) VALUES ($field_id,$last_datum_id,'$event_time',$fertilizer_id,'{$GLOBALS['auth']->getUsername()}','$fertilizer_comment',$application_method_id,$fertilizer_amount,'$seeder_userid')";
                    $res = $GLOBALS ['pdo']->exec($query);
                }

                $event_time = date('r', strtotime($event_time) + 504); //add seconds so the date is correct. and so that planting is most recent for that date.
                $query = "INSERT INTO cropping.seed_event (field_id,datum_id,event_time,seed_id,userid,comment,seeder_userid,amount,population) VALUES ($field_id,$last_datum_id,'$event_time',$seed_id,'{$GLOBALS['auth']->getUsername()}','$comment','$seeder_userid',null,$population)";
                $res = $GLOBALS ['pdo']->exec($query);




                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    function displaySeedingInfo() {

        $sql = <<<SQL
With temp as (
SELECT seed_event .id as seed_event_id,seed_event.event_time as seed_event_time, field.id as field_id,alpha_numeric_id,common_name, specific_type,trait_rr,trait_bt,chu,company || ' ' || name as seed_name,population,comment,userid,event_time,round((gis.ST_Area(gis.ST_Transform(geom, 2036))/10000)::numeric,2) as area,datum.id as datum_id  
        FROM cropping.seed_event 
        LEFT JOIN cropping.seed ON seed_event.seed_id=seed.id
        LEFT JOIN cropping.seed_category ON seed_category.id=seed.seed_category_id
        LEFT JOIN cropping.field ON field.id=seed_event.field_id
        LEFT JOIN cropping.datum ON seed_event.datum_id = datum.id
        WHERE seed_event.event_time >= (date_part('year', current_date) || '-01-01')::date AND seed_event.event_time < (date_part('year', current_date) || '-12-31 23:59:59')::timestamp AND (seed_category.specific_type = 'Soybean' OR seed_category.specific_type ='Corn' OR seed_category.specific_type ='BMR Corn')
ORDER BY alpha_numeric_id
) SELECT * FROM temp
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Size (ha)'] = $this->printFieldArea($row);
            $rowOut['Planting Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Crop'] = ($row['specific_type']);
            $rowOut['Seed'] = ($row['seed_name']);
            $rowOut['Population'] = ($row['population']);
            $rowOut['CHU'] = ($row['chu']);
            $rowOut['Roundup?'] = ($row['trait_rr']);
            $rowOut['Corn Borer?'] = ($row['trait_bt']);
            $rowOut['Planting Comments'] = ($row['comment']);
            $rowOut['Employee'] = ($row['userid']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Corn and Soybean Seedings</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    /**
     * Adds a new seeding to a field.
     * This adds the field border and the start time of when it was valid.
     * Fields can have changing borders through time.
     */
    function addNewSeedingToAFieldQuickform($type = 'default', $field_id = null) {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        // select active people in manure group to say who can seed, change in future to a seeder group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='manure' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $seederUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $seederUsers [$row ['userid']] = $row ['userid'];
        }


        if ($type == 'potential') {
            $sql = "SELECT seed.id as seed_id,* FROM cropping.seed LEFT JOIN cropping.seed_category ON seed.seed_category_id = seed_category.id WHERE seed.active=true AND (seed.name='Generic' OR seed.name='generic') ORDER BY general_type,specific_type,name";
        } else {
            $sql = "SELECT seed.id as seed_id,* FROM cropping.seed LEFT JOIN cropping.seed_category ON seed.seed_category_id = seed_category.id WHERE seed.active=true ORDER BY general_type,specific_type,name";
        }
        $res = $GLOBALS ['pdo']->query($sql);
        $seedArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $seedArray [$row ['seed_id']] = $row ['specific_type'] . ' - ' . $row ['name'] . ' type: ' . $row ['general_type'];
        }


        if ($type == 'potential') { // this is used when we want to just add generic corn
            // quickly.
            $form = new HTML_QuickForm("addNewSeedingToAFieldPotentialQuickform_$type", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
            $renderer = new HTML_QuickForm_Renderer_Tableless();

            $form->addElement('header', 'addNewSeedingToAFieldPotentialQuickform', 'Add a New Potential Seeding');
            if ($field_id != null) {
                $form->addElement('hidden', 'field', $field_id);
            } else {
                $form->addElement('select', 'field', 'Field', $fieldArray);
            }
            $form->addElement('select', 'seed_type', 'Seed Type', $seedArray);
            $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));

            $form = self::quickformLinestringFORM($form);
            $form->addElement('html', 'Choose after June 1 for double crop.');
            $form->addElement('date', 'date_start', 'Seeding Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => date("Y")+1, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));
            $form->addElement('hidden', 'seeder_userid', $GLOBALS['auth']->getUsername());
            $form->setDefaults(array('seed_type' => 3713, 'use_current_border_polygon' => '1', 'date_start' => array('d' => '1', 'm' => '1', 'Y' => date('Y', strtotime('now')))));
            $form->addRule('seed_type', 'Seed Type is required', 'required');
        }
        //an actual seeding
        else {
            $form = new HTML_QuickForm("addNewSeedingToAFieldActualQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
            $renderer = new HTML_QuickForm_Renderer_Tableless();

            $form->addElement('header', 'addNewSeedingToAFieldActualQuickform', 'Add a New Seeding');
            if ($field_id != null) {
                $form->addElement('hidden', 'field', $field_id);
            } else {
                $form->addElement('select', 'field', 'Field', $fieldArray);
            }
            $form->addElement('date', 'date_start', 'Seeding Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));

            $seedGroup1 [] = $form->createElement('select', 'seed_type', 'Seed Type', $seedArray);
            $seedGroup1 [] = $form->createElement('text', 'seed_amount_kg', 'Seed Amount kg/ha');

            $form->addGroup($seedGroup1, 'seedGroup1', 'Seed Type', array('<b>amount (kg/ha)</b>', 'OR <b>corn population/ha</b>'));
            //
            $seedGroup2 [] = $form->createElement('select', 'seed_type', 'Seed Type', $seedArray);
            $seedGroup2 [] = $form->createElement('text', 'seed_amount_kg', 'Seed Amount kg/ha');

            $form->addGroup($seedGroup2, 'seedGroup2', 'Seed Type 2', array('<b>amount (kg/ha)</b>', 'OR <b>corn population/ha</b>'));
            //
            $seedGroup3 [] = $form->createElement('select', 'seed_type', 'Seed Type', $seedArray);
            $seedGroup3 [] = $form->createElement('text', 'seed_amount_kg', 'Seed Amount kg/ha');

            $form->addGroup($seedGroup3, 'seedGroup3', 'Seed Type 3', array('<b>amount (kg/ha)</b>', 'OR <b>corn population/ha</b>'));

            // Complex rule for group's elements
            $form->addGroupRule('seedGroup1', array(
                'seed_type' => array(
                    array('Seed Type 1 is required', 'required', null, 'client')
                ),
                'seed_amount_kg' => array(
                    array('Amount 1 must be numeric', 'numeric')
                )
            ));

            // Complex rule for group's elements
            $form->addGroupRule('seedGroup2', array(
                'seed_amount_kg' => array(
                    array('Amount 2 must be numeric', 'numeric')
                )
            ));

            // Complex rule for group's elements
            $form->addGroupRule('seedGroup3', array(
                'seed_amount_kg' => array(
                    array('Amount 3 must be numeric', 'numeric')
                )
            ));


            //only show the choice on who did the seeding for admin users.
           if ($GLOBALS['auth']->getOwnerAccess() == 1) {
                $form->addElement('select', 'seeder_userid', 'Who did the seeding', $seederUsers);
            } else {
                $form->addElement('hidden', 'seeder_userid', $GLOBALS['auth']->getUsername());
            }

            $form->addElement('textarea', 'comment', 'Comment', array('cols' => 40, 'rows' => 2));
            $form = self::quickformLinestringFORM($form);

            $form->setDefaults(array('date_start' => 'now', 'use_current_border_polygon' => '1')); // defaults set default to today
        }

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');


        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('date_start', 'Seeding Date Start is required', 'required');

        $form->addRule('seeder_userid', 'Seed User is required', 'required');



        // Try to validate a form
        if ($form->validate()) {

            $seed_idArr = array();

            $field_id = $form->exportValue('field');

            $tempArray = $form->getElementValue('date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));

            $seeder_userid = $form->exportValue('seeder_userid');

            $comment = pg_escape_string(trim($form->exportValue('comment')));
            if ($comment == '') {
                $comment = '';
            }

            if ($type == 'potential') {
                $seed_id = $form->exportValue('seed_type');
            } else {
                $seedGroup1 = $form->getElementValue('seedGroup1');
                $seedGroup2 = $form->getElementValue('seedGroup2');
                $seedGroup3 = $form->getElementValue('seedGroup3');

                //we need to check if there was multiple seedings selected or not.
                $seed_id = $seedGroup1['seed_type'][0];
                $seed_idArr[1]['type'] = $seedGroup1['seed_type'][0];
                if ($seedGroup2['seed_type'] != '') {
                    $seed_idArr[2]['type'] = $seedGroup2['seed_type'][0];
                }
                if ($seedGroup3['seed_type'] != '') {
                    $seed_idArr[3]['type'] = $seedGroup3['seed_type'][0];
                }
                //
                if ($seedGroup1['seed_amount_kg'] != '') {
                    $seed_idArr[1]['kg'] = $seedGroup1['seed_amount_kg'];
                } else {
                    $seed_idArr[1]['kg'] = 'null';
                }
                if ($seedGroup2['seed_amount_kg'] != '') {
                    $seed_idArr[2]['kg'] = $seedGroup2['seed_amount_kg'];
                } else {
                    $seed_idArr[2]['kg'] = 'null';
                }
                if ($seedGroup3['seed_amount_kg'] != '') {
                    $seed_idArr[3]['kg'] = $seedGroup3['seed_amount_kg'];
                } else {
                    $seed_idArr[3]['kg'] = 'null';
                }
            }

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                // do insert of new polygon data and get the datum_id for it after
                // insert.
                $last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);

                // do an insert into cropping field and datum of data.
                if ($type == 'potential') {
                    $query = "INSERT INTO cropping.seed_event_scheduled (field_id,datum_id,event_time,seed_id,userid,comment) VALUES ($field_id,$last_datum_id,'$event_time',$seed_id,'{$GLOBALS['auth']->getUsername()}','$comment')";
                    $res = $GLOBALS ['pdo']->exec($query);
                } else {


                    $event_time = date('r', strtotime($event_time) + 504); //add seconds so the date is correct. and so that planting is most recent for that date.
                    foreach ($seed_idArr as $key => $value) {

                        if ($value['type'] != null) {
                            $query = "INSERT INTO cropping.seed_event (field_id,datum_id,event_time,seed_id,userid,comment,seeder_userid,amount,population) VALUES ($field_id,$last_datum_id,'$event_time',{$value['type']},'{$GLOBALS['auth']->getUsername()}','$comment','$seeder_userid',{$value['kg']},0)";
                            $event_time = date('r', strtotime($event_time) - 1); //remove 1 second everytime through the loop, a hack to make the seedings not to all be at the same time.
                            $res = $GLOBALS ['pdo']->exec($query);
                        }
                    }
                }

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        return ($renderer->toHtml());
    }

    /**
     * add new field border
     * This adds the field border and the start time of when it was valid.
     *
     * Fields can have changing borders through time.
     */
    function addNewFieldBorderQuickform($type = 'default', $field_id = null) {

        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        $form = new HTML_QuickForm("addNewFieldBorderQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'addNewFieldBorderQuickform', 'Add a New Field Border');
        if ($field_id != null) {
            $form->addElement('hidden', 'field', $field_id);
        } else {
            $form->addElement('select', 'field', 'Field', $fieldArray);
        }
        $form->addElement('date', 'border_date_start', 'Border Start Date', array('format' => 'Y-m-d', 'minYear' => 1975, 'maxYear' => 2021, 'addEmptyOption' => TRUE, 'emptyOptionText' => array('m' => 'Month', 'd' => 'Day', 'Y' => 'Year')));

        $form = self::quickformLinestringFORM($form);

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        $form->setDefaults(array('border_date_start' => 'now')); // set default
        // to today.
        // rules
        $form->addRule('field', 'Field is required', 'required');
        $form->addRule('border_date_start', 'Border Date Start is required', 'required');
        $form->addRule('polygon', 'Field Polygon is required', 'required');

        // custom rule to check polygon input is of the correct format
        $form->registerRule('areaValidRule', 'function', 'checkAreaIsValid', 'CroppingHelper');
        $form->addRule(array('polygon'), 'ERROR: Area has to be of form "num,num,num OR linestring AND closed loop.".', 'areaValidRule', array('polygon'));

        // Try to validate a form
        if ($form->validate()) {

            $field_id = $form->exportValue('field');
            $linestring = (new CroppingHelper)->kmlArrayToLinestringStr((new CroppingHelper)->convertLineStringORNumNumNum($form->getElementValue('polygon')));
            $tempArray = $form->getElementValue('border_date_start');
            $event_time = date('Y-m-d', strtotime($tempArray ['Y'] [0] . '-' . $tempArray ['m'] [0] . '-' . $tempArray ['d'] [0]));


            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();


                $last_datum_id = Field::quickformLinestringVALIDATE($form, $event_time, true);




                // do an insert into cropping field and datum of data.
                $query = "INSERT INTO cropping.border_event (field_id,datum_id,event_time,userid) VALUES ($field_id,$last_datum_id,'$event_time','{$GLOBALS['auth']->getUsername()}')";
                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
            // Close the transaction

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    /**
     * *********************************
     * Add a new field
     * ********************************
     */
    function addNewFieldQuickform() {

        // take all the major names used already and use them as choices.
        $sql = "SELECT distinct major_name FROM cropping.field ORDER BY major_name";
        $res = $GLOBALS ['pdo']->query($sql);
        $majorNameArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $majorNameArray [$row ['major_name']] = $row ['major_name'];
        }

        $form = new HTML_QuickForm("addNewFieldQuickform", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();


        $form->addElement('header', 'addNewFieldQuickform', 'Add a New Field');
        $form->addElement('text', 'alpha_numeric', 'Field Alpha Numeric Name');
        $form->addElement('textarea', 'description', 'Short Description', array('cols' => 40, 'rows' => 1));
        $form->addElement('select', 'major_name', 'General Name', $majorNameArray);

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // rules
        $form->addRule('alpha_numeric', 'alpha_numeric is required', 'required');
        $form->addRule('description', 'description is required', 'required');
        $form->addRule('major_name', 'major_name is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            // get form values
            $alpha_numeric_id = pg_escape_string($form->exportValue('alpha_numeric'));
            $common_name = pg_escape_string($form->exportValue('description'));
            $major_name = pg_escape_string($form->exportValue('major_name'));

            // do an insert into cropping field and datum of data.
            $query = "INSERT INTO cropping.field (alpha_numeric_id,common_name,major_name) VALUES ('$alpha_numeric_id','$common_name','$major_name')";
            $res = $GLOBALS ['pdo']->exec($query);

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&field_id={$_REQUEST['field_id']}");
            exit();
        } // end validation

        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function detailsQuickForm($fieldID) {

        $form = new HTML_QuickForm("formDetailsButton", '', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('hidden', 'id', 5);
        $form->addElement('hidden', 'fieldID', $fieldID);
        $form->addElement('submit', 'btnDetails', 'Details');

        // Try to validate a form
        if ($form->validate()) {

            // get values
            $id = $form->exportValue('id');
            $fieldID = $form->getElementValue('fieldID');

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?id={$_REQUEST['id']}&fieldID=$fieldID");
            exit();
        }
        $form->accept($renderer);
        echo($renderer->toHtml());
    }

    function displayAllActiveFieldsList($mode = 'default') {

        $sql = <<<SQL
   SELECT *
FROM cropping.fieldcurr 
WHERE active=true ORDER BY alpha_numeric_id
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['ID Name'] = ($row['id']);
            $rowOut['Size (Ha)'] = ($row['area']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ("<h3>Active Field List</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

 
    function printFormattedArea($record) {

        // return area in ha
        return round($GLOBALS['MiscObj']->areaLatLonInHectares($record ['perimeter_coordinates']), 2);
    }

}

// end class

Class QF2_SprayEvent extends QF2_Base {

  

    function definition($form) {


        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

        $sql = "SELECT id,name FROM cropping.spray ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $typeArray [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $typeArray [$row ['id']] = $row ['name'];
        }


        // select active people in manure group to say who can spray, change in future to a seeder group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE groupid='spray' AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $sprayUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $sprayUsers [$row ['userid']] = $row ['userid'];
        }

      

        ////

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('fieldSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($fieldArray);
        } else {
            $form->addElement('hidden', 'fieldSelected')->setValue($this->param->field_id);
        }
        $date = $form->addElement('date', 'dateEvent', null, array('label' => 'Spray Date', 'format' => 'd-F-Y', 'minYear' => date('Y'), 'maxYear' => 2030));
       

        $form->addElement('select', 'spray_type', null, array('options' => $typeArray, 'label' => 'Spray Type'))->addRule("required", "Please specify your Spray Type");
        $form->addElement('text', 'spray_amount', null, array( 'label' => 'Chemical Amount (l/hectare)'))->addRule("required", "Please specify your Spray Amount");

        $form->addElement('textarea', 'comment', array('rows' => 2, 'cols' => 20))->setLabel('Comment');

        //only show the choice on who did the spray for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'spray_userid', null, array('options' => $sprayUsers, 'label' => 'Who did the spraying'))->addRule("required", "Please specify who did the srpaying");
        } else {
            $form->addElement('hidden', 'spray_userid')->setValue($GLOBALS['auth']->getUsername());
        }


        //default
          $current_date=$date = strtotime(date('Y-m-d'));
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array( 'dateEvent' => $current_date))); //set default date to today.

        return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
        $jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Field(s)",
    tags: false
});   
      </script>      
JSCRIPT;

        return $jscript;
    }

    function process($value) {



        $value['event_time'] = date('Y-m-d H:i:sO', strtotime($value['dateEvent']['d'] . '-' . $value['dateEvent']['F'] . '-' . $value['dateEvent']['Y']));
        $comment = pg_escape_string(trim($value['comment']));





        if ($comment == '') {
            $comment = null;
        }

        // Open a transaction
        try {
            $res = $GLOBALS ['pdo']->beginTransaction();




            // do insert of new polygon data and get the datum_id for it after
            // insert.
            //$last_datum_id = self::quickformLinestringVALIDATE($form, $event_time);




            if (is_array($value['fieldSelected'])) {
                //group
                foreach ($value['fieldSelected'] as $k => $v) {

                    $field_id = $v;
                    $query = "INSERT INTO cropping.spray_event (field_id,datum_id,event_time,spray_id,userid,comment,sprayer_userid,amount) VALUES ($field_id, cropping.field_border_at_timestamp ($field_id, '{$value['event_time']}'),'{$value['event_time']}',{$value['spray_type']},'{$GLOBALS['auth']->getUsername()}','$comment','{$value['spray_userid']}',{$value['spray_amount']})";
                    $res = $GLOBALS ['pdo']->exec($query);
                }
            }
            //individual case
            else {
                // print("hi");
                // exit();
                //$query = "INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ({$value['fieldSelected']},'{$value['event_time']}',{$value['newLocation']},'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                //$res = $GLOBALS['pdo']->exec($query);
            }


            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

}

//end class   


Class QF2_LimeEvent extends QF2_Base {

    
      
    function definition($form) {


        $fieldArray = (new CroppingHelper)->allActiveFieldsArray();

       
        
          // type of lime
        $sql = "SELECT id,name FROM cropping.lime ORDER BY name";
        $res = $GLOBALS ['pdo']->query($sql);
        $limeType [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $limeType [$row ['id']] = $row ['name'];
        }


        // select active people in manure group to say who can lime, change in future to a seeder group if necessary
        $sql = "Select users_in_groups.userid FROM wcauthentication.users_in_groups Left Join wcauthentication.users ON users.userid=users_in_groups.userid WHERE (groupid='manure' OR groupid='custom_cropping') AND users.active=true";
        $res = $GLOBALS ['pdo']->query($sql);
        $limeUsers [null] = "Please Select one..."; // dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $limeUsers [$row ['userid']] = $row ['userid'];
        }

          $limeAmount = array(null => 'Please Select one...', '1' => '1', '1.25' => '1.25', '1.5' => '1.5', '1.75' => '1.75', '2' => '2', '2.25' => '2.25', '2.5' => '2.5', '2.75' => '2.75', '3' => '3', '3.25' => '3.25', '3.5' => '3.5', '3.75' => '3.75', '4' => '4', '4.25' => '4.25', '4.5' => '4.5', '4.75' => '4.75', '5' => '5', '5.25' => '5.25', '5.5' => '5.5', '5.75' => '5.75', '6' => '6', '6.25' => '6.25', '6.5' => '6.5', '6.75' => '6.75', '7' => '7','8' => '8','9' => '9','10' => '10');

        
        
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('fieldSelected', array('multiple' => 'multiple', 'id' => 'selectEventsLime'))->loadOptions($fieldArray);
        } else {
            $form->addElement('hidden', 'fieldSelected')->setValue($this->param->field_id);
        }
        $date = $form->addElement('date', 'dateEvent', null, array('label' => 'Lime Date', 'format' => 'd-F-Y', 'minYear' => date('Y'), 'maxYear' => 2030));
        $date->setValue(date('Y-m-d', strtotime('now')));

        $form->addElement('select', 'lime_type', null, array('options' => $limeType, 'label' => 'Lime Type'))->addRule("required", "Please specify your Lime Type");
        $form->addElement('select', 'lime_amount', null, array('options' => $limeAmount, 'label' => 'Amount (tonne/hectare)'))->addRule("required", "Please specify your Lime Amount");

        $form->addElement('textarea', 'comment', array('rows' => 2, 'cols' => 20))->setLabel('Comment');

        //only show the choice on who did the lime for admin users.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $form->addElement('select', 'lime_userid', null, array('options' => $limeUsers, 'label' => 'Who did the limeing'))->addRule("required", "Please specify who did the limeing.");
        } else {
            $form->addElement('hidden', 'lime_userid')->setValue($GLOBALS['auth']->getUsername());
        }



        return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
        $jscript = <<<JSCRIPT
 <script>           
var s55 = $("#selectEventsLime").select2({
    placeholder: "Choose Field(s)",
    tags: false
});   
      </script>      
JSCRIPT;

        return $jscript;
    }

    function process($value) {


        $value['event_time'] = date('Y-m-d H:i:sO', strtotime($value['dateEvent']['d'] . '-' . $value['dateEvent']['F'] . '-' . $value['dateEvent']['Y']));
        $comment = pg_escape_string(trim($value['comment']));

        if ($comment == '') {
            $comment = null;
        }

        // Open a transaction
        try {
            $res = $GLOBALS ['pdo']->beginTransaction();


            if (is_array($value['fieldSelected'])) {
                //group
                foreach ($value['fieldSelected'] as $k => $v) {

                    $field_id = $v;
 $sql = <<<SQL
INSERT INTO cropping.lime_event (field_id,datum_id,event_time,lime_id,userid,comment,spreader_userid,lime_amount) 
    VALUES ($field_id, cropping.field_border_at_timestamp($field_id, '{$value['event_time']}'),'{$value['event_time']}',{$value['lime_type']},'{$value['lime_userid']}','$comment','{$value['lime_userid']}',{$value['lime_amount']})
SQL;                   
               
                    $res = $GLOBALS ['pdo']->exec($sql);
                }
            }
            //individual case
            else {
               throw new Exception("Individual case not supported yet.");
            }

            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }
    }

}

//end class   