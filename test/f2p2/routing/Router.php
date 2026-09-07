<?php

    namespace f2p2\routing;


    require_once('f2p2/requests/Request.php');
    require_once('f2p2/routing/Route.php');


    /**
     * Class that manages and executes routes
     */
    final class Router
    {

        /**
         * Registered routes
         */
        private static array $routes = [];


        /**
         * Returns a route by its name
         * 
         * @param string $name Route name
         * 
         * @return ?\f2p2\routing\Route Route if exists, `null` otherwise
         */
        public static function getRoute(string $name): ?\f2p2\routing\Route
        {
            if (array_key_exists($name, self::$routes))
                return (self::$routes[$name]);
            return (null);
        }

        /**
         * Adds routes to the router
         * 
         * @param array $routes Routes to add (name => route)
         */
        public static function addRoutes(array $routes): void
        {
            self::$routes = array_merge(self::$routes, $routes);
        }

        /**
         * Adds a route to the router
         * 
         * @param string $name Name of the route
         * @param array $route Route to add
         */
        public static function addRoute(string $name, array $route): void
        {
            self::$routes[$name] = $route;
        }

        /**
         * Runs the router for a request
         * 
         * @param \f2p2\requests\Request $request Request to run
         */
        public static function run(\f2p2\requests\Request $request): void
        {
            $status = 404;
            foreach (self::$routes as $route)
            {
                $status = $route->run($request);
                if ($status !== 404)
                {
                    if (array_key_exists($status, self::$routes))
                        self::$routes[$status]->call($request, ['status' => $status]);
                    else
                        (new \f2p2\responses\Error($status))->send();
                }
            }
        }

    }

?>
