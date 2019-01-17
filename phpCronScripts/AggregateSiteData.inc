<?php 
/**
 *  This class grabs data from CDN mobile and main site, HOL main site and webservice.
 * 
 *  It put all this data into DB table aggregate_site_data.
 * 
 *  First part does all sires and bovines in system.
 *  Second part goes up the tree from female bovines in system and looks for female parents and records them.
 * 
 * 
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

class AggregateSiteData {

    const generations = 10; //how far to go back.
    const secSincUpdateLimit = 604800; //604800; //604800 is a week. How many seconds until data in DB is considered out of date.
    //const secSincUpdateLimit = 14400; //14400 is 4 hours.

    //run this when you want to empty the db table first befofre running main code.    

    function resetRun() {
        self::clearDBtable();
        $this->main();
    }

    public function sse($request = null) {
        $this->main();
    }

    public function main() {
       
          //for testing
      /*
          $cowReg='HOCANF11141966';
          //$cowReg='HOCANM103631566'; //FEVER
          //
          //
          $HolsteinCanadaWebserviceJSON = new HolsteinCanadaWebserviceJSON;
          $jsonStrHolsteinCanadaWebservice = ($HolsteinCanadaWebserviceJSON->run($cowReg));
          print_r($jsonStrHolsteinCanadaWebservice);
          print("\nr\n\r");
          //
          $CDNMobileJSON = new CDNMobileJSON;
          $jsonStrMobileSite = ($CDNMobileJSON->run($cowReg));
          print_r($jsonStrMobileSite);
          print("\nr\n\r");
          //
          $CDNPedigreePageJSON = new CDNPedigreePageJSON;
          print_r($CDNPedigreePageJSON->run($cowReg));
          print("\nr\n\r");
          //
          $CDNGenomicsPageJSON = new CDNGenomicsPageJSON;
          print_r($CDNGenomicsPageJSON->run($cowReg));
          print("\nr\n\r");
          //
          $HolsteinCanadaConformationPageJSON = new HolsteinCanadaConformationPageJSON;
          print_r(($HolsteinCanadaConformationPageJSON->run($cowReg)));
          print("\nr\n\r");
          //    
          exit();
      */
      
        
        //first off we do all the current cows and bulls. 
        //then we run the code to do everything. It will skip the current ones 
        //when it does everything because they are up to date.
        
        $this->cowsAndSiresWebData(self::getCowRegArrayCurrent());

        $this->cowsAndSiresWebData(self::getCowRegArray());
        $this->allReallyDeadCows();
    }

    /**
     * This ignores when the last time main() was run and just runs any animals
     * submitted with reg number array.
     * 
     */
    public function forceAggregateUpdate(array $regNumArr) {
        foreach ($regNumArr as $cowReg) {
         $this->upsertSingleAnimal($cowReg, 'all');
        }
    }
    
    
   /*
    * Grabs all bovines (dead or alive) and sires in system, then grabs all web data it can for them
    * 
    */
    public function cowsAndSiresWebData($cowRegArray) {

        print('<h1>Started processing bovine data CDN website.</h1>' . "\n\r" . "\n\r");

//go through array for each cow 
        $cowArrayTotalCount = count($cowRegArray);
        $cowLoopCount = 0;
        foreach ($cowRegArray as &$cowReg) {
            SSE_Message::send_message($cowLoopCount, date('r',strtotime("now")) .' Doing ' . $cowReg . ' on Animal iteration ' . $cowLoopCount . ' of ' . $cowArrayTotalCount, round(($cowLoopCount / $cowArrayTotalCount ) * 100, 1)); //sse


            $sql = "SELECT full_reg_number,update_time FROM batch.aggregate_site_data WHERE full_reg_number='$cowReg' ORDER BY full_reg_number";
            $res = $GLOBALS['pdo']->query($sql);
            $row = $res->fetch(PDO::FETCH_ASSOC);

            //if secSincUpdateLimit seconds ago is greater than the last update, do an update.
            if ((strtotime("-".self::secSincUpdateLimit." second") > strtotime($row['update_time'])) OR ( $res->rowCount() == 0)) {

                $this->upsertSingleAnimal($cowReg, 'all');
            } else {
                print("Skipping $cowReg, already up to date. Last update: {$row['update_time']}\n\r\n\r");
            }


            $cowLoopCount++;
        }
        SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
        print('<h1>Finished processing bovine haplotype data from CDN website.</h1>' . "\n\r");
    }

    /*
     * Always run this one second.
     * //used for family tree calculation
     * //only grabs basic data from holstein canada for cows that are long dead.
     * 
     */

    public function allReallyDeadCows() {
        $sql = "SELECT full_reg_number,local_number from bovinemanagement.bovine ORDER BY id ";
        $res = $GLOBALS['pdo']->query($sql);
        $loopCount = 0;
        $loopCountTotal = $res->rowCount();
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            //first lookup the starting animal
            $full_reg_number = $row['full_reg_number'];
            SSE_Message::send_message($loopCount, 'Family Tree: Doing ' . $full_reg_number . ' on iteration ' . $loopCount . ' of ' . $loopCountTotal, round(($loopCount / $loopCountTotal ) * 100, 1)); //sse



            try {
                //For each cow, go back
                for ($i = 1; $i <= self::generations; $i++) {
                    print("Current Cow: $full_reg_number for Local Cow: {$row['local_number']} Generation: $i.\n");
                    if ($full_reg_number == 'HOCANF7846683') {
                        print("dsadsa");
                        exit();
                    }
                    //break out of loop when we don't have the dam's reg.
                    if (empty($full_reg_number)) {
                        throw new AggregateCowDoesntExistException("Error: can't find dam reg number of current cow, so cannot continue.");
                    }


                    $sql1 = "SELECT full_reg_number,hol_webservice_data->>'dam_full_reg_number' as dam_full_reg_number,update_time FROM batch.aggregate_site_data WHERE full_reg_number='$full_reg_number' limit 1";
                    $res1 = $GLOBALS['pdo']->query($sql1);
                    $row1 = $res1->fetch(PDO::FETCH_ASSOC);

                    //now check if we are out of date and it should be fetched again or if it has never been fetched.   
                    //NOTE: leave at one week, there is no reason to ever really change this.
                    if ((strtotime("-1 week") > strtotime($row1['update_time'])) OR ( $res1->rowCount() == 0)) {
                        print("DOING Upsert for: $full_reg_number\n\r");

                        //if it is a alive cow and needs updated, then we do a full update from web.
                        if (array_search($full_reg_number, self::getCowRegArray()) == true) {
                            $this->upsertSingleAnimal($full_reg_number, 'all');
                        } else {
                            $this->upsertSingleAnimal($full_reg_number, 'simple');
                        }
                    } else {
                        print("Skipping $full_reg_number, already up to date. Last update: {$row1['update_time']}\n\r\n\r");
                    }

                    //so at this point anyway you cut it, the animal is in DB. S
                    //So now find the dam of the animal who we just inserted or was already in DB.
                    //now query DB again, to get dams info we just inserted so we can continue up the tree.
                    $sql3 = "SELECT full_reg_number,hol_webservice_data->>'dam_full_reg_number' as dam_full_reg_number,update_time FROM batch.aggregate_site_data WHERE full_reg_number='$full_reg_number' limit 1";
                    $res3 = $GLOBALS['pdo']->query($sql3);
                    $row3 = $res3->fetch(PDO::FETCH_ASSOC);

                    if (!empty($row3['dam_full_reg_number'])) {
                        $full_reg_number = $row3['dam_full_reg_number'];
                    } else {
                        //no dam info, so we are done, break out.
                        throw new AggregateCowDoesntExistException("Error 2: can't find dam reg number of current cow ($full_reg_number), so cannot continue.");
                    }
                }//end generation loop
            } catch (Exception $e) {
                Print("EXCEPTION CAUGHT: skipping cow $e.");
            } catch (AggregateCowDoesntExistException $e) {
                Print("EXCEPTION CAUGHT: skipping cow $e.");
            }

            //add some spaces for legability.
            print("\n\r" . "\n\r" . "--------------------------------------" . "\n\r" . "\n\r" . "\n\r");
            $loopCount++;
        }//end while
        SSE_Message::send_message('CLOSE', 'Process complete', 100); //sse
    }

//end main

    /**
     * used to add a single animal to agregate_site_data table, 
     * ie when we add a sire. Also used for every insert.
     * @param type $cowReg
     */
    public function upsertSingleAnimal($cowReg, $mode = 'all') {

        //parse the page and return json string.

        if ($mode == 'all') {

            ////////////// HOLSTEIN CANADA ///////////////////////
            $HolsteinCanadaWebserviceJSON = new HolsteinCanadaWebserviceJSON;
            $jsonStrHolsteinCanadaWebservice = ($HolsteinCanadaWebserviceJSON->run($cowReg));
            //check if webservice returned nothing. if it did, don't even bother looking on other sites, cow does not exist.
            //array returns a null text string......bloody php.
            if (($jsonStrHolsteinCanadaWebservice) == 'null') {
                try {
                    throw new AggregateCowDoesntExistException();   //just to record we couldn't find it.
                } catch (AggregateCowDoesntExistException $e) {

                    //handle exception by setting everything to null.
                    $jsonStrHolsteinCanadaWebservice = null;
                    $jsonStrMobileSite = null;
                    $jsonStrPedigreePage = null;
                    $CDNGenomicsPageJSON = null;
                    $jsonStrGenomicsPage = null;
                    $jsonStrHolsteinCanadaConformationPage = null;
                }
                //cow is alive and well, so contact other services and grab data.
            } else {
                $HolsteinCanadaConformationPageJSON = new HolsteinCanadaConformationPageJSON;
                $jsonStrHolsteinCanadaConformationPage = ($HolsteinCanadaConformationPageJSON->run($cowReg));

                ////////////// CDN //////////////////////////////////
                $CDNMobileJSON = new CDNMobileJSON;
                $jsonStrMobileSite = ($CDNMobileJSON->run($cowReg));

                $CDNPedigreePageJSON = new CDNPedigreePageJSON;
                $jsonStrPedigreePage = ($CDNPedigreePageJSON->run($cowReg));

                $CDNGenomicsPageJSON = new CDNGenomicsPageJSON;
                $jsonStrGenomicsPage = ($CDNGenomicsPageJSON->run($cowReg));
            }
        }
        //used for old dead cows, don't need all the info and if there dead it doesn't change (except for CDN genetic model data).
        elseif ($mode == 'simple') {
            $jsonStrMobileSite = null;
            $jsonStrPedigreePage = null;
            $CDNGenomicsPageJSON = null;
            $jsonStrGenomicsPage = null;
            //
            $HolsteinCanadaWebserviceJSON = new HolsteinCanadaWebserviceJSON;
            $jsonStrHolsteinCanadaWebservice = ($HolsteinCanadaWebserviceJSON->run($cowReg));
            //
            $jsonStrHolsteinCanadaConformationPage = null;
        } else {
                throw new Exception("Error: Mode not selected for upsertSingleAnimal.");
        }

        //////////////////now put ALL in DB /////////////////////////
        $result=$this->insertDataIntoDB($cowReg, $jsonStrMobileSite, $jsonStrPedigreePage, $jsonStrGenomicsPage, $jsonStrHolsteinCanadaWebservice, $jsonStrHolsteinCanadaConformationPage);
        return $result;
    }

    //grab all sires and alive and dead cows in system.
    private function getCowRegArray() {
        //get an array of all current female bovines reg numbers.
        $sql = "SELECT full_reg_number FROM bovinemanagement.bovine UNION SELECT full_reg_number FROM bovinemanagement.sire"; //could be sped up, but this is all inclusive and does dead cows.


        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $cowRegArray[] = $row['full_reg_number'];
        }
        return $cowRegArray;
    }
    
     //only updates currently alive cows
    private function getCowRegArrayCurrent() {
        //get an array of all current female bovines reg numbers.
        $sql = "SELECT full_reg_number FROM bovinemanagement.bovine WHERE death_date is null UNION SELECT full_reg_number FROM bovinemanagement.semen_straw_curr_summary"; //could be sped up, but this is all inclusive and does dead cows.


        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $cowRegArray[] = $row['full_reg_number'];
        }
        return $cowRegArray;
    }

    /*
      insert the DATA into DB as JSON.
     */

    private function insertDataIntoDB($full_reg_number, $jsonStrMobileSite, $jsonStrPedigreePage, $jsonStrGenomicsPage, $jsonStrHolsteinCanadaWebservice, $jsonStrHolsteinCanadaConformationPage) {
//only insert data if it were found.
        if (!empty($full_reg_number)) {

            //escape json strings.
            $jsonStrMobileSite = pg_escape_string($jsonStrMobileSite);
            $jsonStrPedigreePage = pg_escape_string($jsonStrPedigreePage);
            $jsonStrGenomicsPage = pg_escape_string($jsonStrGenomicsPage);
            $jsonStrHolsteinCanadaWebservice = pg_escape_string($jsonStrHolsteinCanadaWebservice);
            $jsonStrHolsteinCanadaConformationPage = pg_escape_string($jsonStrHolsteinCanadaConformationPage);


//insert collected info into the db.
            // Open a transaction
            try {
                $res = $GLOBALS['pdo']->beginTransaction();

//delete any old records (only one allowed).
                $sql = "DELETE FROM batch.aggregate_site_data WHERE full_reg_number='$full_reg_number'";
                $res = $GLOBALS['pdo']->exec($sql);

//insert collected info into the db.	

                $sql2 = "INSERT INTO batch.aggregate_site_data (full_reg_number, cdn_mobile_data,cdn_pedigree_data,cdn_genomics_data,hol_webservice_data, hol_conformation_data ) VALUES ('$full_reg_number',NULLIF('$jsonStrMobileSite', '')::jsonb,NULLIF('$jsonStrPedigreePage', '')::jsonb,NULLIF('$jsonStrGenomicsPage', '')::jsonb,NULLIF('$jsonStrHolsteinCanadaWebservice', '')::jsonb, NULLIF('$jsonStrHolsteinCanadaConformationPage', '')::jsonb)";

                $res = $GLOBALS['pdo']->exec($sql2);

// determine if the commit or rollback

                $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }

            return $res;
        }
        return -1;
    }

   

    /* returns datetime of last DB update, thus assumes main function completed then */

    public function lastRun() {
        $sql = "SELECT max(update_time) as max_update_time FROM batch.aggregate_site_data";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        return strtotime($row['max_update_time']);
    }

}

//end class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * 
 * uses webservice to get data for an animal from holstein canada.
 * returns json
 */
class HolsteinCanadaWebserviceJSON {

    public function run($full_reg_number) {


        $a = $this->SOAPServiceDataIn(array($full_reg_number));
        ;
        $x = $this->SOAPServiceProcessData($a, array($full_reg_number));
        // print_r($x);

        return json_encode($x["$full_reg_number"]);
    }

    /**
     * supports one or more reg numbers as input, 
     * @param array $full_reg_numberArr
     * @return array
     */
    public function SOAPServiceDataIn(array $full_reg_numberArr) {
        if (!empty($full_reg_numberArr)) {

            $queryStr = '';
            // make like this:  <animal xmlns="" breed="HO" country="CAN" sex="F" regno="7906641" />
            foreach ($full_reg_numberArr as $reg) {
                $queryStr = $queryStr . '  <animal xmlns="" breed="' . substr($reg, 0, 2) . '" country="' . substr($reg, 2, 3) . '" sex="' . substr($reg, 5, 1) . '" regno="' . substr($reg, 6, strlen($reg)) . '" />' . "\n\r";
            }


            $requestParams = array(
                'argstrUserId' => $GLOBALS['config']['HOLSTEIN_CANADA']['CDX_USERID'],
                'argstrPassword' => $GLOBALS['config']['HOLSTEIN_CANADA']['CDX_PASSWORD'],
                'argstrXmlDocument' => '<?php xml version="1.0" encoding="utf-8" standalone="yes"?>
             <herdbook-request xmlns="http://www.holstein.ca/HBControlFileSchema" 
                  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
                  xsi:schemaLocation="http://www.holstein.ca/HBControlFileSchema
                  http://www.holstein.ca/WebServices/XMLSchema/HBControlFile.xsd">
	              ' . $queryStr . '	
             </herdbook-request>'
            );
            
            ini_set("soap.wsdl_cache_enabled", "0"); //turn off caching, needed if holstein changes wsdl, which they seem to have.
            $client = new SoapClient('https://cdx.holstein.ca/cdxservice.asmx?wsdl');
          
            //DEBUG: list functions
            //$functions = $client->__getFunctions();
            //var_dump ($functions);
      
            try {           
               $response = $client->HerdBookByControlFile($requestParams);
            } catch (SoapFault $fault) {
                print(" ERROR SOAP: " . $fault->faultcode . "-" . $fault->faultstring . ".");
            }

            if (empty($response)) {
                throw new Exception("Error: No Response Obj from Holstein Canada Web Service.  ");
            }
            
            
            
            //format and return array
            $xml = simplexml_load_string($response->HerdBookByControlFileResult, "SimpleXMLElement", LIBXML_NOCDATA);
            $json = json_encode($xml);
            $json = str_replace('@attributes', 'at_attributes', $json); //the @ symbol seems to cause php problems, just get rid of it.
            $array = json_decode($json, TRUE);
            return ($array);
        }
    }

    /*
     * take data returned by soap service and process it into something we want.
     * returns an array of animals with regnumber as key
     */

    public function SOAPServiceProcessData(array $SOAPArr, array $full_reg_numberArr) {
        $allArr = array();

        //check first that animal array was returned or cow does't exist.
        if (empty($SOAPArr)) {
            return null;
        }
        if (empty($SOAPArr['animal'])) {
            return null;
        }

        //then we multiple animal situation.   
        if (!empty($SOAPArr['animal'][0])) {
            //loop through results
            $i = 0;
            foreach ($SOAPArr['animal'] as $aa) {
                $t = $this->SOAPServiceCreateRetArray($aa);
                $allArr[$full_reg_numberArr[$i]] = $t;         //ALERT: this has never been tested, is array in right order?
                $i++;
            }
        }
        //single animal sitiuation (there is nothing to loop through, just pass it)
        else {
            $t = $this->SOAPServiceCreateRetArray($SOAPArr['animal']);
            $allArr[$full_reg_numberArr[0]] = $t;
        }

        return $allArr;
    }

    /*
     * 
     *  creates output array.
     */

    private function SOAPServiceCreateRetArray($a) {
        $retArray = array();
        $retArray['raw_data'] = $a; //raw data.

        try {
            //we need all parts of reg number to be valid
            if (!empty($a['primBreed']['breed']) AND ( !empty($a['at_attributes']['country'])) AND ( !empty($a['at_attributes']['sex'])) AND ( !empty($a['at_attributes']['numberID']))) {
                $retArray['full_reg_number'] = $a['primBreed']['breed'] . $a['at_attributes']['country'] . $a['at_attributes']['sex'] . $a['at_attributes']['numberID'];
            } else {
                throw new AggregateCowDoesntExistException("Holstein Canada Cow SOAP Service Can't find cow.");
            }

            $retArray['full_name'] = (!empty($a['animalName']) ? $a['animalName'] : '');
            $retArray['birth_date'] = (!empty($a['birthDate']) ? $a['birthDate'] : '');
            $retArray['local_number'] = (!empty($a['ID']['earID']) ? $a['ID']['earID'] : '');
            //
            $retArray['reg_date'] = (!empty($a['RegDate']) ? $a['RegDate'] : '');
            $retArray['reg_status'] = (!empty($a['RegStat']) ? $a['RegStat'] : '');

            //we need all parts of reg number to be valid
            if (!empty($a['sireBreed']['breed']) AND ( !empty($a['sireID']['at_attributes']['country'])) AND ( !empty($a['sireID']['at_attributes']['sex'])) AND ( !empty($a['sireID']['at_attributes']['numberID']))) {
                $retArray['sire_full_reg_number'] = $a['sireBreed']['breed'] . $a['sireID']['at_attributes']['country'] . $a['sireID']['at_attributes']['sex'] . $a['sireID']['at_attributes']['numberID'];
            } else {
                $retArray['sire_full_reg_number'] = '';
            }

            //we need all parts of reg number to be valid
            if (!empty($a['damBreed']['breed']) AND ( !empty($a['damID']['at_attributes']['country'])) AND ( !empty($a['damID']['at_attributes']['sex'])) AND ( !empty($a['damID']['at_attributes']['numberID']))) {
                $retArray['dam_full_reg_number'] = $a['damBreed']['breed'] . $a['damID']['at_attributes']['country'] . $a['damID']['at_attributes']['sex'] . $a['damID']['at_attributes']['numberID'];
            } else {
                $retArray['dam_full_reg_number'] = '';
            }

            
            //do a check for really old data that is bad, ie with [], for example: HOUSAF5854779
            //set to blank, if so.
            if (is_array($a['FinalScore'])) { $a['FinalScore']='';}
            if (is_array($a['FinalClass'])) { $a['FinalClass']='';}

            if (!empty($a['FinalClass'])) {
                $retArray['class_all'] = $a['FinalClass'];
                $cls = (explode("-", $a['FinalClass']));
                $retArray['class'] = $cls[0];
                $retArray['score'] = $a['FinalScore'];
            } else {
                $retArray['class_all'] = '';
                $retArray['class'] = '';
                $retArray['score'] = '';
            }
        } catch (AggregateCowDoesntExistException $e) {
            return null;
        }


        return $retArray;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Takes a reg number and turns it into JSON 
 * uses xpath to parse holstein canada conformation page and grab all data and returns it in JSON.
 */

class HolsteinCanadaConformationPageJSON {

    public function run($full_reg_number) {
        //$full_reg_number='HOCANF4013805';  //for testing

        $pageContent = $this->getHolsteinCanadaConformationPageData($full_reg_number);
        $JSON = $this->parseHolsteinCanadaConformationPagePageData($pageContent, $full_reg_number);
        return $JSON;
    }

    private function getHolsteinCanadaConformationPageData($animalRegNum) {


        $link = "http://www.holstein.ca/en/OnlineServices/ReportViewer/?r=ais&p=$animalRegNum~KG~BCA~0"; //generic page that redirects to cow

        $ch = curl_init($link);
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

// some curl option to do with how data is returned.
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60);
        curl_setopt($ch, CURLOPT_HEADER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36'); //holstein canada does not work with out a modern user agent.
// Grab the page and save the contents in the $data variable
        $data = curl_exec($ch);
        curl_close($ch);
        //search for holstein canada DB key
        if (preg_match_all('/AIS\?AnimalId=(?<AnimalId>(\d*))&/', $data, $match)) {
            //we have sucessfully found holstein canada's database key, now do the query we want.

            $link1 = 'https://www.holstein.ca/en/AIS/Conformation?animalId=' . $match['AnimalId'][0];  //holstein canada conformation page
            $ch1 = curl_init($link1);
            if (!$ch1) {
                die("Cannot allocate a new PHP-CURL handle #1");
            }
            curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch1, CURLOPT_TIMEOUT, 60);
            curl_setopt($ch1, CURLOPT_HEADER, true);
            curl_setopt($ch1, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36'); //holstein canada does not work with out a modern user agent.
            $data = curl_exec($ch1);
            curl_close($ch1);

            return $data;
        } else {
            throw new Exception("Can't find holstein Canada DB key, thus can't download the data. They probably changed their website again.");
        }
    }

    private function parseHolsteinCanadaConformationPagePageData($pageContent, $full_reg_number) {

        $dom = new DOMDocument();
        libxml_use_internal_errors(true);
        $dom->loadHTML($pageContent);
        //
        libxml_clear_errors();
        $xpath = new DOMXpath($dom);

        //run parses. 
        $classificationDateArr = $this->xPathToArray_ClassificationDate($xpath);
        $conformationSummaryArr = $this->xPathToArray_ConformationSummary($xpath);
        $conformationIndividualArr = $this->xPathToArray_ConformationIndividual($xpath);

//create array to turn into JSO
        $retArray = array();
        $retArray['full_reg_number'] = $full_reg_number;
        $retArray['classifications'] = $classificationDateArr;
        $retArray['summary_traits'] = $conformationSummaryArr;
        $retArray['individual_traits'] = $conformationIndividualArr;

        return json_encode($retArray);
    }

    private function xPathToArray_ClassificationDate($xpath) {

        $values = array();

        $row1 = $xpath->query('//div[@id="AisConformationHistory"]/table/tbody/tr[*]/td[1]'); //date of classificaton
        $row2 = $xpath->query('//div[@id="AisConformationHistory"]/table/tbody/tr[*]/td[2]'); //score of classification

        foreach ($row1 as $key => $value) {
            $arr = array();
            if ((!empty($row1[$key]->textContent)) AND ( !empty($row2[$key]->textContent))) {
                $arr['ClassificationDate'] = trim($row1[$key]->textContent);
                $arr['ClassificationScore'] = trim($row2[$key]->textContent);
            }
            if (!empty($arr['ClassificationDate'])) {
                $arr['ClassificationDate'] = (new DateTime($arr['ClassificationDate']))->getTimestamp(); //convert valid
                $values[] = $arr; //add to array for returning
            }
        } //end foreach

        return $values;
    }

    private function xPathToArray_ConformationSummary($xpath) {
        $values = array();


        $row1 = $xpath->query('//div[@id="AisConformation"]/table/tbody/tr[position() >= 1]/th[position() = 1 or position() = 3]');
        $row2 = $xpath->query('//div[@id="AisConformation"]/table/tbody/tr[position() >= 1]/th[position() = 2  or position() = 4]');
        foreach ($row1 as $key => $value) {
            if (!empty($row1[$key]->textContent)) {
                $values[trim($row1[$key]->textContent)] = trim(preg_replace('/\s+/', '', $row2[$key]->textContent));
            }
        }

        return($values);
    }

    private function xPathToArray_ConformationIndividual($xpath) {

        $values = array();

        $row1 = $xpath->query('//div[@id="AisConformation"]/table/tbody/tr[position() >= 3]/td[position() = 1 or position() = 3]');
        $row2 = $xpath->query('//div[@id="AisConformation"]/table/tbody/tr[position() >= 3]/td[position() = 2  or position() = 4]');
        foreach ($row1 as $key => $value) {
            if (!empty($row1[$key]->textContent)) {
                $values[trim($row1[$key]->textContent)] = trim(preg_replace('/\s+/', '', $row2[$key]->textContent));
            }
        }
        return $values;
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * takes reg number as input and parses CDN main site pedigree page
 * uses xpath to parse CDN mobile page and grab all data and returns it in JSON.
 */
class CDNGenomicsPageJSON {

    public function run($full_reg_number) {
        //$full_reg_number='HOCANF4013805';  //for testing

        $pageContent = $this->getCDNGenomicsPageData($full_reg_number);
        $JSON = $this->parseCDNGenomicsPagePageData($pageContent, $full_reg_number);
        return $JSON;
    }

    private function getCDNGenomicsPageData($animalRegNum) {

//break up the reg number.
        $regArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($animalRegNum);

        $link = "https://www.cdn.ca/query/detail_genomics.php?breed={$regArray['breed']}&country={$regArray['country']}&sex={$regArray['sex']}&regnum={$regArray['number']}"; //pedigree page, main site.
//grab the weather data for the farm weather station from weather underground
        $ch = curl_init($link);
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

// some curl option to do with how data is returned.
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60);

// Grab the page and save the contents in the $data variable
        $data = curl_exec($ch);
// close the connection
        curl_close($ch);

        return $data;
    }

    private function parseCDNGenomicsPagePageData($pageContent, $full_reg_number) {

        $dom = new DOMDocument();
        libxml_use_internal_errors(true);
        $dom->loadHTML($pageContent);
        //
        libxml_clear_errors();
        $xpath = new DOMXpath($dom);

        $genotypePanelArr = $this->xPathToArray_GenotypePanel($xpath);
        $retArray = array();
        $retArray['genotype_panel'] = $genotypePanelArr['genotype_panel'];

        return json_encode($retArray);
    }

    /**
     *  Returns what type of genoptype panel was used
     * 
     * @param type $xpath
     * @return array
     */
    private function xPathToArray_GenotypePanel($xpath) {
        $retArr = array();

        $row = $xpath->query('//div[@class="AnimalDetails"]/table/tr[1]/td/table/tr[1]/td[2]');

        $retArr['genotype_panel'] = !empty(trim($row[1]->textContent)) ? trim($row[1]->textContent) : null;  //3K or 10K, etc.

        return $retArr;
    }

}

//end class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * takes reg number as input and parses CDN main site pedigree page
 * uses xpath to parse CDN mobile page and grab all data and returns it in JSON.
 */
class CDNPedigreePageJSON {

    public function run($full_reg_number) {
        //$full_reg_number='HOCANF4013805';  //for testing

        $pageContent = $this->getCDNPedigreePageData($full_reg_number);
        $JSON = $this->parseCDNPedigreePagePageData($pageContent, $full_reg_number);
        return $JSON;
    }

    private function getCDNPedigreePageData($animalRegNum) {

//break up the reg number.
        $regArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($animalRegNum);

        $link = "https://www.cdn.ca/query/detail_pedigree.php?breed={$regArray['breed']}&country={$regArray['country']}&sex={$regArray['sex']}&regnum={$regArray['number']}"; //pedigree page, main site.
//grab the weather data for the farm weather station from weather underground
        $ch = curl_init($link);
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

// some curl option to do with how data is returned.
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60);

// Grab the page and save the contents in the $data variable
        $data = curl_exec($ch);
// close the connection
        curl_close($ch);

        return $data;
    }

    private function parseCDNPedigreePagePageData($pageContent, $full_reg_number) {

        $dom = new DOMDocument();
        libxml_use_internal_errors(true);
        $dom->loadHTML($pageContent);
        //
        libxml_clear_errors();
        $xpath = new DOMXpath($dom);
///////////
        $haplotypeArr = $this->xPathToArray_Haplotypes($xpath);
        $inbreedingArr = $this->xPathToArray_Inbreeding($xpath);

        $retArray = array();
        $retArray['inbreeding'] = $inbreedingArr;
        $retArray['haplotypes'] = $haplotypeArr;

        return json_encode($retArray);
    }

    /**
     *  Returns inbreeding and reliability.
     * 
     * @param type $xpath
     * @return array
     */
    private function xPathToArray_Inbreeding($xpath) {
        $retArr = array();

        $row = $xpath->query('//div[@class="AnimalDetails"]/table/tr[1]/td/table/tr[2]/td[4]');
        preg_match_all("/\s?(?<inbreeding>\d+.\d{1,2})%\s?/", $row[0]->textContent, $matches);  //remove percent, verify it is a number.
        $retArr['inbreeding'] = !empty($matches['inbreeding'][0]) ? $matches['inbreeding'][0] : null;


        $row1 = $xpath->query('//div[@class="AnimalDetails"]/table/tr[1]/td/table/tr[2]/td[5]');
        preg_match_all("/\s?(?<reliability>\d{1,2})%\s?/", $row1[0]->textContent, $matches2);  //remove percent, verify it is a number.

        $retArr['reliability'] = !empty($matches2['reliability'][0]) ? $matches2['reliability'][0] : null;

        return $retArr;
    }

    /**
     * Because CDN doesn't have a clue how to make an HTML page with CSS
     * A single TD element contains all the haplotype stuff,
     * we use xpath to get to that element and then use preg match to search the 
     * text strings to get the haplotype info.
     * @param type $xpath
     * @return array
     * 
     */
    private function xPathToArray_Haplotypes($xpath) {

        $values = array();
        $row = $xpath->query('//div[@class="AnimalDetails"]/table/tr[1]/td/table/tr[2]/td[2]');

        //do HH haplotypes 
        $retArr = array();
        preg_match_all("/HH:\s(?<HH1>\d{1,2})%,\s(?<HH2>\d{1,2})%,\s(?<HH3>\d{1,2})%,\s(?<HH4>\d{1,2})%,\s(?<HH5>\d{1,2})%/", $row[0]->textContent, $matches);
        if ($matches[0] != null) {
            $retArr['HH1'] = $matches['HH1'][0];
            $retArr['HH2'] = $matches['HH2'][0];
            $retArr['HH3'] = $matches['HH3'][0];
            $retArr['HH4'] = $matches['HH4'][0];
            $retArr['HH5'] = $matches['HH5'][0];
        }

        //now do HCD1
        preg_match_all("/HCD:\s(?<HCD>(\d{1,2}))%/", $row[0]->textContent, $matchesHCD);
        $retArr['HCD'] = !empty($matchesHCD['HCD'][0]) ? $matchesHCD['HCD'][0] : null;


        return $retArr;
    }

}

//end class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Takes a reg number and turns it into JSON 
 * uses xpath to parse CDN mobile page and grab all data and returns it in JSON.
 */

class CDNMobileJSON {

    public function run($full_reg_number) {
        //$full_reg_number='HOCANF4013805';  //for testing

        $pageContent = $this->getCDNMobileData($full_reg_number);
        $JSON = $this->parseCDNMobilePageData($pageContent, $full_reg_number);
        return $JSON;
    }

    private function getCDNMobileData($animalRegNum) {

//break up the reg number.
        $regArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($animalRegNum);

        $link = "https://mobile.cdn.ca/bull-result.php?breed={$regArray['breed']}&country={$regArray['country']}&sex={$regArray['sex']}&regnum={$regArray['number']}"; //mobile site.
//grab the weather data for the farm weather station from weather underground
        $ch = curl_init($link);
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

// some curl option to do with how data is returned.
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60);

// Grab the page and save the contents in the $data variable
        $data = curl_exec($ch);
// close the connection
        curl_close($ch);

        return $data;
    }

    private function parseCDNMobilePageData($pageContent, $full_reg_number) {

        $dom = new DOMDocument();
        libxml_use_internal_errors(true);
        $dom->loadHTML($pageContent);
        //
        libxml_clear_errors();
        $xpath = new DOMXpath($dom);
///////////

        $geneticEvalArr = $this->xPathToArray_GeneticEval($xpath);
        $scoreCardArr = $this->xPathToArray_ScoreCard($xpath);
        $descriptiveArr = $this->xPathToArray_Descriptive($xpath);
        $functionalArr = $this->xPathToArray_Functional($xpath);
        $dateArr = $this->xPathToArray_Date($xpath);
        $semenCodeArr = $this->xPathToArray_SemenCode($xpath);
        $geneticCodesArr = $this->xPathToArray_GeneticCodes($xpath);
        $maleShortNameArr= $this->xPathToArray_MaleShortName($xpath);
        $fullNameArr=$this->xPathToArray_FullName($xpath);
//////////
//create array to turn into JSON
        $retArray = array();
        $retArray['full_reg_number'] = $full_reg_number;
        $retArray['date'] = $dateArr['date'];
        $retArray['semen_code'] = $semenCodeArr['semen_code'];
        $retArray['genetic_codes']=$geneticCodesArr;
        $retArray['male_short_name'] = $maleShortNameArr['male_short_name'];
        $retArray['full_name'] = $fullNameArr['male_full_name'];
        $retArray['genetic_index'] = $geneticEvalArr;
        $retArray['score_card'] = $scoreCardArr;
        $retArray['descriptive_traits'] = $descriptiveArr;
        $retArray['functional_traits'] = $functionalArr;

//print(json_encode($retArray));

        return json_encode($retArray);
    }
    
    
    //codes such as polled, ET, etc.
    private function xPathToArray_GeneticCodes($xpath) {

        $out = array();
        $row = $xpath->query('//*[@id="MainContent"]/div/article/div[1]/div/div/div[2]/p');
      
       if (isset( $row[0]->textContent)) {
           $arrayCodes = preg_split('/\s+/', trim($row[0]->textContent));
           $out= ($arrayCodes);
       }
       else {
           $out = null;
       }
       return $out;      
    }
    
    
    
    //semen code for males.
    private function xPathToArray_SemenCode($xpath) {

        $out = array();
        $row = $xpath->query('//div[@class="HeadingBlock"]/div/div/div/div[2]/p[2]');
      
       if (isset( $row[0]->textContent)) {
              preg_match_all("/\s?(?<semencode>[0-9]{4}[a-zA-Z]{2}[0-9]{5})\s?/", $row[0]->textContent, $matches);  //check semen code is valid
       if (!empty($matches['semencode'])) {
          $out['semen_code'] = $matches['semencode'][0];
       }}
       else {
           $out['semen_code'] = null;
       }
       return $out;      
    }
    
     //semen name for males.
    private function xPathToArray_MaleShortName($xpath) {

        $out = array();
        $row = $xpath->query('//div[@class="HeadingBlock"]/div/div/div/div[2]/p[1]');
       if  (isset( $row[0]->textContent )) {
       if (!empty(($row[0]->textContent))) {
          $out['male_short_name'] = trim(($row[0]->textContent));
       }}
       else {
           $out['male_short_name'] = null;
       }
       return $out;      
    }
    
     //semen full name for males.
    private function xPathToArray_FullName($xpath) {

        $out = array();
        $row = $xpath->query('//div[@class="HeadingBlock"]/div/h1');
       if  (isset( $row[0]->textContent )) {
       if (!empty(($row[0]->textContent))) {
          $out['male_full_name'] = trim(($row[0]->textContent));
       }}
       else {
           $out['male_full_name'] = null;
       }
       return $out;      
    }
    

    private function xPathToArray_Date($xpath) {

        $values = array();
        $row = $xpath->query('//h2[@class="AlignC HasBackground"][1][normalize-space()]');
        foreach ($row as $value) {
            $values[] = trim($value->textContent);
        }
        $out = array();
        $a = (explode('-', $values[0]));
        if (trim($a[0]) == 'Genetic Evaluations') {
            $b = explode('*', $a[1]);
            try {
            $date = (new DateTime($b[1] . ' 1 20' . trim($b[0])))->getTimestamp();
            }
             catch (Exception $e) {
            var_dump(get_class($e));
        }
            $out['date'] = $date;
        } else {
            $out['date'] = '';
        }
        return $out;
    }

    private function xPathToArray_GeneticEval($xpath) {
        $values = array();
        $row = $xpath->query('//div[@class="InfoBox CF"][2]/div/p[normalize-space()]');
        foreach ($row as $value) {
            $a = trim(preg_replace('/\s+/', '', $value->textContent));
            $a = str_replace(chr(194) . chr(160), '', $a); //removes &nbsp;
            $values[] = $a;
        }

        $values[0] = 'Index Type:' . array_values($values)[0]; //text is juist EBV or GEBV, so make descriptive. 
        foreach ($values as $item) {
            // $item=str_replace('EBV','Index Type:EBV',$item);
            $a = explode(':', $item);
            $out[$a[0]] = trim(!empty($a[1]) ? $a[1] : '');
        }

         //remove $ from prodoll and remove % from reliability
        if (!empty($out['Pro$'])) {
        $out['Pro$']=str_replace('$','',$out['Pro$']);
        $out['Pro$'] = is_numeric($out['Pro$']) ? $out['Pro$'] : '';
        }
        if (!empty($out['Reliability'])) {
        $out['Reliability']=str_replace('%','',$out['Reliability']);
        $out['Reliability'] = is_numeric($out['Reliability']) ? $out['Reliability'] : '';
        }
       
        
        
        return($out);
    }

    private function xPathToArray_ScoreCard($xpath) {
        $values = array();
        $row1 = $xpath->query('//div[@class="InfoBox MB0 CF"]/table[2]//tbody/tr/td[contains(concat(" ", normalize-space(@class), " "), " FirstCol ")]');
        $row2 = $xpath->query('//div[@class="InfoBox MB0 CF"]/table[2]//tbody/tr/td[contains(concat(" ", normalize-space(@class), " "), " SecondCol ")]');
        foreach ($row1 as $key => $value) {
            $values[trim($row1[$key]->textContent)] = trim($row2[$key]->textContent);
        }
        return $values;
    }

    private function xPathToArray_Descriptive($xpath) {
        $values = array();
        $row1 = $xpath->query('//div[@class="InfoBox MT1 MB0 CF"]/table[2]/tr/td[contains(concat(" ", normalize-space(@class), " "), " FirstCol ")]');
        $row2 = $xpath->query('//div[@class="InfoBox MT1 MB0 CF"]/table[2]/tr/td[contains(concat(" ", normalize-space(@class), " "), " SecondCol ")]');
        foreach ($row1 as $key => $value) {
            $values[trim($row1[$key]->textContent)] = trim(preg_replace('/\s+/', '', $row2[$key]->textContent));
        }
        return $values;
    }

    private function xPathToArray_Functional($xpath) {
        $values = array();
        $row1 = $xpath->query('//div[@class="InfoBox CF"][5]/table[2]/tr/td[contains(concat(" ", normalize-space(@class), " "), " FirstCol ")]');
        $row2 = $xpath->query('//div[@class="InfoBox CF"][5]/table[2]/tr/td[contains(concat(" ", normalize-space(@class), " "), " SecondCol ")]');
        foreach ($row1 as $key => $value) {
            $values[trim($row1[$key]->textContent)] = trim(preg_replace('/\s+/', '', $row2[$key]->textContent));
        }
        return $values;
    }

}

class AggregateCowDoesntExistException extends Exception {
    
}

//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new AggregateSiteData();
    $cls->main();
} else {
//nothing.
}
?>
