<?php

/* Bootstrap generate html class */
/* bovibook customizations bootstrap */

class BootStrap {
    
    public function errorNotify($message) {
       

       $out[]= ' <div class="alert alert-dismissible alert-danger">';
       $out[]= '                 <button type="button" class="close" data-dismiss="alert">&times;</button>';
       $out[]= "               <strong>$message</strong>";
       $out[]= '</div>';
        return implode($out);
    }

    public function sucessNotify($message) {
        

       $out[]= ' <div class="alert alert-dismissible alert-success">';
       $out[]= '                 <button type="button" class="close" data-dismiss="alert">&times;</button>';
       $out[]= "               <strong>$message</strong>";
       $out[]= '</div>';
        return implode($out);
    }
    
     public function infoNotify($message) {
       

       $out[]= ' <div class="alert alert-dismissible alert-info">';
       $out[]= '                 <button type="button" class="close" data-dismiss="alert">&times;</button>';
       $out[]= "               <strong>$message</strong>";
       $out[]= '</div>';
          return implode($out);
    }

    public function warningNotify($message) {


       $out[]= ' <div class="alert alert-dismissible alert-warning">';
       $out[]= '                 <button type="button" class="close" data-dismiss="alert">&times;</button>';
       $out[]= "               <strong>$message</strong>";
       $out[]= '</div>';
          return implode($out);
    }

    public function dangerRibbon($message) {
        
       $out = <<<HTML
          <div class="ribbon-wrapper ribbon-lg">
                        <div class="ribbon bg-danger text-lg">
                          $message
                        </div>
                      </div>
       HTML; 
       return $out; 
    }
    
    /*
     * ALERTS
     */
    
    public function dangerAlert($title,$message) {
        
        $out = <<<HTML
                
        <div class="alert alert-danger">
                  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">×</button>
                  <h5><i class="icon fas fa-ban"></i> $title</h5>
                 $message
                </div>
       HTML; 
       return $out;  
    }
    
    public function warningAlert($title,$message) {
        
        $out = <<<HTML
                
        <div class="alert alert-warning">
                  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">×</button>
                  <h5><i class="icon fas fa-exclamation-triangle"></i> $title</h5>
                 $message
                </div>
       HTML; 
       return $out;  
    }
    
    public function infoAlert($title,$message) {
        
        $out = <<<HTML
                
        <div class="alert alert-info">
                  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">×</button>
                  <h5><i class="icon fas fa-info"></i> $title</h5>
                 $message
                </div>
       HTML; 
       return $out;  
    }
    
    public function successAlert($title,$message) {
        
        $out = <<<HTML
                
        <div class="alert alert-success">
                  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">×</button>
                  <h5><i class="icon fas fa-check"></i> $title</h5>
                 $message
                </div>
       HTML; 
       return $out;  
    }
    
    /*
     * Basic Alert
     */
    
    
    
     public function successAlertBasic($message) {      
      return '<div class="alert alert-success block">'.$message.'</div>';  
     } 
     public function infoAlertBasic($message) {      
      return '<div class="alert alert-info block">'.$message.'</div>';  
     } 
      public function warningAlertBasic($message) {      
      return '<div class="alert alert-warning block">'.$message.'</div>';  
     } 
      public function dangerAlertBasic($message) {      
      return '<div class="alert alert-danger block">'.$message.'</div>';  
     } 
    
     /* Small Badges */
     public function successBadge($message) {
       return '<small class="badge badge-success">'.$message.'</small>';
     }
     public function infoBadge($message) {
       return '<small class="badge badge-info">'.$message.'</small>';
     }
     public function warningBadge($message) {
       return '<small class="badge badge-warning">'.$message.'</small>';
     }
     public function dangerBadge($message) {
       return '<small class="badge badge-danger">'.$message.'</small>';
     }
     
    //spans
      public function successSpan($message) {
       return '<span class=" text-success">'.$message.'</span>';
     }
     public function infoSpan($message) {
       return '<span class=" text-info">'.$message.'</span>';
     }
     public function warningSpan($message) {
       return '<span class=" text-warning">'.$message.'</span>';
     }
     public function dangerSpan($message) {
       return '<span class=" text-danger">'.$message.'</span>';
     }
     public function mutedSpan($message) {
       return '<span class=" text-muted">'.$message.'</span>';
     }
     public function primarySpan($message) {
       return '<span class=" text-primary">'.$message.'</span>';
     }
     
     
     
     
     
    /*
     * CALLOUTS
     */
    
    function warningCallout($message) {
         $out = <<<HTML
              <div class="callout callout-warning">
                  <p>$message</p>
            </div>
   HTML;
              
    return $out;
    }

    function dangerCallout($message) {
         $out = <<<HTML
              <div class="callout callout-danger">
                  <p>$message</p>
            </div>
   HTML;
              
    return $out;
    }

    function sucessCallout($message) {
        $out = <<<HTML
              <div class="callout callout-sucess">
                  <p>$message</p>
            </div>
   HTML;
              
    return $out;
    }

    function infoCallout($message) {
      $out = <<<HTML
              <div class="callout callout-info">
                  <p>$message</p>
            </div>
   HTML;
              
    return $out;
}

/*
 * APPLICATION BUTTON
 */

function applicationButtonWarning($message,$icon) {
      $out = <<<HTML
              <a class="btn btn-app bg-warning">
                  <span class="badge bg-info">12</span>
                  <i class="fas fa-envelope"></i> $message
              </a>
   HTML;
              
    return $out;
}


 public function fourthfourthfourthfourth($left,$centreLeft,$centreRight,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='col-sm-3 '>
            {$left}
        </div> <!-- End 1/4 Row --> 
        <div class='col-sm-3 '>
            {$centreLeft}
        </div> <!-- End 2/4 Row -->     
             <div class='col-sm-3 '> 
            {$centreRight}
        </div> <!-- End 3/4 Row --> 
        <div class='col-sm-3 '> 
            {$right}
        </div> <!-- End 4/4 Row --> 
    </div>   <!-- End Fourths Row --> 
HTML; 
            
            return $html;
      
    }


    /*
     * used for main display pages to show 1/3 or 2/3. 
     * 
     */
    public function oneThirdTwoThirds($left,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='majorCol col-sm-8 col-sm-push-4'>
            {$left}
        </div> <!-- End 2/3 Row --> 
        <div class='d-print-none minorCol col-sm-4 col-sm-pull-8'> 
            {$right}
        </div> <!-- End 1/3 Row --> 
    </div>   <!-- End 1/3 / 2/3 Row --> 
HTML; 
            
            return $html;
      
    }
    
    public function twoThirdsOneThird($left,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='d-print-none minorCol col-sm-4 col-sm-pull-8'> 
            {$left}
        </div> <!-- End 1/3 Row --> 
        <div class='majorCol col-sm-8 col-sm-push-4'>           
            {$right}
        </div> <!-- End 2/3 Row --> 
    </div>   <!-- End 2/3 / 1/3 Row --> 
HTML; 
            
            return $html;
      
    }
    
    
    /*
     * used for main display pages to show 1/3 or 2/3. 
     * 
     */
    public function thirdThirdThird($left,$centre,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='col-sm-4 '>
            {$left}
        </div> <!-- End 1/3 Row --> 
        <div class='col-sm-4 '>
            {$centre}
        </div> <!-- End 2/3 Row -->     
        <div class='col-sm-4 '> 
            {$right}
        </div> <!-- End 3/3 Row --> 
    </div>   <!-- End Thirds Row --> 
HTML; 
            
            return $html;
      
    }
    
    
     public function thirdSmallThird($left,$centre,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='col-sm-5 '>
            {$left}
        </div> <!-- End 1/3 Row --> 
        <div class='col-sm-3 '>
            {$centre}
        </div> <!-- End 2/3 (Small) Row -->     
        <div class='col-sm-4 '> 
            {$right}
        </div> <!-- End 3/3 Row --> 
    </div>   <!-- End Thirds Row --> 
HTML; 
            
            return $html;
      
    }
    
    
    
     public function halfHalf($left,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='col-md-6'>
            {$left}
        </div> <!-- End Left Half Row --> 
        <div class='col-md-6'> 
            {$right}
        </div> <!-- End Right Half Row --> 
    </div>    <!-- End Half / Half Row --> 
HTML; 
            
            return $html;
      
    }
    
    
    public function oneFifthFourFifth($left,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='col-md-2'>
            {$left}
        </div> <!-- End One Fifth Row --> 
        <div class='col-md-10'> 
            {$right}
        </div> <!-- End Four Fifth Row --> 
    </div>    <!-- End One Fifth / Four Fifth Row --> 
HTML; 
            
            return $html;
      
    }
    
     
    public function fourFifthOneFifth($left,$right) {
        
            $html = <<<HTML
                    
                    
    <div class='row'>
        <div class='col-md-10'>
            {$left}
        </div> <!-- End Four Fifth Row --> 
        <div class='col-md-2'> 
            {$right}
        </div> <!-- End One Fifth Row --> 
    </div>    <!-- End Four Fifth / One Fifth Row --> 
HTML; 
            
            return $html;
      
    }


    //used for home page STATIC
    public function bootstrapSmallBox($header, $content, $footer = null, $colour = null, $iconIon = null) {
        $footerStr = '';
        if ($footer != null) {

            $footerStr = <<<HTML
     <a href="https://{$_SERVER['HTTP_HOST']}{$_SERVER['PHP_SELF']}?pageid=$footer" class="small-box-footer">More info <i class="fa fa-arrow-circle-right"></i></a>
HTML;
        }
        if ($iconIon == null) {
            $iconIon = 'ion-stats-bars';
        }

        $iconIon=strpos( $iconIon , 'ion-') ? 'ion '.$iconIon : 'fa '.$iconIon; //to support both fa and ion
        
        /*
         * if colour is not specified change it based on content. 
         */
        if ((empty($content)) && (empty($colour))) {
            $colour = 'green';
        } elseif (!empty($colour)) {
            $colour = $colour;
        } else {
            $colour = 'yellow';
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
              <i class="$iconIon"></i>
            </div>
          $footerStr
          </div>
HTML;

        return $html;
    }

    
    //used for home page AJAX
    /*
     * normally if the ajax returns nothing the box will be green and yellow otherwise.
     * if you don't want this functinailty use the $searchStringToColour and give a string to search for in the AJAX that says everything is ok, if it doesn't find that string, things are bad and box goes yellow. 
     * $searchStringToColour = "'All Good'"
     */
    public function bootstrapSmallBoxAjax($header, $ajaxUrl, $footer = null, $searchStringToColour = null, $iconIon = null) {
       
        //$searchStringToColour='foo';
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
                 

                var whatColour='$searchStringToColour';
                //when whatcolour is set we override the default functionality of just looking for any string and colouring it yellow.
                //instead we look for the string and if it is found then we colour it green, ie today's date.
                //check its set to something
            if (whatColour){

                //string found
                if(JSON.stringify(objData).indexOf(whatColour) != -1){
                 $("#small-box_$uuid").removeClass("bg-yellow").addClass("bg-green");
                }
                //string not found  
                else {
                $("#small-box_$uuid").removeClass("bg-green").addClass("bg-yellow");
                }
                
            }
            else {
                
                 /* when empty green, when not empty yellow */
            if (jQuery.isEmptyObject(objData.data)) {
                
                 $("#small-box_$uuid").removeClass("bg-yellow").addClass("bg-green");
                 }
                 else 
                 {
                
                 $("#small-box_$uuid").removeClass("bg-green").addClass("bg-yellow");
                 }
            }     
                
          /* loop though array  and concate each seperate json object into its own line*/
      $.each(objData.data, function( index, value ) {
                
                    $( "#content_$uuid").append( ''+ value.home_page +'</br>');
                 
                if (value.importance==1) {   $("#small-box_$uuid").removeClass("bg-yellow").addClass("bg-red");}
                
 
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
        $iconIon= strpos( $iconIon , 'ion-') ? 'ion '.$iconIon : 'fa '.$iconIon; //to support both fa and ion
         
        $html = <<<HTML
  <div id="small-box_$uuid" class="small-box bg-green">
            <div class="inner">
              <h3>$header</h3>       
            </div>
               <div class="inner">       
                <div id="content_$uuid">  </div>
                 
            </div>
            <div class="icon">
              <i class="$iconIon"></i>
            </div>
          $footerStr
          </div>
HTML;

        return $javascript . $html;
    }
    
    
    //used for home page AJAX
    //to send raw HTML ajax. 
    public function bootstrapRawHtmlAjax($ajaxUrl) {
        $content='';
        $uuid = uniqid();
        $javascript = <<<EOT
<script type="text/javascript">  
    
                
               
                
                
                
  $(document).ready(function() {
             
           (function bootstrapRawHtmlAjax_$uuid() {
                 
  $.ajax({
    url: '$ajaxUrl', 
    dataType : "html",
    success: function(data) {
                console.log("Hi");
                  $("#content_$uuid").html('');
                  $("#content_$uuid").append(data);
              
    },
    complete: function() {
      // Schedule the next request when the current one's complete
      setTimeout(bootstrapRawHtmlAjax_$uuid, 30000);
    }
  });
});
     }); 
      
</script>
EOT;

        
        $html = <<<HTML
 
                <div id="content_$uuid"> $content </div>           
HTML;

        return $javascript . $html;
    }

    
    //used on calf feeder page, etc.
    function animalBox($class='calfBoxAttentionSolidYellow',$bovineId,$local_number,$content) {
        
           $out[]= '<div class="calfBox ' . $class . '">';
           $out[]= ('<h3>' . '<img class=""    src="/images/cowHeadFront.svg"><a href="' . $GLOBALS['MiscObj']->hrefToBovinePage($bovineId) . '" >' . $local_number . '</a></h3>');
           $out[]=$content;
           $out[]= ('</div>');
        
        return implode($out);
    }
    
    
    function plainCardPrimarySolid($title, $body) {
        $html = <<<HTML
        
       <div class="card card-primary">
            <div class="card-header with-border">
              <h3 class="card-title">$title</h3>

              <div class="card-tools pull-right">
                <button type="button" class="btn btn-card-tool" data-widget="collapse"><i class="fa "></i>
                </button>
              </div>
              <!-- /.card-tools -->
            </div>
            <!-- /.card-header -->
            <div class="card-body">
             $body
            </div>
            <!-- /.card-body -->
          </div>
HTML;

        return $html;
    }
    
    function plainCardPrimarySolidAll($title, $body) {
        $html = <<<HTML
        
       <div class="card bg-primary">
            <div class="card-header with-border">
              <h3 class="card-title">$title</h3>

              <div class="card-tools pull-right">
                <button type="button" class="btn btn-card-tool" data-widget="collapse"><i class="fa "></i>
                </button>
              </div>
              <!-- /.card-tools -->
            </div>
            <!-- /.card-header -->
            <div class="card-body">
             $body
            </div>
            <!-- /.card-body -->
          </div>
HTML;

        return $html;
    }
    
    function plainCardSecondarySolidAll($title, $body) {
        $html = <<<HTML
        
       <div class="card bg-secondary">
            <div class="card-header with-border">
              <h3 class="card-title">$title</h3>

              <div class="card-tools pull-right">
                <button type="button" class="btn btn-card-tool" data-widget="collapse"><i class="fa "></i>
                </button>
              </div>
              <!-- /.card-tools -->
            </div>
            <!-- /.card-header -->
            <div class="card-body">
             $body
            </div>
            <!-- /.card-body -->
          </div>
HTML;

        return $html;
    }
    
    function plainCardWarningSolidAll($title, $body) {
        $html = <<<HTML
        
       <div class="card bg-warning">
            <div class="card-header with-border">
              <h3 class="card-title">$title</h3>

              <div class="card-tools pull-right">
                <button type="button" class="btn btn-card-tool" data-widget="collapse"><i class="fa "></i>
                </button>
              </div>
              <!-- /.card-tools -->
            </div>
            <!-- /.card-header -->
            <div class="card-body">
             $body
            </div>
            <!-- /.card-body -->
          </div>
HTML;

        return $html;
    }
    
    
    
    function plainCard($title, $body) {
        $html = <<<HTML
        
       <div class="card card-outline card-primary">
            <div class="card-header with-border">
              <h3 class="card-title">$title</h3>

              <div class="card-tools pull-right">
                <button type="button" class="btn btn-card-tool" data-widget="collapse"><i class="fa "></i>
                </button>
              </div>
              <!-- /.card-tools -->
            </div>
            <!-- /.card-header -->
            <div class="card-body">
             $body
            </div>
            <!-- /.card-body -->
          </div>
HTML;

        return $html;
    }

     function plainCardCollapsed($title, $body) {
        $html = <<<HTML
        
        <div class="card card-primary collapsed-card">
              <div class="card-header">
                <h3 class="card-title">$title</h3>

                <div class="card-tools">
                  <button type="button" class="btn btn-tool" data-card-widget="collapse"><i class="fas fa-plus"></i>
                  </button>
                </div>
                <!-- /.card-tools -->
              </div>
              <!-- /.card-header -->
              <div class="card-body" style="display: none;">
                $body
              </div>
              <!-- /.card-body -->
            </div>        
                
             
HTML;

        return $html;
    }
    
    public function bootstrapModalAjax($title, $ajaxUrl, $icon=null) {

        $uuid = uniqid();

        if (empty($icon)) {
            $icon='fas fa-chart-bar';
        }
        
        
     // <button data-widget="modal"  class="btn  btn-modal"  href="{$ajaxUrl}" data-target="#myModal_{$uuid}">  <i class="$icon"></i> </button>
$html = <<<HTML
                
   
     
        
               <button type="button" class="btn btn-primary" data-toggle="modal" data-target="#myModal_{$uuid}">  <i class="$icon"></i>    </button>

               
               
                <div class="modal fade" id="myModal_{$uuid}" aria-hidden="true" style="display: none;">
        <div class="modal-dialog modal-xl">
          <div class="modal-content bg-primary">
            <div class="modal-header">
              <h4 class="modal-title">{$title}</h4>
              <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                <span aria-hidden="true">×</span></button>
            </div>
            <div class="modal-body modal-modify-background">
             <!-- replaced by AJAX -->
            </div>
            <div class="modal-footer justify-content-between">
              <button type="button" class="btn btn-outline-light" data-dismiss="modal">Close</button>
            </div>
          </div>
          <!-- /.modal-content -->
        </div>
        <!-- /.modal-dialog -->
      </div>        
               
           <script>
$(document).ready(function(){
    $("#myModal_{$uuid}").on("show.bs.modal", function(event){
        // Place the returned HTML into the selected element
        $(this).find(".modal-body").load("{$ajaxUrl}");
        
    });
        $('#myModal_{$uuid}').modal('handleUpdate');
});
</script>    
                

                
HTML;
        return $html;
    }

    //call like  Bootsrap::generalInfoBox('ion-ios-eye-outline','bg-info','Watch For Heats',$content);
     public function generalInfoBox($iconIon, $colour, $title, $content) {
         
        $iconIon=strpos( $iconIon , 'ion-') ? 'ion '.$iconIon : 'fa '.$iconIon; //to support both fa and ion

        $html = <<<HTML
<div class="info-box">
              <span class="info-box-icon $colour"><i class="$iconIon"></i></span>

              <div class="info-box-content">
                <span class="info-box-text">$title</span>
                <span class="info-box-number">$content</span>
              </div>
              <!-- /.info-box-content -->
            </div>
        
HTML;

        
       
        
        
        return $html;
    }
    
    
    //array keys are headers, keys need to the be the same
  /*  $arr = array( array("title"=>"rose", "price"=>1.25 , "number"=>15),
               array("title"=>"daisy", "price"=>0.75 , "number"=>25),
               array("title"=>"orchid", "price"=>1.15 , "number"=>7) 
             ); 
   */
    //from: https://stackoverflow.com/questions/4746079/how-to-create-a-html-table-from-a-php-array
    public function simpleTable($arr) {
        if (!empty($arr[0])) {
            $tbody = array_reduce($arr, function($a, $b) {
                return $a .= "<tr><td>" . implode("</td><td>", $b) . "</td></tr>";
            });
            $thead = "<tr><th>" . implode("</th><th>", array_keys($arr[0])) . "</th></tr>";
            return "<table>\n$thead\n$tbody\n</table>";
        } else {
            return null;
        }
    }

    /*
     * there seems to be no simple way to do this without a whole framework.
     */
    //from:  https://bootsnipp.com/snippets/Q0d6E
    function flipSwitchOnOff($onOff='') {
        if ($onOff===1)
        {
            $checked='checked';
        }else{
            $checked='';
        }
        
        $style = <<<STYLE
<style>
    .ques {
    color: darkslateblue;
}
.flipOnOffSwitch {
  position: relative;
  display: inline-block;
  width: 260px;
  height: 100px;
}

.flipOnOffSwitch input {display:none;}

.slider {
  position: absolute;
  cursor: pointer;
  overflow: hidden;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #f2f2f2;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  z-index: 2;
  content: "";
  height: 96px;
  width: 96px;
  left: 2px;
  bottom: 2px;
  background-color: darkslategrey;
      -webkit-box-shadow: 0 2px 5px rgba(0, 0, 0, 0.22);
    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.22);
  -webkit-transition: .4s;
  transition: all 0.4s ease-in-out;
}
.slider:after {
  position: absolute;
  left: 0;
  z-index: 1;
  content: "ON";
    font-size: 45px;
    text-align: left !important;
    line-height: 95px;
  padding-left: 0;
    width: 260px;
    color: #fff;
    height: 100px;
    border-radius: 100px;
    background-color: #ff6418;
    -webkit-transform: translateX(-160px);
    -ms-transform: translateX(-160px);
    transform: translateX(-160px);
    transition: all 0.4s ease-in-out;
}

input:checked + .slider:after {
  -webkit-transform: translateX(0px);
  -ms-transform: translateX(0px);
  transform: translateX(0px);
  /*width: 235px;*/
  padding-left: 25px;
}

input:checked + .slider:before {
  background-color: #fff;
}

input:checked + .slider:before {
  -webkit-transform: translateX(160px);
  -ms-transform: translateX(160px);
  transform: translateX(160px);
}

/* Rounded sliders */
.slider.round {
  border-radius: 100px;
}

.slider.round:before {
  border-radius: 50%;
}
.absolute-no {
	position: absolute;
	left: 0;
	color: darkslategrey;
	text-align: right !important;
    font-size: 45px;
    width: calc(100% - 25px);
    height: 100px;
    line-height: 95px;
    cursor: pointer;
}
</style>        
STYLE;
        
    $html = <<<HTML
<label class="flipOnOffSwitch">
  <input type="checkbox" $checked>
  <span class="slider round"></span>
  <span class="absolute-no">OFF</span>
</label>
HTML;
    
       return  $style . $html;
    }
    
 /*
  * array holds carousel item string content. 
  */
 public function carousel(string $title,array $contentArr) {
     
 
  $html = <<<HTML
    <div class="card">
              <div class="card-header">
                <h3 class="card-title">Carousel</h3>
              </div>
              <!-- /.card-header -->
              <div class="card-body">
                <div id="carouselExampleIndicators" class="carousel slide pointer-event" data-ride="carousel">
                  <ol class="carousel-indicators">
                    <li data-target="#carouselExampleIndicators" data-slide-to="0" class=""></li>
                    <li data-target="#carouselExampleIndicators" data-slide-to="1" class="active"></li>
                    <li data-target="#carouselExampleIndicators" data-slide-to="2"></li>
                  </ol>
                  <div class="carousel-inner">
                    <div class="carousel-item">
                      <img class="d-block w-100" src="https://placehold.it/900x500/39CCCC/ffffff&amp;text=I+Love+Bootstrap" alt="First slide">
                    </div>
                    <div class="carousel-item active">
                      <img class="d-block w-100" src="https://placehold.it/900x500/3c8dbc/ffffff&amp;text=I+Love+Bootstrap" alt="Second slide">
                    </div>
                    <div class="carousel-item">
                      <img class="d-block w-100" src="https://placehold.it/900x500/f39c12/ffffff&amp;text=I+Love+Bootstrap" alt="Third slide">
                    </div>
          <div class="carousel-item">
                    Hello World
                    </div>
          
                  </div>
                  <a class="carousel-control-prev" href="#carouselExampleIndicators" role="button" data-slide="prev">
                    <span class="carousel-control-prev-icon" aria-hidden="true"></span>
                    <span class="sr-only">Previous</span>
                  </a>
                  <a class="carousel-control-next" href="#carouselExampleIndicators" role="button" data-slide="next">
                    <span class="carousel-control-next-icon" aria-hidden="true"></span>
                    <span class="sr-only">Next</span>
                  </a>
                </div>
              </div>
              <!-- /.card-body -->
            </div>
HTML;
  
   return  $html;
 }
 
 
 public function initials($initials) {
     
$html = <<<HTML
      <div class='circle'><span class='initials'>{$initials}</span></div>
     
HTML;
  
   return  $html;
 }
    
}//end class