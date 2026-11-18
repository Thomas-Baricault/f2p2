<?php

define('PRODUCTION', false);

if (PRODUCTION)
{
    define('USE_CACHED_ROUTES', true);
    define('USE_CACHED_MODELS', true);
}
else
{
    define('USE_CACHED_ROUTES', false);
    define('USE_CACHED_MODELS', false);
}

define('LANG', 'en');
