<?php

    namespace f2p2\form;


    use Override;


    abstract class FieldContainer extends \f2p2\form\FormComponent
    {

        private array $fields = [];


        public function field(\f2p2\form\FormComponent $field): FieldContainer
        {
            $this->fields[] = $field;
            return ($this);
        }

        #[Override]
        public function dump(): array
        {
            $data = [];
            foreach ($this->fields as $field)
                $data += $field->dump();
            return ($data);
        }

        #[Override]
        public function fill(array $data): void
        {
            foreach ($this->fields as $field)
                $data += $field->fill($data);
        }

        #[Override]
        public function validate(): bool
        {
            foreach ($this->fields as $field)
                if ($field->validate() === false)
                    return (false);
            return (true);
        }

        #[Override]
        public function renderContent(): string
        {
            $html = '';
            foreach ($this->fields as $field)
                $html .= $field->render();
            return ($html);
        }

    }

?>
