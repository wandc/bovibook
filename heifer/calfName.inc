<?php 



require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/transition/transitionCalving.inc'); //used for potential calf sire loginc.


//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if(class_exists('DataGridLR_Ext') != true) {
/* //extends datgrid class with specific logic to colour the line. */
class DataGridLR_Ext extends DataGridLR {

    function colourLineCriteria($row) {

        if (strlen($row['potential_name']) == 0) {
            return true;
        } else {
            return false;
        }
    }
}
}

class CalfName extends BasePage {

    public function defaultDisplay() {

        self::calfNameQuickForm();
        self::displayCalvesNeedingApotentialNameAndRegisterd();
         print("<br/><h2>Recent Calvings</h2>");
        TransitionCalving::displayLast100DaysCalvingEvents();   
    }

    /*
     * Look for calves not in holstein canada system.
     * 
     */

    function displayCalvesNeedingApotentialNameAndRegisterd() {
        print("<h2>Calves to be Named Who Are Not Yet Registered</h2>");
        //NOTE: The conceiver is taken from the dam which in the case of an embryo should really be the reciepeint, if we ever fix this deign flaw this will need to be changed.
        $sql = "SELECT calf_potential_name.id,potential_name,local_number,xx.id as bovine_id,xx.full_name,xx.full_reg_number,event_time_reg_sent,birth_date as birth_time,(SELECT id FROM bovinemanagement.bovine WHERE full_reg_number=xx.dam_full_reg_number) as conceiver_id 
                 FROM bovinemanagement.bovine xx
                 LEFT JOIN batch.holstein_canada_registered ON xx.id=bovine_id 
                 LEFT JOIN bovinemanagement.calf_potential_name ON xx.id=calf_potential_name.bovine_id 
                 WHERE (local_reg_status is null OR local_reg_status=false) AND death_date is null ORDER BY potential_name DESC, local_number DESC";
        $dg = new DataGridLR_Ext($sql, 50);
        
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Animal', 'local_number', 'local_number', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Potential Name', 'potential_name', 'potential_name', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Relatives', 'Relatives', 'Relatives', array('' => ''), null, array('CalfName', 'printDamAndGrandDam()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Potential Calf Sire', null, '', array('' => ''),null,array('TransitionCalving','potentialSireandRecipient()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Registration Sent', 'event_time_reg_sent', 'event_time_reg_sent', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', 'Action', 'Action', array('' => ''), null, array('CalfName', 'printActionButtons()')));
          print($dg->render('datagrid datagridNarrow'));
        $dg->pager();
    }

   
    
    function printDamAndGrandDam($params) {
          extract($params);
          $sql0 = "SELECT dam_full_reg_number FROM bovinemanagement.bovine WHERE full_reg_number='{$record['full_reg_number']}' limit 1";
          $res0 = $GLOBALS['pdo']->query($sql0);
          $row0 = $res0->fetch(PDO::FETCH_ASSOC);
          //
          $sql1 = "SELECT full_reg_number,dam_full_reg_number,bovinemanagement.short_name(full_name) as short_name FROM bovinemanagement.bovine WHERE full_reg_number='{$row0['dam_full_reg_number']}' limit 1";
          $res1 = $GLOBALS['pdo']->query($sql1);
          $row1 = $res1->fetch(PDO::FETCH_ASSOC); 
          //
          $sql2 = "SELECT full_reg_number,dam_full_reg_number,bovinemanagement.short_name(full_name) as short_name FROM bovinemanagement.bovine WHERE full_reg_number='{$row1['dam_full_reg_number']}' limit 1";
          $res2 = $GLOBALS['pdo']->query($sql2);
          $row2 = $res2->fetch(PDO::FETCH_ASSOC);
       
          //   
          return  "Dam  {$row1['short_name']}, Grand-Dam {$row2['short_name']}";          
    }
        
    function calfNameQuickForm() {

        $sql = "SELECT bovine.id,bovine.local_number,bovine.dam_full_reg_number,bovine.sire_full_reg_number,bovinedam.full_name as dam_full_name,sire.full_name as sire_full_name,sire.short_name as sire_short_name
FROM bovinemanagement.bovine 
LEFT JOIN bovinemanagement.bovine as bovinedam ON bovinedam.full_reg_number = bovine.dam_full_reg_number
LEFT JOIN bovinemanagement.sire as sire on sire.full_reg_number = bovine.sire_full_reg_number
WHERE bovine.full_name IS NULL AND bovine.death_date IS NULL ORDER BY local_number";
        $res = $GLOBALS['pdo']->query($sql);
        $bovineToBeNamed[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if ($row['sire_short_name'] == NULL) {
                //throw new exception("Unknown Sire for Calf #{$row['local_number']}, please update with sire info before we can continue.");
            }

            //parse for last word of name and then make an array for the select list.
            $bovineToBeNamed[$row['id']] = $row['local_number'];
        }


        $form = new HTML_QuickForm("calfNameQuickForm",   'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'calfNameQuickform', 'Name Calf');
        $form->addElement('select', 'calfNumber', 'Calf Number', $bovineToBeNamed);
        $form->addElement('text', 'newName', 'New Calf Name','onkeyup="this.value = this.value.toUpperCase();"');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('animalNumber' => 0)); //set default to no variety.
        $form->setDefaults(array('dateHeat' => 0));
        $form->setDefaults(array('heatType' => 0));

        //rules
        $form->addRule('calfNumber', 'Calf number is required', 'required');
        $form->addRule('newName', 'Calf Name is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $bovine_id = $form->exportValue('calfNumber');
            $newName = strtoupper($form->exportValue('newName'));
            $pageid = $form->exportValue('pageid');

            print("bovine_id:$bovine_id<br>");
            print("newName:$newName<br>");
            
            //don't put id value, uses next one in DB.
            $query = "INSERT INTO bovinemanagement.calf_potential_name (bovine_id,potential_name,userid) VALUES ($bovine_id,'$newName','{$GLOBALS['auth']->getUsername()}')";
            
            $res = $GLOBALS['pdo']->exec($query);

            //forward to same page, ob_start() is key to making this work!
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation
        //displayLastEstrusEvents();
        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    

    /*******************************************
     * Show Delete Button.
     * ******************************************* */
    function printActionButtons($params) {
        extract($params);
        $returnButtonsAsString=null;
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        //allow any  that has not yet been given to be deleted by management.
          if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $sql = "SELECT id,'calf_potential_name' as tablename FROM bovinemanagement.calf_potential_name LEFT JOIN batch.holstein_canada_registered ON holstein_canada_registered.bovine_id=calf_potential_name.bovine_id  WHERE event_time_reg_sent is null ORDER BY calf_potential_name.update_time DESC ";
        } else { //only allow the last one to be deleted for normal users.
            $sql = "SELECT id,'calf_potential_name' as tablename FROM bovinemanagement.calf_potential_name LEFT JOIN batch.holstein_canada_registered ON holstein_canada_registered.bovine_id=calf_potential_name.bovine_id  WHERE userid='$currentUserid' AND event_time_reg_sent is null ORDER BY calf_potential_name.update_time DESC limit 1 ";
        }
        $res = $GLOBALS['pdo']->query($sql);

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            if (($record['id'] == $row['id'])) {

                $returnButtonsAsString = DataGridHelper::deleteQuickForm($row['id'], $row['tablename']); //show delete button, with Breeding table id as index for delete.
            }
        }

        //print button
        //$returnButtonsAsString=$returnButtonsAsString;

        return $returnButtonsAsString;
    }

    
    public function error() {
        $error = array();

        //find who doesn't have a name yet that should, that are over 30 days old.
        $res = $GLOBALS['pdo']->query("
                SELECT count(bovine.id) as count, now() as event_time
                FROM bovinemanagement.bovine 
                 LEFT JOIN batch.holstein_canada_registered ON bovine.id=bovine_id 
                 LEFT JOIN bovinemanagement.calf_potential_name ON bovine.id=calf_potential_name.bovine_id 
                 WHERE local_reg_status=false AND death_date is null AND potential_name is null AND date_part('epoch',(birth_date))::integer  < date_part('epoch',( now() - interval '30 days'))::integer 
                                        ");
          
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['count'] >0){
            $error[] = new notifyObj(get_class($this), $row['event_time'], 3, $row['count'] . ' calves are overdue for a potential name.');
            }
        }
        return $error;
    }
    
    
}//end class
?>