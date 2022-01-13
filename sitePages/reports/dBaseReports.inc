<?php

/** reports for Fred that are like his old dBase reports */
class DBaseReports extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Weekly #1 - Not Bred';
        $tabArray[2]['name'] = 'Weekly #2 - Bred & Not Pregnant';
        $tabArray[3]['name'] = 'Weekly #3 - Breeding Age Heifer';
        $this->renderTabs($tabArray);
    }

    function tab1() {

        $this->displayWeekly1Report();
    }

    function tab2() {
        $this->displayWeekly2Report();
        print('<div id="twoColumnLayout">');

        print('</div>');
    }
    
    function tab3() {
        print($this->displayWeekly3Report());
        print('<div id="twoColumnLayout">');

        print('</div>');
    }

    
    
     public function displayWeekly3Report($request=null) {
        $sql = "
                SELECT bovinecurr.id as bovine_id, bovinecurr.location_name,bovinecurr.id, bovinecurr.local_number,bovinecurr.full_name, bovinecurr.fresh_date, 
                current_date-matview_breeding_voluntary_waiting_period.calculate_breeding_voluntary_waiting_period  as vwp,
               bovinecurr.location_name,(SELECT event_time::date ||' '|| type || ': '  ||  text FROM bovinemanagement.reproduction_events(bovinecurr.id) limit 1) as last_repro_event,
               date_trunc('days',current_date - bovinemanagement.last_breeding(bovinecurr.id)) as last_breeding
FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
    LEFT JOIN bovinemanagement.matview_breeding_voluntary_waiting_period ON matview_breeding_voluntary_waiting_period.id=bovinecurr.id
WHERE  bovinemanagement.lactation_number(bovinecurr.id)=0 AND calculated_potential_due_date is null AND matview_breeding_voluntary_waiting_period.calculate_breeding_voluntary_waiting_period <=current_date
ORDER BY matview_breeding_voluntary_waiting_period.calculate_breeding_voluntary_waiting_period ASC;
";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $rowOut=array();
        //change presentation
        foreach ($results as &$row) {
              $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
              $rowOut['Current Location'] = $row['location_name'];
              $rowOut['Past VWP'] = ($row['vwp']);
              $rowOut['Last Breeding'] = ($row['last_breeding']);
              $rowOut['Last Repro Event'] = $row['last_repro_event'];
              $retArr = (new BovineQueryBreedingChoice)->nextBreedingAction($row['bovine_id'], false);
              $rowOut['Next Repro Action'] = $retArr['nextActionTxt'];
              $rowOut['colour_row'] =  ( $rowOut['Past VWP'] >60) ? 1 : 0;
               
              
            $resultsOut[] = $rowOut;
        }
     
       
         //output
        $dataArr = $resultsOut;
        //var_dump($dataArr);
        $out[]=   ("<h3>Heifers not pregnant.</h3>");
        $out[] = ( (new JQueryDataTable)->startBasicArrayColour('Title BasicArrayColour CHANGE ME', $dataArr, 'searching: true,paging: false,ordering: false, "order": [[ 1, "desc" ]], info: true'));
        return implode($out);
    }

    
    public function weekly2Report($request) {
        $sql = "
               SELECT bovinecurr.id as bovine_id, bovinecurr.location_name,bovinecurr.id, bovinecurr.local_number,bovinecurr.full_name, bovinecurr.fresh_date, date_trunc('days',current_date - bovinemanagement.last_breeding(bovinecurr.id)) as last_breeding,
               bovinecurr.location_name,(SELECT event_time::date ||' '|| type || ': '  ||  text FROM bovinemanagement.reproduction_events(bovinecurr.id) limit 1) as last_repro_event
FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id=bovinecurr.id
WHERE bovinecurr.location_id in (40,41,39,42) AND bovinemanagement.calculate_number_of_recent_breedings_embyro_implants(bovinecurr.id)>0 AND calculated_potential_due_date is null
ORDER BY fresh_date ASC;
";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);

            $row['event_time_raw'] = strtotime($row['fresh_date']);
            //$row['note'] = $this->reasonsNotToBreed($row['bovine_id'],$row['event_time'],$row['birth_date']);
            //$row['action'] = $this->printActionButtons2($row['estrus_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
             
            $row['fresh_date'] = date("Y-m-d", strtotime($row['fresh_date']));  //has to be at end.
            $retArr = (new BovineQueryBreedingChoice)->nextBreedingAction($row['bovine_id'], false);
            $row['breed_to'] = $retArr['nextActionTxt'];
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayWeekly2Report() {
        print("<h3>Cows bred/implanted and not pregnant.</h3>");
        $dt = new JQueryDataTable();
              $options = '"paging": false,  "order": [[0,\'asc\']],';
        $columnsStr = <<<EX
                {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },   
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "fresh_date",
                title: "Fresh Date",
                type: "date"
            },
            {
                data: "last_repro_event",
                title: "Last Repro Event"
            },
            {
                data: "last_breeding",
                title: "Last Breeding"
            },
            {
                data: "breed_to",
                title: "Next Repro Action"
            }
           
EX;
        print($dt->start('Title CHANGE ME', $this->pageid, 'weekly2Report', null, $columnsStr, null, false, $options));
    }

    public function weekly1Report($request) {
        $sql = "
               SELECT bovinecurr.id as bovine_id, bovinecurr.location_name,bovinecurr.id, bovinecurr.local_number,bovinecurr.full_name, bovinecurr.fresh_date, bovinecurr.location_name
               ,(SELECT event_time::date ||' '|| type || ': '  ||  text FROM bovinemanagement.reproduction_events(bovinecurr.id) limit 1) as last_repro_event
FROM bovinemanagement.bovinecurr 
WHERE bovinecurr.location_id in (40,41,39,42) AND bovinemanagement.calculate_number_of_recent_breedings_embyro_implants(bovinecurr.id)=0
ORDER BY fresh_date ASC;
";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['event_time_raw'] = strtotime($row['fresh_date']);
            $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            //$row['event_time_raw']=strtotime($row['event_time']);
            //$row['note'] = $this->reasonsNotToBreed($row['bovine_id'],$row['event_time'],$row['birth_date']);
            //$row['action'] = $this->printActionButtons2($row['estrus_id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            $row['fresh_date'] = date("Y-m-d", strtotime($row['fresh_date']));  //has to be at end.
            $retArr = (new BovineQueryBreedingChoice)->nextBreedingAction($row['bovine_id'], false);
            $row['breed_to'] = $retArr['nextActionTxt'];
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayWeekly1Report() {
        print("<h3>Milking animals with zero breedings.</h3>");
        $dt = new JQueryDataTable();
        $options = '"paging": false,  "order": [[0,\'asc\']],';
        $columnsStr = <<<EX
                {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },   
           {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "fresh_date",
                title: "Fresh Date",
                type: "date"
            },
            {
                data: "last_repro_event",
                title: "Last Repro Event"
            },
            {
                data: "breed_to",
                title: "Next Repro Action"
            }
           
EX;
        print($dt->start('Title CHANGE ME', $this->pageid, 'weekly1Report', null, $columnsStr, null, false, $options));
    }

}

//end class
?>