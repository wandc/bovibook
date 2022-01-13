<?php


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
  googleVisualizationLine::genericGoogleVisComboChart($headerStrArr, $dataArr1,$dataArr2);
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
  $x = new GoogleVisualizationCombo( null, $opt, $headerStrArr, $valactaFatArr,$valactaProtArr);
  print($x->toString());
 * 
 */


/*
 * NOTE: if you want a mixture of bars and lines, you maybe want a line chart with extra options, like credit exchange chart.
 */

class GoogleVisualizationCombo extends GoogleVisualization {

    public function __construct($objOptions = null, $chartOptions = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();

        
        $objOptions=null; //add functionality if needed.
        
        /* will probably have to customize, depends on where you want line vs bar, for what dataset */
        if ($chartOptions == null) {
            $chartOptions = <<<OPT
             
              seriesType: 'bars',
          series: {0: {type: 'line'}}
        OPT;
            
        }
        
        return $this->main($chartType = 'combo', $objOptions, $chartOptions, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
    }

}

?>
