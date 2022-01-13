<?php

//Test cases:
//$this->processNRC2001Output($this->testCase1());
// $this->processNRC2001Output($this->testCase2());

class NRC2001 extends BasePage {

    public $editrecipeid;
  
    public function __construct() {
        parent::__construct(); //pageid looked after here. 
        //optional recipe id

        if ((isset($_REQUEST['editrecipeid']) == false) || ($_REQUEST['editrecipeid'] == null)) {
            $_REQUEST['editrecipeid'] = null;
        }
        if (!filter_var($_REQUEST['editrecipeid'], FILTER_VALIDATE_INT) === false) {
            $this->editrecipeid = $_REQUEST['editrecipeid'];
        } else {
            $this->editrecipeid = null;
        }
              
    }

    
    //rounding function
          private function round($item) {
            if (is_numeric($item) == true) {
                return round($item, 1);
          }}
    
    private function bootstrapBox($header,$content,$footer=null) {
      $html = <<<HTML
   <div class="card card-primary">
            <div class="card-header with-border">
              <h3 class="card-title">$header</h3>
         </div>
            <!-- /.card-header -->
        <div class="card-body">
        $content
        </div>
        </div>
            <!-- /.card-body -->
HTML;

                return ($html);        
    }      
          
          
    public function defaultDisplay() {
      
        ?>
        <script>
            $(function () {
                $('[data-widget="popover"]').popover(); /* Enable popover. performace hit so need to run per page */
            })
        </script>
        <?php
        if (!empty($this->editrecipeid)) {

            $sql9 = "SELECT title FROM nutrition.nrc_recipe_param where nrc_recipe=$this->editrecipeid";
            $res9 = $GLOBALS['pdo']->query($sql9);
            $row9 = $res9->fetch((PDO::FETCH_ASSOC));
            $title = $row9['title'];

            //show EDIT screen
              print("<h2>Model Results: <span class='text-default'>$title (Recipe #{$this->editrecipeid})</span></h2><br/>");
                    print("<div class='row'>");
                   
                    print("<div class='col-sm-4 col-sm-push-8' >");
                    $this->showNRC2001Accordion($this->editrecipeid, $title);
                    print("</div>");
                    print("<div class='col-sm-8 col-sm-pull-4' >");

                    print("<div class='row'>");
                    print("<div class='col-sm-4 col-sm-push-8' >");
                    print($this->nrcButtons());
                   
                    print("</div>");
                    print("<div class='col-sm-8 col-sm-pull-4' >");
                    
                    //check for linked feed error.
                    print($this->checkReceipeForLinkedFeeds($this->editrecipeid));
        
                    print($this->displayRecipeCost($this->editrecipeid));
                    print($this->displayActualCurrentMilkProductionByGroupStats($this->editrecipeid));

                    print($this->displayValactaMilkProductionByGroupStats($this->editrecipeid));
                    
                    
                    print("</div>");
                    print("</div>");

                    $dataArr=$this->processNRC2001Output($this->runStandardModel_UserAnimalInputs($this->editrecipeid));
                    print($this->render($dataArr));
                    
                    print("</div>");
                    print("</div>");
                } else {
            //show GENERAL screen
            print($this->checkActiveRecipesForErrors($this->editrecipeid));
            print("<br/>");
            print("<div id='recipeGroupActive'>");
            print("<h1>Active</h1>");
            print($this->duplicateAllActiveAndArchiveOld());
                 print("<br/>");
            print($this->addLatestBagConsumptionForageTestToStagingRecipes());
              print("<br/>");
            print($this->moveAllActiveToStaging());
            print($this->recipeInfo('active'));
            print('</div>');

            //////////////////
            print("<br/>");

            print("<div id='recipeGroupStaging'>");
            print("<h1>Staging</h1>");
            print($this->recipeInfo('staging'));
            print('</div>');
        }
    }
  
     public function recipeLatestDryMattersTable($locationId,$days=7) {
       
      
        $sql = <<<SQL
SELECT nrc_recipe_id,title,recipe_create_time
,recipe, 
array(select location_id from nutrition.nrc_recipe_location where nrc_recipe=nrc_recipe_id) as location_arr, 
array(select modifier from nutrition.nrc_recipe_location where nrc_recipe=nrc_recipe_id)    as modifier_arr            
FROM nutrition.nrc_archive 
LEFT JOIN nutrition.nrc_recipe_param ON nrc_recipe_param.nrc_recipe=nrc_archive.nrc_recipe_id 

where recipe->0->>'location_id_arr' like '%$locationId%' AND recipe_create_time >= (current_date - interval '$days days') 
ORDER BY recipe_create_time ASC
SQL;
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
       $recipeObj=json_decode($row['recipe']);
          $rowOut['date']= (new JQueryDataTable)->formattedDate($row['recipe_create_time'], 'M d'); 
          $rowOut['kg_dry']=$recipeObj[0]->kg_day_dry_sum;
           //$rowOut['modified_mix_total_count']=$recipeObj[0]->modified_mix_total_count;
          $rowOut['modifier_arr']=$row['modifier_arr'];
          
         
          //$rowOut['mix_cost']=$recipeObj[0]->modified_mix_total_count * $recipeObj[0]->kg_day_cost_sum;
   

        $resultsOut[] = $rowOut; //output
        }
        
       // $out[] = ( (new JQueryDataTable)->startBasicSql('Latest Dry Matter Δ', $sql));
        $out[] =( (new JQueryDataTable)->startBasicArray('Latest Dry Matter Δ', $resultsOut));
        return implode($out);
    }
    
    
    private function bootstrapModal($title,$content) {
$html = <<<HTML
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-content">
        <div class="container">;
          <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
            <h3 class="modal-title" id="myModalLabel">$title</h3>
          </div>
              <div class="modal-body" >
              $content
              </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
          </div>
        </div>
    </div>                                      
</div>
HTML;

        
      return $html;  
    }
    
    
    private function nrcButtons() {
        $str='';
         $str=$str.'<button type="button" class="btn btn-primary" data-widget="modal" data-target="#myModal">How To Balance A Ration</button>';
         $str=$str. $this->bootstrapModal('How To Balance A Ration',$this->howToBalanceARation());
         //$str=$str.(' <button type="button" class="btn btn-default" data-container="body" data-widget="popover" data-placement="right" data-html="true" data-content="' . ($this->howToBalanceARation()) . '" data-original-title="" title="" aria-describedby="popover77277">How To Balance A Ration</button>');
                     $str=$str.("<br/><br/>");
                     $str=$str.($this->activateButton($this->editrecipeid));
                     $str=$str.("<br/>");
                     $str=$str.($this->deleteButton($this->editrecipeid));
        
        return $this->bootstrapBox('Buttons...',$str);
    }
    /*
     * show actual parlor milking stats for each group
     */

    public function displayActualCurrentMilkProductionByGroupStats($recipeID) {
        //look up what groups were are doing for this recipe.
        $sql = "SELECT location_id FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$recipeID";

         $str='';
        $res = $GLOBALS ['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $str=$str. MilkingStats::displayMilkingStatsByGroup($row['location_id'], 'nutrition');
        }
        return $this->bootstrapBox('Milking Stats',$str);
    }
    
    /*
     * 
     * show valcata stats for group
     */
     public function displayValactaMilkProductionByGroupStats($recipeID) {
        //look up what groups were are doing for this recipe.
        $sql = "SELECT location_id FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$recipeID";

         $str='';
        $res = $GLOBALS ['pdo']->query($sql);
         $str=$str.(new BootStrap)->infoNotify('NRC 2001 does not support palm fat, thus reduce model input group fat percentage by 0.2%.');
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $str=$str. MilkingStats::displayValactaStatsByGroup($row['location_id'], 'nutrition');
        }
        return $this->bootstrapBox('Valacta Stats',$str);
    }
    
    

    /*
     * calculates recipe sum cost.
     */

    private function calculateRecipeCost($recipeID) {
        $sql = "
with temp as (
SELECT concentrate, sum(kg_day_dry*.001*cost) OVER () as cost_sum,
sum(kg_day_dry*.001*cost) OVER (PARTITION BY feedcurr.concentrate) as cost_sum_concentrate,
sum(kg_day_dry) OVER (PARTITION BY feedcurr.concentrate) as kg_day_dry_concentrate,
count(feed_library_name) OVER () - count(feedcurr_id) OVER () as error_check
 FROM nutrition.nrc_recipe_item 
LEFT JOIN nutrition.feedcurr ON feedcurr_id=feedcurr.id
WHERE nrc_recipe=$recipeID
)
SELECT (SELECT error_check from temp limit 1),(SELECT cost_sum  from temp limit 1) as total_cost_sum,(SELECT cost_sum_concentrate  from temp WHERE concentrate=false limit 1) as cost_sum_forage,(SELECT kg_day_dry_concentrate  from temp WHERE concentrate=false limit 1) as kg_day_dry_forage,
(SELECT cost_sum_concentrate  from temp WHERE concentrate=true limit 1) as cost_sum_concentrate,(SELECT kg_day_dry_concentrate  from temp WHERE concentrate=true limit 1) as kg_day_dry_concentrate";

        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if (($row['error_check']) != 0) {
            return (new BootStrap)->errorNotify("Error: On Farm Feed Not Linked!");
        } else {
            //make return obj
            $retObj = new stdClass();
            $retObj->total_cost_sum = round($row['total_cost_sum'], 2);
            $retObj->cost_sum_concentrate = round($row['cost_sum_concentrate'], 2);
            $retObj->cost_sum_forage = round($row['cost_sum_forage'], 2);
            $retObj->kg_day_dry_concentrate = round($row['kg_day_dry_concentrate'], 2);
            $retObj->kg_day_dry_forage = round($row['kg_day_dry_forage'], 2);

            return $retObj;
        }
    }

    public function displayRecipeCost($recipeID) {
        $obj = $this->calculateRecipeCost($recipeID);
$str='';
        if (empty($obj->total_cost_sum)) {
            $str='' . '<span class="text-danger">Not calculated as some items do not have cost entered. Link all recipe inputs to on farm feeds.</span>';
        }

        $total_cost_sum = '<b>$' . $obj->total_cost_sum . '</b>';
        $cost_sum_concentrate = '$' . $obj->cost_sum_concentrate . '';
        $cost_sum_forage = '$' . $obj->cost_sum_forage . '';
        $total_percent_forage = $obj->kg_day_dry_forage/($obj->kg_day_dry_concentrate +$obj->kg_day_dry_forage);

        $str='' . '<ul><li>' . 'Total ' . $total_cost_sum . '</li><li>Concentrate ' . $cost_sum_concentrate . ' on ' . $obj->kg_day_dry_concentrate . 'kg ' . '</li><li>Forage ' . $cost_sum_forage . ' on ' . $obj->kg_day_dry_forage . 'kg ' . '</li><li>Forage Percent ' . round($total_percent_forage*100,1) . '%' . '</li></ul>';
     
        return $this->bootStrapBox('Recipe Cost',$str);   
    }

    /**
     * plots a bar graph of percent of things blanced.
     */
    public function plotNRCBalanceBarChartGraph($modelOutArr = null) {
        // print_r($modelOutArr);
        $dmi = (($modelOutArr->DMIActual / $modelOutArr->DMIPred * 100) - 100);
        $ndf = (($modelOutArr->DietNDF / .27 * 100) - 100);

        $arrayDied = array(
            "DMI" => $dmi,
            "NDF" => $ndf,
            "Effective NDF" => 2,
            "Forage NDF" => 1,
            "Net Energy" => -3,
            "NRC 2001 NEL" => 2,
            "Crude Protein" => 1,
            "Metabolized Protein (MP)" => -2,
            "RDP" => 4,
            "Calcium" => 2,
            "Phosporous" => -2,
            "Potassium" => 2,
            "Sodium" => 2
        );
        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string'; //BACKWARDS
        $headerStrArr['Died'] = 'number';

        $opt = ' hAxis: { title: "+/- % of Required" },  width: 300,  height: 600, legend:"none"';
        $x = new GoogleVisualizationBar($opt, $headerStrArr, $arrayDied);

                 return $this->bootstrapBox('Balance Chart (fixme)',$x->toString());
        
    }

    function showNRC2001Accordion($editrecipeid, $title) {

        $accordionArray[1]['name'] = 'Recipe Inputs';
        $accordionArray[2]['name'] = 'Animal Inputs';
        $accordionArray[3]['name'] = 'Location & Title';
        $accordionArray[4]['name'] = 'Link to On Farm Feeds';

        $accordion = new AccordionImplementationNRC2001Quickforms($editrecipeid, $title);
        $accordion->setCSS('accordionNarrow');
        print($accordion->render($accordionArray));
    }

    function recipeInfo($statusType = 'active') {
        $str = '';
        if (($statusType != 'active') AND ( $statusType != 'staging')) {
            throw new Exception("Error unknow status type ($statusType) for box: $e.");
        }

        $query = "SELECT * FROM nutrition.nrc_recipe_param WHERE status='$statusType' ORDER BY update_time DESC";
        $res = $GLOBALS ['pdo']->query($query);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $right=array();
            if ($statusType == 'active') {
                $str = $str . ("<div class='recipeBoxActive'>");
            } elseif ($statusType == 'staging') {
                $str = $str . ("<div class='recipeBoxStaging'>");
            }

            /* popover content is the recipe in a basic table format */
            //now load up some of the recipe items
            $sql2 = " SELECT *,locations[1] as main_location_id FROM nutrition.nrc_recipe_active_staging WHERE nrc_recipe={$row['nrc_recipe']} ORDER BY kg_day_dry DESC";
            $res2 = $GLOBALS ['pdo']->query($sql2);
            $popoverContent = '<table >    <thead>      <tr>        <th>Feed</th>        <th>kg</th>           </tr>    </thead>    <tbody>';
            while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
                $popoverContent = $popoverContent . (" <tr><td>{$row2['feed_library_name']}</td><td> {$row2['kg_day_dry']} </td></tr>");
                $kg_day_dry_sum = $row2['kg_day_dry_sum'];
                $main_location_id=$row2['main_location_id'];
            }
            $popoverContent = $popoverContent . ' </tbody>  </table>';

            /*  */



            $str = $str . ('<h4>' . $row['title'] . '</h4>');
            $str = $str . ('<h6>' . ' Recipe #' . $row['nrc_recipe'] . ' ');

       
            $left=  $this->recipeLatestDryMattersTable($main_location_id,9);
            $centre= (new BootStrap)->plainCard('Comment',$row['comment']);
                
                
           // $str = $str . ' <button type="button" class="btn btn-default" data-container="body" data-widget="popover" data-placement="right" data-html="true" data-content="' .  . '" data-original-title="" title="" aria-describedby="popover77277">Info</button>';
            $str = $str . ('</h6><hr>');
            $right[]= '<dl>';

      
            $right[]= ('<dt>Last Edit</dt><dd>' . date('Y-m-d H:m:s', strtotime($row['update_time'])) . ' by ' . $row['update_userid'] . '</dd>');
            $right[]= ('<dt>Created</dt><dd>' . date('Y-m-d H:m:s', strtotime($row['create_time'])) . '</dd>');
           $right[]= '<dt>DM</dt><dd>' . $kg_day_dry_sum . ' kg/cow/day</dd>';
           $right[]= '</dl>';
            //find locations
            $sql3 = "SELECT * FROM nutrition.nrc_recipe_location
LEFT JOIN bovinemanagement.location on location.id=location_id
 WHERE nrc_recipe={$row['nrc_recipe']} ORDER BY modifier DESC";
            $res3 = $GLOBALS ['pdo']->query($sql3);
            $right[]= ('<h6>Locations</h6>');
            while ($row3 = $res3->fetch(PDO::FETCH_ASSOC)) {
                 $right[]= ('<h5>' . $row3['name'] . ' ' . $row3['modifier'] * 100 . '%</h5>');
            }

            
             $str = $str .   (new BootStrap)->fourthfourthfourthfourth((new BootStrap)->plainCard('Stats',implode($right)),$left,$centre,(new BootStrap)->plainCard('Recipe',($popoverContent)));
            
            
            
            $str = $str . ' <hr>';


            if ($statusType == 'active') {
                $out = $str = $str . ($this->duplicateButton($row['nrc_recipe'], 'Duplicate To Staging'));
                $str = $str . ($this->backToStagingButton($row['nrc_recipe']));  //if recipe is in progress this will disappear.               OR below
                //  $str = $str . ($this->archiveButton($row['nrc_recipe']));    //only can be archived if recipe is not in progress yet. ENABLE WHEN ipad starts working in tractor.
            } elseif ($statusType == 'staging') {
                $str = $str . ($this->editButton($row['nrc_recipe']));
                 $str = $str . "<br/>";
                $str = $str . ($this->duplicateButton($row['nrc_recipe'], 'Duplicate Here'));

                $str = $str . ($this->deleteButton($row['nrc_recipe']));
                $str = $str . ($this->activateButton($row['nrc_recipe']));
            }

            $str = $str . ('</div>');
        }

        return $str;
    }

    function editButton($recipeID) {

        $form = new HTML_QuickForm("tmrAdminEditButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipeID', $recipeID);
        $form->addElement('submit', 'btnEdit', 'Edit');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $recipeID = $form->getElementValue('recipeID');

            //print("<br>Editing $recipeID !!!!<br>");
            //run edit code HERE!


            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=114&editrecipeid=$recipeID");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function deleteButton($recipeID) {
        $form = new HTML_QuickForm("tmrAdminDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipeID', $recipeID);
        $form->addElement('submit', 'btnDelete', 'Delete', array('class'=>'btn btn-warning','id' => 'btnDelete', 'onclick' => "return confirm('Are you sure you want to delete recipe #$recipeID?')")); //call javascript to confirm submit.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $recipeID = $form->getElementValue('recipeID');

            $res = $GLOBALS['pdo']->query("DELETE FROM nutrition.nrc_recipe_param where nrc_recipe='$recipeID'"); //does a cascade delete.
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function backToStagingButton($recipeID) {
        $form = new HTML_QuickForm("tmrAdminBackToStagingButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipeID', $recipeID);
        $form->addElement('submit', 'btnBackToStaging', 'Back To Staging');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $recipeID = $form->getElementValue('recipeID');

            //run backToStaging code
            $this->moveRecipe($recipeID, 'staging'); //move back to staging

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    //to be used at midnight to reset all the recipes and store the old ones.
    //
    function duplicateAllActiveAndArchiveOld() {
        $form = new HTML_QuickForm("duplicateAllActiveAndArchiveOldButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        $form->addElement('submit', 'btnduplicateAllActiveAndArchiveOld', 'Duplicate All Active And Archive Old', array('id' => 'btnduplicateAllActiveAndArchiveOld', 'onclick' => "return confirm('Are you sure you want to copy all the active recipes and make duplicates? This is only used once per day at midnight.')")); //call javascript to confirm submit.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');

            //find all active recipes.
            $res = $GLOBALS['pdo']->query("SELECT nrc_recipe FROM nutrition.nrc_recipe_param where status='active'"); //does a cascade delete.

            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $recipeID = $row['nrc_recipe'];
                //now duplicate each.
                $this->duplicateRecipe($recipeID, 'active'); //create duplicate in active
                $this->archiveRecipe($recipeID); //archive recipe in json format.
                $this->moveRecipe($recipeID, 'archive'); //move old recipe to archive
            }

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }
    
    //adds latest forage tests to staging recipe
    //NOTE: as of Oct 2017, the sql code to find the closest forage analysis is pretty basic will only pick the closest test, even if we are in a different field.
    //after we pick the closest test we need to update feed locations
    function addLatestBagConsumptionForageTestToStagingRecipes() {
        $form = new HTML_QuickForm("addLatestBagConsumptionForageTestToStagingRecipes", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        $form->addElement('submit', 'bnnaddLatestBagConsumptionForageTestToStagingRecipes', 'Add Latest Bag Consumption Forage Test to Staging Recipes', array('id' => 'bnnaddLatestBagConsumptionForageTestToStagingRecipes', 'onclick' => "return confirm('Are you sure you want to add the latest forage tests to all the staging recipes?')")); //call javascript to confirm submit.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');

            //find all active recipes.
            $res = $GLOBALS['pdo']->query("SELECT nrc_recipe FROM nutrition.nrc_recipe_param where status='staging'");

            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $recipeID = $row['nrc_recipe'];

                //for each staging recipe lookup the closest forage analysis for all current open bags
                //check if this is already in the recipe, if it is, do nothing.
                //if it is not then add to the recipe with zero amount.
                $sql2 = "INSERT INTO nutrition.nrc_recipe_item
    (nrc_recipe,feed_library_name,kg_day_dry,feedcurr_id)
(SELECT {$row['nrc_recipe']},\"Feed Name\",0,null from nutrition.bag_analysis_overlay_feed_library_bag_current () WHERE \"IFN\"::int  IN 
(SELECT nutrition.bag_closest_analysis_id (id,footage,direction) FROM nutrition.bag_current_open))
ON CONFLICT DO NOTHING ;
";
                $res2 = $GLOBALS['pdo']->exec($sql2);
                
/* //FIXME, this seems to change yeast, not working??
                //this gives us a feedcurr_id by linking via feed name. 
                $sql3 = <<<SQL
SELECT feed_library_name,bag_id,'bagf|'||bag_id as feedcurr_id from nutrition.nrc_recipe_active_staging 
LEFT JOIN nutrition.bag_analysis_overlay_feed_library() on "Feed Name"=feed_library_name
LEFT JOIN nutrition.bag_analysis ON id="IFN"::integer
where feedcurr_id is null and nrc_recipe={$row['nrc_recipe']} AND bag_id is not null
SQL;
                //update the feed_curr_id automatically
                $res3 = $GLOBALS['pdo']->query($sql3);
                while ($row3 = $res3->fetch(PDO::FETCH_ASSOC)) {
                    $query4 = "UPDATE nutrition.nrc_recipe_item SET  feedcurr_id = '{$row3['feedcurr_id']}' WHERE nrc_recipe_item.feed_library_name='{$row3['feed_library_name']}' and nrc_recipe={$row['nrc_recipe']}";
                    $upd4 = $GLOBALS['pdo']->exec($query4);
                }
            
*/
            }
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    //move all active recipes to staging
    //
    function moveAllActiveToStaging() {
        $form = new HTML_QuickForm("moveAllActiveToStaging", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        $form->addElement('submit', 'btndmoveAllActiveToStaging', 'Move All Active To staging', array('id' => 'btnmoveAllActiveToStaging', 'onclick' => "return confirm('Are you sure you want to move all active recipes to staging for editing?')")); //call javascript to confirm submit.
        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');

            //find all active recipes.
            $res = $GLOBALS['pdo']->query("SELECT nrc_recipe FROM nutrition.nrc_recipe_param where status='active'"); //does a cascade delete.

            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $recipeID = $row['nrc_recipe'];
               
                $this->moveRecipe($recipeID, 'staging'); //move to staging
            }

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }
    

    //used by various function to move a recipe to archive,active, or staging.
    function moveRecipe($recipeID, $toLocation = 'staging') {
        $res = $GLOBALS['pdo']->query("UPDATE nutrition.nrc_recipe_param SET status='$toLocation',update_userid='{$GLOBALS['auth']->getUsername()}' WHERE nrc_recipe='$recipeID'");
    }
    
    
     //used by archive button to archive recipe to long term json log.
    function archiveRecipe($recipeID) {
        $res = $GLOBALS['pdo']->query("insert into nutrition.nrc_archive select '$recipeID' as nrc_recipe_id,(SELECT create_time from nutrition.nrc_recipe_param where nrc_recipe='$recipeID') as recipe_create_time ,
array_to_json(array_agg(row_to_json(t)))  
from (  
 SELECT * from nutrition.nrc_recipe_display('$recipeID')
) t");
    }

    function archiveButton($recipeID) {
        $form = new HTML_QuickForm("tmrAdminArchiveButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipeID', $recipeID);
        $form->addElement('submit', 'btnArchive', 'Archive');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $recipeID = $form->getElementValue('recipeID');



            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function duplicateButton($recipeID, $nameOfButton = 'Duplicate') {
        $form = new HTML_QuickForm("tmrAdminDuplicateButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipeID', $recipeID);
        $form->addElement('submit', 'btnDuplicate', $nameOfButton);


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $recipeID = $form->getElementValue('recipeID');

            //do duplicating to staging
            $this->duplicateRecipe($recipeID, 'staging');

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    function duplicateRecipe($recipeID, $duplicateLocation = 'staging') {

        // Open a transaction
        try {
            $GLOBALS['pdo']->beginTransaction();

            ////START nrc_recipe_param
            //select old recipe name AND comment2
            $sql = "SELECT title,comment,status	FROM nutrition.nrc_recipe_param WHERE nrc_recipe=$recipeID LIMIT 1";
            $res0 = $GLOBALS['pdo']->query($sql);
            $row0 = $res0->fetch(PDO::FETCH_ASSOC);

            //do an insert into new recipe
            $sql = "INSERT INTO nutrition.nrc_recipe_param(title,comment,status,create_userid,update_userid) VALUES ('{$row0['title']}','{$row0['comment']}','$duplicateLocation','{$GLOBALS['auth']->getUsername()}','{$GLOBALS['auth']->getUsername()}')";
            $res1 = $GLOBALS['pdo']->exec($sql);
            $clone_recipe_id = $GLOBALS['pdo']->lastInsertId('nutrition.nutrition_sequence');


            ////START nrc_recipe_animal_input
            //do all in one statement replacing id with new clone id.
            $sql = "INSERT INTO nutrition.nrc_recipe_animal_input (
            \"nrc_recipe\",\"AnimalType\",\"Age\",\"BW\",\"DaysPreg\",\"CS\",\"DaysInMilk\",\"LactNum\",\"FirstCalf\",\"CalfInt\",\"DesiredADG\",\"UseTargetADG\",\"CalfBW\",\"CalfTemp\",\"MW\",\"MWFromBreed\",\"Breed\",\"CBW\",\"CBWFromMW\",\"MilkProd\",\"MilkFat\",\"ShowMilkTrue\",\"MilkTrueProtein\",\"Lactose\",\"Temp\",\"PrevTemp\",\"WindSpeed\",\"Grazing\",\"Distance\",\"Topography\",\"Trips\",\"CoatCond\",\"HeatStress\",\"HairDepth\",\"NightCooling\"
            )
           SELECT  $clone_recipe_id,\"AnimalType\",\"Age\",\"BW\",\"DaysPreg\",\"CS\",\"DaysInMilk\",\"LactNum\",\"FirstCalf\",\"CalfInt\",\"DesiredADG\",\"UseTargetADG\",\"CalfBW\",\"CalfTemp\",\"MW\",\"MWFromBreed\",\"Breed\",\"CBW\",\"CBWFromMW\",\"MilkProd\",\"MilkFat\",\"ShowMilkTrue\",\"MilkTrueProtein\",\"Lactose\",\"Temp\",\"PrevTemp\",\"WindSpeed\",\"Grazing\",\"Distance\",\"Topography\",\"Trips\",\"CoatCond\",\"HeatStress\",\"HairDepth\",\"NightCooling\"
           FROM nutrition.nrc_recipe_animal_input WHERE nrc_recipe=$recipeID";
            $res2 = $GLOBALS['pdo']->query($sql);

            ////START nrc_recipe_item
            //do all in one statement replacing id with new clone id.
            $sql = "INSERT INTO nutrition.nrc_recipe_item (
             	\"nrc_recipe\",\"feed_library_name\",\"kg_day_dry\",\"feedcurr_id\"
              )
           SELECT  $clone_recipe_id,\"feed_library_name\",\"kg_day_dry\",\"feedcurr_id\"
              FROM nutrition.nrc_recipe_item WHERE nrc_recipe=$recipeID";
            $res3 = $GLOBALS['pdo']->query($sql);

            ////START nrc_recipe_location
            //do all in one statement replacing id with new clone id.
            $sql = "INSERT INTO nutrition.nrc_recipe_location (
             	\"nrc_recipe\",\"location_id\",\"modifier\"
              )
           SELECT  $clone_recipe_id,\"location_id\",\"modifier\"
              FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$recipeID";
            $res4 = $GLOBALS['pdo']->query($sql);


            // determine if the commit or rollback
            $GLOBALS['pdo']->commit();
        } catch (PDOException $e) {
            $GLOBALS['pdo']->rollBack();
            throw $e;
        }
        // Close the transaction
        //////////////////////////////////////////
        return $clone_recipe_id;
    }

    function activateButton($recipeID) {
        $form = new HTML_QuickForm("tmrAdminActivateButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipeID', $recipeID);
        $form->addElement('submit', 'btnActivate', 'Activate',array('class'=>'btn btn-success'));


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $recipeID = $form->getElementValue('recipeID');

            $this->moveRecipe($recipeID, 'active');

            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }

        return $form->toHtml(); //pass on as html string to display later in the table
    }

    /*
     * checks that we have receipes for every group and not more then one.
     * 
     */

    public function checkActiveRecipesForErrors($recipe_id) {
        $str = '';
        $str = $str . $this->checkErrorsFunction("Close-up Dry Group", 37);
        $str = $str . $this->checkErrorsFunction("99 Maternity Pack", 38);
        $str = $str . $this->checkErrorsFunction("99 Fresh/Sick Group", 39);
        $str = $str . $this->checkErrorsFunction("99 High Group", 40);
        $str = $str . $this->checkErrorsFunction("99 Low Group", 41);
        $str = $str . $this->checkErrorsFunction("99 Heifer Group", 42);
        $str = $str . $this->checkErrorsFunction("Heifer Barn", 46);
        $str=  $str . $this->checkReceipeForLinkedFeeds($recipe_id);
        $str=  $str . $this->checkReceipeItemsAllHaveCostEntered($recipe_id);
        return $str;
    }

    private function checkErrorsFunction($locationName = null, $locationID = null) {
        $str = '';
        $sql = "SELECT count(nrc_recipe_param.nrc_recipe)
FROM nutrition.nrc_recipe_param
LEFT JOIN nutrition.nrc_recipe_location  on nrc_recipe_param.nrc_recipe=nrc_recipe_location.nrc_recipe
 WHERE nrc_recipe_param.status='active' AND location_id=$locationID";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row['count'] == 0) {
            $str = $str . (new BootStrap)->errorNotify("Error: Group $locationName does not have an active recipe.");
        } elseif ($row['count'] > 1) {
            $str = $str . (new BootStrap)->errorNotify("Error: Group $locationName has more than one active recipe.");
        }

        return $str;
    }

    /*
     * checks the we linked the NRC recipe to local 
     */
    private function checkReceipeForLinkedFeeds($recipe_id=null) {
        $str='';
         $sql = "SELECT title,nrc_recipe from nutrition.nrc_recipe_active_staging where 
(feedcurr_id = '') IS NOT FALSE";
        $res = $GLOBALS['pdo']->query($sql);
         while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
             //shows for individual recipe on recipe page and for all receipes on staging page.
             if ( (empty($recipe_id)) OR ($recipe_id==$row['nrc_recipe'])) {
               $str = $str . (new BootStrap)->errorNotify("Error: Missing Linked Feed for recipe {$row['title']}, ie did you change feed tanks or silage bags?");
             }
         }
         return $str;
    }
    
    /*
     * checks that all recipe items have a feed cost entered.
     */
    private function checkReceipeItemsAllHaveCostEntered($recipe_id=null) {
        $str='';
         $sql = "SELECT feed_type,location from nutrition.feedcurr where cost is null";
        $res = $GLOBALS['pdo']->query($sql);
         while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
             //shows for individual recipe on recipe page and for all receipes on staging page.
             if ( (empty($recipe_id)) OR ($recipe_id==$row['nrc_recipe'])) {
               $str = $str . (new BootStrap)->errorNotify("Error: Missing Cost for Feed {$row['feed_type']} - {$row['location']}, is cost per tonne entered?");
             }
         }
         return $str;
    }
    
    
    
    
    /*
     * check recipe animal inputs match NRC inputs
     * 
     */
     private function checkReceipeForAnimalInputs($recipeID=null) {
        $str='';
        
        //FIXME, will only show one locaiton
         $sql = " SELECT nrc_recipe_location.nrc_recipe,location_id,\"MilkProd\",\"MilkFat\",\"MilkTrueProtein\" 
FROM nutrition.nrc_recipe_animal_input 
LEFT JOIN nutrition.nrc_recipe_location ON nrc_recipe_animal_input.nrc_recipe=nrc_recipe_animal_input.nrc_recipe
WHERE nrc_recipe_animal_input.nrc_recipe=$recipeID limit 1";

         $str='';
        $res = $GLOBALS ['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
           
             $sql2="SELECT location_name,round(avg(total_milk)::numeric,2) as total_milk,round(avg(fat_per)::numeric,2) as fat_per, round(avg(prot_per)::numeric,2) as prot_per,round(avg(lactose_per)::numeric,2) as lactose_per, round(avg(ssc)::numeric,-3) as ssc, round(avg(mun)/10::numeric,1) as mun FROM bovinemanagement.bovinecurr
LEFT JOIN batch.valacta_data_latest_test_view ON bovinecurr.local_number = valacta_data_latest_test_view.chain WHERE location_id={$row['location_id']} GROUP BY location_name";
         
           

        $res2 = $GLOBALS['pdo']->query($sql2);
         while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {
             //shows for individual recipe on recipe page and for all receipes on staging page.
            // if ( (empty($recipe_id)) OR ($recipe_id==$row['nrc_recipe'])) {
               $str = $str . (new BootStrap)->errorNotify("Error:  for recipe {$row['title']}, BLAH BLAH.");
            // }
         }
        }
         return $str;
    }
    
    
    
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    ///
    ///

    function editRecipeLocationsQuickform($recipe_id = -1, $title = null) {
     
        $form = new HTML_QuickForm("editRecipeLocationsQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        /* Edit Recipe Locations and mod factors and allow deletes */
        $form->addElement('header', 'hdrreditRecipeQuickformLocations', "Edit recipe Bovine Loaction(s):"); //set title
        //grab mod Factor array
        $mod_percentage = self::modFactor();

        //grab a list of locations
        $location = self::bovineLocations();

        $sql = "SELECT * FROM nutrition.nrc_recipe_location LEFT JOIN bovinemanagement.location on location.id=location_id WHERE nrc_recipe=$recipe_id ORDER BY modifier DESC";
        $res = $GLOBALS['pdo']->query($sql);

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $mod_percentageDef = $row['modifier'] * 100;
            $locationDef = $row['location_id'];
            $loc = array();
            //create group of items and delete buttons.
            $loc['location_id'] =  $form->createElement('static', $row['location_id'], 'Location', $row['name']);
            $loc['modification_factor'] =  $form->createElement('select', 'mod_percentage', 'Location DM Modification %', $mod_percentage);
            $loc['delete'] =  $form->createElement('checkbox', 'delete', null, 'delete');
            $loc['loc2'] =  $form->createElement('hidden', 'location_id', $row['location_id']);
            $form->addGroup($loc, "locitem_$locationDef", "Location, Mod Factor", ',&nbsp;');

            //set default dry matter values
            $form->setDefaults(array("locitem_$locationDef" => array('location_id' => "$locationDef", 'mod_percentage' => $mod_percentageDef, 'delete' => '')));
        }


        //now add option for new locations to be added.
        $newLoc['location_id'] =  $form->createElement('select', 'location_id', 'Location', $location);
        $newLoc['mod_percentage'] =  $form->createElement('select', 'mod_percentage', 'Location DM Modification %', $mod_percentage);
        $form->addGroup($newLoc, 'locitemnew', "New Location (location, mod factor)", ',&nbsp;');

        ///comments and name

        /* Edit Comment2 */
        $form->addElement('header', 'hdrreditCommentRecipeQuickformComment', "Comment:"); //set title
        $sql = "SELECT title,comment FROM nutrition.nrc_recipe_param WHERE nrc_recipe=$recipe_id LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $form->addElement('text', 'title', 'Title');
        $form->setDefaults(array('title' => "{$row['title']}")); //set default to no variety.
        //
        $form->addElement('textarea', 'comment', 'Comment', array('cols' => 60, 'rows' => 4));
        $form->setDefaults(array('comment' => "{$row['comment']}")); //set default to no variety.
        //
        //
        //add hidden
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipe_id', $recipe_id);

        //submit buttons

        $submitGroup[] = $form->createElement('reset', 'btnReset', 'Reset');
        $submitGroup[] =  $form->createElement('button', 'action', 'Cancel', array("onClick" => "{window.location.href='?pageid={$_REQUEST['pageid']}';}")); //cancel javascript button
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $form->freeze();
            $form->process(array('NRC2001', 'editRecipeLocationsQuickformProcess'));   //Callback, either function name or array(&$object, 'method') 
        } //end validation
        else {
            $form->accept($renderer);
            return $renderer->toHtml();
        }
    }

    function editRecipeLocationsQuickformProcess($values) {

        //get values
        $pageid = $values['pageid'];
        $recipe_id = $values['recipe_id'];
        $recipeID = $recipe_id;

        /* do mix locations */

        //create array to search later from location data.
        $recipeLocsArray = self::preg_grep_keys('/locitem_/', $values);
        //go through array and see if data has changed for each location
        foreach ($recipeLocsArray as $key => $val) {

            $sql = "SELECT modifier FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$recipe_id AND location_id={$val['location_id']}";
            $res3 = $GLOBALS['pdo']->query($sql);
            $row3 = $res3->fetch(PDO::FETCH_ASSOC);
            $modification_factor = $val['mod_percentage'] / 100;

            //update or delete data if necessary
            if ($val['delete'] == true) {
                $query = "DELETE FROM nutrition.nrc_recipe_location WHERE nrc_recipe=$recipe_id AND location_id={$val['location_id']}";
                $upd3 = $GLOBALS['pdo']->exec($query);
            } elseif ($row3['modifier'] != $modification_factor) {
                $query = "UPDATE nutrition.nrc_recipe_location SET  modifier = '{$modification_factor}' WHERE nrc_recipe=$recipe_id AND location_id={$val['location_id']}";
                $upd3 = $GLOBALS['pdo']->exec($query);
            }
        }

        //see if the new location field has been used or not.
        if ($values['locitemnew']['location_id'] != null) {

            $modification_factor = $values['locitemnew']['mod_percentage'] * .01;

            $query = "INSERT INTO nutrition.nrc_recipe_location(nrc_recipe,location_id,modifier) VALUES ($recipe_id,{$values['locitemnew']['location_id']},$modification_factor)";
            $upd3a = $GLOBALS['pdo']->exec($query);
        }
        /* end mix locations */

        //check name data has changed or not.
        $sql = "SELECT comment FROM nutrition.nrc_recipe_param WHERE nrc_recipe=$recipe_id LIMIT 1";
        $res1 = $GLOBALS['pdo']->query($sql);
        $row1 = $res1->fetch(PDO::FETCH_ASSOC);
        if ($row1['comment'] != $values['comment']) {
            $values['comment'] = pg_escape_string($values['comment']);
            $query = "UPDATE nutrition.nrc_recipe_param SET comment = '{$values['comment']}',update_userid='{$GLOBALS['auth']->getUsername()}' WHERE nrc_recipe=$recipe_id";
            $upd1 = $GLOBALS['pdo']->exec($query);
        }

        //check comment data has changed or not.
        $sql = "SELECT title FROM nutrition.nrc_recipe_param WHERE nrc_recipe=$recipe_id LIMIT 1";
        $res1 = $GLOBALS['pdo']->query($sql);
        $row1 = $res1->fetch(PDO::FETCH_ASSOC);
        if ($row1['title'] != $values['title']) {
            $values['title'] = pg_escape_string($values['title']);
            $query = "UPDATE nutrition.nrc_recipe_param SET title = '{$values['title']}',update_userid='{$GLOBALS['auth']->getUsername()}' WHERE nrc_recipe=$recipe_id";
            $upd1 = $GLOBALS['pdo']->exec($query);
        }



        // Close the transaction
        //////////////////////////////////////////
        //forward to same page, ob_start() is key to making this work!
        header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$pageid}&editrecipeid=$recipe_id");
        exit();
    }

    private function bovineLocations() {

        //grab a list of locations
        $sql = "SELECT id,name FROM bovinemanagement.location WHERE active=true AND on_farm=true";
        $res = $GLOBALS['pdo']->query($sql);
        $location[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $location[$row['id']] = $row['name'];
        }
        return $location;
    }

    private function modFactor() {

        $mod_percentage[null] = "Please Select one..."; //dummy value
        for ($j = 1; $j <= 225; $j++) {
            $index = $j;
            $mod_percentage[$index] = "$j%";
        }
        return $mod_percentage;
    }

    function getCurrentFeedsArray() {
        $sql = " SELECT *,strpos(id, 'bag_r') as reverse FROM nutrition.feedcurr ORDER BY feed_type";
        $res = $GLOBALS['pdo']->query($sql);
        $feedCurrArr[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            $bagreverse = '';
            if ($row['reverse'] == 1) {
                $bagreverse = ' (reverse)';
            }

            //parse for last word of name and then make an array for the select list.
            $feedCurrArr[$row['id']] = $row['feed_type'] . ' @ ' . $row['location'] . $bagreverse;
        }
        return $feedCurrArr;
    }

    /**
     * 
     * This links on farm feeds to the NRC model feed library feeds in the recipe.
     * 
     */
    public function editLinkFeedsNRCtoOnFarmQuickform($recipe_id = 1, $title = null) {
        $form = new HTML_QuickForm("editLinkFeedsNRCtoOnFarmQuickform_$recipe_id", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('html', "<h3>Edit Link Feeds: $title (Recipe #$recipe_id)</h3>"); //set title
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipe_id', $recipe_id);
        $form->addElement('hidden', 'editrecipeid', $recipe_id); //very important, this variable is looked for at the start of the class, the form validation does not work without it.
        //select the items. NRC FEEDS
        $sql = "SELECT feed_library_name,kg_day_dry,feedcurr_id FROM nutrition.nrc_recipe_item where nrc_recipe=$recipe_id ORDER BY kg_day_dry DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $currentlyUsedFeedsArray = array();
        // ON FARM FEEDS
        $feedCurrArr = self::getCurrentFeedsArray();

        while (($row = $res->fetch((PDO::FETCH_ASSOC)))) {
            $currentlyUsedFeedsArray[$row['feed_library_name']] = "{$row['feed_library_name']}";
            $displayText = $row['feed_library_name'];
            $id = $row['feed_library_name'];
            $item = array();
            //create group of items and delete buttons.
            $item['id'] =  $form->createElement('hidden', 'id', "$id");
            //$item['location_id'] =  $form->createElement('static', $row['location_id'], 'Location', $row['name']);
            $item['static'] =  $form->createElement('static', '');
            $item['feedcurr_id'] =  $form->createElement('select', 'feedcurr_id', 'on Farm Feeds', $feedCurrArr);
            $form->addGroup($item, "recipeitem_$id", "$displayText", '&nbsp;');

            //set default dry matter values
            $form->setDefaults(array("recipeitem_$id" => array('feedcurr_id' => $row['feedcurr_id'])));
        }



        //submit buttons

        $submitGroup[] = $form->createElement('reset', 'btnReset', 'Reset');
        $submitGroup[] =  $form->createElement('button', 'action', 'Cancel', array("onClick" => "{window.location.href='?pageid={$_REQUEST['pageid']}';}")); //cancel javascript button
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $form->freeze();
            $form->process(array('NRC2001', 'editLinkFeedsNRCtoOnFarmProcess'));   //Callback, either function name or array(&$object, 'method') 
        } //end validation
        else {
            $form->accept($renderer);
            return $renderer->toHtml();
        }
    }

    function editLinkFeedsNRCtoOnFarmProcess($values) {



        //get values
        $pageid = $values['pageid'];
        $recipe_id = $values['recipe_id'];
        $recipeID = $recipe_id;
        //print("recipeID".$recipeID);
        //////////////////////////
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            /* Do recipe items */

            //create array to search later from recipe item data.
            $recipeItemArray = self::preg_grep_keys('/recipeitem_/', $values);

            //go through array and see if data has changed for each kg_day_dry
            foreach ($recipeItemArray as $key => $val) {

                $sql = "SELECT feedcurr_id FROM nutrition.nrc_recipe_item WHERE nrc_recipe_item.feed_library_name='{$val['id']}' and nrc_recipe=$recipe_id LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch((PDO::FETCH_ASSOC));

                if ($row2['feedcurr_id'] != $val['feedcurr_id']) {
                    $query = "UPDATE nutrition.nrc_recipe_item SET  feedcurr_id = '{$val['feedcurr_id']}' WHERE nrc_recipe_item.feed_library_name='{$val['id']}' and nrc_recipe=$recipe_id";
                    $upd2 = $GLOBALS['pdo']->exec($query);
                }
            }



            /* end of recipe items */
            // determine if the commit or rollback    
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }


        // Close the transaction
        //////////////////////////////////////////
        //forward to same page, ob_start() is key to making this work!
        header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&editrecipeid=$recipeID");
        exit();
    }

    function getNRCFeedArrayForRecipe($recipe_id) {
        //get feeds from db.
        $sql = "SELECT feed_library_name,kg_day_dry FROM nutrition.nrc_recipe_item WHERE nrc_recipe=$recipe_id ";
        $res2 = $GLOBALS['pdo']->query($sql);
        $recipe = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $recipe["{$row2['feed_library_name']}"] = $row2['kg_day_dry'];
        }

        $Feeds = NRCHelper::createFeedArrayOfFeedObjs($recipe); //load this into NRC model
        return $Feeds;
    }

    //get recipe from db and then generates model stuff
    //FOR USER GENERATED RECIPES
    function runStandardModel_UserAnimalInputs($recipe_id = 1) {

        $Feeds = $this->getNRCFeedArrayForRecipe($recipe_id);
        //
        $sql = "Select * FROM nutrition.nrc_recipe_animal_input WHERE nrc_recipe=$recipe_id";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $input2 = $statement->fetchObject();   //get nrc animal param input obj
        //
        $nrc = new NRCDairlyCattleComputations($input2, $Feeds);
        $retArray = $nrc->ModelComputations();

        return $retArray;
    }

    //get recipe from db and then generates model stuff
    //FOR USER GENERATED RECIPES
    function runStandardModel_HeiferGroup($recipe_id = 2) {

        $Feeds = $this->getNRCFeedArrayForRecipe($recipe_id);
        //
        $input = NRCHelper::createDefaultInputObject('Lactating Cow');

        //override default inputs.
        $input->Age = 26;            # age  (months)
        $input->BW = 450;             # body weight
        $input->DaysPreg = 15;      # days pregnant
        $input->CS = 3.0;             # condition score (1 - 5)
        $input->DaysInMilk = 120;    # days in milk
        $input->LactNum = 1;          # lactation number
        $input->FirstCalf = 24;      # age at first calving  (months)
        $input->MilkProd = 31;           # milk production
        $input->MilkFat = 3.85;            # milk fat  (%)
        $input->MilkTrueProtein = 3.2;    # milk true protein (%)
        $input->Lactose = 4.8;            # milk lactose (%)

        $nrc = new NRCDairlyCattleComputations($input, $Feeds);
        $retArray = $nrc->ModelComputations();

        return $retArray;
    }

    //get recipe from db and then generates model stuff
    //FOR USER GENERATED RECIPES
    function runStandardModel_LowGroup($recipe_id = 3) {

        $Feeds = $this->getNRCFeedArrayForRecipe($recipe_id);
        //
        $input = NRCHelper::createDefaultInputObject('Lactating Cow');

        //override default inputs.
        $input->Age = 40;            # age  (months)
        $input->BW = 650;             # body weight
        $input->DaysPreg = 20;      # days pregnant
        $input->CS = 3.0;             # condition score (1 - 5)
        $input->DaysInMilk = 240;    # days in milk
        $input->LactNum = 2;          # lactation number
        $input->FirstCalf = 24;      # age at first calving  (months)
        $input->MilkProd = 25;           # milk production
        $input->MilkFat = 4;            # milk fat  (%)
        $input->MilkTrueProtein = 3.2;    # milk true protein (%)
        $input->Lactose = 4.8;            # milk lactose (%)

        $nrc = new NRCDairlyCattleComputations($input, $Feeds);
        $retArray = $nrc->ModelComputations();

        return $retArray;
    }

    /* NRC 1.19 standard simulation for lacatating cow */

    function testCase1() {

        //Feeds
//name / amount
        $recipe = array();
        $recipe['Legume Forage Hay, immature'] = 5.410;
        $recipe['Corn Silage, normal'] = 12.010;
        $recipe['Corn Grain, steam-flaked'] = 6.610;
        $recipe['Calcium soaps of fatty acids'] = 0.300;
        $recipe['Tallow'] = 0.300;
        $recipe['Cottonseed, Whole with lint'] = 2.25;
        $recipe['Soybean, Meal, solv. 48% CP'] = 2.40;
        $recipe['Blood Meal, ring dried'] = 0.150;
        $recipe['Calcium Carbonate'] = 0.030;
        $recipe['MonoSodium Phosphate (1 H2O)'] = 0.060;
        $recipe['Salt'] = 0.150;
        $recipe['Vitamin premix 1'] = 0.360;

        $Feeds = NRCHelper::createFeedArrayOfFeedObjs($recipe); //load this into NRC model
//Input
        $input = new Input();
//Animal Description
        $input->AnimalType = 'Lactating Cow';     # Options : "Lactating Cow", "Dry Cow" "Replacement Heifer", "Young Calf"
        $input->Age = 65;            # age  (months)
        $input->BW = 680;             # body weight
        $input->DaysPreg = 0;      # days pregnant
        $input->CS = 3.0;             # condition score (1 - 5)
        $input->DaysInMilk = 90;    # days in milk
        $input->LactNum = 3;          # lactation number
        $input->FirstCalf = 24;      # age at first calving  (months)
        $input->CalfInt = 12;        # calving interval  (months)
        $input->DesiredADG = 0;    # desired ADG (g/day) //NOTE: set to value when UseTargetADG = false
        $input->UseTargetADG = true;  # if True, indicates that program should use Target ADG, instead of Desired ADG to determine growth requirements
        $input->CalfBW = 0;         # Calf Body Weight (kg)
        $input->CalfTemp = 0;       # Calf Temperature (deg C)
//Production
        $input->MW = 680;                 # mature weight
        $input->MWFromBreed = true;       # indicates if the animal#s MW should be set as a function of the breed
        $input->Breed = 'Holstein';              # Choices :  "Ayrshire", "Brown Swiss" "Guernsey", "Holstein", "Jersey"
        $input->CBW = 43;                # calf birth weight
        $input->CBWFromMW = null;          # to compute CBW from MW /* not used */
        $input->MilkProd = 54.5;           # milk production
        $input->MilkFat = 3.5;            # milk fat  (%)
        $input->ShowMilkTrue = null;      # indicates if milk protein should be shown on a true or crude basis /* not used */
        $input->MilkTrueProtein = 3.0;    # milk true protein (%)
        $input->Lactose = 4.8;            # milk lactose (%)
# Management and Environment variables
        $input->Temp = 20;           # current temperature
        $input->PrevTemp = 0;       # previous temperature
        $input->WindSpeed = 0;
        $input->Grazing = 0;
        $input->Distance = 0;       # Distance between Pasture and Milking Center
        $input->Topography = 'Flat';     # "Flat", "Hilly"
        $input->Trips = 0;         # Number of one-way trips
        $input->CoatCond = 'Clean/Dry';       # Choices :  "Clean/Dry", "Some Mud" "Wet/Matted", "Covered with Snow/Mud"
        $input->HeatStress = 'None';     # "None", "Rapid/Shallow", "Open Mouth"
        $input->HairDepth = 0;      # depth of hair on coat
        $input->NightCooling = false;  # True = Night Cooling, False = None
//run model
        $nrc = new NRCDairlyCattleComputations($input, $Feeds);
        $retArray = $nrc->ModelComputations();

        return $retArray;
    }

    /* NRC 1.19 standard simulation for replacement heifer */

    function testCase2() {

        //Feeds
//name / amount
        $recipe = array();
        $recipe['Legume Forage Sil., mid-mat.'] = 3.84;
        $recipe['Corn Silage, normal'] = 8.67;
        $recipe['Corn Grain, ground, dry'] = 0.0;
        $recipe['Soybean, Meal, solv. 48% CP'] = .28;
        $recipe['Blood Meal, ring dried'] = 0.0;
        $recipe['Calcium Phosphate (Di-)'] = 0.010;
        $recipe['Calcium Carbonate'] = 0.0;

        $Feeds = NRCHelper::createFeedArrayOfFeedObjs($recipe); //load this into NRC model
//Input
        $input = new Input();
//Animal Description
        $input->AnimalType = 'Replacement Heifer';     # Options : "Lactating Cow", "Dry Cow" "Replacement Heifer", "Young Calf"
        $input->Age = 20;            # age  (months)
        $input->BW = 550;             # body weight
        $input->DaysPreg = 220;      # days pregnant
        $input->CS = 3.0;             # condition score (1 - 5)
        $input->DaysInMilk = 0;    # days in milk
        $input->LactNum = 0;          # lactation number
        $input->FirstCalf = 24;      # age at first calving  (months)
        $input->CalfInt = 12;        # calving interval  (months)
        $input->DesiredADG = 800;    # desired ADG (g/day) //NOTE: set to value when UseTargetADG = false
        $input->UseTargetADG = true;  # if True, indicates that program should use Target ADG, instead of Desired ADG to determine growth requirements
        $input->CalfBW = 0;         # Calf Body Weight (kg)
        $input->CalfTemp = 0;       # Calf Temperature (deg C)
//Production
        $input->MW = 0;                 # mature weight
        $input->MWFromBreed = true;       # indicates if the animal#s MW should be set as a function of the breed
        $input->Breed = 'Holstein';              # Choices :  "Ayrshire", "Brown Swiss" "Guernsey", "Holstein", "Jersey"
        $input->CBW = null;                # calf birth weight
        $input->CBWFromMW = null;          # to compute CBW from MW /* not used */
        $input->MilkProd = 0;           # milk production
        $input->MilkFat = 0;            # milk fat  (%)
        $input->ShowMilkTrue = null;      # indicates if milk protein should be shown on a true or crude basis /* not used */
        $input->MilkTrueProtein = 0;    # milk true protein (%)
        $input->Lactose = 0;            # milk lactose (%)
# Management and Environment variables
        $input->Temp = 20;           # current temperature
        $input->PrevTemp = 20;       # previous temperature
        $input->WindSpeed = 0;
        $input->Grazing = 0;
        $input->Distance = 0;       # Distance between Pasture and Milking Center
        $input->Topography = 'Flat';     # "Flat", "Hilly"
        $input->Trips = 0;         # Number of one-way trips
        $input->CoatCond = 'Clean/Dry';       # Choices :  "Clean/Dry", "Some Mud" "Wet/Matted", "Covered with Snow/Mud"
        $input->HeatStress = 'None';     # "None", "Rapid/Shallow", "Open Mouth"
        $input->HairDepth = 0;      # depth of hair on coat
        $input->NightCooling = false;  # True = Night Cooling, False = None
//run model
        $nrc = new NRCDairlyCattleComputations($input, $Feeds);
        $retArray = $nrc->ModelComputations();

        return $retArray;
    }

    
    
    /* we use this because sometimes we just want part of the model output and not 
     * display it all, this is for when we do want to display it.
     */
    public function render($out) {
        
        $inArr[] =  $out['addicanBalanceParamaters'];
      
       
        $inArr[] = ("<h3>Summary Report</h3>");
        $inArr[] = '<div id="nrc_summaryReportPage1">';
        $inArr[] =  $out['summaryReportPage1'];
        $inArr[] =  $out['summaryReportPage2'];
        $inArr[] = '</div>';
  
        $inArr[] = $out['ingrediantEnergyProteinSupply'];
        $inArr[] = $out['aminoAcids'];
        $inArr[] = $out['macroMineralDataTable'];
        $inArr[] = $out['microMineralDataTable'];
        $inArr[] = $out['vitaminMineralDataTable'];
        
         return implode($inArr);
    }
    
    
    /* main function that returns printable parts of the model to later render */
    public function processNRC2001Output($retArray) {

        //get data broken out of main array
        $input = $retArray['Input'];
        $output = $retArray['Output'];
        $mineral = $retArray['Mineral'];
        $feed = $retArray['Feed'];
        $feedInput = $retArray['FeedInput'];

      
        
        //show summaryReport
        $out['addicanBalanceParamaters']= $this->addicanBalanceParamaters($input, $output);
        $out['summaryReportPage1']=  $this->summaryReportPage1($input, $output, $mineral) ;
        $out['summaryReportPage2']=  $this->summaryReportPage2($input, $output, $mineral) ;
        $out['ingrediantEnergyProteinSupply']=($this->ingrediantEnergyProteinSupply($feed, $output, $feedInput));
        $out['aminoAcids']=($this->aminoAcids($output));
        $out['macroMineralDataTable']=($this->macroMineralDataTable($mineral));
        $out['microMineralDataTable']=($this->microMineralDataTable($mineral));
        $out['vitaminMineralDataTable']=($this->vitaminMineralDataTable($mineral));



        //DEBUG OUTPUT ARRAY
        /*      print("<br/>feed Array<br/>");
          //$f= get_object_vars($feed);
          foreach ($feed as $key => $value) {
          print("$key: {$feed[$key]->TDN} ");
          }
          print_r2($feed);//$this->Output->MPFeed
         *//*
          print("<H1>DEBUG</H1>");
          print("<br/>Outpur Array<br/>");
          $y = get_object_vars($output);
          print_r2($y);
          print("<br/>NEl<br/>");
          print_r2($y['NEl']);
          print("<br/>Mineral<br/>");
          $z = get_object_vars($mineral[1]);
          print_r2($z);
          print("<br/>Feed out<br/>");
          $v = get_object_vars($feed[1]);
          print_r2($v);
          print("<br/>Feed in <br/>");
          $v = get_object_vars($feedInput[1]);
          print_r2($v);

          print("<br/><br/>");
          print("<br/>DE<br/>");
          print_r2($y['DE']);

          print_r2($y['TDN_Act']);
         */
         
         return $out;
    }

    /**
     * shows a DataTable of the engery/protein paramaters of each feed.
     *  
     */
    function ingrediantEnergyProteinSupply($feed, $output, $feedInput) {

        $totalKey = count($feed) + 1;
        ///////////// FEEDS //////////////
        foreach ($feed as $key => $value) {
            $nameArr[$key] = $value->Name;

            $ME_day[$key] = round($value->MEng * $value->DMFed, 1);
            $NEl_day[$key] = round($value->NEl * $value->DMFed, 1);
            $NEg_day[$key] = round($value->NEg * $value->DMFed, 1);
            //

            $MCP[$key] = ' ';   //MCP for individual feeds are not calculated in NRC2001 example program. so just put a dash
            $MEng[$key] = round($value->MEng);
            $NEl[$key] = round($value->NEl);
            $NEg[$key] = round($value->NEg);

            $Kp[$key] = round($output->PsgRate);
        }
     
        //change units
        $output->RUP100 = array();
        foreach ($output->RUP as $key => $value) {
            $output->RUP100[$key] = round($output->RUP[$key] * 1000);
        }

        $output->RDP100 = array();
        foreach ($output->RDP as $key => $value) {
            $output->RDP100[$key] = round($output->RDP[$key] * 1000);
        }

        $output->CP100 = array();
        foreach ($output->CP as $key => $value) {
            $output->CP100[$key] = round($output->CP[$key]);
        }

        $output->TDN100 = array();
        foreach ($feedInput as $key => $value) {
            $output->TDN100[$key] = round($output->TDN[$key]);
        }

        $output->NDF100 = array();
        foreach ($feedInput as $key => $value) {
            $output->NDF100[$key] = round($output->NDF[$key], 1);
        }


        $feedParamArr['Name'] = $nameArr;

        $feedParamArr['DMI'] = $output->DMI;              # dry matter intake (kg/day)
        $feedParamArr['TDN'] = $output->TDN100;             # total digestible nutrients (g/day)  
        $feedParamArr['ME_day'] = $ME_day;              # metabolizable energy (Mcal/day)
        $feedParamArr['NEl_day'] = $NEl_day;
        $feedParamArr['NEg_day'] = $NEg_day;
        //
        $feedParamArr['CP'] = $output->CP100;                # crude protein intake (g/day)
        $feedParamArr['RUP'] = $output->RUP100;              # ruminally undegradable intake protein (g/day)
        $feedParamArr['RDP'] = $output->RDP100;              # ruminally degradable intake protein (g/day)
        $feedParamArr['NDF'] = $output->NDF100;              # neutral detergent fiber (kg/day)
        $feedParamArr['MCP'] = $MCP;
        $feedParamArr['ME_kg'] = $MEng;              # metabolizable energy (Mcal/day)       
        $feedParamArr['NEl_kg'] = $NEl;             # net energy for lactation (Mcal/day)
        $feedParamArr['NEg_kg'] = $NEg;             # net energy for growth (Mcal/day)
        $feedParamArr['Kp_%hr'] = $Kp;           # predicted passage rate
        ///////////// TOTALS //////////////
        //add total horizontal line
        $nameArr[$totalKey] = 'Total';
        $feedParamArr['DMI'][$totalKey] = round($output->DMI_Total, 1);
        $feedParamArr['TDN'][$totalKey] = round($output->TDN_Total, 0);
        $feedParamArr['ME_day'][$totalKey] = round($output->MEng_Total, 1);
        $feedParamArr['NEl_day'][$totalKey] = round($output->NEl_Total, 1);
        $feedParamArr['NEg_day'][$totalKey] = round($output->NEg_Total, 1);
        //
        $feedParamArr['CP'][$totalKey] = round($output->CP_Total, 0);
        $feedParamArr['RUP'][$totalKey] = round($output->RUP_Total * 1000, 0);
        $feedParamArr['RDP'][$totalKey] = round($output->RDP_Total * 1000, 0);
        $feedParamArr['NDF'][$totalKey] = round($output->NDF_Total, 1);
        $feedParamArr['MCP'][$totalKey] = round($output->MCP_Total, 0);
        $feedParamArr['ME_kg'][$totalKey] = 'N/A';   //no totals are shown for these values             
        $feedParamArr['NEl_kg'][$totalKey] = 'N/A';  //no totals are shown for these values                  
        $feedParamArr['NEg_kg'][$totalKey] = 'N/A';  //no totals are shown for these values                
        $feedParamArr['Kp_%hr'][$totalKey] = 'N/A';  //no totals are shown for these values        

        $feedParamArr['Name'] = $nameArr; //add names back in, becaue rounding sets to 0.

        $feedParamArr = $this->flip2DArray($feedParamArr); //flip to display better in datatgrid
        //replace 'Name' with array key name
        foreach ($feedParamArr as $key => $value) {
            $feedParamArr[$key]['Name'] = $value['Name'];
        }

        //lambda function to round all values in array for display purposes
        $func = function(&$item) {
            if (is_numeric($item) == true) {
                $item = round($item, 1);
            }
        };

        //execute function
        array_walk_recursive($feedParamArr, $func);

        //go through array and take the key to amke a display name inside 
        //2nd dimension of array, then make 1st dimension keys numbers
        //this allows us to to make a JSON array that works.
        $count = 0;
        foreach ($feedParamArr as $key => $value) {
            if (!empty($value)) {
                $feedParamArrFixed[$count] = $value;
                $feedParamArrFixed[$count]['dName'] = $key;
                $count = $count + 1;
            }
        }





        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
          
               {  data: "Name",
               title: "Name" },
         {  data: "DMI",
               title: "DMI" },
          {  data: "TDN",
               title: "TDN" },
          {  data: "ME_day",
               title: "ME_day" },
          {  data: "NEl_day",
               title: "NEl_day" },
          {  data: "NEg_day",
               title: "NEg_day" },
          {  data: "CP",
               title: "CP" },
    {  data: "RDP",
               title: "RDP" },      
   {  data: "RUP",
               title: "RUP" },
         
          {  data: "NDF",
               title: "NDF" },
          {  data: "MCP",
               title: "MCP" },
          {  data: "ME_kg",
               title: "ME_kg" },
          {  data: "NEl_kg",
               title: "NEl_kg" },
          {  data: "MCP",
               title: "MCP" },
          {  data: "NEg_kg",
               title: "NEg_kg" },
          {  data: "Kp_%hr",
               title: "Kp_%hr" }
        
        
EX;


 $str = '';
        $str = $str .($dt->startLocal('Title Local CHANGE ME', $feedParamArrFixed, $columnsStr, null, '  searching: false,
        paging: false,
        ordering: false,
        info: false'));


        $str = $str . ('<ul>');
        $str = $str . ("<li>Increment over Maintenance: " . round($output->DMI_to_DMIMaint, 1) . "X</li>");
        $str = $str . ("<li>Energy/Protein Discount Factor: " . round(abs(1 - $output->Discount) * 100, 1) . "%</li>");
        $str = $str . ("<li>Undiscounted TDN in Diet: " . round(($output->DietTDN) * 100, 1) . "%</li>");
        $str = $str . ("<li>Diet RUP Digestibility: " . round(($output->DietRUPDigest) * 100, 1) . "%</li>");
        $str = $str . ('</ul>');

        return $this->bootstrapBox('Energy and Protein Supply',$str);
    }

    private function summaryReportPage1($input, $output, $mineral) {
        $str = '';
         $str = $str . '<div class="row">' ;
        $str = $str . '<div class="col-md-3" id="">' . $this->animalInputs($input, $output) . '</div>';
        $str = $str . '<div class="col-md-6" id="">' . $this->dietNutrientBalance($output, $mineral) . '</div>';
        $str = $str . '<div class="col-md-3" id="">' . $this->animalPerformanceValues($output) . '</div>';
 $str = $str .'</div>';
        return ($str);
    }

    private function summaryReportPage2($input, $output, $mineral) {
        $str = '';
         $str = $str . '<div class="row">' ;
        $str = $str . '<div class="col-md-4" id="">' . $this->animalProteinValues($output) . $this->dietTargetConcentrations($input, $output, $mineral).'</div>';
       
        $str = $str . '<div class="col-md-4" id="">' . $this->dietConcentrations($input, $output, $mineral) . '</div>';
        
        $str = $str . '<div class="col-md-4" id="">' . $this->plotNRCBalanceBarChartGraph($output) . '</div>';
 $str = $str .'</div>';
        return ($str);
    }

    function dietConcentrations($input, $output, $mineral) {

$html = <<<HTML
 
         <ul>
        <li>NDF: {$this->attention($output->DietNDF * 100, 30, 2, 2, 1, $input->AnimalType)}  (%DM)</li>
        <li>Forage NDF:  {$this->round($output->ForageNDF / $output->DMI_Total * 100, 1)} (%DM)</li>
        <li>ADF: {$this->round($output->DietADF * 100, 1)} (%DM)</li>
        <li>NFC: {$this->round($output->DietaryNFC / $output->DMI_Total * 100, 1)} (%DM)</li>
        <li>Undiscounted TDN: {$this->round($output->DietTDN * 100, 1)} (%DM)</li>
        <li>ME: {$this->round($output->DietME, 2)} (Mcal/kg DM)</li>
        <li>NEl: {$this->round($output->DietNEl, 2)} (Mcal/kg DM)</li>
        <li>NEg: {$this->round($output->DietNEg, 2)} (Mcal/kg DM)</li>
        <li>Ca: {$this->round($mineral[1]->RD * 100, 1)} (%DM)</li>
        <li>P: {$this->round($mineral[2]->RD * 100, 1)} (%DM)</li>
        <li>K: {$this->attention($mineral[5]->RD * 100, 2, .05, .05, 1, $input->AnimalType)} (%DM)</li>
        <li>Ether-Extract (crude fat): {$this->round($output->Fat_Total, 1)} (%DM)</li>  <!-- ether is not referenced in NRC code, ether extract=crude fat. -->
        <li>DCAD: {$this->round($output->DCAD, 1)} (mEq/kg)</li>
        </ul>
           
HTML;
         
               return $this->bootstrapBox('Diet Concentrations',$html);
    }

    function dietTargetConcentrations($input, $output, $mineral) {
        $str = '';
        $str = $str . '<ul>';
        $str = $str . ("<li>NEl: " . round($output->Energy_TargetDietConc, 1) . " (Mcal/kg)</li>");
        $str = $str . ("<li>MP: " . round($output->MP_TargetDietConc, 1) . " (g/kg)</li>");

        $str = $str . ("<br/>");
        $str = $str . ("<li>Ca: " . round($output->Ca_TargetDietConc, 1) . " (g/kg)</li>");
        $str = $str . ("<li>P: " . round($output->P_TargetDietConc, 1) . " (g/kg)</li>");
        $str = $str . '</ul>';

        return $this->bootstrapBox('Target Diet Concentrations',$str);
    }

    /* draws -> to something out of normal range */

    function attention($value, $referenceValue, $safeLowerBound = .2, $safeUpperBound = .2, $roundValue = 2, $AnimalType) {

        //NOTE lower and upperbound ARE ALWAYS POSITIVE NUMBER.
        //only do attention code for lacting cow and not null
        if (($AnimalType != 'Lactating Cow') && (!empty($AnimalType))) {

            return round($value, $roundValue);
        } else {
            $msg = '';
            if ($value >= ($referenceValue - $safeLowerBound) && ($value <= ($referenceValue + $safeUpperBound))) {  //within upper and lower bounds
                $msg = $msg . (new BootStrap)->sucessCallout(round($value, $roundValue)); //slightly lower
            } elseif ($value >= ($referenceValue - 2 * $safeLowerBound) && ($value <= ($referenceValue + 2 * $safeUpperBound))) {      //within 2 TIMES upper and lower bounds
                $msg = $msg . (new BootStrap)->warningCallout(round($value, $roundValue));  //slightly higher
            } else {
                $msg = $msg . (new BootStrap)->dangerCallout(round($value, $roundValue));

                $txt = "Error: " . round($value, $roundValue) . " out of range: (" . round($referenceValue - 2 * $safeLowerBound, $roundValue) . " to " . round($referenceValue + 2 * $safeUpperBound, $roundValue) . "), rebalance!";
                $msg = $msg . (new BootStrap)->errorNotify($txt);
            }
            return $msg;
        }
    }

    function animalPerformanceValues($output) {
        $str = '';

        $str = $str . '<ul>';
        $str = $str . ("<li>DMI - Actual : " . $this->attention($output->DMIActual, $output->DMIPred, .5, .5, 2, null) . " (kg/day)" . "</li>");
        $str = $str . ("<li>DMI - Predicted : " . round($output->DMIPred, 2) . " (kg/day) </li>");
        $str = $str . ("<li>NEl Allowable Milk : " . round($output->EnergyAllowableMilk, 1) . " (kg/day)  </li>");
        $str = $str . ("<li>   MP Allowable Milk : " . round($output->ProteinAllowableMilk, 1) . " (kg/day) </li>");
        $str = $str . ("<li>Milk Production : " . round($output->DailyMilk, 1) . " (kg/day) </li>");
        $str = $str . ("<li>$output->CondMessage </li>");
        $str = $str . ("<li>Daily Weight Change due to Reserves : " . round($output->Reserves_WG, 1) . " (kg/day) </li>");
        $str = $str . '</ul>';
       
        
        return $this->bootstrapBox('Animal Performance',$str);
    }

    function animalProteinValues($output) {
        //error check

        $msgRDP = $this->attention($output->RDPBal, 0, 10, 50, 0, null);
        $msgRUP = $this->attention($output->RUPBal, 0, 10, 50, 0, null);

$html = <<<HTML

         <ul>
      <li>RDP Required {$this->round($output->RDPReq)} (g/day)
       <li>RDP Supplied {$this->round($output->RDPSup)} (g/day)
       <li>RDP Balanced $msgRDP (g/day)
        <li></li>
       <li>RUP Required {$this->round($output->RUPReq)} (g/day)
       <li>RUP Supplied {$this->round($output->RUPSup)} (g/day)
       <li>RUP Balanced $msgRUP (g/day)
       <li></li>
       <li>MP - Bacterial {$this->round($output->MPBact)} (g/day)
       <li>MP - RUP {$this->round($output->TotalDigestedRUP * 1000)} (g/day)
       <li>MP - Endogenous {$this->round($output->MPEndo)} (g/day)
        <li></li>
       <li>CP - Diet {$this->round($output->DietCP * 100, 1)} (% DM)
       <li>CP - RDP {$this->round(($output->RDPSup / 1000) / $output->TotalDMFed * 100, 1)} (% DM)
       <li>CP - RUP {$this->round(($output->RUPSup / 1000) / $output->TotalDMFed * 100, 1)} (% DM)
         </ul>
HTML;
    
        return $this->bootstrapBox('Protein Values',$html);
    }

    /**
     * shows general important parameters
     * 
     * @param type $input
     * @param type $output 
     */
    function animalInputs($input, $output) {
$html = <<<HTML

         <ul>
          <li>Animal Type : {$input->AnimalType}</li>
          <li>Age : {$input->Age} months</li>
          <li>Body Weight : {$input->BW} kg</li>
          <li>Milk Fat : {$input->MilkFat} %</li>
          <li>Days In Milk : {$input->DaysInMilk}</li>
          <li>Milk Production : {$input->MilkProd} (kg/day) </li>
          <li>Days Pregnant : {$input->DaysPreg}</li>
          <li>Breed : {$input->Breed}</li>
          <li>Milk True Protein : {$input->MilkTrueProtein} %</li>
         </ul>
       
HTML;


               return $this->bootstrapBox('Animal Inputs',$html);
    }

    
    
    /*
     * show main things in ration to balance for with min max.
     * 
     */
    public function addicanBalanceParamaters($input, $output) {
        //CP %	RDP  %	RUP %	NDF %	ADF %	NEL (MCal/kg)	NFC % DCAD

    //function attention($value, $referenceValue, $safeLowerBound = .2, $safeUpperBound = .2, $roundValue = 2, $AnimalType) 
        
       
        $out[] = '<ul>';
        $out[] = " <li>CP - Diet  {$this->attention($output->DietCP * 100, 17, 1.4, 1, 1, $input->AnimalType)} (% DM) </li>";
        $out[] = " <li>CP - RUP   {$this->attention( ($output->RUPSup/$output->TotalDMFed*100/1000), 7, 1.5, 1.5, 2, $input->AnimalType)} (% DM)";
        $out[] = " <li>NDF: {$this->attention($output->DietNDF * 100, 30, 2, 3, 1, $input->AnimalType)}  (%DM)</li>";
        $out[] = " <li>ADF: {$this->attention($output->DietADF * 100, 22, 4, 4, 1, $input->AnimalType)}  (%DM)</li>";
        
     //  
        $out[] = ("<li>NEl Allowable Milk : {$this->attention($output->EnergyAllowableMilk/$output->TotalDMFed, 1.7, .5, .5, 3, $input->AnimalType)} (kg/ kg of DM)  </li>"); //WRONG???
        $out[] = ("  <li>NFC: {$this->attention($output->DietaryNFC / $output->DMI_Total * 100, 39, 1, 1, 1, $input->AnimalType)}  (%DM)</li>");
       
      
        $out[] = ("    <li>DCAD:   {$this->attention($output->DCAD, 300, 50, 50, 1, $input->AnimalType)} (mEq/kg)</li>");

        $out[] = '</ul>';




        return $this->bootstrapBox('Major Paramaters', implode($out));
    }

    /**
     * Amino Acid Grid
     *  
     */
    function aminoAcids($output) {
        //////////// Amino Acids ////////////////////
        

        $aminoAcidArr['Arginine']['Amino Acid'] = 'Arginine';
        $aminoAcidArr['Histidine']['Amino Acid'] = 'Histidine';
        $aminoAcidArr['Isoleucine']['Amino Acid'] = 'Isoleucine';
        $aminoAcidArr['Leucine']['Amino Acid'] = 'Leucine';
        $aminoAcidArr['Lysine']['Amino Acid'] = 'Lysine';
        $aminoAcidArr['Methionine']['Amino Acid'] = 'Methionine';
        $aminoAcidArr['Phenylalanine']['Amino Acid'] = 'Phenylalanine';
        $aminoAcidArr['Threonine']['Amino Acid'] = 'Threonine';
        $aminoAcidArr['Valine']['Amino Acid'] = 'Valine';
        $aminoAcidArr[' ']['Amino Acid'] = '';
        $aminoAcidArr['Total']['Amino Acid'] = 'Totals';
        //
        $aminoAcidArr['Arginine']['Flow (g/day)'] = round($output->Arg_Flow);
        $aminoAcidArr['Histidine']['Flow (g/day)'] = round($output->His_Flow);
        $aminoAcidArr['Isoleucine']['Flow (g/day)'] = round($output->Ile_Flow);
        $aminoAcidArr['Leucine']['Flow (g/day)'] = round($output->Leu_Flow);
        $aminoAcidArr['Lysine']['Flow (g/day)'] = round($output->Lys_Flow);
        $aminoAcidArr['Methionine']['Flow (g/day)'] = round($output->Met_Flow);
        $aminoAcidArr['Phenylalanine']['Flow (g/day)'] = round($output->Phe_Flow);
        $aminoAcidArr['Threonine']['Flow (g/day)'] = round($output->Thr_Flow);
        $aminoAcidArr['Valine']['Flow (g/day)'] = round($output->Val_Flow);
        $aminoAcidArr[' ']['Flow (g/day)'] = '';
        $aminoAcidArr['Total']['Flow (g/day)'] = round($output->Total_Flow);
        //
        $aminoAcidArr['Arginine']['DIGAA Flow (g/day)'] = round($output->Dig_Arg_Flow);
        $aminoAcidArr['Histidine']['DIGAA Flow (g/day)'] = round($output->Dig_His_Flow);
        $aminoAcidArr['Isoleucine']['DIGAA Flow (g/day)'] = round($output->Dig_Ile_Flow);
        $aminoAcidArr['Leucine']['DIGAA Flow (g/day)'] = round($output->Dig_Leu_Flow);
        $aminoAcidArr['Lysine']['DIGAA Flow (g/day)'] = round($output->Dig_Lys_Flow);
        $aminoAcidArr['Methionine']['DIGAA Flow (g/day)'] = round($output->Dig_Met_Flow);
        $aminoAcidArr['Phenylalanine']['DIGAA Flow (g/day)'] = round($output->Dig_Phe_Flow);
        $aminoAcidArr['Threonine']['DIGAA Flow (g/day)'] = round($output->Dig_Thr_Flow);
        $aminoAcidArr['Valine']['DIGAA Flow (g/day)'] = round($output->Dig_Val_Flow);
        $aminoAcidArr[' ']['DIGAA Flow (g/day)'] = '';
        $aminoAcidArr['Total']['DIGAA Flow (g/day)'] = round($output->Dig_Total_Flow);
        //
        $aminoAcidArr['Arginine']['Percent of MP'] = round($output->ArgPctMP, 2) . '%';
        $aminoAcidArr['Histidine']['Percent of MP'] = round($output->HisPctMP, 2) . '%';
        $aminoAcidArr['Isoleucine']['Percent of MP'] = round($output->IlePctMP, 2) . '%';
        $aminoAcidArr['Leucine']['Percent of MP'] = round($output->LeuPctMP, 2) . '%';
        $aminoAcidArr['Lysine']['Percent of MP'] = round($output->LysPctMP, 2) . '%';
        $aminoAcidArr['Methionine']['Percent of MP'] = round($output->MetPctMP, 2) . '%';
        $aminoAcidArr['Phenylalanine']['Percent of MP'] = round($output->PhePctMP, 2) . '%';
        $aminoAcidArr['Threonine']['Percent of MP'] = round($output->ThrPctMP, 2) . '%';
        $aminoAcidArr['Valine']['Percent of MP'] = round($output->ValPctMP, 2) . '%';
        $aminoAcidArr[' ']['Percent of MP'] = '';
        $aminoAcidArr['Total']['Percent of MP'] = round($output->TotalPctMP, 2) . '%';
        // 
        $aminoAcidArr['Arginine']['Diff of Optimal'] = '';
        $aminoAcidArr['Histidine']['Diff of Optimal'] = '';
        $aminoAcidArr['Isoleucine']['Diff of Optimal'] = '';
        $aminoAcidArr['Leucine']['Diff of Optimal'] = '';
        $aminoAcidArr['Lysine']['Diff of Optimal'] = round($output->LysPctMP - 6.83, 2) . '%'; //from charles Schwab using NRC value
        $aminoAcidArr['Methionine']['Diff of Optimal'] = round($output->MetPctMP - 2.28, 2) . '%'; //from charles Schwab using NRC value /ratio lys to met should be 3.
        $aminoAcidArr['Phenylalanine']['Diff of Optimal'] = '';
        $aminoAcidArr['Threonine']['Diff of Optimal'] = '';
        $aminoAcidArr['Valine']['Diff of Optimal'] = '';
        $aminoAcidArr[' ']['Diff of Optimal'] = '';
        $aminoAcidArr['Total']['Diff of Optimal'] = '';



        //replace 'Name' with array key name
        foreach ($aminoAcidArr as $key => $value) {
            $aminoAcidArr[$key]['Name'] = $key;
        }

        //lambda function to round all values in array for display purposes
        $func = function(&$item) {
            if (is_numeric($item) == true) {
                $item = round($item, 1);
            }
        };

        //execute function
        array_walk_recursive($aminoAcidArr, $func);

        //go through array and take the key to amke a display name inside 
        //2nd dimension of array, then make 1st dimension keys numbers
        //this allows us to to make a JSON array that works.
        $count = 0;
        foreach ($aminoAcidArr as $key => $value) {
            if (!empty($value)) {
                $aminoAcidArrFixed[$count] = $value;
                $aminoAcidArrFixed[$count]['dName'] = $key;
                $count = $count + 1;
            }
        }





        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
          
               {  data: "Name",
               title: "Name" },
         {  data: "Flow (g/day)",
               title: "Flow (g/day)" },
         {  data: "DIGAA Flow (g/day)",
               title: "DIGAA Flow (g/day)" },
         {  data: "Percent of MP",
               title: "Percent of MP" },
              {  data: "Diff of Optimal",
               title: "Diff of Optimal" }
        
        
EX;

//
        
        $str = '';
        $str = $str . '<ul>';
        $str = $str . ("<li>Total Dry Matter Intake " . round($output->DMI_Total, 1) . " kg/day</li>");
        $str = $str . ("<li>Total RUP " . round($output->RUP_Total * 1000, 0) . " (g/day)</li>");
        $str = $str . ("<li>Total MCP " . round($output->MCP_Total, 0) . " (g/day)</li>");
        $str = $str . ("<li>Total Essential Amino Acids " . round($output->TotalEAA, 0) . " (g/day)</li>");
        $str = $str . '</ul>';
        $str = $str . ("<br/>");
        
         $str = $str .($dt->startLocal('Title Local CHANGE ME', $aminoAcidArrFixed, $columnsStr, null, '  searching: false,
        paging: false,
        ordering: false,
        info: false'));
        
        
        return $this->bootstrapBox('Duodenal Amino Acid Supply',$str);
    }

    function macroMineralDataTable($mineral) {

        $macroMineral = array();
        $macroMineral['AA_Name'] = array_combine(array_keys(get_object_vars($mineral[1])), array_keys(get_object_vars($mineral[1]))); //get keys array to do name.
        $macroMineral['Ca'] = get_object_vars($mineral[1]);
        $macroMineral['P'] = get_object_vars($mineral[2]);
        $macroMineral['Mg'] = get_object_vars($mineral[3]);
        $macroMineral['Cl'] = get_object_vars($mineral[4]);
        $macroMineral['K'] = get_object_vars($mineral[5]);
        $macroMineral['Na'] = get_object_vars($mineral[6]);
        $macroMineral['S'] = get_object_vars($mineral[7]);

        // a pile of mess to rotate a subsection of the array   
        //
        $yy['Units'] = array_column($macroMineral, 'Units', 'Name');
        $yy['Maintenance (Fecal)'] = array_column($macroMineral, 'Fecal', 'Name');
        $yy['Maintenance (Urine, Sweat, Misc.)'] = array_column($macroMineral, 'Urine', 'Name') + array_column($macroMineral, 'Sweat', 'Name') + array_column($macroMineral, 'Misc', 'Name');
        $yy['Pregnancy'] = array_column($macroMineral, 'Fetal', 'Name');
        $yy['Lactation'] = array_column($macroMineral, 'Milk', 'Name');
        $yy['Growth'] = array_column($macroMineral, 'Growth', 'Name');
        $yy['Total Absorbed Required (TAR)'] = array_column($macroMineral, 'Total', 'Name');
        $yy['Total Dietary Supplied'] = $ints = array_map(function($arr) {
            return round($arr, 2);
        }, array_column($macroMineral, 'Supplied', 'Name'));
        $yy['Total Absorbable Supplied (TAS)'] = array_map(function($arr) {
            return round($arr, 2);
        }, array_column($macroMineral, 'Absorbable', 'Name'));
        $yy['Difference (TAS - TAR)'] = array_map(function($arr) {
            return $this->attention($arr, 0, 10, 50, 0, null);
        }, array_column($macroMineral, 'Balance', 'Name'));
        $yy['Ration Density (%)'] = array_column($macroMineral, 'RD', 'Name');


        //replace 'Name' with array key name
        foreach ($yy as $key => $value) {
            $yy[$key]['Name'] = $key;
        }

        //round
        foreach ($yy['Ration Density (%)'] as $key => $value) {
            if (is_numeric($value)) {
                $yy['Ration Density (%)'][$key] = round($value * 100, 2);
            }
        }



        //lambda function to round all values in array for display purposes
        $func = function(&$item) {
            if (is_numeric($item) == true) {
                $item = round($item, 1);
            }
        };

        //execute function
        array_walk_recursive($yy, $func);

        //go through array and take the key to amke a display name inside 
        //2nd dimension of array, then make 1st dimension keys numbers
        //this allows us to to make a JSON array that works.
        $count = 0;
        foreach ($yy as $key => $value) {
            if (!empty($value)) {
                $macroMinArr[$count] = $value;
                $macroMinArr[$count]['dName'] = $key;
                $count = $count + 1;
            }
        }

        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
          
               {  data: "Name",
               title: "Name" },
           {  data: "Ca",
               title: "Ca" },
         {  data: "P",
               title: "P" },
            {  data: "Mg",
               title: "Mg" },
            {  data: "Cl",
               title: "Cl" },
            {  data: "K",
               title: "K" },
            {  data: "Na",
               title: "Na" },
            {  data: "S",
               title: "S" }
EX;


        $str=($dt->startLocal('Title Local CHANGE ME', $macroMinArr, $columnsStr, null, '  searching: false,
        paging: false,
        ordering: false,
        info: false'));
        
        
        return $this->bootstrapBox('Macro-Minerals',$str);
    }

    function microMineralDataTable($mineral) {

     

        $microMineral = array();
        $microMineral['AA_Name'] = array_combine(array_keys(get_object_vars($mineral[1])), array_keys(get_object_vars($mineral[1]))); //get keys array to do name.
        $microMineral['Co'] = get_object_vars($mineral[8]);
        $microMineral['Cu'] = get_object_vars($mineral[9]);
        $microMineral['I'] = get_object_vars($mineral[10]);
        $microMineral['Fe'] = get_object_vars($mineral[11]);
        $microMineral['Mn'] = get_object_vars($mineral[12]);
        $microMineral['Se'] = get_object_vars($mineral[13]);
        $microMineral['Zn'] = get_object_vars($mineral[14]);
        $microMineral['Mo'] = get_object_vars($mineral[15]);

        // a pile of mess to rotate a subsection of the array   
        //
        $yy['Units'] = array_column($microMineral, 'Units', 'Name');
        $yy['Maintenance (Fecal)'] = array_column($microMineral, 'Fecal', 'Name');
        $yy['Maintenance (Urine, Sweat, Misc.)'] = array_column($microMineral, 'Urine', 'Name') + array_column($microMineral, 'Sweat', 'Name') + array_column($microMineral, 'Misc', 'Name');
        $yy['Pregnancy'] = array_column($microMineral, 'Fetal', 'Name');
        $yy['Lactation'] = array_column($microMineral, 'Milk', 'Name');
        $yy['Growth'] = array_column($microMineral, 'Growth', 'Name');
        $yy['Total Absorbed Required (TAR)'] = array_column($microMineral, 'Total', 'Name');
        $yy['Total Dietary Supplied'] = array_map(function($arr) {
            return round($arr, 2);
        }, array_column($microMineral, 'Supplied', 'Name'));
        $yy['Total Absorbable Supplied (TAS)'] = array_map(function($arr) {
            return round($arr, 2);
        }, array_column($microMineral, 'Absorbable', 'Name'));
        $yy['Difference (TAS - TAR)'] = array_map(function($arr) {
            return round($arr, 2);
        }, array_column($microMineral, 'Balance', 'Name'));
        $yy['Ration Density (ppm)'] = array_column($microMineral, 'RD', 'Name');

        //replace 'Name' with array key name
        foreach ($yy as $key => $value) {
            $yy[$key]['Name'] = $key;
        }

        //lambda function to round all values in array for display purposes
        $func = function(&$item) {
            if (is_numeric($item) == true) {
                $item = round($item, 1);
            }
        };

        //execute function
        array_walk_recursive($yy, $func);

        //go through array and take the key to amke a display name inside 
        //2nd dimension of array, then make 1st dimension keys numbers
        //this allows us to to make a JSON array that works.
        $count = 0;
        foreach ($yy as $key => $value) {
            if (!empty($value)) {
                $microMinArr[$count] = $value;
                $microMinArr[$count]['dName'] = $key;
                $count = $count + 1;
            }
        }

        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
          
               {  data: "Name",
               title: "Name" },
           {  data: "Co",
               title: "Co" },
         {  data: "Cu",
               title: "Cu" },
            {  data: "I",
               title: "I" },
            {  data: "Fe",
               title: "Fe" },
            {  data: "Mn",
               title: "Mn" },
            {  data: "Se",
               title: "Se" },
            {  data: "Zn",
               title: "Zn" },
            {  data: "Mo",
               title: "Mo" }
EX;


        $str=($dt->startLocal('Title Local CHANGE ME', $microMinArr, $columnsStr, null, '  searching: false,
        paging: false,
        ordering: false,
        info: false'));
          

         return $this->bootstrapBox('Micro-Minerals',$str);
    }

    function vitaminMineralDataTable($mineral) {

    
        $macroMineral = array();
        $macroMineral['AA_Name'] = array_combine(array_keys(get_object_vars($mineral[1])), array_keys(get_object_vars($mineral[1]))); //get keys array to do name.
        $macroMineral['VitA'] = get_object_vars($mineral[16]);
        $macroMineral['VitD'] = get_object_vars($mineral[17]);
        $macroMineral['VitE'] = get_object_vars($mineral[18]);

        //delete first value of all internal arrays
        foreach ($macroMineral as $key => $value) {
            array_shift($value);
            $macroMineral[$key] = $value;
        }


        // a pile of mess to rotate a subsection of the array   
        //
        $yy['Units'] = array_column($macroMineral, 'Units', 'Name');
        $yy['Maintenance (Fecal)'] = array_column($macroMineral, 'Fecal', 'Name');
        $yy['Maintenance (Urine, Sweat, Misc.)'] = array_column($macroMineral, 'Urine', 'Name') + array_column($macroMineral, 'Sweat', 'Name') + array_column($macroMineral, 'Misc', 'Name');
        $yy['Pregnancy'] = array_column($macroMineral, 'Fetal', 'Name');
        $yy['Lactation'] = array_column($macroMineral, 'Milk', 'Name');
        $yy['Growth'] = array_column($macroMineral, 'Growth', 'Name');
        $yy['Total Absorbed Required (TAR)'] = array_column($macroMineral, 'Total', 'Name');
        $yy['Total Dietary Supplied'] = array_column($macroMineral, 'Supplied', 'Name');
        $yy['Total Absorbable Supplied (TAS)'] = array_column($macroMineral, 'Absorbable', 'Name');
        $yy['Difference (TAS - TAR)'] = array_column($macroMineral, 'Balance', 'Name');
        $yy['Ration Density (ppm)'] = array_column($macroMineral, 'RD', 'Name');


        //lambda function to round all values in array for display purposes
        $func = function(&$item) {
            if (is_numeric($item) == true) {
                $item = round($item, 1);
            }
        };

        //execute function
        array_walk_recursive($yy, $func);


        //go through array and take the key to make a display name inside 
        //2nd dimension of array, then make 1st dimension keys numbers
        //this allows us to to make a JSON array that works.
        $count = 0;
        foreach ($yy as $key => $value) {
            if (!empty($value)) {
                $vitArray[$count] = $value;
                $vitArray[$count]['dName'] = $key;
                $count = $count + 1;
            }
        }




        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
           
               {  data: "dName",
               title: "Name" },
           {  data: "1",
               title: "Vit A" },
            {  data: "2",
               title: "Vit D" },
            {  data: "3",
               title: "Vit E" }
EX;


        $str=($dt->startLocal('Title Local CHANGE ME', $vitArray, $columnsStr, null, '  searching: false,
        paging: false,
        ordering: false,
        info: false'));
 
          return $this->bootstrapBox('Vitamins',$str);
    }

    /**
     * Diet Nutrient Balances Chart
     *   //FIXME: not done yet.
     */
    function dietNutrientBalance($output, $mineral) {
        $str = '';
        
        $dietNutrientBlanaces['Maintenance']['Requirements'] = 'Maintenance';
        $dietNutrientBlanaces['Pregnancy']['Requirements'] = 'Pregnancy';
        $dietNutrientBlanaces['Lactation']['Requirements'] = 'Lactation';
        $dietNutrientBlanaces['Growth']['Requirements'] = 'Growth';
        $dietNutrientBlanaces['Total Required']['Requirements'] = 'Total Required';
        $dietNutrientBlanaces['Total Supplied']['Requirements'] = 'Total Supplied';
        $dietNutrientBlanaces['Balance']['Requirements'] = 'Balance';
        //   
        $dietNutrientBlanaces['Maintenance']['NEL (Mcal/day)'] = round($output->NEMaint, 1);
        $dietNutrientBlanaces['Pregnancy']['NEL (Mcal/day)'] = round($output->NEPreg, 1);
        $dietNutrientBlanaces['Lactation']['NEL (Mcal/day)'] = round($output->NELact, 1);
        $dietNutrientBlanaces['Growth']['NEL (Mcal/day)'] = round($output->NEGrowth, 1);
        $dietNutrientBlanaces['Total Required']['NEL (Mcal/day)'] = round(($output->NEMaint + $output->NEPreg + $output->NELact + $output->NEGrowth), 1);
        $dietNutrientBlanaces['Total Supplied']['NEL (Mcal/day)'] = round($output->NEl_Total, 1);
        $dietNutrientBlanaces['Balance']['NEL (Mcal/day)'] = round(($dietNutrientBlanaces['Total Supplied']['NEL (Mcal/day)'] - $dietNutrientBlanaces['Total Required']['NEL (Mcal/day)']), 1);
        //    
        $dietNutrientBlanaces['Maintenance']['MP (g/day)'] = round($output->MPMaint, 0);
        $dietNutrientBlanaces['Pregnancy']['MP (g/day)'] = round($output->MPPreg, 0);
        $dietNutrientBlanaces['Lactation']['MP (g/day)'] = round($output->MPLact, 0);
        $dietNutrientBlanaces['Growth']['MP (g/day)'] = round($output->MPGrowth, 0);
        $dietNutrientBlanaces['Total Required']['MP (g/day)'] = round($output->MPRequired, 0);
        $dietNutrientBlanaces['Total Supplied']['MP (g/day)'] = round($output->MPSupplied, 0);
        $dietNutrientBlanaces['Balance']['MP (g/day)'] = round($output->MPBalance, 0);
        //$dietNutrientBlanaces['Balance']['MP (g/day)'] = $output->MPBalance;


        $dietNutrientBlanaces['Maintenance']['Ca (g/day)'] = round($mineral[1]->Maint);
        $dietNutrientBlanaces['Pregnancy']['Ca (g/day)'] = round($mineral[1]->Fetal);
        $dietNutrientBlanaces['Lactation']['Ca (g/day)'] = round($mineral[1]->Milk);
        $dietNutrientBlanaces['Growth']['Ca (g/day)'] = round($mineral[1]->Growth);
        $dietNutrientBlanaces['Total Required']['Ca (g/day)'] = round($mineral[1]->Total);
        $dietNutrientBlanaces['Total Supplied']['Ca (g/day)'] = round($mineral[1]->Absorbable);
        $dietNutrientBlanaces['Balance']['Ca (g/day)'] = round($mineral[1]->Balance);
        //    
        $dietNutrientBlanaces['Maintenance']['P (g/day)'] = round($mineral[2]->Maint);
        $dietNutrientBlanaces['Pregnancy']['P (g/day)'] = round($mineral[2]->Fetal);
        $dietNutrientBlanaces['Lactation']['P (g/day)'] = round($mineral[2]->Milk);
        $dietNutrientBlanaces['Growth']['P (g/day)'] = round($mineral[2]->Growth);
        $dietNutrientBlanaces['Total Required']['P (g/day)'] = round($mineral[2]->Total);
        $dietNutrientBlanaces['Total Supplied']['P (g/day)'] = round($mineral[2]->Absorbable);
        $dietNutrientBlanaces['Balance']['P (g/day)'] = round($mineral[2]->Balance);
        //    
        $dietNutrientBlanaces['Maintenance']['K (g/day)'] = round($mineral[5]->Maint);
        $dietNutrientBlanaces['Pregnancy']['K (g/day)'] = round($mineral[5]->Fetal);
        $dietNutrientBlanaces['Lactation']['K (g/day)'] = round($mineral[5]->Milk);
        $dietNutrientBlanaces['Growth']['K (g/day)'] = round($mineral[5]->Growth);
        $dietNutrientBlanaces['Total Required']['K (g/day)'] = round($mineral[5]->Total);
        $dietNutrientBlanaces['Total Supplied']['K (g/day)'] = round($mineral[5]->Absorbable);
        $dietNutrientBlanaces['Balance']['K (g/day)'] = round($mineral[5]->Balance);

        //use datatable
        
        
        
         //go through array and take the key to amke a display name inside 
        //2nd dimension of array, then make 1st dimension keys numbers
        //this allows us to to make a JSON array that works.
        $count = 0;
        foreach ($dietNutrientBlanaces as $key => $value) {
            if (!empty($value)) {
                $dietNutrientBlanacesFixed[$count] = $value;
                $dietNutrientBlanacesFixed[$count]['dName'] = $key;
                $count = $count + 1;
            }
        }

        $dt = new JQueryDataTable();
        $columnsStr = <<<EX
          
               {  data: "Requirements",
               title: "Requirements" },
         {  data: "NEL (Mcal/day)",
               title: "NEL (Mcal/day)" },
          {  data: "MP (g/day)",
               title: "MP (g/day)" },
          {  data: "Ca (g/day)",
               title: "Ca (g/day)" },
          {  data: "P (g/day)",
               title: "P (g/day)" },
          {  data: "K (g/day)",
               title: "K (g/day)" }
       
EX;

        $str = $str .($dt->startLocal('Title Local CHANGE ME', $dietNutrientBlanacesFixed, $columnsStr, null, '  searching: false,        paging: false,        ordering: false,        info: false'));     
        $str = $str . "<p>Note: Total Supplied is Total Absorbable for Ca,P, and K.</p>";

       return $this->bootstrapBox('Diet Nutrient Balances',$str);
    }

    /**
     * flips a 2d array
     * @param type $arr
     * @return type 
     */
    function flip2DArray($arr) {
        $out = array();

        foreach ($arr as $key => $subarr) {
            foreach ($subarr as $subkey => $subvalue) {
                $out[$subkey][$key] = $subvalue;
            }
        }

        return $out;
    }

    public function editRecipeAnimalInputParamQuickform($recipe_id = 1, $title = null) {


        $form = new HTML_QuickForm("editRecipeAnimalInputParamQuickform_$recipe_id", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('html', "<h3>Edit Animal Inputs: $title (Recipe #$recipe_id)</h3>"); //set title
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipe_id', $recipe_id);
        $form->addElement('hidden', 'editrecipeid', $recipe_id); //very important, this variable is looked for at the start of the class, the form validation does not work without it.
        //
        $form->addElement('text', 'Age', 'Age (months)');
        $form->addElement('text', 'BW', 'Body Weight');
        $form->addElement('text', 'DaysPreg', 'Days Pregnant');
        $form->addElement('text', 'CS', 'Body Condition Score');
        $form->addElement('text', 'DaysInMilk', 'Days In Milk');
        $form->addElement('text', 'LactNum', 'Lactation #');
        $form->addElement('text', 'FirstCalf', 'Age at First Calving');
        $form->addElement('text', 'MilkProd', 'Milk Production');
        $form->addElement('text', 'MilkFat', 'Milk Fat %');
        $form->addElement('text', 'MilkTrueProtein', 'Milk True Protein %');
        $form->addElement('text', 'Lactose', 'Lactose %');
        /*
          //override default inputs.
          $input->Age = 26;            # age  (months)
          $input->BW = 450;             # body weight
          $input->DaysPreg = 15;      # days pregnant
          $input->CS = 3.0;             # condition score (1 - 5)
          $input->DaysInMilk = 120;    # days in milk
          $input->LactNum =1;          # lactation number
          $input->FirstCalf = 24;      # age at first calving  (months)
          $input->MilkProd = 31;           # milk production
          $input->MilkFat = 3.85;            # milk fat  (%)
          $input->MilkTrueProtein = 3.2;    # milk true protein (%)
          $input->Lactose = 4.8;            # milk lactose (%)
         */


        //select the items.
        $sql = "SELECT * FROM nutrition.nrc_recipe_animal_input where nrc_recipe=$recipe_id limit 1";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);  //get data
        $row = $results[0];
        //set default dry matter values
        $form->setDefaults(array("Age" => $row['Age'], "BW" => $row['BW'], 'DaysPreg' => $row['DaysPreg'], 'CS' => $row['CS'], 'DaysInMilk' => $row['DaysInMilk'], 'LactNum' => $row['LactNum'], 'FirstCalf' => $row['FirstCalf'], 'MilkProd' => $row['MilkProd'], 'MilkFat' => $row['MilkFat'], 'MilkTrueProtein' => $row['MilkTrueProtein'], 'Lactose' => $row['Lactose']));



        //submit buttons

        $submitGroup[] = $form->createElement('reset', 'btnReset', 'Reset');
        $submitGroup[] =  $form->createElement('button', 'action', 'Cancel', array("onClick" => "{window.location.href='?pageid={$_REQUEST['pageid']}';}")); //cancel javascript button
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $form->freeze();
            $form->process(array('NRC2001', 'editRecipeAnimalInputParamQuickformProcess'));   //Callback, either function name or array(&$object, 'method') 
        } //end validation
        else {
            $form->accept($renderer);
            return $renderer->toHtml();
        }
    }

    function editRecipeAnimalInputParamQuickformProcess($values) {

        //get values
        $pageid = $values['pageid'];
        $recipe_id = $values['recipe_id'];
        $recipeID = $recipe_id;

       // print_r2($values);



        $sql = "SELECT nrc_recipe FROM nutrition.nrc_recipe_animal_input WHERE nrc_recipe=$recipe_id LIMIT 1";
        $res2 = $GLOBALS['pdo']->query($sql);



        /* Check the number of rows that match the SELECT statement */
        if ($res2->fetchColumn() == 0) {

            $sql2 = ('INSERT INTO nutrition.nrc_recipe_animal_input (nrc_recipe,"Age","BW","DaysPreg","CS","DaysInMilk","LactNum","FirstCalf","MilkProd","MilkFat","MilkTrueProtein","Lactose")'
                    . "VALUES (:nrc_recipe, :Age, :BW, :DaysPreg, :CS, :DaysInMilk,:LactNum,:FirstCalf,:MilkProd,:MilkFat,:MilkTrueProtein,:Lactose )");
        } else {



            $sql2 = 'UPDATE nutrition.nrc_recipe_animal_input SET
           "Age" = :Age, 
           "BW" = :BW,  
           "DaysPreg" = :DaysPreg,  
           "CS" = :CS,  
           "DaysInMilk" = :DaysInMilk,  
           "LactNum" = :LactNum,  
           "FirstCalf" = :FirstCalf,  
           "MilkProd" = :MilkProd,  
           "MilkFat" = :MilkFat,  
           "MilkTrueProtein" = :MilkTrueProtein, 
           "Lactose" = :Lactose
           WHERE nrc_recipe = :nrc_recipe';
        }


        try {
            $stmt = $GLOBALS['pdo']->prepare($sql2);

            $stmt->bindValue(':nrc_recipe', $values['recipe_id'], PDO::PARAM_STR);
            $stmt->bindValue(':Age', empty($values['Age']) ? 0 : $values['Age'], PDO::PARAM_STR);
            $stmt->bindValue(':BW', empty($values['BW']) ? 0 : $values['BW'], PDO::PARAM_STR);
            $stmt->bindValue(':CS', empty($values['CS']) ? 0 : $values['CS'], PDO::PARAM_STR);
            $stmt->bindValue(':DaysPreg', empty($values['DaysPreg']) ? 0 : $values['DaysPreg'], PDO::PARAM_STR);
            $stmt->bindValue(':DaysInMilk', empty($values['DaysInMilk']) ? 0 : $values['DaysInMilk'], PDO::PARAM_STR);
            $stmt->bindValue(':LactNum', empty($values['LactNum']) ? 0 : $values['LactNum'], PDO::PARAM_STR);
            $stmt->bindValue(':FirstCalf', empty($values['FirstCalf']) ? 0 : $values['FirstCalf'], PDO::PARAM_STR);
            $stmt->bindValue(':MilkProd', empty($values['MilkProd']) ? 0 : $values['MilkProd'], PDO::PARAM_STR);
            $stmt->bindValue(':MilkFat', empty($values['MilkFat']) ? 0 : $values['MilkFat'], PDO::PARAM_STR);
            $stmt->bindValue(':MilkTrueProtein', empty($values['MilkTrueProtein']) ? 0 : $values['MilkTrueProtein'], PDO::PARAM_STR);
            $stmt->bindValue(':Lactose', empty($values['Lactose']) ? 0 : $values['Lactose'], PDO::PARAM_STR);
            $stmt->execute();
        } catch (PDOException $e) {
            echo 'Connection failed: ' . $e->getMessage();
            exit;
        }

        // Close the transaction
        //////////////////////////////////////////
        //forward to same page, ob_start() is key to making this work!
        header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&editrecipeid=$recipeID");
        exit();
    }

    public function editRecipeQuickform($recipe_id = 1, $title = null) {
        $form = new HTML_QuickForm("editRecipeQuickform_$recipe_id", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('html', "<h3>Edit: $title (Recipe #$recipe_id)</h3>"); //set title
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'recipe_id', $recipe_id);
        $form->addElement('hidden', 'editrecipeid', $recipe_id); //very important, this variable is looked for at the start of the class, the form validation does not work without it.
        //select the items.
        $sql = "SELECT feed_library_name,kg_day_dry FROM nutrition.nrc_recipe_item where nrc_recipe=$recipe_id ORDER BY kg_day_dry DESC";
        $res = $GLOBALS['pdo']->query($sql);
        $currentlyUsedFeedsArray = array();
        while (($row = $res->fetch((PDO::FETCH_ASSOC)))) {
            $currentlyUsedFeedsArray[$row['feed_library_name']] = "{$row['feed_library_name']}";
            $displayText = $row['feed_library_name'];
            $id = $row['feed_library_name'];
            $defaultValue = $row['kg_day_dry'];

            //create group of items and delete buttons.
            $item['id'] =  $form->createElement('hidden', 'id', "$id");
               $item['sss']= $form->createElement('static', 'dddd');
            $item['kg_day_dry'] =  $form->createElement('text', 'kg_day_dry', null, array('size' => 4));
            $item['delete'] =  $form->createElement('checkbox', 'delete', null, 'delete');
         
            $form->addGroup($item, "recipeitem_$id", "$displayText", '&nbsp;');

            //set default dry matter values
            $form->setDefaults(array("recipeitem_$id" => array('kg_day_dry' => "$defaultValue", 'delete' => '')));
        }

        //grab a list of current feeds (for new item) and make sure they are not already in the recipe. Only shows current bags with feed tests.
        $sql = "SELECT \"Feed Name\" as feed_library_name FROM (Select * from nutrition.bag_analysis_overlay_feed_library_bag_current()
UNION 
Select * from nutrition.feed_library_nrc2001
UNION 
Select * from nutrition.feed_library_local
) x order by feed_library_name";
        $res8 = $GLOBALS['pdo']->query($sql);
        $feedType[null] = "Please Select one..."; //dummy value
        while (($row8 = $res8->fetch((PDO::FETCH_ASSOC)))) {
            //parse for last word of name and then make an array for the select list.
            if (is_array($currentlyUsedFeedsArray) == true) {
                if (array_key_exists($row8['feed_library_name'], $currentlyUsedFeedsArray) == false) {
                    $feedType[$row8['feed_library_name']] = $row8['feed_library_name'];
                }
            } else { //this is called when recipe has 0 items in it.
                $feedType[$row8['feed_library_name']] = $row8['feed_library_name'];
            }
        }



        //now add option for new recipe items.
        $newItem['feed_type'] =  $form->createElement('select', 'feed_type', 'Feed Type', $feedType);
        $newItem['kg_day_dry'] =  $form->createElement('text', 'kg_day_dry', null, array('size' => 4));
        $form->addGroup($newItem, 'recipeitemnew', "New Recipe Item (item,kg dry matter / day)", ',&nbsp;');



        //submit buttons

        $submitGroup[] = $form->createElement('reset', 'btnReset', 'Reset');
        $submitGroup[] =  $form->createElement('button', 'action', 'Cancel', array("onClick" => "{window.location.href='?pageid={$_REQUEST['pageid']}';}")); //cancel javascript button
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        // Try to validate a form
        if ($form->validate()) {

            $form->freeze();
            $form->process(array('NRC2001', 'editRecipeQuickformProcess'));   //Callback, either function name or array(&$object, 'method') 
        } //end validation
        else {
            $form->accept($renderer);
            return $renderer->toHtml();
        }
    }

    /** search array keys for pattern and return array from PEAR comments */
    private function preg_grep_keys($pattern, $input, $flags = 0) {
        $keys = preg_grep($pattern, array_keys($input), $flags);
        $vals = array();
        foreach ($keys as $key) {
            $vals[$key] = $input[$key];
        }
        return $vals;
    }

    function editRecipeQuickformProcess($values) {



        //get values
        $pageid = $values['pageid'];
        $recipe_id = $values['recipe_id'];
        $recipeID = $recipe_id;

        //////////////////////////
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            /* Do recipe items */

            //create array to search later from recipe item data.
            $recipeItemArray = self::preg_grep_keys('/recipeitem_/', $values);

            //go through array and see if data has changed for each kg_day_dry
            foreach ($recipeItemArray as $key => $val) {
                $sql = "SELECT kg_day_dry FROM nutrition.nrc_recipe_item WHERE nrc_recipe_item.feed_library_name='{$val['id']}' and nrc_recipe=$recipe_id LIMIT 1";
                $res2 = $GLOBALS['pdo']->query($sql);
                $row2 = $res2->fetch((PDO::FETCH_ASSOC));
                //update or delete data if necessary
                if ($val['delete'] == true) {
                    $query = "DELETE FROM nutrition.nrc_recipe_item WHERE nrc_recipe_item.feed_library_name='{$val['id']}' and nrc_recipe=$recipe_id";
                    $upd2 = $GLOBALS['pdo']->exec($query);
                } elseif ($row2['kg_day_dry'] != $val['kg_day_dry']) {
                    $query = "UPDATE nutrition.nrc_recipe_item SET  kg_day_dry = '{$val['kg_day_dry']}' WHERE nrc_recipe_item.feed_library_name='{$val['id']}' and nrc_recipe=$recipe_id";
                    $upd2 = $GLOBALS['pdo']->exec($query);
                }
            }

            //see if the new recipe item field has been used or not.
            if ($values['recipeitemnew']['feed_type'] != null) {

                //do an insert of new recipe item
                $query = "INSERT INTO nutrition.nrc_recipe_item (nrc_recipe,feed_library_name,kg_day_dry) VALUES ($recipe_id,'{$values['recipeitemnew']['feed_type']}',{$values['recipeitemnew']['kg_day_dry']})";
                $res2_1 = $GLOBALS['pdo']->exec($query);
            }

            print_r($values['recipeitemnew']);


            /* end of recipe items */
            // determine if the commit or rollback    
            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
             echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
        }


        // Close the transaction
        //////////////////////////////////////////
        //forward to same page, ob_start() is key to making this work!
        header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&editrecipeid=$recipeID");
        exit();
    }

    /**
     * SOURCE: http://expeng.anr.msu.edu/uploads/files/15/Spartan%20Dairy%20Help%20Manual%20May%202011.pdf
     *  Used without permission. Taken verbatim.
     */
    function howToBalanceARation() {

$html = <<<HTML
        Steps for Balancing a Lactating Cow Ration:
        <p>1. Describe the target cow and choose feeds.</p> 

        <p>2. If you want to use a high-fiber byproduct feed, choose up front how much you want to use. There are many factors to consider when making this decision, but one that is helpful is the cost per unit energy. You can examine the cost per unit ME in the Feed Relationships dialog. Select the feed row after entering the price you can purchase it for, and then choose Feeds > Feed Relationships. Spartan Dairy Help Manual Michigan State University Page 91 
        </p><p>
            3. Forage NDF. Add forages to make up about half the diet and adjust so that the Forage NDF requirement is just met. To make this easy, turn on the ForNDF display in the Nutrient Balance Graph. • Instead of using Forage NDF, you could add forages to about 90% of the Effective NDF requirement. Because grains have some Effective NDF in the Spartan 3 system, you do not need to be at 100% of the Effective NDF requirement yet. • For animals with low requirements, you may want to feed well above the Forage NDF requirement. 
        </p><p>
            4. Effective fiber and energy. Add grains (concentrates) to make up most of the remaining predicted DMI (leave ~2% of predicted DMI for mineral and vitamin supplements). • Adjust forages and grains to meet Effective NDF and NEL requirements (just substitute in one for the other on a pound for pound basis). It may be impossible to do both–in such case, make sure diet has adequate effective fiber as the first priority. Being short on energy means the cow will mobilize body reserves, which is okay for awhile. • If you want to try some fat, include that in the grain portion. But recognize that the cow may eat less than predicted if you add fat, especially oils. You can track fatty acid fractions in the lipid tab. • You may want to examine other indicators of fiber and energy nutrition as checks. For fiber, you can examine NDF, ForNDF, and EfNDF; for energy, examine NEL, NEL01, NEL89, or ME, ME01, and ME89. For heifers, examine NEm, NEg, or ME alternatives. The three systems (Spartan 3, NRC 2001, and NRC 1989) will give slightly different estimates of the diets energy availability. 
        </p><p>
            5. Protein. Balance the concentrates to meet the Crude Protein and/or Metabolizable Protein requirements. • For every pound of protein supplement included, take out a pound of grain. • If you first balance for CP, then check to see if the MP requirement is met (the RUP and digestible RUP requirements will give the same info). Replace some of the protein supplement with a high RUP supplement if needed. • Make sure the RDP requirement is met. A shortage of RDP will decrease microbial protein yield, and therefore increase the requirement for RUP. • You may want to examine MP01 as another check on the diet. • Check amino acids. It is okay if they are a bit short – even 10 to 20% short is probably okay, especially if MP is higher than needed. 
        </p><p>
            6. Minerals and vitamins. Add minerals and vitamins last because their supplementation (especially for Ca and P) depends on the feeds used to meet fiber, energy, and protein. • Be sure to use the mineral and vitamin supplements that are actually available on the farm. If you have a feed tag with nutrient specifications, copy these into the Spartan program. Remember that feed tags usually report nutrients on an as-fed basis and that Spartan data is on a dry matter basis. For dry mineral supplements (98% DM), this distinction is not important. • Note that most minerals can be considered on both a total and an absorbed basis. Both bases may be useful. For some minerals, such as Na, K, or Cl, the absorption coefficients are so high that the absorbed basis was deemed to add no value. • Note that most feeds in the Spartan 3 libraries do not have values assigned for concentrations of trace minerals and vitamins. This is because the concentrations vary considerably depending on soil type and other factors. If you have values Spartan Dairy Help Manual Michigan State University Page 92 from a good feed analysis, you should go ahead and include those concentrations for the feed. • We recommend that TM and vitamins be added to diets so that the supplement is at least 50% of the total requirement. For some TM and vitamins, such as Se, meeting the requirement is so beneficial that you should meet 100% of the requirement with the supplement. • Pay special attention to Se, which has a maximum legal limit on inclusion. • If a complete min/vit supplement is used, add it to meet the Se reqt and then inspect everything else • If separate supplements are used, add them in a systematic order. First balance for P, then Ca, then other macrominerals, then trace minerals and vitamins. For example, you might first add dicalcium phosphate to meet P requirement, then add limestone to meet Ca requirement, add salt to meet Na requirement, use a TM supplement to meet Se requirement, and finally add a vitamin supplement to meet vit A or E. • After you are finished adding mineral and vitamin supplements, check the mineral, trace mineral, and vitamin tabs to ensure that all mineral and vitamin are adequately supplied. 
        </p><p>
            7. Go back and check everything one last time. 
        </p><p>
            8. Evaluate the actual response to this new diet by monitoring feed intake and milk yield and then fine tune the diet accordingly. 
        </p><p>
            9. DM content of silages should be checked regularly.      
        </p>      
        
HTML;
return $html;

      
    }

}

/* supports both individual and group */

class AccordionImplementationNRC2001Quickforms extends AccordionSub {

    public static $recipeID;
    public static $title;

    /* NRC 2001 only works with individual recipes. */

    public function __construct($recipeID = null, $title = null) {
        

        if (!filter_var($recipeID, FILTER_VALIDATE_INT) === false) {
            self::$recipeID = $recipeID;
            self::$title = $title;
        } else {
            throw new Exception("Error Accordian RecipeID is invalid: ($recipeID), $e.");
        }
    }

    function tab1() {
        return NRC2001::editRecipeQuickform(self::$recipeID, self::$title);
    }

    function tab2() {
       
        return NRC2001::editRecipeAnimalInputParamQuickform(self::$recipeID, self::$title);
    }

    function tab3() {
       
       return  NRC2001::editRecipeLocationsQuickform(self::$recipeID, self::$title);
    }

    function tab4() {

       
        return NRC2001::editLinkFeedsNRCtoOnFarmQuickform(self::$recipeID, self::$title);
    }

}

//end class