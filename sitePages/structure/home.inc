<?php
class Home extends BasePage {

    public function defaultDisplay() {

        //above all else, this is important.
        $this->treatedCowsinMilkingGroup();
        
        
        //only show to people actually working on the farm.
        if (in_array('employee', $GLOBALS['auth']->getAuthData('groups')) == TRUE) {
            $this->bootstrapWidgetEvents();
            $this->smallBoxes();
            //   $this->milkingShiftStats();
            $this->bootstrapWidget();
            $this->bootstrapWidget2();
           
            //TODO: finish.
           if ($GLOBALS['auth']->getOwnerAccess() == 1) {
           
               $contentArr=array();
           print (new BootStrap)->carousel('TIT:E',$contentArr );
           
                require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/directChat.inc');
                $x = new directChat();
                $x->example();
            }
        }


        $this->knob();
    }

    function smallBoxes() {
        
        print('<div class="row"> ');
        $this->crcBolus(); //ajax
        $this->needsMedicalTreatment(); //fast
        $this->greyBarnFed();  //slow 9 seconds
        $this->urbanFeeder(); //ajax      
        print('</div>');
        print('<div class="row"> ');
        $this->displayNeedsBredThisAm();  //ajax
        $this->displayMilkDroppedOff(); //ajax
        $this->needsMovedtoCalving(); //ajax
        $this->needsMovedto99Dry(); //ajax
        $this->displaySaleableMilk(); //ajax
        if ($GLOBALS['auth']->getOwnerAccess() == 1) {
        $this->displayAlproSyncWorking(); //ajax
        $this->displayAlproSortWorking(); //ajax
        $this->displayImportantErrors();
         }
        print('</div>');
    }
    
    function treatedCowsinMilkingGroup() {
            ?>   
        <div class="col-lg-3 col-xs-6">
            <!-- small box --> 
        <?php
        $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/html/72/milkWithHoldingAllCowsSaleableMilkGroup';
         print((new BootStrap)->bootstrapRawHtmlAjax($ajaxUrl));
        ?>

        </div>
        <?php
    }
    
    
    
    
    /*
     * process any REALLY important errors from error system and put on home page. 
     * currently only does order earltag, but eaasy to add others.
     */
    public function importantErrors() {
                      $sql1 = <<<SQL
SELECT * FROM batch.error_curr WHERE calling_class='EarTag' and level=1
SQL;
         $res1 = $GLOBALS['pdo']->query($sql1);
        
        //get data
        $results = $res1->fetchAll(PDO::FETCH_ASSOC);
        //process data
        $x=array();
        foreach ($results as &$row) {
            $x[]['home_page']= ($row['detail']);
          }
          
        //output
        $output['data'] = $x;
        return ($output);
               
    }
    
    //just checks the read teh db directly sync.
    public function alproSyncWorking() {
          $x=array();  
        
         $sql1 = "with temp as(
SELECT max(update_time) as update_time FROM alpro.cow LIMIT 1
)
select * from temp where update_time < current_timestamp - interval '12 hours' ";
        $res1 = $GLOBALS['pdo']->query($sql1);
        
        //get data
        $results = $res1->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
            $x[]['home_page']= 'Milk Weights Not Updated Since: ' . date('M d H:i',strtotime($row['update_time']));
              $x[]['importance'] = 1;
          }
       
          /* now check cron sync over io protocol is working */
          
            $sql2 = " with temp as (
                SELECT max(end_date) as update_time FROM alpro.cron LIMIT 1
)
select * from temp where update_time < current_timestamp - interval '20 minutes' ";
        $res2 = $GLOBALS['pdo']->query($sql2);
        
        //get data
        $results2 = $res2->fetchAll(PDO::FETCH_ASSOC);
         foreach ($results2 as &$row) {
           $x[]['home_page'] = 'Iso Protocol Sync Not Updated Since: ' . date('M d H:i',strtotime($row['update_time']));
           $x[]['importance'] = 1;
          }
  
        //output
        $output['data'] = $x;
        return ($output);
              
    }
    
    
    /*
     * ratio below .8 for 1:7 days production.
     * 
     */

    public function MilkDroppedOff() {
        $sql2 = "SELECT *,alpro.latest_single_milking(id), alpro.one_to_seven_day_milking_ratio(id) FROM bovinemanagement.bovinecurr WHERE location_id IN (39,40,41,42) AND alpro.one_to_seven_day_milking_ratio(id) <.8;";
        $res2 = $GLOBALS['pdo']->query($sql2);
        
        //get data
        $results = $res2->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
             $row['home_page']= '<a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['id']) . '">#' . $row['local_number'] .' - '.round($row['latest_single_milking'],1).'ℓ - ' .$row['location_name'] ."</a> ";
          }
          
        //output
        $output['data'] = $results;
        return ($output);
    }

    /*
     * 
     * uptining am or pm medical events that need done
     * 
     */

    public function medicalScheduledUpcoming() {
        if (date('a', strtotime("now")) == 'am') {
            $sql2 = "SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type=ANY(ARRAY['Scheduled Meds','Scheduled Action'])  AND (event_time >= current_date + interval '0 hours') AND  (event_time < current_date + interval '12 hours') 
		ORDER BY medical_summary.event_time ASC";
        } else { //pm
            $sql2 = "SELECT medical_summary.id,local_number,type,event_time,text,userid,full_name,bovine_id FROM bovinemanagement.medical_summary 
LEFT JOIN bovinemanagement.bovineall ON bovineall.id = medical_summary.bovine_id
		WHERE type=ANY(ARRAY['Scheduled Meds','Scheduled Action'])  AND (event_time >= current_date + interval '12 hours') AND  (event_time < current_date + interval '24 hours')
		ORDER BY medical_summary.event_time ASC";
        }
        $res2 = $GLOBALS['pdo']->query($sql2);

        
         //get data
        $results = $res2->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
            $time = date('H', strtotime($row['event_time']));
             $row['home_page']=  '' . $time . 'h <a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['bovine_id']) . '">' . $row['local_number'] . "</a> {$row['text']}";
          }
          
        //output
        $output['data'] = $results;
        return ($output);
        
    }

    
    
    public function lastParlorSaleableMilk() {
  $sql=<<<SQL
          
        with temp as (
SELECT date_trunc('day', dd):: date
FROM generate_series
        ((current_date - interval '4 days')::date
        , current_date
        , '1 day'::interval) dd
), temp2 as (
SELECT date_trunc, alpro.alpro_production_saleable_milk (date_trunc, 1) as one ,alpro.alpro_production_saleable_milk (date_trunc, 2)  as two From temp
)
SELECT * FROM temp2 where one is not null and two is not null ORDER BY date_trunc DESC
SQL;
  
   $res2 = $GLOBALS['pdo']->query($sql);
         //get data
        $results = $res2->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
            $time = date('D', strtotime($row['date_trunc']));
             $row2['home_page']=  '' . $time . '  AM: '. round($row['one']). ' ℓ'. '  PM: '. round($row['two']). ' ℓ <br/>';
             $out[]=$row2['home_page'];
             
          }
          
        //output
       // $output['data'] = $results;
        
        return implode($out);
        
        //eturn ($output);  
    }
    
    /*
     * anything 10 days to calving not in calving
     * 
     */

    
    //FIXME NOT IMPKEMENTED
    private function needsMovedFromSicktoLactating() {
        $sql2 = "SELECT * from bovinemanagement.pregnant_view WHERE calculated_potential_due_date-current_date <=10 AND location_id !=38";
        $res2 = $GLOBALS['pdo']->query($sql2);

        $out = '';
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $out = $out . '<a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row2['id']) . '">' . $row2['local_number'] . "</a> ";
        }
        return $out;
    }

    /*
     * anything 10 days to calving not in calving
     * 
     */

    public function needsMovedtoCalvingArea() {
        $sql2 = "with temp as (
SELECT *,bovinemanagement.short_name (full_name) as short_name FROM bovinemanagement.pregnant_view 
WHERE calculated_potential_due_date-current_date <=10 AND location_id !=38
)
SELECT *,(SELECT 1 where calculated_potential_due_date <= current_date + interval '6 days') as importance 
FROM temp";
        $res2 = $GLOBALS['pdo']->query($sql2);

        //get data
        $results = $res2->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
             $row['home_page']= '<a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['id']) . '">' . $row['local_number'] .' - '. $row['short_name']."</a> ";
          }
          
        //output
        $output['data'] = $results;
        return ($output);
    }

    /*
     * anything 18 days to calving not in pack or 99 dry.
     * 
     */

    //json
      public function needsMovedto99DryArea() {
   $sql= "SELECT * from bovinemanagement.pregnant_view WHERE calculated_potential_due_date-current_date <=18 AND location_id !=38 AND location_id !=37";
     $statement = $GLOBALS['pdo']->query($sql);
       
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
             $row['home_page']= '<a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['id']) . '">' . $row['local_number'] . "</a> ";
          }
          
        //output
        $output['data'] = $results;
        return ($output);
    }
    
  
    public function needsBredThisAm() {
        $sql = "SELECT * from bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON bovine_id=bovine.id WHERE actual_breeding_userid is null AND 	estimated_breeding_time <@ tsrange( current_date , current_date + interval '12 hours', '[)' )";
        $res = $GLOBALS['pdo']->query($sql);

        //get data
        $results = $res->fetchAll(PDO::FETCH_ASSOC);
        //process data
        foreach ($results as &$row) {
             $row['home_page']= '<a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($row['bovine_id']) . '">' . $row['local_number'] . "</a> ";
          }
          
        //output
        $output['data'] = $results;
        return ($output);
        
    }

    private function greyBarnCentreGroupFedToday() {
        require_once($_SERVER['DOCUMENT_ROOT'] . '/phpCronScripts/controlByWeb.php');
        $str = '';
        $obj = (new FeedAugerTimeLog)->todayFeedAugerUsage(11);
        if (!empty($obj)) {
            //arbitrary time of 60 seconds to say they were fed enough, adjust as needed.
            if ($obj->auger_on_time_seconds > 60) {
                $when = date('H:i', strtotime($obj->auger_last_on));
                $str = $str . "Fed Today @ $when.";
            } else {
                $str = $str . "NOT Fed Enough Today Yet.";
            }
        } else {
            $str = $str . "NOT Fed Today Yet.";
        }
        return $str;
        print($str);
    }

    
     public function displayImportantErrors() {    
   ?>

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <!-- small box -->
        <?php
          $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/importantErrors';
        print((new BootStrap)->bootstrapSmallBoxAjax('Show Stoppers', $ajaxUrl, 73, null, 'ion-ios-alarm'));
        ?>
        </div>
            <?php
        }
    
    public function displayAlproSyncWorking() {    
   ?>

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <!-- small box -->
        <?php
          $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/alproSyncWorking';
        print((new BootStrap)->bootstrapSmallBoxAjax('Alpro', $ajaxUrl, 73, null, 'ion-ios-alarm'));
        ?>
        </div>
            <?php
        }
        
         
        public function displayAlproSortWorking() {    
   ?>

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <!-- small box -->
        <?php
          $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/105/lastSyncStatus';
        print((new BootStrap)->bootstrapSmallBoxAjax('Alpro Sort Gate', $ajaxUrl, 73, null, 'ion-ios-alarm'));
        ?>
        </div>
            <?php
        }
        
        
        public function displaySaleableMilk2() {    
   ?>

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <!-- small box -->
        <?php
         (strstr('ddd', 'NOT')) ? $colour = 'yellow' : $colour = 'green';
          $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/lastParlorSaleableMilk';
        print((new BootStrap)->bootstrapSmallBoxAjax('Parlour Milk', $ajaxUrl, 73, $colour, 'fas fa-fill'));
        ?>
        </div>
            <?php
        }
    
        
        function displaySaleableMilk() {
        
        ?>   
       
        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <?php
            (strstr($this->lastParlorSaleableMilk(), 'NOT')) ? $colour = 'yellow' : $colour = 'green';
            print((new BootStrap)->bootstrapSmallBox('Parlour Milk',           $this->lastParlorSaleableMilk(), null, $colour, 'fas fa-fill'));
            ?>

        </div>
        <?php
    }
        

    function displayNeedsBredThisAm() {
        ?>

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <!-- small box -->
        <?php 
          $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/needsBredThisAm';
        print((new BootStrap)->bootstrapSmallBoxAjax('Breed this '.date('A',strtotime('now')), $ajaxUrl, 30, null, 'ion-ios-alarm'));
        ?>
        </div>
            <?php
        }

        function displayMilkDroppedOff() {
            ?>

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
        <?php
        $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/milkDroppedOff';
        print((new BootStrap)->bootstrapSmallBoxAjax('Down In Milk', $ajaxUrl, 1, null, 'ion-ios-alarm'));
        ?>
        </div>

            <?php
        }

        function needsMovedtoCalving() {
            ?>   

        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
        <?php
         $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/needsMovedtoCalvingArea';
        print((new BootStrap)->bootstrapSmallBoxAjax('Move to 99 Maternity Pack', $ajaxUrl, 116, null, 'ion-ios-alarm'));
        ?>

        </div>  
            <?php
        }

        function needsMovedto99Dry() {
            ?>
        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
        <?php
        $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/needsMovedto99DryArea';
        print((new BootStrap)->bootstrapSmallBoxAjax('Move to 99 Dry', $ajaxUrl, 116, null, 'ion-ios-alarm'));
        ?>

        </div>
            <?php
        }

        function crcBolus() {
            ?>   
        <div class="col-lg-3 col-xs-6">
            <!-- small box --> 
        <?php
        $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/90/crcBolusList';
        print((new BootStrap)->bootstrapSmallBoxAjax('Kexxtone Bolus Needed', $ajaxUrl, 90, null, 'ion-ios-alarm'));
        ?>

        </div>
        <?php
    }

    function greyBarnFed() {
        ?>   
       
        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <?php
            (strstr($this->greyBarnCentreGroupFedToday(), 'NOT')) ? $colour = 'yellow' : $colour = 'green';
            print((new BootStrap)->bootstrapSmallBox('Grey Barn Centre Group', $this->greyBarnCentreGroupFedToday(), null, $colour, 'ion-model-s'));
            ?>

        </div>
        <?php
    }

    function needsMedicalTreatment() {
        ?>   
        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
            <?php
            $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/1/medicalScheduledUpcoming';
            print((new BootStrap)->bootstrapSmallBoxAjax('Treat this ' . date('A', strtotime("now")), $ajaxUrl, 64, null, 'ion-ios-alarm'));
            ?>

        </div>
        <?php
    }

    /**
     * so the problem with this is it is javascript checking every 10 seconds for a change. Since the error is in json format the 
     * the smart thing would be to write a bootstrapSmallBox that does ajax and turns yellow if json string coming back is non empty */
    function urbanFeeder() {
        ?>  
        <div class="col-lg-3 col-xs-6">
            <!-- small box -->
        <?php
        $ajaxUrl = $GLOBALS['config']['HTTP']['URL_SHORT'].'api/json/53/currentUrbanFeederErrorMessages';
        print((new BootStrap)->bootstrapSmallBoxAjax('Urban Feeder Error', $ajaxUrl, 53, null, 'ion-ios-alarm'));
        ?>

        </div>
        <?php
    }

    /*
     * shows table of milking shift stats
     */

    function milkingShiftStats() {

        
        //{$GLOBALS['config']['HTTP']['JSON_API']}/128/milkingTimeStats
        

        $arr = array();

        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/reports/parlorMilking.inc');
        $c = new ParlorMilking();

        $resArr = $c->milkingTimeStats();

        print_r($resArr);

        $arr[] =$resArr[0];
        $arr[] =$resArr[1];
        $arr[] =$resArr[2];
        $arr[] =$resArr[3];
        /*
          //today am, yest pm, yest pm
          if ((date('G', strtotime('now')) >= 9) && (date('G', strtotime('now')) < 21)) {

          $arr[] = $c->milkingTimeStats(date("Y-m-d"), 1);
          $arr[] = $c->milkingTimeStats(date("Y-m-d", strtotime('-1 days')), 2);
          $arr[] = $c->milkingTimeStats(date("Y-m-d", strtotime('-1 days')), 1);
          }
          //9pm to 9am show yest pm, yest am, 2 days ago pm
          else {

          $arr[] = $c->milkingTimeStats(date("Y-m-d", strtotime('-1 days')), 2);
          $arr[] = $c->milkingTimeStats(date("Y-m-d", strtotime('-1 days')), 1);
          $arr[] = $c->milkingTimeStats(date("Y-m-d", strtotime('-2 days')), 2);
          }
         */
        ?>
        <div class="box box-info">
            <div class="box-header with-border">
                <h3 class="box-title">Latest Milkings</h3>

                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                    </button>
                    <button type="button" class="btn btn-box-tool" data-widget="remove"><i class="fa fa-times"></i></button>
                </div>
            </div>
            <!-- /.box-header -->
            <div class="box-body" style="">
                <div class="table-responsive">
                    <table class="table no-margin">
                        <thead>
                            <tr>
                                <th>Date</th>
                                <th>Milking Number</th>
                                <th>First Attach</th>
                                <th>Last Attach</th>
                                <th>First Attach Sick</th>
                                <th>Last Attach Sick</th>
                            </tr>
                        </thead>
                        <tbody>
        <?php
        foreach ($arr as $key1 => $value1) {
            print('<tr>');
            foreach ($value1 as $key => $value) {
                print('<td>' . $value . '</td>');
            }
            print('</tr>');
        }
        ?>

                        </tbody>
                    </table>
                </div>
                <!-- /.table-responsive -->
            </div>
            <!-- /.box-body -->

            <!-- /.box-footer -->
        </div>
        <?php
    }

    //jquery knob test
    function knob() {
        ?>
        <script type="text/javascript" src='https://cdnjs.cloudflare.com/ajax/libs/jQuery-Knob/1.2.13/jquery.knob.min.js'></script>

        <input type="text" value="95" class="dial">

        <script>
            $(".dial").knob({
                'change': function (v) {
                    console.log(v);
                },
                'readOnly': true

            });
        </script>
        <?php
    }

    function bootstrapWidget() {
        ?>
        <div class="col-sm-6 col-lg-3">
            <div class="panel panel-primary">
                <div class="panel-heading">
                    <h4 class="text-center">Emergency Contacts<span class="glyphicon glyphicon-phone pull-right"></span></h4>
                    <button class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i></button>
                </div>
                <div class="panel-body text-center">
                    <p class="lead">
                        <strong>Staff</strong>
                    </p>
                </div>
                <ul class="list-group list-group-flush">
                    <li class="list-group-item liitem"><strong>David (home):</strong>
                        <span class="pull-right">506-372-4564</span>
                    </li>
                    <li class="list-group-item liitem"><strong>David (cell):</strong>
                        <span class="pull-right"> 506-961-5091</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Fred (office):</strong>
                        <span class="pull-right">506-372-8812</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Fred (home):</strong>
                        <span class="pull-right">506-372-9255</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Fred (cell):</strong>
                        <span class="pull-right">506-866-7530</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Gurpreet:</strong>
                        <span class="pull-right">506-567-7756</span>
                    </li>
                     <li class="list-group-item liitem"><strong>Darcy:</strong>
                        <span class="pull-right">506-229-3998</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Matt:</strong>
                        <span class="pull-right">450-775-8765</span>
                    </li>
                     <li class="list-group-item liitem"><strong>Anne Roel:</strong>
                        <span class="pull-right">506-380-2855</span>
                    </li>  
                    <li class="list-group-item liitem"><strong>Nadine:</strong>
                        <span class="pull-right">506-378-0038</span>
                    </li>  
                    <li class="list-group-item liitem"><strong>Isaac:</strong>
                        <span class="pull-right">506-863-4590</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Gerry:</strong>
                        <span class="pull-right">506-863-7970</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Richard:</strong>
                        <span class="pull-right">506-372-9602</span>
                    </li>

                </ul>

                <div class="panel-footer">
                    <div class="row">
                        <div class="col-xs-4 col-sm-3 col-md-4 col-lg-2">
                        </div>
                        <div class="col-xs-2 col-sm-4 col-md-4 col-lg-4" id="qr1">
                        </div>
                        <div class="col-xs-6 col-sm-5 col-md-4 col-lg-6">
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <?php
    }

    function bootstrapWidget2() {
        ?>
        <div class="col-sm-6 col-lg-3">
            <div class="panel panel-primary">
                <div class="panel-heading">
                    <h4 class="text-center">Emergency Contacts<span class="glyphicon glyphicon-phone pull-right"></span></h4>
                    <button class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i></button>
                </div>
                <div class="panel-body text-center">
                    <p class="lead">
                        <strong>Suppliers</strong>
                    </p>
                </div>
                <ul class="list-group list-group-flush">
                    <li class="list-group-item liitem"><strong>Delaval - Fundy Ag (24/7):</strong>
                        <span class="pull-right">506-432-6944</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Delaval - Drew Gay:</strong>
                        <span class="pull-right">506-433-0361</span>
                    </li>
                    </li>
                    <li class="list-group-item liitem"><strong>Urban Calf Feeder - Ryan Weeks:</strong>
                        <span class="pull-right"> 1-902-393-1037</span>
                    </li>

                    <li class="list-group-item liitem"><strong>Houle - Sussex Farm Supplies:</strong>
                        <span class="pull-right"> 506-433-1699 / 506-434-1688</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Vet (24/7):</strong>
                        <span class="pull-right"> 506-856-2277</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Mechanic - Gary Tingley:</strong>
                        <span class="pull-right"> 506-372-4294 / 506-866-6112</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Cattle Shipping - Ron Beckwith:</strong>
                        <span class="pull-right"> 506-866-2130</span>
                    </li>
                    <li class="list-group-item liitem"><strong>Feed - Rob Lister:</strong>
                        <span class="pull-right"> 1-506-432-0175 or mill 1-800-663-4811</span>
                    </li>

                </ul>
                <div class="panel-footer">
                    <div class="row">
                        <div class="col-xs-4 col-sm-3 col-md-4 col-lg-2">
                        </div>
                        <div class="col-xs-2 col-sm-4 col-md-4 col-lg-4" id="qr1">
                        </div>
                        <div class="col-xs-6 col-sm-5 col-md-4 col-lg-6">
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <?php
    }

    
    public function vetCheckEvent() {
        require_once($_SERVER['DOCUMENT_ROOT'] . 'sitePages/medical/vetToCheck.inc');
        $obj=new QF2_VetToCheckEvent((object) array('forwardURLArr' => array('pageid' => $_REQUEST['pageid'])), 'group'); //do not need construcotr paramaters
        
      $out[]= '<li class="list-group-item liitem"><strong>Vet Check</strong>';
             $out[]='<span class="pull-right">'. $obj->whatDayIsTheNextVetCheck().'</span></li> ';
      return implode($out);               
    }
    
    
    function bootstrapWidgetEvents()  {
  
   
      
        $html = <<<HTML
 <div class="col-sm-6 col-lg-3">
            <div class="panel panel-danger">
                <div class="panel-heading">
                    <h4 class="text-center">Events<span class="glyphicon glyphicon-calendar pull-right"></span></h4>
                    <button class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i></button>
                </div>
                <div class="panel-body text-center">
                    <p class="lead">
                        <strong>Upcoming</strong>
                    </p>
                </div>
                   <!-- 
                <ul class="list-group list-group-flush">
                    <li class="list-group-item liitem"><strong>Milk Test</strong>
                        <span class="pull-right">Fri AM Feb 21</span>
                    </li>
                 -->  
                {$this->vetCheckEvent()}
                
        
              <!--         <li class="list-group-item liitem"><strong>Fred Away:</strong>
                        <span class="pull-right">Nov 12 to 13</span>
                    </li>
                  -->  
                </ul>
                

                <div class="panel-footer">
                    <div class="row">
                        <div class="col-xs-4 col-sm-3 col-md-4 col-lg-2">
                        </div>
                        <div class="col-xs-2 col-sm-4 col-md-4 col-lg-4" id="qr1">
                        </div>
                        <div class="col-xs-6 col-sm-5 col-md-4 col-lg-6">
                        </div>
                    </div>
                </div>
            </div>
        </div>
HTML;
       
       print($html);
    }
    
    
}

//end class
?>
