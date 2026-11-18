<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\traits;


trait Where
{

    private ?string $where = null;


    public function __toString(): string
    {
        return $this->where ?? '';
    }

    public function where(string $value, array $prepare = []): static
    {
        if ($this->where)
            $this->where .= ' AND ';
        else
            $this->where = 'WHERE ';
        $this->where .= $value;
        $this->prepare += $prepare;
        return $this;
    }

}
