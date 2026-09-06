<?php

    namespace F2P2;


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
        public function getCode() : int
        {
            return ($this->code);
        }

        /**
         * Returns the response headers
         * 
         * @return array Response headers (name => value)
         */
        public function getHeaders() : array
        {
            return ($this->headers);
        }

        /**
         * Returns a response header
         * 
         * @param string $name Header name
         * @return ?string Header value if exists, `null` otherwise
         */
        public function getHeader(string $name) : ?string
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
        public function getBody() : ?string
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
        public function setCode(int $code) : Response
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
        public function setHeaders(array $headers) : Response
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
        public function setHeader(string $name, string|array $value) : Response
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
        public function setBody(string $body) : Response
        {
            $this->body = $body;
            return ($this);
        }

        /**
         * Send the response
         */
        public function send() : void
        {
            http_response_code($this->code);
            foreach ($this->headers as $key => $value)
                header("$key: $value");
            if ($this->body)
                echo $this->body;
            die();
        }

    }


    /**
     * Abtract base class for HTTP data responses
     */
    abstract class DataResponse extends Response
    {

        /**
         * Constructs a response
         * 
         * @param string $data Response data
         * @param ?string $extension Extension defining the type of data
         * @param bool $isFile Whether `$data` is a path to a file
         */
        public function __construct(string $data, ?string $extension = null, bool $isFile = false)
        {
            if ($isFile && $extension === null)
            {
                $parts = explode('.', $data);
                $extension = $parts[count($parts) - 1];
            }
            $this
                ->setHeader('Content-Type', [
                    $extension
                        ? \F2P2\extensionToMIME($extension)
                        : 'text/html',
                    'charset=utf-8',
                ])
                ->setBody(
                    $isFile
                        ? file_get_contents($data)
                        : $data
                );
        }

    }


    /**
     * Class for responses displayed as a webpage
     */
    class PageResponse extends DataResponse
    {

        public function __construct(string $data, ?string $extension = null, bool $isFile = false)
        {
            parent::__construct($data, $extension, $isFile);
            $this->setHeader('Content-Disposition', 'inline');
        }

    }


    /**
     * Class for responses that are downloads
     */
    class DownloadResponse extends DataResponse
    {

        public function __construct(string $data, ?string $extension = null, bool $isFile = false, ?string $fileName = null)
        {
            parent::__construct($data, $extension, $isFile);
            if ($fileName)
                $fileName = $fileName;
            else if ($isFile)
            {
                $parts = explode('/', $data);
                $fileName = $parts[count($parts)-1];
            }
            $this->setHeader('Content-Disposition', 'attachment' . (
                $fileName
                    ? '; filename="' . $fileName . '"'
                    : ''
            ));
        }

    }


    /**
     * HTTP error response
     */
    class ErrorResponse extends Response
    {

        /**
         * @param int $code Error code
         */
        public function __construct(int $code)
        {
            $this->setCode($code);
        }

    }


    /**
     * Class for responses that are redirections to URL
     */
    class RedirectURLResponse extends Response
    {

        /**
         * @param string $url Redirection URL
         */
        public function __construct(string $url)
        {
            $this
                ->setCode(302)
                ->setHeader('Location', $url);
        }

    }


    /**
     * Class for responses that are redirections to route
     */
    class RedirectRouteResponse extends RedirectURLResponse
    {

        /**
         * @param string $route Redirection route name
         * @param array $params Route parameters
         * @param array $get GET parameters
         */
        public function __construct(string $route, array $params = [], array $get = [])
        {
            parent::__construct(\F2P2\View::route($route, $params, $get));
        }

    }

?>
