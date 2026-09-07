<?php

    namespace f2p2;


    abstract class Component
    {

        public const NAME = '';

        public const CLOSE = true;

        protected array $attrs = [];

        protected string $content = '';


        public function get(string $name): mixed
        {
            if (isset($this->attrs[$name]))
                return ($this->attrs[$name]);
            return (null);
        }

        public function id(string $value): Component
        {
            $this->attrs['id'] = $value;
            return ($this);
        }

        public function class(string $value): Component
        {
            $this->attrs['class'] = $value;
            return ($this);
        }

        public function content(string $value): Component
        {
            $this->content = $value;
            return ($this);
        }

        public function render(): string
        {
            $html = '<' . get_class($this)::NAME;
            foreach ($this->attrs as $key => $value)
                $html .= " $key=\"$value\"";
            switch (get_class($this)::CLOSE)
            {
                case null:
                {
                    $html .= '/>';
                    break;
                }
                case true:
                {
                    $html .= '>' . $this->renderContent() . '</' . get_class($this)::NAME . '>';
                    break;
                }
                case false:
                {
                    $html .= '>';
                    break;
                }
            }
            return ($html);
        }

        public function renderContent(): string
        {
            return ($this->content);
        }

    }

?>
