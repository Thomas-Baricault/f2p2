<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\routing;


require_once('f2p2/http/requests/Request.php');
require_once('f2p2/routing/Cache.php');
require_once('f2p2/routing/Route.php');
require_once('f2p2/utils/utils.php');


/**
 * Class that manages and executes routes
 */
final class Router
{

    /**
     * Current route
     */
    private static \f2p2\routing\Route $current;

    /**
     * Registered routes
     */
    private static array $routes = [];


    /**
     * Init the routes
     */
    public static function init(): void
    {
        $routes = \f2p2\routing\Cache::load();
        if ($routes)
        {
            self::$routes = $routes;
            return;
        }
        foreach (\f2p2\utils\discover('app/controllers') as $class)
        {
            $reflection = new \ReflectionClass($class);
            foreach ($reflection->getMethods() as $method)
            {
                foreach ($method->getAttributes(\f2p2\routing\Route::class) as $attribute)
                {
                    $route = $attribute->newInstance();
                    $route->controller = $class;
                    $route->action = $method->getName();
                    self::$routes[$route->name] = $route;
                }
            }
        }
        \f2p2\routing\Cache::save(self::$routes);
        return;
    }

    /**
     * Returns a route by its name
     * 
     * @param ?string $name Route name, `null` for current route
     * 
     * @return ?\f2p2\routing\Route Route if exists, `null` otherwise
     */
    public static function route(?string $name = null): ?\f2p2\routing\Route
    {
        if ($name === null)
            return self::$current;
        if (array_key_exists($name, self::$routes))
            return self::$routes[$name];
        return null;
    }

    /**
     * Runs the router for a request
     * 
     * @param \f2p2\http\requests\Request $request Request to run
     */
    public static function run(\f2p2\http\requests\Request $request): void
    {
        $status = 404;
        foreach (self::$routes as $route)
        {
            self::$current = $route;
            try
            {
                $result = $route->run($request);
            }
            catch (\Throwable $error)
            {
                if (!PRODUCTION)
                    throw $error;
                $status = 500;
                break;
            }
            if (!is_int($result))
                $result->send();
            else if ($result !== 404)
                $status = $result;
        }
        if (array_key_exists($status, self::$routes))
            self::$routes[$status]->call($request, ['status' => $status])->send();
        else
            (new \f2p2\http\responses\Error($status))->send();
        return;
    }

}
