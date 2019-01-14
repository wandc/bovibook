<?php 
/* php function to print out passed in svg data, NOT SECURE! */
include_once($_SERVER['DOCUMENT_ROOT'].'/global.php');
header('Content-type: application/xhtml+xml');

print('<?php xml version="1.0" encoding="UTF-8"?>'."\n\r");
print("<html xmlns='http://www.w3.org/1999/xhtml' xmlns:svg='http://www.w3.org/2000/svg' xml:lang='en'>"."\n\r");
print('<body>'."\n\r");

  $res = $GLOBALS['pdo']->query("SELECT data FROM system.svg_temp WHERE id={$_REQUEST['id']}");
  $row = $res->fetch(PDO::FETCH_ASSOC);
  
  $b=base64_decode($row['data']); //not sure why we don't need the url decode here too.
  print($b); 
  
  print('');
  print('</body>'."\n\r");
  print('</html>'."\n\r");
?>
