<?php 

//include_once($_SERVER['DOCUMENT_ROOT'].'/template/basePage.php');
/**
 * Extends base page to have a somewhat generic tab page, using jquery tabs.
 * Jan 5 2011
 *
 * Supports up to 15 tabs.
 */
Class TabPage extends BasePage {
    
    public $pageid;
    
     public function __construct()
    {
     parent::__construct();
    
    }

    protected $classCSS = 'jquery_tabs';
    private $numberOfTabs = 0; //default to 0 until set.

    //the child pages ovveride this constructor.

    

    //change default css for tabs.
    public function setCSStabs($classCSS) {
        $this->classCSS = $classCSS;
        return 1;
    }

    public function getCSStabs($classCSS) {
        return($this->classCSS);
    }

    
   
    
    
    final function customJavascript() {
        


        /* We store the selected tab in local storage 
         * For the server to know what tab we are on we use the tabtocall callback. 
         * Other methods would be a hiddned field writen by jquery before form submission or a URL rewriter of some sort.
         */
        ?>
        <script type="text/javascript"> 
                                            
                                            
$(document).ready(function () {
    
   
      /* set local storage tab value from URL first, this is for when we link directly to specific tab with <a> link */
      /* need to make it null afterwards or it doesn not work for forms, maybe called mulktiple times? */
               var url_string = window.location.href; 
               var url = new URL(url_string);
               var urlTab = url.searchParams.get("tabtocallAnchor");
               if (urlTab != null) {
                        url.searchParams.set("tabtocallAnchor",null);
                        localStorage.setItem('JQuery_TabID_Storage_<?php echo(get_called_class()); ?>', urlTab);   
         }
    
    var currentTabIndex = "0";

    $tab = $("#jquery_tabs").tabs({
        effect: 'ajax',
        beforeLoad: function(event, ui) {
    ui.panel.html('<img src="/images/ajax-loader-big.gif" width="24" height="24" style="vertical-align:middle;"> Loading...');
  },
         activate : function (event, ui) {
            currentTabIndex = ui.newTab.index().toString();
            localStorage.setItem('JQuery_TabID_Storage_<?php  echo(get_called_class()); ?>', currentTabIndex);
         }
    });
    
    if (localStorage.getItem('JQuery_TabID_Storage_<?php  echo(get_called_class()); ?>') != null) {
        currentTabIndex = localStorage.getItem('JQuery_TabID_Storage_<?php  echo(get_called_class()); ?>');  
        $tab.tabs('option', 'active', currentTabIndex);
    }
    $('#btn-sub').on('click', function () {
        localStorage.setItem('JQuery_TabID_Storage_<?php  echo(get_called_class()); ?>', currentTabIndex);    
    });
});
      
        </script>
        <?php 
    }
    
    
    

    /**
     *
      Call with a tabArray like this:

      $tabArray[1]['name']='one';
      $tabArray[1]['extra_callback_param']="&field_id={$this->field_id}";
      $tabArray[2]['name']='Soil, Lime, Fertilizer';
      $tabArray[2]['extra_callback_param']="&field_id={$this->field_id}";
      OR (overide everything).
      $tabArray[2]['overide_callback_param']='/functions...../';
      $tabArray[3]['name']='three';
      $tabArray[3]['load_inline']='1';

     */
    
    function renderTabs($tabArray) {

        $this->numberOfTabs = count($tabArray);

        //find out what tab will be loaded, when page refreshes from jquery tabs localStorage
        // VERY IMPORTANT: without this, quckform callbacks don't work, because page isn't loaded.    
        if ((isset($_REQUEST['tabtocall']) ? $_REQUEST['tabtocall'] : null) == null) { //if it doesn't exist set it to null.
            $tabToBeLoaded=1;
        }
        else {
        $tabToBeLoaded=$_REQUEST['tabtocall'];
        }
        
        
        //ENABLE this if you want the first tab to always be loaded without using ajax
        //seems to have to be enabled for tab quickfoms to work. why???
        $tabArray[$tabToBeLoaded ]['load_inline'] = 1;
        //print_r2($tabArray);  //DEBUG

        print("
            <div id='jquery_tabs' class='{$this->classCSS}'>
              <ul>");

        $count = 1;
        foreach ($tabArray as $value) {
            if ($count == 15+1) {
                throw new Exception("TabPage only supports a max of 15 tabs. Reduce your number.");
            }

            if (empty($value['extra_callback_param'])) { $value['extra_callback_param']=null;}
            
            
            if (isset($value['load_inline']) && ($value['load_inline'] == 1)) {
                print(" <li><a title=\"x_$count\" href=\"#x_$count\"><span>{$value['name']}</span></a></li>");
            } elseif ( (isset($value['overide_callback_param'])==false) || ($value['overide_callback_param'] == null)) {
                print(" <li><a title=\"x_$count\" href=\"/index.php?pageid={$_REQUEST['pageid']}&amp;callback=1&amp;tabtocall=$count{$value['extra_callback_param']}\"><span>{$value['name']}</span></a></li>");
            } else { //this overrides everything, used for external sites like cdn.
                print(" <li><a title=\"x_$count\" href=\"{$value['overide_callback_param']}\"><span>{$value['name']}</span></a></li>");
            }
            $count++;
        }

        print('</ul>');

        $count = 1;
        foreach ($tabArray as $value) {
            if (isset($value['load_inline']) && ($value['load_inline'] == 1)) {
                print("<div id=\"x_$count\">");
                $this->callBack($count);
                print("</div> <!-- End tab content inlineTab-$count. -->");
            }
            $count++;
        }

        print('</div> <!--end of tabs div -->');
    }

    function callBack($tab_num = '') {

        //if not set request value.
        if ($tab_num == '') {
            $tab_num = $_REQUEST['tabtocall'];
        }

        $this->customSQL();   //must be put here, so it is in scope for every tab. 

        switch ($tab_num) {
            case 1:
                $this->tab1();
                break;
            case 2:
                $this->tab2();
                break;
            case 3:
                $this->tab3();
                ;
                break;
            case 4:
                $this->tab4();
                break;
            case 5:
                $this->tab5();
                break;
            case 6:
                $this->tab6();
                break;
            case 7:
                $this->tab7();
                break;
            case 8:
                $this->tab8();
                break;
            case 9:
                $this->tab9();
                break;
            case 10:
                $this->tab10();
                break;
            case 11:
                $this->tab11();
                break;
            case 12:
                $this->tab12();
                break;
            case 13:
                $this->tab13();
                break;
            case 14:
                $this->tab14();
                break;
            case 15:
                $this->tab15();
                break;
            default:
                throw new Exception("ERROR: Unknown tab number.");
        }//end switch
    }

      protected function tab1() {}
      protected function tab2() {}
      protected function tab3() {}
      protected function tab4() {}
      protected function tab5() {}
      protected function tab6() {}
      protected function tab7() {}
      protected function tab8() {}
      protected function tab9() {}
      protected function tab10() {}
      protected function tab11() {}
      protected function tab12() {}
      protected function tab13() {}
      protected function tab14() {}
      protected function tab15() {}
      
  
      /*
       Override in child class.
       */
      function pageJavascript() {}
    
      function customBody() {}
      function defaultDisplay() {}
      //function customJavascript() {}
      function pageHeadJavascript() {}
      function customCSS() {}
      //function callBack() {}
      function error() {}
      protected function customSQL() {}
      function tabJavaScript() {} 
    
      
    }//end class 
?>