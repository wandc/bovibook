<?php

;

class MovementLocationList extends TabPage {

    
     public function defaultDisplay() {
        $tabArray[1]['name'] = 'Lists';
        $tabArray[2]['name'] = 'Maps';
              
        $this->renderTabs($tabArray);
        
    }
    
    
     public function tab1() {
        
       
        print($this->locationSelectQuickFormJS());

        if (!empty($_REQUEST['location'])) {
            self::displayAnimalsByLocation(($_REQUEST['location']));
        }
    }
    
    public function tab2() {
        
          $this->locationMap();
        
       
    }

    
     private function oldFarmPoints($results) {
             foreach ($results as $key => $value) {
          
      $skip=false;
      
      switch ($value['location_id']) {
          case 57: //RV-10
            $top=490;
            $left=490;
            $colour='f39c12';  
              break;
          case 58:  //CL-10
            $top=120;
            $left=60;
            $colour='EF0BD8';  
              break;
           case 60:
            $top=400;
            $left=340;
            $colour='EF0BD8';  
              break;
           
          default:
              $skip=true;
              break;
      }
     if ($skip==false) {
      $icons[]="<span class='mapCirlces'  style='position:absolute;left:{$left}px;top:{$top}px;background: #{$colour}'>{$value['count']}</span>";
     }
    }
 
        return $icons;
    }
    
    private function milkyWayPoints($results) {
             foreach ($results as $key => $value) {
          
      $skip=false;
      
      switch ($value['location_id']) {
          case 37:
            $top=670;
            $left=1010;
            $colour='f39c12';  
              break;
           case 38:
            $top=620;
            $left=1090;
            $colour='f39c12';  
              break;
           case 39:
            $top=540;
            $left=1010;
            $colour='f39c12';  
              break;
           case 40:
            $top=450;
            $left=790;
            $colour='00a65a';  
              break;
           case 41:
            $top=570;
            $left=910;
            $colour='00a65a';  
              break;
          case 42:
            $top=380;
            $left=870;
            $colour='00a65a';  
              break;
           case 46:
            $top=810;
            $left=270;
            $colour='EF0BD8';  
              break;
          /* calf barn */
           case 68:
            $top=810;
            $left=0;
            $colour='EF0BD8';  
              break;
           case 67:
            $top=810;
            $left=40;
            $colour='EF0BD8';  
              break;
           case 66:
            $top=810;
            $left=80;
            $colour='EF0BD8';  
              break;
           case 65:
            $top=810;
            $left=120;
            $colour='EF0BD8';  
            /* HO Pasture */
           case 63:
            $top=200;
            $left=980;
            $colour='EF0BD8';  
              break;
          default:
              $skip=true;
              break;
      }
     if ($skip==false) {
      $icons[]="<span class='mapCirlces'  style='position:absolute;left:{$left}px;top:{$top}px;background: #{$colour}'>{$value['count']}</span>";
     }
    }
 
        return $icons;
    }
    
    function locationMap() {
        
        $sql = <<<SQL
SELECT distinct location_name ,location_id, count(id) FROM bovinemanagement.bovinecurr group by location_name,location_id order by location_id
SQL;
        
      
      $res = $GLOBALS['pdo']->query($sql);   
      $results = $res->fetchAll(PDO::FETCH_ASSOC);
     
      /* this is using hard coded relative to the map locations */ 
      /* its not worth the effort to write a lat/long mapping program for each group..... */
      
        $str='';
        $icons=$this->milkyWayPoints($results);
        $zoom=18;
        $centre='45.9815,-64.9947'; //new farm
        $out1[]=('<div style="position:relative;zoom: 0.5;" id="containerX">');
        $out1[]= ("<img id='locationMap' src='https://maps.googleapis.com/maps/api/staticmap?key={$GLOBALS['config']['GOOGLE_MAPS']['API_KEY']}&center=$centre&weight:2%7C$str&size=640x640&sensor=false&maptype=hybrid&zoom=$zoom&scale=2'/>");
        $out1[]=(implode($icons));
        $out1[]=('</div>');
         
        $centre='45.9845,-64.970'; //old farm
        $icons=$this->oldFarmPoints($results);
        $out2[]=('<div style="position:relative;zoom: 0.5;" id="containerY">');  
        $out2[]= ("<img id='locationMap' src='https://maps.googleapis.com/maps/api/staticmap?key={$GLOBALS['config']['GOOGLE_MAPS']['API_KEY']}&center=$centre&weight:2%7C$str&size=640x640&sensor=false&maptype=hybrid&zoom=$zoom&scale=2'/>");
        $out2[]=(implode($icons));
        $out2[]=('</div>');
     
          
          //ouput
         print("<div class='row'>");
            print("<div class='col-sm-6 col-sm-6'>");        
        print(implode($out2));
        print("</div>");
        print("<div class='col-sm-6 col-sm-6'>");
           print(implode($out1));
        print("</div>");
       
        print("</div>");
        
          
          
    }

    
    function locationSelectQuickFormJS() {


        //get active locations
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementIndividual.inc');
        $locations = QF2_MoveAnimal::listActiveLocationsAndCounts();
        $locations[9999999] = 'All Locations';
        $style = '';
        // custom select input
        // with javascript to allow submission on change of select element.
        $out[]= ("<select class='form-control' $style onchange=\"javascript:location.href='?pageid={$_REQUEST['pageid']}&amp;location='+this.options[this.selectedIndex].value;$('#example &gt; ul').tabs('select', 0);\" name=\"locationList\">");
        foreach ($locations as $key => $value) {
            $out[]= ("<option value=\"$key\">$value</option>" . "\n\r");
        }
        $out[]= ('</select>');
        
        $right=(new BootStrap)->plainCard('Location Select', implode($out));
        $left='';
        return  (new BootStrap)->oneThirdTwoThirds($left, $right);
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
            $row['due_date'] =  (empty($row['due_date']) ? null : date('M d, Y', strtotime($row['due_date'])));
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
        print($dt->start('Animals at a Location', $this->pageid, 'showCowsInSpecificLocation', $location, $columnsStr, null,true,' "iDisplayLength": 100,'));
    }

}

//end class
?>