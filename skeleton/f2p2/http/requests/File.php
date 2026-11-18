<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\requests;


class File
{

    private array $data = [];


    public static function currents(): array
    {
        $files = [];
        foreach ($_FILES as $name => $data)
        {
            $file = new File($data);
            if ($file->valid())
                $files[$name] = $file;
        }
        return ($files);
    }

    public function __construct(array $data = [])
    {
        $this->data = $data;
    }

    public function name(): ?string
    {
        return isset($this->data['name']) && is_string($this->data['name'])
            ? $this->data['name']
            : null;
    }

    public function fullPath(): ?string
    {
        return isset($this->data['full_path']) && is_string($this->data['full_path'])
            ? $this->data['full_path']
            : null;
    }

    public function tmpName(): ?string
    {
        return isset($this->data['tmp_name']) && is_string($this->data['tmp_name'])
            ? $this->data['tmp_name']
            : null;
    }

    public function type(): ?string
    {
        return isset($this->data['type']) && is_string($this->data['type'])
            ? $this->data['type']
            : null;
    }

    public function size(): ?int
    {
        return isset($this->data['size']) && is_int($this->data['size'])
            ? $this->data['size']
            : null;
    }

    public function error(): ?int
    {
        return isset($this->data['error']) && is_int($this->data['error'])
            ? $this->data['error']
            : null;
    }

    public function valid(): bool
    {
        return (
            $this->name() &&
            $this->tmpName() &&
            $this->size() &&
            $this->type() &&
            $this->error()
        );
    }

    public function save(string $path): bool
    {
        $tmpName = $this->tmpName();
        return $tmpName
            ? move_uploaded_file($tmpName, $path)
            : false;
    }

}
