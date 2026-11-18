<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\cache;


#[\Attribute(\Attribute::TARGET_CLASS)]
final class Infos
{

    public string $path;

    public bool $enabled;


    public static function __set_state(array $data): self
    {
        return new self(...$data);
    }

    public function __construct(string $path, bool $enabled = true)
    {
        $this->path = $path;
        $this->enabled = $enabled;
        return;
    }

}
