<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\routing;


require_once('f2p2/http/requests/Request.php');
require_once('f2p2/http/Method.php');


#[\Attribute(\Attribute::TARGET_METHOD | \Attribute::IS_REPEATABLE)]
final class Route
{

    /**
     * Name
     */
    public string $name;

    /**
     * HTTP Method
     */
    private \f2p2\http\Method $method;

    /**
     * Permissions needed to access the route
     */
    public ?array $permissions;

    /**
     * Associated controller
     */
    public ?string $controller = null;

    /**
     * Associated controller method
     */
    public ?string $action = null;

    /**
     * URI components
     */
    private array $uri;


    /**
     * Sanitizes an URI and return an array of components
     * 
     * @param string $uri URI to sanitize
     * 
     * @return array URI components
     */
    private static function sanitizeURI(string $uri): array
    {
        $length = strlen($uri);
        $hasBeginSlash = $length > 0 && $uri[0] === '/';
        $hasEndSlash = $length > 1 && $uri[$length-1] === '/';
        $uri = substr($uri, $hasBeginSlash, $length-$hasBeginSlash-$hasEndSlash);
        return $uri === ''
            ? []
            : explode('/', $uri);
    }

    public static function __set_state(array $data): self
    {
        $route = new self('', '');
        foreach ($data as $property => $value)
            $route->$property = $value;
        return $route;
    }

    /**
     * @param string $name Route name
     * @param string $uri Route URI
     * @param \f2p2\http\Method $method HTTP method
     * @param ?array $permissions Permissions needed to access the route
     */
    public function __construct(string $name, string $uri, \f2p2\http\Method $method = \f2p2\http\Method::Get, ?array $permissions = null)
    {
        $this->name = $name;
        $this->uri = self::sanitizeURI($uri);
        $this->method = $method;
        $this->permissions = $permissions;
        return;
    }

    /**
     * Computes the route URI
     * 
     * @param array $params URI parameters
     * @param array $get GET parameters
     * 
     * @return string Resulting URI
     * 
     * @throws \RuntimeException If a parameter is missing
     */
    public function uri(array $params = [], array $get = []): string
    {
        $uri = '';
        foreach ($this->uri as $arg)
        {
            if ($arg[0] === '$')
            {
                $param = substr($arg, 1);
                if (!array_key_exists($param, $params))
                    throw new \RuntimeException("missing parameter '$param'");
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
        return $uri;
    }

    /**
     * Runs the route for a request
     * 
     * @param \f2p2\http\requests\Request $request Request to run
     * @param ?array &$sanitizedURI Cached sanitized request URI
     * 
     * @return int Response status code or 404 if request doesn't correspond to the route
     * 
     * @throws \RuntimeException If the route is not associated to a controller method
     */
    public function run(\f2p2\http\requests\Request $request, ?array &$sanitizedURI = null): int | \f2p2\http\responses\Response
    {
        if ($this->controller === null || $this->action === null)
            throw new \RuntimeException('route not associated to a controller method');
        if ($sanitizedURI === null)
            $sanitizedURI = self::sanitizeURI($request->url);
        $length = count($sanitizedURI);
        if ($length !== count($this->uri))
            return 404;
        $params = [];
        for ($i = 0; $i < $length; $i++)
        {
            if ($this->uri[$i][0] === '$')
                $params[substr($this->uri[$i], 1)] = $sanitizedURI[$i];
            else if ($sanitizedURI[$i] !== $this->uri[$i])
                return 404;
        }
        if ($request->method !== $this->method)
            return 405;
        // if ($this->permissions)
        // {
        //     if (!isset($_SESSION['user']))
        //         return 401;
        //     foreach ($this->permissions as $permission)
        //         if (!in_array($permission, $_SESSION['user']->getPermissions()))
        //             return 401;
        // }
        return $this->call($request, $params);
    }

    /**
     * Call the route action
     * 
     * @param \f2p2\http\requests\Request $request Request to call for
     * @param array $params Call parameters
     * 
     * @return \f2p2\http\responses\Response Response
     */
    public function call(\f2p2\http\requests\Request $request, array $params = []): \f2p2\http\responses\Response
    {
        require_once(str_replace('\\', '/', $this->controller) . '.php');
        return (new $this->controller())->{$this->action}($request, $params);
    }

}
