<?php 
error_reporting(E_ALL);
require_once('../global.php');

/*
 * Class that holds all the files needed to do a dBase export.
 * Called by another file to actually do the export.
 */

class dBase {

    CONST CSVDIR = '/home/common/dbImport';

//does the export.    
    public function main() {
        SSE_Message::send_message(0, 'Doing dbase Export.', round((0 / 10 ) * 100, 1)); //sse           
        $this->dBaseExportBovine();
        $this->dBaseExportLocation();
        $this->dBaseExportHeats();
        $this->dBaseExportBreedings();
        $this->dBaseExportPregnancies();
        SSE_Message::send_message(5, 'Doing dbase Export.', round((5 / 10 ) * 100, 1)); //sse
        $this->dBaseExportSalePrice();
        $this->dBaseExportReproHormone();
        $this->dBaseExportVaccine();
        $this->dBaseExportBreedingChoices();
        $this->dBaseExportCalvings();
        $this->dBaseExportHerdCull();
        SSE_Message::send_message('CLOSE', 'Process complete',100);//sse
    }

    function dBaseExportBovine() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT * FROM bovinemanagement.bovine ORDER BY local_number");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//create a csv line
            $line = "{$row['id']},{$row['local_number']},{$row['full_reg_number']},{$row['full_name']},{$row['birth_date']},{$row['death_date']},{$row['colour']},{$row['sire_full_reg_number']},{$row['dam_full_reg_number']},{$row['recipient_full_reg_number']},{$row['rfid_number']},{$row['ownerid']},{$row['create_time']},{$row['update_time']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/bovine.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportLocation() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT bovinecurr.id,bovinecurr.local_number,bovinecurr.full_reg_number,bovinecurr.location_name,location_name_dbase FROM bovinemanagement.bovinecurr ORDER BY bovinecurr.local_number");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//create a csv line
            $line = "{$row['id']},{$row['local_number']},{$row['full_reg_number']},{$row['location_name_dbase']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/loc.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportSalePrice() {

//grab a list of latest prices.
        $res = $GLOBALS['pdo']->query("SELECT bovinecurr.id as bovine_id,bovinecurr.full_name,bovinecurr.full_reg_number, bovinecurr.location_name,bovinecurr.local_number,sale_price.price,sale_price.plus,sale_price.custom_comment,sale_price_comment.comment,sale_price.event_time,sale_price.userid
FROM bovinemanagement.bovinecurr
LEFT JOIN bovinemanagement.sale_price ON sale_price.id=(SELECT id from bovinemanagement.sale_price WHERE sale_price.bovine_id=bovinecurr.id AND sale_price.event_time=(SELECT max(event_time) FROM bovinemanagement.sale_price WHERE bovine_id=bovinecurr.id))
LEFT JOIN bovinemanagement.sale_price_comment ON sale_price.comment_id=sale_price_comment.id
ORDER BY local_number");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//create a csv line
            $line = "{$row['bovine_id']},{$row['local_number']},{$row['full_reg_number']},{$row['price']},{$row['plus']},{$row['comment']},{$row['custom_comment']},{$row['event_time']},{$row['userid']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/price.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportHeats() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT estrus_event.id as estrus_id,estrus_event.bovine_id,local_number,full_name,name as event_name,estrus_event.event_time as event_time,birth_date,estrus_event.userid,bovine.full_reg_number,breeding_rating   
FROM bovinemanagement.estrus_event 
LEFT JOIN bovinemanagement.bovine ON bovine.id=estrus_event.bovine_id 
LEFT JOIN bovinemanagement.estrus_type ON estrus_event.estrus_type_id=estrus_type.id 
LEFT JOIN bovinemanagement.lactation ON bovine.id=lactation.bovine_id 
GROUP BY estrus_event.id,estrus_event.bovine_id,bovine.local_number,full_name,estrus_type.name,event_time,birth_date,estrus_event.userid,full_reg_number,breeding_rating 
ORDER BY event_time");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//create a csv line
            $line = "{$row['estrus_id']},{$row['event_time']},{$row['local_number']},{$row['full_reg_number']},{$row['full_name']},{$row['event_name']},{$row['breeding_rating']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/heats.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportBreedings() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT breeding_event.id, breeding_event.estimated_breeding_time, breeding_event.estimated_breeding_userid, breeding_event.actual_breeding_time, breeding_event.actual_breeding_userid, breeding_event.bovine_id, bovine.local_number, bovine.full_name , sire.full_name as service_sire_full_name,sire.full_reg_number as sire_full_reg_number,semen_straw_choice_userid,sexed_semen,palpate_comment.comment as breeding_comment,bovine.full_reg_number,semen_straw.bin,semen_straw.freeze_date,sire.short_name,semen_straw.semen_code,semen_straw.supplier_code
FROM bovinemanagement.breeding_event 
LEFT JOIN bovinemanagement.bovine ON breeding_event.bovine_id = bovine.id 
LEFT JOIN bovinemanagement.semen_straw ON breeding_event.semen_straw_id = semen_straw.id 
LEFT JOIN bovinemanagement.sire_semen_code ON sire_semen_code.semen_code=semen_straw.semen_code
LEFT JOIN bovinemanagement.sire ON sire.full_reg_number = sire_semen_code.sire_full_reg_number
LEFT JOIN bovinemanagement.palpate_comment ON palpate_comment.id = breeding_event.comment_id
WHERE actual_breeding_time IS NOT NULL
ORDER BY actual_breeding_time ASC");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

//break up registration number for fred.
            $cowRegArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['full_reg_number']);
            $sireRegArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['sire_full_reg_number']);


            $row['freeze_date'] = date('ymd', strtotime($row['freeze_date'])); //change to 2 digit year and get rid of dashes for dBase.
//$row['freeze_date']=str_replace('-','',$row['freeze_date']);//line to change date type to string? for fred.
//create a csv line
            $line = "{$row['id']},{$row['actual_breeding_time']},{$row['local_number']},{$row['full_name']},{$cowRegArray['breed']},{$cowRegArray['country']}{$cowRegArray['sex']}{$cowRegArray['number']},{$row['service_sire_full_name']},{$row['short_name']},{$sireRegArray['breed']},{$sireRegArray['country']},{$sireRegArray['sex']},{$sireRegArray['number']},{$row['semen_code']},{$row['bin']},{$row['freeze_date']},{$row['supplier_code']},{$row['breeding_comment']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/breeding.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportHerdCull() {
        $sql = "SELECT *,cull_event.id as id FROM bovineManagement.cull_event LEFT JOIN bovinemanagement.bovine ON bovine.id=cull_event.bovine_id ORDER BY event_time DESC";

        $res = $GLOBALS['pdo']->query($sql);


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {


//break up registration number for fred.
            $cowRegArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['full_reg_number']);

//create a csv line
            $line = "{$row['id']},{$row['event_time']},{$row['local_number']},{$row['full_name']},{$cowRegArray['breed']},{$cowRegArray['country']}{$cowRegArray['sex']}{$cowRegArray['number']},{$row['reason']},{$row['comment']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/culls.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportCalvings() {
        $sql = "Select calving_event_ease_types.name as calving_ease, bovine.full_name, bovine.local_number,bovine.id as bovine_id, calving_event.id,calving_event.update_time,calving_event.event_time,calving_event.userid,calving_event.create_time,calving_event.alive_or_dead,calving_event.calf_rfid_number,calving_event.calf_sex,calving_event.id as calving_event_id,calving_event.calf_mass,calving_event.lactation_id,bovine.full_reg_number,bovine.birth_date as dam_birth_date, (SELECT local_number FROM bovinemanagement.bovine WHERE bovine.full_reg_number='HOCANF' || calf_rfid_number LIMIT 1) as calf_local_number 
FROM bovinemanagement.calving_event LEFT JOIN bovinemanagement.lactation ON lactation.id=calving_event.lactation_id LEFT JOIN bovinemanagement.calving_event_ease_types ON calving_event_ease_types.id = calving_event.calving_ease LEFT JOIN bovinemanagement.bovine ON bovine.id=lactation.bovine_id ORDER BY calving_event.event_time  DESC 
";


        $res = $GLOBALS['pdo']->query($sql);


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {


//break up registration number for fred.
            $cowRegArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($row['full_reg_number']);

//create a csv line
            $line = "{$row['id']},{$row['event_time']},{$row['local_number']},{$row['full_name']},{$cowRegArray['breed']},{$cowRegArray['country']}{$cowRegArray['sex']}{$cowRegArray['number']},{$row['calving_ease']},{$row['alive_or_dead']},{$row['calf_rfid_number']},{$row['calf_local_number']},{$row['calf_sex']},{$row['calf_mass']},{$row['lactation_id']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/calving.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportPregnancies() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT preg_check_event.id,preg_check_event.event_time,local_number,full_reg_number,full_name,test_full_name,preg_check_result,userid,service_sire_short_name,preg_check_comment_custom,comment,combined_breeding_embryo_view.event_time as actual_breeding_time
FROM bovinemanagement.preg_check_event
LEFT JOIN bovinemanagement.combined_breeding_embryo_view ON combined_breeding_embryo_view.id=preg_check_event.estimated_breeding_embryo_event_id
JOIN bovinemanagement.preg_check_type ON preg_check_event.preg_check_type_id = preg_check_type.id
JOIN bovinemanagement.bovine ON bovine.id=preg_check_event.bovine_id
LEFT JOIN bovinemanagement.preg_check_comment ON preg_check_comment.id = preg_check_event.preg_check_comment_id
ORDER BY preg_check_event.id DESC;");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

//combine comments into one field.
            if ($row['preg_check_comment_custom'] == null) {
                $comment = $row['comment'];
            } elseif ($row['comment'] == null) {
                $comment = $row['preg_check_comment_custom'];
            } else {
                $comment = $row['comment'] . ' & ' . $row['preg_check_comment_custom'];
            }


//create a csv line
            $line = "{$row['id']},{$row['event_time']},{$row['local_number']},{$row['full_reg_number']},{$row['full_name']},{$row['test_full_name']},{$row['preg_check_result']},{$row['userid']},{$row['service_sire_short_name']},{$row['actual_breeding_time']},$comment";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/preg.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportReproHormone() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT medicine_administered.id as med_id,local_number,full_name,full_reg_number,trade_name,dosage,dosage_unit,location,note,userid,event_time
		FROM bovinemanagement.medicine_administered 
		LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
		LEFT JOIN bovinemanagement.bovine ON medicine_administered.bovine_id = bovine.id
		WHERE medicine.medicine_class='hormone'
		ORDER BY medicine_administered.event_time DESC");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//create a csv line
            $line = "{$row['med_id']},{$row['event_time']},{$row['local_number']},{$row['full_reg_number']},{$row['full_name']},{$row['trade_name']},{$row['dosage']},{$row['dosage_unit']},{$row['note']},{$row['userid']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/reprohor.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportVaccine() {

//grab a list of locations.
        $res = $GLOBALS['pdo']->query("SELECT medicine_administered.id as med_id,local_number,full_name,full_reg_number,trade_name,dosage,dosage_unit,location,note,userid,event_time
		FROM bovinemanagement.medicine_administered 
		LEFT JOIN bovinemanagement.medicine ON medicine_administered.medicine_index=medicine.id 
		LEFT JOIN bovinemanagement.bovine ON medicine_administered.bovine_id = bovine.id
		WHERE medicine.medicine_class='vaccine'
		ORDER BY medicine_administered.event_time DESC");


        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
//create a csv line
            $line = "{$row['med_id']},{$row['event_time']},{$row['local_number']},{$row['full_reg_number']},{$row['full_name']},{$row['trade_name']},{$row['dosage']},{$row['dosage_unit']},{$row['note']},{$row['userid']}";
            $fileArray[] = $line;
        }

        $fp = fopen(self::CSVDIR . '/vaccine.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

    function dBaseExportBreedingChoices() {



        $sql = "SELECT * FROM bovinemanagement.bovinecurr";
        $res = $GLOBALS['pdo']->query($sql);
       
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
             $BovineQueryBreedingChoice=new BovineQueryBreedingChoice($row['id']);
            $retArr = $BovineQueryBreedingChoice->nextBreedingAction($row['id'], true);
            $line = "{$row['id']},{$row['local_number']},{$row['full_reg_number']},{$retArr['row']['p1_name']},{$retArr['row']['p2_name']},{$retArr['row']['p3_name']},{$retArr['row']['s1_name']},{$retArr['row']['s2_name']},{$retArr['row']['s3_name']},{$retArr['nextActionTxt']}";
            $fileArray[] = $line;
        }



        $fp = fopen(self::CSVDIR . '/breedcho.csv', 'w');

        foreach ($fileArray as $line) {
            $this->fputcsvDBASE($fp, explode(',', $line));
        }

        fclose($fp);
    }

//from fputcsv docs.
    function fputcsvDBASE(&$handle, $fields = array(), $delimiter = ',', $enclosure = '"') {
        $str = '';
        $escape_char = '\\';
        foreach ($fields as $value) {
            if (strpos($value, $delimiter) !== false ||
                    strpos($value, $enclosure) !== false ||
                    strpos($value, "\n") !== false ||
                    strpos($value, "\r") !== false ||
                    strpos($value, "\t") !== false ||
                    strpos($value, ' ') !== false) {
                $str2 = $enclosure;
                $escaped = 0;
                $len = strlen($value);
                for ($i = 0; $i < $len; $i++) {
                    if ($value[$i] == $escape_char) {
                        $escaped = 1;
                    } else if (!$escaped && $value[$i] == $enclosure) {
                        $str2 .= $enclosure;
                    } else {
                        $escaped = 0;
                    }
                    $str2 .= $value[$i];
                }
                $str2 .= $enclosure;
                $str .= $str2 . $delimiter;
            } else {
                $str .= $value . $delimiter;
            }
        }
        $str = substr($str, 0, -1);
        $str .= "\r\n"; //needed for windows dBase, normally just /n
        return fwrite($handle, $str);
    }

}

//end class
?>