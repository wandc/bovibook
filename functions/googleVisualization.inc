<?php 
/* makes it easier to use google visulization line charts. */




/**
 * Used by basepage to load any needed visulaization for all pages.  
 * must be called at top of page, so not within jquery tab for callbacks to work.
 */
class GoogleVisGlobal {

    static public function loadVisualization() {
        return "
             <script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>
        <script type='text/javascript'>
         google.charts.load('current', {packages: ['calendar','corechart','orgchart','table']});
        </script>	
        ";
    }

}

/**
 * Google Visualization base common class other extend form this.
 *  
 */
class GoogleVisCommon {

    public $uuid; //so charts are uniquely identified and seperated.
    protected $headerStrArr; //need by data method and header
    public $output;

    public function __construct() {
        $this->uuid = uniqid();
        $chartType = 'line';
    }

    public function toString() {
        return $this->output;
    }

    /**
     * Main method called by constructor.
     * 
     * @param type $headerStrArr
     * @param type $dimArray
     * @param type $dimArray2
     * @param type $dimArray3
     * @param type $dimArray4
     * @return string 
     */
    public function main($chartType, $options = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {

        $dataArr = array();
        //throw arrays at it, with key values being the x-axis.
        $dataArr[] = $dimArray;
        if ($dimArray2 != null) {
            $dataArr[] = $dimArray2;
        }
        if ($dimArray3 != null) {
            $dataArr[] = $dimArray3;
        }
        if ($dimArray4 != null) {
            $dataArr[] = $dimArray4;
        }


        //put it all together....

        $str = '';

        $str = $str . $this->header($headerStrArr);


        $str = $str . $this->data($dataArr);

        $str = $str . $this->options($options, $chartType);

        $str = $str . $this->htmlDiv(1400, 300);


        //return the output to be printed elswhere.
        $this->output = $str;
        return $str;
    }

    /**
     * not used anymore, called at top of every page with GoogleVisCommon in basepage
     */
    public function headerJS() {
        $str = '';
        return $str;
    }

    /*
     * header code needed for eeach table.
     */

    public function header($headerStrArr) {
        $str = '';
        $this->headerStrArr = $headerStrArr;

        $str = $str . ("  
         <script type='text/javascript'>  " . "\n\r");


        $str = $str . ("
                /* make data and chart global for ajax interactions */
                var data_{$this->uuid};
                var chart_{$this->uuid};
            function drawVisualization_{$this->uuid}() {
                // Create and populate the data table.
                data_{$this->uuid} = new google.visualization.DataTable();" . "\n\r");

        //of the form: BACKWARDS, so array key not overwritten
        //key=name of coulmn, value=type 
        //
         // $headerStrArr['xAxis']='number';
        //
         foreach ($headerStrArr as $key => $val) {
            $str = $str . ("\t\t" . "data_{$this->uuid}.addColumn('$val', '$key');" . "\n\r");  //print out each column header js             
        }

        return $str;
    }

    /**
     * data for graph 
     */
    public function data(array $dataArr, array $dataArr2=null) {
        $str = '';

        if (count($dataArr[0]) > 0) {

            /**
             * provide arrays of X an Y values $key=X and $value=Y.
             * this code then has to create a 2d array based on that.
             * then go through array and turn all blanks to word null.
             */
            //make 2d array with first dimension key as X-values, 
            //then second dimension key as column number 
            $dataArrNew = array();
            for ($i = 0; $i < count($dataArr); $i++) {
                foreach ($dataArr[$i] as $key => $value) {
                    $dataArrNew[$key][$i] = $value;
                }
            }

            //now go through the new array and change any blanks to nulls.
            foreach ($dataArrNew as $key => $value) {
                //note: must do count on old array $dataArr, to always get all dimensions.       
                for ($x = 0; $x < count($dataArr); $x++) {
                    if ((isset($dataArrNew[$key][$x]) == false) || ($dataArrNew[$key][$x] == '') || ($dataArrNew[$key][$x] == null)) {
                        $dataArrNew[$key][$x] = 'null';
                    }
                }
            }

            //now go through the last time and print out a row string.  
            $dataStr = "\n\r";
            foreach ($dataArrNew as $key => $value) {
                $dataStr = $dataStr . "data_{$this->uuid}.addRow([";
                for ($x = 0; $x < count($value); $x++) {

                    //when first one, put the x-axis value ($key)
                    if ($x == 0) {
                        //see if the x axis is date format and if so, process it.
                        if (reset($this->headerStrArr) == 'date') {
                            $dataStr = $dataStr . "new Date('" . date('m/d/Y', $key) . "'),";
                        } elseif (reset($this->headerStrArr) == 'string') {
                            $dataStr = $dataStr . "'$key'" . ',';
                        } else {
                            $dataStr = $dataStr . $key . ',';
                        }
                    }


                    //normally
                    if (count($value) - 1 != $x) {
                        $dataStr = $dataStr . $dataArrNew[$key][$x] . ",";
                    }
                    //when it is the last value, don't put the trailing comma.
                    else {
                        $dataStr = $dataStr . $dataArrNew[$key][$x];
                    }
                }
                $dataStr = $dataStr . "]);" . "\n\r";
            }

            //output final result for data rows
            $str = $str . ($dataStr);
        }
        return $str;
    }

    public function options($customOptions = null, $chartType) {

        //make more generic by supporting differnet chart types.
        switch ($chartType) {
            case 'line':
                $type = 'LineChart';
                break;
            case 'bar':
                $type = 'BarChart';
                break;
            case 'column':
                $type = 'ColumnChart';
                break;
            case 'scatter':
                $type = 'ScatterChart';
                break;
            case 'pie':
                $type = 'PieChart';
                break;
            default:
                throw new Exception("Google Vis Chart: {$this->uuid} must have type.<br />");
                break;
        }


        $str = '';
        if ($customOptions == null) {
            $str = $str . ('
        var options = { curveType: "function",                  
                        chartArea: {width: "90%", height: "80%"},
                        legend: {position: "in"}
                    } ;              
        ');
        } else {
            $str = $str . ('
        var options = { curveType: "function",' . $customOptions . '                 
                    } ;              
        ' . "\n\r");
        }

        $str = $str . ("    
                    // Create and draw the visualization.
                    chart_{$this->uuid}=new google.visualization." . $type . "(document.getElementById('visualization_{$this->uuid}')).
                    draw(data_{$this->uuid}, options);
                }
             
                google.charts.setOnLoadCallback(drawVisualization_{$this->uuid},true);
                    $(window).resize(function(){
            google.charts.setOnLoadCallback(drawVisualization_{$this->uuid});
            });
          " . "\n\r");


        $str = $str . ("  </script> " . "\n\r");

        return $str;
    }

    public function htmlDiv($width = '200', $height = '200') {
        $str = '';
        $str = $str . "<div id='visualization_{$this->uuid}' style='width: $width px; height: $height px;' ></div>   " . "\n\r" . "\n\r";

        return $str;
    }

}

//end class



/**
 *
 * Generic line graphing function, the array keys all have to be the same
 * type, ie date (since epoch).
 * @param type $headerStrArr
 * @param type $dimArray
 * @param type $dimArray2
 * @param type $dimArray3
 * @param type $dimArray4 
 * 
 *  Call like this:
  print ("<h3>Chart Data1 and Data2</h3>");
  $headerStrArr = array();
  $headerStrArr['xAxis'] = 'date';
  $headerStrArr['Data1 ($)'] = 'number';
  $headerStrArr['Data2 (l)'] = 'number';
  GoogleVisLine::genericGoogleVisLineChart($headerStrArr, $dataArr1,$dataArr2);
 * 
 * 
 */
//end class


/**
  Call like this:
  $headerStrArr = array();
  $headerStrArr['xAxis'] = 'number'; //BACKWARDS
  $headerStrArr['Fat %'] = 'number';
  $headerStrArr['Protein %'] = 'number';
  $x = new GoogleVisLine($opt, $headerStrArr, $valactaFatArr,$valactaProtArr);
  print($x->toString());
 * 
 */
class GoogleVisPie extends GoogleVisCommon {

    public function __construct($options = null, $headerStrArr, $dimArray, $dimArray2, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();
        if ($options == null) {

            $options = "
                  width: 900, height: 220                                 
                  ";
            return $this->main($chartType = 'pie', $options, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
        }
    }

}

class GoogleVisLine extends GoogleVisCommon {

    public function __construct($options = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();

        return $this->main($chartType = 'line', $options, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
    }

}

class GoogleVisBar extends GoogleVisCommon {

    public function __construct($options = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();

        return $this->main($chartType = 'bar', $options, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
    }

}

class GoogleVisColumn extends GoogleVisCommon {

    public function __construct($options = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();

        return $this->main($chartType = 'column', $options, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
    }

}

//this class is custom and doesn't use much of GoogleVisCommon except uuid.
class GoogleVisSparkLine2 extends GoogleVisCommon {

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



/**
 * call like this:
 *              $headerStrArr=array();
  $headerStrArr['DIM'] = 'number';
  $headerStrArr['MUN'] = 'number';
  $x=new GoogleVisScatter(null,$headerStrArr,$dimArr,$munArr);
  print($x->toString());
 * 
 * 
 */
class GoogleVisScatter extends GoogleVisLine {

    public function __construct($options = null, $headerStrArr, $dimArray, $dimArray2) {
        $this->uuid = uniqid();

        if ($options == null) {
            $options = '
                  width: 600, height: 300,
                   vAxis: {minValue: 0},
                   hAxis: {minValue: 0},
                   chartArea: {width: "90%", height: "80%"}';
        }

        return $this->main($chartType = 'scatter', $options, $headerStrArr, $dimArray, $dimArray2);
    }

    //main function to run.

    public function main($chartType, $options = null, $headerStrArr, $dimArray, $dimArray2) {

        //takes in two data arrays with the same index, but different values (x,y)
        //put it all together....

        $str = '';

        $str = $str . $this->header($headerStrArr);


        $str = $str . $this->data($dimArray, $dimArray2);

        $str = $str . $this->options($options, $chartType);

        $str = $str . $this->htmlDiv(600, 600);


        //return the output to be printed elswhere.
        $this->output = $str;
        return $str;
    }

    //does not do a null check
    public function data(array $dataArrX, array $dataArrY) {
        $str = '';
        //$dataArrX,$dataArrY must have same key
        //now go through the last time and print out a row string.  
        $dataStr = "\n\r";
        foreach ($dataArrX as $key => $value) {
            $dataStr = $dataStr . "data_{$this->uuid}.addRow([";

            $dataStr = $dataStr . $dataArrX[$key] . "," . $dataArrY[$key];

            $dataStr = $dataStr . "]);" . "\n\r";


            //output final result for data rows
            $str = $str . ($dataStr);
        }
        return $str;
    }

}

//end class
?>