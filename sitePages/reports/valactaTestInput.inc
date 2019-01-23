<?php
class ValactaTestInput extends BasePage {

    public function defaultDisplay() {

        $this->run();
    }

    function run() {
        //
        print("<h3>Calvings</h3>");
        $calving = new TransitionCalving();
        $calving->milkTestInputsDisplay();

        //
        print($this->displayCombinedBreedingsAndEmbryos());

        //
        print("<h3>Dried Off List</h3>");
        $dryoff = new TransitionDryoff();
        $dryoff->milkTestInputsDisplay();
        //

        print("<h3>Herd Cull</h3>");
        $culled = new HerdCull();
        $culled->displayCowsWhoHaveBeenCulled('milkTestInputsDisplay');
    }

    function displayCombinedBreedingsAndEmbryos() {
                 
$sql = <<<SQL
SELECT type,event_time ,(recipient_local_number || ' -' || recipient_full_name) as Recipient,(service_sire_short_name || ' - ' ||  service_sire_full_name) as Sire, 
    service_sire_full_reg_number as "full_reg_number", (donor_dam_local_number  || ' - ' ||  donor_dam_full_name)  as "donor_dam"	  
    FROM bovinemanagement.combined_breeding_embryo_view 
    WHERE event_time >= current_date - interval '9 weeks'  
    ORDER BY event_time DESC
SQL;


        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
            
            $rowOut['Type'] = $row['type'];
            $rowOut['Date'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Recipient'] = $row['recipient'];
            $rowOut['Sire Name'] = $row['sire'];
            $rowOut['Sire Number'] = $row['full_reg_number'];          
            $rowOut['Donor Dam'] = $row['donor_dam'];
  
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;
      
        $out[]="<h3>Combined Breeding and Embryo Implantations (last 9 weeks)</h3>";
        $out[]=( (new JQueryDataTable)->startBasicArray($dataArr,'')); 
        return implode($out);   
        

}
}
//end class
?>