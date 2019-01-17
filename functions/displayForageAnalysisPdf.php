<?php 

/* used to display pictures from picture db, based on bovine db */
require_once('../global.php');

$sample_number = filter_var($_REQUEST['sample_number'], FILTER_SANITIZE_STRING);

$res = $GLOBALS['pdo']->prepare("SELECT report_file,octet_length(report_file) as length FROM nutrition.bag_analysis WHERE sample_number=? limit 1");
$res->execute(array($sample_number));
$res->bindColumn(1, $lob, PDO::PARAM_LOB);
$res->bindColumn(2, $length, PDO::PARAM_INT);
$res->fetch(PDO::FETCH_BOUND);
if ($res->rowCount() == 1) {

    header('Content-type: application/pdf');
    header("Cache-Control: no-cache");
    header("Pragma: no-cache");
    header("Content-Disposition: inline;filename='$sample_number.pdf'");
    header("Content-length: " . $length); //use octet_length

    fpassthru($lob); //echo out data
} else {

    print("<h1>Error pdf $sample_number does not exist!</h1>");
}
?> 