<?php

/* Bootstrap generate html class */
/* bovibook customizations bootstrap */

class BootStrap {

    public function errorNotify($message) {
        $str = '';

        $str = $str . ' <div class="alert alert-dismissible alert-danger">';
        $str = $str . '                 <button type="button" class="close" data-dismiss="alert">&times;</button>';
        $str = $str . "               <strong>$message</strong>";
        $str = $str . '</div>';
        return $str;
    }

    public function sucessNotify($message) {
        $str = '';

        $str = $str . ' <div class="alert alert-dismissible alert-success">';
        $str = $str . '                 <button type="button" class="close" data-dismiss="alert">&times;</button>';
        $str = $str . "               <strong>$message</strong>";
        $str = $str . '</div>';
        return $str;
    }

    public function warningNotify($message) {
        $str = '';

        $str = $str . <<<STR
 <div class="alert alert-warning alert-dismissible">
                <button type="button" class="close" data-dismiss="alert" aria-hidden="true">×</button>
                <h4><i class="icon fa fa-warning"></i> Alert!</h4>
              $message
              </div>
STR;
        return $str;
    }

    function warningSpan($message) {
        return " <span class='label label-warning'>$message</span> ";
    }

    function dangerSpan($message) {
        return " <span class='label label-danger'>$message</span> ";
    }

    function sucessSpan($message) {
        return " <span class='label label-success'>$message</span> ";
    }

    function infoSpan($message) {
    return "<span class='label label-info'>$message</span>";
}

    
    //used for home page STATIC
    public function bootstrapSmallBox($header, $content, $footer = null, $colour = 'green', $iconIon = null) {
        $footerStr = '';
        if ($footer != null) {

            $footerStr = <<<HTML
     <a href="https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid=$footer" class="small-box-footer">More info <i class="fa fa-arrow-circle-right"></i></a>
HTML;
        }
        if ($iconIon == null) {
            $iconIon = 'ion-stats-bars';
        }

        $html = <<<HTML
  <div class="small-box bg-$colour">
            <div class="inner">
              <h3>$header</h3>       
            </div>
               <div class="inner">         
                  $content
            </div>
            <div class="icon">
              <i class="ion $iconIon"></i>
            </div>
          $footerStr
          </div>
HTML;

        return $html;
    }

    //used for home page AJAX
    public function bootstrapSmallBoxAjax($header, $ajaxUrl, $footer = null, $colour = 'green', $iconIon = null) {
        $uuid = uniqid();
        $javascript = <<<EOT
<script type="text/javascript">
  $(document).ready(function() {
           (function bootstrapSmallBoxAjax_$uuid() {
  $.ajax({
    url: '$ajaxUrl', 
    dataType : "html",
    success: function(data) {
         var objData = JSON.parse(data);            
         /* erase content */
         $("#content_$uuid").html('');
                 
                 /* when empty green, when not empty yellow */
            if (jQuery.isEmptyObject(objData.data)) {
                
                 $("#small-box_$uuid").removeClass("bg-yellow").addClass("bg-green");
                 }
                 else 
                 {
                
                 $("#small-box_$uuid").removeClass("bg-green").addClass("bg-yellow");
                 }
                 
          /* loop though array  and concate each seperate json object into its own line*/
      $.each(objData.data, function( index, value ) {
                
                    $( "#content_$uuid").append( ''+ value.home_page +'</br>');
                 
 
});
    },
    complete: function() {
      // Schedule the next request when the current one's complete
      setTimeout(bootstrapSmallBoxAjax_$uuid, 30000);
    }
  });
})();
});
      
</script>
EOT;

        $footerStr = '';
        if ($footer != null) {

            $footerStr = <<<HTML
     <a href="https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid=$footer" class="small-box-footer">More info <i class="fa fa-arrow-circle-right"></i></a>
HTML;
        }
        if ($iconIon == null) {
            $iconIon = 'ion-stats-bars';
        }

        $html = <<<HTML
  <div id="small-box_$uuid" class="small-box bg-green">
            <div class="inner">
              <h3>$header</h3>       
            </div>
               <div class="inner">       
                <div id="content_$uuid">  </div>
                 
            </div>
            <div class="icon">
              <i class="ion $iconIon"></i>
            </div>
          $footerStr
          </div>
HTML;

        return $javascript . $html;
    }

    function plainBox($title, $body) {
        $html = <<<HTML
        
       <div class="box box-default box-solid">
            <div class="box-header with-border">
              <h3 class="box-title">$title</h3>

              <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa "></i>
                </button>
              </div>
              <!-- /.box-tools -->
            </div>
            <!-- /.box-header -->
            <div class="box-body">
             $body
            </div>
            <!-- /.box-body -->
          </div>
HTML;

        return $html;
    }

     function plainBoxCollapsed($title, $body) {
        $html = <<<HTML
        
       <div class="box box-default box-solid collapsed-box">
            <div class="box-header with-border">
              <h3 class="box-title">$title</h3>
              <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-plus"></i>
                </button>
              </div>
              <!-- /.box-tools -->
            </div>
            <!-- /.box-header -->
            <div class="box-body">
             $body
            </div>
            <!-- /.box-body -->
          </div>
HTML;

        return $html;
    }
}

//end class
?>