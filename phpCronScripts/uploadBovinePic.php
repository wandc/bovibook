<?php 
require_once('../global.php');
require_once('../functions/misc.php');
/**
* Uploads a picture of a paticular cow to the database.
*
*/
if (defined('STDIN')) { //when called from cli, command line

  /* check for pictures in email */  
  if (($argc == 2) && ($argv[1]=='email')) {
  getPicturesViaEmail();
  }  
  /* check for picture at command lines */  
  elseif ($argc == 3) {
  $bovineNumber=$argv[1];
  $fileName=$argv[2];
  print("Adding cow #: $bovineNumber"."<br>"."\n\r");
  insertPictureIntoDBViaFile($bovineNumber,$fileName);
  }  
  else {
  print("USAGE: php uploadBovinePic.php COW# PICTURE_NAME_WITH_PATH"."\n\r");  
  print("OR: php uploadBovinePic.php email"."\n\r");  
  }  

}
else {
 print("No action when called via the web<br/>"); 
}  

/** *************************************************
* METHODS
****************************************************/

function getPicturesViaEmail() {

//make a delete msgno array. reason for this is that msgno are dynamic and when you delete one it changes them, so do deletes at end.
$deleteMsgnoArray=null;
  
//login to imap server
$mbox = imap_open ("{imap.gmail.com:993/imap/ssl/novalidate-cert}INBOX", "picture@littleriver.ca", "69K639")
     or die("can't connect to email account: " . imap_last_error());
     
 
   
  
$finished=false;
while ($finished==false) {

print("Starting While Loop.\n\t");  
  
  $finished=insertPictureIntoDBViaEmail($mbox);


}//end while


}//end function


function insertPictureIntoDBViaEmail($mbox) {
  

//get list of email headers.
$MC = imap_check($mbox); 
$headers =imap_fetch_overview($mbox,"1:{$MC->Nmsgs}",0);
print_r($headers);
if ($headers == false) {
    echo('No emails in account to process.'.'<br/>'."\n\r");
    //finished=true;
    return true;
} 
else {

      //just grab the first email, porgrammed this way because delete changes the state of the mail box.
      $uid=$headers[0]->uid;

 // get header
 $info=imap_headerinfo($mbox,imap_msgno($mbox,$uid)); 

 //check the info we get from header is valid
 if (((is_numeric(trim($info->subject)) ==true) && (strstr($info->fromaddress,'littleriver.ca') != false)) ||   ((is_numeric(trim($info->subject)) ==true) && (strstr($info->fromaddress,'amy.currie@live.com') != false))) {  
 $retArray=getAttachmentFromSpecificEmail($mbox,$uid);

 print("Local Number:{$retArray['local_number']}\n\r");
 
$bovineNumber=$retArray['local_number'];
$event_time=$retArray['date'];
$data=$retArray['file_content'];

//now resize the picture.
$tmpFile='/tmp/uploadfile_temp.jpg';
$fh = fopen($tmpFile, 'w') or die("can't open file");
fwrite($fh, $data); //write temp file.
$data=resizeJPEG($tmpFile);
fclose($fh);
unlink($tmpFile);//delete temp file.
//end resizing

 // Open a transaction
 try {$res = $GLOBALS['pdo']->beginTransaction();
	      
//sql
$sql="SELECT id FROM bovinemanagement.bovine WHERE local_number=$bovineNumber LIMIT 1";
$res = $GLOBALS['pdo']->query($sql);  
$row = $res->fetch(PDO::FETCH_ASSOC);
$bovine_id=$row['id']; 
$escaped_data =  pg_escape_bytea($data); 

$sqlInsert="INSERT INTO picture.bovine (bovine_id,event_time,picture) VALUES ($bovine_id,'$event_time','$escaped_data'::bytea)";

//check if for some reason we already did insert and the email was not deleted.
$sql2="SELECT id FROM picture.bovine WHERE bovine_id='$bovine_id' AND event_time='$event_time' LIMIT 1";
$res2 = $GLOBALS['pdo']->query($sql2);  

//check if for some reason we already did insert and the email was not deleted.
if ($res2->rowCount() == 0) {
//do insert
 print("\t\tAdding cow #: $bovineNumber"."<br>"."\n\r");
$res=$GLOBALS['pdo']->exec($sqlInsert); //insert
}
else {
  print("\t\tIgnoring... cow #: $bovineNumber, already in DB."."<br>"."\n\r");
}  


              // determine if the commit or rollback
	  
                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
		
		//now delete the original email.
		 
		   $info=imap_headerinfo($mbox,imap_msgno($mbox,$uid)); 
		    print("\t\tDeleteing... cow #: $bovineNumber / {$info->subject}"."<br>"."\n\r");
		  imap_mail_move($mbox, imap_msgno($mbox,$uid).':'.imap_msgno($mbox,$uid), '[Gmail]/Trash'); //move to trash to be safe.
		   //finished=false;
                   return false;
	      

    }
    //message is not valid format, so delete it.
    else {
    print("\t\tDeleteing... : Not a valid message: ".imap_msgno($mbox,$uid)."<br>"."\n\r");
    //imap_mail_move($mbox, imap_msgno($mbox,$uid).':'.imap_msgno($mbox,$uid), '[Gmail]/Trash'); //move to trash to be safe.
    // $info=imap_headerinfo($mbox,imap_msgno($mbox,$uid)); 
		   
     //finished=false;
     return false;
    }


}//end big else

  
}  //end function



//from: http://www.electrictoolbox.com/extract-attachments-email-php-imap/
function getAttachmentFromSpecificEmail($mbox,$uid) {
  $structure = imap_fetchstructure($mbox,imap_msgno($mbox,$uid));
  $attachments = array();
if(isset($structure->parts) && count($structure->parts)) {

	for($i = 0; $i < count($structure->parts); $i++) {

		$attachments[$i] = array(
			'is_attachment' => false,
			'filename' => '',
			'name' => '',
			'attachment' => ''
		);
		
		if($structure->parts[$i]->ifdparameters) {
			foreach($structure->parts[$i]->dparameters as $object) {
				if(strtolower($object->attribute) == 'filename') {
					$attachments[$i]['is_attachment'] = true;
					$attachments[$i]['filename'] = $object->value;
				}
			}
		}
		
		if($structure->parts[$i]->ifparameters) {
			foreach($structure->parts[$i]->parameters as $object) {
				if(strtolower($object->attribute) == 'name') {
					$attachments[$i]['is_attachment'] = true;
					$attachments[$i]['name'] = $object->value;
				}
			}
		}
		
		if($attachments[$i]['is_attachment']) {
			$attachments[$i]['attachment'] = imap_fetchbody($mbox, imap_msgno($mbox,$uid), $i+1);
			if($structure->parts[$i]->encoding == 3) { // 3 = BASE64
				$attachments[$i]['attachment'] = base64_decode($attachments[$i]['attachment']);
			}
			elseif($structure->parts[$i]->encoding == 4) { // 4 = QUOTED-PRINTABLE
				$attachments[$i]['attachment'] = quoted_printable_decode($attachments[$i]['attachment']);
			}
		}
	}
}
else { return false;} //no attachements 
  
 
   //download all the info we need.  
   $info=imap_headerinfo($mbox,imap_msgno($mbox,$uid));   
   $retArray['date']=$info->date;
   $retArray['local_number']=$info->subject;
   $retArray['file_content']= $attachments[1]['attachment']; //only get first attachment
   
  
    return $retArray;

} //end function




function insertPictureIntoDBViaFile($bovineNumber,$fileName) {

//lookup bovine_id via cow number.
$sql="SELECT id FROM bovinemanagement.bovine WHERE local_number=$bovineNumber LIMIT 1";
$res = $GLOBALS['pdo']->query($sql);  
$row = $res->fetch(PDO::FETCH_ASSOC);
$bovine_id=$row['id'];

//get current time for postgres
$event_time=date('c',strtotime('now'));

/*now insert picture*/
//load file and resize
$resizedPic=resizeJPEG($fileName);

$data = $resizedPic;
//necessary to properly escape the binary data for bytea type. GET IT BACK to original via  $original_data = pg_unescape_bytea($escaped_data)); 
$escaped_data =  pg_escape_bytea($data); 
$sql="INSERT INTO picture.bovine (bovine_id,event_time,picture) VALUES ($bovine_id,'$event_time','$escaped_data'::bytea)";
$res=$GLOBALS['pdo']->exec($sql);

}
?>