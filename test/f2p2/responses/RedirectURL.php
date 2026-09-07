<?php

    namespace f2p2\responses;


    require_once('f2p2/responses/Response.php');


    /**
     * Class for responses that are redirections to URL
     */
    class RedirectURL extends \f2p2\responses\Response
    {

        /**
         * @param string $url Redirection URL
         */
        public function __construct(string $url)
        {
            $this
                ->setCode(302)
                ->setHeader('Location', $url);
        }

    }

?>
