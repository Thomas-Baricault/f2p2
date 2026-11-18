<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/responses/Response.php');


/**
 * HTTP error response
 */
class Error extends \f2p2\http\responses\Response
{

    /**
     * @param int $code Error code
     */
    public function __construct(int $code)
    {
        $this->code($code);
    }

}
