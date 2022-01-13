<?php 

/** Header at top of every normal secure page
 */


class SideBar {

    public function __construct() {
        
    }

    
    public function sideBarContent(){
                    
$verticalMenu = new VerticalMenu;
$username=ucfirst($GLOBALS['Oauth2']['userArr']['userid']);
 
$html = <<<HTML
        
        
         <!-- Main Sidebar Container -->
  <aside class="main-sidebar sidebar-dark-primary elevation-4">
    <!-- Brand Logo -->
    <a href="index.php" class="brand-link">
      <img src="images/LittleRiverCrest.svg" alt="Logo" class="brand-image  elevation-3"
           style="opacity: .8">
      <span class="brand-text font-weight-light">Litte River</span>
    </a>

    <!-- Sidebar -->
    <div class="sidebar">
      <!-- Sidebar user panel (optional) -->
      <div class="user-panel mt-3 pb-3 mb-3 d-flex">
        <div class="image">
          
        </div>
        <div class="">
          <a href="#" class="d-block"><i class=" fas fa-user-alt"></i> {$username}</a>
        </div>
      </div>

      <!-- Sidebar Menu -->
      <nav class="mt-2">
        <ul class="nav nav-pills nav-sidebar flex-column" data-widget="treeview" role="menu">
          <!-- Add icons to the links using the .nav-icon class
               with font-awesome or any other icon font library -->
          
             {$verticalMenu->createMenuList()}          
            
        </ul>
      </nav>
      <!-- /.sidebar-menu -->
    </div>
    <!-- /.sidebar -->
  </aside>

        
        
        


  
        
HTML;

      
//        
      return $html;
    }
    

}
//end class