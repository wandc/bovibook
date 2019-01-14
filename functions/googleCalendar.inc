<?php 

/**
 * The class logs us into the google calendar service using oauth, server method
 * 
 * login keys, etc for stored in config file.
 * bovine_tratement calendarID=littleriver.ca_phrs87e7efrsf6qc0hk4bfk88g@group.calendar.google.com
 *  littleriver.ca_3shnhbtafpt4dtkg078epi707c@group.calendar.google.com //work_schedule
 */
//for config file.
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
}

if (!class_exists('Google_Client')) {
     // require_once $_SERVER['DOCUMENT_ROOT'].'/vendor/autoload.php';
}

class GoogleCalendar {

    public $cal;
    public $calendarID;
    public $CLIENT_ID;
    public $SERVICE_ACCOUNT_NAME;
    public $KEY_FILE;      // Make sure you keep your key.p12 file in a secure location, and isn't readable by others.

    //default is bovine_treatement, change this string for other things.

    public function __construct($calendarID = 'littleriver.ca_phrs87e7efrsf6qc0hk4bfk88g@group.calendar.google.com') {
        $this->calendarID = $calendarID;


       // UPDATE:
       //March 2017. 
       // to make this work you need a service account. 
       //here: https://console.developers.google.com/
       //delegate to whole domain, so service account can acccess everything
       //then you go here:
       //https://admin.google.com/AdminHome?chromeless=1#OGX:ManageOauthClients
       //add the numbers only id to allow access to whatever, in this case clendar    
       //you then use the private json credentials with the php code. 
        
        
//DOCS:https://code.google.com/p/google-api-php-client/wiki/OAuth2
// Set your client id, service account name, and the path to your private key.
// For more information about obtaining these keys, visit:
// https://developers.google.com/console/help/#service_accounts
//FOR THIS TO WORK YOU HAVE TO GO ADD SERVICE_ACCOUNT_NAME IN THE SPECIFIC CALENDAR
//SETTINGS AS A SHARED PERSON.
//ALSO THE DOMAIN WIDE SETTINGS HAVE TO ALLOW IT TOO. THEY TAKE PRECEDENCE.
// Load the key in PKCS 12 format (you need to download this from the
// Google API Console when the service account was created.
        //this all created by the calendar user. 
     
        
         require_once $_SERVER['DOCUMENT_ROOT'].'framework/composer/vendor/autoload.php';
 
       $client = new Google_Client();
      
       $client->setAuthConfig($_SERVER['DOCUMENT_ROOT'] .'/local/config/'.'Calendar API Project-790d47705ed2.json');
       $user_to_impersonate = 'calendar@littleriver.ca';
       $client->setSubject($user_to_impersonate);
    
              
       
         $scopes = array('https://www.googleapis.com/auth/calendar');
        $client->setScopes($scopes);
         $this->cal = new Google_Service_Calendar($client);

        
        // $client->setUseObjects(true); //get info as objects and not as array. //TURN OFF FOR NOW, TURN ON IF API FIXED.
     //   $client->setClientId($this->CLIENT_ID);
      //  $this->cal = new Google_Service_Calendar($client);
        
         
          
         
        $calendarListEntry = $this->cal->calendarList->get($calendarID);
        
        // print("HELLO");
        
 //echo $calendarListEntry->getSummary();
        

 
       
       /* 
        OLD WAY VERSION 1
        $client->setAssertionCredentials(new Google_Auth_AssertionCredentials(
                $this->SERVICE_ACCOUNT_NAME, 'https://www.google.com/calendar/feeds/' . $this->calendarID . '/private/basic', $key)
        );

        $this->cal = new Google_Service_Calendar($client); //client is from calendar settings page, xml link.
        
        */
        
        $calendarId = 'primary';
$optParams = array(
  'maxResults' => 10,
  'orderBy' => 'startTime',
  'singleEvents' => TRUE,
  'timeMin' => date('c'),
);
$results = $this->cal->events->listEvents($calendarId, $optParams);

/* //DEBUG
if (count($results->getItems()) == 0) {
  print "No upcoming events found.<br/>\n";
} else {
  print "Upcoming events:\n";
  foreach ($results->getItems() as $event) {
    $start = $event->start->dateTime;
    if (empty($start)) {
      $start = $event->start->date;
    }
    printf("%s (%s)\n", $event->getSummary(), $start);
  }
}
 */
        
    }

    public function test() {
//list calendars
        $calList = $this->cal->calendarList->listCalendarList();
        print "<h1>Calendar List</h1><pre>" . print_r($calList, true) . "</pre>";

        $retID = $this->createRecurringEvent($this->calendarID, 'Appointment Test 44', 'No description', 'Somewhere, NB', '2013-03-20', '10:00:00', '2013-03-20', '11:00:00', '-04', '2013-03-22');
        print_r($retID);

//read it back
        $ret2 = $this->getEvent($this->calendarID, $retID);
        print_r($ret2);

//now delete it
        $ret3 = $this->deleteEvent($this->calendarID, $ret2['id']);
        print_r($ret3);
    }

    public function createEvent($calendarName = 'primary', $title = 'No data', $desc = 'No data description', $where = 'Somewhere, NB', $startDate = '2014-01-20', $startTime = '10:00:00', $endDate = '2014-01-20', $endTime = '11:00:00', $tzOffset = '-04') {

        $event = new Google_Service_Calendar_Event();
        $event->setSummary($title); //title
        $event->setLocation($where);
// 
        $start = new Google_Service_Calendar_EventDateTime();
        $start->setDateTime($startDate . 'T' . $startTime . $tzOffset); // '2013-03-03T10:00:00.000-07:00'
        $event->setStart($start);
//
        $end = new Google_Service_Calendar_EventDateTime();
        $end->setDateTime($endDate . 'T' . $endTime . $tzOffset);
        $event->setEnd($end);
//
        // Upload the event to the calendar server
        $createdEvent = $this->cal->events->insert($calendarName, $event); //what calendar?
//debug
//print_r($createdEvent);
        // A copy of the event as it is recorded on the server is returned
        return $createdEvent['id']; //before we would return $createdEvent['htmlLink']
    }

    public function createRecurringEvent($calendarName = 'primary', $title = 'Tennis with Beth', $desc = 'Meet for a quick lesson', $where = 'On the courts', $startDate = '2008-01-20', $startTime = '10:00:00', $endDate = '2008-01-20', $endTime = '11:00:00', $tzOffset = '-04', $untilDate = '2015-01-24') {
        /*
          //debug
          print("<br/>");
          print("calendarName::$calendarName<br/>");
          print("title::$title<br/>");
          print("desc:: $desc<br/>");
          print("where:: $where<br/>");
          print("startDate::   $startDate<br/>");
          print("startTime:: $startTime<br/>");
          print("endDate:: $endDate<br/>");
          print(" endTime:: $endTime<br/>");
          print("tzOffset::   $tzOffset<br/>");
          print("untilDate::$untilDate<br/>");
          print("<br/>");

         */


        $event = new Google_Service_Calendar_Event();
        $event->setSummary($title); //title
        $event->setLocation($where);

        $start = new Google_Service_Calendar_EventDateTime();
        //assume all day event when no time given.
        if ($startTime == '') {
            $start->setDate($startDate);
        } else {
            $start->setDateTime($startDate . 'T' . $startTime); // '2013-03-03T10:00:00.000'
        }
        $start->setTimeZone('America/Halifax');
        $event->setStart($start);

        $end = new Google_Service_Calendar_EventDateTime();
        if ($endTime == '') {
            $end->setDate($endDate);
        } else {
            $end->setDateTime($endDate . 'T' . $endTime); // '2013-03-03T10:00:00.000' 
        }
        $end->setTimeZone('America/Halifax');
        $event->setEnd($end);
        //
        //recurrence 
        $untilDateMod = date('Ymd', strtotime($untilDate));
        $event->setRecurrence(array("RRULE:FREQ=DAILY;UNTIL=" . $untilDateMod . "T000000Z")); //does the Z at the end mean zulu time, or use the default timezone????
        //upload event
        $createdEvent = $this->cal->events->insert($calendarName, $event);
        return $createdEvent['id'];
    }

    public function getEvent($calendarID, $eventId) {

        $event = $this->cal->events->get($calendarID, $eventId);
        return $event;
    }

    public function deleteEvent($calendarID, $eventID = null) {

        //attempt delete
        try {
            $this->cal->events->delete($calendarID, $eventID);
        } catch (Exception $e) {
            throw new Exception("Error: Google Calendar Delete Failed: " . $e->getMessage());
        }
        return true; //sucessful.
    }

    /* show us all the events, used for seeing who was working on a certain day */
    /* used by cron job to find out who worked on certain date */

    function grabEventsFromGoogleCalendarWorkSchedule() {
        $minCheck = date(DATE_ATOM, mktime(0, 0, 0, 1, 1, date("2012"))); //start at 2012.
        $maxCheck = date(DATE_ATOM, mktime(0, 0, 0, date("m"), date("d"), date("Y"))); //today
//print("Date Range: $minCheck::$maxCheck");

        $calendarId = $this->calendarID;
        $events = $this->cal->events->listEvents($calendarId, array('singleEvents' => 'true', 'pageToken' => $result['nextPageToken'], 'timeMin' => $minCheck, 'timeMax' => $maxCheck));

        $out = array();
        while (true) {
            foreach ($events['items'] as $event) {

                $out[] = $event; //put into array
            }
            $pageToken = $events['nextPageToken'];
            if ($pageToken) {
                $events = $this->cal->events->listEvents($calendarId, array('singleEvents' => 'true', 'pageToken' => $pageToken, 'timeMin' => $minCheck, 'timeMax' => $maxCheck));
            } else {
                break;
            }
        }



        //find milking events.
        foreach ($out as $key => $value) {

            if (strstr($value['summary'], 'milk:') == true) {

                //find who it is.    
                $who = trim(strtolower(str_replace('milk: ', '', $value['summary']))); //parse string for userid.
                //remove start and end timezone because php strtotime does not support it 
                $start = str_replace('-03:00', '', str_replace('-04:00', '', $value['start']['dateTime']));  //take into account both possibilitiues for timezones (AST/DST).
                $end = str_replace('-03:00', '', str_replace('-04:00', '', $value['end']['dateTime']));
                $startHour = date('h', strtotime($start));
                $endHour = date('h', strtotime($start));
                // 
                $out2["{$value['id']}"]['userid'] = $who;
                $out2["{$value['id']}"]['updated'] = $value['updated'];
                $out2["{$value['id']}"]['start'] = $value['start']['dateTime'];
                $out2["{$value['id']}"]['end'] = $value['end']['dateTime'];
                $out2["{$value['id']}"]['date'] = date('Y-m-d', strtotime($start));

                //determine am or pm milkimg shift.
                if ($startHour < 12) {
                    $out2["{$value['id']}"]['shift'] = 'am';
                } elseif ($startHour >= 12) {
                    $out2["{$value['id']}"]['shift'] = 'pm';
                } else {
                    throw new Exception("shift type can't be determined for id={$value['id']}./n/r<br/>");
                }
            }
        }


        //now go through and use the most updated one as the true milker when there
        // are two milkers for the same shift.

        $out3 = array();
        foreach ($out2 as $key => $value) {
            $newKey = $value['date'] . '|' . $value['shift'];

            if (array_key_exists($newKey, $out3) == true) {
                //look which one was updated the latest and use it.
                //current one in array:
                if (strtotime($out3[$newKey]['updated']) > strtotime($value['updated'])) {
                    //do nothing, one already in is the latest
                } elseif (strtotime($out3[$newKey]['updated']) < strtotime($value['updated'])) {
                    unset($out3[$newKey]); //delete old element
                    $out3[$newKey]['userid'] = $value['userid'];
                    $out3[$newKey]['date'] = $value['date'];
                    $out3[$newKey]['shift'] = $value['shift'];
                } else {
                    throw new Exception("update time the same for id {$value['updated']} and  {$out3['updated']}. /n/r <br/>"); //probablity of beeing called is very very small.
                }
            } else {
                $out3[$newKey]['userid'] = $value['userid'];
                $out3[$newKey]['date'] = $value['date'];
                $out3[$newKey]['shift'] = $value['shift'];
            }
        }//end foreach

        ksort($out3);

        //print_r2($out3);

        return $out3;
    }

//end function
}

//end class
?>