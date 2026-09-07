<?php

    namespace f2p2\form\inputs;


    use Override;


    class Hidden extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'hidden';


        #[Override]
        public function __construct()
        {
            parent::__construct();
            $this->required();
        }

    }

?>
