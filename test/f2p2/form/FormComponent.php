<?php

    namespace f2p2\form;


    abstract class FormComponent extends \f2p2\utils\Component
    {

        abstract public function dump(): array;

        abstract public function fill(array $data): void;

        abstract public function validate(): bool;

    }

?>
