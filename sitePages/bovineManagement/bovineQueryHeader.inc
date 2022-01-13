<?php 
/*
 * Header box above tabs of bovine query page. 
 */
class BovineQueryHeader {

    private $bovineID;
    private $bovineLocalNumber;
    private $bovineShortName;
    private $bovineFullRegNumber;
    private $bovineRfidNumber;
    
    public function __construct($bovineID,$bovineLocalNumber,$bovineShortName,$bovineFullRegNumber,$bovineRfidNumber) {
        $this->bovineID = $bovineID;
        $this->bovineLocalNumber= $bovineLocalNumber;
        $this->bovineShortName= $bovineShortName;
        $this->bovineFullRegNumber = $bovineFullRegNumber;
        $this->bovineRfidNumber = $bovineRfidNumber;               
    }

// end constructor
    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    /** main method with name,etc. at top of page */
    public function main() {

        
        
        
        // load from bovine table so we can show dead cows, etc.
        $sql = "SELECT bovine.*,dam.full_name as dam_full_name,dam.local_number as dam_local_number, calf_potential_name.potential_name,
            dam.id as dam_id, bovinecurrall.location_name, aggregate_view_curr.lpi,aggregate_view_curr.prodoll,aggregate_view_curr.geno_test,
            sire.short_name as sire_short_name, (SELECT dnatest_type FROM bovinemanagement.dnatest_event WHERE bovine_id={$this->bovineID} ORDER BY event_time DESC limit 1) as dna_pending,
                 batch.prodoll_birthyear_quintile_rank( bovine.id,(EXTRACT(YEAR FROM bovine.birth_date))::integer)
FROM bovinemanagement.bovine
LEFT JOIN bovinemanagement.calf_potential_name ON calf_potential_name.bovine_id = bovine.id
LEFT JOIN bovinemanagement.bovine as dam ON dam.full_reg_number=bovine.dam_full_reg_number
LEFT JOIN batch.aggregate_view_curr    ON aggregate_view_curr.full_reg_number = bovine.full_reg_number
LEFT JOIN bovinemanagement.bovinecurrall ON bovinecurrall.id=bovine.id       
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = bovine.sire_full_reg_number
WHERE bovine.id={$this->bovineID} LIMIT 1";
        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        


        // print('<div id="wrapper"> <hr />');
        // show reg number
        $holsteinQueryString = HolsteinCanadaHelper::createHolsteinCanadaQuery($this->bovineFullRegNumber); // create
        // string
        // to
        // query
        // holstein
        // Canada
        // from
        // reg
        // number
        // show cow name
        
       $shortName= $this->shortNameNumber($row['potential_name'],$row ['death_date']); 
     
       
       $longName=$this->LongName($row['full_name'],$row['potential_name']);
       $locationName=$this->currentLocation($row['location_name']);
       $birthday= $this->birthday($row['birth_date']);
       $genetics=$this->geneticIndex($row['prodoll'],$row['prodoll_birthyear_quintile_rank'],$row['geno_test']);
       $damName=$this->damName($row['dam_id'],$row['dam_local_number'],$row['dam_full_name']);
       $sireName=$this->sireName($row['sire_full_reg_number'],$row['birth_date'],$row['dam_id'],$row['sire_short_name']);
       
       $deadRibbon=(!empty($row['death_date'])) ? (new BootStrap)->dangerRibbon('DEAD') : '';
       $deadClass= (!empty($row['death_date'])) ? 'headerDead' : '';
       
       
        $holQuery_sire = HolsteinCanadaHelper::createHolsteinCanadaQuery($row['sire_full_reg_number']);
        $holQuery_dam = HolsteinCanadaHelper::createHolsteinCanadaQuery($row['dam_full_reg_number']);
       
      // print(implode($out));
       
       $column1a = <<<HTML
            <span><h1><a href='{$GLOBALS['MiscObj']->hrefToBovinePage($this->bovineID)}'>#$shortName<a/></h1></span>
            <h6>$longName</h6>
            
HTML;   
            
            
       $column1b = <<<HTML
          
            <h5>$locationName</h5>
            <h5>$birthday</h5>
HTML;         
       

       $column2a = <<<HTML
                
            {$this->rfid()} 
              
                 
HTML;
            
        $column2b = <<<HTML
     
            <h5>{$this->cdnIcon()}</h5>
            <h5>{$this->holsteinCanadaIcon()}</h5>
HTML;    
        
        $column3a = <<<HTML
            <h5>{$this->price()}</h5>
           
HTML;    
         $column3b = <<<HTML
     
            <h5> $genetics</h5>                
            <h5>{$this->classification()}</h5>
HTML;        
            
            
       
        $column4a = <<<HTML
            <h5>$damName</h5>
            <h5>$sireName</h5>             
HTML;    
        
          $finalOut = <<<HTML
        <div class="card card-widget widget-user shadow $deadClass">                      
             <div class="d-flex justify-content-start">     
                <div class='headerBucket'>  
                  {$this->summaryPicture()}
                 </div>  
                <div class='headerBucket'>
                   $column1a
                </div>
                 <div class='headerBucket'>
                   $column1b
                </div>          
                <div class='headerBucket'>
                    $column2b
                </div>
                <div class='headerBucket'>
                    $column2a
                </div>          
                <div class='headerBucket'>
                    $column3a
                </div>
                <div class='headerBucket'>
                    $column3b
                </div>          
                <div class='headerBucket'>
                    $column4a
                </div>          
            </div>                         
                $deadRibbon             
        </div>      
HTML;
        
        
      
return $finalOut;


    }

//end main

   // show latest bovine pic. 
   private function summaryPicture() {
        return ("<img class='bovinePic' alt='{$this->bovineID}' src='/functions/displayBovinePic.php?bovine_id={$this->bovineID}'  height='120' width='160' />");
    }

    private function summaryPrice() {
        // only show if userid is admin or owner group.
       if ($GLOBALS['auth']->getOwnerAccess() == 1) {

            print ("<div class='summaryBlock'>");
            print ("<h1>Price</h1>");
            // get three latest locations
            $query = "SELECT bovinecurr.id as bovine_id,bovinecurr.full_name, bovinecurr.location_name,bovinecurr.local_number,sale_price.price,sale_price.plus,sale_price.custom_comment,sale_price_comment.comment,sale_price.event_time,sale_price.userid
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.sale_price ON sale_price.id=(SELECT id from bovinemanagement.sale_price WHERE sale_price.bovine_id={$this->bovineID} AND sale_price.event_time=(SELECT max(event_time) FROM bovinemanagement.sale_price WHERE bovine_id={$this->bovineID}))
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id=sale_price_comment.id";
            $res = $GLOBALS['pdo']->query($query);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $row['plus'] = $row['plus'] ?: 'f';
            if ($row['plus'] === 't') {
                $plus = '+';
            } else {
                $plus = '';
            }

            print ("<h2>$ {$row['price']}$plus</h2>");
            print ("Comment: {$row['comment']} {$row['custom_comment']}<br/>");

            print ("</div> <!-- End of summaryBlock Price -->" . "\n\r");
        }
    }

    private function shortNameNumber($potentialName,$deathDate) 
    {
        //every animal has a local number.
        $localNumber=$this->bovineLocalNumber;
        
        //unamed animals usually have potential name
        $shortName = empty($this->bovineShortName) ?  $potentialName . '<span id="potentialName">(potential)</span>' : $this->bovineShortName; 
        
        //unregisterd means no name
        $shortName = (empty($this->bovineFullRegNumber)) ? '' : $shortName;
        
        $final =$localNumber. ' '.$shortName;
        
         //when dead show strikethrough. OPTIONAL
         //$shortNameOut = (!empty($deathDate)) ? '<del>'.$final.'</del>' : $final;
        
        return $final;
    }
    
    private function LongName($fullName,$potentialName) {
        
        return empty($fullName) ? $potentialName : $fullName;
        
    }
    
    
    private function damName($dam_id,$dam_local_number,$dam_full_name) {
        if ($dam_local_number != null) {
            $damShortName = $GLOBALS['MiscObj']->femaleShortName($dam_full_name);
            $damLink = $GLOBALS['MiscObj']->hrefToBovinePageFullName($dam_id, $dam_local_number, $dam_full_name);
        }
          
       return  '<i class="fas fa-venus"></i> '.$damShortName . $damLink;
    }
    
    private function sireName($sire_full_reg_number,$bovine_birth_date,$dam_id,$sire_short_name) {
        
        //If not registered yet, display potential sire instead.
        if ($sire_full_reg_number == '11111111111111111') {
  
            try {
                $probArr = HolsteinCanadaERA::listMostLikelyConceptionEvents($bovine_birth_date,$dam_id);
            } catch (HolsteinCanadaERA_NoConceptionEventException $exception) {
                return "Error: No conception Event!";
            }

            $str = "";

            foreach ($probArr as $value) {


                $holQuery_sire = HolsteinCanadaHelper::createHolsteinCanadaQuery($value['service_sire_full_reg_number']);
                 $column4[]= ("<small><small> <b>(potential)</b> {$value['days_ago']} <a href='" . $GLOBALS['MiscObj']->createCDNLink($value['service_sire_full_reg_number']) . "'>{$value['service_sire_short_name']}</a>  &nbsp; &nbsp;<a href=\"$holQuery_sire\"><b>{$value['service_sire_full_reg_number']}</b></a> </small></small> " . '');
            }
        } else {
           
              $holQuery_sire = HolsteinCanadaHelper::createHolsteinCanadaQuery($sire_full_reg_number);
             $column4[]= (" <a href='" . $GLOBALS['MiscObj']->createCDNLink($sire_full_reg_number) . "'>{$sire_short_name}</a>  &nbsp; &nbsp;<a href=\"$holQuery_sire\"><b>{$sire_full_reg_number}</b></a>" . '');
        }
        
        
        return  '<i class="fas fa-mars"></i> '. implode($column4);
    }
    
    
    
    /* not used currently */
    private function owner($owner) {
        
        $owner = '';
        if ($owner != 'W&C') {
            $owner = 'owner: ' . $owner;
        }
        
        return $owner;
    }
    
    private function geneticIndex($prodoll,$prodoll_birthyear_quintile_rank,$geno_test){
        
         $prodoll = empty($prodoll) ? 'Pro$ <small>N/A</small>' : 'Pro$ ' . $prodoll;

        //rank of prodoll

        $prodollRank = empty($prodoll_birthyear_quintile_rank) ? '' : '<small>R' . $prodoll_birthyear_quintile_rank . '/5</small>';

       
        $lpi = $prodoll . ' ' . $prodollRank; //overrite lpi with Pro$
        //say a dna test was done, if applicable.
        if (!empty($geno_test)) {
            $lpi = $lpi . '<span id="genoBox"> (' . $geno_test . ')</span>';
        }
        //or pending dna test, waiting for results.
        elseif (!empty($dna_pending)) {
            $lpi = $lpi . '<span id="genoBox"> (&#8253 ' . substr($dna_pending, 0, 3) . ' &#8253)</span>';
        }
        
       return '<i class="fas fa-dna"></i> '. $lpi; 
    }
    
    
    private function price() {

        ;

        // pricing data, only show for owner.
        $price = '';
        $query = "SELECT event_time,price,plus,comment FROM bovinemanagement.sale_price_curr WHERE bovine_id={$this->bovineID} LIMIT 1";
        $res3 = $GLOBALS ['pdo']->query($query);
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);
        $plus = null;
        if ($res3->rowCount() > 0) {

            if ($GLOBALS['auth']->getOwnerAccess() == 1) {

                $plus = ($row3 ['plus'] == 't') ? '+' : '';
                $dollars = empty($row3['price'] . $plus) ? '' : '$' . $row3['price'] . $plus;
                $price = $dollars . ' ' . $row3 ['comment'] . ' @ ' . date('M Y', strtotime($row3['event_time']) . '');
            }
        } else {
            
            $price = 'N/A.';
        }

        return '<i class="fas fa-file-invoice-dollar"></i> ' . $price;
    }

    private function cdnIcon() {
        
      return ('<a href="' . $GLOBALS['MiscObj']->createCDNLink($this->bovineFullRegNumber) . '"><img style="  margin-top:3px; vertical-align: middle;"  width="50" height="19" src="/images/cdnLogo.svg" /></a>');
    }
      private function holsteinCanadaIcon() {
           $holsteinQueryString=HolsteinCanadaHelper::createHolsteinCanadaQuery($this->bovineFullRegNumber);
          return ("<a href=\"$holsteinQueryString\">" . '<img style=" margin-top:3px; vertical-align: middle;" width="28" height="20" src="/images/holsCanadaBWSmall.svg" />' . "</a>");
      }
    
    private function currentLocation($locationName) {
        
       return  '<i class="fas fa-home"></i> '.$locationName;
        
    }  
      
    private function birthday($birthDay) {
        
        $birthDay=(date('M d, Y', strtotime($birthDay)));
        return '<i class="fas fa-birthday-cake"></i> ' . $birthDay;
    }
    
    
    private function rfid() {
        
              
    $out=<<<HTML
         
         
                    <h5>{$this->bovineRfidNumber}</h5>
                    <h5>{$this->bovineFullRegNumber}</h5>
            
    HTML;
        
       return $out; 
        
    }
   
    
    private function classification() {
        
        // load classification from batch DB.
        $query = "SELECT class,score,class_all FROM batch.aggregate_view_curr WHERE full_reg_number = '{$this->bovineFullRegNumber}' LIMIT 1";
        $res2 = $GLOBALS ['pdo']->query($query);
        $row2 = $res2->fetch(PDO::FETCH_ASSOC);


        //when cow has multiple excellents, show it.
        $class_all =!(empty($row2['class_all']) != '') ?  '-' . $row2['class_all'] : '';
        $classification = !empty($row2['score']) ? "{$row2['class']}-{$row2['score']}$class_all" :  $classification = '<b><small>Not Classified</small></b>';
        
         $tokClass = explode('-', $classification);
         return '<i class="far fa-eye"></i> '.( (empty($tokClass[0]) ? '' : $tokClass[0]) . '-' . (empty($tokClass[1]) ? '' : $tokClass[1]) . ' ' . (empty($tokClass[3]) ? '' : $tokClass[3]));

    }
    
    
    
  
}
//end class
?>