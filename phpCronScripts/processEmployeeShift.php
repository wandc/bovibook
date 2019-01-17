<?php 

if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
/*
 * Takes information from google calendar and put into a datble.
 */

class EmployeeShift {

    public function __construct() {
        //run main function
        print("\n\rStarting Employee Shift. \n\r");
        $this->doShiftProcessing();
        print("Ending Employee Shift. \n\r");
    }

  

    function doShiftProcessing() {

        // print("\n\rGrabing Calendar data.\n\r");

        include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/googleCalendar.inc');
        $x = new GoogleCalendar('littleriver.ca_3shnhbtafpt4dtkg078epi707c@group.calendar.google.com'); // work schedule calendar
        $dataArr = $x->grabEventsFromGoogleCalendarWorkSchedule();
        
// print("Starting DB insert.\n\r");
// Open a transaction
        try {$res = $GLOBALS['pdo']->beginTransaction();

        //empty the table first
        $res = $GLOBALS['pdo']->exec("TRUNCATE batch.employee_shift");
        {
            print_r($res);
          
        }


        foreach ($dataArr as $key => $value) {


            //$value['userid'];
            //$value['date'];
            //$value['shift'];         	
            //convert milking shift to a number to match alpro system.
            $milking_number = null;
            if ($value['shift'] == 'am') {
                $milking_number = 1;
            } elseif ($value['shift'] == 'pm') {
                $milking_number = 2;
            } else {
                throw new Exception("Shift not am pr pm! \n\r<br/>");
            }


            $query = "INSERT INTO batch.employee_shift (userid, date, shift,milking_number) VALUES ('{$value['userid']}', '{$value['date']}', '{$value['shift']}',$milking_number);";

            $res = $GLOBALS['pdo']->exec($query);
        }


        // determine if the commit or rollback

           $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
        
        //print("<h3>All done!</h3>");
    }

//end function
}

//end class

new EmployeeShift();  //initiate class (run it).
?>