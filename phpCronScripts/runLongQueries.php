<?php 
require_once('../global.php');

/*
//show errors
error_reporting(E_ALL);
ini_set('display_errors', 1);
*/
//run various classes. 
include_once('processNumberMilkingByDate.php');
include_once('processValactaAvgComponentsForGroups.php');
include_once('processAlproProductionForGroups.php');
include_once('provincialMilkTestData.php');
include_once('commodityReportData.php');
include_once('holsteinCanadaRegisteredBatch.php');
include_once('processEmployeeShift.php');
include_once('insertHolsteinCanadaFamilyTree.php');
include_once('processMaterializedViews.php');
?>
