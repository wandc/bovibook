<?php 

//adding deleting users, etc.
require_once "HTML/QuickForm.php";



class PageSecurityAdmin extends BasePage {

   

    public function defaultDisplay() {
        $this->showPageSecurityLevels();
    }

    private function showPageSecurityLevels() {

        
        
        
        $sql = <<<SQL
SELECT page.pageid,page.title, (SELECT array(SELECT trim(page_security.group) FROM intwebsite.page_security WHERE page.pageid=pageid))
FROM intwebsite.page order by page.pageid ;
SQL;

        print((new JQueryDataTable)->startBasicSql($sql));   
       
    }

}

//end class
?>