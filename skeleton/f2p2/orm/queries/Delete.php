<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\queries;


require_once('f2p2/orm/queries/DMLQuery.php');
require_once('f2p2/orm/traits/Limit.php');
require_once('f2p2/orm/traits/OrderBy.php');
require_once('f2p2/orm/traits/Where.php');


final class Delete extends \f2p2\orm\queries\DMLQuery
{

    use
        \f2p2\orm\traits\Limit,
        \f2p2\orm\traits\OrderBy,
        \f2p2\orm\traits\Where
    {
        \f2p2\orm\traits\Limit::__toString as toStringLimit;
        \f2p2\orm\traits\OrderBy::__toString as toStringOrderBy;
        \f2p2\orm\traits\Where::__toString as toStringWhere;
    }


    #[\Override]
    public function __toString(): string
    {
        return parent::__toString();
    }

    #[\Override]
    public function clauses(): array
    {
        return [
            "DELETE FROM {$this->table}",
            $this->toStringWhere(),
            $this->toStringOrderBy(),
            $this->toStringLimit(),
        ];
    }

}
