<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http;


abstract class Message
{

    public array $headers = [];


    public function header(string $name, null|string|array $value = null): null|string|static
    {
        if ($value === null)
        {
            return array_key_exists($name, $this->headers)
                ? $this->headers[$name]
                : null;
        }
        $this->headers[$name] = is_string($value)
            ? $value
            : implode('; ', $value);
        return $this;
    }

    public function headers(array $headers): static
    {
        foreach ($headers as $name => $value)
            $this->header($name, $value);
        return $this;
    }

}
