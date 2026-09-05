<?php 
    namespace F2P2;


    /**
     * The class that manages and executes routes
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Router
    {
        static private array $routes = [];
    

        /**
         * Add routes to the router
         * 
         * @param  array $routes The routes
         * @return void
         */
        static public function addRoutes(
            array $routes
        ) : void
        {
            foreach ($routes as $route)
            {
                self::addRoute(
                    $route['name'],
                    $route['method'],
                    $route['uri'],
                    $route['controller'],
                    $route['action'],
                    array_key_exists('logged', $route) ? $route['logged'] : null,
                    array_key_exists('permissions', $route) ? $route['permissions'] : null,
                );
            }
        }

        /**
         * Add a route to the router
         * 
         * @param  string $name        The route name
         * @param  string $method      The route access method
         * @param  string $uri         The route URI
         * @param  string $controller  The route controller name
         * @param  string $action      The route action of the controller
         * @param  ?bool  $logged      true if the user must be logged in to access this route
         * @param  ?array $permissions An array of permissions needed to access this route
         * @return void
         */
        static public function addRoute(
            string $name,
            string $method,
            string $uri,
            string $controller,
            string $action,
            ?bool  $logged      = null,
            ?array $permissions = null
        ) : void
        {
            self::$routes[$name] = [
                'method'      => $method,
                'uri'         => self::filterURI($uri),
                'controller'  => $controller,
                'action'      => $action,
                'logged'      => $logged,
                'permissions' => $permissions
            ];
        }

        /**
         * Generate URI for a route
         * 
         * @param  string $name   The route name
         * @param  array  $params The values to use for URI parameters
         * @param  array  $get    The GET arguments
         * @return string         The URI
         */
        static public function getRouteURI(
            string $name,
            array  $params = [],
            array  $get    = []
        ) : string
        {
            if (!isset(self::$routes[$name]))
            {
                throw new \Exception("No route named \"$name\"");
            }
            $uri = '';
            foreach (self::$routes[$name]['uri'] as $arg)
            {
                if ($arg[0] === '$')
                {
                    $param = substr($arg, 1);
                    if (!array_key_exists($param, $params))
                    {
                        throw new \Exception("Undefined parameter \"$param\"");
                    }
                    $uri .= '/'.$params[$param];
                }
                else
                {
                    $uri .= '/'.$arg;
                }
            }
            if (count($get) > 0)
            {
                $uri .= '?';
                $args = [];
                foreach ($get as $var => $value)
                {
                    $args[] = "$var=$value";
                }
                $uri .= implode('&', $args);
            }
            return $uri;
        }

        /**
         * Generate URL for a route
         * 
         * @param  string $name   The route name
         * @param  array  $params The values to use for URL parameters
         * @param  array  $get    The GET arguments
         * @return string         The URL
         */
        static public function getRouteURL(
            string $name,
            array  $params = [],
            array  $get    = []
        ) : string
        {
            return \F2P2\Server::getBaseURL().self::getRouteURI($name, $params, $get);
        }

        /**
         * Process a request in the router
         * 
         * @param  \F2P2\Request $request The request
         * @return void
         */
        static public function run(
            \F2P2\Request $request
        ) : void
        {
            $uri = self::filterURI($request->getURL());
            $length = count($uri);
            foreach (self::$routes as $name => $route)
            {
                if ($length === count($route['uri']))
                {
                    $params = [];
                    $valid = true;
                    for ($i = 0; $i < $length; $i++)
                    {
                        if ($route['uri'][$i][0] === '$')
                        {
                            $params[substr($route['uri'][$i], 1)] = $uri[$i];
                        }
                        else if ($uri[$i] !== $route['uri'][$i])
                        {
                            $valid = false;
                            break;
                        }
                    }
                    if ($valid)
                    {
                        if ($request->getMethod() !== $route['method'])
                        {
                            self::runError($request, 405);
                        }
                        else
                        {
                            if (USE_AUTH && isset($route['logged']) && $route['logged'])
                            {
                                if (!isset($_SESSION['user']))
                                {
                                    self::runError($request, 401);
                                }
                                else if (isset($route['permissions']))
                                {
                                    foreach ($route['permissions'] as $permission)
                                    {
                                        if (!in_array($permission, $_SESSION['user']->getPermissions()))
                                        {
                                            self::runError($request, 401);
                                        }
                                    }
                                }
                            }
                            require_once('app/controllers/'.$route['controller'].'Controller.php');
                            ((new ('\\App\\Controllers\\'.$route['controller'].'Controller')())->{$route['action']}($request, $params))->send();
                        }
                    }
                }
            }
            self::runError($request, 404);
        }

        /**
         * Process an error in the router
         * 
         * @param  \F2P2\Request $request The request that generated the error
         * @param  int           $code    The error code
         * @return void
         */
        static private function runError(
            \F2P2\Request $request,
            int           $code
        ) : void
        {
            if (file_exists('app/controllers/ErrorController.php'))
            {
                require_once('app/controllers/ErrorController.php');
                if (class_exists('\\App\\Controllers\\ErrorController') && method_exists('\\App\\Controllers\\ErrorController', "error$code"))
                {
                    ((new ('\\App\\Controllers\\ErrorController')())->{"error$code"}($request))->send();
                }
            }
            (new \F2P2\ResponseError($code))->send();
        }

        /**
         * Returns an array of filtered URI parts
         * 
         * @param  string $uri The URI
         * @return array       The parts
         */
        static private function filterURI(
            string $uri
        ) : array
        {
            $length = strlen($uri);
            $hasBeginSlash = $length > 0 && $uri[0] === '/';
            $hasEndSlash = $length > 1 && $uri[$length-1] === '/';
            $uri = substr($uri, $hasBeginSlash, $length-$hasBeginSlash-$hasEndSlash);
            return $uri === '' ? [] : explode('/', $uri);
        }
    }
?>
