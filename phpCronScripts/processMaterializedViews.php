<?php
require_once('../global.php');



/** This refreshes daily materialized views */
class ProcessMaterializedViews {

  public function __construct() {
		self::main();
	    }

function main() {
//run nightly
$sql1="REFRESH MATERIALIZED VIEW CONCURRENTLY batch.bovinecurr_long";
$res = $GLOBALS['pdo']->exec($sql1);


//used by genetics page, run nightly.
$sql2="REFRESH MATERIALIZED VIEW bovinemanagement.herdgenetics";
$res = $GLOBALS['pdo']->exec($sql2);


}

}//end class

//run it
new ProcessMaterializedViews();

?>
