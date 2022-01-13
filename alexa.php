<?php

require_once("global.php");
/* this is used to handle elexa skill requests. there should be some authentication here to know alexa is really asking and not anyone. but at the same time do we really care? it's read only. */
/* based off of https://welaunch.io/en/2018/05/create-an-alexa-skill-with-php-tutorial/ */

class AlexaHandler {

    function main() {


// Get Alexa Request
        $jsonRequest = file_get_contents('php://input');

// Decode the Request
        $data = json_decode($jsonRequest, true);

// Abort when Empty
        if (empty($data) || (!isset($data) )) {
            header('HTTP/1.0 400 Bad Request', true, 400);
            die('Bad Request');
        }


        $obj = new stdClass();

        /* get the important things from the JSON */
        $obj->intent = !empty($data['request']['intent']['name']) ? $data['request']['intent']['name'] : 'default';
        $obj->intentData = !empty($data['request']['intent']['slots']) ? $data['request']['intent']['slots'] : 'default';
        $obj->sessionId = !empty($data['session']['sessionId']) ? $data['session']['sessionId'] : 'default';
        $obj->data = $data;

        /* all the intents for the skill */
        switch ($obj->intent) {
            case 'hello':
                $responseArray = $this->intent_hello($obj);
                break;
            //who is in to breed
             case 'whoIsToBreed':
                $responseArray = $this->intent_whoIsToBreed($obj);
                break;
            //who is in sick group
             case 'whoInSickGroup':
                $responseArray = $this->intent_whoInSickGroup($obj);
                break;
// whatGroup
            case 'whatGroup':
                $responseArray = $this->intent_whatGroup($obj);
                break;
            
            case 'whatDIM':
                $responseArray = $this->intent_whatDIM($obj);
                break;
// Default
            default:
                $responseArray = $this->intent_default($obj);
                break;
        }


        /* send response */
        header('Content-Type: application/json');
        echo json_encode($responseArray);
        die();


        /*
          $logger->info( var_export($data, true));
          $logger->info( $intentData );
          $logger->info( $sessionId);
         */
    }

    
    //all breedings today/./change to 12 hours for just am.
    function intent_whoIsToBreed($obj) {
       
            $res = $GLOBALS['pdo']->query("SELECT local_number from bovinemanagement.breeding_event LEFT JOIN bovinemanagement.bovine ON bovine_id=bovine.id WHERE actual_breeding_userid is null AND 	estimated_breeding_time <@ tsrange( current_date , current_date + interval '24 hours', '[)' )");
            $out=array();
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $out[]=$row['local_number'].' ';       
            }
            $animalList=implode($out);
            
            if ($res->rowCount() > 0) {
                 $str= "$animalList need bred today.";                
            }
            else{
                 $str='No animals are currently marked to breed today.';
            }
     
            $responseArray = ['version' => '1.0',
                'response' => [
                    'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => $str,
                        'ssml' => null
                    ],
                    'shouldEndSession' => false
                ]
            ];
       

        return $responseArray;
    }
    
     function intent_whoInSickGroup($obj) {
       
            $res = $GLOBALS['pdo']->query("SELECT local_number FROM bovinemanagement.bovinecurr WHERE location_id=39");
            $out=array();
            while($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $out[]=$row['local_number'].' ';       
            }
            $animalList=implode($out);
            
             if (!$row) {
                   $str= "$animalList are currently in the sick group.";
               
             } else {
                 $str='No animals are currently in the sick group.';
             }
            
            
            
            $responseArray = ['version' => '1.0',
                'response' => [
                    'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => $str,
                        'ssml' => null
                    ],
                    'shouldEndSession' => false
                ]
            ];
       

        return $responseArray;
    }
    
    
    function intent_whatGroup($obj) {
        /* get cow number slot, FIXME, check if slot exists, parse JSON better */
        $cowNumber = !empty(filter_var($obj->data['request']['intent']['slots']['cow_number']['value'], FILTER_SANITIZE_NUMBER_INT)) ? $obj->data['request']['intent']['slots']['cow_number']['value'] : 'null';
        //print("cowNumber is" . $cowNumber);
        /* first always check cow number is valid.  */
        $res0 = $GLOBALS['pdo']->query("Select id FROM bovinemanagement.bovinecurr WHERE local_number=$cowNumber LIMIT 1");
        $row0 = $res0->fetch(PDO::FETCH_ASSOC);

//invalid    
        if (!$row0) {

            $responseArray = ['version' => '1.0',
                'response' => [
                    'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => "Animal number $cowNumber is not valid. Please try again.",
                        'ssml' => null
                    ],
                    'shouldEndSession' => false
                ]
            ];
        }
//valid
        else {
            $res = $GLOBALS['pdo']->query("Select location_name FROM bovinemanagement.bovinecurr WHERE local_number=$cowNumber LIMIT 1");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            $responseArray = ['version' => '1.0',
                'response' => [
                    'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => "The current group for #$cowNumber is {$row['location_name']}.",
                        'ssml' => null
                    ],
                    'shouldEndSession' => false
                ]
            ];
        }

        return $responseArray;
    }
    
    function intent_whatDIM($obj) {
        /* get cow number slot, FIXME, check if slot exists, parse JSON better */
        $cowNumber = !empty(filter_var($obj->data['request']['intent']['slots']['cow_number']['value'], FILTER_SANITIZE_NUMBER_INT)) ? $obj->data['request']['intent']['slots']['cow_number']['value'] : 'null';
        //print("cowNumber is" . $cowNumber);
        /* first always check cow number is valid.  */
        $res0 = $GLOBALS['pdo']->query("Select id FROM bovinemanagement.bovinecurr WHERE local_number=$cowNumber LIMIT 1");
        $row0 = $res0->fetch(PDO::FETCH_ASSOC);

//invalid    
        if (!$row0) {

            $responseArray = ['version' => '1.0',
                'response' => [
                    'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => "Animal number $cowNumber is not valid. Please try again.",
                        'ssml' => null
                    ],
                    'shouldEndSession' => false
                ]
            ];
        }
//valid
        else {
            $res = $GLOBALS['pdo']->query("Select bovinemanagement.dim((SELECT id FROM bovinemanagement.bovinecurr WHERE local_number=$cowNumber LIMIT 1 ))");
            $row = $res->fetch(PDO::FETCH_ASSOC);
            if (empty($row['dim'])) {
                $str="No days in milk for Animal #$cowNumber.";
            }else {
                $str="{$row['dim']} in milk for #$cowNumber.";
            }
            
            
            $responseArray = ['version' => '1.0',
                'response' => [
                    'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => $str,
                        'ssml' => null
                    ],
                    'shouldEndSession' => false
                ]
            ];
        }

        return $responseArray;
    }
    

    function intent_default($obj) {
        $responseArray = ['version' => '1.0',
            'response' => [
                'outputSpeech' => [
                    'type' => 'PlainText',
                    'text' => 'Hello. I can help you with things like animals groups and heats. Intent is ' . $obj->intent,
                    'ssml' => null
                ],
                'shouldEndSession' => false
            ]
        ];
        return $responseArray;
    }

    function intent_hello($obj) {

// hello intent
        $name = !empty($obj->intentData['name']['value']) ? $obj->intentData['name']['value'] : '';

// save $name in data file - here also mysql can be used
// later you can use session id to get data like name
        if (!empty($name)) {
            $dataToSave = array($sessionId => array(
                    'name' => $name
            ));
            $fp = fopen('data.json', 'w');
            fwrite($fp, json_encode($dataToSave));
            fclose($fp);
        }

        $responseArray = ['version' => '1.0',
            'response' => [
                'outputSpeech' => [
                    'type' => 'PlainText',
                    'text' => 'Hello W&C Employee. Name is' . $name,
                    'ssml' => null
                ],
                'shouldEndSession' => false
            ]
        ];

        return $responseArray;
    }

}

//end class

(new AlexaHandler)->main(); //run it