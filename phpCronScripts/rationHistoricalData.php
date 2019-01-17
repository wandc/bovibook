<?php 
/**
 * This should be run every week or so, to store historical ration information in
 * a batch table. Used for calculating bag densities, etc. Nov 11, 2013.
 * 
 * 
 *  
 */
if (defined('STDIN')) { //when called from cli, command line
include_once('../global.php');
include_once('../functions/misc.inc');
}

echo("starting ration historical");


/*
$sql="INSERT INTO batch.day_ingredient_usage_result SELECT * from nutrition.day_ingredient_usage('2011-01-01')";
$res = $GLOBALS['pdo']->exec($sql);
*/


$start = strtotime('2010-01-01');
$end = strtotime('yesterday');
$date=null;
while($start < $end)
{
    $date=date('Y-m-d', $start);
     echo 'Doing: ' .$date . "\n";
     
     //find out if there is already info from this date.
      $sql1="SELECT feedcurr_id FROM batch.day_ingredient_usage_result WHERE date='$date 00:00:00'";
      $res1 = $GLOBALS['pdo']->query($sql1);
      
      if ($res1->rowCount()==0) { 
          //only insert if there is no data from that day.
       $sql="INSERT INTO batch.day_ingredient_usage_result SELECT * from nutrition.day_ingredient_usage('$date')";
    $res = $GLOBALS['pdo']->exec($sql);
   // Always check that result is not an error
    {
  
    }   
          
      }
          
     
    
    
     $start = strtotime("+1 day", $start); //advance
}

echo("stopping ration historical");

//end
?>
