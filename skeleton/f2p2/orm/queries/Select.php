<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\queries;


require_once('f2p2/orm/queries/DQLQuery.php');
require_once('f2p2/orm/traits/GroupBy.php');
require_once('f2p2/orm/traits/Limit.php');
require_once('f2p2/orm/traits/Offset.php');
require_once('f2p2/orm/traits/OrderBy.php');
require_once('f2p2/orm/traits/Where.php');


final class Select extends \f2p2\orm\queries\DQLQuery
{

    use
        \f2p2\orm\traits\GroupBy,
        \f2p2\orm\traits\Limit,
        \f2p2\orm\traits\Offset,
        \f2p2\orm\traits\OrderBy,
        \f2p2\orm\traits\Where
    {
        \f2p2\orm\traits\GroupBy::__toString as toStringGroupBy;
        \f2p2\orm\traits\Limit::__toString as toStringLimit;
        \f2p2\orm\traits\Offset::__toString as toStringOffset;
        \f2p2\orm\traits\OrderBy::__toString as toStringOrderBy;
        \f2p2\orm\traits\Where::__toString as toStringWhere;
    }


    private string $columns = '*';

    private string $distinct = '';


    #[\Override]
    public function __toString(): string
    {
        return parent::__toString();
    }

    #[\Override]
    public function clauses(): array
    {
        return [
            "SELECT {$this->columns}",
            "FROM {$this->table}",
            $this->distinct,
            $this->toStringWhere(),
            $this->toStringOrderBy(),
            $this->toStringGroupBy(),
            $this->toStringLimit(),
            $this->toStringOffset(),
        ];
    }

    public function column(string $value): static
    {
        if ($value)
        {
            if ($this->columns === '*')
                $this->columns = $value;
            else
                $this->columns .= ", $value";
        }
        return $this;
    }

    public function columns(array $values): static
    {
        if ($values)
            $this->columns = implode(', ', $values);
        return $this;
    }

    public function distinct(): static
    {
        $this->distinct = 'DISTINCT';
        return $this;
    }

}
