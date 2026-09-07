<?php

    namespace f2p2\form;


    final class Legend extends \f2p2\utils\Component
    {

        public const NAME = 'legend';


        public function __construct(?string $content = null)
        {
            if ($content)
                $this->content($content);
        }

    }

?>
