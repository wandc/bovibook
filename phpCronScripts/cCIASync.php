<?php

/*
 * This class contacts CCIA and uploads data reporting birthdate event, move out, etc.
 *
 */
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
    include_once('../functions/misc.inc');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/misc.inc');
}

class CCIASync {
    /*
     * This code works on the premises that when they are born there first location is the calving pen. Cows are never in the calving pen (they are in 99 maternity). 
     * So the only time an animal is in the calving pen is when they are born. So calving pen, death, beefed, shot are all special locations. When any of these type of events 
     * occur we know we need to report them to CCIA. 
     * 
     * So this script looks for events that have not been reported yet (or reporting has failed?) and syncs this system with CCIA.
     * 
     * CCIA allows batch transfers and the low level code here supports that. It is much eaasier to keep straight what is synced by doing it on an individual basis, so 1 sync matches 1 location_event.
     * 
     */

    private $premisesID;

    function __construct() {
        $this->premisesID = $GLOBALS['config']['CCIA']['PremisesID'];
    }

//do not sync events newer then 5 days, this gives us time on farm to fix calving and other mistakes before reporting.
    function createBirthDateEvent() {

        $retObj = new stdClass();


        $sql = "SELECT bovine.local_number, bovine.rfid_number,bovine.birth_date,location_event.*,location.*,location_event.id as input_location_event_id,ccia_reported.* from bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location.id=location_id
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN batch.ccia_reported ON location_event_id = location_event.id
where ccia_reportable ='birthdate'  AND location_event.event_time < (current_date-interval '5 days') AND ccia_reported.event_time is null
order by location_event.event_time asc ";

        $res2 = $GLOBALS['pdo']->query($sql);

        $storageArr = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $temp = new stdClass();
            $temp->location_event_id = $row2['input_location_event_id'];

            $date = date('Ymd', strtotime($row2['birth_date']));
            $temp->data = $this->birthDateEvent($row2['rfid_number'], $date, $this->premisesID);
            $storageArr[] = $temp; //add object to array
        }

        return $storageArr;
    }

//do not sync events newer then 5 days, this gives us time on farm to fix reporting mistakes.
    function createMoveOutEvent() {

        $retObj = new stdClass();


        $sql = "SELECT bovine.local_number, bovine.rfid_number,bovine.birth_date,location_event.*,location.*,location_event.id as input_location_event_id,location_event.event_time as input_location_event_time,ccia_reported.* from bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location.id=location_id
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN batch.ccia_reported ON location_event_id = location_event.id
where ccia_reportable ='moveout'  AND location_event.event_time < (current_date-interval '5 days') AND ccia_reported.event_time is null
order by location_event.event_time asc 
 ";

        $res2 = $GLOBALS['pdo']->query($sql);

        $storageArr = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $temp = new stdClass();
            $temp->location_event_id = $row2['input_location_event_id'];
            $date = date('Ymd', strtotime($row2['input_location_event_time']));
            $temp->data = $this->moveOutEvent($row2['rfid_number'], $date, $this->premisesID);
            $storageArr[] = $temp; //add object to array
        }

        return $storageArr;
    }
    
    
    //do not sync events newer then 5 days, this gives us time on farm to fix reporting mistakes.
    function createMoveInEvent() {

        $retObj = new stdClass();


        $sql = "SELECT bovine.local_number, bovine.rfid_number,bovine.birth_date,location_event.*,location.*,location_event.id as input_location_event_id,location_event.event_time as input_location_event_time,ccia_reported.* from bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location.id=location_id
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN batch.ccia_reported ON location_event_id = location_event.id
where ccia_reportable ='movein'  AND location_event.event_time < (current_date-interval '5 days') AND ccia_reported.event_time is null
order by location_event.event_time asc 
 ";

        $res2 = $GLOBALS['pdo']->query($sql);

        $storageArr = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $temp = new stdClass();
            $temp->location_event_id = $row2['input_location_event_id'];
            $date = date('Ymd', strtotime($row2['input_location_event_time']));
            $temp->data = $this->moveInEvent($row2['rfid_number'], $date, $this->premisesID);
            $storageArr[] = $temp; //add object to array
        }

        return $storageArr;
    }

//do not sync events newer then 5 days, this gives us time on farm to fix reporting mistakes.
    function createRetiredEvent() {

        $retObj = new stdClass();


        $sql = "SELECT bovine.local_number, bovine.rfid_number,bovine.birth_date,location_event.*,location.*,location_event.id as input_location_event_id,location_event.event_time as input_location_event_time,ccia_reported.* from bovinemanagement.location_event 
LEFT JOIN bovinemanagement.location ON location.id=location_id
LEFT JOIN bovinemanagement.bovine ON bovine.id=bovine_id
LEFT JOIN batch.ccia_reported ON location_event_id = location_event.id
where ccia_reportable ='retired'  AND location_event.event_time < (current_date-interval '5 days') AND ccia_reported.event_time is null
order by location_event.event_time asc
 ";

        $res2 = $GLOBALS['pdo']->query($sql);

        $storageArr = array();
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $temp = new stdClass();
            $temp->location_event_id = $row2['input_location_event_id'];
            $date = date('Ymd', strtotime($row2['input_location_event_time']));
            $temp->data = $this->retiredEvent($row2['rfid_number'], $date, $this->premisesID);
            $storageArr[] = $temp; //add object to array
        }

        return $storageArr;
    }

    function main() {

        /* do birth dates */
        print("Doing Birth Events...\n");
        $manyArr = $this->createBirthDateEvent();
        if (!empty($manyArr)) {
            foreach ($manyArr as $k => $v) {
                $sent = $this->encapsulatePayload($manyArr[$k]->data); //send only 1 event
                $received = $this->SendRequest($sent);
                $this->processResult($manyArr[$k]->location_event_id, date("c"), $sent, $received);
            }
        }
        
        /* end birth dates */

        /* do Move Out: beef, sold, partial interest */
        print("Doing Move Out Events...\n");
        $manyArr2 = $this->createMoveOutEvent();
        if (!empty($manyArr2)) {
            foreach ($manyArr2 as $k => $v) {
                $sent = $this->encapsulatePayload($manyArr2[$k]->data); //send only 1 event
                $received = $this->SendRequest($sent);
                $this->processResult($manyArr2[$k]->location_event_id, date("c"), $sent, $received);
            }
        }
        /* end Move Out */


        /* do Move In: 4-H Show, bought cow */
        print("Doing Move In Events...\n");
        $manyArr4 = $this->createMoveInEvent();
        if (!empty($manyArr4)) {
            foreach ($manyArr4 as $k => $v) {
                $sent = $this->encapsulatePayload($manyArr4[$k]->data); //send only 1 event
                $received = $this->SendRequest($sent);
                $this->processResult($manyArr4[$k]->location_event_id, date("c"), $sent, $received);
            }
        }
        /* end Move In */
        
        /* do Retired: died on farm or shot */
         print("Doing Retired Events...\n");
        $manyArr3 = $this->createRetiredEvent();
        if (!empty($manyArr3)) {
            foreach ($manyArr3 as $k => $v) {
                $sent = $this->encapsulatePayload($manyArr3[$k]->data); //send only 1 event
                $received = $this->SendRequest($sent);
                $this->processResult($manyArr3[$k]->location_event_id, date("c"), $sent, $received);
            }
        }
        /* end Retired */
    }

    /* USED ONLY WHEN TESTING */
    /* used with test server, need to change creditials to test server */
    /* expires in july 2018, need to renew if needed */

    function mainTEST() {

        //emulates createBirthDateEvent
        $temp = new stdClass();
        $temp->location_event_id = 35400;
        $temp->data = $this->birthDateEvent('124000299971904', '20170322', 'NB9999860');
        $manyArr[] = $temp; //add object to array
        //
     if (!empty($manyArr)) {
            $sent = $this->encapsulatePayload($manyArr[0]->data); //send only 1 event
            //print($sent);
            $received = $this->SendRequest($sent);
            $this->processResult($manyArr[0]->location_event_id, date("c"), $sent, $received);
        }

        print("\n\r done \n\r");
    }

    /*
     * after data is sent to CCIA a result (response)
     * comes back. We track this in the DB for sucessful results.
     * 
     */

    function processResult($location_event_id, $event_time, $sent, $received) {

        if (empty($received)) {
            throw new Exception("The json returned form CCIA is empty.");
        }


        $jsonArr = json_decode($received, true);
        if ($jsonArr['status'] == "SUCCESS") {
            //don't put id value, uses next one in DB.
            $query = "INSERT INTO batch.ccia_reported  (location_event_id,event_time,sent,received) VALUES ($location_event_id,'$event_time','$sent','$received')";
            $res = $GLOBALS['pdo']->exec($query);
        } elseif ($jsonArr['status'] == "FAILED") {
            print("Failed:\n\r");
            print_r($sent);
            print_r($jsonArr);
        } else {
            throw new Exception("The json returned from CCIA is not valid. Or not ");
        }

        return true;
    }

    /*
     * send to CCIA
     * 
     */

    function SendRequest($data_string) {

//124000299971900-124000299971919
//https://www.clia.livestockid.ca/CLTS/public/help/en/webservices/WebServiceAnimalSearch/IAnimalSearchWSv2.wsdl

        $urlSuffix = '/uploadEvent'; //always this
        $urlDemo = 'https://www.clia.demo.livestockid.ca/CLTS/webservice'; //used when testing with test tags
        $urlProduction = 'https://www.clia.livestockid.ca/CLTS/webservice'; //used for actual tags and data
        $auth = base64_encode($GLOBALS['config']['CCIA']['Auth']); //base64, as CCIA requires the standard way.
        $ch = curl_init($urlProduction . $urlSuffix);
        curl_setopt($ch, CURLINFO_HEADER_OUT, true); // enable tracking
        curl_setopt($ch, CURLOPT_USERPWD, $GLOBALS['config']['CCIA']['Auth']);
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data_string);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
        curl_setopt($ch, CURLOPT_HTTPHEADER, array(
            'Content-Type: application/json',
            "WSCID: {$GLOBALS['config']['CCIA']['WSCID']}"
                )
        ); //account number WSCID provided as special header field.
        curl_setopt($ch, CURLOPT_TIMEOUT, 30);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30);

//execute post
        $result = curl_exec($ch);
        if (curl_errno($ch)) {
            echo 'Curl error: ' . curl_error($ch);
        }
        $info = curl_getinfo($ch);
        $headerSent = curl_getinfo($ch, CURLINFO_HEADER_OUT); // request headers
//var_dump($info);
//close connection
        curl_close($ch);

        //DEBUG
        //echo $headerSent;
        echo $result . "\n\r";
        return $result;
    }

    /*
     * used for single or many events.
     * takes in array of single or many events
     * returns json
     */

    function encapsulatePayload($inputArr) {
        $data = array('events' => array($inputArr));
        return json_encode($data); //return json string
    }

    /**
     * List of function webservice supports
     * 
      Issued	Used to report approved tag numbers sold to producers by tag manufacturers, tag distributors, or tag dealers.
      Animal Events
      Birth Date	Used to associate an animal’s birth date with its unique approved tag number.
      Cross Reference	Used to associate a newly applied approved tag number on a previously tagged animal where the lost tag number is known.
      Replaced	Used to associate a newly applied approved tag number on a previously tagged animal where the lost tag number is not known.
      Move In	Report the identification of an approved tag applied to an individual animal, or report a group of animals based on species, that have been received at a defined location on a defined day.
      Move Out	Report the identification of an approved tag applied to an individual animal, or report a group of animals based on species, that have departed from a defined location on a defined day.
      Sighted	Used to report the identification of an approved tag applied to an animal that has been observed at a defined location on a defined day. (e.g., livestock operation, veterinary clinic, etc.)
      Imported	Used to report the identification of an approved tag applied to an animal that has entered Canada.
      Exported	Used to report the identification of an approved tag applied to an animal that has left Canada.
      Temporary Export	Used to report the identification of an approved tag applied to an animal that is being temporarily shipped outside of Canada. Note: When animals return to Canada, submit an imported event to move the tag numbers back into your animal inventory.
      Retired	Used to report the identification of an approved tag applied to an animal that has died or was slaughtered.
     */
    /*
     * create json payload to insert single tag birth date.
     * you can call this many times to send many at once and encapsulate it once.
     */
    function birthDateEvent($rfid, $birthDate, $premisesID) {

        $foo = new StdClass();
        $foo->name = "EVENT_TYPE";
        $foo->value = "birthdate";

        $foo1 = new StdClass();
        $foo1->name = "SOURCE_ACCOUNT";
        $foo1->value = $GLOBALS['config']['CCIA']['Account'];

        $foo2 = new StdClass();
        $foo2->name = "TAG_START";
        $foo2->value = $rfid;

        $foo3 = new StdClass();
        $foo3->name = "DOB";
        $foo3->value = $birthDate; //20080222

        $foo4 = new StdClass();
        $foo4->name = "DOB_METHOD";
        $foo4->value = "AB";

        $foo5 = new StdClass();
        $foo5->name = "SOURCE_PREMISES";
        $foo5->value = $premisesID;

//combine into proper format
        $objects = array($foo, $foo1, $foo2, $foo3, $foo4, $foo5);
        $data = array('attribute' => $objects);

//return array
        return $data;


        /*
         * EXAMPLE:
          {
          "events": [
          {"attribute":[{"name":"EVENT_TYPE","value":"birthdate"},
          {"name":"SOURCE_ACCOUNT","value":"A0369202"},
          {"name":"TAG_START","value":"0124000278887801"},
          {"name":"DOB","value":"20080222"},
          {"name":"DOB_METHOD","value":"AB"}
          ]},
          {"attribute":[{"name":"EVENT_TYPE","value":"birthdate"},
          {"name":"SOURCE_ACCOUNT","value":"A0369202"},
          {"name":"TAG_START","value":"0124000278887802"},
          {"name":"DOB","value":"20080222"},
          {"name":"DOB_METHOD","value":"AB"}
          ]}
          ]}
         */
    }

    /* USED WHEN YOU BUY A COW OR 4H SHOW COMING BACK
     * create json payload for move into premises
     * you can call this many times to send many at once and encapsulate it once.
     */

    function moveInEvent($rfid, $eventDate, $premisesID) {
//required fields:
        /*
          EVENT_TYPE
          EVENT_DATE
          DEST_ACCOUNT
          DEST_PREMISES
          TAG_START
         */

        $foo = new StdClass();
        $foo->name = "EVENT_TYPE";
        $foo->value = "MOVEIN";

        $foo1 = new StdClass();
        $foo1->name = "EVENT_DATE";
        $foo1->value = $eventDate; //20080222

        $foo2 = new StdClass();
        $foo2->name = "DEST_ACCOUNT";
        $foo2->value = $GLOBALS['config']['CCIA']['Account'];

        $foo3 = new StdClass();
        $foo3->name = "DEST_PREMISES";
        $foo3->value = $premisesID;

        $foo4 = new StdClass();
        $foo4->name = "TAG_START";
        $foo4->value = $rfid;

//combine into proper format
        $objects = array($foo, $foo1, $foo2, $foo3, $foo4);
        $data = array('attribute' => $objects);

//return array
        return $data;
    }

    /* USED WHEN COW GETS ON TRAILER AND LEAVES FARM
     * create json payload for move out of premises
     * you can call this many times to send many at once and encapsulate it once.
     */

    function moveOutEvent($rfid, $eventDate, $premisesID) {
//required fields:
        /*
          EVENT_TYPE
          EVENT_DATE
          SOURCE_ACCOUNT
          SOURCE_PREMISES
          TAG_START
         */

        $foo = new StdClass();
        $foo->name = "EVENT_TYPE";
        $foo->value = "MOVEOUT";

        $foo1 = new StdClass();
        $foo1->name = "EVENT_DATE";
        $foo1->value = $eventDate; //20080222

        $foo2 = new StdClass();
        $foo2->name = "SOURCE_ACCOUNT";
        $foo2->value = $GLOBALS['config']['CCIA']['Account'];

        $foo3 = new StdClass();
        $foo3->name = "SOURCE_PREMISES";
        $foo3->value = $premisesID;

        $foo4 = new StdClass();
        $foo4->name = "TAG_START";
        $foo4->value = $rfid;

//combine into proper format
        $objects = array($foo, $foo1, $foo2, $foo3, $foo4);
        $data = array('attribute' => $objects);

//return array
        return $data;
    }

    /* USED WHEN COW DIES (OR GETS SHOT) ON FARM
     * create json payload for move out of premises
     * you can call this many times to send many at once and encapsulate it once.
     */

    function retiredEvent($rfid, $eventDate, $premisesID) {
//required fields:
        /*
          EVENT_TYPE
          EVENT_DATE
          SOURCE_ACCOUNT
          TAG_START
          SOURCE_PREMISES (optional, but we will send for completeness).
         */

        $foo = new StdClass();
        $foo->name = "EVENT_TYPE";
        $foo->value = "RETIRED";

        $foo1 = new StdClass();
        $foo1->name = "EVENT_DATE";
        $foo1->value = $eventDate; //20080222

        $foo2 = new StdClass();
        $foo2->name = "SOURCE_ACCOUNT";
        $foo2->value = $GLOBALS['config']['CCIA']['Account'];

        $foo3 = new StdClass();
        $foo3->name = "TAG_START";
        $foo3->value = $rfid;

        $foo4 = new StdClass();
        $foo4->name = "SOURCE_PREMISES";
        $foo4->value = $premisesID;

//combine into proper format
        $objects = array($foo, $foo1, $foo2, $foo3, $foo4);
        $data = array('attribute' => $objects);

//return array
        return $data;
    }

}

//end class
//if it is run from the command line.
//only run when called direectly from stdin
if ((defined('STDIN')) AND ( !empty($argc) && strstr($argv[0], basename(__FILE__)))) {
    $cls = new CCIASync();
    $cls->main();
} else {
//nothing.
}
?>