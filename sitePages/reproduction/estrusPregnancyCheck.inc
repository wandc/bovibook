<?php

//keeps track of any cows who have a pregancy check done.
class EstrusPregnancyCheck extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Preg Check';
        $tabArray[2]['name'] = 'Due Dates';
        $tabArray[3]['name'] = 'Preg Chk needed > 29';
        $tabArray[4]['name'] = 'Preg Chk needed > 75';
        $tabArray[5]['name'] = 'Needs Bred';
        $tabArray[6]['name'] = 'Post Fresh Check';
        $tabArray[7]['name'] = '120 Days and No Vet Check';
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            $tabArray[8]['name'] = 'Insem Score';
        }
        $this->renderTabs($tabArray);
    }

    function tab1() {

        $this->param = new stdClass();
        $this->param->bovineID = null;
        $this->param->aniNumber = null;
        $a = new QF2_PregCheckEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'], 'bovine_id' => $this->param->bovineID)), 'group', $this->param); //group
        //only show to owner
        $pregCheckQF2 = ($GLOBALS['auth']->getOwnerAccess() == true) ? $a->renderBox('QF2_PregCheckEvent', 'Add Preg Check Event') : '';


        $left = <<<HTML
            {$this->displayPregnancyCheckLog('default')}
        HTML;

        $right = <<<HTML
        {$pregCheckQF2}  
        HTML;

        $content = (new Bootstrap)->oneThirdTwoThirds($left, $right);
        print($content);
    }

    function tab2() {
        $this->displayListOfConfirmedPregnant('default');
    }

    function tab3() {

        $this->displayListOfAnimalsNeedingPregnancyCheckDataTable();
    }

    function tab4() {
        print($this->displayListOfAnimalsNeedingADueDate('default'));
    }

    function tab5() {
        print($this->displayListOfAnimalsWithMostOpenDays('default'));
    }

    function tab6() {
        print($this->displayListOfAnimalsPreVWP('default'));
    }

    function tab7() {
        print($this->displayListOfAnimalsNotChecked('default'));
    }

    function tab8() {
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
            print($this->inseminationScore());

            print($this->inseminationScoreByFreezeDate());

            print($this->inseminationScoreByMonthUserBreed());
        }
    }

    public function summaryDisplay() {
        
    }

    public function inseminationScoreByMonthUserBreed() {
        $sql = <<<SQL

WITH temp as(
SELECT 'breeding|'::text || breeding_event.id AS id, 'breeding' AS type, breeding_event.id AS table_id,  bovinemanagement.round_to_nearest_date(breeding_event.actual_breeding_time + '280 days 12:00:00'::interval) AS calculated_potential_due_date,preg_check_event.preg_check_result,breeding_event.actual_breeding_userid,breeding_event.actual_breeding_time,sire_semen_code.*, date_trunc('month', breeding_event.actual_breeding_time) as month,actual_breeding_userid as userid,bovinemanagement.semen_code_to_dairy_or_beef(sire_semen_code.semen_code) as breed
           FROM bovinemanagement.breeding_event
LEFT JOIN bovinemanagement.preg_check_event ON estimated_breeding_embryo_event_id=('breeding|'::text || breeding_event.id)
 LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id
 LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code
WHERE actual_breeding_time is not null
)
SELECT distinct month,userid,breed,COUNT(id) OVER (PARTITION BY month,userid,breed) as total_breedings ,COUNT(preg_check_result) FILTER (WHERE preg_check_result= 'pregnant') OVER (PARTITION BY month,userid,breed) as total_pregnant FROM temp ORDER BY month DESC


SQL;
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $json = json_encode($results, JSON_PRETTY_PRINT);
        return $GLOBALS['MiscObj']->format_json_for_html($json);
    }

    public function inseminationScoreByFreezeDate() {
        $sql = <<<SQL
WITH temp as(
SELECT 'breeding|'::text || breeding_event.id AS id, 'breeding' AS type, breeding_event.id AS table_id,  bovinemanagement.round_to_nearest_date(breeding_event.actual_breeding_time + '280 days 12:00:00'::interval) AS calculated_potential_due_date,preg_check_event.preg_check_result,breeding_event.actual_breeding_userid,breeding_event.actual_breeding_time,sire_semen_code.*, date_trunc('month', breeding_event.actual_breeding_time) as month,actual_breeding_userid as userid,bovinemanagement.semen_code_to_dairy_or_beef(sire_semen_code.semen_code) as breed,semen_straw.freeze_date,short_name,sire.full_name
           FROM bovinemanagement.breeding_event
LEFT JOIN bovinemanagement.preg_check_event ON estimated_breeding_embryo_event_id=('breeding|'::text || breeding_event.id)
 LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id
 LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code
   LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number
WHERE actual_breeding_time is not null AND actual_breeding_time <= (current_date-interval '60 days')
)
SELECT distinct bovinemanagement.short_name(full_name),semen_code,breed,freeze_date,COUNT(id) OVER (PARTITION BY semen_code,freeze_date) as total_breedings ,COUNT(preg_check_result) FILTER (WHERE preg_check_result= 'pregnant') OVER (PARTITION BY semen_code,freeze_date) as total_pregnant FROM temp ORDER BY semen_code DESC


SQL;
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data


        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            /*
              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']);
              $rowOut['Full Name'] = ($row['full_name']);
              $rowOut['Sire'] = ($row['sire_short_name']);
              $rowOut['Fresh'] = JQueryDataTable::formattedDate($row['fresh_date']);
              $rowOut['Due'] = JQueryDataTable::formattedDate($row['due_date']);
              $rowOut['Last Pricing'] = JQueryDataTable::formattedDate($row['last_update']);
              $rowOut['Price'] = ($row['price']);
              $rowOut['Comment'] = ($row['comment']);
              $rowOut['Employee'] = ($row['userid']);
              $rowOut['Location'] = ($row['location_name']);
             */
            $resultsOut[] = $row;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = (new BootStrap)->infoCallout('NOTE: For breeding times 60 days ago and earlier.');

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Insemination Score by Freeze Data', $dataArr, 'paging: false'));
        return implode($out);
    }

    /* calculated insemination score for fred and david only */

    public function inseminationScore() {
        $res = $GLOBALS['pdo']->query("WITH temp as (
SELECT 'breeding|'::text || breeding_event.id AS id, 'breeding' AS type, breeding_event.id AS table_id,  bovinemanagement.round_to_nearest_date(breeding_event.actual_breeding_time + '280 days 12:00:00'::interval) AS calculated_potential_due_date,preg_check_event.preg_check_result,breeding_event.actual_breeding_userid,breeding_event.actual_breeding_time
           FROM bovinemanagement.breeding_event
      LEFT JOIN bovinemanagement.bovinecurr ON breeding_event.bovine_id = bovinecurr.id
   LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id
   LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code
   LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number
   LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id = breeding_event.comment_id
LEFT JOIN bovinemanagement.preg_check_event ON estimated_breeding_embryo_event_id=('breeding|'::text || breeding_event.id)
), temp2 as(
SELECT  DISTINCT ON(id) * FROM temp
)
 SELECT *, (SELECT count(temp2.id) FROM temp2 WHERE preg_check_result='pregnant' AND actual_breeding_userid='david') as david_num
,(SELECT count(temp2.id) FROM temp2 WHERE actual_breeding_userid='david') as david_den
,(SELECT count(temp2.id) FROM temp2 WHERE preg_check_result='pregnant' AND actual_breeding_userid='fred') as fred_num
,(SELECT count(temp2.id) FROM temp2 WHERE actual_breeding_userid='fred') as fred_den
 FROM temp2 
LIMIT 1
 ");
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $title = 'Percent pregnant over all breedings by that user';
        $david = "<p>David: " . round($row['david_num'] / $row['david_den'] * 100, 2) . "%</p>";
        $fred = "<p>Fred: " . round($row['fred_num'] / $row['fred_den'] * 100, 2) . "%</p>";
        $body = <<<HTML
                $david
                $fred
                HTML;

        return (new Bootstrap)->plainCard($title, $body);
    }

    public function displayPregnancyCheckLog($mode = 'default') {

        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
              {
                data: "event_time",
                title: "Event Time"
            },   
            {
                data: "local_number",
                title: "Animal"
            },   
            {
                data: "test_full_name",
                title: "Test"
            },   
            {
                data: "preg_check_result",
                title: "Result"
            },  
            {
                data: "estimate_twins",
                title: "Twins?"
            },
            {
                data: "comment_combined",
                title: "Comment"
            },   
            {
                data: "selected_event",
                title: "Selected Event"
            },   
            {
                data: "vet_userid",
                title: "Vet"
            },   
            {
                data: "userid",
                title: "Employee"
            },   
            {
                data: "action",
                title: "Action"
            }
       
EX;
        return ($dt->start('Preg Check Events', 35, 'pregnancyCheckLog', $mode, $columnsStr, 'highlight_important', ' paging: false'));
    }

    public function pregnancyCheckLog($request) {
        $mode = Sanit::reqVal('item', false, 'STR');

        $sql = "SELECT preg_check_event.id,preg_check_event.event_time as event_time,bovine.local_number,bovine.full_name,estimate_twins,
		combined_breeding_embryo_view.event_time as breeding_embryo_event_time,combined_breeding_embryo_view.id as breeding_embryo_id,combined_breeding_embryo_view.calculated_potential_due_date, combined_breeding_embryo_view.service_sire_short_name, combined_breeding_embryo_view.type, preg_check_event.preg_check_result,preg_check_event.userid,preg_check_type.test_full_name,preg_check_comment_custom,comment,bovine.id as bovine_id,vet_userid
FROM bovinemanagement.preg_check_event
LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id=preg_check_event.estimated_breeding_embryo_event_id
JOIN bovinemanagement.preg_check_type ON preg_check_event.preg_check_type_id = preg_check_type.id
JOIN bovinemanagement.bovine ON bovine.id=preg_check_event.bovine_id
LEFT JOIN bovinemanagement.preg_check_comment ON preg_check_comment.id = preg_check_event.preg_check_comment_id
ORDER BY preg_check_event.create_time DESC limit 1000";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'dateTimeName' => 'update_time', 'schemaTable' => 'bovinemanagement.preg_check_event'));

        //change presentation
        foreach ($results as &$row) {
            $row['event_time'] = date("M d, Y", strtotime($row['event_time']));  //has to be at end.

            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['comment_combined'] = $this->printCommnet($row['comment'], $row['preg_check_comment_custom']);

            $row['selected_event'] = $this->printSelectedBreedingEmbryoEvent($row['type'], $row['breeding_embryo_id'], $row['breeding_embryo_event_time'], $row['service_sire_short_name']);
            $row['estimate_twins'] = empty($row['estimate_twins']) ? '' : 'True';

            if ($mode == 'default') {
                $row['action'] = $delButton->output($row['id']);
            }
        }


        //output
        $output['data'] = $results;
        return ($output);
    }

    function printCommnet($comment, $preg_check_comment_custom) {

        //combine comments into one field.
        if ($preg_check_comment_custom == null) {
            $commentOut = $comment;
        } elseif ($comment == null) {
            $commentOut = $preg_check_comment_custom;
        } else {
            $commentOut = $comment . ' & ' . $preg_check_comment_custom;
        }
        return $commentOut;
    }

    function printSelectedBreedingEmbryoEvent($type, $breeding_embryo_id, $breeding_embryo_event_time, $service_sire_short_name) {

        if ($breeding_embryo_id != null) {
            return $type . ' : ' . date("M d Y", strtotime($breeding_embryo_event_time)) . ' to ' . $service_sire_short_name;
        } else {
            return null;
        }
    }

    function displayListOfConfirmedPregnant($mode) {

        $dt = new JQueryDataTable();
        $options = '"paging": false,  "order": [[6,\'asc\']],';
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },   
              {  
                data: "event_time",
                title: "Event Time",
                type: "date"
            },
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_current_name",
                title: "Current Location"
            },
            {
                data: "preg_check_method",
                title: "Method"
        
            },
            {
             data: "estimate_twins",
                title: "Twins?"
            },
            {
                data: "due_date",
                title: "Due Date",
               type: "date"
            },
             {
                data: "service_sire_short_name",
                title: "Service Sire"
            },
          {
                data: "offspring_pro_doll",
                title: "Fetus Pro$"
            },
        
        
        
EX;
        print($dt->start('Pregnant w/ Confirmed Due Date', 35, 'listOfConfirmedPregnant', null, $columnsStr, null, false, $options));
    }

    public function listOfConfirmedPregnant($mode) {
        // Setup your database connection
        $sql = "with temp as(
SELECT id as bovine_id,calculated_potential_due_date as event_time2,* 

from bovinemanagement.pregnant_view

)
/* find PA pro doll for offspring */
SELECT * , round(((SELECT prodoll FROM batch.aggregate_view_all WHERE full_reg_number=(SELECT full_reg_number FROM bovinemanagement.bovine WHERE id=temp.bovine_id)) +
(SELECT prodoll FROM batch.aggregate_view_all WHERE full_reg_number=service_sire_full_reg_number))/2) as offspring_pro_doll
from temp order by calculated_potential_due_date";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['event_time']);
            $row['event_time'] = date("M d,Y", strtotime($row['event_time']));
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['estimate_twins'] = empty($row['estimate_twins']) ? '' : 'True';
            $row['offspring_pro_doll'] = empty($row['offspring_pro_doll']) ? '<i class="fab fa-github-alt"></i>' : $row['offspring_pro_doll'];

            //location_current_name
            //preg_check_method 
            $row['due_date'] = date("M d, Y", strtotime($row['calculated_potential_due_date']));
            //service_sire_short_name
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    //return JSON
    public function listOfAnimalsNeedingPregnancyCheck($request) {
        $sql = "SELECT combined_breeding_embryo_view.recipient_bovine_id  as bovine_id, combined_breeding_embryo_view.recipient_local_number as local_number, combined_breeding_embryo_view.event_time, donotbreed,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,
preg_check_event.event_time as preg_check_time, date_trunc('days',now()-combined_breeding_embryo_view.event_time) as days_ago, bovinecurr.full_name, bovinecurr.location_name,preg_check_event.estimated_breeding_embryo_event_id,date_trunc('days',now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id)) as days_volun
   FROM bovinemanagement.bovinecurr

   LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id = 
        (SELECT id FROM bovinemanagement.combined_breeding_embryo_view 
         WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id AND event_time = 
             (SELECT max(event_time) FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id))

   LEFT JOIN bovinemanagement.preg_check_event ON preg_check_event.id = 
        ((SELECT preg_check_event.id FROM bovinemanagement.preg_check_event
        WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric AND preg_check_event.event_time = 
             (( SELECT max(preg_check_event.event_time) AS max FROM bovinemanagement.preg_check_event WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric))))
 
LEFT JOIN bovinemanagement.service_picks_curr ON service_picks_curr.bovine_id=bovinecurr.id

WHERE combined_breeding_embryo_view.event_time IS NOT NULL
AND ( preg_check_event.event_time IS NULL OR preg_check_event.preg_check_result = 'recheck' OR preg_check_event.event_time <=  combined_breeding_embryo_view.event_time)
AND combined_breeding_embryo_view.event_time + interval '29 days' <= now()
ORDER BY days_ago DESC;";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);



        //change presentation
        foreach ($results as &$row) {

            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['event_time_raw'] = ($row['days_ago']);
            $row['note'] = $this->printWarning($row['bovine_id']);
            //  $row['note'] = 'h';
            if ($row['days_ago'] >= 80) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            if ($row['days_ago'] >= 80) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            $row['donotbreed2'] = ($row['donotbreed'] == 1 ? 'true' : '');
            // $row['action'] = $this->printActionButtons2($row['estrus_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            // $row['event_time']= date("l M d H:00",strtotime($row['event_time']));  //has to be at end.
            // if  ((($row['estrus_type_id'])==1) AND empty($row['note']))  {  $row['colour_row']=1;} else {  $row['colour_row']=0;}
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayListOfAnimalsNeedingPregnancyCheckDataTable($bovine_id = null) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
                 
            {
                data: "local_number",
                title: "Animal"
            },
         {
                data: "days_ago",
                title: "Last Breeding (days)"
            },
         {
                data: "location_name",
                title: "Current Location"
            },
        {
                data: "days_volun",
                title: "Days Open"
            },
         {
                data: "dim",
                title: "DIM"
            },
 
         {
                data: "donotbreed2",
                title: "donotbreed2"
            },
        {
                data: "note",
                title: "note"
            }
           
EX;
        print($dt->start('Need a Preg Check', 35, 'listOfAnimalsNeedingPregnancyCheck', $bovine_id, $columnsStr, 'highlight_important'));
    }

//check if 29D conception test works.
    function printWarning($bovine_id) {

        //lookup if it has been less than 100 days from calving. If so, print warning that 29D conception does not work.
        $sql = "SELECT event_time + interval '100 days' as check_date FROM bovinemanagement.calving_event WHERE lactation_id = (SELECT lactation.id FROM bovinemanagement.lactation WHERE lactation.bovine_id=$bovine_id ORDER BY fresh_date DESC LIMIT 1)";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (!empty($row['check_date'])) {
            if (strtotime($row['check_date']) > strtotime("now")) {
                return "Warning: 29D Conception test ineffective until 100 DIM.";
            } else {
                return null;
            }
        }
        return null;
    }

    public function displayListOfAnimalsNeedingADueDate($mode = 'default') {


        // Setup your database connection
        //This query works thanks to Jamie. The main key was putting on a combined unqiue constraint for bovine_id/actual_breeding_time on breeding_event and bovine_id/event_time on preg_check_event
        //this allows the select max part to work and not return more than one row in the inner select statements.
        $sql = "with temp as (
SELECT combined_breeding_embryo_view.recipient_bovine_id  as bovine_id, combined_breeding_embryo_view.recipient_local_number as local_number, combined_breeding_embryo_view.event_time,  donotbreed,
preg_check_event.event_time as preg_check_time, date_trunc('days',now()-combined_breeding_embryo_view.event_time) as days_ago, bovinecurr.full_name, bovinecurr.location_name,preg_check_event.estimated_breeding_embryo_event_id,date_trunc('days',now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id)) as days_volun
   FROM bovinemanagement.bovinecurr

   LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id = 
        (SELECT id FROM bovinemanagement.combined_breeding_embryo_view 
         WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id AND event_time = 
             (SELECT max(event_time) FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id))

LEFT JOIN bovinemanagement.service_picks ON service_picks.bovine_id=bovinecurr.id

   LEFT JOIN bovinemanagement.preg_check_event ON preg_check_event.id = 
        ((SELECT preg_check_event.id FROM bovinemanagement.preg_check_event
        WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric AND preg_check_event.event_time = 
             (( SELECT max(preg_check_event.event_time) AS max FROM bovinemanagement.preg_check_event WHERE preg_check_event.bovine_id = combined_breeding_embryo_view.recipient_bovine_id::numeric))))
 
WHERE combined_breeding_embryo_view.event_time IS NOT NULL
AND ( preg_check_event.event_time IS NULL OR preg_check_event.preg_check_result = 'recheck' OR preg_check_event.event_time <=  combined_breeding_embryo_view.event_time OR preg_check_event.estimated_breeding_embryo_event_id IS NULL )
AND combined_breeding_embryo_view.event_time + interval '75 days' <= now()
ORDER BY days_ago DESC
)
SELECT distinct(bovine_id),* from temp";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Last Breeding (days)'] = $row['days_ago'];
            $rowOut['Days Open'] = $row['days_volun'];
            $rowOut['Location'] = $row['location_name'];
            $rowOut['Do Not Breed?'] = $row['donotbreed'];
            $rowOut['Estimated Breeding/Embryo ID'] = (empty($row['estimated_breeding_embryo_event_id'])) ? '' : $row['estimated_breeding_embryo_event_id'];



            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Preg Check Needed > 75 days', $dataArr));

        return implode($out);
    }

    public function displayListOfAnimalsWithMostOpenDays($mode = 'default') {

        // Setup your database connection
        //display cows who have not been bred or had embryo put in for 21 days after voluntary waiting period.
        //the 60 day term is a bit of a hack to make sure we catch cows we breed before voluntary waiting period.
        $sql = "SELECT bovinecurr.local_number, bovinecurr.full_name, bovinecurr.id as bovine_id, bovinecurr.location_name, combined_breeding_embryo_view.event_time as breeding_embryo_event_time,	date_trunc('day',justify_hours(now()-bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id))) as days_open,service_picks_curr.donotbreed as donotbreed
 FROM bovinemanagement.bovinecurr
  LEFT JOIN bovinemanagement.service_picks_curr ON service_picks_curr.bovine_id=bovinecurr.id
 LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id = 
        (SELECT id FROM bovinemanagement.combined_breeding_embryo_view 
         WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id AND event_time = 
             (SELECT max(event_time) FROM bovinemanagement.combined_breeding_embryo_view WHERE combined_breeding_embryo_view.recipient_bovine_id=bovinecurr.id))
WHERE (combined_breeding_embryo_view.event_time < bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id) - interval '60 days' OR combined_breeding_embryo_view.event_time IS null)
      AND bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id) + interval '21 days' <= now() AND donotbreed=false
ORDER BY days_open DESC
";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            //  $row['date'] = date("M Y", strtotime($row['date']));  //has to be at end.      
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Past Waiting Period'] = $row['days_open'];
            $rowOut['Location'] = $row['location_name'];


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Cows >21 Days Past Waiting Period with No Breedings and Eligible to Breed', $dataArr, ''));
        return implode($out);
    }

    public function displayListOfAnimalsPreVWP($type) {

        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                           
             {
                            data: "local_number",
                            title: "local_number"
                        },
                        {
                            data: "location_name",
                            title: "location_name"
                        },
                        {
                            data: "fresh_date",
                            title: "fresh_date"
                        },
                    {
                            data: "calculate_breeding_voluntary_waiting_period",
                            title: "calculate_breeding_voluntary_waiting_period"
                        }
EX;
        return ($dt->start('Pre VWP Check', $this->pageid, 'freshCowCheckBeforeVWP2', null, $columnsStr));
    }

    public function displayListOfAnimalsNotChecked($type) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                           
             {
                            data: "local_number",
                            title: "local_number"
                        },
                        {
                            data: "location_name",
                            title: "location_name"
                        },
                        {
                            data: "fresh_date",
                            title: "fresh_date"
                        },
                    {
                            data: "checkToBeSorted",
                            title: "To Be Sorted"
                        }
EX;
        return ($dt->start('120 Days and No Vet Check', $this->pageid, 'animalsNotChecked', null, $columnsStr));
    }

    public function animalsNotChecked($request) {
        $sql = "with temp as (
SELECT * ,
(SELECT event_time from bovinemanagement.preg_check_event where bovine_id=bovinecurr.id order by event_time desc limit 1) as peg_event_time
from bovinemanagement.bovinecurr 
  LEFT JOIN bovinemanagement.service_picks_curr ON service_picks_curr.bovine_id=bovinecurr.id
where fresh_date is not null and dry_date is null
)
SELECT * from temp where (peg_event_time <= fresh_date OR peg_event_time is null) AND current_date >= fresh_date + (interval '120 days') AND  donotbreed=false";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'], $row['local_number'], $row['full_name']);

            //sort gate

            $row['checkToBeSorted'] = MovementSortGate::checkToBeSorted($row['bovine_id'], $row['event_time']);
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

    public function freshCowCheckBeforeVWP2($request) {
        $sql = "with temp as (
SELECT *,id as bovine_id,bovinemanagement.calculate_breeding_voluntary_waiting_period (id),bovinemanagement.calculate_number_of_breeding_since_fresh (id),bovinemanagement.donotbreed_at_timestamp (id, current_date::timestamp) from bovinemanagement.bovinecurr
)
SELECT * from temp where calculate_number_of_breeding_since_fresh=0 and donotbreed_at_timestamp=false AND calculate_breeding_voluntary_waiting_period >= current_date";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'], $row['local_number'], $row['full_name']);
        }
        //output
        $output['data'] = $results;
        return ($output);
    }

//passes along error info.
    public function error() {
        $error = array();

        //find who has not been moved to maternity pack a week from calving. TODO: We maybe should have a lower grade warning at 2 weeks before.
        $res = $GLOBALS['pdo']->query("SELECT local_number,calculated_potential_due_date,location_current_name FROM bovinemanagement.pregnant_view WHERE (calculated_potential_due_date - interval '7 days') <= current_timestamp AND location_id!=38 ORDER BY calculated_potential_due_date DESC,local_number");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error[] = new notifyObj(get_class($this), $row['calculated_potential_due_date'], 1, $row['local_number'] . ' needs to be moved to Maternity Pack.');
        }



        return $error;
    }

    //grab a list of vets for optional record of who the vet was for preg check.
    public function listOfVetsForPregCheck(): array {

        $sqlVet = "SELECT * from wcauthentication.users_in_groups
LEFT JOIN wcauthentication.users ON users.userid=users_in_groups.userid
WHERE groupid='veterinary' and active=true";
        $resVet = $GLOBALS['pdo']->query($sqlVet);

        $vet[null] = "Please Select one..."; //dummy value
        while (($row = $resVet->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $vet[$row['userid']] = $row['firstname'] . ' ' . $row['lastname'];
        }
        return $vet;
    }

}

//end class



Class QF2_PregCheckEvent extends QF2_Base {

    function definition($form) {


        $dateArray = $GLOBALS['MiscObj']->createDailyDatesForLastYearAtNoon(); //used when we have a problem. should add an if admin clause. 

        $aniNumberBreedingsStd = $this->bredAnimalList($this->param->bovineID);

        //var_dump($aniNumberBreedingsStd->aniNumber);
        // var_dump($aniNumberBreedingsStd->aniNumberBreedings);
        //grab a list of pregnancy check tests
        $res = $GLOBALS['pdo']->query("SELECT id,test_full_name FROM bovinemanagement.preg_check_type ORDER BY id");

        $typeOfPregnacyCheck[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $typeOfPregnacyCheck[$row['id']] = $row['test_full_name'];
        }
        ///
        //grab a list of pregnancy check comments
        $res = $GLOBALS['pdo']->query("SELECT id,comment FROM bovinemanagement.preg_check_comment ORDER BY id");

        $pregnancyCheckComment[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $pregnancyCheckComment[$row['id']] = $row['comment'];
        }
        ///
        //grab a list of vets for optional record of who the vet was for preg check.
        $sqlVet = "SELECT * from wcauthentication.users_in_groups
LEFT JOIN wcauthentication.users ON users.userid=users_in_groups.userid
WHERE groupid='veterinary' and active=true";
        $resVet = $GLOBALS['pdo']->query($sqlVet);

        $vet[null] = "Please Select one..."; //dummy value
        while (($row = $resVet->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $vet[$row['userid']] = $row['firstname'] . ' ' . $row['lastname'];
        }


        //if vet was recent last entry use that as default. 
        $sqlVetDefault = "SELECT id,trim(vet_userid) as vet_userid FROM bovinemanagement.preg_check_event ORDER BY update_time  DESC limit 1";
        $resVetDefault = $GLOBALS['pdo']->query($sqlVetDefault);
        $rowVetDefault = $resVetDefault->fetch(PDO::FETCH_ASSOC);
        $vetDefaultUserid = $rowVetDefault['vet_userid']; //will be null if no vet used, so should be fine.
        ///
        //preg result types
        $pregnancyCheckResult = array(null => 'Please Select one...', 'pregnant' => 'Pregnant', 'open' => 'Open', 'recheck' => 'Recheck');


        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $hierselect = $form->addElement('hierselect', 'breeding');
            $hierselect->setLabel('Animal');
            $hierselect->loadOptions(array($aniNumberBreedingsStd->aniNumber, $aniNumberBreedingsStd->aniNumberBreedings));
        } else {           
            if (!empty($aniNumberBreedingsStd->aniNumberBreedings)) {
            $breedingsForThisBovine = (array_key_exists($this->param->bovineID, $aniNumberBreedingsStd->aniNumberBreedings)) ? $aniNumberBreedingsStd->aniNumberBreedings[$this->param->bovineID] : null;
            }
            else {
              $breedingsForThisBovine = null;  
            }
            $form->addElement('hidden', 'bovine_id')->setValue($this->param->bovineID);
            $form->addElement('select', 'breeding', null, array('options' => $breedingsForThisBovine, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");
        }




        $form->addElement('select', 'eventTime', null, array('options' => $dateArray, 'label' => 'Time of Preg Check'))->addRule("required", "Please specify your time of preg check.");
        $form->addElement('select', 'typeOfPregnacyCheck', null, array('options' => $typeOfPregnacyCheck, 'label' => 'Type of Check'))->addRule("required", "Please specify your type of check.");
        $form->addElement('select', 'pregnancyCheckResult', null, array('options' => $pregnancyCheckResult, 'label' => 'Result of Check'))->addRule("required", "Please specify your result of preg check.");
        $form->addElement('select', 'vet_userid', null, array('options' => $vet, 'label' => 'Vet (Optional)'));
        $form->addElement('checkbox', 'twins', array(), array('label' => 'Twins?'));
        $form->addElement('select', 'pregnancyCheckComment', null, array('options' => $pregnancyCheckComment, 'label' => 'Canned Comment'));
        $form->addElement('text', 'customComment', null, array('label' => 'Custom Comment'));


        //defaults
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array('animalNumber' => 0, 'vet_userid' => $vetDefaultUserid, 'twins' => false)));


        return $form; //required       
    }

    function process($value) {
        //  (DEBUG) ?: $GLOBALS['debug']->writeLine(var_dump($value));
        //individual vs group to get breeding event and bovine_id
        $breeding_event_id = ($this->mode) == 'group' ? $value['breeding'][1] : $value['breeding'];
        $bovine_id = ($this->mode) == 'group' ? $value['breeding'][0] : $value['bovine_id'];
        //
        $value['event_time'] = date('Y-m-d H:i:sO', $value['eventTime']);
        $userid = $GLOBALS['auth']->getUsername();
        $pregnancyCheckComment = empty($value['pregnancyCheckComment']) ? null : $value['pregnancyCheckComment'];
        $customComment = empty($value['customComment']) ? null : $value['customComment'];



        //if a cow is open it doesn't really make sense to link it to a breeding.
        if ($value['pregnancyCheckResult'] == 'open' || $value['pregnancyCheckResult'] == 'recheck') {
            $breeding_event_id = 'NULL';
        }
        elseif (($value['pregnancyCheckResult'] == 'pregnant' ) &&  (strstr('breeding|',$breeding_event_id))) {
           throw new exception("Error: Result is pregnant, but no breeding_event_id, instead got $breeding_event_id. Must have valid breeding_event_id.");
        }


        $sql = <<<SQL
          INSERT INTO bovinemanagement.preg_check_event 
          (event_time,preg_check_type_id,preg_check_comment_id,preg_check_comment_custom,userid,estimated_breeding_embryo_event_id,preg_check_result,bovine_id,vet_userid)
          VALUES
          (:event_time,:preg_check_type_id,:preg_check_comment_id,:preg_check_comment_custom,:userid,:estimated_breeding_embryo_event_id,:preg_check_result,:bovine_id,:vet_userid)    
       SQL;

        $GLOBALS['pdo']->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        $res = $GLOBALS['pdo']->prepare($sql);
        $res->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $res->bindParam(':preg_check_type_id', $value['typeOfPregnacyCheck'], PDO::PARAM_INT);
        $res->bindParam(':preg_check_comment_id', $pregnancyCheckComment, PDO::PARAM_INT);
        $res->bindParam(':preg_check_comment_custom', $customComment, PDO::PARAM_STR);
        $res->bindParam(':userid', $userid, PDO::PARAM_STR);
        $res->bindParam(':estimated_breeding_embryo_event_id', $breeding_event_id, PDO::PARAM_STR);
        $res->bindParam(':preg_check_result', $value['pregnancyCheckResult'], PDO::PARAM_STR);
        $res->bindParam(':bovine_id', $bovine_id, PDO::PARAM_INT);
        $res->bindParam(':vet_userid', $value['vet_userid'], PDO::PARAM_STR);


        $res->execute();
        $res->debugDumpParams();
        //   (DEBUG) ?: $GLOBALS['debug']->writeLine('Preg Check');
        //   (DEBUG) ?: $GLOBALS['debug']->writeLine($res->debugDumpParams());
    }

    //used to generate arrays for hier select. 
    function bredAnimalList($bovine_id) {

        $aniNumber = null;
        $aniNumberBreedings = null;
        if ($this->mode == 'group') {
            //grab a list of cows
            //show any current cows who have been bred since there last lactation or for heifers since birth....
            $res = $GLOBALS['pdo']->query("SELECT 
		combined_breeding_embryo_view.id as combined_breeding_embryo_id, combined_breeding_embryo_view.recipient_bovine_id as bovine_id, combined_breeding_embryo_view.event_time as event_time, bovinecurr.local_number, bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.dry_date,
	  date_part('day',(localtimestamp - combined_breeding_embryo_view.event_time)) as days_since_breeding, combined_breeding_embryo_view.service_sire_full_name as sire_full_name, combined_breeding_embryo_view.service_sire_short_name as sire_short_name
,(SELECT preg_check_result FROM bovinemanagement.preg_check_event where combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id AND preg_check_result != 'open' AND event_time=(SELECT max(event_time) FROM bovinemanagement.preg_check_event WHERE bovine_id=bovinecurr.id))
FROM bovinemanagement.combined_breeding_embryo_view
JOIN bovinemanagement.bovinecurr ON bovinecurr.id = combined_breeding_embryo_view.recipient_bovine_id
WHERE combined_breeding_embryo_view.event_time IS NOT NULL
ORDER BY local_number,event_time DESC");
            $aniNumber[null] = "Please Select one..."; //dummy value
        } elseif ($this->mode == 'individual') {

            $res = $GLOBALS['pdo']->query("SELECT 
		combined_breeding_embryo_view.id as combined_breeding_embryo_id, combined_breeding_embryo_view.recipient_bovine_id as bovine_id, combined_breeding_embryo_view.event_time as event_time, bovinecurr.local_number, bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.dry_date,
	  date_part('day',(localtimestamp - combined_breeding_embryo_view.event_time)) as days_since_breeding, combined_breeding_embryo_view.service_sire_full_name as sire_full_name, combined_breeding_embryo_view.service_sire_short_name as sire_short_name
,(SELECT preg_check_result FROM bovinemanagement.preg_check_event where combined_breeding_embryo_view.id=estimated_breeding_embryo_event_id AND preg_check_result != 'open' AND event_time=(SELECT max(event_time) FROM bovinemanagement.preg_check_event WHERE bovine_id=$bovine_id))
                    FROM bovinemanagement.combined_breeding_embryo_view
JOIN bovinemanagement.bovinecurr ON bovinecurr.id = combined_breeding_embryo_view.recipient_bovine_id
WHERE combined_breeding_embryo_view.event_time IS NOT NULL AND bovinecurr.id=$bovine_id
ORDER BY local_number,event_time DESC");
        } else {
            throw new Exception("Error unkown bredAnimalList mode: (group or individual)<br/>.");
        }

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //parse for last word of name and then make an array for the select list.
            $a = preg_split("/[\s,]+/", $row['full_name']);
            $aniNumber[$row['bovine_id']] = $row['local_number'] . " - " . end($a);
            $preg_check_result = null;
            if (trim($row['preg_check_result']) == 'pregnant') {
                $preg_check_result = ' (marked ' . $row['preg_check_result'] . ')';
            }
            $aniNumberBreedings[$row['bovine_id']][$row['combined_breeding_embryo_id']] = $row['days_since_breeding'] . " days ago to " . $row['sire_short_name'] . $preg_check_result;
        }

        //return
        $ret = new stdClass();
        $ret->aniNumber = $aniNumber;
        $ret->aniNumberBreedings = $aniNumberBreedings;
        return $ret;
    }

}

//end class

