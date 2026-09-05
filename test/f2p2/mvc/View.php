<?php
    namespace F2P2;


    /**
     * An useful class for the views
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class View
    {
        /**
         * Render a view
         * 
         * @param  string $name   The name of the view
         * @param  array  $params An array of values that can be called in the view file
         * @return string         The html result
         */
        static public function render(
            string $name,
            array  $params = []
        ) : string
        {
            ob_start();
            require('../app/views/'.$name);
            return ob_get_clean();
        }

        /**
         * Returns the URL to an asset
         * 
         * @param  string $name The name of the asset
         * @return string       The URL
         */
        static public function asset(
            string $name
        ) : string
        {
            return \F2P2\Server::getBaseURL().'/'.$name;
        }

        /**
         * Returns the URL to a CSS file
         * 
         * @param  string $name The name of the CSS file
         * @return string       The URL
         */
        static public function css(
            string $name
        ) : string
        {
            return self::asset("css/$name");
        }
    
        /**
         * Returns the URL to a Javascript file
         * 
         * @param  string $name The name of the Javascript file
         * @return string       The URL
         */
        static public function js(
            string $name
        ) : string
        {
            return self::asset("js/$name");
        }
    
        /**
         * Returns the URL to an image
         * 
         * @param  string $name The name of the image
         * @return string       The URL
         */
        static public function img(
            string $name
        ) : string
        {
            return self::asset("img/$name");
        }
    
        /**
         * Returns the URL of a route
         * 
         * @param  string $name   The name of the route
         * @param  array  $params The array of the route required parameters
         * @param  array  $get    An array of get values
         * @return string         The URL
         */
        static public function route(
            string $name,
            array  $params = [],
            array  $get    = []
        ) : string
        {
            return \F2P2\Router::getRouteURL($name, $params, $get);
        }
    }
?>