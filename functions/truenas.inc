<?php 
/* used to access rest api of truenas system to know if there are problems. 
 * 
 * 
 */
//run the export functions whenever this file is accessed.
if (defined('STDIN')) { //when called from cli, command line
    include_once('../global.php');
} else { //when called any other way
    include_once($_SERVER['DOCUMENT_ROOT'] . '/global.php');
}

class TrueNas {

    private $truenasIPAddr;
    private $token;
    
    public function __construct() {
        $this->truenasIPAddr=$GLOBALS['config']['TRUENAS']['IPADDR'];
        $this->token=$GLOBALS['config']['TRUENAS']['APIKEY'];
    }
    
    public function main() {
        $error=array();
 
        
         $result=$this->dataSetInfo();
         
         /*
          * check if too full
          */
         foreach ($result as $key => $value) {
                if   (($value->used->rawvalue / $value->available->rawvalue) >= 0.80) {
                     $returnMessageX=$value->name . ' is  over 80% full: '. $value->used->value . ' of '. $value->available->value .'.';
                     $error[] = new notifyObj(get_class($this), date('r',strtotime('now')), 1, $returnMessageX);
                }
         }
    
         /*
          * check if status OK.
          */
         $basicArr=$this->checkBasics();
         if ($basicArr['healthy'] != 1) {
             
           $returnMessage='TrueNase Status Error: '.$basicArr['status'] ;
                     $error[] = new notifyObj(get_class($this),date('r',strtotime('now')), 1, $returnMessage);  
         }
         
        var_dump($error);
         
       if (!empty($error)) {
            return array_unique($error);
        } else {
            return null;
        }
        
        
    }
    
    /*
     * removes partitions we dont care about. 
     */
    public function dataSetInfo() {
        
        $result=$this->jwt_request('pool/dataset','');
         $out=array();
         foreach ($result as $key => $value) {
             
             if ((str_contains($value->name, 'freenas-boot')) OR (str_contains($value->name, 'iocage'))) {
             //do nothing
             }
             else {
                 $out[]=$value;
             } 
         }
        return $out;
    }
    
    
    public function checkBasics() {        
         $result=$this->jwt_request('pool','');
        $out['status']=$result[0]->status;
        $out['healthy']=$result[0]->healthy;
        return $out;
    }
    
    

    
    private function jwt_request($resource,$post) {

       //NEEDS TO BE A GET REQUEST (maybe if only no post payload?)
        
       $ch = curl_init('http://'.$this->truenasIPAddr.'/api/v2.0/'.$resource); // Initialise cURL
       //curl_setopt($ch, CURLOPT_VERBOSE, true);
       
       $authorization = "Authorization: Bearer ". $this->token; // Prepare the authorisation token
       curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json' , $authorization )); // Inject the token into the header
       curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
       //curl_setopt($ch, CURLOPT_POST, 1); // Specify the request method as POST
       //$post = json_encode($post); // Encode the data array into a JSON string
       //curl_setopt($ch, CURLOPT_POSTFIELDS, $post); // Set the posted fields
       curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1); // This will follow any redirects
       $result = curl_exec($ch); // Execute the cURL statement
       if (curl_errno($ch)) {
            $error_msg = curl_error($ch);
        }
       curl_close($ch); // Close the cURL connection
       if (isset($error_msg)) {
            throw new Exception('Curl TruNas Error '.$error_msg);
        }   
       
       return json_decode($result); // Return the received data

    }
    
     //shows error.
    public function error() {

      
    }

}//end class

//when run from command line.
if (defined('STDIN')) {
$x=new TrueNas();
$x->main();
}