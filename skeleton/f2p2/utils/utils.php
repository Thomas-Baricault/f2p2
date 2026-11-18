<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\utils;


function discover(string $path): array
{
    if (is_dir($path))
    {
        $items = [];
        foreach (scandir($path) as $entry)
            if ($entry !== '.' && $entry != '..')
                $items = array_merge($items, discover("$path/$entry"));
        return $items;
    }
    else if (is_file($path))
    {
        $result = require_once($path);
        if (is_string($result))
            return [$result];
    }
    return [];
}

function validateDate(string $value, string $format)
{
    $date = \DateTimeImmutable::createFromFormat('!' . $format, $value);
    return $date !== false && $date->format($format) === $value;
}
