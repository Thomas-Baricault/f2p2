<?php

    require_once('config/env.php');
    require_once('config/db.php');

    require_once('f2p2/autoload.php');


    \F2P2\Language::init();

    \F2P2\Router::addRoutes(require_once('config/routes.php'));
    \F2P2\Router::run(\F2P2\Request::current());

?>
