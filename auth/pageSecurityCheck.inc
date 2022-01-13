<?php 

//Group of functions to determine if user can see a page, used by controller currently.

class PageSecurityCheck {

//when true allow user to see page.
    function canUserSeePage($pageid, $userid) {
        $userid=trim($userid);
//search arrays to see if there is an item found in both.
        $pageGroupsArray = self::getSecurityGroupsAllowedToSeePage($pageid);
        $authUserGroupsArray = self::getGroupsAuthUserIsIn($userid);
        
        $intersectCount = count(array_intersect($pageGroupsArray, $authUserGroupsArray));
        if ($intersectCount >= 1) {
            return true;
        } else {
            return false;
        }
    }

    function isUserInAnyGroups($userid) {
        $authUserGroupsArray = self::getGroupsAuthUserIsIn($userid);
        if (count($authUserGroupsArray) == 0) {
            return false;
        } else {
            return true;
        }
    }

    function getSecurityGroupsAllowedToSeePage($pageid) {
//find information about page clicked
        $res = $GLOBALS['pdo']->query("Select trim(page_security.group) as group FROM intwebsite.page LEFT JOIN intwebsite.page_security on page.pageid=page_security.pageid WHERE page.pageid=$pageid");

        $count = 0;
        $pageGroupsArray=array();
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {

//create pageGroups Array.
            $pageGroupsArray[$count] = $row['group'];
            $count++;
        }
        return $pageGroupsArray;
    }

    function getGroupsAuthUserIsIn($userid) {
        $authUserGroupsArray = array();
//now only display the page if the users security profile allows them to see it.
//load all the users group.
        $res2 = $GLOBALS['pdo']->query("SELECT groupid FROM wcauthentication.users_in_groups WHERE userid='$userid'");

        $count1 = 0;
        while (($row2 = $res2->fetch(PDO::FETCH_ASSOC))) {
            $authUserGroupsArray[$count1] = $row2['groupid'];
            $count1++;
        }
        return $authUserGroupsArray;
    }

    /*
     * immedicate forward to login page, used if ajax detects someone not logged in (session expired).
     */
    function forwardToLoginPage() {
         header("Location: {$GLOBALS['config']['HTTP']['URL']}?resetauth=1");
         exit();
    }
    
    
}

//end class
?>