<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/responses/Response.php');


/**
 * HTTP data response
 */
class Data extends \f2p2\http\responses\Response
{

    public function __construct(string $data, ?string $mime = null, bool $isFile = false)
    {
        $this
            ->header('Content-Type', [
                $mime ?? ($isFile
                    ? (new \finfo(FILEINFO_MIME_TYPE))->file($data)
                    : (new \finfo(FILEINFO_MIME_TYPE))->buffer($data)),
                'charset=utf-8',
            ])
            ->body($isFile
                ? file_get_contents($data)
                : $data
            );
    }

}
