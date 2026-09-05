<?php
    require('../config/env.php');
    require('../config/db.php');
    require('../f2p2/autoload.php');

    \F2P2\Language::init(\F2P2\readJSON('../config/languages.json'));

    \F2P2\Router::addRoutes(\F2P2\readJSON('../config/routes.json'));
    \F2P2\Router::run(\F2P2\Request::fromCurrent());
?>