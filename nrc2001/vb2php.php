<?php  


$file_contents = file_get_contents($argv[1]);


function fix_param_def($matches) 
{
	return "public $" . $matches[2] . ";";
}

function fix_non_pub_param($matches) {
	return "public $" . $matches[1] . ";";
}

function fix_arrays($matches) {
	return "public $" . $matches[2] . " = array();";


}

//$param_def_pattern = '/Public ([A-Z][A-Z0-9]+) As [A-Z]+ /';
$param_def_pattern = '/(Public )([A-Za-z_][A-Z_a-z0-9]*)( As [A-Za-z0-9]+)/';

$non_pub_param_pattern = '/([A-Za-z_][A-Za-z0-9_]*)( As [A-Za-z0-9]+)/'; 


#Public Header(1 To 3) As String
$arrays_pattern = '/(Public )([A-Za-z_][A-Z_a-z0-9]*)\([0-9]+ To [0-9]+\)( As [A-Za-z0-9]+)/';

$tmp_contents = preg_replace_callback($param_def_pattern, 'fix_param_def', $file_contents);

$tmp_contents = preg_replace_callback($non_pub_param_pattern, 'fix_non_pub_param', $tmp_contents);

$tmp_contents = preg_replace_callback($arrays_pattern, 'fix_arrays', $tmp_contents);

echo $tmp_contents;





