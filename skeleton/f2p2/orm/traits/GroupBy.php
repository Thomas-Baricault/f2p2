<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\traits;


trait GroupBy
{

    private array $groupBy = [];


    public function __toString(): string
    {
        return $this->groupBy
            ? 'GROUP BY ' . implode(', ', $this->groupBy)
            : '';
    }

    public function groupBy(string|array $column): static
    {
        if (is_string($column))
            $this->groupBy[] = $column;
        else
            $this->groupBy = $column;
        return $this;
    }

}
