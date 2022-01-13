<?php

class Vaccination extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Vaccination Entry';
        $tabArray[2]['name'] = 'BVD Vaccination Required';
        $tabArray[3]['name'] = 'Covaxin (Blackleg) Vaccination Report';
        $tabArray[4]['name'] = 'ScourGuard 4KC List';
        $tabArray[5]['name'] = 'TopVac';
        $this->renderTabs($tabArray);
    }

    function tab1() {
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');
         $paramMed = new stdClass();
         $paramMed->medicineListCustom = 'vaccine';
         $paramMed->bovineID=null;
         $paramMed->aniNumber=null; 
        $b = new QF2_GeneralTreatment((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group', $paramMed); //group

        $html = <<<HTML
    <div class='row'>
        <div class='col-sm-4 col-sm-push-8'>
            {$b->renderBox('qf2_GeneralTreatmentVaccination', 'Select Animals / Groups to Treat')}
         
        </div>
        <div class='col-sm-8 col-sm-pull-4'> 
            {$this->displayListOfVaccinesAdministered()}
        </div>
    </div>    
HTML;

        print($html);
    }

    function tab2() {
      print(new BootStrap)->infoNotify('Use Bovi-Shield Gold 5 or Starvac 4+ or Triangle 4+ where directed.');

             $body=<<<BODY
      <h3>Heifers</h3> 
<ul> 1st dose between 6 & 8 months of age </ul> 
<ul>  booster 30 days later </ul> 
<ul> 3rd shot at 13 months </ul> 
<h3>Cows</h3>
 <ul>  one shot each year, 30 days after freshing </ul> 
BODY;
               print(new BootStrap)->plainCardCollapsed('BVD Protocol', $body);
        print($this->displayListNeedingBVDShot());
    }

    function tab3() {

                   $body=<<<BODY
                
        <p>  In order that a balanced response to vaccination is obtained, a primary course
          of two injections of 4 mL each should be given with an interval of 6 weeks
          between injections. To maintain a constant high level of immunity, 
         booster injections should be administered at intervals of 6 months, 
          or when outbreaks are seasonal, at least 2 weeks before the anticipated outbreak. 
         Calves vaccinated under 3 months of age should be revaccinated at 4-6 months of age. 
          Calves vaccinated at 3 months of age or older should be revaccinated 6 weeks later. 
          Inject subcutaneously with strict aseptic precautions. </p>                 
        BODY;
                   
        print(new BootStrap)->plainCardCollapsed('Covaxin 8 Protocol', $body);
        

            print($this->displayListNeedingBlacklegShot());
     
    }

    function tab4() {

        print($this->displayListNeedingScourGaurd4KCShot());
    }

    function tab5() {
     
        
        $body=<<<BODY
                
        <p> CRITERIA 1: All females, with zero shots, ie 45 days before due (or fresh) .</p>
        <p> CRITERIA 2: All females with one shots, and min 30 days since last shot until due date - '45 days' ) </p> 
        <p> CRITERIA 3: All females with two shots, and min 55 days since last shot untildue - interval '45 days' ) </p>                 
        BODY;
        print(new BootStrap)->plainCardCollapsed('TopVac Protocol', $body);
        
        print($this->displayListNeedingTopVacShot());
    }

    function displayListNeedingScourGaurd4KCShot() {
        $sql = "with temp as (
SELECT id as bovine_id,local_number,full_name,location_current_name,calculated_potential_due_date as due_date,
(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=pregnant_view.id AND (medicine_administered.medicine_index=87) ) as lifetime_shot_count,
 calculated_potential_due_date - (interval '9 weeks') as nine_weeks,calculated_potential_due_date - (interval '6 weeks') as six_weeks,   calculated_potential_due_date - (interval '3 weeks') as three_weeks
FROM bovinemanagement.pregnant_view 
)
SELECT *, 
(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=temp.bovine_id AND (medicine_administered.medicine_index=87) AND event_time > nine_weeks AND event_time < six_weeks) as period_shot_count_6_9,
(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=temp.bovine_id AND (medicine_administered.medicine_index=87) AND event_time > six_weeks AND event_time < three_weeks) as period_shot_count_3_6
FROM temp WHERE now() > nine_weeks AND now() < three_weeks";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = ($row['location_current_name']);
            $rowOut['Lifetime Shot Count'] = ($row['lifetime_shot_count']);
            $rowOut['Due Date'] = JQueryDataTable::formattedDate($row['due_date']);
            
             $rowOut['lifetime_shot_count']=$row['lifetime_shot_count'];
             $rowOut['nine_weeks']=$row['nine_weeks'];
             $rowOut['six_weeks']=$row['six_weeks'];	
             $rowOut['three_weeks']=$row['three_weeks'];	
             $rowOut['period_shot_count_6_9']=$row['period_shot_count_6_9'];	
             $rowOut['period_shot_count_3_6']=$row['period_shot_count_3_6'];

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('ScourGuard 4KC Vaccinations Needed', $dataArr, ''));
        return implode($out);
    }

    function displayListNeedingBlacklegShot() {

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

        $sql = <<<SQL
 /*Criteria 1: All animals over 3 months of age and never given a shot. */
   SELECT id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) as lifetime_shot_count,1 as criteria 
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28))=0
 

   UNION
   
   /*Criteria 2: All animals over 3 months of age and who have only had one lifetime shot and are over 6 weeks since that shot. */
   SELECT id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) as lifetime_shot_count,2 as criteria 
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28))=1
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) <= current_date - (interval '42 days')
    
      
   UNION

   /*Criteria 3: All animals who have >= 2 lifetime shots and it has been >= 6 months since the last shot. */
   SELECT id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) as lifetime_shot_count,3 as criteria 
   FROM bovinemanagement.bovinecurr 
   WHERE (bovinecurr.birth_date <= (current_date - interval '3 months')) 
   AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28))>=2
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=28)) <= current_date - (interval '6 months')
 
      
   ORDER BY location_name,local_number;
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = ($row['location_name']);
            $rowOut['Birth Date'] = JQueryDataTable::formattedDate($row['birth_date']);
            $rowOut['Lifetime Shot Count'] = ($row['lifetime_shot_count']);
            $rowOut['Fresh Date'] = JQueryDataTable::formattedDate($row['fresh_date']);
            $rowOut['Criteria #'] = ($row['criteria']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Animals Needing Blackleg Vaccine', $dataArr, $options = 'paging: false'));
        return implode($out);
    }

    public function displayListNeedingTopVacShot($mode = 'default') {

        $sql = <<<SQL
               
  /* CRITERIA 1: All females vaccinations = 0 bovinemanagement.due_date(id) - interval '45 days', ie 45 days before due (or fresh) . */ 
   SELECT '1' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,bovinemanagement.due_date(id) as due_date
   ,null as last_shot_date
,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND    ( (event_time  BETWEEN (bovinemanagement.due_date(id) - interval '45 days' ) AND bovinemanagement.due_date(id))  OR (event_time  BETWEEN (fresh_date- interval '45 days' ) AND fresh_date) ) )
   FROM bovinemanagement.bovinecurr 
   WHERE 
   (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND ((event_time  >= (bovinemanagement.due_date(bovine_id) - interval '45 days' ))  OR  (event_time  >= (fresh_date - interval '45 days' )) )  )=0 AND             
   ( (current_date  BETWEEN (bovinemanagement.due_date(id) - interval '45 days' ) AND bovinemanagement.due_date(id))  OR (current_date  BETWEEN (fresh_date- interval '45 days' ) AND fresh_date) )
    AND (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND    ( (event_time  BETWEEN (bovinemanagement.due_date(id) - interval '45 days' ) AND bovinemanagement.due_date(id))  OR (event_time  BETWEEN (fresh_date- interval '45 days' ) AND fresh_date) ) ) =0


   UNION

      /* CRITERIA 2: All females  vaccinations = 1 and min 30 days since last shot until bovinemanagement.due_date(id) - interval '45 days' ) */ 

  SELECT '2' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,bovinemanagement.due_date(id) as due_date
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=96AND     ((event_time  >= (bovinemanagement.due_date(bovine_id) - interval '45 days' ))  OR  (event_time  >= (fresh_date - interval '45 days' )) )  ) ) as last_shot_date
,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND    ( (event_time  BETWEEN (bovinemanagement.due_date(id) - interval '45 days' ) AND bovinemanagement.due_date(id))  OR (event_time  BETWEEN (fresh_date- interval '45 days' ) AND fresh_date) ) )
   FROM bovinemanagement.bovinecurr 
   WHERE 
   (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND ((event_time  >= (bovinemanagement.due_date(bovine_id) - interval '45 days' ))  OR  (event_time  >= (fresh_date - interval '45 days' )) )  )=1
AND ((SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96) + interval '30 days') <= current_date
   
   UNION
      
    /* CRITERIA 3: All females  vaccinations = 2 and min 55 days since last shot until bovinemanagement.due_date(id) - interval '45 days' ) */ 
  
  SELECT '3' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,bovinemanagement.due_date(id) as due_date
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=96 AND     ((event_time  >= (bovinemanagement.due_date(bovine_id) - interval '45 days' ))  OR  (event_time  >= (fresh_date - interval '45 days' )) )  ) ) as last_shot_date
,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND    ( (event_time  BETWEEN (bovinemanagement.due_date(id) - interval '45 days' ) AND bovinemanagement.due_date(id))  OR (event_time  BETWEEN (fresh_date- interval '45 days' ) AND fresh_date) ) )
   FROM bovinemanagement.bovinecurr 
   WHERE 
   (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96 AND ((event_time  >= (bovinemanagement.due_date(bovine_id) - interval '45 days' ))  OR  (event_time  >= (fresh_date - interval '45 days' )) )  )=2
AND ((SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND medicine_administered.medicine_index=96) + interval '55 days') <= current_date


   ORDER BY location_name,local_number;
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = ($row['location_name']);
             $rowOut['Due Date'] = JQueryDataTable::formattedDate($row['due_date']);
            $rowOut['Birth Date'] = JQueryDataTable::formattedDate($row['birth_date']);
            $rowOut['Last Shot'] = JQueryDataTable::formattedDate($row['last_shot_date']);
            $rowOut['Fresh Date'] = JQueryDataTable::formattedDate($row['fresh_date']);
            $rowOut['Criteria #'] = ($row['criteria']);
             $rowOut['Shots Since -45 Days Calving'] = ($row['count']);

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;

        $out[] = (new BootStrap)->infoNotify('Shot counts reset at 45 days before calving. ');
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('TopVac', $dataArr, $options = 'paging: false'));
        return implode($out);
    }

    public function displayListNeedingBVDShot($mode = 'default') {
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
  
   UNION
 
  /* CRITERIA Urgent 3: Animals with  lifetime vaccinations >= 2 and more than 10 months from last shot. */
  SELECT 'U3' as criteria, id as bovine_id,full_name,local_number,location_name,birth_date,fresh_date,(SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as lifetime_shot_count 
   ,(SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) as last_shot_date   FROM bovinemanagement.bovinecurr 
   WHERE
            (SELECT count(state_frozen) FROM bovinemanagement.breeding_event WHERE bovine_id=bovinecurr.id)=0 and (SELECT count(id) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58))>=2
   AND (SELECT max(event_time) FROM bovinemanagement.medicine_administered WHERE medicine_administered.bovine_id=bovinecurr.id AND (medicine_administered.medicine_index=27 OR medicine_administered.medicine_index=53 OR medicine_administered.medicine_index=58)) <= current_date - (interval '10 months')
   AND (SELECT count(id) FROM bovinemanagement.lactation WHERE bovine_id=bovinecurr.id)=0
  
   ORDER BY location_name,local_number;
   ";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Current Location'] = ($row['location_name']);
            $rowOut['Birth Date'] = JQueryDataTable::formattedDate($row['birth_date']);
            $rowOut['Lifetime Shot Count'] = ($row['lifetime_shot_count']);
            $rowOut['Fresh Date'] = JQueryDataTable::formattedDate($row['fresh_date']);
            $rowOut['Criteria #'] = ($row['criteria']);
            $rowOut['drug_to_use'] = $this->printDrugToUse($row['bovine_id']);
            $rowOut['last_shot_date'] = (empty($row['last_shot_date']) ? '' : date("M d Y", strtotime($row['last_shot_date'])));

            //find U1 and U2, mark urgent.
            if (strlen($row['criteria']) > 1) {
                $rowOut['colour_row'] = 1;
            } else {
                $rowOut['colour_row'] = 0;
            }


            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayColour('BVD Vaccinations Needed', $dataArr, $options = 'paging: false'));
        return implode($out);
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
  WHERE medicine.medicine_class='vaccine' AND event_time > current_date - interval '2 years'
  ORDER BY medicine_administered.event_time DESC,trade_name,local_number ";




        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

            $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            $rowOut['Trade Name'] = ($row['trade_name']);
            $rowOut['Dosage'] = ($row['dosage']);
            $rowOut['Dosage Unit'] = ($row['dosage_unit']);
            $rowOut['Location'] = ($row['location']);
            $rowOut['Note'] = ($row['note']);
            $rowOut['Employee'] = ($row['userid']);
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            //$rowOut['Action'] = $this->printActionButtons($row, "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}&bovine_id={$row['bovine_id']}");
            
             //the generic delete button works for all cases with normal 'id' and 'userid' columns
            $delButton = new QF2_DeleteButtonDecider('user_timeLimit', (object) array('dateTimePeriodDays' => '1', 'schemaTable' => 'bovinemanagement.medicine_administered'));
            $rowOut['Action'] = $delButton->output($row['id']);
            
                  

            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr, 'order:false'));
        return implode($out);
    }

    function printActionButtons($record, $callBackURL) {

        $currentUserid = $GLOBALS ['auth']->getUsername();


        $res = $GLOBALS ['pdo']->query("SELECT medicine_administered.id,'medicine_administered' as tablename FROM bovinemanagement.medicine_administered LEFT JOIN bovinemanagement.medicine ON medicine_index=medicine.id WHERE userid='$currentUserid' AND medicine.medicine_class='vaccine' ORDER BY update_time DESC LIMIT 1");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $returnButtonsAsString = null;
        if (($record ['id'] == $row ['id'])) {
            $returnButtonsAsString = (new MedicalCase)->deleteQuickForm($row ['id'], $row ['tablename'], $callBackURL); // show
            //delete button, with _____ table id as index for delete.
        }

        return $returnButtonsAsString;
    }

}
//end class  