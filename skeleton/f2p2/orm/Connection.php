<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm;


require_once('f2p2/orm/DatabaseType.php');


#[\Attribute(\Attribute::TARGET_CLASS)]
final class Connection
{

    private static array $cache = [];

    private static ?Connection $current = null;

    public \PDO $pdo;


    private static function buildDSNMySQL(?string $name, ?string $host, ?int $port, ?string $unixSocket, ?string $charset): string
    {
        if ($name === null)
            throw new \ValueError('missing database name for MySQL connection');
        if ($port === null)
            $port = 3306;
        if ($charset === null)
            $charset = 'utf8mb4';
        if ($host)
            $dsn = "host=$host;port=$port";
        else if ($unixSocket)
            $dsn = "unix_socket=$unixSocket";
        else
            throw new \ValueError('missing host or unix socket for MySQL connection');
        $dsn .= ";dbname=$name;charset=$charset";
        return $dsn;
    }

    private static function buildDSNOCI(?string $name, ?string $host, ?int $port, ?string $charset): string
    {
        if ($name === null)
            throw new \ValueError('missing database name for OCI connection');
        if ($host === null)
            throw new \ValueError('missing host for OCI connection');
        if ($port === null)
            $port = 1521;
        if ($charset === null)
            $charset = 'AL32UTF8';
        return "dbname=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=$host)(PORT=$port))(CONNECT_DATA=(SERVICE_NAME=$name)));charset=$charset";
    }

    private static function buildDSNSQLite(?string $name): string
    {
        return $name
            ? $name
            : '';
    }

    public static function init(): void
    {
        \f2p2\utils\discover('app/connections');
        return;
    }

    public static function use(string $class): void
    {
        if (!array_key_exists($class, self::$cache))
        {
            $attributes = (new \ReflectionClass($class))->getAttributes(\f2p2\orm\Connection::class);
            if (!$attributes)
                throw new \RuntimeException('missing connection attribute');
            self::$cache[$class] = $attributes[0]->newInstance();
        }
        self::$current = self::$cache[$class];
        return;
    }

    public static function current(): Connection
    {
        if (self::$current)
            return self::$current;
        throw new \RuntimeException('there is no current connection');
    }

    public function __construct(\f2p2\orm\DatabaseType $type, ?string $name = null, ?string $host = null, ?int $port = null, ?string $unixSocket = null, ?string $charset = null, ?string $user = null, ?string $pass = null)
    {
        $this->pdo = new \PDO(
            $type->value . ':' . match ($type)
            {
                \f2p2\orm\DatabaseType::MySQL => Connection::buildDSNMySQL($name, $host, $port, $unixSocket, $charset),
                \f2p2\orm\DatabaseType::OCI => Connection::buildDSNOCI($name, $host, $port, $charset),
                \f2p2\orm\DatabaseType::SQLite => Connection::buildDSNSQLite($name),
            },
            $type === \f2p2\orm\DatabaseType::SQLite ? null : $user,
            $type === \f2p2\orm\DatabaseType::SQLite ? null : $pass,
            [
                \PDO::ATTR_DEFAULT_FETCH_MODE => \PDO::FETCH_ASSOC,
                \PDO::ATTR_ERRMODE => \PDO::ERRMODE_EXCEPTION,
            ],
        );
        return;
    }

    public function execute(string $query, array $values = []): \PDOStatement
    {
        $statement = $this->pdo->prepare($query);
        $statement->execute($values);
        return $statement;
    }

}
