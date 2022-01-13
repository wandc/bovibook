<?php

class FileUpload extends TabPage {

    public function defaultDisplay() {
        
        //NOTE: the file upload QF2 form seems to not work if it is ajax tab, so load everything inline, page is fast so doen not matter.
        $tabArray[1]['name'] = 'Valacta';
        $tabArray[1]['load_inline'] = '1'; 
        $tabArray[2]['name'] = 'Holstein Canada Index Data File';
        $tabArray[2]['load_inline'] = '1'; 
        $tabArray[3]['name'] = 'Shur-Gain Flock Report';
        $tabArray[3]['load_inline'] = '1'; 
        $tabArray[4]['name'] = 'Shur-Gain Forage Analysis';
        $tabArray[4]['load_inline'] = '1'; 
        $tabArray[5]['name'] = 'Agri Analyze (Addican) Forage Analysis';
        $tabArray[5]['load_inline'] = '1'; 
        $tabArray[6]['name'] = 'PEI Soil Lab CSV';
        $tabArray[6]['load_inline'] = '1'; 

        $this->renderTabs($tabArray);
    }

    function tab1() {
        print((new ValactaFiles())->main());
    }

    function tab2() {
        print((new HolsteinCanadaIndexFiles())->main());
    }
    
     function tab3() {
        print((new ShurGainFlockFiles())->main());
    }

     function tab4() {
        print((new ShurGainForageAnalysisFiles())->main());
    }
    
     function tab5() {
        print((new AgriAnalyzeForageAnalysisFiles())->main());
    }
    
     function tab6() {
        print((new PEISoilLabFiles())->main());
    }
    
}

//end class

class PEISoilLabFiles {
    
         public function main() {

        $a = new QF2_UploadPEISoilLabCSV((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');
        
         $out[]= (new BootStrap)->oneThirdTwoThirds($this->displayPEISoilLabFileList(), $a->renderBox('QF2_UploadPEISoilLabCSV', 'Upload PEI Soil Lab CSV File'));     
         return (implode($out));
    }
    
     public function displayPEISoilLabFileList() {


        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.pei_soil_csv order by file_name
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.pei_soil_csv'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded PEI Soil Lab Files', $output, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }
    
    
}//end class

/* addican quebec lab */
class AgriAnalyzeForageAnalysisFiles {
    
         public function main() {

        $a = new QF2_UploadAgriAnalyzeFile((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');

          $out[]= (new BootStrap)->oneThirdTwoThirds(  $this->displayAgriAnalyzeForageAnalysisFileList() ,  $a->renderBox('QF2_UploadAgriAnalyzeFile', 'Upload Agri Analyze Forage Analysis File'));       
         
     print(implode($out));
    }
     

     public function displayAgriAnalyzeForageAnalysisFileList() {
       

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.forage_analysis_agri_analyze order by event_time DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.forage_analysis_agri_analyze'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded Agri Analyze Forage Analysis File', $output, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }
    
    
}//end class


class ShurGainForageAnalysisFiles {
    
         public function main() {

        $a = new QF2_UploadShurGainForageAnalysisFile((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');       
              
 $out[]= (new BootStrap)->oneThirdTwoThirds( $this->displayShurGainForageAnalysisFileList(), $a->renderBox('QF2_UploadShurGainForageAnalysisFile', 'Upload Shur-Gain Forage Analysis File'));       
         
     print(implode($out));
    }
     

     public function displayShurGainForageAnalysisFileList() {
     

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.forage_analysis_shur_gain order by event_time DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.forage_analysis_shur_gain'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded Shur-Gain Forage Analysis File', $output, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }
    
    
}//end class


class ShurGainFlockFiles {
    
         public function main() {

        $a = new QF2_UploadShurGainFlockFile((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');

        $out[]= (new BootStrap)->oneThirdTwoThirds( $this->displayShurGainFlockFileList(), $a->renderBox('QF2_UploadShurGainFlockFile', 'Upload Shur-Gain Flock Report File'));       
         
     print(implode($out));
    }
     
     public function displayShurGainFlockFileList() {
   

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.shur_gain_flock order by event_time desc,file_name
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.shur_gain_flock'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded Shur-Gain Flock Files', $output, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }
    
    
}//end class


class ValactaFiles {

    public function main() {

        $a = new QF2_UploadValctaXMLFile((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');

        $left = <<<HTML
             {$this->displayValactaXMLList()}
             {$this->displayValactaTIPList()}  
HTML;
        
         $out[]= (new BootStrap)->oneThirdTwoThirds(  $left, $a->renderBox('QF2_UploadValctaXMLFile', 'Upload Valacta XML File'));       
         
     print(implode($out));
        
        
    }

    public function displayValactaXMLList() {
     

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.valacta_xml order by file_name DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.valacta_xml'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded Valacta XML Files', $output, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }

    public function displayValactaTIPList() {
     

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.valacta_tip order by file_name
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.valacta_tip'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded Valacta TIP Files', $output, 'searching: true,paging: true,ordering: false,info: true'));
        return implode($out);
    }

}

Class QF2_UploadValctaXMLFile extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $fsUpload = $form->addElement('fieldset')->setLabel('Upload File');
        $upload = $fsUpload->addElement('file', 'fileName', array('style' => 'width: auto'))->setLabel('File (xml, &lt;=20MB):');
        $upload->addRule('required', 'Please select file to upload.', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('regex', 'Allowed extensions:  .xml', '/\\.(xml)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an XML file', array('text/xml'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['fileName']['tmp_name'])) {

            $tmpFileName = $_FILES['fileName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        //file data
        $data = file_get_contents($tmpFileName);  //load file stored in tmp dir.
        $escaped_data = pg_escape_bytea($data); //escape data
        //file name
        $fileName = $_FILES['fileName']['name'];

        $sql = "INSERT INTO documents.valacta_xml (event_time,data,file_name,userid) VALUES (:event_time,:data::bytea,:file_name,:userid) ON CONFLICT DO NOTHING";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $statement->bindParam(':data', $escaped_data, PDO::PARAM_LOB);
        $statement->bindParam(':file_name', $fileName, PDO::PARAM_STR);
        $statement->bindParam(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
    }

}

class HolsteinCanadaIndexFiles {

    function main() {

        $a = new QF2_UploadHolsteinCanadaIndexFile((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'notdone');
       
          $out[]= (new BootStrap)->oneThirdTwoThirds( $this->displayHolsteinIndexFileList(), $a->renderBox('QF2_UploadHolsteinCanadaIndexFile', 'Upload Holstein Canada Index File'));       
         
     print(implode($out));
        
        
    }

    public function displayHolsteinIndexFileList() {
   

        $sql = <<<SQL
		Select id,event_time,file_name,userid from documents.holstein_canada_index_data order by event_time DESC,file_name DESC
SQL;

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        $rowOut = array();
        $output = array();
        foreach ($results as &$row) {


            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['File Name'] = $row['file_name'];
            $rowOut['Employee'] = $row['userid'];
            $rowOut['File Date'] = date('M d, Y', strtotime($this->getFileDateHolsteinIndexFile($row['id'])));

            //delete button
            $callBackURL = "Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}";
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $row['id'], 'schemaTable' => 'documents.valacta_xml'));
            $rowOut['Action'] = ($a->renderDelete($row['id']));

            $output[] = $rowOut;
        }
        $out[] = ( (new JQueryDataTable)->startBasicArrayOpt('Uploaded Holstein Index Files', $output, 'searching: true,paging: true,ordering: false, "order": [[ 4, "desc" ]], info: true'));
        return implode($out);
    }

    //give the id of file we get the extract date and return it. 
    function getFileDateHolsteinIndexFile($id) {
        $sql = "SELECT id,data::bytea,file_name FROM documents.holstein_canada_index_data where id=$id limit 1";
        $stmt = $GLOBALS['pdo']->prepare($sql);
        $stmt->execute();

        /* Bind by column number */
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $data, PDO::PARAM_LOB);
        $stmt->bindColumn(3, $fileName, PDO::PARAM_STR);

        $stmt->fetch(PDO::FETCH_BOUND);

        $file = explode(PHP_EOL, (pg_unescape_bytea(stream_get_contents($data))));

        $a = trim(substr($file[0], 677, 8));

        return($a);
    }

}

//end class

//end class

Class QF2_UploadHolsteinCanadaIndexFile extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $fsUpload = $form->addElement('fieldset')->setLabel('Upload File');
        $upload = $fsUpload->addElement('file', 'fileName', array('style' => 'width: auto'))->setLabel('File (xml, &lt;=20MB):');
        $upload->addRule('required', 'Please select file to upload.', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('regex', 'Allowed extensions:  .txt', '/\\.(txt)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an Index TXT file', array('text/plain'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['fileName']['tmp_name'])) {

            $tmpFileName = $_FILES['fileName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        //file data
        $data = file_get_contents($tmpFileName);  //load file stored in tmp dir.
        $escaped_data = pg_escape_bytea($data); //escape data
        //file name
        $fileName = $_FILES['fileName']['name'];

        $sql = "INSERT INTO documents.holstein_canada_index_data (event_time,data,file_name,userid) VALUES (:event_time,:data::bytea,:file_name,:userid) ON CONFLICT DO NOTHING";


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $statement->bindParam(':data', $escaped_data, PDO::PARAM_LOB);
        $statement->bindParam(':file_name', $fileName, PDO::PARAM_STR);
        $statement->bindParam(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
    }

}


Class QF2_UploadShurGainFlockFile extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $fsUpload = $form->addElement('fieldset')->setLabel('Upload File');
        $upload = $fsUpload->addElement('file', 'fileName', array('style' => 'width: auto'))->setLabel('File (xlsx, &lt;=20MB):');
        $upload->addRule('required', 'Please select file to upload.', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('regex', 'Allowed extensions:  .xlsx', '/\\.(xlsx)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an Index XLSX file', array('application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['fileName']['tmp_name'])) {

            $tmpFileName = $_FILES['fileName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        //file data
        $data = file_get_contents($tmpFileName);  //load file stored in tmp dir.

        //file name
        $fileName = $_FILES['fileName']['name'];

        $sql = "INSERT INTO documents.shur_gain_flock (event_time,data,file_name,userid) VALUES (:event_time,:data::bytea,:file_name,:userid) ON CONFLICT DO NOTHING";
       

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $statement->bindParam(':data', $data, PDO::PARAM_LOB);
        $statement->bindParam(':file_name', $fileName, PDO::PARAM_STR);
        $statement->bindParam(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
    }

}//end class

Class QF2_UploadShurGainForageAnalysisFile extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $fsUpload = $form->addElement('fieldset')->setLabel('Upload File');
        $upload = $fsUpload->addElement('file', 'fileName', array('style' => 'width: auto'))->setLabel('File (pdf, &lt;=20MB):');
        $upload->addRule('required', 'Please select file to upload.', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('regex', 'Allowed extensions:  .pdf', '/\\.(pdf)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an PDF file', array('application/pdf'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['fileName']['tmp_name'])) {

            $tmpFileName = $_FILES['fileName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        //file data
        $data = file_get_contents($tmpFileName);  //load file stored in tmp dir.

        //file name
        $fileName = $_FILES['fileName']['name'];

        //make file name safe for HTML download later.
        $fileNameNew1 = mb_ereg_replace("([^\w\s\d\-_~,;\[\]\(\).])", '', $fileName); //html does not support commas and other weird chars in filenames, even if OS does. sanitize.
        $fileNameNew2 = mb_ereg_replace("([\.]{2,})", '', $fileNameNew1); //removes runs of periods.
        
        
        $sql = "INSERT INTO documents.forage_analysis_shur_gain (event_time,data,file_name,userid) VALUES (:event_time,:data::bytea,:file_name,:userid) ON CONFLICT DO NOTHING";
       

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $statement->bindParam(':data', $data, PDO::PARAM_LOB);
        $statement->bindParam(':file_name', $fileNameNew2, PDO::PARAM_STR);
        $statement->bindParam(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
    }

}//end class


Class QF2_UploadPEISoilLabCSV extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $fsUpload = $form->addElement('fieldset')->setLabel('Upload File');
        $upload = $fsUpload->addElement('file', 'fileName', array('style' => 'width: auto'))->setLabel('File (csv, &lt;=20MB):');
        $upload->addRule('required', 'Please select file to upload.', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('regex', 'Allowed extensions:  .csv', '/\\.(csv)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an CSV file', array('text/csv'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['fileName']['tmp_name'])) {

            $tmpFileName = $_FILES['fileName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        //file data
        $data = file_get_contents($tmpFileName);  //load file stored in tmp dir.

        //file name
        $fileName = $_FILES['fileName']['name'];

        //make file name safe for HTML download later.
        $fileNameNew1 = mb_ereg_replace("([^\w\s\d\-_~,;\[\]\(\).])", '', $fileName); //html does not support commas and other weird chars in filenames, even if OS does. sanitize.
        $fileNameNew2 = mb_ereg_replace("([\.]{2,})", '', $fileNameNew1); //removes runs of periods.
        
        
        $sql = "INSERT INTO documents.pei_soil_csv (event_time,data,file_name,userid) VALUES (:event_time,:data::bytea,:file_name,:userid) ON CONFLICT DO NOTHING";
       

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $statement->bindParam(':data', $data, PDO::PARAM_LOB);
        $statement->bindParam(':file_name', $fileNameNew2, PDO::PARAM_STR);
        $statement->bindParam(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
    }

}//end class

/* addican , lab in quebec they use */
Class QF2_UploadAgriAnalyzeFile extends QF2_Base {

    function definition($form) {

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        // for file upload to work
        $form->setAttribute('enctype', 'multipart/form-data');
        $fsUpload = $form->addElement('fieldset')->setLabel('Upload File');
        $upload = $fsUpload->addElement('file', 'fileName', array('style' => 'width: auto'))->setLabel('File (pdf, &lt;=20MB):');
        $upload->addRule('required', 'Please select file to upload.', null, HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('regex', 'Allowed extensions:  .pdf', '/\\.(pdf)$/i', HTML_QuickForm2_Rule::ONBLUR_CLIENT_SERVER);
        $upload->addRule('mimetype', 'Your browser doesn\'t think that\'s an PDF file', array('application/pdf'));
        $upload->addRule('maxfilesize', 'File is too big, allowed size 20MB', 20480000);

        return $form; //required       
    }

    function process($value) {


        $value['event_time'] = date('r', strtotime("now"));

        if (is_uploaded_file($_FILES['fileName']['tmp_name'])) {

            $tmpFileName = $_FILES['fileName']['tmp_name'];
        } else {
            throw new Exception("Error possible upload attack.");
        }



        //file data
        $data = file_get_contents($tmpFileName);  //load file stored in tmp dir.

        //file name
        $fileName = $_FILES['fileName']['name'];

        //make file name safe for HTML download later.
        $fileNameNew1 = mb_ereg_replace("([^\w\s\d\-_~,;\[\]\(\).])", '', $fileName); //html does not support commas and other weird chars in filenames, even if OS does. sanitize.
        $fileNameNew2 = mb_ereg_replace("([\.]{2,})", '', $fileNameNew1); //removes runs of periods.
        
        
        $sql = "INSERT INTO documents.forage_analysis_agri_analyze (event_time,data,file_name,userid) VALUES (:event_time,:data::bytea,:file_name,:userid) ON CONFLICT DO NOTHING";
       

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->bindParam(':event_time', $value['event_time'], PDO::PARAM_STR);
        $statement->bindParam(':data', $data, PDO::PARAM_LOB);
        $statement->bindParam(':file_name', $fileNameNew2, PDO::PARAM_STR);
        $statement->bindParam(':userid', $GLOBALS['auth']->getUsername(), PDO::PARAM_STR);
        $statement->execute();
    }

}//end class
?>