<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class Submit extends \f2p2\form\inputs\Input
{

    public const TYPE = 'submit';


    public function __construct(string $name, string $text)
    {
        parent::__construct($name);
        $this->set('value', $text);
        $this->required();
        return;
    }

}
