<?php

class BagSample extends BasePage {

    public function defaultDisplay() {

               $html = <<<HTML
            <div class='row'>
                <div class='col-sm-4 col-sm-push-8'>   
                   <h3>CO-OP cumberland valley lab. Type lab ID number into form and then go into db and change feet and bag_id.</h3>
               {$this->CumberLandValleySampleImportQuickform()}
                   <h3>Link Sample to Bag Footage</h3>
               {$this->footageToSampleLinkcQuickform()}
                </div>       
                <div class='col-sm-8 col-sm-pull-4'>
                    <h2>Shur-Gain Samples</h2>
               {$this->displayLastShurGainSampleNumbers(2)}
                    <h2>Log</h2>
               {$this->displayLastShurGainSampleNumbers(365)}

                    <h2>Cumberland Lab Samples</h2>
               {$this->displayLastCumberlandSampleNumbers(2)}
                     <h2>Log</h2>
               {$this->displayLastCumberlandSampleNumbers(365)}
                </div>
            </div>           
HTML;
               
        print($html);
     
    }

    function displayLastCumberlandSampleNumbers($days = 2) {
//SELECT sample_number from nutrition.bag_analysis  WHERE NOT (nutrition.bag_dimenension(bag_id))[1] < footage AND (nutrition.bag_dimenension(bag_id))[2] >= footage ; //cehck for error

        $sql = <<<SQL
                SELECT '<a href=\"http://www.foragelab.com/ViewSamples.aspx?LabID=' || sample_number || '\">' || sample_number || '</a>' as sample_number ,bag_analysis.raw_report->'comment' as "Comment",
 SUBSTR(location, 1, 1) || slot || '-' || footage as linked_bag,nutrition.bag_dimenension(bag_id)
FROM nutrition.bag_analysis 
LEFT JOIN nutrition.bag ON bag_id=bag.id
WHERE bag_analysis.create_time > current_timestamp  - interval '$days day'  AND lab_name='cumberland' ORDER BY bag_analysis.create_time DESC
SQL;
        
        $str[]=("<h3>Samples Added in Last $days Days</h3>");
        $str[]=( (new JQueryDataTable)->startBasicSql($sql,''));
       return implode($str);
    }

    function displayLastShurGainSampleNumbers($days = 2) {
        $sql = <<<SQL
                SELECT sample_number ,bag_analysis.raw_report->'comment' as "Comment",
 SUBSTR(location, 1, 1) || slot || '-' || footage as linked_bag,nutrition.bag_dimenension(bag_id)
FROM nutrition.bag_analysis 
LEFT JOIN nutrition.bag ON bag_id=bag.id
WHERE bag_analysis.create_time > current_timestamp  - interval '$days day'  AND lab_name='shur-gain' ORDER BY linked_bag DESC,sample_number,bag_analysis.create_time DESC
SQL;
       
        $str[]=("<h3>Samples Added in Last $days Days</h3>");
        $str[]=( (new JQueryDataTable)->startBasicSql($sql,''));
       return implode($str);
    }

    /*
     * looks for samples with empty footage and allows footage TO BE ADDED.
     */

    public function footageToSampleLinkcQuickform() {


        //Find bag samples with empty footage.
        $sql = "SELECT id,raw_report->'comment',sample_number FROM nutrition.bag_analysis WHERE bag_id isnull or footage isnull order by sample_number";
        $res = $GLOBALS['pdo']->query($sql);
        $bagSampleArr = array();
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $bagSampleArr[$row['sample_number']] = $row['sample_number'] . ' ' . $row['comment'];
        }

        //Find current bags
        $sql = "SELECT id,substring(location,1,1)||slot as slotnew,event_time FROM nutrition.bag_current ORDER BY substring(location,1,1)||slot,event_time DESC";
        $res = $GLOBALS['pdo']->query($sql);

        $bagArr[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $bagArr[$row['id']] = $row['slotnew'] . '   &nbsp;&nbsp;&nbsp;' . date('Y-m-d', strtotime($row['event_time']));
        }

        //List of Footages
        $listOfFootages[null] = "Please Select one..."; //dummy value
        for ($i = -20; $i <= 300; $i++) {
            $listOfFootages[$i] = "$i";
        }


        $form = new HTML_QuickForm("footageToSampleLinkcQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdrcullCowQuickformt', 'Link Sample to Bag Footage');
        $form->addElement('select', 'bagSample', 'Bag Sample', $bagSampleArr);
        $form->addElement('select', 'bag', 'Bag', $bagArr);
        $form->addElement('select', 'footage', 'Footage', $listOfFootages);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('bagSample' => 0)); //set default to no variety.
        $form->setDefaults(array('bag' => 0)); //set default to no variety.
        $form->setDefaults(array('footage' => 0)); //set default to no variety.
        //rules
        $form->addRule('bagSample', 'Bag Sample is required', 'required');
        $form->addRule('bag', 'bag is required', 'required');
        $form->addRule('footage', 'footage is required', 'required');


        // Try to validate a form
        if ($form->validate()) {

            //get values
            $bag = $form->exportValue('bag');
            $bagSample = $form->exportValue('bagSample');
            $footage = $form->exportValue('footage');

            $pageid = $form->exportValue('pageid');

            //record death_date in bovine
            $query = "UPDATE nutrition.bag_analysis SET footage=$footage,bag_id=$bag WHERE sample_number='$bagSample'";
            $res = $GLOBALS['pdo']->exec($query);


            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation
        else {
            $form->accept($renderer);
            return $renderer->toHtml();
        }
    }

    function CumberLandValleySampleImportQuickform() {



        $form = new HTML_QuickForm("CumberLandValleySampleImportQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);
        $renderer = new HTML_QuickForm_Renderer_Tableless();



        $form->addElement('text', 'labid', 'Lab ID', array('size' => 8, 'maxlength' => 8));

        $form->addElement('hidden', 'pageid', $_REQUEST ['pageid']);

        // submit buttons
        $submitGroup [] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup [] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');
        //
        $form->addRule('labid', 'This element is required.', 'required');
        $form->addRule('labid', 'A valid number is required.', 'numeric');

        if ($form->validate()) {

            $pageid = $form->exportValue('pageid');

            $labid = $form->exportValue('labid');
            $labid_64 = base64_encode($labid); //needs base 64 version
            //
            $xml = $this->downloadSampleXML($labid_64);
            $pdfData = $this->downloadSamplePDF($labid, $labid_64);
            //
            $sampleArr = $this->xmlSampletoArray($xml, $pdfData);
            //
            $this->sampleDBInsert($sampleArr);


            //print("<br>OK");
            // forward to same page, ob_start() is key to making this work!
            $host = $_SERVER ['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } // end validation


        $form->accept($renderer);
        return $renderer->toHtml();
    }

    function downloadSampleXML($labid_64) {
        //try and get XML from forage lab
        $sample_url = "http://foragelab.com/ReportExport.aspx?LabIds=$labid_64&reportName=StandardXML";


        if (($response_xml_data = file_get_contents($sample_url)) === false) {
            echo "Error fetching XML\n";
        } else {
            libxml_use_internal_errors(true);
            $xml = simplexml_load_string($response_xml_data);
            if (!$xml) {
                echo "Error loading XML\n";
                foreach (libxml_get_errors() as $error) {
                    echo "\t", $error->message;
                }
            } else {


                return $xml;
            }
        }
    }

    function downloadSamplePDF($labid, $labid_64) {
        //they seem to be tracking cookies now, so to get pdf we need to got to the page where you would click it first.
        /*
          $ckfile = tempnam ("/tmp", 'cookiename');

          $ch=curl_init();
          $timeout=40;

          //   CURLOPT_COOKIEJAR and CURLOPT_COOKIEFILE

          $url1="http://www.foragelab.com/ViewSamples.aspx?LabID=$labid";

          $url2="http://foragelab.com/ReportExport.aspx?LabIds=$labid_64&reportName=StandardWetChemistry";
          $agent= 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.0.3705; .NET CLR 1.1.4322)';

          curl_setopt($ch, CURLOPT_USERAGENT, $agent);
          curl_setopt ($ch, CURLOPT_COOKIEJAR, $ckfile);
          curl_setopt ($ch, CURLOPT_COOKIEFILE, $ckfile);
          curl_setopt($ch, CURLOPT_URL, $url1);
          curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
          curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
          curl_setopt($ch, CURLOPT_TIMEOUT, $timeout);
          $ignoreResponse=curl_exec($ch);
          //2nd call
          curl_setopt($ch, CURLOPT_URL, $url2);
          $pdfData=curl_exec($ch);
          curl_close($ch);
          return $pdfData;
         */

        $sample_url_pdf = "http://foragelab.com/ReportExport.aspx?LabIds=$labid_64&reportName=StandardWetChemistry";
        $pdfData = file_get_contents($sample_url_pdf);
        return $pdfData;
    }

    function xmlSampletoArray($xml, $pdfData) {

        $rowArray['Sample no'] = (string) $xml->Sample_Data->LabID['Value'];
        $rowArray['Analysis date'] = date('r', strtotime((string) $xml->Sample_Data->datesampled['Value']));
        $rowArray['comment'] = pg_escape_string((string) $xml->Sample_Data->Desc_1['Value']); //escape it, can have arbitrary characters.
        $rowArray['DryMatter'] = (string) $xml->Sample_Data->DM['Value'];
        $rowArray['pH'] = (string) $xml->Sample_Data->pH;
        $rowArray['CrudeProtein'] = (string) $xml->Sample_Data->CP['Value'];
        $rowArray['SolubleProtein'] = (string) $xml->Sample_Data->sp['Value'];
        $rowArray['ADFCP'] = (string) $xml->Sample_Data->ADICP_CP['Value'];
        $rowArray['NDFCP'] = (string) $xml->Sample_Data->NDICP_CP['Value'];
        $rowArray['ADF'] = (string) $xml->Sample_Data->ADF['Value'];
        $rowArray['NDF'] = (string) $xml->Sample_Data->NDF['Value'];
        $rowArray['Lignin'] = (string) $xml->Sample_Data->Lignin['Value'];
        $rowArray['Non-fibreCarbohydrates'] = (string) $xml->Sample_Data->NFC['Value'];
        $rowArray['Starch'] = (string) $xml->Sample_Data->Starch['Value'];
        $rowArray['CrudeFat'] = (string) $xml->Sample_Data->Fat_EE['Value'];
        $rowArray['TDN'] = (string) $xml->Sample_Data->TDN['Value'];
        $rowArray['DE'] = (string) '';
        $rowArray['Calcium'] = (string) $xml->Sample_Data->Ca['Value'];
        $rowArray['Phosphorus'] = (string) $xml->Sample_Data->P['Value'];
        $rowArray['Magnesium'] = (string) $xml->Sample_Data->Mg['Value'];
        $rowArray['Potassium'] = (string) $xml->Sample_Data->K['Value'];
        $rowArray['Sodium'] = (string) $xml->Sample_Data->Na['Value'];
        $rowArray['Ash'] = (string) $xml->Sample_Data->Ash['Value'];
        $rowArray['ADFN'] = (string) '';
        $rowArray['Vomitoxine'] = (string) '';
        $rowArray['Zearalenone'] = (string) '';
        $rowArray['pdf_file_report'] = (pg_escape_bytea($pdfData));
//extra 
        $rowArray['RFQ'] = (string) $xml->Sample_Data->RFQ['Value'];


//clenup blank fields and put in null instead.
        $array2 = array_map(function($value) {
            return $value === "" ? 'null' : $value;
        }, $rowArray);


        return $array2;
    }

    function sampleDBInsert($sampleArr) {

        //FIXME, needs to be changed to JSON if we use cumberland again.
        //now insert into db.
        $sql = 'INSERT INTO nutrition.bag_analysis (bag_id,footage,sample_number,sample_date,comment,"DM","pH","CP","SP","ADFCP","NDFCP","ADF","NDF","lignin","NFC","starch","CF","TDN","DE","Ca","P","Mg","K","Na","ash","ADFN","Vomitoxine","Zearalenone","report_file",userid,"RFQ",lab_name) VALUES (' . "
null,null,
'{$sampleArr['Sample no']}','{$sampleArr['Analysis date']}','{$sampleArr['comment']}',
{$sampleArr['DryMatter']},
{$sampleArr['pH']},
{$sampleArr['CrudeProtein']},
{$sampleArr['SolubleProtein']},
{$sampleArr['ADFCP']},
{$sampleArr['NDFCP']},
{$sampleArr['ADF']},
{$sampleArr['NDF']},
{$sampleArr['Lignin']},
{$sampleArr['Non-fibreCarbohydrates']},
{$sampleArr['Starch']},
{$sampleArr['CrudeFat']},
{$sampleArr['TDN']},
{$sampleArr['DE']},
{$sampleArr['Calcium']},
{$sampleArr['Phosphorus']},
{$sampleArr['Magnesium']},
{$sampleArr['Potassium']},
{$sampleArr['Sodium']},
{$sampleArr['Ash']},
{$sampleArr['ADFN']},
{$sampleArr['Vomitoxine']},
{$sampleArr['Zearalenone']},
'{$sampleArr['pdf_file_report']}'::bytea,	
'david',{$sampleArr['RFQ']},'cumberland'
)";


        $res = $GLOBALS['pdo']->exec($sql);
    }

}

//end class


/* run this code from the command line when you want to add some data from pre exisiting cumberland labs forage sample results.
 *    $cls = new BagSample();
  $cls->reprocessOuter();
 * 
 * 
 */

class ReprocessForageSamples {

    function reprocessOuter() {
        $sql = "SELECT sample_number FROM nutrition.bag_analysis WHERE lab_name='cumberland'";

        $res = $GLOBALS['pdo']->query($sql);


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            print("sample_number" . $row['sample_number'] . " ");
            $labid_64 = base64_encode($row['sample_number']); //needs base 64 version
            //
            $xml = $this->downloadSampleXML($labid_64);
            $pdfData = $this->downloadSamplePDF($labid_64);
            //
            $sampleArr = $this->xmlSampletoArray($xml, $pdfData);
            $this->reprocessInner($sampleArr);
        }
    }

    //this is called when we want to add more data to currently stored data filed from original 
    function reprocessInner($sampleArr) {
        //now insert into db.
        if (!empty($sampleArr['RFQ'])) {
            $sql = "UPDATE nutrition.bag_analysis SET \"RFQ\"={$sampleArr['RFQ']} WHERE sample_number='{$sampleArr['Sample no']}'";
            $res = $GLOBALS['pdo']->exec($sql);
        }
    }

}

//end class
?>