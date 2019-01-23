<?php 
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
    include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/misc.inc');
}

/**
 * This reads the herdbook datafile emailed to us with classification data from holstein canada
 * Standard Genetic Client
 */

 
class HolCanHerdBookImport {


function main() {
		//load all the herbook files in the directory.
		$filepath="../local/dataFile/holsteinCanada/"; //hardcoded....bad.
		if ($handle = opendir($filepath)) {
			/* This is the correct way to loop over the directory. */
			while (false !== ($file = readdir($handle))) {
				if (strpos($file,'.txt') !==false) {
					$arrayOfFiles[]=$file;
				}
			}
			closedir($handle);
		}

		print("Doing the following files:\n");
		print_r($arrayOfFiles);

                //override with one file for debug ONLY
                //$arrayOfFiles=array('Heifer-1508.txt'); //debug
		
		//empty the data table first
		$res=$GLOBALS['pdo']->exec("TRUNCATE batch.holstein_canada_data");
		
		
		//read file
		foreach ($arrayOfFiles as $arrayOfFiles_num => $fileName)
		{
			$lines = file("$filepath$fileName");

                 set_time_limit(30);//add 30 seconds to time out counter, to give time to script to complete. This allo
print("Doing $fileName.\n\r");

			foreach ($lines as $line_num => $line) {

				/* NOTE: Reg numbers can have letters in them in countries like Italy */
				
				$lineArray['CLIENT_ID']['val']=trim(substr($line,$x=0 ,$y=20)); $lineArray['CLIENT_ID']['num']=false;
				$lineArray['PHONE_NUMBER']['val']=trim(substr($line,$x=$x+$y,$y=16));$lineArray['PHONE_NUMBER']['num']=false;
				$lineArray['BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['BREED']['num']=false;
				$lineArray['COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['COUNTRY']['num']=false;
				$lineArray['SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['SEX']['num']=false;
				$lineArray['REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12));$lineArray['REG_NO']['num']=false;
				$lineArray['NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['NAME']['num']=false;
				$lineArray['BIRTH_DATE']['val']=trim(substr($line,$x=$x+$y,$y= 8));$lineArray['BIRTH_DATE']['num']=false;
				$lineArray['COLOUR']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['COLOUR']['num']=false;
				$lineArray['PROV_EARTAG']['val']=trim(substr($line,$x=$x+$y,$y=10));$lineArray['PROV_EARTAG']['num']=true;
				$lineArray['ONFARM_EARTAG']['val']=trim(substr($line,$x=$x+$y,$y= 10 ));$lineArray['ONFARM_EARTAG']['num']=true;
				$lineArray['RFID']['val']=trim(substr($line,$x=$x+$y,$y=16));$lineArray['RFID']['num']=true;
				$lineArray['CHAIN_NUM']['val']=trim(substr($line,$x=$x+$y,$y= 6 ));$lineArray['CHAIN_NUM']['num']=true;
				$lineArray['COMMON_NAME']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['COMMON_NAME']['num']=false;
				$lineArray['CLASSIFICATION_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y= 3 ));$lineArray['CLASSIFICATION_COUNTRY']['num']=false;
				$lineArray['CLASSIFICATION']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['CLASSIFICATION']['num']=false;
				$lineArray['CLASSIFICATION_SCORE']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['CLASSIFICATION_SCORE']['num']=true;
				$lineArray['MULTIPLE_EX']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['MULTIPLE_EX']['num']=true;
				$lineArray['SIRE_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['SIRE_BREED']['num']=false;
				$lineArray['SIRE_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['SIRE_COUNTRY']['num']=false;
				$lineArray['SIRE_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['SIRE_SEX']['num']=false;
				$lineArray['SIRE_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12));$lineArray['SIRE_REG_NO']['num']=false;
				$lineArray['SIRE_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['SIRE_NAME']['num']=false;
				$lineArray['DAM_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['DAM_BREED']['num']=false;
				$lineArray['DAM_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['DAM_COUNTRY']['num']=false;
				$lineArray['DAM_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['DAM_SEX']['num']=false;
				$lineArray['DAM_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['DAM_REG_NO']['num']=false;
				$lineArray['DAM_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['DAM_NAME']['num']=false;
				$lineArray['DAM_CLASSIFICATION_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y= 3 ));$lineArray['DAM_CLASSIFICATION_COUNTRY']['num']=false;
				$lineArray['DAM_CLASSIFICATION']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['DAM_CLASSIFICATION']['num']=false;
				$lineArray['DAM_CLASSIFICATION_SCORE']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['DAM_CLASSIFICATION_SCORE']['num']=true;
				$lineArray['DAM_MULTIPLE_EX']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['DAM_MULTIPLE_EX']['num']=true;
				$lineArray['MGS_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['MGS_BREED']['num']=false;
				$lineArray['MGS_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['MGS_COUNTRY']['num']=false;
				$lineArray['MGS_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['MGS_SEX']['num']=false;
				$lineArray['MGS_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['MGS_REG_NO']['num']=false;
				$lineArray['MGS_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['MGS_NAME']['num']=false;
                $lineArray['PROD_EVAL_DATE']['val']=trim(substr($line,$x=$x+$y,$y= 6));$lineArray['PROD_EVAL_DATE']['num']=false;
				$lineArray['PROD_TYPE_OF_PROOF']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['PROD_TYPE_OF_PROOF']['num']=true;
				$lineArray['PROD_EBV_MILK_KG']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['PROD_EBV_MILK_KG']['num']=true;
				$lineArray['PROD_RANKING_MILK']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['PROD_RANKING_MILK']['num']=true;
				$lineArray['PROD_EBV_FAT_KG']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['PROD_EBV_FAT_KG']['num']=true;
				$lineArray['PROD_RANKING_FAT']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['PROD_RANKING_FAT']['num']=true;
				$lineArray['PROD_PERCENT_FAT']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['PROD_PERCENT_FAT']['num']=true;
				$lineArray['PROD_PROTEIN_KG']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['PROD_PROTEIN_KG']['num']=true;
				$lineArray['PROD_RANKING_PROTEIN']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['PROD_RANKING_PROTEIN']['num']=true;
				$lineArray['PROD_PERCENT_PROTEIN']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['PROD_PERCENT_PROTEIN']['num']=true;
				$lineArray['PROD_RELIABILITY']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['PROD_RELIABILITY']['num']=true;
				$lineArray['PROD_SCS']['val']=trim(substr($line,$x=$x+$y,$y= 4));$lineArray['PROD_SCS']['num']=true;
				$lineArray['PROD_SCS_RELIABILITY']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['PROD_SCS_RELIABILITY']['num']=true;
				$lineArray['PROD_INBREEDING_COEFF']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['PROD_INBREEDING_COEFF']['num']=true;
				$lineArray['PROD_R_VALUE']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['PROD_R_VALUE']['num']=true;
				$lineArray['CONF_EVAL_DATE']['val']=trim(substr($line,$x=$x+$y,$y= 6));$lineArray['CONF_EVAL_DATE']['num']=false;
				$lineArray['CONF_TYPE_OF_PROOF']['val']=trim(substr($line,$x=$x+$y,$y=2 ));$lineArray['CONF_TYPE_OF_PROOF']['num']=true;
				$lineArray['CONF_EBV_CONF']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_EBV_CONF']['num']=true;
				$lineArray['CONF_RANKING']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['CONF_RANKING']['num']=true;
				$lineArray['CONF_TYPE_RELIABILITY']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['CONF_TYPE_RELIABILITY']['num']=true;
				$lineArray['CONF_TYPE_OF_PROOF_SCORECARD']['val']=trim(substr($line,$x=$x+$y,$y= 2));$lineArray['CONF_TYPE_OF_PROOF_SCORECARD']['num']=true;
				$lineArray['CONF_EBV_RP']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_RP']['num']=true;
				$lineArray['CONF_EBV_MS']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_MS']['num']=true;
				$lineArray['CONF_EBV_FL']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_FL']['num']=true;
				$lineArray['CONF_EBV_DS']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_DS']['num']=true;
				$lineArray['CONF_TYPE_OF_PROOF_SECONDARY_TRAITS']['val']=trim(substr($line,$x=$x+$y,$y=2 ));$lineArray['CONF_TYPE_OF_PROOF_SECONDARY_TRAITS']['num']=true;
				$lineArray['CONF_EBV_PS']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_PS']['num']=true;
				$lineArray['CONF_EBV_RA']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_RA']['num']=true;
				$lineArray['CONF_EBV_RA_CODE']['val']=trim(substr($line,$x=$x+$y,$y=1 ));$lineArray['CONF_EBV_RA_CODE']['num']=false;
				$lineArray['CONF_EBV_PW']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_PW']['num']=true;
				$lineArray['CONF_EBV_LS']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_LS']['num']=true;
				$lineArray['CONF_EBV_UD']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_EBV_UD']['num']=true;
				$lineArray['CONF_EBV_UD_CODE']['val']=trim(substr($line,$x=$x+$y,$y=1 ));$lineArray['CONF_EBV_UD_CODE']['num']=false;
				$lineArray['CONF_UT']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_UT']['num']=true;
				$lineArray['CONF_MED']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_MED']['num']=true;
				$lineArray['CONF_FA']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_FA']['num']=true;
				$lineArray['CONF_FTP']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_FTP']['num']=true;
				$lineArray['CONF_FTP_CODE']['val']=trim(substr($line,$x=$x+$y,$y= 1));$lineArray['CONF_FTP_CODE']['num']=false;
				$lineArray['CONF_RAH']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_RAH']['num']=true;
				$lineArray['CONF_RAW']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_RAW']['num']=true;
				$lineArray['CONF_RTP']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_RTP']['num']=true;
				$lineArray['CONF_RTP_CODE']['val']=trim(substr($line,$x=$x+$y,$y= 1));$lineArray['CONF_RTP_CODE']['num']=false;
				$lineArray['CONF_TL']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_TL']['num']=true;
				$lineArray['CONF_TL_CODE']['val']=trim(substr($line,$x=$x+$y,$y= 1));$lineArray['CONF_TL_CODE']['num']=false;
				$lineArray['CONF_FT']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_FT']['num']=true;
				$lineArray['CONF_HD']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_HD']['num']=true;
				$lineArray['CONF_BQ']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_BQ']['num']=true;
				$lineArray['CONF_RL']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_RL']['num']=true;
				$lineArray['CONF_RLSV']['val']=trim(substr($line,$x=$x+$y,$y=3 ));$lineArray['CONF_RLSV']['num']=true;
				$lineArray['CONF_RLSV_CODE']['val']=trim(substr($line,$x=$x+$y,$y= 1));$lineArray['CONF_RLSV_CODE']['num']=false;
				$lineArray['CONF_RLRV']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_RLRV']['num']=true;
				$lineArray['CONF_ST']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_ST']['num']=true;
				$lineArray['CONF_FE']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_FE']['num']=true;
				$lineArray['CONF_CW']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_CW']['num']=true;
				$lineArray['CONF_BD']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_BD']['num']=true;
				$lineArray['CONF_ANGULARITY']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['CONF_ANGULARITY']['num']=true;
				$lineArray['LPI']['val']=trim(substr($line,$x=$x+$y,$y= 5));$lineArray['LPI']['num']=true;
				$lineArray['LPI_RANKING']['val']=trim(substr($line,$x=$x+$y,$y= 3));$lineArray['LPI_RANKING']['num']=true;
				$lineArray['MGD_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['MGD_COUNTRY']['num']=false;
				$lineArray['MGD_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['MGD_BREED']['num']=false;
				$lineArray['MGD_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['MGD_SEX']['num']=false;
				$lineArray['MGD_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['MGD_REG_NO']['num']=false;
				$lineArray['MGD_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['MGD_NAME']['num']=false;
				$lineArray['PGS_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['PGS_BREED']['num']=false;
				$lineArray['PGS_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['PGS_COUNTRY']['num']=false;
				$lineArray['PGS_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['PGS_SEX']['num']=false;
				$lineArray['PGS_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['PGS_REG_NO']['num']=false;
				$lineArray['PGS_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['PGS_NAME']['num']=false;
				$lineArray['PGD_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['PGD_BREED']['num']=false;
				$lineArray['PGD_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['PGD_COUNTRY']['num']=false;
				$lineArray['PGD_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['PGD_SEX']['num']=false;
				$lineArray['PGD_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['PGD_REG_NO']['num']=false;
				$lineArray['PGD_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['PGD_NAME']['num']=false;
				$lineArray['MGGS_BREED']['val']=trim(substr($line,$x=$x+$y,$y=2));$lineArray['MGGS_BREED']['num']=false;
				$lineArray['MGGS_COUNTRY']['val']=trim(substr($line,$x=$x+$y,$y=3));$lineArray['MGGS_COUNTRY']['num']=false;
				$lineArray['MGGS_SEX']['val']=trim(substr($line,$x=$x+$y,$y=1));$lineArray['MGGS_SEX']['num']=false;
				$lineArray['MGGS_REG_NO']['val']=trim(substr($line,$x=$x+$y,$y= 12 ));$lineArray['MGGS_REG_NO']['num']=false;
				$lineArray['MGGS_NAME']['val']=trim(substr($line,$x=$x+$y,$y=  30));$lineArray['MGGS_NAME']['num']=false;
				$lineArray['EXTRACT_DATE']['val']=trim(substr($line,$x=$x+$y,$y= 8));$lineArray['EXTRACT_DATE']['num']=false;
				$lineArray['GEBV']['val']=trim(substr($line,$x=$x+$y,$y=  1));$lineArray['GEBV']['num']=false;
				//added Aug 2015 (do not have data dictionary, so all a guess).
                                $lineArray['XXX1']['val']=trim(substr($line,$x=$x+$y,$y=  3));$lineArray['XXX1']['num']=true; //3 digit number
                                $lineArray['XXX2']['val']=trim(substr($line,$x=$x+$y,$y=  2));$lineArray['XXX2']['num']=false; //blank space
                                $lineArray['XXX3']['val']=trim(substr($line,$x=$x+$y,$y=  1));$lineArray['XXX3']['num']=true; //number
                                $lineArray['XXX4']['val']=trim(substr($line,$x=$x+$y,$y=  1));$lineArray['XXX4']['num']=false; //letter
                                $lineArray['XXX5']['val']=trim(substr($line,$x=$x+$y,$y=  1));$lineArray['XXX5']['num']=false;//blank space
                                $lineArray['PRO_DOLL']['val']=trim(substr($line,$x=$x+$y,$y=  4));$lineArray['PRO_DOLL']['num']=true;//blank space
				

                               // print_r($lineArray); //debug
				$this->dbInsert($lineArray);
				
				}}

				
}

///is it really blank?
function isItBlank($value) {
	if (trim($value)=='') {
		return 'null';
	}
	else {
		return $value;
	}
}

///is it really blank?
function isItBlankText($value) {
	if (trim($value)=='') {
		return '';
	}
	else {
		return $value;
	}
}


function dbInsert($lineArray) {


//correct any ones that are blank and replace with null.	
	foreach ($lineArray as $key => $value)
	{
		
		/* If it is suppose to be a number:
		   make it null if it is blank.
		*/
		if ($value['num']==true)
		{
			if (trim($value['val'])=='') 
			    {	
				        $lineArray[$key]['val']= 'null';
				}
				else {	
					    $lineArray[$key]['val']= $value['val']; 
				}
		}
		else {  //its text so escape it out.
			if (trim($value['val'])=='') {
				$lineArray[$key]['val']= 'null';
			}
			else { 	
				  $lineArray[$key]['val']= "'{$value['val']}'";   
			}
			
		}
	
    }

    /*print_r($lineArray);*/
	
$query="INSERT INTO batch.holstein_canada_data (
                                 client_id 
				,phone_number 
				,breed 
				,country 
				,sex 
				,reg_no 
				,name 
				,birth_date 
				,colour 
				,prov_eartag 
				,onfarm_eartag 
				,rfid 
				,chain_num 
				,common_name 
				,classification_country 
				,classification 
				,classification_score 
				,multiple_ex 
				,sire_breed 
				,sire_country 
				,sire_sex 
				,sire_reg_no 
				,sire_name 
				,dam_breed 
				,dam_country 
				,dam_sex 
				,dam_reg_no 
				,dam_name 
				,dam_classification_country 
				,dam_classification 
				,dam_classification_score 
				,dam_multiple_ex 
				,mgs_breed 
				,mgs_country 
				,mgs_sex 
				,mgs_reg_no 
				,mgs_name 
                ,prod_eval_date 
				,prod_type_of_proof 
				,prod_ebv_milk_kg
				,prod_ranking_milk 
				,prod_ebv_fat_kg 
				,prod_ranking_fat 
				,prod_percent_fat 
				,prod_protein_kg 
				,prod_ranking_protein 
				,prod_percent_protein 
				,prod_reliability 
				,prod_scs 
				,prod_scs_reliability 
				,prod_inbreeding_coeff 
				,prod_r_value
				,conf_eval_date 
				,conf_type_of_proof 
				,conf_ebv_conf 
				,conf_ranking 
				,conf_type_reliability 
				,conf_type_of_proof_scorecard 
				,conf_ebv_rp 
				,conf_ebv_ms 
				,conf_ebv_fl 
				,conf_ebv_ds 
				,conf_type_of_proof_secondary_traits 
				,conf_ebv_ps 
				,conf_ebv_ra 
				,conf_ebv_ra_code 
				,conf_ebv_pw 
				,conf_ebv_ls 
				,conf_ebv_ud 
				,conf_ebv_ud_code 
				,conf_ut 
				,conf_med 
				,conf_fa 
				,conf_ftp 
				,conf_ftp_code 
				,conf_rah 
				,conf_raw 
				,conf_rtp 
				,conf_rtp_code 
				,conf_tl 
				,conf_tl_code 
				,conf_ft 
				,conf_hd 
				,conf_bq 
				,conf_rl 
				,conf_rlsv 
				,conf_rlsv_code 
				,conf_rlrv 
				,conf_st 
				,conf_fe 
				,conf_cw 
				,conf_bd 
				,conf_angularity 
				,lpi 
				,lpi_ranking 
				,mgd_breed 
				,mgd_country 
				,mgd_sex 
				,mgd_reg_no 
				,mgd_name 
				,pgs_breed 
				,pgs_country 
				,pgs_sex 
				,pgs_reg_no 
				,pgs_name 
				,pgd_breed 
				,pgd_country 
				,pgd_sex 
				,pgd_reg_no 
				,pgd_name 
				,mggs_breed 
				,mggs_country 
				,mggs_sex 
				,mggs_reg_no 
				,mggs_name 
				,extract_date 
				,gebv
                                ,pro_doll
) VALUES (

                                 {$lineArray['CLIENT_ID']['val']} 
				,{$lineArray['PHONE_NUMBER']['val']} 
				,{$lineArray['BREED']['val']} 
				,{$lineArray['COUNTRY']['val']} 
				,{$lineArray['SEX']['val']} 
				,{$lineArray['REG_NO']['val']} /*number*/
				,{$lineArray['NAME']['val']} 
				,{$lineArray['BIRTH_DATE']['val']} 
				,{$lineArray['COLOUR']['val']} 
				,{$lineArray['PROV_EARTAG']['val']} /*number*/
				,{$lineArray['ONFARM_EARTAG']['val']} /*number*/
				,{$lineArray['RFID']['val']} /*number*/
				,{$lineArray['CHAIN_NUM']['val']} /*number*/
				,{$lineArray['COMMON_NAME']['val']} 
				,{$lineArray['CLASSIFICATION_COUNTRY']['val']} 
				,{$lineArray['CLASSIFICATION']['val']} 
				,{$lineArray['CLASSIFICATION_SCORE']['val']} /*number*/
				,{$lineArray['MULTIPLE_EX']['val']} /*number*/
				,{$lineArray['SIRE_BREED']['val']} 
				,{$lineArray['SIRE_COUNTRY']['val']} 
				,{$lineArray['SIRE_SEX']['val']} 
				,{$lineArray['SIRE_REG_NO']['val']} /*number*/
				,{$lineArray['SIRE_NAME']['val']} 
				,{$lineArray['DAM_BREED']['val']} 
				,{$lineArray['DAM_COUNTRY']['val']} 
				,{$lineArray['DAM_SEX']['val']} 
				,{$lineArray['DAM_REG_NO']['val']} /*number*/
				,{$lineArray['DAM_NAME']['val']} 
				,{$lineArray['DAM_CLASSIFICATION_COUNTRY']['val']} 
				,{$lineArray['DAM_CLASSIFICATION']['val']} 
				,{$lineArray['DAM_CLASSIFICATION_SCORE']['val']} /*number*/
				,{$lineArray['DAM_MULTIPLE_EX']['val']} /*number*/
				,{$lineArray['MGS_BREED']['val']} 
				,{$lineArray['MGS_COUNTRY']['val']} 
				,{$lineArray['MGS_SEX']['val']} 
				,{$lineArray['MGS_REG_NO']['val']} /*number*/
				,{$lineArray['MGS_NAME']['val']} 
                                ,{$lineArray['PROD_EVAL_DATE']['val']} 
				,{$lineArray['PROD_TYPE_OF_PROOF']['val']} /*number*/
				,{$lineArray['PROD_EBV_MILK_KG']['val']} /*number*/
				,{$lineArray['PROD_RANKING_MILK']['val']} /*number*/
				,{$lineArray['PROD_EBV_FAT_KG']['val']} /*number*/
				,{$lineArray['PROD_RANKING_FAT']['val']}  /*number*/
				,{$lineArray['PROD_PERCENT_FAT']['val']}  /*number*/
				,{$lineArray['PROD_PROTEIN_KG']['val']}  /*number*/
				,{$lineArray['PROD_RANKING_PROTEIN']['val']} /*number*/
				,{$lineArray['PROD_PERCENT_PROTEIN']['val']} /*number*/
				,{$lineArray['PROD_RELIABILITY']['val']} /*number*/
				,{$lineArray['PROD_SCS']['val']} /*number*/
				,{$lineArray['PROD_SCS_RELIABILITY']['val']} /*number*/
				,{$lineArray['PROD_INBREEDING_COEFF']['val']} /*number*/
				,{$lineArray['PROD_R_VALUE']['val']} /*number*/
				,{$lineArray['CONF_EVAL_DATE']['val']}
				,{$lineArray['CONF_TYPE_OF_PROOF']['val']} /*number*/
				,{$lineArray['CONF_EBV_CONF']['val']} /*number*/
				,{$lineArray['CONF_RANKING']['val']} /*number*/
				,{$lineArray['CONF_TYPE_RELIABILITY']['val']} 
				,{$lineArray['CONF_TYPE_OF_PROOF_SCORECARD']['val']} 
				,{$lineArray['CONF_EBV_RP']['val']} 
				,{$lineArray['CONF_EBV_MS']['val']} 
				,{$lineArray['CONF_EBV_FL']['val']} 
				,{$lineArray['CONF_EBV_DS']['val']} 
				,{$lineArray['CONF_TYPE_OF_PROOF_SECONDARY_TRAITS']['val']} 
				,{$lineArray['CONF_EBV_PS']['val']} 
				,{$lineArray['CONF_EBV_RA']['val']} 
				,{$lineArray['CONF_EBV_RA_CODE']['val']} 
				,{$lineArray['CONF_EBV_PW']['val']} 
				,{$lineArray['CONF_EBV_LS']['val']} 
				,{$lineArray['CONF_EBV_UD']['val']} 
				,{$lineArray['CONF_EBV_UD_CODE']['val']} 
				,{$lineArray['CONF_UT']['val']} 
				,{$lineArray['CONF_MED']['val']} 
				,{$lineArray['CONF_FA']['val']} 
				,{$lineArray['CONF_FTP']['val']} 
				,{$lineArray['CONF_FTP_CODE']['val']} 
				,{$lineArray['CONF_RAH']['val']} 
				,{$lineArray['CONF_RAW']['val']} 
				,{$lineArray['CONF_RTP']['val']} 
				,{$lineArray['CONF_RTP_CODE']['val']} 
				,{$lineArray['CONF_TL']['val']} 
				,{$lineArray['CONF_TL_CODE']['val']} 
				,{$lineArray['CONF_FT']['val']} 
				,{$lineArray['CONF_HD']['val']} 
				,{$lineArray['CONF_BQ']['val']} 
				,{$lineArray['CONF_RL']['val']} 
				,{$lineArray['CONF_RLSV']['val']} 
				,{$lineArray['CONF_RLSV_CODE']['val']} 
				,{$lineArray['CONF_RLRV']['val']} 
				,{$lineArray['CONF_ST']['val']} 
				,{$lineArray['CONF_FE']['val']} 
				,{$lineArray['CONF_CW']['val']} 
				,{$lineArray['CONF_BD']['val']} 
				,{$lineArray['CONF_ANGULARITY']['val']} 
				,{$lineArray['LPI']['val']} 
				,{$lineArray['LPI_RANKING']['val']} 
				,{$lineArray['MGD_BREED']['val']} 
				,{$lineArray['MGD_COUNTRY']['val']} 
				,{$lineArray['MGD_SEX']['val']} 
				,{$lineArray['MGD_REG_NO']['val']} 
				,{$lineArray['MGD_NAME']['val']} 
				,{$lineArray['PGS_BREED']['val']} 
				,{$lineArray['PGS_COUNTRY']['val']} 
				,{$lineArray['PGS_SEX']['val']} 
				,{$lineArray['PGS_REG_NO']['val']} 
				,{$lineArray['PGS_NAME']['val']} 
				,{$lineArray['PGD_BREED']['val']} 
				,{$lineArray['PGD_COUNTRY']['val']} 
				,{$lineArray['PGD_SEX']['val']} 
				,{$lineArray['PGD_REG_NO']['val']} 
				,{$lineArray['PGD_NAME']['val']} 
				,{$lineArray['MGGS_BREED']['val']} 
				,{$lineArray['MGGS_COUNTRY']['val']} 
				,{$lineArray['MGGS_SEX']['val']} 
				,{$lineArray['MGGS_REG_NO']['val']} 
				,{$lineArray['MGGS_NAME']['val']} 
				,{$lineArray['EXTRACT_DATE']['val']} 
				,{$lineArray['GEBV']['val']} 
                                ,{$lineArray['PRO_DOLL']['val']}     

)
";

    //check row is not already in DB, we only have to check reg_no and not whole number, because holstein canada only registers canadian animals. is this true?? assume false.
$sqlCheck="SELECT reg_no FROM batch.holstein_canada_data where breed={$lineArray['BREED']['val']} AND country={$lineArray['COUNTRY']['val']} AND sex={$lineArray['SEX']['val']} AND reg_no={$lineArray['REG_NO']['val']} AND extract_date={$lineArray['EXTRACT_DATE']['val']}";                                
   $res2 = $GLOBALS['pdo']->query($sqlCheck);
   if ($res2->rowCount() ==0) {
  //only insert when zero.                        
$res=$GLOBALS['pdo']->exec($query);
   }

}


}//end class

$_SERVER['DOCUMENT_ROOT']='/var/www/int';
//only run when called directly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
  $dFile=new HolCanHerdBookImport();
  $dFile->main();
}


/* Kepp here in case we ever need to make this table again */
/**
DROP TABLE batch.holstein_canada_data;
CREATE TABLE batch.holstein_canada_data
(
                 "client_id" varchar(20)
                ,"phone_number" varchar(16)
                ,"breed" varchar(2)
				,"country" varchar(3)
				,"sex" varchar(1)
				,"reg_no"  varchar( 12)
				,"name" varchar(  30)
				,"birth_date" date
				,"colour" varchar( 3)
				,"prov_eartag" bigint
				,"onfarm_eartag" bigint
				,"rfid" bigint
				,"chain_num" bigint
				,"common_name" varchar( 12 )
				,"classification_country" varchar( 3 )
				,"classification" varchar( 2)
				,"classification_score" bigint
				,"multiple_ex" bigint
				,"sire_breed" varchar(2)
				,"sire_country" varchar(3)
				,"sire_sex" varchar(1)
				,"sire_reg_no"  varchar( 12)
				,"sire_name" varchar(  30)
				,"dam_breed" varchar(2)
				,"dam_country" varchar(3)
				,"dam_sex" varchar(1)
				,"dam_reg_no"  varchar( 12)
				,"dam_name" varchar(  30)
				,"dam_classification_country" varchar( 3 )
				,"dam_classification" varchar( 2)
				,"dam_classification_score" bigint
				,"dam_multiple_ex" bigint
				,"mgs_breed" varchar(2)
				,"mgs_country" varchar(3)
				,"mgs_sex" varchar(1)
				,"mgs_reg_no"  varchar( 12)
				,"mgs_name" varchar(  30)
                ,"prod_eval_date" varchar( 6)
				,"prod_type_of_proof" bigint
				,"prod_ebv_milk_kg" bigint
				,"prod_ranking_milk" bigint
				,"prod_ebv_fat_kg" bigint
				,"prod_ranking_fat" bigint
				,"prod_percent_fat" numeric
				,"prod_protein_kg" bigint
				,"prod_ranking_protein" bigint
				,"prod_percent_protein" numeric
				,"prod_reliability" bigint
				,"prod_scs" numeric
				,"prod_scs_reliability" bigint
				,"prod_inbreeding_coeff" numeric
				,"prod_r_value" bigint
				,"conf_eval_date" varchar( 6)
				,"conf_type_of_proof" bigint
				,"conf_ebv_conf" bigint
				,"conf_ranking" bigint
				,"conf_type_reliability" bigint
				,"conf_type_of_proof_scorecard" bigint
				,"conf_ebv_rp" bigint
				,"conf_ebv_ms" bigint
				,"conf_ebv_fl" bigint
				,"conf_ebv_ds" bigint
				,"conf_type_of_proof_secondary_traits" bigint
				,"conf_ebv_ps" bigint
				,"conf_ebv_ra" bigint
				,"conf_ebv_ra_code" varchar(1 )
				,"conf_ebv_pw" bigint
				,"conf_ebv_ls" bigint
				,"conf_ebv_ud" bigint
				,"conf_ebv_ud_code" varchar(1 )
				,"conf_ut" bigint
				,"conf_med" bigint
				,"conf_fa" bigint
				,"conf_ftp" bigint
				,"conf_ftp_code" varchar( 1)
				,"conf_rah" bigint
				,"conf_raw" bigint
				,"conf_rtp" bigint
				,"conf_rtp_code" varchar( 1)
				,"conf_tl" bigint
				,"conf_tl_code" varchar( 1)
				,"conf_ft" bigint
				,"conf_hd" bigint
				,"conf_bq" bigint
				,"conf_rl" bigint
				,"conf_rlsv" bigint
				,"conf_rlsv_code" varchar( 1)
				,"conf_rlrv" bigint
				,"conf_st" bigint
				,"conf_fe" bigint
				,"conf_cw" bigint
				,"conf_bd" bigint
				,"conf_angularity" bigint
				,"lpi" bigint
				,"lpi_ranking" bigint
				,"mgd_breed" varchar(2)
				,"mgd_country" varchar(3)
				,"mgd_sex" varchar(1)
				,"mgd_reg_no"  varchar( 12)
				,"mgd_name" varchar(  30)
				,"pgs_breed" varchar(2)
				,"pgs_country" varchar(3)
				,"pgs_sex" varchar(1)
				,"pgs_reg_no"  varchar( 12)
				,"pgs_name" varchar(  30)
				,"pgd_breed" varchar(2)
				,"pgd_country" varchar(3)
				,"pgd_sex" varchar(1)
				,"pgd_reg_no"  varchar( 12)
				,"pgd_name" varchar(  30)
				,"mggs_breed" varchar(2)
				,"mggs_country" varchar(3)
				,"mggs_sex" varchar(1)
				,"mggs_reg_no"  varchar( 12)
				,"mggs_name" varchar(  30)
				,"extract_date" date
				,"gebv" varchar(  1)
			);


*/
?>