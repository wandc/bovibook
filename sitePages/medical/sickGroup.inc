<?php 
class SickGroup extends BasePage{


	public function defaultDisplay() {
	
                print($this->sickCowBoxes());
    }


function sickCowBoxes() {
    
    //<span class="label label-success">Approved</span>
    
       $sql = <<<SQL
SELECT bovinecurr.id as bovine_id,local_number,full_name,location_last_event_time,date_trunc('days',now()-location_last_event_time) as age_location_last_event_time,birth_date,date_trunc('days',now()-fresh_date) as age_fresh_date,(SELECT milk_withholding FROM bovinemanagement.milk_and_beef_withholding(bovinecurr.id)) as milk_withholding,
		(SELECT open_date FROM bovinemanagement.medical_case WHERE close_date is null AND bovine_id=bovinecurr.id ) as medical_case_open_date
		FROM bovinemanagement.bovinecurr WHERE location_id=39
SQL;
               
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            
            $classBox='';
            /*
             * BOX COLOUR : why there
             */
            if (!empty($row['medical_case_open_date'])) {
            $classBox='calfBoxYellow';   //medical case open
            }
            elseif ($dryoff==true) {  //there to be dried off
                 $classBox='calfBoxPurple';
            }
            else {
                  $classBox='calfBoxGreen'; //clear to not be there. 
            }
            
            
            /*
             * BORDER OF BOX COLOUR: milk in tank status
             */
           $classBoxBorder='';
            
           if (empty($row['age_fresh_date'])) {
                $classBoxBorder = 'calfBoxAttentionSolidPurple';  //dry or not a milking animal, does not need to be in group
            } elseif (strtotime($row['milk_withholding']) <= strtotime('+12 hour')) {
                $classBoxBorder = 'calfBoxAttentionSolidYellow';  //milk withholding almost over, might be able to be put out.
            } elseif (!empty($row['milk_withholding'])) {
                $classBoxBorder = 'calfBoxAttentionSolidRed';   //milk with holding                  
            } else {
                $classBoxBorder = 'calfBoxAttentionSolidGreen';
            }

            $bootstrapModal=new ModalBox($row['bovine_id']);
       
           $str[]= '<div class="calfBox ' . $classBox . ' '. $classBoxBorder .'">';
           $str[]= ('<h3>' . '<img class=""    src="/images/cowHeadFront.svg"><a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['bovine_id']) . '" >' . $row['local_number'] . '</a></h3>');
           $str[]= '<ul>';
           $str[]= '<li>'.$bootstrapModal->button().' '.$openMedicalCase.' </li>';
           $str[]= ('<li>DIM: '  .$row['age_fresh_date'] . '</li>');
        //   $str[]= ('<li>' . 'Drank 24h: ' . round($row['gefressen_letzte_24h'], 1) . ' / ' . round($row['gefressen_letzte_24h'] + $row['bewertung_futter_24h']) . '&#8467</li>');
           $str[]= '</ul>';
           $str[]= $manualMilkText;
           $str[]= ('</div>');
            
           $str[]= ($bootstrapModal->popup($popoverTitle,$popoverContent));
    
        }
           return implode($str);
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
  $str=$str.'<span id=\'colourBack\'>milk withhold until: '.date('D M j/Y ga',strtotime($record['milk_withholding'])).'</span><br/>';
  }
  
  return $str;
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