<?php

    require_once('config/env.php');

    require_once('f2p2/autoload.php');


    \f2p2\Language::init();

    \f2p2\routing\Router::addRoutes(require_once('config/routes.php'));
    \f2p2\routing\Router::run(\f2p2\requests\Request::current());

?>
