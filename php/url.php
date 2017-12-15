<?php

function parseUrl($url, $decode = false)
{
    $urlData = parse_url($url);
    if (empty($urlData['query'])) { return null; }
    $query = explode("&", $urlData['query']);
    $parameters = array();
    foreach($query as $parameter) {
        $param = explode("=", $parameter);
        if (!empty($param) && count($param) == 2)
            $parameters[$param[0]] = $decode == true ? urldecode($param[1]) : $param[1];
    }
    return $parameters;
}

$out=array();
$url = $_SERVER['REQUEST_URI'];
$out["url"] = $url;
$out["parsed"] = parseUrl($url);
$out["body"] = $body = file_get_contents("php://input");

if($body!="") {
    $obj = json_decode($body);
} else {
    $obj = NULL;
}
$out["decoded"]=$obj;
echo json_encode($out);
