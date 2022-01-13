<?php 

/**
   Shows a parlor milking report.
 */
class ParlorMilking extends BasePage {

    private $milking_date;

    public function __construct() {
        parent::__construct();
        if (isset($_REQUEST['milking_date'])) {
            $this->milking_date = $_REQUEST['milking_date'];
        }
//  $this->loadVars();
    }


    public function defaultDisplay() {

        $this->dateChooserHTML5Form();

        if ($this->milking_date != null) {
            $this->MilkingTimelineChart($this->milking_date, 1);
            $this->MilkingTimelineChart($this->milking_date, 2);
        }
    }

    
         
    /*
     * look up start and stop milking times for a date and milking number.
     * show on home page, one off function.
     */
   public  function milkingTimeStats() {
  
       
/* previous 2 days milkings *, same code repeated for each day&milking #*/       
$sql = <<<SQL
(
With temp as (
SELECT local_number,milkyield,duration,manualkey,milktime,milktime+duration as milkendtime,alpro.alpro_mpcnumber_to_human_readable(mpcnumber) as mpcnumber ,idtimemm ,bovinemanagement.location_id_at_timestamp(bovine_id, milktime) as location_id
FROM alpro.cow 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cow.bovine_id
WHERE date=(current_date-interval '1 day') and milking_number=1 order by mpcnumber
)
SELECT
(SELECT (current_date-interval '1 day')) as date, (SELECT alpro.milkingnumber_to_human_readable(1) limit 1) as milking_number,
(SELECT min(milktime) from temp) as first_attach,(SELECT max(milkendtime) from temp WHERE location_id  = ANY(ARRAY[40,41,42])) as last_detach_saleable ,(SELECT min(milktime) from temp where location_id=39) as first_attach_sick,(SELECT max(milkendtime) from temp where location_id=39) as last_detach_sick    
)
UNION
(
With temp as (
SELECT local_number,milkyield,duration,manualkey,milktime,milktime+duration as milkendtime,alpro.alpro_mpcnumber_to_human_readable(mpcnumber) as mpcnumber ,idtimemm ,bovinemanagement.location_id_at_timestamp(bovine_id, milktime) as location_id
FROM alpro.cow 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cow.bovine_id
WHERE date=(current_date-interval '0 day') and milking_number=1 order by mpcnumber
)
SELECT
(SELECT (current_date-interval '0 day')) as date, (SELECT alpro.milkingnumber_to_human_readable(1) limit 1) as milking_number,
(SELECT min(milktime) from temp) as first_attach,(SELECT max(milkendtime) from temp WHERE location_id  = ANY(ARRAY[40,41,42])) as last_detach_saleable ,(SELECT min(milktime) from temp where location_id=39) as first_attach_sick,(SELECT max(milkendtime) from temp where location_id=39) as last_detach_sick    
)
UNION
(
With temp as (
SELECT local_number,milkyield,duration,manualkey,milktime,milktime+duration as milkendtime,alpro.alpro_mpcnumber_to_human_readable(mpcnumber) as mpcnumber ,idtimemm ,bovinemanagement.location_id_at_timestamp(bovine_id, milktime) as location_id
FROM alpro.cow 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cow.bovine_id
WHERE date=(current_date-interval '1 day') and milking_number=2 order by mpcnumber
)
SELECT
(SELECT (current_date-interval '1 day')) as date, (SELECT alpro.milkingnumber_to_human_readable(2) limit 1) as milking_number,
(SELECT min(milktime) from temp) as first_attach,(SELECT max(milkendtime) from temp WHERE location_id  = ANY(ARRAY[40,41,42])) as last_detach_saleable ,(SELECT min(milktime) from temp where location_id=39) as first_attach_sick,(SELECT max(milkendtime) from temp where location_id=39) as last_detach_sick    
)
UNION
(
With temp as (
SELECT local_number,milkyield,duration,manualkey,milktime,milktime+duration as milkendtime,alpro.alpro_mpcnumber_to_human_readable(mpcnumber) as mpcnumber ,idtimemm ,bovinemanagement.location_id_at_timestamp(bovine_id, milktime) as location_id
FROM alpro.cow 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cow.bovine_id
WHERE date=(current_date-interval '0 day') and milking_number=2 order by mpcnumber
)
SELECT
(SELECT (current_date-interval '0 day')) as date, (SELECT alpro.milkingnumber_to_human_readable(2) limit 1) as milking_number,
(SELECT min(milktime) from temp) as first_attach,(SELECT max(milkendtime) from temp WHERE location_id  = ANY(ARRAY[40,41,42])) as last_detach_saleable ,(SELECT min(milktime) from temp where location_id=39) as first_attach_sick,(SELECT max(milkendtime) from temp where location_id=39) as last_detach_sick    
)
SQL;

               $res = $GLOBALS['pdo']->query($sql);
           $result = $res->fetchAll(PDO::FETCH_ASSOC);
            
           
           foreach ($result as $key => $value) {
             $result[$key]['first_attach']= date('G:i',strtotime($result[$key]['first_attach']));
             $result[$key]['last_detach_saleable']= date('G:i',strtotime($result[$key]['last_detach_saleable']));
             $result[$key]['first_attach_sick']= date('G:i',strtotime($result[$key]['first_attach_sick']));
             $result[$key]['last_detach_sick']= date('G:i',strtotime( $result[$key]['last_detach_sick']));
        
           }
           
            
           return ($result);
    }
    
    
    
    function dateChooserHTML5Form() {
        //todays date or currently selected date as default
        if (isset($this->milking_date)) {
            $date = $this->milking_date;
        } else {
            $date = date('o-m-d'); //today
        }
$maxDate = date('Y-m-d'); //today
        $out = <<<HTML
        <form action='https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}' method='post'>
Enter a date:<br>
<input type="date" name="milking_date" min="2010-03-01" max="$maxDate" value="$date"><br><br>
<input type="submit"> 
</form>
HTML;
        print($out);
    }

    function MilkingTimelineChart($date, $milking_number) {

        print("<h3>Parlor Report for " . date('M d, Y', strtotime($date)) . " #$milking_number</h3>");


        $sql = "SELECT local_number,milkyield,duration,manualkey,milktime,milktime+duration as milkendtime,alpro.alpro_mpcnumber_to_human_readable(mpcnumber) as mpcnumber ,idtimemm ,bovinemanagement.location_name_at_timestamp(bovine_id, milktime) as location_name	 
FROM alpro.cow 
LEFT JOIN bovinemanagement.bovine ON bovine.id=cow.bovine_id
WHERE date='$date' and milking_number=$milking_number order by mpcnumber";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() != 0) {

            $uuid = $milking_number;
            $out = '';
            $out = $out . <<<JS
  

         
<script type="text/javascript">
  google.charts.load('current', {'packages':['timeline']});
  setTimeout(drawChart_$uuid, 200); //firefox workaround. 
google.charts.setOnLoadCallback(drawChart_$uuid);
                     $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart_$uuid);
            });
function drawChart_$uuid() {

  var container_$uuid = document.getElementById('example5.2_$uuid');
  var chart_$uuid = new google.visualization.Timeline(container_$uuid);
  var dataTable_$uuid = new google.visualization.DataTable();

  dataTable_$uuid.addColumn({ type: 'string', id: 'Room' });
  dataTable_$uuid.addColumn({ type: 'string', id: 'Name' });
  dataTable_$uuid.addColumn({ type: 'date', id: 'Start' });
  dataTable_$uuid.addColumn({ type: 'date', id: 'End' });
  dataTable_$uuid.addRows([
    
JS;


            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                if ($row['manualkey']=='t') {$manulaMilking='- Manual';}
                else { $manulaMilking='';}
                $locationTruncated= substr($row['location_name'],0,6);
                $out = $out . ("['{$row['mpcnumber']}', '{$row['local_number']} - {$locationTruncated} $manulaMilking', " . GoogleChartsHelper::javascriptDateTime($row['milktime']) . "	, " . GoogleChartsHelper::javascriptDateTime($row['milkendtime']) . "],");
//print("['{$row['mpcnumber']}', '{$row['local_number']} ', ".GoogleChartsHelper::javascriptDateTime($row['milktime'])."	, ".GoogleChartsHelper::javascriptDateTime($row['milkendtime'])."],");                
            }


            $out = $out . <<<JS
        
    ]);

  var options = {

    timeline: { rowLabelStyle: {fontName: 'Helvetica', fontSize: 12, color: '#8d8' },
                barLabelStyle: { fontName: 'Garamond', fontSize: 10 } }
  };

  chart_$uuid.draw(dataTable_$uuid, options);
}
</script>

<div id="example5.2_$uuid" style="height: 1200px;"></div>
                        
JS;



            print($out);
        } else {
            print("<h3>No Data.</h3>");
        }
    }

}

//end class
?>
