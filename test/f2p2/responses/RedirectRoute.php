<?php

    namespace f2p2\responses;


    require_once('f2p2/mvc/View.php');
    require_once('f2p2/responses/RedirectURL.php');


    /**
     * Class for responses that are redirections to route
     */
    class RedirectRoute extends \f2p2\responses\RedirectURL
    {

        /**
         * @param string $route Redirection route name
         * @param array $params Route parameters
         * @param array $get GET parameters
         */
        public function __construct(string $route, array $params = [], array $get = [])
        {
            parent::__construct(\f2p2\View::route($route, $params, $get));
        }

    }

?>
