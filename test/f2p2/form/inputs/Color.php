<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Input.php');
    require_once('f2p2/utils/regex.php');


    class Color extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'color';


        public function __construct()
        {
            parent::__construct();
            $this->required();
        }

        #[\Override]
        public function validate(): bool
        {
            return (
                parent::validate() &&
                (!isset($this->attrs['value']) || preg_match(\f2p2\regex\COLOR, $this->attrs['value']))
            );
        }

    }

?>
