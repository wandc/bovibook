<?php 
require_once('../global.php');
/**
* syncs with john deere.
*/

class JDLinkSync  {

   /* 
    //should never call this.
    function resetDBTable() {
        $sql="Truncate batch.jdLink";
        $res = $GLOBALS['pdo']->exec($sql);
    }
    
    * 
    */
    /**
     * main function that gets data from bovine quebec auctions
     * 
     */
    function main() {
        echo("<h1>Started processing JD Link webpages.</h1>\n");
        $dataArr = array();

       

            $pageContent = $this->downloadPageData();

            
            
           
               print($pageContent);

               exit();
               
               if (empty($pageContent)) {
                   throw new Exception('\nError: curl did not download data from bovine quebec. site or the Internet is down?\n\n');
               }
               
            $dom = new DOMDocument();
            libxml_use_internal_errors(true);
            $dom->loadHTML($pageContent);
            //
            libxml_clear_errors();
            $xpath = new DOMXPath($dom);

        
          /*
            //first do a check if the day had no data, if zero cull cows traded, then no action occured (Assumed)
            $valueArr = $xpath->query("//form[@id='form1']/table[@class='t_info']/tr[9]/td[2]");
            $checkData = $valueArr[0]->textContent;


            if ((is_numeric($checkData)) AND ( $checkData != 0)) {



                //loop through different lines in html page to get all data.
                //these numbers will change if bovine quebec changes the webpage
                foreach (array(44, 45, 46, 47, 48, 49, 50, 51) as $key => $lineNum) {
                    echo $lineNum;

                    //encapsulate type as array key for json later.
                    $temp=null;
                    $temp=$this->xPathToArray_Data($xpath, $lineNum, $date);                  
                    $dataArr[$temp['type']] = $this->xPathToArray_Data($xpath, $lineNum, $date);
                }//end foreach

                //cull cows are handled differently
                 foreach (array(5,6) as $key => $lineNum) {
                    echo $lineNum;

                    //encapsulate type as array key for json later.
                    $temp=null;
                    $temp=$this->xPathToArray_Data_Cull($xpath, $lineNum, $date);                  
                    $dataArr[$temp['type']] = $this->xPathToArray_Data_Cull($xpath, $lineNum, $date);
                }//end foreach
                
                
                 //hack here where to support legacy way, we need to name the 1100-1500 type as "Cull cattle", so just duplicate the "Dairy type 1100-1500 lb" when it shows up. Price Range is no long supported for culls by bovine quebec. 
                foreach ($dataArr as $key => $value) {

                    if ($key == "Dairy type  1100-1500 lb") {
                        //change type to Cull cattle and copy into array
                        $value['type'] = "Cull cattle";
                        $dataArr["Cull cattle"] = $value;
                    }
           * 
           */
                

            

                //write the data collected to db.
              //  $query = "INSERT INTO batch.jdLink (event_date,data) VALUES ('$date','$dataJson');";
              //  $res = $GLOBALS['pdo']->exec($query);

                //print_r($dataArr);
            
        }//end while
    

    /**
     *  The website seems to be missing a tbody tag that when you use chrome xpath , it inserts it. 
     *  The easiest way to find out whats wrongs is to just print out a leser path and narrow it down:
                $nodes = $xpath->query("//form[@id='form1']/table[@class='t_info']/tr[9]/td[2]");  //this catches all elements with itemprop attribute
                 foreach ($nodes as $node) {  print_r($node);} 
     * 
     * 
     */
    
    //grabs individual lines from bovine quebec page, ie Holstein good male calves (90-120 lb)	119	1,85 - 2,10	2,01
    function xPathToArray_Data($xpath, $lineNum,$date,$auction='all') {
        $answer = array();

        $valueArr = $xpath->query("//form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[1]");
        $answer['type'] = $valueArr[0]->textContent;
        //
        $valueArr = $xpath->query("//form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[2]");
        $answer['volume'] = $valueArr[0]->textContent;
        //
        $valueArr = $xpath->query("//form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[3]");
        $answer['price_range'] = trim(preg_replace('/,/', '.', $valueArr[0]->textContent)); //change to decimal
        //
        $valueArr = $xpath->query("//form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[4]");
        $answer['avg_price'] = trim(preg_replace('/,/', '.', $valueArr[0]->textContent)); //change to decimal
       
        //extras
         $answer['date'] =$date;       
         $answer['auction'] =$auction;
         

        return($answer);
    }
    
    //grabs individual lines from bovine quebec page for CULL COWS
    function xPathToArray_Data_Cull($xpath, $lineNum,$date,$auction='all') {
        $answer = array();

        $valueArr = $xpath->query("/html/body/div/div/div/div/div/div/div/form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[3]");
       
        $answer['type'] = "Dairy type " . $valueArr[0]->textContent;
        //
        $valueArr = $xpath->query("/html/body/div/div/div/div/div/div/div/form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[4]");
        $answer['volume'] = $valueArr[0]->textContent;
        //
        $valueArr = $xpath->query("/html/body/div/div/div/div/div/div/div/form[@id='form1']/table[@class='t_info']/tr[$lineNum]/td[5]");
        $answer['avg_price'] = trim(preg_replace('/,/', '.', $valueArr[0]->textContent)); //change to decimal
       
        //extras
         $answer['date'] =$date;       
         $answer['auction'] =$auction;
         
        //print_r($answer);
       
         
        return($answer);
    }
    
    

    function downloadPageData() {
        
    



 $url="https://myjohndeere.deere.com/mjd/my/login"; //login
 
   $ch = curl_init($url); //init
   
   
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch,CURLOPT_USERAGENT,'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.77 Safari/537.36'); //give a somewhat valid user agent string.
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30); 
    curl_setopt($ch,CURLOPT_TIMEOUT,30);
    curl_setopt($ch, CURLOPT_COOKIESESSION, true);
    
    
         curl_setopt($ch, CURLOPT_COOKIEFILE, "/tmp/cookie2.txt");
        curl_setopt($ch, CURLOPT_COOKIEJAR, "/tmp/cookie2.txt");
        curl_setopt($ch, CURLOPT_POST, 1);
        $postdata = array( 'userName1' => 'davidwaddy2',
            'password1' => 'Foolfool67', 
            "Sign In"=>"Sign In",
            "form_id" => "loginFormValueProp",

        );
        curl_setopt($ch, CURLOPT_POSTFIELDS, $postdata);
      
    
 
 $pageData= curl_exec($ch);  

 
 //show information regarding the request
print_r(curl_getinfo($ch));
echo curl_errno($ch) . '-' .  curl_error($ch);

//close the connection
//curl_close($ch); //NO


//

/*
 * 2nc request after logging in. 
 */

curl_setopt($ch, CURLOPT_URL, 'https://machines.deere.com/app/map/?machineId=553915&modalView=general');
curl_setopt($ch, CURLOPT_POST, false);
curl_setopt($ch, CURLOPT_POSTFIELDS, "");
$answer = curl_exec($ch);
if (curl_error($ch)) {
    echo curl_error($ch);
}
print("Answer:".$answer.":Answer DOne.");
 

    return $pageData;
    }
    
  
    
    
}//end class



//only run class if being called via url with reg.

    $xx=new JDLinkSync();
$xx->main();

?>