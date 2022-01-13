<?php 

/*
 * This class is responsible for all contact with Holstein Canada website and webservices.
 * 
 * There are three ways to contact site as of March 2016:
 * 1. get request to animal query search page, which returns JSON 
 *     (NOTE: there are other ajax calls on site that return JSON, 
 *      but data returned doesn't seem to be as good as animal query search)
 * 2. holstein canada soap webservice
 * 3. scrape website.
 */

/* What information do we want to get from Holstein Canada?????
 * We should only get information from Holstein Canada that it is actually 
 * a source of, CDN and Holstein Canada and DHI have a lot of overlap.
 * Things like:
 *              classification data
 *              classification score
 *              name/reg, name dam/reg dam,name sire/ reg sire
 *              birth date
 *              herd #
 *              parentage/registration status.
 * 
 *      Everything else should be from CDN, because it is genetic in nature
 * 
 * 
 */

/*   soap webservice provides:
 *   Provides everything above except classification data and possible parentage status.
 *   Use it as our main data collector.
 * 
 */

/* LIST FOR CDN:
 * geno_test
 * reliability
 * lpi
 * prodoll
 * 
 * 
 */

class HolsteinCanadaHelper {

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

            $client = new SoapClient('https://cdx.holstein.ca/cdxservice.asmx?wsdl');
            try {
                $response = $client->HerdBookByControlFile($requestParams);
            } catch (SoapFault $fault) {
                print(" ERROR SOAP" . $fault->faultcode . "-" . $fault->faultstring . ".");
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
    public function SOAPServiceProcessData(array $SOAPArr) {
        $allArr = array();

        if (!empty($SOAPArr)) {

            //then we multiple animal situation.   
            if (!empty($SOAPArr['animal'][0])) {
                //loop through results
                foreach ($SOAPArr['animal'] as $aa) {
                    $t = $this->SOAPServiceCreateRetArray($aa);
                    $allArr[$t['full_reg_number']] = $t;
                }
            }
            //single animal sitiuation (there is nothing to loop through, just pass it)
            else {
                $t = $this->SOAPServiceCreateRetArray($SOAPArr['animal']);
                $allArr[$t['full_reg_number']] = $t;
            }

            return $allArr;
        }
    }

    /*
     * 
     *  creates output array.
     */

    private function SOAPServiceCreateRetArray($a) {
        $retArray = array();

        //we need all parts of reg number to be valid
        if (!empty($a['primBreed']['breed']) AND ( !empty($a['at_attributes']['country'])) AND ( !empty($a['at_attributes']['sex'])) AND ( !empty($a['at_attributes']['numberID']))) {
            $retArray['full_reg_number'] = $a['primBreed']['breed'] . $a['at_attributes']['country'] . $a['at_attributes']['sex'] . $a['at_attributes']['numberID'];
        } else {
            throw new CowDoesntExistException("Holstein Canada Cow SOAP Service Can't find cow.");
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

        return $retArray;
    }

   public static function createHolsteinCanadaQuery($full_reg_number) {
//break into component parts:
        $breed = substr($full_reg_number, 0, 2);
        $country = substr($full_reg_number, 2, 3);
        $sex = substr($full_reg_number, 5, 1);
        $number = substr($full_reg_number, 6, strlen($full_reg_number));
        //don't change "&" to &amp; here, will not work.
        return $queryString = "http://www.holstein.ca/en/OnlineServices/ReportViewer/?r=ais&p=$breed$country$sex$number~KG~BCA~0";
    }

    static function createHolsteinCanadaQueryGeneticsTab($full_reg_number) {
        return $queryString = "http://www.holstein.ca/en/OnlineServices/ReportAnimalGenetics?RegNo=$full_reg_number&La=KG&ME=BCA";
    }

    /* get all the data from header and different tabs and returns as one array */

    public function main($full_reg_number) {



        $retArray = null;
        print("REG:$full_reg_number\n\r");
        $a = $this->SOAPServiceDataIn(array($full_reg_number));

        $x = $this->SOAPServiceProcessData($a);
        // print_r($x);
  
        $retArray = $x["$full_reg_number"];

        /*
          $retArray = $this->headerPortion($full_reg_number);
          $retArray = array_merge($retArray, $this->geneticsTab($full_reg_number));
         */


        return $retArray;
    }

    private function headerPortion($full_reg_number) {

        $pageData = $this->init($full_reg_number, 'header');
        $name = $this->getName($pageData);
        $birthDate = $this->getBirthDate($pageData);
        $classificationArray = $this->getClassification($pageData);
        $local_number = $this->getLocalNumber($pageData);
        $damArray = $this->getDam($pageData);
        $sireArray = $this->getSire($pageData);
        /*
          //print("$pageData<br>");
          print("full_reg_number:$full_reg_number<br>");
          print("birthDate:$birthDate<br>");
          print("Class: {$classificationArray['class']} Score: {$classificationArray['score']} <br>");
         */

        $retArray['full_reg_number'] = $full_reg_number;
        $retArray['local_number'] = $local_number;
        $retArray['full_name'] = $name;
        $retArray['birth_date'] = $birthDate;
        $retArray['class'] = $classificationArray['class'];
        $retArray['score'] = $classificationArray['score'];
        $retArray['multi_e'] = $classificationArray['multi_e'];
        $retArray['dam_full_reg_number'] = $damArray['full_reg_number'];
        $retArray['sire_full_reg_number'] = $sireArray['full_reg_number'];

//print_r($retArray);
        return $retArray;
    }

    /* used for geentics tab */

    private function geneticsTab($full_reg_number) {

        $pageData = $this->init($full_reg_number, 'genetics');


        $proDollArray = $this->proDollChooser($pageData);
        $retArray['prodoll'] = $proDollArray['prodoll'];

        $lpiArray = $this->lpiChooser($pageData);
        $retArray['lpi'] = $lpiArray['lpi'];

        $relArray = $this->reliabilityChooser($pageData);
        $retArray['reliability'] = $relArray['reliability'];

        $genoArray = $this->geneticTestChooser($pageData);
        $retArray['geno_test'] = $genoArray['geno_test'];

        return $retArray;
    }

    /* parse a holstein canada page for information */

    function init($full_reg_number, $type) {

        if ($type == 'header') {
            $url = self::createHolsteinCanadaQuery($full_reg_number);
        } elseif ($type == 'genetics') {
            $url = self::createHolsteinCanadaQueryGeneticsTab($full_reg_number);
        } else {
            throw new Exception("No type specified for holstein canada page to get.<br/>");
        }


        //print("$url<br>");
        $ch = curl_init($url);
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.13) Gecko/20080311 Firefox/2.0.0.13'); //it seems holstein canada displays site only to browsers it recognizes.

        $data = curl_exec($ch);
        $pageData = curl_exec($ch);
// close the connection
        curl_close($ch);

        //check for parentage verify first.
        $strToMatch = 'Pending parentage confirmation';
        if (strstr($pageData, $strToMatch) == true) {
            throw new CowParentageVerifyException("CURL Holstein Canada website error (The animal $full_reg_number is awaiting parentage verification on Holstein Canada, do a manual check of Holstein Canada site).");
        }


//first do a basic check that this is an actual animal info page, if it is not excp.
        $strToMatch = 'ReportAnimalError';
        if (strstr($pageData, $strToMatch) == true) {
            throw new CowDoesntExistException("CURL Holstein Canada website parse error (The animal $full_reg_number is not found on Holstein Canada, do a manual check of Holstein Canada site).");
        }




        return $pageData;
    }

//end function
}

//end class

/* exception class used here */
class CowDoesntExistException extends Exception {
    
}

class CowParentageVerifyException extends Exception {
    
}

;
?>