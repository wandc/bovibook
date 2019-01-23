<?php 
//Lists cow genetic info
class Genetics extends BasePage {

  

    public function defaultDisplay() {
        $this->displayAllCowLPI2();
    }

    public function displayAllCowLPI2($type = null) {
           $dt=new JQueryDataTable();
            $options = '"paging": false,  "order": [[0,\'asc\']],';
$columnsStr= <<<EX
             {
                            data: "local_number",
                            title: "Animal",
                            type: "num-html"
                        },
                        {
                            data: "CDN",
                            title: "CDN"
                        },
                        {
                            data: "HC",
                            title: "HC"
                        },
                        {
                            data: "prodoll",
                            title: "Pro$"
                        },
                        {
                            data: "lpi",
                            title: "LPI"
                        },
                        {
                            data: "geno_test",
                            title: "GEBV"
                        },
                        {
                            data: "score",
                            title: "Classification Score"
                        },
                        {
                            data: "birth_date",
                            title: "Birth Date"
                        },
                        {
                            data: "full_name",
                            title: "Name"
                        },
                        {
                            data: "prodoll_birthyear_quintile_rank",
                            title: "prodoll_birthyear_quintile_rank"
                        }
        
           
EX;
    
    print($dt->start($this->pageid,'AllAliveBovineLPI',null,$columnsStr,null,false,$options));     
} 

/*
 * switched to materlized viw for this to speed it up, run nightly
 * 
 * CREATE materialized view bovinemanagement.herdgenetics as
SELECT bovinecurr.id, local_number,lpi,prodoll,geno_test,score,bovinecurr.full_name,bovinecurr.full_reg_number,bovinecurr.birth_date,
 batch.prodoll_birthyear_quintile_rank( bovinecurr.id,(EXTRACT(YEAR FROM bovinecurr.birth_date))::integer)
FROM bovinemanagement.bovinecurr 
LEFT JOIN batch.aggregate_view_curr ON aggregate_view_curr.full_reg_number = bovinecurr.full_reg_number
ORDER BY prodoll DESC NULLS LAST;
 * 
 */






    public function AllAliveBovineLPI($request) {
        $sql = "
SELECT * from bovinemanagement.herdgenetics";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            $row['score'] = $row['score'];
            $row['local_number'] =JQueryDataTable::formattedAnimalNameLink($row['id'],$row['local_number'],$row['full_name']);
            $row['HC'] = ('<a href="' . HolsteinCanadaHelper::createHolsteinCanadaQuery($row['full_reg_number']) . '">' . '<img  width="28" height="20" src="/images/holsCanadaBWSmall.svg" />' . "</a>" . "\n\r");
            $row['CDN'] = ('<a href="' . $GLOBALS['MiscObj']->createListOfAllCowsMilking($row['full_reg_number']) . '"><img   width="50" height="19" src="/images/cdnLogo.svg" /></a>');
            
        }

        //output
        $output['data'] = $results;
        return ($output);
    }

}

//end class
?>