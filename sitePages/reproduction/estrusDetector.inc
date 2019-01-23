<?php
class EstrusDetector extends TabPage {

    public function defaultDisplay() {
        // $this->displayAllCowsLastEvent();
        $tabArray[1]['name'] = 'Latest Events';
        $tabArray[2]['name'] = 'Provision Tag';
        $tabArray[3]['name'] = 'Other Tag';
        $tabArray[4]['name'] = 'Map';
        $tabArray[5]['name'] = 'Sort Pen Area';
        $this->renderTabs($tabArray);
    }

    public function tab1() {

        print("<h3>Latest Tag Events</h3>");

        $this->displayAllCowsLastEvent();
    }

    public function tab2() {

        print("<h3>BLE to RFID Correlation</h3>");

        //print($this->bleToRfidCorrelationChart());
        print($this->displayTagMaxRSSIAtSortGate());
    }

    public function tab3() {
        $this->otherTagLatestDataChart('e8:c0:2f:f1:04:c4');
    }

    public function tab4() {
        $this->checkBaseStationsWorking();
        $obj = new Trilateration();

        $obj->allCow();
    }

    public function tab5() {

        $this->quickFormMatchTagToCow();
        $this->displayWhoIsInSortPenNow();
    }

    /* over 1000 variance in last 2 days */

    public function highProbabilityHeats() {
        $sql = "SELECT * FROM bas.ble_tag_event_high_variance_2days";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $row['event_time'] = date("D H:i", strtotime($row['event_time']));  //has to be at end.
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function quickFormMatchTagToCow() {

        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //hourly array, with unix timestamp as index.
        //grab a list of tags.
        //grab a list of tags.
        $res = $GLOBALS['pdo']->query("SELECT
     distinct  tag_id 
FROM   bas.ble_tag_event WHERE
 event_time:: timestamptz > (now() - interval '1 hour') 
ORDER BY tag_id");


        $tagNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {

            $tagNumber[$row[0]] = $row[0];
        }

        ////cows
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovine WHERE death_date IS NULL ORDER BY local_number"); {
            
        } // Always check that result is not an error
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
        }



        $form = new HTML_QuickForm("quickFormMatchTagToCow", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Match Tag to Cow');

        $form->addElement('select', 'tag_id', 'Tag Number', $tagNumber);
        $form->addElement('select', 'bovine_id', 'Animal Number', $aniNumber);




        $form->addElement('select', 'DateTagInService', 'Time of Tag Put on Cow', $dateArray);

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults

        $form->setDefaults(array('bovine_id' => 0)); //set default to nono.
        $form->setDefaults(array('DateTagInService' => 0));

        //rules
        $form->addRule('tag_id', 'Tag number is required', 'required');
        $form->addRule('bovine_id', 'Animal number is required', 'required');
        $form->addRule('DateTagInService', 'Date of Tag added to cow is required', 'required');



        // Try to validate a form
        if ($form->validate()) {

            //get values
            $tag_id = $form->exportValue('tag_id');
            $bovine_id = $form->exportValue('bovine_id');
            $tempArray = $form->getElementValue('DateTagInService');
            $DateTagInService = $tempArray[0];
            $estimated_time = date('Y-m-d H:i:sO', $DateTagInService);

            $pageid = $form->exportValue('pageid');



            //don't put id value, uses next one in DB. '{$GLOBALS['auth']->getUsername()}'
            $query = "INSERT INTO bas.ble_bovine_tag (bovine_id,tag_id,event_time) VALUES ($bovine_id,'$tag_id','$estimated_time')";
            $res = $GLOBALS['pdo']->exec($query);





            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation
        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function tagsInSortPen() {

        $sql = "

SELECT
       bovine_id,local_number,full_name,ble_tag_event.tag_id,rssi ,(ble_tag_event.event_time) 
FROM   bas.ble_tag_event
LEFT JOIN
bas.ble_bovine_tag ON ble_bovine_tag.tag_id=ble_tag_event.tag_id
LEFT JOIN
bovinemanagement.bovine ON bovine.id = ble_bovine_tag.bovine_id
WHERE base_id='b8:27:eb:e6:c4:36' AND ble_tag_event.event_time:: timestamptz > (now() - interval '40 seconds') AND rssi > -80
ORDER BY RSSI DESC

";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] =  $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            // $row['event_time']= date("l, d-M-Y H:i:s T",strtotime($row['event_time']));  //has to be at end.
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayWhoIsInSortPenNow() {
        print("<h3>Tags in sort pen now</h3>");
        print("<h4>RSSI of -60 is stronger signal then -80 on reader c4:36</h4>");
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
       
    
            {
                data: "tag_id",
                title: "Tag #"
            },  
            {
                data: "event_time",
                title: "event_time"
            },
        {
                data: "rssi",
                title: "rssi"
            },
                 {
                data: "local_number",
                title: "Animal (if known)"
            },
                
           
EX;
        print($dt->start($this->pageid, 'tagsInSortPen', null, $columnsStr));
    }

    public function otherTagLatestData($request) {


        /*
          if (!empty($request['item'])) {
          if (is_numeric($request['item'])) {
          $bovine_id=$request['item'];
          }}
          else {
          throw new Exception('Error: no bovine id specified');
          }

         */

        $tag_id = 'e8:c0:2f:f1:04:c4';
        $array = array();
        $sql = " SELECT distinct(x.event_time), 
 (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:38:ff:8d' and event_time=x.event_time) as base_ff8d_rssi,
 (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:cf:fe:95' and event_time=x.event_time) as base_fe95_rssi,
 (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:06:eb:c0' and event_time=x.event_time) as base_ebc0_rssi,
  (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:5c:b7:0e' and event_time=x.event_time) as base_b70e_rssi,
   (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:80:17:02' and event_time=x.event_time) as base_1702_rssi
FROM bas.ble_tag_event x 

 WHERE tag_id='$tag_id' AND x.event_time < now() AND  x.event_time > (now() - interval '7 day')
 ORDER BY event_time DESC  limit 80000";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $d1 = new DateTime($row['event_time']);
            $d1->sub(new DateInterval('P1M')); //subtract one month for Javascript, becuase it is the poorest language....well maybe cobol is worse.
            //format: Date(Year, Month, Day, Hours, Minutes, Seconds, Milliseconds)
            $row['event_time'] = 'Date(' . $d1->format('Y,m,d,H,i,s') . ')'; //epoch plus microseconds 
        }

        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'datetime', 'label' => 'Date');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Base ff8d RSSI (dBm)'); //y axis #1
        $array['cols'][] = array('type' => 'number', 'label' => 'Base fe95 RSSI (dBm)'); //y axis #2
        $array['cols'][] = array('type' => 'number', 'label' => 'Base ebc0 RSSI (dBm)'); //y axis #3
        $array['cols'][] = array('type' => 'number', 'label' => 'Base b70e RSSI (dBm)'); //y axis #4
        $array['cols'][] = array('type' => 'number', 'label' => 'Base 1702 RSSI (dBm)'); //y axis #4

        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_time']}"),
                array('v' => $row['base_ff8d_rssi']),
                array('v' => $row['base_fe95_rssi']),
                array('v' => $row['base_ebc0_rssi']),
                array('v' => $row['base_b70e_rssi']),
                array('v' => $row['base_1702_rssi']),
            );
        }
        return $array;
    }

    /*
     * for debuging shows data from each base station
     * 
     */

    function otherTagLatestDataChart($tag_id) {
        ?>

        <!--Load the AJAX API-->


        <script type="text/javascript">

            // Load the Visualization API and the piechart package.




            function drawChartC() {
                var jsonData = $.ajax({
                    url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']); ?>140/otherTagLatestData/<?php echo($tag_id); ?>",
                                dataType: "json",
                                async: false
                            }).responseText;

                            // Create our data table out of JSON data loaded from server.
                            var data = new google.visualization.DataTable(jsonData);

                            // Instantiate and draw our chart, passing in some options.
                            var chart = new google.visualization.LineChart(document.getElementById('chart_divC'));
                            chart.draw(data, {chartArea: {left: 40, top: 20}, width: 1600, height: 400, pointSize: 2, lineWidth: 0, vAxis: {viewWindow: {
                                        min: -120,
                                        max: -60
                                    }}});
                        }

                        // Set a callback to run when the Google Visualization API is loaded.
                        google.charts.setOnLoadCallback(drawChartC, true);
                        $(window).resize(function () {
                            google.charts.setOnLoadCallback(drawChartC);
                        });
        </script>

        <div id="chart_divC" ></div>
        <?php
    }

    function bleToRfidCorrelationChart() {
        
    }

    /**
     * 
     * not used.
     */
    function tagMaxRSSIAtSortGate() {
        /* using base_id 'b8:27:eb:38:ff:8d' located over sort gate reader we record the time tags pass by, 
         * by looking for maximum rssi for the previous 4 milking periods speratley. 
         */
        $sql = "
with temp as (
SELECT one.tag_id,event_time1,event_time2,event_time3,event_time4 FROM (
SELECT DISTINCT 
tag_id 
, first_value(event_time) OVER w as event_time1
 FROM bas.ble_tag_event  WHERE base_id = 'b8:27:eb:38:ff:8d'  AND event_time <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking1 ) 
WINDOW w AS (PARTITION BY tag_id  order by rssi DESC
             ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
) one
LEFT JOIN 
(SELECT DISTINCT 
tag_id 
, first_value(event_time) OVER w as event_time2
 FROM bas.ble_tag_event  WHERE base_id = 'b8:27:eb:38:ff:8d'  AND event_time <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking2 ) 
WINDOW w AS (PARTITION BY tag_id  order by rssi DESC
             ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
) two on one.tag_id=two.tag_id
LEFT JOIN 
(SELECT DISTINCT 
tag_id 
, first_value(event_time) OVER w as event_time3
 FROM bas.ble_tag_event  WHERE base_id = 'b8:27:eb:38:ff:8d'  AND event_time <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking3 ) 
WINDOW w AS (PARTITION BY tag_id  order by rssi DESC
             ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
) three on one.tag_id=three.tag_id
LEFT JOIN 
(SELECT DISTINCT 
tag_id 
, first_value(event_time) OVER w as event_time4
 FROM bas.ble_tag_event  WHERE base_id = 'b8:27:eb:38:ff:8d'  AND event_time <@ ( SELECT (bovinemanagement.last_four_milkings_tstzrange()).prevmilking4 ) 
WINDOW w AS (PARTITION BY tag_id  order by rssi DESC
             ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)
) four on one.tag_id=four.tag_id
)
SELECT (SELECT bovine_id FROM bas.ble_bovine_tag_curr WHERE tag_id=temp.tag_id)
,*, alpro.bovine_at_sort_gate_at_specifc_time( event_time1::timestamp) as potential_1, alpro.bovine_at_sort_gate_at_specifc_time( event_time2::timestamp) as potential_2, alpro.bovine_at_sort_gate_at_specifc_time( event_time3::timestamp) as potential_3, alpro.bovine_at_sort_gate_at_specifc_time( event_time4::timestamp) as potential_4
 from temp
";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            // $row['event_time']= date("l, d-M-Y H:i:s T",strtotime($row['event_time']));  //has to be at end.
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayTagMaxRSSIAtSortGate() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
       
         {
                data: "bovine_id",
                title: "bovine_id"
            },
            {
                data: "tag_id",
                title: "Tag #"
            },  
            {
                data: "event_time1",
                title: "Previous Milking #1"
            },
        {
                data: "potential_1",
                title: "Potential 1"
            },
                 {
                data: "event_time2",
                title: "Previous Milking #2"
            },
        {
                data: "potential_2",
                title: "Potential 2"
            },
        {
                data: "event_time3",
                title: "Previous Milking #3"
            },
        {
                data: "potential_3",
                title: "Potential 3"
            },
                 {
                data: "event_time4",
                title: "Previous Milking #4"
            },
        {
                data: "potential_4",
                title: "Potential 4"
            }
           
EX;
        print($dt->start($this->pageid, 'tagMaxRSSIAtSortGate', null, $columnsStr));
    }

    public function allCowsLastEvent() {

        /* takes the last 20 seconds and reports avg reading from all bases */
        $sql = "SELECT * FROM bas.all_tags_latest_event
";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] =  $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);

            $row['event_time'] = date("l, d-M-Y H:i:s T", strtotime($row['event_time']));  //has to be at end.
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayAllCowsLastEvent() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },                
            {
                data: "local_number",
                title: "Animal"
            },
            {
                data: "tag_id",
                title: "Tag #"
            },  
            {
                data: "base_ff8d_rssi",
                title: "base_ff8d_rssi (dBm)"
            },
                 {
                data: "base_fe95_rssi",
                title: "base_fe95_rssi (dBm)"
            },
        {
                data: "base_ebc0_rssi",
                title: "base_ebc0_rssi (dBm)"
            },
                 {
                data: "base_b70e_rssi",
                title: "base_b70e_rssi (dBm)"
            },
                 
                 {
                data: "base_1702_rssi",
                title: "base_1702_rssi (dBm)"
            },
            {
                data: "event_time",
                title: "Average Event Time"
            }
           
EX;
        print($dt->start($this->pageid, 'allCowsLastEvent', null, $columnsStr));
    }

// AND ble_tag_event.event_time > '2016-03-20 0:00 -03' AND  ble_tag_event.event_time < '2016-03-20 9:15 -03'
    public function oneCowLatestData($request) {


        if (!empty($request['item'])) {
            if (is_numeric($request['item'])) {
                $bovine_id = $request['item'];
            }
        } else {
            throw new Exception('Error: no bovine id specified');
        }



        $array = array();
        $sql = " SELECT distinct(x.event_time), 
 (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:38:ff:8d' and event_time=x.event_time) as base_ff8d_rssi,
 (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:cf:fe:95' and event_time=x.event_time) as base_fe95_rssi,
 (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:06:eb:c0' and event_time=x.event_time) as base_ebc0_rssi,
  (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:5c:b7:0e' and event_time=x.event_time) as base_b70e_rssi,
    (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:80:17:02' and event_time=x.event_time) as base_1702_rssi
FROM bas.ble_tag_event x 
LEFT JOIN bas.ble_bovine_tag ON ble_bovine_tag.tag_id=x.tag_id
LEFT JOIN bovinemanagement.bovine ON bovine.id=ble_bovine_tag.bovine_id
 WHERE bovine_id=$bovine_id AND x.event_time < now() AND  x.event_time > (now() - interval '1 day')
 ORDER BY event_time ASC  limit 100000";

        /*
          //std dev and avg, one minute periods.
          $sql="
          SELECT distinct(to_timestamp(floor((extract('epoch' from ble_tag_event.event_time) / 150 )) * 150) )  as event_time,stddev(rssi) OVER w as stddev_rssi, avg(rssi) OVER w AS avg_rssi
          FROM bas.ble_tag_event
          LEFT JOIN bas.ble_bovine_tag ON ble_bovine_tag.tag_id=ble_tag_event.tag_id
          WHERE bovine_id=$bovine_id
          WINDOW w AS (PARTITION BY ble_tag_event.tag_id, to_timestamp(floor((extract('epoch' from ble_tag_event.event_time) / 150 )) * 150) )
          limit 36000";
         */

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $d1 = new DateTime($row['event_time']);
            $d1->sub(new DateInterval('P1M')); //subtract one month for Javascript, becuase it is the poorest language....well maybe cobol is worse.
            //format: Date(Year, Month, Day, Hours, Minutes, Seconds, Milliseconds)
            $row['event_time'] = 'Date(' . $d1->format('Y,m,d,H,i,s') . ')'; //epoch plus microseconds 
        }

        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'datetime', 'label' => 'Date');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Base ff8d RSSI (dBm)'); //y axis #1
        $array['cols'][] = array('type' => 'number', 'label' => 'Base fe95 RSSI (dBm)'); //y axis #2
        $array['cols'][] = array('type' => 'number', 'label' => 'Base ebc0 RSSI (dBm)'); //y axis #3
        $array['cols'][] = array('type' => 'number', 'label' => 'Base b70e RSSI (dBm)'); //y axis #4
        $array['cols'][] = array('type' => 'number', 'label' => 'Base 1702 RSSI (dBm)'); //y axis #4

        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_time']}"),
                array('v' => $row['base_ff8d_rssi']),
                array('v' => $row['base_fe95_rssi']),
                array('v' => $row['base_ebc0_rssi']),
                array('v' => $row['base_b70e_rssi']),
                array('v' => $row['base_1702_rssi']),
            );
        }
        return $array;
    }

    /*
     * for debuging shows data from each base station
     * 
     */

    public function oneCowLatestDataChart($bovine_id) {
$js = <<<JS
<script type="text/javascript">

            function drawChart_oneCowLatestData() {
                var jsonData = $.ajax({
                    url: "{$GLOBALS['config']['HTTP']['JSON_API']}140/oneCowLatestData/$bovine_id)",
                                dataType: "json",
                                async: false
                            }).responseText;

                            // Create our data table out of JSON data loaded from server.
                            var data = new google.visualization.DataTable(jsonData);

                            // Instantiate and draw our chart, passing in some options.
                            var chart = new google.visualization.LineChart(document.getElementById('div_chart_oneCowLatestData'));
                            chart.draw(data, {chartArea: {left: 40, top: 20}, width: 1600, height: 400, pointSize: 2, lineWidth: 0, vAxis: {viewWindow: {
                                        min: -120,
                                        max: -60
                                    }}});
                        }

                        // Set a callback to run when the Google Visualization API is loaded.
                        google.charts.setOnLoadCallback(drawChart_oneCowLatestData, true);
                        $(window).resize(function () {
                            google.charts.setOnLoadCallback(drawChart_oneCowLatestData);
                        });

        </script>

        <div id="div_chart_oneCowLatestData" ></div>
JS;
    
       return $js;
    }

    public function oneCowLatestDataFiltered($request) {


        if (!empty($request['item'])) {
            if (is_numeric($request['item'])) {
                $bovine_id = $request['item'];
            }
        } else {
            throw new Exception('Error: no bovine id specified');
        }



        $array = array();
        $sql = " SELECT ble_tag_event_filtered.tag_id,	ble_tag_event_filtered.event_time,	base_ff8d_rssi	,base_fe95_rssi,	base_ebc0_rssi	,base_b70e_rssi,base_1702_rssi
FROM bas.ble_tag_event_filtered
LEFT JOIN bas.ble_bovine_tag ON ble_bovine_tag.tag_id=ble_tag_event_filtered.tag_id
LEFT JOIN bovinemanagement.bovine ON bovine.id=ble_bovine_tag.bovine_id
 WHERE bovine_id=$bovine_id
 ORDER BY event_time DESC  limit 80000";



        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $d1 = new DateTime($row['event_time']);
            $d1->sub(new DateInterval('P1M')); //subtract one month for Javascript, becuase it is the poorest language....well maybe cobol is worse.
            //format: Date(Year, Month, Day, Hours, Minutes, Seconds, Milliseconds)
            $row['event_time'] = 'Date(' . $d1->format('Y,m,d,H,i,s') . ')'; //epoch plus microseconds 
        }


        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'datetime', 'label' => 'Date');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Base ff8d RSSI (dBm)'); //y axis #1
        $array['cols'][] = array('type' => 'number', 'label' => 'Base fe95 RSSI (dBm)'); //y axis #2
        $array['cols'][] = array('type' => 'number', 'label' => 'Base ebc0 RSSI (dBm)'); //y axis #3
        $array['cols'][] = array('type' => 'number', 'label' => 'Base b70e RSSI (dBm)'); //y axis #4
        $array['cols'][] = array('type' => 'number', 'label' => 'Base 1702 RSSI (dBm)'); //y axis #4

        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_time']}"),
                array('v' => $row['base_ff8d_rssi']),
                array('v' => $row['base_fe95_rssi']),
                array('v' => $row['base_ebc0_rssi']),
                array('v' => $row['base_b70e_rssi']),
                array('v' => $row['base_1702_rssi']),
            );
        }
        return $array;
    }

    function oneCowLatestDataChartFiltered($bovine_id) {
        ?>

        <!--Load the AJAX API-->


        <script type="text/javascript">

            // Load the Visualization API and the piechart package.




            function drawChartB() {
                var jsonData = $.ajax({
                    url: "<?php echo($GLOBALS['config']['HTTP']['JSON_API']); ?>140/oneCowLatestDataFiltered/<?php echo($bovine_id); ?>",
                                dataType: "json",
                                async: false
                            }).responseText;

                            // Create our data table out of JSON data loaded from server.
                            var data = new google.visualization.DataTable(jsonData);

                            // Instantiate and draw our chart, passing in some options.
                            var chart = new google.visualization.LineChart(document.getElementById('chart_divB'));
                            chart.draw(data, {chartArea: {left: 40, top: 20}, width: 1600, height: 400, pointSize: 2, lineWidth: 0, vAxis: {viewWindow: {
                                        min: -120,
                                        max: -60
                                    }}});
                        }

                        // Set a callback to run when the Google Visualization API is loaded.
                        google.charts.setOnLoadCallback(drawChartB, true);
                        $(window).resize(function () {
                            google.charts.setOnLoadCallback(drawChartB);
                        });

        </script>

        <div id="chart_divB"></div>
        <?php
    }

    public function oneCowLatestDataFilteredVar($request) {


        if (!empty($request['item'])) {
            if (is_numeric($request['item'])) {
                $bovine_id = $request['item'];
            }
        } else {
            throw new Exception('Error: no bovine id specified');
        }



        $array = array();
        //$sql = "SELECT event_time,sum_var  FROM bas.ble_tag_event_variance_final   WHERE bovine_id=$bovine_id AND event_time >( now() - interval '3 weeks'))) as sum_var FROM bas.ble_tag_event_variance_all  WHERE bovine_id=$bovine_id ORDER BY event_time DESC  limit 20000";
        $sql = "SELECT ble_tag_event_filtered_variance_final.event_time,ble_tag_event_filtered_variance_final.sum_var FROM bas.ble_tag_event_filtered_variance_final 
LEFT JOIN bas.ble_bovine_tag_curr ON ble_bovine_tag_curr.tag_id=ble_tag_event_filtered_variance_final.tag_id WHERE bovine_id=$bovine_id AND ble_tag_event_filtered_variance_final.event_time > (now()- interval '90 days')";



        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $d1 = new DateTime($row['event_time']);
            $d1->sub(new DateInterval('P1M')); //subtract one month for Javascript, becuase it is the poorest language....well maybe cobol is worse.
            //format: Date(Year, Month, Day, Hours, Minutes, Seconds, Milliseconds)
            $row['event_time'] = 'Date(' . $d1->format('Y,m,d,H,i,s') . ')'; //epoch plus microseconds 
            $row['sum_var'] = round($row['sum_var'], 2);
        }


        /* put repro events on same chart */
        ///////// 2nd SQL query for same visulization /////////
        $sql = "SELECT * FROM bovinemanagement.reproduction_events($bovine_id) WHERE event_time > (now() - interval '90 days')";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results2 = $statement->fetchAll(PDO::FETCH_ASSOC);
        //change presentation
        foreach ($results2 as &$row2) {
            $d1 = new DateTime($row2['event_time']);
            $d1->sub(new DateInterval('P1M')); //subtract one month for Javascript, becuase it is the poorest language....well maybe cobol is worse.
            //format: Date(Year, Month, Day, Hours, Minutes, Seconds, Milliseconds)
            $row2['event_time2'] = 'Date(' . $d1->format('Y,m,d,H,i,s') . ')'; //epoch plus microseconds 
            $row2['data'] = 1000;
        }




        //make google viz php format.
        $array['cols'][] = array('id' => '', 'type' => 'datetime', 'label' => 'Date');   //x axis
        $array['cols'][] = array('type' => 'number', 'label' => 'Variance'); //y axis #1


        foreach ($results as &$row) {
            $array['rows'][]['c'] = array(
                array('v' => "{$row['event_time']}"),
                array('v' => $row['sum_var'])
            );
        }
        return $array;
    }

    function oneCowLatestDataChartFilteredVar($bovine_id) {
        
$js = <<<JS
  <script type="text/javascript">

            // Load the Visualization API and the piechart package.
            function drawChartB() {
                var jsonData = $.ajax({
                    url: "{$GLOBALS['config']['HTTP']['JSON_API']}140/oneCowLatestDataFilteredVar/$bovine_id",
                                dataType: "json",
                                async: false
                            }).responseText;

                            // Create our data table out of JSON data loaded from server.
                            var data = new google.visualization.DataTable(jsonData);

                            // Instantiate and draw our chart, passing in some options.
                            var chart = new google.visualization.LineChart(document.getElementById('chart_divB'));
                            chart.draw(data, {chartArea: {left: 40, top: 20}, width: 1600, height: 400, pointSize: 2, lineWidth: 1, vAxis: {viewWindow: {
                                    }}});
                        }

                        // Set a callback to run when the Google Visualization API is loaded.
                        google.charts.setOnLoadCallback(drawChartB, true);
                        $(window).resize(function () {
                            google.charts.setOnLoadCallback(drawChartB);
                        });
        </script>

        <div id="chart_divB" ></div>
JS;
      return $js;
    }

    /*
     * 
     * checks that base stations are up.
     */

    public function checkBaseStationsWorking() {
        $errorSub = array();

        $baseArr = array('ff8d', 'fe95', 'ebc0', 'b70e', '1702');

        foreach ($baseArr as $key => $value) {

            
            //check if there is no data, if we dont do this we get divide by zero below.
            $sql0 = "SELECT count(*) as null_count from (SELECT tag_id FROM bas.all_tags_latest_event WHERE  base_{$value}_rssi is null ) as null_count";
            $res0 = $GLOBALS['pdo']->query($sql0);
            $row0 = $res0->fetch();
            if (($row0['null_count']) == 0) {
                $errorSub[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 1,$value . " Base station $value not working.");
            } else {
                $sql = "
with temp as (
SELECT count(*) as null_count from (SELECT tag_id FROM bas.all_tags_latest_event WHERE  base_{$value}_rssi is null ) as null_count
)
SELECT 100-null_count/(SELECT count(*) from (SELECT tag_id FROM bas.all_tags_latest_event) as d)*100  as answer,'$value' as base FROM temp
";
                $res = $GLOBALS['pdo']->query($sql);
                $row = $res->fetch();
                if (($row['answer']) == 0) {

                    $errorSub[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 1, $row['base'] . " Base station {$row['base']} not working.");
                }
            }
        }
        return $errorSub;
    }

    //passes along error info.
    public function error() {
        $error = array();

        $error = self::checkBaseStationsWorking();


        return $error;
    }

}

//end class



class Trilateration {

    //correction from autocad to svg coordinates
    //top right corner of barn is 0,0 in SVG and following in autocad.
    //3045.0 x         -32208.75 y
    private $autocadCorrX = 3045.0;
    private $autocadCorrY = -32208.75;

    public function oneCow($bovine_id) {
        $outputObj = $this->calculateLocationCow($bovine_id);
        $barnSVG = new barnSVG;
        print($barnSVG->main($outputObj));
    }

    public function allCow() {

        $sql2 = "SELECT local_number,tag_id FROM bas.all_tags_latest_event  ";
        $res2 = $GLOBALS['pdo']->query($sql2);
        $outputObjArr = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            try {

                if (empty($row2['local_number'])) {
                    $label = $row2['tag_id'];
                } else {
                    $label = $row2['local_number'];
                }
                $outputObjArr[] = $this->calculateLocationCore($row2['tag_id'], $label);
            } catch (\Exception $e) {
                //var_dump($e->getMessage());
                //show nothing.
            }
        }

        $barnSVG = new barnSVG;
        print( $barnSVG->main($outputObjArr));
    }

    /*
     * use bovine_id to find current tag_id and then location.
     */

    private function calculateLocationCow($bovine_id) {
        $sql2 = "SELECT tag_id,local_number FROM bas.ble_bovine_tag_curr LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id WHERE bovine_id=$bovine_id";
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        return $this->calculateLocationCore($row2['tag_id'], $row2['local_number']);
    }

    /*
     * core code, takes tag_id and calculates its location, label is used for text to display on map.
     * 
     */

    private function calculateLocationCore($tag_id, $label) {

        //return null when no tag is supplied.
        if (empty($tag_id)) {
            return null;
        }

        //findout if we have valid data.
        $sql2 = "SELECT * FROM bas.all_tags_latest_event WHERE tag_id='$tag_id' limit 1";
        $res2 = $GLOBALS['pdo']->query($sql2);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);


        //check data is valid and available.   
        if ((is_numeric($row2['base_ff8d_rssi']) != true) OR ( is_numeric($row2['base_fe95_rssi']) != true) OR ( is_numeric($row2['base_ebc0_rssi']) != true) OR ( is_numeric($row2['base_b70e_rssi']) != true) OR ( is_numeric($row2['base_1702_rssi']) != true)) {
            throw new Exception("Data not valid to perform location calculation for $label.");
        }



        //b8:27:eb:cf:fe:95
        $sql = "SELECT x,y FROM bas.ble_base WHERE base_id='b8:27:eb:cf:fe:95'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        // 
        $base['fe95'] = new stdClass;
        $base['fe95']->x = $row['x'];
        $base['fe95']->y = $row['y'];

        //b8:27:eb:38:ff:8d	
        $sql = "SELECT x,y FROM bas.ble_base WHERE base_id='b8:27:eb:38:ff:8d'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        // 
        $base['ff8d'] = new stdClass;
        $base['ff8d']->x = $row['x'];
        $base['ff8d']->y = $row['y'];

        //b8:27:eb:06:eb:c0	
        $sql = "SELECT x,y FROM bas.ble_base WHERE base_id='b8:27:eb:06:eb:c0'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        // 
        $base['ebc0'] = new stdClass;
        $base['ebc0']->x = $row['x'];
        $base['ebc0']->y = $row['y'];

        //b8:27:eb:5c:b7:0e	
        $sql = "SELECT x,y FROM bas.ble_base WHERE base_id='b8:27:eb:5c:b7:0e'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        // 
        $base['b70e'] = new stdClass;
        $base['b70e']->x = $row['x'];
        $base['b70e']->y = $row['y'];

        //b8:27:eb:80:17:02		
        $sql = "SELECT x,y FROM bas.ble_base WHERE base_id='b8:27:eb:80:17:02'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        // 
        $base['1702'] = new stdClass;
        $base['1702']->x = $row['x'];
        $base['1702']->y = $row['y'];


        //// end of base stations....
        //do rssi portion and convert to distance feet

        $base['ff8d']->dist = $this->freeSpaceLoss($row2['base_ff8d_rssi']);
        $base['fe95']->dist = $this->freeSpaceLoss($row2['base_fe95_rssi']);
        $base['ebc0']->dist = $this->freeSpaceLoss($row2['base_ebc0_rssi']);
        $base['b70e']->dist = $this->freeSpaceLoss($row2['base_b70e_rssi']);
        $base['1702']->dist = $this->freeSpaceLoss($row2['base_1702_rssi']);

        //create input arrays
        $arr_ff8d = [$base['ff8d']->x, $base['ff8d']->y, $base['ff8d']->dist];
        $arr_fe95 = [$base['fe95']->x, $base['fe95']->y, $base['fe95']->dist];
        $arr_ebc0 = [$base['ebc0']->x, $base['ebc0']->y, $base['ebc0']->dist];
        $arr_b70e = [$base['b70e']->x, $base['b70e']->y, $base['b70e']->dist];
        $arr_1702 = [$base['1702']->x, $base['1702']->y, $base['1702']->dist];


        //do all combinations.                    
        $out[0] = $this->trilaterationAlgorithm($arr_ff8d, $arr_fe95, $arr_ebc0);
        $out[1] = $this->trilaterationAlgorithm($arr_ff8d, $arr_fe95, $arr_b70e);
        $out[2] = $this->trilaterationAlgorithm($arr_b70e, $arr_ebc0, $arr_fe95);
        $out[3] = $this->trilaterationAlgorithm($arr_b70e, $arr_ebc0, $arr_ff8d);
        //now 1702 with everything.
        $out[4] = $this->trilaterationAlgorithm($arr_1702, $arr_b70e, $arr_ebc0);
        $out[5] = $this->trilaterationAlgorithm($arr_1702, $arr_b70e, $arr_fe95);
        $out[6] = $this->trilaterationAlgorithm($arr_1702, $arr_b70e, $arr_ff8d);
        $out[7] = $this->trilaterationAlgorithm($arr_1702, $arr_ebc0, $arr_fe95);
        $out[8] = $this->trilaterationAlgorithm($arr_1702, $arr_ebc0, $arr_ff8d);
        $out[9] = $this->trilaterationAlgorithm($arr_1702, $arr_fe95, $arr_ff8d);
        // 10 combinations???
        //use array map to find average of first and second element of multi-dimensional array
        $x_elements = array_map(function($i) {
            return $i[0];
        }, $out);
        $outAvg[0] = array_sum($x_elements) / count($x_elements);
        $y_elements = array_map(function($i) {
            return $i[1];
        }, $out);
        $outAvg[1] = array_sum($y_elements) / count($y_elements);


        //do correction for coordinate systems
        $outB[0] = $outAvg[0] - $this->autocadCorrX;
        $outB[1] = ($outAvg[1] - $this->autocadCorrY) * -1;

        //debug
        //  var_dump($outB);
        //  
        //final output
        $finalOutput = new stdClass();
        $finalOutput->label = $label;
        $finalOutput->x = $outB[0];
        $finalOutput->y = $outB[1];

        return $finalOutput;
    }

//this is a crap shoot, use calibration tags to determind this accuractely.
    private function freeSpaceLoss($rssi) {
        //base receiver is 60 dBm at 0 meters, 
        $db = ($rssi + 40);

        //bluetooth mid frequency is 2439 Mhz
        ////from wikipedia: 
        //
         //
         //Free space loss (dB) = -27.55 + 20Log10(Frequency Mhz) + 20Log10(Distance in meters)


        $distance = -1 * ( 10 ^ ($db / 20) + 2439 + 10 ^ (-27.55 / 20) );
        //  return $distance;
        //needs to be in feet.
        $a = 10 ^ (abs($db) / 20) * 80;
        //print("Distance Magnitude: " . $a . " <br>");
        return $a;
        // return abs(($rssi+60)*8);
    }

    //based on: http://stackoverflow.com/questions/29656921/trilateration-2d-algorithm-implementation which is based on wikipedia
    private function trilaterationAlgorithm($array1, $array2, $array3) {


        $point1 = [$array1[0], $array1[1]];
        $r1 = $array1[2];
        $point2 = [$array2[0], $array2[1]];
        $r2 = $array2[2];
        $point3 = [$array3[0], $array3[1]];
        $r3 = $array3[2];

        //algorithm inputs
        // $point1,  $point2,  $point3,  $r1,  $r2,  $r3
        /*
          // test input
          $point1 = [4.0,4.0];
          $point2 = [9.0,7.0];
          $point3 = [9.0,1.0];
          $r1 = 4.0;
          $r2 = 3.0;
          $r3 = 3.25;
          // output:
          0 =>  8.021875
          1 =>  4.1302083333333
         */

        //unit vector in a direction from $point1 to point 2
        $p2p1Distance = pow(pow($point2[0] - $point1[0], 2) + pow($point2[1] - $point1[1], 2), 0.5);
        $ex = [($point2[0] - $point1[0]) / $p2p1Distance, ($point2[1] - $point1[1]) / $p2p1Distance];
        $aux = [$point3[0] - $point1[0], $point3[1] - $point1[1]];
        //signed magnitude of the x component
        $i = $ex[0] * $aux[0] + $ex[1] * $aux[1];
        //the unit vector in the y direction. 
        $aux2 = [$point3[0] - $point1[0] - $i * $ex[0], $point3[1] - $point1[1] - $i * $ex[1]];
        $norm_aux2 = pow(pow($aux2[0], 2) + pow($aux2[1], 2), .5);
        $ey = [$aux2[0] / $norm_aux2, $aux2[1] / $norm_aux2];
        //the signed magnitude of the y component
        $j = $ey[0] * $aux[0] + $ey[1] * $aux[1];
        //coordinates
        $x = (pow($r1, 2) - pow($r2, 2) + pow($p2p1Distance, 2)) / (2 * $p2p1Distance);
        $y = (pow($r1, 2) - pow($r3, 2) + pow($i, 2) + pow($j, 2)) / (2 * $j) - $i * $x / $j;
        //result coordinates
        $finalX = $point1[0] + $x * $ex[0] + $y * $ey[0];
        $finalY = $point1[1] + $x * $ex[1] + $y * $ey[1];
        $resultPose[0] = $finalX;
        $resultPose[1] = $finalY;
        return $resultPose;
    }

}

//end class
//draw svg of barn
class barnSVG {

    public function main($objArray) {

        //we are in a multiple cows on map situation
        if (is_array($objArray) == true) {

            $str = '';
            $str = $str . $this->svgTop('large');
            foreach ($objArray as $key => $value) {

                $str = $str . $this->svgOverlayDetailed($value->label, $value->x, $value->y);
            }
            $str = $str . $this->svgBottom();

            return $str;
        }
        //just a single object
        else {
            return($this->svgTop('small') . $this->svgOverlayRed($objArray->label, $objArray->x, $objArray->y) . $this->svgBottom());
        }
    }

    function svgTop($size = 'small') {

        if ($size == 'small') {
            $scale = 1;
            $width = 380;
            $height = 260;
        } else {
            $scale = 3;
            $width = 1380;
            $height = 1260;
        }

        $svgTop = <<<EX
        <svg width="$width" height="$height">
         <g transform="scale($scale)">
        
        <!--main barn square -->
        <line x1='0' y1='0' x2='0' y2='123' stroke = 'red' stroke-width = '1'/>        
         <line x1='337' y1='0' x2='337' y2='123' stroke = 'black' stroke-width = '1'/>
         <line x1='0' y1='0' x2='337' y2='0' stroke = 'black' stroke-width = '1'/>
        
         <line x1='0' y1='123' x2='158' y2='123' stroke = 'black' stroke-width = '1'/>
         <line x1='172' y1='123' x2='337' y2='123' stroke = 'black' stroke-width = '1'/>
        
        <!-- divide up barn into squares -->
        
        <!--centre vertical -->
        <line x1='158' y1='0' x2='158' y2='123' stroke = 'grey' stroke-width = '1'/> 
        <line x1='172' y1='0' x2='172' y2='123' stroke = 'grey' stroke-width = '1'/> 
        
        <!--other vertical -->
         <line x1='82.5' y1='0' x2='82.5' y2='58.5' stroke = 'grey' stroke-width = '1'/> 
        <line x1='82.5' y1='61.5' x2='82.5' y2='123' stroke = 'grey' stroke-width = '1'/> 
        
        <line x1='247.5' y1='0' x2='247.5' y2='58.5' stroke = 'grey' stroke-width = '1'/> 
         <line x1='247.5' y1='61.5' x2='247.5' y2='123' stroke = 'grey' stroke-width = '1'/> 
        
        <!--horizontal -->
        <line x1='0' y1='58.5' x2='158' y2='58.5' stroke = 'grey' stroke-width = '1'/>    
        <line x1='172' y1='58.5' x2='337' y2='58.5' stroke = 'grey' stroke-width = '1'/>  
         <line x1='0' y1='61.5' x2='158' y2='61.5' stroke = 'grey' stroke-width = '1'/>    
        <line x1='172' y1='61.5' x2='337' y2='61.5' stroke = 'grey' stroke-width = '1'/>  
        
        <!-- breezeway -->
         <line x1='158' y1='123' x2='158' y2='186' stroke = 'black' stroke-width = '1'/>    
         <line x1='172' y1='123' x2='172' y2='233' stroke = 'black' stroke-width = '1'/>   
            <!-- parlor -->
         <line x1='158' y1='186' x2='50' y2='186' stroke = 'black' stroke-width = '1'/>   
         <line x1='50' y1='186' x2='50' y2='233' stroke = 'black' stroke-width = '1'/>   
         <line x1='50' y1='233' x2='172' y2='233' stroke = 'black' stroke-width = '1'/>   
        
        <!-- text -->
          <text x="35" y="35" fill="grey">Dry</text>
         <text x="85" y="35" fill="grey">Low</text>
        
          <text x="15" y="95" fill="grey">Pack</text>
         <text x="105" y="80" fill="grey">Sick</text>
        <text x="105" y="115" fill="grey">Sort</text>
        
         <text x="230" y="35" fill="grey">High</text>
         <text x="225" y="95" fill="grey">Heifer</text>
        
     
        

EX;
        return $svgTop;
    }

    function svgBottom() {
        $svgBottom = <<<EX
  
  </g>
  Sorry, your browser does not support inline SVG.
</svg>

EX;
        return $svgBottom;
    }

    function svgOverlayRed($label, $location_x, $location_y) {


        $svgOverlay = <<<EX
     <!-- location -->
        <circle cx="$location_x" cy="$location_y" r="10" stroke-width="3" fill="red">
         <animate
            attributeType="XML"
            attributeName="fill"
            values="#800;#f00;#800;#800"
            dur="0.8s"
            repeatCount="indefinite"/>
        </circle>
  <text  text-anchor="middle"  x="$location_x" y="$location_y" style="fill-opacity=0.5; fill: #000000; stroke: none; font-size: .5em;">{$label}</text>    
EX;
        return $svgOverlay;
    }

    function svgOverlayDetailed($label, $location_x, $location_y) {


        $svgOverlay = <<<EX
     <!-- location -->
               
        <circle cx="$location_x" cy="$location_y" r="5" stroke-width="3" fill="green" fill-opacity="0.5">
           
        
        </circle>
   <text  text-anchor="middle"  x="$location_x" y="$location_y" style="fill-opacity=0.5; fill: #000000; stroke: none; font-size: .25em;">{$label}</text>    
EX;
        return $svgOverlay;
    }

//passes along error info.
    public function error() {
        $error = array();


        $sql = "SELECT max(event_time) as max from bas.ble_tag_event_filtered";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (strottime($row['max']) <= strtotime('-1 day')) {
            $error[] = new notifyObj(get_class($this), $row['max'], 2, 'Estrus Detector script has not run.');
        }

        $sql2 = "SELECT max(event_time) from bas.ble_tag_event_filtered_variance_final";
        $res = $GLOBALS['pdo']->query($sql2);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (strottime($row['max']) <= strtotime('-2 day')) {
            $error[] = new notifyObj(get_class($this), $row['max'], 2, 'Estrus Detector final script has not run.');
        }

        return $error;
    }

}

//end class
?>
