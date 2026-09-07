<?php

    namespace f2p2\form\inputs;


    class Month extends \f2p2\form\inputs\Date
    {

        public const TYPE = 'month';

        public const PATTERN = \f2p2\regex\MONTH;

    }

?>
