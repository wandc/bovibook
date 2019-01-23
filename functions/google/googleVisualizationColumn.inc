<?php
class GoogleVisualizationColumn extends GoogleVisualization {

    public function __construct($options = null, $headerStrArr, $dimArray, $dimArray2 = null, $dimArray3 = null, $dimArray4 = null) {
        $this->uuid = uniqid();

        return $this->main($chartType = 'column', $options, $headerStrArr, $dimArray, $dimArray2, $dimArray3, $dimArray4);
    }

}

?>
