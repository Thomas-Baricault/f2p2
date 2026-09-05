<?php
    namespace F2P2;


    /**
     * The class that represent a network request
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Request
    {
        private ?string $error            = null;
        private ?string $url              = null;
        private string  $method           = 'GET';
        private array   $headers          = [];
        private int     $port             = 80;
        private array   $args             = [];
        private array   $files            = [];
        private ?int    $timeout          = null;
        private ?int    $transferMinSpeed = null;
        private ?int    $transferMinTime  = null;
        private ?int    $transferResume   = null;
        private ?int    $downloadMaxSize  = null;
        private ?int    $downloadMaxSpeed = null;
        private ?string $downloadPath     = null;
        private ?int    $uploadSize       = null;
        private ?int    $uploadMaxSpeed   = null;
        private ?string $uploadPath       = null;


        /**
         * Initialize a request from the actual session
         * 
         * @return Request The request
         */
        static public function fromCurrent() : Request
        {
            $request = (new Request())
                ->setUrl(\F2P2\Server::getCurrentURI())
                ->setMethod(\F2P2\Server::getRequestMethod())
                ->setPort(\F2P2\Server::getServerPort())
                ->setHeaders(getallheaders())
                ->setArguments(\F2P2\Server::getRequestMethod() === 'GET' ? $_GET : (\F2P2\Server::getRequestMethod() === 'POST' ? $_POST : []));
            foreach ($_FILES as $name => $file)
            {
                $request->addFile($file['tmp_name'], $file['type'], $name);
            }
            return $request;
        }

        /**
         * Returns the error generated when the request was sent
         * 
         * @return string The error if there is one, otherwise null
         */
        public function getError() : ?string
        {
            return $this->error;
        }

        /**
         * Returns the request URL
         * 
         * @return string The URL
         */
        public function getURL() : ?string
        {
            return $this->url;
        }

        /**
         * Defines the request URL
         * 
         * @param  string  $url The URL
         * @return Request      The request
         */
        public function setURL(
            string $url
        ) : Request
        {
            $this->url = $url;
            return $this;
        }

        /**
         * Returns the request method
         * 
         * @return string The method
         */
        public function getMethod() : string
        {
            return $this->method;
        }

        /**
         * Defines the request method
         * 
         * @param  string  $method The method
         * @return Request         The request
         */
        public function setMethod(
            string $method
        ) : Request
        {
            $this->method = $method;
            return $this;
        }

        /**
         * Returns the request port
         * 
         * @return string The port
         */
        public function getPort() : int
        {
            return $this->port;
        }

        /**
         * Defines the request port
         * 
         * @param  string  $url The URL
         * @return Request      The request
         */
        public function setPort(
            int $port
        ) : Request
        {
            $this->port = $port;
            return $this;
        }

        /**
         * Returns the request headers
         * 
         * @return array The headers (name => value)
         */
        public function getHeaders() : array
        {
            return $this->headers;
        }

        /**
         * Returns a request header
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
         * Defines the request headers
         * 
         * @param  array   $headers The headers (name => value)
         * @return Request          The request
         */
        public function setHeaders(
            array $headers
        ) : Request
        {
            foreach ($headers as $name => $value)
            {
                $this->addHeader($name, $value);
            }
            return $this;
        }

        /**
         * Add a header to the request
         * 
         * @param  string       $name  The header name
         * @param  string|array $value The header value
         * @return Request             The request
         */
        public function addHeader(
            string       $name,
            string|array $value
        ) : Request
        {
            $this->headers[$name] = is_array($value) ? implode('; ', $value) : $value;
            return $this;
        }

        /**
         * Returns all the request data
         * 
         * @return array The data
         */
        public function getData() : array
        {
            return array_merge($this->args, $this->files);
        }

        /**
         * Returns the request arguments
         * 
         * @return array The arguments
         */
        public function getArguments() : array
        {
            return $this->args;
        }

        /**
         * Returns a request argument
         * 
         * @param  string  $name The argument name
         * @return ?string       The argument value if exists, otherwise null
         */
        public function getArgument(
            string $name
        ) : ?string
        {
            return array_key_exists($name, $this->args) ? $this->args[$name] : null;
        }

        /**
         * Defines the GET or POST arguments of the request
         * 
         * @param  array   $args The arguments (name => value)
         * @return Request       The request
         */
        public function setArguments(
            array $args
        ) : Request
        {
            foreach ($args as $name => $value)
            {
                $this->addArgument($name, $value);
            }
            return $this;
        }

        /**
         * Add a GET or POST argument to the request
         * 
         * @param  string  $name  The argument name
         * @param  string  $value The argument value
         * @return Request        The request
         */
        public function addArgument(
            string $name,
            string $value
        ) : Request
        {
            $this->args[$name] = $value;
            return $this;
        }

        /**
         * Returns the request files
         * 
         * @return array The files
         */
        public function getFiles() : array
        {
            return $this->files;
        }

        /**
         * Returns a request file
         * 
         * @param  string  $name The file name
         * @return ?string       The file if exists, otherwise null
         */
        public function getFile(
            string $name
        ) : ?string
        {
            return array_key_exists($name, $this->files) ? $this->files[$name] : null;
        }

        /**
         * Add a file to the request
         * 
         * @param  string  $path   The path to the file if there is one, otherwise the content
         * @param  ?string $mime   The mime type of the file
         * @param  ?string $name   The download name of the file
         * @param  bool    $isFile true if the path refers to a file
         * @return Request         The request
         */
        public function addFile(
            string  $path,
            ?string $mime,
            ?string $name,
            bool    $isFile = true
        ) : Request
        {
            $this->files[] = $isFile ? new \CURLFile($path, $mime, $name) : new \CURLStringFile($path, $mime, $name);
            return $this;
        }

        /**
         * Defines the request timeout
         * 
         * @param  int     $timeout The timeout (ms)
         * @return Request          The request
         */
        public function setTimeout(
            int $timeout
        ) : Request
        {
            $this->timeout = $timeout;
            return $this;
        }

        /**
         * Defines the file transfer min speed
         * 
         * @param  int     $speed The speed (o/s)
         * @param  ?int    $time  The duration of the verification (s)
         * @return Request        The request
         */
        public function setTransferMinSpeed(
            int  $speed,
            ?int $time  = null
        ) : Request
        {
            $this->transferMinSpeed = $speed;
            $this->transferMinTime = $time;
            return $this;
        }

        /**
         * Defines the download max speed
         * 
         * @param  int     $speed The speed (o/s)
         * @return Request        The request
         */
        public function setDownloadMaxSpeed(
            int $speed
        ) : Request
        {
            $this->downloadMaxSpeed = $speed;
            return $this;
        }

        /**
         * Defines the upload max speed
         * 
         * @param  int     $speed The speed (o/s)
         * @return Request        The request
         */
        public function setUploadMaxSpeed(
            int $speed
        ) : Request
        {
            $this->uploadMaxSpeed = $speed;
            return $this;
        }

        /**
         * Defines the download max size
         * 
         * @param  int     $size The size (o)
         * @return Request       The request
         */
        public function setDownloadMaxSize(
            int $size
        ) : Request
        {
            $this->downloadMaxSize = $size;
            return $this;
        }

        /**
         * Defines the upload size
         * 
         * @param  int     $size The size (o)
         * @return Request       The request
         */
        public function setUploadSize(
            int $size
        ) : Request
        {
            $this->uploadSize = $size;
            return $this;
        }

        /**
         * Defines the position of the transfer resume
         * 
         * @param  int     $position The position (o)
         * @return Request           The request
         */
        public function setTransferResume(
            int $position
        ) : Request
        {
            $this->transferResume = $position;
            return $this;
        }

        /**
         * Defines where the file of the transfer will be downloaded
         * 
         * @param  string  $path The path
         * @return Request       The request
         */
        public function setDownloadPath(
            string $path
        ) : Request
        {
            $this->downloadPath = $path;
            return $this;
        }

        /**
         * Defines the path of the file that will be transfered
         * 
         * @param  string  $path The path of the file
         * @return Request       The request
         */
        public function setUploadPath(
            string $path
        ) : Request
        {
            $this->uploadPath = $path;
            return $this;
        }

        /**
         * Send the request
         * 
         * @return ?\F2P2\Response The response received if success, otherwise null
         */
        public function send() : ?\F2P2\Response
        {
            if ($this->url === null)
            {
                throw new \Exception('Cannot send the request because URL is not defined');
            }
            $curl = curl_init($this->url.($this->method === 'GET' ? http_build_query($this->args, '?') : ''));
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            if ($this->downloadPath === null)
            {
                curl_setopt($curl, CURLOPT_HEADER, true);
            }
            curl_setopt($curl, CURLOPT_CUSTOMREQUEST, $this->method);
            curl_setopt($curl, CURLOPT_PORT, $this->port);
            $headers = [];
            foreach ($this->headers as $key => $value)
            {
                $headers[] = "$key: $value";
            }
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            if ($this->method === 'POST')
            {
                curl_setopt($curl, CURLOPT_POSTFIELDS, $this->getData());
            }
            foreach ([
                CURLOPT_TIMEOUT_MS           => $this->timeout,
                CURLOPT_LOW_SPEED_LIMIT      => $this->transferMinSpeed,
                CURLOPT_LOW_SPEED_TIME       => $this->transferMinTime,
                CURLOPT_RESUME_FROM          => $this->transferResume,
                CURLOPT_MAXFILESIZE_LARGE    => $this->downloadMaxSize,
                CURLOPT_MAX_RECV_SPEED_LARGE => $this->downloadMaxSpeed,
                CURLOPT_INFILE               => $this->uploadSize,
                CURLOPT_MAX_SEND_SPEED_LARGE => $this->uploadMaxSpeed,
            ] as $option => $value)
            {
                if ($value)
                {
                    curl_setopt($curl, $option, $value);
                }
            }
            $downloadFile = $this->downloadPath ? fopen($this->downloadPath, 'w') : null;
            if ($downloadFile)
            {
                curl_setopt($curl, CURLOPT_FILE, $downloadFile);
            }
            $uploadFile = $this->uploadPath ? fopen($this->uploadPath, 'r') : null;
            if ($uploadFile)
            {
                curl_setopt($curl, CURLOPT_INFILE, $uploadFile);
            }
            $responseTXT = curl_exec($curl);
            $this->error = curl_error($curl);
            if ($downloadFile)
            {
                fclose($downloadFile);
            }
            if ($uploadFile)
            {
                fclose($uploadFile);
            }
            if ($this->error === '')
            {
                $this->error = null;
                $headerSize = curl_getinfo($curl, CURLINFO_HEADER_SIZE);
                $response = (new \F2P2\Response())
                    ->setCode(curl_getinfo($curl, CURLINFO_HTTP_CODE))
                    ->setBody(substr($responseTXT, $headerSize));
                foreach (explode("\n", substr($responseTXT, 0, $headerSize)) as $header)
                {
                    $parts = explode(': ', $header);
                    if (count($parts) === 2)
                    {
                        $response->addHeader($parts[0], $parts[1]);
                    }
                }
                curl_close($curl);
                return $response;
            }
            else
            {
                return null;
            }
        }
    }
?>