<?php 





;
require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc'; 

/** records an actual cull from the herd and why */
class HerdCull extends BasePage {

  

    public function defaultDisplay() {
        
           print("<div class='row'>");
            print("<div class='col-sm-4 col-sm-push-8'>");
            
        $a=new QF2_HerdCullAnimal((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group');
        print($a->renderBox('QF2_HerdCullAnimal','Mark Animal(s) That Have Been Culled'));  
        print($a->formJS());
        print("</div>");
        print("<div class='col-sm-8 col-sm-pull-4'>");
         $this->plotCullBarGraph();
         $this-> displayCowsWhoHaveBeenCulled('default');
        //$this->cowsWhoHaveBeenCulled();
        print("</div>");
       
        print("</div>");
        
    
       
    }

    /**
     * plots a bar graph of who died, etc.
     */
    public function plotCullBarGraph() {

        $arrayDied = $this->findMonthlyTotals('Died');
        $arrayBeefed = $this->findMonthlyTotals('Beefed');
        $arraySold = $this->findMonthlyTotals('Sold');


        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'date'; //BACKWARDS
        $headerStrArr['Died'] = 'number';
        $headerStrArr['Beefed'] = 'number';
        $headerStrArr['Sold'] = 'number';
         $opt='';
        $x = new GoogleVisColumn($opt, $headerStrArr, $arrayDied, $arrayBeefed, $arraySold);
        print($x->toString());
    }

    private function findMonthlyTotals($reason) {

        /* find all the cows who died (or whatever) in the last year. */
        $sql = "
  With temp as (
Select bovine_id,date_trunc('month',event_time) as month,1 as amount FROM bovinemanagement.cull_event
WHERE reason='$reason' AND event_time between date_trunc('month',current_date) - interval '1 year'  AND date_trunc('month',current_date)  + interval '1 month'
)
SELECT 
DISTINCT(month),sum(amount) OVER(PARTITION BY month) as total 
FROM temp ORDER BY month
";
        $arrayReason = null;

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $arrayReason[strtotime($row['month'])] = $row['total'];
        }



        return $arrayReason;
    }

    
    function displayCowsWhoHaveBeenCulled($mode='default') {
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },
{
                data: "event_time",
                title: "Date",
                type: "date"
            },                
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"       
            },
            {
                data: "reg_num",
                title: "Reg #"
            },
            {
                data: "reason",
                title: "Reason"
            },
            {
                data: "details",
                title: "Details"
            },
         
            {
                data: "userid",
                title: "Employee"
            },
           
            {
                data: "action",
                title: "Action"
            }
EX;
    print($dt->start($this->pageid,'cowsWhoHaveBeenCulled',$mode,$columnsStr));   
    }
    
    
    public function cowsWhoHaveBeenCulled($request) {
         if (!empty($request['item'])) {
        
        

        if ($request['item'] == 'default') {
                // Setup your database connection
                $sql = "	     
SELECT *,cull_event.id as id FROM bovineManagement.cull_event LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id ORDER BY event_time DESC;
";
            } elseif ($request['item'] == 'milkTestInputsDisplay') {
                $sql = "	     
SELECT *,cull_event.id as id FROM bovineManagement.cull_event LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id WHERE event_time  >= (current_date - interval '10 weeks') ORDER BY event_time DESC;
";
            } else {
                throw new Exception("ERROR invalid paramater sent with request");
            }
        } else {
            throw new Exception("ERROR no paramater sent with request");
        }


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
           $row['event_time_raw']=strtotime($row['event_time']);
           $row['event_time']= date("M d, Y",strtotime($row['event_time'])); 
           $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'],$row['local_number'],$row['full_name']); 
           $row['reg_num'] =JQueryDataTable::formattedShortRegNum($row['full_reg_number']);
           $row['reason'] =  $row['reason'];
           $row['details'] =   $row['comment'];
        
           $row['action'] = $this->printActionButtons($row['id'],"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}");
        }

        //output
        $output['data'] = $results;
        return ($output);
        
        
        /*
        $dg = new DataGridLR($sql, 100);
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedTime()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Reg #', null, '', array('' => ''), null, array('DataGridHelper', 'printShortRegNum()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Reason', 'reason', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Details', 'comment', '', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('User', 'userid', '', array('' => ''), null, null));
        if ($mode == 'default') {
            $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('HerdCull', 'printActionButtons()')));
        }
        print("<h3>Culled Cows</h3>");
        print($dg->render());
        if ($mode == 'default') {
            $dg->pager();
        }
        
         */
    }

//end function
    //show delete button
    function printActionButtons($id,$callBackURL=null) {
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.

        $res = $GLOBALS['pdo']->query("SELECT id FROM bovineManagement.cull_event WHERE userid='$currentUserid' ORDER BY update_time DESC LIMIT 1");
        $returnButtonsAsString = null;
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if (($id == $row['id'])) {

                $returnButtonsAsString = self::deleteQuickForm($row['id'],$callBackURL); //show delete button
            }
        }

   
        return $returnButtonsAsString;
    }

    function deleteQuickForm($tableid,$callBackURL) {

        $form = new HTML_QuickForm("CullEventDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'tableid', $tableid);
        $form->addElement('submit', 'btnDelete', 'Delete');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $pageid = $form->exportValue('pageid');
            $tableid = $form->getElementValue('tableid');


            // Open a transaction
            try {$res = $GLOBALS['pdo']->beginTransaction();


            //delete movement
            $res = $GLOBALS['pdo']->query("SELECT location_event_id,bovine_id FROM bovineManagement.cull_event WHERE id='$tableid' LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $location_event_id = $row['location_event_id'];
            $bovine_id = $row['bovine_id'];

            //delete the cull event
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.cull_event where id='$tableid'");

            //delete movement
            $res = $GLOBALS['pdo']->query("DELETE FROM bovinemanagement.location_event where id='$location_event_id'");


            //change the bovine so that the death date is erased.
            $query = "UPDATE bovinemanagement.bovine SET death_date=null WHERE id=$bovine_id";
            $res = $GLOBALS['pdo']->exec($query);


            // determine if the commit or rollback
  
               $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
            


            header($callBackURL);
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

}
//end class

/*
 * Herd Cull animal form.
 * 
 */
Class QF2_HerdCullAnimal extends Quickform2Helper {

    function definition($form) {
       
        //only show if userid is admin or owner group.
        if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $dateArray = Misc::createHourlyDatesForLastSevenDays(); //for admin only.   //Misc::createDailyDatesForLastYearAtNoon //use if needed.
        }else {
           $dateArray = Misc::createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.  
        }

        $aniNumber = Misc::createListOfAllAliveCows();
        
        //list of cull reasons
        $cullReason = array(null => 'Please Select one...', 'Sold' => 'Sold (put who sold to)', 'Beefed' => 'Beefed  (put name of driver)', 'Died' => 'Died  (put cuase of death)');

        
        
        
        ////
        //$fieldset = $form->addFieldset('header')->setLabel('Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        if ($this->mode == 'group') {
            $form->addSelect('animalSelected', array('multiple' => 'multiple', 'id' => 'selectEvents'))->loadOptions($aniNumber);
        } else {
                $form->addElement('hidden', 'animalSelected')->setValue( $this->param->bovine_id);       
        }
        
        $form->addElement('select', 'reason', null, array('options' => $cullReason, 'label' => 'Reason'))->addRule("required", "Please specify your type of cull");
        $form->addElement('textarea', 'details',  array('rows' => 6))->setLabel("Comments : ");
               
        
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");
        
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


          $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);

        if (is_array($value['animalSelected'])) {
            //group
            foreach ($value['animalSelected'] as $k => $v) {
                //now we insert all the animals that were checked.
                $bovine_id = $v;

                //get random transaction id
                $transaction_id = Misc::generatePseudoRandomTransactionInteger();

                //get location id from reason.
                $location_id = $this->offFarmLcoations($value['reason']);
                // Open a transaction
                try {
                    $res = $GLOBALS['pdo']->beginTransaction();

                    $query = "INSERT INTO bovinemanagement.location_event (bovine_id,event_time,location_id,userid,transaction_id) VALUES ($bovine_id,'{$value['event_time']}',$location_id,'{$GLOBALS['auth']->getUsername()}',$transaction_id)";
                    $res = $GLOBALS['pdo']->exec($query);
                    //get last insertid for location event	
                    $last_location_event_id = $GLOBALS['pdo']->lastInsertId('bovinemanagement.location_event_id_seq');

                    //record in cull_event
                    $query = "INSERT INTO bovinemanagement.cull_event (bovine_id,event_time,reason,comment,userid,location_event_id) VALUES ($bovine_id,'{$value['event_time']}','{$value['reason']}','{$value['details']}','{$GLOBALS['auth']->getUsername()}',$last_location_event_id)";
                    $res = $GLOBALS['pdo']->exec($query);

                    //record death_date in bovine
                    $query = "UPDATE bovinemanagement.bovine SET death_date='{$value['event_time']}' WHERE id=$bovine_id";
                    $res = $GLOBALS['pdo']->exec($query);

                    $GLOBALS['pdo']->commit();
                } catch (Exception $e) {
                    $GLOBALS['pdo']->rollBack();
                    echo "Failed: " . $e->getMessage();
                }
            }
        }
        //individual case
        else {
            //TODO: not implemented.
        }


        // determine if the commit or rollback
    }

    //this is kind of stupid. not really andy difference from harcoding the id numbers. 
function offFarmLcoations($reason){
   //safe way to find location_event id.
            switch ($reason) {
                case 'Sold':
                    $sql = "SELECT id FROM bovineManagement.location WHERE name='Off Farm - Sold' LIMIT 1";
                    $res = $GLOBALS['pdo']->query($sql);
                    $row = $res->fetch(PDO::FETCH_ASSOC);
                    $location_id = $row['id'];
                    break;
                case 'Beefed':
                    $sql = "SELECT id FROM bovineManagement.location WHERE name='Off Farm - Beef' LIMIT 1";
                    $res = $GLOBALS['pdo']->query($sql);
                    $row = $res->fetch(PDO::FETCH_ASSOC);
                    $location_id = $row['id'];
                    break;
                case 'Died':
                    $sql = "SELECT id FROM bovineManagement.location WHERE name='Off Farm - Dead' LIMIT 1";
                    $res = $GLOBALS['pdo']->query($sql);
                    $row = $res->fetch(PDO::FETCH_ASSOC);
                    $location_id = $row['id'];
                    break;
                default:
                    $location_id = 'null'; //should never be called.
                    throw new Exception("Error: reason method input not given to offFarmLcoations method.");
            }
return $location_id;
}

}//end class   

?>