<?php

    namespace f2p2\form;


    final class Label extends \f2p2\utils\Component
    {

        public const NAME = 'label';


        public function __construct(?string $content = null)
        {
            if ($content)
                $this->content($content);
        }

        public function for(string $value): Label
        {
            $this->attrs['for'] = $value;
            return ($this);
        }

    }

?>
