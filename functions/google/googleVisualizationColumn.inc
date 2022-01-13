<?php
class GoogleVisualizationColumn extends GoogleVisualization {

    public function __construct($chartOptions = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();
        
        $objOptions=null; //add functionality if needed.

        return $this->main($chartType = 'column', $objOptions, $chartOptions, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
    }

}

?>
