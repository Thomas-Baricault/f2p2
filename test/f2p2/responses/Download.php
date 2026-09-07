<?php

    namespace f2p2\responses;


    /**
     * Class for responses that are downloads
     */
    class Download extends \f2p2\responses\Data
    {

        public function __construct(string $data, ?string $extension = null, bool $isFile = false, ?string $fileName = null)
        {
            parent::__construct($data, $extension, $isFile);
            if ($fileName)
                $fileName = $fileName;
            else if ($isFile)
            {
                $parts = explode('/', $data);
                $fileName = $parts[count($parts)-1];
            }
            $this->setHeader('Content-Disposition', 'attachment' . (
                $fileName
                    ? '; filename="' . $fileName . '"'
                    : ''
            ));
        }

    }

?>
