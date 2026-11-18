<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');
require_once('f2p2/utils/regex.php');


class Color extends \f2p2\form\inputs\Input
{

    public const TYPE = 'color';


    public function __construct(string $name)
    {
        parent::__construct($name);
        $this->required();
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        return parent::validate() && (!$this->has('value') || preg_match(\f2p2\regex\COLOR, $this->attrs['value']));
    }

}
