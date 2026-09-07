<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Date.php');
    require_once('f2p2/utils/regex.php');


    class Month extends \f2p2\form\inputs\Date
    {

        public const TYPE = 'month';

        public const PATTERN = \f2p2\regex\MONTH;

    }

?>
