<?php

;

class MovementLocationList extends BasePage {

    public function defaultDisplay() {

        $this->locationSelectQuickFormJS();




        if (!empty($_REQUEST['location'])) {
            self::displayAnimalsByLocation(($_REQUEST['location']));
        }
    }

    function locationSelectQuickFormJS() {


        //get active locations
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        $locations = QF2_MoveAnimal::listActiveLocationsAndCounts();
        $locations[9999999] = 'All Locations';
        $style = '';
        // custom select input
        // with javascript to allow submission on change of select element.
        print ("<select $style onchange=\"javascript:location.href='?pageid={$_REQUEST['pageid']}&amp;location='+this.options[this.selectedIndex].value;$('#example &gt; ul').tabs('select', 0);\" name=\"locationList\">");
        foreach ($locations as $key => $value) {
            print ("<option value=\"$key\">$value</option>" . "\n\r");
        }
        print ('</select>');
    }

    public function showCowsInSpecificLocation($request) {

        //get location
        if (!empty($request['item'])) {
            if (is_numeric($request['item'])) {
                $location = ($_REQUEST['item']);
            } else {
                $location = 9999999;
            }
        } else {
            $location = 9999999;
        }

        if ($location == 9999999) {

            $sql = "SELECT bovinecurrall.id as bovine_id,bovinecurrall.local_number, prodoll,bovinecurrall.full_name, bovinecurrall.location_last_event_time as newest_event_time, bovinecurrall.location_name, bovinecurrall.birth_date ,regexp_replace(bovinecurrall.full_name, '^.* ', '') as short_name,bovinecurrall.fresh_date,bovinemanagement.due_date (bovinecurrall.id) FROM bovinemanagement.bovinecurrall left join batch.aggregate_view_curr on aggregate_view_curr.bovine_id = bovinecurrall.id";
            $statement = $GLOBALS['pdo']->prepare($sql);
            $statement->execute();
        } else {

            $sql = "SELECT bovinecurrall.id as bovine_id,bovinecurrall.local_number, prodoll,bovinecurrall.full_name, bovinecurrall.location_last_event_time as newest_event_time, bovinecurrall.location_name, bovinecurrall.birth_date ,regexp_replace(bovinecurrall.full_name, '^.* ', '') as short_name,bovinecurrall.fresh_date,bovinemanagement.due_date (bovinecurrall.id) FROM bovinemanagement.bovinecurrall left join batch.aggregate_view_curr on aggregate_view_curr.bovine_id = bovinecurrall.id where bovinecurrall.location_id=:location ";
            $statement = $GLOBALS['pdo']->prepare($sql);
            $statement->bindParam(':location', $location, PDO::PARAM_INT);
            $statement->execute();
        }

        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['number_name'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['birth_date'] = date('M d, Y', strtotime($row['birth_date']));
            $row['fresh_date'] = (empty($row['fresh_date']) ? null : date('M d, Y', strtotime($row['fresh_date'])));
            $row['due_date'] = date('M d, Y', strtotime($row['due_date']));
            $row['location_name'] = ($row['location_name']);
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

    function displayAnimalsByLocation($location) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
        {
                            data: "number_name",
                            title: "Animal",
                             type: "num-html"
                        },
                        {
                            data: "birth_date",
                            title: "Birth Date",
                            type: "date",
                        },
                        {
                            data: "prodoll",
                            title: "prodoll"
                        },
                        {
                            data: "location_name",
                            title: "Location"
                        },
                        {
                            data: "fresh_date",
                            title: "Fresh Date",
                            type: "date",
                        },
                        {
                            data: "due_date",
                            title: "Due Date",
                            type: "date",
                        }
         
EX;
        print($dt->start($this->pageid, 'showCowsInSpecificLocation', $location, $columnsStr, null));
    }

}

//end class
?>