<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form;


require_once('f2p2/form/Field.php');


class TextArea extends \f2p2\form\Field
{

    public function __construct(string $name)
    {
        parent::__construct('textarea', true, $name);
        return;
    }

    public function minLength(int $value): static
    {
        $this->set('minlength', $value);
        return $this;
    }

    public function maxLength(int $value): static
    {
        $this->set('maxlength', $value);
        return $this;
    }

    public function placeholder(string $value): static
    {
        $this->set('placeholder', $value);
        return $this;
    }

    #[\Override]
    public function fill(array $data): void
    {
        if (!$this->has('disabled') && isset($data[$this->name]) && is_string($data[$this->name]))
            $this->content($data[$this->name]);
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        $value = count($this->children) === 1 && is_string($this->children[0])
            ? $this->children[0]
            : '';
        if ($this->get('required') && $value === '')
            return false;
        return (
            (!$this->get('minlength') || strlen($value) >= $this->attrs['minlength']) &&
            (!$this->get('maxlength') || strlen($value) <= $this->attrs['maxlength'])
        );
    }

}
