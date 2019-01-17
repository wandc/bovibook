<?php 
/**
 * Class for handling google mail related stuff for internal site 
 * 
 * 
 */

class GoogleMailSite {

//main mail anchor link
public function mailAnchorLink($mode = 'default') {
    return('<a  class="mailButton"  target="_blank" href="https://www.google.com/a/littleriver.ca/ServiceLogin?service=mail&passive=true&rm=false&continue=https://mail.google.com/a/littleriver.ca/"></a>'. "\n");  
}

//get mail count
  public function original() {
       $str='';
       $access_token=rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '=');  
       
      $str=$str.("<span id='googleMailNewCount'> <!--overwritten with AJAX --></span>"."\n\r");
       
$str=$str. <<<JS
                <script type="text/javascript" language="javascript">        
                    $(document).ready(function(){ 
                        $(function(){
          
function load(){
         $.ajax({
  url: "/api/html/10004/content",
   headers: {"Authorization": "Bearer $access_token) "}    
  }).success(function(data) {
     $('#googleMailNewCount').html(data).fadeIn("slow");
});
    }
    load();
    setTimeout(load,120000);//120 seconds
    });
                       
                    }); 
                 </script>
JS;
  }

  //get top 5 unread email messages in a tooltip bubble.
  public function originalNewMessages($access_token=null) {
     $str='';
         

         
         
$str=$str. <<<JS
        
      <script type="text/javascript" language="javascript">     
      
         //this grabs the mailButton a link because it is the only one in scope. 
     $('a.mailButton').each(function() {
                 $(this).popover({
                    content: {
                        text: function(event, api) {
                             $.ajax({
           url: "/api/html/10004/newmessagesummary",
           headers: {"Authorization": "Bearer $access_token"}    
          })
                            .then(function(content) {
                                // Set the tooltip content upon successful retrieval
                                api.set('content.text', content);
                            }, function(xhr, status, error) {
                                // Upon failure... set the tooltip content to error
                                api.set('content.text', status + ': ' + error);
                            });
                
                            return 'Loading...'; // Set some initial text
                        }
                    },
                    position: {
                        viewport: $(window)
                    },
                    style: 'qtip-wiki'
                 });
             });
                  
          
          
          
                    $(document).ready(function(){ 
                        $(function(){
      function load(){
         $.ajax({
  url: "/api/html/10004/content",
   headers: {"Authorization": "Bearer $access_token"}    
  }).success(function(data) {
     $('#googleMailNewCount').html(data).fadeIn("slow");
});
    }
    load();
    setTimeout(load,30000);//30 seconds
    });
                       
                    }); 
                 </script>
      
JS;
  }
  
  
  
  //ajax callback
    public function content() {

        /*
         *  This gets a count users current NEW emails. 
         * 
         *  This needs the token to be requested with a
         * scope of https://mail.google.com/ and then it will work (not verified, but pretty sure about this)
         * I think only works for google apps domains as well.
         */
        $feedStr = $this->getGmailFeed();
        if (isset($feedStr)) {
            $xmlobjc = new SimpleXMLElement($feedStr);
            $count = $xmlobjc->fullcount[0]; //get unread mail count
            $content = '' . '<b>' . "$count" . '</b>' . '';
            return $content;
        } else {
            return null;
        }
    }

    //ajax callback
    public function newmessagesummary() {

        /*
         *  This gets 5 latest messages. 
         * 
         *  This needs the token to be requested with a
         * scope of https://mail.google.com/ and then it will work (not verified, but pretty sure about this)
         * I think only works for google apps domains as well.
         */
        $feedStr = $this->getGmailFeed();
        if (isset($feedStr)) {
            $xmlobjc = new SimpleXMLElement($feedStr);
            $xx = $xmlobjc->entry[0]->title[0]; //first unread message title.
            $xx1 = $xmlobjc->entry[0]->author[0]->email[0];
            
            $yy = $xmlobjc->entry[1]->title[0]; //second....
            $yy1 = $xmlobjc->entry[1]->author[0]->email[0];
            
            $zz = $xmlobjc->entry[2]->title[0]; //first unread message title.
            $zz1 = $xmlobjc->entry[2]->author[0]->email[0];
            return '<h2>Recent Emails</h2>'.'<b>'.$xx1.'</b>'.' - '.$xx.'<br/><br/>'.'<b>'.$yy1.'</b>'.' - '.$yy.'<br/><br/>'.'<b>'.$zz1.'</b>'.' - '.$zz;
        } else {
            return null;
        }
    }

    //worker method to get actual gmail feed data.
    function getGmailFeed() {
        $feedStr = null;
        if (isset($_SESSION['access_token'])) {
            $token = json_decode($_SESSION['access_token']);
            $tok = $token->access_token;

            $header[] = 'GET  /gmail/feed/atom HTTP/1.1';
            $header[] = 'Host: mail.google.com';
            $header[] = 'Accept: */*';
            $header[] = 'Authorization: OAuth ' . $tok;

            $feed = curl_init('https://gmail.google.com/gmail/feed/atom');
            curl_setopt($feed, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
            curl_setopt($feed, CURLOPT_COOKIESESSION, true);
            curl_setopt($feed, CURLOPT_HTTPHEADER, $header);
            curl_setopt($feed, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($feed, CURLOPT_FOLLOWLOCATION, 1);
            curl_setopt($feed, CURLOPT_SSL_VERIFYPEER, 0);
            curl_setopt($feed, CURLOPT_SSL_VERIFYHOST, 2);
            curl_setopt($feed, CURLOPT_UNRESTRICTED_AUTH, 1);
            $feedStr = curl_exec($feed);
            curl_close($feed);
        }
        return $feedStr;
    }

}//end class
?>