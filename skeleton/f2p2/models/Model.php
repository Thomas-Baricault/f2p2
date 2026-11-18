<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\models;


require_once('f2p2/models/Cache.php');
require_once('f2p2/models/Field.php');
require_once('f2p2/models/Infos.php');
require_once('f2p2/orm/DataType.php');


abstract class Model
{

    static private array $infos = [];

    #[\f2p2\models\Field(\f2p2\orm\DataType::Int, primary: true, autoIncrement: true)]
    protected ?int $id = null;


    public static function init(): void
    {
        $infos = \f2p2\models\Cache::load();
        if ($infos)
        {
            self::$infos = $infos;
            \f2p2\utils\discover('app/models');
            return;
        }
        foreach (\f2p2\utils\discover('app/models') as $class)
        {
            $reflection = new \ReflectionClass($class);
            $attributes = $reflection->getAttributes(\f2p2\models\Infos::class);
            if ($attributes)
            {
                $infos = $attributes[0]->newInstance();
                $infos->table = $reflection->getShortName();
                foreach ($reflection->getProperties() as $property)
                {
                    $attributes = $property->getAttributes(\f2p2\models\Field::class);
                    if ($attributes)
                    {
                        $field = $attributes[0]->newInstance();
                        $field->name = $property->getName();
                        $infos->fields[$field->name] = $field;
                    }
                    // TODO
                    // $attributes = $property->getAttributes(\f2p2\models\Reference::class);
                    // if ($attributes)
                    // {
                    // }
                }
                self::$infos[$class] = $infos;
            }
        }
        \f2p2\models\Cache::save(self::$infos);
        return;
    }

    public static function create(): void
    {
        $infos = static::infos();
        $query = 'CREATE TABLE ' . $infos->table . '(';
        $first = true;
        foreach ($infos->fields as $field)
        {
            if ($first)
                $first = false;
            else
                $query .= ', ';
            $query .= strval($field);
        }
        $query .= ") ENGINE = {$infos->engine} DEFAULT CHARSET = {$infos->charset} COLLATE = {$infos->collate}";
        \f2p2\orm\Connection::current()
            ->execute($query);
    }

    public static function drop(): void
    {
        \f2p2\orm\Connection::current()
            ->execute('DROP TABLE ' . static::infos()->table);
    }

    public static function find(?int $id = null, ?array $fields = null, null|string|array $orderBy = null, ?int $limit = null, ?int $offset = null, bool $dump = false): null|static|array
    {
        $query = new \f2p2\orm\queries\Select(static::infos()->table);
        if ($id === null)
        {
            if ($orderBy)
                $query->orderBy($orderBy);
            if ($limit)
                $query->limit($limit);
            if ($offset)
                $query->offset($offset);
        }
        else
        {
            $query
                ->where("id = $id")
                ->limit(1);
        }
        if ($fields)
            $query->columns($fields);
        $items = $query->execute();
        if (empty($items))
        {
            return $id === null
                ? []
                : null;
        }
        if ($dump)
        {
            return $id === null
                ? $items
                : $items[0];
        }
        return $id === null
            ? static::fromArrayList($items)
            : new static($items[0]);
    }

    public static function infos(): \f2p2\models\Infos
    {
        return self::$infos[static::class];
    }

    public static function fromArray(array $data): static
    {
        return new static($data);
    }

    public static function fromArrayList(array $items): array
    {
        return array_map(
            fn(array $item) => static::fromArray($item),
            $items
        );
    }

    public function __construct(array $data = [])
    {
        foreach (static::infos()->fields as $field)
        {
            if (array_key_exists($field->name, $data))
                $this->{$field->name} = $field->load($data[$field->name]);
            else if ($field->default !== null)
            {
                $this->{$field->name} = $field->default instanceof \f2p2\orm\Constant
                    ? match ($field->default)
                    {
                        \f2p2\orm\Constant::CurrentTimestamp => new \DateTime(),
                        default => null,
                    }
                    : $field->default;
            }
            else if ($field->nullable)
                $this->{$field->name} = null;
        }
    }

    public function __get(string $name): ?int
    {
        if ($name === 'id')
            return $this->id;
        throw new \ValueError("undefined property '$name'");
    }

    public function isValid(): bool
    {
        foreach (static::infos()->fields as $field)
            if ($field->validate($this->{$field->name}))
                return false;
        return true;
    }

    public function dump(): array
    {
        $data = [];
        foreach (static::infos()->fields as $field)
            $data[$field->name] = $field->dump($this->{$field->name});
        return $data;
    }

    public function push(): void
    {
        $infos = static::infos();
        $values = [];
        foreach ($infos->fields as $field)
            if ($this->id !== null || $this->{$field->name} !== null)
                $values[$field->name] = $field->dump($this->{$field->name});
        if ($this->id === null)
        {
            (new \f2p2\orm\queries\Insert($infos->table))
                ->values($values)
                ->execute();
            $this->id = \f2p2\orm\Connection::current()->pdo->lastInsertId();
        }
        else
        {
            (new \f2p2\orm\queries\Update($infos->table))
                ->updates($values)
                ->where("id = {$this->id}")
                ->execute();
        }
        return;
    }

    public function remove(): void
    {
        if ($this->id === null)
            throw new \RuntimeException('cannot remove entity without an id');
        (new \f2p2\orm\queries\Delete(static::infos()->table))
            ->where("id = {$this->id}")
            ->execute();
        return;
    }

}
