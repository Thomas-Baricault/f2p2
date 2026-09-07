<?php

    namespace f2p2\requests;


    class File
    {

        private array $data = [];


        public static function currents(): array
        {
            $files = [];
            foreach ($_FILES as $name => $data)
            {
                $file = new File($data);
                if ($file->valid())
                    $files[$name] = $file;
            }
            return ($files);
        }

        public function __construct(array $data = [])
        {
            $this->data = $data;
        }

        public function name(): ?string
        {
            if (isset($this->data['name']) && is_string($this->data['name']))
                return ($this->data['name']);
            return (null);
        }

        public function fullPath(): ?string
        {
            if (isset($this->data['full_path']) && is_string($this->data['full_path']))
                return ($this->data['full_path']);
            return (null);
        }

        public function tmpName(): ?string
        {
            if (isset($this->data['tmp_name']) && is_string($this->data['tmp_name']))
                return ($this->data['tmp_name']);
            return (null);
        }

        public function type(): ?string
        {
            if (isset($this->data['type']) && is_string($this->data['type']))
                return ($this->data['type']);
            return (null);
        }

        public function size(): ?int
        {
            if (isset($this->data['size']) && is_int($this->data['size']))
                return ($this->data['size']);
            return (null);
        }

        public function error(): ?int
        {
            if (isset($this->data['error']) && is_int($this->data['error']))
                return ($this->data['error']);
            return (null);
        }

        public function valid(): bool
        {
            return (
                $this->name() &&
                $this->tmpName() &&
                $this->size() &&
                $this->type() &&
                $this->error()
            );
        }

        public function save(string $path): bool
        {
            $tmpName = $this->tmpName();
            return (
                $tmpName
                    ? move_uploaded_file($tmpName, $path)
                    : false
            );
        }

    }

?>
