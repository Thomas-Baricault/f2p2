<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\form;


require_once('f2p2/form/Field.php');


class Form
{

    private string $view;

    private array $fields = [];


    public function __construct(string $view)
    {
        $this->view = $view;
        return;
    }

    public function __toString(): string
    {
        return \f2p2\View::render($this->view, ['form' => $this]);
    }

    public function get(int|string $id): ?\f2p2\form\Field
    {
        if (is_int($id))
            return $this->fields[$id];
        foreach ($this->fields as $field)
            if ($field->get('id') === $id)
                return $field;
        return null;
    }

    public function field(\f2p2\form\Field $value): static
    {
        $this->fields[] = $value;
        return $this;
    }

    public function fill(array $data): void
    {
        foreach ($this->fields as $field)
            $field->fill($data);
        return;
    }

    public function validate(): array
    {
        $errors = [];
        foreach ($this->fields as $field)
            if (!in_array($field->name, $errors) && $field->validate() === false)
                $errors[] = $field->name;
        return $errors;
    }

}
