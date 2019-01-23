<?php

/**
 * call like this:
 *              $headerStrArr=array();
  $headerStrArr['DIM'] = 'number';
  $headerStrArr['MUN'] = 'number';
  $x=new googleVisualizationScatter(null,$headerStrArr,$dimArr,$munArr);
  print($x->toString());
 * 
 * 
 */
class GoogleVisualizationScatter extends GoogleVisLine {

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
