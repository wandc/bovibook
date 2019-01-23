<?php 

/** Header at top of every normal secure page
 */


class SideBar {

    public function __construct() {
        
    }

    
    public function sideBarContent(){
        ?>
  <script>
      /* menu toggle in and out*/
      /* from: https://github.com/almasaeed2010/AdminLTE/issues/1240 */
     

    $(function () {
        
         $.AdminLTESidebarTweak = {};

    $.AdminLTESidebarTweak.options = {
            EnableRemember: true,
            NoTransitionAfterReload: true
            //Removes the transition after page reload.
    };
        
        
        "use strict";

        $("body").on("collapsed.pushMenu", function() {
            console.log("collpased");
            if($.AdminLTESidebarTweak.options.EnableRemember) {
                localStorage.setItem("toggleState", "closed");
            }
        });
            
        $("body").on("expanded.pushMenu", function() {
                if($.AdminLTESidebarTweak.options.EnableRemember) {
                    localStorage.setItem("toggleState", "opened");
                } 
        });

        if ($.AdminLTESidebarTweak.options.EnableRemember) {
            var toggleState = localStorage.getItem("toggleState");
            if (toggleState == 'closed'){
                if ($.AdminLTESidebarTweak.options.NoTransitionAfterReload) {
                    $("body").addClass('sidebar-collapse hold-transition').delay(100).queue(function() {
                        $(this).removeClass('hold-transition');
                    });
                } else {
                    $("body").addClass('sidebar-collapse');
                }
            }
        }
    });
  
</script>
  <!-- Left side column. contains the logo and sidebar -->
  <aside class="main-sidebar">

    <!-- sidebar: style can be found in sidebar.less -->
    <section class="sidebar">

    

      <!-- search form (Optional) -->
      <?php
       
                $s = new Search();
                 print($s->searchForm());
            
      ?>
      
      <!--
      <form action="#" method="get" class="sidebar-form">
        <div class="input-group">
          <input type="text" name="q" class="form-control" placeholder="Search...">
              <span class="input-group-btn">
                <button type="submit" name="search" id="search-btn" class="btn btn-flat"><i class="fa fa-search"></i>
                </button>
              </span>
        </div>
      </form>
      -->
      <!-- /.search form -->

      <!-- Sidebar Menu -->
      <ul class="sidebar-menu" data-widget="tree">
        <li class="header">MAIN NAV</li>
        <!-- Optionally, you can add icons to the links -->
     <!--    <li class="active"><a href="#"><i class="fa fa-link"></i> <span>Link</span></a></li>
        <li><a href="#"><i class="fa fa-link"></i> <span>Another Link</span></a></li> -->
        
     
        <?php

    print(VerticalMenu::createMenuList());
    print("Hello2");
   ?> 
      </ul>
      

      
      
      <!-- /.sidebar-menu -->
    </section>
    <!-- /.sidebar -->
  </aside>


  
  
  
<?php        
    }
    
   

}

//end class