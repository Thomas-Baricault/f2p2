<?php

    namespace f2p2\form\inputs;


    class Time extends \f2p2\form\inputs\Date
    {

        public const TYPE = 'time';

        public const PATTERN = \f2p2\regex\TIME;

    }

?>
