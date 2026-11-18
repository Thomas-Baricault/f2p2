<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\routing;


require_once('f2p2/cache/Cache.php');
require_once('f2p2/cache/Infos.php');


if (!defined('USE_CACHED_ROUTES'))
    define('USE_CACHED_ROUTES', false);


#[\f2p2\cache\Infos('routes', USE_CACHED_ROUTES)]
final class Cache extends \f2p2\cache\Cache
{
}
