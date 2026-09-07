<?php

    namespace f2p2\form;


    require_once('f2p2/form/FieldContainer.php');
    require_once('f2p2/form/Legend.php');


    class Fieldset extends \f2p2\form\FieldContainer
    {

        public const NAME = 'fieldset';

        private ?\f2p2\form\Legend $legend = null;


        public function legend(string|\f2p2\form\Legend $value): Fieldset
        {
            $this->legend = (
                is_string($value)
                    ? new \f2p2\form\Legend($value)
                   : $value
            );
            return ($this);
        }

        #[\Override]
        public function renderContent(): string
        {
            return (
                (
                    $this->legend
                        ? $this->legend->render()
                       : ''
                ) + parent::renderContent()
            );
        }

    }

?>
