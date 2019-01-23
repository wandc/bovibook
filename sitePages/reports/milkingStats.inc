<?php
/*
  //SQL TO FIND WHAT WE SHOULD BE SHIPPING. LOCATION has to be at the time of milking to be accurate.
  SELECT sum(milkyield) as group_milkyield,round(avg(milkyield),2) as groupavg_milkyield,date_trunc('day',avg(date_trunc('day',current_date-bovinecurr.fresh_date))) as avg_dim,'all shipping' as location_name,((SELECT max(date) FROM alpro.cow)-interval '1 day' ) as date,'am' as milking_time
  FROM alpro.cow
  LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id = cow.bovine_id
  WHERE (location_id=40 OR location_id=41 OR location_id=42) AND date=((SELECT max(date) FROM alpro.cow)-interval '1 day' ) AND milking_number=1
  UNION
  SELECT sum(milkyield) as group_milkyield,round(avg(milkyield),2) as groupavg_milkyield,date_trunc('day',avg(date_trunc('day',current_date-bovinecurr.fresh_date))) as avg_dim,'all shipping' as location_name,((SELECT max(date) FROM alpro.cow)-interval '1 day' ) as date,'pm' as milking_time
  FROM alpro.cow
  LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id = cow.bovine_id
  WHERE (location_id=40 OR location_id=41 OR location_id=42) AND date=((SELECT max(date) FROM alpro.cow)-interval '1 day' ) AND milking_number=2


 */


class MilkingStats extends TabPage {

    public function defaultDisplay() {
        //setup for tabs.
        $tabArray[1]['name'] = 'Individual Stats (Parlor)';
        $tabArray[2]['name'] = 'Group Stats (Valacta)';
        $tabArray[3]['name'] = 'Group Stats (Parlor)';
        $tabArray[4]['name'] = 'Herd Stats (NB Lab)';
        $tabArray[5]['name'] = 'Saleable Milk Stats (Parlor)';
        $tabArray[6]['name'] = 'Milked Yet this Morning';
        $this->renderTabs($tabArray);
    }

    function tab6() {
        $this->displayNotMilkedYetThisAm();
    }

    function tab1() {
       
        print("<h3> Single Stats</h3>");
        print($this->displayAverageAgeOfFirstCalving());
        print($this->displayFirstBreedingStats());
      //
        print($this->displayAggregateStats());    
        print($this->displayListOfLatestMilkYields());
        print($this->displayListOfDailyRevenue());
       
    }

    function tab2() {
        $this->showChartValactaDataByGroupTempTable(); //create temp table
        //
          
        print("<h1>High</h1>");
        print('<div id="twoColumnLayout">');
        print($this->displayValactaStatsByGroup(40));
        print("<br/>");
        print($this->displayMilkingStatsByGroup(40));
        print($this->showChartValactaDataByGroup(40, 'High'));
        $this->displayLastValactaMUNByGroup(40);
        print('</div>');
        print("<br/><hr/><br/>");

        print("<h1>Heifer</h1>");
        print('<div id="twoColumnLayout">');
        print($this->displayValactaStatsByGroup(42));
        print("<br/>");
        print($this->displayMilkingStatsByGroup(42));
        $this->showChartValactaDataByGroup(42, 'Heifer');
        $this->displayLastValactaMUNByGroup(42);
        print('</div>');
        print("<br/><hr/><br/>");

        print("<h1>Low</h1>");
        print('<div id="twoColumnLayout">');
        print($this->displayValactaStatsByGroup(41));
        print("<br/>");
        print($this->displayMilkingStatsByGroup(41));
        $this->showChartValactaDataByGroup(41, 'Low');
        $this->displayLastValactaMUNByGroup(41);
        print('</div>');
        print("<br/><hr/><br/>");


        print('</div>');
    }

    function tab3() {

        /* pulls out milk yields per cow per day for each group (last 90 days) */
        $sql0 = "
                  CREATE TEMP TABLE group_milk AS
                  (
 with temp as (
SELECT date,milking_number,1 as c, bovine_id,milkyield,milktime,bovinemanagement.location_id_at_timestamp(bovine_id,milktime) as location_id FROM alpro.cow
WHERE date > current_date - interval '90 days' AND  date < current_date
)
,temp2 as ( 
SELECT distinct ( location_id) location_id, (SELECT name FROM bovinemanagement.location WHERE id=location_id),date, milking_number,
(sum(milkyield) over (PARTITION BY  location_id,milking_number,date)  ) as milkyield,    
count(c) over (PARTITION BY  location_id,milking_number,date) FROM temp order by location_id          
),temp3 as (
SELECT *,milkyield/count as milk_per_cow from temp2
), temp4 as (
SELECT *,sum(milk_per_cow) over (PARTITION BY  location_id,date)  as milk_per_cow_day_total  from temp3
)
select date,name,location_id,milk_per_cow,
(SELECT milk_per_cow FROM temp4  as n2 where n2.date=n1.date AND n1.location_id=n2.location_id AND n2.milking_number=1 ) as milk_per_cow_am,
(SELECT milk_per_cow FROM temp4  as n2 where n2.date=n1.date AND n1.location_id=n2.location_id AND n2.milking_number=2 ) as milk_per_cow_pm,
(SELECT milkyield FROM temp4  as n2 where n2.date=n1.date AND n1.location_id=n2.location_id AND n2.milking_number=1 ) as milk_yield_am,
(SELECT milkyield FROM temp4  as n2 where n2.date=n1.date AND n1.location_id=n2.location_id AND n2.milking_number=2 ) as milk_yield_pm,
(SELECT count FROM temp4  as n2 where n2.date=n1.date AND n1.location_id=n2.location_id AND n2.milking_number=1 ) as count_am,
(SELECT count FROM temp4  as n2 where n2.date=n1.date AND n1.location_id=n2.location_id AND n2.milking_number=2 ) as count_pm, milk_per_cow_day_total
from temp4 as n1 
)
                     ";
        $res0 = & $GLOBALS['pdo']->exec($sql0);

        print("<h2>Milk Production by Group</h2>");

        $this->drawGroupMilk(40);
        $this->drawGroupMilk(42);
        $this->drawGroupMilk(41);
    }

    function tab4() {
       
        $this->showChartOfProvincialFatTest();

    }

    function tab5() {

        //FIXME, make this a chart, see's how many cows were milked.
        $xxx = "
  with temp as (
SELECT date,milking_number,1 as c, bovine_id,milkyield,milktime,bovinemanagement.location_id_at_timestamp(bovine_id,milktime) as location_id FROM alpro.cow
WHERE date='2014-1-2'::date AND milking_number=1
)
SELECT distinct ( location_id) location_id, (SELECT name FROM bovinemanagement.location WHERE id=location_id),date, milking_number,count(c) over (PARTITION BY  location_id) FROM temp order by location_id          
";



        /* finds saleable milk production by milking number */
        $sql = "
with temp as (
SELECT date_trunc('day', dd):: date
FROM generate_series
        ((current_date - interval '2.5 month')::date
        , current_date
        , '1 day'::interval) dd
), temp2 as (
SELECT date_trunc, alpro.alpro_production_saleable_milk (date_trunc, 1) as one ,alpro.alpro_production_saleable_milk (date_trunc, 2)  as two From temp
)
SELECT * FROM temp2 where one is not null and two is not null
";

        ///////////////
        $res2 = $GLOBALS['pdo']->query($sql);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $t = strtotime($row2['date_trunc']);
            $one[$t] = round($row2['one']);
            $two[$t] = round($row2['two']);
        }

        print("<h3>Saleable Milk by Date</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['AM'] = 'number';
        $headerStrArr['PM'] = 'number';

        $x = new GoogleVisualizationLine(null, $headerStrArr, $one, $two);
        print($x->toString());
    }
    
    /*
     * depends on sync being run, not robust at all, just quick bit of code to see if we missed a cow.
     * 
     */

    public function notMilkedYetThisAm($request) {

        $sql = "
 with temp as (
SELECT *, (SELECT milktime from alpro.cow where bovine_id=id and milking_number=1 and milktime>current_date  limit 1)
 from bovinemanagement.bovinecurr where location_id = any ('{39,40,41,42}'::int[]) 
)
select * from temp where milktime is null order by location_id,local_number          
";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['id'], $row['local_number'], $row['full_name']);
            // $row['location_name'];
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayNotMilkedYetThisAm() {
        print("<h3>Note: This is only good for am milking, and it relies on delaval sync being done.</h3>");
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_name",
                title: "Current Location"
           }
EX;
        print($dt->start(73, 'notMilkedYetThisAm', null, $columnsStr));
    }

    function drawGroupMilk($location_id) {

        $sql1 = "SELECT name FROM bovinemanagement.location WHERE id=$location_id limit 1";
        $res1 = & $GLOBALS['pdo']->query($sql1);
        $row1 = $res1->fetch(PDO::FETCH_ASSOC);

        print("<h3>Group: {$row1['name']}</h3>");


        $sql = "SELECT * FROM group_milk WHERE location_id=$location_id ORDER by date"; //low=41, heif=42, high=40


        $res = & $GLOBALS['pdo']->query($sql);
        $str = '';
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (trim($row['milk_per_cow_am']) == '') {
                $row['milk_per_cow_am'] = 0;
            } //check for blank data.
            if (trim($row['milk_per_cow_pm']) == '') {
                $row['milk_per_cow_pm'] = 0;
            }
            isset($row['milk_per_cow_am']) ? $row['milk_per_cow_am'] : 0;
            isset($row['milk_per_cow_pm']) ? $row['milk_per_cow_pm'] : 0;

            $str = $str . "['{$row['date']}', {$row['milk_per_cow_am']}, {$row['milk_per_cow_pm']}],\n\r";
        }
        ?>
        <div id='chart_<?php echo($location_id); ?>' ></div>   

        <script type='text/javascript'>

            setTimeout(drawChart_<?php echo($location_id); ?>, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart_<?php echo($location_id); ?>, true);
            $(window).resize(function () {
                google.charts.setOnLoadCallback(drawChart_<?php echo($location_id); ?>);
            });
            function drawChart_<?php echo($location_id); ?>() {
                var data = google.visualization.arrayToDataTable([
                    ['Genre', 'am', 'pm'],
        <?php echo($str); ?>
                ]);

                var options = {
                    width: 1200,
                    height: 400,
                    legend: {position: 'top', maxLines: 3},
                    bar: {groupWidth: '75%'},
                    isStacked: true,
                };

                var chart = new google.visualization.ColumnChart(document.getElementById('chart_<?php echo($location_id); ?>'));
                chart.draw(data, options);
            }
            drawChart_<?php echo($location_id); ?>();
        </script>

        <?php
    }

    

    /*
     * 
     * MUN
     * 
     */

    function displayLastValactaMUNByGroup($location_id) {

        $sql1 = "SELECT name FROM bovinemanagement.location WHERE id=$location_id limit 1";
        $res1 = & $GLOBALS['pdo']->query($sql1);
        $row1 = $res1->fetch(PDO::FETCH_ASSOC);

        print("<h3>MUN Group: {$row1['name']}</h3>");

        $query2 = "with temp as (
SELECT local_number,round(mun/10,1) as mun,days_in_mi as dim_test_date,total_milk,fat_per,total_milk*fat_per/100 as kg,bovinemanagement.location_id_at_timestamp (bovine.id, test_date) as location_id
 FROM bovinemanagement.bovine 
LEFT JOIN batch.valacta_data_latest_test_view ON bovine.local_number = valacta_data_latest_test_view.chain 
)
SELECT * from temp WHERE location_id=$location_id AND mun is not null";


        ///////////////
        $res2 = $GLOBALS['pdo']->query($query2);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $dimArr[] = ($row2['dim_test_date']);
            $munArr[] = round($row2['mun'], 2);
        }

        $headerStrArr = array();
        $headerStrArr['DIM'] = 'number'; //BACKWARDS
        $headerStrArr['MUN'] = 'number';
        $x = new GoogleVisualizationScatter(null, $headerStrArr, $dimArr, $munArr);
        print($x->toString());
    }

    function displayFirstBreedingStats() {
        $sql = "With temp as (
SELECT id,bovine_id,fresh_date,(SELECT event_time FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=lactation.bovine_id AND combined_breeding_embryo_view.event_time > lactation.fresh_date ORDER BY combined_breeding_embryo_view.event_time ASC LIMIT 1) as first_breeding_date
FROM bovinemanagement.lactation WHERE fresh_date >= '2011-01-01'
)
Select date_trunc('days',avg(first_breeding_date-fresh_date)) as avg_first_breeding FROM temp WHERE first_breeding_date is not null";
        
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return '<p> Current average age to first breeding: '. $row['avg_first_breeding'].'.</p>';
    }

    function displayListOfDailyRevenue($mode = 'default') {
 $out[]=("<h3>Yesterday's Milk Revenue by Cow</h3>");

         $sql = <<<SQL
With temp as (
SELECT bovinecurr.id,batch.calculate_milk_revenue_per_cow_per_date(bovinecurr.id, (CURRENT_DATE - interval '1 day')::date) as milk_revenue,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,location_name FROM bovinemanagement.bovinecurr 
)
SELECT bovine.id as bovine_id,full_name,local_number,round(milk_revenue,2) as milk_revenue,temp.dim,temp.location_name FROM temp 
LEFT JOIN bovinemanagement.bovine ON temp.id=bovine.id
WHERE milk_revenue is not null ORDER BY milk_revenue ASC
SQL;
         
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           //  $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.      
               $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               $rowOut['Location']=$row['location_name'];
               $rowOut['DIM']=$row['dim'];   
               $rowOut['Milk Revenue'] = round($row['milk_revenue'],2);                                 
          
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
         return implode($out); 
    }

    function displayAverageAgeOfFirstCalving() {

//find group stats
        $sql = "
WITH temp as (
SELECT DISTINCT ON (bovine_id) bovine_id,bovinemanagement.due_date(bovine_id),min(fresh_date) as first_calving_date,birth_date,local_number 
FROM bovinemanagement.lactation 
LEFT JOIN bovinemanagement.bovine ON bovine.id=lactation.bovine_id
GROUP BY bovine_id,fresh_date,birth_date,local_number)
, temp2 as (
SELECT *,birth_date-first_calving_date as age_at_first_calving FROM temp WHERE first_calving_date >= (now()- interval '12 months')
)
SELECT date_part('epoch',avg(age_at_first_calving))::int as average FROM temp2 
";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return("Average Age of first calving in last year: " . round(abs($row['average'] / 2592000), 2) . " months.<br/>");
    }

//finds the time of fist attach and last attach plus milking time to give total milking time.
    function displayMilkingTim() {

        /*
         * SLOW, maybe put this in a batch process.
         * 
          SELECT DISTINCT ON (cow.date) cow.date
          ,(SELECT min(milktime) FROM alpro.cow as xx1 WHERE date=cow.date AND milking_number=1) as am_start
          ,(SELECT max(milktime) FROM alpro.cow as xx2 WHERE date=cow.date AND milking_number=1) as am_finish
          FROM alpro.cow

         */
    }

    public function aggregateStats($mode = 'default') {
     
//show stats only for last week.
   $sql = <<<SQL
With temp_table as (
SELECT * FROM alpro.cow WHERE milktime >= (current_timestamp - interval '7 days')
), temp2 as (
SELECT local_number,full_name,bovinecurr.id as bovine_id, round(sum((manualid)::int)/count(manualid)::numeric*100) as manualid_perc, 
		round(sum((manualkey)::int)/count(manualkey)::numeric*100) as manualkey_perc, 
		round(sum((reattatch)::int)/count(reattatch)::numeric*100) as reattatch_perc FROM temp_table
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id = temp_table.bovine_id
GROUP BY local_number,full_name,bovinecurr.id,bovinecurr.location_name

) SELECT *, (manualid_perc+manualkey_perc+reattatch_perc) as manual_sum
FROM temp2 
where manualid_perc+manualkey_perc+reattatch_perc  !=0
ORDER BY local_number ASC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           //  $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.      
               $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               $rowOut['Manual ID %']=$row['manualid_perc'];
               $rowOut['Manual Milk Key %']=$row['manualkey_perc'];   
               $rowOut['Reattatch %'] = $row['reattatch_perc'];   
               
            if (($row['manual_sum']) >=40) {
                $rowOut['colour_row'] = 1;
            } else {
                $rowOut['colour_row'] = 0;
            }
                
           $resultsOut[]=$rowOut;  
        }
           // print_r($resultsOut);
        //output
        $dataArr['data'] = $resultsOut;
      
      return $dataArr;
        
    }

    function  displayAggregateStats() {     
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                 
            {
                data: "Animal",
                title: "Animal"
            },
            {
                data: "Manual ID %",
                title: "Manual ID %"
            },
            {
                data: "Manual Milk Key %",
                title: "Manual Milk Key %"
            },
            {
                data: "Reattatch %",
                title: "Reattatch %"
            }
EX;
        $out[]=("<h3>Aggregate Milking Stats For Last 7 Days</h3>");
        $out[]=($dt->start($this->pageid, 'aggregateStats', null, $columnsStr, 'highlight_important', ' paging: false'));
        return (implode($out));
    } 
    
    
    
    function displayMilkingStatsByGroup($location_id, $mode = 'default') {
//find group stats
        $sql = "SELECT sum(milkyield) as group_milkyield,round(avg(milkyield),2)*2 as group_avg_daily_milkyield,date_trunc('day',avg(date_trunc('day',current_date-bovinecurr.fresh_date))) as avg_dim,max(location_name) as location_name,
round(AVG(current_date - bovinecurr.birth_date)/30.4167) as avg_age_months,
date_trunc('day',sum(current_date - ((bovinemanagement.due_date(bovinecurr.id) - '280 days 12:00:00'::interval)))/count(bovinecurr.id)) as ave_age_gestation_days
                FROM alpro.cow 
                LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id = cow.bovine_id
		WHERE location_id=$location_id AND date=(SELECT max(date)  FROM alpro.cow) AND milking_number=(SELECT max(milking_number) FROM alpro.cow WHERE date=(SELECT max(date) FROM alpro.cow)) LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $row['group_milkyield'] = round($row['group_milkyield'], 2);
        if ($row['ave_age_gestation_days'] == null) {
            $row['ave_age_gestation_days'] = 0;
        }
        if ($mode == 'default') {
            return("Last Parlor Milking Stats for {$row['location_name']}: {$row['group_milkyield']} litres group total, average dim {$row['avg_dim']}, per cow {$row['group_avg_daily_milkyield']} average liters., average age {$row['avg_age_months']} (months), average age of gestation {$row['ave_age_gestation_days']} days<br/>");
        } elseif ($mode == 'nutrition') {
            return("Last Parlor Milking Stats for {$row['location_name']}: average dim {$row['avg_dim']}, per cow {$row['group_avg_daily_milkyield']} average liters., average age {$row['avg_age_months']} (months), average age of gestation {$row['ave_age_gestation_days']} days<br/>");
        } else {
            return false;
        }
    }

    function displayValactaStatsByGroup($location_id, $mode = 'default') {

        $sql = "SELECT location_name,round(avg(total_milk)::numeric,2) as total_milk,round(avg(fat_per)::numeric,2) as fat_per, round(avg(prot_per)::numeric,2) as prot_per,round(avg(lactose_per)::numeric,2) as lactose_per, round(avg(ssc)::numeric,-3) as ssc, round(avg(mun)/10::numeric,1) as mun FROM bovinemanagement.bovinecurr
LEFT JOIN batch.valacta_data_latest_test_view ON bovinecurr.local_number = valacta_data_latest_test_view.chain WHERE location_id=$location_id GROUP BY location_name";

        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return("Valacta Last Test Stats for {$row['location_name']}: {$row['total_milk']} liters, fat: {$row['fat_per']}%, crude prot: {$row['prot_per']}%, lactose: {$row['lactose_per']}%,SSC: {$row['ssc']}, MUN: {$row['mun']}.<br/>");
    }

    public function displayListOfLatestMilkYields($mode = 'default') {
        $out[]=("<h3>Latest Milking List</h3>");

        // NOTE: lactation number for older cows is not necessarily correct.
        $sql = <<<SQL
               SELECT bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name,(SELECT COUNT(lactation.id) FROM bovinemanagement.lactation WHERE lactation.bovine_id = bovinecurr.id) as lactation_number, 
bovinecurr.location_name,milkyield,date,milking_number,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,manualid,manualkey,reattatch,manualdetatch FROM alpro.cow 
		LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id = cow.bovine_id
		WHERE date=(SELECT max(date) FROM alpro.cow) AND milking_number=(SELECT max(milking_number) FROM alpro.cow WHERE date=(SELECT max(date) FROM alpro.cow))
SQL;
        
           $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           //  $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.      
               $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
               $rowOut['Location']=$row['location_name'];
               $rowOut['Lactation Number']=$row['lactation_number'];
               $rowOut['One Milking Yeild (l)'] = round($row['milkyield'],1);                    
                $rowOut['Milking #'] =date('l',strtotime($row['date'])). ' #'.$row['milking_number'];            
                $rowOut['DIM']=$row['dim'];              
                $rowOut['Manual ID']= ($row['manualid']==true) ? 'true':'';
                $rowOut['Manual Milking']= ($row['manualkey']==true) ? 'true':'';
                $rowOut['Manual Detatch']= ($row['manualdetatch']==true) ? 'true':'';
                $rowOut['Reattatch']= ($row['reattatch']==true) ? 'true':'';
            
           $resultsOut[]=$rowOut;  
        }
            
        //output
        $dataArr = $resultsOut;
      
         $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,''));
        return implode($out);
    }


    private function showChartValactaDataByGroupTempTable() {

//pulls avegerages out of valacta data based on teh group the animal was in at the time.
        $sql = "CREATE TEMP TABLE temp_valacta_by_location AS 
With temp as (
SELECT chain,total_milk,fat_per,prot_per,ssc,mun,test_date,bovine.id as bovine_id,bovinemanagement.location_id_at_timestamp(bovine.id, test_date::date) as location_id
FROM batch.valacta_data
LEFT JOIN bovinemanagement.bovine ON bovine.local_number=valacta_data.chain
WHERE test_date > (CURRENT_DATE - interval '3 year') ORDER BY test_date 
)
SELECT  location_id,test_date,avg(total_milk) as avg_total_milk,avg(fat_per) as avg_fat_per,avg(prot_per) as avg_prot_per,avg(ssc) as avg_ssc,avg(mun)/10 as avg_mun FROM TEMP GROUP BY test_date,location_id ORDER BY location_id,test_date;";


        $res = $GLOBALS['pdo']->query($sql); //create temp table
    }

    function showChartValactaDataByGroup($location_id, $title) {
        $sql2 = "SELECT * FROM temp_valacta_by_location WHERE location_id=$location_id";



        $res2 = $GLOBALS['pdo']->query($sql2); //create temp table

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $t = strtotime($row2['test_date']);
            $fatArr[$t] = round($row2['avg_fat_per'], 2);
            $protArr[$t] = round($row2['avg_prot_per'], 2);
        }


        print('<div class="lactationCurve">');

        print("<h3>$title Valacta Fat & Protein</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Fat (kg)'] = 'number';
        $headerStrArr['Protein (kg)'] = 'number';

        $x = new GoogleVisualizationLine(null, $headerStrArr, $fatArr, $protArr);
        print($x->toString());


        print('</div> <!-- end LactationCurve -->');
    }

    function showChartOfProvincialFatTest() {

        $query2 = ("SELECT average_test_time,fat,protein,lactose,scc FROM batch.nb_bulk_tank_sample WHERE average_test_time >= (CURRENT_DATE - interval '3 year') order by average_test_time");

        ///////////////
        $res2 = $GLOBALS['pdo']->query($query2);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $t = strtotime($row2['average_test_time']);
            $fatArr[$t] = round($row2['fat'], 2);
            $protArr[$t] = round($row2['protein'], 2);
        }


        print('<div class="lactationCurve">');

        print("<h3>Bulk Tank Fat/Protein</h3>");

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Fat (kg)'] = 'number';
        $headerStrArr['Protein (kg)'] = 'number';

        $x = new GoogleVisualizationLine(null, $headerStrArr, $fatArr, $protArr);
        print($x->toString());


        print('</div> <!-- end LactationCurve -->');
    }

//end function
}

//end class
?>
