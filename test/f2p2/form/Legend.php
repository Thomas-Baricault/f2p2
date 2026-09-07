<?php

    namespace f2p2\form;


    require_once('f2p2/utils/Component.php');


    final class Legend extends \f2p2\Component
    {

        public const NAME = 'legend';


        public function __construct(?string $content = null)
        {
            if ($content)
                $this->content($content);
        }

    }

?>
