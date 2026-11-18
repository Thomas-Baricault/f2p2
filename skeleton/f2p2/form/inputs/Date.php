<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');
require_once('f2p2/utils/utils.php');


class Date extends \f2p2\form\inputs\Input
{

    public const TYPE = 'date';

    public const FORMAT = 'Y-m-d';


    public function min(string $value): static
    {
        if (!\f2p2\utils\validateDate(static::FORMAT, $value))
            throw new \ValueError('value has to match date format');
        $this->set('min', $value);
        return $this;
    }

    public function max(string $value): static
    {
        if (!\f2p2\utils\validateDate(static::FORMAT, $value))
            throw new \ValueError('value has to match date format');
        $this->set('max', $value);
        return $this;
    }

    #[\Override]
    public function validate(): bool
    {
        if (!parent::validate())
            return (false);
        if (!$this->has('value'))
            return (true);
        if (!\f2p2\utils\validateDate(static::FORMAT, $this->attrs['value']))
            return (false);
        $value = new \DateTime($this->attrs['value']);
        return (
            (!$this->has('min') || $value >= new \DateTime($this->attrs['min'])) &&
            (!$this->has('max') || $value <= new \DateTime($this->attrs['max']))
        );
    }

}
