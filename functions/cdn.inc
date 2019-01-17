<?php 
/**
* functions for dealing with cdn website. 
*
*/


//return an array of animal,sire,dam basic info
function getBovineCDNData($regNum) {

//break up the reg number.
$regArray=$GLOBALS['MiscObj']->breakUpFullRegNumber($regNum);

$link= "https://www.cdn.ca/query/detail_ge.php?breed={$regArray['breed']}&country={$regArray['country']}&sex={$regArray['sex']}&regnum={$regArray['number']}";
//print($link."\n\r");
//grab the weather data for the farm weather station from weather underground
$ch = curl_init($link);
if (! $ch) {
	die( "Cannot allocate a new PHP-CURL handle" );
}

// some curl option to do with how data is returned.
curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt ($ch, CURLOPT_TIMEOUT, 60); 

// Grab the page and save the contents in the $data variable
$data = curl_exec($ch);
// close the connection
curl_close($ch);

//first do a basic check that this is an actual animal info page, if it is not excp.
$strToMatch='<div class="AnimalDetails">';
$a=strstr($data,$strToMatch);
if (strstr($data,$strToMatch) == false) {
throw new Exception("CURL CDN website parse error (The animal $regNum is not found on CDN, do a manual check of CDN site).");
}



/* //skip lpi because young calves don't have it calculated yet.
//parse for bovine lpi string.
ereg('<td style="width: 60px; text-align: right; font-weight: bold;">[^<]*</td>',$data,$regs);
$t1=(strip_tags($regs[0]));
$lpi=trim($t1);

if (is_numeric($lpi) == false) {
throw new Exception('CURL CDN website parse error (LPI is not numeric).');
}
*/

//parse for bovines full name
ereg('<td class="animalHeaderSubTitle" style="width: 155px;">'.ucwords($regNum).'</td>[^<]*<td class="animalHeaderSubTitle" colspan=2>[^<]*</td>',$data,$regs3); //narrow search
$p1=(strip_tags($regs3[0])); //strip out html
$p2=trim(str_replace(ucwords($regNum),'',$p1)); //remove the regnumber that was just used to find the right place.
$bovineFullName=$p2;

//now find dams full reg number
ereg('<tr>[^<]*<td>Dam:</td>[^<]*<td><a href="[^<]*">[^<]*</a></td>[^<]*<td>[^<]*</td>',$data,$regs4);
$q1=(strip_tags($regs4[0])); //strip out html
$q2=trim(str_replace('Dam:','',$q1)); //strip out Dam: text
$q3=trim(substr($q2,0,(strpos($q2,"\n")))); //strip out dams reg number.
$damRegNum=$q3;

if ($GLOBALS['MiscObj']->breakUpFullRegNumber($damRegNum) == false) {
throw new Exception('CURL CDN website parse error (Dam Reg Number is not valid.');
}

//now find dams full name using data from prev
$q4=trim(strstr($q2,"\n")); //strip out dams reg number.
$damFullName=$q4;

if ((preg_match('#[0-9]#',$damFullName))==true){ 
throw new Exception('CURL CDN website parse error (Dam Name has numbers in it, thus not valid.');
}

//find sire full reg number
ereg('<tr>[^<]*<td>Sire:</td>[^<]*<td><a href="[^<]*">[^<]*</a></td>[^<]*<td>[^<]*</td>',$data,$regs5);
$r1=(strip_tags($regs5[0])); //strip out html
$r2=trim(str_replace('Sire:','',$r1)); //strip out sire: text
$r3=trim(substr($r2,0,(strpos($r2,"\n")))); //strip out sires reg number.
$sireRegNum=$r3;

if ($GLOBALS['MiscObj']->breakUpFullRegNumber($sireRegNum) == false) {
throw new Exception('CURL CDN website parse error (Sire Reg Number is not valid.');
}

//now find sires full name using data from prev
$r4=trim(strstr($r2,"\n")); //strip out sires reg number.
$sireFullName=$r4;

if ((preg_match('#[0-9]#',$sireFullName))==true){ 
throw new Exception('CURL CDN website parse error (Sire Name has numbers in it, thus not valid.');
}


//find bovine birth date
if ($regArray['sex']=='F') { ereg('<td>[^<]*Born[^<]*</td>',$data,$regs6);} //differnt html for male and female.
else { ereg('<td style="text-align: right;">[^<]*Born[^<]*</td>',$data,$regs6); }

$s1=(strip_tags($regs6[0])); //strip out html
$s2=trim(str_replace('Born','',$s1)); //strip out Born text
$s3=trim(str_replace('&nbsp;','',$s2)); //strip out &nbsp; text
$timestampStr=$s3;
if (($timestamp = strtotime($timestampStr)) === false) {
throw new Exception("CURL CDN website parse error (Bovine Birth Date is bogus: $timestampStr.");
} else {
    $bovine_birthdate=$timestampStr;
}

//some differences for males and females.
if ($regArray['sex']=='M') {
$cArray['bovine_full_name']=findMaleFullName($data);
$cArray['bovine_short_name']=findMaleShortName($data);
}
else {
$cArray['bovine_full_name']=$bovineFullName; 
}  

//make storage array
$cArray['bovine_full_reg_number']=$regNum;
$cArray['bovine_birthdate']=$bovine_birthdate;
//$cArray['bovine_lpi']=$lpi;
// 
$cArray['dam_full_reg_number']=$damRegNum;
$cArray['dam_full_name']=$damFullName;
$cArray['sire_full_reg_number']=$sireRegNum;
$cArray['sire_full_name']=$sireFullName;

//return the output array
return $cArray;
}


// find bulls full name.
function findMaleFullName($data) {
  //bulls long name
		if (ereg('<td colspan=2 class="animalHeaderSubTitle">[^<]*</td>',$data,$regs)) {
			$fullName=strip_tags($regs[0]);
		}
		else { $fullName=null; }
		return $fullName;
}

// find bulls short name.
function findMaleShortName($data) {

		//short bull name
		if (ereg('<td colspan=2 class="animalHeaderSubTitle" style="text-align: right;">[^<]*</td>',$data,$regs)) {
			$shortName=strip_tags($regs[0]);
		}
		else { $shortName=null; }
		return $shortName;
}  


?>