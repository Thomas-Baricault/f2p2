<?php

    namespace f2p2\responses;


    /**
     * HTTP response
     */
    class Response
    {

        /**
         * Status code
         */
        private int $code = 200;

        /**
         * HTTP headers
         */
        private array $headers = [];

        /**
         * Response body
         */
        private ?string $body = null;


        /**
         * Returns the response status code
         * 
         * @return int Status code
         */
        public function getCode(): int
        {
            return ($this->code);
        }

        /**
         * Returns the response headers
         * 
         * @return array Response headers (name => value)
         */
        public function getHeaders(): array
        {
            return ($this->headers);
        }

        /**
         * Returns a response header
         * 
         * @param string $name Header name
         * @return ?string Header value if exists, `null` otherwise
         */
        public function getHeader(string $name): ?string
        {
            return (
                array_key_exists($name, $this->headers)
                    ? $this->headers[$name]
                    : null
            );
        }

        /**
         * Returns the response body
         * 
         * @return ?string Response body
         */
        public function getBody(): ?string
        {
            return ($this->body);
        }

        /**
         * Sets the response status code
         * 
         * @param int $code Status code
         * 
         * @return Response Reference to this response
         */
        public function setCode(int $code): Response
        {
            $this->code = $code;
            return ($this);
        }

        /**
         * Sets the response headers
         * 
         * @param array $headers Response headers (name => value)
         * 
         * @return Response Reference to this response
         */
        public function setHeaders(array $headers): Response
        {
            foreach ($headers as $name => $value)
                $this->setHeader($name, $value);
            return ($this);
        }

        /**
         * Set a response header
         * 
         * @param string $name Header name
         * @param string|array $value Header value
         * 
         * @return Response Reference to this response
         */
        public function setHeader(string $name, string|array $value): Response
        {
            $this->headers[$name] = (
                is_array($value)
                    ? implode('; ', $value)
                    : $value
            );
            return ($this);
        }

        /**
         * Sets the response body
         * 
         * @param string $body Response body
         * 
         * @return Response Reference to this response
         */
        public function setBody(string $body): Response
        {
            $this->body = $body;
            return ($this);
        }

        /**
         * Send the response
         */
        public function send(): void
        {
            http_response_code($this->code);
            foreach ($this->headers as $key => $value)
                header("$key: $value");
            if ($this->body)
                echo $this->body;
            die();
        }

    }

?>
