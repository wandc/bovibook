<?php 

class HistoricalHours extends BasePage{



    public function defaultDisplay() {   
    $this->showChartsForSpecificYear(date("Y",strtotime("-1 year")));
    $this->showChartsForSpecificYear(date("Y",strtotime("-2 year")));
    }
    
    function showChartsForSpecificYear($year) {
           print("<h3>Year:$year</h3>");
        $this->hoursWorkedInAYear($year);
        $this->calendarChartAllYearly($year);
        $this->calendarChartAllDaily($year);
        $this->calendarChartAllWeekly($year, 'Darcy');
        $this->calendarChartCropperDaily($year);
        $this->calendarChartMilkerDaily($year);
        $this->calendarChartCropperWeekly($year);
         $this->calendarChartCropperYearly($year);
        $this->calendarChartMilkerWeekly($year);
        $this->calendarChartMilkerYearly($year);
        print("<hr>");
    }

    function hoursWorkedInAYear($year) {
        $sql="
         with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct milker,date_trunc('year',date)::date as date,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('year',date)) )) as hours 

from temp 
order by date 
        ";
        $res = $GLOBALS['pdo']->query($sql);
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
         // print_r2($row);
            }
        
    }
    

  
        public function calendarChartAllYearly($year) {
        
        $sql="
               with temp as (
SELECT event_time::date as date,time_worked,userid,date_trunc('year',event_time)
FROM hr.time_sheet WHERE EXTRACT('year' FROM event_time)='$year'
)
SELECT distinct date,((sum(time_worked) OVER (PARTITION BY date_trunc('year',date)) )) as data from temp order by date
                ";
 $this->calendarChart($sql,'Total by All That Year');
    }
    
    public function calendarChartAllDaily($year) {
        
        $sql="
               with temp as (
SELECT event_time::date as date,time_worked,userid,date_trunc('year',event_time)
FROM hr.time_sheet WHERE EXTRACT('year' FROM event_time)='$year'
)
SELECT distinct date,((sum(time_worked) OVER (PARTITION BY date_trunc('day',date)) )) as data from temp order by date
                ";
 $this->calendarChart($sql,'All Daily Hours');
    }
    
    function calendarChartAllWeekly($year,$userid='tommy') {
     $sql=" 
         with temp as (
SELECT event_time::date as date,time_worked,userid
FROM hr.time_sheet WHERE EXTRACT('year' FROM event_time)='$year'
), temp2 as (
SELECT distinct date_trunc('week',date) as date, userid,((sum(time_worked) OVER (PARTITION BY userid,date_trunc('week',date)) )) as data
from temp order by date_trunc('week',date),userid
)
SELECT * from temp2 where userid='$userid'
        
   ";
 
      $this->calendarChart($sql,'All Weekly Hours for user: '. strtoupper($userid));
    
    }
    
    
    function calendarChartCropperDaily($year) {
     $sql=" 
        with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct date,milker,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('day',date)) )) as data 
from temp 
WHERE milker is null
order by date 
        
   ";
 $this->calendarChart($sql,'Cropper Daily Hours');
    
    }
    
    function calendarChartMilkerDaily($year) {
     $sql=" 
        with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct date,milker,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('day',date)) )) as data 
from temp 
WHERE milker is true
order by date 
        
   ";
 $this->calendarChart($sql,'Milker Daily Hours');
    }
    
    function calendarChartCropperWeekly($year) {
        
    $sql=" 
        with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct date_trunc('week',date)::date as date,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('week',date)) )) as data 
from temp 
WHERE milker is null
order by date
";
    $this->calendarChart($sql,'Cropping Weekly');
    }
    
    function calendarChartCropperYearly($year) {
        
    $sql=" 
        with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct date_trunc('year',date)::date as date,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('year',date)) )) as data 
from temp 
WHERE milker is null
order by date
";
    $this->calendarChart($sql,'Cropping Yearly');
    }
    
    
    function calendarChartMilkerWeekly($year) {
        
    $sql=" 
        with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct date_trunc('week',date)::date as date,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('week',date)) )) as data 
from temp 
WHERE milker is true
order by date
";
    $this->calendarChart($sql,'Milker Weekly');
    }
    
     function calendarChartMilkerYearly($year) {
        
    $sql=" 
        with temp as (
SELECT event_time::date as date,time_worked,time_sheet.userid,milking_number::boolean as milker
FROM hr.time_sheet 
LEFT JOIN batch.employee_shift ON (employee_shift.userid,employee_shift.date)=(time_sheet.userid,time_sheet.event_time::date)
WHERE EXTRACT('year' FROM event_time)='$year'
)  SELECT distinct date_trunc('year',date)::date as date,((sum(time_worked) OVER (PARTITION BY milker,date_trunc('year',date)) )) as data 
from temp 
WHERE milker is true
order by date
";
    $this->calendarChart($sql,'Milker Yearly');
    }
    
    /* sql must output date and data field */
    /* generic function */
    function calendarChart($sql,$title=null) {
      
        $name=$GLOBALS['MiscObj']->uuid_create();
         ?>

    <script type="text/javascript">

      google.charts.setOnLoadCallback(drawChart);
       $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart);
            });

   function drawChart() {
       var dataTable = new google.visualization.DataTable();
       dataTable.addColumn({ type: 'date', id: 'Date' });
       dataTable.addColumn({ type: 'number', id: 'Hours Worked' });
       
        dataTable.addRows([
            <?php 
            $res = $GLOBALS['pdo']->query($sql);
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            print("[". GoogleChartsHelper::javascriptDateTime($row['date']). " , ". $row['data']."],\n");        
            }
            ?>
    ]);

       var chart = new google.visualization.Calendar(document.getElementById('calendar_basic_<?php echo($name);?>'));

       var options = {
         title: "<?php echo($title);?>",
         height: 200,
       };

       chart.draw(dataTable, options);
   }
    </script>
 
    <div id="calendar_basic_<?php echo($name);?>" ></div>
 
            <?php 
    
    }
        
        

    
    
    
}//end class