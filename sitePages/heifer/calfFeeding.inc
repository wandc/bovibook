<?php

/*
 * Dec 19 2017
 * Urban Calf Feeder Main Display Page
 * 
 */

class CalfFeeding extends BasePage {

    var $pdoUrban;
    protected $cosumptionArr;

    function __construct() {
        //try and connect to calf feeder
        try {
            $pdoDsnUrbanCalf = ($GLOBALS['config']['PDO']['dsnUrbanCalf']); //local socket
            $pdoUrban = new PDO($pdoDsnUrbanCalf);
            //$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES,false); //is this slow? no idea
            $pdoUrban->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); //DEBUG:: turns on DB debugging site wide.
            $this->pdoUrban = $pdoUrban;
        } catch (PDOException $Exception) {
            print("Exception: " . $Exception->getMessage());
            throw new PDOException($Exception->getMessage(), $Exception->getCode());
        }

        $this->cosumptionArr[9] = $this->consumptionSinceXhour(9);
        $this->cosumptionArr[15] = $this->consumptionSinceXhour(15);
    }

    public function defaultDisplay() {



        //print_r2($this->cosumptionArr);
        /*

          $medicalTemperature=new QF2_MedicalTemperature((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
          print($medicalTemperature->renderBox('qf_medicalTemp','dsf'));
         */





        /*
         * Forms
         */

        //manual milk qf2 form
        $calfFeedManualMilk = new QF2_CalfFeedingMilk((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group');

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllYoungCalves(); //pass along list of animals to show.

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/reproduction/estrusProtocol.inc');
        $paramCalf = new stdClass();
        $paramCalf->type = 'calf';
        $paramCalf->bovineID = null;
        $paramCalf->aniNumber = $aniNumber;
        $a = new QF2_Protocol((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => null)), 'group', $paramCalf);



        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
        $paramMed = new stdClass();
        $paramMed->aniNumber = $aniNumber;
        $paramMed->bovineID = null;
        $b = new QF2_GeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', $paramMed); //group

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/groupTreatment.inc');
        //$paramDeh= new stdClass();
        $d = new QF2_Dehorn((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', $paramMed);


        $htmlInner = <<<HTML
      
        <div class="row">
            <div class="col-md-8">
              {$this->displayCurrentlyInFeederHTML()}
              {$this->displayCurrentlyInFeeder()} <!-- display who is in feeder now with ajax. -->            
            </div>
            <div class="col-md-4">
                {$this->displayTotalConsumption()}  
               
            </div>
        </div>
        <div class="row">
            <div class="col-md-8">
                {$this->displayWashLog()}
            </div>
            <div class="col-md-4">
               
              
                {$this->displayTempLog()}
            </div>
        </div>   
           
           
        <div class="row">
         <div class="col-md-6">
   
      </div>
        <div class="col-md-6">
    <!-- No Content Yet -->
    </div>
    </div>
        

        
   {$this->displayCurrentUrbanFeederErrorMessages()}  <!--ajax every 10 seconds to see if there is an error. -->
   
   
   <div class="row">
            <div class="col-md-6">
            {$this->calvesBornButNotRegisteredInFeeder()}  
            </div>
            <div class="col-md-6">
             {$this->displayUpcomingCalfMedsToAdminister()}   
             </div>    
    </div>
                
     
   
   {$calfFeedManualMilk->renderBox('qf_manualMilk', 'Record Manual Milk Event')}

   {$b->renderBox('qf2_GeneralTreatment', 'Select Animal to Treat')}

   {$a->renderBox('QF2_Protocol_Calf', 'Record Calf Protocol Event')}

        {$d->renderBox('qf2_Dehorn', 'Select Animals / Groups to Dehorn')}

   {$this->manuals()}
 
HTML;

        $html = <<<HTML
 <div class="row">
         <div class="col-md-4">
        $htmlInner
        </div>
        <div class="col-md-8">
            {$this->displayEmergencyAttentionList()}
            {$this->calfList()}
        </div> 
</div>
<br/>
 <hr>   
HTML;


        print($html);
    }

    private function displayTotalConsumption() {


        $sum = round($this->cosumptionArr[9][0]['total'], 1);
        $conNine = "<h4><b>Since 9am</b> $sum ℓ</h4>";
        $sum2 = round($this->cosumptionArr[15][0]['total'], 1);
        $conFifteen = "<h4><b>Since 3pm</b> $sum2 ℓ </h4>";

        return (new BootStrap)->plainCard('Total Consumption', $conNine . $conFifteen);
    }

    private function manuals() {

        $sop = <<<HTML
   <!-- HELP -->
     <button type=button class="btn btn-default btn-lg" onclick="location.href='{$GLOBALS['config']['HTTP']['URL']}?pageid=89&tabtocallAnchor=36';">
            <span class="glyphicon glyphicon-question-sign" aria-hidden=true></span>
        </button> 
            SOP<br/>

HTML;

        $docs = (new Documents)->displayeListOfDocuments('machine', 74);

        return (new BootStrap)->plainCard('Manual', $sop . $docs);
    }

    function calvesBornButNotRegisteredInFeeder() {
        //complicated query because we can't just use location info, in case the crew moves calf in computer, but not physically.
        $sql = "SELECT id,local_number,location_id from bovinemanagement.bovinecurrall WHERE birth_date > current_date - interval '10 days' AND id NOT IN (
with temp as (
select distinct on(responder_nr) responder_nr as partial_rfid_number,tiere_id,
(SELECT terminal || '.' || automat || '.' || station || '.' ||  box as feedstall from urban_feeder_foreign_tiere.tabelle_stationsbesuch where tiere_id=identifikation.tiere_id order by letzte_erkennung DESC limit 1) as feedstall
FROM  urban_feeder_foreign_tiere.identifikation  WHERE substring(responder_nr from 1 for 3) != 'DEL'
)
select id as bovine_id
FROM temp
LEFT JOIN bovinemanagement.bovineall ON partial_rfid_number::text=(bovinemanagement.rfid_number_to_partial(rfid_number))::text
LEFT JOIN bovinemanagement.location_urban_feedstall on bovineall.location_id=location_urban_feedstall.location_id
)";
        $res2 = $GLOBALS['pdo']->query($sql);
        $out = array();
        $start = '<ul>';
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $out[] = '<li>' . ' <a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row2['id']) . '">' . $row2['local_number'] . "</a> </li> ";
        }
        $end = '</ul>';

        $colour = !empty($out) ? 'yellow' : 'green';

        return((new BootStrap)->bootstrapSmallBox('Calves Not Moved Yet', $start . implode($out) . $end, null, $colour, 'fas fa-truck'));
    }

    function upcomingCalfMedsToAdminister() {
        if (date('a', strtotime("now")) == 'am') {
            $sql2 = "SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type='Scheduled Meds' AND (event_time >= current_date + interval '0 hours') AND  (event_time < current_date + interval '12 hours') AND  location_id IN(64,65,66,67,68) 
		ORDER BY medical_summary.event_time ASC";
        } else { //pm
            $sql2 = "SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type='Scheduled Meds' AND (event_time >= current_date + interval '12 hours') AND  (event_time < current_date + interval '24 hours') AND  location_id IN(64,65,66,67,68) 
		ORDER BY medical_summary.event_time ASC";
        }
        $res2 = $GLOBALS['pdo']->query($sql2);

        $out = '<ul>';
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $time = date('H', strtotime($row2['event_time']));
            $out = $out . '<li>' . $time . 'h <a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row2['bovine_id']) . '">' . $row2['local_number'] . "</a> {$row2['text']}</li> ";
        }
        $out = $out . '</ul>';
        return $out;
    }

    function displayUpcomingCalfMedsToAdminister() {

        $str = (new MedicineScheduled)->displayNext8HoursScheduledMedicines(null, 'calf');


        return ((new BootStrap)->bootstrapSmallBox('Upcoming Meds', $str, 53, null, 'fa-stethoscope'));

        //return (new BootStrap)->plainCard('Upcoming Calf Meds', $str);
    }

    function displayCurrentUrbanFeederErrorMessages() {

        $ajaxUrl = $GLOBALS['config']['HTTP']['JSON_API'] . '/53/currentUrbanFeederErrorMessages';
        $out[] = ((new BootStrap)->bootstrapSmallBoxAjax('Feeder Errors', $ajaxUrl, 53, null, 'fa-bug'));

        return implode($out);
    }

    function currentUrbanFeederErrorMessages() {
        $sql = "
SELECT *,(SELECT automat_nr FROM  terminal.view_bp_automaten_nummer where con10_id=system_error.con10_id limit 1) as automat_nr
 from logging.system_error 
left join logging.system_error_description
ON system_error.error_number=system_error_description.error_number
where error_solved is null order by error_occurred DESC";
        $statement = $this->pdoUrban->prepare($sql);
        $statement->execute();

        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        //process data
        $x = array();
        foreach ($results as &$row) {
            $row['error_description_eng'] = $this->errorDescription($row['error_number'], $row['error_description']);
            $row['terminal_num'] = $this->convertUrbanTerminalToRoom($row['automat_nr']);
            $x[]['home_page'] = $row['terminal_num'] . ' ' . $row['error_description_eng'];
        }

        //output
        $output['data'] = $x;
        return ($output);
    }

    private function errorDescription($errorNum, $error_description_ger) {
        switch ($errorNum) {
            case 4201:
                $out = "Feeder in cleaning mode.";
                break;
            case 2001:
                $out = "Pump over error.";
                break;
            case 2505:
                $out = "Error while pumping out remainders.";
                break;
            case 1010:
                $out = "No whole milk available.";
                break;
            case 3500:
                $out = "Check Milk Lance";
                break;
            case 3540:
                $out = "Detergent empty: blue yellow.";
                break;
            case 4021:
                $out = "Temperature at station is too low. Room Door Open?";
                break;
            case 4121:
                $out = "Station temperature is too low. Room Door Open?";
                break;
            case 4200:
                $out = "Feeder automatically in standby.";
                break;
            case 5521:
                $out = "Filling level probe: index too short.";
                break;
            case 5525:
                $out = "Transducer error.";
                break;
            case 7221:
                $out = "Station 1 is not recognized. Is It Off?";
                break;
            case 7261:
                $out = "Machine AC 1 not recognized";
                break;
            case 7281:
                $out = "Machine DC 1 not recognized";
                break;
            case 7282:
                $out = "Machine DC 2 not recognized";
                break;
            default:
                $out = 'UNKNOWN (' . $errorNum . '), check feeder itself for description. German error is: ' . $error_description_ger;
                break;
        }
        return $out;
    }

    private function displayCurrentlyInFeederHTML() {
        $HTML = <<<HTML
    
    <div class="row">
     <div class="col-md-6">
           <div class="calfStall">
             <h1>Pen D</h1>
             <ul>
             <li><img class="calfStall_cowHead"    src="/images/cowHeadFront.svg"><a id="calfStall_CalfNumber_PenD" src=""></a></li>
             <li><img class="calfStall_milkCarton" src="/images/milkCarton.svg"  ><span id="calfStall_milkAmount_PenD"></span></li>
             </ul>
           </div>
        
           <div class="calfStall">
             <h1>Pen C</h1>
             <ul>
             <li><img class="calfStall_cowHead"    src="/images/cowHeadFront.svg"><a id="calfStall_CalfNumber_PenC" src=""></a></li>
             <li><img class="calfStall_milkCarton" src="/images/milkCarton.svg"  ><span id="calfStall_milkAmount_PenC"></span></li>
             </ul>
           </div>
     </div>
     <div class="col-md-6">    
         <div class="calfStall">
             <h1>Pen B</h1>
             <ul>
             <li><img class="calfStall_cowHead"    src="/images/cowHeadFront.svg"><a id="calfStall_CalfNumber_PenB" src=""></a></li>
             <li><img class="calfStall_milkCarton" src="/images/milkCarton.svg"  ><span id="calfStall_milkAmount_PenB"></span></li>
             </ul>
         </div>
        
        <div class="calfStall">
             <h1>Pen A</h1>
             <ul>
             <li><img class="calfStall_cowHead"    src="/images/cowHeadFront.svg"><a id="calfStall_CalfNumber_PenA" src=""></a></li>
             <li><img class="calfStall_milkCarton" src="/images/milkCarton.svg"  ><span id="calfStall_milkAmount_PenA"></span></li>
             </ul>
         </div>
     </div>
    </div>    
HTML;



        return (new BootStrap)->plainCard('Live View', $HTML);
    }

    //who is currently in the feeder.
    function displayCurrentlyInFeeder() {
        $js = <<<JS
  <script>
            $(document).ready(function() {
           (function worker() {
  $.ajax({
    url: '{$GLOBALS['config']['HTTP']['JSON_API']}/53/currentlyInFeeder', 
    dataType : "html",
    success: function(data) {
         var objData = JSON.parse(data);  
         
         //erase the old data
         $( "#calfStall_CalfNumber_PenA").html('     ');
         $( "#calfStall_milkAmount_PenA").html('     ');
         $( "#calfStall_CalfNumber_PenB").html('     ');
         $( "#calfStall_milkAmount_PenB").html('     ');
         $( "#calfStall_CalfNumber_PenC").html('     ');
         $( "#calfStall_milkAmount_PenC").html('     ');
         $( "#calfStall_CalfNumber_PenD").html('     ');
         $( "#calfStall_milkAmount_PenD").html('     ');
         
         /* loop though array */
      $.each(objData.data, function( index, value ) {
          /* write local number to div a for correct pen*/
 $( "#calfStall_CalfNumber_Pen"+value.pen_name.slice(-1) ).html(value.local_number);
 $( "#calfStall_milkAmount_Pen"+value.pen_name.slice(-1) ).html( parseFloat(value.tagesverbrauch_milch).toFixed(3) );
 
});
    },
    complete: function() {
      // Schedule the next request when the current one's complete
      setTimeout(worker, 2000);
    }
  });
})();
});

        </script>
JS;
        return $js;
    }

    //used for 2 second updates
    function currentlyInFeeder() {
        $sql = "
            with temp as (
SELECT bovine.id as bovine_id, local_number,responder_nr as partial_rfid_number,stationsbesuch_id,	xx.tiere_id,	erste_erkennung,	letzte_erkennung,
 	urban_feeder_foreign_tiere.feeder_number_to_pen_name(terminal || '.'||	automat	 || '.'||station || '.'||box)  as pen_name          
FROM (SELECT * FROM urban_feeder_foreign_tiere.tabelle_stationsbesuch order by letzte_erkennung DESC limit 15) as xx 
LEFT JOIN urban_feeder_foreign_tiere.identifikation ON identifikation.tiere_id = xx.tiere_id
LEFT JOIN bovinemanagement.bovine ON responder_nr::numeric=bovinemanagement.rfid_number_to_partial(rfid_number)
where noch_in_der_station = true
)
/* joins the latest data from drinking table */
SELECT temp.*, tagesverbrauch_milch, tagesverbrauch_wasser ,tagesverbrauch_vollmilch
from temp 
LEFT JOIN LATERAL  (SELECT * from urban_feeder_foreign_tiere.tageswerte_verbrauch_milch WHERE tiere_id=temp.tiere_id order by letztes_update DESC limit 1) e2 ON true
            ";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //output
        $output['data'] = $results;
        return ($output);
    }

    //show littlele boxes for each calf colour coded.
    function calfList() {

        $out = array();

        //NOTE: this code searches for  'DEL' which means the calf was recently deleted from the system, it then ignores it.     
        $sql = "
 
with temp as (
select distinct on(responder_nr) responder_nr as partial_rfid_number, erste_erkennung as start_drinking_time,terminal || '.'||	automat	 || '.'||station || '.'||box as feeder
FROM urban_feeder_foreign_tiere.tabelle_stationsbesuch
LEFT JOIN urban_feeder_foreign_tiere.identifikation ON identifikation.tiere_id = tabelle_stationsbesuch.tiere_id WHERE substring(responder_nr from 1 for 3) != 'DEL'
), temp2 as (
select id as bovine_id,local_number,current_date - birth_date::date as age_days,temp.*,view_mh_aktuelle_tier_bewertungen.*,(SELECT id from bovinemanagement.medical_case where close_userid is null AND bovine_id=bovine.id limit 1) as medical_case_id,location_id   
FROM temp
LEFT JOIN bovinemanagement.bovine ON partial_rfid_number::text=(bovinemanagement.rfid_number_to_partial(rfid_number))::text
LEFT JOIN urban_feeder_foreign_tiere.view_mh_aktuelle_tier_bewertungen ON view_mh_aktuelle_tier_bewertungen.responder_nr = partial_rfid_number::text
LEFT JOIN bovinemanagement.location_urban_feedstall ON feedstall_id= feeder
UNION
/* add calves in calving pen to query */
SELECT bovineall.id as bovine_id,bovineall.local_number,current_date - birth_date::date as age_days,(bovinemanagement.rfid_number_to_partial(rfid_number))::text aspartial_rfid_number,
null as start_drinking_time, null as feeder,null as tiere_id,null asresponder_nr,
null as bewertung_futter_24h,2 as farbe_bewertung_futter_24h,null as bewertung_gesundheit_72h,null as farbe_bewertung_gesundheit_72h,null as letzte_berechnung_um,null as zustaendiges_terminal,
null as zustaendiger_automat,null as gefressen_letzte_24h,(SELECT id from bovinemanagement.medical_case where close_userid is null AND bovine_id=bovineall.id limit 1) as medical_case_id, null as location_id
FROM bovinemanagement.bovineall 
WHERE location_id=64

)
select distinct(bovine_id), *
,(SELECT dehorn_event_type FROM bovinemanagement.dehorn_event WHERE dehorn_event.bovine_id=temp2.bovine_id order by event_time desc  limit 1)       
,(SELECT calf_breed FROM bovinemanagement.calving_event WHERE calving_event.calf_bovine_id=temp2.bovine_id   limit 1) 
,(SELECT calf_chimeric FROM bovinemanagement.calving_event WHERE calving_event.calf_bovine_id=temp2.bovine_id   limit 1) 
,(SELECT calf_sex FROM bovinemanagement.calving_event WHERE calving_event.calf_bovine_id=temp2.bovine_id   limit 1) 
FROM temp2 
WHERE tiere_id is not null 
ORDER BY location_id ASC NULLS FIRST, age_days DESC 
   
";



        //check if any of the bovine_id's are null.   
        $errorCheck = $GLOBALS['pdo']->query($sql)->fetchAll(PDO::FETCH_COLUMN);
        if (in_array(null, $errorCheck, true)) {
            $out[] = ("<h1>Error: A calf has an RFID eartag on at the feeder that is not in the Little River Int system.</h1>");
            $out[] = ("<p>This occurs  in two situations:</p><p>1) When someone forgets to put a calf was born into the Little River Int system.<p/><p>2) Or when they put the rifd tag in wrong ear and then put a bull tag on and do not remove the original rfid. The original tag needs to be manually deleted from the urban feeder by management.</p>");

            //show list of int calves missing from urban
            $out[] = ("<h3>Missing from INT system:</h3>");
            $sqlErr = "select distinct on(responder_nr) responder_nr as partial_rfid_number
FROM urban_feeder_foreign_tiere.tabelle_stationsbesuch
LEFT JOIN urban_feeder_foreign_tiere.identifikation ON identifikation.tiere_id = tabelle_stationsbesuch.tiere_id 
LEFT JOIN bovinemanagement.bovine ON bovinemanagement.rfid_number_to_partial(rfid_number)::text= responder_nr
WHERE substring(responder_nr from 1 for 3) != 'DEL' AND rfid_number is null
";
            $resErr = $GLOBALS['pdo']->query($sqlErr);
            while ($rowErr = $resErr->fetch(PDO::FETCH_ASSOC)) {
                $out[] = ($rowErr['partial_rfid_number'] . "</br>");
            }

            throw new Exception("Error: A calf has an RFID eartag on at the feeder that is not in the Little River Int system.");
            exit();
        }



        $res = $GLOBALS['pdo']->query($sql);
        $prevFeeder = -1;

        //get what they have drank today for all calves.
        $attentionListArr = $this->emergencyAttentionListRaw();


        $loopStr = ''; //this is needed because we don't know to add some things until the end.
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $str = '';
            //first time through setting. must be a top of loop
            if ($prevFeeder == -1) {
                $prevFeeder = $row['feeder'];
                $loopStr = $loopStr . ("<div id='calfPenGroup'>");
                $loopStr = $loopStr . ("<h1>" . $this->convertFeedStall($row['feeder'], $row['tiere_id'], $row['bovine_id']) . "</h1>");
                //show consumtion for feeder stall
                $consumptionArr = $this->penConsumtion($row['feeder']);
                $loopStr = $loopStr . $this->consumptionPerPen($row['feeder']);
            }


            //use urban feeder to decide colour here to keep consistent.
            switch ($row['farbe_bewertung_futter_24h']) {
                case 0:
                    $class = 'calfBoxGreen';
                    break;
                case 1:
                    $class = 'calfBoxYellow';
                    break;
                case 2:
                    $class = 'calfBoxRed';
                    break;
            }

            //modal
            $bootstrapModal = new ModalBox($row['bovine_id']);

            // show open medical cases  
            $openMedicalCase = '';
            if (!empty($row['medical_case_id'])) {
                $openMedicalCase = '<a class="calfBoxMedical" href="' . $GLOBALS['config']['HTTP']['URL'] . '?index.php?pageid=64">&#9764</a>';
            }

            //colour box purple if they are weaned. I'm not sure this is the correct logic, but it is close.
            if (($row['bewertung_futter_24h'] == 1) AND ( $row['bewertung_gesundheit_72h'] == 6)) {
                $class = 'calfBoxPurple';
            }

            //NOTE NEEDED NOW??? I DONT THINK SO.
            //now check if any calf has drank less then 2 liters in last 24 hours on urban feeder.
            //if that is the case then draw attention to it.
            //if (($row['gefressen_letzte_24h'] < 2) AND ( $row['bewertung_futter_24h'] != 1)) {
            //     $class = $class . ' calfBoxAttention';
            // }

            $manualMilkText = '';
            //now check if calf has drank more than 2 liters manually before or after 1pm to midnight and put green border if all is good.
            if ((new DateTime('now') >= (new DateTime((new DateTime('now'))->format('Y-m-d') . ' 13:00'))) AND ( new DateTime('now') < (new DateTime((new DateTime('tomorrow'))->format('Y-m-d') . ' 0:00')))) {
                $sql1300 = "select sum(dosage) as manual_milk_amount 
                from bovinemanagement.calf_milk_administered
                LEFT JOIN bovinemanagement.calf_milk on calf_milk.id=calf_milk_id 
                where bovine_id={$row['bovine_id']} and
                event_time > (now() - interval '4 hours') limit 1";
                $row1300 = $GLOBALS['pdo']->query($sql1300)->fetch(PDO::FETCH_ASSOC);
                $manual_milk_amount = $row1300['manual_milk_amount'];
                if ($row1300['manual_milk_amount'] > 2) {
                    $class = $class . ' calfBoxAttentionSolidGreen';
                    $manualMilkText = '<h3 class="manualMilkText">Manually Fed</h3>';
                } elseif ($row1300['manual_milk_amount'] > 1) {
                    $class = $class . ' calfBoxAttentionSolidYellow';
                    $manualMilkText = '<h3 class="manualMilkText">Manually Fed</h3>';
                } elseif ($row1300['manual_milk_amount'] > .1) {
                    $class = $class . ' calfBoxAttentionSolidRed';
                    $manualMilkText = '<h3 class="manualMilkText">Manually Fed</h3>';
                } else {
                    //show default calfBoxAttention border   
                }
            } elseif ((new DateTime('now') < (new DateTime((new DateTime('now'))->format('Y-m-d') . ' 13:00'))) AND ( new DateTime('now') >= (new DateTime((new DateTime('now'))->format('Y-m-d') . ' 0:00')))) {
                $sql0000 = "select sum(dosage) as manual_milk_amount 
                from bovinemanagement.calf_milk_administered
                LEFT JOIN bovinemanagement.calf_milk on calf_milk.id=calf_milk_id 
                where bovine_id={$row['bovine_id']} and
                event_time > (now() - interval '5 hours') limit 1";
                $row0000 = $GLOBALS['pdo']->query($sql0000)->fetch(PDO::FETCH_ASSOC);
                $manual_milk_amount = $row0000['manual_milk_amount'];
                if ($row0000['manual_milk_amount'] > 2) {
                    $class = $class . ' calfBoxAttentionSolidGreen';
                    $manualMilkText = '<h3 class="manualMilkText">Manually Fed</h3>';
                } elseif ($row0000['manual_milk_amount'] > 1) {
                    $class = $class . ' calfBoxAttentionSolidYellow';
                    $manualMilkText = '<h3 class="manualMilkText">Manually Fed</h3>';
                } elseif ($row0000['manual_milk_amount'] > .1) {
                    $class = $class . ' calfBoxAttentionSolidRed';
                    $manualMilkText = '<h3 class="manualMilkText">Manually Fed</h3>';
                } else {
                    //show default calfBoxAttention border   
                }
            }


            $needsFed = '';

            /*
             * if they didn't have full amount of am or 2x for pm, blink the symbol.
             */
            foreach ($attentionListArr as $key => $value) {

                if (($value['bovine_id'] == $row['bovine_id']) AND ($value['attention'] == 1)) {
                    $needsFed = ' <i class="calfFeederBoxIcon fas fa-utensils fa-2x fa-blink"></i>';
                }
            }



            //dehorning symbol
            if (!empty($row['dehorn_event_type'])) {
                $dehorned = '<img class=""    src="/images/dehorn.svg">';
            } else {
                $dehorned = '';
            }

            //angus sybmol
            if ((!empty($row['calf_breed'])) AND ($row['calf_breed'] == 'AN')) {
                $breed = '<i class="calfFeederBoxIcon fab  fa-github-alt fa-2x"></i>';
            } elseif ((!empty($row['calf_breed'])) AND ($row['calf_breed'] == 'HO')) {
                //show nothing for holstein
                $breed = '';
            } else {
                $breed = '????'; //we don't know the breed.
            }

            //gender symbol
            if ((!empty($row['calf_sex'])) AND ($row['calf_sex'] == 'female')) {
                $gender = ' <i class="calfFeederBoxIcon fas fa-venus fa-2x"></i>';
            } elseif ((!empty($row['calf_sex'])) AND ($row['calf_sex'] == 'male')) {
                $gender = ' <i class="calfFeederBoxIcon fas fa-mars fa-2x"></i>';
            } else {
                $gender = ' <i class="calfFeederBoxIcon fas fa-genderless fa-2x"></i>';
            }
            //overrides above.
            if ($row['calf_chimeric'] == true) {
                $gender = ' <i class="calfFeederBoxIcon fas fa-venus-mars fa-2x"></i>';
            }



            //create modal
            $inputs = array('tiere_id' => $row['tiere_id'], 'bovine_id' => $row['bovine_id'], 'local_number' => $row['local_number']);
            $json = urlencode(json_encode($inputs, JSON_UNESCAPED_UNICODE));
            $url = $GLOBALS['config']['HTTP']['HTML_API'] . '53/modalAjaxContent/' . $json;
            $buttonX = null;
            $buttonX = (new BootStrap)->bootstrapModalAjax('#' . $row['local_number'], $url, 'calfFeederBoxIcon fas fa-chart-bar');



            //content for calfbox
            $outS = null;

            $outS[] = '<ul>';
            $outS[] = '<li>' . $buttonX . ' ' . $openMedicalCase . ' ' . $dehorned . $breed . $gender . $needsFed . '</li>';
            $outS[] = ('<li><i class="fas fa-birthday-cake"></i> ' . $row['age_days'] . 'ᵈ</li>');
            $outS[] = ('<li>' . '<i class="fas fa-robot"></i>  24h: ' . round($row['gefressen_letzte_24h'], 1) . ' / ' . round($row['gefressen_letzte_24h'] + $row['bewertung_futter_24h']) . '&#8467</li>');
            $outS[] = ($manual_milk_amount > 0) ? ('<li>' . '<i class="far fa-hand-paper"></i>  4h: ' . round($manual_milk_amount, 1) . '&#8467</li>') : '';

            $outS[] = '</ul>';
            $outS[] = $manualMilkText;

            $str = $str . (new BootStrap)->animalBox($class, $row['bovine_id'], $row['local_number'], implode($outS));

            //$str = $str . ;
            //calf feeding modal 
            //  $cls = new calfFeedingModal($this->pdoUrban, $row['tiere_id'], $row['bovine_id'], $row['local_number']);
            // $str = $str . ($bootstrapModal->popup($cls->toStringTitle(), $cls->toStringContent()));
            //now do box around to make pen groups
            //every loop checks here to see if end of a pen
            $end = '';
            //    $out[]=( $this->checkPenOverDays($row['location_id']));
            if ($row['feeder'] != $prevFeeder) {
                $end = $end . ("</div>");
                $end = $end . ("<div id='calfPenGroup'>");
                $end = $end . ("<h1>" . $this->convertFeedStall($row['feeder'], $row['tiere_id'], $row['bovine_id']) . "</h1>");
                //show consumtion for feeder stall
                $end = $end . $this->consumptionPerPen($row['feeder']);
            }
            //set for next loop
            $prevFeeder = $row['feeder'];

            //add to loopStr 
            $loopStr = $loopStr . $end . $str;
        }//end while
        $loopStr = $loopStr . ("</div>");
        return ($loopStr);
    }

    /*
     * call this via URL to show load content for modal calf box. .
     */

    public function modalAjaxContent($request) {

        //filter and decode url
        if (isset($request['item'])) {
            $jsonObj = urldecode($request['item']);
        }

        //filter JSON
        $inputs = filter_var_array(json_decode($jsonObj, true), [
            'tiere_id' => ['filter' => FILTER_VALIDATE_INT,
                'flags' => FILTER_NULL_ON_FAILURE],
            'bovine_id' => ['filter' => FILTER_VALIDATE_INT,
                'flags' => FILTER_NULL_ON_FAILURE],
            'local_number' => ['filter' => FILTER_VALIDATE_INT,
                'flags' => FILTER_NULL_ON_FAILURE]
                ]);



        $cls = new calfFeedingModal($this->pdoUrban, $inputs['tiere_id'], $inputs['bovine_id'], $inputs['local_number']);

        return $cls->toStringContent();
    }

    private function consumptionPerPen($feeder) {
        $loopStr = null;
        $consumptionArr = $this->penConsumtion($feeder);
        if (!empty($consumptionArr)) {
            foreach ($consumptionArr as $key => $value) {
                $value = round($value, 1);
                $loopStr = $loopStr . " Since $key:00 $value litres consumed.</p>";
            }
        }

        return $loopStr;
    }

    //checks if pen has to great of an age range
    private function checkPenOverDays($location_id) {
        //var_dump($feeder);
        //find number in pen
        //$sql="SELECT count(bovine_id),location_id from bovinemanagement.locationcurr group by location_id ";
        //find age spread in pen
        $sql = <<<SQL
         with temp as (
SELECT id,birth_date from bovinemanagement.bovinecurrall WHERE location_id =$location_id
)
SELECT max(birth_date)- min(birth_date) as answer FROM temp
       
SQL;

        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $out = '';
        if (($row['answer'] >= 14) AND ($row['answer'] < 18)) {
            $out = (new BootStrap)->warningNotify("Warning this pen is almost full due to maximum calf age range.");
        } elseif (($row['answer'] >= 18) AND ($row['answer'] < 18)) {
            $out = (new BootStrap)->errorNotify("Error: Stop filling this pend, maximum age range exceeded.");
        }

        return $out;
    }

    //used for calibration of scales
    //shows last bit of raw data one web page
    function calibrationRaw() {
        print("<h3>DEBUG START</h3>");
        $sql = "SELECT * from bas.calf_barn_scale_event_raw order by event_time DESC limit 10";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("{$row['event_time']} {$row['scale_id']} {$row['mass']} <br>");
        }
        print("<h3>DEBUG STOP</h3>");
    }

    public function displayWashLog() {

        $ajaxUrl = $GLOBALS['config']['HTTP']['JSON_API'] . '/53/washLog';
        $today = date('M d', strtotime('now')); //if we find todays date, then colour green
        $out[] = ((new BootStrap)->bootstrapSmallBoxAjax('Wash', $ajaxUrl, 53, $today, 'ion-ios-pulse'));

        return implode($out);
    }

    public function displayTempLog() {

        $ajaxUrl = $GLOBALS['config']['HTTP']['JSON_API'] . '/53/feederTemperatureLog';
        $out[] = ((new BootStrap)->bootstrapSmallBoxAjax('Temp Log', $ajaxUrl, 53, null, 'fas fa-temperature-low'));

        return implode($out);
    }

    //get last time machine was washed from urban feeder, only shows 1.1 feeder and last 21 days.
    public function washLog() {

        $sql = <<<SQL
        SELECT automat_nr,zeit_start as start_time, zeit_ende as end_time,spuelmittel_id as detergent_id, spuelen_abgebrochen as rinse_aborted
FROM automat.spuelen_verwaltung
LEFT JOIN terminal.view_bp_automaten_nummer ON view_bp_automaten_nummer.con10_id=spuelen_verwaltung.con10_id 
WHERE spuelen_abgebrochen = false
ORDER BY zeit_start DESC limit 4
SQL;
        $statement = $this->pdoUrban->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            if ($row['detergent_id'] == 0) {
                $detergent = 'acid';
            } elseif ($row['detergent_id'] == 1) {
                $detergent = 'detergent';
            } else {
                $detergent = ('Unknown Detergent ID!</br>');
            }

            if (empty(($row['end_time']))) {
                $time = 'Cleaning NOW';
            } else {
                $time = date('D M j G:i', strtotime($row['end_time']));
            }




            $x[]['home_page'] = $time . ' ' . $this->convertUrbanTerminalToRoom($row['automat_nr']) . '  ' . $detergent . '';
        }

        //output
        $output['data'] = $x;
        return ($output);
    }

    //used to see how much milk we used in total and per feedstall
    public function consumptionSinceXhour($hour) {
        $sql = "With temp as (
SELECT terminal || '.' || automat || '.' || station || '.' ||  box as feedstall,ta.tiere_id, to_char(ta.erste_erkennung, 'YYDD.MM HH24:MI'::text) AS wann, 
        CASE
            WHEN tk.sollmenge_milch IS NULL THEN 0::double precision
            ELSE tk.sollmenge_milch
        END AS sollmenge_milch, 
        CASE
            WHEN tk.verbrauch_milch IS NULL THEN 0::double precision
            ELSE tk.verbrauch_milch
        END AS verbrauch_milch, 
        CASE
            WHEN tj.bewertung IS NULL THEN 100
            ELSE tj.bewertung
        END AS bewertung, 
        CASE
            WHEN tj.bewertung IS NULL OR tj.bewertung = 100 THEN 100
            ELSE 
            CASE
                WHEN tj.bewertung >= 60 THEN 0
                ELSE 
                CASE
                    WHEN tj.bewertung >= 40 THEN 1
                    ELSE 2
                END
            END
        END AS bewertung_farbe, ta.erste_erkennung, (date_part('day'::text, 'now'::text::timestamp without time zone - ta.erste_erkennung) * 24::double precision + date_part('hour'::text, 'now'::text::timestamp without time zone - ta.erste_erkennung)) || to_char('now'::text::timestamp without time zone - ta.erste_erkennung, ':MI:SS'::text) AS zeit_vor, ( SELECT array_agg(tabelle_verbrauch_zusatz.verbrauch_zusatz) AS array_agg
           FROM tiere.tabelle_verbrauch_zusatz
          WHERE tabelle_verbrauch_zusatz.stationsbesuch_id = ta.stationsbesuch_id) AS verbrauch_zusatz, ( SELECT array_agg(tabelle_verbrauch_zusatz.sollmenge_zusatz) AS array_agg
           FROM tiere.tabelle_verbrauch_zusatz
          WHERE tabelle_verbrauch_zusatz.stationsbesuch_id = ta.stationsbesuch_id) AS sollmenge_zusatz, ( SELECT array_agg(tabelle_verbrauch_zusatz.zusatz_id) AS array_agg
           FROM tiere.tabelle_verbrauch_zusatz
          WHERE tabelle_verbrauch_zusatz.stationsbesuch_id = ta.stationsbesuch_id) AS zusatz_id
   FROM tiere.tabelle_stationsbesuch ta
   LEFT JOIN tiere.tabelle_verbrauch_bewertung tj ON ta.stationsbesuch_id = tj.stationsbesuch_id
   LEFT JOIN tiere.tabelle_verbrauch_milch tk ON ta.stationsbesuch_id = tk.stationsbesuch_id
  WHERE 
  /* current day from $hour until midnight*/
(ta.erste_erkennung > (current_date) AND 
((extract('hour' from ta.erste_erkennung) >= $hour) ) AND 
(extract('hour' from now()) >$hour))
OR
/* previous day from midnight to $hour */
(ta.erste_erkennung > (current_date-interval '1 day') AND 
((extract('hour' from ta.erste_erkennung) >= $hour) ) AND 
(extract('hour' from now()) <=$hour))

AND
 (tj.bewertung >= 20 OR tj.bewertung IS NULL) order by wann
)
/* gets the total for each stall as well as overall total */
SELECT distinct feedstall, sum(verbrauch_milch) OVER (PARTITION BY feedstall)  as feedstall_total ,(SELECT sum(verbrauch_milch) from temp) as total,$hour as hour  from temp
";
        $res = $this->pdoUrban->query($sql);
        $results = $res->fetchAll(PDO::FETCH_ASSOC);
        return $results;
    }

    /*
     * search array for consumption by feed stall and returns as array by time in hour.
     * 
     */

    private function penConsumtion($feederStall) {
        $outputArray = array();
        if (!empty($feederStall)) {
            $arrIt = new RecursiveIteratorIterator(new RecursiveArrayIterator($this->cosumptionArr));
            foreach ($arrIt as $sub) {
                $subArray = $arrIt->getSubIterator();
                if ($subArray['feedstall'] === $feederStall) {
                    $outputArray[$subArray['hour']] = $subArray['feedstall_total'];
                }
            }
            return $outputArray;
        } else {
            return array();
        }
    }

    //shows a log of all animals and what they drank.
    public function listAllLog() {

        //code taken from Urban Feeder.
        $sql = "SELECT ta.tiere_id, to_char(ta.erste_erkennung, 'YYDD.MM HH24:MI'::text) AS wann, 
        CASE
            WHEN tk.sollmenge_milch IS NULL THEN 0::double precision
            ELSE tk.sollmenge_milch
        END AS sollmenge_milch, 
        CASE
            WHEN tk.verbrauch_milch IS NULL THEN 0::double precision
            ELSE tk.verbrauch_milch
        END AS verbrauch_milch, 
        CASE
            WHEN tj.bewertung IS NULL THEN 100
            ELSE tj.bewertung
        END AS bewertung, 
        CASE
            WHEN tj.bewertung IS NULL OR tj.bewertung = 100 THEN 100
            ELSE 
            CASE
                WHEN tj.bewertung >= 60 THEN 0
                ELSE 
                CASE
                    WHEN tj.bewertung >= 40 THEN 1
                    ELSE 2
                END
            END
        END AS bewertung_farbe, ta.erste_erkennung, (date_part('day'::text, 'now'::text::timestamp without time zone - ta.erste_erkennung) * 24::double precision + date_part('hour'::text, 'now'::text::timestamp without time zone - ta.erste_erkennung)) || to_char('now'::text::timestamp without time zone - ta.erste_erkennung, ':MI:SS'::text) AS zeit_vor, ( SELECT array_agg(tabelle_verbrauch_zusatz.verbrauch_zusatz) AS array_agg
           FROM tiere.tabelle_verbrauch_zusatz
          WHERE tabelle_verbrauch_zusatz.stationsbesuch_id = ta.stationsbesuch_id) AS verbrauch_zusatz, ( SELECT array_agg(tabelle_verbrauch_zusatz.sollmenge_zusatz) AS array_agg
           FROM tiere.tabelle_verbrauch_zusatz
          WHERE tabelle_verbrauch_zusatz.stationsbesuch_id = ta.stationsbesuch_id) AS sollmenge_zusatz, ( SELECT array_agg(tabelle_verbrauch_zusatz.zusatz_id) AS array_agg
           FROM tiere.tabelle_verbrauch_zusatz
          WHERE tabelle_verbrauch_zusatz.stationsbesuch_id = ta.stationsbesuch_id) AS zusatz_id
   FROM tiere.tabelle_stationsbesuch ta
   LEFT JOIN tiere.tabelle_verbrauch_bewertung tj ON ta.stationsbesuch_id = tj.stationsbesuch_id
   LEFT JOIN tiere.tabelle_verbrauch_milch tk ON ta.stationsbesuch_id = tk.stationsbesuch_id
  WHERE ta.erste_erkennung > ('now'::text::timestamp without time zone - '11172:00:00'::interval) AND (tj.bewertung >= 20 OR tj.bewertung IS NULL) order by wann";

        $statement = $this->pdoUrban->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            //  $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
            //  $row['event_time_raw']=strtotime($row['event_time']);
            //  $row['note'] = $this->reasonsNotToBreed($row['bovine_id'],$row['event_time'],$row['birth_date']);
            //  $row['action'] = $this->printActionButtons2($row['estrus_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            // $row['event_time']= date("l M d H:00",strtotime($row['event_time']));  //has to be at end.
            //  if  ((($row['estrus_type_id'])==1) AND empty($row['note']))  {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    //changes urban id to our barn design.
    //use bovinmanagement location_urban_feedstall in the future.
    private function convertFeedStall($input, $tiere_id, $bovine_id) {

        /*
         * hackish way to check if in calving pen.
         */
        if (empty($tiere_id)) {
            //feeder doesn't have calf registered.
            $sql = "SELECT local_number FROM bovinemanagement.bovineall WHERE id=$bovine_id and location_id=64";
            $row = $GLOBALS['pdo']->query("$sql")->fetch();
            if (!empty($row)) {
                return 'Calving Pen';
            }
        }


        switch ($input) {
            case '1.1.1.2':
                return "Pen A";
                break;
            case '1.1.1.1':
                return "Pen B";
                break;
            case '1.2.1.1':
                return "Pen D";
                break;
            case '1.2.1.2':
                return "Pen C";
                break;
            default:
                return "Error, Unknown feedstall number: $input";
        }
    }

    /*
     * changes 1.2 or 1.1 to correct room.
     */

    private function convertUrbanTerminalToRoom($input) {


        switch ($input) {
            case '1.1':
                return "Room AB";
                break;
            case '1.2':
                return "Room CD";
                break;

            default:
                return "Error, Unknown urban terminal number: $input";
        }
    }

    public function convertPenToFeedStall($input) {
        //see bovinmanagement location_urban_feedstall instead.
    }

    //gets tiere number from local number. 
    //this doesn't do historical data. only current.
    public function convertLocalNumberToUrbanTeireID($input) {

        $sql = "SELECT tiere_id FROM tiere.identifikation WHERE RIGHT(responder_nr,4) = '$input'::text limit 1";
        $res = $this->pdoUrban->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return (!empty($row['tiere_id'])) ? $row['tiere_id'] : null;
    }

    public function urban() {

        print("Hello Urban<br>");


        $sql = "SELECT * FROM tiere.view_mh_aktuelle_tier_bewertungen";
        $res = $this->pdoUrban->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            print_r($row);
        }
    }

    public function summaryDisplay() {
        //$this->calvingEventQuickForm();
    }

    function heifersSOP() {
        print("<h3>SOP</h3>");
        print("Start Feeding 2.8 liters a day");
    }

    function printFormattedNumber($params) {
        extract($params);
        $number = "<a href=\"../index.php?pageid=6&bovine_id={$record['id']}\"> {$record['local_number']}</a>";
        return $number;
    }

    function printPartialName($params) {
        extract($params);
        $animalShortName = $GLOBALS['MiscObj']->femaleShortName($row['full_name']);
        return $animalShortName;
    }

    function printSex($params) {
        extract($params);
        $regArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($record['full_reg_number']);
        return $regArray['sex'];
    }

    function printFeedColostrum($params) {
        extract($params);
        if ($record['age_days'] <= 1) {
            return '2 x 4 &#x2113;';
        } else {
            return '';
        }
    }

    function printFeedMilk($params) {
        extract($params);
        if ($record['age_days'] > 1 && $record['age_days'] <= 21) {
            return '2 x 2.8 &#x2113;';
        } else {
            return '';
        }
    }

    function printFeedMilkReplacer($params) {
        extract($params);
        if ($record['age_days'] > 21 && $record['age_days'] <= 30) {
            return '2 x 3 &#x2113;';
        } else {
            return '';
        }
    }

    function printFeedCalfStarter($params) {
        extract($params);
        if ($record['age_days'] > 21 && $record['age_days'] <= 200) {
            return '1 kg';
        } else {
            return '';
        }
    }

    function printFeedWater($params) {
        extract($params);
        if ($record['age_days'] > 30 && $record['age_days'] <= 200) {
            return '2 &#x2113;';
        } else {
            return '';
        }
    }

    function printFeedHay($params) {
        extract($params);
        if ($record['age_days'] > 49 && $record['age_days'] <= 200) {
            return '1 flake';
        } else {
            return '';
        }
    }

    /*
     * animals that have has less than 6 liters in a day or 3 liters in a morning. 
     * 
     */

    public function emergencyAttentionListRaw() {
        /* this query is slow, because of the RFID join I think */
        $sql = <<<SQL
         with temp as (
SELECT tag as date ,local_number,tageswerte_verbrauch_milch.tiere_id, (tag-(CURRENT_DATE -6)) as day_of_life,tagesverbrauch_milch as drank_amount, tagesmenge_milch as eligible_amount,bovine.id as bovine_id,
(SELECT sum(dosage) FROM bovinemanagement.calf_milk_administered LEFT JOIN bovinemanagement.calf_milk ON calf_milk.id=calf_milk_administered.calf_milk_id WHERE bovine.id=bovine_id AND tag=date_trunc('day', calf_milk_administered.event_time) ) as manual_amount
FROM urban_feeder_foreign_tiere.tageswerte_verbrauch_milch 
LEFT JOIN urban_feeder_foreign_tiere.identifikation ON identifikation.tiere_id = tageswerte_verbrauch_milch.tiere_id 
LEFT JOIN bovinemanagement.bovine ON bovinemanagement.rfid_number_to_partial(rfid_number)::text= responder_nr
WHERE tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY tag DESC
)
SELECT date,local_number,bovine_id, COALESCE(drank_amount,0) as feeder_amount, COALESCE(manual_amount,0) as manual_amount, COALESCE(drank_amount,0)+ COALESCE(manual_amount,0) as total_amount FROM temp 
WHERE eligible_amount >= 6 AND date=current_date
ORDER BY date, local_number 
SQL;
        $res = $GLOBALS['pdo']->query($sql);
        //get data
        $results = $res->fetchAll(PDO::FETCH_ASSOC);

        foreach ($results as &$row) {
            //morning


            $row['attention'] = 0; //default
            if (strtotime('now') < strtotime('12 pm')) {

                if ($row['total_amount'] < 2.5) {
                    $row['attention'] = 1;
                    $row['home_page'] = '#' . ($row['local_number'] . ' has had ' . round($row['total_amount'], 1) . 'ℓ today, needs at least 2.5ℓ.');
                    $row['numerator'] = round($row['total_amount'], 1);
                    $row['denominator'] = 2.5;
                }
            } else { //afternoon
                if ($row['total_amount'] < 5.0) {
                    $row['attention'] = 1;
                    $row['home_page'] = '#' . ($row['local_number'] . ' has had ' . round($row['total_amount'], 1) . 'ℓ today, needs at least 5ℓ.');
                    $row['numerator'] = round($row['total_amount'], 1);
                    $row['denominator'] = 5;
                }
            }
        }





        return $results;
    }

    public function emergencyAttentionList() {

        $results = $this->emergencyAttentionListRaw();

        //process data
        $x = array();
        foreach ($results as &$row) {

            if (!empty($row['home_page'])) {
                $x[]['home_page'] = $row['home_page'];
            }
        }
        //output
        $output['data'] = $x;
        return ($output);
    }

    function displayEmergencyAttentionList() {

        $ajaxUrl = $GLOBALS['config']['HTTP']['JSON_API'] . '/53/emergencyAttentionList';
        $out[] = ((new BootStrap)->bootstrapSmallBoxAjax('Attention', $ajaxUrl, 53, null, 'ion-ios-pulse'));

        return implode($out);
    }

    /*
     * 1 minute intervals of feeder sink and boiler temperature. 
     */

    public function feederTemperatureLog() {

        $sql = <<<SQL
                SELECT  automat_nr, fuellstand_ml as mixing_sink_fill_level_ml, temperatur_boiler as temp_boiler, temperatur_anruehrbehaelter as temp_mixing_sink, 
                    zeit as event_time, temperatur_anruehrbehaelter_soll as ideal_mixing_sink_temp, temperatur_boiler_soll  as ideal_boiler_temp , temperatur_station_array                
FROM automat.tabelle_automat_temperaturen 
LEFT JOIN terminal.view_bp_automaten_nummer ON view_bp_automaten_nummer.con10_id=tabelle_automat_temperaturen.con10_id
ORDER BY zeit DESC limit 10
SQL;

        $statement = $this->pdoUrban->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $x = array();
//change presentation
        foreach ($results as &$row) {

            $time = date('D M j G:i', strtotime($row['event_time']));

            if ($row['mixing_sink_fill_level_ml'] <= 0) {
                $x[]['home_page'] = $time . ' ' . $this->convertUrbanTerminalToRoom($row['automat_nr']) . '  ' . 'Sink has negative fill level of' . $row['mixing_sink_fill_level_ml'] . '';
            }

            if (($row['temp_mixing_sink']) <= 38) {
                $x[]['home_page'] = $time . ' ' . $this->convertUrbanTerminalToRoom($row['automat_nr']) . '  ' . 'Milk Temperature is only : ' . $row['temp_mixing_sink'] . '°C';
            }
        }
        //output
        $output['data'] = $x;
        return ($output);
    }

    //used just to dhow what some of teh sql queries urban runs do 
    function dummy() {

        /*
          // this seems to show error codes or somehting.
          SELECT CASE WHEN rote_fehler IS NULL THEN 0 ELSE rote_fehler END AS rote_fehler, CASE WHEN orange_fehler IS NULL THEN 0 ELSE orange_fehler END AS orange_fehler, CASE WHEN gruene_fehler IS NULL THEN 0 ELSE gruene_fehler END AS gruene_fehler, automat_standby_spuelen
          , con10_id FROM view_bp_fehler_wartung_automaten WHERE con10_id =4

          //THis seems to how fill the mixing bowl is, updated all the time.
          SELECT "con10_id","fuellstand_ts_soll_ml","wasser_ml","vollmilch_ml","mat1_g","mat2_g","zusatz_g","mat1_prozent","mat2_prozent","zeit","aktueller_fuellstand_ml","pumpeninhalt" FROM "automat"."tabelle_automat_anruehrbehaelter"

          //if this has a true for noch in der station, then they are still in the station, seems to update frequently.
          tiere.Tabletabelle_stationsbesuch

          //shows summary for visit maybe
          filter.Tableverbrauch_milch_letzter_fuer_letzter_besuch


         */

        $sql = "


/* which feed curve */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT kurvennr FROM tiere.identifikation WHERE tiere_id =25
/* daily alert colour  */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tagesmenge_note_farbe, (tag-(CURRENT_DATE -6)) as eintrag  FROM tiere.tageswerte_verbrauch_milch WHERE tiere_id = 25 AND tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY eintrag ASC
/* no idea  something right*/
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tiere_id, anrecht-verbrauch_milch AS anrecht FROM terminal.view_mh_futter_anrecht WHERE tiere_id = 25
/* daily amount allowed */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tagesmenge_milch, (tag - (CURRENT_DATE - 6)) AS index FROM tiere.tageswerte_verbrauch_milch WHERE tiere_id = 25 AND tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY index ASC
/*last 6 days of WHOLE milk consumtion */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tagesverbrauch_vollmilch, (tag-(CURRENT_DATE -6)) as eintrag FROM tiere.tageswerte_verbrauch_milch WHERE tiere_id = 25 AND tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY eintrag ASC
/* daily consumption of material 2 */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tagesverbrauch_mat2, (tag-(CURRENT_DATE -6)) as eintrag FROM tiere.tageswerte_verbrauch_milch WHERE tiere_id = 25 AND tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY eintrag ASC
/* daily consumption of material 1 */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tagesverbrauch_mat1, (tag-(CURRENT_DATE -6)) as eintrag FROM tiere.tageswerte_verbrauch_milch WHERE tiere_id = 25 AND tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY eintrag ASC
/* daily consumption of milk (maybe total) */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT tagesverbrauch_milch, (tag-(CURRENT_DATE -6)) as eintrag FROM tiere.tageswerte_verbrauch_milch WHERE tiere_id = 25 AND tag <= CURRENT_DATE AND (CURRENT_DATE -6) <= tag ORDER BY eintrag ASC
/*something with colour */
<2018-01-29 15:42:29 UTC # terminal> LOG:  execute <unnamed>: SELECT (ROUND(note*10)::double precision)/10, farbe FROM tiere.tabelle_bewertungsnoten_farbe_anzeige WHERE tiere_id = 25 ORDER BY index ASC           

";
    }

}

//end class

Class QF2_CalfFeedingMilk extends QF2_Base {

    function definition($form) {

        // grab a list calf milk types
        $calfMilkArr[null] = "Please Select one..."; // dummy value
        $res = $GLOBALS['pdo']->query("SELECT id,name from bovinemanagement.calf_milk ORDER BY id");
        while (($row = $res->fetch())) {
            $calfMilkArr[$row[0]] = $row[1];
        }

        // grab a list of calves in calf barn location and calving pen
        $calfNumberArr = $GLOBALS['MiscObj']->createListOfAllYoungCalves();



        $calfMilkAMountArr = array('0.25' => 0.25, '0.50' => 0.50, '0.75' => 0.75, '1.00' => 1.00, '1.25' => 1.25, '1.50' => 1.50, '1.75' => 1.75, '2.00' => 2.00, '2.25' => 2.25, '2.50' => 2.50, '2.75' => 2.75, '3.00' => 3.00, '3.25' => 3.25, '3.50' => 3.50, '3.75' => 3.75, '4.00' => 4.00);

        $calfMilkMethodArr = array('bottle' => 'Bottle Fed', 'esophageal_tube' => 'Esophageal Tube Fed');


        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.
        //defaults at run time for calf 1.
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
                    'calfMilkType' => 1,
                    'calfMilkAmount' => '3.00'
        )));



        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        if ($this->mode == 'group') {
            $form = (new Quickform2_GroupSelect())->definitionAnimalGroup($form, $calfNumberArr);
        } else {
            $form = (new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalGroup($form, $calfNumberArr);
        }



        $form->addElement('select', 'calfMilkType', null, array('options' => $calfMilkArr, 'label' => 'Type of Milk / Liquid'))->addRule("required", "Please specify your type of milk / liquid.");
        $form->addElement('select', 'calfMilkAmount', null, array('options' => $calfMilkAMountArr, 'label' => 'Amount of Milk / Liquid'))->addRule("required", "Please specify the amount of milk / liquid administered.");
        $form->addElement('select', 'calfMilkMethod', null, array('options' => $calfMilkMethodArr, 'label' => 'Feeding Method'))->addRule("required", "Please specify the way you fed the calf.");
        $form->addElement('select', 'dateCalfMilk', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your time of milk / liquid administered.");

        return $form; //required       
    }

    function process($value) {
        $value['event_time'] = date('Y-m-d H:i:sO', $value['dateCalfMilk']);
        $value['calf_milk_id'] = $value['calfMilkType'];


        $bovineIdArr = (new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);

        foreach ($bovineIdArr as $key => $bovineId) {

            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.calf_milk_administered (bovine_id,event_time,calf_milk_id,dosage,method,userid) VALUES ({$bovineId},'{$value['event_time']}',{$value['calf_milk_id']},{$value['calfMilkAmount']},'{$value['calfMilkMethod']}','{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);
        }
    }

}

//end class


/* holds calf feeding Modal content */

class CalfFeedingModal {

    private $tiere_id;
    private $bovine_id;
    private $local_number;
    private $pdoUrban;

    function __construct($pdoUrban, $tiere_id, $bovine_id, $local_number) {
        $this->tiere_id = $tiere_id;
        $this->bovine_id = $bovine_id;
        $this->local_number = $local_number;
        $this->pdoUrban = $pdoUrban;
    }

    /*
     * 
     * manual milk and feeder milk per day for last few weeks. 
     */

    function totalConsumptionThroughTimePerCalf($tiere_id) {
        $retArr_total = array();
        $retArr_manual = array();
        $retArr_feeder = array();

        $sql = <<<SQL
        with temp as (
SELECT tag as date ,local_number,tageswerte_verbrauch_milch.tiere_id, (tag-(CURRENT_DATE -6)) as day_of_life,tagesverbrauch_milch as drank_amount, tagesmenge_milch as eligible_amount,bovine.id as bovine_id,
(SELECT sum(dosage) FROM bovinemanagement.calf_milk_administered LEFT JOIN bovinemanagement.calf_milk ON calf_milk.id=calf_milk_administered.calf_milk_id WHERE bovine.id=bovine_id AND tag=date_trunc('day', calf_milk_administered.event_time) ) as manual_amount
FROM urban_feeder_foreign_tiere.tageswerte_verbrauch_milch 
LEFT JOIN urban_feeder_foreign_tiere.identifikation ON identifikation.tiere_id = tageswerte_verbrauch_milch.tiere_id 
LEFT JOIN bovinemanagement.bovine ON bovinemanagement.rfid_number_to_partial(rfid_number)::text= responder_nr
WHERE tageswerte_verbrauch_milch.tiere_id=$tiere_id AND tag <= CURRENT_DATE AND (CURRENT_DATE -21) <= tag ORDER BY tag DESC
)
SELECT date,local_number,bovine_id, eligible_amount,COALESCE(drank_amount,0) as feeder_amount, COALESCE(manual_amount,0) as manual_amount, COALESCE(drank_amount,0)+ COALESCE(manual_amount,0) as total_amount FROM temp 
SQL;

        $res = $GLOBALS['pdo']->query($sql);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
//var_dump($row);
            $retArr_total[strtotime($row['date'])] = round($row['total_amount'], 2); //not used
            $retArr_manual[strtotime($row['date'])] = round($row['manual_amount'], 2);
            $retArr_feeder[strtotime($row['date'])] = round($row['feeder_amount'], 2);
        }

        $output = new stdClass();
        $output->total = $retArr_total;
        $output->manual = $retArr_manual;
        $output->feeder = $retArr_feeder;
        return $output;
    }

    function displayChartForTotalConsumptionThroughTimePerCalf($tiere_id) {


        $retCls = $this->totalConsumptionThroughTimePerCalf($tiere_id);


        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date';
        $headerStrArr['Feeder'] = 'number';
        $headerStrArr['Manual'] = 'number';


        $opt = "                  
                         isStacked: true,
                         width:1000, height:500,               
          vAxis: {title: 'Milk (l)',  titleTextStyle: {color: 'red'}}
          ";

        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $retCls->feeder, $retCls->manual);
        $out[] = $x->toString();
        return implode($out);
    }

    /*
     * retuns the sum of milk drank at the feeder for the last week. 
     */

    private function dailySumFeeder($tiere_id, $stringOutput = null) {
        $tableNewerArr = array();
        $tableOlderArr = array();

        //look up milk consumption history for each calf and show in popup
        //

        $sql2 = "SELECT (tag-(CURRENT_DATE -96)) as day_of_life,* FROM urban_feeder_foreign_tiere.tageswerte_verbrauch_milch WHERE tiere_id={$this->tiere_id} AND tag <= CURRENT_DATE AND (CURRENT_DATE -96) <= tag ORDER BY tag DESC";
        $res2 = $GLOBALS['pdo']->query($sql2);
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $feederMilk[strtotime($row2['tag'])] = array('event_time' => strtotime($row2['tag']), 'dateTime' => date('D M j', strtotime($row2['tag'])), 'bovine_id' => $this->bovine_id, 'drank_amount' => round($row2['tagesverbrauch_milch'], 2), 'eligible_amount' => round($row2['tagesmenge_milch'], 2));
        }


        //return a string or array. 
        if (($stringOutput) == true) {

            $counter = 0;
            foreach ($feederMilk as $key => $value) {
                if ($counter <= 7) {  //Oonly returns last 7 days of full array for display
                    $tableNewerArr[] = array('Date' => $value['dateTime'], 'Drank (ℓ)' => $value['drank_amount'], 'Eligible (ℓ)' => $value['eligible_amount']);
                } else { //older than 7d ays
                    //table form              
                    $tableOlderArr[] = array('Date' => $value['dateTime'], 'Drank (ℓ)' => $value['drank_amount'], 'Eligible (ℓ)' => $value['eligible_amount']);
                }
                $counter++;
            }

            $tableNewerStr = (new BootStrap)->simpleTable($tableNewerArr);
            $tableOlderStr = (new BootStrap)->simpleTable($tableOlderArr);

            //put older data into collapsible list. 
            $older = (new BootStrap)->plainCardCollapsed('Older', $tableOlderStr);

            $ret = $tableNewerStr . $older;
        } else {
            $ret = $feederMilk;
        }
        return $ret;
    }

    /*
     * every feeder visit in the last 72 hours, whether they drank or not
     */

    private function everyFeederVisit72h($tiere_id, $stringOutput = null) {

        $sql3 = "SELECT wann,(TO_TIMESTAMP(date_part('year', CURRENT_DATE)::text||'.'||wann,'YYYY.DD.MM HH24:MI'))::timestamp without time zone as event_time, sollmenge_milch, verbrauch_milch, bewertung, bewertung_farbe, zeit_vor, zusatz_id, verbrauch_zusatz, sollmenge_zusatz FROM terminal.view_mh_futterwerte_letzte_72h WHERE tiere_id={$tiere_id} ORDER BY erste_erkennung DESC";

        $statement = $this->pdoUrban->prepare($sql3);
        $statement->execute();
        $feederVisit = array();

        //urban feeder to set to UTC, but not really set to ADT. 
        //when 1 daylight savinsg is true
        $dayLight = (date('I') == 1) ? true : false;


        while ($row3 = $statement->fetch(PDO::FETCH_ASSOC)) {


            if ($dayLight == true) {  //change time by an hour if we are daylight savings
                $datetime = new DateTime($row3['event_time']);
                $datetime->modify('+1 hour');
                $row3['event_time'] = $datetime->format('r');
            }


            $feederVisit[strtotime($row3['event_time'])] = array('event_time' => strtotime($row3['event_time']), 'dateTime' => date('D M j, G:i', strtotime($row3['event_time'])), 'drank_amount' => round($row3['verbrauch_milch'], 2), 'eligible_amount' => round($row3['sollmenge_milch'], 2));
        }

        //return a string or array. 
        if (($stringOutput) == true) {
            $out[] = '<ul>';
            foreach ($feederVisit as $key => $value) {
                $out[] = '<li>';
                $out[] = $value['dateTime'] . '&nbsp;&nbsp;&nbsp;&nbsp; ' . $value['drank_amount'] . ' / ' . $value['eligible_amount'] . ' &#8467';
                $out[] = '</li>';
            }
            $out[] = '</ul>';
            $ret = implode($out);
        } else {
            $ret = $feederVisit;
        }
        return $ret;
    }

    /*
     * manual milk or elctrolyte fed by hand.
     */

    private function manualMilkEvents($bovine_id, $stringOutput = null) {

        $sql4 = "SELECT *,calf_milk_administered.id as calf_milk_administered_id FROM bovinemanagement.calf_milk_administered LEFT JOIN bovinemanagement.calf_milk ON calf_milk.id=calf_milk_administered.calf_milk_id WHERE bovine_id={$bovine_id} ORDER BY event_time DESC";
        $res4 = $GLOBALS['pdo']->query($sql4);
        while ($row4 = $res4->fetch(PDO::FETCH_ASSOC)) {
            $manualMilk[strtotime($row4['event_time'])] = array('id' => $row4['calf_milk_administered_id'], 'event_time' => strtotime($row4['event_time']), 'dateTime' => date('D M j, H:i', strtotime(($row4['event_time']))), 'method' => $row4['method'], 'userid' => $row4['userid'], 'name' => $row4['name'], 'drank_amount' => round($row4['dosage'], 2), 'eligible_amount' => 3);
        }

        //return a string or array. 
        if (($stringOutput) == true) {
            $out[] = '<ul>';
            foreach ($manualMilk as $key => $value) {

                $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('schemaTable' => 'bovinemanagement.calf_milk_administered'));

                $out[] = '<li>' . $value['dateTime'] . ' ' . $value['name'] . ' ' . $value['drank_amount'] . ' liters via ' . $value['method'] . ' by ' . $value['userid'] . $delButton->output($value['id']) . '</li>';
            }
            $out[] = '</ul>';

            $ret = implode($out);
        } else {
            $ret = $manualMilk;
        }
        return $ret;
    }

    private function medicalHistory($bovine_id) {


        //loookup bovineFUlleRegNumber
        $sql5 = "SELECT full_reg_number FROM bovinemanagement.bovine WHERE id={$bovine_id} limit 1";
        $res5 = $GLOBALS['pdo']->query($sql5);
        $row5 = $res5->fetch(PDO::FETCH_ASSOC);
        $bovineQueryMedicalLog = new BovineQueryMedicalLog($bovine_id, $row5['full_reg_number']);
        return $bovineQueryMedicalLog->main();
    }

    /**
     * popup content for calf list squares
     * @param type $tiere_id
     */
    private function calfListPopUpContent() {

        if (!empty($this->tiere_id) AND!empty($this->bovine_id)) {

            $dailyFeederVisits = (new BootStrap)->plainCard('Daily Feeder Visit Summary', $this->dailySumFeeder($this->tiere_id, true));

            //output formatting
            $html = <<<HTML
  
            
           <div class="row">
                <div class="col-md-4">
          
                    {$dailyFeederVisits}
                    
                    <h3>Every Feeder Visit > 72h</h3>
                    {$this->everyFeederVisit72h($this->tiere_id, true)}
                    
                </div>
                <div class="col-md-8">
                     
                            <h3> Manual Milk History </h3>
                            {$this->manualMilkEvents($this->bovine_id, true)}
                            
                            <h3> Medical History </h3>
                            {$this->medicalHistory($this->bovine_id)}
                            
                </div>
           </div>                  
           <div class="row">
                     <div class="col-md-8">
                            <h3> Feeder + Manual Milk Chart </h3>
                    {$this->displayChartForTotalConsumptionThroughTimePerCalf($this->tiere_id)}
                     </div>    
           </div>              
HTML;

            $html2 = <<<HTML
                                                <div class="container-fluid">
    <div class="row">
      <div class="col-md-4">.col-md-4</div>
      <div class="col-md-4 ml-auto">.col-md-4 .ml-auto</div>
    </div>
    <div class="row">
      <div class="col-md-3 ml-auto">.col-md-3 .ml-auto</div>
      <div class="col-md-2 ml-auto">.col-md-2 .ml-auto</div>
    </div>
    <div class="row">
      <div class="col-md-6 ml-auto">.col-md-6 .ml-auto</div>
    </div>
    <div class="row">
      <div class="col-sm-9">
        Level 1: .col-sm-9
        <div class="row">
          <div class="col-8 col-sm-6">
            Level 2: .col-8 .col-sm-6
          </div>
          <div class="col-4 col-sm-6">
            Level 2: .col-4 .col-sm-6
          </div>
        </div>
      </div>
    </div>
  </div>
         
HTML;



            $popoverContent = $html;

            return $popoverContent;
        } else {
            return 'Error: No Content. Animal ID Error.'; //no bovine id
        }
    }

    //main output 
    public function toStringContent() {
        return $this->calfListPopUpContent();
    }

    public function toStringTitle() {
        //popup title
        return 'Calf ' . $this->local_number;
    }

}

//end class
