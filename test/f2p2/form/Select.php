<?php

    namespace f2p2\form;


    require_once('f2p2/form/Field.php');
    require_once('f2p2/form/Option.php');


    class Select extends \f2p2\form\Field
    {

        private array $options = [];


        #[\Override]
        public function name(string $value): Select
        {
            return (
                isset($this->attrs['multiple'])
                    ? parent::name("$value\[]")
                    : parent::name($value)
            );
        }

        public function multiple(): Select
        {
            if (isset($this->attrs['name']) && !str_ends_with($this->attrs['name'], '[]'))
                $this->attrs['name'] .= '[]';
            $this->attrs['multiple'] = 'true';
            return ($this);
        }

        public function size(int $value): Select
        {
            $this->attrs['size'] = $value;
            return ($this);
        }

        public function option(string|\f2p2\form\Option $value, ?string $content = null): Select
        {
            $this->options[] = (
                is_string($value)
                    ? new \f2p2\form\Option($value, $content)
                    : $value
            );
            return ($this);
        }

        public function select(string|array $value): Select
        {
            if (is_string($value))
                $value = [$value];
            foreach ($this->options as $option)
                $option->select($value);
            return ($this);
        }

        #[\Override]
        public function dump(): array
        {
            if (!isset($this->attrs['name']))
                return ([]);
            $value = null;
            foreach ($this->options as $option)
            {
                if ($option->get('selected'))
                {
                    if (isset($this->attrs['multiple']))
                    {
                        $value = $option->get('value');
                        break;
                    }
                    else
                    {
                        if ($value === null)
                            $value = [];
                        $value[] = $option->get('value');
                    }
                }
            }
            return ([$this->attrs['name'] => $value]);
        }

        #[\Override]
        public function fill(array $data): void
        {
            if (isset($this->attrs['name']) && isset($data[$this->attrs['name']]))
            {
                $set = (
                    is_array($data[$this->attrs['name']])
                        ? $data[$this->attrs['name']]
                        : [$data[$this->attrs['name']]]
                );
                foreach ($this->options as $option)
                    $option->select($set);
            }
        }

        #[\Override]
        public function validate(): bool
        {
            if (!isset($this->attrs['required']))
                return (true);
            $n = 0;
            foreach ($this->options as $option)
                if ($option->get('selected'))
                    $n++;
            if (isset($this->attrs['multiple']))
                return ($n > 0);
            return ($n === 1);
        }

        #[\Override]
        public function renderContent(): string
        {
            $html = '';
            foreach ($this->options as $option)
                $html .= $option->render();
            return ($html);
        }

    }

?>
