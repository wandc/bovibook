<?php



require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/dataGridHelper.php');
//
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/reproduction/estrusBreedings.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/reproduction/estrusSemen.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/transition/transitionCalving.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/transition/transitionDryoff.inc');
require_once($_SERVER['DOCUMENT_ROOT'] . '/sitePages/bovineManagement/herdCull.inc');

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
        print("<h3>Breedings and Embryo Implantations</h3>");
        $this->displayCombinedBreedingsAndEmbryos();

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
        $sql = 'SELECT type,event_time ,(recipient_local_number || \' - \' || recipient_full_name) as Recipient,(service_sire_short_name || \' - \' ||  service_sire_full_name) as Sire, service_sire_full_reg_number as "full_reg_number", (donor_dam_local_number  || \' - \' ||  donor_dam_full_name)  as "donor_dam"	  FROM bovinemanagement.combined_breeding_embryo_view WHERE event_time >= current_date - interval \'9 weeks\'  ORDER BY event_time DESC';

        $numberPerPage = 1040;
        $dg = new DataGridLR($sql, $numberPerPage);

        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Type', 'type', 'type', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Date', null, '', array('' => ''), null, array('DataGridHelper', 'printFormattedTime()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Recipient', 'recipient', 'recipient', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sire', 'sire', 'sire', array('' => ''), null, null));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Sire Reg #', null, '', array('' => ''), null, array('DataGridHelper', 'printCDNRegNumberLink()')));
        $dg->datagrid->addColumn(new Structures_DataGrid_Column('Donor Dam', 'donor_dam', 'donor_dam', array('' => ''), null, null));



        // Output table and paging links
        print($dg->render());
        $dg->pager();
    }

}

//end class
?>