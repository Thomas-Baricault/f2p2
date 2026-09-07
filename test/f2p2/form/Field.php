<?php

    namespace f2p2\form;


    use Override;


    abstract class Field extends \f2p2\form\FormComponent
    {

        private ?\f2p2\form\Label $label = null;


        #[Override]
        public function id(string $value): Field
        {
            if ($this->label)
                $this->label->for($value);
            return (parent::id($value));
        }

        public function name(string $value): Field
        {
            $this->attrs['name'] = $value;
            return ($this);
        }

        public function required(): Field
        {
            $this->attrs['required'] = 'true';
            return ($this);
        }

        public function label(string|\f2p2\form\Label $value): Field
        {
            $this->label = (
                is_string($value)
                    ? new Label($value)
                    : $value
            );
            if (isset($this->attrs['id']))
                $this->label->for($this->attrs['id']);
            return ($this);
        }

        #[Override]
        public function render(): string
        {
            return (
                (
                    $this->label
                        ? $this->label->render()
                       : ''
                ) + parent::render()
            );
        }

    }

?>
