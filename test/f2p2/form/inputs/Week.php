<?php

    namespace f2p2\form\inputs;


    class Week extends \f2p2\form\inputs\Date
    {

        public const TYPE = 'week';

        public const PATTERN = \f2p2\regex\WEEK;

    }

?>
