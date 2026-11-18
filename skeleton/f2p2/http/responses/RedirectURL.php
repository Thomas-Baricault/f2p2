<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/responses/Response.php');


/**
 * Class for responses that are redirections to URL
 */
class RedirectURL extends \f2p2\http\responses\Response
{

    /**
     * @param string $url Redirection URL
     */
    public function __construct(string $url)
    {
        $this
            ->code(302)
            ->header('Location', $url);
    }

}
