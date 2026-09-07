<?php

    namespace f2p2\responses;


    /**
     * Class for responses displayed as a webpage
     */
    class Page extends \f2p2\responses\Data
    {

        public function __construct(string $data, ?string $extension = null, bool $isFile = false)
        {
            parent::__construct($data, $extension, $isFile);
            $this->setHeader('Content-Disposition', 'inline');
        }

    }

?>
