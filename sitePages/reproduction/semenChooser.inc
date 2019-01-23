<?php 

class SemenChooser extends BasePage {

  
    public function defaultDisplay() {

        $this->run();
    }

    function customJavascript() {
        ?>
        <script type="text/javascript" language="javascript">
            $(document).ready(function () {

                $('#selectID :selected').val();
                $("#selectID").change(function () {
                    $('#dynamic_div_1').load($('#selectID').val());
                });

                $('#selectID2 :selected').val();
                $("#selectID2").change(function () {
                    $('#dynamic_div_2').load($('#selectID2').val());
                });

            });
        </script>
        <?php 

    }

    function run() {
$str='';
$str1='';
$str2='';

        $str=$str.('<style> #cdnframe { position:absolute;}</style>'); //NOTE: css hack to put next to each other. replace when we have time (unfortunately never).
        

        $str1=$str1.('
<select id="selectID">
   <option selected="selected">Please select a cow...</option>
');
        //go through array for each cow and sire.
        foreach ($this->getFemale() as $key => $value) {
            $str1=$str1.("<option value=\"/functions/cDNPageParser.php?reg=$key&iframe=1\">$value</option>");
        }
        $str1=$str1.('
</select>
<div id="dynamic_div_1"></div>
');




        $str2=$str2.('
<select id="selectID2">
   <option selected="selected">Please select a sire...</option>
');
        //go through array for each cow and sire.
        foreach ($this->getMale() as $key => $value) {
            $str2=$str2.("<option value=\"/functions/cDNPageParser.php?reg=" . trim($key) . "&iframe=1\">$value</option>");
        }
        $str2=$str2.('
</select>
<div id="dynamic_div_2"></div>
');

         print("<div class='row'>" . "\n\r");
        print("<div class='col-sm-6 col-sm-push-6'>" . "\n\r");
        print($str1);
        print("</div>" . "\n\r");
        print("<div class='col-sm-6 col-sm-pull-6'>" . "\n\r");
         print($str2);
        print("</div>" . "\n\r");
        print("</div>" . "\n\r");


        print($str);
    }

    function getFemale() {
        //get an array of all current female bovines reg numbers.
        $sql = "SELECT full_reg_number,full_name,local_number FROM bovinemanagement.bovinecurr";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $damRegArray[$row['full_reg_number']] = $row['local_number'] . " - " . $GLOBALS['MiscObj']->femaleShortName($row['full_name']);;
        }
        return $damRegArray;
    }
    
    
    function getMale(){
        
        
        /////////////////////////////////////////////////////////////////////////////////
        //get an array of all sires we have semen for in the tank. NOT THE SAME AS THE SEMEN INVENTORY CODE.
        $sql = "SELECT DISTINCT full_reg_number,short_name,full_name,invoice_cost
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE
GROUP BY full_reg_number,short_name,full_name,invoice_cost
ORDER BY short_name";

        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //only show if we have a values for, some day we can take this out when cost is required.
            if ($row['invoice_cost'] != null) {
                $invoice_cost = ' $' . $row['invoice_cost'];
            } else {
                $invoice_cost = null;
            }
            $sireRegArray[$row['full_reg_number']] = $row['short_name'] . ' - ' . $row['full_name'] . $invoice_cost;
        }
        return $sireRegArray;
    }
    
}
//end class
?>