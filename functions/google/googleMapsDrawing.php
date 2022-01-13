<?php 
/**
 * This class shows a map of a given size that can be used for drawing points on
 * and then the points can be submitted through some method.
 *
 */


//look at bottom of page for page open code.


class GoogleMapsDrawing {

     
     private $uniqueNumber;
    
    
    public function __construct() {
        //make up a random number and use it to make this code unique.
        //php must have something built in to make a class unique?
        $this->uniqueNumber=rand(0,100000000);
        //put in field id
        
        
        // nothing
    }

    public function __destruct() {
        // nothing
    }

    /** Google Maps v3 drawing example code with some modifications by d waddy */
    public function renderGoogleMapsDrawing($field_id) {
        
      
        
        ?>
        <script type="text/javascript" src="https://maps.google.com/maps/api/js?sensor=false&libraries=drawing"></script>

        <style type="text/css">
            #map_dw-<?php echo($this->uniqueNumber);?>, html, body {
                padding: 0;
                margin: 0;
                height: 100%;
            }

            #panel {
                width: 200px;
                font-family: Arial, sans-serif;
                font-size: 13px;
                float: right;
                margin: 10px;
            }

            #color-palette {
                clear: both;
            }

            .color-button {
                width: 14px;
                height: 14px;
                font-size: 0;
                margin: 2px;
                float: left;
                cursor: pointer;
            }

            #delete-button {
                margin-top: 5px;
            }
        </style>

        <script type="text/javascript">
            var drawingManager;
            var selectedShape;
            var colors = ['#1E90FF', '#FF1493', '#32CD32', '#FF8C00', '#4B0082'];
            var selectedColor;
            var colorButtons = {};

            function clearSelection() {
                if (selectedShape) {
                    selectedShape.setEditable(false);
                    selectedShape = null;
                }
            }

            function setSelection(shape) {
                clearSelection();
                selectedShape = shape;
                shape.setEditable(true);
                selectColor(shape.get('fillColor') || shape.get('strokeColor'));
            }

            function deleteSelectedShape() {
                if (selectedShape) {
                    selectedShape.setMap(null);
                    selectedShape=null;
                }
            }

            function selectColor(color) {
                selectedColor = color;
                for (var i = 0; i < colors.length; ++i) {
                    var currColor = colors[i];
                    colorButtons[currColor].style.border = currColor == color ? '2px solid #789' : '2px solid #fff';
                }

                // Retrieves the current options from the drawing manager and replaces the
                // stroke or fill color as appropriate.
           
                var polylineOptions = drawingManager.get('polylineOptions');
                polylineOptions.strokeColor = color;
                drawingManager.set('polylineOptions', polylineOptions);

                var circleOptions = drawingManager.get('circleOptions');
                circleOptions.fillColor = color;
                drawingManager.set('circleOptions', circleOptions);

                var rectangleOptions = drawingManager.get('rectangleOptions');
                rectangleOptions.fillColor = color;
                drawingManager.set('rectangleOptions', rectangleOptions);
                
                var polygonOptions = drawingManager.get('polygonOptions');
                polygonOptions.fillColor = color;
                drawingManager.set('polygonOptions', polygonOptions);
            }

            function setSelectedShapeColor(color) {
                if (selectedShape) {
                    if (selectedShape.type == google.maps.drawing.OverlayType.POLYLINE) {
                        selectedShape.set('strokeColor', color);
                    } else {
                        selectedShape.set('fillColor', color);
                    }
                }
            }

            function makeColorButton(color) {
                var button = document.createElement('span');
                button.className = 'color-button';
                button.style.backgroundColor = color;
                google.maps.event.addDomListener(button, 'click', function() {
                    selectColor(color);
                    setSelectedShapeColor(color);
                });

                return button;
            }

            function buildColorPalette() {
                var colorPalette = document.getElementById('color-palette');
                for (var i = 0; i < colors.length; ++i) {
                    var currColor = colors[i];
                    var colorButton = makeColorButton(currColor);
                    colorPalette.appendChild(colorButton);
                    colorButtons[currColor] = colorButton;
                }
                selectColor(colors[0]);
            }
               
            /* d waddy to put the data somewhere */
            /* only uspports rectangles and polygons. */
            function getData() {
                if (selectedShape == null) {
                    alert('Please click a shape to submit and try again.');
                }
                else if (selectedShape.type == google.maps.drawing.OverlayType.POLYGON) {
                     
                     //put the first index at the end, hackish way to make polygon closed.
                     var tempArr=selectedShape.getPath().getArray();
                     tempArr.push(tempArr[0]);
                     
                     writeToForm(tempArr); //send to output function.
                     
                } else if (selectedShape.type == google.maps.drawing.OverlayType.RECTANGLE) {
                    //bounds only returns two corners, so calcualte the other two corners and return as a polygon
                                    
                    var LineCordinates = new Array();
                    LineCordinates[0] = (selectedShape.getBounds().getNorthEast());
                    LineCordinates[1] = new google.maps.LatLng(selectedShape.getBounds().getSouthWest().lat(), selectedShape.getBounds().getNorthEast().lng());
                    LineCordinates[2] = (selectedShape.getBounds().getSouthWest());
                    LineCordinates[3] = new google.maps.LatLng(selectedShape.getBounds().getNorthEast().lat(), selectedShape.getBounds().getSouthWest().lng());

                    var line = new google.maps.Polygon({
                        path: (LineCordinates),
                        geodesic: false
                    });           
                    alert(line.getPath().getArray());        
                }
                else {
                    alert('Please click a shape to submit and try again.');
                }
                   
                    
            }

            //we can use jquery call for this, because we know we have it with littleriver.
            function writeToForm(strToWrite) {
            //write to form textarea and close color box.
            parent.$.fn.colorbox.close();
            
            var newStr=String(strToWrite);
            
            //use string replacing to convert to linsestring. we use regexp, because otherwise replace only does first occurence.
            newStr=newStr.replace(/\)\,\(/g, 'xxx').replace(/,/g, '').replace(/xxx/g, ',').replace(/\(/g, 'LINESTRING(');
            
            parent.$('#polygon').val(newStr);
            }
            
            
            function initialize() {
                  
                var mapCenterPoint;
                //find center of polygon we are loading to center map to
        <?php  echo((new CroppingHelper)->fieldBorderCentroidToGoogleMapsJavascriptPoint($field_id)); ?>
                  
                  
                      var map = new google.maps.Map(document.getElementById('map_dw-<?php echo($this->uniqueNumber);?>'), {
                          zoom: 16,
                          center: mapCenterPoint,
                          mapTypeId: google.maps.MapTypeId.HYBRID,
                          disableDefaultUI: true,
                          zoomControl: true
                      });

                    
                      // georssLayer.setMap(map);
                      //get from PHP, the points for a field from the db
        <?php  echo((new CroppingHelper)->fieldBorderToGoogleMapsJavascriptVar($field_id)); ?>

         
                // Construct the polygon
                var bermudaTriangle = new google.maps.Polygon({
                    paths: polygonPoints,
                    strokeColor: "#FF0000",
                    strokeOpacity: 0.8,
                    strokeWeight: 2,
                    fillColor: "#FF0000",
                    fillOpacity: 0.35
                });
         
                bermudaTriangle.setMap(map);




                var polyOptions = {
                    strokeWeight: 0,
                    fillOpacity: 0.45,
                    editable: true
                };
                // Creates a drawing manager attached to the map that allows the user to draw
                // only rectangles and polygons in this case
                drawingManager = new google.maps.drawing.DrawingManager({
                    drawingMode: google.maps.drawing.OverlayType.POLYGON,
                    drawingControlOptions: {
                        position: google.maps.ControlPosition.TOP_LEFT,
                        drawingModes: [google.maps.drawing.OverlayType.RECTANGLE, google.maps.drawing.OverlayType.POLYGON]
                    },
                    markerOptions: {
                        draggable: true
                    },
                    polylineOptions: {
                        editable: true
                    },
                    rectangleOptions: polyOptions,
                    circleOptions: polyOptions,
                    polygonOptions: polyOptions,
                    map: map
                });

                google.maps.event.addListener(drawingManager, 'overlaycomplete', function(e) {
                    if (e.type != google.maps.drawing.OverlayType.MARKER) {
                        // Switch back to non-drawing mode after drawing a shape.
                        drawingManager.setDrawingMode(null);

                        // Add an event listener that selects the newly-drawn shape when the user
                        // mouses down on it.
                        var newShape = e.overlay;
                        newShape.type = e.type;
                        google.maps.event.addListener(newShape, 'click', function() {
                            setSelection(newShape);
                        });
                        setSelection(newShape);
                    }
                });

                // Clear the current selection when the drawing mode is changed, or when the
                // map is clicked.
                google.maps.event.addListener(drawingManager, 'drawingmode_changed', clearSelection);
                google.maps.event.addListener(map, 'click', clearSelection);
                google.maps.event.addDomListener(document.getElementById('delete-button'), 'click', deleteSelectedShape);
                google.maps.event.addDomListener(document.getElementById('getdata-button'), 'click', getData);

                buildColorPalette();
            }
            google.maps.event.addDomListener(window, 'load', initialize);
        </script>



        <div id="panel">
            <div id="color-palette"></div>
            <div>
                <button id="delete-button">Delete Selected Shape</button>
            </div>
            <div>
                <button id="getdata-button">Get Data</button>
            </div>
        </div>
        <div id="map_dw-<?php echo($this->uniqueNumber);?>" style="width: 720px; height: 600px"></div>



        <?php 
    }

    //////////////////////////////////////

    /** jquery slider  */
    public function slider() {
        
        
       
        ?>
        <style>
            .toggler {  width: 50px; height: 30px; }
            #buttonToggle-<?php echo($this->uniqueNumber);?> { position: relative; top:16px; left:12px; padding: .5em 1em; text-decoration: none; }
            #effect-<?php echo($this->uniqueNumber);?> { left:0px; top:30px; width: 640px; height: 635px; padding: 0.4em; position: relative; }
            #effect-<?php echo($this->uniqueNumber);?> h3 { margin: 0; padding: 0.6em; text-align: center; }
        </style>
        <script>
            $(function() {
                
               
                
                //default is to be closed. run once.
                 $( "#effect-<?php echo($this->uniqueNumber);?>" ).toggle( 'slide', {}, 500 );
                
                // run the currently selected effect
                function runEffect<?php echo($this->uniqueNumber);?>() 
                {		
                    // most effect types need no options passed by default
                    var options = {};	
                    // run the effect
                    $( "#effect-<?php echo($this->uniqueNumber);?>" ).toggle( 'slide', options, 500 );
                };
        		
                // set effect from select menu value
                $( "#buttonToggle-<?php echo($this->uniqueNumber);?>" ).click(function() {
                    runEffect<?php echo($this->uniqueNumber);?>();
                    return false;
                });
            });
        </script>

        <div class="demo">

            <div class="toggler">
                <div id="effect-<?php echo($this->uniqueNumber);?>" class="ui-widget-content ui-corner-all">
                    <h3 class="ui-widget-header ui-corner-all">Map</h3>
                    <p>
        <?php  $this->googleMapsDrawing(3247); ?>	
                    </p>
                </div>
            </div>


            <a href="#" id="buttonToggle-<?php echo($this->uniqueNumber);?>" class="ui-state-default ui-corner-all">Map Select</a>
        </div><!-- End demo -->





    <?php 
    }


    
    
    
    
    //shows popup box
    public function colorBoxMap() {
        print('<link type="text/css" media="screen" rel="stylesheet" href="javascript/popup/colorbox.css"/> ');
        print('<script src="/javascript/popup/jquery.colorbox.js"></script>');
        ?>
        <script type="text/javascript" language="javascript">
            $(document).ready(function(){
                $("#colorBoxToggle-map").colorbox({width:"1000px", height:"700px", iframe:true});
                //$("#colorBoxToggle").trigger('click'); //open the page.
            });		
        </script>
        <?php 

    }

    //not really used, EXAMPLE. Instead we would use javascript directly, like on field page.
    public function colorBoxStringMap() {
         return ('<a id="colorBoxToggle-map" href="/functions/googleMapsDrawing.php?openpage=true">Open Map</a>'); //slding box toogle switch
    }
    
    
    
    /**
     * shows a colorbox right now.
     */
    public function showColorBoxNowMap() {
        print('<script type="text/javascript"> 
$(document).ready(function() {   
        $(".colorBoxToggle-map").trigger("click"); //open the page.
    });  
</script>  
');
    }
    
    
    
    
}//end class
if (isset($_REQUEST['openpage'])) 
{
if  ($_REQUEST['openpage']=='true')
{ 
 $a=new GoogleMapsDrawing();
 $a->renderGoogleMapsDrawing($_REQUEST['field_id']);
}} 




?>