<?php

    namespace f2p2\form\inputs;


    use Override;


    abstract class Input extends \f2p2\form\Field
    {

        public const NAME = 'input';

        public const CLOSE_POLICY = \f2p2\utils\ClosePolicy::NoClose;

        public const TYPE = '';


        public function __construct()
        {
            $this->attrs['type'] = get_class($this)::TYPE;
        }

        public function value(string $value): Input
        {
            $this->attrs['value'] = $value;
            return ($this);
        }

        #[Override]
        public function dump(): array
        {
            if (!isset($this->attrs['name']))
                return ([]);
            return ([
                $this->attrs['name'] => (
                    isset($this->attrs['value'])    
                        ? $this->attrs['value']
                        : null
                )
            ]);
        }

        #[Override]
        public function fill(array $data): void
        {
            if (isset($this->attrs['name']) && isset($data[$this->attrs['name']]))
                $this->attrs['value'] = $data[$this->attrs['name']];
        }

        #[Override]
        public function validate(): bool
        {
            return (!isset($this->attrs['required']) || isset($this->attrs['value']));
        }

    }

?>
