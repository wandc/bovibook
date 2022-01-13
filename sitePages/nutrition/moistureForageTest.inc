<?php
class MoistureForageTest extends BasePage {

    public function defaultDisplay() {


        $left = <<<HTML
        {$this->displayBagMoistureTestLog()}
        {$this->moistureTestsForAllOpenBags()}
        HTML; 
           
        $right = <<<HTML
                {$this->addBagMoistureTestQuickform()}
        HTML; 
               
        $content=(new Bootstrap)->oneThirdTwoThirds($left,$right);                
        print($content);
        
       
        
    }

    function moistureTestsForAllOpenBags() {
        $out=array();
        //display moisture graphs for currently open bags that have moisture tests,
        $query = "SELECT DISTINCT bag_current_open.id as bag_id FROM nutrition.bag_current_open
        LEFT JOIN nutrition.bag_moisture_test ON bag_current_open.id = bag_moisture_test.bag_id";
        $res = $GLOBALS['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
          $out[]=  $this->plotMoistureVsTime($row['bag_id']);
        }
        return implode($out);
    }
    
    
    
    function addBagMoistureTestQuickform() {

        //daily dates rray, with unix timestamp as index.
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesAtDailyIntervalsForLastSevenDays();

        //get open bags with footages availble in bag.
        $retArr = (new BagConsumption)->openBagsWithValidFootages(false);
        $listOfBags = $retArr[0];
        $listOfFootages = $retArr[1];

        //generate list of percentages 5 to 95
        $listOfPercentages[null] = "Please Select one..."; //dummy value
        for ($i = 10.0; $i <= 90.0; $i = $i + 0.1) {
            $output=round($i,1);
            $listOfPercentages["$output"] = "$output %";
        }

        $form = new HTML_QuickForm("addBagMoistureTestQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Add Moisture Test Note About Ag Bag');

        //Hierarchical select list
        $opts[] = $listOfBags;
        $opts[] = $listOfFootages;
        $hs = $form->addElement('hierselect', 'whichBag', 'Bag and Footage');
        $hs->setOptions($opts);

        $form->addElement('select', 'percentWater', 'Percent Water (Moisture)', $listOfPercentages);
        $form->addElement('select', 'dateEvent', 'Date of Check', $dateArray);


        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
       // $form->setDefaults(array('whichBag' => 0));
       // $form->setDefaults(array('percentWater' => 0));
       // $form->setDefaults(array('dateEvent' => 0));

        //rules
        $form->addRule('whichBag', 'Bag is required', 'required');
        $form->addRule('percentWater', 'Percentage Dry Matter is required', 'required');
        $form->addRule('dateEvent', 'Date is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            //hierslect
            $bagAndFootage = $form->exportValue('whichBag');
            $bag = $bagAndFootage[0]; //in format 49|forward
            $footage = $bagAndFootage[1];
            //
            //break out the id.
            $bagArr = explode("|", $bag);
            $bag = $bagArr[0];


            $tempArray = $form->getElementValue('dateEvent');
            ;
            $event_date = date('Y-m-d H:i:sO', $tempArray[0]);
            $percentWater = $form->exportValue('percentWater');
            $percentDryMatter = 100 - $percentWater;
            $pageid = $form->exportValue('pageid');


            //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_moisture_test (bag_id,footage,percent_dry_matter,event_time,userid) VALUES ($bag,$footage,$percentDryMatter,'$event_date','{$GLOBALS['auth']->getUsername()}')";

            $res = $GLOBALS['pdo']->exec($query);



            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

    function displayBagMoistureTestLog($mode = 'default') {

        $sql = <<<SQL
SELECT bag_moisture_test.footage,percent_dry_matter,bag_moisture_test.event_time,bag_current_open.slot,bag_current_open.current_feed,substring(location from 1 for 1) || slot as bag2
		FROM nutrition.bag_moisture_test
LEFT JOIN nutrition.bag_current_open ON bag_moisture_test.bag_id = bag_current_open.id
WHERE bag_current_open.slot is not null ORDER BY bag_id,event_time ASC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['raw'] = strtotime($row['event_time']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Bag #'] = ($row['bag2']);
            $rowOut['Feed'] = ($row['current_feed']);
            $rowOut['Footage'] = ($row['footage']);
            $rowOut['Water %'] = 100 - (empty(!($row['percent_dry_matter'])) ? $row['percent_dry_matter'] : null);
            $rowOut['Dry Matter %'] = ($row['percent_dry_matter']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayNoSort('Moisture Test of Feed in Ag Bags', $dataArr));
        return implode($out);
    }

    function plotMoistureVsTime($bagId) {
        $out=array();
        //set how far back to plot data.
        $plotStartDate = date('Y-m-d', strtotime("-250 days"));

        $query = "SELECT bag_moisture_test.footage,abs(percent_dry_matter-100) as percent_dry_matter,bag_moisture_test.event_time::date,bag.slot,current_feed
		FROM nutrition.bag_moisture_test
LEFT JOIN nutrition.bag ON bag_moisture_test.bag_id = bag.id
LEFT JOIN  nutrition.bag_with_current_feed ON bag_with_current_feed.id=bag.id WHERE bag.id=$bagId ORDER BY bag_id,event_time ASC";
        $res = $GLOBALS['pdo']->query($query);

        $one = array();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            $currentFeed = $row['current_feed'];
            $slot = $row['slot'];

            if (is_numeric($row['percent_dry_matter'])) {
                $dd = date('Y-m-d', strtotime($row['event_time']));
                $one[strtotime($row['event_time'])] = round($row['percent_dry_matter'], 4);
            }
        }

        $out[]=('<div class="bagMoisturePlots">');
        if (!empty($slot)) {
            $out[]=("<h3>$currentFeed (slot: $slot)</h3>");
        }
        if (!empty($one)) {

            $headerStrArr = array();
            $headerStrArr['xAxis'] = 'date'; //BACKWARDS
            $headerStrArr['Water %'] = 'number';

            $opt = ' pointSize:20';
            $x = new GoogleVisualizationLine( null, $opt, $headerStrArr, $one);
            $out[]=($x->toString());
        }

        $out[]=('</div> <!-- endbagMoisturePlots -->');
        
        return implode($out);
    }

    function bagDryMatterSparkline() {
        $startDate = date('Y-m-d 00:00:00', strtotime("-90 days"));
        $dataArray = $this->findDryMatterForDateRange($startDate, $endDate);

        $dimDataNorm = GoogleChartsHelper::normalizeGoogleChartData($dataArray, null, null);
        $dimDataNorm['labelLast'] = round($dimDataNorm['labelLast'], 2); //round off last value of time series data.

        sparkline($dimDataNorm['extendedStr'], $dimDataNorm['dataCount'], $dimDataNorm['labelLast'], $bgColour = 'ffffff');
    }

    public function findDryMatterForDateRange($startDate, $endDate) {
//no end date, because average test results could be in the future.
        $query = "SELECT bag_moisture_test.footage,percent_dry_matter,bag_moisture_test.event_time,bag.slot,current_feed
		FROM nutrition.bag_moisture_test
LEFT JOIN nutrition.bag ON bag_moisture_test.bag_id = bag.id
LEFT JOIN  nutrition.bag_with_current_feed ON bag_with_current_feed.id=bag.id WHERE bag.id=7 AND bag_moisture_test.event_time >= '$startDate' ORDER BY bag_id,event_time DESC";
        $res = $GLOBALS['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $dataArray[] = $row['percent_dry_matter'];
        }
        return $dataArray;
    }

}

//end class
?>