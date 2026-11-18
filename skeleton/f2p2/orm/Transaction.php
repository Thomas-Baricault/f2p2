<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm;


require_once('f2p2/orm/Connection.php');


final class Transaction
{

    public function __construct()
    {
        \f2p2\orm\Connection::current()->pdo->beginTransaction();
        return;
    }

    public function savepoint(string $name): void
    {
        \f2p2\orm\Connection::current()->execute("SAVEPOINT $name");
        return;
    }

    public function rollback(?string $savepoint = null): void
    {
        if ($savepoint)
            \f2p2\orm\Connection::current()->execute("ROLLBACK TO SAVEPOINT $savepoint");
        else
            \f2p2\orm\Connection::current()->pdo->rollBack();
        return;
    }

    public function commit(): void
    {
        \f2p2\orm\Connection::current()->pdo->commit();
    }

}
