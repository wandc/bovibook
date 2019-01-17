<?php 
/* used within a page to setup an jquery accordion tab list */

/* call with form of :

  //put this somewhere within scope with the content of the tabs
  class AccordionImplementation extends AccordionSub {
  function tab1() {
  print('hello 1');
  }
  }//end class


  //put this where you want to call it.
  $accordionArray[1]['name']='one';
  $accordionArray[2]['name']='two';
  $accordion=new AccordionImplementation($accordionArray);

 */

abstract class AccordionSub {

    protected $classCSS = 'jquery_accordion';
    public $uuid;

    public function __construct() {
        
    }

    // this will be called automatically at the end of scope
    public function __destruct() {
        //nothing
    }

    //change default css for tabs.
    public function setCSS($classCSS) {
        $this->classCSS = $classCSS;
        return 1;
    }

    public function getCSS($classCSS) {
        return($this->classCSS);
    }

    /**
     * Renders the accordion
     * @param array $accordionArray 
     * @return null
     */
    public function render($accordionArray) {

        //always creates the same hash string, based on the accordion array, to maintain state.
        $this->uuid = (md5(serialize($accordionArray)));


        /* ACCORDIAN */
        ?>     

        <script type="text/javascript" language="javascript">
            function runAccordion() {

                $("#accordion_<?php  print($this->uuid); ?>").accordion({
                    collapsible: true,
                    heightStyle: "content",
                    beforeActivate: function (event, ui) {
                    },
                    /* get from local storage and make the active index. */
                     active: parseInt(localStorage.getItem("accIndex_<?php  print($this->uuid); ?>")),
                                         
                    /* store the accordion index in local storage */
                    activate: function (event, ui) {
                        localStorage.setItem("accIndex_<?php  print($this->uuid); ?>", $(this).accordion("option", "active"));
                      
                    }
                   
                });

            }

            /* assumes o is within tab, document.ready is not run on tab click*/
            $('#jquery_tabs').bind('tabsload', function (event, ui) {
                runAccordion();
            });

            /* accordion when no tabs on page */
            $(document).ready(function () {
                runAccordion();
            });

        </script>  
        <?php 
        $this->numberOfTabs = count($accordionArray);



        print("<div id='accordion_{$this->uuid}' class='{$this->classCSS}'>" . "\n\r");

        //key is tab number
        foreach ($accordionArray as $key => $value) {
            if ($this->numberOfTabs == 15) {
                throw new Exception("Accordion only supports a max of 15 tabs. Reduce your number.");
            }

            print("<h3><a href='#'>{$value['name']}</a></h3>" . "\n\r");
            print('<div>' . "\n\r");
            $this->chooser($key); //content
            print('</div>' . "\n\r");
        }

        print('</div>' . "\n\r");
    }

    function chooser($number) {


        switch ($number) {
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
                ;
                break;
            case 7:
                $this->tab7();
                break;
            case 8:
                $this->tab8();
                break;
            case 9:
                $this->tab9();
                ;
                break;
            case 10:
                $this->tab10();
                break;
            case 11:
                $this->tab11();
                break;
            case 12:
                $this->tab12();
                ;
                break;
            case 13:
                $this->tab13();
                ;
                break;
            case 14:
                $this->tab14();
                ;
                break;
            case 15:
                $this->tab15();
                ;
                break;
            default:
                throw new Exception("ERROR: Unknown accordion tab number.");
        }//end switch
    }

    abstract protected function tab1();

    protected function tab2() {
        
    }

    protected function tab3() {
        
    }

    protected function tab4() {
        
    }

    protected function tab5() {
        
    }

    protected function tab6() {
        
    }

    protected function tab7() {
        
    }

    protected function tab8() {
        
    }

    protected function tab9() {
        
    }

    protected function tab10() {
        
    }

    protected function tab11() {
        
    }

    protected function tab12() {
        
    }

    protected function tab13() {
        
    }

    protected function tab14() {
        
    }

    protected function tab15() {
        
    }

}

//end class
?>