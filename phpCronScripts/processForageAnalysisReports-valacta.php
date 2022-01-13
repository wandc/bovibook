<?php 
/**
File to read valacta feed analysis reports and store in db 
*/
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');

class ForageAnalysisParse {

	public function __construct() {
		//run main function
		$this->doForageProcessing();
	}

	

	
	function doForageProcessing()
	{


//load all the adlic files in the directory.
		$filepath="../local/dataFile/forageAnalysis/valacta/"; //hardcoded....bad.
		if ($handle = opendir($filepath)) {
			/* This is the correct way to loop over the directory. */
			while (false !== ($file = readdir($handle))) {
				if (strpos($file,'.pdf') !==false) {
					$arrayOfFiles[]=$file;
				}
			}
			closedir($handle);
		}

		print("Doing the following files:\n");
		//print_r($arrayOfFiles);

		//read file
		foreach ($arrayOfFiles as $arrayOfFiles_num => $fileName)
		{


//DEBUG:$pdfFileName='97593-0929.pdf';
$pdfFileName=$filepath.'"'.$fileName.'"';
print("File:$pdfFileName"."\n\r");


//go through page numbers until we get a stop signal.
$stop='go';
$pageNo=1;
while ($stop != 'stop') {

  
try {  
  $stop=self::processOnePage($pdfFileName,$pageNo,$pageNo);
} catch (Exception $e) {
    echo "SKIPPING PAGE, Caught EXCEPTION for file $pdfFileName on page $pageNo: ",  $e->getMessage(), "\n";
}  


$pageNo++;
}

		}//filename loop.


	} //end function


function processOnePage($pdfFileName,$startPage,$endPage) {


//now run unix poppler utility to convert pdf to text. http://poppler.freedesktop.org
$retVal=exec("pdftotext $pdfFileName -layout -f $startPage -l $endPage -",$output);

//look for sample no to see if this is a valid feed analysis page. if it is, exit and tell function to stop
preg_match("/Sample no\s*(?<sampleno>(\d*))/",implode("\n\r",$output),$matches);
if (isset($matches['sampleno']) !=true)      { print("skipping page, no data found"."\n\r");  return 'stop'; }
if (is_numeric($matches['sampleno']) !=true) { print("skipping page, no data found"."\n\r");  return 'stop'; }


//get sample no.
preg_match("/Sample no\s*(?<sampleno>(\d*))/",implode("\n\r",$output),$matches);
$rowArray2['Sample no']= array('answer' =>$matches['sampleno']);
//get sample date.

//to get around a bug in one thath has french text.
preg_match("/Analysis date\s*(?<date>(\d\d\d\d-\d\d-\d\d))/",implode("\n\r",$output),$matches);
if (isset($matches['date'])==false) {
preg_match("/Date d'analyse\s*(?<date>(\d\d\d\d-\d\d-\d\d))/",implode("\n\r",$output),$matches);
}

if (isset($matches['date'])==true) {
$rowArray2['Analysis date']= array('answer' =>$matches['date']);
}
else {
throw new Exception("Analysis date not found for page $startPage."."\n\r"); 
}
 


//get sample identification, the x20 stuff says get all visible characters
preg_match("/Identification\s*(?<identification>([\x20-\x7E]*))/",implode("\n\r",$output),$matches);
$rowArray2['Identification']= array('answer' =>$matches['identification']);


//print(implode("\n\r",$output)); //print out read file.

//when its a grass tesxt use this.
preg_match("/Sample type\s*(?<sampletype>([\x20-\x7E]*))/",implode("\n\r",$output),$matches);



//choose which template to use based on sample type.
if ($matches['sampletype'] == 'Fermented Corn Silage') {
$rowArray=self::cornSilageTemplate();
}
elseif  ($matches['sampletype'] == 'Haylage') {
$rowArray=self::grassSilageTemplate();
}
else {
throw new Exception("No template found for sample type: {$matches['sampletype']}."."\n\r"); 
}




foreach ($rowArray as $key => $row)
{
//key is text
$rowArray[$key]['answer']=self::getAnalysisNumbers($row['text'],implode("\n\r",$output),$row['numberColumn']);
//DEBUG: print("$key : {$rowArray[$key]['answer']}\n\r");
}


// Open a transaction
try {$res = $GLOBALS['pdo']->beginTransaction();


//use sample number to see if this forage anaylsis has already been inserted into the db, if so just skip it.
$sql="SELECT sample_number FROM nutrition.feed_analysis WHERE sample_number='{$rowArray2['Sample no']['answer']}'";
$res = $GLOBALS['pdo']->query($sql);
$row = $res->fetch(PDO::FETCH_ASSOC);

if ($row ==null) {

$comment=pg_escape_string($rowArray2['Identification']['answer']);

//now insert into db.
$sql='INSERT INTO nutrition.feed_analysis (bag_id,footage,sample_number,sample_date,comment,"DM","pH","CP","SP","ADFCP","NDFCP","ADF","NDF","lignin","NFC","starch","CF","TDN","DE","Ca","P","Mg","K","Na","ash",userid) VALUES ('."
null,null,{$rowArray2['Sample no']['answer']},'{$rowArray2['Analysis date']['answer']}','$comment',
{$rowArray['DryMatter']['answer']},
{$rowArray['pH']['answer']},
{$rowArray['CrudeProtein']['answer']},
{$rowArray['SolubleProtein']['answer']},
{$rowArray['ADFCP']['answer']},
{$rowArray['NDFCP']['answer']},
{$rowArray['ADF']['answer']},
{$rowArray['NDF']['answer']},
{$rowArray['Lignin']['answer']},
{$rowArray['Non-fibreCarbohydrates']['answer']},
{$rowArray['Starch']['answer']},
{$rowArray['CrudeFat']['answer']},
{$rowArray['TDN']['answer']},
{$rowArray['DE']['answer']},
{$rowArray['Calcium']['answer']},
{$rowArray['Phosphorus']['answer']},
{$rowArray['Magnesium']['answer']},
{$rowArray['Potassium']['answer']},
{$rowArray['Sodium']['answer']},
{$rowArray['Ash']['answer']},
'david'
)";
$res=$GLOBALS['pdo']->exec($sql);

}


// determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                 echo "Failed: " . $e->getMessage(); error_log( $e->getMessage(), 0);
            }
// Close the transaction


} //end function

/** the person at the Valacta lab cuts and pastes her test results into different templates. Corn Silage and Garss Legume silage are different, there are probably more templates. */
function grassSilageTemplate()
{

$rowArray['DryMatter']=array('text' => 'Dry Matter   ,%' , 'numberColumn' =>2);
$rowArray['pH']=array('text' => 'pH' , 'numberColumn' =>1); 
$rowArray['CrudeProtein']=array('text' =>  'Crude Protein,%', 'numberColumn' =>1); 
$rowArray['SolubleProtein']=array('text' => 'Soluble Protein, %' , 'numberColumn' =>1); 
$rowArray['ADFCP']=array('text' => 'ADF-CP,%' , 'numberColumn' =>1); 
$rowArray['NDFCP']=array('text' => 'NDF-CP,%' , 'numberColumn' =>1); 
$rowArray['ADF']=array('text' =>  'ADF,%', 'numberColumn' =>1); 
$rowArray['NDF']=array('text' =>  'NDF,%', 'numberColumn' =>1); 
$rowArray['Lignin']=array('text' =>  'Lignin, %', 'numberColumn' =>1); 
$rowArray['Non-fibreCarbohydrates']=array('text' => 'Non-fibre Carbohydrates,%' , 'numberColumn' =>1); 
$rowArray['CrudeFat']=array('text' => 'Crude Fat, %' , 'numberColumn' =>1); 
$rowArray['NEL']=array('text' => 'NEL,Mcal/kg (CPAQ 1990)' , 'numberColumn' =>1); 
$rowArray['TDN']=array('text' => 'TDN 1X, % (NRC 2001)' , 'numberColumn' =>1); 
$rowArray['DE']=array('text' => 'DE 1X, % (NRC 2001)' , 'numberColumn' =>1); 
$rowArray['NEL3x']=array('text' => 'NEL 3X,Mcal/kg (NRC 2001)' , 'numberColumn' =>1); 
$rowArray['NEM3x']=array('text' => 'NEM 3X Mcal/kg (NRC 2001)' , 'numberColumn' =>1); 
$rowArray['NEG3x']=array('text' =>  'NEG 3X Mcal/kg (NRC 2001)', 'numberColumn' =>1); 
$rowArray['Calcium']=array('text' => 'Calcium (Ca),%' , 'numberColumn' =>1); 
$rowArray['Phosphorus']=array('text' => 'Phosphorus (P),%' , 'numberColumn' =>1); 
$rowArray['Magnesium']=array('text' => 'Magnesium (Mg),%' , 'numberColumn' =>1); 
$rowArray['Potassium']=array('text' =>  'Potassium (K),%', 'numberColumn' =>1); 
$rowArray['Sodium']=array('text' => 'Sodium (Na),%' , 'numberColumn' =>1); 
$rowArray['Ash']=array('text' => 'Ash, %' , 'numberColumn' =>1); 




return $rowArray;
}

function cornSilageTemplate()
{

$rowArray['DryMatter']=array('text' => 'Dry matter,%' , 'numberColumn' =>2);
$rowArray['pH']=array('text' => 'pH' , 'numberColumn' =>1); 
$rowArray['CrudeProtein']=array('text' =>  'Crude protein,%', 'numberColumn' =>1); 
$rowArray['SolubleProtein']=array('text' => 'Soluble protein, %' , 'numberColumn' =>1); 
$rowArray['ADFCP']=array('text' => 'ADF-CP,%' , 'numberColumn' =>1); 
$rowArray['NDFCP']=array('text' => 'NDF-CP,%' , 'numberColumn' =>1); 
$rowArray['ADF']=array('text' =>  'ADF,%', 'numberColumn' =>1); 
$rowArray['NDF']=array('text' =>  'NDF,%', 'numberColumn' =>1); 
$rowArray['Lignin']=array('text' =>  'Lignin, %', 'numberColumn' =>1); 
$rowArray['Non-fibreCarbohydrates']=array('text' => 'Non-fibre carbohydrates,%' , 'numberColumn' =>1); 
$rowArray['Starch']=array('text' => 'Starch, %' , 'numberColumn' =>1); 
$rowArray['CrudeFat']=array('text' => 'Crude fat, %' , 'numberColumn' =>1); 
$rowArray['TDN']=array('text' => 'TDN 1X, % (NRC 2001)' , 'numberColumn' =>1); 
$rowArray['DE']=array('text' => 'DE 1X, % (NRC 2001)' , 'numberColumn' =>1);
$rowArray['NEM3x']=array('text' => 'NEM Mcal/kg (NRC 2001)' , 'numberColumn' =>1); 
$rowArray['NEG3x']=array('text' =>  'NEG Mcal/kg (NRC 2001)', 'numberColumn' =>1); 
$rowArray['NEL']=array('text' => 'NEL,Mcal/kg (CPAQ 1990)' , 'numberColumn' =>1); 
$rowArray['Calcium']=array('text' => 'Calcium (Ca),%' , 'numberColumn' =>1); 
$rowArray['Phosphorus']=array('text' => 'Phosphorus (P),%' , 'numberColumn' =>1); 
$rowArray['Magnesium']=array('text' => 'Magnesium (Mg),%' , 'numberColumn' =>1); 
$rowArray['Potassium']=array('text' =>  'Potassium (K),%', 'numberColumn' =>1); 
$rowArray['Sodium']=array('text' => 'Sodium (Na),%' , 'numberColumn' =>1); 
$rowArray['Ash']=array('text' => 'Ash, %' , 'numberColumn' =>1); 

return $rowArray;
}



function getAnalysisNumbers($needleText,$haystackText,$numberOneOrTwo)
{
//define 
$matches['digit1']='';
$matches['digit2']=null;

//(\b[0-9]+\.([0-9]+\b)) matchs a floating point number.

//escape the string for slashes and brackets.
$needleText=preg_quote($needleText,'/');

if ($numberOneOrTwo==1) {
preg_match("/$needleText\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*/",$haystackText,$matches);

if ((isset($matches['digit1']) ==true) && (is_numeric($matches['digit1']) == true)) {
$num=$matches['digit1'];
}
else {
throw new Exception("No number read for row $needleText"."\n\r"); 
}

return $num;
}


elseif ($numberOneOrTwo==2) {
preg_match("/$needleText\s*(?<digit1>(\b[0-9]+\.([0-9]+\b)))\s*(?<digit2>(\b[0-9]+\.([0-9]+\b)))/",$haystackText,$matches);

if ((isset($matches['digit2']) ==true) && (is_numeric($matches['digit2']) == true)) {
$num=$matches['digit2'];
}
else {
throw new Exception("No number read for row $needleText"."\n\r");
}

return $num;
}

else {
throw new Exception("Number one or two not specified for forage analysis row"."\n\r"); 
}

}


} //end class

if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
   new ForageAnalysisParse(); //run class
}


?>