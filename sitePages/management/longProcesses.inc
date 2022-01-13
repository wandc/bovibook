<?php

/**
 * Runs long running scripts using HTML SSE. 
 */
class LongProcesses extends BasePage {

    public function defaultDisplay() {
        print((new BootStrap)->infoNotify('Page must be left open for processes to continue running.'));
      
        print($this->showAllSSE());
    }

    //prints to the screen all the SSE long scripts the user has security access to use
    public function showAllSSE() {
        $userid = $GLOBALS['auth']->getUsername(); //grab userid
        $sql = ("Select distinct sse_id FROM intWebsite.sse_security WHERE sse_security.group = any(array(SELECT groupid FROM wcauthentication.users_in_groups WHERE userid='$userid')) ORDER BY sse_id");

        $res = $GLOBALS['pdo']->query($sql);
        $out[] = '<div class="row">';
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $out[] = '<div class="col-md-4">';
            $out[] = ($this->showSSEButton($row['sse_id']));
            $out[] = '</div>';
        }
        $out[] = '</div>';


        return implode($out);
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
        if (method_exists($controller, 'lastRun')) {
            return $controller->lastRun();
        } else {
            return null;
        }
    }

    //draw a single sse button based on provided sse_id.
    function showSSEButton($sse_id = null) {
        //get info in the individual sse.
        $sql = ("Select * FROM intWebsite.sse WHERE id=$sse_id");
        $res = $GLOBALS['pdo']->query($sql);
        $row = $res->fetch(PDO::FETCH_ASSOC);

        $name = ($row['name']);

        $js = <<<JS

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
JS;

        $out[] = ($js);

        //runs method to see last run time of each sse script 
        $lastRunTimeEpoch = $this->seeLastTimeRun($row);
        if (isset($lastRunTimeEpoch)) {
            $lastRunTime = date('M d, Y G:i', $lastRunTimeEpoch);
            $lastRunTimeOutput = ("Last Run Time: $lastRunTime");
        } else {
            $lastRunTimeOutput = '';
        }



        $html = <<<HTML

    <div class="container">    
        <div class="row">
            <div class="col-12"> 
                    <input class="btn-success" type="button" onclick="startTask_$sse_id();"  value="Start Long Task" />
                    <input class="btn-warning" type="button" onclick="stopTask_$sse_id();"  value="Stop Task" />
            </div>
            <div class="col-12"> 
                    <progress style="width:90%;" id='progressor_$sse_id' value="0" max='100' style=""></progress>  <span id="percentage_$sse_id" style="margin-left:0.5em;   line-height: 2;">0%</span>      
            </div>
        
            <div class="col-12"> 
                     <div id="results_$sse_id" style="border:1px solid #000; padding:2em; width:100%; height:2em; overflow:auto; background:#eee;"></div>
            </div>
        
             <div class="col-12"> 
                    $lastRunTimeOutput
             </div>
        
        
        </div>
    </div>   
       
       
      
HTML;
       
        $out[] = (new BootStrap)->plainCard($name, $html);
       


        return (implode($out));
    }

}
//end class
?>