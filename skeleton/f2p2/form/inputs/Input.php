<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/Field.php');


abstract class Input extends \f2p2\form\Field
{

    public const TYPE = '';


    public function __construct(string $name)
    {
        parent::__construct('input', null, $name);
        $this->set('type', static::TYPE);
        return;
    }

    #[\Override]
    public function fill(array $data): void
    {
        if (!$this->has('disabled') && isset($data[$this->name]) && is_string($data[$this->name]))
            $this->set('value', $data[$this->name]);
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        return !($this->has('required') && (!$this->has('value') || $this->attrs['value'] === ''));
    }

}
