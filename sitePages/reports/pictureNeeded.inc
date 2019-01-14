<?php

require_once "HTML/QuickForm.php";


include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
include_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');

class PictureNeeded extends TabPage {

    public function defaultDisplay() {
        $tabArray[1]['name'] = 'Insert';
        $tabArray[2]['name'] = 'Log (Insert All)';

        $this->renderTabs($tabArray);
    }

    protected function tab1() {
        $a = new QF2_UploadPicture((object) array(forwardURLArr => array('pageid' => $_REQUEST['pageid'])), 'notdone');
        print($a->renderBox('QF2_UploadPicture', 'Upload Animal Photo Needed'));
        $this->displayListOfCowsNeedingAPictureTaken();
    }

    protected function tab2() {
        $a = new QF2_UploadPicture((object) array(forwardURLArr => array('pageid' => $_REQUEST['pageid'])), 'default');
        print($a->renderBox('QF2_UploadPicture', 'Upload Animal Photo All'));
        $this->displayPictureLog();
    }

    function displayPictureLog() {
        print("<h3>Log</h3>");
        
        $sqlTable = <<<SQL
SELECT xx.*,bovine.id,bovine.bovine_id,bovine.event_time,bovine.userid,bovine.id as picture_bovine_id FROM picture.bovine LEFT JOIN bovinemanagement.bovine xx on bovine.bovine_id=xx.id order by event_time DESC 
SQL;
        $statement = $GLOBALS['pdo']->prepare($sqlTable);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        

        $sqlDeleteButtonShow= <<<SQL
SELECT id FROM picture.bovine where userid='{$GLOBALS['auth']->getUsername()}' AND event_time >= (current_date - interval '1 week') order by event_time DESC limit 1
SQL;
       $statementDel = $GLOBALS['pdo']->prepare($sqlDeleteButtonShow);
       $statementDel->execute();
       $showDeleteButtonArr = $statementDel->fetchAll(PDO::FETCH_COLUMN);  //return just first column, index works with one id.       

       


        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {
            //$rowOut['event_time_raw'] = strtotime($row['event_time']);
            $rowOut['Animal'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $rowOut['Date'] = date("M d, Y", strtotime($row['event_time']));  //has to be at end.
            $rowOut['Employee'] = $row['userid'];
            $rowOut['action']=$this->deleteButtonSimpleTable((object) array('callBackURL' => "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}"), 'individual', (object) array('deleteID' => $row['picture_bovine_id'], 'schemaTable' => 'picture.bovine'),$showDeleteButtonArr);
      $output[] = $rowOut;
        }
        print( (new JQueryDataTable)->startBasicArray($output, 'searching: true,paging: true,ordering: false,info: true'));
    }

    /*
     * 
     */
    function deleteButtonSimpleTable(object $options, $mode = 'individual', object $param=null,array $idToShowButtonAtArray) {

       if  (in_array($param->deleteID,$idToShowButtonAtArray)) {
         $a = new QF2_GenericDeleteButton($options, 'individual', $param);     
         return  $a->renderDelete($param->deleteID);
    }
         else {return null;}
        
    }
    
   
    public function displayListOfCowsNeedingAPictureTaken() {
        print("<h2>Need Picture Taken</h2>");

        $sql = <<<SQL
		Select bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name,location_name
FROM bovinemanagement.bovinecurr 
LEFT JOIN picture.bovine  ON bovine.bovine_id=bovinecurr.id WHERE local_number is not null AND bovine.event_time is null
ORDER BY location_name,local_number
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {

            $rowOut['Animal'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
            $rowOut['Location'] =(($row['location_name']));  
            $output[] = $rowOut;
        }
        print( (new JQueryDataTable)->startBasicArray($output, 'searching: true,paging: false,ordering: false,info: true'));
    }

    //passes along error info.
    public function error() {
        $error = array();

        //find how many need a picture taken
        $res = $GLOBALS['pdo']->query("Select count(bovinecurr.id) as need_picture_count 
FROM bovinemanagement.bovinecurr 
LEFT JOIN picture.bovine  ON bovine.bovine_id=bovinecurr.id WHERE local_number is not null AND bovine.event_time is null");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 3, $row['need_picture_count'] . ' animals need a picture taken.');
        }




        return $error;
    }

}

//end class

require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/quickform2Helper.inc';

Class QF2_UploadPicture extends Quickform2Helper {

    function definition($form) {

        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');


        $aniNumber = $this->listOfAnimalsForPicture($this->mode);



        ////
        // $fieldset = $form->addFieldset('header')->setLabel('Record Heat Event');
        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);

        $form->addElement('select', 'bovine_id', null, array('options' => $aniNumber, 'label' => 'Animal'))->addRule("required", "Please specify your animal #.");

        $fsUpload = $form->addElement('fieldset')->setLabel('Upload picture');
        $upload = $fsUpload->addElement('file', 'imageName', array('style' => 'width: 200px'))
                ->setLabel('Picture (jpg, &lt;=20MB):');
// no longer using special 'uploadedfile' rule for uploads, allow client-side validation
        $upload->addRule('required', 'Please upload picture', null,
                HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
// no longer using 'filename' rule for uploads, allow client-side validation
        $upload->addRule('regex', 'Allowed extensions:  .jp(e)g', '/\\.(jpe?g)$/i',
                HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
// these don't work client-side, for obvious reasons
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an image',
                array('image/jpeg'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);



        //$form->addElement('file', 'imageName', null, array('label' => 'image'))->addRule('Needs to be a jpg', 'mimetype', 'image/jpeg');

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['imageName']['tmp_name'])) {

            $tmpFileName = $_FILES['imageName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        $data = resizeJPEG($tmpFileName);  //change pic size to smaller
        $escaped_data = pg_escape_bytea($data); //escape data


        $query = "INSERT INTO picture.bovine (bovine_id,event_time,picture,userid) VALUES ({$value['bovine_id']},'{$value['event_time']}','$escaped_data'::bytea,'{$GLOBALS['auth']->getUsername()}')";
        $res = $GLOBALS['pdo']->exec($query);
    }

    private function listOfAnimalsForPicture($type = 'default') {
        if ($type == 'default') {

            $aniNumber = Misc::createListOfAllAliveCows();
        } elseif ($type == 'notdone') {


            $sql = "Select bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.local_number
FROM bovinemanagement.bovinecurr 
LEFT JOIN picture.bovine  ON bovine.bovine_id=bovinecurr.id WHERE local_number is not null AND bovine.event_time is null
ORDER BY local_number";

            $aniNumber[null] = "Please Select one..."; //dummy value
            //grab a list of cows.
            $res = $GLOBALS['pdo']->query($sql);

            while (($row = $res->fetch())) {
//parse for last word of name and then make an array for the select list.
                $aniNumber[$row[0]] = $row[2] . " - " . Misc::femaleShortName($row[1]);
            }
        } else {
            throw new Exception("Programming Error: Type input for picture not specified\n");
        }

        return $aniNumber;
    }

}

//end class
?>
