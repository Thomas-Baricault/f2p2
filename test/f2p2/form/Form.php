<?php

    namespace f2p2\form;


    require_once('f2p2/form/FieldContainer.php');
    require_once('f2p2/mvc/View.php');


    class Form extends \f2p2\form\FieldContainer
    {

        public function __construct()
        {
            $this->method('POST');
        }

        public function method(string $value): Form
        {
            $this->attrs['method'] = $value;
            return ($this);
        }

        public function route(string $value, array $params = []): Form
        {
            $this->attrs['action'] = \f2p2\View::route($value, $params);
            return ($this);
        }

        public function allowFiles(): Form
        {
            $this->attrs['enctype'] = 'multipart/form-data';
            return ($this);
        }

    }

?>
