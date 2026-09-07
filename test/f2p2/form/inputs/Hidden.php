<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Input.php');


    class Hidden extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'hidden';


        public function __construct()
        {
            parent::__construct();
            $this->required();
        }

    }

?>
