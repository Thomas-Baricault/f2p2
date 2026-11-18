<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2;


/**
 * Class that represent the actual state of the server
 */
final class Server
{

    /**
     * Returns the site base URL
     * 
     * @return string Base URL
     */
    public static function baseURL(): string
    {
        return ($_SERVER['REQUEST_SCHEME'] ?? 'http') . '://' . $_SERVER['SERVER_NAME'];
    }

    /**
     * Returns the current request URI
     * 
     * @return string Request URI
     */
    public static function currentURI(): string
    {
        return $_SERVER['PATH_INFO'] ?? '/';
    }

    /**
     * Returns the current request URL
     * 
     * @return string Request URL
     */
    public static function currentURL(): string
    {
        return self::baseURL() . self::currentURI();
    }

    /**
     * Returns the current request method
     * 
     * @return \f2p2\http\Method Request method
     */
    public static function requestMethod(): \f2p2\http\Method
    {
        return \f2p2\http\Method::from($_SERVER['REQUEST_METHOD']);
    }

    /**
     * Returns the current request port
     * 
     * @return int Request port
     */
    public static function serverPort(): int
    {
        return $_SERVER['SERVER_PORT'];
    }

}
