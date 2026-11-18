<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\responses;


require_once('f2p2/http/Message.php');


/**
 * HTTP response
 */
class Response extends \f2p2\http\Message
{

    /**
     * Status code
     */
    public int $code = 200;

    /**
     * Response body
     */
    public ?string $body = null;


    /**
     * Sets the response status code
     * 
     * @param int $code Status code
     * 
     * @return static Reference to this response
     */
    public function code(int $code): static
    {
        $this->code = $code;
        return ($this);
    }

    /**
     * Sets the response body
     * 
     * @param string $body Response body
     * 
     * @return static Reference to this response
     */
    public function body(string $body): static
    {
        $this->body = $body;
        return ($this);
    }

    /**
     * Send the response
     */
    public function send(): void
    {
        http_response_code($this->code);
        foreach ($this->headers as $key => $value)
            header("$key: $value");
        if ($this->body)
            echo $this->body;
        die();
    }

}
