<?php 





include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
include_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/movementLocationList.inc');


//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if (class_exists('DataGridLR_Ext') != true) { //FFDA7F
    //extends datgrid class with specific logic to colour the line.

    class DataGridLR_Ext extends DataGridLR {

        function colourLineCriteria($row) {
            if ($row['criteria'] >= 100) {
                return true;
            } else {
                return false;
            }
        }

    }

}

class Vaccination extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Vaccination Entry';
        $tabArray[2]['name'] = 'BVD Vaccination Required';
        $tabArray[3]['name'] = 'Covaxin (Blackleg) Vaccination Report';
        $tabArray[4]['name'] = 'ScourGuard 4KC List';
          $tabArray[5]['name'] = 'JVac Ecoli Mastitis';
        $this->renderTabs($tabArray);
    }

    function tab1() {
        print('<div id="twoColumnLayout">');
     
        
        //   $res = $GLOBALS['pdo']->query("SELECT id,trade_name,default_dosage,default_dosage_unit,default_body_location from bovinemanagement.medicine WHERE medicine_class='vaccine'");
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');    
        $paramMed= new stdClass();
        $b=new QF2_GeneralTreatment((object) array(forwardURLArr=>array('pageid'=>$_REQUEST['pageid'])),'group',null); //group
        print($b->renderBox('qf2_GeneralTreatment','Select Animals / Groups to Treat'));
        print($b->formJS());
        
        
        $this->displayListOfVaccinesAdministered();
        print('</div>');
    }

    function tab2() {
        print("<h3>BVD Vaccinations Needed</h3>");
        print('<h4>Use Bovi-Shield Gold 5 or Starvac 4+ OR Triangle 4+ when directed.</h4>');
        $this->displayListNeedingBVDShot();
    }

    function tab3() {

        print("<h3>Blackleg Vaccinations Needed</h3>");
        print('<h4>Use Covexin 8 when directed.</h4>');
        pRINT("<h1>FIXME: DOES NOT WORK. NEEDS TO BE CHANGED TO AN AJAX REQUEST, TO LOAD TABLE, SO THAT THE location request variable gets through to the tab. ajax avoids this.</h1>");
        MovementLocationList::locationSelectQuickFormJS();
        print_r($_REQUEST);
        if (isset($_REQUEST['location']) == true) {

            print($this->displayListNeedingBlacklegShot($_REQUEST['location']));
        } else {
            print("<h3>No Location Selected. Please select one.</h3>");
        }
    }

    function tab4() {
        print("<h3>AcourGuard 4KC Vaccinations Needed</h3>");

        print($this->displayListNeedingScourGaurd4KCShot());
    }
    
     function tab5() {
        print("<h3>JVac</h3>");

      //  print($this->displayListNeedingScourGaurd4KCShot());
    }

    function displayListNeedingScourGaurd4KCShot() {
        $sql="with temp as (
SELECT id as bovine_id,local_number,full_name,location_current_name,calculated_potential_due_date as due_date,
(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=pregnant_view.id AND (medicine_administered.medicine_index=87) ) as lifetime_shot_count,
 calculated_potential_due_date - (interval '9 weeks') as nine_weeks,calculated_potential_due_date - (interval '6 weeks') as six_weeks,   calculated_potential_due_date - (interval '3 weeks') as three_weeks
FROM bovinemanagement.pregnant_view 
)
SELECT *, 
(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=temp.bovine_id AND (medicine_administered.medicine_index=87) AND event_time > nine_weeks AND event_time < six_weeks) as period_shot_count_6_9,
(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=temp.bovine_id AND (medicine_administered.medicine_index=87) AND event_time > six_weeks AND event_time < three_weeks) as period_shot_count_3_6
FROM temp WHERE now() > nine_weeks AND now() < three_weeks";
         $dg1 = new DataGridLR($sql, 50);

         /*
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Current Location', 'location_name', '', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Birth Date', 'birth_date', 'birth_date', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Lifetime Shot Count', 'lifetime_shot_count', 'lifetime_shot_count', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Fresh Date', 'fresh_date', 'fresh_date', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Criteria #', 'criteria', 'criteria', array('' => ''), null, null));
*/

        return ($dg1->render('datagrid'));
    }
    
    
    function displayListNeedingBlacklegShot($location_id) {

        /*
         * In order that a balanced response to vaccination is obtained, a primary course
         * of two injections of 4 mL each should be given with an interval of 6 weeks
         * between injections. To maintain a constant high level of immunity, 
         * booster injections should be administered at intervals of 6 months, 
         * or when outbreaks are seasonal, at least 2 weeks before the anticipated outbreak. 
         * Calves vaccinated under 3 months of age should be revaccinated at 4-6 months of age. 
         * Calves vaccinated at 3 months of age or older should be revaccinated 6 weeks later. 
         * Inject subcutaneously with strict aseptic precautions.
         */

        $sql = "
   /*Criteria 1: All animals over 3 months of age and never given a shot. */
   SELECT id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) as lifetime_shot_count,1 as criteria 
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28))=0
   AND location_id=$location_id   

   UNION
   
   /*Criteria 2: All animals over 3 months of age and who have only had one lifetime shot and are over 6 weeks since that shot. */
   SELECT id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) as lifetime_shot_count,2 as criteria 
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28))=1
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) <= current_date - (interval '42 days')
   AND location_id=$location_id   
      
   UNION

   /*Criteria 3: All animals who have >= 2 lifetime shots and it has been >= 6 months since the last shot. */
   SELECT id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) as lifetime_shot_count,3 as criteria 
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28))>=2
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) <= current_date - (interval '6 months')
   AND location_id=$location_id   
      
   ORDER BY location_name,local_number;
    ";
       
        $dg1 = new DataGridLR($sql, 50);

        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Current Location', 'location_name', '', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Birth Date', 'birth_date', 'birth_date', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Lifetime Shot Count', 'lifetime_shot_count', 'lifetime_shot_count', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Fresh Date', 'fresh_date', 'fresh_date', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Criteria #', 'criteria', 'criteria', array('' => ''), null, null));


        return ($dg1->render('datagrid'));

        // $dg1->pager();
    }

    
    
     public function listNeedingBVDShot($mode = 'default') {
        $sql = "
   
   /* CRITERIA 1: Calf over 3 months of age, and lifetime vaccinations = 0 and lifetime breedings=0 and calvings=0. */ 
   SELECT '1' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58))=0
   AND (SELECT count(state_frozen) FROM bovinemanagement.breeding_event WHERE bovine_id=bovinecurr.id)=0
   AND (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)=0
   
   UNION     
   
   /* CRITERIA 2: Calves that have  lifetime vaccinations = 1  and lifetime breedings=0 and more than 20 days since 1st shot and calvings=0  NOTE: maximum(event_time) is just used to keep the returned results at 1 or less*/
   SELECT '2' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count 
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date
   FROM bovinemanagement.bovinecurr 
   WHERE (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58))=1
   AND (SELECT count(state_frozen) FROM bovinemanagement.breeding_event WHERE bovine_id=bovinecurr.id)=0
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) <= current_date - (interval '20 days')
   AND (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)=0
   
   
   UNION
    
   /* CRITERIA 3: Calves lifetime breedings=0 and lifetime vaccinations >= 2 and more than 2 months from last shot. */
   SELECT '3' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count 
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date   FROM bovinemanagement.bovinecurr 
   WHERE
            (SELECT count(state_frozen) FROM bovinemanagement.breeding_event WHERE bovine_id=bovinecurr.id)=0
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58))>=2
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) <= current_date - (interval '2 months')
   AND (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)=0
   
   UNION
   
   /* CRITERIA 4: Cows more than 25 days fresh (breedings=0 since fresh) and (vaccinations = 0 since fresh). */
   SELECT '4' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count  
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date
   FROM bovinemanagement.bovinecurr 
   WHERE
   (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)>=1
   AND bovinecurr.fresh_date + (interval '25 days') <= current_date
   AND (SELECT count(state_frozen) FROM bovinemanagement.breeding_event WHERE breeding_event.bovine_id=bovinecurr.id AND breeding_event.actual_breeding_time >= bovinecurr.fresh_date)=0
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58) AND event_time > bovinecurr.fresh_date)=0
   
   UNION
   
 
   /* CRITERIA Urgent 1: Calves lifetime breedings=0 and  lifetime vaccinations >= 2 and more than 3 months from last shot. */
  SELECT 'U1' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count 
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date   FROM bovinemanagement.bovinecurr 
   WHERE
            (SELECT count(state_frozen) FROM bovinemanagement.breeding_event WHERE bovine_id=bovinecurr.id)=0
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58))>=2
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) <= current_date - (interval '3 months')
   AND (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)=0
  
   UNION
   
   /* CRITERIA Urgent 2:  Cows with (vaccinations = 0 since fresh) and breedings since fresh > 0 */
   SELECT 'U2' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count  
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date
   FROM bovinemanagement.bovinecurr 
   WHERE
   (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)>=1
   AND  (SELECT count(id) FROM bovinemanagement.combined_breeding_embryo_view WHERE recipient_bovine_id=bovinecurr.id AND event_time <= current_date and event_time > bovinecurr.fresh_date )>=1
   AND bovinecurr.fresh_date < current_date
   AND bovinemanagement.calculate_breeding_voluntary_waiting_period(bovinecurr.id) - (interval '1 months') < current_date
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58) AND event_time > bovinecurr.fresh_date)=0
   
   ORDER BY location_name,local_number;
   ";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

       
        //change presentation
        foreach ($results as &$row) {
           //Animal	Current Location	Birth Date	Lifetime Shot Count	Fresh Date	Last Shot Date	 #	Drug To UseAnimal	Current Location	Birth Date	Lifetime Shot Count	Fresh Date	Last Shot Date	Criteria #	Drug To Use
   
            $row['local_number'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $row['birth_date'] = date("M d Y", strtotime($row['birth_date']));
            $row['current_location'] = strtotime($row['location_name']);
            //lifetime_shot_count
             $row['fresh_date'] =  (empty($row['fresh_date']) ? '' : date("M d Y", strtotime($row['fresh_date'])));
              $row['last_shot_date'] =  (empty($row['last_shot_date']) ? '' : date("M d Y", strtotime($row['last_shot_date'])));
            //criteria
            $row['drug_to_use'] = $this->printDrugToUse($row['bovine_id']);
            
            
            //find U1 and U2, mark urgent.
            if (strlen($row['criteria'])>1) {
                $row['colour_row'] = 1;
            } else {
                $row['colour_row'] = 0;
            }
            
            
            
        }

        //output
        $output['data'] = $results;
        return ($output);
    }


    
    
    function displayListNeedingBVDShot($mode = 'default') {
  $options = '"paging": false,  "order": [[0,\'asc\']],';
        
        
        $dt=new JQueryDataTable();
$columnsStr= <<<EX
            
               {
                data: "local_number",
                title: "Animal",
                type: "num-html"
        },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "birth_date",
                title: "Birth Date",
                type: "date"
            },
     
            {
                data: "lifetime_shot_count",
                title: "Lifetime Shot Count"
            },
        {
                data: "fresh_date",
                title: "Fresh Date",
                type: "date"
            },
        {
                data: "last_shot_date",
                title: "Last Shot Date",
                type: "date"
        
            },
             {
                data: "criteria",
                title: "Criteria",
                type: "date"
            },
            {
                data: "drug_to_use",
                title: "Drug To Use"
            }
EX;
   
     print($dt->start(84,'listNeedingBVDShot',$mode,$columnsStr,'highlight_important',$options));
}     
        
     
    

    /**
     * if the cow could be pregnant we can only use certain vaccines.
     */
    function printDrugToUse($bovine_id) {
       
        //be conservative and assume if they have been bred, that they are pregnant.
        $res = $GLOBALS['pdo']->query("
   SELECT id FROM  bovinemanagement.bovinecurr WHERE bovinecurr.id=$bovine_id AND bovinemanagement.last_breeding($bovine_id) is not null  
  ");
        if ($res->rowCount() >= 1) {
            return 'Pregnancy Risk: Use Triangle 4+ Vaccine.';
        } else {
            return 'Bovi-Shield Gold 5';
        }
    }

    

    /**     * ****************************************** */
    /** Display list of all vaccines Administered   */

    /**     * ****************************************** */
    function displayListOfVaccinesAdministered($mode = 'default') {

        $sql = "SELECT medicine_administered.id as id,bovine_id as bovine_id,local_number,full_name,trade_name,dosage,dosage_unit,location,note,userid,event_time
  FROM bovinemanagement.medicine_administered 
  LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
  LEFT JOIN bovinemanagement.bovinecurr ON medicine_administered.bovine_id = bovinecurr.id
  WHERE medicine.medicine_class='vaccine'
  ORDER BY medicine_administered.event_time DESC,trade_name,local_number";


        $dg1 = new DataGridLR($sql, 20);

        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Animal', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedBovineNameAndNumber()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('trade name', 'trade_name', 'trade_name', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('dosage', 'dosage', 'dosage', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('dosage unit', 'dosage_unit', 'dosage_unit', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('location', 'location', 'location', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('note', 'note', 'note', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('userid', 'userid', 'userid', array('' => ''), null, null));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Event Time', 'event_time', 'event_time', array('' => ''), null, array('DataGridHelper', 'printFormattedDateGeneric()')));
        $dg1->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('Vaccination', 'printActionButtons()')));
        print($dg1->render('datagrid'));
        $dg1->pager();
    }

    function printActionButtons($params) {
        extract($params);
        $currentUserid = $GLOBALS ['auth']->getUsername();


        $res = $GLOBALS ['pdo']->query("SELECT medicine_administered.id,'medicine_administered' as tablename FROM bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine_index=medicine.id WHERE userid='$currentUserid' AND medicine.medicine_class='vaccine' ORDER BY update_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $returnButtonsAsString = null;
        if (($record ['id'] == $row ['id'])) {
            $returnButtonsAsString = MedicalCase::deleteQuickForm($row ['id'], $row ['tablename']); // show
            //delete button, with _____ table id as index for delete.
        }

        return $returnButtonsAsString;
    }

}

//end class  