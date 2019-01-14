<?php 




class Documents extends BasePage {

   

    public function defaultDisplay() {
       print($this->displayeExternalPartsSites());
       print($this->displayeListOfDocuments());
       
    }

    function displayeExternalPartsSites() {
        $str='';
     $str=$str.("<h2><a target='_blank' href='http://ecatalog.kuhn.com/kuhn-pdr-3/index.asp?COLANDATA=3&COLAN=3&c=1'>Kuhn Parts (external site)</></h2>"."\n\r");
     $str=$str.("<h2><a target='_blank' href='http://www.kubota.ca/en/partssearch.aspx'>Kubota Parts (external site)</></h2>"."\n\r");
     $str=$str.("<h2><a target='_blank' href='http://jdpc.deere.com/jdpc/servlet/com.deere.u90490.partscatalog.view.servlets.HomePageServlet_Alt'>John Deere Parts (external site)</></h2>"."\n\r");
     $str=$str.("<hr/>");
     return $str;
    }
    
    public function displayeListOfDocuments($mode = 'default', $machine_id = null) {
        $str='';
        // Setup your database connection
        if ($mode == 'default') {
            if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
                $sql = "SELECT * FROM documents.document WHERE active=true Order by display_name ASC";
            } else {
                $sql = "SELECT * FROM documents.document WHERE active=true and document.management=false::boolean Order by display_name ASC";
            }
        }
        //for when queried by machine.
        else if ($mode == 'machine') {
            if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
                $sql = "SELECT * FROM documents.document WHERE active=true and machine_id=$machine_id Order by display_name ASC";
            } else {
                $sql = "SELECT * FROM documents.document WHERE active=true and machine_id=$machine_id AND document.management=false::boolean Order by display_name ASC";
            }
        }

        $res = $GLOBALS['pdo']->query($sql);

        if ($mode == 'default') {
            $str=$str.("<div class='documentList'>");
            $firstLeterPrev = null;
            while ($record = $res->fetch(PDO::FETCH_ASSOC)) {

                $firstLeterNow = substr($record['display_name'], 0, 1);
                if ($firstLeterNow != $firstLeterPrev) {
                    $str=$str.("<h3>$firstLeterNow</h3>");
                }
                $firstLeterPrev = $firstLeterNow;


                $number = "<a href=\"local/documents/{$record['file_name']}  \"> {$record['display_name']} </a><br/>";
                $str=$str.($number);
            }
            $str=$str.("</div>");
            //when it is machine just print a list.
        } elseif ($mode == 'machine') {
            while ($record = $res->fetch(PDO::FETCH_ASSOC)) {
                $number = "<a href=\"local/documents/{$record['file_name']}  \"> {$record['display_name']} </a><br/>";
                $str=$str.($number);
            }
        }
        return $str;
    }

}

//end class	