<?php

class GoogleVisualizationPie extends GoogleVisualization {

    public function __construct($chartOptions = null, $headerStrArr, $dimArray, $dimArray2, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();
        
        $objOptions=null; //add functionality if needed.
            
        if ($chartOptions == null) {

            $chartOptions = "
                  width: 900, height: 220                                 
                  ";
            return $this->main($chartType = 'pie', $objOptions, $chartOptions, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
        }
    }

}
?>
