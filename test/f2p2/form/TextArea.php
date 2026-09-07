<?php

    namespace f2p2\form;


    use Override;


    class TextArea extends \f2p2\form\Field
    {

        public const NAME = 'textarea';


        public function minLength(int $value): TextArea
        {
            $this->attrs['minlength'] = $value;
            return ($this);
        }

        public function maxLength(int $value): TextArea
        {
            $this->attrs['maxlength'] = $value;
            return ($this);
        }

        public function placeholder(string $value): TextArea
        {
            $this->attrs['placeholder'] = $value;
            return ($this);
        }

        #[Override]
        public function dump(): array
        {
            if (!isset($this->attrs['name']) || $this->content === '')
                return ([]);
            return ([$this->attrs['name'] => $this->content]);
        }

        #[Override]
        public function fill(array $data): void
        {
            if (isset($this->attrs['name']) && isset($data[$this->attrs['name']]))
                $this->content($data[$this->attrs['name']]);
        }

        #[Override]
        public function validate(): bool
        {
            return (!isset($this->attrs['required']) || $this->content !== '');
        }

    }

?>
