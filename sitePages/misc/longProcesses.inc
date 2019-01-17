<?php 
/**
 * Runs long running scripts using HTML SSE. 
 */

class LongProcesses extends BasePage {


    public function defaultDisplay() {
         print("<h3>Note: Page must be left open for processes to continue running</h3>");   
         $this->showAllSSE();
    }
    
    //prints to the screen all the SSE long scripts the user has security access to use
    public function showAllSSE() {
          $userid = $GLOBALS['auth']->getUsername();//grab userid
          $sql=("Select distinct sse_id FROM intWebsite.sse_security WHERE sse_security.group = any(array(SELECT groupid FROM wcauthentication.users_in_groups WHERE userid='$userid'))");  
   $res = $GLOBALS['pdo']->query($sql);  
while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
$this->showSSEButton($row['sse_id']);
}
         
    }
    
  
    //returns the epoch time of the last time DB was updated for script.
    function seeLastTimeRun($row) {

        //find file path
        $returnArray['filePath'] = '/' . $row['filesubdir'] . '/' . $row['filename'];

//object name from file name
        $objName = $row['filename'];
        $objName = str_ireplace('.php', '', $objName);
        $objName = str_ireplace('.inc', '', $objName);
        $returnArray['objectName'] = $objName;

        //load 
        require_once( $_SERVER['DOCUMENT_ROOT'] . $returnArray['filePath']);
        $controller = new $returnArray['objectName'];
        //if method exists, then run it.
        if (method_exists($controller , 'lastRun')) {
           return $controller->lastRun();
        } else {
            return null;
        }
    }
     
     

    //draw a single sse button based on provided sse_id.
    function showSSEButton($sse_id=null) {
        //get info in the individual sse.
        $sql=("Select * FROM intWebsite.sse WHERE id=$sse_id");  
        $res = $GLOBALS['pdo']->query($sql);  
        $row = $res->fetch(PDO::FETCH_ASSOC);
        
       $name=($row['name']);

$a=<<<JS

                <script type="text/javascript">                  
        var es_$sse_id;

        function startTask_$sse_id() {
        es_$sse_id = new EventSource('sse.php?sseid=$sse_id');

        //a message is received
        es_$sse_id.addEventListener('message', function(e) {
        var result = JSON.parse( e.data );

        addLog_$sse_id(result.message);

        if(e.lastEventId === 'CLOSE') {
            addLog_$sse_id('Received CLOSE closing');
            es_$sse_id.close();
            var pBar = document.getElementById('progressor_$sse_id');
                        pBar.value = pBar.max; //max out the progress bar
                        var perc = document.getElementById('percentage_$sse_id');
                        perc.innerHTML = "100%"; //always assume CLOSE means 100%
                    }
                    else {
                        var pBar = document.getElementById('progressor_$sse_id');
                        pBar.value = result.progress;
                        var perc = document.getElementById('percentage_$sse_id');
                        perc.innerHTML = result.progress + "%";
                        perc.style.width = (Math.floor(pBar.clientWidth * (result.progress / 100)) + 15) + 'px';
                    }
                });

                es_$sse_id.addEventListener('error', function(e) {
                    addLog_$sse_id('Error occurred.');
                    es_$sse_id.close();
                });
            }

            function stopTask_$sse_id() {
                es_$sse_id.close();
                addLog_$sse_id('Interrupted');
            }

            function addLog_$sse_id(message) {
                var r = document.getElementById('results_$sse_id');
                r.innerHTML += message + '<br>';
                r.scrollTop = r.scrollHeight;
            }
        </script>

       
        <input type="button" onclick="startTask_$sse_id();"  value="Start Long Task" />
        <input type="button" onclick="stopTask_$sse_id();"  value="Stop Task" />
        <progress id='progressor_$sse_id' value="0" max='100' style=""></progress>
        <span id="percentage_$sse_id" style="margin-left:5px;   line-height: 2;">0%</span>      
    
        <div id="results_$sse_id" style="display:inline-flex; border:1px solid #000; padding:30px; width:300px; height:60px; overflow:auto; background:#eee;"></div>
         <span class="long_progress_name">$name</span>
       
JS;
print($a);

        //runs method to see last run time of each sse script 
        $lastRunTimeEpoch = $this->seeLastTimeRun($row);
        if (isset($lastRunTimeEpoch)) {
            $lastRunTime = date('M d, Y G:i', $lastRunTimeEpoch);
            print("<p>Last Run Time: $lastRunTime</p><hr>");
        }

        print('<br/>');
    }

}
//end class

?>