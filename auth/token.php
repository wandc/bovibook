<?php 
// include our OAuth2 Server object
require_once __DIR__.'/server.php';

// Handle a request for an OAuth2.0 Access Token and send the response to the client
//$server->handleTokenRequest(OAuth2\Request::createFromGlobals())->send();  //this line does the same as the 4 lines below.


$request = OAuth2\Request::createFromGlobals();
$response = new OAuth2\Response();

// will set headers, status code, and json response appropriately for success or failure
$server->handleTokenRequest($request, $response);
$response->send();



?>