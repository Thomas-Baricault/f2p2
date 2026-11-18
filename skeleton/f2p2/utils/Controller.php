<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2;


/**
 * Abstract base class for controllers
 */
abstract class Controller
{

    protected static function render($params = [])
    {
        return new \f2p2\http\responses\Page(\f2p2\View::render('template', $params));
    }

}

