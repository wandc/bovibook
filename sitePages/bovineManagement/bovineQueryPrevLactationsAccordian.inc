<?php

class bovineQueryPrevLactationsAccordian {

    public function showAccordian($bovineFullRegNumber) {
        $out=array();
        $accordionArray = array();
        $query = "SELECT max(lact_nu) as max_lact_nu,min(lact_nu) as min_lact_nu FROM batch.valacta_data WHERE reg='$bovineFullRegNumber'";
        $res = $GLOBALS ['pdo']->query($query);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        // print out each lactation, full info in accordian tab
        if (is_numeric($row['max_lact_nu']) AND is_numeric($row['min_lact_nu'])) {
            for ($i = $row['max_lact_nu']; $i >= $row ['min_lact_nu']; $i--) {
                $accordionArray[$i]['name'] = "Lactation #$i"; //could be off by one.
            }
            $accordion = new AccordionImplementationPreviousLactations($bovineFullRegNumber);
            $accordion->setCSS('accordionNarrowBovineQuery');

            //previousLactationInfo         
            $out[] = $accordion->render($accordionArray);
            //used because google visualization doesn't know what size to render chart in hidden div. so force resize when accordian tab opened.     
            $js = <<<JSCRIPT
<script>       
                    $( document ).ready(function() {
 $("#accordion_{$accordion->uuid}").accordion({ activate: function(event, ui) {
  var ev = document.createEvent('Event');
    ev.initEvent('resize', true, true);
    window.dispatchEvent(ev);
  }
}); 
 });
</script>     
JSCRIPT;
            $out[] = $js;
        }


        return (implode($out));
    }

    public function error(){}
}

/**
 * used to display tabs for previous lactations. 
 */
class AccordionImplementationPreviousLactations extends AccordionSub {

    public static $bovineFullRegNumber;

    public function __construct($bovineFullRegNumber = null) {
        if ($bovineFullRegNumber != null) {
            self::$bovineFullRegNumber = $bovineFullRegNumber;
        } else {
            throw new Exception("Must Have bovine reg number to run query.");
        }
    }

    private function sqlData($lact_num) {
        $bovineFullRegNumber = self::$bovineFullRegNumber;

        $str = '';



        $sql = <<<SQL
SELECT days_in_mi as "DIM", bca_milk ||'-'|| bca_fat ||'-'||  bca_prot as "BCA", total_milk as "Milk (kg)",round(fat_per,2) as "Fat %",round(prot_per,2) as "Prot %",round(ssc/1000,0)||'k' as "SSC",round(mun,1) as "MUN" , 
    round(total_milk*fat_per*.01,2) as "Fat (kg/day)",round(total_milk*prot_per*.01,2) as "Prot (kg/day)" 
    FROM batch.valacta_data 
        WHERE reg='$bovineFullRegNumber' AND lact_nu=$lact_num 
            ORDER BY test_date DESC
SQL;




        // $out[]=( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql));
        $str = $str . ( (new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', $sql));

        $valactaMilkYieldArr = null;
        $valactaFatArr = null;
        $valactaProtArr = null;
        $valactaSSCArr = null;

        $query = "SELECT lact_nu,test_date,total_milk,fresh,days_in_mi,fat_per,prot_per,ssc,bca_milk,bca_fat,bca_prot,ssc/1000 as ssc,days_in_mi,fat_per,prot_per,mun/10 as mun FROM batch.valacta_data WHERE reg='$bovineFullRegNumber' AND lact_nu=$lact_num ORDER BY test_date DESC";
        $res = $GLOBALS ['pdo']->query($query);

        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {

            //used to plot graphs later...
            if ($row ['total_milk'] != null) {
                $valactaMilkYieldArr [$row ['days_in_mi']] = $row ['total_milk'];
                $valactaFatArr [$row ['days_in_mi']] = $row ['fat_per'];
                $valactaProtArr [$row ['days_in_mi']] = $row ['prot_per'];
                $valactaSSCArr [$row ['days_in_mi']] = $row ['ssc'] / 1000;
            }
        }
        $dailyMilkYieldArr = null; //do not sow daily data for previos lactations. No need. 
        $str = $str . (BovineQuery::valactaMilkPlot($dailyMilkYieldArr, $valactaMilkYieldArr));
        $str = $str . (BovineQuery::valactaSCCPlot($valactaSSCArr));
        $str = $str . (BovineQuery::valactaFatPlot($valactaFatArr, $valactaProtArr));


        return $str;
    }

    function printBCA($params) {
        extract($params);
        return "{$record['bca_milk']}-{$record['bca_fat']}-{$record['bca_prot']}";
    }

    //NOTE: only supports 14 lactations.
    function tab1() {
        return(self::sqlData(1));
    }

    function tab2() {
        return(self::sqlData(2));
    }

    function tab3() {
        return(self::sqlData(3));
    }

    function tab4() {
        return(self::sqlData(4));
    }

    function tab5() {
        return(self::sqlData(5));
    }

    function tab6() {
        return(self::sqlData(6));
    }

    function tab7() {
        return(self::sqlData(7));
    }

    function tab8() {
        return(self::sqlData(8));
    }

    function tab9() {
        return(self::sqlData(9));
    }

    function tab10() {
        return(self::sqlData(10));
    }

    function tab11() {
        return(self::sqlData(11));
    }

    function tab12() {
        return(self::sqlData(12));
    }

    function tab13() {
        return(self::sqlData(13));
    }

    function tab14() {
        return(self::sqlData(14));
    }

}

//end class