<?php 
/**
 *  gets animal information from holstein canada on dam,sire,classification and plots a family tree.
 * 
 */
include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php');
include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/misc.inc');
include_once($_SERVER['DOCUMENT_ROOT'] . '/functions/holsteinCanadaHelper.inc');

class FamilyTree extends BasePage {

    public function defaultDisplay() {
        //draw family tree
        print("<h2>Little River Family Tree circa " . date('Y', strtotime("now")) . "</h2>");
        self::drawFamilyTree();
    }

    /**
     * draw the female side of a family tree based on a starting reg number
     *  
     */
    public function drawFemaleFamilyTree($full_reg_number) {

        //postgres recursive funcion to search a tree.
        $sql = "
        WITH RECURSIVE search_graph AS (
 SELECT full_reg_number, dam_full_reg_number, \"full_name\", \"full_name\"::text as path, 0 AS depth, birth_date,score,geno_test
   FROM batch.aggregate_view_all d
   WHERE d.full_reg_number='$full_reg_number'
 UNION ALL
 SELECT r.full_reg_number, r.dam_full_reg_number, r.\"full_name\", sg.path||'/'||r.full_reg_number as path,
        sg.depth + 1 AS depth, r.birth_date,r.score,r.geno_test
   FROM batch.aggregate_view_all r, search_graph sg
   WHERE sg.dam_full_reg_number = r.full_reg_number AND depth < 6 AND r.full_name is not null
)
SELECT * FROM search_graph ORDER BY path";
        ?> 
        <div id='chart_div_<?php  echo($full_reg_number); ?>'></div>
        <script type='text/javascript'>
        
            setTimeout(drawChart_<?php  echo($full_reg_number); ?>, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart_<?php  echo($full_reg_number); ?>, true);
             $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart_<?php  echo($full_reg_number); ?>);
            });
            function drawChart_<?php  echo($full_reg_number); ?>() {
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'Name');
                data.addColumn('string', 'Manager');
                data.addColumn('string', 'ToolTip');
              
        <?php 
        $res = $GLOBALS['pdo']->query($sql);
       $str='';
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
             $geno_test =  (!empty($row['geno_test'])) ? ' &nbsp;&nbsp; &#x24bc; ' : ''  ;
            if (!empty(trim($row['full_name']))) {
            $row['full_name'] = str_replace('LITTLE RIVER', 'L.R.', $row['full_name']); //shorten little river prefix.          
           $str=$str.'data.addRow('.(" [{v:'{$row['full_reg_number']}', f:'{$row['full_reg_number']}". $geno_test ."<div style=\"color:red; font-style:italic\">{$row['full_name']}</div><div style=\"color:purple; font-style:italic\">{$row['score']}</div><div style=\"color:green; font-style:italic\">{$row['birth_date']}'}, '{$row['dam_full_reg_number']}', 'VP']);"."\n\r");
        }
        print($str);
       
           }
        ?>
                 
              
                var chart_<?php echo($full_reg_number);?> = new google.visualization.OrgChart(document.getElementById('chart_div_<?php  echo($full_reg_number); ?>'));
                chart_<?php echo($full_reg_number);?>.draw(data, {allowHtml: true});
            }
        </script>

        <?php 
    }

    /**
     * used to display the family tree in DB. via google visulaizations
     */
    function drawFamilyTree($uuid = 'familiy_tree1') {
        ?> 
        <div id='chart_div_<?php  echo($uuid); ?>'></div>
 
        <script type='text/javascript'>
            

            setTimeout(drawChart_<?php  echo($uuid); ?>, 200); //firefox workaround. 
            google.charts.setOnLoadCallback(drawChart_<?php  echo($uuid); ?>, true);
              $(window).resize(function(){
            google.charts.setOnLoadCallback(drawChart_<?php  echo($uuid); ?>);
            });
            function drawChart_<?php  echo($uuid); ?>() {
                var data = new google.visualization.DataTable();
                data.addColumn('string', 'Name');
                data.addColumn('string', 'Manager');
                data.addColumn('string', 'ToolTip');
                data.addRows([
        <?php 
        $sql = "SELECT trim(full_reg_number) as full_reg_number,full_name,score,geno_test,birth_date::date,dam_full_reg_number from batch.aggregate_view_all WHERE sex='F'";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
               $geno_test =  (!empty($row['geno_test'])) ? ' &nbsp;&nbsp; &#x24bc; ' : ''  ;
            $row['full_name'] = str_replace('LITTLE RIVER', 'L.R.', $row['full_name']); //shorten little river prefix.      
            print(" [{v:'{$row['full_reg_number']}', f:'{$row['full_reg_number']}". $geno_test ."<div style=\"color:red; font-style:italic\">".addslashes($row['full_name'])."</div><div style=\"color:purple; font-style:italic\">{$row['score']}</div><div style=\"color:green; font-style:italic\">{$row['birth_date']}'}, '{$row['dam_full_reg_number']}', 'VP'],"."\n\r");
        }
        ?>
                    ['', '', '']
                ]);
                var chart_<?php  echo($uuid); ?> = new google.visualization.OrgChart(document.getElementById('chart_div_<?php  echo($uuid); ?>'));
                chart_<?php  echo($uuid); ?>.draw(data, {allowHtml: true});
            }
        </script>

        <?php 
    }

    //passes along error info.
    public function error() {
        $error = array();

        //if data is out of date by more than 8 days throw error  
        $res = $GLOBALS['pdo']->query("SELECT count(update_time) as count_update, min(update_time) as min_update FROM batch.aggregate_view_all WHERE update_time < (current_timestamp - interval '8 days' )");
        $row = $res->fetch(PDO::FETCH_ASSOC);
        if ($row['count_update'] != 0) {
            $error[] = new notifyObj(get_class($this), date('Y-m-d', strtotime("now")), 2, $row['count_update'] . ' classifciation records not updated in last 8 days.');
        }


        return $error;
    }

}

//end class
?>