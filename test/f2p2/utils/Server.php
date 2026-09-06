<?php

    namespace F2P2;


    /**
     * Class that represent the actual state of the server
     */
    class Server
    {

        /**
         * Returns the site base URL
         * 
         * @return string Base URL
         */
        public static function baseURL() : string
        {
            return ($_SERVER['REQUEST_SCHEME'].'://'.$_SERVER['SERVER_NAME']);
        }

        /**
         * Returns the current request URI
         * 
         * @return string Request URI
         */
        public static function currentURI() : string
        {
            return ($_SERVER['PATH_INFO'] ?? '/');
        }

        /**
         * Returns the current request URL
         * 
         * @return string Request URL
         */
        public static function currentURL() : string
        {
            return (self::baseURL() . self::currentURI());
        }

        /**
         * Returns the current request method
         * 
         * @return string Request method
         */
        public static function requestMethod() : string
        {
            return ($_SERVER['REQUEST_METHOD']);
        }

        /**
         * Returns the current request port
         * 
         * @return int Request port
         */
        public static function serverPort() : int
        {
            return ($_SERVER['SERVER_PORT']);
        }

    }

?>
