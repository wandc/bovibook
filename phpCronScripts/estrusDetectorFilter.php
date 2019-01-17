<?php

/*
 * This class will execute DB queries run by a cron script to do the filtering of the data. Should be run every hour or so.
 * 
 */
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
}
require_once($_SERVER['DOCUMENT_ROOT'].'global.php');

/*
 * Takes information from google calendar and put into a datble.
 */

class EstrusDetectorFilter {

    public function __construct() {
        
    }

        
    function main() {
        print('<h1>Started processing EstrusDetectorFilter.</h1>' . "\n\r" . "\n\r");
      
        $this->mainA();
        $this->mainB();
        $this->mainDeleteOldData();
        print('<h1>Finished processing EstrusDetectorFilter.</h1>' . "\n\r" . "\n\r");
    }

    function mainA() {
        
        
        
                  /*
             * DELETE cell phone and other extraneous bluetooth signals from data  
             */
            $sql1 = <<<SQL1
 	DELETE FROM bas.ble_tag_event lnk WHERE lnk.tag_id NOT IN (SELECT tag_id FROM bas.ble_valid_tag_anytime itm) 
SQL1;
            $res = $GLOBALS['pdo']->exec($sql1);

            
         //check when the oldest data in the ble_tag_event is. If it is older then 21 days, lets throw an exception. 
         //the reason for this is that if we try and do more then 21 days at a time we run out of disk space on the server.        
                    $sql44 = <<<SQL44
 	SELECT event_time FROM bas.ble_tag_event ORDER by event_time ASC limit 1
SQL44;
            $res = $GLOBALS['pdo']->query($sql44);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if (strtotime($row['event_time']) <= strtotime(date('21 days ago'))) {
                throw new Exception("Error: Cannot run because this has not been run in so long that there is more then 21 days of data. Run manually in smaller chunks until you have less then 21 days.");
            }
            
       

            
            
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();
            
            /* Main Filter Step to pivot data and put into 20 second time slices. 
             * 
             * THe key to this is that if it is run multiple times it must divide up into the same time slices. Is that what is occuring?
             */
            $sql2 = <<<SQL2
 with temp as 
(SELECT distinct(x.tag_id,x.event_time),x.event_time,x.tag_id, 
         (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:38:ff:8d' and event_time=x.event_time) as base_ff8d_rssi, 
         (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:cf:fe:95' and event_time=x.event_time) as base_fe95_rssi, 
         (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:06:eb:c0' and event_time=x.event_time) as base_ebc0_rssi,  
         (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:5c:b7:0e' and event_time=x.event_time) as base_b70e_rssi,
         (SELECT rssi FROM  bas.ble_tag_event WHERE tag_id=x.tag_id AND base_id='b8:27:eb:80:17:02' and event_time=x.event_time) as base_1702_rssi
         FROM bas.ble_tag_event x  WHERE  x.event_time < now() - interval '21 second' AND  x.event_time > (now() - interval '2100 day')), 
         
         temp2 as ( SELECT tag_id, to_timestamp(avg(extract(epoch from event_time)) OVER w) AS event_time,
                 round(avg(base_ff8d_rssi) OVER w,2) AS base_ff8d_rssi,
                 round(avg(base_fe95_rssi) OVER w,2) AS base_fe95_rssi ,
                 round(avg(base_ebc0_rssi) OVER w,2) AS base_ebc0_rssi ,
                 round(avg(base_b70e_rssi) OVER w,2) AS base_b70e_rssi ,
                 round(avg(base_1702_rssi) OVER w,2) AS base_1702_rssi 
                     FROM temp
                 /* use a 20 second window to average rssi over */
                 WINDOW w AS (PARTITION BY tag_id, to_timestamp(floor((extract('epoch' from event_time) / 20 )) * 20)  
                         ORDER BY event_time   
                         ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING))
         /* do insert into filtered table */
         insert into bas.ble_tag_event_filtered (tag_id,event_time,base_ff8d_rssi,base_fe95_rssi,base_ebc0_rssi,base_b70e_rssi,base_1702_rssi)
         SELECT DISTINCT ON (tag_id,event_time) tag_id,event_time,base_ff8d_rssi,base_fe95_rssi,base_ebc0_rssi,base_b70e_rssi,base_1702_rssi from temp2 order by tag_id,event_time  ON CONFLICT DO NOTHING 
               
SQL2;

            $res = $GLOBALS['pdo']->exec($sql2);


            /* After we have the data in 20 second time slices from previous command, when can delete the raw data to save space. 
             * This command must be smart, because if the previous step failed for some reason, then we risk data loss. So do it in a transaction!
             */

            $sql3 = <<<SQL3
 	DELETE FROM bas.ble_tag_event x WHERE x.event_time < current_date - interval '2100 day' 

SQL3;
            $res = $GLOBALS['pdo']->exec($sql3);
            // determine if the commit or rollback

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
        }
    }

    function mainB() {
        
        
        /*
         //check when the oldest data in the ble_tag_event is. If it is older then 21 days, lets throw an exception. 
         //the reason for this is that if we try and do more then 21 days at a time we run out of disk space on the server.        
                    $sql44 = <<<SQL44
 	SELECT event_time FROM bas.ble_tag_event_filtered_variance_10min ORDER by event_time ASC limit 1
SQL44;
            $res = $GLOBALS['pdo']->query($sql44);
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if (strtotime($row['event_time']) <= strtotime(date('21 days ago'))) {
                throw new Exception("Error: Cannot run because this has not been run in so long that there is more then 21 days of data. Run manually in smaller chunks until you have less then 21 days.");
            }
            
       */
        
        
        
        /*
         * calculate 20 min variance on all data in ble_tag_event_filtered table  
         */
        $sql4 = <<<SQL4
 	 	with temp as 
                    (SELECT distinct on (tag_id,event_time)  ble_tag_event_filtered.tag_id, 
                        to_timestamp(avg(extract(epoch from ble_tag_event_filtered.event_time)) OVER w)::timestamp with time zone at time zone 'UTC' AS event_time,
                            variance(base_ff8d_rssi) over w as var_ff8d_rssi,
                            variance(base_fe95_rssi) over w  as var_fe95_rssi,
                            variance(base_ebc0_rssi) over w  as var_ebc0_rssi,
                            variance(base_b70e_rssi) over w  as var_b70e_rssi, 
                            variance(base_1702_rssi) over w  as var_1702_rssi 
                  FROM bas.ble_tag_event_filtered 
                  LEFT JOIN bas.ble_bovine_tag ON ble_bovine_tag.tag_id=ble_tag_event_filtered.tag_id 
                  LEFT JOIN bovinemanagement.bovine ON bovine.id=ble_bovine_tag.bovine_id WHERE ble_tag_event_filtered.event_time >= ( now() - interval '20 days')
                      /* the minus 20 minutes is used, so that the last 20 minute period is ignored. Not sure this will work, not sure what is atomic */
                          /* use a 20 min window to do variance over, this was chosen after a little R&D, maybe not the best */ 
                              WINDOW w AS (PARTITION BY ble_tag_event_filtered.tag_id, to_timestamp(floor((extract('epoch' from ble_tag_event_filtered.event_time) / 1200 )) * 1200) 
                                  ORDER BY ble_tag_event_filtered.event_time       ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)), 
                                      
   temp2 as (
       /* there should be a normalizaion by the group variance or something here in teh future, to make it more robust */
           
   SELECT tag_id,event_time,
       var_ff8d_rssi+var_fe95_rssi+var_ebc0_rssi+var_b70e_rssi as sum_var,
           var_ff8d_rssi,var_fe95_rssi,var_ebc0_rssi,var_b70e_rssi,var_1702_rssi 
               FROM temp 
               )
                   /* insert into table */
   INSERT into bas.ble_tag_event_filtered_variance_20min (tag_id,event_time,var_ff8d_rssi,var_fe95_rssi,var_ebc0_rssi,var_b70e_rssi,sum_var)
       SELECT DISTINCT ON (tag_id,event_time) tag_id,event_time,var_ff8d_rssi,var_fe95_rssi,var_ebc0_rssi,var_b70e_rssi,sum_var 
               FROM temp2 
               ORDER by tag_id,event_time ON CONFLICT DO NOTHING 	
SQL4;
        $res = $GLOBALS['pdo']->exec($sql4);

        /*
         * calculate 7 day variance on all data in ble_tag_event_filtered table  
         *  SHOULD RECORD MEAN TIMESTAMP FOR 7 DAY, NOT MAX. FIXME.
         */
        $sql5 = <<<SQL5
 	 	with temp as 
                    (SELECT distinct on (tag_id,event_time)  ble_tag_event_filtered.tag_id, 
                        to_timestamp(avg(extract(epoch from ble_tag_event_filtered.event_time)) OVER w)::timestamp with time zone at time zone 'UTC' AS event_time,
                            variance(base_ff8d_rssi) over w as var_ff8d_rssi,
                            variance(base_fe95_rssi) over w  as var_fe95_rssi,
                            variance(base_ebc0_rssi) over w  as var_ebc0_rssi,
                            variance(base_b70e_rssi) over w  as var_b70e_rssi, 
                            variance(base_1702_rssi) over w  as var_1702_rssi 
                  FROM bas.ble_tag_event_filtered 
                  LEFT JOIN bas.ble_bovine_tag ON ble_bovine_tag.tag_id=ble_tag_event_filtered.tag_id 
                  LEFT JOIN bovinemanagement.bovine ON bovine.id=ble_bovine_tag.bovine_id WHERE ble_tag_event_filtered.event_time >= ( now() - interval '20 days')
                      /* 7 day window */
                              WINDOW w AS (PARTITION BY ble_tag_event_filtered.tag_id, to_timestamp(floor((extract('epoch' from ble_tag_event_filtered.event_time) / 604800 )) * 604800) 
                                  ORDER BY ble_tag_event_filtered.event_time       ROWS BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING)), 
                                      
   temp2 as (
       /* now insert variance. */
           
   SELECT tag_id,event_time,
           var_ff8d_rssi,var_fe95_rssi,var_ebc0_rssi,var_b70e_rssi,var_1702_rssi 
               FROM temp 
               )
                   /* insert into table */
   INSERT into bas.ble_tag_event_filtered_variance_7day (tag_id,event_time,var_ff8d_rssi,var_fe95_rssi,var_ebc0_rssi,var_b70e_rssi,var_1702_rssi)
       SELECT DISTINCT ON (tag_id,event_time) tag_id,event_time,var_ff8d_rssi,var_fe95_rssi,var_ebc0_rssi,var_b70e_rssi ,var_1702_rssi
               FROM temp2 
               ORDER by tag_id,event_time ON CONFLICT DO NOTHING 	
SQL5;
        $res = $GLOBALS['pdo']->exec($sql5);



        /*
         * combines 20 min and 7 day table into one table, to then plot etc. Runs pretty fast, 6 seconds. for 200k rows.
         * 
         */
        $sql6 = <<<SQL6
   with twenty as (
        SELECT
         bal.*,
          (
              select event_time
              from bas.ble_tag_event_filtered_variance_7day trn2
              where trn2.tag_id = bal.tag_id
              order by abs( extract( epoch from  bal.event_time) - extract (epoch from trn2.event_time)) limit 1
          ) as event_time_7day
        FROM
          bas.ble_tag_event_filtered_variance_20min bal WHERE tag_id=tag_id ORDER BY event_time DESC

        ), temp2 as (
         SELECT  twenty.tag_id,twenty.event_time,
             (SELECT sum(v) FROM unnest(ARRAY[twenty.var_ff8d_rssi/NULLIF(seven.var_ff8d_rssi,0) , twenty.var_fe95_rssi/NULLIF(seven.var_fe95_rssi,0) , twenty.var_ebc0_rssi/NULLIF(seven.var_ebc0_rssi,0) , twenty.var_b70e_rssi/NULLIF(seven.var_b70e_rssi,0),twenty.var_1702_rssi/NULLIF(seven.var_1702_rssi,0)]) g(v)) as sum_var,
                            (SELECT avg(v) FROM unnest(ARRAY[twenty.var_ff8d_rssi , twenty.var_fe95_rssi , twenty.var_ebc0_rssi , twenty.var_b70e_rssi,twenty.var_1702_rssi]) g(v)) as sum_var2

        FROM twenty JOIN bas.ble_tag_event_filtered_variance_7day   seven ON seven.event_time=twenty.event_time_7day AND seven.tag_id=twenty.tag_id

        )
    /* insert into table */
   INSERT into bas.ble_tag_event_filtered_variance_final (tag_id,event_time,sum_var)
       SELECT DISTINCT ON (tag_id,event_time) tag_id,event_time,sum_var
               FROM temp2 
               ORDER by tag_id,event_time ON CONFLICT DO NOTHING 
SQL6;




        $res = $GLOBALS['pdo']->exec($sql6);
    }

    function mainDeleteOldData()
    {
        
             /*
         * delete old data
         * 
         */
        $sql7 = <<<SQL7
                    
DELETE  from bas.ble_tag_event WHERE event_time < (now() - interval '3 days')
SQL7;
        $res = $GLOBALS['pdo']->exec($sql7); 
        
        
         $sql7 = <<<SQL8
                    
DELETE  from bas.ble_tag_event_filtered WHERE event_time < (now() - interval '120 days')
SQL8;
        $res = $GLOBALS['pdo']->exec($sql7); 
    
        
    }
    
    
}

//end class
//rund from cli
$xx = new EstrusDetectorFilter();
$xx->main();
?>