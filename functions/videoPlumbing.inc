<?php

// Trait for video display classes 
// holds a bunch of functions that would otherwise be duplicated to make the video page. 
trait videoPlumbing { 
   
  /*
   * take a single camera array and 
   * 
   * 
   */
    public function tabIndividualVideo($cameraArr) { 
           //check for basic error
      if (empty($cameraArr) OR (!is_array($cameraArr))) { throw new Exception("Error: camera Array not supplied to tabIndividualVideo.");}

      $url=$this->urlGenerator($cameraArr['security'],$cameraArr['host']);
//choose type of string based on camera type.  
    switch ($cameraArr['type']) {
                case 'cropFishEyeLarge':
                    $out[] =('<img class="img-responsive" src="'.$url.'/mjpg/1/video.mjpg?timestamp='.strtotime("now").'&resolution='. $cameraArr['resVideo'] .'"/>'."\n\r");   
                    break;           
                 case 'newer':
                    $out[] =('<img class="img-responsive" src="'.$url.'/mjpg/1/video.mjpg?timestamp='.strtotime("now").'&resolution='. $cameraArr['resVideo'] .'"/>'."\n\r");   
                    break;
                 case 'm3204':
                    $out[] =('<img class="img-responsive" src="'.$url.'/mjpg/video.mjpg?timestamp='.strtotime("now").'&resolution='. $cameraArr['resVideo'] .'"/>'."\n\r");   
                    break;
                 case 'm3044':
       	               $out[] = ("<div class='cropFishEyeLarge'><img src='$url/mjpg/1/video.mjpg?timestamp=".strtotime("now")."'/></div>  "."\n\r");   //Axis M3044-V
                    break;
                 case 'm3007':
                      $out[] =('<div class="cropFishEyeLargeBottom"><img class=""  src="'.$url.'/mjpg/video.mjpg?timestamp='.strtotime("now").'"/></div>'."\n\r");   
                    break;
                default:
                    $out[] = ('<img class="img-responsive" src="'.$url.'/axis-cgi/mjpg/video.cgi?resolution='.$cameraArr['resVideo'].'&dummy='.strtotime("now").'&resolution='. $cameraArr['resVideo'] .'"/>'."\n\r");   
                    break;
            }

return implode($out);
    }
    
  //generate HTML for static video feeds from an array of camera data.  //used for when coming from the internet, to just show pics. 
  public function staticFeedsHTML($cameraArr) {
      
      //check for basic error
      if (empty($cameraArr) OR (!is_array($cameraArr))) { throw new Exception("Error: camera Array not supplied.");}
      
       $out[]=(new BootStrap)->infoAlertBasic('You are here from the internet and thus only get static pictures, no video, to save bandwidth.');

         $out[]='<div class="container">'; 
    
      foreach ($cameraArr as $key => $value) {
           
          if($key % 2 == 0){  //even number
       $out[]='<div class="row mx-md-n5 ">';
    }   
           
      $out[]=" <div class='col-md-6 col-md-offset-2'><br/><img style='width:640px;'class='img-responsive' id='staticVideoImage' src='/functions/displayVideoImage.php?host={$value['host']}&security={$value['security']}' /></div>";    
        
       if($key % 2 == 1){  //odd number
       $out[]='</div>';
    }   
             
      } 
        $out[]='</div>';
        
     return  implode($out);
  } 
  
  

/**
 *  show all video feeds, used for first tab usually
 */
public function dynamicFeedsHTML($cameraArr) {
    $time=strtotime("now");
    
      //check for basic error
      if (empty($cameraArr) OR (!is_array($cameraArr))) { throw new Exception("Error: camera Array not supplied.");}
      
     
      foreach ($cameraArr as $key => $value) {
      
      
       $url=$this->urlGenerator($value['security'],$value['host']);
       $errorText='Image failed to load from: '.$value['host'];
          
          if($key % 2 == 0){  //even number
      $out[]= '<div class="row mx-md-n5">';
    }   
  
    //choose type of string based on camera type.  
    switch ($value['type']) {
                 case 'cropFishEyeLarge':
                    $out[] = ('<div class="col-md-6 col-md-offset-2"><div class="cropFishEyeLargePreview"><img alt="'.$errorText.'" width="640px" src="'.$url.'/mjpg/1/video.mjpg?resolution='.$value['resVideo'].'&timestamp=' . strtotime("now") . '"/></div></div>  ' . "\n\r");   //Axis M3044-V
                    break;  
                case 'cropFishEyeLargeBottom':
                    $out[] = ('<div class="col-md-6 col-md-offset-2"><div class="cropFishEyeLargeBottomPreview"><img alt="'.$errorText.'" width="640px" src="'.$url.'/mjpg/1/video.mjpg?resolution='.$value['resVideo'].'&timestamp=' . strtotime("now") . '"/></div></div>  ' . "\n\r");   //Axis M3044-V
                    break;             
                 case 'newer':
                    $out[] = ('<div class="col-sm-6 col-md-offset-2"><img  alt="'.$errorText.'" width="640px" src="'.$url.'/mjpg/1/video.mjpg?resolution='.$value['resVideo'].'&timestamp=' . strtotime("now") . '"/></div> ' . "\n\r");   //Axis M3044-V
                    break;  
                case 'm3204':
                    $out[] = ('<div class="col-sm-6 col-md-offset-2"><img  alt="'.$errorText.'" width="640px" src="'.$url.'/mjpg/1/video.mjpg?resolution='.$value['resVideo'].'&timestamp=' . strtotime("now") . '"/></div> ' . "\n\r");   //Axis M3044-V
                    break;  
                case 'm3044':
       	               $out[] = ('<div class="col-md-6 col-md-offset-2"><div class="cropFishEye"> <img alt="'.$errorText.'" class="img-responsive" src="'.$url.'/axis-cgi/mjpg/video.cgi?resolution=640x640&dummy='.strtotime("now") .'"/></div> </div> '.  "\n\r" );   //Axis M3044-V
                    break;
                 case 'm3007':                                                                                                    
       	               $out[] = ('<div class="col-md-6 col-md-offset-2"><div class="cropFishEyeLargeBottomPreview"> <img alt="'.$errorText.'" width="640px" src="'.$url.'/mjpg/video.mjpg?camera=1'.'&timestamp=' . strtotime("now") . '"/></div></div>  ' . "\n\r");   //Axis M3007
                    break;
                default:
                    $out[] = "   <div class='col-md-6 col-md-offset-2'><img  alt='$errorText' width=\"640px\" class='img-responsive' src='$url/axis-cgi/mjpg/video.cgi?resolution={$value['resVideo']}&dummy=$time'/></div>";
                    break;
            }

            if($key % 2 == 1){  //odd number
       $out[]='</div>';
    }   
             
      } 
      
     return  implode($out);
    
   
}//end function

/*
 * 
 * creates URL from host name and security type. 
 */
  public function urlGenerator($security,$host) {
      
     return $security.'://'.$host.'.'.$GLOBALS['config']['HTTP']['ROOT_DNS'];
      
      
  }
  
} //end trait
