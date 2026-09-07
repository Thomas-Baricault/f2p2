<?php

    namespace f2p2\form\inputs;


    use Override;


    class Range extends \f2p2\form\inputs\Number
    {

        public const TYPE = 'range';


        #[Override]
        public function __construct()
        {
            parent::__construct();
            $this->required();
        }

    }

?>
