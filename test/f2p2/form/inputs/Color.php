<?php

    namespace f2p2\form\inputs;


    use Override;


    class Color extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'color';


        #[Override]
        public function __construct()
        {
            parent::__construct();
            $this->required();
        }

        #[Override]
        public function validate(): bool
        {
            return (
                parent::validate() &&
                (!isset($this->attrs['value']) || preg_match(\f2p2\regex\COLOR, $this->attrs['value']))
            );
        }

    }

?>
