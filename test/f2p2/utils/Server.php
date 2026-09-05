<?php
    namespace F2P2;


    /**
     * The class that represent the actual state of the server
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Server
    {
        /**
         * Returns the site base URL
         * 
         * @return string The URL
         */
        static public function getBaseURL() : string
        {
            return $_SERVER['REQUEST_SCHEME'].'://'.$_SERVER['SERVER_NAME'];
        }

        /**
         * Returns the current request URI
         * 
         * @return string The URI
         */
        static public function getCurrentURI() : string
        {
            return $_SERVER['PATH_INFO'] ?? '/';
        }

        /**
         * Returns the current request URL
         * 
         * @return string The URL
         */
        static public function getCurrentURL() : string
        {
            return self::getBaseURL().self::getCurrentURI();
        }

        /**
         * Returns the current request method
         * 
         * @return string The method
         */
        static public function getRequestMethod() : string
        {
            return $_SERVER['REQUEST_METHOD'];
        }

        /**
         * Returns the current request port
         * 
         * @return int The port
         */
        static public function getServerPort() : int
        {
            return $_SERVER['SERVER_PORT'];
        }
    }
?>