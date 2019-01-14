<?php 
/* used with adminLTE to have a chat client.
 * 
 * 
 */
class directChat {

    
    function x() {
        $sql="SELECT chat_text.userid,text,create_time,firstname,lastname from misc.chat_text LEFT JOIN wcauthentication.users ON chat_text.userid=users.userid ORDER BY create_time";
$res = $GLOBALS['pdo']->query($sql);
while($row = $res->fetch(PDO::FETCH_ASSOC)) {
    
    $colour = substr(md5(hash("md5",$row['firstname'].' '.$row['lastname'])), 0, 6); //change colour for each user, but make it consistent through time.
    
    $str=array();
    if (trim($GLOBALS['auth']->getUsername()) == trim($row['userid'])) {
          
                $str[] = '<div class="direct-chat-msg right">';
                $str[] = ' <div class="direct-chat-info clearfix">';
                $str[] = '<span class="direct-chat-name pull-right">' . $row['firstname'] . ' ' . $row['lastname'] . '</span>';
                $str[] = ' <span class="direct-chat-timestamp pull-left">' . date('j M G:i', strtotime($row['create_time'])) . '</span>';
                $str[] = ' </div>';
                $str[] = ' <div class="avatar-square right" style="float:right; background-color: #' . $colour . '"><span class="initials">' . strtoupper(substr($row['firstname'], 0, 1) . substr($row['lastname'], 0, 1)) . '</span></div>';
                $str[] = '  <div class="direct-chat-text">' . $row['text'] . '</div>';
                $str[] = '</div>';
            }
        //another user
        else {
             
                $str[] = '<div class="direct-chat-msg">';
                $str[] = ' <div class="direct-chat-info clearfix">';
                $str[] = '<span class="direct-chat-name pull-left">' . $row['firstname'] . ' ' . $row['lastname'] . '</span>';
                $str[] = ' <span class="direct-chat-timestamp pull-right">' . date('j M G:i', strtotime($row['create_time'])) . '</span>';
                $str[] = ' </div>';
                $str[] = ' <div class="avatar-square" style="float:left; background-color: #' . $colour . '"><span class="initials">' . strtoupper(substr($row['firstname'], 0, 1) . substr($row['lastname'], 0, 1)) . '</span></div>';
                $str[] = '  <div class="direct-chat-text">' . $row['text'] . '</div>';
                $str[] = '</div>';
            
        }

      print( implode('',$str));
}
}
    
    
function example() {
    ?>



<div class="row">
            <div class="col-6 col-md-4">

   <!-- Construct the box with style you want. Here we are using box-danger -->
<!-- Then add the class direct-chat and choose the direct-chat-* contexual class -->
<!-- The contextual class should match the box, so we are using direct-chat-danger -->
<div class="box box-danger direct-chat direct-chat-danger">
  <div class="box-header with-border">
    <h3 class="box-title">Direct Chat</h3>
    <div class="box-tools pull-right">
      <span data-toggle="tooltip" title="3 New Messages" class="badge bg-red">3</span>
      <button class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i></button>
      <!-- In box-tools add this button if you intend to use the contacts pane -->
      <button class="btn btn-box-tool" data-toggle="tooltip" title="Contacts" data-widget="chat-pane-toggle"><i class="fa fa-comments"></i></button>
      <button class="btn btn-box-tool" data-widget="remove"><i class="fa fa-times"></i></button>
    </div>
  </div>
  <!-- /.box-header -->
  <div class="box-body">
    <!-- Conversations are loaded here -->
    <div class="direct-chat-messages">
      <!-- Message. Default to the left -->
      
          
          <?php $this->x();?>
          
    
    </div>
    <!--/.direct-chat-messages-->

    <!-- Contacts are loaded here -->
    <div class="direct-chat-contacts">
      <ul class="contacts-list">
        <li>
          <a href="#">
            <img class="contacts-list-img" src="framework/composer/vendor/almasaeed2010/adminlte/dist/img/user1-128x128.jpg" alt="Contact Avatar">
            <div class="contacts-list-info">
              <span class="contacts-list-name">
                Count Dracula
                <small class="contacts-list-date pull-right">2/28/2015</small>
                </span>
              <span class="contacts-list-msg">How have you been? I was...</span>
            </div>
            <!-- /.contacts-list-info -->
          </a>
        </li>
        <!-- End Contact Item -->
      </ul>
      <!-- /.contatcts-list -->
    </div>
    <!-- /.direct-chat-pane -->
  </div>
  <!-- /.box-body -->
  <div class="box-footer">
    <div class="input-group">
      <input type="text" name="message" placeholder="Type Message ..." class="form-control">
      <span class="input-group-btn">
                <button type="button" class="btn btn-danger btn-flat">Send</button>
                </span>
    </div>
  </div>
  <!-- /.box-footer-->
</div>
<!--/.direct-chat -->
</div></div>

    <?php
}

}//end class

?>
