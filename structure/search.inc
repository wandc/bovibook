<?php 

/*
 * when a search is run on the header bar of the web page. This shows the results 
 * and holds the logic.
 */

include_once($_SERVER['DOCUMENT_ROOT'] . 'global.php'); //need to include here for javascript refresh.

include_once($_SERVER['DOCUMENT_ROOT'] . '/template/basePage.php'); //for ajax usage

class Search Extends BasePage {

    const pageid = 122; //kind of hackish, change if the pages id ever changes.

   

    /** does not currently run from page */
    public function defaultDisplay() {
        
    }

    //probable need rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])) or something
    function searchForm() {
        $str = '';

        $access_token = rtrim(strtr(base64_encode(json_encode($_SESSION['access_token'])), '+/', '-_'), '='); //session is serverside only, js can't access directly. 

        $str = $str . <<<JS
        <span id='searchOverwrite'> <!--overwritten with AJAX --> </span>
        
                <script type="text/javascript" language="javascript">        
        $(document).ready(function () {
               $("#searchForm").submit(function (e) {
          
           e.preventDefault(); //prevent default form submit  
        searchValue=$("#searchBox").val();
        $.ajax({
   url: '/api/html/10003/content&searchItem='+encodeURIComponent(searchValue),
   headers: {"Authorization": "Bearer $access_token"},
   success: function(data){
         $('#searchOverwrite').html(data);},
   error: function(data, textStatus, jqXHR) {
   console.log(data.error);}
});   
        });
        });
                 </script>
JS;

        /*
         * javascript captures this and stops submit and calls ajax instead.
         */
        $str = $str . <<<HTML
   <!--      <form id='searchForm' >
             <input id='searchBox' type='search' maxlength='255' size='19' autocomplete="off" autocorrect="off" autocapitalize="off" autofocus="on" spellcheck="false" '> 
             <input id='searchBoxSubmit' type='submit' style='display: none;' >
        </form>
                -->
                 <form action="#"  id='searchForm' class="sidebar-form">
        <div class="input-group">
          <input type="text"  id='searchBox' maxlength='255' autocomplete="off" autocorrect="off" autocapitalize="off" autofocus="on" spellcheck="false"  name="q" class="form-control" placeholder="Search...">
              <span class="input-group-btn">
                <button type="submit" name="search" id="search-btn" class="btn btn-flat"><i class="fa fa-search"></i>
                </button>
              </span>
        </div>
      </form>
                
HTML;


        //focus on this
        $str = $str . "
        <script type='text/javascript'>
           document.getElementById('searchBox').focus(); 
         </script>
         ";

        return $str;
    }

    public function content() {
        $str = '';

        /*
         * form an array of field numbers, field names, cow names, cow numbers, page names and machinery names 
         * 
         */
        $sql = " SELECT row_number() over(),id,textid,type FROM system.search_view";
        $res = $GLOBALS['pdo']->query($sql);
        $allObjArr = $res->fetchAll(PDO::FETCH_OBJ);

        //create a sub array with just arbitrary index and possible text to search
        $toMatchArray = array();
        foreach ($allObjArr as $key => $Obj) {
            $toMatchArray[$key] = $Obj->textid;
        }

        //run search engine with input
        $inputStr = urldecode($_REQUEST['searchItem']);
        $retArr = self::searchEngine($inputStr, $toMatchArray);

        if ($retArr['percent'] <= 50) {   //nothing found
            //generate error string in form.
            // the error portion should probably fade away.
            $str = $str . "
        <script type='text/javascript'> 
          $('#searchBox').val($('#searchBox').val() + ' - Error nothing found!');
         </script>
                ";
        } else { //succesful match
            //use arbitrary index to find real id and type from orginal array.
            $id = $allObjArr[$retArr['key']]->id;
            $type = $allObjArr[$retArr['key']]->type;

            switch ($type) {
                case 'bovineid':
                    $pageid = 6;
                    $type = 'bovine_id';
                    break;
                case 'fieldid':
                    $pageid = 107;
                    $type = 'field_id';
                    break;
                case 'pageid':
                    $pageid = $id;
                    $type = 'null';
                    $id = 'null';
                    break;
                case 'machineid':
                    $pageid = 95;
                    $type = 'machine_id';
                    break;
                default:       //this should maybe be an error page. For now it is the home page.
                    $pageid = 1;
                    break;
            }

            //generate new page to go to
            $host = $_SERVER['HTTP_HOST'];
            $url = ("https://$host/index.php?pageid=$pageid&$type=$id");

            $str = $str . ('<script type="text/javascript">window.location.replace("' . $url . '");</script>'); //force reload via js of new page
        }
        return $str;
    }

    /* returns an array with percent sure and key value of array that matches */

    function searchEngine($inputStr, $searchArray) {
        //CODE based on php.net example for  levenshtein and I adapted for similiar text      
        // input misspelled word
        $input = strtolower($inputStr);

        // array of words to check against, make lower case first
        $words = array_map('strtolower', $searchArray);

        // no shortest distance found, yet
        $shortest = -1;

        // loop through words to find the closest
        foreach ($words as $key => $word) {

            // calculate the distance between the input word,
            // and the current word
            $lev = similar_text($input, $word, $sim);
            $lev = $sim;
            // check for an exact match
            if ($lev == 100) {

                // closest word is this one (exact match)
                $closest = $word;
                $shortest = 100;
                $keyest = $key;

                // break out of the loop; we've found an exact match
                break;
            }

            // if this distance is less than the next found shortest
            // distance, OR if a next shortest word has not yet been found
            if ($lev >= $shortest || $shortest < 0) {
                // set the closest match, and shortest distance
                $closest = $word;
                $shortest = $lev;
                $keyest = $key;
            }
        }


        //return what was found.      
        $retArr['match'] = $searchArray[$keyest];
        $retArr['percent'] = $shortest;
        $retArr['key'] = $keyest;

        return $retArr;
    }

}//end class