<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Input.php');


    class Submit extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'submit';


        public function __construct()
        {
            parent::__construct();
            $this
                ->name('submit')
                ->required();
        }

    }

?>
