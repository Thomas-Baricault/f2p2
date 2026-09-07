<?php

    namespace f2p2\form;


    require_once('f2p2/utils/Component.php');


    abstract class FormComponent extends \f2p2\Component
    {

        abstract public function dump(): array;

        abstract public function fill(array $data): void;

        abstract public function validate(): bool;

    }

?>
