Node.php
========
Evented I/O for PHP

### Example HTTP server
    <?php
    
    $http = new node_http();
    
    $http->listen(8080, function($request, $response) {
        $response->end("yay, super awesome response");
    });
    
    nodephp_run();
    
    ?>

### To build:
First build nodephp as a php module

    git clone git://github.com/JosephMoniz/node.php.git
    cd node.php
    make
    sudo make install
    
Then you must add the following line to your php.ini

    extension=nodephp.so

Now you can go and run the example script

    php src/test.php

Now if you point your browser to [127.0.0.1:8080](http://127.0.0.1:8080) you 
should see the response being served by the test script via nodephp