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
        $js = <<<JS
<script type="text/javascript" language="javascript">

            function drawVisualization_{$this->uuid}() {
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'x');
                data.addColumn('number', 'blue');
                data.addColumn('number', 'red');
                data.addColumn({type: 'string', role: 'annotation'});
                data.addRows([
                                {$data}
                ]);

        // Create and draw the visualization.
                new google.visualization.LineChart(document.getElementById('visualization_{$this->uuid}')).
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
                google.charts.setOnLoadCallback(drawVisualization_{$this->uuid}, true);
            });
            $(window).resize(function(){
            google.charts.setOnLoadCallback(drawVisualization_{$this->uuid});
            
            });
        </script>
JS;
           ?>        
        
        <?php 
        /////////////////////////////////////////////////////
        //output 
        $out[]= $js;
        $out[]=  ("<div class='ss00'>");
        $out[]=  ("<div class='ss0'>");
        $out[]=  ("<p>$text</p>");
        $out[]=  ("<div class='ss1'>");

        $out[]=  ("<div id='visualization_{$this->uuid}' class='googleVisSparkline' style='left:1em; width: 12em; height: 2em;'></div>");
        $out[]=  ("</div>");

        $out[]=  ("<div class='ss2'>");
        $out[]=  ("$number");
        $out[]=  ("</div>");

        $out[]=  ("</div>");
        $out[]=  ("</div>");
        
        
        return implode($out);
    }

}

//end class
?>
