<?php

    namespace F2P2;


    /**
     * Class that manages and executes routes
     */
    class Router
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
         * @return ?\F2P2\Route Route if exists, `null` otherwise
         */
        public static function getRoute(string $name) : ?\F2P2\Route
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
        public static function addRoutes(array $routes) : void
        {
            self::$routes = array_merge(self::$routes, $routes);
        }

        /**
         * Adds a route to the router
         * 
         * @param string $name Name of the route
         * @param array $route Route to add
         */
        public static function addRoute(string $name, array $route) : void
        {
            self::$routes[$name] = $route;
        }

        /**
         * Runs the router for a request
         * 
         * @param \F2P2\Request $request Request to run
         */
        public static function run(\F2P2\Request $request) : void
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
                        (new \F2P2\ErrorResponse($status))->send();
                }
            }
        }

    }

?>
