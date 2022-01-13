<?php

class HerdProjections extends BasePage {

    public function defaultDisplay() {

        ini_set('memory_limit', '512M');

        (new milkProjection)->main();

        $this->replacementsByQuarterHistogram();
        $this->numberCalvingByMonthHolsteinVsOther();

        $this->plotHeifers();
        $this->plotPreg();
        $this->lactationsOfMilkingHerd();




        //DEV
        // $this->angusHolsteinData();
        //   $cls=new milkProjection();
        //   $cls->main();
    }

    /* show by quarter to know if we have hole in replacements.  */

    function numberCalvingByMonthHolsteinVsOther() {

        $sql = <<<SQL
with temp as (
select width_bucket((calculated_potential_due_date-date_trunc('month', current_date)::date), 0,270, 9) as buckets,
         count(*)
    from bovinemanagement.pregnant_view where substring(service_sire_full_reg_number FROM 1 for 2) = 'HO' AND calculated_potential_due_date >= current_date
group by buckets
order by buckets)
Select *,(buckets-1)*270/9 as bin_bot,buckets*270/9 as bin_top from temp

SQL;

        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $numberDays = ($row['bin_top'] - ($row['bin_top'] - $row['bin_bot']));
            $date = strtotime(date('Y-m-01 00:00:00', strtotime("+ $numberDays days")));
            $arrayAnimals[$date] = $row['count'];
        }

        //not holstein
        $sql2 = <<<SQL2
with temp as (
select width_bucket((calculated_potential_due_date-date_trunc('month', current_date)::date), 0,270, 9) as buckets,
         count(*)
    from bovinemanagement.pregnant_view where substring(service_sire_full_reg_number FROM 1 for 2) != 'HO' 
group by buckets
order by buckets)
Select *,(buckets-1)*270/9 as bin_bot,buckets*270/9 as bin_top from temp

SQL2;

        $res = $GLOBALS['pdo']->query($sql2);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $numberDays = ($row['bin_top'] - ($row['bin_top'] - $row['bin_bot']));
            $date = strtotime(date('Y-m-01 00:00:00', strtotime("+ $numberDays days")));
            $arrayAnimalsNOT[$date] = $row['count'];
        }

        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Holstein'] = 'number';
        $headerStrArr['Other'] = 'number';

        $opt = ' isStacked: true';



        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $arrayAnimals, $arrayAnimalsNOT);
        print("<h3>Animals Pregnant by Month (Due to Holstein vs. Other, ie Angus)</h3>");
        print("<p>From the first of the month, so current month is remianing pregnant animals. 30 day periods, not months.</p>");
        print($x->toString());
    }

    /* 2 years of # of replacements by quarters for animals less than 2 years old. */

    function replacementsByQuarterHistogram() {

        $sql = <<<SQL
            with temp as (
  select width_bucket((current_date-birth_date), 0, 730, 8) as buckets,
         count(*)
    from bovinemanagement.bovinecurr  where birth_date >= (current_date - interval '2 years')
group by buckets
order by buckets
            )
Select *,(buckets-1)*730/8 as bin_bot,buckets*730/8 as bin_top from temp
SQL;

        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $numberDays = ($row['bin_top'] - ($row['bin_top'] - $row['bin_bot']));
            $date = strtotime(date('Y-m-d 00:00:00', strtotime("+ $numberDays days")));
            $date = $numberDays;
            $arrayReplacements[$date] = $row['count'];
        }

        $arrayReplacementsNOT=null;

        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'number'; //BACKWARDS
        $headerStrArr['Replacement'] = 'number';


        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $arrayReplacements, $arrayReplacementsNOT);
        print("<h3>Quarterly Number of Replacements (< 2 years old) by Age in days</h3>");
        print($x->toString());
    }

    //FIXME. not sure what I was doing here. 
    function angusHolsteinData() {

        $sql = <<<SQL
with temp as (
SELECT id,local_number, birth_date,bovinemanagement.last_breeding(id) + '280 days 12:00:00'::interval as possible_due_date ,bovinemanagement.due_date(id),	bovinemanagement.last_breeding_id(id)
,(SELECT bovinemanagement.calculate_breeding_voluntary_waiting_period(x.id) + '280 days 12:00:00'::interval FROM bovinemanagement.bovinecurrall where id=x.id AND bovinemanagement.donotbreed_at_timestamp (id, current_date) is false) as vwp_9month
FROM bovinemanagement.bovinecurrall as x
) select *,
(SELECT sire_full_reg_number FROM bovinemanagement.breeding_event
LEFT JOIN bovinemanagement.semen_straw ON semen_straw_id= semen_straw.id
LEFT JOIN bovinemanagement.sire_semen_code ON semen_straw.semen_code= sire_semen_code.semen_code
 WHERE breeding_event.id= last_breeding_id) as last_breeding_sire
 FROM temp WHERE birth_date <= current_date-interval '10 months'

SQL;

        $res = $GLOBALS['pdo']->query($sql);

        $out = array();

        /*
         * make some business decion logic to see what category a cow is in. We will then take the array of objects and sum them up over each month. 
         * 
         */
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $a = new stdClass();
            $a->date = '';
            $a->poss_pregnant_hol = 0;
            $a->poss_pregnant_not_hol = 0;
            $a->pregnant_hol = 0;
            $a->pregnant_not_hol = 0;
            $a->eligible = 0;
            $a->not_eligible = 0;

            /* possible pregnant */
            if (empty($row['due_date']) AND (!empty($row['possible_due_date']))) {

                // use possible_due_date as date
                //find if holstein or not holstein

                $a->date = $row['possible_due_date'];

                if (substr($row['last_breeding_sire'], 0, 2) == 'HO') {
                    $a->poss_pregnant_hol = 1;
                } else {
                    $a->poss_pregnant_not_hol = 1;
                }
            }
            /* pregnant */ elseif (!empty($row['due_date']) AND (!empty($row['possible_due_date']))) {
                // use due_date as date
                //find if holstein or not holstein

                $a->date = $row['due_date'];

                if (substr($row['last_breeding_sire'], 0, 2) == 'HO') {
                    $a->pregnant_hol = 1;
                } else {
                    $a->pregnant_not_hol = 1;
                }


                /* eligible */
            } elseif (empty($row['due_date']) AND ( empty($row['last_breeding_id'])) AND (!empty($row['vwp_9month']))) {
                //use vwp_9month as date

                $a->date = $row['vwp_9month'];
                $a->eligible = 1;

                /*  not eligible. */
            } elseif (empty($row['due_date']) AND ( empty($row['last_breeding_id'])) AND ( empty($row['vwp_9month'])) AND ( empty($row['possible_due_date']))) {

                $a->date = date('r', strtotime('now')); //use today as date
                $a->not_eligible = 1;
            } else {
                throw new Exception("ERROR: What's missing? $row");
            }

            $out[] = $a;
        }


        var_dump($out);
    }

    //not really the rigth place for this, but the goal should be 50% of herd 3+ lactations.
    function lactationsOfMilkingHerd() {
        print("<h3>Lactations of current milking herd</h3>");
        $sql = "SELECT distinct bovinemanagement.lactation_number (bovinecurr.id),count( bovinemanagement.lactation_number (bovinecurr.id)) FROM bovinemanagement.bovinecurr WHERE location_id= ANY('{39,40,41,42}'::int[]) group by bovinemanagement.lactation_number (bovinecurr.id)";

        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            //   print("lact num/count : " . $row['lactation_number'] . " " . $row['count'] . "<br/>");
            $arrayNumberInLactation[$row['lactation_number']] = $row['count'];
        }

        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'number'; //BACKWARDS
        $headerStrArr['# in Lactation'] = 'number';


        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $arrayNumberInLactation);

        print($x->toString());
    }

    function plotHeifers() {
        print("<h2>Eligibile heifers to breed at 13 Months</h2>");
        $sql = "
             CREATE TEMP TABLE two AS 
SELECT xx.id,local_number,pro_doll,xx.full_name,xx.birth_date
 FROM bovinemanagement.bovinecurr xx
LEFT JOIN batch.holstein_canada_data_curr ON holstein_canada_data_curr.full_reg_number = xx.full_reg_number
WHERE xx.birth_date >= current_date - interval '24 months'; 
             ";
        $res = $GLOBALS['pdo']->query($sql);

        $sql2 = "CREATE TEMP TABLE three AS SELECT * FROM   bovinemanagement.bovinecurr;
/* the count query con bovinecurr is really slow, so temp table created */";
        $res = $GLOBALS['pdo']->query($sql2);

        $sql3 = "SELECT (xx.birth_date + interval '13 months') as birth_date14 ,pro_doll,local_number, 
           (SELECT count(id)  FROM three WHERE birth_date < xx.birth_date + interval '1.5 months' AND  birth_date >= xx.birth_date - interval '1.5 months' )  
           FROM two xx
";
        ?>
        <script type="text/javascript">


            setTimeout(drawChart, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart, true);
            $(window).resize(function () {
                google.charts.setOnLoadCallback(drawChart);
            });
            function drawChart() {
                var data = google.visualization.arrayToDataTable([
                    ['local_number', 'birth_date14', 'pro_doll', 'count'],
        <?php
        $res = $GLOBALS['pdo']->query($sql3);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("['{$row['local_number']}', " . GoogleChartsHelper::javascriptDate($row['birth_date14']) . ",	{$row['pro_doll']},   {$row['count']}],");
        }
        ?>


                ]);

                var options = {
                    title: 'HEIFER Yields',
                    hAxis: {title: '13 Months'},
                    vAxis: {title: 'Pro$'},
                    bubble: {textStyle: {fontSize: 11}},
                    colorAxis: {colors: ['yellow', 'red']}
                };




                var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all'));
                chart.draw(data, options);
            }
        </script>




        <?php
        print("<div id='chart_divv_all' style='height:400px'></div>");
    }

    function plotPreg() {
        $sql = "
           CREATE TEMP TABLE one AS 
SELECT xx.id,xx.local_number,xx.calculated_potential_due_date,pro_doll
            FROM bovinemanagement.pregnant_view xx
            LEFT JOIN bovinemanagement.bovine ON bovine.id = xx.id
            LEFT JOIN batch.holstein_canada_data_curr ON holstein_canada_data_curr.full_reg_number = bovine.full_reg_number;
             ";
        $res = $GLOBALS['pdo']->query($sql);

        $sql2 = "SELECT *, (SELECT count(id)  FROM one WHERE calculated_potential_due_date < xx.calculated_potential_due_date + interval '1.5 months' AND  calculated_potential_due_date >= xx.calculated_potential_due_date - interval '1.5 months' )  
FROM one xx
            ";
        ?>
        <script type="text/javascript">


            setTimeout(drawChart, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart, true);
            $(window).resize(function () {
                google.charts.setOnLoadCallback(drawChart);
            });
            function drawChart() {
                var data = google.visualization.arrayToDataTable([
                    ['local_number', 'calculated_potential_due_date', 'pro_doll', 'count'],
        <?php
        $res = $GLOBALS['pdo']->query($sql2);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("['{$row['local_number']}', " . GoogleChartsHelper::javascriptDate($row['calculated_potential_due_date']) . ",	{$row['pro_doll']},   {$row['count']}],");
        }
        ?>


                ]);

                var options = {
                    title: 'PREGNANCY Yields',
                    hAxis: {title: 'Calving Date'},
                    vAxis: {title: 'Pro$'},
                    bubble: {textStyle: {fontSize: 11}},
                    colorAxis: {colors: ['yellow', 'red']}
                };




                var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all2'));
                chart.draw(data, options);
            }
        </script>




        <?php
        print("<div id='chart_divv_all2' style='height:400px'></div>");
    }

}

//end class


/*
 * take days dry, lactation info. etc. to project:
 * number cows milking
 * days in milk
 * butterfat being produced
 * 
 */

class milkProjection {

    //so we should do every cow in the herd, even baby calves, since we are a closed herd.
    //create an array for each cow going through time for the next five years that projects what will happen.


    function main() {

        $this->milkingNumberPrediction();
        print("<br>");

        print("<div class='row'>");

        print("<div class='col-sm-4 col-sm-push-8'>");

        $a = new QF2_ExcludeFromHerdProjection((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');
        print($a->renderBox('qf2_ExcludeFromHerdProjection', 'Select Animal(s) to Exclude From Projection'));



        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
        print($this->displayExcludeList());
        print("</div>");

        print("</div>");
    }

    public function displayExcludeList() {
        $sql = <<<SQL
SELECT herd_projection_exclude_list.*, bovine.local_number,bovine.full_name,bovine.id as bovine_id
    FROM system.herd_projection_exclude_list
    LEFT JOIN       bovinemanagement.bovine ON bovine.id=    herd_projection_exclude_list.bovine_id               
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $delButton = new QF2_DeleteButtonDecider('anyUser_anyTime', (object) array('dateTimePeriodDays' => '1', 'schemaTable' => 'system.herd_projection_exclude_list'));
        $delButtonTransaction = new QF2_DeleteButtonDecider('anyUser_anyTime', (object) array('dateTimePeriodDays' => '1', 'deleteName' => 'transaction_id', 'schemaTable' => 'system.herd_projection_exclude_list'));

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Employee'] = ($row['userid']);
            
            //delete buttons
            $but = array();
            $but[] = $delButton->output($row['id']);
            $but[] = $delButtonTransaction->output($row['transaction_id'], null, "<i class='fa fa-fw fa-object-group'></i><i class='fa fa-fw fa-trash'></i>");   //show transaction icon
            $rowOut['Action'] = implode($but);


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

     
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Exclude List (Potential Culls)', $dataArr));
        return implode($out);
    }

   

    public function milkingNumberPrediction() {

        $sql = <<<SQL
SELECT    repro_status_curr.id,local_number,birth_date,lactation_number,lactation_latest_id,fresh_date::date,dry_date::date,due_date::date,voluntary_waiting_period,
    donotbreed,prodoll,repro_state,lactation_state,last_breeding as last_breeding_when_open, last_preg_check as last_preg_check_when_open,	 
   herd_projection_exclude_list.event_time::date as exclude_time 
    FROM bovinemanagement.repro_status_curr 
     LEFT JOIN system.herd_projection_exclude_list ON herd_projection_exclude_list.bovine_id=repro_status_curr.id 
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $dataArr = $statement->fetchAll(PDO::FETCH_ASSOC);


        /**
         * 
         * DEBUG
         * 
         */
        //var_dump($dataArr);
        $serializedJSONStr = json_encode($dataArr, JSON_PRETTY_PRINT); //used to display chart source data. 
        $serializedJSONStrPretty = $GLOBALS['MiscObj']->format_json_for_html($serializedJSONStr);

        print( (new BootStrap)->plainCardCollapsed("Projection Chart Data For Each Animal", $serializedJSONStrPretty));



        /**
         * 
         * 
         */
        /*
         * a bunch of wonderfully complicated code to calculate a due date and dry date for every animal even if they don't have one (that's the trick).
         * this then allows us to do herd projections based on the assumptions made here. 
         */
        $gestLength = '+280 day';
        $getPregTime = '+30 day';
        $dryPeriodTime = '+60 day';
        $dryPeriodTimeInv = '-60 day';
        $lactationLength = '+305 day';
        $avgVWPDays='+ 65 day';
        $maxLactationLength = '+335 day';
                
        $calculatedArr = array();
        foreach ($dataArr as $key => $value) {

            //record if animal is lacating as of today.
            //this is used to give a start date 
            if ($value['lactation_state'] == 'lactating') {
                $calculatedArr[$key]['lactating'] = 1;
            } else {
                $calculatedArr[$key]['lactating'] = 0;
            }

            //see if we want to cull them in the future. 
            if (!empty($value['exclude_time'])) {
                $calculatedArr[$key]['exclude_time'] = $value['exclude_time'];
            } else {
                $calculatedArr[$key]['exclude_time'] = null;
            }



            $switchVal = $value['repro_state'] . '|' . $value['lactation_state'];
            $calculatedArr[$key]['switchVal'] = $switchVal;    //for debugging
            $calculatedArr[$key]['bovine_id'] = $value['id'];    //for debugging
            $calculatedArr[$key]['local_number'] = $value['local_number'];    //for debugging

            switch ($switchVal) {
                case 'not_vwp|lactating':
                    //use their voluntary_waiting_period plus 30 days to get pregnant plus 279.5 days gestation to guess when they are due and minus 60 for when dry.
                    $calculatedArr[$key]['calc_due_date'] = (new DateTime($value['voluntary_waiting_period']))->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //end of gest length is due date.
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($dryPeriodTimeInv)->format('Y-m-d'); //due date minus driver period.
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');
                    break;
                case 'not_vwp|dry':
                    //use their due date    
                    $calculatedArr[$key]['calc_due_date'] = (new DateTime($value['due_date']))->format('Y-m-d');
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($dryPeriodTimeInv)->format('Y-m-d'); 
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');

                    break;
                case 'not_vwp|replacement':
                    if (!empty($value['due_date'])) {
                        //if they have a due date then guess at dry date with 60 day VWP. 
                        $calculatedArr[$key]['calc_due_date'] = (new DateTime($value['due_date']))->format('Y-m-d');
                        $calculatedArr[$key]['calc_dry_date'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($dryPeriodTime)->modify($getPregTime)->modify($gestLength)->format('Y-m-d');
                    } else {
                        //due date is empty, so not pregnant, thus guess 
                        $calculatedArr[$key]['calc_due_date'] = (new DateTime($value['voluntary_waiting_period']))->modify($getPregTime)->modify($gestLength)->modify($dryPeriodTime)->format('Y-m-d');
                        $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['voluntary_waiting_period']))->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //or should we calculate NEXT dry date?
                    }

                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');

                    break;
                ///////////////////////
                case 'dnb|lactating':

                    $calculatedArr[$key]['calc_due_date'] = null; //they wont have a due date, because not in herd. 
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['fresh_date']))->modify($lactationLength)->format('Y-m-d'); //this might be optimistic. 305 days should maybe be 250 days.
                    $calculatedArr[$key]['calc_due_date_2nd'] = null; //lactation after
                    $calculatedArr[$key]['calc_dry_date_2nd'] = null;
                    break;
                case 'dnb|dry':

                    //this is a cow that is about to calve, but has already been marked DNB. 
                    if (!empty($value['due_date'])) {
                        $calculatedArr[$key]['calc_due_date'] = $value['due_date'];
                        $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['due_date']))->modify($lactationLength)->format('Y-m-d'); //this might be optimistic. 305 days should maybe be 250 days.
                    }
                    //this is a cow who has no due date and is dry and is on DNB, basically she will not milk, thus should be ZEROS.
                    else {
                        $calculatedArr[$key]['calc_due_date'] = null;
                        $calculatedArr[$key]['calc_dry_date'] = null;
                    }

                    $calculatedArr[$key]['calc_due_date_2nd'] = null; //lactation after
                    $calculatedArr[$key]['calc_dry_date_2nd'] = null;

                    break;
                case 'dnb|replacement':
                    //same as previous, has due date, then DNB applies to after that, otherwise....
                    if (!empty($value['due_date'])) {
                        $calculatedArr[$key]['calc_due_date'] = $value['due_date'];
                        $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['due_date']))->modify($lactationLength)->format('Y-m-d'); //this might be optimistic. 305 days should maybe be 250 days.
                    }
                    //this is a cow who has no due date and is a replacement, they will never milk. 
                    else {
                        $calculatedArr[$key]['calc_due_date'] = null;
                        $calculatedArr[$key]['calc_dry_date'] = null;
                    }

                    $calculatedArr[$key]['calc_due_date_2nd'] = null; //lactation after
                    $calculatedArr[$key]['calc_dry_date_2nd'] = null;

                    break;
                ///////////////////////
                case 'open|lactating':
                    //this could be an aniaml perfectly normal and just by there VWP or it could be a problem breeder 200 days in milk. 
                    //NOTE:, maybe an if statement if over 150 DIM, that would bias them for leaving earlier?
                    //NOTE: starting from today's date causes discontinuties unless we at random number.
                    //Fixed by using a normal distribution.
                    //We need to have the normal distribution start at a negative day, because they could actually have been bred any time after there VWP. Check when VWP is and use that as day zero or last preg check date?
                    $dayModifier = $this->startingDateModiferForOpenAniamls($value['last_preg_check_when_open'], $value['last_breeding_when_open'], $value['voluntary_waiting_period']);
                    $calculatedArr[$key]['calc_due_date'] = (new DateTime('now'))->modify('+' . round($GLOBALS['MiscObj']->purebell($dayModifier, 60 + $dayModifier, 20)) . ' day')->modify($gestLength)->format('Y-m-d');
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($dryPeriodTimeInv)->format('Y-m-d'); //due - 60 days.
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');

                    break;
                case 'open|dry':
                    //cow who is open and dry should be heading to a career change shortly. Will never milk.
                    $calculatedArr[$key]['calc_due_date'] = null;
                    $calculatedArr[$key]['calc_dry_date'] = null;
                    $calculatedArr[$key]['calc_due_date_2nd'] = null; //lactation after
                    $calculatedArr[$key]['calc_dry_date_2nd'] = null;
                    break;
                case 'open|replacement':
                    //assume they will get pregnant shortly.
                    $dayModifier = $this->startingDateModiferForOpenAniamls($value['last_preg_check_when_open'], $value['last_breeding_when_open'], $value['voluntary_waiting_period']);
                    $calculatedArr[$key]['calc_due_date'] = (new DateTime('now'))->modify('+' . round($GLOBALS['MiscObj']->purebell($dayModifier, 60 + $dayModifier, 20)) . ' day')->modify($gestLength)->format('Y-m-d');
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($dryPeriodTimeInv)->format('Y-m-d'); //due - 60 days.
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');

                    break;
                ///////////////////////
                case 'pregnant|lactating':
                   
                    
                    //dry off early case
                    $origin = new DateTime($value['fresh_date']);
                    $target = new DateTime($value['due_date']);
                    $interval = $origin->diff($target);
                    if ($interval->format('%a') >= 395) {  //335+60 days for dry. 
                         $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['fresh_date']))->modify($maxLactationLength)->format('Y-m-d'); //special case when lacation will be too long (ie they took to long to get pregnant)
                    } else {
                         $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['due_date']))->modify($dryPeriodTimeInv)->format('Y-m-d'); //dry date X dry period days less than due date. //Normal case
                    }           
                    $calculatedArr[$key]['calc_due_date'] = $value['due_date']; //future due date of course 
                    //
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');

                    break;
                    break;
                case 'pregnant|dry':
                    $calculatedArr[$key]['calc_due_date'] = $value['due_date']; //future due date but coming soon.
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['due_date']))->modify($lactationLength)->format('Y-m-d'); //normal lacation length.
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');
                    break;
                    break;
                case 'pregnant|replacement':
                    $calculatedArr[$key]['calc_due_date'] = $value['due_date']; //future due date but coming soon.
                    $calculatedArr[$key]['calc_dry_date'] = (new DateTime($value['due_date']))->modify($lactationLength)->format('Y-m-d'); //normal lacation length.
                    $calculatedArr[$key]['calc_due_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date']))->modify($avgVWPDays)->modify($getPregTime)->modify($gestLength)->format('Y-m-d'); //lactation after: due_date + VWP days + time to get pregnant + gestation.
                    $calculatedArr[$key]['calc_dry_date_2nd'] = (new DateTime($calculatedArr[$key]['calc_due_date_2nd']))->modify($dryPeriodTimeInv)->format('Y-m-d');

                    break;

                default:
                    throw new Exception(" Error: state not in switch statement: $switchVal.");
                    break;
            }


            //  var_dump($calculatedArr[$key]);  
        }//end foreach
        // print(json_encode($calculatedArr));

       /* 
        $fp = fopen('/tmp/file.csv', 'w');

foreach ($calculatedArr as $fields) {
    fputcsv($fp, $fields);
}

fclose($fp);
        */
        

        $serializedJSONStr = json_encode($calculatedArr, JSON_PRETTY_PRINT); //used to display chart source data. 
        $serializedJSONStrPretty = $GLOBALS['MiscObj']->format_json_for_html($serializedJSONStr);
        

        
        print( (new BootStrap)->plainCardCollapsed("Projection Chart Data For Each Animal SPLITS",  (new JQueryDataTable)->startBasicArrayOpt('Projection Chart Data For Each Animal SPLITS', $calculatedArr)));
        
        
        $this->generatePredictionArr($calculatedArr);
    }

    //find start date of when we know or don't know that a cow could be pregnant. 
    // A cow can't be pregnant if they have not been bred and they can't be pregnant before VWP (99.9%) and hey can't be pregnant any date before a preg check (95%+ of cases). 
    private function startingDateModiferForOpenAniamls($last_preg_check_when_open, $last_breeding_when_open, $voluntary_waiting_period) {

        $dayModifier = 0;  //today is default, should never happen.
        if ((strtotime($last_preg_check_when_open)) > (strtotime($last_breeding_when_open))) { //then they can get bred anytime after that, use last preg check as starting date.  
            $a = new \DateTime($last_preg_check_when_open);
            $b = new \DateTime;
            $dayModifier = $a->diff($b)->days;
        } elseif ((strtotime($last_preg_check_when_open)) < (strtotime($last_breeding_when_open))) { //then breeding has occured after the last open preg check, use that as starting date. 
            $a = new \DateTime($last_breeding_when_open);
            $b = new \DateTime;
            $dayModifier = $a->diff($b)->days;
        } else {
            $a = new \DateTime($voluntary_waiting_period);
            $b = new \DateTime;
            $dayModifier = $a->diff($b)->days;
        }

        return $dayModifier * -1;
    }

    private function generatePredictionArr($calculatedArr) {

        //make an array where keys are daily dates over a period of interest.
        $begin = new DateTime('now');
        $end = (new DateTime('now'))->modify('+1200 day'); //example $end = $end->modify( '+1 day' );
        $interval = new DateInterval('P1D');
        $daterange = new DatePeriod($begin, $interval, $end);

        foreach ($daterange as $date) {
            $dateArr[$date->format("Y-m-d")] = null;
        }


        //var_dump($blahs);



        $outArr = array();
        foreach ($calculatedArr as $key => $value) {

            //set initial state of the animal : milking or not.
            if ($value['lactating'] == 1) {

                $state = 1;  //aniamal is milking
                $why = 'lactating';
            } else {
                $state = 0;
                $why = 'lactating';
            }

            $tempArr = array();
            foreach ($dateArr as $key2 => $value2) {

                /*
                 * DEBUG uncomment
                 */
                /*
                  print('calc_dry_date:'.$value['calc_dry_date'].'|| ');
                  print('calc_due_date:'.$value['calc_due_date'].'|| ');
                  print('switchVal:'.$value['switchVal'].'|| ');
                  print('bovine_id:'.$value['bovine_id'].'|| ');
                  print('state:'.$state.'|| ');
                  print('key2:'.$key2.'.<br> ');
                 */

                //switch to not milking when dried off.
                if ($value['calc_dry_date'] == $key2) {
                    $state = 0;
                    $why = 'calc_dry_date';
                }
                //switch to milking when calved  
                if ($value['calc_due_date'] == $key2) {
                    $state = 1;
                    $why = 'calc_due_date';

                    //now calculate DIM relative to due date.
                    $calvingDate = (new DateTime($value['calc_due_date']));
                    $dateOfInterest = (new DateTime($key2));
                    $dim = $dateOfInterest->diff($calvingDate)->format("%d");
                }

                //switch to not milking when dried off 2nd time.
                if ($value['calc_dry_date_2nd'] == $key2) {
                    $state = 0;
                    $why = 'calc_dry_date_2nd';
                }
                //switch to milking when calving 2nd time
                if ($value['calc_due_date_2nd'] == $key2) {
                    $state = 1;
                    $why = 'calc_due_date_2nd';

                    //now calculate DIM relative to due date.       
                    $calvingDate = (new DateTime($value['calc_due_date_2nd']));
                    $dateOfInterest = (new DateTime($key2));
                    $dim = $dateOfInterest->diff($calvingDate)->format("%d");
                }


                if (!empty($value['exclude_time'])) {
                    if (new DateTime($value['exclude_time']) <= (new DateTime($key2))) {
                        $state = 0;
                        $why = 'exclude_time';
                    }
                }



                $tempArr[$key2] = $state;
                $tempArrWhy[$key2] = $why . '|' . $state;
            }

            $outArr[$key] = $tempArr;
            $outArrWhy[$key] = $tempArrWhy;
        }

        //DEBUG print raw data 
        // var_dump($outArr);
        //sum arryas together to get daily total
        $sumArray = array();
        foreach ($outArr as $k => $subArray) {
            foreach ($subArray as $id => $value) {
                // print($value.' ');
                $key = strtotime($id);
                (array_key_exists($key, $sumArray)) ? $sumArray["$key"] += $value : $sumArray["$key"] = $value; //for the first time through the key doesnt exist yet.
                $sumArrayTooltip[$key] = 'tool tip text here'; //strtotime for google charts
            }
        }


//for why array
//changes to an array of dates as key with a long unindexed subarray of data
        $sumUp = array();
        foreach ($outArrWhy as $k => $subArrayWhy) {
            foreach ($subArrayWhy as $date => $combo) {
                $sumUp[$date][] = $combo;
            }
        }

//do the counting up of each type in the why array, then change it to json str for each data.
        /* now do a sum of values for each data. */
        $whyAnswerArr = array();
        foreach ($sumUp as $k => $value) {
            $whyAnswerArr[$k] = json_encode(array_count_values($value));
        }
        $serializedJSONStr = json_encode($whyAnswerArr, JSON_PRETTY_PRINT); //used to display chart source data. 
        $serializedJSONStrPretty = $GLOBALS['MiscObj']->format_json_for_html($serializedJSONStr);


        print("<h3>Projection of Future Number of Animals Milking</h3>");
        print((new BootStrap)->infoCallout('Note: Model assume 2 dead animals per month. FIXME: NOT TRUE YET'));
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['# of Cows'] = 'number';


        $opt = "                  
                        series: {
            0:{ lineWidth: 2, targetAxisIndex: 0 }
                                },
                       
                        legend: {position: 'right'}";

        $x = new GoogleVisualizationLine(null, $opt, $headerStrArr, $sumArray);
        print($x->toString());

        print( (new BootStrap)->plainCardCollapsed("Projection Chart Source Data", $serializedJSONStrPretty));

        $js = <<<JS
        <script>
google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
        var dataTable = new google.visualization.DataTable();
        dataTable.addColumn('date', 'Year');
        dataTable.addColumn('number', 'Sales');
        // A column for custom tooltip content
        dataTable.addColumn({type: 'number', role: 'tooltip'});
        dataTable.addRows([
          ['2010', 600,'$600K in our first year!'],
          ['2011', 1500, 'Sunspot activity made this our best year ever!'],
          ['2012', 800, '$800K in 2012.'],
          ['2013', 1000, '$1M in sales last year.']
        ]);

        var options = { legend: 'none' };
        var chart = new google.visualization.ColumnChart(document.getElementById('tooltip_action'));
        chart.draw(dataTable, options);
      }  
          </script>
JS;


        //  print($js);
        //  var_dump($outArr);
    }

//end function
}

//end class


/*
 * move animal form.
 * 
 */

Class QF2_ExcludeFromHerdProjection extends QF2_Base {

    function definition($form) {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForNextYear();

        $form = (new Quickform2_GroupSelect)->definitionAnimalGroup($form);


        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");

        return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
        $jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

        return $jscript;
    }

    function process($value) {

        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            $transaction_id = $GLOBALS['MiscObj']->generatePseudoRandomTransactionInteger();

            $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);


            if (is_array($value['animalSelected'])) {
                //group
                foreach ($value['animalSelected'] as $k => $v) {


                    // GROUP (LOCATION) MOVE
                    if (strstr($v, 'GROUP_') != false) { //when group or pen of animals
                        //remove the 'GROUP_' to find location_id.
                        $integerLocationID = preg_replace('/^GROUP_/', '', $v);
                        if (filter_var($integerLocationID, FILTER_VALIDATE_INT)) {
                            $locationID = $integerLocationID;
                        } else {
                            throw new Exception("Error: bad data, location id not an integer");
                        }


                        //now look up which cows are in that location and we will loop through them. this is inside a transaction, so by doing this here we make it atomic. 
                        $sql2 = "SELECT bovine_id From bovinemanagement.locationcurr WHERE location_id=$integerLocationID";
                        $res2 = $GLOBALS['pdo']->query($sql2);
                        while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {

                            $query = "/*GROUP*/ INSERT INTO system.herd_projection_exclude_list (bovine_id,event_time,userid,transaction_id) VALUES ({$row2['bovine_id']},'{$value['event_time']}','{$GLOBALS['auth']->getUsername()}',$transaction_id) ON CONFLICT DO NOTHING";
                            $res = $GLOBALS['pdo']->exec($query);
                        }
                        //NORMAL BUNCH OF ANIMALS    
                    } else {

                        //now we insert all the animals that were checked.
                        $bovine_id = $v;
                        $query = "/*ANIMAL*/ INSERT INTO system.herd_projection_exclude_list (bovine_id,event_time,userid,transaction_id) VALUES ($bovine_id,'{$value['event_time']}','{$GLOBALS['auth']->getUsername()}',$transaction_id) ON CONFLICT DO NOTHING";
                        $res = $GLOBALS['pdo']->exec($query);
                    }
                }
            }
            //individual case
            else {
                $query = "/*SINGLE ANIMAL*/ INSERT INTO system.herd_projection_exclude_list (bovine_id,event_time,userid,transaction_id) VALUES ({$value['animalSelected']},'{$value['event_time']}','{$GLOBALS['auth']->getUsername()}',$transaction_id) ON CONFLICT DO NOTHING";
                $res = $GLOBALS['pdo']->exec($query);
            }


            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
    }

}

//end class   
?>