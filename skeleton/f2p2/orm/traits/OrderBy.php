<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\traits;


trait OrderBy
{

    private array $orderBy = [];


    public function __toString(): string
    {
        return $this->orderBy
            ? 'ORDER BY ' . implode(', ', $this->orderBy)
            : '';
    }

    public function orderBy(string|array $column, bool $ascending = true): static
    {
        if (is_string($column))
            $this->orderBy[] = $column . ' ' . ($ascending ? 'ASC' : 'DESC');
        else if (array_is_list($column))
        {
            foreach ($column as $name)
                $this->orderBy($name, $ascending);
        }
        else
        {
            foreach ($column as $name => $value)
                $this->orderBy($name, $value);
        }
        return $this;
    }

}
