<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Text.php');


class URL extends \f2p2\form\inputs\Text
{

    public const TYPE = 'url';


    #[\Override]
    public function validate(): bool
    {
        if (!parent::validate())
            return false;
        if (!$this->has('value'))
            return true;
        return filter_var($this->attrs['value'], FILTER_VALIDATE_URL) !== false;
    }

}
