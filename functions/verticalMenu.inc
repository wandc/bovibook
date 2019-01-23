<?php

// code to add a vertical menu down the side of the page.
//D Waddy March 15 2009


class VerticalMenu {

    //used with adminlte
    static function createMenuList() {

        $sql = "
           With temp as (
with recursive rel_tree as (
   select pageid, title::text, parent_id,filesubdir::text,rank,filename,auto_refresh, 1 as level, array[pageid] as path_info
   from intwebsite.page 
   where parent_id = 0 AND active = true
   union all
   select c.pageid, rpad('', p.level * 2, ' ') || c.title::text, c.parent_id,c.filesubdir::text,c.rank,c.filename,c.auto_refresh, p.level + 1, p.path_info||c.pageid
   from intwebsite.page c
     join rel_tree p on c.parent_id = p.pageid  where c.active = true
)
select pageid, title,filesubdir,filename,rank,auto_refresh,level, path_info,REPLACE(path_info::text,',' || pageid::text,'') || '.' || rank::text as path_info2
from rel_tree

)
Select distinct(temp.pageid), temp.*,userid,temp.path_info[2] as two,temp.path_info[3] as three
from temp
 JOIN intwebsite.page_security ON temp.pageid=page_security.pageid
 JOIN wcauthentication.users_in_groups ON page_security.group=users_in_groups.groupid
WHERE users_in_groups.userid = '{$GLOBALS['auth']->getUsername()}' AND temp.pageid !=1
order by two,rank NULLS FIRST
       ";
        $res = $GLOBALS['pdo']->query($sql);
        $dataArr = $res->fetchAll(PDO::FETCH_ASSOC);

        //before we start, we need to find the active pages parent level, so when it comes up we can open it, this is for level 2.
        $key = array_search($_REQUEST['pageid'], array_column($dataArr, 'three'));
        $activeLevel2 = $dataArr[$key]['two'];

        $str = '';

        //assume it is in order
        $prevLevel = 1;
        $level1_n = 1;
        foreach ($dataArr as $key => $vArr) {

            $vArr['title'] = trim($vArr['title']);


            if (($prevLevel < $vArr['level']) && ($vArr['level'] == 3)) {
                $str = $str . ("\t\t<ul class='treeview-menu'>\n\n");
            }

            //closing brackets
            if ($prevLevel > $vArr['level']) {
                $str = $str . ("\t\t</ul>\n\n");
            }
            if ($prevLevel > $vArr['level']) {
                $str = $str . ("\t\t</li>\n\n");
            }




            //only $str=$str.  a page id for the first level if it actually exists (has a file associated with it)
            if (($vArr['filename'] != null) && ($vArr['level'] == 1)) {
                //home page do nothing
            } elseif (($vArr['filename'] == '') && ($vArr['level'] == 2)) {
                ($activeLevel2 == $vArr['two']) ? $active2 = 'active menu-open' : $active2 = '';  //using array search logic above, see if we are in the active level 2.  

                $str = $str . ("\t\t<li class='treeview $active2'><a href='#' > <i class='fa " . (VerticalMenu::iconChooser($vArr['title'])) . "'></i> <span>{$vArr['title']}</span>
            <span class='pull-right-container'>
              <i class='fa fa-angle-left pull-right'></i>
            </span>  
            </a>\n");
                $level1_n++;
            } elseif (($vArr['level'] == 3)) {
                ($_REQUEST['pageid'] == $vArr['pageid']) ? $active3 = 'active' : $active3 = '';      //see if we are the active page and if so add class to list to highlight it.       

                $str = $str . ("\t\t\t<li class='$active3'><a href=\"/index.php?pageid={$vArr['pageid']}\"><i class='fa fa-circle-o'></i>{$vArr['title']}</a></li>\n");
            } else {
                throw new Exception("Error Improper menu level, not 1,2,or 3.");
            }

            $prevLevel = $vArr['level'];
        }

        return $str;
    }

    /**
      get an array that list all the valid pageids this particular user can see.
     */
    public function getListofAllowedPages() {
        $sql = "SELECT DISTINCT ON (page.pageid) page.pageid,title FROM intwebsite.page
LEFT JOIN intwebsite.page_security ON page.pageid=page_security.pageid
LEFT JOIN wcauthentication.users_in_groups ON page_security.group=users_in_groups.groupid
WHERE page.active=true AND users_in_groups.userid = '{$GLOBALS['auth']->getUsername()}'";
        $res = $GLOBALS['pdo']->query($sql);
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $allowedArray[] = $row['pageid'];
        }
        return $allowedArray;
    }

    /*
     * chooses a bootstrap icon class, based on header name.
     * this is kind of hackish, but keeps display stuff out of DB.
     */

    private static function iconChooser($name) {
        switch ($name) {
            case 'Reproduction':
                $class = 'fa-venus-mars';
                break;
            case 'Movement':
                $class = 'fa-truck';
                break;
            case 'Transition':
                $class = 'fa-rotate-right';
                break;
            case 'Medical':
                $class = 'fa-medkit';
                break;
            case 'Heifers':
                $class = 'fa-link'; //baby??
                break;
            case 'Cropping':
                $class = 'fa-map';
                break;
            case 'Misc':
                $class = 'fa-question-circle';
                break;
            case 'HR':
                $class = ' fa-user';
                break;
            case 'Reports':
                $class = 'fa-file-image-o';
                break;
            case 'Nutrition':
                $class = 'fa-cutlery';
                break;
            case 'Machinery':
                $class = 'fa-bus';
                break;
            default:
                $class = 'fa-link';
        }
        return $class;
    }

}

//end class    
?>