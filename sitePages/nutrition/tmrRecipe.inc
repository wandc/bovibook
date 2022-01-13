<?php 


;

/**
 *  Displays Active resipes created with NRC class.
 * 
 */
class TMRRecipe extends BasePage {

    public function defaultDisplay() {
        
        
        $out = array();

        $build = new Doors();
        $buttons = <<<HTML
           
           <div class="row">
  <div class="col-sm-3">{$build->showDoorButton(3)}</div>
  <div class="col-sm-3">{$build->showDoorButton(4)}</div>
</div>
           <div class="row">
  <div class="col-sm-3">{$build->showDoorButton(2)}</div>
  <div class="col-sm-3">{$build->showDoorButton(1)}</div>
</div>       
           
           
       
HTML;

        //so nutritionist cant open doors.
        if ((in_array('employee', $GLOBALS ['auth']->getAuthData('groups')) == TRUE)) {
            $out[] = ($buttons);
        }



      

        $seasonalArr = array();
        $dateTime = new DateTime();
        $dayOfTheYear = $dateTime->format('z');
        if (($dayOfTheYear > 150) AND ($dayOfTheYear < 274)) {
            //june 1 to Oct 1    
            $seasonalArr[] = (new BootStrap)->infoNotify('Milking crew is resposible for feeding Quad and HO-1 pasture cows hay and mineral');
            $seasonalArr[] = (new BootStrap)->infoNotify('99 Dry is fed every day');
        } else {
            //non pasture time (winter)    
            $seasonalArr[] = (new BootStrap)->infoNotify('Gray Barn CL-10, is only fed every 2nd day on milk truck days');
            $seasonalArr[] = (new BootStrap)->infoNotify('99 Dry/Maternity, is only fed every opposite 2nd day');         
        }

        $seasonalArr[] = (new BootStrap)->infoNotify('All weigh-back gets fed to only CL-10 or HO-1 the same day.');
        
          $out[] =(implode($seasonalArr));



        $sql = "Select *,(SELECT (sum(kg_day_dry)) FROM nutrition.nrc_recipe_item  c WHERE c.nrc_recipe=nrc_recipe_param.nrc_recipe)  as total_dm  from nutrition.nrc_recipe_param WHERE status='active' ORDER BY total_dm DESC";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() == 0) {
              $out[] =((new BootStrap)->warningNotify("Warning: No Currently Active Recipes"));
        }
          $out[] =("<div id='tmrAllRecipeWarapper'>");
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            print($this->displayRecipeWrapper($row['nrc_recipe'],$row['update_time']));
        }
          $out[] =("</div>");
          
          //output final page. 
          print(implode($out));
    }

    function displayRecipeWrapper($nrc_recipe_id,$nrc_recipe_update_time,$strBottom='') {
       
       $out[]= "<div id='tmrIndivRecipeWarapper'>";

       $locArr=$this->locationInfo($nrc_recipe_id,$nrc_recipe_update_time,null);
       $out[]= $locArr['title'];
       $out[]=  $this->displayRecipeDataTable($nrc_recipe_id);       
       $out[]= implode($locArr['amountToFeed']);
       $out[]= $strBottom;
       $out[]=  "</div>";
       $out[]=  ('<hr>');
        return implode($out);
    }

    /*
     * title. how much to feed to each group, etc.
     * 
     */
    public function locationInfo($nrc_recipe_id,$nrc_recipe_update_time,$results) {
        
         /* recipe wet info */
        $results = $this->recipeQuery($nrc_recipe_id);
        $recipe_kg_day_wet_sum = $results[0]['kg_day_wet_sum'];
        $total_total_kg_day_wet_sum = $results[0]['kg_day_wet_sum'] * $results[0]['modified_mix_total_count'];
        
         /* location info */
        $sql = "Select *,(bovinemanagement.location_total_at_timestamp(location_id,now()::timestamp)) as location_count, (bovinemanagement.location_total_at_timestamp(location_id,now()::timestamp)*modifier) as location_mod_count from nutrition.nrc_recipe_location LEFT JOIN bovinemanagement.location ON location.id=nrc_recipe_location.location_id WHERE nrc_recipe=$nrc_recipe_id";
        $res = $GLOBALS['pdo']->query($sql);

        $locationNames = '';
        $counter=0;
        while ($row3 = $res->fetch(PDO::FETCH_ASSOC)) {
            $locationNames = $locationNames . $row3['name'] . " & ";   //summing string of location names.

            $mix_by_location_kg_day_wet_sum = $row3['location_mod_count'] * $recipe_kg_day_wet_sum;

          
           $ret['amountToFeed'][$counter]= ("<p class='lead'>Feed <b>" . round($mix_by_location_kg_day_wet_sum) . "</b> / " . round($total_total_kg_day_wet_sum) . " kg to {$row3['location_count']} animals in {$row3['name']}." . "</p>");
           $counter++;
        }
        $ret['locationNames'] = rtrim($locationNames, '& '); //erase last &.
        $ret['title']=  "<h2> {$ret['locationNames']}  <small>(Recipe # $nrc_recipe_id) <small style='float:right'>Last Update: ". date('l jS \of M H:i:s',strtotime($nrc_recipe_update_time)) ."</small></small></h2>";
        
      return  $ret; 
    }

    /** return a specific recipe as json */
    //outer
    function recipe($request) {
        $nrc_recipe_id = Sanit::reqVal('item', false, 'INT');

        $results = $this->recipeQuery($nrc_recipe_id);

        //output
        $output['data'] = $results;
        return ($output);
    }

    //inner
    function recipeQuery($nrc_recipe_id) {


        $sql="SELECT * from nutrition.nrc_recipe_display($nrc_recipe_id)";
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);         //get data
        //  print_r($results);
        //change presentation
        foreach ($results as &$row) {
            
            $bagreverse='';
            if ($row['reverse']==1) {
                $bagreverse=' (reverse)';
            }
            $row['location']=$row['location'].$bagreverse;
            
            if (($row['kg_day_wet_mix']) <= 50) {
                $row['kg_day_wet_mix'] = round($row['kg_day_wet_mix'], 1);
            } else {
                $row['kg_day_wet_mix'] = round($row['kg_day_wet_mix']);
            }
            //
        }
        return $results;
    }

    function displayRecipeDataTable($nrc_recipe_id) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
            {
                data: "kg_day_wet_mix",
                title: "Load Amount (kg)",
                  "visible": false       
            },
            {
                data: "kg_day_wet_mix",
                title: "Load Amount (kg)",
                          "visible": false
            },    
            {
                data: "feed_type",
                title: "Feed Name"
            },
            {
                data: "location",
                title: "Feed Loc"
            },
            {
                data: "kg_day_dry",
                title: "DM kg/cow/day"
            },
            {
                data: "moisture_percent",
                title: "Moisture %"
            },
     
            {
                data: "kg_day_wet_mix",
                title: "Load Amount (kg)"
            }
EX;
        return($dt->startCustom('Title Custom CHANGE ME', 139, 'recipe', $nrc_recipe_id, $columnsStr));
    }

}

//end class
?>      