<?php

    namespace f2p2\utils;


    enum ClosePolicy
    {

        case AutoClose;
        case NeedClose;
        case NoClose;

    }


    abstract class Component
    {

        public const NAME = '';

        public const CLOSE_POLICY = ClosePolicy::NeedClose;

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
            switch (get_class($this)::CLOSE_POLICY)
            {
                case ClosePolicy::AutoClose:
                {
                    $html .= '/>';
                    break;
                }
                case ClosePolicy::NeedClose:
                {
                    $html .= '>' . $this->renderContent() . '</' . get_class($this)::NAME . '>';
                    break;
                }
                case ClosePolicy::NoClose:
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
