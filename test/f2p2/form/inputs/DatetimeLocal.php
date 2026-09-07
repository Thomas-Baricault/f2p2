<?php

    namespace f2p2\form\inputs;


    require_once('f2p2/form/inputs/Date.php');


    class DatetimeLocal extends \f2p2\form\inputs\Date
    {

        public const TYPE = 'datetime-local';

        public const FORMAT = 'Y-m-d\TH:i';

    }

?>
