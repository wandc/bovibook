<?php

//this class is custom and doesn't use much of GoogleVisualization except uuid.
class GoogleVisualizationSparkLine extends GoogleVisualization {

    //uses google visulization library, ie not picture, but JS.
    public function sparklineContainerVis($text, $dataArr, $number) {

        //format data for google js DataTable. 
        $data = '';

        $numItems = count($dataArr);
        $i = 0;
        foreach ($dataArr as $v) {
            if ($i === $numItems) { //when last one, skip the comma and put red dot value
                $data = $data . " [,$v,$v, null]\n\r";
            } else {
                $data = $data . " [,$v,null, null],\n\r";
            }
            $i++;
        }

        ////////////////////////////////////////////////////
        //
           ?>        
        <script type="text/javascript" language="javascript">

            function drawVisualization_<?php  echo($this->uuid); ?>() {
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'x');
                data.addColumn('number', 'blue');
                data.addColumn('number', 'red');
                data.addColumn({type: 'string', role: 'annotation'});
                data.addRows([
        <?php  echo($data); ?>
                ]);

        // Create and draw the visualization.
                new google.visualization.LineChart(document.getElementById('visualization_<?php  echo($this->uuid); ?>')).
                        draw(data, {curveType: "function",
                            width: 120, height: 36, enableInteractivity: false, legend: 'none',
                            vAxis: {
                                baselineColor: 'transparent',
                                gridlineColor: 'transparent',
                                textPosition: 'none'
                            },
                            backgroundColor: 'transparent',
                            series: {
                                0: {pointShape: null, pointSize: 0},
                                1: {pointShape: 'circle', pointSize: 4}
                            }
                        }
                        );
            }
            ;

            $(document).ready(function () {
                google.charts.setOnLoadCallback(drawVisualization_<?php  echo($this->uuid); ?>, true);
            });
            $(window).resize(function(){
            google.charts.setOnLoadCallback(drawVisualization_<?php  echo($this->uuid); ?>);
            });
        </script>
        <?php 
        /////////////////////////////////////////////////////
        //output 
        $str = '';
        $str = $str . ("<div class='ss00'>");
        $str = $str . ("<div class='ss0'>");
        $str = $str . ("<p>$text</p>");
        $str = $str . ("<div class='ss1'>");

        $str = $str . ("<div id='visualization_{$this->uuid}' style='width: 120px; height: 30px;'></div>");
        $str = $str . ("</div>");

        $str = $str . ("<div class='ss2'>");
        $str = $str . ("$number");
        $str = $str . ("</div>");

        $str = $str . ("</div>");
        $str = $str . ("</div>");
        return $str;
    }

}

//end class
?>
