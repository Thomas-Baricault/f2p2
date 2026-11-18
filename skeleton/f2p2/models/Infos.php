<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\models;


require_once('f2p2/models/Field.php');


#[\Attribute(\Attribute::TARGET_CLASS)]
final class Infos
{

    public string $engine;

    public string $charset;

    public string $collate;

    public string $table = '';

    public array $fields = [];

    public array $reference = [];


    public static function __set_state(array $data): self
    {
        $infos = new self();
        foreach ($data as $property => $value)
            $infos->{$property} = $value;
        return $infos;
    }

    public function __construct(string $engine = 'InnoDB', string $charset = 'utf8mb4', string $collate = 'utf8mb4_unicode_ci')
    {
        $this->engine = $engine;
        $this->charset = $charset;
        $this->collate = $collate;
        return;
    }

}
