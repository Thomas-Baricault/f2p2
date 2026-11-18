<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/responses/Data.php');


/**
 * Class for JSON responses
 */
class JSON extends \f2p2\http\responses\Data
{

    public function __construct(array $data, bool $isFile = false)
    {
        parent::__construct(json_encode($data), 'application/json', $isFile);
    }

}
