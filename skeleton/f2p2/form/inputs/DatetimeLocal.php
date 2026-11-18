<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Date.php');


class DatetimeLocal extends \f2p2\form\inputs\Date
{

    public const TYPE = 'datetime-local';

    public const FORMAT = 'Y-m-d\TH:i';

}
