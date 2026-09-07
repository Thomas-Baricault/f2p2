<?php

    namespace f2p2\responses;


    require_once('f2p2/responses/Response.php');


    /**
     * Abtract base class for HTTP data responses
     */
    abstract class Data extends \f2p2\responses\Response
    {

        public function __construct(string $data, bool $isFile = false)
        {
            $this
                ->setHeader('Content-Type', [
                    (
                        $isFile
                            ? (new \finfo(FILEINFO_MIME_TYPE))->file($data)
                            : (new \finfo(FILEINFO_MIME_TYPE))->buffer($data)
                    ),
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
