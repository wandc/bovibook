<?php 
class SickGroup extends BasePage{


	public function defaultDisplay() {
           
	
             
    }


  
    
    
    function sickCowBoxes() {


        $sql = <<<SQL
SELECT bovinecurr.id as bovine_id,local_number,full_name,location_last_event_time,date_trunc('days',now()-location_last_event_time) as age_location_last_event_time,
    birth_date,EXTRACT(DAY FROM now()-fresh_date) as age_fresh_date,(SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding(bovinecurr.id) limit 1) as milk_withholding,
		(SELECT open_date FROM bovinemanagement.medical_case WHERE close_date is null AND bovine_id=bovinecurr.id limit 1) as medical_case_open_date, (SELECT true FROM bovinemanagement.dryoff_schedule LEFT JOIN bovinemanagement.lactation on lactation.id=lactation_id WHERE dryoff_event_id is null AND bovine_id=bovinecurr.id limit 1) as dry_off_schedule
		FROM bovinemanagement.bovinecurr WHERE location_id=39
SQL;
        $str[]='';
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $str=null;

            $str[] = ('<h3>' . '<img class=""    src="/images/cowHeadFront.svg"><a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['bovine_id']) . '" >' . $row['local_number'] . '</a></h3>');
            $str[] = '<ul>';
            $str[] = ('<li>DIM: ' . $row['age_fresh_date'] . 'ᵈ</li>');
           
            
            $str[]= (new BovineQuery)->getLatestMilkingSimple($row['bovine_id'], 2,false);
            
            $str[] = '</ul>';

        
        
        //generate alert w/ color
        
        if (($row['dry_off_schedule'])==true) {
            $out[]=(new BootStrap)->infoAlertBasic(implode($str));    //in dry off protocol, as soon as they are dry treated, then turn red.        
            } elseif ((strtotime($row['milk_withholding']) <= strtotime('+12 hour')) AND (strtotime($row['milk_withholding']) > strtotime('now'))) {
                $out[]=(new BootStrap)->warningAlertBasic(implode($str)); //milk withholding almost over, might be able to be put out.
            } elseif (!empty($row['milk_withholding'])) {
                $out[]=(new BootStrap)->dangerAlertBasic(implode($str)); //milk with holding                  
            } else {
                $out[]=(new BootStrap)->successAlertBasic(implode($str)); //nothing wrong
            }   
            
        }  
        
        return implode($out);
    }

    
    
       public function getLatestMilkingss($bovine_id,$days=7) {

        $sql = <<<SQL
SELECT DISTINCT on (date) date as "Date"
,COALESCE( round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=1 AND x.date=cow.date ),1)::text,'??'::text) as "AM"
,COALESCE( round((SELECT avg(milkyield) FROM alpro.cow x WHERE bovine_id=$bovine_id AND milking_number=2 AND x.date=cow.date ),1)::text,'??'::text) as "PM"
		FROM alpro.cow 
                WHERE bovine_id=$bovine_id AND date >= (current_date - interval '$days days') ORDER BY date DESC limit 2
SQL;
       
        
        $out[] = ( (new JQueryDataTable)->startBasicSql('Latest Parlor Milkings', $sql));
        return implode($out);
    }
    
    /** ******************************************/
  /** Milk Withholding **/
  /** ******************************************/
  function printMilkWithholding($params)
  {
  extract($params);
  $str='';
   //only show milk withholding if it is in the future, else it doesn't apply
  if (strtotime($record['milk_withholding']) > strtotime("now")) {
      $txt='milk withhold until: '.date('D M j/Y ga',strtotime($record['milk_withholding']));
      $str=$str. (new BootStrap)->dangerAlert('Milk', $txt);
      
  }
  
  return $str;
  }

  
public function sickGroupParams($bovine_id) {
  
   $sql = <<<SQL
      SELECT bovinecurr.id as bovine_id,local_number,full_name,location_last_event_time,date_trunc('days',now()-location_last_event_time) as age_location_last_event_time,birth_date,date_trunc('days',now()-fresh_date) as age_fresh_date,(SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding(bovinecurr.id)) as milk_withholding,
		(SELECT max(open_date) FROM bovinemanagement.medical_case WHERE close_date is null AND bovine_id=bovinecurr.id ) as medical_case_open_date, (SELECT true FROM bovinemanagement.dryoff_schedule LEFT JOIN bovinemanagement.lactation on lactation.id=lactation_id WHERE dryoff_event_id is null AND bovine_id=bovinecurr.id ) as dry_off_schedule
		FROM bovinemanagement.bovinecurr WHERE bovinecurr.id=$bovine_id limit 1
SQL;

        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
       
  if (!empty($row['dry_off_schedule'])) {
      $out[]= (new BootStrap)->infoAlert('Milk', 'DRY OFF');
  }
            
  if (!empty($row['milk_withholding'])) {
      $out[]= (new BootStrap)->dangerAlert('Milk', 'MILK WITHHOLD');
  } else {
      $out[]= (new BootStrap)->successAlert('Milk', 'MILK Clear');
  }
            
        
        return implode($out);
  }
  
  

private function reasonsToBeInSickGroup($age_fresh_date) {
  
  
         $returnMessage=null;

	 //do all reason with respect to todays date

	 //if animal is less than 420 days old then too young.
	 if ($age_fresh_date != '') {
	 
	 if (strtotime($age_fresh_date) < strtotime("3 days")) {
	 	$returnMessage='Just Fresh.';
	 }
	 
	  if (strtotime($age_fresh_date) < strtotime("5 days")  &&(strtotime($age_fresh_date) > strtotime("3 days"))) {
	 	$returnMessage='Fresh, but should be leaving.....';
	 }
	 }

	 return $returnMessage;
	}

            
    /* checks all cows to see if one is in a lactating group at the moment. */
     public function milkWithHoldingAllCowsSaleableMilkGroup() {
        $sql = "with temp as (
SELECT *,bovinecurr.id as bovine_id,(SELECT milk_withholding FROM  bovinemanagement.milk_and_beef_withholding(bovinecurr.id) ) as milk_withholding
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.location ON location.id=bovinecurr.location_id
WHERE saleable_milk=TRUE
) 
SELECT * FROM temp 
WHERE milk_withholding >= CURRENT_TIMESTAMP";      
        $res = $GLOBALS ['pdo']->query($sql);
        $out=array();
          while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
              $out[]='<span>'.$GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']) . '  until: ' .  ' '. date('D M j/Y ga', (new BovineQueryMedicalLog)->milkWithHolding($row['bovine_id'])) . '</span><br/>';
          }
          
          if (empty($out)) {
          return '';    //maybe blank is better.
          //return (new BootStrap)->sucessNotify('All Treated Cows in Sick Group.'); 
          }
          else {
            return (new BootStrap)->errorNotify('<h3>Warning Treated Cow(s) are in a Milking Group:</h3>'.implode($out));   
          }
              
          
     
    }
        

public function error() {
/** if there is no reason for the cow to be in the sick group send a notification error. */
          $error=null;
          $sql ="SELECT bovinecurr.id as bovine_id,local_number,full_name,location_last_event_time,date_trunc('days',now()-location_last_event_time) as age_location_last_event_time,birth_date,date_trunc('days',now()-fresh_date) as age_fresh_date,(SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding(bovinecurr.id)) as milk_withholding,
		(SELECT open_date FROM bovinemanagement.medical_case WHERE close_date is null AND bovine_id=bovinecurr.id ) as medical_case_open_date
		FROM bovinemanagement.bovinecurr WHERE location_id=39 ";
          $res = $GLOBALS['pdo']->query($sql);
		while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
		  if ((self::reasonsToBeInSickGroup($row['age_fresh_date']) == null) && ($row['medical_case_open_date'] == null)) {
		  $error["{$row['local_number']}"]= new notifyObj(get_class($this),$row['location_last_event_time'],2,$row['local_number'] . ' should not be in sick group.');
		  }
		}
  
 return $error; 
}


} //end class
?>