<?php


require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryBreedingChoice.inc');
;

/** reports for Fred that are like his old dBase reports */
class DBaseReports extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Weekly 1';
        $tabArray[2]['name'] = 'Weekly 2';
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

    public function weekly2Report($request) {
        $sql = "
               SELECT bovinecurr.id as bovine_id, bovinecurr.location_name,bovinecurr.id, bovinecurr.local_number,bovinecurr.full_name, bovinecurr.fresh_date, 
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
            $retArr = BovineQueryBreedingChoice::nextBreedingAction($row['bovine_id'], false);
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
                data: "breed_to",
                title: "Next Repro Action"
            }
           
EX;
        print($dt->start($this->pageid, 'weekly2Report', null, $columnsStr, null, false, $options));
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
            $retArr = bovineQueryBreedingChoice::nextBreedingAction($row['bovine_id'], false);
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
        print($dt->start($this->pageid, 'weekly1Report', null, $columnsStr, null, false, $options));
    }

}

//end class
?>