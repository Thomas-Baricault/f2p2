<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form\inputs;


require_once('f2p2/form/inputs/Input.php');


class File extends \f2p2\form\inputs\Input
{

    public const TYPE = 'file';

    private ?int $maxSize = null;

    private array $files = [];


    public function accept(string $value): static
    {
        if ($this->has('accept'))
            $this->attrs['accept'] .= ",$value";
        else
            $this->attrs['accept'] = $value;
        return $this;
    }

    public function maxSize(int $value): static
    {
        $this->maxSize = $value;
        return $this;
    }

    public function multiple(): static
    {
        $this->set('multiple');
        return $this;
    }

    #[\Override]
    public function fill(array $data): void
    {
        if (!$this->has('disabled') && isset($data[$this->name]))
        {
            $this->files = array_is_list($data)
                ? $data
                : [$data];
        }
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        if (
            ($this->has('required') && count($this->files) === 0) ||
            (!$this->has('multiple') && count($this->files) > 1)
        )
            return (false);
        $accept = [];
        if ($this->has('accept'))
            $accept = explode(',', $this->attrs['accept']);
        foreach ($this->files as $file)
        {
            return (
                $file->error() === 0 &&
                ($this->maxSize === null || $file->size() <= $this->maxSize) &&
                (count($accept) === 0 || in_array($file->type(), $accept))
            );
        }
        return true;
    }

}
