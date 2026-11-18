<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form;


require_once('f2p2/form/Field.php');


class Select extends \f2p2\form\Field
{

    private array $options = [];


    public function __construct(string $name, array $options)
    {
        parent::__construct('select', true, $name);
        foreach ($options as $value => $content)
        {
            $this->options[] = (new \f2p2\components\Component('option'))
                ->set('value', $value)
                ->content($content);
        }
        return;
    }

    public function multiple(bool $value = true): static
    {
        if ($value)
            $this->set('multiple');
        return $this;
    }

    public function size(int $value): static
    {
        $this->set('size', $value);
        return $this;
    }

    #[\Override]
    public function fill(array $data): void
    {
        if (!$this->has('disabled') && isset($data[$this->name]))
        {
            $set = is_array($data[$this->name])
                ? $data[$this->name]
                : [$data[$this->name]];
            foreach ($this->options as $option)
                if (in_array($option->get('value'), $set))
                    $option->set('selected');
        }
        return;
    }

    #[\Override]
    public function validate(): bool
    {
        if (!$this->get('required'))
            return (true);
        $n = 0;
        foreach ($this->options as $option)
            if ($option->get('selected'))
                $n++;
        if ($this->get('multiple'))
            return $n > 0;
        return $n === 1;
    }

}
