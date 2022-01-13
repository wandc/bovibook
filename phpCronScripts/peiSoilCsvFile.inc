<?php
require_once('../global.php');
/*
 * Processes pei soil sample csv files reading from DB.
 */

class PeiSoilCsvFile {

   

    public function __construct() {
        //run main function
        $this->main();
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        //nothing
    }

    //main function that processes old and new files.
    function main() {

        ini_set("memory_limit", "512M"); //for some reason this code needs more than 33MB of ram to run...Easier just to be sloppy unfortunately.
        set_time_limit(30); //add 30 seconds to time out counter, to give time to script to complete. This allo
        //    
            //then new
            $this->doPeiSoilCsvProcessing();

        print("<h3>All done!</h3>");
    }

    //in 2017 we switched to XML files for valvacta old flat format, so use that from now on.
    function doPeiSoilCsvProcessing() {
        print("Doing the following CSV files:\n");

        $sql = "SELECT id,data::bytea,file_name FROM documents.pei_soil_csv ORDER BY file_name DESC";
        $stmt = $GLOBALS['pdo']->prepare($sql);
        $stmt->execute();

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $csvStr, PDO::PARAM_LOB);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);


        while ($row = $stmt->fetch(PDO::FETCH_BOUND)) {

            print("CSV File $fileName <br/>\n");
       //  print(pg_unescape_bytea(stream_get_contents($data)));
            
   //take csv and put into assocative array with headers          
   //from: https://gist.github.com/benbalter/3173096  

            if (!empty($csvStr)) {
$lines = explode( "\n", pg_unescape_bytea(stream_get_contents($csvStr)) );

$headers = str_getcsv( array_shift( $lines ) );

foreach ( $lines as $line ) {
	$rowX = array();
	foreach ( str_getcsv( $line ) as $key => $field )
		$rowX[ $headers[ $key ] ] = $field;
	$rowX = array_filter( $rowX );
	$data[] = $rowX;
}}
 //print("Dumping:");  //DEBUG         
 //var_dump($data);


//now figure out what we need. 
//map my db to csv names.
//
$out=array();
foreach ($data as $key => $value) {
    if (!empty($value)) {
    
//print_r($value);
  print("HI\n\r");      
    //attempt to match field name on report to one of our fields. NOTE that this uses the current state (fieldcurr) and will mismatch old fields.
$sql = <<<SQL
SELECT id, border_datum_id FROM cropping.fieldcurr WHERE alpha_numeric_id ILIKE '{$value['FIELD_ID']}' limit 1
SQL;

$res = $GLOBALS['pdo']->query($sql);
$row = $res->fetch(PDO::FETCH_ASSOC);
if (empty($row['id'])) {
    try {
    $out['field_id']='null';
     $out['datum_id']='null';	
    throw new exception("Error field name cannnot be matched for {$value['FIELD_ID']}\n\r");
    } catch (Exception $e) {print("Caught Exception $e");}
}
else {
     $out['field_id']=$row['id'];
     $out['datum_id']=$row['border_datum_id'];	 

       
    $out['userid']='system'; //system userid for insert via script.	
    $out['event_time']=date('Y-m-d',strtotime($value['DATE_SAMPL']));	
    $out['report_date']=date('Y-m-d',strtotime($value['Date_RECD']));	
    $out['ph']=$value['WATER_PH'];	
    $out['cec']=$value['CEC'];	
    $out['phosphorus']=round($value['P2O5_RES'] * 0.4364);	//convernt to P /conversion factors to correct for PEI lab reporting in phosphate and potash
    $out['potassium']=round($value['K2O_RES'] * 0.8301);	//convert to K
    $out['calcium']=$value['CA_RES'];	
    $out['magnesium']=$value['MG_RES'];	
    $out['boron']=$value['B_RES'];
    $out['copper']=$value['CU_RES'];	 
    $out['zinc']=$value['ZN_RES'];	 
    $out['sulphur']=$value['S_RES'];	 
    $out['manganese']=$value['MN_RES'];	 
    $out['iron']=$value['FE_RES'];	 
    $out['aluminum']=$value['AL_RES'];	 
    $out['sodium']=$value['NA_RES'];	 
    $out['buffer_ph']=$value['Lime_Index'];
    $out['organic_matter']=$value['ORG_MTR_RS'];
  
    $this->doInsert($out);   
}


 }}}//end main foreach

 
    
    
//end file loop
            //var_dump($data);
            

            print("File $fileName CSV is done.<br/><br/>\n");
        }
    

function doInsert(array $out)  {
    //print_r($out);

//low probability of failure way to check if already in DB.    
$sql2 = <<<SQL
SELECT id FROM cropping.soil_sample_event WHERE field_id={$out['field_id']} AND magnesium={$out['magnesium']} AND aluminum={$out['aluminum']} AND calcium={$out['calcium']} AND date_part('year','{$out['report_date']}'::date)=date_part('year',report_date) limit 1
SQL;
print($sql2);
$res2 = $GLOBALS['pdo']->query($sql2);
//when empty this is not already in the DB, thus do insert.
 if ($res2->fetchColumn() == 0) {
  
    
$columns=array_keys($out);
$column_list = join(',', $columns);
$param_list = join(',', array_map(function($col) { return ":$col"; }, $columns));
$param_values = array_intersect_key($out, array_flip($columns));

$sqlInsert = <<<SQL
INSERT INTO cropping.soil_sample_event ($column_list) VALUES ($param_list)
SQL;



print($sqlInsert);

   
                        $stmt = $GLOBALS['pdo']->prepare($sqlInsert);
                        $stmt->bindValue(':datum_id', $out['datum_id'], PDO::PARAM_INT);
                        $stmt->bindValue(':userid', $out['userid'], PDO::PARAM_STR);
                        $stmt->bindValue(':event_time', $out['event_time'], PDO::PARAM_STR);
                        $stmt->bindValue(':report_date', $out['report_date'], PDO::PARAM_STR);
                        $stmt->bindValue(':ph', $out['ph'], PDO::PARAM_STR);
                        $stmt->bindValue(':cec', $out['cec'], PDO::PARAM_STR);
                        $stmt->bindValue(':phosphorus', $out['phosphorus'], PDO::PARAM_STR);
                        $stmt->bindValue(':potassium', $out['potassium'], PDO::PARAM_STR);
                        $stmt->bindValue(':calcium', $out['calcium'], PDO::PARAM_STR);
                        $stmt->bindValue(':magnesium', $out['magnesium'], PDO::PARAM_STR);
                        $stmt->bindValue(':boron', $out['boron'], PDO::PARAM_STR);
                        $stmt->bindValue(':copper', $out['copper'], PDO::PARAM_STR);
                        $stmt->bindValue(':zinc', $out['zinc'], PDO::PARAM_STR);
                        $stmt->bindValue(':sulphur', $out['sulphur'], PDO::PARAM_STR);
                        $stmt->bindValue(':manganese', $out['manganese'], PDO::PARAM_STR);
                        $stmt->bindValue(':iron', $out['iron'], PDO::PARAM_STR);
                        $stmt->bindValue(':aluminum', $out['aluminum'], PDO::PARAM_STR);
                        $stmt->bindValue(':sodium', $out['sodium'], PDO::PARAM_STR);
                        $stmt->bindValue(':buffer_ph', $out['buffer_ph'], PDO::PARAM_STR);
                        $stmt->bindValue(':organic_matter', $out['organic_matter'], PDO::PARAM_STR);



                        $status= $stmt->execute($param_values);
   if ($status === false) {
  die(print_r($statement->errorInfo(), true));
}
}}

}
//end class

new PeiSoilCsvFile();  //initiate class (run it).

?>