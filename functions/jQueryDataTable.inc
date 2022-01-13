<?php

/** helper functions to feed php code to create the javascript code needed */

/*
 * Example:
 *    print($dt->start('Title CHANGE ME', $this->pageid, 'jsonclass',  null, $columnsStr, null, false," \"pageLength\": 100,"));
 * 
 */
class JQueryDataTable {

    public $htmlCard;

    /*
     * Defaults to showing HTML card
     */
    public function __construct($htmlCard = false) {

        $this->htmlCard = $htmlCard;
    }

    public function htmlTable($tableTitle = '', $jsonInPageFunctionName = '', $uuid) {

        $jsonInPageFunctionName = !empty($jsonInPageFunctionName) ? $jsonInPageFunctionName . '_' : '';

        $str = <<<HTML
            <div class="card card-outline card-primary">
                <div class="card-header">
                    <h3 class="card-title">$tableTitle</h3>
                </div>
                <!-- /.card-header -->
                <div class="card-body">
                    <table  class='display compact' id='JQueryDataTable_{$jsonInPageFunctionName}{$uuid}'></table>
                </div> 
            </div>
     HTML;

        $strBasic = <<<HTML
          <table  class='display compact' id='JQueryDataTable_{$jsonInPageFunctionName}{$uuid}'></table>   
     HTML;

        //Based on class constructor we show card or not.   
        return ($this->htmlCard == false) ? $str : $strBasic;
    }

    //use this one.
    public function start($tableTitle, $pageid, $jsonInPageFunctionName, $item = '', $stdObjColumns, $colourRowClass = null, $reload = true, $extraOptions = '') {

        $this->dataTableDefaults();
      
        $uuid=uniqid();//do not give UUID or state save does not work. //$uuid = uniqid(); //give unique id
        //$uuid=1;
        
        $str = '';

        $str = $str . <<<js
       {$this->htmlTable($tableTitle,$jsonInPageFunctionName,$uuid)}
    <script type="text/javascript">
             "use strict";
           $(document).ready(function() {
        
       
           /* code to delete old datattable local storage data. not tested. this can be only here and not start custom, just has to be run once in a while. */
        /* deletes local storage older than 2 weeks. */
  /*  Object.entries(localStorage).forEach(([key, val]) => {
  var obj = jQuery.parseJSON(val);
  if (((new Date(Date.now() - 12096e5)).valueOf()) >= obj.time) delete localStorage[key];
}); */
        
        
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
    //call like: print((new JQueryDataTable)->startBasicSql('Title BasicSQL CHANGE ME', "SELECT * FROM TABLE"));      
    public function startBasicSql($tableTitle,$sql,$options='searching: false,paging: false,ordering: false,info: false') {
        
         $sth = $GLOBALS['pdo']->prepare($sql);
         $sth->execute();
         $data = $sth->fetchAll(PDO::FETCH_ASSOC);
         
         $out=array();
          if (!(empty($data))) 
         {
         $columnNames = array_keys(($data)[0]);
         
       //convert to jquerydatabale format for column string
         
         $count=0;
         foreach ($columnNames as $key => $value) {               
             $out[$count]=array('title'=>$value,'data'=>$value);
             $count++;     
         }}
         
         
         $columnsStr= trim(json_encode(($out)), '[]');
 
         $dt = new JQueryDataTable();

        $str = '';
        $str = $str .($dt->startLocal($tableTitle, $data, $columnsStr, null, $options));

        
        return ($str);   
    }
    
        //with options, just send sql and it prints. 
    //call like: print((new JQueryDataTable)->startSql("SELECT * FROM TABLE"));      
    public function startSql($sql,$options='searching: true,paging: true,ordering: true,info: true') {
        
         $sth = $GLOBALS['pdo']->prepare($sql);
         $sth->execute();
         $data = $sth->fetchAll(PDO::FETCH_ASSOC);
         
         $out=array();
          if (!(empty($data))) 
         {
         $columnNames = array_keys(($data)[0]);
         
       //convert to jquerydatabale format for column string
         
         $count=0;
         foreach ($columnNames as $key => $value) {    
             $out[$count]=array('title'=>$value,'data'=>$value);
                $count++;     
         }}
         $columnsStr= trim(json_encode($out), '[]');
 
         $dt = new JQueryDataTable();

        $str = '';
        $str = $str .($dt->startLocal('Title Local CHANGE ME', $data, $columnsStr, null, $options));

        
        return ($str);   
    }
    
    //no options just send an array of the form....
    // call: print( (new JQueryDataTable)->startBasicArrayOpt('Title BasicArr CHANGE ME', $dataArr));
    //array is 2d. 
     public function startBasicArray($tableTitle,$data,$options='searching: false,paging: false,ordering: false,info: false') {
        
         
        
         $out=array();
         
         if (!(empty($data))) 
         {
             
         $columnNames = array_keys(($data)[0]);
           //convert to jquerydatabale format for column string
         $count=0;
         foreach ($columnNames as $key => $value) {    
             $out[$count]=array('title'=>$value,'data'=>$value);
                $count++;     
         }
         }

         $columnsStr= trim(json_encode($out), '[]');

         $dt = new JQueryDataTable();

        $str = '';
        $str = $str .($dt->startLocal($tableTitle, $data, $columnsStr, null, $options));
     
     
        
        return ($str);   
    }
    
    public function startBasicArrayOpt($tableTitle,$data,$options='searching: true,paging: true,ordering: true,info: false') {
        return $this->startBasicArray($tableTitle,$data,$options);
    }
    
    public function startBasicArrayNoSort($tableTitle,$data,$options='searching: true,paging: true,false: true,info: false') {
        return $this->startBasicArray($tableTitle,$data,$options);
    }
    
  
  /* same as basic but has colour_row support */
    //call like: ( (new JQueryDataTable)->startBasicArrayColour('Title BasicArrayColour CHANGE ME', $dataArr,$options='paging: false')); 
     public function startBasicArrayColour($tableTitle,$data,$options='searching: false,paging: false,ordering: false,info: false',$colourRowClassName=null) {
        
         if (empty($colourRowClassName)) {
            $colourRowClassName= 'highlight_important';
         }
        
     
        
         
         $columnNames = array_keys(($data)[0]);
           //convert to jquerydatabale format for column string
         $out=array();
         $count=0;
         foreach ($columnNames as $key => $value) {          
             if ($value!='colour_row') //skip colour class so not displayed
             {
             $out[$count]=array('title'=>$value,'data'=>$value);        
             $count++;     
             }
         }
    
         $columnsStr= trim(json_encode(($out)), '[]');

         $dt = new JQueryDataTable();

        $str = '';
        $str = $str .($dt->startLocal($tableTitle, $data, $columnsStr, $colourRowClassName, $options));
     
        return ($str);   
    }
    
    
    //local with array, then converted to JSON.
    public function startLocal($tableTitle,$dataArr, $stdObjColumns, $colourRowClass = null, $options = null) {

        $this->dataTableDefaults();

        if (empty($options)) {
            
        } else {
            $options = $options . ',';
        }


        //convert to JSON
     //     $jsonStringofData=json_encode(unserialize(str_replace(array('NAN;','INF;'),'0;',serialize($dataArr)))); //get rid of not a numbers
            
        $jsonStringofData = json_encode($dataArr, JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT);
        $uuid = uniqid(); //give unique id
        //$uuid=1;
        $str = '';

        $str = $str . <<<js
     {$this->htmlTable($tableTitle,null,$uuid)}
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
    public function startCustom($tableTitle,$pageid, $jsonInPageFunctionName, $item = '', $stdObjColumns, $colourRowClass = null, $options = null, $reload = true) {

        $this->dataTableDefaults();

        $uuid = uniqid(); //give unique id
        //$uuid=1;
        $str = '';

        if (empty($options)) {
            
        } else {
            $options = $options . ',';
        }


        $str = $str . <<<js
       {$this->htmlTable($tableTitle,$jsonInPageFunctionName,$uuid)}
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
        //$uuid=1;
        $str = '';

        if (empty($options)) {
            
        } else {
            $options = $options . ',';
        }


        $str = $str . <<<js
        {$this->htmlTable(null,$jsonInPageFunctionName,$uuid)}
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

    /*orderClasses fixes so colour stays when  sort happens. */  
    /* depends on column called colour_row */
    private function colourRow($colourRowClass = null) {
        $str = '';
        $str = $str . <<<jss
  createdRow: function ( node, data ) {   
      if ( data['colour_row'] == 1 ) {
        $(node).addClass( '$colourRowClass' );
      }
    },
       "orderClasses": false,       
jss;
        return $str;
    }

    //takes the object and makes columns.
    private function changestdObjColumnsToJSStr($stdObjColumns) {
      //  var_dump($stdObjColumns);
        $str = '';
        //  $page=new stdClass();

        return ($stdObjColumns);
    }

    public static function formattedAnimalNameLink($bovine_id, $local_number, $full_name, $class=null,$breed=null) {
        return $GLOBALS['MiscObj']->hrefToBovinePageFullName($bovine_id,$local_number,$full_name,$class,$breed);
    }
    
    public static function formattedAnimalNameLinkHolsteinCanada($bovine_id, $local_number, $full_name, $class = null, $breed = null, $full_reg_number) {

        $link = (new HolsteinCanadaHelper())->createHolsteinCanadaQuery($full_reg_number);
        $t = preg_split("/[\s,]+/", $full_name); // this must be this way for pass by reference error. limnitation of the language. 
        return "<a class='bovineNameNumber $class' href=\"" . $link . "\"> $breed<b>$local_number</b> - " . end($t) . "</a>";
    }

    public static function printFormattedFieldAlphaNumeric($field_id, $alpha_numeric_id) {
        $number = "<a href=\"" . "/index.php?pageid=107&field_id={$field_id}" . "\"> <b>{$alpha_numeric_id}</b> " . "</a>";
        return $number;
    }

    public static function formattedShortRegNum($full_reg_number) {
        //just returns the number portion of the reg number (used for herd cull)
        $returnArray = $GLOBALS['MiscObj']->breakUpFullRegNumber($full_reg_number);
        return empty($returnArray) ? null : $returnArray['number'];
    }

    
    /* we probably should choose 4 functions here and have all out date display use them */
    public static function formattedDateYear($date,$dateStr='Y') {
        return (empty($date)) ? '':  date($dateStr, strtotime($date));
    }
    
    public static function formattedDateMonthYear($date,$dateStr='M Y') {
        return (empty($date)) ? '':  date($dateStr, strtotime($date));
    }
    
    public static function formattedDate($date,$dateStr='M d, Y') {
        return (empty($date)) ? '':  date($dateStr, strtotime($date));
    }
    
    //day of week date time
    public static function formattedDateTimeDofW($date,$dateStr='l M d H:00') {
        return (empty($date)) ? '':  date($dateStr, strtotime($date));
    }
    
    //date time
     public static function formattedDateTime($date,$dateStr='M d, Y H:00') {
        return (empty($date)) ? '':  date($dateStr, strtotime($date));
    }
    
    
    /*
     * very generic delete button. TODO: add callback function to be called with delete. 
     * 
     */
    // Call like:
    // deleteAll (true)is for things like lists where all are equal and any can be deleted at anytime.
    //$rowOut['action']= (new JQueryDataTable())->actionDeleteButton($row['estrus_id'], 'bovinemanagement.estrus_event', 'id',null,"Location: {$GLOBALS['config']['HTTP']['URL']}?pageid={$_REQUEST['pageid']}",null,null);
    //SUPERCEDED BY QF2_DeleteButtonGeneric
    public function actionDeleteButton($deleteID, $schemaTable, $schemaTableID, $sql = null, $callBackURL = null, $callBackFunctionVar = null, $deleteAll = null) {
        $deleteButtonAsString = '';


        if (empty($sql)) {
            $stmt = $GLOBALS['pdo']->prepare("SELECT $schemaTableID FROM $schemaTable WHERE userid=:currentUserid ORDER BY $schemaTableID DESC LIMIT 1");  //find the last entry for the current user and use that as a basais to display the delete button or not.
        } else {
            $stmt = $GLOBALS['pdo']->prepare($sql);
        }
        $currentUserid=$GLOBALS['auth']->getUsername();
        $stmt->bindParam(':currentUserid',$currentUserid);

       $stmt->execute();
   
     
   
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
       
        //admin mode on
        if (($GLOBALS['auth']->getAdminMode() == 1) OR ($deleteAll==true)) {
            $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $deleteID, 'deleteName' => $schemaTableID, 'schemaTable' => $schemaTable));
            $deleteButtonAsString = ($a->renderDelete($deleteID));
        }
        else {

                //regular user        //We only want the delete button shown once for the latest entry.
             if (!empty($row['id'])) {
                if ($deleteID == $row['id']) {
                   $a = new QF2_DeleteButtonGeneric((object) array('callBackURL' => $callBackURL), 'individual', (object) array('deleteID' => $deleteID, 'deleteName' => $schemaTableID, 'schemaTable' => $schemaTable));
                  $deleteButtonAsString = ($a->renderDelete($deleteID));
             }}

        }

        return $deleteButtonAsString;
    }

}

//end class
?>