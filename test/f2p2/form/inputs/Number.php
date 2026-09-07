<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Input.php');


    class Number extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'number';


        public function min(int $value): Number
        {
            $this->attrs['min'] = $value;
            return ($this);
        }

        public function max(int $value): Number
        {
            $this->attrs['max'] = $value;
            return ($this);
        }

        public function step(int $value): Number
        {
            if ($value <= 0)
                throw new \Exception('Step must be greater than 0');
            $this->attrs['step'] = $value;
            return ($this);
        }

        #[\Override]
        public function validate(): bool
        {
            if (!parent::validate())
                return (false);
            if (!isset($this->attrs['value']))
                return (true);
            if (!filter_var($this->attrs['value'], FILTER_VALIDATE_INT))
                return (false);
            return (
                (!isset($this->attrs['min']) || $this->attrs['value'] >= $this->attrs['min']) &&
                (!isset($this->attrs['max']) || $this->attrs['value'] <= $this->attrs['max']) &&
                (!isset($this->attrs['step']) || (
                    isset($this->attrs['min'])
                        ? $this->attrs['value'] - $this->attrs['min']
                       : $this->attrs['value']
                ) % $this->attrs['step'] === 0)
            );
        }

    }

?>
