<?php 


;

/**
 *  Displays Active resipes created with NRC class.
 * 
 */
class TMRRecipe extends BasePage {

    public function defaultDisplay() {
  require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/misc/building.inc');
        
        $build=new Building();
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
   
print($buttons);
   

        
        
         print('<div id="buttonDiv">');
    
         print('</div>');

        
        //print("<h1>Milking Crew is resposible for feeding Quad and HO-1 Pasture cows</h1>");
          print("<h1>Gray Barn CL-10, is only fed every 2nd day on milk truck days</h1>");
           print("<h1>99 Dry/Maternity, is only fed every opposite 2nd day</h1>");
        
        $sql = "Select *,(SELECT (sum(kg_day_dry)) FROM nutrition.nrc_recipe_item  c WHERE c.nrc_recipe=nrc_recipe_param.nrc_recipe)  as total_dm  from nutrition.nrc_recipe_param WHERE status='active' ORDER BY total_dm DESC";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() == 0) {
            print(BootStrap::warningNotify("Warning: No Currently Active Recipes"));
        }
        print("<div id='tmrAllRecipeWarapper'>");
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            print($this->displayRecipeWrapper($row['nrc_recipe'],$row['update_time']));
        }
        print("</div>");
    }

    function displayRecipeWrapper($nrc_recipe_id,$nrc_recipe_update_time) {
        $str = '';
        $str2 = '';
        $str = $str . "<div id='tmrIndivRecipeWarapper'>";

        $results = $this->recipeQuery($nrc_recipe_id);


        //$recipe_kg_day_wet_sum=-1;
        // $total_total_kg_day_wet_sum =-1;
        $recipe_kg_day_wet_sum = $results[0]['kg_day_wet_sum'];
        $total_total_kg_day_wet_sum = $results[0]['kg_day_wet_sum'] * $results[0]['modified_mix_total_count'];

        $sql = "Select *,(bovinemanagement.location_total_at_timestamp(location_id,now()::timestamp)) as location_count, (bovinemanagement.location_total_at_timestamp(location_id,now()::timestamp)*modifier) as location_mod_count from nutrition.nrc_recipe_location LEFT JOIN bovinemanagement.location ON location.id=nrc_recipe_location.location_id WHERE nrc_recipe=$nrc_recipe_id";
        $res = $GLOBALS['pdo']->query($sql);

        $locationNames = '';
        while ($row3 = $res->fetch(PDO::FETCH_ASSOC)) {
            $locationNames = $locationNames . $row3['name'] . " & ";   //summing string of location names.

            $mix_by_location_kg_day_wet_sum = $row3['location_mod_count'] * $recipe_kg_day_wet_sum;

            $recipeDMTotal = 5;
            $str2 = $str2 . ("<p class='lead'>Feed <b>" . round($mix_by_location_kg_day_wet_sum) . "</b> / " . round($total_total_kg_day_wet_sum) . " kg to {$row3['location_count']} animals in {$row3['name']}." . "</p>");
        }
        $locationNames = rtrim($locationNames, '& '); //erase last &.


        $str = $str . "<h2>$locationNames <small>(Recipe # $nrc_recipe_id) <small style='float:right'>Last Update: ". date('l jS \of M H:i:s',strtotime($nrc_recipe_update_time)) ."</small></small></h2>";
        //
        $str = $str . $this->displayRecipeDataTable($nrc_recipe_id);
        //
        $str = $str . $str2;


        $str = $str . "</div>";
        $str = $str . ('<hr>');
        return $str;
    }

    public function defaultDisplayTEST() {
        ?>

        <script type="text/javascript">
            $(document).ready(function () {
                function beep() {
                    //  var snd = new Audio("data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");  
                    var typeWriter = new Audio("https://www.freesound.org//data/previews/237/237992_2993669-lq.mp3");
                    //   snd.play();
                    typeWriter.play();
                }
                beep();
                //new Audio('data:audio/wav;base64,UklGRl9vT19XQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YU'+Array(1e3).join(123)).play();
            });




        </script>

        <ul class="nav nav-tabs">
            <li class=""><a href="#home" data-toggle="tab" aria-expanded="true">High Group</a></li>
            <li class="active"><a href="#profile" data-toggle="tab" aria-expanded="true">Low Group</a></li>
            <li class="active"><a href="#profile2" data-toggle="tab" aria-expanded="true">Sick Group</a></li>


        </ul>
        <div id="myTabContent" class="tab-content">
            <div class="tab-pane fade" id="home">
        <?php  print($this->displayRecipe(5015)); ?>
            </div>
            <div class="tab-pane fade active in" id="profile">
        <?php  print($this->displayRecipe(5020)); ?>
            </div>
            <div class="tab-pane fade active in" id="profile2">
        <?php  print($this->displayRecipe(5022)); ?>
            </div>
        </div>

        <?php 
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
        return($dt->startCustom(139, 'recipe', $nrc_recipe_id, $columnsStr));
    }

}

//end class
?>      