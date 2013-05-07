<?php

$http = new node_http();

$http->listen(8080, function($request, $response) {
    $response->setStatus(200);
    $response->setHeader("Content-Type", "text/plain");
    $response->write("This is the data in the first chunk\r\n");
    $response->write("and this is the second one\r\n");
    $response->write("con");
    $response->end("sequence");
});

nodephp_run();
