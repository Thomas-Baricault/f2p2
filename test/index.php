<?php

    require_once('config/env.php');
    require_once('config/db.php');

    require_once('f2p2/autoload.php');


    \f2p2\Language::init();

    \f2p2\Router::addRoutes(require_once('config/routes.php'));
    \f2p2\Router::run(\f2p2\Request::current());

?>
