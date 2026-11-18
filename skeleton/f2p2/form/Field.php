<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form;


require_once('f2p2/components/Component.php');


abstract class Field extends \f2p2\components\Component
{

    public string $name;


    public function __construct(string $tag, ?bool $close, string $name)
    {
        parent::__construct($tag, $close);
        $this->name = $name;
        $this->set('name', $name);
        return;
    }

    public function required(bool $value = true): static
    {
        if ($value)
            $this->set('required');
        return $this;
    }

    public function disabled(bool $value = true): static
    {
        if ($value)
            $this->set('disabled');
        return $this;
    }

    abstract public function fill(array $data): void;

    abstract public function validate(): bool;

}
