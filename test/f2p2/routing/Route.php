<?php

    namespace f2p2\routing;


    final class Route
    {

        /**
         * URI components
         */
        private ?array $uri = null;

        /**
         * Method
         */
        private ?string $method = null;

        /**
         * Controller associated
         */
        private ?string $controller = null;

        /**
         * Action associated
         */
        private ?string $action = null;

        /**
         * Whether authentication is needed to access the route
         */
        private bool $auth = false;

        /**
         * Permissions needed to access the route
         */
        private array $permissions = [];


        /**
         * Sanitizes an URI and return an array of components
         * 
         * @param string $uri URI to sanitize
         * 
         * @return array URI components
         */
        public static function sanitizeURI(string $uri): array
        {
            $length = strlen($uri);
            $hasBeginSlash = $length > 0 && $uri[0] === '/';
            $hasEndSlash = $length > 1 && $uri[$length-1] === '/';
            $uri = substr($uri, $hasBeginSlash, $length-$hasBeginSlash-$hasEndSlash);
            return (
                $uri === ''
                    ? []
                    : explode('/', $uri)
            );
        }

        /**
         * Sets the route URI
         * 
         * @param string $uri Route URI
         * 
         * @return Route Reference to this route
         */
        public function uri(string $uri): Route
        {
            $this->uri = self::sanitizeURI($uri);
            return ($this);
        }

        /**
         * Sets the route method
         * 
         * @param string $method Route method
         * 
         * @return Route Reference to this route
         */
        public function method(string $method): Route
        {
            $this->method = $method;
            return ($this);
        }

        /**
         * Sets the route action
         * 
         * @param string $controller Controller to call
         * @param string $action Action to call on the controller
         * 
         * @return Route Reference to this route
         */
        public function action(string $controller, string $action): Route
        {
            $this->controller = $controller;
            $this->action = $action;
            return ($this);
        }

        /**
         * Make the authentication needed to access the route
         * 
         * @param array $permissions Permissions needed to access the route
         * 
         * @return Route Reference to this route
         */
        public function auth(array $permissions = []): Route
        {
            $this->auth = true;
            $this->permissions = $permissions;
            return ($this);
        }

        /**
         * Computes the route URI
         * 
         * @param array $params URI parameters
         * @param array $get GET parameters
         * 
         * @return string Resulting URI
         */
        public function computeURI(array $params = [], array $get = []): string
        {
            $uri = '';
            foreach ($this->$uri as $arg)
            {
                if ($arg[0] === '$')
                {
                    $param = substr($arg, 1);
                    if (!array_key_exists($param, $params))
                        throw new \Exception("Undefined parameter '$param'");
                    $uri .= '/' . $params[$param];
                }
                else
                    $uri .= '/' . $arg;
            }
            if (count($get) > 0)
            {
                $args = [];
                foreach ($get as $key => $value)
                    $args[] = "$key=$value";
                $uri .= '?' . implode('&', $args);
            }
            return ($uri);
        }

        /**
         * Runs the route for a request
         * 
         * @param \f2p2\requests\Request $request Request to run
         * @param ?array &$sanitizedURI Cached sanitized request URI
         * 
         * @return int Response status code or 404 if request doesn't correspond to the route
         * 
         * @throws \Exception If the route is not correctly configured
         */
        public function run(\f2p2\requests\Request $request, ?array &$sanitizedURI = null): int
        {
            if ($this->uri === null || $this->method === null || $this->controller === null || $this->action === null)
                throw new \Exception('Route not correctly configured');
            if ($sanitizedURI === null)
                $sanitizedURI = self::sanitizeURI($request->getURL());
            $length = count($sanitizedURI);
            if ($length !== count($this->uri))
                return (404);
            $params = [];
            for ($i = 0; $i < $length; $i++)
            {
                if ($this->uri[$i][0] === '$')
                    $params[substr($this->uri[$i], 1)] = $sanitizedURI[$i];
                else if ($sanitizedURI[$i] !== $this->uri[$i])
                    return (404);
            }
            if ($request->getMethod() !== $this->method)
                return (405);
            if ($this->auth)
            {
                if (!isset($_SESSION['user']))
                    return (401);
                foreach ($this->permissions as $permission)
                    if (!in_array($permission, $_SESSION['user']->getPermissions()))
                        return (401);
            }
            $this->call($request, $params);
            return (200);
        }

        /**
         * Call the route action
         * 
         * @param \f2p2\requests\Request $request Request to call for
         * @param array $params Call parameters
         */
        public function call(\f2p2\requests\Request $request, array $params = []): void
        {
            require_once("app/controllers/{$this->controller}Controller.php");
            ((new ("\\App\\{$this->controller}Controller")())->{$this->action}($request, $params))->send();
        }

    }

?>
