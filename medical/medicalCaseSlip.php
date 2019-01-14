<?php 
print('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">');
print('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">');

require_once($_SERVER['DOCUMENT_ROOT'].'global.php');
require_once($_SERVER['DOCUMENT_ROOT'].'secure.php');
require_once($_SERVER['DOCUMENT_ROOT'].'/functions/misc.inc');
require_once ($_SERVER ['DOCUMENT_ROOT'] . 'template/basePage.php');
require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/medical/medicalCase.inc');

print('<head>');
print('<title>LRH - Medical Case Summary</title>');
print('<meta http-equiv="content-type" content="text/html; charset=iso-8859-1"></meta>');
print('<link rel="icon" type="image/png" href="/images/favicon.png">'); //favicon code
print('<meta name="description" content=""/>');
print('<link rel="stylesheet" type="text/css" media="all" href="/css/XXX.css" />');
print('</head>');
print('<body>');

$medCaseID=$_REQUEST['case_id'];

$res = $GLOBALS['pdo']->query("SELECT medical_case.id,bovine_id,open_date,close_date,bovinecurr.full_name, bovinecurr.local_number, bovinecurr.location_name, date_trunc('day',current_date-bovinecurr.fresh_date) as dim FROM bovinemanagement.medical_case LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.id=medical_case.bovine_id WHERE medical_case.id=$medCaseID");

$row = $res->fetch(PDO::FETCH_ASSOC);


///////
include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
	    $sql = "
    (SELECT id,event_time,type,text,userid,bovine_id FROM bovinemanagement.medical_summary WHERE medical_summary.medical_case_id={$row['id']})
      UNION
    (SELECT id,event_time,type,text,userid,bovine_id FROM bovinemanagement.medical_summary WHERE medical_summary.bovine_id={$row['bovine_id']} AND type='Scheduled Meds')
      ORDER BY event_time DESC ,type ASC 
   ";
           $dg=new DataGridLR($sql,1000);
           $dg->datagrid->addColumn(new Structures_DataGrid_Column('All', null, '', array('' => ''), null, array('MedicalCase', 'printPaticularTreatmentInfo()')));
           //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Action', null, '', array('' => ''), null, array('MedicalCase', 'printActionButtons()')));
           //$dg->datagrid->addColumn(new Structures_DataGrid_Column('Action2', null, '', array('' => ''), null, array('MedicineScheduled', 'printActionButtons2()')));
           $str = '<div id=\'scrollable\'>' . $dg->render('datagrid datagridNested') . '</div>'; // wrap in
           print($str);
    
/////////    






print("<h1>Medical Case # {$row['id']}</h1>");
print("<h3>Breed </h2>");
print("<h2>{$row['local_number']} - {$row['cow_full_name']}</h2>");
print("<h3>to: </h2>");
print("<h2>{$row['sire_full_name']} - {$row['sire_full_reg_number']}</h2>");
print("<br>");
print("<br>");
print("<h2>Location:{$row['location_name']}</h2>");
print("<br>");
print("<h2>Tank: A</h2>");
print("<h2>Bin: {$row['bin']}</h2>");
print("<h2>Code: {$row['semen_code']}</h2>");
print("<h2>Freeze Date: {$row['freeze_date']}</h2>");
print("<br>");
print("<h4>Breeding unique id: {$row['breeding_event_id']}</h4>");
print('</body>');
print('</html>');


?>