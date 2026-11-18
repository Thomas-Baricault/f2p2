<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm\queries;


abstract class Query
{

    protected array $prepare = [];


    public function __toString(): string
    {
        return implode(' ', array_filter(
            $this->clauses(),
            fn($clause) => $clause !== '',
        ));
    }

    public function execute(): mixed
    {
        return \f2p2\orm\Connection::current()
            ->execute(strval($this), $this->prepare);
    }

    abstract public function clauses(): array;

}
