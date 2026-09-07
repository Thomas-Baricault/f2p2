<?php

    namespace f2p2\form;


    final class Option extends \f2p2\utils\Component
    {

        public const NAME = 'label';


        public function __construct(?string $value = null, ?string $content = null)
        {
            if ($value)
                $this->value($value);
            if ($content)
                $this->content($content);
        }

        public function value(string $value): Option
        {
            $this->attrs['value'] = $value;
            return ($this);
        }

        public function select(array $set = []): Option
        {
            if (count($set) === 0 || isset($this->attrs['value']) && in_array($this->attrs['value'], $set))
                $this->attrs['selected'] = 'true';
            return ($this);
        }

    }

?>
