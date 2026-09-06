<?php

    namespace F2P2;


    /**
     * An useful class for views
     */
    class View
    {

        /**
         * Render a view
         * 
         * @param string $name Name of the view
         * @param array $params An array of values that can be used in the view file
         * 
         * @return string HTML result
         */
        public static function render(string $name, array $params = []) : string
        {
            ob_start();
            require('app/views/' . $name);
            return (ob_get_clean());
        }

        /**
         * Returns the URL to an asset
         * 
         * @param string $path Path of the asset
         * 
         * @return string Resulting URL
         */
        public static function asset(string $path) : string
        {
            return (\F2P2\Server::baseURL() . '/' . $path);
        }

        /**
         * Returns the URL to a CSS file
         * 
         * @param string $path Path of the CSS file
         * 
         * @return string Resulting URL
         */
        public static function css(string $path) : string
        {
            return (self::asset("css/$path"));
        }
    
        /**
         * Returns the URL to a Javascript file
         * 
         * @param string $path Path of the Javascript file
         * 
         * @return string Resulting URL
         */
        public static function js(string $path) : string
        {
            return (self::asset("js/$path"));
        }
    
        /**
         * Returns the URL to an image
         * 
         * @param string $path Path of the image
         * 
         * @return string Resulting URL
         */
        public static function img(string $path) : string
        {
            return self::asset("img/$path");
        }
    
        /**
         * Returns the URL of a route
         * 
         * @param string $name Name of the route
         * @param array $params Route parameters
         * @param array $get GET values
         * 
         * @return string Resulting URL
         */
        public static function route(string $name, array $params = [], array $get = []) : string
        {
            return (\F2P2\Server::baseURL() . \F2P2\Router::getRoute($name)->computeURI($params, $get));
        }

    }

?>
