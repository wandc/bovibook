<?php 
/**
 * Pure javascript for telling logging out idle users. 
 * 
 * 
 */

class IdleTimer {

    //only run on specific computers., ie vet room.
 public static function main() {
//vet room
  if ($_SERVER['REMOTE_ADDR'] == '192.168.8.38') {
  self::original();
  }   
     
     
 }

//do idle counter
  public static function original() {
      //   <!-- from http://stackoverflow.com/questions/667555/detecting-idle-time-in-javascript-elegantly -->       
       ?>          
            <script type="text/javascript">
var idleTime = 0;
$(document).ready(function () {
    //Increment the idle time counter every minute.
    var idleInterval = setInterval(timerIncrement, 60000); // 1 minute

    //Zero the idle timer on mouse movement.
    $(this).mousemove(function (e) {
        idleTime = 0;
    });
    $(this).keypress(function (e) {
        idleTime = 0;
    });
});

function timerIncrement() {
    idleTime = idleTime + 1;
    if (idleTime > 14) { // 15 minutes
        window.location.href(location.protocol + '//' + location.host + location.pathname + '?reset=1'); //call logout url
        
    }
}
</script>   
        <?php 
  }

}

//end class
?>