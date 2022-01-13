<?php

/**
 * Lists soil samples for fields and allows us to enter new ones *
 */
class SoilSample extends TabPage {

    public function defaultDisplay() {

        $fertR = new FertilzerRequirements();
        $fertR->test();

        //setup for tabs.
        $tabArray[1]['name'] = 'Soil Sample';
        $tabArray[2]['name'] = 'Plant Tissue Test';
        $tabArray[3]['name'] = 'Soil Sample Needed';
        $this->renderTabs($tabArray);
    }

    protected function tab1() {

        $note = (new BootStrap)->infoNotify("Note: PEI lab uses Phosphate and Potash (oxidated) and not P and K like NB lab use to. We now enter things as Phosphate and Potash and the software converts to P and K and uses P and K from then on.");

        $html = <<<HTML
    <div class='row'>
        <div class='col-sm-2 col-sm-push-10'>                
            {$this->soilTestQuickForm()}
        </div>
        <div class='col-sm-10 col-sm-pull-2'> 
            $note
            {$this->P_Phosphorus_Calibration_Olsen(17)}
            {$this->displaySoilSamples('default')}
        </div>
    </div>    
HTML;

        print($html);
    }

    protected function tab2() {
        $xxx = new PlantTissue();
        $xxx->plantTissueTestQuickForm();
        print($xxx->displayPlantTissueSamples('default'));
    }

    protected function tab3() {
        $this->displaySoilTestNeeded();
    }

    function displaySoilTestNeeded() {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                           
            {
                data: "alpha_numeric_id",
                title: "alpha_numeric_id",
                type: "num-html"
            },
            {
                data: "common_name",
                title: "Current Location"
            },
            {
                data: "area",
                title: "area"
        
            },
            {
                data: "last_soil_test",
                title: "last_soil_test Time"
            },
            {
                data: "last_fertilizer_event",
                title: "last_fertilizer_event"
            },
             {
                data: "spfh_suitable",
                title: "spfh_suitable"
           }
EX;
        print($dt->start('Needs Soil Test', 59, 'soilTestNeeded', null, $columnsStr));

        //    print($dt->startLocal('Title Local CHANGE ME', $this->AllHeats7Days(''),$columnsStr,null,' "paging": false'));     
    }

    // called by displaySingleSoilTest
    public function displaySingleSoilSampleFancy($obj) {
        $soilSampleTable = self::soilSampleTable($obj);
        $soilSampleColumnChart = self::soilSampleColumnChart($obj);
        $baseSaturationTable = self::baseSaturationTable($obj);
        $baseSaturationChart = self::baseSaturationChart($obj);

        $html = <<<HTML

        <style type = "text/css">
            #soilContainer {
                position:relative;
                width:900px;
                height:400px;
                /*border: #cccccc solid 1px;  */
            }

            #soilColumnChart {
                position:absolute;
                top:90px;
                left:0;
            }    

            #soilBaseSatChart {
                position:absolute;
                left:585px;
                width:180px;

            }

            #pieChart2 {

            }

            #baseSaturationTable {
                font-size:.8em;
                margin-top: 5px;
                    
                        /* border: #cccccc solid 1px; */
                        text-align: center;
                        background: #FCFCFC;

            }

        </style>
        <div id="soilContainer">
        {$soilSampleTable}
             <div id="soilColumnChart">
                {$soilSampleColumnChart}
            </div>
            <div id="soilBaseSatChart">
                {$baseSaturationTable}
                {$baseSaturationChart}
            </div>
        </div>
    HTML;


        return (new BootStrap())->plainCard('Latest Soil Sample', $html);
    }

    function baseSaturationPieChart($K, $Mg, $Ca, $H, $Na) {

        $one = array(0 => "K", 1 => "Mg", 2 => "Ca", 3 => "H", 4 => "Na");
        $two = array(0 => 10, 1 => 20, 2 => 30, 3 => 40, 4 => 50);

        $headerStrArr = array();
        $headerStrArr['Element'] = 'number'; //BACKWARDS
        $headerStrArr['Percent'] = 'number';
        $x = new GoogleVisualizationPie($opt, $headerStrArr, $one, $two);
        return ($x->toString());
    }

    function baseSaturationChart($stdObj) {

        $js = <<<JS
          <script type="text/javascript">
            function drawChart2() {
                var data2 = google.visualization.arrayToDataTable([
                    ['Element', 'ppm'],
                    ['K', {$stdObj->k_base_sat}],
                    ['Mg', {$stdObj->mg_base_sat}],
                    ['Ca', {$stdObj->ca_base_sat}],
                    ['Na', {$stdObj->na_base_sat}],
                    ['H', {$stdObj->h_base_sat}]
                ]);

                var options2 = {
                    title: 'Base Saturation',
                    backgroundColor: 'FCFCFC',
                    chartArea: {left: 10, top: 20, width: "85%", height: "85%"}
                };

                var chart2 = new google.visualization.PieChart(document.getElementById('piechart2'));
                chart2.draw(data2, options2);
            }
            setTimeout(drawChart2, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart2, true);
             $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart2);
            });
        </script><div id="piechart2" style="width: 180px; height: 155px;"></div>
    JS;
        return $js;
    }

    function baseSaturationTable($stdObj) {
        $html = <<<HTML
        <div id="baseSaturationTable">
            <li><b>CEC: </b>{$stdObj->cec} meq/100g</li> 

            <li><b>K: </b>{$stdObj->k_base_sat}%</li>
            <li><b>Mg: </b>{$stdObj->mg_base_sat}%</li>
            <li><b>Ca: </b>{$stdObj->ca_base_sat}%</li>
            <li><b>Na: </b> {$stdObj->na_base_sat}%</li> 
            <li><b>H: </b> {$stdObj->h_base_sat}%</li> 
            <li><b>Total Base Sat: </b> {$stdObj->total_base_sat}%</li> 
        </div>  
        HTML;
        return $html;
    }

    function soilSampleTable($stdObj) {
        $date = date('M d, Y', strtotime($stdObj->event_time));
        $caMg = round($stdObj->ca / $stdObj->mg, 1);
        $pAl = round($stdObj->p / $stdObj->al, 2);
        $mgK = round($stdObj->mg / $stdObj->k, 2);

        $html = <<<HTML

        <style type = "text/css">
            .soilsampletable  {
                width:765px; border:1px solid black; font-size: 1.2em; background: #c4e4ea;  
            }

            table.soilsampletable,table.soilsampletable td,table.soilsampletable th {
                border-right:1px solid;
                margin:0;
                padding:0;
            } 
            .symbol_row {
                font-style: italic;
                text-align: center;
                background-color: #FFFFCC;


            }
            tr.symbol_row th {
                border:0px; border-right:4px solid;
            }

            tr.name_row {
                font-size:0.4em;

            }

            tr.name_row th {
                border-bottom:1px solid graytext;
            }

            tr.magnitude_row td{
                text-align: center;
                border-bottom:2px solid black;
            }

            tr.micro_name_row {
                font-size:0.4em;

            }
            tr.micro_magnitude_row  {
                font-size:0.4em;
                text-align: center;

            }
        </style>

        <table  class="soilsampletable">
            <tr  class="toprow">
                <th>P</th>
                <th>K</th>
                <th>Mg</th>
                <th>Ca</th>
                <th>Soil pH</th>
                <th>Buffer pH</th>
                <th>S</th>
                <th>B</th>
                <th>Zn</th>
                <th>Mn</th>
                <th>Fe</th>
                <th>Cu</th>
            </tr>

            <tr class="name_row">
                <th>Phosphorus</th>
                <th>Potassium</th>
                <th>Magnesium</th>
                <th>Calcium</th>
                <th></th>
                <th></th>
                <th>Sulphur</th>
                <th>Boron</th>
                <th>Zinc</th>
                <th>Manganese</th>
                <th>Iron</th>
                <th>Copper</th>
            </tr>



            <tr class="magnitude_row">
                <td> {$stdObj->p}</td>
                <td> {$stdObj->k}</td>
                <td> {$stdObj->mg}</td>
                <td> {$stdObj->ca}</td>
                <td> {$stdObj->ph}</td>
                <td> {$stdObj->buffer_ph}</td>
                <td> {$stdObj->s}</td>
                <td> {$stdObj->b}</td>
                <td> {$stdObj->zn}</td>
                <td> {$stdObj->mn}</td>
                <td> {$stdObj->fe}</td>
                <td> {$stdObj->cu}</td>

            </tr>

            <tr class="micro_name_row">
                <th>Aluminum</th>
                <th>Sodium</th>
                <th>Nitrate N</th>
                <th>Soluble Salts</th>
                <th>Organic Matter</th>
                <th>Date</th>
                <th>Molybdenum</th>
                <th>NH4</th>
                <th>Nickel</th>
                <th>Ca:Mg</th>
                <th>P:Al</th>
                <th>Mg:K</th>
            </tr>



            <tr class="micro_magnitude_row">

                <td> {$stdObj->al}</td>
                <td> {$stdObj->na}</td>
                <td> </td>
                <td> </td>
                <td> {$stdObj->organic_matter} %</td>
                <td> {$date}</td>
                <td></td>
                <td> </td>
                <td></td>
                <td>{$caMg}</td>
                <td>{$pAl}</td>
                <td>{$mgK}</td>

            </tr>

        </table> 
        HTML;

        return ($html);
    }

    private function soilSampleColumnChartDataLine($value, $elem, $name) {

        return "['$elem', " . self::calibrationArrays($elem, $value) . ", 'color: " . self::colourValueForNormalizedSoilElementValue(self::calibrationArrays($elem, $value)) . "', ('<b>$name</b><br/>" . $value . "ppm')],";
    }

    function soilSampleColumnChart($stdObj) {
        $js = <<<JS
        <script type="text/javascript">

            function drawChart() {

                /*data.addColumn({type: 'string', role: 'style'}); */
                var data = google.visualization.arrayToDataTable([
                    ['Element', 'Amount', {role: 'style'}, {'type': 'string', 'role': 'tooltip', 'p': {'html': true}}],
                    {$this->soilSampleColumnChartDataLine($stdObj->p, 'P', 'Phosphorus')}
                    {$this->soilSampleColumnChartDataLine($stdObj->k, 'K', 'Potassium')}
                    {$this->soilSampleColumnChartDataLine($stdObj->mg, 'Mg', 'Magnesium')}
                    {$this->soilSampleColumnChartDataLine($stdObj->ca, 'Ca', 'Calcium')}
                    {$this->soilSampleColumnChartDataLine($stdObj->s, 'S', 'Sulphur')}
                    {$this->soilSampleColumnChartDataLine($stdObj->b, 'B', 'Boron')}
                    {$this->soilSampleColumnChartDataLine($stdObj->zn, 'Zn', 'Zinc')}
                    {$this->soilSampleColumnChartDataLine($stdObj->mn, 'Mn', 'Manganese')}
                    {$this->soilSampleColumnChartDataLine($stdObj->fe, 'Fe', 'Iron')}
                    {$this->soilSampleColumnChartDataLine($stdObj->cu, 'Cu', 'Copper')}
                    
                      
                ]);



                var options = {width: 580, height: 312,
                    backgroundColor: "FCFCFC",
                    vAxis: {ticks: [{v: 5, f: "Low minus"}, {v: 15, f: "Low"}, {v: 25, f: "Med"}, {v: 35, f: "Med plus"}, {v: 45, f: "High"}, {v: 55, f: "Excessive"}]},
                    chartArea: {left: 60, top: 20, width: "98%", height: "88%"},
                    tooltip: {isHtml: true}
                };

                var chart = new google.visualization.ColumnChart(document.getElementById('chart_divc'));
                chart.draw(data, options);
            }
            setTimeout(drawChart, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart, true);
             $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart);
            });
        </script>
    JS;

        $out[] = $js;
        $out[] = '<div id="chart_divc" style="width: 580px; height: 460px;"></div>';

        return implode($out);
    }

    /* find the latest soil test id  for a field */

    public function getLatestSoilSample($field_id) {
        $sql = "SELECT field_id,alpha_numeric_id,common_name,report_date,ph,organic_matter,phosphorus as P,potassium as K,calcium as Ca,
        magnesium as Mg,boron as B,copper as Cu,zinc as Zn,sulphur as S,manganese as Mn,iron as Fe,aluminum as Al,sodium as Na,buffer_ph,userid,(cropping.base_saturation(soil_sample_event.id)).*,event_time
FROM cropping.soil_sample_event
LEFT JOIN cropping.field ON field.id = soil_sample_event.field_id
WHERE field_id=$field_id
	ORDER BY event_time DESC limit 1
	";

        $obj = null;
        $res = $GLOBALS ['pdo']->query($sql);
        if ($res->rowCount() == 1) {
            $obj = $res->fetch(PDO::FETCH_OBJ);
        }
        return $obj;
    }

    function displaySoilSamples($mode = 'default', $field_id = NULL) {

        // Setup your database connection

        if ($mode == 'default') {
            $sql = "SELECT field_id,alpha_numeric_id,common_name,report_date,ph,organic_matter,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,sulphur,
                manganese,iron,aluminum,sodium,buffer_ph,userid,(cropping.base_saturation(soil_sample_event.id)).* as bsat
FROM cropping.soil_sample_event
LEFT JOIN cropping.field ON field.id = soil_sample_event.field_id
ORDER BY update_time DESC";
        } else {   // assume looking at an individual field
            $sql = "SELECT field_id,alpha_numeric_id,common_name,report_date,ph,cec,organic_matter,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,sulphur,
            manganese,iron,aluminum,sodium,buffer_ph,userid,(cropping.base_saturation(soil_sample_event.id)).* as bsat
FROM cropping.soil_sample_event
LEFT JOIN cropping.field ON field.id = soil_sample_event.field_id
	WHERE field_id=$field_id
	ORDER BY update_time DESC";
        }


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Report Date'] = JQueryDataTable::formattedDate($row['report_date']);
            $rowOut['pH'] = $this->printPh($row);
            $rowOut['Org %'] = ($row['organic_matter']);
            $rowOut['(P)'] =  $this->printPhosphorus($row);
            $rowOut['(K)'] = $this->printPotassium($row);
            $rowOut['(C)'] = ($row['calcium']);
            $rowOut['(Mg)'] = $this->printMagnesium($row);
            $rowOut['B'] = $this->printBoron($row);
            $rowOut['Cu'] = ($row['copper']);
            $rowOut['Zn'] = $this->printZinc($row);
            $rowOut['S'] = $this->printSulphur($row);
            $rowOut['(Mn)'] = $this->printManganese($row); //Manganese 
            $rowOut['Fe'] = ($row['iron']);
            $rowOut['Al'] = $this->printAluminum($row);
            $rowOut['Na'] = ($row['sodium']);
            $rowOut['Buffer pH'] = ($row['buffer_ph']);
            $rowOut['Base Sat'] = $this->CECCalculation($row);
            $rowOut['Employee'] = ($row['userid']);


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);

        $options = <<<OPT
        dom: 'Bfrtip',
        buttons: [
   
            'csvHtml5',
        ],
                 searching: false,paging: false,ordering: false,info: false
OPT;
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Soil Samples', $dataArr, $options));
        return implode($out);
    }

    function FertilizerRecommendationPerCrop($obj, $crop = null) {
        $crop = "soybean";

        $rec = new FertilizerRecommendation();
        $recommendation = (array) $rec->lookupAll($crop, $obj);
        foreach ($recommendation as $key => $value) {
            $out[] = '<li>' . $key . ' ' . $value . '</li>';
        }

        return (new BootStrap())->plainCard('Fertilizer Recommendation for ' . $crop, implode($out));
    }

    function soilTestNeeded() {
        $sql = "with temp as (
SELECT field.*,field.id as field_id,cropping.field_border_area_at_timestamp(field.id,current_date) as area,field_parameter.not_normally_farmed, field_parameter.pasture, field_parameter.spfh_suitable,(SELECT max(event_time) FROM cropping.soil_sample_event WHERE field_id=field.id) as last_soil_test , (SELECT max(event_time) FROM cropping.fertilizer_event WHERE field_id=field.id) as last_fertilizer_event 
FROM cropping.field
LEFT JOIN cropping.field_parameter ON field_id=field.id
)
SELECT distinct (id),*,last_soil_test::date,last_fertilizer_event::date FROM temp 
WHERE pasture = FALSE AND active is true AND not_normally_farmed=false AND ((temp.last_soil_test <= (date_trunc('year',current_date) - interval '2 years')::date) OR temp.last_soil_test IS NULL)
ORDER BY alpha_numeric_id
";



        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['alpha_numeric_id'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            if ((($row['spfh_suitable']) == 1) AND empty($row['note'])) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            if (($row['spfh_suitable']) == 1) {
                $row['spfh_suitable'] = 'spfh';
            } else {
                $row['spfh_suitable'] = '';
            }
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function soilTestQuickForm() {


        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); // daily dates rray, with
        // unix timestamp as index.
        // grab a list of current fields.
        $sql = "SELECT id,alpha_numeric_id,common_name FROM cropping.fieldcurr";
        $res = $GLOBALS ['pdo']->query($sql);

        $fieldNumber [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            // parse for last word of name and then make an array for the select
            // list.
            $fieldNumber [$row [0]] = $row [1] . " - " . $row [2];
        }

        $form = new HTML_QuickForm("SoilTestQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'soilTestQuickform', 'Record Soil Test Event');


        $form->addElement('select', 'fieldNumber', 'Field Number', $fieldNumber);

        $form->addElement('select', 'reportDate', 'Date of Report', $dateArray);
        $form->addElement('select', 'sampleDate', 'Date of Sample', $dateArray);
        $form->addElement('text', 'organic', 'organic', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'ph', 'pH', array('size' => 5, 'maxlength' => 5));

        $form->addElement('text', 'phosphorus', 'Phosphate P2O5', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'potassium', 'Potash K2O', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'calcium', 'Calcium', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'magnesium', 'Magnesium', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'boron', 'Boron', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'copper', 'Copper', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'zinc', 'Zinc', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'sulphur', 'Suphur', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'manganese', 'Manganese', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'iron', 'Iron', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'sodium', 'Sodium', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'aluminum', 'Aluminum', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'soil_index', 'Buffer ph (Soil/Lime Index]', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'cec', 'CEC', array('size' => 5, 'maxlength' => 5));

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // defaults
        //find the last test and use that as defaults. date conversion to noon. 
        $sql = "SELECT report_date::date + interval '12 hours' as report_date ,event_time FROM cropping.soil_sample_event ORDER BY extract(epoch from create_time) DESC limit 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch();

        $form->setDefaults(array('sampleDate' => strtotime($row['event_time']), 'reportDate' => strtotime($row['report_date']),)); //does not work???

        $form->addRule('fieldNumber', 'A field is required.', 'required');
        $form->addRule('sampleDate', 'The date the sample was taken is required.', 'required');
        $form->addRule('reportDate', 'The date the report was made is required.', 'required');
        //
        //optional
        $form->addRule('organic', 'A valid number is required.', 'numeric');
        //
        $form->addRule('ph', 'This value is required.', 'required');
        $form->addRule('ph', 'A valid number is required.', 'numeric');
        //
        $form->addRule('cec', 'This value is required.', 'required');
        $form->addRule('cec', 'A valid number is required.', 'numeric');
        //
        $form->addRule('phosphorus', 'This element is required.', 'required');
        $form->addRule('phosphorus', 'A valid number is required.', 'numeric');
        //
        $form->addRule('potassium', 'This element is required.', 'required');
        $form->addRule('potassium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('calcium', 'This element is required.', 'required');
        $form->addRule('calcium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('magnesium', 'This element is required.', 'required');
        $form->addRule('magnesium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('boron', 'This element is required.', 'required');
        $form->addRule('boron', 'A valid number is required.', 'numeric');
        //
        $form->addRule('copper', 'This element is required.', 'required');
        $form->addRule('copper', 'A valid number is required.', 'numeric');
        //
        $form->addRule('zinc', 'This element is required.', 'required');
        $form->addRule('zinc', 'A valid number is required.', 'numeric');
        //
        $form->addRule('sulphur', 'This element is required.', 'required');
        $form->addRule('sulphur', 'A valid number is required.', 'numeric');
        //
        $form->addRule('manganese', 'This element is required.', 'required');
        $form->addRule('manganese', 'A valid number is required.', 'numeric');
        //
        $form->addRule('iron', 'This element is required.', 'required');
        $form->addRule('iron', 'A valid number is required.', 'numeric');
        //
        $form->addRule('aluminum', 'This element is required.', 'required');
        $form->addRule('aluminum', 'A valid number is required.', 'numeric');
        //
        $form->addRule('sodium', 'This element is required.', 'required');
        $form->addRule('sodium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('soil_index', 'This value is required.', 'required');
        $form->addRule('soil_index', 'A valid number is required.', 'numeric');

        // Try to validate a form
        if ($form->validate()) {

            $pageid = $form->exportValue('pageid');

            $fieldNumber = $form->exportValue('fieldNumber');

            $t1Array = $form->getElementValue('sampleDate');
            $sampleDate = date('Y-m-d H:i:sO', $t1Array [0]);

            $t2Array = $form->getElementValue('reportDate');
            $reportDate = date('Y-m-d H:i:sO', $t2Array [0]);

            //optional
            $organic = trim($form->exportValue('organic'));
            if ($organic == '') {
                $organic = 'null';
            }

            $ph = trim($form->exportValue('ph'));
            $cec = trim($form->exportValue('cec'));
            $phosphorus = round(trim($form->exportValue('phosphorus')) * 0.4364);  //conversion factors to correct for PEI lab reporting in phosphate and potash
            $potassium = round(trim($form->exportValue('potassium')) * 0.8301);
            $calcium = trim($form->exportValue('calcium'));
            $magnesium = trim($form->exportValue('magnesium'));
            $boron = trim($form->exportValue('boron'));
            $copper = trim($form->exportValue('copper'));
            $zinc = trim($form->exportValue('zinc'));
            $sulphur = trim($form->exportValue('sulphur'));
            $manganese = trim($form->exportValue('manganese'));
            $iron = trim($form->exportValue('iron'));
            $aluminum = trim($form->exportValue('aluminum'));
            $sodium = trim($form->exportValue('sodium'));
            $soil_index = trim($form->exportValue('soil_index'));

            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                $query = "INSERT INTO cropping.soil_sample_event  (field_id,userid,event_time,report_date,organic_matter,ph,cec,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,sulphur,manganese,iron,aluminum,sodium,buffer_ph	) VALUES ($fieldNumber,'{$GLOBALS['auth']->getUsername()}', '$sampleDate','$reportDate',$organic,$ph,$cec,$phosphorus,$potassium,$calcium,$magnesium,$boron,$copper,$zinc,$sulphur,$manganese,$iron,$aluminum,$sodium,$soil_index)";
                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }
            // Close the transaction
            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } // end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    //prints lime recomendations for a field.
    function displayFieldLimeRequirements($mode = 'default', $field_id = NULL) {

        if ($field_id == null) {
            throw new Exception("Error: lime requires field id.\n\r", $code, $previous);
        }

        $sql = "SELECT ph,cec,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,sulphur,manganese,iron,aluminum,sodium,buffer_ph,userid
FROM cropping.soil_sample_event
LEFT JOIN cropping.field ON field.id = soil_sample_event.field_id
WHERE field_id=$field_id
	ORDER BY event_time DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        {
            
        } // Always check that result is not an error
        $row = $res->fetch(PDO::FETCH_ASSOC);

        if (($row['ph'] != null) && ($row['buffer_ph'] != null)) {
            print("<h2>FIXME: Units error? Recommendations incorrect.</h2>");
            print("<h3>Lime Recommendations:</h3>");
            print("<h3>ph=6.0 : " . self::printLimeRequirements($row['buffer_ph'], 6.0) . " t\ha</h3>");
            print("<h3>ph=6.5 : " . self::printLimeRequirements($row['buffer_ph'], 6.5) . " t\ha</h3>");
            print("<h3>ph=6.8 : " . self::printLimeRequirements($row['buffer_ph'], 6.8) . " t\ha</h3>");
        }
    }

    /*
     * return the latest field soiul test, used for calculating niturrient requirements.
     * 
     */

    public static function latestFieldSoilTest($field_id = NULL) {
        $sql = "SELECT field_id,alpha_numeric_id,common_name,report_date,ph,cec,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,sulphur,manganese,iron,aluminum,sodium,buffer_ph,userid
FROM cropping.soil_sample_event
LEFT JOIN cropping.field ON field.id = soil_sample_event.field_id
	WHERE field_id=$field_id
	ORDER BY event_time DESC LIMIT 1";

        $res = $GLOBALS ['pdo']->query($sql);
        $rows = $res->fetch(PDO::FETCH_ASSOC);
        return $rows; //return as array with labels
    }

    //add level value to report.
    function printPh($record) {


        $level = self::Ph_Ph_Calibration_NB($record['ph']);

        return $record['ph'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    //add level value to report.
    function printPhosphorus($record) {


        $level = self::P_Phosphorus_Calibration_NB($record['phosphorus']);

        return $record['phosphorus'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    //add level value to report.
    function printPotassium($record) {


        $level = self::K_Potassium_Calibration_Ontario($record['potassium']);

        return $record['potassium'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    function printMagnesium($record) {


        $level = self::Mg_Magnesium_Calibration_NB($record['magnesium']);
        if ($record['magnesium'] != '') {
            return $record['magnesium'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
        } else {
            return null;
        }
    }

    
    //add level value to report.
    function printBoron($record) {


        $level = self::B_Boron_Calibration_Ontario($record['boron']);

        return $record['boron'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }
    
    function printZinc($record) {


        $level = self::Zn_Zinc_Calibration_Wisc($record['zinc']);

        return $record['zinc'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    function printManganese($record) {


        $level = self::Mn_Manganese_Calibration_Wisc($record['manganese']);

        return $record['manganese'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    function printSulphur($record) {


        $level = self::S_Sulphur_Calibration_Wisc($record['sulphur']);

        return $record['sulphur'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    function printAluminum($record) {

        $level = self::Al_Aluminum_Calibration_AL($record['aluminum']);
        return $record['aluminum'] . ' ' . '<span style="color: ' . self::colourLevelChooser($level) . '">' . $level . '</span>';
    }

    //based on an input of L- to H+, puts out a differnt colour code.
    static function colourLevelChooser($input) {
        switch ($input) {
            case 'L-':
                $output = "red";
                break;
            case 'L':
                $output = "orange";
                break;
            case 'M':
                $output = "lime";
                break;
            case 'M+':
                $output = "green";
                break;
            case 'H':
                $output = "#006400";
                break;
            case 'H+':
                $output = "#006400";
                break;
            default:
                throw new Exception("Error Input value  for colour code, out of range.");
                break;
        }

        return $output;
    }

    /** Talked to Marie-Josee Garand,Phd of Chignecto Agri-Environmental Club Agrologist 
     *  on Jan 16, 2014 email. She gave me a chart NB uses for lime requirements. There is no
     *  formula. 
     *  The chart is for 100% limestone. A buffer ph above 7 makes no sense and 
     *  is a lab artifact or unreacted lime. It means re-test.
     *  A rule of thumb is to maintain pH at 6.5 we need to add 0.5 tonne/acre per year
     *  Ca:Mg ratio above 10:1 means use dolmitic (magnesium) lime. 
     *  Also should check the absolute values to Mag and Ca to make sure they are not out of wack.
     * @param type $bufferPh
     * @param type $required_ph
     * @return type
     * @throws Exception
     */
    public function printLimeRequirementsNB($pH, $bufferPh, $required_ph = 6.8) {


        //do 6.5 and 7.0 and linear interpolated 6.8.
        $neutralizingFactor = 0.95; //graymont aglime 


        if ($bufferPh > 7) {
            $bufferPh = 7.0;
            return("Buffer pH above 7, we must retest. Lab artifact or unreacted lime.</br>");
        }

        if ($bufferPh < 4.8) {
            $bufferPh = 4.8;
            return("Buffer pH  below 4.8, put alot of lime on!!!</br>");
        }



        if ($pH >= $required_ph) {
            return "Already have sufficient lime.</br>";
        }




        $lim65['7'] = 1.1;
        $lim65['7.0'] = 1.1;
        $lim65['6.9'] = 1.6;
        $lim65['6.8'] = 2.1;
        $lim65['6.7'] = 2.7;
        $lim65['6.6'] = 3.2;
        $lim65['6.5'] = 3.9;
        $lim65['6.4'] = 4.5;
        $lim65['6.3'] = 5.3;
        $lim65['6.2'] = 6.0;
        $lim65['6.1'] = 6.8;
        $lim65['6'] = 7.6;
        $lim65['6.0'] = 7.6;
        $lim65['5.9'] = 8.5;
        $lim65['5.8'] = 9.4;
        $lim65['5.7'] = 10.4;
        $lim65['5.6'] = 11.4;
        $lim65['5.5'] = 12.4;
        $lim65['5.4'] = 13.5;
        $lim65['5.3'] = 14.6;
        $lim65['5.2'] = 15.7;
        $lim65['5.1'] = 16.9;
        $lim65['5'] = 18.2;
        $lim65['5.0'] = 18.2;
        $lim65['4.9'] = 19.5;
        $lim65['4.8'] = 20.8;
        //
        $lim68['7.0'] = 1.2;
        $lim68['7'] = 1.2;
        $lim68['6.9'] = 1.8;
        $lim68['6.8'] = 2.5;
        $lim68['6.7'] = 3.2;
        $lim68['6.6'] = 3.9;
        $lim68['6.5'] = 4.6;
        $lim68['6.4'] = 5.4;
        $lim68['6.3'] = 6.3;
        $lim68['6.2'] = 7.1;
        $lim68['6.1'] = 7.9;
        $lim68['6'] = 8.8;
        $lim68['6.0'] = 8.8;
        $lim68['5.9'] = 9.8;
        $lim68['5.8'] = 10.7;
        $lim68['5.7'] = 11.7;
        $lim68['5.6'] = 12.7;
        $lim68['5.5'] = 13.7;
        $lim68['5.4'] = 14.8;
        $lim68['5.3'] = 15.9;
        $lim68['5.2'] = 17;
        $lim68['5.1'] = 18.2;
        $lim68['5'] = 19.4;
        $lim68['5.0'] = 19.4;
        $lim68['4.9'] = 20.6;
        $lim68['4.8'] = 21.8;
        //
        $lim70['7'] = 1.2;
        $lim70['7.0'] = 1.2;
        $lim70['6.9'] = 1.9;
        $lim70['6.8'] = 2.7;
        $lim70['6.7'] = 3.5;
        $lim70['6.6'] = 4.3;
        $lim70['6.5'] = 5.1;
        $lim70['6.4'] = 6.0;
        $lim70['6.3'] = 6.9;
        $lim70['6.2'] = 7.8;
        $lim70['6.1'] = 8.7;
        $lim70['6'] = 9.6;
        $lim70['6.0'] = 9.6;
        $lim70['5.9'] = 10.6;
        $lim70['5.8'] = 11.6;
        $lim70['5.7'] = 12.6;
        $lim70['5.6'] = 13.6;
        $lim70['5.5'] = 14.6;
        $lim70['5.4'] = 15.7;
        $lim70['5.3'] = 16.8;
        $lim70['5.2'] = 17.9;
        $lim70['5.1'] = 19.0;
        $lim70['5'] = 20.2;
        $lim70['5.0'] = 20.2;
        $lim70['4.9'] = 21.3;
        $lim70['4.8'] = 22.5;

        switch ($required_ph) {
            case 7.0:
                return round($lim70["$bufferPh"] * (1 / $neutralizingFactor), 1);
                break;
            case 6.8:
                return round($lim68["$bufferPh"] * (1 / $neutralizingFactor), 1);
                break;
            case 6.5:
                return round($lim65["$bufferPh"] * (1 / $neutralizingFactor), 1);
                break;
            default:
                throw new Exception("ERROR: Unknown or un supported lime ph rate, try 6.5, 6.8, or 7.0</br>");
                break;
        }
    }

    public static function printLimeBudget($mode = 'default', $field_id, $required_ph = 6.8) {



        if ($field_id == null) {
            throw new Exception("Error: lime requires field id.\n\r", $code, $previous);
        }

        //latest test
        $sql = "SELECT event_time,ph,calcium,magnesium,buffer_ph,userid
FROM cropping.soil_sample_event
LEFT JOIN cropping.field ON field.id = soil_sample_event.field_id
WHERE field_id=$field_id
	ORDER BY event_time DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);

        if ($res->rowCount() == 0) {
            $out[] = ("No soil tests available!<br/>");
            return null;
        } else {
            $row = $res->fetch(PDO::FETCH_ASSOC);

            //$out[]=("Ca:Mg Ratio: " . round($row['calcium'] / $row['magnesium']) .":1 <br/>");
            $reqLime = -100;
            try {
                $reqLime = (new SoilSample)->printLimeRequirementsNB($row['ph'], $row['buffer_ph'], $required_ph);
            } catch (Exception $e) {
                echo 'Caught exception: ', $e->getMessage(), "\n";
            }
        }



        $out[] = ("Required Lime= $reqLime tonne/ha<br/>");

        //now find any lime events applied after the soil sample.
        $sql2 = "Select event_time,comment,lime_amount,sum(lime_amount) as sum_lime_spread FROM cropping.lime_event 
              WHERE event_time > '{$row['event_time']}' AND field_id=$field_id
              Group BY   event_time,comment,lime_amount
                ";
        $res2 = $GLOBALS['pdo']->query($sql2);
        if ($res2->rowCount() > 0) {
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
        } else {
            $row2['sum_lime_spread'] = 0;
        }





        $out[] = ("Lime spread since test: {$row2['sum_lime_spread']} tonne/ha<br/>");
        $total = (is_numeric($reqLime)) ? $reqLime - $row2['sum_lime_spread'] : $reqLime; //passes though non numeric error message....really not a great way to do this.

        $out[] = ("Total Required= $total tonne/ha<br/>");

        return implode($out);
    }

    //used in quickforms to print a lime req.
    function printLimeRequirementsQuickform($record, $required_ph = 6.8) {

        // t /ha 
        return (new SoilSample)->printLimeRequirementsNB($record['ph'], $record['buffer_ph'], $required_ph);
    }

    function CECCalculation($record) {

        return "CEC:{$record['cec']} meq/100g K:{$record['k_base_sat']} % Mg:{$record['mg_base_sat']} % Ca:{$record['ca_base_sat']} % Na:{$record['na_base_sat']} %  H:{$record['h_base_sat']} % Total:{$record['total_base_sat']} %";
    }

    /* Jan 2014 */
    /* used to compare different soil properties on a chart by doing a normalization */
    /* note: some of the micro nutrients, small diffeences can then be excessive */

    function calibrationArrays($type, $value) {


        /* 0 L- L M M+ H H+ to infinity */
        /*  P,K,Mg,Ca,S from fertiplan 2012 David Christie */
        /*  L- is array 0 to 1
         *  L  is 1 to 2
         *  M  is 2 to 3
         *  M+ is 3 to 4
         *  H  is 4 to 5
         *  H+ greather than 5 (is this excessive or not??)
         */

        $all['P'] = array(0, 10, 19, 39, 58, 78); //fertiplan
        $all['K'] = array(0, 45, 100, 120, 150, 180); //https://www.plant.uoguelph.ca/sites/plant.uoguelph.ca/files/forages/documents/A7-Fertilizer%20practices%20for%20alfalfa%20production-Mar%201987-87-034.pdf
        $all['Mg'] = array(0, 22, 44, 89, 134, 178); //fertiplan
        $all['Ca'] = array(0, 223, 446, 893, 1340, 1785); //fertiplan
        $all['S'] = array(0, 30, 40, 50, 60, 70); //fertiplan
        $all['B'] = array(0, 0.3, 0.9, 1, 2, 3); //based on http://corn.agronomy.wisc.edu/Management/pdfs/a2522.pdf
        $all['Zn'] = array(0, 1.5, 3, 4, 5, 20); // ???
        $all['Mn'] = array(0, 20, 40, 60, 80, 100); //made up
        $all['Fe'] = array(0, 15, 30, 150, 200, 500); //based on http://www.novascotia.ca/agri/documents/lab-services/analytical-lab-understand-soil.pdf Iron tests are not accurate.
        $all['Cu'] = array(0, 0.3, 0.6, 1.5, 3, 20);  //guidance from http://www.novascotia.ca/agri/documents/lab-services/analytical-lab-understand-soil.pdf 

        if (array_key_exists($type, $all) != true) {
            throw new Exception("Unknown element, no calibrations", $code, $previous);
        }

        //print_r($all[$type]);
        $input = $value;
        //now do normalization. 0 to 50
        // 50 is highest value normally, excessive values are allowed to go to infinity (overflow :) ) and still scaled above 50.
        switch ($input) {
            case (($all[$type][0] <= $input) AND ( $input <= $all[$type][1])):
                $output = 0 + 10 * (($input - $all[$type][0]) / ($all[$type][1] - $all[$type][0]));
                break;
            case (($all[$type][1] <= $input) AND ( $input <= $all[$type][2])):
                $output = 10 + 10 * (($input - $all[$type][1]) / ($all[$type][2] - $all[$type][1]));
                break;
            case (($all[$type][2] <= $input) AND ( $input <= $all[$type][3])):
                $output = 20 + 10 * (($input - $all[$type][2]) / ($all[$type][3] - $all[$type][2]));
                break;
            case (($all[$type][3] <= $input) AND ( $input <= $all[$type][4])):
                $output = 30 + 10 * ($input - $all[$type][3]) / ($all[$type][4] - $all[$type][3]);
                break;
            case (($all[$type][4] <= $input) AND ( $input <= $all[$type][5])):
                $output = 40 + 10 * ($input - $all[$type][4]) / ($all[$type][5] - $all[$type][4]);
                break;
            case ($input > $all[$type][5]):
                $output = ($input / $all[$type][5]) * 50;
                break;
            default:
                throw new Exception("ERROR: Calibration Value input value out of range.");
                break;
        }


        return $output;
    }

    function colourValueForNormalizedSoilElementValue($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 10)):
                $output = "#FF9900";
                break;
            case ((10 <= $input) AND ( $input <= 20)):
                $output = "#FFCC00";
                break;
            case ((20 <= $input) AND ( $input <= 30)):
                $output = "#FFFF66";
                break;
            case ((30 <= $input) AND ( $input <= 40)):
                $output = "#66FF33";
                break;
            case ((40 <= $input) AND ( $input <= 50)):
                $output = "#33CC33";
                break;
            case ((50 <= $input)):
                $output = "#FF4747";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    /*
     * Calibrations from http://www.ndsu.edu/fileadmin/soils/pdfs/sf882.pdf
     */

    function P_Phosphorus_Calibration_NB($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 10)):
                $output = "L-";
                break;
            case ((11 <= $input) AND ( $input <= 19)):
                $output = "L";
                break;
            case ((20 <= $input) AND ( $input <= 39)):
                $output = "M";
                break;
            case ((40 <= $input) AND ( $input <= 58)):
                $output = "M+";
                break;
            case ((59 <= $input) AND ( $input <= 78)):
                $output = "H";
                break;
            case ((79 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    //from: https://www.plant.uoguelph.ca/sites/plant.uoguelph.ca/files/forages/documents/A7-Fertilizer%20practices%20for%20alfalfa%20production-Mar%201987-87-034.pdf
    // this is specific for alfalfa, which basically sets the rate for potash on a field.
    function K_Potassium_Calibration_Ontario($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 45)):
                $output = "L-";
                break;
            case ((46 <= $input) AND ( $input <= 100)):
                $output = "L";
                break;
            case ((101 <= $input) AND ( $input <= 120)):
                $output = "M";
                break;
            case ((121 <= $input) AND ( $input <= 150)):
                $output = "M+";
                break;
            case ((151 <= $input) AND ( $input <= 180)):
                $output = "H";
                break;
            case ((181 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }
    
    function K_Potassium_Calibration_NB($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 18)):
                $output = "L-";
                break;
            case ((19 <= $input) AND ( $input <= 37)):
                $output = "L";
                break;
            case ((38 <= $input) AND ( $input <= 74)):
                $output = "M";
                break;
            case ((75 <= $input) AND ( $input <= 112)):
                $output = "M+";
                break;
            case ((113 <= $input) AND ( $input <= 148)):
                $output = "H";
                break;
            case ((149 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    //Note: these values were inferred by looking at many soil samples and 
    //guessing there scale.
    function Mg_Magnesium_Calibration_NB($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 44)):
                $output = "L";
                break;
            case ((45 <= $input) AND ( $input <= 89)):
                $output = "M";
                break;
            case ((90 <= $input) AND ( $input <= 134)):
                $output = "M+";
                break;
            case ((135 <= $input) AND ( $input <= 179)):
                $output = "H";
                break;
            case ((180 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    //https://www.plant.uoguelph.ca/sites/plant.uoguelph.ca/files/forages/documents/A6-Boron%20fertilization%20of%20alfalfa-July1986.pdf
    //basically less than 1 is a problem for alfalfa
    function B_Boron_Calibration_Ontario($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 0.3)):
                $output = "L-";
                break;
            case ((0.4 <= $input) AND ( $input <= 0.9)):
                $output = "L";
                break;
            case ((1.0 <= $input) AND ( $input <= 1.5)):
                $output = "M";
                break;
            case ((1.6 <= $input) AND ( $input <= 3.0)):
                $output = "H";
                break;
            case ((3.1 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }
    
    
    //Note: info from here: http://www.soils.wisc.edu/extension/pubs/A2528.pdf
    //Values abobe 3.1 till 20 are  sufficient. 
    //and other source: http://corn.agronomy.wisc.edu/Management/pdfs/A3340.pdf
    function Zn_Zinc_Calibration_Wisc($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 1.5)):
                $output = "L-";
                break;
            case ((1.6 <= $input) AND ( $input <= 3)):
                $output = "L";
                break;
            case ((3.1 <= $input) AND ( $input <= 20)):
                $output = "H";
                break;
            case ((20 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    // 11 to 20 is suffiecent.
    //and other source: http://corn.agronomy.wisc.edu/Management/pdfs/A3340.pdf
    // Higher and lower than 11 and 20 are just guesses.
    function Mn_Manganese_Calibration_Wisc($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 11)):
                $output = "L-";
                break;
            case ((11.1 <= $input) AND ( $input <= 20)):
                $output = "M";
                break;
            case ((20 <= $input) AND ( $input <= 40)):
                $output = "H";
                break;
            case ((40 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    //source: http://www.holmestead.ca/chemtrails/soiltest.html
    //over 400 ppm limits root growth????
    function Al_Aluminum_Calibration_AL($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 200)):
                $output = "L-";
                break;
            case ((200 <= $input) AND ( $input <= 500)):
                $output = "L";
                break;
            case ((500 <= $input) AND ( $input <= 800)):
                $output = "M";
                break;
            case ((800 <= $input) AND ( $input <= 1100)):
                $output = "H";
                break;
            case ((1100 <= $input) AND ( $input <= 1600)):
                $output = "H";
                break;
            case ((1600 <= $input)):
                $output = "Ex!"; //excessive
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    function S_Sulphur_Calibration_Wisc($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 15)):
                $output = "L-";
                break;
            case ((15.1 <= $input) AND ( $input <= 29.9)):
                $output = "L";
                break;
            case ((30 <= $input) AND ( $input <= 40)):
                $output = "M";
                break;
            case ((40 <= $input) AND ( $input <= 60)):
                $output = "H";
                break;
            case ((60 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    //Note: the scale is arbitrary, with "H" being the optium for our farm, with
    //corn/alfalfa rotate. 
    function Ph_Ph_Calibration_NB($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 5)):
                $output = "L-";
                break;
            case ((5.1 <= $input) AND ( $input <= 6)):
                $output = "L";
                break;
            case ((6.1 <= $input) AND ( $input <= 6.4)):
                $output = "M+";
                break;
            case ((6.5 <= $input) AND ( $input <= 7)):
                $output = "H";
                break;
            case ((7.1 <= $input)):
                $output = "H+";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    function P_Phosphorus_Calibration_Olsen($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 3)):
                $output = "VL";
                break;
            case ((4 <= $input) AND ( $input <= 7)):
                $output = "L";
                break;
            case ((8 <= $input) AND ( $input <= 11)):
                $output = "M";
                break;
            case ((12 <= $input) AND ( $input <= 15)):
                $output = "H";
                break;
            case ((16 <= $input)):
                $output = "VH";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    /* not used normally */

    function P_Phosphorus_Calibration_Bray1($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 5)):
                $output = "VL";
                break;
            case ((6 <= $input) AND ( $input <= 10)):
                $output = "L";
                break;
            case ((11 <= $input) AND ( $input <= 15)):
                $output = "M";
                break;
            case ((16 <= $input) AND ( $input <= 20)):
                $output = "H";
                break;
            case ((21 <= $input)):
                $output = "VH";
                break;
            default:
                throw new Exception("ErrorL Input value out of range.");
                break;
        }
        return $output;
    }

    function K_Potassium_Calibration_Ammonium_Acetate($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 40)):
                $output = "VL";
                break;
            case ((41 <= $input) AND ( $input <= 80)):
                $output = "L";
                break;
            case ((81 <= $input) AND ( $input <= 120)):
                $output = "M";
                break;
            case ((121 <= $input) AND ( $input <= 160)):
                $output = "H";
                break;
            case ((161 <= $input)):
                $output = "VH";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

    function Zn_Zinc_Calibration_DTPA($input) {

        switch ($input) {
            case ((0 <= $input) AND ( $input <= 0.25)):
                $output = "VL";
                break;
            case ((0.26 <= $input) AND ( $input <= 0.50)):
                $output = "L";
                break;
            case ((0.51 <= $input) AND ( $input <= 0.75)):
                $output = "M";
                break;
            case ((0.76 <= $input) AND ( $input <= 1.00)):
                $output = "H";
                break;
            case ((1.01 <= $input)):
                $output = "VH";
                break;
            default:
                throw new Exception("Error Input value out of range.");
                break;
        }
        return $output;
    }

}

// end class

/** class to hold all the plant tissue stuff
 * Dec 9 2012
 */
class PlantTissue {

    public function __construct() {
        // nothing
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    function plantTissueTestQuickForm() {


        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); // daily dates rray, with
        // unix timestamp as index.
        // grab a list of current fields.
        $sql = "SELECT id,alpha_numeric_id,common_name FROM cropping.fieldcurr";
        $res = $GLOBALS ['pdo']->query($sql);

        $fieldNumber [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            // parse for last word of name and then make an array for the select
            // list.
            $fieldNumber [$row [0]] = $row [1] . " - " . $row [2];
        }

        //load paramaters
        $sql = "SELECT plant_tissue_parameter.id, specific_type,crop_growth_phase,sample_note FROM cropping.plant_tissue_parameter
LEFT JOIN  cropping.seed_category ON plant_tissue_parameter.seed_category_id=seed_category.id
ORDER BY specific_type";
        $res = $GLOBALS ['pdo']->query($sql);

        $tissueTestParam [null] = "Please Select one..."; // dummy value
        while (($row = $res->fetch())) {
            // parse for last word of name and then make an array for the select
            // list.
            $tissueTestParam [$row [0]] = $row [1] . " - " . $row [2] . " - " . $row [3];
        }


        $form = new HTML_QuickForm("plantTissueTestQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'plantTissueTestQuickForm', 'Record Plant Tissue Test Event');

        $form->addElement('select', 'fieldNumber', 'Field Number', $fieldNumber);

        $form->addElement('select', 'sampleDate', 'Date of Sample', $dateArray);

        $form->addElement('select', 'reportDate', 'Date of Report', $dateArray);

        $form->addElement('select', 'tissueTestParam', 'Type sampled', $tissueTestParam);

        $form->addElement('textarea', 'comment', 'Comment', array('size' => 50, 'maxlength' => 50));

        $form->addElement('text', 'nitrogen', 'Nitrogen %', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'phosphorus', 'Phosphorus % (P)', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'potassium', 'Potassium % (K)', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'calcium', 'Calcium %', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'magnesium', 'Magnesium %', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'boron', 'Boron', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'copper', 'Copper', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'zinc', 'Zinc', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'iron', 'Iron', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'manganese', 'Manganese', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'sodium', 'Sodium %', array('size' => 5, 'maxlength' => 5));
        $form->addElement('text', 'sulphur', 'Sulphur %', array('size' => 5, 'maxlength' => 5));

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');


        // rules
        $form->addRule('fieldNumber', 'A field is required.', 'required');
        $form->addRule('sampleDate', 'The date the sample was taken is required.', 'required');
        $form->addRule('reportDate', 'The date the report was made is required.', 'required');
        $form->addRule('tissueTestParam', 'The tissue test type is required.', 'required');
        //
        $form->addRule('nitrogen', 'This element is required.', 'required');
        $form->addRule('nitrogen', 'A valid number is required.', 'numeric');
        //
        $form->addRule('phosphorus', 'This element is required.', 'required');
        $form->addRule('phosphorus', 'A valid number is required.', 'numeric');
        //
        $form->addRule('potassium', 'This element is required.', 'required');
        $form->addRule('potassium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('calcium', 'This element is required.', 'required');
        $form->addRule('calcium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('magnesium', 'This element is required.', 'required');
        $form->addRule('magnesium', 'A valid number is required.', 'numeric');
        //
        $form->addRule('boron', 'This element is required.', 'required');
        $form->addRule('boron', 'A valid number is required.', 'numeric');
        //
        $form->addRule('copper', 'This element is required.', 'required');
        $form->addRule('copper', 'A valid number is required.', 'numeric');
        //
        $form->addRule('zinc', 'This element is required.', 'required');
        $form->addRule('zinc', 'A valid number is required.', 'numeric');
        //
        //
        // Try to validate a form
        if ($form->validate()) {

            $pageid = $form->exportValue('pageid');

            $fieldNumber = $form->exportValue('fieldNumber');

            $tissueTestParam = $form->exportValue('tissueTestParam');

            $t1Array = $form->getElementValue('sampleDate');
            $sampleDate = date('Y-m-d H:i:sO', $t1Array [0]);

            $t2Array = $form->getElementValue('reportDate');
            $reportDate = date('Y-m-d H:i:sO', $t2Array [0]);

            $comment = pg_escape_string(trim($form->exportValue('comment')));
            $nitrogen = trim($form->exportValue('nitrogen'));
            $phosphorus = trim($form->exportValue('phosphorus'));
            print("<br>|||||" . $phosphorus . "||||||<br>");
            $potassium = trim($form->exportValue('potassium'));
            $calcium = trim($form->exportValue('calcium'));
            $magnesium = trim($form->exportValue('magnesium'));
            $boron = trim($form->exportValue('boron'));
            $copper = trim($form->exportValue('copper'));
            $zinc = trim($form->exportValue('zinc'));


            $manganese = trim($form->exportValue('manganese'));
            $iron = trim($form->exportValue('iron'));

            $sodium = trim($form->exportValue('sodium'));
            $sulphur = trim($form->exportValue('sulphur'));

            $manganese = $this->replaceBlankWithTextNull($manganese);
            $iron = $this->replaceBlankWithTextNull($iron);
            $sodium = $this->replaceBlankWithTextNull($sodium);
            $sulphur = $this->replaceBlankWithTextNull($sulphur);


            // Open a transaction
            try {
                $res = $GLOBALS ['pdo']->beginTransaction();

                $query = "INSERT INTO cropping.plant_tissue_event  (field_id,userid,event_time,report_date,comment,nitrogen,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,manganese,iron,sodium,plant_tissue_parameter_id,sulphur) VALUES ($fieldNumber,'{$GLOBALS['auth']->getUsername()}', '$sampleDate','$reportDate','$comment',$nitrogen,$phosphorus,$potassium,$calcium,$magnesium,$boron,$copper,$zinc,$manganese,$iron,$sodium,$tissueTestParam,$sulphur)";
                $res = $GLOBALS ['pdo']->exec($query);

                // determine if the commit or rollback
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                error_log($e->getMessage(), 0);
            }
            // Close the transaction
            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid");
            exit();
        } // end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function replaceBlankWithTextNull($item) {
        if (($item == '') OR ( $item == null) OR ( strlen($item) == 0)) {
            return 'null';
        } else {
            return $item;
        }
    }

    function displayPlantTissueSamples($mode = 'default', $field_id = NULL) {

        // Setup your database connection

        if ($mode == 'default') {

            $out[] = ("<h3>Tissue Samples Log</h3>");
            $out[] = ("<h3>Note:VL and VH are out of sufficency range. VH is probably not a problem. VL probably <b>is</b> a problem.</h3>");

            $sql = "SELECT field_id,alpha_numeric_id,common_name,report_date,comment,nitrogen,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,manganese,iron,sodium,sulphur,userid,plant_tissue_parameter_id, specific_type,crop_growth_phase
FROM cropping.plant_tissue_event
LEFT JOIN cropping.field ON field.id = plant_tissue_event.field_id
LEFT JOIN cropping.plant_tissue_parameter ON plant_tissue_event.plant_tissue_parameter_id=plant_tissue_parameter.id
LEFT JOIN  cropping.seed_category ON plant_tissue_parameter.seed_category_id=seed_category.id
ORDER BY event_time DESC,alpha_numeric_id";
        } else {   // assume looking at an individual field
            $sql = "SELECT field_id,alpha_numeric_id,common_name,report_date,comment,nitrogen,phosphorus,potassium,calcium,magnesium,boron,copper,zinc,manganese,iron,sodium,sulphur,userid,plant_tissue_parameter_id, specific_type,crop_growth_phase
FROM cropping.plant_tissue_event
LEFT JOIN cropping.field ON field.id = plant_tissue_event.field_id
LEFT JOIN cropping.plant_tissue_parameter ON plant_tissue_event.plant_tissue_parameter_id=plant_tissue_parameter.id
            LEFT JOIN  cropping.seed_category ON plant_tissue_parameter.seed_category_id=seed_category.id
	WHERE field_id=$field_id
	ORDER BY event_time DESC";
        }


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {


            $rowOut['Field'] = JQueryDataTable::printFormattedFieldAlphaNumeric($row['field_id'], $row['alpha_numeric_id']);
            $rowOut['Common Name'] = ($row['common_name']);
            $rowOut['Report Date'] = JQueryDataTable::formattedDate($row['report_date']);
            $rowOut['Plant'] = $row['specific_type'];

            $rowOut['nitrogen'] = $this->printNitrogenTissue($row);
            $rowOut['Phosphorus (P)'] = $this->printPhosphorusTissue($row);
            $rowOut['Potassium (K)'] = $this->printPotassiumTissue($row);
            $rowOut['Calcium'] = $this->printCalciumTissue($row);
            $rowOut['Magnesium (Mg)'] = $this->printMagnesiumTissue($row);
            $rowOut['Boron'] = $this->printBoronTissue($row);
            $rowOut['Copper'] = $this->printCopperTissue($row);
            $rowOut['Zinc'] = $this->printZincTissue($row);
            $rowOut['Manganese (Mn)'] = $this->printManganeseTissue($row);
            $rowOut['Iron'] = $this->printIronTissue($row);
            $rowOut['Sodium'] = $this->printSodiumTissue($row);
            $rowOut['Sulphur'] = $this->printSulphurTissue($row);
            $rowOut['Employee'] = ($row['userid']);


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order: false'));
        return implode($out);
    }

    //paramaters from: http://www.soils.wisc.edu/extension/wcmc/2007/pap/Peters.pdf
    function printNitrogenTissue($record) {

        //chnaging paramaters.
        $value = $record['nitrogen'];
        $type = 'nitrogen';
        $sql = "SELECT nitrogen_min,nitrogen_max,nitrogen_median,nitrogen_sufficient_min,nitrogen_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printPhosphorusTissue($record) {

        //chnaging paramaters.
        $value = $record['phosphorus'];
        $type = 'phosphorus';
        $sql = "SELECT phosphorus_min,phosphorus_max,phosphorus_median,phosphorus_sufficient_min,phosphorus_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);

        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printPotassiumTissue($record) {

        //chnaging paramaters.
        $value = $record['potassium'];
        $type = 'potassium';
        $sql = "SELECT potassium_min,potassium_max,potassium_median,potassium_sufficient_min,potassium_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printCalciumTissue($record) {

        //chnaging paramaters.
        $value = $record['calcium'];
        $type = 'calcium';
        $sql = "SELECT calcium_min,calcium_max,calcium_median,calcium_sufficient_min,calcium_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printMagnesiumTissue($record) {

        //chnaging paramaters.
        $value = $record['magnesium'];
        $type = 'magnesium';
        $sql = "SELECT magnesium_min,magnesium_max,magnesium_median,magnesium_sufficient_min,magnesium_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printBoronTissue($record) {

        //chnaging paramaters.
        $value = $record['boron'];
        $type = 'boron';
        $sql = "SELECT boron_min,boron_max,boron_median,boron_sufficient_min,boron_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printCopperTissue($record) {

        //chnaging paramaters.
        $value = $record['copper'];
        $type = 'copper';
        $sql = "SELECT copper_min,copper_max,copper_median,copper_sufficient_min,copper_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printZincTissue($record) {

        //chnaging paramaters.
        $value = $record['zinc'];
        $type = 'zinc';
        $sql = "SELECT zinc_min,zinc_max,zinc_median,zinc_sufficient_min,zinc_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printIronTissue($record) {

        //chnaging paramaters.
        $value = $record['iron'];
        $type = 'iron';
        $sql = "SELECT iron_min,iron_max,iron_median,iron_sufficient_min,iron_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printManganeseTissue($record) {

        //chnaging paramaters.
        $value = $record['manganese'];
        $type = 'manganese';
        $sql = "SELECT manganese_min,manganese_max,manganese_median,manganese_sufficient_min,manganese_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printSodiumTissue($record) {

        //chnaging paramaters.
        $value = $record['sodium'];
        $type = 'sodium';
        //we have no data for ideal sodium values. I assume too high is a problem.
        $sql = "SELECT 0 as sodium_min,0 as sodium_max,0 as sodium_median,0 as sodium_sufficient_min,.04 as sodium_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function printSulphurTissue($record) {

        //chnaging paramaters.
        $value = $record['sulphur'];
        $type = 'sulphur';
        //http://www.nrcresearchpress.com/doi/pdf/10.4141/cjps76-104 for alfalfa. .3 is good. .23 is stunted. 
        //https://www.agry.purdue.edu/ext/corn/news/timeless/sulfurdeficiency.pdf corn .15 is bad.
        $sql = "SELECT 0 as sulphur_min,.9 as sulphur_max,sulphur_median,sulphur_sufficient_min,sulphur_sufficient_max FROM cropping.plant_tissue_parameter WHERE id={$record['plant_tissue_parameter_id']} LIMIT 1 ";

        $level = self::tissueCalibrationWisc($value, $type, $sql);
        return $value . ' ' . '<span style="color: ' . self::colourLevelChooserTissue($level) . '">' . $level . '</span>';
    }

    function tissueCalibrationWisc($input, $mineral, $sql) {

        //there will be a different sql string depending on the mineral as input.
        //run sql  
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch();

        //fill in paramaters.
        $min = $row[0];
        $max = $row[1];
        $median = $row[2];
        $sufficient_min = $row[3];
        $sufficient_max = $row[4];


        if (empty($row[1]) OR empty($row[2]) OR empty($row[3]) OR empty($row[4])) {
            return '';
        }


        //now do some math to determine levels.
        //assume VL and VH are bad and thus break up suffucency levels in L,M,H.
        //a gaussian distribution would probably be better here.
        $split = abs($sufficient_max - $sufficient_min) / 3;

        switch ($input) {
            case (($sufficient_min > $input) ):
                $output = "VL";
                break;
            case (($sufficient_min + ($split * 0) <= $input) AND ( $input < ($sufficient_min + ($split * 1)))):
                $output = "L";
                break;
            case (($sufficient_min + ($split * 1) <= $input) AND ( $input < ($sufficient_min + ($split * 2)))):
                $output = "M";
                break;
            case (($sufficient_min + ($split * 2) <= $input) AND ( $input <= ($sufficient_min + ($split * 3)))):
                $output = "H";
                break;
            case (($sufficient_max < $input)):
                $output = "VH";
                break;
            default:
                throw new Exception("Error Input: \"$input\" out of range of $sufficient_min to $sufficient_max, really 0 to infinity.");
                break;
        }



        return $output;
    }

    //based on an input of VL to VH, puts out a differnt colour code.
    //note this is differnet then soil test, VL and VH or out of sufficenecy range.
    static function colourLevelChooserTissue($input) {
        switch ($input) {
            case 'VL':
                $output = "red";
                break;
            case 'L':
                $output = "#80FF80";
                break;
            case 'M':
                $output = "#00FF00";
                break;
            case 'H':
                $output = "#006600";
                break;
            case 'VH':
                $output = "#FF9900";
                break;
            default:
                $output = "000"; //when not supported. 
                break;
        }

        return $output;
    }

}

//end class

/*
 * Jan 2014
 */

class FertilizerRecommendation {

    function lookupAll($cropName, $soilTestObj) {
        $ret = new stdClass;

        $ret->p2o5 = self::lookup($cropName, 'p', $soilTestObj->p);
        $ret->k2o = self::lookup($cropName, 'k', $soilTestObj->k);
        $ret->mg = self::lookup($cropName, 'mg', $soilTestObj->mg);
        $ret->caco3 = self::lookup($cropName, 'ca', $soilTestObj->ca);
        $ret->s = self::lookup($cropName, 's', $soilTestObj->s);
        $ret->b = self::lookup($cropName, 'b', $soilTestObj->b);
        $ret->zn = self::lookup($cropName, 'zn', $soilTestObj->zn);
        $ret->mn = self::lookup($cropName, 'mn', $soilTestObj->mn);
        $ret->cu = self::lookup($cropName, 'cu', $soilTestObj->cu);
        $ret->fe = self::lookup($cropName, 'fe', $soilTestObj->fe);
        //return recomendation object
        return $ret;
    }

    /* looks up in the database fertilizer recommendations */

    function lookup($cropName, $element, $currentValue) {

        $sql = "
         SELECT recommendation
FROM  cropping.fertilizer_recommendation_crop
LEFT JOIN cropping.fertilizer_recommendation ON fertilizer_recommendation.id=fertilizer_recommendation_id
WHERE specific_type='$cropName' AND element='$element' AND $currentValue::numeric <@ element_range limit 1    
        ";
        $res = $GLOBALS ['pdo']->query($sql);

        if ($res->rowCount() == 0) {
            //nothing to recommend.
            $row['recommendation'] = '???';
        } else {
            $row = $res->fetch(PDO::FETCH_ASSOC);
        }

        return $row['recommendation'];
    }

    /* example of how to add more info to DB easily */
    /*

      INSERT INTO fertilizer_recommendation VALUES (2, 'p', '[0,10]', 120);
      INSERT INTO fertilizer_recommendation VALUES (2, 'p', '[11,19]', 100);
      INSERT INTO fertilizer_recommendation VALUES (2, 'p', '[20,39]', 68);
      INSERT INTO fertilizer_recommendation VALUES (2, 'p', '[40,58]', 50);
      INSERT INTO fertilizer_recommendation VALUES (2, 'p', '[59,78]', 30);
      INSERT INTO fertilizer_recommendation VALUES (2, 'p', '[78,]', 30);
      INSERT INTO fertilizer_recommendation_crop VALUES ('alfalfa',2, 'source: NB Crop Fertilization Guide 2001');
      INSERT INTO fertilizer_recommendation_crop VALUES ('trefoil',2, 'source: NB Crop Fertilization Guide 2001');

     */
}

//end class


/*
 * 
 * used to find fertilizer requirements based on current test (supplied)
 * 2021
 */
class FertilzerRequirements {

    function test() {

        $k_ppm = 140;
        $cec = 15;
        print $this->pottassiumAlfalfa($k_ppm, $cec);
    }
           
    
    //from: https://stackoverflow.com/questions/5464919/find-a-matching-or-closest-value-in-an-array
        function getNearest($arr, $var) {
            usort($arr, function($a, $b) use ($var) {
                return abs($a - $var) - abs($b - $var);
            });
            return array_shift($arr);
        }
    
    //source: https://www.extension.purdue.edu/extmedia/ay/ay-331-w.pdf
    function pottassiumAlfalfa($k_ppm, $cec) {

        

        $y = array(0, 74, 75, 99, 100, 124, 125, 149, 150, 174, 175, 199, 200);
        $x = array(5, 10, 20, 30);


        $y_value = $this->getNearest($y, $k_ppm);
        $x_value = $this->getNearest($x, $cec);


        //in lbs k2O per acre
        $y[0] = array(285, 300, 300, 300);
        $y[74] = array(285, 300, 300, 300);
        $y[75] = array(150, 270, 300, 300);
        $y[99] = array(150, 270, 300, 300);
        $y[100] = array(40, 160, 270, 300);
        $y[124] = array(40, 160, 270, 300);
        $y[125] = array(0, 55, 160, 270);
        $y[149] = array(0, 55, 160, 270);
        $y[150] = array(0, 0, 55, 160);
        $y[174] = array(0, 0, 55, 160);
        $y[175] = array(0, 0, 0, 55);
        $y[199] = array(0, 0, 0, 55);
        $y[200] = array(0, 0, 0, 0);

        //for x array we need to find correspoding key value.
        $x_value_raw = array_search($x_value, $x);

        //debug
        //var_dump($y_value);
        //var_dump($x_value_raw);

        //var_dump($y[149]);

        $k20_lb_ac = $y[$y_value][$x_value_raw];
 

        //convert to K units per hectare from k20 lbs/acre
       return round(1.1209 * $k20_lb_ac * 0.8301) .' K units';
        
        
    }

}
