<?php

    namespace f2p2\responses;


    require_once('f2p2/responses/Response.php');


    /**
     * HTTP error response
     */
    class Error extends \f2p2\responses\Response
    {

        /**
         * @param int $code Error code
         */
        public function __construct(int $code)
        {
            $this->setCode($code);
        }

    }

?>
