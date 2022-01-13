<?php 



/**
 *  Displays recipes with added NRC 2001 info
 * 
 */
class AddicanReport extends BasePage {

    public function defaultDisplay() {
    
        
   
        
     
        
        $sql = "Select *,(SELECT (sum(kg_day_dry)) FROM nutrition.nrc_recipe_item  c WHERE c.nrc_recipe=nrc_recipe_param.nrc_recipe)  as total_dm  from nutrition.nrc_recipe_param WHERE status='active' ORDER BY total_dm DESC";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() == 0) {
            print((new BootStrap)->warningNotify("Warning: No Currently Active Recipes"));
        }
        print("<div id='tmrAllRecipeWarapper'>");
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            print($this->displayRecipeWrapper($row['nrc_recipe'],$row['update_time']));
        }
        print("</div>");
    }

    function displayRecipeWrapper($nrc_recipe_id,$nrc_recipe_update_time) {
        
     
      $nrc=new Nrc2001();
          
    
            $ret=$nrc->runStandardModel_UserAnimalInputs($nrc_recipe_id);
       
                 $html = <<<HTML
  <div class="row">
  <div class="col-md-3">{$nrc->displayRecipeCost($nrc_recipe_id)}</div>
                         <div class="col-md-3">{$nrc->displayActualCurrentMilkProductionByGroupStats($nrc_recipe_id)}</div>
                         <div class="col-md-3">{$nrc->displayValactaMilkProductionByGroupStats($nrc_recipe_id)}</div>
                           <div class="col-md-3">{$nrc->addicanBalanceParamaters($ret['Input'], $ret['Output'])}</div>
                           <div class="col-md-3">{$this->currentlyBeingUsedSamples()}</div>
</div>
HTML;
          
           
        print((new TmrRecipe)->displayRecipeWrapper($nrc_recipe_id,$nrc_recipe_update_time,$html));
        
        
       
    }

    public function currentlyBeingUsedSamples() 
    {
                         $sql = <<<SQL
with temp as(
SELECT nutrition.bag_closest_analysis_id (id,footage,direction) FROM nutrition.bag_current_open
) select id,sample_number,info_template	 from  temp LEFT JOIN nutrition.bag_analysis ON id = temp.bag_closest_analysis_id
SQL;
        $res = $GLOBALS['pdo']->query($sql);
       
       $out[]= '<ul>';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
         $out[]= '<li>Sample #: '.$row['sample_number'].'</li>';
            
        }                   
         $out[]= '</ul>';
      return  (new BootStrap)->plainCard('Current Forage Tests', implode($out));
           
    }
   

  
}

//end class
?>      