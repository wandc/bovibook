<?php 
class SalePrice extends TabPage {

    private $bovine_id;
 
    public function defaultDisplay() {
     
        //fill class variables.
       array_key_exists('bovine_id',$_REQUEST) ? $_REQUEST['bovine_id'] : null;
        if (!empty($_REQUEST['bovine_id'])) {
            $this->bovine_id = $_REQUEST['bovine_id'];
        }
        
       
/*
        $b=new QF2_SalesListCustom((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($b->renderBox('qf2_SalesListCustomEvent2','Select Animals to Add to Custom Sales List SECONDARY'));  
        print($b->formJS());
  */      
        
        //setup for tabs.
        $tabArray[1]['name'] = 'Current Prices';
        $tabArray[2]['name'] = 'Custom List Primary';
        $tabArray[3]['name'] = 'Custom List Secondary';
        $tabArray[4]['name'] = 'Custom List Low pro$ Calves';
        $tabArray[5]['name'] = 'Fresh List';
        $tabArray[6]['name'] = 'Pregnant List';
        $tabArray[7]['name'] = 'Springer Heifer List';
        $tabArray[8]['name'] = 'Due Soon List';
        $tabArray[9]['name'] = 'Recent Sold (cull) Buyer Output';
        $tabArray[10]['name'] = 'Recent Sold (custom primary list) Buyer Output';
        
        $this->setCSStabs('jquery_tabs_sale_price');

        

        
            $this->renderTabs($tabArray);
        }
       
    

    function customSQL() {

        // create some temp tables to speed things up.
        $sql = "CREATE TEMP TABLE specific_cow_price AS 
		SELECT sale_price.id,event_time,price,plus,comment,custom_comment,userid,bovine_id
		FROM bovinemanagement.sale_price
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id = sale_price_comment.id
";
        $res = $GLOBALS['pdo']->exec($sql);
    }

    function tab1() {
    print($this->displayListOfSalePrices());
    }

    
    //primary
    function tab2() {
    
         $a=new QF2_SalesListCustom((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'custom');
        print($a->renderBox('qf2_SalesListCustomEvent1','Select Animals to Add to Custom Sales List PRIMARY'));  
        print($a->formJS());
        print($this->displaySalesList('custom'));
    }

    //secondary
    function tab3() {

        $a=new QF2_SalesListCustom((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'custom_secondary');
        print($a->renderBox('qf2_SalesListCustomEvent2','Select Animals to Add to Custom Sales List SECONDARY'));  
        print($a->formJS_Secondary());
        print($this->displaySalesList('custom_secondary'));

    }
    
     //low pro$ calves
    function tab4() {

        $a=new QF2_SalesListCustom((object) array('forwardURLArr'=>array('pageid'=>$_REQUEST['pageid'])),'custom_lowProDollarcalves');
        print($a->renderBox('qf2_SalesListCustomEvent3','Select Animals to Add to Custom Sales List Low Pro$ Calves'));  
        print($a->formJS_Secondary());
        print($this->displaySalesList('custom_lowProDollarcalves'));

    }
    

    function tab5() {
        print($this->displaySalesList('fresh'));
    }

    function tab6() {
        print($this->displaySalesList('pregnant'));
    }

    function tab7() {
        print($this->displaySalesList('springer'));
    }

    function tab8() {
        print($this->displaySalesList('dueSoon'));
    }
    
    
    /*
     * print medical and repro report to give to buyer from list based on cull sold.
     */
     function tab9() {
      $sql="SELECT * FROM bovinemanagement.cull_event
LEFT JOIN bovinemanagement.bovine ON cull_event.bovine_id=bovine.id 
WHERE event_time >= now() - interval '5 days' AND reason='Sold' ORDER BY local_number";
             $res = $GLOBALS ['pdo']->query($sql);
       
       while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
       
      $this->soldCowInfo($row['bovine_id'],$row['local_number'],$row['full_name'],$row['full_reg_number'],$row['birth_date']);
        
       }
      
      
    }
    
     /*
     * print medical and repro report to give to buyer from list based on custom list.
     */
     function tab10() {
      $sql="select bovine_id,local_number,full_name,full_reg_number,birth_date from system.salesreport_temp
left join bovinemanagement.bovine on bovine_id=id order by local_number";
             $res = $GLOBALS ['pdo']->query($sql);
       
       while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
       
      $this->soldCowInfo($row['bovine_id'],$row['local_number'],$row['full_name'],$row['full_reg_number'],$row['birth_date']);
        
       }
      
      
    }
    
    
    /*
     * info to give buyer when they buy the cow.
     */
    function soldCowInfo($bovine_id,$local_number,$full_name,$full_reg_number,$birth_date) {
        
        print("<h1>#{$local_number} - {$full_name}</h1>");   
        print("<h2>{$full_reg_number}</h2>");   
        print("<h3>Born: ".date('M d, Y',strtotime($birth_date))."</h3>");   
        $number2 = "<a href=\"" . HolsteinCanadaHelper::createHolsteinCanadaQuery($full_reg_number) . "\"> <b>HOL</b>" . "</a>";
        $number3 = "<a href=\"" . $GLOBALS['MiscObj']->createCDNLink($full_reg_number) . "\"> <b>CDN</b>" . "</a>";
        print(  $number2 . "&nbsp;&nbsp;" . $number3);
        
        
        
        //repro
        print("<br/><br/>");   
        print("<h4>Repro Log:</h4>");   
        print ('<div id="bovine_query_log">');      
        include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryReproductionLog.inc');
        $bovineQueryReproductionLog=new BovineQueryReproductionLog($bovine_id,$full_reg_number);
        print($bovineQueryReproductionLog->main(true));
        print ('</div>');
          print('<br/><br/>');
        ///medical
          print("<h4>Medical Log:</h4>"); 
                 print ('<div id="bovine_query_log">');   
         include_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/bovineQueryMedicalLog.inc');
        $bovineQueryMedicalLog=new BovineQueryMedicalLog($bovine_id,$full_reg_number);
        print($bovineQueryMedicalLog->main());
        print ('</div>');
        
        print('<hr style="height:20px;border:none;color:#333;background-color:#333;" /><br/><br/><br/><br/>');  
        
        
    }
    
    
    
    function salePriceCowQuickForm($bovine_id) {
        //grab name/number info on current bovine
        $sql = "SELECT id as bovine_id,full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $animalText =$GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);

 
        $priceArray = array(null => 'Do Not Sell!', 500 => 500, 600 => 600, 700 => 700, 800 => 800, 900 => 900,
            1000 => 1000, 1100 => 1100, 1200 => 1200, 1300 => 1300, 1400 => 1400, 1500 => 1500, 1600 => 1600, 1700 => 1700, 1800 => 1800, 1900 => 1900,
            2000 => 2000, 2100 => 2100, 2200 => 2200, 2300 => 2300, 2400 => 2400, 2500 => 2500, 2600 => 2600, 2700 => 2700, 2800 => 2800, 2900 => 2900,
            3000 => 3000, 3100 => 3100, 3200 => 3200, 3300 => 3300, 3400 => 3400, 3500 => 3500, 3600 => 3600, 3700 => 3700, 3800 => 3800, 3900 => 3900,
            4000 => 4000, 4100 => 4100, 4200 => 4200, 4300 => 4300, 4400 => 4400, 4500 => 4500, 4600 => 4600, 4700 => 4700, 4800 => 4800, 4900 => 4900,
            5000 => 5000, 5500 => 5500, 6000 => 6000, 6500 => 6500, 7000 => 7000, 7500 => 7500, 8000 => 8000, 8500 => 8500, 9000 => 9000, 9500 => 9500,
            10000 => '$ 10,000', 10500 => '$ 10,500', 11000 => '$ 11,000', 11500 => '$ 11,500', 12000 => '$ 12,000', 12500 => '$ 12,500', 13000 => '$ 13,000', 13500 => '$ 13,500', 14000 => '$ 14,000', 14500 => '$ 14,500', 15000 => '$ 15,000');

        //grab a list of sale price comments
        $res = $GLOBALS['pdo']->query("SELECT id,comment FROM bovinemanagement.sale_price_comment ORDER BY id");
       
        $pregnancyCheckComment[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.
            $pregnancyCheckComment[$row['id']] = $row['comment'];
        }


        //grab a list of cows.
        $res = $GLOBALS['pdo']->query("SELECT id,full_name,local_number from bovinemanagement.bovine WHERE death_date IS NULL ORDER BY local_number");
        
        $aniNumber[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $aniNumber[$row[0]] = $row[2] . " - " . $GLOBALS['MiscObj']->femaleShortName($row[1]);
        }



        $form = new HTML_QuickForm("salePriceCowQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'salePriceCowQuickForm', "Record Sale Price for $animalText");

        $form->addElement('select', 'price', 'Price', $priceArray);

        $form->addElement('checkbox', 'plusPrice', 'Plus Price?');

        $form->addElement('select', 'salePriceComment', 'Comment (Optional)', $pregnancyCheckComment);

        $form->addElement('text', 'customComment', 'Custom Comment (Optional):');

        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id',$bovine_id );

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('bovine_id');
            $price = $form->exportValue('price');
            if ($price == '') {
                $price = 'null';
            }

            if ($form->exportValue('plusPrice') != null) {
                $plusPrice = 'true';
            } else {
                $plusPrice = 'false';
            }
            $salePriceComment = $form->exportValue('salePriceComment');
            if ($salePriceComment == '') {
                $salePriceComment = 'null';
            }
            $customComment = $form->exportValue('customComment');
            $pageid = $form->exportValue('pageid');
            $event_time = date('r', strtotime('now'));

            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();
            $query = "INSERT INTO bovinemanagement.sale_price (bovine_id,price,plus,comment_id,custom_comment,event_time,userid)
		VALUES ($bovine_id,$price,$plusPrice,$salePriceComment,'$customComment','$event_time','{$GLOBALS['auth']->getUsername()}')";
            $res = $GLOBALS['pdo']->exec($query);

            // determine if the commit or rollback
       
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid=$pageid&bovine_id=$bovine_id");
            exit();
        } //end validation

        $form->accept($renderer);
        return $renderer->toHtml();
    }

   
    // displays info on potential offspring from cdn website.
    function displayPreviousPricesForChosenCow($bovine_id) {
         $sql = "SELECT id as bovine_id,full_name,local_number FROM bovinemanagement.bovine WHERE id=$bovine_id";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $animalShortName = $GLOBALS['MiscObj']->femaleShortName($row['full_name']);

         $title=("<h3>Pricing History for " . $row['local_number'] . " - " . $animalShortName . "</h3>");
        
        ////


   
$sql = <<<SQL
SELECT *,(SELECT id FROM bovinemanagement.sale_price WHERE bovine_id = $bovine_id AND update_time=(SELECT max(update_time) FROM bovinemanagement.sale_price WHERE bovine_id = $bovine_id)) as latest_id 
    FROM
   (SELECT sale_price.id,event_time,price,plus,comment,custom_comment,userid,bovine_id
		FROM bovinemanagement.sale_price
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id = sale_price_comment.id) x
   WHERE bovine_id = $bovine_id 
SQL;

        
         $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {
          
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Price'] = $this->printFormattedPrice($row);
            $rowOut['Comment'] = $this->printComment($row);
            $rowOut['Employee'] = ($row['userid']);
          $rowOut['Action'] = $this->printActionButtons($row,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&bovine_id={$_REQUEST['bovine_id']}");
         // $rowOut['colour_row'] =  (($row['completed']) ? 1 : 0);
          
          $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;
      

         $out[]=( (new JQueryDataTable)->startBasicArrayOpt($title, $dataArr)); 
         return implode($out); 
      
    }

   

  




    function printFormattedPrice($record) {
    

        if ($record['plus'] == 't') {
            $plus = '+';
        } else {
            $plus = '';
        }

        return $record['price'] . $plus;
    }

    function printComment($record) {

        //combine comments into one field.
        if ($record['custom_comment'] == '') {
            $comment = $record['comment'];
        } elseif ($record['comment'] == '') {
            $comment = $record['custom_comment'];
        } else {
            $comment = $record['comment'] . ' & ' . $record['custom_comment'];
        }
        return $comment;
    }

    
     function printActionButtons($record,$callBackURL=null) {
        

        $currentUserid = $GLOBALS['auth']->getUsername();
          
         //normnal user
            $sql = "SELECT id FROM bovinemanagement.sale_price WHERE userid='$currentUserid' AND update_time=(SELECT max(update_time) FROM bovinemanagement.sale_price WHERE userid='$currentUserid')";
       
        $res = $GLOBALS['pdo']->query($sql);
     
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
           
            if (($record['id'] == $row['id'])) {
                 $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'bovinemanagement.sale_price'));
                 $returnButtonsAsString = ($a->renderDelete('salePrice' . $row['id']));                
            }
        }

        //print button
        return $returnButtonsAsString;
    }
    
    
    function displayListOfSalePrices($mode = 'default') {
 
	
$sql = <<<SQL
SELECT bovinecurr.id as bovine_id,bovinecurr.full_name, bovinecurr.location_name,bovinecurr.local_number,sale_price.price,sale_price.plus,sale_price.custom_comment,sale_price_comment.comment,sale_price.event_time as last_update,sale_price.userid,bovinecurr.fresh_date,pregnant_view.calculated_potential_due_date as due_date,sire.short_name as sire_short_name
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.sale_price ON sale_price.id=(SELECT id from bovinemanagement.sale_price WHERE sale_price.bovine_id=bovinecurr.id AND sale_price.event_time=(SELECT max(event_time) FROM bovinemanagement.sale_price WHERE bovine_id=bovinecurr.id))
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id=sale_price_comment.id
LEFT JOIN bovinemanagement.pregnant_view on pregnant_view.id=bovinecurr.id
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
          
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']);     
            $rowOut['Full Name'] = ($row['full_name']);
            $rowOut['Sire'] = ($row['sire_short_name']);
            $rowOut['Fresh'] = JQueryDataTable::formattedDate($row['fresh_date']);    
            $rowOut['Due'] = JQueryDataTable::formattedDate($row['due_date']);
            $rowOut['Last Pricing'] = JQueryDataTable::formattedDate($row['last_update']);
            $rowOut['Price'] = ($row['price']);
            $rowOut['Comment'] = ($row['comment']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Location'] = ($row['location_name']);
          
          $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;
      
         $out[]=('<h2>Current Prices</h2>');
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'paging: false')); 
         return implode($out);                 
                
     
          
    }

    



     function chooseCowsToPutOnSalesListQuickForm() {

        $form = new HTML_QuickForm("salesListQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
       



        //grab a list of cows with fresh date.
        $res = $GLOBALS['pdo']->query("
		SELECT id,local_number,score,location_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,checked  FROM bovinemanagement.bovinecurr 
LEFT JOIN system.salesreport_temp ON salesreport_temp.bovine_id=bovinecurr.id
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
 ORDER BY local_number,location_name
		");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //parse for last word of name and then make an array for the select list.
            $str = '#' . $row['local_number'] . '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ' . $row['dim'] . ' &nbsp;&nbsp;&nbsp;' . $row['location_name'] . ' &nbsp;&nbsp;&nbsp;' . $row['score'];

            //create checkboxes.
            $temp = $form->createElement('checkbox', $row['id'], '', "<b>$str</b> <hr>",array('style' => 'display: flex; width: 6px;'));
            if ($row['checked'] == true) {
                $temp->setChecked(true);
            } //if previously checked, pass it along.
            $checkGroup[] = $temp;
        }


        $form->addGroup($checkGroup, 'checkGroup');

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');




        // Try to validate a form
        if ($form->validate()) {
            print("VALIDATION<BR>");
            $checkArray = $form->exportValue('checkGroup');
            //print_r2($checkArray);

            $pageid = $form->exportValue('pageid');


            //fist step is to delete everything in the temp storage table.
            $res = $GLOBALS['pdo']->exec("TRUNCATE system.salesreport_temp"); //erase contents of temp db table.	

            //since there is so many checkboxes, we insert each one individually
            foreach ($checkArray as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.salesreport_temp (bovine_id,checked) VALUES ($k,'$v')");
                $res = $GLOBALS['pdo']->exec($query);
            }

            //return to same page.
            $host = $_SERVER['HTTP_HOST'];
            header("Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
            exit();
        }


        /* code to add css to renderer */
        $class = "noprint"; // name of css table class
        $renderer = $form->defaultRenderer(); // instantiate renderer
        $renderer->setFormTemplate("<form{attributes}><table class=\"$class\" border=\"0\">{content}</table></form>");  // change default template
        $form->accept($renderer);
        echo $renderer->toHtml(); //render
    }

//end function

    public function displaySalesList($listType = 'next2monthsFresh') {



        $numberToshow = 1000;
        $numberPerPage = 550;

     


//custom list shows cows chosen earlier
        if ($listType == 'custom') {
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number,bovinecurr.birth_date as b_birth_date,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
 ,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll	  
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
LEFT JOIN system.salesreport_temp ON salesreport_temp.bovine_id = temp.bovine_id
WHERE salesreport_temp.checked=true ORDER BY temp.bovine_local_number;
";
        }

        
                if ($listType == 'custom_secondary') {
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number,bovinecurr.birth_date as b_birth_date,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
 ,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll	  
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
LEFT JOIN system.salesreport_temp2 ON salesreport_temp2.bovine_id = temp.bovine_id
WHERE salesreport_temp2.checked=true ORDER BY temp.bovine_local_number;
";
        }
       
         if ($listType == 'custom_lowProDollarcalves') {
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number,bovinecurr.birth_date as b_birth_date,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr 
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
 ,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll	  
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
LEFT JOIN system.salesreport_temp3 ON salesreport_temp3.bovine_id = temp.bovine_id
WHERE salesreport_temp3.checked=true ORDER BY temp.bovine_local_number;
";
        }
        

//60 days from calving list.
        if ($listType == 'dueSoon') {
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number,bovinecurr.birth_date as b_birth_date,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
WHERE (calculated_potential_due_date  <= (current_date + interval '60 day') )
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
 ,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
ORDER BY calculated_potential_due_date ASC
";
        }

        if ($listType == 'fresh') {
//fresh cows 20 to 90 dim
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number,bovinecurr.birth_date as b_birth_date,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
WHERE (fresh_date + interval '90 day' >= current_date) AND (fresh_date + interval '20 day' <= current_date)
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
ORDER BY bovinecurr_fresh_date DESC
";
        }

        if ($listType == 'pregnant') {
//fresh cows 20 to 90 dim
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number,bovinecurr.birth_date as  b_birth_date,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
WHERE (calculated_potential_due_date is not null )
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
ORDER BY calculated_potential_due_date ASC
";
        }

//heifers due within 4 months
        if ($listType == 'springer') {
            $sql = "
WITH temp as (
SELECT
 bovinecurr.id as bovine_id,calculated_potential_due_date as event_time2, bovinecurr.local_number as bovine_local_number, bovinecurr.full_name as bovine_full_name,calculated_potential_due_date,service_sire_short_name,bovinecurr.location_name,bovinecurr.full_reg_number as bovine_full_reg_number, bovinecurr.birth_date as b_birth_date ,bovinecurr.sire_full_reg_number,bovinecurr.dam_full_reg_number,dam.full_name as dam_full_name,sire.short_name as sire_short_name,bovinecurr.fresh_date as bovinecurr_fresh_date,(SELECT date FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_date, (SELECT milkyield FROM alpro.latest_full_day_production(bovinecurr.id)) as latest_production_milkyield
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.pregnant_view ON pregnant_view.id = bovinecurr.id
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number = bovinecurr.dam_full_reg_number
WHERE (calculated_potential_due_date  <= (current_date + interval '4 months') ) 
AND (SELECT count(id) FROM bovinemanagement.lactation WHERE lactation.bovine_id = bovinecurr.id)=0
)
SELECT temp.*,
(aggregate_view_curr.class ||' - ' ||	aggregate_view_curr.score) as classification,
(dam_aggregate_view_curr.class ||' - ' ||	dam_aggregate_view_curr.score) as dam_classification
 ,price,comment,valacta_data_latest_test_view.*,aggregate_view_curr.prodoll
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
ORDER BY calculated_potential_due_date ASC
";
        }

        
        
        
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    
        //change presentation
        foreach ($results as &$row) {
          
            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['bovine_local_number'], $row['bovine_full_name']);
            $rowOut['Sire'] = ($row['sire_short_name']);
            $rowOut['Dam'] = ($row['dam_full_name']);
            $rowOut['Birth Date'] = JQueryDataTable::formattedDate($row['b_birth_date']);
            $rowOut['Reg #'] = ($row['bovine_full_reg_number']);
            $rowOut['Pro$'] = ($row['prodoll']);
            $rowOut['Classification'] = ($row['classification']);
            $rowOut['Fresh'] = JQueryDataTable::formattedDate($row['bovinecurr_fresh_date']);
            $rowOut['Due To'] = ($row['service_sire_short_name']);
            $rowOut['Due Date'] = JQueryDataTable::formattedDate($row['calculated_potential_due_date']);
            $rowOut['Latest Parlor Yield'] = $this->printLatestProduction($row);
            $rowOut['Projection'] = $this->printProjection($row);
            $rowOut['SSC'] = ($row['ssc']);
            $rowOut['Price'] = ($row['price']);
            $rowOut['Location'] = ($row['location_name']);

            $resultsOut[]=$rowOut;  
        }

       //output
        $dataArr = $resultsOut;
      
         $out[]=( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr,'paging:false')); 
         return implode($out);                
        
    }

    function printFormattedSalesBovineNameAndNumber($params) {
        extract($params);
        $number1 = $GLOBALS['MiscObj']->hrefToBovinePageFullName($record['bovine_id'],$record['bovine_local_number'],$record['bovine_full_name']);
        $number2 = "<a href=\"" . HolsteinCanadaHelper::createHolsteinCanadaQuery($record['bovine_full_reg_number']) . "\"> <b>HOL</b>" . "</a>";
        $number3 = "<a href=\"" . $GLOBALS['MiscObj']->createCDNLink($record['bovine_full_reg_number']) . "\"> <b>CDN</b>" . "</a>";
        return $number1 . "&nbsp;&nbsp;" . $number2 . "&nbsp;&nbsp;" . $number3;
    }

    function printSire($params) {
        extract($params);
        $number2 = "<a href=\"" . $GLOBALS['MiscObj']->createCDNLink($record['sire_full_reg_number']) . "\">{$record['sire_short_name']}" . "</a>";
        return $number2;
    }

    function printDam($params) {
        extract($params);
        $number2 = "<a href=\"" . $GLOBALS['MiscObj']->createCDNLink($record['dam_full_reg_number']) . "\">{$record['dam_full_name']} &nbsp;&nbsp;{$record['dam_classification']}" . "</a>";
        return $number2;
    }

    function printBirthDate($params) {
        extract($params);
        if ($record['b_birth_date'] != null) {
            return date("M d Y", strtotime($record['b_birth_date']));
        } else {
            return null;
        }
    }

    function printFreshDate($params) {
        extract($params);
        if ($record['bovinecurr_fresh_date'] != null) {
            return date("M d Y", strtotime($record['bovinecurr_fresh_date']));
        } else {
            return null;
        }
    }

    function printProjection($record) {

        if ($record['days_in_mi'] == null) {
            return "";
        } else {
            $ssc = '';
            if ($record['ssc'] != null) {
                $ssc_t=$record['ssc']/1000;
                $ssc = "SSC {$ssc_t}k";
            }
            return "DIM {$record['days_in_mi']} Proj: {$record['milk305']}kg BCA {$record['bca_milk']}-{$record['bca_fat']}-{$record['bca_prot']} " . $ssc;
        }
    }

    function printSSC($params) {
        $ssc=null;
        extract($params);
        if (isset($record['ssc'])) {
        if ($record['ssc'] != null) {
            $ssc_t=$record['ssc']/1000;
            $ssc = "{$ssc_t}k";
        }}
        return $ssc;
    }

    //from parlor
    function printLatestProduction($record) {

        if ($record['latest_production_milkyield'] != 0) {
            return round($record['latest_production_milkyield'], 1) . ' &nbsp;&nbsp; <b class="smallish">(' . $record['latest_production_date'] . ')</b> ';
        }
    }


    
    
    
}//end class

       
Class QF2_SalesListCustom extends QF2_Base {

    function definition($form) {


        $mode = 'group';

        $aniNumber = $GLOBALS['MiscObj']->createListOfAllAliveBovinesFemale();

        ////
        //$fieldset = $form->addFieldset('header')->setLabel('Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        

        //read animals that are currently marked as slected and add them as defaults to the form
            if ($this->mode == 'custom') {
            $query = "SELECT bovine_id FROM system.salesreport_temp";
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($aniNumber);
        } elseif ($this->mode == 'custom_secondary') {
            $query = "SELECT bovine_id FROM system.salesreport_temp2";
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents_secondary'))->loadOptions($aniNumber);
         } elseif ($this->mode == 'custom_lowProDollarcalves') {
            $query = "SELECT bovine_id FROM system.salesreport_temp3";
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'custom_lowProDollarcalves'))->loadOptions($aniNumber);    
        } else {
            throw new Exception("ERROR: Custom sales list type of priamary or secondary not chosen in definition.");
        }

        $statement = $GLOBALS['pdo']->prepare($query);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_COLUMN); //fetch column

        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
    'animalSelected' =>  $results
)));
  return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s3 = $("#selectEvents").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    //select2 javascript needed for form
    function formJS_Secondary() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents_secondary").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>    
          
         <script>           
var s3 = $("#custom_lowProDollarcalves").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script> 
        
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {
       
        if ($this->mode == 'custom') {

            //fist step is to delete everything in the temp storage table.
            $res = $GLOBALS['pdo']->exec("TRUNCATE system.salesreport_temp"); //erase contents of temp db table.	

            foreach ($value['animalSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.salesreport_temp (bovine_id,checked) VALUES ($v,true)");
                $res = $GLOBALS['pdo']->exec($query);
            }
        } elseif ($this->mode == 'custom_secondary') {
            //fist step is to delete everything in the temp storage table.
            $res = $GLOBALS['pdo']->exec("TRUNCATE system.salesreport_temp2"); //erase contents of temp db table.	

            foreach ($value['animalSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.salesreport_temp2 (bovine_id,checked) VALUES ($v,true)");
                $res = $GLOBALS['pdo']->exec($query);
            }
        } elseif ($this->mode == 'custom_lowProDollarcalves') {
            //fist step is to delete everything in the temp storage table.
            $res = $GLOBALS['pdo']->exec("TRUNCATE system.salesreport_temp3"); //erase contents of temp db table.	

            foreach ($value['animalSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.salesreport_temp3 (bovine_id,checked) VALUES ($v,true)");
                $res = $GLOBALS['pdo']->exec($query);
            }    
        } else {
            throw new Exception("ERROR: Custom sales list type of priamry or secondary or lowProDollar not chosen.");
        }
    }

}//end class   
?>