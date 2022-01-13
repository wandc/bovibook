<?php 
/* makes it easier to use google visulization line charts. */


/**
 * Google Visualization base common class other extend form this.
 *  
 */
class GoogleVisualization {

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

    //used to load google charts on every page (usually).
    /**
 * Used by basepage to load any needed visulaization for all pages.  
 * must be called at top of page, so not within jquery tab for callbacks to work.
 */
    static public function loadVisualization() {
        return "
             <script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>
        <script type='text/javascript'>
         google.charts.load('current', {packages: ['calendar','corechart','orgchart','table']});
        </script>	
        ";
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
    public function main($chartType, $objOptions=null, $chartOptions = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {

               
        //configuration paramaters
        //if (isset($objOptions->xxx)) { }//if object is set, pass along.     
        
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

        $str = $str . $this->options($chartOptions, $chartType);

        $str = $str . $this->htmlDiv(1400, 300);


        //return the output to be printed elswhere.
        $this->output = $str;
        return $str;
    }

    /**
     * not used anymore, called at top of every page with GoogleVisualization in basepage
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
    public function data(array $dataArr) {
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
                            
                            $dataStr = $dataStr . 'new Date("'. date('c', $key) . '"),'; //this takes care of JS months starting at zero.
                                                 
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
            case 'combo':
                $type = 'ComboChart';
                break;
            default:
                throw new Exception("Google Vis Chart: {$this->uuid} must have type, line, bar, etc.<br />");
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
        $str = $str . "<div id='visualization_{$this->uuid}' class='googleVis' style='width: 100%; height: $height px;' ></div>   " . "\n\r" . "\n\r";

        return $str;
    }

}//end class