<?php 




require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/holsteinCanadaHelper.inc');
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';  

//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if (class_exists('DataGridLR_Ext') != true) {

    //extends datgrid class with specific logic to colour the line.
    //use print($dg1->render('datagrid',true,'00FF00')); to set colour
    class DataGridLR_Ext extends DataGridLR {

     /**
     * Method to change the color of rows when they need to be priced.
     */
        function colourLineCriteria($row) {
            if ((strtotime($row['last_update']) <= strtotime('now - 6 months')) AND ( (strtotime($row['fresh_date']) >= strtotime('now - 60 days')) OR (strtotime($row['due_date']) <= strtotime('now + 60 days')) )) {
                return true;
            } else {
                return false;
            }
        }

    }

}
 if (class_exists('DataGridLR_Ext2') != true) {

    //extends datgrid class with specific logic to colour the line.
    //use print($dg1->render('datagrid',true,'00FF00')); to set colour
    class DataGridLR_Ext2 extends DataGridLR {

     /**
     * Method to change the color of latest row
     */
        function colourLineCriteria($row) {
          
            if ($row['id']==$row['latest_id'])
                { return true;
            } else {
                return false;
            }
        }

    }

}                        
                 

class SalePrice extends TabPage {

    private $bovine_id;
 
    public function defaultDisplay() {
     
        //fill class variables.
       array_key_exists('bovine_id',$_REQUEST) ? $_REQUEST['bovine_id'] : null;
        if (!empty($_REQUEST['bovine_id'])) {
            $this->bovine_id = $_REQUEST['bovine_id'];
        }
        
          $a=new QF2_SalesListCustom((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('qf2_SalesListCustomEvent','Select Animals to Add to Custom Sales List'));  
        print($a->formJS());

        
        //setup for tabs.
        $tabArray[1]['name'] = 'Enter Price';
        $tabArray[2]['name'] = 'Current Prices';
        $tabArray[3]['name'] = 'Custom List';
        $tabArray[4]['name'] = 'Fresh List';
        $tabArray[5]['name'] = 'Pregnant List';
        $tabArray[6]['name'] = 'Springer Heifer List';
        $tabArray[7]['name'] = 'Due Soon List';
        $tabArray[8]['name'] = 'Recent Sold (cull) Buyer Output';
        $tabArray[9]['name'] = 'Recent Sold (custom list) Buyer Output';
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

  print ('<div class="row">');
        $selectedBovineID = $this->animalQueryQuickForm();
       
          print('</div>');
           print("<hr>");
        print ('<div class="row">');
        print('<div class="col-md-6">');
       if (($this->bovine_id != -1)) {
         //lookup animal reg number.
        //load from Little River DB!
        $sql = "SELECT * from bovinemanagement.bovinecurr WHERE id={$this->bovine_id} LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $bovineFullRegNumber = $row['full_reg_number'];

        //draw holstein canada iframe.
        include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/holsteinCanadaPageParser.php');
        $xx=new HolsteinCanadaPageParser(850,900); //reg,iframe,width,height.
        print($xx->generateIframe($bovineFullRegNumber));  
       }
        print('</div>');
        print('<div class="col-md-4">');
        
         if ($this->bovine_id != -1) {
      
        $this->salePriceCowQuickForm();
        $this->displayPreviousPricesForChosenCow($this->bovine_id);
   
         }
          
        print('</div>');
        print('</div>');
        
        
    }

    function tab2() {
        $this->displayListOfSalePrices();
    }

    function tab3() {
      
        print('<br/>');
        print('<br/>');
        $this->displaySalesList('custom');

    }

    function tab4() {
        $this->displaySalesList('fresh');
    }

    function tab5() {
        $this->displaySalesList('pregnant');
    }

    function tab6() {
        $this->displaySalesList('springer');
    }

    function tab7() {
        $this->displaySalesList('dueSoon');
    }
    
    
    /*
     * print medical and repro report to give to buyer from list based on cull sold.
     */
     function tab8() {
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
     function tab9() {
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
        $number3 = "<a href=\"" . Misc::createListOfAllCowsMilking($full_reg_number) . "\"> <b>CDN</b>" . "</a>";
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
    
    
    
    function salePriceCowQuickForm() {
        //grab name/number info on current bovine
        $sql = "SELECT id as bovine_id,full_name,local_number FROM bovinemanagement.bovine WHERE id={$this->bovine_id}";
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
            $aniNumber[$row[0]] = $row[2] . " - " . Misc::femaleShortName($row[1]);
        }



        $form = new HTML_QuickForm("salePriceCowQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'salePriceCowQuickForm', "Record Sale Price for $animalText");

        $form->addElement('select', 'price', 'Price', $priceArray);

        $form->addElement('checkbox', 'plusPrice', 'Plus Price?');

        $form->addElement('select', 'salePriceComment', 'Comment (Optional)', $pregnancyCheckComment);

        $form->addElement('text', 'customComment', 'Custom Comment (Optional):');

        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id',$this->bovine_id );

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
                echo "Failed: " . $e->getMessage();
            
            }
            // Close the transaction
            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id={$this->bovine_id}");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    function animalQueryQuickForm() {

        $aniNumber = Misc::createListOfAllAliveCows();

        //javascript to allow submission on change of select element.
        $attrs = array('onchange' => "javascript:location.href='?bovine_id='+this.options[this.selectedIndex].value+'&pageid={$_REQUEST['pageid']}';$('#example > ul').tabs('select', 0);");

        //javascript to allow submission when enter is pressed within select form
        //$attrs= array('onKeyPress' => "javascript:if(event.keyCode=='13'){document.frmTest.submit();}");

        $form = new HTML_QuickForm("frmBovineQuery",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrQuickformtest', 'Select an Animal');
        $form->addElement('select', 'animalNumber', '', $aniNumber, $attrs);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //defaults
        if ($this->bovine_id != null) {
            $form->setDefaults(array('animalNumber' => $this->bovine_id)); //set current bovine.
        } else {
            $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        }


        // Try to validate a form
        if ($form->validate()) {

            //get values
            __construct($form->exportValue('animalNumber'));
            $pageid = $form->exportValue('pageid');
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
        return $form->exportValue('animalNumber'); // return what bovineID was selected.
    }

    // displays info on potential offspring from cdn website.
    function displayPreviousPricesForChosenCow($bovine_id) {
         $sql = "SELECT id as bovine_id,full_name,local_number FROM bovinemanagement.bovine WHERE id={$this->bovine_id}";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $animalShortName = Misc::femaleShortName($row['full_name']);

        print("<h3>Pricing History for " . $row['local_number'] . " - " . $animalShortName . "</h3>");
        
        ////


        $sql = "SELECT *,(SELECT id FROM bovinemanagement.sale_price WHERE bovine_id = $bovine_id AND update_time=(SELECT max(update_time) FROM bovinemanagement.sale_price WHERE bovine_id = $bovine_id)) as latest_id FROM specific_cow_price WHERE bovine_id = $bovine_id ";

                 $dg = new DataGridLR_Ext2($sql, 40);
                 
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', 'event_time', 'event_time', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Price', 'price', 'price', array('' => ''), null, array('SalePrice', 'printFormattedPrice()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Comment', 'comment', 'comment', array('' => ''), null, array('SalePrice', 'printComment()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', 'userid', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('SalePrice','printActionButtons()')));
  
        print($dg->render('datagrid',true,$GLOBALS['config']['COLOUR']['attention'])); //to set colour
       
        $dg->pager();
      

      

    }

    function printFormattedNumber($params) {
        extract($params);
        $number = "<a href=\"" . Misc::hrefToBovinePage($record['bovine_id']) . "\"> {$record['local_number']}</a>";
        return $number;
    }

    function printPartialName($params) {
        extract($params);
      
        $animalShortName = Misc::femaleShortName($row['full_name']);
        return $animalShortName;
    }

//datagrid customizations.....


    function printFormattedPrice($params) {
        extract($params);

        if ($record['plus'] == 't') {
            $plus = '+';
        } else {
            $plus = '';
        }

        return $record['price'] . $plus;
    }

    function printComment($params) {
        extract($params);
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

    
     function printActionButtons($params) {
        $customHeaderLocation="http://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id={$_REQUEST['bovine_id']}";
        extract($params);

        $currentUserid = $GLOBALS['auth']->getUsername();
          
         //normnal user
            $sql = "SELECT id FROM bovinemanagement.sale_price WHERE userid='$currentUserid' AND update_time=(SELECT max(update_time) FROM bovinemanagement.sale_price WHERE userid='$currentUserid')";
       
        $res = $GLOBALS['pdo']->query($sql);
     
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
           
            if (($record['id'] == $row['id'])) {
                $returnButtonsAsString = DataGridHelper::deleteQuickForm($row['id'], 'sale_price',$customHeaderLocation); //show delete button, with table id as index for delete.
            }
        }

        //print button
        return $returnButtonsAsString;
    }
    
    
    function displayListOfSalePrices($mode = 'default') {
 
		$sql = "SELECT bovinecurr.id as bovine_id,bovinecurr.full_name, bovinecurr.location_name,bovinecurr.local_number,sale_price.price,sale_price.plus,sale_price.custom_comment,sale_price_comment.comment,sale_price.event_time as last_update,sale_price.userid,bovinecurr.fresh_date,pregnant_view.calculated_potential_due_date as due_date,sire.short_name as sire_short_name
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.sale_price ON sale_price.id=(SELECT id from bovinemanagement.sale_price WHERE sale_price.bovine_id=bovinecurr.id AND sale_price.event_time=(SELECT max(event_time) FROM bovinemanagement.sale_price WHERE bovine_id=bovinecurr.id))
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovinecurr.sire_full_reg_number
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id=sale_price_comment.id
LEFT JOIN bovinemanagement.pregnant_view on pregnant_view.id=bovinecurr.id";  //no order by so that datagrid sorting works.
                
        $dg = new DataGridLR_EXT($sql, 400);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Number', 'local_number', 'local_number', array('' => ''), null, array('SalePrice', 'printFormattedNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Name', 'full_name', 'full_name', array('' => ''), null, array('SalePrice', 'printPartialName()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sire', 'sire_short_name', 'sire_short_name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Fresh', 'fresh_date', 'fresh_date', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Due', 'due_date', 'due_date', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Last Pricing', 'last_update', 'last_update', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Price', null, 'price', array('' => ''), null, array('SalePrice', 'printFormattedPrice()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Comment', null, 'comment', array('' => ''), null, array('SalePrice', 'printComment()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', 'userid', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Location', 'location_name', 'location_name', array('' => ''), null, null));
        
         print($dg->render('datagrid',true,$GLOBALS['config']['COLOUR']['attention'])); //to set colour
        
        $dg->pager();
        
                
          
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
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
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
 ,price,comment,valacta_data_latest_test_view.*	  
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
LEFT JOIN system.salesreport_temp ON salesreport_temp.bovine_id = temp.bovine_id
WHERE salesreport_temp.checked=true ORDER BY temp.bovine_local_number;
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
 ,price,comment,valacta_data_latest_test_view.*
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
,price,comment,valacta_data_latest_test_view.*
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
,price,comment,valacta_data_latest_test_view.*
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
 ,price,comment,valacta_data_latest_test_view.*
FROM temp
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = temp.bovine_full_reg_number
LEFT JOIN batch.aggregate_view_curr as dam_aggregate_view_curr ON dam_aggregate_view_curr.full_reg_number = temp.dam_full_reg_number
LEFT JOIN bovinemanagement.sale_price_curr ON sale_price_curr.bovine_id=temp.bovine_id
LEFT JOIN batch.valacta_data_latest_test_view ON  temp.bovine_full_reg_number = valacta_data_latest_test_view.reg
ORDER BY calculated_potential_due_date ASC
";
        }

        
         $dg = new DataGridLR($sql, $numberToshow);
         $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('SalePrice', 'printFormattedSalesBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sire', 'sire_short_name', 'sire_short_name', array('' => ''), null, array('SalePrice', 'printSire()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Dam', 'dam_full_name', 'dam_full_name', array('' => ''), null, array('SalePrice', 'printDam()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Birth Date', 'birth_date', 'birth_date', array('' => ''), null, array('SalePrice', 'printBirthDate()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Classification', 'classification', 'classification', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Fresh', 'bovinecurr_fresh_date', 'bovinecurr_fresh_date', array('' => ''), null, array('SalePrice', 'printFreshDate()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Due To', 'service_sire_short_name', 'service_sire_short_name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Due Date', 'calculated_potential_due_date', 'calculated_potential_due_date', array('' => ''), null, array('DataGridHelper', 'printFormattedTime2()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Latest Parlor Yield', 'latest_production_milkyield', 'latest_production_milkyield', array('' => ''), null, array('SalePrice', 'printLatestProduction()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Projection', 'bca_milk', 'bca_milk', array('' => ''), null, array('SalePrice', 'printProjection()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('SSC', 'ssc', 'ssc', array('' => ''), null, array('SalePrice', 'printSSC()')));   
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Price', 'price', 'price', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Location', 'location_name', 'location_name', array('' => ''), null, null));

 print($dg->render('datagrid', false, $GLOBALS['config']['COLOUR']['okay']));
 $dg->pager();
    
    }

    function printFormattedSalesBovineNameAndNumber($params) {
        extract($params);
        $number1 = $GLOBALS['MiscObj']->hrefToBovinePageFullName($record['bovine_id'],$record['bovine_local_number'],$record['bovine_full_name']);
        $number2 = "<a href=\"" . HolsteinCanadaHelper::createHolsteinCanadaQuery($record['bovine_full_reg_number']) . "\"> <b>HOL</b>" . "</a>";
        $number3 = "<a href=\"" . Misc::createListOfAllCowsMilking($record['bovine_full_reg_number']) . "\"> <b>CDN</b>" . "</a>";
        return $number1 . "&nbsp;&nbsp;" . $number2 . "&nbsp;&nbsp;" . $number3;
    }

    function printSire($params) {
        extract($params);
        $number2 = "<a href=\"" . Misc::createListOfAllCowsMilking($record['sire_full_reg_number']) . "\">{$record['sire_short_name']}" . "</a>";
        return $number2;
    }

    function printDam($params) {
        extract($params);
        $number2 = "<a href=\"" . Misc::createListOfAllCowsMilking($record['dam_full_reg_number']) . "\">{$record['dam_full_name']} &nbsp;&nbsp;{$record['dam_classification']}" . "</a>";
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

    function printProjection($params) {
        extract($params);
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
    function printLatestProduction($params) {
        extract($params);
        if ($record['latest_production_milkyield'] != 0) {
            return round($record['latest_production_milkyield'], 1) . ' &nbsp;&nbsp; <b class="smallish">(' . $record['latest_production_date'] . ')</b> ';
        }
    }


    
    
    
}//end class

       
Class QF2_SalesListCustom extends Quickform2Helper {

    function definition($form) {


        $mode = 'group';

        $aniNumber = Misc::createListOfAllAliveCowsFemale();

        ////
        //$fieldset = $form->addFieldset('header')->setLabel('Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($mode == 'group') {
            $form->addSelect('aniamlSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($aniNumber);
        } else {
            $form->addElement('hidden', 'aniamlSelected')->setValue($bovine_id);
        }

        //read animals that are currently marked as slected and add them as defaults to the form
        $query="SELECT bovine_id FROM system.salesreport_temp";
        $statement = $GLOBALS['pdo']->prepare($query);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_COLUMN); //fetch column

        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
    'aniamlSelected' =>  $results
)));
  return $form; //required   
    }

    //select2 javascript needed for form
    function formJS() {
$jscript = <<<JSCRIPT
 <script>           
var s2 = $("#selectEvents").select2({
    placeholder: "Choose Animals",
    tags: false
});   
      </script>      
JSCRIPT;

            return $jscript;
    }
    
    
    function process($value) {
       
       
        
        //fist step is to delete everything in the temp storage table.
            $res = $GLOBALS['pdo']->exec("TRUNCATE system.salesreport_temp"); //erase contents of temp db table.	
        
        foreach ($value['aniamlSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.salesreport_temp (bovine_id,checked) VALUES ($v,true)");
                $res = $GLOBALS['pdo']->exec($query);
            }
       
    }

}//end class   
?>