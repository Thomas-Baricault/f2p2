<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\queries;


require_once('f2p2/orm/queries/Query.php');


abstract class DQLQuery extends \f2p2\orm\queries\Query
{

    protected string $table;


    public function __construct(string $table)
    {
        $this->table = $table;
        return;
    }

    #[\Override]
    public function execute(): array
    {
        return parent::execute()
            ->fetchAll();
    }

}
