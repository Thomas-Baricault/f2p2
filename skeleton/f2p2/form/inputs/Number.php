<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class Number extends \f2p2\form\inputs\Input
{

    public const TYPE = 'number';


    public function min(int $value): static
    {
        $this->set('min', $value);
        return $this;
    }

    public function max(int $value): static
    {
        $this->set('max', $value);
        return $this;
    }

    public function step(int $value): static
    {
        if ($value <= 0)
            throw new \ValueError('step must be greater than 0');
        $this->set('step', $value);
        return $this;
    }

    #[\Override]
    public function validate(): bool
    {
        if (!parent::validate())
            return false;
        if (!$this->has('value'))
            return true;
        if (!filter_var($this->attrs['value'], FILTER_VALIDATE_INT))
            return false;
        return (
            (!$this->has('min') || $this->attrs['value'] >= $this->attrs['min']) &&
            (!$this->has('max') || $this->attrs['value'] <= $this->attrs['max']) &&
            (!$this->has('step') || (isset($this->attrs['min'])
                ? $this->attrs['value'] - $this->attrs['min']
                : $this->attrs['value']
            ) % $this->attrs['step'] === 0)
        );
    }

}
