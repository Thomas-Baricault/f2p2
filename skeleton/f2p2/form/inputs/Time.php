<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Date.php');
require_once('f2p2/utils/regex.php');


class Time extends \f2p2\form\inputs\Date
{

    public const TYPE = 'time';

    public const PATTERN = \f2p2\regex\TIME;

}
