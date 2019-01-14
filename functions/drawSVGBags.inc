<?php 

include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/milk2006.inc');

class DrawSVGBags {

    private $lastSlot; //holds the name (ie S5) of the last slot number (for display customizations).
    protected $dateOfInterest; //holds the date to render to.
    private $heightSlot = 59;
    private $widthSlot = 1380;

    public function __construct($dateOfInterest) {
        $this->dateOfInterest = $dateOfInterest;
        //this has to go here because if you put it after it still hasn't loaded the external svg.php file yet.
    
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        //nothing
    }

    public function main() {
        $scaleMultiplier = 4;
        $heightScale = 2;

//get a list of all current bags.
        $listOfBagsArray = $this->getListOfBagsToDraw();

//find last slot name/number.
        $lastElementIndex = count($listOfBagsArray);
        $keys = (array_keys($listOfBagsArray));
        $this->lastSlot = $keys[$lastElementIndex - 1];

        print($this->drawLegend());

        print("<h2>As of " . date('M d, Y', strtotime($this->dateOfInterest)) . "</h2>");

//go through each bag and draw them out.
      //  print("<div id='bagContainer' class='container'>");
      
        foreach ($listOfBagsArray as $slot => $bagsInSlot) {

            if ($slot == $this->lastSlot) {
                $heightX = $this->heightSlot + 30;
            } else {
                $heightX = $this->heightSlot;
            }


//preamble
            $str = ""; //start with blank string
                //viewbox and no styling allows it to be responsive. */
            $str=$str."<svg version='1.1' viewBox='0 0 {$this->widthSlot} $heightX' >";
            
            
//do something different if slot currently has no bags.
            if ($bagsInSlot != null) {
                foreach ($bagsInSlot as $bagId) {
                    $str = $str . $this->drawSVGOfSpecificBag($bagId, $scaleMultiplier, $heightScale, $slot);
                }
            } else { // null (ie no current bags in slot) 
                $str = $str . $this->drawEmptySlot($scaleMultiplier, $heightScale, $slot, $bagId);
            }

//postamble
            $str = $str . "</svg>" . "\n\r";

//make the last one wider to show numbers.
            if ($slot == $this->lastSlot) {
                $height = $this->heightSlot + 50;
            } else {
                $height = $this->heightSlot + 20;
            }

            $this->showSVG($str); 
           
        } //end foreach
       //   print("</div>");
    }

//end function

    function drawLegend() {
        $str = '';
     //   $str = $str . '<p><svg xmlns="http://www.w3.org/2000/svg"  version="1.1" viewBox="0 0 140 53" width: 100%; >   <rect width="30" height="10" style="fill:#983191;" /> <text x="40" y="8" fill="#000000">High cow suitable</text></svg>';
     //   $str = $str . '<svg  xmlns="http://www.w3.org/2000/svg"  version="1.1"  viewBox="0 0 140 53" width: 100%;>   <rect width="30" height="10" style="fill:#ef8995;" /> <text x="40" y="8" fill="#000000">Low cow suitable</text></svg>';
     //   $str = $str . '<svg  xmlns="http://www.w3.org/2000/svg"  version="1.1"  viewBox="0 0 140 53" width: 100%;>   <rect width="30" height="10" style="fill:#f3b149;" /> <text x="40" y="8" fill="#000000">Dry cow suitable</text></svg></p>';
        return $str;
    }

//get a list of all current bags, ie ones that have not been consumed.
    function getListOfBagsToDraw() {

        $listOfBags = null;
        $bagsInSlot = null;


        //////////
        $sqlX = "CREATE TEMP TABLE temp_bags_of_interest AS 
with temp as (
    SELECT bagx.id, bagx.location, bagx.slot, bagx.diameter_foot, bagx.event_time, bagx.create_time, bagx.update_time, bagx.comment, bagx.userid

/* over all feed types in the bag find the start and end point of the bag */
,(SELECT min(footage_start) FROM nutrition.bag_feed WHERE bag_id=bagx.id ) as min_feed_footage
,(SELECT max(footage_finish) FROM nutrition.bag_feed WHERE bag_id=bagx.id ) as max_feed_footage

/* find all possible consumption points (forward or reverse or both) at the time period on interest */
,(SELECT max(footage) FROM nutrition.bag_consumption WHERE bag_id=bagx.id and direction='forward' and event_time<='{$this->dateOfInterest}') as max_forward_consumed_footage
,(SELECT min(footage) FROM nutrition.bag_consumption WHERE bag_id=bagx.id and direction='forward' and event_time<='{$this->dateOfInterest}') as min_forward_consumed_footage
,(SELECT max(footage) FROM nutrition.bag_consumption WHERE bag_id=bagx.id and direction='reverse' and event_time<='{$this->dateOfInterest}') as max_reverse_consumed_footage
,(SELECT min(footage) FROM nutrition.bag_consumption WHERE bag_id=bagx.id and direction='reverse' and event_time<='{$this->dateOfInterest}') as min_reverse_consumed_footage
FROM nutrition.bag bagx
)

/* we have to use EXCEPT,because null is undefined and thus comparisons don't work. */
SELECT * from temp EXCEPT (SELECT * from temp WHERE (max_feed_footage = max_forward_consumed_footage) OR 
(min_reverse_consumed_footage=min_feed_footage)  OR (max_forward_consumed_footage=min_reverse_consumed_footage) OR event_time >= '{$this->dateOfInterest}')
/* we have to ignore bags that have not been created before date of interest, for historical views */
";


        $resX = $GLOBALS['pdo']->query($sqlX);


        /* 2nd loop is used for case of multiple bags in one slot */
        
        // a bag is considered consumed when the consumption footage is equal or greater than then bag finished_footage.
        //FIXME does not do bags feeding from reverse side.
        $res = $GLOBALS['pdo']->query("SELECT * FROM temp_bags_of_interest WHERE location='North Silage Pad' Order BY location,slot ASC");

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $bagsInSlot = null;
            $res2 = $GLOBALS['pdo']->query("SELECT id FROM temp_bags_of_interest WHERE slot={$row['slot']} and location='North Silage Pad' ORDER BY event_time ASC");
            while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {
                $bagsInSlot[] = $row2['id'];
            }
            $listOfBags["N{$row['slot']}"] = $bagsInSlot;
        }

        //do again for the south silage pad, only drawing slots that have a bag and skipping others (numrows line).
        $res = null;
        $res2 = null;
        $res = $GLOBALS['pdo']->query("SELECT * FROM temp_bags_of_interest WHERE location='South Silage Pad' Order BY location,slot ASC");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $bagsInSlot = null;
            $res2 = $GLOBALS['pdo']->query("SELECT id FROM temp_bags_of_interest WHERE slot={$row['slot']} and location='South Silage Pad' ORDER BY event_time ASC");
            if ($res2->rowCount() > 0) {
                while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {
                    $bagsInSlot[] = $row2['id'];
                }
                $listOfBags["S{$row['slot']}"] = $bagsInSlot;
            }
        }

        return $listOfBags;
    }

    function drawSVGOfSpecificBag($bagId, $scaleMultiplier, $heightScale, $slot) {




        $str = ""; //start with blank string
//go through steps in specific order to draw bag
        $str = $str . $this->drawBlankBag($bagId, $scaleMultiplier, $heightScale);


//draw feed in bag.
        $str = $str . $this->drawFeedInBag($bagId, $scaleMultiplier, $heightScale);

//draw bag_id at start of bag. //NOTE: only use for debugging/forage analysis.
//$str=$str.$this->drawBagId($bagId,$scaleMultiplier,$heightScale);
//draw field names
        $str = $str . $this->drawFieldNames($bagId, $scaleMultiplier, $heightScale);

//draw ensile dates
        $str = $str . $this->drawEnsileDate($bagId, $scaleMultiplier, $heightScale);

//draw comments
        $str = $str . $this->drawComment($bagId, $scaleMultiplier, $heightScale);



        $str = $str . $this->drawDistances($scaleMultiplier, $heightScale, $slot);
        $str = $str . $this->drawSlotNumber($scaleMultiplier, $heightScale, $slot, $bagId);

//consumed
        $str = $str . $this->drawLatestConsumption($bagId, $scaleMultiplier, $heightScale);

        //draw forage test, put here so we can still read the forage tests, when masked by consumed mask.
        $str = $str . $this->drawForageTest($bagId, $scaleMultiplier, $heightScale);


        return $str;
    }

    function drawEmptySlot($scaleMultiplier, $heightScale, $slot, $bagId) {
        $str = ""; //start with blank string 
        $str = $str . $this->drawDistances($scaleMultiplier, $heightScale, $slot);
        $str = $str . $this->drawSlotNumber($scaleMultiplier, $heightScale, $slot, $bagId);
        return $str;
    }

    function drawLatestConsumption($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
//first find out if we are in a situation where we are feeding from both ends of the bag simultaneously.
        $sql = "SELECT Distinct(direction) FROM nutrition.bag_consumption WHERE bag_id=$bagId";
        $res6 = $GLOBALS['pdo']->query($sql);
        if ($res6->rowCount() == 2) { //open at both ends
            $direction = 'both';
        } elseif ($res6->rowCount() == 0) { //open at both ends
            $direction = 'none';
        } else {  //one direction, find if forward or reverse.
            $row6 = $res6->fetch(PDO::FETCH_ASSOC);
            $direction = $row6['direction'];
        }

//find out how big the bag is.
//find where a bag starts and ends so we don't put a blank square over other bags that lie before it.
        $sql = "SELECT min(footage_start) as bag_start_footage,max(footage_finish) as max_feed_footage FROM nutrition.bag_feed WHERE bag_id=$bagId";
        $res3 = $GLOBALS['pdo']->query($sql);
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);
        $bagStart = $row3['bag_start_footage'] * $scaleMultiplier;
        $bagStop = $row3['max_feed_footage'] * $scaleMultiplier;

//find the max and min consumption footages, we use different ones, depending on the direction.
        $sql = "SELECT max(footage) as max_consumption_footage FROM nutrition.bag_consumption WHERE bag_id=$bagId AND direction='forward' AND bag_consumption.event_time<= '$this->dateOfInterest'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $footageConsumptionForward = $row['max_consumption_footage'] * $scaleMultiplier;
        $sql = "SELECT min(footage) as min_consumption_footage FROM nutrition.bag_consumption WHERE bag_id=$bagId AND direction='reverse' AND bag_consumption.event_time<= '$this->dateOfInterest'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $footageConsumptionReverse = $row['min_consumption_footage'] * $scaleMultiplier;


        if ($direction == 'forward') {


            //don't show consumption when there is no consumption OR when bag is already fully consumed.
            if (($footageConsumptionForward != "") OR ( $footageConsumptionForward >= $bagStop)) {
                //draw the white box and jagged line
                $str = $str . self::drawWhiteConsumptionPart($bagId, $scaleMultiplier, $heightScale, $bagStart, $footageConsumptionForward - $bagStart);
                $str = $str . self::drawJaggedConsumptionLine($bagId, $scaleMultiplier, $heightScale, $footageConsumptionForward);
            }
        } elseif ($direction == 'reverse') {


            //don't show consumption when there is no consumption OR when bag is already fully consumed.
            if (($footageConsumptionReverse != "") OR ( $footageConsumptionReverse >= $bagStart)) {
                //draw the white box and jagged line

                $str = $str . self::drawWhiteConsumptionPart($bagId, $scaleMultiplier, $heightScale, $footageConsumptionReverse, $bagStop - $footageConsumptionReverse);
                $str = $str . self::drawJaggedConsumptionLine($bagId, $scaleMultiplier, $heightScale, $footageConsumptionReverse);
            }
        } elseif ($direction == 'both') {


            //don't show consumption when there is no consumption OR when bag is already fully consumed.
            if (($footageConsumptionForward != "") OR ( $footageConsumptionForward >= $bagStop)) {
                //draw the white box and jagged line
                $str = $str . self::drawWhiteConsumptionPart($bagId, $scaleMultiplier, $heightScale, $bagStart, $footageConsumptionForward - $bagStart);
                $str = $str . self::drawJaggedConsumptionLine($bagId, $scaleMultiplier, $heightScale, $footageConsumptionForward);
            }

            //don't show consumption when there is no consumption OR when bag is already fully consumed.
            if (($footageConsumptionReverse != "") OR ( $footageConsumptionReverse >= $bagStart)) {
                //draw the white box and jagged line 
                $str = $str . self::drawWhiteConsumptionPart2($bagId, $scaleMultiplier, $heightScale, $footageConsumptionReverse, $bagStop - $footageConsumptionReverse);
                $str = $str . self::drawJaggedConsumptionLine($bagId, $scaleMultiplier, $heightScale, $footageConsumptionReverse);
            }
        } elseif ($direction == 'none') {
            
        }//do nothing
        else {
            throw new Exception("Error: direction of bag consumption invalid!.</br>");
        }

        return $str;
    }

    /**
     * draws a white rectangle to hide the parts consumed.
     * now changed to backgroud of a tab and not white. 
     */
    function drawWhiteConsumptionPart($bag_id, $scaleMultiplier, $heightScale, $footageStartBag, $consumedLength) {
        $str = ""; //start with blank string
        $str = $str . "<!-- bag consumption white part-->";
        $str = $str . "<rect x = '$footageStartBag' y = '0' width = '$consumedLength' height = '" . ($heightScale * 18) . "' fill = '#f5f3e5' stroke = '#f5f3e5' fill-opacity='.85' stroke-width = '2'/>" . "\n\r";
        return $str;
    }

    function drawWhiteConsumptionPart2($bag_id, $scaleMultiplier, $heightScale, $footageStartBag, $consumedLength) {
        $str = ""; //start with blank string
        $str = $str . "<!-- bag consumption white part-->";
        $str = $str . "<rect x = '$footageStartBag' y = '0' width = '$consumedLength' height = '" . ($heightScale * 18) . "' fill = '#f5f3e5' stroke = '#f5f3e5' fill-opacity='.85' stroke-width = '2'/>" . "\n\r";
        return $str;
    }

    /**
     * just draws the jagged consumption line.
     */
    function drawJaggedConsumptionLine($bag_id, $scaleMultiplier, $heightScale, $footageConsumption) {
        //this does not check if bag is all consumed, just does what it is told.
        //since it would be a pain to support negative distances, we just move the 
        //consumtion line up so we can see it. change if we ever support negative distances. 
        if ($footageConsumption <= 0) {
            $footageConsumption = 2;
        }

        $xminus = $footageConsumption - 2;
        $xplus = $footageConsumption + 2;

        $str = ""; //start with blank string

        $str = $str . "<!-- bag consumption jagged line part-->";
        $str = $str . "<line x1='$xminus' y1='0' x2='$xplus' y2='" . $heightScale * 4.5 . "' stroke = 'black' stroke-width = '4'/>" . "\n\r";
        $str = $str . "<line x1='$xplus' y1='" . $heightScale * 4.5 . "' x2='$xminus' y2='" . $heightScale * 9 . "' stroke = 'black' stroke-width = '4'/>" . "\n\r";
        $str = $str . "<line x1='$xminus' y1='" . $heightScale * 9 . "' x2='$xplus' y2='" . $heightScale * 13.5 . "' stroke = 'black' stroke-width = '4'/>" . "\n\r";
        $str = $str . "<line x1='$xplus' y1='" . $heightScale * 13.5 . "' x2='$xminus' y2='" . $heightScale * 18 . "' stroke = 'black' stroke-width = '4'/>" . "\n\r";

        return $str;
    }

//FIXME add data from db
    function drawForageTest($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
        $sql = <<<SQL
                SELECT nutrition.bag_feed_name_at_footage(bag_id,footage),bag_id,footage,sample_number,info_template->'DM' as "DM", info_template->'RFQ' as "RFQ", info_template->'TDN' as "TDN",info_template->'CP' as "CP",info_template->'Ca' as "Ca",info_template->'P' as "P",
                    info_template->'K' as "K",info_template->'TDN'as "TDN",info_template->'ADF' as "ADF",info_template->'NDF' as "NDF",info_template->'lignin' as "lignin",info_template->'ash' as "ash",info_template->'starch' as "starch"        
                            FROM nutrition.bag_analysis 
                            WHERE bag_id=$bagId
SQL;

    
            
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_OBJ))) {
            $footage = $row->footage * $scaleMultiplier; //choose where to place text
//create String to display forage results for the given test.
            $forageTestResults = "Test Results:
DM: {$row->DM},\n\r\t\n\r\t
CP: {$row->CP},
ADF: {$row->ADF}, 
NDF: {$row->NDF},
Lignin: {$row->lignin},
Starch {$row->starch},
TDN {$row->TDN},
Ca {$row->Ca},
P {$row->P},
K {$row->K},
RFQ {$row->RFQ},
sample_number {$row->sample_number}
";

            $pdfUrl = "http://{$_SERVER['HTTP_HOST']}/functions/displayForageAnalysisPdf.php?sample_number={$row->sample_number}";


            $tdn = null;
            $rfq = null;
            switch ($row->bag_feed_name_at_footage) {
                case 'Corn Silage (BMR)':
                 $rfq = $row->TDN;
                    //use TDN as potenital digestibility, not sure this is right.
                    $retArr = CornSilageMilk2006Calculation::calculateMilk2006($row->DM, $row->CP, $row->NDF, $row->TDN, $row->ash, $row->CF, $row->starch, 1);
                    $forageTestResults = $forageTestResults . ' Milk 2006 lb/ton dm: ' . round($retArr['lb_milk_per_ton_dm']);
                    break;
                case 'Corn Silage':
                    $rfq = $row->TDN;
                    //use TDN as potenital digestibility, not sure this is right.
                    $retArr = CornSilageMilk2006Calculation::calculateMilk2006($row->DM, $row->CP, $row->NDF, $row->TDN, $row->ash, $row->CF, $row->starch, 1);
                    $forageTestResults = $forageTestResults . ' Milk 2006 lb/ton dm: ' . round($retArr['lb_milk_per_ton_dm']);
                    break;
                case 'Legume/Grass Mix':
                    $rfq = $row->RFQ; //alwyas use relative feed quality, maybe move to milk per ton.
                    break;
                case 'Haylage':
                    $rfq = $row->RFQ;
                    break;
                case 'Soybean Silage':
                    $rfq = $row->RFQ;
                    break;
                case 'Snaplage':
                    $rfq = $row->TDN;
                    break;
                default:
                    $rfq = $row->RFQ;
                    ; //unknown feed.
            }

            if (empty($rfq)) {
                $rfq = -1;
            }


//choose colour of dot based on rfq test.
            $color_code = $this->chooseColourCodeFromInt($rfq);
$rfq=round($rfq,1);
//draw colour coded circle and text with rfq.
            $str = $str . "<circle onclick='alert(\"$forageTestResults\");' cx='$footage' cy='8' r='8' stroke='#$color_code' stroke-width='1' fill='#$color_code'/>" . "\n\r";
            $str = $str . "<text   onclick='alert(\"$forageTestResults\");'  x='$footage' y='" . ($heightScale * 5) . "' dx='0,0,0,0,0' dy='0,0,0,0,2' textLength='' lengthAdjust='spacing' font-size='.5em' fill='#000000' text-anchor='middle' >$rfq</text>" . "\n\r";
            $str = $str . "<circle   onclick='window.open(\"$pdfUrl\");'  cx='$footage' cy='28' r='3' stroke='purple' stroke-width='1' fill='purple'/>" . "\n\r";
        }
        return $str;
    }

//chooses html colour code based on interger number.
    function chooseColourCodeFromInt($input) {

        switch ($input) {

            //junk
            case ($input <= 100):
                $color_code = "feec70";
                break;
            //dry cows  less than 100 to 115
            case ($input <= 107):
                $color_code = "f7bd4e";
                break;
            case ($input <= 115):
                $color_code = "f3b149";
                break;
            //heifer 12-18 months 115 to less than 125 in our case
            case ($input <= 125):
                $color_code = "f2a041";
                break;
            //change to different colour here
            //Low cows 125 to 140 
            case ($input <= 130):
                $color_code = "e35087";
                break;
            case ($input < 140):
                $color_code = "ef8995";
                break;
            //
            //Hight cows 140+ 
            case ($input <= 150):
                $color_code = "983191";
                break;
            case ($input <= 160):
                $color_code = "883191";
                break;
            case ($input <= 170):
                $color_code = "731f96";
                break;
            case ($input <= 180):
                $color_code = "5d1a88";
                break;
            case ($input <= 200):
                $color_code = "4e1872";
                break;
            case ($input <= 300):
                $color_code = "3d145a";
                break;
            default:
                $color_code = '09f3ff';
        }

        return $color_code;
    }

    function drawBagId($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
        $sql = "SELECT footage_start FROM nutrition.bag_feed WHERE bag_id=$bagId  ORDER BY footage_start ASC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $footage = ($row['footage_start'] + 20) * $scaleMultiplier; //choose where to place text
            $str = $str . "<text  x='$footage' y='" . ($heightScale * 6) . "' dx='0,0,0,0,0' dy='0,0,0,0,0' fill='#0000FF' textLength='50' lengthAdjust='spacing' font-size='.85em' text-anchor='middle' >Bag #$bagId</text>" . "\n\r";
        }
        return $str;
    }

    function drawComment($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
        $sql = "SELECT footage,comment FROM nutrition.bag_comment WHERE bag_id=$bagId";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $footage = $row['footage'] * $scaleMultiplier; //choose where to place text
            $str = $str . "<text  x='$footage' y='" . ($heightScale * 6.5) . "' dx='0,0,0,0,0' dy='0,0,0,0,0' textLength='0' lengthAdjust='spacing' font-size='.5em' text-anchor='middle' >{$row['comment']}</text>" . "\n\r";
        }
        return $str;
    }

    function drawEnsileDate($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
        $sql = "SELECT footage,event_time FROM nutrition.bag_ensile_date WHERE bag_id=$bagId";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $footage = $row['footage'] * $scaleMultiplier; //choose where to place text
            $date = date('M d/y', strtotime($row['event_time']));
            $str = $str . "<text  x='$footage' y='" . ($heightScale * 16) . "' dx='0,0,0,0,0' dy='0,0,0,0,0' textLength='' lengthAdjust='spacing' font-size='.5em' text-anchor='middle' >$date</text>" . "\n\r";
        }
        return $str;
    }

//FIXME
    function drawFieldNames($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
        $sql = "SELECT footage_start,footage_finish,alpha_numeric_id FROM nutrition.bag_field 
LEFT JOIN cropping.field ON bag_field.field_id = field.id
WHERE bag_id=$bagId";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $footage = (($row['footage_finish'] + $row['footage_start']) / 2) * $scaleMultiplier; //choose where to place text
            $footageStart = $row['footage_start'] * $scaleMultiplier;
            $footageFinish = $row['footage_finish'] * $scaleMultiplier;
            $str = $str . "<text  x='$footage' y='" . ($heightScale * 11) . "' dx='0,0,0,0,0' dy='0,0,0,0,0'  font-size='.7em' text-anchor='middle' >{$row['alpha_numeric_id']}</text>" . "\n\r";
            $str = $str . "<line x1='$footageStart' y1='0' x2='$footageStart' y2='" . ($heightScale * 18) . "' stroke = 'black' stroke-width = '1'/>" . "\n\r";
            $str = $str . "<line x1='$footageFinish' y1='0' x2='$footageFinish' y2='" . ($heightScale * 18) . "' stroke = 'black' stroke-width = '1'/>" . "\n\r";
        }
        return $str;
    }

    function drawSlotNumber($scaleMultiplier, $heightScale, $slot, $bagId) {
        $str = ""; //start with blank string

        $x1 = 328 * $scaleMultiplier;
        $y1 = 7.75 * $scaleMultiplier;
        $x2 = $x1 + 40;
        $str = $str . "<text  x='$x1' y='$y1' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='2.4em' text-anchor='middle' >$slot</text>" . "\n\r";
        $str = $str . "<text  x='$x2' y='$y1' dx='0,0,0,0,20' dy='0,0,0,0,0' textLength='' lengthAdjust='' font-size='0.5em' text-anchor='middle' >$bagId</text>" . "\n\r";
        return $str;
    }

    function drawDistances($scaleMultiplier, $heightScale, $slot) {
        $str = ""; //start with blank string

        if ($slot == $this->lastSlot) {

            $xaa = -50 * $scaleMultiplier;
            $x1 = 1 * $scaleMultiplier;
            $xa = 50 * $scaleMultiplier;
            $x2 = 100 * $scaleMultiplier;
            $xb = 150 * $scaleMultiplier;
            $x3 = 200 * $scaleMultiplier;
            $xc = 250 * $scaleMultiplier;
            $x4 = 300 * $scaleMultiplier;
            $y = 26.25 * $heightScale;
            
            $str = $str . "<text  x='$xaa' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >-50'</text>" . "\n\r";
            $str = $str . "<text  x='$x1' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >0'</text>" . "\n\r";
            $str = $str . "<text  x='$xa' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >50'</text>" . "\n\r";
            $str = $str . "<text  x='$x2' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >100'</text>" . "\n\r";
            $str = $str . "<text  x='$xb' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >150'</text>" . "\n\r";
            $str = $str . "<text  x='$x3' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >200'</text>" . "\n\r";
            $str = $str . "<text  x='$xc' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >250'</text>" . "\n\r";
            $str = $str . "<text  x='$x4' y='$y' dx='0,0,0,0,20' dy='0,0,0,0,8' textLength='' lengthAdjust='spacing' font-size='.9em' text-anchor='middle' >300'</text>" . "\n\r";
        }

//print 10' tick marks
        for ($counter = 0; $counter <= 1300; $counter += 40) {
            $str = $str . "<line x1='$counter' y1='37' x2='$counter' y2='39' stroke = 'grey' stroke-width = '1'/>" . "\n\r";
        }
        //print darker ticks at 50' inetrvals
        for ($counter = 0; $counter <= 1300; $counter += 200) {
            $str = $str . "<line x1='$counter' y1='37' x2='$counter' y2='39' stroke = 'black' stroke-width = '3'/>" . "\n\r";
        }


        return $str;
    }

    function drawFeedInBag($bagId, $scaleMultiplier, $heightScale) {
        $str = ""; //start with blank string
        $sql = "SELECT footage_start,footage_finish,colour_code FROM nutrition.bag_feed 
LEFT JOIN nutrition.feed_type on bag_feed.feed_id = feed_type.id
WHERE bag_id=$bagId";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //format the footages for the svg x and y.
            $footage_start = $row['footage_start'] * $scaleMultiplier;
            $footage_finish = abs(($row['footage_finish'] - $row['footage_start']) * $scaleMultiplier);

            $str = $str . "<rect x='$footage_start' y='0' width='$footage_finish' height='" . $heightScale * 18 . "' fill='#{$row['colour_code']}' stroke='black' stroke-width='1'/>" . "\n\r";
        }
        return $str;
    }

    function drawBlankBag($bagId, $scaleMultiplier, $heightScale) {
//find the least footage_start and greatest footage finish of the bag.
        $sql = "SELECT min(footage_start) as min_footage_start FROM nutrition.bag_feed WHERE bag_id=$bagId";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $bagStartFootage = $row['min_footage_start'] * $scaleMultiplier;
//
        $sql = "SELECT max(footage_finish) as max_footage_finish FROM nutrition.bag_feed WHERE bag_id=$bagId";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $bagWidth = abs(($row['max_footage_finish'] * $scaleMultiplier) - $bagStartFootage);

        $str = "<!-- Blank bag -->" . "\n\r";
        $str = $str . "<rect x='$bagStartFootage' y='0' width='$bagWidth' height='" . $heightScale * 18 . "' fill='brown' stroke='black' stroke-width='1'/>" . "\n\r";

        return $str;
    }

    public function showSVG($svg) {
        
        print($svg);
    }

}

//end class
?>