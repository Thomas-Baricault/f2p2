<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Date.php');
require_once('f2p2/utils/regex.php');


class Week extends \f2p2\form\inputs\Date
{

    public const TYPE = 'week';

    public const PATTERN = \f2p2\regex\WEEK;

}
