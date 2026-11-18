<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/responses/Data.php');


/**
 * Class for responses displayed as a webpage
 */
class Page extends \f2p2\http\responses\Data
{

    public function __construct(string $data, bool $isFile = false)
    {
        parent::__construct($data, 'text/html', $isFile);
        $this->header('Content-Disposition', 'inline');
    }

}
