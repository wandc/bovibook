<?php 

/* shows how TMR recipes change through time */
;

class TmrHistorical extends TabPage {

    public function defaultDisplay() {
        //setup for tabs.
        $tabArray[1]['name'] = 'TMR Log';
        $tabArray[2]['name'] = 'Historical TMR Cost Charts';
        $tabArray[3]['name'] = 'OLD FIXME';
        $tabArray[4]['name'] = 'Low';
        $tabArray[5]['name'] = 'High';
        $tabArray[6]['name'] = 'Heifer';
        $tabArray[7]['name'] = 'Dry';
        $this->renderTabs($tabArray);
    }

    
     function tab1() {
                
     $this->displayNrcArchive(null);

    }
    
    function tab2() {
      
          print("<h3>Cost for location High</h3>");
         $this->mixCostsChartNew(40);
            print("<h3>Cost for location Low</h3>");
         $this->mixCostsChartNew(41);
            print("<h3>Cost for location Heifer</h3>");
         $this->mixCostsChartNew(42);
        
  
    }

    function tab3() {
        $this->milkChart(41);
        $this->milkChart(40);
        $this->milkChart(42);
    }

    function tab4() {
        $location_id = 41;
        $this->displayNrcArchive($location_id);
    }

    function tab5() {
        $location_id = 40;
        $this->displayNrcArchive($location_id);
    }

    function tab6() {
        $location_id = 42;
        $this->displayNrcArchive($location_id);
    }

    function tab7() {
        $location_id = 37;
        $this->displayNrcArchive($location_id);
    }

    
    
    public function mixCostsChartNew($location_id) {
     

         /* get recipe for high group. */
        $sql="SELECT nrc_recipe_id,title,	recipe_create_time, recipe->0->>'location_id_arr' as location_id_arr
,recipe->0->>'kg_day_cost_sum' as kg_day_cost_sum
,recipe->0->>'kg_day_dry_sum' as kg_day_dry_sum
FROM nutrition.nrc_archive 
LEFT JOIN nutrition.nrc_recipe_param ON nrc_recipe_param.nrc_recipe=nrc_archive.nrc_recipe_id 
where recipe->0->>'location_id_arr' like '%$location_id%'
ORDER BY recipe_create_time DESC";
        

        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
         
   
            $one[strtotime($row['recipe_create_time'])] = $row['kg_day_cost_sum'];
            $two[strtotime($row['recipe_create_time'])] = $row['kg_day_dry_sum'];
        }

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['kg_day_cost_sum'] = 'number';
        $headerStrArr['kg_day_dry_sum'] = 'number';
        $x = new GoogleVisualizationLine(null, $headerStrArr, $one, $two);
        print($x->toString());
    }
    
    
    
    public function test() {
        
        
       
        
        
    }
    
    
    public function nrcArchive($request) {
        if (isset($request['item'])) {
       if(filter_var($request['item'], FILTER_VALIDATE_INT) === 0 || filter_var($request['item'], FILTER_VALIDATE_INT)) { //this allows zero to be a valid int, not the default behaviour.
           $location_id=$request['item'];
       }
       else {
          $location_id=null; 
        }}else {
             $location_id=null; 
       }
        
        //show all
        if (empty($location_id)) {        
        $sql="SELECT nrc_recipe_id,title,	recipe_create_time, recipe 
FROM nutrition.nrc_archive 
LEFT JOIN nutrition.nrc_recipe_param ON nrc_recipe_param.nrc_recipe=nrc_archive.nrc_recipe_id 
ORDER BY recipe_create_time DESC";       
        }
        //specific location
        else {
         $sql="SELECT nrc_recipe_id,title,	recipe_create_time,recipe
FROM nutrition.nrc_archive 
LEFT JOIN nutrition.nrc_recipe_param ON nrc_recipe_param.nrc_recipe=nrc_archive.nrc_recipe_id 
where recipe->0->>'location_id_arr' like '%$location_id%'
ORDER BY recipe_create_time DESC";    
            
        }
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
        //    var_dump(json_decode($row['recipe']));
       //  $row['recipe_clean']= (json_decode($row['recipe']));
          //iterate over the json
         
         $recipeObj=json_decode($row['recipe']);
          
          $row['cost_per_day']=$recipeObj[0]->kg_day_cost_sum;
         
         $jsonOut=array();
         foreach ($recipeObj as $key2 => $value2) {
           $jsonOut[]=$value2->feed_type.' '.$value2->kg_day_dry.' (dry kg)'.   '<br>';           
          }
         $row['recipe_clean']=implode($jsonOut);
         
         
            $row['recipe']=null; //save bandwidth.
        }

      
        //output
        $output['data'] = $results;
        return ($output);
    }
        
        
       public function displayNrcArchive($location_id=null) {
        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
                        
            {
                data: "nrc_recipe_id",
                title: "nrc_recipe_id"
              
            },
            {
                data: "title",
                title: "Recipe Title"
              
            },
            
            {
                data: "recipe_create_time",
                title: "recipe_create_time",
                type: "date"
            },
            {
                data: "recipe_clean",
                title: "recipe"
        },
           {
                data: "cost_per_day",
                title: "cost_per_day"
        }
                
                
                
           
EX;
        print("<h3>Recipe Log Through Time</h3>");
        print($dt->start(92, 'nrcArchive', $location_id, $columnsStr));
    }
    
    
    
    public function milkChart($location_id) {
       

        $sql = "
With temp as (
SELECT mix.location_id,location.name as location_name,mix.date as mix_date,mix.id as mix_id,recipe_id,comment,nutrition.mix_cost(mix.id),
 valacta_group_averages.avg_fat_per,valacta_group_averages.avg_prot_per,alpro_group_production.avg_milk_production,1.033*.01*avg_fat_per*avg_milk_production as bf_kg_cow,1.033*avg_fat_per as cor_avg_fat_per
FROM bovinemanagement.location 
LEFT JOIN nutrition.mix ON mix.location_id = location.id
LEFT JOIN nutrition.recipe ON mix.recipe_id=recipe.id
LEFT JOIN batch.valacta_group_averages ON mix.date::date=valacta_group_averages.event_time::date
LEFT JOIN batch.alpro_group_production ON mix.date::date=alpro_group_production.event_time::date
WHERE mix.location_id=$location_id AND valacta_group_averages.location_id=$location_id AND alpro_group_production.location_id=$location_id AND mix.date > (current_date - interval '2 year')
 ORDER BY mix.date LIMIT 100
)
 SELECT temp.*,(Select avg(avg_milk_production) FROM temp)  as avg_avg_milk_production,(Select avg(bf_kg_cow) FROM temp)  as avg_bf_kg_cow FROM temp;
";

        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            //since we are missing some alpro data, check if it is 0 and if so, replace with group averages.
            if ($row['avg_milk_production'] == 0) {
                $row['avg_milk_production'] = $row['avg_avg_milk_production'];
            }


            $location_name = $row['location_name'];


            $one[strtotime($row['mix_date'])] = $row['avg_milk_production'];
            $two[strtotime($row['mix_date'])] = $row['cor_avg_fat_per'];
        }

        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['avg_milk_production'] = 'number';
        $headerStrArr['cor_avg_fat_per'] = 'number';
        $x = new GoogleVisualizationLine(null, $headerStrArr, $one, $two);
        print("<h3>Milk for $location_name</h3>");
        print($x->toString());
    }

//end function 

   

   

    function printRecipeItems($params) {
        extract($params);

        $sql = "
	    WITH tt AS (
SELECT feedcurr_id,recipe_id,recipe_item.id as recipe_item_id,date,kg_day_dry,round_to_tens,fed_free_choice,comment as recipe_name,active,editable,comment2 as comment
,(SELECT cost FROM nutrition.ingredient_info(recipe_item.feedcurr_id,recipe.date)) as cost
,(SELECT dry_matter_percent FROM nutrition.ingredient_info(recipe_item.feedcurr_id,recipe.date)) as dry_matter_percent
,(SELECT type FROM nutrition.ingredient_info(recipe_item.feedcurr_id,recipe.date)) as type
,(SELECT comment FROM nutrition.ingredient_info(recipe_item.feedcurr_id,recipe.date)) as comment
,(SELECT location FROM nutrition.ingredient_info(recipe_item.feedcurr_id,recipe.date)) as location
FROM nutrition.recipe_item 
LEFT JOIN nutrition.recipe ON recipe_item.recipe_id=recipe.id
WHERE recipe_id={$record['recipe_id']}
)
SELECT  *,tt.cost*.001*kg_day_dry as item_cost,kg_day_dry*(1/(dry_matter_percent*.01)) as as_fed_kg  FROM tt
";



        $res = $GLOBALS['pdo']->query($sql);
        $str = "";
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['item_cost'] != 0) {
                $costPercentage = round(($row['item_cost'] / $record['mix_cost']) * 100, 1);
            } else {
                $costPercentage = 0;
            }
            $str = $str . $row['kg_day_dry'] . " kg " . $row['type'] . " - " . " ({$row['location']})" . " - cost: $" . $row['cost'] . "/t $" . trim(money_format('%#10n', $row['item_cost'])) . " / $costPercentage%" . "<br>";
        }
        return $str;
    }

}

//end class
