<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class Hidden extends \f2p2\form\inputs\Input
{

    public const TYPE = 'hidden';


    public function __construct(string $name)
    {
        parent::__construct($name);
        $this->required();
        return;
    }

}
