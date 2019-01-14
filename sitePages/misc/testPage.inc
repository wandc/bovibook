<?php
//

//require_once($_SERVER['DOCUMENT_ROOT'].'/functions/croppingHelper.inc');
;



      

/**
  Test page to try new things with.
 */

 

   

class TestPage extends BasePage {

      function defaultDisplay() {
          
          print("h");

;
  $dt = new JQueryDataTable();
           print($dt->start($this->pageid, 'anywhereToMaternity',  null, $columnsStr, null, false," \"pageLength\": 7,\"order\": [[ 3, 'desc' ]],"));    
    }
    
    
    function timeline() {
        ?>
        <ul class="timeline">

    <!-- timeline time label -->
    <li class="time-label">
        <span class="bg-red">
            10 Feb. 2014
        </span>
    </li>
    <!-- /.timeline-label -->

    
    <li>
        <i class="fa fa-wrench bg-aqua"></i>
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> 5 mins ago</span>
            <h3 class="timeline-header no-border"><a href="#">Sarah Young</a> accepted your friend request</h3>
        </div>
    </li>
    
    
    
    <!-- timeline item -->
    <li>
        <!-- timeline icon -->
        <i class="fa fa-envelope bg-blue"></i>
        <div class="timeline-item">
            <span class="time"><i class="fa fa-clock-o"></i> 12:05</span>

            <h3 class="timeline-header"><a href="#">Support Team</a> ...</h3>

            <div class="timeline-body">
                ...
                Content goes here
            </div>

            <div class="timeline-footer">
                <a class="btn btn-primary btn-xs">...</a>
            </div>
        </div>
    </li>
    <!-- END timeline item -->

    ...

</ul>
      <?php
    }
    
    
    function shurgain() {
        /*
      
     with temp as (
SELECT distinct (item_num,uom), item_desc,sum(mass) as mass_sum,sum(price_total) as price_sum,tax_code FROM batch.shur_gain_invoice_data where event_time >= '18-09-01' and event_time < '18-10-01' group by item_num,uom,item_desc,tax_code
)
SELECT *,(SELECT sum(price_sum) from temp) as overall_sum from temp ;

/*
SELECT (item_num,uom), item_desc,(mass) as mass_sum,(price_total) as price_sum FROM batch.shur_gain_invoice_data where event_time >= '18-09-01' and event_time < '18-10-01' order by item_desc

*/
         
    }
    
    
    
       function openstreetmap() {
          
           
           ?>

  <div id="mapdiv"></div>
      <script src="https://openlayers.org/api/OpenLayers.js"></script>
 

  
  <script>
    map = new OpenLayers.Map("mapdiv");
    map.addLayer(new OpenLayers.Layer.OSM());

    var lonLat = new OpenLayers.LonLat( -64.9 ,45.9 )
          .transform(
            new OpenLayers.Projection("EPSG:4326"), // transform from WGS 1984
            map.getProjectionObject() // to Spherical Mercator Projection
          );
          
    var zoom=16;

    var markers = new OpenLayers.Layer.Markers( "Markers" );
    map.addLayer(markers);
    
    markers.addMarker(new OpenLayers.Marker(lonLat));
    
    map.setCenter (lonLat, zoom);
    
  </script>



<?php
      }

    
    
    /**
     * 
     * does not work.
     * 
     */
    function calf() {
        
        
       

        $link = "http://192.168.9.180/login.php"; //login first

        $ch = curl_init($link);
        if (!$ch) {
            die("Cannot allocate a new PHP-CURL handle");
        }

// some curl option to do with how data is returned.
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60);
        curl_setopt($ch, CURLOPT_HEADER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
         curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36'); 
        // post data for login
        $fields = array('username' => urlencode('anon'), 'password' => urlencode('anonanon'));
        //url-ify the data for the POST
        foreach ($fields as $key => $value) {
            $fields_string .= $key . '=' . $value . '&';
        }
        rtrim($fields_string, '&');
        curl_setopt($ch, CURLOPT_POST, count($fields));
        curl_setopt($ch, CURLOPT_POSTFIELDS, $fields_string);
        curl_setopt($ch, CURLOPT_COOKIEJAR, 'cookie-name');  //could be empty, but cause problems on some hosts
        curl_setopt($ch, CURLOPT_COOKIEFILE, '/tmp/phpcookie.tmp');  //could be empty, but cause problems on some hosts
//
// Grab the page and save the contents in the $data variable
        $data = curl_exec($ch);
        //search for holstein canada DB key
        if (preg_match_all('/SUCCESS/', $data, $match)) {
            //we have sucessfully logged in, now do the query we want.

            $link1 = 'http://192.168.9.180';  
           curl_setopt($ch, CURLOPT_URL, $link1);
curl_setopt($ch, CURLOPT_POST, false);
curl_setopt($ch, CURLOPT_POSTFIELDS, "");
$data2 = curl_exec($ch);
if (curl_error($ch)) {
    echo curl_error($ch);
}
           
            curl_close($ch);

            return $data2;
        } else {
            throw new Exception("Can't connect.");
        }
    }
    
    
      function chooseCowsToPutOnSalesListQuickForm() {

        $form = new HTML_QuickForm("salesListQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('header', 'hdrQuickform', 'Check Animals to Add to Sales List');
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
       



        //grab a list of cows with fresh date.
        $res = $GLOBALS['pdo']->query("
		SELECT id,local_number,score,location_name,date_trunc('day',current_date-bovinecurr.fresh_date) as dim,checked  FROM bovinemanagement.bovinecurr 
LEFT JOIN system.salesreport_temp ON salesreport_temp.bovine_id=bovinecurr.id
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
 ORDER BY local_number,location_name
		");
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

            //parse for last word of name and then make an array for the select list.
            $str = '#' . $row['local_number'] . '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ' . $row['dim'] . ' &nbsp;&nbsp;&nbsp;' . $row['location_name'] . ' &nbsp;&nbsp;&nbsp;' . $row['score'];

            //create checkboxes.
            $temp = $form->createElement('checkbox', $row['id'], '', "<b>$str</b> <hr>",array('style' => 'display: flex; width: 6px;'));
            if ($row['checked'] == true) {
                $temp->setChecked(true);
            } //if previously checked, pass it along.
            $checkGroup[] = $temp;
        }


        $form->addGroup($checkGroup, 'checkGroup');

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');




        // Try to validate a form
        if ($form->validate()) {
            print("VALIDATION<BR>");
            $checkArray = $form->exportValue('checkGroup');
            //print_r2($checkArray);

            $pageid = $form->exportValue('pageid');


            //fist step is to delete everything in the temp storage table.
            $res = $GLOBALS['pdo']->exec("TRUNCATE system.salesreport_temp"); //erase contents of temp db table.	

            //since there is so many checkboxes, we insert each one individually
            foreach ($checkArray as $k => $v) {
                //now we insert all the animals that were checked.
                $query = ("INSERT INTO system.salesreport_temp (bovine_id,checked) VALUES ($k,'$v')");
                $res = $GLOBALS['pdo']->exec($query);
            }

            //return to same page.
            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        }


        /* code to add css to renderer */
        $class = "noprint"; // name of css table class
        $renderer = $form->defaultRenderer(); // instantiate renderer
        $renderer->setFormTemplate("<form{attributes}><table class=\"$class\" border=\"0\">{content}</table></form>");  // change default template
        $form->accept($renderer);
        echo $renderer->toHtml(); //render
    } 
   
    
    /** shadow of mordor button */
function but() {
    ?>
<script>
    var Handler = function() {
       // create a function which sets an action to handle in 5 seconds.
       this.clickHandler = function() {
          var self = this;
          this.timerId = setTimeout(function() {
              console.log('fired!!!!');
              self.timerId = null;
              // do whatever here.
          },5000)
       };
       
       //creat a function which will cancel the previously scheduled task.
       this.cancelHandler = function() {
           // fire cancel logic if needed
           if (this.timerId) {
               console.log('cancelling');
               clearTimeout(this.timerId);
           }
       }
    }


$(function() {
 //Add custom event listener
$(':root').on('mousedown touchstart', '*', function() {
  var el = $(this),
    events = $._data(this, 'events');
  if (events && events.clickHold) {
    el.data(
      'clickHoldTimer',
      setTimeout(
        function() {
          el.trigger('clickHold')
        },
        el.data('clickHoldTimeout')
      )
    );
  }
}).on('mouseup mouseleave mousemove  touchcancel', '*', function() {
  clearTimeout($(this).data('clickHoldTimer'));
});

//Attach it to the element
$('#HoldListener').data('clickHoldTimeout', 1700); //Time to hold
$('#HoldListener').on('clickHold', function() {
      $('#HoldListener').removeClass('david').addClass('david2');
      $("#HoldListener").text("AM MILK COMPLETED");
      alert('Worked!');
});
});

    </script>
    <style>
        .david {
  color: #31302B;
    background: #FFF;
    padding: 12px 17px;
    margin: 25px;
    font-family: 'OpenSansBold', sans-serif;
    border: 3px solid #31302B;
    font-size: 14px;
    font-weight: bold;
    letter-spacing: 1px;
    text-transform: uppercase;
    border-radius: 2px;
    display: inline-block;
    text-align: center;
    cursor: pointer;
    -webkit-touch-callout: none;
    -webkit-user-select: none; /* Disable selection/copy in UIWebView */
    box-shadow: inset 0 0 0 0 #31302B;
	-webkit-transition: all ease-in 1.5s;
	-moz-transition: all ease-in 1.5s;
	transition: all ease-in 1.5s;
}
.david:active {
    box-shadow: inset 100px 0 0 0 #e0e0e0;
    color: #fff;
}
        .david2 {
  color: #31302B;
    background: greenyellow;
    padding: 12px 17px;
    margin: 25px;
    font-family: 'OpenSansBold', sans-serif;
    border: 3px solid #31302B;
    font-size: 14px;
    font-weight: bold;
    letter-spacing: 1px;
    text-transform: uppercase;
    border-radius: 2px;
    display: inline-block;
    text-align: center;
   cursor: not-allowed;
    box-shadow: inset 0 0 0 0 #31302B;
    -webkit-touch-callout: none;
-webkit-user-select: none; /* Disable selection/copy in UIWebView */
	
}  
        
      #HoldListener  { 
   
      }
    #HoldListener:hover {
   
}  
      
      
    </style>

    <button class="david" id="HoldListener">AM Milk</button>
    <?php
}
    
  
    
    
      public function bovineSelectQuickForm5($mode = 'default') {
        
        
        
        if ($mode=='default') {
           $style='bovineSelectPage';
            $aniNumber = Misc::createListOfAllAliveCows(false);
        }
        else {
           $style='bovineSelect';  
           $aniNumber = Misc::createListOfAllAliveCows(true);
        }
    
        $pageid=6;
     
        //javascript to allow submission on change of select element.
        $attrs = array('id' => "$style", 'onchange' => "javascript:location.href='?bovine_id='+this.options[this.selectedIndex].value+'&pageid=$pageid';$('#example > ul').tabs('select', 0);");

        //javascript to allow submission when enter is pressed within select form
        //$attrs= array('onKeyPress' => "javascript:if(event.keyCode=='13'){document.frmTest.submit();}");

        $form = new HTML_QuickForm("animalBovineQuickSelect", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
     
        
        //$form->addElement('header', 'hdrQuickfrmbovineQuickSelect','Select a bovine');
        $form->addElement('select', 'bovineNumber', '', $aniNumber, $attrs);
        $form->addElement('hidden', 'pageid', $pageid);

        //defaults
        if ((isset( $this->bovine_id) ) && ($this->bovine_id != null)) {
            $form->setDefaults(array('bovineNumber' => $this->bovine_id)); //set to current bovine
        } else {
            $form->setDefaults(array('bovineNumber' => 0)); //set default to no.  
        }


        // Try to validate a form
        if ($form->validate()) {

            //get value
            $this->bovine_id = $form->exportValue('bovineNumber');
            $this->pageid = $form->exportValue('pageid');
        } //end validation
     
        $form->accept($renderer);
        return $renderer->toHtml();
    }
    
    
    function select() {
        
        
        
        
        ?>


<select class="js-example-basic-multiple" name="states[]" multiple="multiple">
  <option value="AL">Alabama</option>
 <option value="volvo">Volvo</option>
  <option value="saab">Saab</option>
  <option value="mercedes">Mercedes</option>
  <option value="audi">Audi</option>
  <option value="WY">Wyoming</option>
</select>


<script type="text/javascript">
$(document).ready(function() {
    $('.js-example-basic-multiple').select2();
});
</script>


<?php
    }
    
    
    function tmr() {
        print ("<h1>TMR</h1>");
          print ("<div id='idtest'>TMR</div>");
        ?>
    <script type="text/javascript">
    $( document ).ready(function() {
    console.log( "ready!" );
    $('#idtest').load("http://tmr.littleriver.ca:5000")
 /*   var source = new EventSource("http://tmr.littleriver.ca:5000");
source.onmessage = function(e) {
    console.log(e);
}; */
});
    
  

</script>
        <?php
    }
    
    function beep() {
        ?>

<script>
function beep() {
    var snd = new  Audio("data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");  
    snd.play();
}


document.getElementsByTagName("button")[0].addEventListener("click",    beep);
</script>
<button type="button">Beep!</button>
<?php        
    }
    
       function getWhoIsLoggedIn() {
        if ($GLOBALS['auth']->checkAuth()) {
            $loginName = ucwords($GLOBALS['auth']->getUsername()); //grab current user name and make first letter upper-case.
            return("<h2 class=\"loginNameBox\">$loginName</h2>");
        } else {
            return('<!--No One Logged In-->');
        }
    }
    
    function bootstrapmenu() {
        ?>

<ul>
 <li class="dropdown messages-menu">
            <!-- Menu toggle button -->
        
            <a href="#" class="dropdown-toggle" data-toggle="dropdown">
              <i class="fa fa-envelope-o"></i>
              <span class="label label-success">5</span>
            </a>
            
            <ul class="dropdown-menu">
              <li class="header">You have 4 messages</li>
              <li>
                <!-- inner menu: contains the messages -->
                <ul class="menu">
                  <li><!-- start message -->
                    <a href="#">
                      <div class="pull-left">
                        <!-- User Image -->
                        <img src="<?php echo($GLOBALS['config']['ADMINLTE']['path']);?>dist/img/user2-160x160.jpg" class="img-circle" alt="User Image">
                      </div>
                      <!-- Message title and timestamp -->
                      <h4>
                        Support Team
                        <small><i class="fa fa-clock-o"></i> 5 mins</small>
                      </h4>
                      <!-- The message -->
                      <p>Why not buy a new awesome theme?</p>
                    </a>
                  </li>
                  <!-- end message -->
                </ul>
                <!-- /.menu -->
              </li>
              <li class="footer"><a href="#">See All Messages</a></li>
            </ul>
          </li>
          <!-- /.messages-menu -->
</ul>

       

        <?php
    }

 

    function ddd666() {
        include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/.Calendar.inc');
        $gCal = new GoogleCalendar('littleriver.ca_phrs87e7efrsf6qc0hk4bfk88g@group.calendar.google.com'); //bovine_treatement
    }

    function ddd555() {
        $event_time = '2016-03-03 10:00';
        $startDate = date('Y-m-d', strtotime($event_time));
        $startTime = date('H:i:00', strtotime($event_time));


        try {
            include_once($_SERVER['DOCUMENT_ROOT'] . 'functions/googleCalendar.inc');
            $gCal = new GoogleCalendar('littleriver.ca_phrs87e7efrsf6qc0hk4bfk88g@group.calendar.google.com'); //bovine_treatement
            $eventId = null;
            $eventId = $gCal->createEvent($gCal->calendarID, $title, $description, '99 Milky Way,Colpitts Settlement,NB,Canada', $startDate, $startTime, $startDate, $startTime, '-04');
        } catch (Exception $e) {
            echo 'Caught Google Calendar exception and bypassing: ', $e->getMessage(), "\n";
            return 'NOTINCAL_' . uniqid();
        }
    }

    function defaultDisplay44444() {

        /*
         * below works, but is a service account accessing its own calendar...we want a service account to access calendar@littleriver.ca, not the same.
         * 
         * 
         */
   

        $client = new Google_Client();

        $client->setAuthConfig('Calendar API Project-790d47705ed2.json');
        $user_to_impersonate = 'calendar@littleriver.ca';
        $client->setSubject($user_to_impersonate);


//putenv('GOOGLE_APPLICATION_CREDENTIALS=/var/www/int/framework/google-keys/Calendar API Project-85e5cd44b681.json');
//$client = new Google_Client();
//$client->useApplicationDefaultCredentials();



        $scopes = array('https://www.googleapis.com/auth/calendar');
        $client->setScopes($scopes);
        $this->cal = new Google_Service_Calendar($client);






        $calendarList = $this->cal->calendarList->listCalendarList();
        print_r($calendarList);
        while (true) {
            foreach ($calendarList->getItems() as $calendarListEntry) {
                echo $calendarListEntry->getSummary();
            }
            $pageToken = $calendarList->getNextPageToken();
            if ($pageToken) {
                $optParams = array('pageToken' => $pageToken);
                $calendarList = $this->cal->calendarList->listCalendarList($optParams);
            } else {
                break;
            }
        }


        $events = $this->cal->events->listEvents('primary');

        while (true) {
            foreach ($events->getItems() as $event) {
                echo $event->getSummary();
            }
            $pageToken = $events->getNextPageToken();
            if ($pageToken) {
                $optParams = array('pageToken' => $pageToken);
                $events = $this->cal->events->listEvents('primary', $optParams);
            } else {
                break;
            }
        }




        $calendarListEntry = $this->cal->calendarList->get('calendarId');
        echo $calendarListEntry->getSummary();


        /*
          $sqladmin = new Google_Service_SQLAdmin($client);
          $response = $sqladmin->instances
          ->listInstances('examinable-example-123')->getItems();
          echo json_encode($response) . "\n";
         * 
         * 
         * 
         */
    }

    function defaultDisplayTrilaterionOLD() {





        $point1 = [4.0, 4.0];
        $point2 = [9.0, 7.0];
        $point3 = [9.0, 1.0];
        $r1 = 4;
        $r2 = 3;
        $r3 = 3.25;

        /*
          $point1 = [0,160];
          $point2 = [123,180];
          $point3 = [15,337];
          $r1 = 77;
          $r2 = 93;
          $r3 = 98;
         */

        $res = $this->trilateration($point1, $point2, $point3, $r1, $r2, $r3);
        var_dump($res);
    }

    //based on: http://stackoverflow.com/questions/29656921/trilateration-2d-algorithm-implementation which is based on wikipedia
    function trilateration(array $point1, array $point2, array $point3, $r1, $r2, $r3) {

        $resultPose = array();
        //unit vector in a direction from $point1 to $point 2
        $p2p1Distance = pow(pow($point2[0] - $point1[0], 2) + pow($point2[1] - $point1[1], 2), 0.5);
        $exx = ($point2[0] - $point1[0]) / $p2p1Distance;
        $exy = ($point2[1] - $point1[1]) / $p2p1Distance;

        $i = $exx * ($point3[0] - $point1[0]) + $exy * ($point3[1] - $point1[1]);


        //the unit vector in the y direction. 
        $eyx = ($point3[0] - $point1[0] - $exx) / pow(pow($point3[0] - $point1[0] - $exx, 2) + pow($point3[1] - $point1[1] - $exy, 2), 0.5);
        $eyy = ($point3[1] - $point1[1] - $exy) / pow(pow($point3[0] - $point1[0] - $exx, 2) + pow($point3[1] - $point1[1] - $exy, 2), 0.5);
        //the signed magnitude of the y component
        $j = $eyx * ($point3[0] - $point1[0]) + $eyy * ($point3[1] - $point1[1]);

        //coordinates
        $x = (pow($r1, 2) - pow($r2, 2) + pow($p2p1Distance, 2)) / (2 * $p2p1Distance);
        $y = (pow($r1, 2) - pow($r3, 2) + pow($i, 2) + pow($j, 2)) / (2 * $j) - $x;


        //result coordinates
        $finalX = $point1[0] + $x * $exx + $y * $eyx;
        $finalY = $point1[1] + $x * $exy + $y * $eyy;
        $resultPose['finalX'] = $finalX;
        $resultPose['finalY'] = $finalY;
        return $resultPose;
    }

    function defaultDisplay44q332() {

        require_once('HTML/QuickForm2.php');
        $form = new HTML_QuickForm2('ContactForm', 'post', array('action' => $_SERVER['REQUEST_URI']));
        // Let's start with a fieldset
        $fs = $form->addFieldset('contact_us')->setLabel("Contact us");

// Add a title select box
        $title = $fs->addSelect('title')
                ->setLabel("Title : ")
                ->loadOptions(array('' => "Select your title...", "Mr.", "Ms"));

// Add a name text field
        $name = $fs->addText('name')->setLabel("Name : ");

// Add a comment textarea with 6 rows
        $comment = $fs->addTextarea('comment', array('rows' => 6))
                ->setLabel("Comments : ");

// Add the submit button to our form (outside of our fieldset)
        $form->addSubmit('submit', array('value' => "Send us your comments"));

        $name->setAttribute('maxlength', 64);
        $title->setAttribute('style', 'width:200px');

        $title->addRule("required", "Please specify your title");
        $name->addRule("required", "Please specify your name");
        $comment->addRule("required", "Please enter some comments");


// Apply the trim() function recursively on all form children elements
        $form->addRecursiveFilter('trim');

        if ($form->validate()) {
            echo "Success"; // Use a redirect here...
        } else {
            echo $form;
        }
    }

    /* nutrition.recipe_acitive //delete this if this function is not needed.

      WITH teemp_table AS (
      SELECT recipe_item.id AS recipe_item_id,
      recipe.comment2,
      feedcurr.feed_type,
      feedcurr.location,
      recipe_item.kg_day_dry,
      recipe_item.recipe_id,
      recipe_item.feedcurr_id,
      recipe_item.round_to_tens,
      recipe_item.rounding_accuracy,
      feedcurr.default_grain_ratio,
      feedcurr.default_forage_ratio,
      recipe_item.fed_free_choice,
      abs(100::numeric - feedcurr.dry_matter_percent) AS moisture_percent,
      recipecurr2.total_real_location_count AS recipe_count,
      recipecurr2.total_recipe_weighted_bovine_count AS total_weighted_bovine_count,
      recipe_item.kg_day_dry AS dry_ingred_mass,
      recipe_item.kg_day_dry * (1::numeric / (feedcurr.dry_matter_percent / 100::numeric)) AS wet_ingred_mass,
      feedcurr.cost
      FROM recipe
      LEFT JOIN ( SELECT DISTINCT recipecurr2_1.recipe_id,
      recipecurr2_1.recipe_start_timestamp,
      recipecurr2_1.recipe_stop_timestamp,
      recipecurr2_1.timestamp_of_interest,
      recipecurr2_1.total_recipe_as_fed_kg,
      recipecurr2_1.comment2,
      recipecurr2_1.total_real_location_count,
      recipecurr2_1.total_recipe_weighted_bovine_count
      FROM recipecurr2 recipecurr2_1) recipecurr2 ON recipecurr2.recipe_id = recipe.id
      LEFT JOIN recipe_item ON recipe_item.recipe_id = recipe.id
      LEFT JOIN feedcurr ON recipe_item.feedcurr_id::text = feedcurr.id
      WHERE recipe.active = true OR recipe.editable = true
      ), teemp_table2 AS (
      SELECT teemp_table.recipe_item_id,
      teemp_table.comment2,
      teemp_table.feed_type,
      teemp_table.location,
      teemp_table.kg_day_dry,
      teemp_table.recipe_id,
      teemp_table.feedcurr_id,
      teemp_table.cost,
      teemp_table.moisture_percent,
      teemp_table.round_to_tens,
      teemp_table.rounding_accuracy,
      teemp_table.default_grain_ratio,
      teemp_table.default_forage_ratio,
      teemp_table.fed_free_choice,
      teemp_table.recipe_count,
      teemp_table.total_weighted_bovine_count,
      teemp_table.dry_ingred_mass,
      teemp_table.wet_ingred_mass,
      ( SELECT sum(tt.dry_ingred_mass) AS sum
      FROM teemp_table tt
      WHERE tt.recipe_id = teemp_table.recipe_id) AS dry_ingred_mass_recipe,
      ( SELECT sum(tt.wet_ingred_mass) AS sum
      FROM teemp_table tt
      WHERE tt.recipe_id = teemp_table.recipe_id) AS wet_ingred_mass_recipe
      FROM teemp_table
      ), teemp_table3 AS (
      SELECT teemp_table2.feed_type,
      teemp_table2.comment2,
      teemp_table2.location,
      teemp_table2.kg_day_dry,
      teemp_table2.recipe_id,
      teemp_table2.recipe_item_id,
      teemp_table2.feedcurr_id,
      teemp_table2.moisture_percent,
      teemp_table2.recipe_count,
      teemp_table2.total_weighted_bovine_count,
      teemp_table2.dry_ingred_mass,
      teemp_table2.wet_ingred_mass,
      teemp_table2.dry_ingred_mass_recipe,
      teemp_table2.wet_ingred_mass_recipe,
      teemp_table2.total_weighted_bovine_count * teemp_table2.wet_ingred_mass AS ingrediant_load_amount,
      teemp_table2.total_weighted_bovine_count * teemp_table2.dry_ingred_mass * 0.001 * teemp_table2.cost AS ingrediant_load_cost,
      teemp_table2.round_to_tens,
      teemp_table2.rounding_accuracy,
      teemp_table2.default_grain_ratio,
      teemp_table2.default_forage_ratio,
      teemp_table2.fed_free_choice,
      100::numeric - teemp_table2.dry_ingred_mass_recipe / teemp_table2.wet_ingred_mass_recipe * 100::numeric AS recipe_moisture_percent,
      teemp_table2.wet_ingred_mass_recipe * teemp_table2.recipe_count::numeric AS recipe_load_amount,
      teemp_table2.wet_ingred_mass_recipe * teemp_table2.total_weighted_bovine_count AS total_weighted_recipe_load_amount
      FROM teemp_table2
      )
      SELECT teemp_table3.feed_type,
      teemp_table3.location,
      teemp_table3.kg_day_dry,
      teemp_table3.recipe_id,
      teemp_table3.recipe_item_id,
      teemp_table3.feedcurr_id,
      teemp_table3.moisture_percent,
      teemp_table3.recipe_count,
      teemp_table3.total_weighted_bovine_count,
      teemp_table3.dry_ingred_mass,
      teemp_table3.wet_ingred_mass,
      teemp_table3.dry_ingred_mass_recipe,
      teemp_table3.wet_ingred_mass_recipe,
      teemp_table3.ingrediant_load_amount,
      teemp_table3.ingrediant_load_cost,
      teemp_table3.round_to_tens,
      teemp_table3.rounding_accuracy,
      teemp_table3.default_grain_ratio,
      teemp_table3.default_forage_ratio,
      teemp_table3.fed_free_choice,
      teemp_table3.recipe_moisture_percent,
      teemp_table3.recipe_load_amount,
      teemp_table3.total_weighted_recipe_load_amount,
      teemp_table3.comment2
      FROM teemp_table3
      ORDER BY teemp_table3.recipe_id, teemp_table3.feedcurr_id;


     */

    function defaultDisplay42377() {


        $access_token = rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');
        print($access_token);
        print("    ");
        print_r($_SESSION);
    }

    function api() {

        //($this->app);
        echo("in test page");
        //var_dump($this->app);
        //  $rootUri=$this->app->request->getRootUri();


        $xxx = $this;

        $this->app->get('/book', function ($request, $response, $args) use($xxx) {
            print("bookj");
            //$xxx->callPageObject($args['pageid'], true); //calls api code.
        });
    }

    function defaultDisplaySlimFramework() {





        $access_token = rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');
        print($access_token);
        print("    ");
        print_r($_SESSION);


        //grab a list of heat events.
        $res = $GLOBALS['pdo']->query("SELECT id,name from bovinemanagement.estrus_type WHERE display_order > 0 ORDER BY display_order");

        $heatEvents[null] = "Please Select one..."; //dummy value
        while (($row = $res->fetch())) {
            //parse for last word of name and then make an array for the select list.
            $heatEvents[$row[0]] = $row[1];
        }
        $json = json_encode(($heatEvents));
        ?>
        <script src="//ajax.googleapis.com/ajax/libs/angularjs/1.5.0/angular.min.js"></script>



        </head>
        <body ng-app="formExample">
            <div ng-controller="ExampleController">
                <form novalidate class="simple-form">

                    Name: <input type="text" ng-model="user.name" /><br />
                    E-mail: <input type="email" ng-model="user.email" /><br />
                    Gender: <input type="radio" ng-model="user.gender" value="male" />male
                    <input type="radio" ng-model="user.gender" value="female" />female<br />

                    <select ng-model="user.Date" ng-options="item.Dates as item.Dates for item in Dates">
                        <option value="">Select From Date</option>
                    </select>


                    <select ng-model="user.jsonPartners" ng-options="place for place in jsonPartners" ></select>


                    <input type="button" ng-click="reset()" value="Reset" />
                    <input type="submit" ng-click="update(user)" value="Save" />
                </form>
                <pre>user = {{user| json}}</pre>
                <pre>master = {{master| json}}</pre>
            </div>

            <script>

                angular.module('formExample', [])


                        .controller('ExampleController', ['$scope', function ($scope) {

                                $scope.jsonPartners =<?php echo($json); ?>;
                                $scope.service_id = $scope.jsonPartners[1];


                                $scope.Dates = <?php echo($json); ?>;

                                console.log($scope.Dates);



                                $scope.master = {};

                                $scope.update = function (user) {
                                    $scope.master = angular.copy(user);
                                };

                                $scope.reset = function () {
                                    $scope.user = angular.copy($scope.master);
                                };

                                $scope.reset();
                            }]);
            </script>
        </body>
        </html>

        <?php
    }

    function defaultDisplay423() {
        ?>
        <link rel="stylesheet" href="/framework/Zebra_Form/public/css/zebra_form.css">
        <script src="/framework/Zebra_Form/public/javascript/zebra_form.js"></script>
        <?php
     
        // include the Zebra_Form class
        require_once($_SERVER['DOCUMENT_ROOT'] . '/framework/Zebra_Form/Zebra_Form.php');
        print("http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]");
        // instantiate a Zebra_Form object
        $form = new Zebra_Form('form', 'post', "https://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]");

        // the label for the "email" element
        $form->add('label', 'label_email', 'email_xxx', 'EmailX');
        $form->add('hidden', 'pageid', '55');
        // add the "email" element
        $obj = $form->add('text', 'email_xxx', '', array('autocomplete' => 'off'));

        // set rules
        $obj->set_rule(array(
            // error messages will be sent to a variable called "error", usable in custom templates
            'required' => array('error', 'Email is required!'),
            'email_xxx' => array('error', 'Email address seems to be invalid!'),
        ));

        // "password"
        $form->add('label', 'label_password', 'password_xxx', 'PasswordX');
        $obj = $form->add('password', 'password_xxx', '', array('autocomplete' => 'off'));
        $obj->set_rule(array(
            'required' => array('error', 'Password is required!'),
            'length' => array(6, 10, 'error', 'The password must have between 6 and 10 characters!'),
        ));

        // "remember me"
        $form->add('checkbox', 'remember_me', 'yes');
        $form->add('label', 'label_remember_me_yes', 'remember_me_yes', 'Remember me', array('style' => 'font-weight:normal'));

        // "submit"
        $form->add('submit', 'btnsubmit', 'Submit');

        // if the form is valid
        if ($form->validate()) {

            // show results
            // show_results();

            print("Submitted!");
            exit();

            // otherwise
        } else

        // auto generate output, labels above form elements
            $form->render();
    }

 

    function defaultDisplayQuickForm2() {
        // Load the main class
        require_once 'HTML/QuickForm2.php';

// Instantiate the HTML_QuickForm2 object
        $form = new HTML_QuickForm2('tutorial', 'post', $_SERVER ["REQUEST_URI"]);

// Set defaults for the form elements
        $form->addDataSource(new HTML_QuickForm2_DataSource_Array(array(
            'name' => 'Joe User'
        )));

// Add some elements to the form
        $fieldset = $form->addElement('fieldset')->setLabel('QuickForm2 tutorial example');
        $name = $fieldset->addElement('text', 'name', array('size' => 50, 'maxlength' => 255))
                ->setLabel('Enter your name:');
        $fieldset->addElement('submit', null, array('value' => 'Send!'));

// Define filters and validation rules
        $name->addFilter('trim');
        $name->addRule('required', 'Please enter your name');

// Try to validate a form
        if ($form->validate()) {
            echo '<h1>Hello, ' . htmlspecialchars($name->getValue()) . '!</h1>';
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}&bovine_id={$_REQUEST['bovine_id']}");
            exit();
        }

// Output the form
        echo $form;
    }

    function defaultDisplayZEBRA() {
        print("<h1>HELLOM</h1>");
        
        ?>
        <meta charset="utf-8">
        <link rel="stylesheet" href="/framework/Zebra_Form/public/css/zebra_form.css">
        <script src="/framework/Zebra_Form/public/javascript/zebra_form.js"></script>
        <?php
        require_once($_SERVER['DOCUMENT_ROOT'] . '/framework/Zebra_Form/Zebra_Form.php');


        // instantiate a Zebra_Form object
        $form = new Zebra_Form('forfdsdfsdfsm');



        // "message"
        $form->add('label', 'label_message', 'message', 'Message:');
        $obj = $form->add('textarea', 'message');
        $obj->set_rule(array(
            'required' => array('error', 'Message is required!'),
            'length' => array(0, 140, 'error', 'Maximum length is 140 characters!', true)
        ));

        // "submit"
        $form->add('submit', 'btnsubmitd', 'Submit');

        // if the form is valid
        if ($form->validate()) {

            // show results
            //  show_results();
            print("dsadsadsa");
            // otherwise
        } else {
            print_r($form);
            // generate output using a custom template
            $form->render();
        }
    }

//end function
}

//end class
/*

  SELECT rc.constraint_catalog,
  rc.constraint_schema||'.'||tc.table_name AS table_name,
  kcu.column_name,
  match_option,
  update_rule,
  delete_rule
  FROM information_schema.referential_constraints AS rc
  JOIN information_schema.table_constraints AS tc USING(constraint_catalog,constraint_schema,constraint_name)
  JOIN information_schema.key_column_usage AS kcu USING(constraint_catalog,constraint_schema,constraint_name)
  JOIN information_schema.key_column_usage AS ccu ON(ccu.constraint_catalog=rc.unique_constraint_catalog AND ccu.constraint_schema=rc.unique_constraint_schema AND ccu.constraint_name=rc.unique_constraint_name)
  WHERE
  ccu.table_schema='bovinemanagement'
  AND ccu.table_name='bovine'
  AND ccu.column_name='id';
 */
?>