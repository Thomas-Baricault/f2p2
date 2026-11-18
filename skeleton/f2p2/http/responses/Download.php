<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/responses/Data.php');


/**
 * Class for responses that are downloads
 */
class Download extends \f2p2\http\responses\Data
{

    public function __construct(string $name, string $data, ?string $mime = null, bool $isFile = false)
    {
        parent::__construct($data, $mime, $isFile);
        $this->header('Content-Disposition', "attachment; filename=\"$name\"");
    }

}
