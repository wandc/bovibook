<?php 

if (defined('STDIN')) { //when called from cli, command line
    include_once('../functions/dataGridHelper.php');
} else { //when called any other way
    include_once ($_SERVER ['DOCUMENT_ROOT'] . 'functions/dataGridHelper.php');
    ;
}


//super hack, we use this class alot and for the error notifier to work
//we need to have it as a unique name, this is so it skips loading this.
//YUCK.
if (class_exists('DataGridLR_Ext') != true) {

    //extends datgrid class with specific logic to colour the line.
    //use print($dg1->render('datagrid',true,'00FF00')); to set colour
    class DataGridLR_Ext extends DataGridLR {

        function colourLineCriteria($row) {
            if ($row['straw_count'] <= 1) {
                return true;
            } else {
                return false;
            }
        }

    }

}

class BovineQueryBreedingChoice {

    private $bovine_id;
    private $bovineLocalNumber;
    private $bovineShortName;
    
    public function __construct($bovineID=null,$bovineLocalNumber=null,$bovineShortName=null) {
        $this->bovine_id = $bovineID;
        $this->bovineLocalNumber= $bovineLocalNumber;
        $this->bovineShortName= $bovineShortName;
    }

// end constructor
    // this will be called automatically at the end of scope
    public function __destruct() {
        // nothing
    }

    public function main() {

        print ('<div class="row">');
        print('<div class="col-xs-8">');
        $this->displaySireInfoAsItRelatesToChosenCow($this->bovine_id);
        print('</div>');
        print('<div class="col-xs-4">');
         $this->showIframeCDN($this->bovine_id);
       
        print('</div>');
        print('</div>');
        
        print ('<div class="row">');
        print('<div class="col-md-5">');
        require_once ($_SERVER ['DOCUMENT_ROOT'] . 'sitePages/bovineManagement/customComment.inc');
        CustomComment::newCustomCommentQuickForm(null, $this->bovine_id, 'trait');
        CustomComment::displayCustomComment('individual', $this->bovine_id, 'trait');
        print('</div>');
        print('<div class="col-md-5">');
        $this->breedingChoiceQuickForm();
        $this->showHaplotypeInfo($this->bovine_id);
        print('</div>');
        print('</div>');
        
        print ('<div class="row">');
        $this->displayBreedingChoicesHistorical($this->bovine_id);
        print('</div>');
    }

//end main

    public function showHaplotypeInfo($bovine_id) {
        $sql = "SELECT hh1,hh2,hh3,hh4,hh5,hcd1 FROM batch.aggregate_view_curr 
        LEFT JOIN bovinemanagement.bovine ON aggregate_view_curr.full_reg_number=bovine.full_reg_number
        WHERE id=$bovine_id LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
       
        if ($res->rowCount() == 0) {
             $out[]=("<li>No haplotype data available</li>");
        } else {
            $row = $res->fetch(PDO::FETCH_ASSOC);
             $out[]=("<li>HH1: {$row['hh1']}% </li><li> HH2: {$row['hh2']}% </li><li> HH3: {$row['hh3']}% </li><li> HH4: {$row['hh4']}% </li><li> HH5: {$row['hh5']}% </li><li> HCD1: {$row['hcd1']}%");
        }
        
         return (new BootStrap())->plainBox('Haplotypes',implode($out));
    }

    /** show iframe of cow's CDN page */
    public function showIframeCDN($bovine_id) {
        $sql = "SELECT full_reg_number FROM bovinemanagement.bovine WHERE id=($bovine_id) LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/cdnPageParser.php');
        $a = new CDNPageParser();
        $out = $a->generateIframe($row['full_reg_number']);
        print($out);
    }

    /**
     * This prints out what bull to breed to. Algorithm changed May 20, 2011 to second choice is whatever the second choice is.
     */
    public function testBullOrProvenAlgorithm($bovine_id) {

        $ret = array();

        //algorithm
        //when number of breedings is 0. use the bull in primary slot one. If none is available go down the primary AND secondary list until one is found.
        //when number of breedings is 1. use the bull in secondary slot one. If none is available go down the secondary list until one is found.
        //see how many breedings the cow has in this lactation, if the cow is dried off this will state 0, should maybe be from last fresh date whatever that is.
        $sql = "SELECT bovinemanagement.calculate_number_of_recent_breedings($bovine_id) as number_of_breedings";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);
        $ret['number_of_breedings'] = $row['number_of_breedings'];

        $sql3 = "SELECT bovinemanagement.calculate_number_of_recent_embyro_implants ($bovine_id) as number_of_embryo_implants";
        $res3 = $GLOBALS['pdo']->query($sql3);
        $row3 = $res3->fetch(PDO::FETCH_ASSOC);
        $ret['number_of_embryo_implants'] = $row3['number_of_embryo_implants'];

        //fancy statement to get a known order.
        $sql2 = "
            With temp as (
                SELECT 1 as od,short_name,id as sire_id,full_reg_number,(SELECT primary_one FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id) as semen_code FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=(SELECT primary_one FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id))
 UNION  
SELECT 2 as od,short_name,id as sire_id,full_reg_number,(SELECT primary_two FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id) as semen_code  FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=(SELECT primary_two FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id))
 UNION 
 SELECT 3 as od,short_name,id as sire_id,full_reg_number,(SELECT primary_three FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id) as semen_code  FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=(SELECT primary_three FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id))
 UNION
SELECT 4 as od,short_name,id as sire_id,full_reg_number,(SELECT secondary_one FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id) as semen_code FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=(SELECT secondary_one FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id))
 UNION  
SELECT 5 as od,short_name,id as sire_id,full_reg_number,(SELECT secondary_two FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id) as semen_code  FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=(SELECT secondary_two FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id))
 UNION 
 SELECT 6 as od,short_name,id as sire_id,full_reg_number,(SELECT secondary_three FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id) as semen_code  FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=(SELECT secondary_three FROM bovinemanagement.service_picks_curr WHERE bovine_id=$bovine_id))
)
            SELECT temp.*, bovinemanagement.check_semen_inventory_for_straw ( temp.semen_code) 
           FROM temp 
ORDER BY od ASC
 ";
        $res2 = $GLOBALS['pdo']->query($sql2);       //  print($sql2);
        //cow has not been bred yet.
        if ($row['number_of_breedings'] == 0) {
            while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
                if (($row2['od'] >= 0) AND ( $row2['check_semen_inventory_for_straw'] > 0)) {
                    $ret['short_name'] = $row2['short_name'];
                    $ret['semen_code'] = $row2['semen_code'];
                    return $ret;
                }
            } //end while
            $ret['text'] = 'Sire Needs to Be Selected (none seleced or no semen in stock)';
        }
        /* bred once or more */ elseif ($row['number_of_breedings'] >= 1) {

            while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
                if (($row2['od'] > 3) AND ( $row2['check_semen_inventory_for_straw'] > 0)) {
                    $ret['short_name'] = $row2['short_name'];
                    $ret['semen_code'] = $row2['semen_code'];
                    return $ret;
                }
            } //end while
            $ret['text'] = 'Sire Needs to Be Selected (none seleced or no semen in stock)';
        } else {
            throw new Exception('Number of Breedings out of range.');
        }

        return $ret;
    }

//end function

    /* used with bovinequery and dbase export  and dbase reports */
    function nextBreedingAction($bovine_id, $dbase = false) {
//return null when no bovine_id is supplied.
   if (empty($bovine_id)) {
    return null;
}
        
        // check to see if the cow is already in the DB and if it is, use the
        // default values already in.
        $row = null;
        $sql = "SELECT local_number,bovine.full_name,service_picks_curr.create_time,service_picks_curr.update_time,recipient,donotbreed,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks_curr.primary_one)) as p1_name,(SELECT true from bovinemanagement.semen_straw_curr_summary_available WHERE semen_code = service_picks_curr.primary_one limit 1) as p1_avail,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks_curr.primary_two)) as p2_name,(SELECT true from bovinemanagement.semen_straw_curr_summary_available WHERE semen_code = service_picks_curr.primary_two limit 1) as p2_avail,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks_curr.primary_three)) as p3_name,(SELECT true from bovinemanagement.semen_straw_curr_summary_available WHERE semen_code = service_picks_curr.primary_three limit 1)as p3_avail,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks_curr.secondary_one)) as s1_name,(SELECT true from bovinemanagement.semen_straw_curr_summary_available WHERE semen_code = service_picks_curr.secondary_one limit 1)as s1_avail,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks_curr.secondary_two)) as s2_name,(SELECT true from bovinemanagement.semen_straw_curr_summary_available WHERE semen_code = service_picks_curr.secondary_two limit 1) as s2_avail,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks_curr.secondary_three)) as s3_name,(SELECT true from bovinemanagement.semen_straw_curr_summary_available WHERE semen_code = service_picks_curr.secondary_three limit 1) as s3_avail,
		service_picks_curr.userid
FROM bovinemanagement.service_picks_curr 
LEFT JOIN bovinemanagement.bovine ON service_picks_curr.bovine_id =bovine.id
WHERE bovine.id={$bovine_id}
 ORDER BY event_time DESC LIMIT 1;
";

        $res = $GLOBALS ['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $nextActionTxt = '';

        $ret = bovineQueryBreedingChoice::testBullOrProvenAlgorithm($bovine_id);
        if ((isset($ret['text']) == false) || ($ret['text'] == null)) {
            $nextActionTxt = 'use <b>' . $ret['short_name'] . '</b>';
        } else {
            $nextActionTxt = $ret['text'];
        }

        // show if they are recipient
        if ($row ['recipient'] == 't') {
            if ($ret['number_of_embryo_implants'] > 0) {
                $nextActionTxt = "<b>RECIPIENT (implanted {$ret['number_of_embryo_implants']}X already)</b>";
            } else {
                $nextActionTxt = "<b>RECIPIENT</b>";
            }
        }

        // show if do not breed (OVERRIDES ALL)
        if ($row ['donotbreed'] == 't') {
            $nextActionTxt = " <b>DO NOT BREED</b>";
        }

        $arr['nextActionTxt'] = $nextActionTxt;
        $arr['row'] = $row;


        //cleanup text to send to dBase
        if ($dbase == true) {

            $arr['nextActionTxt'] = str_replace('use ', '', str_replace('use as ', '', strip_tags($arr['nextActionTxt'])));
        }




        return $arr;
    }

    
    function siresOldAndCurrentSeperate() {
          //fancy query to grab all sires, except for current ones in tank or projection.
         //then union with current ones, all vs tank.
        $sql = "SELECT *,'all' as type FROM(
SELECT semen_code,sire.id,sire.full_reg_number,sire.full_name,short_name,sexed_semen
 FROM bovinemanagement.sire_semen_code 
LEFT JOIN bovinemanagement.sire  ON sire_semen_code.sire_full_reg_number=full_reg_number
EXCEPT
SELECT semen_straw.semen_code,sire.id,sire.full_reg_number, sire.full_name,short_name,sexed_semen 
FROM bovinemanagement.semen_straw
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE 
 ) as one 
UNION
SELECT semen_straw.semen_code,sire.id,sire.full_reg_number, sire.full_name,short_name,sexed_semen, 'tank' as type 
FROM bovinemanagement.semen_straw
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = sire.full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE 
ORDER BY type DESC,short_name ASC;";



        $sireArray[null] = "Please Select one..."; //dummy value
        $res = $GLOBALS['pdo']->query($sql);
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            //parse for last word of name and then make an array for the select list.


            $str = null;
            if ($row['sexed_semen'] == 't') {
                $str = $row['short_name'] . " (sexed) " . " - " . $row['full_name'];
            } else {
                $str = $row['short_name'] . " - " . $row['full_name'];
            }


            if ($row['type'] == 'all') {
                $str = "OLD --- $str";
            }


            $sireArray[trim($row['semen_code'])] = $str;
        }
        return $sireArray;
    }
    
    function nextSireToUse($bovine_id)
    {
         $ret = $this->testBullOrProvenAlgorithm($bovine_id);

        if (isset($ret['text'])) {
            if ($ret['text'] == null) {
                $sireToUse = $ret['short_name'];
            } else {
                $sireToUse = $ret['text'];
            }
        } else {
            $sireToUse = '(Error: no proper sire picked).';
        }
        return("<h4>Sire To Use for next breeding: $sireToUse</h4><br>");
    }
    
    
    function breedingChoiceQuickForm() {
       

        $sireArray=$this->siresOldAndCurrentSeperate();

       print($this->nextSireToUse($this->bovine_id));

        $form = new HTML_QuickForm("breedingChoiceQuickForm", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true); 
        $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'breedingChoiceQuickform', "Record Service Sire Choice for ".$this->bovineLocalNumber .' - '. $this->bovineShortName);
        $form->addElement('checkbox', 'donotbreed', 'Marked as Do Not Breed?');
        $form->addElement('checkbox', 'recipient', 'Marked as Recipient?');
        $form->addElement('checkbox', 'donor', 'Marked as Donor?');
        $form->addElement('header', 'head1', 'First Two Breedings:');
        $form->addElement('select', 'primary_one', 'Choice 1', $sireArray);
        $form->addElement('select', 'primary_two', 'Choice 2', $sireArray);
        $form->addElement('select', 'primary_three', 'Choice 3', $sireArray);
        $form->addElement('header', 'head2', 'Later Breedings');
        $form->addElement('select', 'secondary_one', 'Choice 1', $sireArray);
        $form->addElement('select', 'secondary_two', 'Choice 2', $sireArray);
        $form->addElement('select', 'secondary_three', 'Choice 3', $sireArray);
        //
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'bovine_id', $this->bovine_id);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //rules     
        //register custom rule
        $form->registerRule('checkSireChoiceConsistencyRule', 'function', 'checkSireChoiceConsistency', 'bovineQueryBreedingChoice');
        $form->addRule(array('bovine_id', 'donotbreed', 'recipient', 'donor'), 'Only one of "Do not breed" or "Recipient" or "donor" can be selected at once.', 'checkSireChoiceConsistencyRule', array('bovine_id', 'donotbreed', 'recipient', 'donor'));
       /*
        //register custom rule 2
        $form->registerRule('checkSireOrBooleanConsistencyRule', 'function', 'checkSireOrBooleanConsistency', 'bovineQueryBreedingChoice');
        $form->addRule(array('bovine_id', 'donotbreed', 'recipient', 'donor'), 'Only one of "Do not breed" or "Recipient" or "donor" can be selected at once 2.', 'checkSireOrBooleanConsistencyRule', array('bovine_id', 'donotbreed', 'recipient', 'donor'));
*/


        //check to see if the cow is already in the DB and if it is, use the default values already in.
        $row = null;
        $sql = "SELECT donotbreed,donor,recipient,primary_one,primary_two,primary_three,secondary_one,secondary_two,secondary_three FROM bovinemanagement.service_picks_curr WHERE bovine_id={$this->bovine_id} ORDER BY event_time DESC LIMIT 1";
        $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount() == 1) {
        $row = $res->fetch(PDO::FETCH_ASSOC);
     //print_r2($row);
        $form->setDefaults(array('donotbreed' =>  empty($row['donotbreed']) ? false :true));
        $form->setDefaults(array('recipient' =>  empty($row['recipient']) ? false :true));
        $form->setDefaults(array('donor' => empty($row['donor']) ? false :true));
        $form->setDefaults(array('primary_one' => trim($row['primary_one'])));
        $form->setDefaults(array('primary_two' => trim($row['primary_two'])));
        $form->setDefaults(array('primary_three' => trim($row['primary_three'])));
        $form->setDefaults(array('secondary_one' => trim($row['secondary_one'])));
        $form->setDefaults(array('secondary_two' => trim($row['secondary_two'])));
        $form->setDefaults(array('secondary_three' => trim($row['secondary_three'])));
        }

        // Try to validate a form
        if ($form->validate()) {
print("submitted!<br>");
            //get values
            $bovine_id = $form->exportValue('bovine_id');
            $donotbreed = $form->exportValue('donotbreed');
            $recipient = $form->exportValue('recipient');
            $donotbreed = $form->exportValue('donotbreed');
            $donor = $form->exportValue('donor');


            $pageid = $form->exportValue('pageid');
            $userid=$GLOBALS['auth']->getUsername();
            $primary_one = $form->exportValue('primary_one');
            $primary_two = $form->exportValue('primary_two');
            $primary_three = $form->exportValue('primary_three');
            $secondary_one = $form->exportValue('secondary_one');
            $secondary_two = $form->exportValue('secondary_two');
            $secondary_three = $form->exportValue('secondary_three');
            


            
                //always do an insert as we recrd each change.
                $query = "INSERT INTO bovinemanagement.service_picks (bovine_id,recipient,donotbreed,primary_one,primary_two,primary_three,secondary_one,secondary_two,secondary_three,userid,event_time,donor)
		VALUES (:bovine_id,:recipient,:donotbreed,:primary_one,:primary_two,:primary_three,:secondary_one,:secondary_two,:secondary_three,:userid,current_timestamp,:donor)";
                $res = $GLOBALS['pdo']->prepare($query);
                $res->bindValue(':bovine_id', $bovine_id, PDO::PARAM_INT);
                $res->bindValue(':donotbreed', empty($donotbreed) ? false :$donotbreed, PDO::PARAM_BOOL);
                $res->bindValue(':donor', empty($donor) ? false :$donor, PDO::PARAM_BOOL);
                $res->bindValue(':recipient', empty($recipient) ? false : $recipient, PDO::PARAM_BOOL);
                $res->bindValue(':primary_one', empty($primary_one) ? null : $primary_one, PDO::PARAM_STR);
                $res->bindValue(':primary_two', empty($primary_two) ? null : $primary_two, PDO::PARAM_STR);
                $res->bindValue(':primary_three', empty($primary_three) ? null : $primary_three, PDO::PARAM_STR);
                $res->bindValue(':secondary_one', empty($secondary_one) ? null : $secondary_one, PDO::PARAM_STR);
                $res->bindValue(':secondary_two', empty($secondary_two) ? null : $secondary_two, PDO::PARAM_STR);
                $res->bindValue(':secondary_three', empty($secondary_three) ? null : $secondary_three, PDO::PARAM_STR);
                $res->bindValue(':userid', $userid, PDO::PARAM_STR);

                $res->execute();
                var_dump($bovine_id, $donotbreed,$donor,$recipient,$primary_one,$primary_two,$primary_three,$secondary_one,$secondary_two,$secondary_three); 

            $host = $_SERVER['HTTP_HOST'];
          header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid&bovine_id={$this->bovine_id}");
            exit();
        } //end validation
        $form->removeAttribute('name');
        $form->accept($renderer);
        echo $renderer->toHtml();
    }

    /*
     * checks that a non empty form submitted.
     */
      function checkSireOrBooleanConsistency($valueArr, $elementNameArr) {

        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData[$elementName] = $valueArr[$count];
            $count++;
        }
        
        //they don't exist when not filled in. Make them exist.
        $primary_one=empty($formData['primary_one']) ? '' : $formData['primary_one'];
        $primary_two=empty($formData['primary_two']) ? '' : $formData['primary_two'];
        $primary_three=empty($formData['primary_three']) ? '' : $formData['primary_three'];
        $secondary_one=empty($formData['secondary_one']) ? '' : $formData['secondary_one'];
        $secondary_two=empty($formData['secondary_two']) ? '' : $formData['secondary_two'];
        $secondary_three=empty($formData['secondary_three']) ? '' : $formData['secondary_three'];
        
        
        //check that something is being submitted, ie form not empty.
        $a=(int) $formData['donotbreed'] +  (int) $formData['recipient'] + (int) $formData['recipient'];
        $b= strlen($primary_one) + strlen($primary_two) + strlen($primary_three);
        $c= strlen($secondary_one) + strlen($secondary_two) + strlen($secondary_three);
        $aa=$a+$b+$c;
        if ($aa >= 1) {
            return true;
        } else {
            return false;
        }

    
    }
    
    
    
    function checkSireChoiceConsistency($valueArr, $elementNameArr) {
        //this has only 2 paramaters because it is outside calling function (ie in the class)
        //generic loop to convert $form elements we want into a usuable array
        //I really hope this isn't how quickforms was designed.

        $count = 0;
        foreach ($elementNameArr as $elementName) {
            $formData[$elementName] = $valueArr[$count];
            $count++;
        }

       

        //only one or zero allowed to be slected at once.
        $a=(int) $formData['donotbreed'] +  (int) $formData['recipient'] + (int) $formData['recipient'];
         
        if ($a <= 1) {
            $returnStatement = true;
        } else {
            $returnStatement = false;
        }

        return $returnStatement;
    }

   

 
     public function progenyBreedingChoiceForBovine($request) {
          if (!empty($request['item'])) {
            if (is_numeric($request['item'])) {
                $bovine_id = $request['item'];
            }
        } else {
            throw new Exception('Error: no bovine id specified');
        }
         
         
         
      $sql = "with main as (
SELECT bovinecurr.id as bovine_id,bovinecurr.local_number,bovinecurr.full_name,bovinecurr.full_reg_number as potential_dam_full_reg_number,
sire.full_name as sire_full_name,sire.short_name as sire_short_name,
cdn_progeny_potential_genetics.sire_full_reg_number,
lpi_code,cdn_progeny_potential_genetics.lpi,cdn_progeny_potential_genetics.prodoll,percent_inbreeding,milk,fat,protein,percent_fat,percent_protein,scs,conf,ms,f_and_l,ds,rp,cdn_progeny_potential_genetics.update_time,
aggregate_view_all.reliability as reliability_percent,
(SELECT count(id) FROM bovinemanagement.semen_straw LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE AND sire_semen_code.sire_full_reg_number=cdn_progeny_potential_genetics.sire_full_reg_number) as straw_count
 FROM batch.cdn_progeny_potential_genetics 
LEFT JOIN bovinemanagement.bovinecurr ON bovinecurr.full_reg_number = cdn_progeny_potential_genetics.dam_full_reg_number
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = cdn_progeny_potential_genetics.sire_full_reg_number
LEFT JOIN batch.aggregate_view_all ON aggregate_view_all.full_reg_number = cdn_progeny_potential_genetics.sire_full_reg_number
WHERE bovinecurr.id =$bovine_id AND cdn_progeny_potential_genetics.prodoll > 1000 ORDER BY cdn_progeny_potential_genetics.prodoll DESC
), temp2 as (
/* this part of the query is the same criteria we use in CDN inbreeding to decide what to download. It is used to tell if there is no data downloaded for certain sires.*/
SELECT short_name FROM (SELECT DISTINCT full_reg_number,short_name,full_name
FROM bovinemanagement.semen_straw 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code = semen_straw.semen_code 
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number 
WHERE semen_straw.breeding_event_id IS NULL AND semen_straw.discarded IS FALSE 
GROUP BY full_reg_number,short_name,full_name
ORDER BY short_name) as d 
), temp3 as (
/*create final table and add haplotype json data */
SELECT  distinct ( sire_full_name) ,main.*,temp2.short_name as possible_sire_short_name,
(SELECT cdn_pedigree_data->'haplotypes' FROM batch.aggregate_site_data WHERE full_reg_number =sire_full_reg_number  ) as potential_sire_haplotypes,
(SELECT cdn_pedigree_data->'haplotypes' FROM batch.aggregate_site_data WHERE full_reg_number =potential_dam_full_reg_number  ) as potential_dam_haplotypes
FROM temp2
LEFT JOIN main on temp2.short_name=sire_short_name 
)
/* convert json to numeric and find bad haplotypes. NOTE: ADD MORE HAPLOTYPES HERE IN FUTURE*/
SELECT * , CONCAT(
(SELECT 'HCD '::text WHERE (potential_sire_haplotypes->>'HCD')::numeric >1 AND(potential_dam_haplotypes->>'HCD')::numeric >1),
(SELECT 'HH1 '::text WHERE (potential_sire_haplotypes->>'HH1')::numeric >1  AND(potential_dam_haplotypes->>'HH1')::numeric >1),
(SELECT 'HH2 '::text  WHERE (potential_sire_haplotypes->>'HH2')::numeric >1 AND(potential_dam_haplotypes->>'HH2')::numeric >1),
(SELECT 'HH3 '::text  WHERE (potential_sire_haplotypes->>'HH3')::numeric >1 AND(potential_dam_haplotypes->>'HH3')::numeric >1),
(SELECT 'HH4 '::text  WHERE (potential_sire_haplotypes->>'HH4')::numeric >1 AND(potential_dam_haplotypes->>'HH4')::numeric >1),
(SELECT 'HH5'::text  WHERE (potential_sire_haplotypes->>'HH5')::numeric >1 AND(potential_dam_haplotypes->>'HH5')::numeric >1)
) as haplotype_error
FROM temp3
ORDER by prodoll DESC
;";

        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        
        //change presentation
        foreach ($results as &$row) {   
           
             if (empty($row['haplotype_error'])) {
                $row['colour_row'] = 0;
            } else {
                $row['colour_row'] = 1;
            }
            
        }
         
    

        //output
        $output['data'] = $results;
        return ($output);
    }
    
    

    // displays info on potential offspring from cdn website.
    function displaySireInfoAsItRelatesToChosenCow($bovine_id, $mode = null) {

         $res = $GLOBALS['pdo']->query("SELECT id FROM bovinemanagement.bovine WHERE id=$bovine_id AND (birth_date + interval '8 months') < current_date LIMIT 1");
        if ($res->rowCount() == 0) {
          print("<h3>Note: Under 8 Months of age, no inbreeding data will be caluclated.<h3>");
          return;
        }
        else {
             
        }

        
          $dt=new JQueryDataTable();
$columnsStr= <<<EX
              {
                 data: "prodoll",
                 "visible": false,
                "searchable": false
            },             
            {
                data: "sire_short_name",
                title: "sire"
            },
            {
                data: "prodoll",
                title: "Pro$"
            },
           
        {
                data: "straw_count",
                title: "# In Stock"
        },
        
        {
                data: "percent_inbreeding",
                title: "INB %"
        },
        
        {
                data: "fat",
                title: "Fat (kg)"
        },
        {
                data: "protein",
                title: "Prot (kg)"
        },
        {
                data: "percent_fat",
                title: "Fat (%)"
        },
        {
                data: "percent_protein",
                title: "Prot (%)"
        },
        {
                data: "scs",
                title: "SCS"
        },
        {
                data: "conf",
                title: "Conf"
        },
        {
                data: "ms",
                title: "Mamm"
        },
        {
                data: "f_and_l",
                title: "F&L"
        },
        {
                data: "ds",
                title: "DS"
        },
        {
                data: "rp",
                title: "RP"
        },
        {
                data: "haplotype_error",
                title: "Haplotype Match"
        }
           
EX;
   print("<h3>Potential Progeny's PA Genetics</h3>");
    print($dt->startCustom(6,'progenyBreedingChoiceForBovine',$bovine_id,$columnsStr,'highlight_important'));     
        
   
      

       
    }


    public function error() {
        $error = array();

        //find if no Haplotype data s in db and if so throw an error.
        $res = $GLOBALS['pdo']->query("SELECT * FROM batch.aggregate_view_curr LIMIT 1");
        if ($res->rowCount() == 0) {
            $error[] = new notifyObj(get_class($this), date('r', strtotime("now")), 2, 'There is no Haplotype (Aggregate) data available at all.');
        }

        //find if no Haplotype data is out of date.
        $res2 = $GLOBALS['pdo']->query("SELECT update_time FROM batch.aggregate_view_curr WHERE update_time <=(current_date - interval '8 days') LIMIT 1");
        if ($res2->rowCount() != 0) {
            $row2 = $res2->fetch(PDO::FETCH_ASSOC);
            $error[] = new notifyObj(get_class($this), date('r', strtotime($row2['update_time'])), 2, 'Haplotype (Aggregate) data is more then 8 days since an update.');
        }



        return $error;
    }
 
    
    
  public function breedingChoicesHistorical($request) {
        if (!empty($request['item'])) {
            if (is_numeric($request['item'])) {
                $bovine_id = $request['item'];
            }
        } else {
            throw new Exception('Error: no bovine id specified');
        }
         

        $sql = "SELECT local_number,bovine.full_name,bovine.id as bovine_id,service_picks.create_time,service_picks.update_time,recipient,donotbreed,donor,event_time,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks.primary_one)) as p1_name,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks.primary_two)) as p2_name,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks.primary_three)) as p3_name,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks.secondary_one)) as s1_name,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks.secondary_two)) as s2_name,
(SELECT short_name FROM bovinemanagement.sire WHERE full_reg_number=(SELECT sire_full_reg_number FROM bovinemanagement.sire_semen_code WHERE semen_code=service_picks.secondary_three)) as s3_name,
		service_picks.userid
FROM bovinemanagement.service_picks 
LEFT JOIN bovinemanagement.bovine ON service_picks.bovine_id =bovine.id
WHERE bovine.id=$bovine_id ORDER BY event_time DESC ";
      
   
  
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {

           $row['local_number'] = $GLOBALS['MiscObj']->hrefToBovinePageFullName($row['bovine_id'],$row['local_number'],$row['full_name']);
           $row['event_time_raw']=strtotime($row['event_time']);
           $row['event_time']= date("M d, Y",strtotime($row['event_time']));  //has to be at end.
         
        }
        //output
        $output['data'] = $results;
        return ($output);
  }
    
    
    
function displayBreedingChoicesHistorical($bovine_id) {
        //  Time	donotbreed	recipient	donor	p1_name	p2_name	p3_name	s1_name	s2_name	s3_name	useri
          $dt=new JQueryDataTable();
$columnsStr= <<<EX
             {
                 data: "event_time_raw",
                 "visible": false,
                "searchable": false
            },              
            {
                data: "event_time",
                title: "Time"
            },
            {
                data: "donotbreed",
                title: "donotbreed"
            },
            {
                data: "recipient",
                title: "recipient"
        },
        {
                data: "donor",
                title: "donor"
        },
        {
                data: "p1_name",
                title: "p1_name"
        },
        {
                data: "p2_name",
                title: "p2_name"
        },
        {
                data: "p3_name",
                title: "p3_name"
        },
        {
                data: "s1_name",
                title: "s1_name"
        },
        {
                data: "s2_name",
                title: "s2_name"
        },
        {
                data: "s3_name",
                title: "s3_name"
        },
        {
                data: "userid",
                title: "userid"
        }
           
EX;
   print("<h3>Historical Breeding Choices</h3>");
    print($dt->startCustom(6,'breedingChoicesHistorical',$bovine_id,$columnsStr));     
        
     
}}

//end class
?>