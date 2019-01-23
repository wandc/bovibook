<?php 
//show errors.
error_reporting(E_ERROR | E_PARSE);
if (defined('STDIN')) { //when called from cli, command line define constant.
    $_SERVER['DOCUMENT_ROOT']=dirname(__DIR__).'/';
    include_once( $_SERVER['DOCUMENT_ROOT'].'global.php');
}
else { //when called any other way
print("Can't be called from the web.<br>");
exit();
}



class ErrorNotifyCron {

    protected $basePath;
    
    public function __construct() {
       $this->basePath='/var/www/int/';
    }

    
    public function __destruct() {
        //nothing
    }

    public function main() {
	print("Starting Error Processing..."."\n\r");
        require_once($this->basePath.'template/basePage.php');
    
        //load the class search path
        $this->setClassPath();

        //now go though every class within sitepages and attempt to load it.
        //so we go to the sitepages directory and look for all files with a .inc extension.
        $retval = exec("ls {$this->basePath}sitePages -R | grep .inc | grep -v .inc~", $output);

        $errorArray = array(); //create blank array to store results.

        /* MAIN LOOP */
   
        foreach ($output as $key => $value) {
             
            unset($x); //class object null.
           
            //now take array and remove the ".inc" from each file name.
            $class_name = str_replace('.inc', '', $value);
             
            //try and make the class inqiue somehow.
            $n=$GLOBALS['MiscObj']->uuid_create();
 
            require_once(lcfirst($class_name) . '.inc');
                   
            eval('$x= new ' . $class_name . '();'); //load class up with eval


            $retArray = null;
            $retArray = $x->error(); //execute error function within the pages class.


            if (!empty($retArray)) {

              //do some simple checks to see if it is a valid notifyObj.
                if (is_array($retArray) == true) {
                    foreach ($retArray as $key => $value) {
                        if (get_class($value) != 'notifyObj') {
                            throw new Exception('Not a notifyObj!');
                        }
                        $rr[] = null;
                        $errorArray[] = $value;
                        
                    } //end foreach
                } //end if
                else {
                    throw new Exception('notifyObj not in array format!');
                }
            } //end count if.
        } //end foreach
        ///
        $this->insertDB($errorArray);
        print("Finished Succesfully."."\n\r");
    }

    /**
     * goes through array of notifyObj's and returns each as an array
     * it then inserts these as lines into the db.
     * @param type $errorObjArray 
     */
    public function insertDB($errorObjArray) {
         $hashArray = array();
         $currentHashArr=array();
        //first off select all the hashes currenty in the table.
        $res2 = $GLOBALS['pdo']->query("SELECT id,hash FROM batch.error_curr");
        while ($row2 = $res2->fetch(PDO::FETCH_ASSOC)) {
            $hashArray[$row2['id']] = $row2['hash'];
        }
        
        try {$res = $GLOBALS['pdo']->beginTransaction();
        foreach ($errorObjArray as $key => $value) {
            $tArr = null;
            $tArr = $value->get_array();
           
            if (($tArr['callingClass'] != null) AND ($tArr['time'] != null) AND ($tArr['level'] != null) AND ($tArr['text'] != null)) {
                $tArr['text'] = pg_escape_string($tArr['text']);
                $tArr['time'] = date('r', strtotime($tArr['time']));
                $hash=null;
                $hash = md5($value->__toString()); //create a has to store in db, so we can later see if it is the same.
                $currentHashArr[]=$hash;
                
                //check to see if hash is already in the db table, if it is, just update the update_time
                if (array_search($hash, $hashArray) == true) {
                    $query = "UPDATE batch.error_curr SET hash='$hash' WHERE hash='hash'"; //kind of stupid, but just need a way to change update_time.           
                    $res = $GLOBALS['pdo']->exec($query);
                } else {   //otherwise do an insert of the new data
                    $query = "INSERT INTO batch.error_curr (calling_class,event_time,level,detail,hash) VALUES ('{$tArr['callingClass']}','{$tArr['time']}',{$tArr['level']},'{$tArr['text']}','$hash')";     
                    $res = $GLOBALS['pdo']->exec($query);
                }
            }
        }
          
        //now that we are all done, delete all the values with a different update time, ie not current.
        $hashDiffArr=array_diff($hashArray,$currentHashArr);
        
        /*
        print_r2($hashArray);
        print_r2($currentHashArr);
        print_r2($hashDiffArr);
         */
        
        foreach($hashDiffArr as $key2=>$value2) {
        $query = "DELETE FROM batch.error_curr WHERE hash='$value2'"; //delete any non current hashes.
        $res = $GLOBALS['pdo']->exec($query);       
        }
        
        //make the whole thing atomic.     
     $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
            }
        
    }
    
    

    //sets the search path to everything in the sitePages dir.
    public function setClassPath() {
        
        //include template directory seperately
        $prevIncludePath = get_include_path();
      
        //first off remove current path, phpcronscripts. causes problems.
        $this->remove_include_path($this->basePath);
        
        $retval = exec("ls -d {$this->basePath}sitePages/*/", $output);
        
        foreach ($output as $key => $value) {
            //go though each dir name and set it.
            $prevIncludePath = get_include_path();
            set_include_path($prevIncludePath . ":" .  $value);
        }
    }

    //this function is automatically called when a class can't be found.
    //FIXME: IS NOT automatically called???? does not look in class?
    public function __autoload($class_name) {
        require_once(lcfirst($class_name) . '.inc');
    }
    
    //from: http://php.net/manual/en/function.set-include-path.php
    protected function remove_include_path ($path)
{
    foreach (func_get_args() AS $path)
    {
        $paths = explode(PATH_SEPARATOR, get_include_path());
        
        if (($k = array_search($path, $paths)) !== false)
            unset($paths[$k]);
        else
            continue;
        
        if (!count($paths))
        {
            trigger_error("Include path '{$path}' can not be removed because it is the only", E_USER_NOTICE);
            continue;
        }
        
        set_include_path(implode(PATH_SEPARATOR, $paths));
    }
}
    

}//end class

//rund code
//only run when called direectly from stdin
if ((defined('STDIN')) AND (!empty($argc) && strstr($argv[0], basename(__FILE__)))) {
  
try {
        $x = new ErrorNotifyCron();
        $x->main();
    } catch (Exception $e) {
        echo $e->getMessage();
        echo $e->getTraceAsString();
        print("\n\r");
    }
}


?>
