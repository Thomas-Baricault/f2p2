<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


require_once('config/env.php');
require_once('f2p2/f2p2.php');


session_start();


\f2p2\Language::init();

\f2p2\orm\Connection::init();
\f2p2\models\Model::init();

\f2p2\routing\Router::init();


try
{
    \f2p2\routing\Router::run(\f2p2\http\requests\Request::current());
}
catch (\Throwable $error)
{
    if (!PRODUCTION)
        throw $error;
    http_response_code(500);
    exit;
}
