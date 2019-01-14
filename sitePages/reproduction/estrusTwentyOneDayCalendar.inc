<?php



class EstrusTwentyOneDayCalendar extends BasePage {

    public function defaultDisplay() {


        // $this->breedingCalendar();
        $this->plotRepro();
    }

    function chooseStatus($row) {
        //logic for colour
        if ($row['past_voluntary_waiting'] != 1) {
            return 'Not Eligible';
        } //mark when not past VWP.
        if ($row['pregnant'] == 1) {
            return 'Pregnant';
        } //pregnant 
        if ($row['do_not_breed'] == 1) {
            return 'DNB';
        } //do not breed
        if ($row['bred_recent'] != 1) {
            return 'Eligible';
        } //not bred yet
        else {
            return 'Bred Recently';
        } //bred
    }

    /*
     * from Stirling Dorance Oct 2018, use this instead of preg rate
     * was the cow pregnant at 150 DIM or not. SHould be 75% pregnant. 
     */
    function percentPreg() {
      
        /* this is wrong, calculates for today, not by cow at 150DIM */
    $sql="    with temp as (
SELECT bovine_id,( EXTRACT(epoch FROM (current_date-fresh_date))/86400)  as dim,
(SELECT true WHERE bovinemanagement.	last_breeding (bovinecurr.id) >= (current_date - interval '30 days')) as bred_recent,
(SELECT true WHERE bovinemanagement.donotbreed_at_timestamp (bovinecurr.id, current_date)) as do_not_breed,
(SELECT true WHERE bovinemanagement.due_date(bovinecurr.id) is not null) as pregnant
FROM bovinemanagement.bovinecurr 
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
LEFT JOIN  batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
WHERE fresh_date is not null 
), temp2 as (
/* calculate pregnant over total cows eligible*/
SELECT
(SELECT count(pregnant)  from temp where dim >150 AND do_not_breed is not true) as numerator, (SELECT count(bovine_id)  from temp where dim >150 AND do_not_breed is not true) as denominator
)
SELECT numerator::numeric/denominator::numeric from temp2
";
    
    
    
    }
    
    
    function plotRepro() {
        print("<h2></h2>");
        $sql = "
         SELECT local_number,( EXTRACT(epoch FROM (current_date-fresh_date))/86400)  as dim, 1 as milk_dollars, prodoll,
(SELECT true WHERE current_date -bovinemanagement.calculate_breeding_voluntary_waiting_period (bovinecurr.id)  > 0) as past_voluntary_waiting,
(SELECT true WHERE bovinemanagement.	last_breeding (bovinecurr.id) >= (current_date - interval '30 days')) as bred_recent,
(SELECT true WHERE bovinemanagement.donotbreed_at_timestamp (bovinecurr.id, current_date)) as do_not_breed,
(SELECT true WHERE bovinemanagement.due_date(bovinecurr.id) is not null) as pregnant,
lactation_revenue/( EXTRACT(epoch FROM (current_date-fresh_date))/86400) as daily_revenue
FROM bovinemanagement.bovinecurr 
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
LEFT JOIN  batch.bovinecurr_long ON bovinecurr.id=bovinecurr_long.id
WHERE fresh_date is not null order by current_date-fresh_date 
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
                    ['local number', 'DIM', 'Daily Revenue ($)', 'status',''],
        <?php
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            $row['status'] = $this->chooseStatus($row);



            print("['{$row['local_number']}', 	{$row['dim']},	{$row['daily_revenue']},   '{$row['status']}', 1],");
        }
        ?>


                ]);

                var options = {
                    title: 'Repro View Cows',

                    hAxis: {title: 'DIM', viewWindow: {

                            min: 0,
                        }},
                    vAxis: {title: 'Avg Daily Revenue ($)', viewWindow: {

                            min: 0,
                        }},

                    options: {
                        series: {
                            'Pregnant': {
                                color: 'purple'
                            },
                            'DNB': {
                                color: 'black'
                            },
                            'Not Eligible': {
                                color: 'blue'
                            },
                            'Eligible': {
                                color: 'green'
                            },
                            'Bred Recently': {
                                color: 'red'
                            }
                        }
                    },

                    height: 1000

                };




                var chart = new google.visualization.BubbleChart(document.getElementById('chart_divv_all'));
                  google.visualization.events.addListener(chart, 'select', selectHandler); 
   /* used to forward to animal when bubble clicked */
    function selectHandler(e)     {   
       // alert(data.getValue(chart.getSelection()[0].row, 0));
        window.open('https://int.littleriver.ca/index.php?pageid=6&tabtocall=4&bovine_local_number='+data.getValue(chart.getSelection()[0].row, 0), '_blank');
    };
                chart.draw(data, options);
            }
        </script>




        <?php
        print("<div id='chart_divv_all'></div>");
    }

}
//end class
?>