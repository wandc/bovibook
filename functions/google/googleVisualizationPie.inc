<?php

class GoogleVisualizationPie extends GoogleVisualization {

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
?>
