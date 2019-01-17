<?php 

/**
 * test code Feb 2016 to see if we can read the weight from scale.
 * 
 */
main2();

function main2() {
    print("Hello world \n");

    $socket = create_socket_connection();

    // set_base_radio_channel($socket); 
//send command to read weight    
    //$stringToWrite=chr(27)."Gs02".chr(4);
    indicator_read_write_pair_ack($socket, chr(27) . "GkL" . chr(4));
    
    
    for ($x=1;$x<200;$x++) {
    $read=indicator_read_write_pair_ack($socket, chr(27) . "Gs02" . chr(4));
    print("READ #$x:$read\n\r");
    sleep(2);
    }
    // indicator_write($socket, $stringToWrite); 
    // sleep(2);
    /*
      echo "Reading response:\n\n";
      $buf = 'This is my buffer.';
      if (false !== ($bytes = socket_recv($socket, $buf, 2048, MSG_WAITALL))) {
      echo "Read $bytes bytes from socket_recv(). Closing socket...";
      } else {
      echo "socket_recv() failed; reason: " . socket_strerror(socket_last_error($socket)) . "\n";
      }
     */

/*
    $readResult = init_radio_read2($socket);
    if ($readResult == TRUE) {
        print("READ!" . $readResult);
    } else {
        print("Not read.");
    }
 
 */
}

//read directly from base radio
function init_radio_read2($socket) {

    print("READding response...");

    $buffer = null;
    while ($out = socket_read($socket, 8196, PHP_BINARY_READ)) {

//take characters as they come in and add them to the buffer.
        $buffer = $buffer . $out;

//if end of line occurs. break out of loop
        if (strpos($buffer, "\r") != FALSE) {
            break;
        }
    }//end socker read loop.
//clean up space,/r,/n etc
    $buffer = trim($buffer);

    if ($buffer == true) {
        print("SUCCESS.\r\n");
        print($buffer);
        return true;
    } else {
        print("FAILURE.\r\n");
        print($buffer);
        return false; //should never be called.
    }
}

function indicator_read2($socket) {
    $buffer = null;
    while ($out = socket_read($socket, 8196, PHP_BINARY_READ)) {
        print($out);
    }
}

/*
 * 
 * 
 * 
 * 
 * 
 */

//run main
//main();   //old way to send feed lines, enable to run


function main() {
    print("*******************************************************************************************\r\n");
    $socket = create_socket_connection();


//send_feedline($socket);
//sleep(1);
//get_number_of_feedlines_comepleted($socket);
//sleep(1);

    read_feedlines($socket);






    close_socket_connection($socket);
}

function send_feedline($socket) {



    try {
        init_radio_write_read_pair($socket, "+++");
        init_radio_write_read_pair($socket, "ATRE\r");
        init_radio_write_read_pair($socket, "ATAT 02\r");
        init_radio_write_read_pair($socket, "ATBT 02\r");
        init_radio_write_read_pair($socket, "ATRR 0\r");
        init_radio_write_read_pair($socket, "ATCD 2\r");
        init_radio_write_read_pair($socket, "ATCN\r");
    } catch (Exception $e) {
        echo 'EXCEPTION: Radio Init Failed, Caught exception: ', $e->getMessage(), "\n";
    }


    /*
     * Set Base Radio Channel and Address ATDT 01, ATHP 0 for scale id =0.
     */
    set_base_radio_channel($socket);


    try {
        indicator_read_write_pair_ack($socket, chr(27) . "CcE" . chr(4)); //3
        indicator_read_write_pair_ack($socket, chr(27) . "Cm" . chr(2) . "DL<-IN" . chr(4)); //3
        indicator_read_write_pair_ack($socket, chr(27) . "GkL" . chr(4));  //lock keypad
///////////////////////////////////////////////////////////
//create feedlines.
        $class = new FeedLine();
        $class = create_feedline_string();
        $class->rationRecipeCode = 'DAVID';
        $class->batchNumber = '1001';
        $dataStrFinal = chr(27) . 'Rd' . chr(2) . $class->string() . chr(13) . chr(3) . $class->checksumByte() . chr(4); //create feedline.

        $class->ingrediantPenCode = 'ALFAL';
        $class->presetCallWeight = '100';
        $dataStrFinal2 = chr(27) . 'Rd' . chr(2) . $class->string() . chr(13) . chr(3) . $class->checksumByte() . chr(4); //create feedline.

        try {
            indicator_read_write_pair_ack($socket, $dataStrFinal);
            indicator_read_write_pair_ack($socket, $dataStrFinal2);
        } catch (Exception $e) {
            echo 'EXCEPTION: Send Feedlines Problem: ', $e->getMessage(), "\n";
        }

/////////////////////////////////////////////////////////////
        indicator_read_write_pair_ack($socket, chr(27) . "GkU" . chr(4));  //lock keypad
        indicator_read_write_pair_ack($socket, chr(27) . "CcD" . chr(4)); //3 //leave control mode
    } catch (Exception $e) {
        echo 'EXCEPTION 4: Send Feedlines Problem: ', $e->getMessage(), "\n";
    }
}

function read_feedlines($socket) {




    try {
        init_radio_write_read_pair($socket, "+++");
        init_radio_write_read_pair($socket, "ATDT 01\r");
        init_radio_write_read_pair($socket, "ATHP 0\r");
        init_radio_write_read_pair($socket, "ATRR 0\r");
        init_radio_write_read_pair($socket, "ATCN\r");
    } catch (Exception $e) {
        echo 'EXCEPTION 2: Send Feedlines Problem: ', $e->getMessage(), "\n";
    }


    $buf = null;

    try {
        indicator_read_write_pair_ack($socket, chr(27) . "CcE" . chr(4)); // not sure what this command does, but it is critical.
        indicator_read_write_pair_ack($socket, chr(27) . "Cm" . chr(2) . "DL>OUT" . chr(4)); //show on screen
        indicator_read_write_pair_ack($socket, chr(27) . "GkL" . chr(4));  //lock keypad
        $buf = indicator_read_write_pair_data($socket, chr(27) . "Rp-99999" . chr(4)); //download feedlines
        indicator_read_write_pair_ack($socket, chr(27) . "Cm" . chr(2) . "<CLR>" . chr(4)); //show on screen
//indicator_read_write_pair_ack($socket, chr(27)."Re-99999".chr(4));  //erase feedlines
        indicator_read_write_pair_ack($socket, chr(27) . "GkU" . chr(4));  //lock keypad
    } catch (Exception $e) {
        echo 'EXCEPTION: Read Feedlines Problem: ', $e->getMessage(), "\n";
    }

    print("buf=$buf\r\n");
}

/**
 * Gets info on how many feedlines are left to do, etc.
 */
function get_number_of_feedlines_comepleted($socket) {

    /*
     *    Initialize radio. ie talk directly to radio and not TMR scale.
     */
    try {
        init_radio_write_read_pair($socket, "+++");
        init_radio_write_read_pair($socket, "ATRE\r");
        init_radio_write_read_pair($socket, "ATAT 02\r");
        init_radio_write_read_pair($socket, "ATBT 02\r");
        init_radio_write_read_pair($socket, "ATRR 0\r");
        init_radio_write_read_pair($socket, "ATCD 2\r");
        init_radio_write_read_pair($socket, "ATCN\r");
    } catch (Exception $e) {
        echo 'EXCEPTION: Radio Init Failed, Caught exception: ', $e->getMessage(), "\n";
    }


    /*
     * Set Base Radio Channel and Address ATDT 01, ATHP 0 for scale id =0.
     */
    set_base_radio_channel($socket);

    /*
     * Try and talk to scale indicator and get number of feedlines done or not.
     */
    try {
        $buf = indicator_read_write_pair_data($socket, chr(27) . "Gs12" . chr(4));
        print("buf=$buf\r\n");
    } catch (Exception $e) {
        echo 'EXCEPTION: Radio Command to scale indicator Failed, Caught exception: ', $e->getMessage(), "\n";
    }
}

/**
 * Demo feedline for testing purposes
 */
function create_feedline_string() {
    $userData['truckNumber'] = '000001';
    $userData['lineStatus'] = 'U';
    $userData['lineType'] = 'I';
    $userData['loadType'] = 'T';
    $userData['batchNumber'] = '1001';
    $userData['ingrediantPenCode'] = 'CORN';
    $userData['rationRecipeCode'] = 'HICOW';
    $userData['presetCallWeight'] = '2500';
    $userData['loadedDeliveredWeight'] = null;
    $userData['userID'] = '7350';
    $userData['time24'] = null;
    $userData['dateFormat'] = null;
    $userData['date'] = '';
    $userData['headCount'] = '117';
    $userData['newPresetCallWeightNextFeeding'] = '';
    $userData['feedZone'] = '1';
    $userData['numberMixerRevs'] = null;
    $userData['grossWeightError'] = '';
    $userData['motionWeight'] = '0';
    $userData['toleranceWeight'] = '0';


    $class = new FeedLine($userData);
    return $class;
}

function send_feedlines_to_indicator_scale($socket, $feedline, $dataFieldFormatStr) {
    try {
//1 set base radio address
//2 perform receive steps 2,3,5,7,8
//indicator_read_write_pair_ack($socket, chr(1)."6"."3");                     //2-2
        set_base_radio_channel($socket);                                        //2-3 assume already set to reserve channel?
        indicator_read_write_pair_ack($socket, chr(27) . "GkL" . chr(4));               //2-5
//indicator_read_write_pair_ack($socket, chr(27)."Cm".chr(2)."<CLR>".chr(4)); //2-7
        indicator_read_write_pair_ack($socket, chr(27) . "Re-99999" . chr(4));          //2-8
//indicator_read_write_pair_ack($socket, chr(27)."Cm".chr(2)."DL<-IN".chr(4));//3
        indicator_read_write_pair_ack($socket, $dataFieldFormatStr);      //4   SEND HEADER AND FEEDLINE AND WAIT FOR ACK
//indicator_read_write_pair_ack($socket,$feedline); 			    
        indicator_read_write_pair_ack($socket, chr(27) . "GkU" . chr(4));        //5
        indicator_read_write_pair_ack($socket, chr(27) . "Cm" . chr(2) . "Feed Lines OK - Press Recipe Key to Continue" . chr(4));  //6
        indicator_read_write_pair_ack($socket, chr(1) . "PP");                       //7 keep message scrolling and take off reserver channel.
    } catch (Exception $e) {
        echo 'EXCEPTION: Send Feedlines Problem: ', $e->getMessage(), "\n";
    }
}

function create_socket_connection() {


    error_reporting(E_ALL);

    echo "<h2>TCP/IP Connection</h2>\n";

    /* Get the port for the WWW service. */
    $service_port = '10001';

    /* Get the IP address for the target host. */
    $address = '192.168.9.149';

    /* Create a TCP/IP socket. */
    $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if ($socket === false) {
        echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
    } else {
        echo "done creating socket.\n";
    }

//set socket timeout to receive dat
    socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, array("sec" => 5, "usec" => 0));


    echo "Attempting to connect to '$address' on port '$service_port'...";
    $result = socket_connect($socket, $address, $service_port);
    if ($result === false) {
        echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
    } else {
        echo "done attempting to connect.\n";
    }

    return $socket;
}

function close_socket_connection($socket) {
    echo "Closing socket...\r\n\n";
    socket_close($socket);
    echo "done closing socket.\r\n\n";
}

function set_base_radio_channel($socket) {
    /**
     * Set Base Radio Address ATDT 0 and Channel ATHP 0 for scale id =0.
     */
    try {
        init_radio_write_read_pair($socket, "+++");
        init_radio_write_read_pair($socket, "ATDT 01\r");
        init_radio_write_read_pair($socket, "ATHP 0\r");
        init_radio_write_read_pair($socket, "ATRR 0\r");
        init_radio_write_read_pair($socket, "ATCN\r");
    } catch (Exception $e) {
        echo 'EXCEPTION: Radio Init of Base CHhannel and Address Failed, Caught exception: ', $e->getMessage(), "\n";
    }
}

/**
 * 1. write something to indicator and expect and ACK back.
 */
function indicator_read_write_pair_ack($socket, $stringToWrite) {

    $retryCounter = 0;

    while ($retryCounter <= 5) {

//don't rety immediately.
        if ($retryCounter >= 1) {
            sleep(.1);
            print("\r\nRETRY #: $retryCounter");
        } //wait 1 sec
//write
        indicator_write($socket, $stringToWrite);



        $readResult = indicator_read($socket);
        if ($readResult == TRUE) {
            break;
        }

        $retryCounter++;
    } //end while
//check if we sucessfully read or not.
    if ($readResult == FALSE) {
        throw new Exception("Indicator Command Write/Read Failed.");
        return false; //should never be called.
    } else {
        return $readResult; //return what was read.
    }
}

//write command to indicator
function indicator_write($socket, $stringToWrite) {
    $stringToWriteHex = ascii2hex($stringToWrite);
    print("\r\nWRITE: Command to Indicator:\r\n");
    print("Dec String to Write:$stringToWrite\r\n");
    print("Hex String to Write:$stringToWriteHex\r\n");
    socket_write($socket, $stringToWrite, strlen($stringToWrite));
}

//read command FROM indicator
function indicator_read($socket) {
    $buffer = null;
    while ($out = socket_read($socket, 8196, PHP_BINARY_READ)) {

//take characters as they come in and add them to the buffer.
        $buffer = $buffer . $out;

//break out of loop if nack or ack occurs
        if (strstr($buffer, chr(6)) != FALSE) {
            break;
        } //ACK
        if (strstr($buffer, chr(21)) != FALSE) {
            break;
        } //NACK
    } //end while
//if ACK (chr(6)) occurs return true. 
    if (strstr($buffer, chr(6)) != FALSE) {
        print("SUCCESS: ACK received.\r\n");
        beep(2);
        return $buffer; //return what was read
    }
//if NACK occurs, then indicator says command failed.
    else if (strstr($buffer, chr(21)) != FALSE) {
        print("FAILURE: NACK received.\r\n");
        throw new Exception("FAILURE: Scale Indicator gave NACK response.");
    } elseif (strlen($buffer) == 0) { //nothing read.
        return false;
    } else {
        $length = strlen($buffer);
        $response = ascii2hex($buffer);
        throw new Exception("FAILURE: Scale Indicator gave an unknown response. Length:$length data:|$response|");
        return false;
    }
}

/**
 * 1. write something to indicator and expect data back. that we will then parse with something else.
 */
function indicator_read_write_pair_data($socket, $stringToWrite) {

    $retryCounter = 0;

    while ($retryCounter <= 5) {

//don't rety immediately.
        if ($retryCounter >= 1) {
            sleep(1);
            print("\r\nRETRY #: $retryCounter");
        } //wait 1 sec
//write
        indicator_write($socket, $stringToWrite);

        $readResult = indicator_read_data($socket);
        if ($readResult != FALSE) {
            break;
        }

        $retryCounter++;
    } //end while


    return $readResult;
}

function indicator_read_data($socket) {

    $buffer = null;

    $prevBufferLength = 0; //used to see if more data has been read through a loop.
    while ($out = socket_read($socket, 63096, PHP_BINARY_READ)) {
//take characters as they come in and add them to the buffer.
        $buffer = $buffer . $out;

//when ACK occurs and the buffer size hasn't increased between loops, break out of read loop.
        if ((strpos($buffer, chr(6)) != FALSE) && ($prevBufferLength == strlen($out))) {
            break;
        }

//record the buffer size for laster comaprison.
        if (strpos($buffer, chr(6)) != FALSE) {
            $prevBufferLength = strlen($out);
        }
    }

//it's possible for socket read to timeout, thus we must check for valid data again.
    if (strpos($buffer, chr(6)) != FALSE) {
        $buffer = str_replace(chr(6), "", $buffer); //get rid of ACK
        return $buffer;
    } elseif ($buffer == NULL) {
        throw new Exception("\r\nScale Indicator Data Read Falied: Read 0 bytes.");
        return false; //should never be called.
    } else {
        throw new Exception("\r\nScale Indicator Data Read Falied: No ACK during read.");
        return false; //should never be called.
    }
}

function init_radio_write_read_pair($socket, $stringToWrite) {

    $retryCounter = 0;

    while ($retryCounter <= 5) {

//don't rety immediately.
        if ($retryCounter >= 1) {
            sleep(1);
        } //wait 1 sec


        init_radio_write($socket, $stringToWrite);


        $readResult = init_radio_read($socket);
        if ($readResult == TRUE) {
            break;
        }

        $retryCounter++;
    } //end while
//check if we sucessfully read or not.
    if ($readResult == FALSE) {
        throw new Exception("Base Radio Write/Read Failed.");
        return false; //should never be called.
    } else {
        return true;
    }
}

//write directly to base radio
function init_radio_write($socket, $stringToWrite) {
    print("\r\nWRITE (direct to base radio): " . "$stringToWrite" . "...\r\n");

//special case, the plusses need gaps between them and after.
    if ($stringToWrite == '+++') {
        sleep(1); //necessary to give a gap between +++ and last command or +++ does not work.
        socket_write($socket, '+', strlen('+'));
        usleep(50000);
        socket_write($socket, '+', strlen('+'));
        usleep(50000);
        socket_write($socket, '+', strlen('+'));
        usleep(250000);
    } else {
        socket_write($socket, $stringToWrite, strlen($stringToWrite));
    }
}

//read directly from base radio
function init_radio_read($socket) {

    print("READding response...");

    $buffer = null;
    while ($out = socket_read($socket, 8196, PHP_BINARY_READ)) {

//take characters as they come in and add them to the buffer.
        $buffer = $buffer . $out;

//if end of line occurs. break out of loop
        if (strpos($buffer, "\r") != FALSE) {
            break;
        }
    }//end socker read loop.
//clean up space,/r,/n etc
    $buffer = trim($buffer);

    if ($buffer == 'OK') {
        print("SUCCESS.\r\n");
        return true;
    } else {
        print("FAILURE.\r\n");
        return false; //should never be called.
    }
}

function ascii2hex($ascii) {
    $hex = '';
    for ($i = 0; $i < strlen($ascii); $i++) {
        $byte = strtoupper(dechex(ord($ascii{$i})));
        $byte = str_repeat('0', 2 - strlen($byte)) . $byte;
        $hex.=$byte . " ";
    }
    return $hex;
}

function beep($int_beeps = 1) {
    $string_beeps = null;
    for ($i = 0; $i < $int_beeps; $i++) {
        $string_beeps .= "\x07";
    }
    if (isset($_SERVER['SERVER_PROTOCOL']) == false) {
        print($string_beeps);
    }
}

/**
 *  Class to hold feedline information. Can make a persistant DB class by putting slect in constructor and update in destructor
 *
 */
class FeedLine {

    private $data = array();
    private $length = array();
    private $type = array();

    public function __construct($userData = null) {

//set what field lengths can be for the array
        $this->length['truckNumber'] = 6;
        $this->length['lineStatus'] = 1;
        $this->length['lineType'] = 1;
        $this->length['loadType'] = 1;
        $this->length['batchNumber'] = 4;
        $this->length['ingrediantPenCode'] = 6;
        $this->length['rationRecipeCode'] = 6;
        $this->length['presetCallWeight'] = 6;
        $this->length['loadedDeliveredWeight'] = 6;
        $this->length['userID'] = 8;
        $this->length['time24'] = 5;
        $this->length['dateFormat'] = 1;
        $this->length['date'] = 8;
        $this->length['headCount'] = 6;
        $this->length['newPresetCallWeightNextFeeding'] = 6;
        $this->length['feedZone'] = 1;
        $this->length['numberMixerRevs'] = 6;
        $this->length['grossWeightError'] = 6;
        $this->length['motionWeight'] = 3;
        $this->length['toleranceWeight'] = 3;

        $this->type = $this->length;

//set what the types can be for the array
        $this->type['truckNumber'] = 'character';
        $this->type['lineStatus'] = 'character';
        $this->type['lineType'] = 'character';
        $this->type['loadType'] = 'character';
        $this->type['batchNumber'] = 'numeric';
        $this->type['ingrediantPenCode'] = 'character';
        $this->type['rationRecipeCode'] = 'character';
        $this->type['presetCallWeight'] = 'numeric';
        $this->type['loadedDeliveredWeight'] = 'numeric';
        $this->type['userID'] = 'character';
        $this->type['time24'] = 'numeric';
        $this->type['dateFormat'] = 'numeric';
        $this->type['date'] = 'numeric';
        $this->type['headCount'] = 'numeric';
        $this->type['newPresetCallWeightNextFeeding'] = 'numeric';
        $this->type['feedZone'] = 'numeric';
        $this->type['numberMixerRevs'] = 'numeric';
        $this->type['grossWeightError'] = 'numeric';
        $this->type['motionWeight'] = 'numeric';
        $this->type['toleranceWeight'] = 'numeric';

        $this->data = $this->type;

//use an array trick to set all values to '', but maintain the keys.
        $this->data = array_combine(array_keys($this->data), array_keys($this->data));
        $this->data = array_fill_keys($this->data, null);

//fill with proper length blank strings.
//this is important if we just init the class and don't put any data in.
        foreach ($this->data as $member => $value) {
            self::__set($member, null);
        }

//if user supplied a data array, enter it via set.
        if (isset($userData)) {
            foreach ($userData as $member => $value) {
                self::__set($member, $value);
            }
        }
    }

    //return data member.
    public function __get($member) {
        if (isset($this->data[$member])) {
            return $this->data[$member];
        }
    }

    //set the data member after performing some sanity checks.
    public function __set($member, $value) {


        //check length array to see if member is valid, this allows null values to pass through
        if (isset($this->length[$member]) == false) {
            throw new Exception('FeedLine data member must exist: ' . "member:$member, value:$value.");
            return FALSE; //should never be called.
        }

        //check that the type is correct, let nulls pass through
        if (($value == null) || ($value == '')) {
            
        }//do nothing 
        else if (($this->type[$member] == 'character') && (is_string($value) != TRUE)) {
            throw new Exception('FeedLine data member must be character string: ' . "member:$member, value:$value.");
            return FALSE; //should never be called.
        } else if (($this->type[$member] == 'numeric') && (ctype_digit($value) == FALSE)) {
            throw new Exception('FeedLine data member must be numeric string: ' . "member:$member, value:$value.");
            return FALSE; //should never be called.
        } else {
            //value is ok
        }

        if (strlen($value) > $this->length[$member]) {
            throw new Exception('FeedLine data member length is longer than allowed length: ' . "member:$member, value:$value.");
            return FALSE; //should never be called.
        }

        $this->data[$member] = str_pad($value, $this->length[$member]); //pad string to specified length and set variable.
        return TRUE;
    }

    public function __destruct() {
        //$query = "UPDATE " . $this->table . " SET name = ?, 
        //	  email = ?, country = ? WHERE id = ?";
        //$this->dbh->query($query, $this->name, $this->email, 
        //	  $this->country, $this->id);
    }

//return data as array
    public function arr() {
        return $this->data;
    }

//return data as string
    public function string() {
        return implode(',', $this->data);
    }

//only really needs to be sent once, stores the data format used in the indicator.
    public function dataFieldFormatString() {
        $dataFormat['truckNumber'] = 'N6     ';
        $dataFormat['lineStatus'] = 'U ';
        $dataFormat['lineType'] = 'G ';
        $dataFormat['loadType'] = 'T ';
        $dataFormat['batchNumber'] = 'B4   ';
        $dataFormat['ingrediantPenCode'] = 'L6     ';
        $dataFormat['rationRecipeCode'] = 'R6     ';
        $dataFormat['presetCallWeight'] = 'P6     ';
        $dataFormat['loadedDeliveredWeight'] = 'A6     ';
        $dataFormat['userID'] = 'I8       ';
        $dataFormat['time24'] = 'C5    ';
        $dataFormat['dateFormat'] = 'F ';
        $dataFormat['date'] = 'D8       ';
        $dataFormat['headCount'] = 'H6     ';
        $dataFormat['newPresetCallWeightNextFeeding'] = 'E6     ';
        $dataFormat['feedZone'] = 'Z ';
        $dataFormat['numberMixerRevs'] = 'M6     ';
        $dataFormat['grossWeightError'] = 'W6     ';
        $dataFormat['motionWeight'] = 'm3  ';
        $dataFormat['toleranceWeight'] = 't3 ';

//return string
        return implode('', $dataFormat);
    }

    /**
     *  |||||||||||| CHECKSUM FUNCTION |||||||||||||||
     *
     * Page 14 of EZII Command Manual
     * Check Sum = XOR'ed bits of each character in the message.
     *            AND'ed with 63 to keep the lower 6 bits.
     *            Added with 64 to make it a printable character.
     * DW note: The "message" is really just the feedline part of the packet plus a chr(13).	    
     */
    public function checksumByte() {

        $feedlineStr = $this->string() . chr(13); //add char 13 for feedline calculation.
        $dataArr = str_split($feedlineStr);

//////////////////  VERIFIED BELOW ////////////////
//this code does XOR same as calculator.
        $xor = ($dataArr[0]);
        for ($i = 1; $i <= count($dataArr) - 1; $i++) {
            $xor = $xor ^ $dataArr[$i];
        }
        $answerXOR = $xor;
//////////////////  VERIFIED ABOVE ////////////////

        $a1 = ($answerXOR);
        $answerAND = $xor & chr(63);
        $a2 = ($answerAND);
        $answerADD = chr(ord($answerAND) + 64);
        $a3 = ($answerADD);
        $answer = $answerADD;

        return $answer;
    }

}

//end class
?>
