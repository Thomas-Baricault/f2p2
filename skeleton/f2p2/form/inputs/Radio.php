<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class Radio extends \f2p2\form\inputs\Input
{

    public const TYPE = 'radio';

    protected bool $found = false;


    public function __construct(string $name, string $value)
    {
        parent::__construct($name);
        $this->set('value', $value);
        return;
    }

    #[\Override]
    public function fill(array $data): void
    {
        if (!$this->has('disabled') && isset($data[$this->name]) && is_string($data[$this->name]))
        {
            $this->found = true;
            if ($data[$this->name] === $this->get('value'))
                $this->set('checked');
        }
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        return !$this->has('required') || $this->found;
    }

}
