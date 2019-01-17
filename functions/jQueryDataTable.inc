<?php

/** helper functions to feed php code to create the javascript code needed */

/*
 * Example:
 *    print($dt->start($this->pageid, 'jsonclass',  null, $columnsStr, null, false," \"pageLength\": 100,"));
 * 
 */
class JQueryDataTable {

    public function __construct() {
        
    }

    //use this one.
    public function start($pageid, $jsonInPageFunctionName, $item = '', $stdObjColumns, $colourRowClass = null, $reload = true, $extraOptions = '') {

        $this->dataTableDefaults();

        $uuid = uniqid(); //give unique id
        $str = '';

        $str = $str . <<<js
        <table  class='display compact' id='JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}'></table>
    <script type="text/javascript">
             "use strict";
           $(document).ready(function() {
        $.fn.dataTable.ext.errMode = 'throw';
               var JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}= $('#JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}').DataTable( {
             "ajax": "{$GLOBALS['config']['HTTP']['JSON_API']}{$pageid}/{$jsonInPageFunctionName}/$item",
             "order": [[0,'desc']],
            "stateSave": true,
             "deferRender": true,
             "responsive": {
        details: {
            display: $.fn.dataTable.Responsive.display.childRowImmediate
        }
    },
              "columnDefs": [ { type: 'natural', targets: [ 1 ] }  ], /*natual sorting on column 2*/
                  $extraOptions   
               "columns": [  {$this->changestdObjColumnsToJSStr($stdObjColumns)}
                          ],
js;



        $str = $str . $this->colourRow($colourRowClass);

        $str = $str . <<<jss1
     
            } );           
jss1;

        /* reload the datatable */
        $reloadStr = <<<reload

          setInterval( function () {
                JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}.ajax.reload( null, false ); // user paging is not reset on reload
            }, 30000 );

reload;


        if ($reload == true) {
            $str = $str . $reloadStr;
        }

        $str = $str . <<<jss2
    
        } );
         </script>
jss2;

        return $str;
    }

    
    //no options, just send sql and it prints very basic. 
    //call like: print((new JQueryDataTable)->startBasicSql("SELECT * FROM TABLE"));      
    public function startBasicSql($sql,$options='searching: false,paging: false,ordering: false,info: false') {
        
         $sth = $GLOBALS['pdo']->prepare($sql);
         $sth->execute();
         $result = $sth->fetchAll(PDO::FETCH_ASSOC);
         $columnNames = array_keys(($result)[0]);
         
       //convert to jquerydatabale format for column string
         $out=array();
         $count=0;
         foreach ($columnNames as $key => $value) {    
             $out[$count]=array('title'=>$value,'data'=>$value);
                $count++;     
         }
         $columnsStr= trim(json_encode($out), '[]');
 
         $dt = new JQueryDataTable();

        $str = '';
        $str = $str .($dt->startLocal($result, $columnsStr, null, $options));

        
        return ($str);   
    }
    
    
    //no options just send an array of the form....
    // call: print( (new JQueryDataTable)->startBasicArray($dataArr));
    //array is 2d. 
     public function startBasicArray($data,$options='searching: false,paging: false,ordering: false,info: false') {
        
         
         $columnNames = array_keys(($data)[0]);
           //convert to jquerydatabale format for column string
         $out=array();
         $count=0;
         foreach ($columnNames as $key => $value) {    
             $out[$count]=array('title'=>$value,'data'=>$value);
                $count++;     
         }
         

         $columnsStr= trim(json_encode($out), '[]');

         $dt = new JQueryDataTable();

        $str = '';
        $str = $str .($dt->startLocal($data, $columnsStr, null, $options));
     
        return ($str);   
    }
  
    
    //local with array, then converted to JSON.
    public function startLocal($dataArr, $stdObjColumns, $colourRowClass = null, $options = null) {

        $this->dataTableDefaults();

        if (empty($options)) {
            
        } else {
            $options = $options . ',';
        }


        //convert to JSON
     //     $jsonStringofData=json_encode(unserialize(str_replace(array('NAN;','INF;'),'0;',serialize($dataArr)))); //get rid of not a numbers
            
        $jsonStringofData = json_encode($dataArr, JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT);
        $uuid = uniqid(); //give unique id
        $str = '';

        $str = $str . <<<js
        <table  class='display compact' id='JQueryDataTable_{$uuid}'></table>
    <script type="text/javascript">
             "use strict";
           $(document).ready(function() {
          $.fn.dataTable.ext.errMode = 'throw';
        var jsonString_$uuid = $jsonStringofData;
               var JQueryDataTable_{$uuid}= $('#JQueryDataTable_{$uuid}').DataTable( {
             "data": jsonString_$uuid,
                 $options
            
             "deferRender": true,
                       "responsive": {
        details: {
            display: $.fn.dataTable.Responsive.display.childRowImmediate
        }
    },
              "columnDefs": [ { type: 'natural', targets: [ 1 ] }  ], /*natual sorting on column 2*/
               "columns": [  {$this->changestdObjColumnsToJSStr($stdObjColumns)}
                          ],
             
js;

        $str = $str . $this->colourRow($colourRowClass);
        $str = $str . <<<jss
    
         
            } );
            
           
        } );
         </script>
jss;
        return $str;
    }

    //more basic, , etc.
    public function startCustom($pageid, $jsonInPageFunctionName, $item = '', $stdObjColumns, $colourRowClass = null, $options = null, $reload = true) {

        $this->dataTableDefaults();

        $uuid = uniqid(); //give unique id
        $str = '';

        if (empty($options)) {
            
        } else {
            $options = $options . ',';
        }


        $str = $str . <<<js
        <table  class='display compact' id='JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}'></table>
    <script type="text/javascript">
             "use strict";
           $(document).ready(function() {
        $.fn.dataTable.ext.errMode = 'throw';
               var JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}= $('#JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}').DataTable( {
             "ajax": "{$GLOBALS['config']['HTTP']['JSON_API']}{$pageid}/{$jsonInPageFunctionName}/$item",
             "order": [[0,'desc']],
             "deferRender": true,
                     "responsive": {
        details: {
            display: $.fn.dataTable.Responsive.display.childRowImmediate
        }
    },
             "bFilter": false,  
             "bPaginate": false,
             "bInfo": false,
             "searching": true,
              "columnDefs": [ { type: 'natural-asc', targets: [ 2 ] }  ], /*natual sorting on column 2*/
               "columns": [  {$this->changestdObjColumnsToJSStr($stdObjColumns)}
                          ],
               $options
js;

        $str = $str . $this->colourRow($colourRowClass);

        $str = $str . <<<jss1
     
            } );           
jss1;

        /* reload the datatable */
        $reloadStr = <<<reload

          setInterval( function () {
                JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}.ajax.reload( null, false ); // user paging is not reset on reload
            }, 30000 );

reload;


        if ($reload == true) {
            $str = $str . $reloadStr;
        }

        $str = $str . <<<jss2
    
        } );
         </script>
jss2;

        return $str;
    }

    /* used for TMR recipe new for now */
    /* NOTE: this returns stdClass, different from other methods in this class */
    public function startCustomSelect($pageid, $jsonInPageFunctionName, $item = '', $stdObjColumns, $colourRowClass = null, $options = null, $reload = true) : stdClass{

        $this->dataTableDefaults();

        $uuid = uniqid(); //give unique id
        $str = '';

        if (empty($options)) {
            
        } else {
            $options = $options . ',';
        }


        $str = $str . <<<js
        <table  class='display compact' id='JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}'></table>
    <script type="text/javascript">
             "use strict";
        window.NS = {}; /*name space so that we can put the datatable into the global (namespace) scope. */
           $(document).ready(function() {
        $.fn.dataTable.ext.errMode = 'throw';
                NS.JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}= $('#JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}').DataTable( {
             "ajax": "{$GLOBALS['config']['HTTP']['JSON_API']}{$pageid}/{$jsonInPageFunctionName}/$item",
             "order": [[0,'desc']],
             "deferRender": true,
                     "responsive": {
        details: {
            display: $.fn.dataTable.Responsive.display.childRowImmediate
        }
    },
             "bFilter": false,  
             "bPaginate": false,
             "bInfo": false,
                "select": 'single',       
              "columnDefs": [ { type: 'natural-asc', targets: [ 2 ] }  ], /*natual sorting on column 2*/
               "columns": [  {$this->changestdObjColumnsToJSStr($stdObjColumns)}
                          ],
               $options
js;

        $str = $str . $this->colourRow($colourRowClass);

        $str = $str . <<<jss1
     
            } );           
jss1;

        /* reload the datatable */
        $reloadStr = <<<reload

          setInterval( function () {
                NS.JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}.ajax.reload( null, false ); // user paging is not reset on reload
            }, 30000 );

reload;




        if ($reload == true) {
            $str = $str . $reloadStr;
        }

        $str = $str . <<<jss2
    
        } );
         </script>
jss2;

        /*
         * Return an object with table name and table text. This is needed because if we want to write external javascript code we need the table name.
         */
        $retObj = new stdClass;
        $retObj->tableName="JQueryDataTable_{$jsonInPageFunctionName}_{$uuid}";
        $retObj->str=$str;
        return $retObj;
    }

    /*
     * called by start and startlocal, etc to set some defaults.
     * 
     */

    private function dataTableDefaults() {
        //SETS some defaults.
        ?>
        <script type="text/javascript">
            $.extend(true, $.fn.dataTable.defaults, {
                "sAjaxDataProp": "data",
                "bAutoWidth": false,
                "bProcessing": true,
                "aLengthMenu": [[15, 20, 50, 75, -1], [15, 20, 50, 75, "All"]],
                "iDisplayLength": 20,
                "bSort": true,
                "dom": '<"top"<"clear">>rt<"bottom"iflp<"clear">>',
                "stateSave": true,
                "oLanguage": {
                    "sProcessing": '<img src="/images/ajax-loader-big.gif">'
                }

            });
            /* add more moment sorting templates here */
             $.extend(true, $.fn.dataTable.moment('dddd MMM DD H:mm'));  /*moment plugin sorting for this date format in datatables */
             $.extend(true, $.fn.dataTable.moment('MMM DD HH:mm dddd'));  /*moment sorting for this date format in datatables */
        </script>


        <?php

    }

    private function colourRow($colourRowClass = null) {
        $str = '';
        $str = $str . <<<jss
  createdRow: function ( node, data ) {   
      if ( data['colour_row'] == 1 ) {
        $(node).addClass( '$colourRowClass' );
      }
    },
jss;
        return $str;
    }

    //takes the object and makes columns.
    private function changestdObjColumnsToJSStr($stdObjColumns) {
        $str = '';
        //  $page=new stdClass();

        return ($stdObjColumns);
    }

    public static function formattedAnimalNameLink($bovine_id, $local_number, $full_name) {
        return $GLOBALS['MiscObj']->hrefToBovinePageFullName($bovine_id,$local_number,$full_name);
    }

    function printFormattedFieldAlphaNumeric($field_id, $alpha_numeric_id) {
        $number = "<a href=\"" . "/index.php?pageid=107&field_id={$field_id}" . "\"> <b>{$alpha_numeric_id}</b> " . "</a>";
        return $number;
    }

    function formattedShortRegNum($full_reg_number) {
        //just returns the number portion of the reg number (used for herd cull)
        $returnArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($full_reg_number);
        return $returnArray['number'];
    }

}

//end class
?>