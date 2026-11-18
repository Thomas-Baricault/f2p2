<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class Text extends \f2p2\form\inputs\Input
{

    public const TYPE = 'text';


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

    public function pattern(string $value): static
    {
        $this->set(
            'pattern',
            $value[0] === '/' && $value[-1] === '/'
                ? substr($value, 1, -1)
                : $value
        );
        return $this;
    }

    public function placeholder(string $value): static
    {
        $this->set('placeholder', $value);
        return $this;
    }

    #[\Override]
    public function validate(): bool
    {
        if (!parent::validate())
            return false;
        if (!$this->has('value'))
            return true;
        return (
            (!$this->has('minlength') || strlen($this->attrs['value']) >= $this->attrs['minlength']) &&
            (!$this->has('maxlength') || strlen($this->attrs['value']) <= $this->attrs['maxlength'])
        );
    }

}
