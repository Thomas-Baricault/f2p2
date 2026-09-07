<?php

    namespace f2p2\form\inputs;


    use Override;


    class Date extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'date';

        public const FORMAT = 'Y-m-d';


        public function min(string $value): Date
        {
            if (!\f2p2\validateDate(get_class($this)::FORMAT, $value))
                throw new \Exception('Value has to match date format');
            $this->attrs['min'] = $value;
            return ($this);
        }

        public function max(string $value): Date
        {
            if (!\f2p2\validateDate(get_class($this)::FORMAT, $value))
                throw new \Exception('Value has to match date format');
            $this->attrs['max'] = $value;
            return ($this);
        }

        #[Override]
        public function validate(): bool
        {
            if (!parent::validate())
                return (false);
            if (!isset($this->attrs['value']))
                return (true);
            if (!\f2p2\validateDate(get_class($this)::FORMAT, $this->attrs['value']))
                return (false);
            $value = new \DateTime($this->attrs['value']);
            return (
                (!isset($this->attrs['min']) || $value >= new \DateTime($this->attrs['min'])) &&
                (!isset($this->attrs['max']) || $value <= new \DateTime($this->attrs['max']))
            );
        }

    }

?>
