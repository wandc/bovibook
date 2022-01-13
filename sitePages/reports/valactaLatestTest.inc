<?php 



class ValactaLatestTest extends BasePage {

    private $tabNumber = 0;

    public function defaultDisplay() {
         $this->displayValactaCalcs();
        $this->displayValactaCharts();      
        $this->displayValactaReport();
        $this->showScatterChartOfCurrentMUN();
    }

    function showScatterChartOfCurrentMUN() {

        $query2 = "SELECT mun,days_in_mi FROM batch.valacta_data_latest_test_view WHERE mun is not null";


        ///////////////
        $res2 = $GLOBALS['pdo']->query($query2);

        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $dimArr[] = ($row2['days_in_mi']);
            $munArr[] = round($row2['mun'], 2) ;
        }

        print("<h3>MUN Herd</h3>");

        $headerStrArr = array();
        $headerStrArr['DIM'] = 'number'; //BACKWARDS
        $headerStrArr['MUN'] = 'number';
        $x = new GoogleVisualizationScatter(null, $headerStrArr, $dimArr, $munArr);
        print($x->toString());
    }

//end function

    
    function valactaReportLatest() {
       //shows latest test data
        $sql = "SELECT total_milk,chain ,bovinecurr.local_number, bovinecurr.id as bovine_id,bovinecurr.full_name,test_date as event_time,location_name,bca_milk,bca_fat,bca_prot,prodoll,
            extract(day from (now()-fresh)) as fresh_days, round(batch.calculate_milk_revenue_per_cow_per_date (bovinecurr.id,test_date),2) as calculate_milk_revenue_per_cow_per_date,
           round(batch.calculate_gross_solid_price(fat305,prot305)) as lact_calculate_gross_solid_price
            FROM batch.valacta_data_latest_test_view
 LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.local_number = valacta_data_latest_test_view.chain
 LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
WHERE  bovinecurr.full_name IS NOT NULL 
ORDER BY total_milk DESC, fresh_days DESC";
        
 $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
         
            $row['local_number'] =  $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
           
        }

        //output
        $output['data'] = $results;
        return ($output);
        
    }
    
    
    function displayValactaReport() {
    

      print("<h2>Milk Production Info</h2>");
        $dt = new JQueryDataTable();
              $options = '"paging": false,  "order": [[0,\'asc\']],';
        $columnsStr = <<<EX
  
            {
                data: "local_number",
                title: "Animal",
                type: "num-html"
            },
            {
                data: "location_name",
                title: "Current Location"
            },
            {
                data: "event_time",
                title: "Test Date",
                type: "date"
            },
                {
                data: "total_milk",
                title: "total_milk"
            },
              {
                data: "calculate_milk_revenue_per_cow_per_date",
                title: "Daily Revenue ($)"
            },
            {
                data: "lact_calculate_gross_solid_price",
                title: "Projected Revenue ($)"
            }, 
            {
                data: "lact_calculate_gross_solid_price",
                title: "Adjusted Projected Revenue ($)"
            },     
              {
                data: "bca_milk",
                title: "bca_milk"
            }
                ,
              {
                data: "bca_fat",
                title: "bca_fat"
            }
                ,
              {
                data: "bca_prot",
                title: "bca_prot"
            }
                ,
                 {
                data: "fresh_days",
                title: "fresh_days",
                type: "date"
            },
                {
                data: "prodoll",
                title: "prodoll",
            }
            
           
EX;
        print($dt->start('Title CHANGE ME', $this->pageid, 'valactaReportLatest', null, $columnsStr, null, false, $options));
        
    }

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

 

   
    function printFormattedNumber($params) {
        extract($params);
        $number = "<a href=\"index.php?pageid=6&bovine_id={$record['bovine_id']}\"> {$record['local_number']}</a>";
        return $number;
    }

    function printPartialName($params) {
        extract($params);
        $animalShortName = $GLOBALS['MiscObj']->femaleShortName($record['full_name']);
        return $animalShortName;
    }

    function printFormattedTime($params) {
        extract($params);
        return date("l M d H:i", strtotime($record['create_time']));
    }

    function printActionButtons($params) {
        extract($params);
        $currentUserid = $GLOBALS['auth']->getUsername();

        //find the last entry for the current user and use that as a basais to display the delete button or not.
        //We only want the delete button shown once for the latest entry.
        $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.location_event WHERE userid='$currentUserid' ORDER BY id DESC LIMIT 1");

        $row = $res->fetch(PDO::FETCH_ASSOC);
        $deleteButtonAsString = null;
        if ($record['id'] == $row['id']) {
            $deleteButtonAsString = self::deleteQuickForm($record['id']); //show delete button, with location table id as index for delete.
        }

        return $deleteButtonAsString;
    }

    function displayValactaCharts() {
        // print("<h2>Test Date: $test_date</h2>");
        print("<h3>SCC of Cows > 200K</h3>");
        $this->displaySCC();
        print("<h3>Cows with BHB >.15 needing Ketone Test</h3>");
        $this->displayBHB();
        print("<h3>Cows with fat < 2.5%</h3>");
        $this->displaylowFat();
        print("<h3>Cows with > 305 DIM</h3>");
        $this->displayHigthDIM();
        print("<h3>Cows with milk < 20 l</h3>");
        $this->displayLowMilk();
       
    
    }

    function displaySCC() {
        $sql = "SELECT local_number,total_milk,fat_per,ssc,lact_nu,days_in_mi,bovinecurr.full_name,test_date
FROM batch.valacta_data_latest_test_view
 Left JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
ORDER BY ssc DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['ssc'] >= 200000) {
                $scc["{$row['local_number']}"] = $row['ssc'];
            }
        }

  if (!empty($scc)) {
        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string'; //BACKWARDS
        $headerStrArr['SCC'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $scc);
        print($x->toString());
  }
    }
    
    function displayBHB() {
        $sql = "with temp as (
SELECT test_date,bovinecurr.id as bovine_id, local_number,reg,bhb,'Suspect'  as status 
                FROM batch.valacta_data_latest_test_view 
                LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
                WHERE bhb >=.15 AND  bhb <.20
UNION
SELECT test_date,bovinecurr.id as bovine_id, local_number,reg,bhb,'Positive'  as status 
FROM batch.valacta_data_latest_test_view 
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
WHERE bhb >=.20 
ORDER BY bhb DESC
)
SELECT local_number,reg,bhb,status,(SELECT 1 from bovinemanagement.medical_ketone where bovine_id=temp.bovine_id AND event_time > temp.test_date limit 1) as local_test FROM temp";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            
             $local_test='';
            if (($row['local_test'])==1) {
                $local_test='TRUE';
            }
            
                $bhb["{$row['local_number']}"." ".$local_test] = $row['bhb'];
            
        }

          if (!empty($bhb)) {
        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string'; //BACKWARDS
        $headerStrArr['BHB mmol'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $bhb);
        print($x->toString());
          }
    }
    

    function displaylowFat() {
        $sql = "SELECT local_number,total_milk,fat_per,ssc,lact_nu,days_in_mi,bovinecurr.full_name,test_date
FROM batch.valacta_data_latest_test_view
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
WHERE total_milk !=0
ORDER BY fat_per ASC";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['fat_per'] <= 2.5) {
                $fat["{$row['local_number']}"] = $row['fat_per'];
            }
        }

          if (!empty($fat)) {
        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string'; //BACKWARDS
        $headerStrArr['Low Fat %'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $fat);
        print($x->toString());
          }
    }

      function displayHigthDIM() {
        $sql = "SELECT local_number,total_milk,fat_per,ssc,lact_nu,days_in_mi,bovinecurr.full_name,test_date
FROM batch.valacta_data_latest_test_view
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
WHERE total_milk !=0
ORDER BY days_in_mi DESC";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['days_in_mi'] >= 305) {
                $dim["{$row['local_number']}"] = $row['days_in_mi'];
            }
        }

        if (!empty($dim)) {
        // google column chart.
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string'; //BACKWARDS
        $headerStrArr['High DIM'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $dim);
        print($x->toString());
        }
    }
    
    function displayLowMilk() {
        $sql = "SELECT local_number,total_milk,fat_per,ssc,lact_nu,days_in_mi,bovinecurr.full_name,test_date
FROM batch.valacta_data_latest_test_view
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
WHERE total_milk !=0
ORDER BY total_milk ASC";
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            if ($row['total_milk'] <= 20) {
                $milk["{$row['local_number']}"] = $row['total_milk'];
            }
        }

        // google column chart.
           if (!empty($milk)) {
        $headerStrArr = array();
        $headerStrArr['xAxis'] = 'string'; //BACKWARDS
        $headerStrArr['Milk Less than 20 litre'] = 'number';
        $opt = '';
        $x = new GoogleVisualizationColumn($opt, $headerStrArr, $milk);
        print($x->toString());
           }
    }
    
    function displayValactaCalcs() {
        
        
        $sql = "SELECT local_number,total_milk,fat_per,ssc,lact_nu,days_in_mi,bovinecurr.full_name,test_date
FROM batch.valacta_data_latest_test_view
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = valacta_data_latest_test_view.reg
WHERE total_milk !=0
ORDER BY total_milk,local_number DESC";
        $res = $GLOBALS['pdo']->query($sql);
//create array of scc 

        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
      

            if ($row['lact_nu'] == 1) {
                $primiparous[]['fatp'] = $row['fat_per'];
                $primiparous[]['milk'] = $row['total_milk'];
                $primiparous[]['dim'] = $row['days_in_mi'];
                $primiparous[]['bf'] = 0.01 * $row['fat_per'] * $row['total_milk'];
            } else {
                $multiparious[]['fatp'] = $row['fat_per'];
                $multiparious[]['milk'] = $row['total_milk'];
                $multiparious[]['dim'] = $row['days_in_mi'];
                $multiparious[]['bf'] = 0.01 * $row['fat_per'] * $row['total_milk'];
            }
            $test_date = date('M d, Y', strtotime($row['test_date']));
        }
       print("<h2>Test Date: $test_date</h2>");

//do a count of 1st lactation
        print('<p>primiparous #:' . count($primiparous) / 4 . ',multiparious #: ' . count($multiparious) / 4 . '</p>');

        foreach ($primiparous as $key => $val) {
            $val['bf'] = isset($val['bf']) ? $val['bf'] : null;
            if ($val['bf'] != null) {
                $outArray[] = $val['bf'];
            }
        }

        foreach ($multiparious as $key => $val) {
            $val['bf'] = isset($val['bf']) ? $val['bf'] : null;
            if ($val['bf'] != null) {
                $outArray2[] = $val['bf'];
            }
        }
        print('<p>');
        print('mean primiparous bf (kg): ' . round(array_sum($outArray) / count($outArray), 3));
        print(' mean multiparious bf (kg): ' . round(array_sum($outArray2) / count($outArray2), 3));
        print('</p>');

        $outArray = null;
        $outArray2 = null;

        foreach ($primiparous as $key => $val) {
            $val['fatp'] = isset($val['fatp']) ? $val['fatp'] : null;
            if ($val['fatp'] != null) {
                $outArray[] = $val['fatp'];
            }
        }

        foreach ($multiparious as $key => $val) {
            $val['fatp'] = isset($val['fatp']) ? $val['fatp'] : null;
            if ($val['fatp'] != null) {
                $outArray2[] = $val['fatp'];
            }
        }
        print('<p>');
        print('mean primiparous fat%: ' . round(array_sum($outArray) / count($outArray), 3));
        print(' mean multiparious fat%: ' . round(array_sum($outArray2) / count($outArray2), 3));
        print('</p>');
    }

    

}

//end class
?>