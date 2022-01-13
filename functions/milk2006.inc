<?php 

/**
 * Milk 2006 corn silage calculation
 * 
 * 
 */
class CornSilageMilk2006Calculation {
    
    
//takes a DB row and does the milk 2006 calculations.
    public static function calculateMilk2006($dm_perc,$cp_perc,$ndf_perc,$potential_digest_perc,$ash_perc,$fat_perc,$starch_perc,$dm_yield_ton_per_acre=1) {

      
  /*
        //DEBUG INPUTS:
        $dm_perc=31.8;
        $cp_perc=8.1;
        $ndf_perc=47;
        $potential_digest_perc=74.1;
        $ash_perc=3.2;
        $fat_perc=3.3;
        $starch_perc=32.2;
        /////////////////////////////////////
  */
        
        //calculate ndf digestible assuming in vitro diestibility is potential digesitibiliy. 
        $NDFD = (1-((100-$potential_digest_perc)/$ndf_perc))*100;
        
              
        //for the first sheet
        //average_percent_of_ndf
        $average_percent_of_ndf = 59;
        $AF4 = $average_percent_of_ndf;

        //incubation time 24,30,or 48
        $incubation_time = 48;
        $AF5 = $incubation_time;
        ///////////////////////////////////
        //kernel process
        $C15 = 'yes';

        //optional
         $D15 =0;
         $E15 =0;
         $F15 =0;
        
        //dry matter percentage
        $G15 = $dm_perc;

        //CP % DM
        $H15 = $cp_perc;

        //NDF %DM
        $I15 = $ndf_perc;

        //NDFD          % NDF
        $J15 = $NDFD;

        //adj. NDFD1    % NDF
        $K15 = ((59 / $AF4) * $J15);

        //adj. NDFD2    % NDF
        if ($K15 < 30) {
            $L15 = 30;
        } elseif ($K15 > 80) {
            $L15 = 80;
        } else {
            $L15 = $K15;
        }

        //Starch      % DM
        $M15 = $starch_perc;

        //Ash       % DM
        $N15 = $ash_perc;

        //Fat         % DM
        $O15 = $fat_perc;

        //DM Yield tons/acre
        $P15 = $dm_yield_ton_per_acre;

        //Starch Dig.,               % of Starch, Unprocessed
        if ((((144.81 + (-1.6707 * $G15)) / 100) * 1.08) > 0.98) {
            $Q15 = 0.98;
        } elseif ((((144.81 + (-1.6707 * $G15)) / 100) * 1.08) < 0.76) {
            $Q15 = 0.76;
        } else {
            $Q15 = (((144.81 + (-1.6707 * $G15)) / 100) * 1.08);
        }

        $Q15=$Q15*100;

        //Starch Dig.,              % of Starch, Processed
        if ((((121.589 + (-0.875 * $G15)) / 100) * 1.08) > 0.98) {
            $R15 = 0.98;
        } elseif ((((121.589 + (-0.875 * $G15)) / 100) * 1.08) < 0.86) {
            $R15 = 0.86;
        } else {
            $R15 = (((121.589 + (-0.875 * $G15)) / 100) * 1.08);
        }
          $R15=$R15*100;

        //Starch Dig.    % of Starch KPS            
        if ($D15 > 69) {
            $S15 = $R15;
        }
        elseif ($D15 < 50) {
            $S15 = $Q15;
        } else {
            $S15 = ($R15 + $Q15) / 2;
        }
           
        //Starch Dig.    % of Starch DSA 
          if($D15>69){$T15=$R15;}
          elseif($D15<50) {$T15=$Q15;}
          else{$T15=($R15+$Q15)/2;}

 //Starch Dig.    % of Starch IS-IV
        if($F15>98) {$U15=0.98;}
        elseif($F15<80) { $U15=0.8;}
        else{ $U15=($F15/100);}


        //NOTE:there is an issue with how percent was used in the spreadsheet, 
        //this seemed to cause some variables to be off by a factor of 100.
        //The start digetibility has bugs in it with reagrds to this,
        //but it is all ignored and $V15=0.98 overrides it all.
        
        //ONLY ONE OF THESE CAN BE YES AT A TIME.
        //Kernel Processing Score (KPS)
        $kernelProcessingScore = 'yes';
        //Degree of Starch Access (DSA)
        $degreeOfStarchAccess= 'no';
        //In Situ-In Vitro Starch Digestion (IS-IV)
        $inSituInVitroStarchDigestion= 'no';
               
        //Starch digetibility as a percentage of starch
        if($kernelProcessingScore=="yes") {$V15=$S15;}
        elseif($degreeOfStarchAccess=="yes") {$V15=$T15;}
        elseif($inSituInVitroStarchDigestion=="yes") {$V15=$U15;}
        elseif($C15=="yes") {$V15=$R15;}
        else {$V15=$Q15;} 
       $V15=$V15*0.01; //corrects for earlier mistake????
       
        //NFC             % DMB
        $W15 = (100 - ($H15 + $I15 + $N15 + $O15 - 1.3));

        //NonStarch NFC                % DMB
        //=IF(W15-M15>15,15,W15-M15)
        if (((float)$W15 - (float)$M15) > 15) {
            $X15 = 15;
        } else {
            $X15 = $W15 - $M15;
        }
        
        //NRC-01 TDN-1x           % DMB
        $Y15 = (((0.93 * (float)$H15) + (0.97 * ((float)$O15 - 1) * 2.25) + ((float)$M15 * (float)$V15) + ((float)$X15 * 0.98) + (((float)$I15 - 1.3) * ((float)$L15 / 100))) - 7);

        //TDN-1x % DM
        //$Z15=IF($Y15>78,78,$Y15); //VB
        if ($Y15 > 78) {
            $Z15 = 78;
        } else {
            $Z15 = $Y15;
        }

        //NEL-3x   Mcal/lb DM
        $AA15 = ((((($Z15 - (($I15 - 1.3) * ($L15 / 100)) + ((((((59 - $L15) * 0.26) * 1.83) + $L15) / 100) * ($I15 - 1.3))) * 0.041) + 0.207) * 0.6741) - 0.5656) / 2.2;
       
        // TotalDMI lbs/d
        $AB15=(($L15-59)*0.26)+((0.0115*1350)/0.3);
        
        //CSDMI lbs/d
        $AC15=(($L15-59)*0.26)+((0.0086*1350)/($I15/100));
        
        //Max. CSDMI lbs/d       
        if($AC15>36) {$AD15=36;}
        else{$AD15=$AC15;}
        
        //calc Schwab Shaver          
        $AE15 = (($AD15 * $AA15) - (0.08 * (pow(613.64,0.75)) * ($AD15 / $AB15))) / 0.31;

        //milk ton/acre
        $AF15 = ($AE15 / $AD15) * 2000;

        //Milk per Acre Index lb/acre
        $AG15=($AF15*$P15);

        /*
        //DEBUG
        print("</br>");
        print("NDFD:$NDFD<br>");
        print("J15: $J15<br>");
        print("K15: $K15<br>");
        print("L15: $L15<br>");
        print("M15: $M15<br>");
        print("N15: $N15<br>");
        print("O15: $O15<br>");
        print("P15: $P15<br>");
        print("Q15: $Q15<br>");
        print("R15: $R15<br>");
        print("S15: $S15<br>");
        print("T15: $T15<br>");
        print("U15: $U15<br>");
        print("V15: $V15<br>");
        print("W15: $W15<br>");
        print("X15: $X15<br>");
        print("Y15: $Y15<br>");
        print("Z15: $Z15<br>");
        print("AA15: $AA15<br>");
        print("AB15: $AB15<br>");
        print("AC15: $AC15<br>");
        print("AD15: $AD15<br>");
        print("AE15: $AE15<br>");
        print("AF15: $AF15<br>");
        print("AG15: $AG15<br>");
       */
        
     $retArr['lb_milk_per_ton_dm']=$AF15;
     $retArr['lb_milk_per_acre']=$AG15;
     $retArr['kg_milk_per_tonne_dm']=($AF15/2000)*(0.453592)*1000;
     $retArr['kg_milk_per_hectare']= $retArr['kg_milk_per_tonne_dm']*$P15*2.47105;
     
     return $retArr;
    }
    
}


?>
