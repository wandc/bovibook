<?php
class BagConsumption extends TabPage {

    private $page_id;
    private $date_of_interest = null;

    public function __construct() {
        parent::__construct();
        if (!empty($_REQUEST['pageid'])) {
            //sometimes called from another class, ie sidebar, thus we need to dyniamically find the page_id.
            $this->pageid = findFileNameOfClass(strtolower(get_class($this)));
        }
        if (isset($_REQUEST['date_of_interest']) AND ( $_REQUEST['date_of_interest'] != null)) {
            $this->date_of_interest = $_REQUEST['date_of_interest'];
        }
    }

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Consumption';
        $tabArray[2]['name'] = 'Historical';
        $tabArray[3]['name'] = 'Historical By Type';
        $tabArray[2]['extra_callback_param'] = "&date_of_interest={$this->date_of_interest}";
        $this->renderTabs($tabArray);
    }

    protected function tab1() {
        
       
$entry = '';
        if (in_array('nutritionist', $GLOBALS['auth']->getAuthData('groups')) == FALSE) { //don't allow nutrtionist to edit anything.
            $this->param = new stdClass();
            $a = new QF2_SilageBagConsumptionEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'list', $this->param); //group


            $entry = ($a->renderBox('QF2_SilageBagConsumptionEvent', 'Add Consumption Note About Ag Bag'));
        }

        $rfq = ("<img src='images/sop/relative_forage_quality_RFQ.jpg'/>"); // source: http://extension.uga.edu/publications/detail.cfm?number=B1425


        $left = $this->displayBagConsumptionLog() . $rfq ;
        $right = $entry.$this->displayBagConsumptionProjection();
        print (new Bootstrap)->oneThirdTwoThirds($left, $right);

       
    }

    protected function tab2() {
        if ($this->date_of_interest != null) {
            $this->dateSelectQuickForm();
            /** put everything here */
            $a = date('Y-m-d', ($this->date_of_interest));

            $DrawSVGBags = new DrawSVGBags(date('Y-m-d', ($this->date_of_interest)));
            print($DrawSVGBags->main());
        }
        //when they are not set, show field select list.
        else {
            $this->dateSelectQuickForm();
        }
    }

    //show yield be field and type for a whole year.
    protected function tab3() {

        /* shows yield from the bags by looking at all the fields, what year they were ensiled, there field areas at that time and then sums it up to show area cut of each type of forage */
        $sql = <<<SQL
with temp as (
SELECT alpha_numeric_id,bag.event_time,bag_id,field_id,footage_start,footage_finish,nutrition.bag_ensile_date_at_footage(bag_id,footage_finish-footage_start), cropping.field_border_at_timestamp(field_id, nutrition.bag_ensile_date_at_footage(bag_id,footage_finish-footage_start)),nutrition.bag_feed_name_at_footage (bag_id, (footage_finish-footage_start)::integer)
FROM nutrition.bag_field 
LEFT JOIN nutrition.bag on bag_id=bag.id
LEFT JOIN cropping.field ON field.id=field_id
), temp2 as (
SELECT temp.*,EXTRACT(YEAR FROM bag_ensile_date_at_footage) as year,cropping.datum_area(datum.id) as area from temp
LEFT JOIN cropping.datum ON datum.id=field_border_at_timestamp
), temp3 as (
SELECT year,bag_feed_name_at_footage,sum(area) over (partition by year,bag_feed_name_at_footage) from temp2
)
SELECT distinct(year,bag_feed_name_at_footage,sum) , year,bag_feed_name_at_footage,sum  FROM temp3 ORDER BY year DESC
SQL;

       $statement = $GLOBALS['pdo']->prepare($sql);
       $statement->execute();
       $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Year'] = ($row['year']);
            $rowOut['Feed'] = ($row['bag_feed_name_at_footage']);
            $rowOut['Sum'] = ($row['sum']);
            
            $resultsOut[] = $rowOut;
        }


        //output
        $dataArr = $resultsOut;

        $out[] = ('<h3>Amount of Feed in Bags Over Time</h3>');
         $out[] = ('<h3>FIXME: Data looks incorrect.</h3>');
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order:false'));
        return implode($out);
      
    }

    /**
     * Small form that selects the field.
     */
    public function dateSelectQuickForm($mode = 'default') {
        $datesArray = $GLOBALS['MiscObj']->createMonthlyDatesForPrev56Months();

        //javascript to allow submission on change of select element.
        $attrs = array('style' => "width: 95px", 'onchange' => "javascript:location.href='?date_of_interest='+this.options[this.selectedIndex].value+'&pageid={$this->pageid}';$('#example > ul').tabs('select', 0);");

        //javascript to allow submission when enter is pressed within select form
        //$attrs= array('onKeyPress' => "javascript:if(event.keyCode=='13'){document.frmTest.submit();}");

        $form = new HTML_QuickForm("frmFieldQuickSelect", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        //$form->addElement('header', 'hdrQuickfrmFieldQuickSelect','Select a Field');
        $form->addElement('select', 'dateInterest', '', $datesArray, $attrs);
        $form->addElement('hidden', 'pageid', $this->pageid);

        //defaults
        if ($this->date_of_interest != null) {
            $form->setDefaults(array('dateInterest' => $this->date_of_interest)); //set to current field
        } else {
            $form->setDefaults(array('dateInterest' => 0)); //set default to no.  
        }


        // Try to validate a form
        if ($form->validate()) {

            //get value
            $this->date_of_interest = $form->exportValue('dateInterest');
            $this->pageid = $form->exportValue('pageid');
        } //end validation
        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    

    function displayBagConsumptionLog() {

        $sql = <<<SQL
SELECT bag_id,bag_consumption.footage,bag_consumption.event_time,location,slot,current_feed,bag_consumption.userid,direction,substring(location from 1 for 1)|| slot as location_slot,bag_consumption.id
		FROM nutrition.bag_consumption
                LEFT JOIN nutrition.bag_with_current_feed ON bag_with_current_feed.id = bag_consumption.bag_id ORDER BY event_time DESC,bag_id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

         $delButton = new QF2_DeleteButtonDecider('anyuser_timeLimit', (object) array('dateTimePeriodDays' => '4', 'schemaTable' => 'nutrition.bag_consumption'));
        
        
        //change presentation
        foreach ($results as &$row) {

            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Bag #'] = ($row['location_slot']);
            $rowOut['Feed'] = ($row['current_feed']);
            $rowOut['Footage'] = ($row['footage']);
            $rowOut['Direction'] = ($row['direction']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Action'] = $delButton->output($row['id']);
            
            
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Consumption of Feed in Ag Bags', $dataArr, 'order:false'));
        return implode($out);
    }

    public function openBagsWithValidFootages($removeFirstFootage = false, $sql = false) {

        $debug = false; //turn on to see debug code.
//allow default sql to be overridden.
        if ($sql != false) {
            $sqlQuery = $sql;
        } else {
            $sqlQuery = "SELECT bag_current_open.id,bag_current_open.id2,bag_current_open.direction,bag_current_open.location,bag_current_open.slot,bag_current_open.event_time,bag_current_open.comment,current_feed,footage FROM nutrition.bag_current_open ORDER BY event_time ASC";
        }


        /** If the Bag is being consumed in the normal forward direction */
        //grab a list of currently open bags and what there feed is
        $res = $GLOBALS['pdo']->query($sqlQuery); {
            
        } // Always check that result is not an error
        $listOfBags[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $listOfBags[$row['id2']] = 'Bag: ' . $row['location'] . ' ' . $row['slot'] . ' | ' . $row['direction'] . ' - ' . $row['current_feed'] . ' - ' . date('M j/y', strtotime($row['event_time']));
            $listOfBagsBasic[$row['id']]  = 'Bag: ' . $row['location'] . ' ' . $row['slot'] . ' | ' . $row['direction'] . ' - ' . $row['current_feed'] . ' - ' . date('M j/y', strtotime($row['event_time']));

            // has to be done this way because of hierselect.

            /** Find max and min consumption notes and direction */
            //supports bag with both ends open
            $res2 = $GLOBALS['pdo']->query("
                WITH tt as(
SELECT max(bag_consumption.footage) as max_consumption_footage,direction FROM nutrition.bag_consumption  WHERE bag_id={$row['id']} AND direction='forward'
Group BY direction
UNION
SELECT min(bag_consumption.footage) as max_consumption_footage,direction FROM nutrition.bag_consumption  WHERE bag_id={$row['id']} AND direction='reverse'
Group BY direction
)
SELECT max(max_consumption_footage) as max_consumption_footage FROM tt
");
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $max_consumption_footage = $row2['max_consumption_footage'];

            //supports bag with both ends open
            $res3 = $GLOBALS['pdo']->query(" WITH tt as(
SELECT max(bag_consumption.footage) as min_consumption_footage,direction FROM nutrition.bag_consumption  WHERE bag_id={$row['id']} AND direction='forward'
Group BY direction
UNION
SELECT min(bag_consumption.footage) as min_consumption_footage,direction FROM nutrition.bag_consumption  WHERE bag_id={$row['id']} AND direction='reverse'
Group BY direction
)
SELECT min(min_consumption_footage) as min_consumption_footage FROM tt
");
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);
            $min_consumption_footage = $row3['min_consumption_footage'];

            $res5 = $GLOBALS['pdo']->query("SELECT max(bag_feed.footage_finish) as max_feed_footage FROM nutrition.bag_feed WHERE bag_id={$row['id']}");
            $row5 = $res5->fetch(PDO::FETCH_ASSOC);
            $max_feed_footage = $row5['max_feed_footage'];

            $res6 = $GLOBALS['pdo']->query("SELECT min(bag_feed.footage_start) as min_feed_footage FROM nutrition.bag_feed WHERE bag_id={$row['id']}");
            $row6 = $res6->fetch(PDO::FETCH_ASSOC);
            $min_feed_footage = $row6['min_feed_footage'];


            //debug
            if ($debug == true) {
                print("<br/>Bag:{$row['id2']} slot:" . $row['slot'] . ' | ' . $row['location'] . "<br/>");
                print("max_consumption_footage:$max_consumption_footage<br/>");
                print("min_consumption_footage:$min_consumption_footage<br/>");
                print("max_feed_footage:$max_feed_footage<br/>");
                print("min_feed_footage:$min_feed_footage<br/>");
            }

            //since bags with two directions are on the list twice (forward and reverse), we do each individually.
            $direction = $row['direction'];

            if ($debug == true) {
                print("Direction:$direction<br/>");
            }

            //do a query to see if bag has both ends open.
            $res8 = $GLOBALS['pdo']->query("SELECT DISTINCT(direction) FROM nutrition.bag_consumption WHERE bag_id={$row['id']}");
            if ($res8->rowCount() == 2) {
                $bothEnds = true;
            } else {
                $bothEnds = false;
            }


            if ($direction == "") {
                $final_footage_min = $min_feed_footage;
                $final_footage_max = $max_feed_footage;
            } elseif (($direction == 'forward') && ($bothEnds == false)) {

                /** three cases:
                  Case 1. bag is not opened yet.
                  Case 2. bag has started to be consumed.
                  Case 3. bag is fully consumed.
                 */
                //CASE 1 //never called
                if (($max_consumption_footage) == "") {
                    $final_footage_min = $min_feed_footage;
                    $final_footage_max = $max_feed_footage;
                }
                //CASE 2 
                elseif (is_numeric($max_consumption_footage) == true AND ( $max_consumption_footage <= $max_feed_footage)) {
                    $final_footage_min = $max_consumption_footage;
                    $final_footage_max = $max_feed_footage;

                    //for consumption remove the first footage as it is already used.
                    if ($removeFirstFootage == true) {
                        $final_footage_min = $final_footage_min + 1;
                    }
                }
                //CASE 3
                elseif (is_numeric($max_consumption_footage) == true AND ( $max_consumption_footage = $max_feed_footage)) {
                    $final_footage_min = $max_feed_footage;
                    $final_footage_max = $max_feed_footage;
                } else {
                    throw new Exception("Error: Should never be called. For Bag consumption Direction $direction the programming logic is wrong!");
                }

                if ($debug == true) {
                    print("footMin:$final_footage_min::footMax:$final_footage_max<br>");
                }

                //forward direction
                for ($i = $final_footage_min; $i <= $final_footage_max; $i++) {
                    $listOfFootages[$row['id2']][$i] = "$i'";
                }
            }//end forward
            elseif (($direction == 'reverse') && ($bothEnds == false)) {
                /** three cases:
                  Case 1. bag is not opened yet.
                  Case 2. bag has started to be consumed.
                  Case 3. bag is fully consumed.
                 */
                //CASE 1 //never called
                if (($min_consumption_footage) == "") {
                    $final_footage_min = $max_consumption_footage;
                    $final_footage_max = $min_feed_footage;
                }
                //CASE 2 
                elseif (is_numeric($min_consumption_footage) == true AND ( $min_consumption_footage >= $min_feed_footage)) {

                    $final_footage_min = $max_consumption_footage;
                    $final_footage_max = $min_feed_footage;

                    //for consumption remove the first footage as it is already used.
                    if ($removeFirstFootage == true) {
                        $final_footage_min = $final_footage_min - 1;
                    }
                }
                //CASE 3
                elseif (is_numeric($min_consumption_footage) == true AND ( $min_consumption_footage = $min_feed_footage)) {
                    $final_footage_min = $max_feed_footage;
                    $final_footage_max = $max_feed_footage;
                } else {
                    throw new Exception("Error: Should never be called. For Bag consumption Direction $direction the programming logic is wrong!");
                }
                if ($debug == true) {
                    print("footMin:$final_footage_min::footMax:$final_footage_max<br>");
                }
                //reverse direction
                for ($i = $final_footage_min; $i >= $final_footage_max; $i--) {
                    $listOfFootages[$row['id2']][$i] = "$i'";
                }
            }//end reverse
            elseif ($bothEnds == true) {
                /** three cases:
                  Case 1. bag is not opened yet.
                  Case 2. bag has started to be consumed.
                  Case 3. bag is fully consumed.
                 */
                //CASE 1 //never called
                if (($min_consumption_footage) == "") {
                    $final_footage_min = $min_feed_footage;
                    $final_footage_max = $max_feed_footage;
                }
                //CASE 2 
                elseif (is_numeric($min_consumption_footage) == true AND ( $min_consumption_footage >= $min_feed_footage)) {

                    //when both ends are being consumed.

                    $final_footage_min = $min_consumption_footage;
                    $final_footage_max = $max_consumption_footage;


                    //for consumption remove the first footage as it is already used.
                    if ($removeFirstFootage == true) {
                        $final_footage_max = $final_footage_max - 1;
                        $final_footage_min = $final_footage_min + 1;
                    }
                }
                //CASE 3
                elseif (is_numeric($min_consumption_footage) == true AND ( $min_consumption_footage = $min_feed_footage)) {
                    $final_footage_min = $max_feed_footage;
                    $final_footage_max = $max_feed_footage;
                } else {
                    throw new Exception("Error: Should never be called. For Bag consumption Direction $direction the programming logic is wrong!");
                }

                if ($debug == true) {
                    print("footMin:$final_footage_min::footMax:$final_footage_max<br>");
                }

                if ($direction == 'forward') {
                    //forward direction of both ends
                    for ($i = $final_footage_min; $i <= $final_footage_max; $i++) {
                        $listOfFootages[$row['id2']][$i] = "$i'";
                    }
                } elseif ($direction == 'reverse') {
                    //reverse direction of both ends
                    for ($i = $final_footage_max; $i >= $final_footage_min; $i--) {
                        $listOfFootages[$row['id2']][$i] = "$i'";
                    }
                } else {
                    throw new Exception("Error: Should never be called. Bag Direction not forward or reverse");
                }
            }//end  both ways
            else {
                throw new Exception("Error: Should never be called. Bag Direction not forward or reverse");
            }
        }
        //print_r($listOfFootages[$row['id2']]);

        $retArray[] = $listOfBags;
        $retArray[] = $listOfFootages;
        $retArray[] = $listOfBagsBasic; //just has bag id, no direction.
        return $retArray;
    }

    /* predict when currently open bags will run out of feed at current feeding rates. Very simplistic. */

    function displayBagConsumptionProjection() {
        $arr=array();

//find bags that are open.
        $sql = "SELECT bag_current_open.id,bag_current_open.slot,bag_with_current_feed.location,bag_with_current_feed.current_feed FROM nutrition.bag_current_open 
LEFT JOIN nutrition.bag_with_current_feed ON bag_current_open.id=bag_with_current_feed.id ORDER BY slot";
        $res1 = $GLOBALS['pdo']->query($sql);
        while (($row1 = $res1->fetch(PDO::FETCH_ASSOC))) {
            $bagid = $row1['id'];

            /* returns the average number of seconds to consume 1 foot of bag */
            $sql = "/* find min event time and all event times and footages */
With t1 as (
SELECT id,(SELECT min(event_time) FROM nutrition.bag_consumption WHERE bag_id=$bagid) as min_event_time,event_time,footage FROM nutrition.bag_consumption WHERE bag_id=$bagid ORDER BY event_time
),  
/*find min footage */
t2 as (
SELECT (SELECT footage FROM nutrition.bag_consumption WHERE bag_id=$bagid AND event_time=min_event_time limit 1) as min_footage,* FROM t1
),
t3 as (
/*subtract footages from mins and calculate time in seconds.*/
SELECT footage-min_footage as footage,EXTRACT(EPOCH FROM (event_time-min_event_time)) as time FROM t2 WHERE footage-min_footage != 0
)
/*return the average number of seconds to consume one foot.*/
SELECT (time/footage)  as average FROM t3";
            $res = $GLOBALS['pdo']->query($sql);
            $average = null;
            while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
                $average = $row['average'];
            } //skip to end because the last value is the most aervaged of greatest time and footage
//now find ending footage of that bag.
            $sql = "SELECT max(footage_finish) as endfootage FROM nutrition.bag_feed WHERE bag_id=$bagid";
            $res2 = $GLOBALS['pdo']->query($sql);
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
//now find current footage of that bag.
            $sql = "SELECT max(footage) as currentfootage FROM nutrition.bag_consumption WHERE bag_id=$bagid";
            $res3 = $GLOBALS['pdo']->query($sql);
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);
//find days left

            $footageLeft = $row2['endfootage'] - $row3['currentfootage'];
            $days = round($footageLeft * $average / 86400, 1);
              $arr[]=("Slot {$row1['slot']}  {$row1['current_feed']} : $days days <br>");
        }
        
         $header=("<br><h3>Cosumption Projection (assuming forward only, whole bag)</h3>");
        
         return (new BootStrap)->plainCard($header, implode($arr));
  
        
        
    }

    
   
    
}//end class

Class QF2_SilageBagConsumptionEvent extends QF2_Base {

    function definition($form) {

 //
        
        
          //daily dates rray, with unix timestamp as index.
        $dateArray = $GLOBALS['MiscObj']->createDailyDatesAtDailyIntervalsForLastSevenDays();


        //get open bags with footages availble in bag.
        $retArr = (new BagConsumption)->openBagsWithValidFootages(true);
      
        $listOfBags = $retArr[2]; //NOTE: just bag id, not bag|direction
        $listOfFootages = $retArr[1];

        //__autoload('AgBagAdmin');
       // include 'agbagdmin.inc';
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/nutrition/agbagadmin.inc');
        $listOfFootagesGeneric=(new AgBagAdmin)->listOfFootages();
        
        
            $form->addElement('select', 'direction', null, array('options' => array('forward' => 'forward', 'reverse' => 'reverse'), 'label' => 'Which end open'))->addRule("required", "Direction is required");
   
            $form->addElement('select', 'bagNumber', null, array('options' => $listOfBags, 'label' => 'Bag'))->addRule("required", "Bag is required");
   
            $footage=$form->addElement('select', 'footage', null, array('options' =>$listOfFootagesGeneric, 'label' => 'Footage'));
            $footage->addRule("required", "Footage is required.");
           // $form->addRule('callback', 'ERROR: Footage is out of bag range.', array('callback' => array($this, 'checkFootageValid'), 'arguments' => array()));
            
           //FIXME: This should be a hier selct, but it is broken with bootstrap. 
           
           $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time");
           
         //   $form->addElement('hierselect', 'bag',null,array('options' =>   array($listOfBags,  $listOfFootages), 'label' => 'Bag', 'XXXclass'=>'foobar')  )->addRule("required", "Bag is required");
      //   $sel = $form->addElement('hierselect', 'cds')->setLabel('Choose CD:');
       //  $sel->loadOptions(array($listOfBags, $listOfFootages));
 
      /*   
         // first select

 $select1[0] = 'Pop';

 $select1[1] = 'Classical';

 $select1[2] = 'Funeral doom';

 

 // second select

 $select2[0][0] = 'Red Hot Chilly Peppers';

 $select2[0][1] = 'The Pixies';

 $select2[1][0] = 'Wagner';

 $select2[1][1] = 'Strauss';

 $select2[2][0] = 'Pantheist';

 $select2[2][1] = 'Skepticism';
*/
 

 // Two selects
/*
 $sel = $form->addElement('hierselect', 'cds')->setLabel('Choose CD:');

 $sel->loadOptions(array($select1, $select2));
  */       
           
   
        //defaults
         $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('direction' => 'forward','bag' => 0,'eventTime' => 0)));
        
        
        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);
        
        /*
        //breakup hirselect
        $bagArr = explode("|", $value['bag'][0]);
        $bag_id=$bagArr[0];
        $bag_footage=$value['bag'][1];
        */
        
        //using standard selects
        $bag_id=$value['bagNumber'];
        $bag_footage=$value['footage'];
        
       
              //don't put id value, uses next one in DB.
            $query = "INSERT INTO nutrition.bag_consumption (bag_id,footage,event_time,userid,direction) VALUES ($bag_id,$bag_footage,'{$value['event_time']}','{$GLOBALS['auth']->getUsername()}','{$value['direction']}')";
    
            $res = $GLOBALS['pdo']->exec($query);
    }
    
     //basic check, DOES NOT TAKE WHAT FEED IS CONSUMED INTO ACCOUNT. 
    function checkFootageValid($value) {

        $bag_id = $value['bagNumber'];
        $footage= $value['footage'];

        $res = $GLOBALS['pdo']->query("SELECT * FROM nutrition.bag_current_open LEFT JOIN nutrition.bag_feed ON bag_id=bag_current_open.id WHERE bag_id=$bag_id AND footage_start>=$footage AND footage_finish<=$footage ");
        
        if ($res->rowCount() >= 1) {
            $returnStatement = true; //means footage is valid
        } else {
            $returnStatement = false; //means footage is not valid
        }

        return $returnStatement;
    }


}//end class