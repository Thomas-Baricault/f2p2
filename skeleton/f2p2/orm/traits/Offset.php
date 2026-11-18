<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\traits;


trait Offset
{

    private ?string $offset = null;


    public function __toString(): string
    {
        return $this->offset ?? '';
    }

    public function offset(int $value): static
    {
        if ($this->offset)
            throw new \RuntimeException('offset clause already defined');
        $this->offset = "OFFSET $value";
        return $this;
    }

}
