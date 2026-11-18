<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\components;


class Component
{

    protected string $tag;

    protected ?bool $close;

    protected array $attrs = [];

    protected array $children = [];


    public function __construct(string $tag, ?bool $close = true)
    {
        $this->tag = $tag;
        $this->close = $close;
        return;
    }

    public function __toString(): string
    {
        $html = '<' . $this->tag;
        foreach ($this->attrs as $key => $value)
        {
            $html .= $value
                ? " $key=\"$value\""
                : " $key";
        }
        switch ($this->close)
        {
            case null:
            {
                $html .= '/>';
                break;
            }
            case true:
            {
                $html .= '>';
                foreach ($this->children as $item)
                {
                    if (is_string($item))
                        $html .= $item;
                    else
                        $html .= $item->render();
                }
                $html .= '</' . $this->tag . '>';
                break;
            }
            case false:
            {
                $html .= '>';
                break;
            }
        }
        return $html;
    }

    public function has(string $name): bool
    {
        return isset($this->attrs[$name]);
    }

    public function get(string $name): mixed
    {
        if (isset($this->attrs[$name]))
            return $this->attrs[$name];
        return null;
    }

    public function set(string $name, string $value = ''): static
    {
        $this->attrs[$name] = $value;
        return $this;
    }

    public function id(string $value): static
    {
        $this->attrs['id'] = $value;
        return $this;
    }

    public function class(string $value): static
    {
        $this->attrs['class'] = $value;
        return $this;
    }

    public function content(string $value): static
    {
        $this->children[] = $value;
        return $this;
    }

    public function child(Component $value): static
    {
        $this->children[] = $value;
        return $this;
    }

}
