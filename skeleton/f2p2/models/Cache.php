<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\models;


require_once('f2p2/cache/Cache.php');
require_once('f2p2/cache/Infos.php');


if (!defined('USE_CACHED_MODELS'))
    define('USE_CACHED_MODELS', false);


#[\f2p2\cache\Infos('models', USE_CACHED_MODELS)]
final class Cache extends \f2p2\cache\Cache
{
}
