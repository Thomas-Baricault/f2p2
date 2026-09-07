<?php

    namespace f2p2\responses;


    require_once('f2p2/responses/Data.php');


    /**
     * Class for responses displayed as a webpage
     */
    class Page extends \f2p2\responses\Data
    {

        public function __construct(string $data, bool $isFile = false)
        {
            parent::__construct($data, $isFile);
            $this->setHeader('Content-Disposition', 'inline');
        }

    }

?>
