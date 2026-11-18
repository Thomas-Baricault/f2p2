<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\traits;


trait Limit
{

    private ?string $limit = null;


    public function __toString(): string
    {
        return $this->limit ?? '';
    }

    public function limit(int $value): static
    {
        if ($this->limit)
            throw new \RuntimeException('limit clause already defined');
        $this->limit = "LIMIT $value";
        return $this;
    }

}
