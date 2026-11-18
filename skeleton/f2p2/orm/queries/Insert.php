<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\queries;


require_once('f2p2/orm/queries/DMLQuery.php');


final class Insert extends \f2p2\orm\queries\DMLQuery
{

    private array $columns = [];

    private array $values = [];


    #[\Override]
    public function clauses(): array
    {
        return [
            "INSERT INTO {$this->table}" . ($this->columns
                ? '(' . implode(', ', $this->columns) .  ')'
                : ''),
            "VALUES " . implode(', ', $this->values),
        ];
    }

    public function columns(array $values): static
    {
        if ($values)
            throw new \RuntimeException('columns already defined');
        if ($this->values)
            throw new \RuntimeException('cannot define columns after values');
        $this->columns = $values;
        return $this;
    }

    public function values(array $values): static
    {
        $n = count($this->values);
        $placeholders = [];
        if (array_is_list($values))
        {
            $count = count($values);
            if ($this->columns && $count !== count($this->columns))
                throw new \ValueError('invalid number of values');
            for ($i = 0; $i < $count; $i++)
            {
                $placeholder = ":column_{$n}_{$i}";
                $placeholders[] = $placeholder;
                $this->prepare[$placeholder] = $values[$i];
            }
        }
        else
        {
            if (!$this->columns)
                $this->columns = array_keys($values);
            $count = count($this->columns);
            for ($i = 0; $i < $count; $i++)
            {
                $placeholder = ":column_{$n}_{$i}";
                $placeholders[] = $placeholder;
                $this->prepare[$placeholder] = $values[$this->columns[$i]];
            }
        }
        $this->values[] = '(' . implode(', ', $placeholders) . ')';
        return $this;
    }

}
