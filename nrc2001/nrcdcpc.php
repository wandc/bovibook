<?php 


//if we access from command line, run the test case. Otherwise, leave it to the user to implement.
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
   require_once('../global.php'); //for db connection
   main(); 
}




function main() {
/////// Test Case 1 ///////////////////

//Feeds
//name / amount
$recipe = array();
$recipe['Legume Forage Hay, immature'] = 5.41;
$recipe['Corn Silage, normal'] = 12.01;
$recipe['Corn Grain, steam-flaked'] = 6.61;
$recipe['Calcium soaps of fatty acids'] = 0.300;
$recipe['Tallow'] = 0.300;
$recipe['Cottonseed, Whole with lint'] = 2.25;
$recipe['Soybean, Meal, solv. 48% CP'] = 2.4;
$recipe['Blood Meal, ring dried'] = 0.15;
$recipe['Calcium Carbonate'] = 0.03;
$recipe['MonoSodium Phosphate (1 H2O)'] = 0.06;
$recipe['Salt'] = 0.15;
$recipe['Vitamin premix 1'] = 0.36;

$Feeds = NRCHelper::createFeedArrayOfFeedObjs($recipe); //load this into NRC model
$input=createDefaultInputObject('Lactating Cow');

//run model
$nrc = new NRCDairlyCattleComputations($input, $Feeds);
$retArray=$nrc->ModelComputations();

print_r($retArray); //output model
print('Model Ended!' . "\n\r");
//////////////////////// END Test Case 1 ///////////////////////////////
}

/**
 * Main Model Class 
 * NOTE: DEC 2012 NRC released an updated version for 64 bit systems and made some changes. 
 * Those changes are implemented below.
 */
class NRCDairlyCattleComputations {

    public $Mineral; //might not be right
    public $Feed; //an array of the feed type class
    public $FeedInput; //added by David, because it seems to overwrite the feed class. this will be unmodified.
    public $Output;

    # Program parameters
    public $NumFeeds;         # keeps track of the total number of feeds loaded by the user
    public $FeedNum;          # keeps track of the current feed selected (e.g. for Edit Feed Comp)
    public $EvalFactor; //might not be right
    
    public function __construct($Input, $feedsArray) {
       
        if (isset($Input)==false) {
            throw new Exception("Error: NRC 2001 Animal Input Obj is not set.</br>");
        }
        if (isset($feedsArray)==false) {
            throw new Exception("Error: NRC 2001 Animal Feeds Array is not set.</br>");
        }
        
        $this->Feed = $feedsArray;
        //clone feed obj to keep intput paramaters
        foreach ($feedsArray as $k => $v) {
            $this->FeedInput[$k] = clone $v;
        }

        $this->Output = new Output(); //to store the output variables.
        $this->Input = $Input;

         $this->NumFeeds=count($this->Feed); //the number of feeds.
        
         //seems to only use 6, so init 6 array elements.
         $this->EvalFactor=array();
         $this->EvalFactor[1]= new EvalType();
         $this->EvalFactor[2]= new EvalType();
         $this->EvalFactor[3]= new EvalType();
         $this->EvalFactor[4]= new EvalType();
         $this->EvalFactor[5]= new EvalType();
         $this->EvalFactor[6]= new EvalType();
         
          
    # The upper grid of the diet evaluation screen is represented
    # as these six Evaluation Factors.  They correspond to NE-Diet,
    # NE-Req, NE-Diff, MP-Diet, MP-Req, and MP-Diff.  It is possible
    # that the first three factors may be given in different energy
    # units, under certain conditions, which is another motivation
    # for this type of variable definition
         
         
         
        /**
         * Hack implementation of Minerals 
         */
        $this->Mineral_list = array(
            "Ca" => 1,
            "P" => 2,
            "Mg" => 3,
            "Cl" => 4,
            "K" => 5,
            "Na" => 6,
            "Su" => 7,
            "Co" => 8,
            "Cu" => 9,
            "Io" => 10,
            "Fe" => 11,
            "Mn" => 12,
            "Se" => 13,
            "Zn" => 14,
            "Mo" => 15,
            "VitA" => 16,
            "VitD" => 17,
            "VitE" => 18
        );


        $this->Mineral = array();
        foreach ($this->Mineral_list as $min_name => $idx) {
            $this->Mineral[$idx] = new MineralType();
        }


    }
    

    
    /**
     * main function
     */
    public function ModelComputations() {
      
        # All of the model#s computations are run by calling this procedure.
        # Different output is computed by separate computational routines, but
        # they are all called here.
        # Important Note :  All calculations in this model are made with metric units
        # Different set of computations are run if the animal
        # type is a Young Calf
        If ($this->Input->AnimalType == "Young Calf") {

            if ($this->Output->TotalDMFed == 0)
                return;

            # Run Calf Computations
            $this->CalfComputations();

            # Mineral Sub-Model Calculations
            $this->MineralComputations();
        }
        else {
            
            # Run Pregnancy Sub-Model Calculations
            $this->PregnancyComputations();

            # Lactation Sub-Model Calculations
            $this->LactationComputations();

            # Target Weights and ADG for Breeding Females
            $this->TargetWeightsComputations();

             //calculate DM here, since NRC code never seems to do it.
             for ($X = 1; $X <= $this->NumFeeds; $X++) {
             $this->Output->TotalDMFed = $this->Output->TotalDMFed + $this->Feed[$X]->DMFed;
            } //end forloop
            

            # All of the remaining sub-models assume that the animal is being fed,
            # so if no feeds are selected or the quantity fed is equal to zero,
            # the computations finish here.

            if (($this->NumFeeds == 0) || ($this->Output->TotalDMFed == 0))
                return;
            else {

                # Energy and Protein Supply Calculations
                $this->EnergyAndProteinSupplyComputations();

                # Amino Acid Supply Calculations
                $this->AminoAcidComputations();

                # Dry Matter Intake Calculations
                $this->DryMatterIntakeComputations();

                # Maintenance Sub-Model Calculations
                $this->MaintenanceComputations();

                # Growth Sub-Model Calculations
                $this->GrowthComputations();

                # Mineral Sub-Model Calculations
                $this->MineralComputations();

                # Diet Evaluation Calculations - Part I
                #
			# The Diet Evaluation Section is done in two parts.  The first
                # part is needed to get the EnergyBalance (aka NE-Differ-Gain,
                # aka EvalFactor(3).Gain), which is needed for the Reserves
                # Calculations, which computes the DaysToChange variable, which
                # is displayed in the Diet Evaluation Screen.
                $this->DietEvalOneComputations();

                # Reserves Calculations
                $this->ReservesComputations();

                # Diet Evaluation Calculations - Part II
                $this->DietEvalTwoComputations();
            }
        }

     //model is done, so output info.
     $retArray=array();
     $retArray['Input']=$this->Input;
     $retArray['Output']=$this->Output;
     $retArray['Mineral']=$this->Mineral;
     $retArray['Feed']=$this->Feed;
     $retArray['FeedInput']=$this->FeedInput;
     
     return $retArray;
    }

    private function MineralComputations() {
        $C;
        $d;
        $X;
        $MilkFeeds;
        $CalfStarter;
        $RegFeeds;
        $m;
        $n;
        $o;




        # Calcium

        $this->Mineral[1]->Name = "Ca";
        $this->Mineral[1]->Units = "(g/d)";

        If ($this->Input->DaysInMilk > 0) {
            $this->Mineral[1]->Fecal = 3.1 * ($this->Input->BW / 100);
        } else {
            $this->Mineral[1]->Fecal = 1.54 * ($this->Input->BW / 100);
        }

        $this->Mineral[1]->Urine = 0.08 * ($this->Input->BW / 100);
        $this->Mineral[1]->Misc = 0;
        $this->Mineral[1]->Sweat = 0;

        If ($this->Input->DaysPreg > 190) {
            $this->Mineral[1]->Fetal = 0.02456 * Exp((0.05581 - (0.00007 * $this->Input->DaysPreg )) * $this->Input->DaysPreg)
                    - 0.02456 * Exp((0.05581 - (0.00007 * ($this->Input->DaysPreg - 1))) * ($this->Input->DaysPreg - 1));
        } else {
            $this->Mineral[1]->Fetal = 0;
        }

        switch ($this->Input->DaysInMilk) {
            case 0:
                $this->Mineral[1]->Milk = 0;
                break;
            default:
                switch ($this->Input->Breed) {
                    case "Holstein":
                        $this->Mineral[1]->Milk = 1.22 * $this->Input->MilkProd;
                        break;
                    case "Milking Shorthorn":
                        $this->Mineral[1]->Milk = 1.22 * $this->Input->MilkProd;
                        break;
                    case "Jersey":
                        $this->Mineral[1]->Milk = 1.45 * $this->Input->MilkProd;
                        break;
                    default:
                        $this->Mineral[1]->Milk = 1.37 * $this->Input->MilkProd;
                }
        }

        
        if (($this->Input->BW > 0) && ($this->Output->WG > 0)) {
            $this->Mineral[1]->Growth = (9.83 * (pow($this->Input->MW , 0.22)) * (pow($this->Input->BW , -0.22))) * ($this->Output->WG / 0.96);
        } else {
            $this->Mineral[1]->Growth = 0;
        }



        # Phosphorus

        $this->Mineral[2]->Name = "P";
        $this->Mineral[2]->Units = "(g/d)";

        If (stripos($this->Input->AnimalType,'Cow') == true) {              

            $this->Mineral[2]->Fecal = 1 * $this->Output->TotalDMFed;
        } else {
            $this->Mineral[2]->Fecal = 0.8 * $this->Output->TotalDMFed;
        }

        $this->Mineral[2]->Urine = 0.002 * $this->Input->BW;
        $this->Mineral[2]->Misc = 0;
        $this->Mineral[2]->Sweat = 0;

        if ($this->Input->DaysPreg >= 190) {
            $this->Mineral[2]->Fetal = 0.02743 * Exp(((0.05527 - (0.000075 * $this->Input->DaysPreg )) * $this->Input->DaysPreg))
                    - 0.02743 * Exp(((0.05527 - (0.000075 * ($this->Input->DaysPreg - 1))) * ($this->Input->DaysPreg - 1)));
        } else {
            $this->Mineral[2]->Fetal = 0;
        }

        switch ($this->Input->DaysInMilk) {
            case 0:
                $this->Mineral[2]->Milk = 0;
                break;
            default:
                $this->Mineral[2]->Milk = 0.9 * $this->Input->MilkProd;
        }


        If (($this->Input->BW > 0) && ($this->Output->WG > 0)) {
            $this->Mineral[2]->Growth = (1.2 + (4.635 * (pow($this->Input->MW,0.22)) * (pow($this->Input->BW,-0.22)))) * ($this->Output->WG / 0.96);
        } else {
            $this->Mineral[2]->Growth = 0;
        }




        # Magnesium

        $this->Mineral[3]->Name = "Mg";
        $this->Mineral[3]->Units = "(g/d)";

        $this->Mineral[3]->Fecal = 0.003 * $this->Input->BW;

        $this->Mineral[3]->Urine = 0;
        $this->Mineral[3]->Misc = 0;
        $this->Mineral[3]->Sweat = 0;

        If ($this->Input->DaysPreg > 190) {
            $this->Mineral[3]->Fetal = 0.33;
        } else {
            $this->Mineral[3]->Fetal = 0;
        }

        switch ($this->Input->DaysInMilk) {
            case 0:
                $this->Mineral[3]->Milk = 0;
                break;
            default:
                $this->Mineral[3]->Milk = 0.15 * $this->Input->MilkProd;
        }

        $this->Mineral[3]->Growth = 0.45 * ($this->Output->WG / 0.96);




        # Chlorine
        #With Mineral(4)
        $this->Mineral[4]->Name = "Cl";
        $this->Mineral[4]->Units = "(g/d)";

        $this->Mineral[4]->Fecal = 2.25 * ($this->Input->BW / 100);

        $this->Mineral[4]->Urine = 0;
        $this->Mineral[4]->Misc = 0;
        $this->Mineral[4]->Sweat = 0;

        If ($this->Input->DaysPreg > 190) {
            $this->Mineral[4]->Fetal = 1;
        } else {
            $this->Mineral[4]->Fetal = 0;
        }


        $this->Mineral[4]->Milk = 1.15 * $this->Input->MilkProd;

        $this->Mineral[4]->Growth = 1 * ($this->Output->WG / 0.96);





        # Potassium
        #With Mineral(5)
        $this->Mineral[5]->Name = "K";
        $this->Mineral[5]->Units = "(g/d)";

        If ($this->Input->AnimalType == "Lactating Cow") {                
            $this->Mineral[5]->Fecal = 6.1 * $this->Output->TotalDMFed;
        } else {
            $this->Mineral[5]->Fecal = 2.6 * $this->Output->TotalDMFed;
        }


        $this->Mineral[5]->Urine = 0.038 * $this->Input->BW;                     # urine loss


        if ($this->Input->Temp < 25) {
            $this->Mineral[5]->Sweat = 0;
        } else if ($this->Input->Temp >= 25 && $temp <= 30) {
            $this->Mineral[5]->Sweat = 0.04 * ($this->Input->BW / 100);
        } else {
            $this->Mineral[5]->Sweat = 0.4 * ($this->Input->BW / 100);
        }

        $this->Mineral[5]->Misc = 0;


        If ($this->Input->DaysPreg > 190) {
            $this->Mineral[5]->Fetal = 1.027;
        } else {
            $this->Mineral[5]->Fetal = 0;
        }


        $this->Mineral[5]->Milk = 1.5 * $this->Input->MilkProd;

        $this->Mineral[5]->Growth = 1.6 * ($this->Output->WG / 0.96);





        # Sodium
        #With Mineral(6)
        $this->Mineral[6]->Name = "Na";
        $this->Mineral[6]->Units = "(g/d)";

        If ($this->Input->AnimalType == "Lactating Cow") {           
            $this->Mineral[6]->Fecal = 0.038 * $this->Input->BW;
        } else {
            $this->Mineral[6]->Fecal = 0.015 * $this->Input->BW;
        }


        $this->Mineral[6]->Urine = 0;
        $this->Mineral[6]->Misc = 0;

        if ($this->Input->Temp < 25) {
            $this->Mineral[6]->Sweat = 0;
        } elseif (($this->Input->Temp >= 25) && ($this->Input->Temp <= 30)) {
            $this->Mineral[6]->Sweat = 0.1 * ($this->Input->BW / 100);
        } else {
            $this->Mineral[6]->Sweat = 0.5 * ($this->Input->BW / 100);
        }


        If ($this->Input->DaysPreg > 190) {
            $this->Mineral[6]->Fetal = 1.39;
        } else {
            $this->Mineral[6]->Fetal = 0;
        }

        $this->Mineral[6]->Milk = 0.63 * $this->Input->MilkProd;

        $this->Mineral[6]->Growth = 1.4 * ($this->Output->WG / 0.96);





        # Sulfur
        #With Mineral(7)
        $this->Mineral[7]->Name = "S";
        $this->Mineral[7]->Units = "(g/d)";

        # Non-factorial approach used
        $this->Mineral[7]->Fecal = 0;
        $this->Mineral[7]->Misc = 0;
        $this->Mineral[7]->Urine = 0;
        $this->Mineral[7]->Sweat = 0;
        $this->Mineral[7]->Fetal = 0;
        $this->Mineral[7]->Milk = 0;
        $this->Mineral[7]->Growth = 0;




        # Cobalt
        #With Mineral(8)

        $this->Mineral[8]->Name = "Co";
        $this->Mineral[8]->Units = "(mg/d)";

        # Factorial approach not used here
        $this->Mineral[8]->Fecal = 0;
        $this->Mineral[8]->Misc = 0;
        $this->Mineral[8]->Urine = 0;
        $this->Mineral[8]->Sweat = 0;
        $this->Mineral[8]->Fetal = 0;
        $this->Mineral[8]->Milk = 0;
        $this->Mineral[8]->Growth = 0;




        # Copper
        #With Mineral(9)
        $this->Mineral[9]->Name = "Cu";
        $this->Mineral[9]->Units = "(mg/d)";

        $this->Mineral[9]->Fecal = 0.0071 * $this->Input->BW;
        $this->Mineral[9]->Urine = 0;
        $this->Mineral[9]->Sweat = 0;
        $this->Mineral[9]->Misc = 0;

        if ($this->Input->DaysPreg == 0) {
            $this->Mineral[9]->Fetal = 0;
        } else if ($this->Input->DaysPreg < 100) {
            $this->Mineral[9]->Fetal = 0.5;
        } else if ($this->Input->DaysPreg >= 100 && $this->Input->DaysPreg <= 225) {
            $this->Mineral[9]->Fetal = 1.5;
        } else {
            $this->Mineral[9]->Fetal = 2;
        }



        if ($this->Input->DaysInMilk == 0) {
            $this->Mineral[9]->Milk = 0;
        } else {
            $this->Mineral[9]->Milk = 0.15 * $this->Input->MilkProd;
        }


        $this->Mineral[9]->Growth = 1.15 * ($this->Output->WG / 0.96);


        # Iodine
        # With Mineral(10)
        $this->Mineral[10]->Name = "I";
        $this->Mineral[10]->Units = "(mg/d)";

        $this->Mineral[10]->Fecal = 0;
        $this->Mineral[10]->Urine = 0;
        $this->Mineral[10]->Sweat = 0;
        $this->Mineral[10]->Fetal = 0;

        if ($this->Input->DaysInMilk > 0) {
            $this->Mineral[10]->Milk = 1.5 * ($this->Input->BW / 100);
            $this->Mineral[10]->Misc = 0;
        } else {
            $this->Mineral[10]->Milk = 0;
            $this->Mineral[10]->Misc = 0.6 * ($this->Input->BW / 100);
        }


        $this->Mineral[10]->Growth = 0;



        # Iron
        # With Mineral(11)
        $this->Mineral[11]->Name = "Fe";
        $this->Mineral[11]->Units = "(mg/d)";

        $this->Mineral[11]->Fecal = 0;
        $this->Mineral[11]->Urine = 0;
        $this->Mineral[11]->Sweat = 0;
        $this->Mineral[11]->Misc = 0;


        if ($this->Input->DaysPreg > 190) {
            $this->Mineral[11]->Fetal = 18;
        } else {
            $this->Mineral[11]->Fetal = 0;
        }

        $this->Mineral[11]->Milk = 1 * $this->Input->MilkProd;

        $this->Mineral[11]->Growth = 34 * ($this->Output->WG / 0.96);  # Requirement is on a full, not shrunk, basis
        #End With
        # Manganese
        #With Mineral(12)

        $this->Mineral[12]->Name = "Mn";
        $this->Mineral[12]->Units = "(mg/d)";

        $this->Mineral[12]->Fecal = 0.002 * $this->Input->BW;
        $this->Mineral[12]->Urine = 0;
        $this->Mineral[12]->Sweat = 0;
        $this->Mineral[12]->Misc = 0;


        if ($this->Input->DaysPreg > 190) {

            $this->Mineral[12]->Fetal = 0.3;
        } else {

            $this->Mineral[12]->Fetal = 0;
        }

        if ($this->Input->DaysInMilk == 0) {
            $this->Mineral[12]->Milk = 0;
        } else {

            $this->Mineral[12]->Milk = 0.03 * $this->Input->MilkProd;
        }



        $this->Mineral[12]->Growth = 0.7 * ($this->Output->WG / 0.96);

        # End With


        # Selenium
        #With Mineral(13)
        $this->Mineral[13]->Name = "Se";
        $this->Mineral[13]->Units = "(mg/d)";

        # Factorial approach not used here
        $this->Mineral[13]->Fecal = 0;
        $this->Mineral[13]->Misc = 0;
        $this->Mineral[13]->Urine = 0;
        $this->Mineral[13]->Sweat = 0;
        $this->Mineral[13]->Fetal = 0;
        $this->Mineral[13]->Milk = 0;
        $this->Mineral[13]->Growth = 0;




        # Zinc
        # With Mineral(14)
        $this->Mineral[14]->Name = "Zn";
        $this->Mineral[14]->Units = "(mg/d)";

        $this->Mineral[14]->Fecal = 0.033 * $this->Input->BW;
        $this->Mineral[14]->Urine = 0.012 * $this->Input->BW;
        $this->Mineral[14]->Sweat = 0;
        $this->Mineral[14]->Misc = 0;


        if ($this->Input->DaysPreg > 190) {
            $this->Mineral[14]->Fetal = 12;
        } else {
            $this->Mineral[14]->Fetal = 0;
        }

        $this->Mineral[14]->Milk = 4 * $this->Input->MilkProd;
        $this->Mineral[14]->Growth = 24 * ($this->Output->WG / 0.96);  # Requirement is on a full, not shrunk, basis

        // Molybdenum
        //this code has to be added, rexr book doesn't seem to have this element, just here for completness.
        $this->Mineral[15]->Name = "Mo";
        $this->Mineral[15]->Units = "(mg/d)";


        # Vitamin A
        #With Mineral(16)
        $this->Mineral[16]->Name = "Vit A";
        $this->Mineral[16]->Units = "(1000 IU/kg)";

        # Factorial approach not used here
        $this->Mineral[16]->Fecal = 0;
        $this->Mineral[16]->Urine = 0;
        $this->Mineral[16]->Sweat = 0;
        $this->Mineral[16]->Misc = 0;
        $this->Mineral[16]->Fetal = 0;
        $this->Mineral[16]->Milk = 0;
        $this->Mineral[16]->Growth = 0;

        #End With
        # Vitamin D
        #With Mineral(17)
        $this->Mineral[17]->Name = "Vit D";
        $this->Mineral[17]->Units = "(1000 IU/kg)";

        # Factorial approach not used here
        $this->Mineral[17]->Fecal = 0;
        $this->Mineral[17]->Urine = 0;
        $this->Mineral[17]->Sweat = 0;
        $this->Mineral[17]->Misc = 0;
        $this->Mineral[17]->Fetal = 0;
        $this->Mineral[17]->Milk = 0;
        $this->Mineral[17]->Growth = 0;

        #End With
        # Vitamin E
        #With Mineral(18)
        $this->Mineral[18]->Name = "Vit E";
        $this->Mineral[18]->Units = "(IU/kg)";

        # Factorial approach not used here
        $this->Mineral[18]->Fecal = 0;
        $this->Mineral[18]->Urine = 0;
        $this->Mineral[18]->Sweat = 0;
        $this->Mineral[18]->Misc = 0;
        $this->Mineral[18]->Fetal = 0;
        $this->Mineral[18]->Milk = 0;
        $this->Mineral[18]->Growth = 0;

        #End With
        # Calves don#t have a factorial mineral requirements system
        if ($this->Input->AnimalType == "Young Calf") {
            foreach (range(1, 18) as $C) {

                $this->Mineral[$C]->Fecal = 0;
                $this->Mineral[$C]->Urine = 0;
                $this->Mineral[$C]->Sweat = 0;
                $this->Mineral[$C]->Misc = 0;
                $this->Mineral[$C]->Fetal = 0;
                $this->Mineral[$C]->Milk = 0;
                $this->Mineral[$C]->Growth = 0;
            }
        }



        if ($this->Input->AnimalType == "Young Calf") {
            $MilkFeeds = 0;
            $CalfStarter = 0;
            $RegFeeds = 0;

            for ($X = 1; $X <=  $this->NumFeeds; $X++) {
                if ($this->Feed[$X]->Name == "Calf Starter") {
                    $CalfStarter = $CalfStarter + $this->Feed[$X]->DMFed;
                } else if (strcmp($this->Feed[$X]->Category, "Calf Feed") > 0) {       // ((InStr(Feed(X).Category, "Calf Feed") > 0)) { 
                    $MilkFeeds = $MilkFeeds + $this->Feed[$X]->DMFed;
                } else {
                    $RegFeeds = $RegFeeds + $this->Feed[$X]->DMFed;
                }
            }
            for ($X = 1; $X <=  $this->NumFeeds; $X++) {
                if ($this->Feed[$X]->Name == "Calf Starter") {
                    $CalfStarter = $CalfStarter + $this->Feed[$X]->DMFed;
                } elseif ((strcmp($this->Feed[$X]->Category, "Calf Feed") > 0)) {
                    $MilkFeeds = $MilkFeeds + $this->Feed[$X]->DMFed;
                } else {
                    $RegFeeds = $RegFeeds + $this->Feed[$X]->DMFed;
                }
            }
        }


        //NOTE: this starts at one, because mineral array is hardcoded to key of 1.
        for ($C = 1; $C <=  18; $C++) {

            #With Mineral(C)
            $this->Mineral[$C]->Maint = $this->Mineral[$C]->Fecal + $this->Mineral[$C]->Urine + $this->Mineral[$C]->Sweat + $this->Mineral[$C]->Misc;


            if ($this->Input->AnimalType != "Young Calf") {

                if ((($this->Input->Age < ($this->Input->FirstCalf + $this->Input->CalfInt)) Or ($this->Input->LactNum <= 1))) {
                    # Keep the already computed growth requirement
                } else {
                    # Set the growth requirement to zero
                    $this->Mineral[$C]->Growth = 0;
                }


                if ($C < 16) {   //NOTE: this was poorly coded, it was set to VitA to mean what index (key) VitA is.
                    $this->Mineral[$C]->Total = ($this->Mineral[$C]->Maint + $this->Mineral[$C]->Fetal + $this->Mineral[$C]->Milk + $this->Mineral[$C]->Growth);
                } else {
                    switch ($C) {
                        Case 16:
                            if (($this->Input->AnimalType == "Replacement Heifer")) {
                                if ($this->Input->DaysPreg > 259) {
                                    $this->Mineral[$C]->Total = 0.11 * $this->Input->BW;
                                } else {
                                    $this->Mineral[$C]->Total = 0.08 * $this->Input->BW;
                                }
                            } else {
                                $this->Mineral[$C]->Total = 0.11 * $this->Input->BW;
                            }
                            break;
                        Case 17:
                            $this->Mineral[$C]->Total = 0.03 * $this->Input->BW;
                            break;
                        Case 18:
                            if ($this->Input->Grazing == True) {
                                if ($this->Input->AnimalType == "Dry Cow") {
                                    $this->Mineral[$C]->Total = 0.5 * $this->Input->BW;
                                } else {
                                    $this->Mineral[$C]->Total = 0.26 * $this->Input->BW;
                                }
                            } else {
                                if ($this->Input->AnimalType == "Dry Cow") {
                                    $this->Mineral[$C]->Total = 1.6 * $this->Input->BW;
                                } elseif ($this->Input->AnimalType == "Young Calf") {
                                    $this->Mineral[$C]->Total = 50 * $this->Output->TotalDMFed;
                                } else {
                                    $this->Mineral[$C]->Total = 0.8 * $this->Input->BW;
                                }
                            }
                            break;
                    }
                }


                if ($this->Mineral[$C]->Name == "Co") {
                    $this->Mineral[$C]->Total = 0.11 * $this->Output->TotalDMFed;
                }

                if ($this->Mineral[$C]->Name == "S") {
                    $this->Mineral[$C]->Total = 2 * $this->Output->TotalDMFed;
                }

                if ($this->Mineral[$C]->Name == "Se") {
                    $this->Mineral[$C]->Total = 0.3 * $this->Output->TotalDMFed;
                }
            } else {

                if ($this->Output->StarterDMI == 0) {

                    switch ($this->Mineral[$C]->Name) {
                        Case "Ca":
                            $this->Mineral[$C]->Total = 10 * $this->Output->TotalDMFed;
                            break;
                        Case "P":
                            $this->Mineral[$C]->Total = (7.5 * $this->Output->TotalDMFed) / 0.9;
                            break;
                        Case "Mg":
                            $this->Mineral[$C]->Total = 0.7 * $this->Output->TotalDMFed;
                            break;
                        Case "Na":
                            $this->Mineral[$C]->Total = 1 * $this->Output->TotalDMFed;
                            break;
                        Case "K":
                            $this->Mineral[$C]->Total = 6.5 * $this->Output->TotalDMFed;
                            break;
                        Case "Cl":
                            $this->Mineral[$C]->Total = 2 * $this->Output->TotalDMFed;
                            break;
                        Case "S":
                            $this->Mineral[$C]->Total = 2.9 * $this->Output->TotalDMFed;
                            break;
                        Case "Fe":
                            $this->Mineral[$C]->Total = 100 * $this->Output->TotalDMFed;
                            break;
                        Case "Mn":
                            $this->Mineral[$C]->Total = 40 * $this->Output->TotalDMFed;
                            break;
                        Case "Zn":
                            $this->Mineral[$C]->Total = 40 * $this->Output->TotalDMFed;
                            break;
                        Case "Cu":
                            $this->Mineral[$C]->Total = 10 * $this->Output->TotalDMFed;
                            break;
                        Case "I":
                            $this->Mineral[$C]->Total = 0.25 * $this->Output->TotalDMFed;
                            break;
                        Case "Co":
                            $this->Mineral[$C]->Total = 0.1 * $this->Output->TotalDMFed;
                            break;
                        Case "Se":
                            $this->Mineral[$C]->Total = 0.3 * $this->Output->TotalDMFed;
                            break;
                        Case "Vit A":
                            $this->Mineral[$C]->Total = 0.11 * $this->Input->CalfBW;
                            break;
                        Case "Vit D":
                            $this->Mineral[$C]->Total = 0.6 * $this->Output->TotalDMFed;
                            break;
                        Case "Vit E":
                            $this->Mineral[$C]->Total = 50 * $this->Output->TotalDMFed;
                            break;
                    }
                } else {

                    switch ($this->Mineral[$C]->Name) {
                        Case "Ca":
                            $this->Mineral[$C]->Total = 7 * $this->Output->TotalDMFed;
                            break;
                        Case "P":
                            $this->Mineral[$C]->Total = (4.5 * $this->Output->TotalDMFed) / 0.78;
                            break;
                        Case "Mg":
                            $this->Mineral[$C]->Total = 1 * $this->Output->TotalDMFed;
                            break;
                        Case "Na":
                            $this->Mineral[$C]->Total = 1.5 * $this->Output->TotalDMFed;
                            break;
                        Case "K":
                            $this->Mineral[$C]->Total = 6.5 * $this->Output->TotalDMFed;
                            break;
                        Case "Cl":
                            $this->Mineral[$C]->Total = 2 * $this->Output->TotalDMFed;
                            break;
                        Case "S":
                            $this->Mineral[$C]->Total = 2 * $this->Output->TotalDMFed;
                            break;
                        Case "Fe":
                            $this->Mineral[$C]->Total = 50 * $this->Output->TotalDMFed;
                            break;
                        Case "Mn":
                            $this->Mineral[$C]->Total = 40 * $this->Output->TotalDMFed;
                            break;
                        Case "Zn":
                            $this->Mineral[$C]->Total = 40 * $this->Output->TotalDMFed;
                            break;
                        Case "Cu":
                            $this->Mineral[$C]->Total = 10 * $this->Output->TotalDMFed;
                            break;
                        Case "I":
                            $this->Mineral[$C]->Total = 0.25 * $this->Output->TotalDMFed;
                            break;
                        Case "Co":
                            $this->Mineral[$C]->Total = 0.1 * $this->Output->TotalDMFed;
                            break;
                        Case "Se":
                            $this->Mineral[$C]->Total = 0.3 * $this->Output->TotalDMFed;
                            break;
                        Case "Vit A":
                            $this->Mineral[$C]->Total = 0.11 * $this->Input->CalfBW;
                            break;
                        Case "Vit D":
                            $this->Mineral[$C]->Total = 0.6 * $this->Output->TotalDMFed;
                            break;
                        Case "Vit E":
                            $this->Mineral[$C]->Total = 50 * $this->Output->TotalDMFed;
                            break;
                    }
                }
            }



            $this->Mineral[$C]->Supplied = 0;
            $this->Mineral[$C]->Absorbable = 0;

            if ($this->NumFeeds > 0) {

                for ($d = 1; $d <= $this->NumFeeds; $d++) {

                    switch ($this->Mineral[$C]->Name) {
                        Case "Ca":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->Ca / 100) * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->Ca / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->CaBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Ca / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;
                        Case "Mg":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->Mg / 100) * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->Mg / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->MgBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Mg / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "P":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->P / 100) * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->P / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->PBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->P / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "K":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->K / 100) * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->K / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->KBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->K / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Na":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->Na / 100) * $this->Feed[$d]->DMFed);
                           
                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->Na / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->NaBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Na / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Cl":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->Cl / 100) * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->Cl / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->ClBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Cl / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Zn":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->Zn * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Zn * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->ZnBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->Zn * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Cu":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->Cu * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Cu * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->CuBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->Cu * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Co":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->Co * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Co * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->CoBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->Co * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Mn":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->Mn * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Mn * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->MnBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->Mn * $this->Feed[$d]->DMFed);
                            }

                            break;
                        Case "I":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->I * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->I * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->IBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->I * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Fe":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->Fe * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Fe * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->FeBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->Fe * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "S":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + (($this->Feed[$d]->s / 100) * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ((($this->Feed[$d]->s / 100) * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->SBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->s / 100) * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Se":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->Se * $this->Feed[$d]->DMFed);

                            if ($this->Input->AnimalType != "Young Calf") {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + (($this->Feed[$d]->Se * $this->Feed[$d]->DMFed) * ($this->Feed[$d]->SeBio));
                            } else {
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->Se * $this->Feed[$d]->DMFed);
                            }
                            break;

                        Case "Vit A":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->VitA * $this->Feed[$d]->DMFed);
                            $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->VitA * $this->Feed[$d]->DMFed);
                            break;

                        Case "Vit D":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->VitD * $this->Feed[$d]->DMFed);
                            $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->VitD * $this->Feed[$d]->DMFed);
                            break;

                        Case "Vit E":
                            $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied + ($this->Feed[$d]->VitE * $this->Feed[$d]->DMFed);
                            $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable + ($this->Feed[$d]->VitE * $this->Feed[$d]->DMFed);
                            break;
                    }
                }

                /* David Start */
                #Ration Density Calculation
                if ($this->Output->TotalDMFed > 0) {
                    switch ($this->Mineral[$C]->Name) {
                        Case "Co":
                        Case "Cu":
                        Case "I":
                        Case "Fe":
                        Case "Mn":
                        Case "Se":
                        Case "Zn":
                            $this->Mineral[$C]->RD = $this->Mineral[$C]->Supplied / $this->Output->TotalDMFed;
                            break;
                        Case "Vit A":
                        Case "Vit D":
                        Case "Vit E":
                            $this->Mineral[$C]->RD = $this->Mineral[$C]->Supplied / $this->Output->TotalDMFed;
                            break;
                        default:
                            $this->Mineral[$C]->RD = $this->Mineral[$C]->Supplied / $this->Output->TotalDMFed;
                    } //end switch
                } else {
                    $this->Mineral[$C]->RD = 0;
                }



                # Balance Calculation - First the .Supplied variable must be converted to
                # the same units as the .Total

                switch ($this->Mineral[$C]->Units) {
                    Case "(g/d)":
                        $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied * 1000;
                        $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable * 1000;
                        break;
                    Case "(mg/d)":
                        switch ($this->Mineral[$C]->Name) {
                            Case "Co":
                            Case "Cu":
                            Case "I":
                            Case "Fe":
                            Case "Mn":
                            Case "Se":
                            Case "Zn":
                                # These minerals are listed as mg/kg, no need to convert them to mg units
                                break;
                            default:
                                $this->Mineral[$C]->Supplied = $this->Mineral[$C]->Supplied * 1000000;
                                $this->Mineral[$C]->Absorbable = $this->Mineral[$C]->Absorbable * 1000000;
                        } //end switch inner
                } //end switch outer

                $this->Mineral[$C]->Balance = $this->Mineral[$C]->Absorbable - $this->Mineral[$C]->Total;



                # Used to calculate Ration Density requirements for young calves
                if ($this->Input->AnimalType == "Young Calf") {
                    switch ($this->Mineral[$C]->Name) {
                        Case "Ca":
                            $m = 1;
                            $n = 0.7;
                            $o = 0.6;
                            break;
                        Case "P":
                            $m = 0.7;
                            $n = 0.45;
                            $o = 0.4;
                            break;
                        Case "Mg":
                            $m = 0.07;
                            $n = 0.1;
                            $o = 0.1;
                            break;
                        Case "Na":
                            $m = 0.4;
                            $n = 0.15;
                            $o = 0.14;
                            break;
                        Case "K":
                            $m = 0.65;
                            $n = 0.65;
                            $o = 0.65;
                            break;
                        Case "Cl":
                            $m = 0.25;
                            $n = 0.2;
                            $o = 0.2;
                            break;
                        Case "S":
                            $m = 0.29;
                            $n = 0.2;
                            $o = 0.2;
                            break;
                        Case "Fe":
                            $m = 100;
                            $n = 50;
                            $o = 50;
                            break;
                        Case "Mn":
                        Case "Zn":
                            $m = 40;
                            $n = 40;
                            $o = 40;
                            break;
                        Case "Cu":
                            $m = 10;
                            $n = 10;
                            $o = 10;
                            break;
                        Case "I":
                            $m = 0.5;
                            $n = 0.25;
                            $o = 0.25;
                            break;
                        Case "Co":
                            $m = 0.11;
                            $n = 0.1;
                            $o = 0.1;
                            break;
                        Case "Se":
                            $m = 0.3;
                            $n = 0.3;
                            $o = 0.3;
                            break;
                        Case "Vit A":
                            $m = 9;
                            $n = 4;
                            $o = 4;
                            break;
                        Case "Vit D":
                            $m = 0.6;
                            $n = 0.6;
                            $o = 0.6;
                            break;
                        Case "Vit E":
                            $m = 50;
                            $n = 25;
                            $o = 25;
                            break;
                    } //end switch

                    if ($this->Output->TotalDMFed > 0) {
                        $this->Mineral[$C]->RDReq = (($MilkFeeds * $m) + ($CalfStarter * $n) + ($RegFeeds * $o)) / $this->Output->TotalDMFed;
                    } else {
                        $this->Mineral[$C]->RDReq = 0;
                    }
                } //end if
            } //end numfeeds if
        }//end forloop
    }

//end function

    private function DietEvalOneComputations() {

        $X; //public
        # The EvalFactors are calculated here as follows :
        #
	# NE Diet, NE Required, NE Differ, MP Diet, MP Required, MP Differ
        # Mcal/d     Mcal/d      Mcal/d      g/d         g/d        g/d

        if ($this->Input->AnimalType != "Replacement Heifer") {
            $this->EvalFactor[1]->Name = "NE Supplied";
            $this->EvalFactor[2]->Name = "NE Required";
            $this->EvalFactor[3]->Name = "NE (Diet - Req.)";
            $this->EvalFactor[4]->Name = "MP Diet";
            $this->EvalFactor[5]->Name = "MP Required";
            $this->EvalFactor[6]->Name = "MP (Diet - Req.)";
        } else {
            $this->EvalFactor[1]->Name = "ME Supplied";
            $this->EvalFactor[2]->Name = "ME Required";
            $this->EvalFactor[3]->Name = "ME (Diet - Req.)";
            $this->EvalFactor[4]->Name = "MP Diet";
            $this->EvalFactor[5]->Name = "MP Required";
            $this->EvalFactor[6]->Name = "MP (Diet - Req.)";
        }


        for ($X = 1; $X <=  6; $X++) {
            if ($X < 4) {
                $this->EvalFactor[$X]->Units = "Mcal/day";
            } else {
                $this->EvalFactor[$X]->Units = "g/day";
            }
        } //end forloop


        if ($this->Input->AnimalType != "Replacement Heifer") {
            $this->EvalFactor[1]->Total = $this->Output->NEl_Total;
        } else {
            $this->EvalFactor[1]->Total = $this->Output->MEng_Total;
        }

        $this->EvalFactor[1]->Maint = $this->EvalFactor[1]->Total;

        if ($this->Input->AnimalType != "Replacement Heifer") {
            $this->EvalFactor[2]->Maint = $this->Output->NEMaint;
        } else {
            if ($this->Output->NEmOverMEng > 0) {
                $this->EvalFactor[2]->Maint = $this->Output->NEMaint / $this->Output->NEmOverMEng;
            } else {
                $this->EvalFactor[2]->Maint = 0;
            }
        }

        $this->EvalFactor[3]->Maint = $this->EvalFactor[1]->Maint - $this->EvalFactor[2]->Maint;

        $this->EvalFactor[1]->Preg = $this->EvalFactor[3]->Maint;

        if ($this->Input->AnimalType != "Replacement Heifer") {
            $this->EvalFactor[2]->Preg = $this->Output->NEPreg;
        } else {
            $this->EvalFactor[2]->Preg = $this->Output->MEPreg;
        }

        $this->EvalFactor[3]->Preg = $this->EvalFactor[1]->Preg - $this->EvalFactor[2]->Preg;

        $this->EvalFactor[1]->Lact = $this->EvalFactor[3]->Preg;

        if ($this->Input->AnimalType != "Replacement Heifer") {
            $this->EvalFactor[2]->Lact = $this->Output->NELact;
        } else {
            $this->EvalFactor[2]->Lact = 0;
        }


        $this->EvalFactor[3]->Lact = $this->EvalFactor[1]->Lact - $this->EvalFactor[2]->Lact;


        $this->EvalFactor[1]->Gain = $this->EvalFactor[3]->Lact;


        if ($this->Input->AnimalType == "Replacement Heifer") {
            $this->EvalFactor[2]->Gain = $this->EvalFactor[1]->Gain;
        } else {
            if ($this->Output->EQEBG > 0) {
                $this->EvalFactor[2]->Gain = $this->Output->NEGrowth;
            } else {
                $this->EvalFactor[2]->Gain = 0;
            }
        }

        $this->EvalFactor[3]->Gain = $this->EvalFactor[1]->Gain - $this->EvalFactor[2]->Gain;

        $this->EvalFactor[1]->Reserves = $this->EvalFactor[3]->Gain;

        $this->EvalFactor[2]->Reserves = $this->Output->NEReserves;


        $this->EvalFactor[2]->Total = $this->EvalFactor[2]->Maint + $this->EvalFactor[2]->Preg + $this->EvalFactor[2]->Lact + $this->EvalFactor[2]->Gain + $this->EvalFactor[2]->Reserves;


        $this->EvalFactor[3]->Total = $this->EvalFactor[1]->Total - $this->EvalFactor[2]->Total;


        $this->Output->MPBact = $this->Output->MCP_Total * 0.64;

        $this->Output->MPFeed = $this->Output->TotalDigestedRUP;


        $this->EvalFactor[4]->Total = $this->Output->MPBact + $this->Output->MPFeed;

        $this->EvalFactor[4]->Maint = $this->EvalFactor[4]->Total;

        $this->EvalFactor[5]->Maint = $this->Output->MPMaint;

        $this->EvalFactor[6]->Maint = $this->EvalFactor[4]->Maint - $this->EvalFactor[5]->Maint;

        $this->EvalFactor[4]->Preg = $this->EvalFactor[6]->Maint;

        $this->EvalFactor[5]->Preg = $this->Output->MPPreg;

        $this->EvalFactor[6]->Preg = $this->EvalFactor[4]->Preg - $this->EvalFactor[5]->Preg;

        $this->EvalFactor[4]->Lact = $this->EvalFactor[6]->Preg;

        $this->EvalFactor[5]->Lact = $this->Output->MPLact;

        $this->EvalFactor[6]->Lact = $this->EvalFactor[4]->Lact - $this->EvalFactor[5]->Lact;

        $this->EvalFactor[4]->Gain = $this->EvalFactor[6]->Lact;


        if ($this->Input->Age > $this->Input->FirstCalf) {
            if ($this->Output->EQEBG > 0) {
                $this->EvalFactor[5]->Gain = $this->Output->MPGrowth;
            } else {
                $this->EvalFactor[5]->Gain = 0;
            }
        } else {
            if ($this->Output->SWG > 0) {
                if ($this->Output->EffMP_NPg > 0) {
                    if ($this->Output->WG > 0) {
                        $this->EvalFactor[5]->Gain = (((268 - (29.4 * ($this->Output->RE / $this->Output->WG))) * $this->Output->WG) / $this->Output->EffMP_NPg);
                    } else {
                        $this->EvalFactor[5]->Gain = 0;
                    }
                } else {
                    $this->EvalFactor[5]->Gain = 0;
                }
            } else {
                $this->EvalFactor[5]->Gain = 0;
            }
        }


        $this->EvalFactor[6]->Gain = $this->EvalFactor[4]->Gain - $this->EvalFactor[5]->Gain;

        $this->EvalFactor[4]->Reserves = $this->EvalFactor[6]->Gain;

        $this->EvalFactor[5]->Total = $this->EvalFactor[5]->Maint + $this->EvalFactor[5]->Preg + $this->EvalFactor[5]->Lact + $this->EvalFactor[5]->Gain;


        $this->EvalFactor[6]->Total = $this->EvalFactor[4]->Total - $this->EvalFactor[5]->Total;

        $this->Output->DMIPred = $this->Output->DryMatterIntake;

        $this->Output->DMIActual = $this->Output->TotalDMFed;

        if ($this->Output->TotalDMFed > 0) {
            $this->Output->DietCP = $this->Output->CP_Total / ($this->Output->TotalDMFed * 1000);
        } else {
            $this->Output->DietCP = 0;
        }


        if ($this->Output->CP_Total > 0) {
            $this->Output->CP_RDP = ($this->Output->RDP_Total * 1000) / $this->Output->CP_Total;
            $this->Output->CP_RUP = ($this->Output->CP_Total - ($this->Output->RDP_Total * 1000)) / $this->Output->CP_Total;
        } else {
            $this->Output->CP_RDP = 0;
            $this->Output->CP_RUP = 0;
        }


        $this->Output->RDPReq = 0.15294 * $this->Output->TDN_Act_Total;

        $this->Output->RDPSup = $this->Output->TotalDMFed * 1000 * $this->Output->DietCP * $this->Output->CP_RDP;

        $this->Output->RDPBal = $this->Output->RDPSup - $this->Output->RDPReq;

        $this->Output->RUPSup = $this->Output->CP_Total - $this->Output->RDPSup;


        if ($this->Output->DietRUPDigest > 0) {
            $this->Output->RUPReq = ($this->EvalFactor[5]->Total - ($this->Output->MPBact + $this->Output->MPEndo)) / $this->Output->DietRUPDigest;
        } else {
            $this->Output->RUPReq = 0;
        }

        $this->Output->RUPBal = $this->Output->RUPSup - $this->Output->RUPReq;



        $this->Output->MPBalance = ((($this->Output->MPFeed * 1000) + $this->Output->MPBact + $this->Output->MPEndo) - ($this->Output->MPMaint + $this->Output->MPPreg + $this->Output->MPLact + $this->Output->MPGrowth));
        $this->Output->MPRequired =$this->Output->MPMaint + $this->Output->MPPreg + $this->Output->MPLact + $this->Output->MPGrowth; //added by David, didn't seem to be here.
        $this->Output->MPSupplied =(($this->Output->MPFeed * 1000) + $this->Output->MPBact + $this->Output->MPEndo);

        if ($this->Output->SWG > 0) {
            $this->Output->ProteinInGain = (268 - (29.4 * ($this->Output->NEGrowthDiet / $this->Output->SWG)));
        } else {
            $this->Output->ProteinInGain = 0;
        }



        $MPBalwoGrowth;  //public

        $MPBalwoGrowth = ((($this->Output->MPFeed * 1000) + $this->Output->MPBact + $this->Output->MPEndo) - ($this->Output->MPMaint + $this->Output->MPPreg + $this->Output->MPLact));

        if ($this->Output->ProteinInGain > 0) {
            $this->Output->MPAllowGain = (($MPBalwoGrowth * $this->Output->EffMP_NPg) / $this->Output->ProteinInGain) / 0.96;
        } else {
            $this->Output->MPAllowGain = 0;
        }


        $this->Output->MPAllowGainPreg = $this->Output->MPAllowGain + ($this->Output->ADGPreg / 1000);
    }

//end function

    Private function ReservesComputations() {

        $EnergyBal; //public                # Equal to EvalFactor(3).Gain
        $X; //public 

        $EnergyBal = $this->EvalFactor[3]->Gain;

        $this->Output->CS_F[1] = 0.726;
        $this->Output->CS_F[2] = 0.794;
        $this->Output->CS_F[3] = 0.863;
        $this->Output->CS_F[4] = 0.931;
        $this->Output->CS_F[5] = 1;
        $this->Output->CS_F[6] = 1.069;
        $this->Output->CS_F[7] = 1.137;
        $this->Output->CS_F[8] = 1.206;
        $this->Output->CS_F[9] = 1.274;


        $this->Output->CS5EBW = ($this->Output->SBW * 0.851) / $this->Output->CS_F[$this->Output->CS9];

        for ($X = 1; $X <=  9; $X++) {
            $this->Output->EBW[$X] = $this->Output->CS_F[$X] * $this->Output->CS5EBW; 
            $this->Output->AF[$X] = 0.037683 * $X;
            $this->Output->TF[$X] = $this->Output->AF[$X] * $this->Output->EBW[$X];
            $this->Output->AP[$X] = 0.200886 - (0.0066762 * $X);
            $this->Output->TP[$X] = $this->Output->AP[$X] * $this->Output->EBW[$X];
            $this->Output->ER[$X] = (9.4 * $this->Output->TF[$X]) + (5.55 * $this->Output->TP[$X]);
        } //end forloop

        if ($this->Output->CS9 >= 3) {
            $this->Output->Lose1CS = $this->Output->ER[$this->Output->CS9] - $this->Output->ER[$this->Output->CS9 - 2];
        } else {
            $this->Output->Lose1CS = 1000000;
        }

        if ($this->Output->CS9 <= 7) {
            $this->Output->Gain1CS = $this->Output->ER[$this->Output->CS9 + 2] - $this->Output->ER[$this->Output->CS9];
        } else {
            $this->Output->Gain1CS = 1000000;
        }

        if ($this->Output->CS9 >= 3) {
            $this->Output->NElSub = 0.82 * $this->Output->Lose1CS;
        } else {
            $this->Output->NElSub = 0.82 * ($this->Output->ER[$this->Output->CS9] - $this->Output->ER[1]);
        }

        if ($this->Output->CS9 <= 7) {
            $this->Output->NElReq = (0.644 / 0.75) * $this->Output->Gain1CS;
        } else {
            $this->Output->NElReq = (0.644 / 0.75) * ($this->Output->ER[9] - $this->Output->ER[$this->Output->CS9]);
        }

        if ($EnergyBal > 0) {
            $this->Output->deltaER = $this->Output->NElReq;
        } else {
            $this->Output->deltaER = $this->Output->NElSub;
        }

        if ($this->Input->AnimalType == "Replacement Heifer") {
            $this->Output->DaysToChange = 0;
        } else {
            $this->Output->DaysToChange = $this->Output->deltaER / $EnergyBal;
        }
    }

//end function

    private function DietEvalTwoComputations() {

        $X; //public 
 
        if ($this->Input->AnimalType == "Replacement Heifer") {

            $this->Output->CondMessage = "No changes in condition score for heifers.";
        } else {
            if ($this->EvalFactor[3]->Total >= 0) {
                if (Abs($this->Output->DaysToChange) <= 305) {
                    $this->Output->CondMessage = "Days to gain one condition score :  " . round(abs($this->Output->DaysToChange),2);
                } else {
                    $this->Output->CondMessage = "Days to gain one condition score :  > 305";
                }
            } else {
                if (abs($this->Output->DaysToChange) <= 305) {
                    $this->Output->CondMessage = "Days to lose one condition score :  " . round(abs($this->Output->DaysToChange),2);
                } else {
                    $this->Output->CondMessage = "Days to lose one condition score :  > 305";
                }
            }
        }


        if ($this->Output->TotalDMFed > 0) {
            $this->Output->DietNDF = $this->Output->NDF_Total / $this->Output->TotalDMFed;
            $this->Output->DietADF = $this->Output->ADF_Total / $this->Output->TotalDMFed;
        } else {
            $this->Output->DietNDF = 0;
            $this->Output->DietADF = 0;
        }


        $this->Output->ForageNDF = 0;

        for ($X = 1; $X <=  $this->NumFeeds; $X++) {
            if ($this->Feed[$X]->EnergyEqClass == "Forage") {
                $this->Output->ForageNDF = $this->Output->ForageNDF + (($this->Feed[$X]->NDF / 100) * $this->Feed[$X]->DMFed);
            }
        } //end forloop


        if ($this->Output->TotalDMFed > 0) {
            $this->Output->DietME = $this->Output->MEng_Total / $this->Output->TotalDMFed;
            $this->Output->DietNEl = $this->Output->NEl_Total / $this->Output->TotalDMFed;
            $this->Output->DietNEg = $this->Output->NEg_Total / $this->Output->TotalDMFed;
        } else {
            $this->Output->DietME = 0;
            $this->Output->DietNEl = 0;
            $this->Output->DietNEg = 0;
        }


        $kgPerMetricTon = 996.9; //constant


        if ($this->Input->AnimalType == "Lactating Cow") {
            $this->Output->DailyMilk = $this->Input->MilkProd;
        } else {
            $this->Output->DailyMilk = 0;
        }


        $NEBalance; //public 
        $MEBalance; //public 
        $EnergyFactor; //public 
        $NE_per_kg; //public 


        if ($this->Input->AnimalType != "Replacement Heifer") {
            $NEBalance = ($this->Output->NEl_Total - ($this->Output->NEMaint + $this->Output->NEPreg + $this->Output->NELact + $this->Output->NEGrowth));
            $MEBalance = 0;
        } else {
            $NEBalance = 0;
            $MEBalance = ($this->Output->MEng_Total - ($this->Output->MEMaint + $this->Output->MEPreg + $this->Output->MEGrowth));
        }



        if ($this->Input->AnimalType == "Lactating Cow") {
            if ($NEBalance < 0) {
                $this->Output->Reserves_WG = $NEBalance / 4.92;
            } else {
                $this->Output->Reserves_WG = $NEBalance / 5.12;
            }
        } elseif ($this->Input->AnimalType == "Dry Cow") {
            if ($NEBalance < 0) {
                $this->Output->Reserves_WG = $NEBalance / 4.92;
            } else {
                $this->Output->Reserves_WG = $NEBalance / 6.4;
            }
        } else {
            $this->Output->Reserves_WG = 0;
        }


        if ($NEBalance > 0) {
            $this->Output->MPReqReserves = ($this->Output->Reserves_WG * $this->Output->ProteinInGain) / 0.492;
            $this->Output->MPProvReserves = 0;

            if ($this->Output->DietRUPDigest > 0) {
                $this->Output->RUPReqReserves = $this->Output->MPReqReserves / $this->Output->DietRUPDigest;
            } else {
                $this->Output->RUPReqReserves = 0;
            }

            $this->Output->RUPProvReserves = 0;
        } else {
            $this->Output->MPReqReserves = 0;
            $this->Output->MPProvReserves = (-1 * $this->Output->Reserves_WG) * $this->Output->ProteinInGain * 0.67;
            $this->Output->RUPReqReserves = 0;

            if ($this->Output->DietRUPDigest > 0) {
                $this->Output->RUPProvReserves = $this->Output->MPProvReserves / $this->Output->DietRUPDigest;
            } else {
                $this->Output->RUPProvReserves = 0;
            }
        }


        $this->Output->TargetADGwoPreg = $this->Output->ADG;
        $this->Output->TargetADGPreg = $this->Output->ADG + ($this->Output->ADGPreg / 1000);


        if ($this->Input->MilkProd > 0) {
            if (($this->Output->NEl_Total - $this->Output->NEMaint - $this->Output->NEPreg ) > 0) {
                if ($this->Output->NELact > 0) {
                    $this->Output->EnergyAllowableMilk = $this->Input->MilkProd * (($this->Output->NEl_Total - $this->Output->NEMaint - $this->Output->NEPreg - $this->Output->NEGrowth) / $this->Output->NELact);
                } else {
                    $this->Output->EnergyAllowableMilk = 0;
                }
            } else {
                $this->Output->EnergyAllowableMilk = 0;
            }

            if (((($this->Output->MPFeed * 1000) + $this->Output->MPBact + $this->Output->MPEndo) - $this->Output->MPMaint - $this->Output->MPPreg) > 0) {
                if ($this->Output->MPLact > 0) {
                    $this->Output->ProteinAllowableMilk = $this->Input->MilkProd * (((($this->Output->MPFeed * 1000) + $this->Output->MPBact + $this->Output->MPEndo) - $this->Output->MPMaint - $this->Output->MPPreg - $this->Output->MPGrowth) / $this->Output->MPLact);
                } else {
                    $this->Output->ProteinAllowableMilk = 0;
                }
            } else {
                $this->Output->ProteinAllowableMilk = 0;
            }
        } else {
            $this->Output->EnergyAllowableMilk = 0;
            $this->Output->ProteinAllowableMilk = 0;
        }



        if ($this->Output->DryMatterIntake > 0) {
            if ((($this->Input->AnimalType != "Replacement Heifer") || ($this->Input->DaysPreg > 259))) {
                $this->Output->Energy_TargetDietConc = ($this->Output->NEMaint + $this->Output->NEPreg + $this->Output->NELact + $this->Output->NEGrowth) / $this->Output->DryMatterIntake;
            } else {
                $this->Output->Energy_TargetDietConc = ($this->Output->MEMaint + $this->Output->MEPreg + $this->Output->MEGrowth) / $this->Output->DryMatterIntake;
            }

            $this->Output->MP_TargetDietConc = ($this->Output->MPMaint + $this->Output->MPPreg + $this->Output->MPLact + $this->Output->MPGrowth) / $this->Output->DryMatterIntake;

            $this->Output->Ca_TargetDietConc = $this->Mineral[1]->Total / $this->Output->DryMatterIntake;

            $this->Output->P_TargetDietConc = $this->Mineral[2]->Total / $this->Output->DryMatterIntake;
        } else {
            $this->Output->Energy_TargetDietConc = 0;
            $this->Output->MP_TargetDietConc = 0;
            $this->Output->Ca_TargetDietConc = 0;
            $this->Output->P_TargetDietConc = 0;
        }


        $this->Output->DCAD = ((($this->Mineral[6]->RD * 100) * 435) + (($this->Mineral[5]->RD * 100) * 256)) - ((($this->Mineral[4]->RD * 100) * 282) + (($this->Mineral[7]->RD * 100) * 624));
    }

//end function

    public function CalfComputations() {

        $X; //public 

        $TotalNEm; //public 
        $TotalNEg; //public 
        $TotalME; //public 
        $TotalCP; //public 
        $TotalDCP; //public 
        $TotalMilkADP; //public 
        $TotalStarterADP; //public 
        $TotalMilkCP; //public 
        $TotalStarterCP; //public 
        $TotalADP; //public 
        $ADP_to_CP; //public 
        $MilkME; //public 
        $StarterME; //public 
        $Fat; //public 
        # Energy computations
        $MilkDMIlocal = 0;
        $StarterDMIlocal = 0;
        $TotalNEm = 0;
        $TotalNEg = 0;
        $TotalME = 0;
        $TotalCP = 0;
        $TotalDCP = 0;

        $TotalMilkADP = 0;
        $TotalStarterADP = 0;
        $TotalMilkCP = 0;
        $TotalStarterCP = 0;
        $DietNEmCalflocal = 0;
        $DietNEgCalflocal = 0;
        $DietMECalflocal = 0;

        $MilkME = 0;
        $StarterME = 0;
        $Fat = 0;


        # if there are no feeds then there can be no ration, so the computations cannot be run
        if ($this->NumFeeds == 0) {
            return -1;
        }


        # if the Calf is not fed anything, then the computations cannot be run
        # NB: Both regular and calf feeds have a .DMFed property, so this is okay as-is
        $this->Output->TotalDMFed = 0;

        for ($X = 1; $X <=  $this->NumFeeds; $X++) {
            $this->Output->TotalDMFed = $this->Output->TotalDMFed + $this->Feed[$X]->DMFed;
        } //end forloop


        if ($this->Output->TotalDMFed == 0) {
            return -1;
        }


        //Call ComputeEnergyValues
        $this->ComputeEnergyValues();



        for ($X = 1; $X <=  $this->NumFeeds; $X++) {

            if (($this->Feed[$X]->Category = "Calf Feed - Starter") || ($this->Feed[$X]->Category == "Calf Feed - Milk") || ($this->Feed[$X]->Category == "Calf Feed - Vitamin/Mineral")) {
                # The feed is a calf feed

                $TotalNEm = $TotalNEm + ($this->Feed[$X]->DMFed * $this->Feed[$X]->cNEm);
                $TotalNEg = $TotalNEg + ($this->Feed[$X]->DMFed * $this->Feed[$X]->cNEg);
                $TotalME = $TotalME + ($this->Feed[$X]->DMFed * $this->Feed[$X]->cMEng);
                $TotalCP = $TotalCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cCP / 100));
                $TotalDCP = $TotalDCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cDCP / 100));
                $Fat = $Fat + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cEE / 100));

                if ($this->Feed[$X]->Category == "Calf Feed - Milk") {
                    $MilkDMIlocal = $MilkDMIlocal + $this->Feed[$X]->DMFed;
                    $MilkME = $MilkME + ($this->Feed[$X]->DMFed * $this->Feed[$X]->cMEng);
                    $TotalMilkADP = $TotalMilkADP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cDCP / 100));
                    $TotalMilkCP = $TotalMilkCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cCP / 100));
                } else {
                    $StarterDMIlocal = $StarterDMIlocal + $this->Feed[$X]->DMFed;
                    $StarterME = $StarterME + ($this->Feed[$X]->DMFed * $this->Feed[$X]->cMEng);
                    $TotalStarterADP = $TotalStarterADP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cDCP / 100));
                    $TotalStarterCP = $TotalStarterCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cCP / 100));
                }
            } else {
                # The feed is a regular feed.
                # All regular feeds in a calf diet are considered starter feeds
                $StarterDMIlocal = $StarterDMIlocal + $this->Feed[$X]->DMFed;

                # Convert regular feed CP values to calf feed CP values
                $this->Feed[$X]->cCP = $this->Feed[$X]->CP;

                # Since regular feeds are automatically starter feeds in a calf
                # diet, the DCP must be 0.75 * CP
                $this->Feed[$X]->cDCP = 0.75 * $this->Feed[$X]->cCP;

                # Once CP and DCP are determined for these regular feeds,
                # Total values are computed the same way
                $TotalCP = $TotalCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cCP / 100));
                $TotalDCP = $TotalDCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cDCP / 100));
                $TotalStarterADP = $TotalStarterADP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cDCP / 100));
                $TotalStarterCP = $TotalStarterCP + ($this->Feed[$X]->DMFed * ($this->Feed[$X]->cCP / 100));
            }
        } //end forloop
        # These values are only used for output, so define them
        # on a percentage basis (eg 83% instead of 0.83)
        if ($this->Output->TotalDMFed > 0) {
            $this->Output->DietCPCalf = ($TotalCP / $this->Output->TotalDMFed) * 100;
            $this->Output->DietDCPCalf = ($TotalDCP / $this->Output->TotalDMFed) * 100;
        } else {
            $this->Output->DietCPCalf = 0;
            $this->Output->DietDCPCalf = 0;
        }


        # ME requirement for maintenance with no stress
        $this->Output->NEmCalf = 0.086 * (pow($this->Input->CalfBW,0.75));



        # These instruction are used to compute a weighted average of
        # efficiencies to convert NEm to ME and NEg to ME
        $NonMineralFeeds; //public 

        $this->Output->CalfKm = 0;
        $this->Output->CalfKg = 0;
        $NonMineralFeeds = 0;
        $this->Output->NEg_Total = 0;
        $this->Output->NEm_Total = 0;



        for ($X = 1; $X <=  $this->NumFeeds; $X++) {
            if (($this->Feed[$X]->Category == "Calf Feed - Starter") || ($this->Feed[$X]->Category == "Calf Feed - Milk") || ($this->Feed[$X]->Category == "Calf Feed - Vitamin/Mineral")) {
                if ($this->Feed[$X]->cMEng != 0) {
                    $this->Output->CalfKm = $this->Output->CalfKm + (0.86 * ($this->Feed[$X]->DMFed * $this->Feed[$X]->cMEng));
                    $this->Output->CalfKg = $this->Output->CalfKg + (0.69 * ($this->Feed[$X]->DMFed * $this->Feed[$X]->cMEng));
                    $NonMineralFeeds = $NonMineralFeeds + ($this->Feed[$X]->DMFed * $this->Feed[$X]->cMEng);
                }
            } else {
                if ($this->Feed[$X]->MEng != 0) {
                    $this->Output->CalfKm = $this->Output->CalfKm + (0.75 * ($this->Feed[$X]->DMFed * $this->Feed[$X]->MEng));
                    $this->Output->CalfKg = $this->Output->CalfKg + (0.57 * ($this->Feed[$X]->DMFed * $this->Feed[$X]->MEng));

                    $this->Feed[$X]->NEm = (0.75 * ($this->Feed[$X]->DMFed * $this->Feed[$X]->MEng));
                    $this->Feed[$X]->NEg = (0.57 * ($this->Feed[$X]->DMFed * $this->Feed[$X]->MEng));

                    $NonMineralFeeds = $NonMineralFeeds + ($this->Feed[$X]->DMFed * $this->Feed[$X]->MEng);

                    $this->Output->NEg_Total = $this->Output->NEg_Total + $this->Feed[$X]->NEg;
                    $this->Output->NEm_Total = $this->Output->NEm_Total + $this->Feed[$X]->NEm;
                }
            }
        } //end forloop

        if ($NonMineralFeeds > 0) {
            $this->Output->CalfKm = $this->Output->CalfKm / $NonMineralFeeds;
            $this->Output->CalfKg = $this->Output->CalfKg / $NonMineralFeeds;
        } else {
            $this->Output->CalfKm = 0;
            $this->Output->CalfKg = 0;
        }





        # Note that energy values for regular feeds are computed based on the total diet.
        # That is, in the ComputeEnergyValues sub-model, NEm, NEg and ME values
        # are computed for the entire ration, not on a feed-by-feed basis.
        # These total energy values are added to those computed for the calf feeds.
        $TotalNEm = $TotalNEm + $this->Output->NEm_Total;
        $TotalNEg = $TotalNEg + $this->Output->NEg_Total;
        $TotalME = $TotalME + $this->Output->MEng_Total;



        # Note that, to get the dietary energy concentrations, the TotalDMFed
        # value is used.  This is correct, because the total energy values
        # used in these equations are Calf + Regular feed totals
        if ($this->Output->TotalDMFed > 0) {
            $DietNEmCalflocal = $TotalNEm / $this->Output->TotalDMFed;
            $DietNEgCalflocal = $TotalNEg / $this->Output->TotalDMFed;
            $DietMECalflocal = $TotalME / $this->Output->TotalDMFed;
        } else {
            $DietNEmCalflocal = 0;
            $DietNEgCalflocal = 0;
            $DietMECalflocal = 0;
        }


        # Factor in lower critical temperature stress
        if ($this->Input->Age > 2) {

            $Is = $this->Input->CalfTemp; //Is is a special variable in VB.
            if ($Is > 5) {
                $this->Output->TempFactor = 0;
            } elseif ($Is > 0) {
                $this->Output->TempFactor = 0.13;
            } elseif ($Is > -5) {
                $this->Output->TempFactor = 0.27;
            } elseif ($Is > -10) {
                $this->Output->TempFactor = 0.4;
            } elseif ($Is > -15) {
                $this->Output->TempFactor = 0.54;
            } elseif ($Is > -20) {
                $this->Output->TempFactor = 0.68;
            } elseif ($Is > -25) {
                $this->Output->TempFactor = 0.81;
            } elseif ($Is > -30) {
                $this->Output->TempFactor = 0.94;
            } else {
                $this->Output->TempFactor = 1.07;
            }
        } else {

            $Is = $this->Input->CalfTemp;
            if ($Is > 15) {
                $this->Output->TempFactor = 0;
            } elseif ($Is > 10) {
                $this->Output->TempFactor = 0.13;
            } elseif ($Is > 5) {
                $this->Output->TempFactor = 0.27;
            } elseif ($Is > 0) {
                $this->Output->TempFactor = 0.4;
            } elseif ($Is > -5) {
                $this->Output->TempFactor = 0.54;
            } elseif ($Is > -10) {
                $this->Output->TempFactor = 0.68;
            } elseif ($Is > -15) {
                $this->Output->TempFactor = 0.86;
            } elseif ($Is > -20) {
                $this->Output->TempFactor = 0.94;
            } elseif ($Is > -25) {
                $this->Output->TempFactor = 1.08;
            } elseif ($Is > -30) {
                $this->Output->TempFactor = 1.21;
            } else {
                $this->Output->TempFactor = 1.34;
            }
        }



        # Apply temperature factor to compute NEm requirement (with stress)
        $this->Output->NEmCalf = ($this->Output->NEmCalf * (1 + $this->Output->TempFactor));



        # Recalculate ME for maintenance, since the NEm has been adjusted for temperature effects
        if ($this->Output->CalfKm != 0) {
            $this->Output->MEMaint = $this->Output->NEmCalf / $this->Output->CalfKm;
        } else {
            $this->Output->MEMaint = 0;
        }


        if ($DietNEmCalflocal != 0) {
            $this->Output->DMIForNEmCalf = $this->Output->NEmCalf / $DietNEmCalflocal;
        } else {
            $this->Output->DMIForNEmCalf = 0;
        }


        $this->Output->DMIForGrowth = ($this->Output->TotalDMFed - $this->Output->DMIForNEmCalf);

        $this->Output->NEFGCalf = $this->Output->DMIForGrowth * $DietNEgCalflocal;


        if ($this->Output->CalfKg != 0) {
            $this->Output->MEFGCalf = $this->Output->NEFGCalf / $this->Output->CalfKg;
        } else {
            $this->Output->MEFGCalf = 0;
        }



        if ($this->Output->NEFGCalf > 0) {

            # The energy allowable ADG equation was derived from the following equation:
            #
	  # NEgReq = 0.69 * (0.84 * CalfBW ^.355) * (LWG ^ 1.2)
            #
	  #
	  # I can re-write the equation in terms of LWG as follows:
            #
	  # (LWG ^ 1.2) = NEgReq / (0.69 * (0.84 * CalfBW ^ .355))
            # (LWG ^ 1.2) = (1.45 * NEgReq) / (0.84 * CalfBW ^ .355)
            #
	  #
	  # Now take the log of both sides of the equation to get:
            # log ((LWG ^ 1.2)) = log((1.45 * NEgReq) / (0.84 * CalfBW ^ .355))
            # log ((LWG ^ 1.2)) = log((1.73 * NEgReq) / (CalfBW ^ .355))
            #
	  #
	  # Since log A^x = x * (log A), the equation can now be re-written as:
            #
	  # 1.2 * (log LWG) = log((1.73 * NEgReq) / (CalfBW ^ .355))
            # log LWG = 0.8333 * (log((1.73 * NEgReq) / (CalfBW ^ .355)))
            #
	  #
	  # Finally, by using the fact that exp(log(A)) = A, we can re-write the original
            # equation as follows:
            #
	  # LWG = exp(0.8333 * (log((1.73 * NEgReq) / (CalfBW ^ .355))))

            $this->Output->EnergyADGCalf = Exp((0.8333 * (Log((1.19 * $this->Output->NEFGCalf) / (0.69 * (pow($this->Input->CalfBW,0.355)))))));
        } else {
            $this->Output->EnergyADGCalf = -9999;
        }


        if ($this->Output->EnergyADGCalf > 0) {
            $this->Output->CalfADG = $this->Output->EnergyADGCalf;
        } else {
            $this->Output->CalfADG = 0;
        }


        # There is 30 g Nitrogen/kg gain ==> (30 g N)(6.25) = 187.5 g Net Protein/kg gain
        $this->Output->ProteinReqCalf = $this->Output->CalfADG * 0.188;



        # Protein Supply
        $TotalADP = (($TotalMilkCP * 0.93) + ($TotalStarterCP * 0.75)) * 1000;


        if (($TotalMilkCP + $TotalStarterCP) > 0) {
            $ADP_to_CP = $TotalADP / (($TotalMilkCP + $TotalStarterCP) * 1000);
        } else {
            $ADP_to_CP = 1;
        }


        # Protein maintenance requirements
        $this->Output->EUN = 0.2 * (pow($this->Input->CalfBW,0.75));
        $this->Output->MFN = ($MilkDMIlocal * 1.9) + ($StarterDMIlocal * 3.3);

        if ($TotalCP > 0) {
            $this->Output->BV = (0.8 * ($TotalMilkCP / $TotalCP)) + (0.7 * ($TotalStarterCP / $TotalCP));
        } else {
            $this->Output->BV = 1;
        }


        if ($this->Output->BV <= 0) {
            $this->Output->BV = 1;
        }

        $this->Output->ADPmaint = 6.25 * (((1 / $this->Output->BV) * ($this->Output->EUN + $this->Output->MFN)) - $this->Output->MFN);


        if ($ADP_to_CP > 0) {
            $this->Output->CPmCalf = $this->Output->ADPmaint / $ADP_to_CP;
        } else {
            $this->Output->CPmCalf = 0;
        }

        $this->Output->ADPgrowth = ($this->Output->ProteinReqCalf * 1000) / $this->Output->BV;

        if ($ADP_to_CP > 0) {
            $this->Output->CPgCalf = $this->Output->ADPgrowth / $ADP_to_CP;
        } else {
            $this->Output->CPgCalf = 0;
        }


        $this->Output->CalfADPBal = $TotalADP - $this->Output->ADPmaint - $this->Output->ADPgrowth;
        $this->Output->CalfCPBal = ($TotalCP * 1000) - $this->Output->CPmCalf - $this->Output->CPgCalf;

        $this->Output->ADPAllowGain = (($TotalADP - $this->Output->ADPmaint) * $this->Output->BV) / 0.188;


        $this->Output->CalfFat = $Fat + (($this->Output->Fat_Total / 100) * $this->Output->TotalRegDMFed);

        if ($this->Output->TotalDMFed > 0) {
            $this->Output->DietFatCalf = $this->Output->CalfFat / $this->Output->TotalDMFed;
        } else {
            $this->Output->DietFatCalf = 0;
        }
    }

//end function

    private function GrowthComputations() {

        if ($this->Input->MW > 0) {
            $this->Output->SRW_to_MSBW = 478 / (0.96 * $this->Input->MW);
        } else {
            $this->Output->SRW_to_MSBW = 478 / (0.96 * ($this->MW_From_Breed($this->Input->Breed)));
        }

        $this->Output->EQSBW = ($this->Output->SBW - $this->Output->CW ) * $this->Output->SRW_to_MSBW;

        $this->Output->SWG = 13.91 * (pow($this->Output->NEGrowthDiet,0.9116)) * (pow($this->Output->EQSBW,-0.6837));


        $this->Output->MEAllowGain = ($this->Output->SWG / 0.96);


        if ($this->Input->Age < $this->Input->FirstCalf) {
            $this->Output->MEAllowGainPreg = $this->Output->MEAllowGain + ($this->Output->ADGPreg / 1000);
        } else {
            $this->Output->MEAllowGainPreg = ($this->Output->EQEBG / 0.956) + ($this->Output->ADGPreg / 1000);
        }


        if ($this->Input->UseTargetADG == false) {
            $this->Output->WG = 0.96 * ($this->Input->DesiredADG / 1000);
            $this->Output->DLWReq = 0;
        } else {
            
            $this->Output->WG = 0.96 * $this->Output->ADG;
            $this->Output->DLWReq = 0;
        }




        $this->Output->EQEBW = 0.891 * $this->Output->EQSBW;

        $this->Output->EQEBG = 0.956 * $this->Output->WG;

        $this->Output->RE = 0.0635 * (pow($this->Output->EQEBW , 0.75)) * (pow($this->Output->EQEBG,1.097));

        if ($this->Output->NEg_Total > 0) {
            if ($this->Input->AnimalType != "Replacement Heifer") {
                $this->Output->NEGrowth = ($this->Output->NEm_Total / $this->Output->NEg_Total) * $this->Output->RE;
            } else {
                $this->Output->NEGrowth = $this->Output->RE;
            }
        } else {
            $this->Output->NEGrowth = 0;
        }

        if ($this->Output->WG == 0) {
            $this->Output->NPg = 0;
        } else {
            $this->Output->NPg = $this->Output->WG * (268 - (29.4 * ($this->Output->RE / $this->Output->WG)));
        }



        if (($this->Output->EQSBW <= 478)) {
            $this->Output->EffMP_NPg = (83.4 - (0.114 * $this->Output->EQSBW)) / 100;
        } else {
            $this->Output->EffMP_NPg = 0.28908;
        }


        $this->Output->MPGrowth = $this->Output->NPg / $this->Output->EffMP_NPg;


        if (($this->Output->DLWReq > 0)) {
            $this->Output->NEReserves = $this->Output->DLWReq * 5.12;
        } else {
            $this->Output->NEReserves = $this->Output->DLWReq * 4.92;
        }


        if ($this->Input->AnimalType == "Replacement Heifer") {
            $this->Output->DMIAvailGrowth = $this->Output->TotalDMFed - $this->Output->DMIMaint - $this->Output->DMIPreg;
        } else {
            $this->Output->DMIAvailGrowth = $this->Output->TotalDMFed - $this->Output->DMIMaint - $this->Output->DMIPreg - $this->Output->DMILact;
        }




        # This value is used only for Replacement Heifers
        if ($this->Output->NEgOverMEng > 0) {
            $this->Output->MEGrowth = $this->Output->NEGrowth / $this->Output->NEgOverMEng;
        } else {
            $this->Output->MEGrowth = 0;
        }
    }

//end function

    private function MaintenanceComputations() {

        if (($this->Input->AnimalType == "Lactating Cow") || ($this->Input->AnimalType == "Dry Cow")) {
            $this->Output->a1 = 0.08;
        } else {
            $this->Output->a1 = 0.086;
        }

        $this->Output->CS9 = (($this->Input->CS - 1) * 2) + 1;

        if ($this->Input->AnimalType == "Replacement Heifer") {
            $this->Output->COMP = 0.8 + (($this->Output->CS9 - 1) * 0.05);
            $this->Output->a2 = 0.0007 * (20 - $this->Input->PrevTemp );
        } else {
            $this->Output->COMP = 1;
            $this->Output->a2 = 0;
        }



        if ($this->Input->Grazing == false) {
            $this->Output->NEmact = 0;
        } else {
            if ($this->Input->AnimalType == "Replacement Heifer") {
                $this->Output->NEmact = ((0.0009 * $this->Input->BW ) + (0.0016 * $this->Input->BW ));

                if ($this->Input->Topography == "Hilly") {
                    $this->Output->NEmact = $this->Output->NEmact + (0.006 * $this->Input->BW );
                }
            } else {

                # These activity equations derived by NRC committee members on
                # a full (not shrunk) body weight basis
                $this->Output->NEmact = (((($this->Input->Distance / 1000) * $this->Input->Trips) * (0.00045 * $this->Input->BW )) + (0.0012 * $this->Input->BW ));

                if ($this->Input->Topography == "Hilly") {
                    $this->Output->NEmact = $this->Output->NEmact + (0.006 * $this->Input->BW );
                }
            }
        }



        if ($this->Input->AnimalType == "Replacement Heifer") {
            $this->Output->NEMaintNS = ((pow(($this->Output->SBW - $this->Output->CW ) , 0.75)) * (($this->Output->a1 * $this->Output->COMP) + $this->Output->a2)) + $this->Output->NEmact;
        } else {
            $this->Output->NEMaintNS = ((pow(($this->Input->BW - $this->Output->CW ) , 0.75)) * ($this->Output->a1 * $this->Output->COMP)) + $this->Output->NEmact;
        }


        if ($this->Output->TotalDMFed > 0) {
            if ($this->Input->AnimalType != "Replacement Heifer") {
                $this->Output->NEDietConc = $this->Output->NEl_Total / $this->Output->TotalDMFed;
            } else {
                $this->Output->NEDietConc = $this->Output->NEm_Total / $this->Output->TotalDMFed;
            }
        } else {
            $this->Output->NEDietConc = 0;
        }


        if ($this->Output->NEDietConc > 0) {
            $this->Output->FeedMaint = $this->Output->NEMaintNS / $this->Output->NEDietConc;
        } else {
            $this->Output->FeedMaint = 0;
        }


        if ($this->Output->TotalDMFed > 0) {
            $this->Output->NEGrowthDietNS = ($this->Output->TotalDMFed - $this->Output->FeedMaint) * ($this->Output->NEg_Total / $this->Output->TotalDMFed);
        } else {
            $this->Output->NEGrowthDietNS = 0;
        }


        if ($this->Input->AnimalType == "Replacement Heifer") {
            $this->Output->NEFP = $this->Output->NEGrowthDietNS;
        } else {
            if ($this->Output->TotalDMFed > 0) {
                $this->Output->NEFP = ($this->Output->TotalDMFed - $this->Output->FeedMaint) * ($this->Output->NEl_Total / $this->Output->TotalDMFed) * 0.65;
            } else {
                $this->Output->NEFP = 0;
            }
        }


        $this->Output->MEI = $this->Output->MEng_Total;

        $this->Output->SA = 0.09 * (pow($this->Output->SBW,0.67));

        if ($this->Output->SA > 0) {
            $this->Output->HP = ($this->Output->MEI - $this->Output->NEFP) / $this->Output->SA;
        } else {
            $this->Output->HP = 0;
        }


        $this->Output->T = $this->Input->Age * 30.4;

        //changed case to if statements because php didn't support t
        //Select Case T, why T?????
        if ($this->Output->T <= 30) {
            $this->Output->TI = 2.5;
        } elseif ($this->Output->T < 183) {
            $this->Output->TI = 6.5;
        } elseif ($this->Output->T < 363) {
            $this->Output->TI = 5.1875 + (0.3125 * $this->Output->CS9);
        } elseif ($this->Output->T >= 363) {
            $this->Output->TI = 5.25 + (0.75 * $this->Output->CS9);
        } else {
            //should never be called, exception?
        }

        switch ($this->Input->CoatCond) {
            Case "Clean/Dry":
                $this->Output->Coat = 1;
                break;
            Case "Some Mud":
                $this->Output->Coat = 0.8;
                break;
            Case "Wet/Matted":
                $this->Output->Coat = 0.5;
                break;
            Case "Covered with Snow/Mud":
                $this->Output->Coat = 0.2;
                break;
        }


        $this->Output->EI = ((7.36 - (0.296 * $this->Input->WindSpeed) + (2.55 * $this->Input->HairDepth)) * $this->Output->Coat) * 0.8;

        if (($this->Output->EI < 0)) {
            $this->Output->EI = 0;
        }

        $this->Output->INS = $this->Output->TI + $this->Output->EI;

        $this->Output->LCT = 39 - ($this->Output->INS * $this->Output->HP * 0.85);

        if (($this->Output->LCT >  $this->Input->Temp)) {
            $this->Output->MEcs = $this->Output->SA * ($this->Output->LCT -  $this->Input->Temp) / $this->Output->INS;
        } else {
            $this->Output->MEcs = 0;
        }

        if ((($this->Output->MEng_Total > 0) && ($this->Output->TotalDMFed > 0))) {
            $this->Output->ColdStr = (($this->Output->NEDietConc / ($this->Output->MEng_Total / $this->Output->TotalDMFed)) * $this->Output->MEcs);
        } else {
            $this->Output->ColdStr = 0;
        }


        if ((($this->Input->HeatStress == "None") || ($this->Input->Temp < 30))) {
            $this->Output->HeatStr = 1;
        } elseif ($this->Input->HeatStress == "Rapid/Shallow") {
            $this->Output->HeatStr = 1.07;
        } elseif ($this->Input->HeatStress == "Open Mouth") {
            $this->Output->HeatStr = 1.18;
        }

        //changed to 2012 NRC version
        $this->Output->NEMaint = (($this->Output->NEMaintNS + $this->Output->ColdStr) * $this->Output->HeatStr);


        if ($this->Output->NEDietConc > 0) {
            $this->Output->DMIMaint = $this->Output->NEMaint / $this->Output->NEDietConc;
        } else {
            $this->Output->DMIMaint = 0;
        }


        if ($this->Input->DaysPreg > 0) {
            if ((($this->Output->MEng_Total > 0) && ($this->Output->TotalDMFed > 0))) {
                $this->Output->DMIPreg = $this->Output->MEPreg / ($this->Output->MEng_Total / $this->Output->TotalDMFed);
            } else {
                $this->Output->DMIPreg = 0;
            }
        } else {
            $this->Output->DMIPreg = 0;
        }


        if ($this->Output->TotalDMFed > 0) {
            $this->Output->NEGrowthDiet = ($this->Output->TotalDMFed - $this->Output->DMIMaint - $this->Output->DMIPreg) * ($this->Output->NEg_Total / $this->Output->TotalDMFed);
        } else {
            $this->Output->NEGrowthDiet = 0;
        }



        # Net energy for growth available in the diet cannot be negative
        if ($this->Output->NEGrowthDiet < 0) {
            $this->Output->NEGrowthDiet = 0;
        }



        $this->Output->MPMaint = (0.3 * (pow(($this->Input->BW - $this->Output->CW ) , 0.6))) + (4.1 * (pow(($this->Input->BW - $this->Output->CW ) , 0.5))) + (($this->Output->TotalDMFed * 1000 * 0.03) - (0.5 * (($this->Output->MPBact / 0.8) - ($this->Output->MPBact)))) + $this->Output->MPEndoReq;


        $this->Output->ScurfMP = 0.3 * (pow(($this->Input->BW - $this->Output->CW ) , 0.6));
        $this->Output->UrineMP = 4.1 * (pow(($this->Input->BW - $this->Output->CW ) , 0.5));
        $this->Output->FecalMP = (($this->Output->TotalDMFed * 1000 * 0.03) - (0.5 * (($this->Output->MPBact / 0.8) - ($this->Output->MPBact))));



        # Used only for Replacement Heifers
        if ($this->Output->NEmOverMEng > 0) {
            $this->Output->MEMaint = $this->Output->NEMaint / $this->Output->NEmOverMEng;
        } else {
            $this->Output->MEMaint = 0;
        }
    }

//end function

    private function DryMatterIntakeComputations() {

        $WOL; //public            # Week of Lactation = 7 * Days In Milk
        $Lag; //public            # week of lactation correction -
        # used in Default and Roseler equations

        switch ($this->Input->CoatCond) {
            Case "Clean/Dry":
                $this->Output->CCFact = 1;
                break;
            Case "Some Mud":
                $this->Output->CCFact = 1;
                break;
            Case "Wet/Matted":
                $this->Output->CCFact = 0.85;
                break;
            Case "Covered with Snow/Mud":
                $this->Output->CCFact = 0.7;
                break;
        }

        $this->Output->CCFact = 1; //NOTE: checked with original code, it does this, so why have the select above?

        //changed to if statements      
        if ( $this->Input->Temp < -15) {
            $this->Output->TempFact = 1.16;
        } elseif ( $this->Input->Temp < -5) {
            $this->Output->TempFact = 1.07;
        } elseif ( $this->Input->Temp < 5) {
            $this->Output->TempFact = 1.05;
        } elseif ( $this->Input->Temp < 15) {
            $this->Output->TempFact = 1.03;
        } elseif ( $this->Input->Temp < 25) {
            $this->Output->TempFact = 1;
        } elseif ( $this->Input->Temp < 35) {
            $this->Output->TempFact = 0.9;
        } elseif ( $this->Input->Temp > 35) {
            if ($this->Input->NightCooling == false) {
                $this->Output->TempFact = 0.65;
            } else {
                $this->Output->TempFact = 0.9;
            }
        } else {
            //should never be called, exception here?
        }


        if ($this->Input->Age > 12) {
            $this->Output->SubFact = 0.0869;
        } else {
            $this->Output->SubFact = 0.1128;
        }



        # These equations really belong in the Maintenance computations section,
        # but are put here since the DivFact variable requires the NEDietConc
        if ($this->Output->TotalDMFed > 0) {
            if ($this->Input->AnimalType != "Replacement Heifer") {
                $this->Output->NEDietConc = $this->Output->NEl_Total / $this->Output->TotalDMFed;
            } else {
                $this->Output->NEDietConc = $this->Output->NEm_Total / $this->Output->TotalDMFed;
            }
        } else {
            $this->Output->NEDietConc = 0;
        }


        if (($this->Output->NEDietConc < 1)) {
            $this->Output->DivFact = 0.95;
        } else {
            $this->Output->DivFact = $this->Output->NEDietConc;
        }


        if ($this->Input->DaysPreg < 259) {
            if ($this->Output->DivFact > 0) {
                $this->Output->DMI_RH = ((pow($this->Input->BW , 0.75)) * (((0.2435 * $this->Output->NEDietConc) - (0.0466 * (pow($this->Output->NEDietConc , 2))) - $this->Output->SubFact) / $this->Output->DivFact)) * $this->Output->TempFact * $this->Output->CCFact;
            } else {
                $this->Output->DMI_RH = 0;
            }
        } else {
            $this->Output->DMI_RH = ((1.71 - (0.69 * Exp(0.35 * ($this->Input->DaysPreg - 280)))) / 100) * $this->Input->BW;
        }


        $DMIRH_Factor; //public 


        if ((($this->Input->DaysPreg > 210) && ($this->Input->DaysPreg < 259))) {
            $DMIRH_Factor = (1 + ((210 - $this->Input->DaysPreg ) * 0.0025));
        } else {
            $DMIRH_Factor = 1;
        }

        $this->Output->DMI_RH = $this->Output->DMI_RH * $DMIRH_Factor;

        $WOL = $this->Input->DaysInMilk / 7; //correct
        $Lag = 1 - (Exp(-1 * 0.192 * ($WOL + 3.67))); //correct
        $this->Output->DMILact = (((pow($this->Input->BW,0.75)) * 0.0968) + (0.372 * $this->Output->FCM) - 0.293) * $Lag; //correct

     
        $this->Output->DMIDry = ((1.97 - (0.75 * Exp(0.16 * ($this->Input->DaysPreg - 280)))) / 100) * $this->Input->BW;



        switch ($this->Input->AnimalType) {
            Case "Lactating Cow":
                $this->Output->DryMatterIntake = $this->Output->DMILact;
                break;
            Case "Dry Cow":
                $this->Output->DryMatterIntake = $this->Output->DMIDry;
                break;
            Case "Replacement Heifer":
                $this->Output->DryMatterIntake = $this->Output->DMI_RH;
                break;
            Case "Young Calf":
                $this->Output->DryMatterIntake = $this->Output->DMI_RH;
                break;
        }


        if ($this->Output->DryMatterIntake <= $this->Output->TotalDMFed) {
            $this->Output->Pasture = 0;
        } else {
            if ($this->Output->DryMatterIntake > 0) {
                $this->Output->Pasture = ($this->Output->DryMatterIntake - $this->Output->TotalDMFed) / $this->Output->DryMatterIntake;
            } else {
                $this->Output->Pasture = 0;
            }
        }
    }

//end function

    private function AminoAcidComputations() {

        $DMFed; //public 
        $NF; //public 
        $X; //public 



        if ($this->NumFeeds > 0) {
            $NF = $this->NumFeeds;
        } else {
            return -1;
        }



        $TArg; //public 
        $THis; //public //NOTE: this is not a reserved word because of the case.
        $TIle; //public 
        $TLeu; //public 
        $TLys; //public 
        $TMet; //public 
        $TPhe; //public 
        $TThr; //public 
        $TTrp; //public 
        $TVal; //public 

        $Dig_TArg; //public 
        $Dig_THis; //public 
        $Dig_TIle; //public 
        $Dig_TLeu; //public 
        $Dig_TLys; //public 
        $Dig_TMet; //public 
        $Dig_TPhe; //public 
        $Dig_TThr; //public 
        $Dig_TTrp; //public 
        $Dig_TVal; //public 


        $TArg = 0;
        $THis = 0; //NOTE: this is not a reserved word because of the case.
        $TIle = 0;
        $TLeu = 0;
        $TLys = 0;
        $TMet = 0;
        $TPhe = 0;
        $TThr = 0;
        $TTrp = 0;
        $TVal = 0;

        $Dig_TArg = 0;
        $Dig_THis = 0;
        $Dig_TIle = 0;
        $Dig_TLeu = 0;
        $Dig_TLys = 0;
        $Dig_TMet = 0;
        $Dig_TPhe = 0;
        $Dig_TThr = 0;
        $Dig_TTrp = 0;
        $Dig_TVal = 0;


        for ($X = 1; $X <=  $NF; $X++) {



            $DMFed = $this->Feed[$X]->DMFed;

            if ((($DMFed > 0) && ($this->Feed[$X]->CP > 0) && ($this->Output->CP[$X] > 0))) {

                $TArg = $TArg + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Arg / 100) * $this->Output->TotalDMFed) * 1000);

                $THis = $THis + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->His / 100) * $this->Output->TotalDMFed) * 1000);

                $TIle = $TIle + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Ile / 100) * $this->Output->TotalDMFed) * 1000);

                $TLeu = $TLeu + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Leu / 100) * $this->Output->TotalDMFed) * 1000);

                $TLys = $TLys + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Lys / 100) * $this->Output->TotalDMFed) * 1000);

                $TMet = $TMet + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Met / 100) * $this->Output->TotalDMFed) * 1000);

                $TPhe = $TPhe + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Phe / 100) * $this->Output->TotalDMFed) * 1000);

                $TThr = $TThr + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Thr / 100) * $this->Output->TotalDMFed) * 1000);

                $TTrp = $TTrp + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Trp / 100) * $this->Output->TotalDMFed) * 1000);

                $TVal = $TVal + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->Val / 100) * $this->Output->TotalDMFed) * 1000);



                $Dig_TArg = $Dig_TArg + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Arg / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_THis = $Dig_THis + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->His / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TIle = $Dig_TIle + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Ile / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TLeu = $Dig_TLeu + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Leu / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TLys = $Dig_TLys + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Lys / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TMet = $Dig_TMet + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Met / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TPhe = $Dig_TPhe + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Phe / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TThr = $Dig_TThr + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Thr / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TTrp = $Dig_TTrp + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Trp / 100) * $this->Output->TotalDMFed) * 1000);

                $Dig_TVal = $Dig_TVal + ((($DMFed / $this->Output->TotalDMFed) * ($this->Feed[$X]->CP / 100) * (($this->Output->RUP[$X] * 1000) / $this->Output->CP[$X]) * ($this->Feed[$X]->RUPDigest / 100) * ($this->Feed[$X]->Val / 100) * $this->Output->TotalDMFed) * 1000);
            }
        }//end for loop


        $EAATotalBeforeMP; //public 
        $x1; //public 
        $x2; //public 
        $TotalArg; //public 
        $TotalHis; //public 
        $TotalIle; //public 
        $TotalLeu; //public 
        $TotalLys; //public 
        $TotalMet; //public 
        $TotalPhe; //public 
        $TotalThr; //public 
        $TotalVal; //public 


        $EAATotalBeforeMP = ($TArg + $THis + $TIle + $TLeu + $TLys + $TMet + $TPhe + $TThr + $TTrp + $TVal);

        if ((($this->Output->RUP_Total * 1000) + $this->Output->EndCP + $this->Output->MCP_Total) > 0) {
            $x2 = (($this->Output->RUP_Total * 1000) / (($this->Output->RUP_Total * 1000) + $this->Output->EndCP + $this->Output->MCP_Total)) * 100;
        } else {
            $x2 = 0;
        }

        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TArg / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalArg = 7.31 + (0.251 * $x1);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($THis / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalHis = 2.07 + (0.393 * $x1) + (0.0122 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TIle / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalIle = 7.59 + (0.391 * $x1) - (0.0123 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TLeu / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalLeu = 8.53 + (0.41 * $x1) + (0.0746 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TLys / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalLys = 13.66 + (0.3276 * $x1) - (0.07497 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TMet / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalMet = 2.9 + (0.391 * $x1) - (0.00742 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TPhe / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalPhe = 7.32 + (0.244 * $x1) + (0.029 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TThr / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalThr = 7.55 + (0.45 * $x1) - (0.0212 * $x2);


        if ($EAATotalBeforeMP > 0) {
            $x1 = (($TVal / $EAATotalBeforeMP) * 100);
        } else {
            $x1 = 0;
        }

        $TotalVal = 8.68 + (0.314 * $x1);



        $this->Output->TotalEAA = 30.9 + (0.863 * $EAATotalBeforeMP) + (0.433 * $this->Output->MCP_Total);




        $TotalRUPArgFlow; //public 
        $TotalRUPHisFlow; //public 
        $TotalRUPIleFlow; //public 
        $TotalRUPLeuFlow; //public 
        $TotalRUPLysFlow; //public 
        $TotalRUPMetFlow; //public 
        $TotalRUPPheFlow; //public 
        $TotalRUPThrFlow; //public 
        $TotalRUPTrpFlow; //public 
        $TotalRUPValFlow; //public 


        $TotalRUPArgFlow = 0.863 * $TArg;
        $TotalRUPHisFlow = 0.863 * $THis;
        $TotalRUPIleFlow = 0.863 * $TIle;
        $TotalRUPLeuFlow = 0.863 * $TLeu;
        $TotalRUPLysFlow = 0.863 * $TLys;
        $TotalRUPMetFlow = 0.863 * $TMet;
        $TotalRUPPheFlow = 0.863 * $TPhe;
        $TotalRUPThrFlow = 0.863 * $TThr;
        $TotalRUPTrpFlow = 0.863 * $TTrp;
        $TotalRUPValFlow = 0.863 * $TVal;




        # Duodenal Flow (g/d)
        $this->Output->Arg_Flow = ($TotalArg / 100) * $this->Output->TotalEAA;

        $this->Output->His_Flow = ($TotalHis / 100) * $this->Output->TotalEAA;

        $this->Output->Ile_Flow = ($TotalIle / 100) * $this->Output->TotalEAA;

        $this->Output->Leu_Flow = ($TotalLeu / 100) * $this->Output->TotalEAA;

        $this->Output->Lys_Flow = ($TotalLys / 100) * $this->Output->TotalEAA;

        $this->Output->Met_Flow = ($TotalMet / 100) * $this->Output->TotalEAA;

        $this->Output->Phe_Flow = ($TotalPhe / 100) * $this->Output->TotalEAA;

        $this->Output->Thr_Flow = ($TotalThr / 100) * $this->Output->TotalEAA;

        $this->Output->Val_Flow = ($TotalVal / 100) * $this->Output->TotalEAA;




        $TotalMCPEndArgFlow; //public 
        $TotalMCPEndHisFlow; //public 
        $TotalMCPEndIleFlow; //public 
        $TotalMCPEndLeuFlow; //public 
        $TotalMCPEndLysFlow; //public 
        $TotalMCPEndMetFlow; //public 
        $TotalMCPEndPheFlow; //public 
        $TotalMCPEndThrFlow; //public 
        $TotalMCPEndValFlow; //public 

        $TotalMCPEndArgFlow = $this->Output->Arg_Flow - $TotalRUPArgFlow;
        $TotalMCPEndHisFlow = $this->Output->His_Flow - $TotalRUPHisFlow;
        $TotalMCPEndIleFlow = $this->Output->Ile_Flow - $TotalRUPIleFlow;
        $TotalMCPEndLeuFlow = $this->Output->Leu_Flow - $TotalRUPLeuFlow;
        $TotalMCPEndLysFlow = $this->Output->Lys_Flow - $TotalRUPLysFlow;
        $TotalMCPEndMetFlow = $this->Output->Met_Flow - $TotalRUPMetFlow;
        $TotalMCPEndPheFlow = $this->Output->Phe_Flow - $TotalRUPPheFlow;
        $TotalMCPEndThrFlow = $this->Output->Thr_Flow - $TotalRUPThrFlow;
        $TotalMCPEndValFlow = $this->Output->Val_Flow - $TotalRUPValFlow;



        $dTotalRUPArg; //public 
        $dTotalRUPHis; //public 
        $dTotalRUPIle; //public 
        $dTotalRUPLeu; //public 
        $dTotalRUPLys; //public 
        $dTotalRUPMet; //public 
        $dTotalRUPPhe; //public 
        $dTotalRUPThr; //public 
        $dTotalRUPVal; //public 

        $dTotalMCPEndArg; //public 
        $dTotalMCPEndHis; //public 
        $dTotalMCPEndIle; //public 
        $dTotalMCPEndLeu; //public 
        $dTotalMCPEndLys; //public 
        $dTotalMCPEndMet; //public 
        $dTotalMCPEndPhe; //public 
        $dTotalMCPEndThr; //public 
        $dTotalMCPEndVal; //public 

        if ($TArg > 0) {
            $dTotalRUPArg = $TotalRUPArgFlow * ($Dig_TArg / $TArg);
        } else {
            $dTotalRUPArg = 0;
        }

        if ($THis > 0) {
            $dTotalRUPHis = $TotalRUPHisFlow * ($Dig_THis / $THis);
        } else {
            $dTotalRUPHis = 0;
        }

        if ($TIle > 0) {
            $dTotalRUPIle = $TotalRUPIleFlow * ($Dig_TIle / $TIle);
        } else {
            $dTotalRUPIle = 0;
        }

        if ($TLeu > 0) {
            $dTotalRUPLeu = $TotalRUPLeuFlow * ($Dig_TLeu / $TLeu);
        } else {
            $dTotalRUPLeu = 0;
        }

        if ($TLys > 0) {
            $dTotalRUPLys = $TotalRUPLysFlow * ($Dig_TLys / $TLys);
        } else {
            $dTotalRUPLys = 0;
        }

        if ($TMet > 0) {
            $dTotalRUPMet = $TotalRUPMetFlow * ($Dig_TMet / $TMet);
        } else {
            $dTotalRUPMet = 0;
        }

        if ($TPhe > 0) {
            $dTotalRUPPhe = $TotalRUPPheFlow * ($Dig_TPhe / $TPhe);
        } else {
            $dTotalRUPPhe = 0;
        }

        if ($TThr > 0) {
            $dTotalRUPThr = $TotalRUPThrFlow * ($Dig_TThr / $TThr);
        } else {
            $dTotalRUPThr = 0;
        }

        if ($TVal > 0) {
            $dTotalRUPVal = $TotalRUPValFlow * ($Dig_TVal / $TVal);
        } else {
            $dTotalRUPVal = 0;
        }


        $dTotalMCPEndArg = 0.8 * $TotalMCPEndArgFlow;
        $dTotalMCPEndHis = 0.8 * $TotalMCPEndHisFlow;
        $dTotalMCPEndIle = 0.8 * $TotalMCPEndIleFlow;
        $dTotalMCPEndLeu = 0.8 * $TotalMCPEndLeuFlow;
        $dTotalMCPEndLys = 0.8 * $TotalMCPEndLysFlow;
        $dTotalMCPEndMet = 0.8 * $TotalMCPEndMetFlow;
        $dTotalMCPEndPhe = 0.8 * $TotalMCPEndPheFlow;
        $dTotalMCPEndThr = 0.8 * $TotalMCPEndThrFlow;
        $dTotalMCPEndVal = 0.8 * $TotalMCPEndValFlow;


        $this->Output->Dig_Arg_Flow = $dTotalRUPArg + $dTotalMCPEndArg;
        $this->Output->Dig_His_Flow = $dTotalRUPHis + $dTotalMCPEndHis;
        $this->Output->Dig_Ile_Flow = $dTotalRUPIle + $dTotalMCPEndIle;
        $this->Output->Dig_Leu_Flow = $dTotalRUPLeu + $dTotalMCPEndLeu;
        $this->Output->Dig_Lys_Flow = $dTotalRUPLys + $dTotalMCPEndLys;
        $this->Output->Dig_Met_Flow = $dTotalRUPMet + $dTotalMCPEndMet;
        $this->Output->Dig_Phe_Flow = $dTotalRUPPhe + $dTotalMCPEndPhe;
        $this->Output->Dig_Thr_Flow = $dTotalRUPThr + $dTotalMCPEndThr;
        $this->Output->Dig_Val_Flow = $dTotalRUPVal + $dTotalMCPEndVal;


        $this->Output->MPBact = 0.64 * $this->Output->MCP_Total;
        $this->Output->MPFeed = $this->Output->TotalDigestedRUP;



        if (($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo) > 0) {
            $this->Output->ArgPctMP = 100 * ($this->Output->Dig_Arg_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->HisPctMP = 100 * ($this->Output->Dig_His_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->IlePctMP = 100 * ($this->Output->Dig_Ile_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->LeuPctMP = 100 * ($this->Output->Dig_Leu_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->LysPctMP = 100 * ($this->Output->Dig_Lys_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->MetPctMP = 100 * ($this->Output->Dig_Met_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->PhePctMP = 100 * ($this->Output->Dig_Phe_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->ThrPctMP = 100 * ($this->Output->Dig_Thr_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
            $this->Output->ValPctMP = 100 * ($this->Output->Dig_Val_Flow / ($this->Output->MPBact + ($this->Output->MPFeed * 1000) + $this->Output->MPEndo));
        } else {
            $this->Output->ArgPctMP = 0;
            $this->Output->HisPctMP = 0;
            $this->Output->IlePctMP = 0;
            $this->Output->LeuPctMP = 0;
            $this->Output->LysPctMP = 0;
            $this->Output->MetPctMP = 0;
            $this->Output->PhePctMP = 0;
            $this->Output->ThrPctMP = 0;
            $this->Output->ValPctMP = 0;
        }
        //DAVID: for some reason amino acid totals are not caluled to that here: 

        $this->Output->Total_Flow= $this->Output->Arg_Flow + $this->Output->His_Flow + $this->Output->Ile_Flow + $this->Output->Leu_Flow + $this->Output->Lys_Flow + $this->Output->Met_Flow + $this->Output->Phe_Flow + $this->Output->Thr_Flow + $this->Output->Val_Flow;      
        $this->Output->Dig_Total_Flow= $this->Output->Dig_Arg_Flow + $this->Output->Dig_His_Flow + $this->Output->Dig_Ile_Flow + $this->Output->Dig_Leu_Flow + $this->Output->Dig_Lys_Flow + $this->Output->Dig_Met_Flow + $this->Output->Dig_Phe_Flow + $this->Output->Dig_Thr_Flow + $this->Output->Dig_Val_Flow;      
        $this->Output->TotalPctMP= $this->Output->ArgPctMP + $this->Output->HisPctMP + $this->Output->IlePctMP + $this->Output->LeuPctMP + $this->Output->LysPctMP + $this->Output->MetPctMP + $this->Output->PhePctMP + $this->Output->ThrPctMP + $this->Output->ValPctMP;
        
    }

//end function




    /* David Edited From Here Down */

    Private function EnergyAndProteinSupplyComputations() {

        $DMFed; //public 
        $NF; //public 
        $X; //public 

        $Kp; //public 
        $BW_DMI; //public 
        $ConcSum; //public 
        $PercentConc; //public 


        if ($this->NumFeeds > 0) {
            $NF = $this->NumFeeds;
        } else {
            return -1;
        }
        //NOTE: probably just allocating the arrays, which php doesn't need.
        //Reallocates storage space for an array variable.
        /* ReDim DMI(1 To NF) As Single
          ReDim TDN(1 To NF) As Single
          ReDim TDN_Act(1 To NF) As Single
          ReDim CP(1 To NF) As Single
          ReDim RUP(1 To NF) As Single
          ReDim RDP(1 To NF) As Single
          ReDim NDF(1 To NF) As Single
          ReDim ADF(1 To NF) As Single
          ReDim PsgRate(1 To NF) As Single */
      

        $this->ComputeEnergyValues();


        if ($this->Input->BW > 0) {
            $BW_DMI = ($this->Output->TotalDMFed / $this->Input->BW ) * 100;
        } else {
            $BW_DMI = 0;
        }


        $ConcSum = 0;

        for ($X = 1; $X <=  $this->NumFeeds; $X++) {
            if ($this->Feed[$X]->ForageDescrp == "Concentrate") {
                $ConcSum = $ConcSum + $this->Feed[$X]->DMFed;
            }
        }


        if ($this->Output->TotalDMFed > 0) {
            $PercentConc = ($ConcSum / $this->Output->TotalDMFed) * 100;
        } else {
            $PercentConc = 0;
        }


        for ($X = 1; $X <=  $this->NumFeeds; $X++) {

            $DMFed = $this->Feed[$X]->DMFed;
            $this->Output->DMI[$X] = $DMFed;

            if ($this->Feed[$X]->Category != "Vitamin/Mineral") {
                $this->Output->TDN[$X] = ($this->Feed[$X]->TDN / 100) * ($DMFed * 1000);
                $this->Output->TDN_Act[$X] = ($this->Feed[$X]->TDN_ActX / 100) * ($DMFed * 1000);
            } else {
                $this->Output->TDN[$X] = 0;
                $this->Output->TDN_Act[$X] = 0;
            }


            $this->Output->CP[$X] = ($this->Feed[$X]->CP / 100) * ($DMFed * 1000);


            # Passage rate equations
            switch ($this->Feed[$X]->ForageDescrp) {
                Case "Concentrate":
                    $Kp = 2.904 + (1.375 * $BW_DMI) - (0.02 * $PercentConc);
                    break;
                Case "Dry":
                    $Kp = 3.362 + (0.479 * $BW_DMI) - (0.017 * $this->Feed[$X]->NDF) + (0.007 * $PercentConc); //fixed NRC 2012 version
                    break;
                Case "Wet":
                    $Kp = 3.054 + (0.614 * $BW_DMI);
                    break;
                default:
                    $Kp = 0;
            }


            # Passage rate cannot be negative
            if ($Kp < 0) {
                $Kp = 0;
            }


            $this->Output->PsgRate[$X] = $Kp;


            if (($this->Feed[$X]->Kd + $Kp) > 0) {
                $this->Output->RDP[$X] = (($this->Feed[$X]->Kd / ($this->Feed[$X]->Kd + $Kp)) * (((($this->Feed[$X]->PrtB / 100) * ($this->Feed[$X]->CP / 100)) * $this->Feed[$X]->DMFed))) + ((($this->Feed[$X]->PrtA / 100) * ($this->Feed[$X]->CP / 100)) * $this->Feed[$X]->DMFed);
            } else {
                $this->Output->RDP[$X] = 0;
            }


            # RDP cannot exceed the total CP
            if (($this->Output->RDP[$X] * 1000) > $this->Output->CP[$X]) {
                $this->Output->RDP[$X] = $this->Output->CP[$X] / 1000;
            }


            $this->Output->RUP[$X] = ($this->Output->CP[$X] - ($this->Output->RDP[$X] * 1000)) / 1000;

            $this->Output->NDF[$X] = ($this->Feed[$X]->NDF / 100) * $DMFed;
            $this->Output->ADF[$X] = ($this->Feed[$X]->ADF / 100) * $DMFed;
        } //end forloop


        $this->Output->DMI_Total = 0;
        $this->Output->TDN_Total = 0;
        $this->Output->TDN_Act_Total = 0;
        $this->Output->CP_Total = 0;
        $this->Output->RUP_Total = 0;
        $this->Output->TotalDigestedRUP = 0;
        $this->Output->RDP_Total = 0;
        $this->Output->NDF_Total = 0;
        $this->Output->ADF_Total = 0;


        for ($X = 1; $X <=  $this->NumFeeds; $X++) {

            $this->Output->DMI_Total = $this->Output->DMI_Total + $this->Output->DMI[$X];
            $this->Output->TDN_Total = $this->Output->TDN_Total + $this->Output->TDN[$X];
            $this->Output->TDN_Act_Total = $this->Output->TDN_Act_Total + $this->Output->TDN_Act[$X];
            $this->Output->CP_Total = $this->Output->CP_Total + $this->Output->CP[$X];
            $this->Output->RUP_Total = $this->Output->RUP_Total + $this->Output->RUP[$X];
            $this->Output->TotalDigestedRUP = $this->Output->TotalDigestedRUP + ($this->Output->RUP[$X] * ($this->Feed[$X]->RUPDigest / 100));
            $this->Output->RDP_Total = $this->Output->RDP_Total + $this->Output->RDP[$X];
            $this->Output->NDF_Total = $this->Output->NDF_Total + $this->Output->NDF[$X];
            $this->Output->ADF_Total = $this->Output->ADF_Total + $this->Output->ADF[$X];
        } //end forloop


        if ($this->Output->RUP_Total > 0) {
            $this->Output->DietRUPDigest = $this->Output->TotalDigestedRUP / $this->Output->RUP_Total;
        } else {
            $this->Output->DietRUPDigest = 0;
        }


        $this->Output->MCP_Total = 0.13 * $this->Output->TDN_Act_Total;



        # This condition assumes that microbial crude protein production cannot
        # exceed 0.85 (efficiency of protein uptake of bacteria) of the ruminal degraded protein
        if ($this->Output->MCP_Total > (0.85 * ($this->Output->RDP_Total * 1000))) {
            $this->Output->MCP_Total = (0.85 * ($this->Output->RDP_Total * 1000));
        }

        if ($this->Output->MCP_Total < 0) {
            $this->Output->MCP_Total = 0;
        }


        $this->Output->DietTDN = ($this->Output->TDN_Total / 1000) / $this->Output->TotalDMFed;


        if ($this->Output->MEng_Total > 0) {
            $this->Output->NEgOverMEng = $this->Output->NEg_Total / $this->Output->MEng_Total;
            $this->Output->NElOverMEng = $this->Output->NEl_Total / $this->Output->MEng_Total;
            $this->Output->NEmOverMEng = $this->Output->NEm_Total / $this->Output->MEng_Total;
        } else {
            $this->Output->NEgOverMEng = 0;
            $this->Output->NElOverMEng = 0;
            $this->Output->NEmOverMEng = 0;
        }


        $this->Output->EndCP = 11.8 * $this->Output->TotalDMFed;
        $this->Output->MPEndo = 0.4 * $this->Output->EndCP;
        $this->Output->MPEndoReq = $this->Output->MPEndo / 0.67;
    }

//end function

    Private function TargetWeightsComputations() {

        $this->Output->SBW = 0.96 * $this->Input->BW;

        $this->Output->Age1st = $this->Input->FirstCalf;
        $this->Output->Age2nd = $this->Output->Age1st + $this->Input->CalfInt;
        $this->Output->Age3rd = $this->Output->Age2nd + $this->Input->CalfInt;
        $this->Output->Age1stBred = $this->Output->Age1st - (280 / 30.4);

        $this->Output->Wt1stBred = $this->Input->MW * 0.55;
        $this->Output->Wt1st = $this->Input->MW * 0.82;
        $this->Output->Wt2nd = $this->Input->MW * 0.92;
        $this->Output->Wt3rd = $this->Input->MW;


        if (($this->Output->Age1stBred - $this->Input->Age ) > 0) {
            $this->Output->ADG1stBred = ($this->Output->Wt1stBred - $this->Input->BW ) / (($this->Output->Age1stBred - $this->Input->Age ) * 30.4);
        }

        if (($this->Output->Age1st - $this->Input->Age ) > 0) {
            $this->Output->ADG1st = ($this->Output->Wt1st - ($this->Input->BW - $this->Output->CW )) / (($this->Output->Age1st - $this->Input->Age ) * 30.4);
        }

        if ($this->Input->CalfInt > 0) {
            $this->Output->ADG2nd = ($this->Output->Wt2nd - $this->Output->Wt1st) / ($this->Input->CalfInt * 30.4);
            $this->Output->ADG3rd = ($this->Output->Wt3rd - $this->Output->Wt2nd) / ($this->Input->CalfInt * 30.4);
        }


        if ($this->Input->AnimalType == "Replacement Heifer") {
            if ($this->Input->Age < $this->Output->Age1stBred) {
                $this->Output->ADGNonBred = ($this->Output->Wt1stBred - $this->Input->BW ) / (($this->Output->Age1stBred - $this->Input->Age ) * 30.4);
            } else {
                $this->Output->ADGNonBred = 0;
            }
        } else {
            $this->Output->ADGNonBred = 0;
        }

        if ($this->Output->ADGNonBred < 0) {
            $this->Output->ADGNonBred = 0;
        }


        if ($this->Input->AnimalType == "Replacement Heifer") {
            if (($this->Input->DaysPreg > 0)) {
                $this->Output->ADG = $this->Output->ADG1st;
            } else {
                $this->Output->ADG = $this->Output->ADG1stBred;
            }
        } else {
            if (($this->Input->Age >= $this->Output->Age3rd)) {
                $this->Output->ADG = 0;
            } elseif (($this->Output->Age2nd <= $this->Input->Age ) && ($this->Input->Age < $this->Output->Age3rd)) {
                $this->Output->ADG = $this->Output->ADG3rd;
            } elseif (($this->Output->Age1st <= $this->Input->Age ) && ($this->Input->Age < $this->Output->Age2nd)) {
                $this->Output->ADG = $this->Output->ADG2nd;
            } elseif (($this->Output->Age1stBred <= $this->Input->Age ) && ($this->Input->Age < $this->Output->Age1st)) {
                $this->Output->ADG = $this->Output->ADG1st;
            }
        }

        if ($this->Output->ADG < 0) {
            $this->Output->ADG = 0;
        }
    }

//end function

    Private function LactationComputations() {

        if ($this->Input->MilkProd == 0) {
            $this->Output->MilkEn = 0;
            $this->Output->NELact = 0;
            $this->Output->YProtn = 0;
            $this->Output->YFatn = 0;
            $this->Output->MPLact = 0;
            $this->Output->FCM = 0;

            return -1;
        }


        if (($this->Input->Lactose == 0)) {
            $this->Output->MilkEn = (0.0929 * $this->Input->MilkFat) + (0.0547 * ($this->Input->MilkTrueProtein / 0.93)) + 0.192;
        } else {
            $this->Output->MilkEn = (0.0929 * $this->Input->MilkFat) + (0.0547 * ($this->Input->MilkTrueProtein / 0.93)) + (0.0395 * $this->Input->Lactose);
        }


        $this->Output->YEn = $this->Output->MilkEn * $this->Input->MilkProd;

        $this->Output->NELact = $this->Output->YEn;

        $this->Output->YProtn = $this->Input->MilkProd * ($this->Input->MilkTrueProtein / 100);

        $this->Output->YFatn = $this->Input->MilkProd * ($this->Input->MilkFat / 100);

        $this->Output->MPLact = ($this->Output->YProtn / 0.67) * 1000;

        $this->Output->FCM = (0.4 * $this->Input->MilkProd) + (15 * ($this->Input->MilkFat / 100) * $this->Input->MilkProd);
    }

//end function

    private function PregnancyComputations() {

        //removed the constant value, not necessary
        $Km = 0.64;       # Changed from 0.576 (Beef NRC value) on June 5, 2000
        $EffMEPreg = 0.14;
        $EffMPPreg = 0.33;


        if ($this->Input->DaysPreg < 190) {
            $this->Output->CW = 0;
            $this->Output->ADGPreg = 0;
            $this->Output->MEPreg = 0;
            $this->Output->MPPreg = 0;
        } else {

            if ($this->Input->DaysPreg > 279) {
                $this->Input->DaysPreg = 279;
            }

            $this->Output->CW = (18 + (($this->Input->DaysPreg - 190) * 0.665)) * ($this->Input->CBW / 45);
            $this->Output->ADGPreg = 665 * ($this->Input->CBW / 45);
            $this->Output->MEPreg = (((2 * 0.00159 * $this->Input->DaysPreg ) - 0.0352) * ($this->Input->CBW / 45)) / $EffMEPreg;
            $this->Output->MPPreg = (((0.69 * $this->Input->DaysPreg ) - 69.2) * ($this->Input->CBW / 45)) / $EffMPPreg;
        }


        $this->Output->NEPreg = $this->Output->MEPreg * $Km;


        if ($this->Input->DaysPreg == 0) {
            $this->Output->MEPreg = 0;
            $this->Output->MPPreg = 0;
            $this->Output->ADGPreg = 0;
            $this->Output->CW = 0;
        }
    }

//end function

    Public function ComputeEnergyValues() {

        $tdNFC; //public                  # truly digestible NFC
        $tdCP; //public                   # truly digestible CP
        $tdFat; //public                  # truly digestible Fat
        $dNDF; //public                   # digestible NDF
        $DiscountVariable; //public       # used to determine discount
        $C; //public 
        $DMFed; //public 
        $NF; //public 
        $X; //public 
        $SBWlocal; //public 
        
        # Reset total energy values  //NOTE: this is ok, checked with orig code.
        $this->Output->MEng_Total = 0;
        $this->Output->NEl_Total = 0;
        $this->Output->NEg_Total = 0;
        $this->Output->NEm_Total = 0;

 


        if ($this->NumFeeds > 0) {
            $NF = $this->NumFeeds;
        } else {
            return -1;
        }



        # Regular (i.e. non-calf) feeds can be used in a calf ration, so it
        # is necessary to differentitate between the TotalDMFed, which accouts
        # for all of the dry matter fed to the animal, and the Total Regular DMFed.
        # It is necessary to do this, since the total energy values computed in this
        # section are done so by first computing total energy concentration values,
        # then multiplying them by the total quantity values.  Thus there must be
        # a total regular feed fed value, since multiplying TotalDMFed * concentration
        # would (if calf feeds were present) over-estimate the computed total energy
        $this->Output->TotalRegDMFed = 0;

        for ($X = 1; $X <=  $NF; $X++) {
            if (strpos($this->Feed[$X]->Category, "Calf Feed") == false) { //changed VB InStr to strpos that returns false instead of 0.
                $this->Output->TotalRegDMFed = $this->Output->TotalRegDMFed + $this->Feed[$X]->DMFed;
            }
        }



        # if there are no regular feeds, exit the procedure
        if (($this->Output->TotalRegDMFed == 0) && ($this->Input->AnimalType == "Young Calf")) {
            return -1;
        }




        $SBWlocal = 0.96 * $this->Input->BW;


        $this->Output->DietaryNFC = 0;







        # Exit procedure if there are no feeds selected
        if ($this->NumFeeds == 0) {
            return -1;
        }


        for ($C = 1; $C <=  $this->NumFeeds; $C++) {
  

            if (strpos($this->Feed[$C]->Category, "Calf Feed") == false) {   //changed VB InStr to strpos that returns false instead of 0. This runs when not a calf feed (ie. a lot of the time)
                $this->Feed[$C]->NFCDigest = 0.98;

                $tdNFC = $this->Feed[$C]->NFCDigest * (100 - $this->Feed[$C]->NDF - $this->Feed[$C]->CP - $this->Feed[$C]->Fat - $this->Feed[$C]->Ash + $this->Feed[$C]->NDFIP) * $this->Feed[$C]->PAF;
                $this->Output->DietaryNFC = $this->Output->DietaryNFC + (((100 - $this->Feed[$C]->NDF - $this->Feed[$C]->CP - $this->Feed[$C]->Fat - $this->Feed[$C]->Ash + $this->Feed[$C]->NDFIP) / 100) * $this->Feed[$C]->DMFed);

                if ($this->Feed[$C]->CP > 0) {
                    if ($this->Feed[$C]->EnergyEqClass == "Forage") {
                        $tdCP = Exp((-1.2 * ($this->Feed[$C]->ADFIP / $this->Feed[$C]->CP))) * $this->Feed[$C]->CP;
                    } else {
                        if ($this->Feed[$C]->Category == "Animal Protein") {
                            $tdCP = ($this->Feed[$C]->CPDigest * $this->Feed[$C]->CP);
                        } else {
                            $tdCP = (1 - (0.4 * ($this->Feed[$C]->ADFIP / $this->Feed[$C]->CP))) * $this->Feed[$C]->CP;
                        }
                    }
                } else {
                    $tdCP = 0;
                }


                if ($this->Feed[$C]->Fat < 1) {
                    $tdFat = 0;
                } else {
                    $tdFat = ($this->Feed[$C]->Fat - 1) * 2.25;
                }


                if ($this->Feed[$C]->NDF > $this->Feed[$C]->NDFIP) {
                    $dNDF = $this->Feed[$C]->NDFDigest * $this->Feed[$C]->NDF;
                } else {
                    $dNDF = 0;
                }


                if ($this->Feed[$C]->Category == "Fat") {
                    if ($this->Feed[$C]->EnergyEqClass == "Fatty Acid") {
                        $this->Output->TDN[$C] = ($this->Feed[$C]->Fat * $this->Feed[$C]->FatDigest * 2.25);
                        $this->Output->DE[$C] = 0.094 * $this->Feed[$C]->FatDigest * $this->Feed[$C]->Fat;
                    } else {
                        $this->Output->TDN[$C] = 10 + (($this->Feed[$C]->Fat - 10) * $this->Feed[$C]->FatDigest * 2.25);
                        $this->Output->DE[$C] = ($this->Feed[$C]->FatDigest * ($this->Feed[$C]->Fat - 10) * 0.094) + 0.43;
                    }
                } else {
                    if ($this->Feed[$C]->Category == "Animal Protein") {
                        $this->Output->TDN[$C] = ($this->Feed[$C]->CPDigest * $this->Feed[$C]->CP) + (($this->Feed[$C]->Fat - 1) * 2.25) + (($this->Feed[$C]->NFCDigest * (100 - $this->Feed[$C]->CP - $this->Feed[$C]->Ash - $this->Feed[$C]->Fat)) - 7);
                        $this->Output->DE[$C] = ($tdNFC * 0.042) + ($tdCP * 0.056) + (0.094 * ($tdFat / 2.25)) - 0.3;
                    } else {
                        if ($dNDF > 0) {
                            $this->Output->TDN[$C] = $tdNFC + $tdCP + $tdFat + $dNDF - 7;
                            $this->Output->DE[$C] = ($tdNFC * 0.042) + ($dNDF * 0.042) + ($tdCP * 0.056) + (0.094 * ($tdFat / 2.25)) - 0.3;
                        } else {
                            if ($this->Feed[$C]->CP > 0) {
                                $this->Output->TDN[$C] = ((0.98 * $this->Feed[$C]->PAF) * (100 - $this->Feed[$C]->CP - $this->Feed[$C]->Fat - $this->Feed[$C]->Ash)) + ($this->Feed[$C]->CP * (1 - (0.4 * ($this->Feed[$C]->ADFIP / $this->Feed[$C]->CP)))) + ((2.25 * ($this->Feed[$C]->Fat - 1) - 7));
                                $this->Output->DE[$C] = (0.98 * $this->Feed[$C]->PAF) * (0.042 * (100 - $this->Feed[$C]->CP - $this->Feed[$C]->Fat - $this->Feed[$C]->Ash)) + ($this->Feed[$C]->CP * (0.056 * (1 - (0.4 * ($this->Feed[$C]->ADFIP / $this->Feed[$C]->CP))))) + (0.094 * ($this->Feed[$C]->Fat - 1)) - 0.3;
                            } else {
                                $this->Output->TDN[$C] = ((0.98 * $this->Feed[$C]->PAF) * (100 - $this->Feed[$C]->Fat - $this->Feed[$C]->Ash)) + ((2.25 * ($this->Feed[$C]->Fat - 1) - 7));
                                $this->Output->DE[$C] = (0.98 * $this->Feed[$C]->PAF) * (0.042 * (100 - $this->Feed[$C]->Fat - $this->Feed[$C]->Ash)) + (0.094 * ($this->Feed[$C]->Fat - 1)) - 0.3;
                            }
                        }
                    }
                }



                # Don't generate energy values for vitamin/minerals
                if ($this->Feed[$C]->Category == "Vitamin/Mineral") {
                    $this->Output->TDN[$C] = 0;
                    $this->Output->DE[$C] = 0;
                }
            }
        }






        # These variables hold the values of DE and ME before a discount
        # is applied since the NEg equation used (from the 1996 Beef NRC)
        # uses an un-discounted ME in its equations
        $UndiscountedME; //public 

        $TotalTDN; //public 
        $TDNConc; //public 


        $TotalTDN = 0;
        for ($X = 1; $X <=  $this->NumFeeds; $X++) {
            if (strpos($this->Feed[$X]->Category, "Calf Feed") == false) {               //changed VB InStr to strpos that returns false instead of 0.
                $TotalTDN = $TotalTDN + (($this->Feed[$X]->TDN / 100) * $this->Feed[$X]->DMFed);
            }
        }


        if ($this->Input->AnimalType != "Young Calf") {
            if ($this->Input->AnimalType == "Replacement Heifer") {
                $this->Output->DMI_to_DMIMaint = $TotalTDN / (0.035 * (pow($SBWlocal , 0.75)));
            } else {
                # Note that this equation uses BW instead of SBW since it
                # is for a mature animal
                $this->Output->DMI_to_DMIMaint = $TotalTDN / (0.035 * (pow($this->Input->BW , 0.75)));
            }
        } else {
            # The DMI for maintenance for a young calf is downstream
            # from the energy computations, which require that the
            # DMI/DMI_Maint value be estimated first.  This is a circular
            # path which cannot be resolved, so instead this ratio is
            # estimated using the equation for mature animals, given above.
            $this->Output->DMI_to_DMIMaint = $TotalTDN / (0.035 * (pow($this->Input->CalfBW , 0.75)));
        }

        if ($this->Output->TotalRegDMFed > 0) {
            $TDNConc = ($TotalTDN / $this->Output->TotalRegDMFed) * 100;
        } else {
            $TDNConc = 0;
        }


        # Compute total fat in diet
        $this->Output->Fat_Total = 0;

        for ($C = 1; $C <=  $this->NumFeeds; $C++) {
            if (strpos($this->Feed[$C]->Category, "Calf Feed") == false) {  //changed VB InStr to strpos that returns false instead of 0.
                $this->Output->Fat_Total = $this->Output->Fat_Total + (($this->Feed[$C]->Fat / 100) * $this->Feed[$C]->DMFed);
            }
        }

        $TotalFat; //public     # This is the total fat in whole number units (e.g. 5% instead of 0.05)

        if ($this->Output->TotalRegDMFed > 0) {
            $TotalFat = ($this->Output->Fat_Total / $this->Output->TotalRegDMFed) * 100;
        } else {
            $TotalFat = 0;
        }


        $TotalDigestibleFat; //public 
        $DigestibleFat; //public 

        $TotalDigestibleFat = 0;

        for ($C = 1; $C <=  $this->NumFeeds; $C++) {
            if ($this->Feed[$C]->Category == "Fat") {
                if ($this->Feed[$C]->EnergyEqClass == "Fat") {
                    $DigestibleFat = 10 + (($this->Feed[$C]->Fat - 10) * $this->Feed[$C]->FatDigest);
                } else {
                    $DigestibleFat = $this->Feed[$C]->Fat * $this->Feed[$C]->FatDigest;
                }
            } else {
                $DigestibleFat = $this->Feed[$C]->Fat - 1;
            }


            $DigestibleFat = $DigestibleFat * $this->Feed[$C]->DMFed;

            $TotalDigestibleFat = $TotalDigestibleFat + $DigestibleFat;
        } //end forloop


        if ($this->Output->TotalDMFed > 0) {
            $TotalDigestibleFat = $TotalDigestibleFat / $this->Output->TotalDMFed;
        } else {
            $TotalDigestibleFat = 0;
        }



        $Adj_TDN; //public 

        if ($this->Output->TotalRegDMFed > 0) {
            if (($this->Output->Fat_Total / $this->Output->TotalRegDMFed) > 0.03) {

                if ($TotalFat > 0) {
                    $Adj_TDN = $TDNConc - ((($TotalFat) - 3) * ($TotalDigestibleFat / $TotalFat) * 2.25);
                } else {
                    $Adj_TDN = 0;
                }

                $TDNConc = $Adj_TDN / ((100 - ($TotalFat - 3)) / 100);
            }
        }


        $DiscountVariable = ((0.18 * $TDNConc) - 10.3) * ($this->Output->DMI_to_DMIMaint - 1);

        if ($DiscountVariable < 0) {
            $DiscountVariable = 0;
        }


        if ($TDNConc < 60) {
            $this->Output->Discount = 1;
        } elseif (($TDNConc > 60) && (($TDNConc - $DiscountVariable) < 60)) {
            $this->Output->Discount = 60 / $TDNConc;
        } elseif ($TDNConc > 0) {
            $this->Output->Discount = ($TDNConc - $DiscountVariable) / $TDNConc;
        } else {
            $this->Output->Discount = 1;
        }


        # TDN Discount cannot be negative
        if ($this->Output->Discount < 0) {
            $this->Output->Discount = 1;
        }




        # Discount the TDN
        for ($C = 1; $C <=  $this->NumFeeds; $C++) {
            if (strpos($this->Feed[$C]->Category, "Calf Feed") == false) {  //changed VB InStr to strpos that returns false instead of 0.
                $this->Feed[$C]->TDN_ActX = $this->Feed[$C]->TDN * $this->Output->Discount;//DAVID ok, allowed to be calculated.
            }
        } //end forloop
        # Note :  This value has units of %DM
        if ($this->Output->TotalRegDMFed > 0) {
            $this->Output->Fat_Total = ($this->Output->Fat_Total / $this->Output->TotalRegDMFed) * 100;
        } else {
            $this->Output->Fat_Total = 0;
        }





        for ($C = 1; $C <=  $this->NumFeeds; $C++) {
            $this->Feed[$C]->DiscDE = $this->Output->Discount * $this->Feed[$C]->DE;  //DAVID ok, allowed to be calculated.

            if ($this->Feed[$C]->Fat >= 3) {
                if (($this->Input->AnimalType != "Replacement Heifer")) {
                    $this->Feed[$C]->MEng = (1.01 * $this->Feed[$C]->DiscDE) - 0.45 + (0.0046 * ($this->Feed[$C]->Fat - 3));//DAVID ok, allowed to be calculated.
                } else {
                    $this->Feed[$C]->MEng = 0.82 * $this->Feed[$C]->DE;//DAVID ok, allowed to be calculated.
                }

                $this->Feed[$C]->NEl = (0.703 * $this->Feed[$C]->MEng) - 0.19 + ((((0.097 * $this->Feed[$C]->MEng) + 0.19) / 97) * ($this->Feed[$C]->Fat - 3));//DAVID ok, allowed to be calculated.
            } else {
                if ((($this->Input->AnimalType != "Young Calf") && ($this->Input->AnimalType != "Replacement Heifer"))) {
                    $this->Feed[$C]->MEng = (1.01 * $this->Feed[$C]->DiscDE) - 0.45;//DAVID ok, allowed to be calculated.
                } else {
                    $this->Feed[$C]->MEng = 0.82 * $this->Feed[$C]->DE;//DAVID ok, allowed to be calculated.
                }

                $this->Feed[$C]->NEl = (0.703 * $this->Feed[$C]->MEng) - 0.19;//DAVID ok, allowed to be calculated.
            }


            if ($this->Feed[$C]->Category != "Fat") {
                $this->Feed[$C]->MEforNEg = 0.82 * $this->Feed[$C]->DE;//DAVID ok, allowed to be calculated.

                $this->Feed[$C]->NEg = ((1.42 * $this->Feed[$C]->MEforNEg) - (0.174 * (pow($this->Feed[$C]->MEforNEg , 2))) + (0.0122 * (pow($this->Feed[$C]->MEforNEg , 3))) - 1.65);

                if ($this->Feed[$C]->NEg < 0) {
                    $this->Feed[$C]->NEg = 0;
                }


                $this->Feed[$C]->NEm = ((1.37 * $this->Feed[$C]->MEforNEg) - (0.138 * (pow($this->Feed[$C]->MEforNEg , 2))) + (0.0105 * (pow($this->Feed[$C]->MEforNEg , 3))) - 1.12);
            } else {
                $this->Feed[$C]->MEng = $this->Feed[$C]->DiscDE;
                $this->Feed[$C]->NEl = 0.8 * $this->Feed[$C]->DiscDE;
                $this->Feed[$C]->NEm = 0.8 * $this->Feed[$C]->MEng;
                $this->Feed[$C]->NEg = 0.55 * $this->Feed[$C]->MEng;
            }


            # Non-negativity constraints
            if ($this->Feed[$C]->MEng < 0) {
                $this->Feed[$C]->MEng = 0;
            }

            if ($this->Feed[$C]->NEl < 0) {
                $this->Feed[$C]->NEl = 0;
            }

            if ($this->Feed[$C]->NEm < 0) {
                $this->Feed[$C]->NEm = 0;
            }

            if ($this->Feed[$C]->NEg < 0) {
                $this->Feed[$C]->NEg = 0;
            }


            $this->Output->MEng_Total = $this->Output->MEng_Total + ($this->Feed[$C]->MEng * $this->Feed[$C]->DMFed);
            $this->Output->NEl_Total = $this->Output->NEl_Total + ($this->Feed[$C]->NEl * $this->Feed[$C]->DMFed);
            $this->Output->NEg_Total = $this->Output->NEg_Total + ($this->Feed[$C]->NEg * $this->Feed[$C]->DMFed);
            $this->Output->NEm_Total = $this->Output->NEm_Total + ($this->Feed[$C]->NEm * $this->Feed[$C]->DMFed);
        } //end forloop
        # These variables are total concentration values (Mcal/kg)
        $TotalDEConc; //public 
        $TotalMEConc; //public 
        $TotalNEmConc; //public 
        $TotalNElConc; //public 
        $TotalNEgConc; //public 


        if ($this->Output->TotalRegDMFed > 0) {
            $TotalMEConc = $this->Output->MEng_Total / $this->Output->TotalRegDMFed;
            $TotalNElConc = $this->Output->NEl_Total / $this->Output->TotalRegDMFed;
            $TotalNEgConc = $this->Output->NEg_Total / $this->Output->TotalRegDMFed;
            $TotalNEmConc = $this->Output->NEm_Total / $this->Output->TotalRegDMFed;
        } else {
            $TotalMEConc = 0;
            $TotalNElConc = 0;
            $TotalNEgConc = 0;
            $TotalNEmConc = 0;
        }


        if ($this->Input->AnimalType != "Replacement Heifer") {
            if ($this->Output->TotalRegDMFed > 0) {
                $this->Output->NEDietConc = $this->Output->NEl_Total / $this->Output->TotalRegDMFed;
            } else {
                $this->Output->NEDietConc = 0;
            }
        } else {
            if ($this->Output->TotalRegDMFed > 0) {
                $this->Output->NEDietConc = $this->Output->NEm_Total / $this->Output->TotalRegDMFed;
            } else {
                $this->Output->NEDietConc = 0;
            }
        }
    }

//end function

    public function MW_From_Breed($Br) {

        switch ($Br) {
            Case "Ayrshire":
                $MW_From_Breed = 545;
                break;
            Case "Brown Swiss":
                $MW_From_Breed = 682;
                break;
            Case "Guernsey":
                $MW_From_Breed = 500;
                break;
            Case "Holstein":
                $MW_From_Breed = 682;
                break;
            Case "Jersey":
                $MW_From_Breed = 454;
                break;
            Case "Milking Shorthorn":
                $MW_From_Breed = 568;
        }

        return $MW_From_Breed;
    }

    public function CBW_From_MW() {

        $CBW_From_MW = 0.06275 * $this->Input->MW;
        return $CBW_From_MW;
    }

}

////////////////////////////////////////////////////////////////////////////
# Feed Components
class FeedType extends stdClass {

    public $DMFed;           # Quantity of this feed fed to the animal (dry matter basis)
    public $Name;            # name of the feed (e.g. "Corn Silage, mature")
    public $ReadOnly;       # is "True" if the feed is part of the original library
    public $Category;        # feed category (e.g. Forage, Grass, Animal Product,...)
    public $IFN;             # International Reference Number
    public $TDN;             # total digestible nutrients (%DM) at 1X Maintenance
    public $EnergyEqClass;   # "Forage", "Concentrate" or "Animal"
    public $ForageDescrp;    # "Wet" vs. "Dry"
    public $PAF;             # processing adjustment factor
    public $DE;              # digestible energy (Mcal)
    public $DM;              # dry matter (%AF)
    public $NDF;             # neutral detergent fiber (%DM)
    public $ADF;             # acid detergent fiber (%DM)
    public $Lignin;          # (% DM)
    public $CP;              # crude protein (%DM)
    public $NDFIP;           # (%DM)
    public $ADFIP;           # (%DM)
    public $PrtA;            # (%CP)
    public $PrtB;            # (%CP)
    public $PrtC;            # (%CP)
    public $Kd;              # Protein Digestion rate (%/hr)
    public $RUPDigest;       # (%)
    public $Fat;             # (%DM)
    public $Ash;             # (%DM)
    public $Ca;              # (%DM)
    public $CaBio;           # Bioavailablity of Ca in the feed (%)
    public $P;               # (%DM)
    public $PBio;            # Bioavailablity of P in the feed (%)
    public $Mg;              # (%DM)
    public $MgBio;           # Bioavailablity of Mg in the feed (%)
    public $Cl;              # (%DM)
    public $ClBio;           # Bioavailability of Cl in the feed (%)
    public $K;               # (%DM)
    public $KBio;            # Bioavailability of K in the feed (%)
    public $Na;              # (%DM)
    public $NaBio;           # Bioavailability of Na in the feed (%)
    public $s;               # (%DM)
    public $SBio;            # Bioavailability of S in the feed (%)
    public $Co;              # (mg/kg)
    public $CoBio;           # Bioavailability of Co in the feed (%)
    public $Cu;              # (mg/kg)
    public $CuBio;           # Bioavailability of Cu in the feed (%)
    public $I;               # (mg/kg)
    public $IBio;            # Bioavailability of I in the feed (%)
    public $Fe;              # (mg/kg)
    public $FeBio;           # Bioavailability of Fe in the feed (%)
    public $Mn;              # (mg/kg)
    public $MnBio;           # Bioavailability of Mn in the feed (%)
    public $Se;              # (mg/kg)
    public $SeBio;           # Bioavailability of Se in the feed (%)
    public $Zn;              # (mg/kg)
    public $ZnBio;           # Bioavailability of Zn in the feed (%)
    public $Met;
    public $Lys;
    public $Arg;
    public $His;
    public $Ile;
    public $Leu;
    public $Cys;
    public $Phe;
    public $Thr;
    public $Trp;
    public $Val;
    public $VitA;            # (1000 IU/kg)
    public $VitD;            # (1000 IU/kg)
    public $VitE;            # (IU/kg)
    public $NFCDigest;       # NFC Digestibility Coefficient
    public $CPDigest;        # CP Digestibility Coefficient
    public $NDFDigest;       # NDF Digestibility Coefficient
    public $FatDigest;       # Fat Digestibility Coefficient
    # The following characteristics apply to Calf Feeds only
    # the "c" prefix is given to any characteristics that have
    # non-calf counterparts (e.g. "cMEng")
    public $cDM;              # (%AF)
    public $cGE;              # (Mcal/kg DM)
    public $cDE;              # (Mcal/kg DM)
    public $cMEng;            # (Mcal/kg DM)
    public $cNEm;             # (Mcal/kg DM)
    public $cNEg;             # (Mcal/kg DM)
    public $cCP;              # (%DM)
    public $cDCP;             # (%DM)
    public $cEE;              # (%DM)
    public $cAsh;             # (%DM)
    # Computed compositional values
    //FIXME: where are they computed from???
    public $TDN_ActX;        # TDN at Intake Over Maintenance //DAVID: these were all originally delcared as singles. seem to be sub values such as: Feed[$X].TDN_AtcX
    public $DiscDE;          # Discounted DE (Mcal/kg)
    public $MEforNEg;        # efficiency of conversion of ME to NEg
    public $MEng;            # *** ME - metabolizable energy (Mcal/kg) ***
    public $NEl;             # *** net energy of lactation (Mcal/kg) ***
    public $NEg;             # *** net energy of growth (Mcal/kg) ***
    public $NEm;             # *** net energy for maintenance (Mcal/kg) ***

}

/** put all the input variables in there own class to help with code readability */
class Input extends stdClass{

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////THESE VARIABLES MUST BE SET BY THE CONSTRUCTOR //////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* not used in the program.
      # Program Settings variables
      public $Metric;            # True = Metric units, False = English units
      public $DryMatterBasis;    # True = Dry Matter Basis
      public $Comments;
      public $Header = array();     # Header(1) = Left, Header(2) = Center, Header(3) = Right
      public $Footer = array();     # Footer(1) = Left, Footer(2) = Center, Footer(3) = Right
      public $SumRes = array();     # Summary Results
      public $DefaultZoom;
     */

    # Animal Description variables
    public $AnimalType;     # Options : "Lactating Cow", "Dry Cow" "Replacement Heifer", "Young Calf"
    public $Age;            # age  (months)
    public $BW;             # body weight
    public $DaysPreg;      # days pregnant
    public $CS;             # condition score (1 - 5)
    public $DaysInMilk;    # days in milk
    public $LactNum;          # lactation number
    public $FirstCalf;      # age at first calving  (months)
    public $CalfInt;        # calving interval  (months)
    public $DesiredADG;    # desired ADG (g/day)
    public $UseTargetADG;  # if True, indicates that program should use Target ADG, instead of Desired ADG to determine growth requirements
    public $CalfBW;         # Calf Body Weight (kg)
    public $CalfTemp;       # Calf Temperature (deg C)
    # Animal Production variables
    public $MW;                 # mature weight
    public $MWFromBreed;       # indicates if the animal#s MW should be set as a function of the breed
    public $Breed;              # Choices :  "Ayrshire", "Brown Swiss" "Guernsey", "Holstein", "Jersey"
    public $CBW;                # calf birth weight
    public $CBWFromMW;         # to compute CBW from MW
    public $MilkProd;           # milk production
    public $MilkFat;            # milk fat  (%)
    public $ShowMilkTrue;      # indicates if milk protein should be shown on a true or crude basis /* not used */
    public $MilkTrueProtein;    # milk true protein (%)
    public $Lactose;            # milk lactose (%)
    # Management and Environment variables
    public $Temp;           # current temperature
    public $PrevTemp;       # previous temperature
    public $WindSpeed;
    public $Grazing;
    public $Distance;       # Distance between Pasture and Milking Center
    public $Topography;     # "Flat", "Hilly"
    public $Trips;         # Number of one-way trips
    public $CoatCond;       # Choices :  "Clean/Dry", "Some Mud" "Wet/Matted", "Covered with Snow/Mud"
    public $HeatStress;     # "None", "Rapid/Shallow", "Open Mouth"
    public $HairDepth;      # depth of hair on coat
    public $NightCooling;  # True = Night Cooling, False = None

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
}

class Output extends stdClass{

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////// OUTPUT VARIABLES  //////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    # output Variables
    public $TotalDMFed;        # Total quantity of dry matter fed (kg/day)
    public $TotalRegDMFed;     # Total quantity of regular feed (i.e. non-calf feed) DM Fed (kg/day)
    public $PredIntake; /* not used */
    # Pregnancy Requirements
    public $Km;                 # diet NEl/diet NE  =  efficiency of use of ME for Maintenance
    public $MEPreg;             # metabolizable energy requirement for pregnancy (Mcal/day)
    public $NEPreg;             # net energy required for pregnancy (Mcal/day)
    public $MPPreg;             # metabolizable protein requirement for pregnancy (g/day)
    public $CPPreg;             # crude protein requirement for pregnancy (g/day) /** not used???? */
    public $CW;                 # conceptus weight (kg)
    public $ADGPreg;            # average daily gain due to conceptus growth (g/day)
    # Lactation Requirements
    public $MilkEn;                  # energy content of milk (Mcal NEl/kg)
    public $YEn;                # daily energy secretion in milk (Mcal NEl/day)
    public $NELact;             # net energy requirement for lactation (Mcal/day)
    public $YProtn;             # daily milk protein yield at current stage of lactation (kg/day)
    public $YFatn;              # daily milk fat yield at current stage of lactation (kg/day)
    public $MPLact;             # metabolizable protein requirement for lactation (g/day)
    public $FCM;                # fat corrected milk production (kg/day)
    # Target Weights and ADG for Breeding Females
    public $Age1st;             # age at first calving = FirstCalf  (mon)
    public $Age2nd;             # age at second calving (mon)
    public $Age3rd;             # age at third calving (mon)
    public $Age1stBred;         # age at first breeding (mon)
    public $Wt1st;              # target weight at first calving (kg)
    public $Wt2nd;              # target weight at second calving (kg)
    public $Wt3rd;              # target weight at third calving (kg)
    public $Wt1stBred;          # target weight at first breeding (kg)
    public $ADG1st;             # average daily gain at first calving (kg/day)
    public $ADG2nd;             # average daily gain at second calving (kg/day)
    public $ADG3rd;             # average daily gain at third calving (kg/day)
    public $ADG1stBred;         # average daily gain at first breeding (kg/day)
    public $ADGNonBred;         # average daily gain needed to get to ADG1stBred (kg/day)
    public $ADG;                # average daily gain for the cow, given the current characteristics (kg/day)
    # Reserves Requirements
    public $CS_F = array();       # factor for condition score x
    public $CS5EBW;             # empty body weight of the animal for condition score 5 (kg)
    public $EBW = array();        # empty body weight of the animal for condition score x (kg)
    public $AF = array();         # proportion of fat in the animal at condition score x
    public $TF = array();         # total weight of fat in the animal at condition score x (kg)
    public $AP = array();         # proportion of protein in the animal at condition score x
    public $TP = array();         # total weight of protein in the animal at condition score x (kg)
    public $ER = array();         # energy reserves of the animal at condition score x (Mcal)
    public $Lose1CS;            # energy needed to lose one condition score (Mcal)
    public $Gain1CS;            # energy needed to gain one condition score (Mcal)
    public $NElSub;             # amount of retained energy that will be substituted for dietary NEl in order to lose one condition score (Mcal)
    public $NElReq;             # amount of energy dietary NEl will supply in order to gain one condition score (Mcal)
    public $deltaER;            # change in ER needed to move up or down one condition score (Mcal)
    public $DaysToChange;       # number of days needed to move up or down one condition score
    # Compute Energy Values
    public $DMI_to_DMIMaint;    # ratio of DMI to DMI required for maintenance
    # Energy and Protein Supply
    # N.B. Those variables that are arrays are for specific feeds.
    # For example DMI(2) will be the dry matter intake for Feed(2).
    public $PsgRate = array();           # predicted passage rate
    public $DMI = array();              # dry matter intake (kg/day)
    public $TDN = array();             # total digestible nutrients (g/day)
    public $DE  = array();             # added by DAVID. not sure about this. DE was being overwritten in feed type, so thought it should go here.
    public $TDN_Act = array();           # discounted total digestible nutrients (g/day)
    public $CP = array();                # crude protein intake (g/day)
    public $MEng = array();              # metabolizable energy (Mcal/day)
    public $NEl = array();             # net energy for lactation (Mcal/day)
    public $NEg = array();              # net energy for growth (Mcal/day)
    public $RUP = array();              # ruminally undegradable intake protein (g/day)
    public $RDP = array();              # ruminally degradable intake protein (g/day)
    public $NDF = array();              # neutral detergent fiber (kg/day)
    public $ADF = array();              # acid detergent fiber (kg/day)
    public $DMI_Total;          # total dry matter intake (kg/day)
    public $TDN_Total;          # total 1X-TDN intake (g/day)
    public $TDN_Act_Total;      # actual, discounted TDN intake (g/day)
    public $Discount;           # discounts energy value to adjust for intake
    public $UndiscDE_Total;     # total, undiscounted, DE intake (Mcal/day)
    public $DE_Total;           # total DE intake (Mcal/day)
    public $Fat_Total;          # total Fat intake (kg/day)
    public $MEng_Total;         # total ME intake (Mcal/day)
    public $NEm_Total;          # total NEm intake (Mcal/day)
    public $NEl_Total;          # total NEl intake (Mcal/day)
    public $NEg_Total;          # total NEg intake (Mcal/day)
    public $CP_Total;           # total crude protein intake (g/day)
    public $RUP_Total;          # total RUP intake (g/day)
    public $TotalDigestedRUP;   # total digested RUP (g/day)
    public $DietRUPDigest;      # total RUP digestibility for the diet (weighted average)
    public $RDP_Total;          # total RDP intake (g/day)
    public $NDF_Total;          # total NDF intake (g/day)
    public $ADF_Total;          # total ADF intake (g/day)
    public $MCP_Total;          # total MCP synthesis (g/day)
    public $DietTDN;            # fraction of TDN in the diet
    public $NEgOverMEng;        # NEg_Total/MEng_Total
    public $NElOverMEng;        # NEl_Total/MEng_Total
    public $NEmOverMEng;        # NEm_Total/MEng_Total
    public $DietaryNFC;         # total dietary NFC (g/day)
    public $EndCP;              # endogenous crude protein (g/day)
    public $MPEndo;             # endogenous metabolizable protein (g/day)
    public $MPEndoReq;          # MPEndo requirement (g/day)
    # Amino Acid Supply
    public $perEAA;             # percent total essential amino acids (%RUP)
    public $TotalEAA;           # total essential amino acids (g/day)
    public $Arg_Flow;           # flow of arginine to the small intestine (g/day)
    public $His_Flow;           # flow of histidine to the small intestine (g/day)
    public $Ile_Flow;           # flow of isoleucine to the small intestine (g/day)
    public $Leu_Flow;           # flow of leucine to the small intestine (g/day)
    public $Lys_Flow;           # flow of lysine to the small intestine (g/day)
    public $Met_Flow;           # flow of methionine to the small intestine (g/day)
    public $Phe_Flow;           # flow of phenylalanine to the small intestine (g/day)
    public $Thr_Flow;           # flow of threonine to the small intestine (g/day)
    public $Val_Flow;           # flow of valine to the small intestine (g/day)
    public $Dig_Arg_Flow;       # flow of digestible arginine to the small intestine (g/day)
    public $Dig_His_Flow;       # flow of digestible histidine to the small intestine (g/day)
    public $Dig_Ile_Flow;       # flow of digestible isoleucine to the small intestine (g/day)
    public $Dig_Leu_Flow;       # flow of digestible leucine to the small intestine (g/day)
    public $Dig_Lys_Flow;       # flow of digestible lysine to the small intestine (g/day)
    public $Dig_Met_Flow;       # flow of digestible methionine to the small intestine (g/day)
    public $Dig_Phe_Flow;       # flow of digestible phenylalanine to the small intestine (g/day)
    public $Dig_Thr_Flow;       # flow of digestible threonine to the small intestine (g/day)
    public $Dig_Val_Flow;       # flow of digestible valine to the small intestine (g/day)
    public $ArgPctMP;           # arginine as a percent of metabolizable protein (%)
    public $HisPctMP;           # histidine as a percent of metabolizable protein (%)
    public $IlePctMP;           # isoleucine as a percent of metabolizable protein (%)
    public $LeuPctMP;           # leucine as a percent of metabolizable protein (%)
    public $LysPctMP;           # lysine as a percent of metabolizable protein (%)
    public $MetPctMP;           # methionine as a percent of metabolizable protein (%)
    public $PhePctMP;           # phenylalanine as a percent of metabolizable protein (%)
    public $ThrPctMP;           # threonine as a percent of metabolizable protein (%)
    public $ValPctMP;           # valine as a percent of metabolizable protein (%)
    # Maintenance Requirements
    public $a1;                 # thermal neutral maintenance requirement (Mcal/day/BW^0.75)
    public $CS9;                # condition score on 1-9 scale (1996 NRC)
    public $COMP;               # effect of previous plane of nutrition on NEMaint requirement
    public $a2;                 # maintenance adjustment for previous ambient temperature (Mcal/day/BW^0.75)
    public $Pasture;            # percent of the predicted dry matter intake supplied by grazing
    public $NEmact;             # activity factor on the NEMaint requirement
    public $NEMaintNS;          # net energy requirement for maintenance without stress (Mcal/day)
    public $NEDietConc;         # concentration of net energy in diet (kg DM/day)
    public $FeedMaint;          # feed required for maintenance (kg DM/day)
    public $NEGrowthDietNS;     # net energy for growth, with no stress, available in the diet (Mcal/day)
    public $NEFP;               # net energy for production (Mcal/day)
    public $MEI;                # metabolizable energy intake (Mcal/day)
    public $SA;                 # surface area (m^2)
    public $HP;                 # heat production (Mcal/m^2/day)
    public $T;                  # age in days
    public $Coat;               # coat condition factor (dependent on CoatCond)
    public $TI;                 # tissue insulation (Mcal/m^2/deg C/day)
    public $EI;                 # external insulation (Mcal/m^2/deg C/day)
    public $INS;                # total insulation (Mcal/m^2/deg C/day)
    public $LCT;                # lower critical temperature (deg C)
    public $MEcs;               # metabolizable energy required due to cold stress (Mcal/day)
    public $ColdStr;            # cold stress factor for computing the net energy requirement
    # for maintenance with stress (Mcal/day/BW^0.75)
    public $HeatStr;            # heat stress factor for computing the net energy requirement
    # for maintenance which is dependent upon the HeatStress variable
    public $NEMaint;            # net energy required for maintenance (Mcal/day)
    public $DMIMaint;           # dry matter intake required for maintenance (kg DM/day)
    public $DMIPreg;            # dry matter intake requirement for pregnancy
    public $NEGrowthDiet;       # net energy for growth available in the diet (Mcal/day)
    public $MPMaint;            # metabolizable protein requirement for maintenance (g/day)
    public $MEMaint;            # used only for Replacement Heifers
    public $ScurfMP;            # scurf MP requirement (g/day)
    public $UrineMP;            # urine MP requirement (g/day)
    public $FecalMP;            # fecal MP requirement (g/day)
    # Dry Matter Intake
    public $TempFact;           # temperature adjustment factor for DMI calculations
    public $CCFact;             # coat condition adjustment factor for DMI calculations
    public $SubFact;            # factor used in the DMI_RH calculation
    public $DivFact;            # factor used in the DMI_RH calculation
    public $DMILact;            # DMI of a lactating cow (kg/day)
    public $DMIDry;             # DMI of a dry cow  (kg/day)
    public $DMI_RH;             # DMI of a replacement heifer (kg/day)
    public $DryMatterIntake;    # dry matter intake (kg/day)
    # Growth Requirements
    public $SBW;                # shrunk body weight (kg)
    public $SRW_to_MSBW;        # standard reference weight / mature shrunk body weight
    public $EQSBW;              # equivalent shrunk body weight (kg)
    public $SWG;               # shrunk weight gain (kg)
    public $WG;                 # weight gain (kg/day)
    public $DLWReq;             # DLW required (kg/day)
    public $EQEBW;              # equivalent empty body weight (kg)
    public $EQEBG;              # equivalent empty body gain (kg)
    public $RE;                 # retained energy (Mcal/day)
    public $NEGrowth;           # net energy required for growth (Mcal/day)
    public $NPg;                # net protein requirement (g/day)
    public $EffMP_NPg;          # efficiency of use of MP for NPg
    public $MPGrowth;           # metabolizable protein requirement for growth (g/day)
    public $NEReserves;         # net energy required for reserves replenishment
    # or provided when reserves are mobilized (Mcal NEl/day)
    public $DMIAvailGrowth;     # dry matter available for growth (kg/day)
    public $ADGwoPreg;          # average daily gain w/o pregnancy (kg/day)
    public $ADGwPreg;           # average daily gain with pregnancy (kg/day)  /* only used here ??? */
    public $MEGrowth;           # only used for Replacement Heifers
    public $MEAllowGain;
    public $MEAllowGainPreg;
   
    public $DMIPred;            # predicted dry matter intake (kg/day)
    public $DMIActual;          # actual dry matter intake (kg/day)
    public $RDPReq;             # RDP required (g/day)
    public $RDPSup;             # RDP supplied (g/day)
    public $RDPBal;             # difference between RDPReq and RDPSup (g/day)
    public $RUPReq;             # RUP required (g/day)
    public $RUPSup;             # RUP supplied (g/day)
    public $RUPBal;             # difference between RUPReq and RUPSup (g/day)
    public $MPBalance;
    public $ProteinInGain;
    public $MPAllowGain;
    public $MPAllowGainPreg;
    public $CondMessage;
    public $MPBact;             # metabolizable protein supplied by microbial protein (g/day)
    public $MPFeed;             # metabolizable protein supplied by animal#s ration (g/day)
    public $DietCP;             # fraction of CP in the diet
    public $CP_RDP;             # fraction of CP which is RDP
    public $CP_RUP;             # fraction of CP which is RUP
    public $DietNDF;            # fraction of NDF in animal#s ration
    public $DietADF;            # fraction of ADF in animal#s ration
    public $ForageNDF;          # fraction of NDF from Forage in ration
    public $DietME;             # quantity of ME in animal#s ration (Mcal/kg)
    public $DietNEl;            # quantity of NEl in animal#s ration (Mcal/kg)
    public $DietNEg;            # quantity of NEg in animal#s ration (Mcal/kg)
    public $EnergyAllowableMilk;    # energy allowable milk production (kg/day)
    public $ProteinAllowableMilk;   # protein allowable milk production (kg/day)
    public $DailyMilk;              # milk production (kg/day)
    public $TargetADGwoPreg;    # target ADG without pregnancy (kg/day)
    public $TargetADGPreg;      # target ADG with pregnancy (kg/day)
    public $Reserves_WG;        # weight gain due to reserves (kg)
    public $MPReqReserves;      # MP required for reserves (g/day)
    public $MPProvReserves;     # MP provided for reserves (g/day)
    public $RUPReqReserves;     # RUP required for reserves (g/day)
    public $RUPProvReserves;    # RUP provided for reserves (g/day)
    public $Energy_TargetDietConc;
    public $MP_TargetDietConc;
    public $Ca_TargetDietConc;
    public $P_TargetDietConc;
    public $DCAD;

    # Young Calf Computation Variables
    public $CalfKm;             # Efficiency of use of ME for NEm
    public $CalfKg;             # Efficiency of use of ME for NEg
    public $MilkDMI;            # Milk Dry Matter Intake (kg/day)
    public $StarterDMI;         # Starter Dry Matter Intake (kg/day)
    public $NEmCalf;            # Net Energy required for maintenance (Mcal/day)
    public $TempFactor;
    public $CalfFat;            # Dietary Fat (kg/day)
    public $DietFatCalf;        # Dietary Fat (%)
    public $DietNEmCalf;        # Dietary NEm (Mcal/kg)
    public $DietMECalf;         # Dietary ME  (Mcal/kg)
    public $DietNEgCalf;        # Dietary NEg (Mcal/kg)
    public $DietCPCalf;         # Dietary CP (%)
    public $DietDCPCalf;        # Dietary DCP (%)
    public $DMIForNEmCalf;      # Dry Matter Intake required to meet NEm requirement (kg/day)
    public $DMIForMECalf;       # Dry Matter Intake required to meet ME requirement  (kg/day)
    public $DMIForGrowth;       # Total dry matter intake used for growth  (kg/day)
    public $MEFGCalf;           # Metabolizable energy available for growth  (Mcal/day)
    public $NEFGCalf;           # Net Energy available for growth (Mcal/day)
    public $EnergyADGCalf;      # Energy allowable Average Daily Gain (kg/day)
    public $ProteinReqCalf;     # Protein allowable Average Daily Gain (kg/day)
    public $BV;                 # Biological value used in ADP calculation
    public $EUN;                # endogenous urinary nitrogen losses (g/day) used in ADP calculation
    public $MFN;                # metabolic fecal nitrogen (g/day), used in ADP calculation
    public $ADPmaint;           # apparently digested crude protein for maintenance (g/day)
    public $ADPgrowth;          # apparently digested crude protein for gain (g/day)
    public $CPmCalf;            # calf maintenance crude protein requirement (g/day)
    public $CPgCalf;            # calf growth crude protein required (g/day)
    public $CalfADG;            # Calf Average Daily Gain (g/day)
    public $CalfADPBal;         # digestible ADP Balance (g/day)
    public $CalfCPBal;          # digestible CP Balance (g/day)
    public $ADPAllowGain;       # ADP Allowable Gain (g/day)

}

# Mineral Requirements
class MineralType extends stdClass{

    public $Name;                    # name of mineral
    public $Units;                   # units the mineral is reported in
    public $Fecal;                   # fecal endogenous loss component
    public $Urine;                   # urine endogenous loss component
    public $Sweat;                   # sweat loss (temperature) component
    public $Misc;                    # miscellaneous loss component
    public $Maint;                   # sum of the misc, fecal, urine and sweat losses
    public $Fetal;                   # fetal requirement
    public $Milk;                    # milk production requirement
    public $Growth;                  # heifer growth requirement
    public $Total;                   # total quantity of mineral required
    public $Supplied;                # total quantity of mineral supplied by the animal#s diet
    public $Absorbable;              # total quantity of absorbable mineral supplied by the diet
    public $Balance;                 # difference between the quantity of the mineral required and supplied
    public $RD;                      # density of the mineral in the animal#s ration
    public $RDReq;                   # ration density required

}

# Diet Evaluation
class EvalType extends stdClass{

    public $Name;                 # name of evaluation factor (e.g. NE-Diet)
    public $Units;                # units of evaluation factor (e.g. Mcal/day)
    public $Total;
    public $Maint;
    public $Preg;
    public $Lact;
    public $Gain;
    public $Reserves;

}



/**
 * This class holds helper function for the NRC 2001
 * Model. Anything which doesn't diretcly involve the model.
 *  
 */
class NRCHelper {

    /**
     * creates an array of feeds with dry matter amounts to use as
     * input into the NRC model
     * @param type $recipeArray
     * @return \FeedType 
     */
    function createFeedArrayOfFeedObjs($recipeArray) {

        $feedsArray = array();

        $count=1; //needed to start index at one, to maintain compatibility with vb.
        foreach ($recipeArray as $key => $value) {
            $sql = "SELECT * FROM (Select * from nutrition.bag_analysis_overlay_feed_library()
UNION 
Select * from nutrition.feed_library_nrc2001
UNION 
Select * from nutrition.feed_library_local
) x WHERE \"Feed Name\"='$key' LIMIT 1";
           
            $res = $GLOBALS['pdo']->query($sql); //global to acces DB, yes globabls are bad, this is the only one.
            $row = $res->fetch(PDO::FETCH_ASSOC);

//convert from db names to nrc program names for feed paramaters
            $feedObj = new FeedType();

            $feedObj->DMFed = $value; //how much of the feed. This is set by the user for each feed.
            $feedObj->ReadOnly = $row['Read Only'];
            $feedObj->Name = $row['Feed Name'];
            $feedObj->Category = $row['Category'];
            $feedObj->IFN = $row['IFN'];
            $feedObj->TDN = $row['TDN (%DM)'];
            $feedObj->EnergyEqClass = $row['Energy Eq Class'];
            $feedObj->ForageDescrp = $row['Forage Description'];
            $feedObj->PAF = $row['PAF'];
            $feedObj->DE = $row['DE (Mcal/kg)'];
            $feedObj->DM = $row['DM (%AF)'];
            $feedObj->NDF = $row['NDF (%DM)'];
            $feedObj->ADF = $row['ADF (%DM)'];
            $feedObj->Lignin = $row['Lignin (%DM)'];
            $feedObj->CP = $row['CP (%DM)'];
            $feedObj->NDFIP = $row['NDFIP (%DM)'];
            $feedObj->ADFIP = $row['ADFIP (%DM)'];
            $feedObj->PrtA = $row['Prt-A (%CP)'];
            $feedObj->PrtB = $row['Prt-B (%CP)'];
            $feedObj->PrtC = $row['Prt-C (%CP)'];
            $feedObj->Kd = $row['Kd (1/hr)'];
            $feedObj->RUPDigest = $row['RUP Digest (%)'];
            $feedObj->Fat = $row['Fat (%DM)'];
            $feedObj->Ash = $row['Ash (%DM)'];
            $feedObj->Ca = $row['Ca (%DM)'];
            $feedObj->CaBio = $row['Ca-Bio (g/g)'];
            $feedObj->P = $row['P (%DM)'];
            $feedObj->PBio = $row['P-Bio (g/g)'];
            $feedObj->Mg = $row['Mg (%DM)'];
            $feedObj->MgBio = $row['Mg-Bio (g/g)'];
            $feedObj->Cl = $row['Cl (%DM)'];
            $feedObj->ClBio = $row['Cl-Bio (g/g)'];
            $feedObj->K = $row['K (%DM)'];
            $feedObj->KBio = $row['K-Bio (g/g)'];
            $feedObj->Na = $row['Na (%DM)'];
            $feedObj->NaBio = $row['Na-Bio (g/g)'];
            $feedObj->s = $row['S (%DM)'];
            $feedObj->SBio = $row['S-Bio (g/g)'];
            $feedObj->Co = $row['Co (mg/kg)'];
            $feedObj->CoBio = $row['Co-Bio (g/g)'];
            $feedObj->Cu = $row['Cu (mg/kg)'];
            $feedObj->CuBio = $row['Cu-Bio (g/g)'];
            $feedObj->I = $row['I (mg/kg)'];
            $feedObj->IBio = $row['I-Bio (g/g)'];
            $feedObj->Fe = $row['Fe (mg/kg)'];
            $feedObj->FeBio = $row['Fe-Bio (g/g)'];
            $feedObj->Mn = $row['Mn (mg/kg)'];
            $feedObj->MnBio = $row['Mn-Bio (g/g)'];
            $feedObj->Se = $row['Se (mg/kg)'];
            $feedObj->SeBio = $row['Se-Bio (g/g)'];
            $feedObj->Zn = $row['Zn (mg/kg)'];
            $feedObj->ZnBio = $row['Zn-Bio (g/g)'];
            $feedObj->Met = $row['Met (%CP)'];
            $feedObj->Lys = $row['Lys (%CP)'];
            $feedObj->Arg = $row['Arg (%CP)'];
            $feedObj->His = $row['His (%CP)'];
            $feedObj->Ile = $row['Ile (%CP)'];
            $feedObj->Leu = $row['Leu (%CP)'];
            $feedObj->Cys = $row['Cys (%CP)'];
            $feedObj->Phe = $row['Phe (%CP)'];
            $feedObj->Thr = $row['Thr (%CP)'];
            $feedObj->Trp = $row['Trp (%CP)'];
            $feedObj->Val = $row['Val (%CP)'];
            $feedObj->VitA = $row['Vit-A (1000 IU/kg)'];
            $feedObj->VitD = $row['Vit-D (1000 IU/kg)'];
            $feedObj->VitE = $row['Vit-E (IU/kg)'];
            $feedObj->NFCDigest = $row['NFC Digest'];
            $feedObj->CPDigest = $row['CP Digest'];
            $feedObj->NDFDigest = $row['NDF Digest'];
            $feedObj->FatDigest = $row['Fat Digest'];
            $feedObj->cDM = $row['cDM (%)'];
            $feedObj->cGE = $row['cGE (Mcal/kg)'];
            $feedObj->cDE = $row['cDE (Mcal/kg)'];
            $feedObj->cMEng = $row['cME (Mcal/kg)'];
            $feedObj->cNEm = $row['cNEm (Mcal/kg)'];
            $feedObj->cNEg = $row['cNEg (Mcal/kg)'];
            $feedObj->cCP = $row['cCP (%DM)'];
            $feedObj->cDCP = $row['cDCP (%DM)'];
            $feedObj->cEE = $row['cEE (%DM)'];
            $feedObj->cAsh = $row['cASH (%DM)'];

            $feedsArray[$count] = $feedObj; //to return later
            $count++;
        } //end foreach

        return $feedsArray;
    }

    /* set defaults that are later overriden */

    function createDefaultInputObject($AnimalType) {

        //Input
        $input = new Input();

        //different default inputs for different animal types.
        switch ($AnimalType) {
            case 'Lactating Cow':
                //Animal Description
                $input->AnimalType = 'Lactating Cow';     # Options : "Lactating Cow", "Dry Cow" "Replacement Heifer", "Young Calf"
                $input->Age = 65;            # age  (months)
                $input->BW = 680;             # body weight
                $input->DaysPreg = 0;      # days pregnant
                $input->CS = 3.0;             # condition score (1 - 5)
                $input->DaysInMilk = 90;    # days in milk
                $input->LactNum = 3;          # lactation number
                $input->FirstCalf = 24;      # age at first calving  (months)
                $input->CalfInt = 12;        # calving interval  (months)
                $input->DesiredADG = 0;    # desired ADG (g/day) //NOTE: set to value when UseTargetADG = false
                $input->UseTargetADG = true;  # if True, indicates that program should use Target ADG, instead of Desired ADG to determine growth requirements
                $input->CalfBW = 0;         # Calf Body Weight (kg)
                $input->CalfTemp = 0;       # Calf Temperature (deg C)
                //Production
                $input->MW = 680;                 # mature weight
                $input->MWFromBreed = true;       # indicates if the animal#s MW should be set as a function of the breed
                $input->Breed = 'Holstein';              # Choices :  "Ayrshire", "Brown Swiss" "Guernsey", "Holstein", "Jersey"
                $input->CBW = 35;                # calf birth weight
                $input->CBWFromMW = null;          # to compute CBW from MW /* not used */
                $input->MilkProd = 0;           # milk production
                $input->MilkFat = 0;            # milk fat  (%)
                $input->ShowMilkTrue = null;      # indicates if milk protein should be shown on a true or crude basis /* not used */
                $input->MilkTrueProtein = 0;    # milk true protein (%)
                $input->Lactose = 0;            # milk lactose (%)
                // Management and Environment variables
                $input->Temp = 20;           # current temperature
                $input->PrevTemp = 0;       # previous temperature
                $input->WindSpeed = 0;
                $input->Grazing = 0;
                $input->Distance = 0;       # Distance between Pasture and Milking Center
                $input->Topography = 'Flat';     # "Flat", "Hilly"
                $input->Trips = 0;         # Number of one-way trips
                $input->CoatCond = 'Clean/Dry';       # Choices :  "Clean/Dry", "Some Mud" "Wet/Matted", "Covered with Snow/Mud"
                $input->HeatStress = 'None';     # "None", "Rapid/Shallow", "Open Mouth"
                $input->HairDepth = 0;      # depth of hair on coat
                $input->NightCooling = false;  # True = Night Cooling, False = None
                break;
            default:
                throw new Exception("ERROR: input animal type is incorrect.");
        }
        
        return $input;
    }

}

/* end of file */
?>
