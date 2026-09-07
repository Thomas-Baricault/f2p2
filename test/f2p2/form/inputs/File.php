<?php

    namespace f2p2\form\inputs;


    use Override;


    class File extends \f2p2\form\inputs\Input
    {

        public const TYPE = 'file';

        private ?int $maxSize = null;

        private array $files = [];


        public function accept(string $value): File
        {
            if (isset($this->attrs['accept']))
                $this->attrs['accept'] += ",$value";
            else
                $this->attrs['accept'] = $value;
            return ($this);
        }

        public function maxSize(int $value): File
        {
            $this->maxSize = $value;
            return ($this);
        }

        public function multiple(): File
        {
            $this->attrs['multiple'] = 'true';
            return ($this);
        }

        #[Override]
        public function dump(): array
        {
            if (!isset($this->attrs['name']))
                return ([]);
            return ([
                $this->attrs['name'] => (
                    isset($this->attrs['multiple'])
                        ? $this->files
                        : (
                    count($this->files) === 0
                        ? null
                        : $this->files[0]
                ))
            ]);
        }

        #[Override]
        public function fill(array $data): void
        {
            if (isset($this->attrs['name']) && isset($data[$this->attrs['name']]))
            {
                $this->files = (
                    array_is_list($data)
                        ? $data
                        : [$data]
                );
            }
        }

        #[Override]
        public function validate(): bool
        {
            if (
                (isset($this->attrs['required']) && count($this->files) === 0) ||
                (!isset($this->attrs['multiple']) && count($this->files) > 1)
            )
                return (false);
            $accept = [];
            if (isset($this->attrs['accept']))
                $accept = explode(',', $this->attrs['accept']);
            foreach ($this->files as $file)
            {
                return (
                    $file->error() === 0 &&
                    ($this->maxSize === null || $file->size() <= $this->maxSize) &&
                    (count($accept) === 0 || in_array($file->type(), $accept))
                );
            }
            return (true);
        }

    }

?>
