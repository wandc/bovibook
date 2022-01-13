<?php

/** so far this is only used in bovinequery */
class CustomComment extends BasePage {

    public function defaultDisplay() {
        $this->newReproCommentQuickForm();
    }

    public function summaryDisplay() {
        //nothing
    }

    // displays info on potential offspring from cdn website.
    function displayCustomComment($mode = 'group', $bovine_id = null, $type = null) {


        if ($mode != 'group') {
            $sql = "SELECT id,type,bovine_id,comment,userid,event_time, date_trunc('days',now()-event_time) as days_ago FROM bovinemanagement.comment_custom WHERE type='$type' AND comment_custom.bovine_id=$bovine_id ORDER BY event_time DESC";
        } else {
            $sql = "SELECT id,type,bovine_id,comment,userid,event_time, date_trunc('days',now()-event_time) as days_ago FROM bovinemanagement.comment_custom WHERE type='$type' ORDER BY event_time DESC";
        }



        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        $resultsOut=array();
        //change presentation
        foreach ($results as &$row) {
            if ($mode == 'group') {
                $rowOut['Animal'] = JQueryDataTable::formattedAnimalNameLink($row['bovine_id'], $row['local_number'], $row['full_name']);
            }
            $rowOut['Time'] = JQueryDataTable::formattedDate($row['event_time']);
            $rowOut['Comment'] = $row['comment'];
            $rowOut['Employee'] = $row['userid'];
            
            $delButton=new QF2_DeleteButtonDecider('user_timeLimit',(object) array('staticWhereClauseSQL'=>"type='$type'",'dateTimePeriodDays' =>'1','schemaTable'=>'bovinemanagement.comment_custom'));
            $rowOut['Action'] =$delButton->output($row['id']);    
            $resultsOut[] = $rowOut;
        }

        //output
        $dataArr = $resultsOut;


        $out[] = ( (new JQueryDataTable)->startBasicArray('Breeding Trait Comments', $dataArr));
        return implode($out);
    }

    
   

    
}

//end class

Class QF2_CustomComment extends QF2_Base {

    function definition($form) {

        

        //for different types of comments, keep adding.
        switch ($this->param->type) {
             case 'reproductive_vet':
                $cannedComment = array(null => 'Please Select one...', 'Ok to breed.' => 'Ok to breed.', 'Cystic.' => 'Cystic.', 'Nice CL.' => 'Nice CL.', 'CL & Follicle.' => 'CL & Follicle..');
                $header = 'Repro Vet Comment';
                $showVetList=true;
                break;
            case 'medical_vet':
                $cannedComment = array(null => 'Please Select one...', 'Monitor' => 'Monitor.', 'Check necxt time.' => 'Check next time.');
                $header = 'Medical Vet Comment';
                $showVetList=true;
                break;
            case 'reproductive':
                $cannedComment = array(null => 'Please Select one...', 'Cloudy Vaginal Discharge.' => 'Cloudy Vaginal Discharge.', 'Thick Bloody Vaginal Disharge.' => 'Thick Bloody Vaginal Disharge.', 'Aborted.' => 'Aborted.');
                $header = 'Repro Comment';
                $showVetList=false;
                break;

            case 'trait':
                $cannedComment = null;
                $header = 'Trait Comment';
                $showVetList=false;
                break;

            case 'genotest':
                $cannedComment = null;
                $header = 'Genomic Test';
                $showVetList=false;
                break;

            default:
                throw new Exception("QF2 Custom Comment has no type. Aborting.<br />\n\r");
        }

       

        $form->addElement('hidden', 'pageid')->setValue($_REQUEST['pageid']);
        $form=(new Quickform2_GroupSelect($this->param->bovineID))->definitionAnimalAll($form,$this->param->aniNumber,$this->mode); 

        $dateArray = $GLOBALS['MiscObj']->createHourlyDatesForLastDay(); //hourly array, with unix timestamp as index.  


       if (empty($cannedComment) == false) {
            $comment1=$form->addElement('select', 'comment1', null, array('options' => $cannedComment, 'label' => 'Canned Comment'));
        } else {
            $comment1=$form->addElement('hidden', 'comment1', '')->setLabel("Generic Comment");
        }
        $comment2=$form->addElement('textarea', 'comment2', array('rows' => 6, 'col' => 40))->setLabel("Custom Comment");
        
        $comment2->addRule('nonempty','Canned or Custom Comment is Required.')->or_($comment1->createRule('nonempty')); //one or the other needs something enetered. 
        
        if ($showVetList==true) {
            
            //preg check and repro comment vets are same. Get a list of vets.
            $vets=(new EstrusPregnancyCheck)->listOfVetsForPregCheck();
            
            //find the last vet, so we can use them as a default. It's likely to be the same. 
        /*    
        $sql = "SELECT id FROM bovinemanagement.comment_custom WHERE update_time=(SELECT max(update_time) FROM bovinemanagement.comment_custom WHERE userid='$currentUserid' AND type='$type') AND userid='$currentUserid' AND type='$type'";
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC)
        */

         $form->addElement('select', 'vetUserid', null, array('options' => $vets, 'label' => 'Vet'))->addRule("required", "Please specify the vet who made comment.");

         //$form->setDefaults(array( 'vetUserid' => 'default foo value')); //this assumes no other defaults are set, which should be ok. 
         
        }    
        
        
        $form->addElement('select', 'dateEvent', null, array('options' => $dateArray, 'label' => 'Time of Event'))->addRule("required", "Please specify your event time");



        return $form; //required   
    }

    function process($value) {

      //  var_dump($value);
      //  exit();
        
        // Open a transaction
        try {
            $res = $GLOBALS['pdo']->beginTransaction();

            
             //loop over animals
          $bovineIdArr=(new Quickform2_GroupSelect)->processAnimalsGroupIndividiual($value['animalSelected']);
          foreach ($bovineIdArr as $key => $bovine_id) {

                
              
               $value['event_time'] = date('Y-m-d H:i:sO', $value['dateEvent']);

                //concatenate comments if they are both entered.
                if (strlen($value['comment1']) == 0) {
                    $comment = $value['comment2'];
                } else {
                    $comment = $value['comment1'] . ' ' . $value['comment2'];
                }
               $comment = pg_escape_string($comment);
              
              

               switch ($this->param->type) {
                    case "reproductive_vet":
                        $jsonArr['vet_userid'] = $value['vetUserid'];
                        $extra = "'" . json_encode($jsonArr) . "'";

                        break;
                    case "medical_vet":
                        $jsonArr['vet_userid'] = $value['vetUserid'];
                        $extra = "'" . json_encode($jsonArr) . "'";

                        break;

                    case "reproductive":
                        $extra = 'null';

                        break;
                    case "trait":
                        $extra = 'null';

                        break;
                    case "genotest":

                        $extra = 'null';
                        break;
                    default:
                        throw new EXCEPTION("ERROR: No type of custom comment selected or not selected correctly.");
                }

                $query = "INSERT INTO bovinemanagement.comment_custom (bovine_id,event_time,comment,userid,type,extra) VALUES ($bovine_id,'{$value['event_time']}','$comment','{$GLOBALS['auth']->getUsername()}','{$this->param->type}',$extra)";
                //now we insert all the animals that were checked.
               
                $res = $GLOBALS['pdo']->exec($query);
            }

            $GLOBALS['pdo']->commit();
        } catch (Exception $e) {
            $GLOBALS['pdo']->rollBack();
            echo "Failed: " . $e->getMessage();
            error_log($e->getMessage(), 0);
        }
        // Close the transaction
    }

}

//end class   