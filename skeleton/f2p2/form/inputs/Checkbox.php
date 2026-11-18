<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class Checkbox extends \f2p2\form\inputs\Input
{

    public const TYPE = 'checkbox';


    public function __construct(string $name, string $value)
    {
        parent::__construct($name);
        if (str_ends_with($this->name, '[]'))
            $this->name = substr($this->name, 0, -2);
        $this->set('value', $value);
        return;
    }

    #[\Override]
    public function fill(array $data): void
    {
        if (!$this->has('disabled') && isset($data[$this->name]) && (
            (is_string($data[$this->name]) && $this->name === $this->get('name') && $data[$this->name] === $this->get('value')) ||
            (is_array($data[$this->name]) && $this->name !== $this->get('name') && in_array($this->get('value'), $data[$this->name]))
        ))
            $this->set('checked');
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        return !$this->has('required') || $this->has('checked');
    }

}
