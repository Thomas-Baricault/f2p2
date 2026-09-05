<?php
    namespace F2P2;


    /**
     * The class that represent a network response
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Response
    {
        private int     $code    = 200;
        private array   $headers = [];
        private ?string $body    = null;


        /**
         * Returns the response status code
         * 
         * @return int The status code
         */
        public function getCode() : int
        {
            return $this->code;
        }

        /**
         * Defines the response status code
         * 
         * @param  int      $code The code
         * @return Response       The response
         */
        public function setCode(
            int $code
        ) : Response
        {
            $this->code = $code;
            return $this;
        }

        /**
         * Returns the response headers
         * 
         * @return array The headers (name => value)
         */
        public function getHeaders() : array
        {
            return $this->headers;
        }

        /**
         * Returns a response header
         * 
         * @param  string  $name The header name
         * @return ?string       The header value if exists, otherwise null
         */
        public function getHeader(
            string $name
        ) : ?string
        {
            return array_key_exists($name, $this->headers) ? $this->headers[$name] : null;
        }

        /**
         * Defines the response headers
         * 
         * @param  array    $headers The headers (name => value)
         * @return Response          The response
         */
        public function setHeaders(
            array $headers
        ) : Response
        {
            foreach ($headers as $name => $value)
            {
                $this->addHeader($name, $value);
            }
            return $this;
        }

        /**
         * Add a header to the response
         * 
         * @param  string       $name  The header name
         * @param  string|array $value The header value
         * @return Response            The response
         */
        public function addHeader(
            string       $name,
            string|array $value
        ) : Response
        {
            $this->headers[$name] = is_array($value) ? implode('; ', $value) : $value;
            return $this;
        }

        /**
         * Returns the response body content
         * 
         * @return ?string The data
         */
        public function getBody() : ?string
        {
            return $this->code;
        }

        /**
         * Defines the response body content
         * 
         * @param  string   $body The data
         * @return Response       The response
         */
        public function setBody(
            string $body
        ) : Response
        {
            $this->body = $body;
            return $this;
        }

        /**
         * Send the response
         * 
         * @return void
         */
        public function send() : void
        {
            http_response_code($this->code);
            foreach ($this->headers as $key => $value)
            {
                header("$key: $value");
            }
            if ($this->body)
            {
                echo $this->body;
            }
            die();
        }
    }


    /**
     * The base class for data responses
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class ResponseData extends Response
    {
        /**
         * @param string  $data      The data
         * @param ?string $extension The extension defining the type of data
         * @param bool    $isFile    true if the content of $data is a path to a file
         */
        public function __construct(
            string  $data,
            ?string $extension = null,
            bool    $isFile    = false
        ) {
            if ($isFile && $extension === null)
            {
                $parts = explode('.', $data);
                $extension = $parts[count($parts)-1];
            }
            $this
                ->addHeader('Content-Type', [($extension ? \F2P2\extensionToMIME($extension) : null) ?? 'text/html', 'charset=utf-8'])
                ->setBody($isFile ? file_get_contents($data) : $data);
        }
    }


    /**
     * The class that represent responses displayed as a webpage
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class ResponsePage extends ResponseData
    {
        public function __construct(
            string  $data,
            ?string $extension = null,
            bool    $isFile    = false
        ) {
            parent::__construct($data, $extension, $isFile);
            $this->addHeader('Content-Disposition', 'inline');
        }
    }


    /**
     * The class that represent responses that are downloads
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class ResponseDownload extends ResponseData
    {
        public function __construct(
            string  $data,
            ?string $extension = null,
            bool    $isFile    = false,
            ?string $fileName  = null
        ) {
            parent::__construct($data, $extension, $isFile);
            if ($fileName)
            {
                $fileName = $fileName;
            }
            else if ($isFile)
            {
                $parts = explode('/', $data);
                $fileName = $parts[count($parts)-1];
            }
            $this->addHeader('Content-Disposition', 'attachment'.($fileName ? '; filename="'.$fileName.'"' : ''));
        }
    }


    /**
     * The class that represent a http error response
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class ResponseError extends Response
    {
        /**
         * @param int $code The error code
         */
        public function __construct(
            int $code
        ) {
            $this->setCode($code);
        }
    }


    /**
     * The class that represent a redirection to an URL
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class ResponseRedirectURL extends Response
    {
        /**
         * @param string $url The redirection URL
         */
        public function __construct(
            string $url
        ) {
            $this
                ->setCode(302)
                ->addHeader('Location', $url);
        }
    }


    /**
     * The class that represent a redirection to a route
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class ResponseRedirectRoute extends ResponseRedirectURL
    {
        /**
         * @param string $route The redirection route name
         */
        public function __construct(
            string $route,
            array  $params = [],
            array  $get    = []
        ) {
            parent::__construct(\F2P2\Router::getRouteURL($route, $params, $get));
        }
    }
?>