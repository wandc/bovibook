<?php 

//adding deleting users, etc.
require_once "HTML/QuickForm.php";
require_once($_SERVER['DOCUMENT_ROOT'] . '/functions/dataGridHelper.php');
require_once($_SERVER['DOCUMENT_ROOT'] . '/template/tabPage.php');
//
;

class UserAdmin extends BasePage {

  
    public function defaultDisplay() {

      
               print('<br/>');
      
       
         print("<div class='row'>"."\n\r");
        
        print("<div class='col-sm-4 col-sm-push-8'>");
       if ((in_array('owner', $GLOBALS['auth']->getAuthData('groups')) == TRUE) || (in_array('admin', $GLOBALS['auth']->getAuthData('groups')) == TRUE)) {
            $this->addUserQuickForm('oauth2Local');
              $this->addUserToGroup();
        }
        print("</div>")."\n\r";
        print("<div class='col-sm-8 col-sm-pull-4'>"."\n\r");
   $this->displayUsersOauth2Local();
        print("</div>"."\n\r");
        print("</div>"."\n\r");
       
    }

  

    /**
     * As of 2014 this is suppose to be a good way to do this. 
     * Very simple, but very easy to screw this up.
     * Takes a plaintext password and returns a hashed password and the salt.
     */
    function HashPassword($password) {
        $salt = bin2hex(mcrypt_create_iv(MCRYPT_CAST_256, MCRYPT_DEV_URANDOM));
        $hashed_password = hash("sha256", $salt . $password); //hased pass
        $retArr['salt'] = $salt;
        $retArr['hashed_password'] = $hashed_password;
        return $retArr;
    }

    //takes hashed password and salt (stored somehwere (DB)) and compares to hased given password.
    function ValidatePassword($password, $salt, $true_hashed_password) {
        $reHash = hash("sha256", $salt . $password);
        return $reHash == $true_hashed_password; //true or false
    }

    function addUserToGroup() {

        //get list all users
        $res = $GLOBALS['pdo']->query("Select userid FROM wcauthentication.users ORDER BY userid ASC");
        $userArray[null] = "Please Select one..."; //dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $userArray[$row['userid']] = $row['userid'];
        }

        //get list all groups
        $res = $GLOBALS['pdo']->query("Select name FROM wcauthentication.groups ORDER BY name ASC");
        $groupArray[null] = "Please Select one..."; //dummy value
        while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
            $groupArray[$row['name']] = $row['name'];
        }


        $form = new HTML_QuickForm("addUserToGroupQuickform", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'hdraddUserToGroupQuickformt', 'Add User To Group');
        $form->addElement('select', 'user', 'User', $userArray);
        $form->addElement('select', 'group', 'Group', $groupArray);
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);

        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        //defaults
        $form->setDefaults(array('user' => 0));
        $form->setDefaults(array('group' => 0));

        //rules
        $form->addRule('user', 'User is required', 'required');
        $form->addRule('group', 'Group is required', 'required');

        // Try to validate a form
        if ($form->validate()) {

            //get values
            $user = trim($form->exportValue('user'));
            $group = trim($form->exportValue('group'));

            // do insert.
            $query = "INSERT INTO wcauthentication.users_in_groups (userid,groupid) VALUES ('$user','$group')";
     
            $res = $GLOBALS['pdo']->exec($query);




            //forward to same page, ob_start() is key to making this work!
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid={$_REQUEST['pageid']}");
            exit();
        } //end validation	
        else {
            $form->accept($renderer);
        echo $renderer->toHtml();
        }
    }

    function addUserQuickForm($auth = 'oauth2Local') {

        $form = new HTML_QuickForm("adduserQuickformtest", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();

        $form->addElement('header', 'adduserhdrQuickformtest', 'Add user');

        $form->addElement('text', 'username', 'Username:');
        $form->addElement('text', 'firstname', 'First Name:');
        $form->addElement('text', 'lastname', 'Last Name:');
        $form->addElement('text', 'email', 'Email:');
        $form->addElement('checkbox', 'active', 'Active Account?', null, null);
        $form->addElement('checkbox', 'onfarm', 'On Farm Employee Acccount', null, null);
        $form->addElement('password', 'txtPassword1', 'Password:');
        $form->addElement('password', 'txtPassword2', 'Password (again):');
        $form->addRule('txtPassword1', 'Password is required', 'required');
        $form->addRule(array('txtPassword1', 'txtPassword2'), 'The two passwords do not match. Please re-enter your password correctly.', 'compare');
        $form->addRule('txtPassword1', 'Please enter a valid "Password" containing between 8 and 20 characters, consisting of alphabets and numbers only.', 'regex', '/^[a-zA-Z0-9_@.\/#&+-]{8,20}$/');
        $form->addRule('username', 'Username is required', 'required');
        $form->addRule('firstname', 'First Name is required', 'required');
        $form->addRule('lastname', 'Last Name is required', 'required');
        $form->addRule('email', 'Valid email address is required', 'required');
        $form->addRule('email', 'Valid email address is required', 'email');

        function compare_field($element, $value, $arg) {
            global $form;
            if ($value == $form->getElementValue($arg)) {
                return true;
            } else {
                return false;
            }
        }

        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        //submit buttons
        $submitGroup[] = $form->createElement('reset', 'btnClear', 'Clear');
        $submitGroup[] = $form->createElement('submit', 'btnSubmit', 'Submit');
        $form->addGroup($submitGroup, 'submitGroup');

        if ($form->validate()) {

//get values
            $userid = $form->exportValue('username');
            $firstname = $form->getElementValue('firstname');
            $lastname = $form->getElementValue('lastname');
            $email = $form->getElementValue('email');
            $password = md5($form->exportValue('txtPassword1')); //convert to md5 one-way hash.
            $pageid = $form->exportValue('pageid');

            if ($auth == 'oauth2Local') {
                $this->userDBInsertOauth2Local($userid, $firstname, $lastname, $email, $password);
            } else {
                throw new Exception("Error: no DB inser performed, because no auth type selected.");
            }

            $host = $_SERVER['HTTP_HOST'];
            header("Location: https://$host{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        } //end validation

        $form->accept($renderer);
        echo $renderer->toHtml();
    }

   

    function userDBInsertOauth2Local($userid, $firstname, $lastname, $email, $password) {
        
        $userid=trim($userid); //needs sanitized.
        
        //TODO: salt the password to reduce rainbow attacks.
        //FIXME: apparently we have to write a wrapper class for the oauth2 
        //server hashing stuff. No native support.
        // $pArr=$this->HashPassword($password);
        //  $salt=$pArr['salt'] ;
        // $hashed_password=$pArr['hashed_password'];

        try {$res = $GLOBALS['pdo']->beginTransaction(); // Open a transaction
        //
            //TODO: check username is not in already
//don't put id value, uses next one in DB.
        
        $sql="SELECT user_id FROM wcauthentication.oauth_clients WHERE user_id='$userid'";
         $res = $GLOBALS['pdo']->query($sql);
        if ($res->rowCount()==1) {
           throw new Exception("Error: User Alread in db.$e"); 
        } 
         
        //insert into oauth local
        $query = "INSERT INTO wcauthentication.oauth_clients (client_id,client_secret,redirect_uri,scope,user_id) VALUES ('$userid','$password','null','null','null')";
        $res = $GLOBALS['pdo']->exec($query);
 
        //insert into site table
        $query2= "INSERT INTO wcauthentication.users (firstname,lastname,userid,email,active,onfarm)VALUES ('$firstname','$lastname','$userid','$email','true','true')";
        $res2 = $GLOBALS['pdo']->exec($query2);


        // determine if the commit or rollback
              $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                return false;
            }
        // Close the transaction   
            return true;
    }

    function displayUsersOauth2Local($mode = 'default') {

          $dt=new JQueryDataTable();
$columnsStr= <<<EX
           
             {
                 data: "active",
                 "visible": false,
                "searchable": false
            },   
              
            {
                data: "client_id",
                title: "User ID"
            },
            {
                data: "client_secret",
                title: "encrypted password"
            },
         //   {
         //       data: "redirect_uri",
         //       title: "redirect_uri"
         //   },
         //   {
         //       data: "grant_types",
         //       title: "grant_types"
         //   },
        //    {
         //       data: "scope",
         //       title: "scope"
         //   },
            {
                data: "firstname",
                title: "First Name"
            },
            {
                data: "lastname",
                title: "Last Name"
            },
            {
                data: "active",
                title: "active"
            },
            {
                data: "email",
                title: "Email"
            },
         {
                data: "groups",
                title: "Groups"
            },
            {
                data: "onfarm",
                title: "On Farm"
            },{
                data: "action",
                title: "Action"
            }
EX;
    print($dt->startLocal($this->usersOauth2Local(),$columnsStr,'highlight_attention',' "paging": false'));     
        
        
        
 
    }

   function usersOauth2Local() {
        // show users
        $sql = "
		Select 	client_id,	client_secret	,redirect_uri,	grant_types	,scope,	user_id, firstname,lastname,email,active,onfarm 
from wcauthentication.users
LEFT JOIN  wcauthentication.oauth_clients on client_id=userid
ORDER BY client_id
		";
        $statement = $GLOBALS['pdo']->prepare($sql);
        $statement->execute();
        //get data
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);

        //change presentation
        foreach ($results as &$row) {
      $row['action']= self::deleteQuickForm($row['client_id']);
        $row['colour_row']= ( $row['active']=='t' ? '1' : '0');
      $row['active']= ( $row['active']=='t' ? 'true' : '');
    
     $row['groups']= $this->printGroups($row['client_id']);
           
        }
        //output
        $output['data']=$results;
        return $output['data'];       
    }
    
    
   
   

    function printGroups($user_id) {
    
        $res = $GLOBALS['pdo']->query("Select groupid FROM wcauthentication.users_in_groups WHERE userid='$user_id' ORDER BY groupid ASC");
        $str = '';
        while (($row = $res->fetch(PDO::FETCH_ASSOC))) {
            $str = $str . ' ' . $row['groupid'];
        }
        return $str;
    }



    function deleteQuickForm($userid) {

//$estrusID=85;

        $form = new HTML_QuickForm("formDeleteButton", 'post', $_SERVER ["REQUEST_URI"], '', array('class' => 'quickformtableless'), true);  $renderer = new HTML_QuickForm_Renderer_Tableless();
        $form->addElement('hidden', 'pageid', $_REQUEST['pageid']);
        $form->addElement('hidden', 'userid', $userid);
        $form->addElement('submit', 'btnDelete', 'Delete', array('id' => 'deleteSubmit', 'onclick' => "return confirm('Are you sure you want to delete $userid? This cannot be undone.');")); //call javascript to confirm delete.


// Try to validate a form 
        if ($form->validate()) {

//get values
            $pageid = $form->exportValue('pageid');
            $userid = $form->getElementValue('userid');

//print("<br>DELETING $estrusID !!!!<br>");
 try {$res = $GLOBALS['pdo']->beginTransaction(); // Open a transaction
       
           
      
         print("USER:$userid");
         $res = $GLOBALS['pdo']->query("DELETE FROM wcauthentication.users where userid='$userid'");
         $res = $GLOBALS['pdo']->query("DELETE FROM wcauthentication.oauth_clients where client_id='$userid'");

        // determine if the commit or rollback
              $GLOBALS['pdo']->commit();
            } catch (Exception $e) {
                $GLOBALS['pdo']->rollBack();
                echo "Failed: " . $e->getMessage();
                return false;
            }
          


//print("DELETING succesful");
            header("Location: https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid=$pageid");
            exit();
        }
        return $form->toHtml(); //pass on as html string to display later in the table
    }

}

//end class
//extends datgrid class with specific logic to colour the line.
class DataGridLR_Ext_R extends DataGridLR {

    function colourLineCriteria($row) {
        $row['active'] = isset($row['active']) ? $row['active'] : '';
   
        if ($row['active'] == 't' AND $row['onfarm'] == 't') {
            return true;
        } else {
            return false;
        }
    }

}

?>
