<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\models;


#[\Attribute(\Attribute::TARGET_PROPERTY)]
final class Reference
{

    public string $model;

    public string $field;


    public static function __set_state(array $data): self
    {
        return new self(...$data);
    }

    public function __construct(string $model, string $field)
    {
        $this->model = $model;
        $this->field = $field;
        return;
    }

}

