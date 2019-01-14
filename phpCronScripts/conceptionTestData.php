<?php 
require_once('../global.php');
/**
* Reads conception test result emails
*
*/

echo("<h1>Started processing email messages.</h1>\n");


//login to imap server
$mbox = imap_open ("{imap.gmail.com:993/imap/ssl/novalidate-cert}INBOX", "conceptiontest@littleriver.ca", "st2u9wleswie")
     or die("can't connect: " . imap_last_error());




//get list of email headers.
$headers = imap_headers($mbox);
if ($headers == false) {
    echo "Call failed<br />\n";
} else {
$count=0;

    foreach ($headers as $val) {
$count++;
    //echo $val . "<br />\n";
$body=imap_body($mbox, $count );
$infoArray[$count]=getAllInformationFromEmail($body); //grab info from email and store in array
    }
}

//store in DB.
//storeInfoInDB($infoArray);


echo("<h1>Finished processing email messages.</h1>\n");



///////////////////////////////////////////////////////


function getAllInformationFromEmail($body) {

//start parsing the email for information
$dateOfTest=getDatePeriod($body);
$testResultArray=getTestResults($body);

}


function getTestResults($messageBody)
{
$bodyLC=strtolower($messageBody);//make it lowercase to be sure.
//clean out html spaces.
$bodyLCTrim=str_replace(" &nbsp;","",$bodyLC); //get rid of html spaces

print("$bodyLCTrim");

preg_match("/tube animal result comments*(?<digit>\d*)/",$messageBody,$matches);
$number=$matches['digit'];
print("GGGGGG:::");
print($matches['digit']);
print("HHHHHH:::");
print_r($matches);
}

//returns date the test result was processed by the testing company.
function getDatePeriod($messageBody)
{
$bodyLC=strtolower($messageBody);//make it lowercase to be sure.
$needleStr='Date : '; //find start of this string
$needleLength=strlen($needleStr); //needed to add the lngth of needle string to search.
$offset=strripos($bodyLC,$needleStr);
$correctedOffset=$offset+$needleLength+0;
$period=substr( $bodyLC , $correctedOffset ,10);

//print($messageBody);
//print("XX:$period");

return $period;
/*

$bodyLC=strtolower($messageBody);//make it lowercase to be sure.

//clean out html spaces.
$bodyLCTrim=str_replace(" &nbsp;","",$bodyLC); //get rid of html spaces


//check that it is a number.
//should be one of 1,2,3,4
if ($period==(1 || 2 || 3 || 4)) {
return $period;
}
else {
throw new Exception('PARSE ERROR: Milk Test Date Period should be 1,2,3,4.');
}
*/
}

?>