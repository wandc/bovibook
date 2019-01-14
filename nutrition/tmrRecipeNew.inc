<?php




require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
;

class TmrRecipeNew extends BasePage {

    public function defaultDisplay() {



        print($this->displayCurrentlyInFeederHTML());
        $this->displayCurrentlyInFeeder();


        $this->renderTabs();
    }

    function renderTabs() {


        print('<div id="jquery_tabs" class="flora"><ul>');


        //print titles for tab labels.
        $res = $GLOBALS['pdo']->query("SELECT distinct nrc_recipe,title,kg_day_dry_sum FROM nutrition.nrc_recipe_active_staging WHERE status='active' order by kg_day_dry_sum desc");
        //$counter=1;
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("	<li><a title=\"x_{$row['nrc_recipe']}\" href=\"#x_{$row['nrc_recipe']}\"><span>{$row['title']}</span></a></li>" . "\n\r");
            // $counter++;
        }
        print('</ul>');


        //print content in tabs


        $res = $GLOBALS['pdo']->query("SELECT distinct nrc_recipe,title,kg_day_dry_sum , update_time FROM nutrition.nrc_recipe_active_staging WHERE status='active' order by kg_day_dry_sum desc");
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("<div id='x_{$row['nrc_recipe']}'>" . "\n\r");
            //TAB CONTENT
            print($this->displayRecipeWrapper($row['nrc_recipe'], $row['update_time']));
            ?>
            <script>

            </script>
            <?php

            ///
            print('</div>' . "\n\r");
        }

        print('</div> <!--end of tabs div -->');
    }

    private function displayCurrentlyInFeederHTML() {
        $HTML = <<<HTML

        
         <div class="row">
    <div class="col-sm">
    <span id="tmrLiveMass"></span>
    </div>
    <div class="col-sm">
      <span id="tmrItemMass"></span> 
    </div>
      </div>  
        <div id = "alert_placeholder"></div>
          
HTML;

        return $HTML;
    }

    //who is currently in the feeder.
    function displayCurrentlyInFeeder() {
        ?> 

        <script>
            $(document).ready(function() {
            (function worker() {

           
           $.ajax({
            url: 'https://int.littleriver.ca/api/json/146/currentTMRMass',
                    dataType : "html",
                    success: function(data) {
                    var objData = JSON.parse(data);
                    //erase the old data
                    $("#tmrLiveMass").html('');
                    var tmrLiveMass = null;
                    /* loop though array */
                    $.each(objData.data, function(index, value) {
                    /* write local number to div a for correct pen*/
                    if (value.display_weight.length == 0) {
                    $("#tmrLiveMass").html('N/A');
                    }
                    else {
                    $("#tmrLiveMass").html(value.display_weight + ' kg');
                    tmrLiveMass = parseFloat(value.display_weight);
                    }
                    });
                    //if we have selelected row show info from it at top of page
                    if (NS.selectedRowObj != null) {
                    //DEBUG                 
                    //console.log(tmrLiveMass);
                    //console.log(NS.selectedRowObj);
                    difference = (tmrLiveMass) - parseFloat(NS.selectedRowObj.start_mass);
                    $("#tmrItemMass").html((difference) + ' / ' + NS.selectedRowObj.kg_day_wet_mix + ' kg ' + NS.selectedRowObj.feed_type);
                    //if we are getting close to the value play a sound. should be continous tone if past it.
                    if (difference / parseFloat(NS.selectedRowObj.kg_day_wet_mix) >= 0.98) {
                    NS.snd1.play(); //beep 
                    } else if (difference / parseFloat(NS.selectedRowObj.kg_day_wet_mix) >= 1.0) {
                    NS.snd2.play(); //tone
                    }

                    } else {
                    $("#tmrItemMass").html('');
                    }

                    },
                    complete: function() {
        // Schedule the next request when the current one's complete
                    setTimeout(worker, 1000);
                    }
            });
            })();
            });

        </script>
        <?php

    }

    //used for 2 second updates
    function currentTMRMass() {
        $sql = "
           SELECT event_time ,display_weight from bas.tmr_event WHERE event_time >( localtimestamp - interval '5 seconds') ORDER BY event_time DESC limit 1
            ";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //output
        $output['data'] = $results;
        return ($output);
    }

    function customJavascript() {

        require_once($_SERVER['DOCUMENT_ROOT'] . 'template/tabPage.php');
        TabPage::customJavascript();
    }

    function displayRecipeWrapper($nrc_recipe_id, $nrc_recipe_update_time) {
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
        //$str = $str . "<h2>$locationNames <small>(Recipe # $nrc_recipe_id) <small style='float:right'>Last Update: ". date('l jS \of M H:i:s',strtotime($nrc_recipe_update_time)) ."</small></small></h2>";
        //
        $obj = $this->displayRecipeDataTable($nrc_recipe_id);

        $str = $str . $obj->str;
        //cutom javascript for select rows
        $str = $str . $this->recipeDataTableJavascript($obj->tableName);

        //bottom stuff
        $str = $str . $str2;


        $str = $str . "</div>";
        $str = $str . ('<hr>');
        return $str;
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

        $sql = "
 with temp as (
SELECT feedcurr_id,nrc_recipe,feed_library_name, location,feed_type,kg_day_dry,round(100-dry_matter_percent,1) as moisture_percent,
kg_day_dry*1/(.01* dry_matter_percent) as kg_day_wet,kg_day_dry*.001*cost as kg_day_cost,strpos(feedcurr_id, 'bag_r') as reverse, nrc_recipe_item.id as nrc_recipe_item_id,start_mass,end_mass,(end_mass-start_mass) as diff_mass,


(array(SELECT(bovinemanagement.location_total_at_timestamp (location_id,now()::timestamp)*modifier) as location_mod_count  FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$nrc_recipe_id   ORDER BY location_id ))  as location_mod_count_arr,

array(SELECT location_id FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$nrc_recipe_id  ORDER BY location_id) as location_id_arr

FROM nutrition.nrc_recipe_item 
LEFT JOIN nutrition.nrc_recipe_item_feed_log ON nrc_recipe_item.id=nrc_recipe_item_feed_log.nrc_recipe_item_id
LEFT JOIN nutrition.feedcurr ON nrc_recipe_item.feedcurr_id=feedcurr.id
WHERE nrc_recipe=$nrc_recipe_id
), temp2 as
( SELECT *, (SELECT sum(kg_day_dry) from temp group by nrc_recipe) as kg_day_dry_sum,(SELECT sum(kg_day_wet) from temp group by nrc_recipe) as kg_day_wet_sum,(SELECT sum(kg_day_cost) from temp group by nrc_recipe) as kg_day_cost_sum,(SELECT SUM(s) FROM UNNEST(location_mod_count_arr) s) as modified_mix_total_count
FROM temp)
SELECT *,modified_mix_total_count*kg_day_wet as kg_day_wet_mix FROM temp2 ORDER BY kg_day_dry ASC
              ";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);         //get data
        //print_r($results);
        //change presentation
        foreach ($results as &$row) {

            $row['kg_day_wet_mix_actual'] = $row['diff_mass'];

            $bagreverse = '';
            if ($row['reverse'] == 1) {
                $bagreverse = ' (reverse)';
            }
            $row['location'] = $row['location'] . $bagreverse;

            if (($row['kg_day_wet_mix']) <= 50) {
                $row['kg_day_wet_mix'] = round($row['kg_day_wet_mix'], 1);
            } else {
                $row['kg_day_wet_mix'] = round($row['kg_day_wet_mix']);
            }
            //
            if (empty($row['kg_day_wet_mix_actual'])) {
                $row['colour_row'] = 0;
            } else {
                $row['colour_row'] = 1;
            }
            
        }
        return $results;
    }

    function displayRecipeDataTable($nrc_recipe_id) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                {
                 data: "kg_day_wet_mix_actual",
                title: "xxxx",
                  "visible": false 
            },
            {
                data: "kg_day_wet_mix",
                title: "Load Amount (kg)",
                  "visible": false       
            },
            {
                data: "nrc_recipe_item_id",
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
                data: "kg_day_wet_mix",
                title: "Required Amount (kg)"
            },
                {
                 data: "kg_day_wet_mix_actual",
                title: "Actual Amount (kg)"
            }
EX;
        return($dt->startCustomSelect(146, 'recipe', $nrc_recipe_id, $columnsStr, 'highlight_okay', null, false));
    }

    private function recipeDataTableJavascript($dataTableName) {

        $str = $str . <<<jss1
      <script>
            //when row is selected.           
 $(document).ready(function() {
                
                /*dynamic bootstrap error */
                bootstrap_alert = function() {}
bootstrap_alert.warning = function(message) {
            $('#alert_placeholder').html('<div class="alert alert-danger alert-dismissable"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button><span>'+message+'</span></div>')
        }
                
       
   NS.selectedRowObj=null; //global to hold what row was last selected.
   NS.snd1=null;
   NS.snd2=null;
                
 NS.{$dataTableName}.on( 'select', function ( e, dt, type, indexes ) {
    if ( type === 'row' ) {
        var nrc_recipe_item_idSelected = NS.{$dataTableName}.rows( indexes ).data().pluck( 'nrc_recipe_item_id' )[0];

         //beep sound files
         //IOS needs to have the audo files loaded by a user click event. You can play at anytime, but loading needs a click.
         NS.snd1 = new Audio("data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");
         NS.snd2 = new Audio('data:audio/wav;base64,UklGRl9vT19XQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YU'+Array(1e3).join(123)); 
        
        //when row is selected store all the data into a global object. We can then use that info to do calculations.
        //console.log(nrc_recipe_item_idSelected);
        //console.log( $("#tmrLiveMass").text());
        var data= NS.{$dataTableName}.rows( { selected: true }).data();
        //console.log("data---"+data.toString());  
        //console.log(JSON.stringify(data[0]));
       
        
        /* check TMR live weight is not blank */
        if ($("#tmrLiveMass").text()!='') {

         /* store in slected row global */
        data[0].start_mass=$("#tmrLiveMass").text().slice(0, -3);
        NS.selectedRowObj=data[0]; //store in global.
        }else {
          console.log('No TMR Live Mass Data');
          bootstrap_alert.warning('No Live TMR weight, go into wifi range or if within range turn digi-star scale off/3 seconds/on to fix.');
        //do not allow the row to be selected, because we have no live weight.  this will fire the deselect event, so we need to always check that we have a valid live mass in it too.
          NS.{$dataTableName}.rows('.selected').deselect();
        }/*end if */   
    }     
} );     
    } );
         
         //when row is deselcted
         
          $(document).ready(function() {
 NS.{$dataTableName}.on( 'deselect', function ( e, dt, type, indexes ) {
    if ( type === 'row' ) {
        var nrc_recipe_item_idSelected = NS.{$dataTableName}.rows( indexes ).data().pluck( 'nrc_recipe_item_id' )[0];

        console.log("deselcted");
        
        //if a row is deselcted and global selcted row object is null, then we we didn't have live data and nothing to save
        //when we have something, grab the live mass and save in the DB as end mass-start mass.
          if (NS.selectedRowObj!=null) {
              console.log(JSON.stringify(NS.selectedRowObj));
        
              if ($("#tmrLiveMass").text()!='') {
         console.log("deselcted and live weight.");
                //get live mass
                var liveMass=$("#tmrLiveMass").text().slice(0, -3);
        
                /* insert into DB. */     
                var obj = { "packet":"tmr_mixer_recipe_item_feed_log", "start_mass": NS.selectedRowObj.start_mass , "end_mass":liveMass, "userid":'david', "nrc_recipe_item_id":   nrc_recipe_item_idSelected.toString()};
                var myJSON = JSON.stringify(obj);
                console.log(myJSON); //debug
                //
                $.ajax({
                url: ' https://int.littleriver.ca/put.php',
                type: 'PUT',
                data: '['+myJSON+']',
                success: function(data) {
                console.log('insert/update db was performed.');
            }
            });
        
              // reload datatable showing fed data.
              NS.{$dataTableName}.ajax.reload();
        
              NS.selectedRowObj=null; //erase global
              
        
        } else {
        
        console.log('cannot save because we have no live weight.');
        //if we don't null global here, they can try saving again.
        }
        
        
        } else {
         console.log('selectedRowObj is null.OK.');
        }
        
     
        
        
    }     
} );     
             } );
         
            </script>
                  
jss1;



        return $str;
    }

}

//end class
?>

