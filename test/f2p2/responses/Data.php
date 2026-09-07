<?php

    namespace f2p2\responses;


    /**
     * Abtract base class for HTTP data responses
     */
    abstract class Data extends \f2p2\responses\Response
    {

        /**
         * Constructs a response
         * 
         * @param string $data Response data
         * @param ?string $extension Extension defining the type of data
         * @param bool $isFile Whether `$data` is a path to a file
         */
        public function __construct(string $data, ?string $extension = null, bool $isFile = false)
        {
            if ($isFile && $extension === null)
            {
                $parts = explode('.', $data);
                $extension = $parts[count($parts) - 1];
            }
            $this
                ->setHeader('Content-Type', [
                    $extension
                        ? \f2p2\extensionToMIME($extension)
                        : 'text/html',
                    'charset=utf-8',
                ])
                ->setBody(
                    $isFile
                        ? file_get_contents($data)
                        : $data
                );
        }

    }

?>
