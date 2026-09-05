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
        static public function getBaseURL() : string
        {
            return ($_SERVER['REQUEST_SCHEME'].'://'.$_SERVER['SERVER_NAME']);
        }

        /**
         * Returns the current request URI
         * 
         * @return string Request URI
         */
        static public function getCurrentURI() : string
        {
            return ($_SERVER['PATH_INFO'] ?? '/');
        }

        /**
         * Returns the current request URL
         * 
         * @return string Request URL
         */
        static public function getCurrentURL() : string
        {
            return (self::getBaseURL() . self::getCurrentURI());
        }

        /**
         * Returns the current request method
         * 
         * @return string Request method
         */
        static public function getRequestMethod() : string
        {
            return ($_SERVER['REQUEST_METHOD']);
        }

        /**
         * Returns the current request port
         * 
         * @return int Request port
         */
        static public function getServerPort() : int
        {
            return ($_SERVER['SERVER_PORT']);
        }

    }

?>
