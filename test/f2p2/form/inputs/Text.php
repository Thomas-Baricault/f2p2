<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Input.php');


    class Text extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'text';


        public function minLength(int $value): Text
        {
            $this->attrs['minlength'] = $value;
            return ($this);
        }

        public function maxLength(int $value): Text
        {
            $this->attrs['maxlength'] = $value;
            return ($this);
        }

        public function pattern(string $value): Text
        {
            $this->attrs['pattern'] = (
                $value[0] === '/' && $value[-1] === '/'
                    ? substr($value, 1, -1)
                    : $value
            );
            return ($this);
        }

        public function placeholder(string $value): Text
        {
            $this->attrs['placeholder'] = $value;
            return ($this);
        }

        #[\Override]
        public function validate(): bool
        {
            if (!parent::validate())
                return (false);
            if (!isset($this->attrs['value']))
                return (true);
            return (
                (!isset($this->attrs['minlength']) || strlen($this->attrs['value'] >= $this->attrs['minlength'])) &&
                (!isset($this->attrs['maxlength']) || strlen($this->attrs['value'] <= $this->attrs['maxlength'])) &&
                (!isset($this->attrs['pattern']) || preg_match('/' . $this->attrs['pattern'] . '/', $this->attrs['value']))
            );
        }

    }

?>
