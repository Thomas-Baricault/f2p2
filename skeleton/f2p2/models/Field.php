<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\models;


require_once('f2p2/orm/DataType.php');


#[\Attribute(\Attribute::TARGET_PROPERTY)]
class Field
{

    public string $name;

    public \f2p2\orm\DataType $type;

    public array $params;

    public bool $primary;

    public bool $unique;

    public bool $nullable;

    public bool $autoIncrement;

    public mixed $default;


    public static function __set_state(array $data): self
    {
        $field = new self(\f2p2\orm\DataType::BigInt);
        foreach ($data as $property => $value)
            $field->{$property} = $value;
        return $field;
    }

    public function __construct(\f2p2\orm\DataType $type, array $params = [], bool $primary = false, bool $unique = false, bool $nullable = false, bool $autoIncrement = false, mixed $default = null)
    {
        $this->type = $type;
        $this->params = $params;
        $this->primary = $primary;
        $this->unique = $unique;
        $this->nullable = $nullable;
        $this->autoIncrement = $autoIncrement;
        $this->default = $default;
        return;
    }

    public function __toString()
    {
        $str = $this->name . ' ' . $this->type->value;
        if (!empty($this->params))
            $str .= '(' . implode(', ', $this->params) . ')';
        if ($this->primary)
            $str .= ' PRIMARY KEY';
        if ($this->unique)
            $str .= 'UNIQUE';
        if (!$this->nullable)
            $str .= ' NOT NULL';
        if ($this->autoIncrement)
            $str .= ' AUTO_INCREMENT';
        if ($this->default !== null)
        {
            $str .= ' DEFAULT ' . match (true)
            {
                is_string($this->default) => "'{$this->default}'",
                $this->default instanceof \f2p2\orm\Constant => $this->default->value,
                default => $this->default,
            };
        }
        return $str;
    }

    public function load(mixed $value): mixed
    {
        return match ($this->type)
        {
            \f2p2\orm\DataType::Date => new \DateTime($value),
            \f2p2\orm\DataType::DateTime => new \DateTime($value),
            \f2p2\orm\DataType::Enum => explode(',', $value),
            default => $value,
        };
    }

    public function dump(mixed $value): mixed
    {
        return match ($this->type)
        {
            \f2p2\orm\DataType::Date => $value->format('Y-m-d'),
            \f2p2\orm\DataType::DateTime => $value->format('Y-m-d H:i:s'),
            \f2p2\orm\DataType::Enum => implode(',', $value),
            default => $value,
        };
    }

    public function validate(mixed $value): bool
    {
        return !$this->nullable && $value === null;
    }

}
