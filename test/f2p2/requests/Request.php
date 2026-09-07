<?php

    namespace f2p2\requests;


    require_once('f2p2/requests/File.php');
    require_once('f2p2/utils/Server.php');


    /**
     * HTTP request
     */
    class Request
    {

        /**
         * Request error
         */
        private ?string $error = null;

        /**
         * Request url
         */
        private ?string $url = null;

        /**
         * Request method
         */
        private string $method = 'GET';

        /**
         * HTTP headers
         */
        private array $headers = [];

        /**
         * Request port
         */
        private int $port = 80;

        /**
         * GET or POST arguments
         */
        private array $args = [];

        /**
         * Files
         */
        private array $files = [];

        /**
         * Request timeout
         */
        private ?int $timeout = null;

        /**
         * Transfer minimum speed
         */
        private ?int $transferMinSpeed = null;

        /**
         * Transfer speed verification time
         */
        private ?int $transferMinTime = null;

        /**
         * Transfer resume position
         */
        private ?int $transferResume = null;

        /**
         * Download max size
         */
        private ?int $downloadMaxSize = null;

        /**
         * Download max speed
         */
        private ?int $downloadMaxSpeed = null;

        /**
         * Download destination path
         */
        private ?string $downloadPath = null;

        /**
         * Upload size
         */
        private ?int $uploadSize = null;

        /**
         * Upload max speed
         */
        private ?int $uploadMaxSpeed = null;

        /**
         * Upload file path
         */
        private ?string $uploadPath = null;


        /**
         * Constructs a request from the actual session
         * 
         * @return Request Resulting request
         */
        public static function current(): Request
        {
            $request = (new Request())
                ->setUrl(\f2p2\Server::currentURI())
                ->setMethod(\f2p2\Server::requestMethod())
                ->setPort(\f2p2\Server::serverPort())
                ->setHeaders(getallheaders())
                ->setArguments(
                    \f2p2\Server::requestMethod() === 'GET'
                        ? $_GET
                        : (
                            \f2p2\Server::requestMethod() === 'POST'
                                ? $_POST
                                : []
                        )
                );
            $request->files = \f2p2\requests\File::currents();
            return ($request);
        }

        /**
         * Returns the error generated when the request was sent
         * 
         * @return string Error if there is one, `null` otherwise
         */
        public function getError(): ?string
        {
            return ($this->error);
        }

        /**
         * Returns the request URL
         * 
         * @return string Request url
         */
        public function getURL(): ?string
        {
            return ($this->url);
        }

        /**
         * Returns the request method
         * 
         * @return string Request method
         */
        public function getMethod(): string
        {
            return ($this->method);
        }

        /**
         * Returns the request port
         * 
         * @return int Request port
         */
        public function getPort(): int
        {
            return ($this->port);
        }

        /**
         * Returns the request headers
         * 
         * @return array Request headers (name => value)
         */
        public function getHeaders(): array
        {
            return ($this->headers);
        }

        /**
         * Returns an header of the request
         * 
         * @param string $name Header name
         * 
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
         * Returns the request arguments
         * 
         * @return array Request arguments
         */
        public function getArguments(): array
        {
            return ($this->args);
        }

        /**
         * Returns a request argument
         * 
         * @param string $name Argument name
         * 
         * @return ?string Argument value if exists, `null` otherwise
         */
        public function getArgument(string $name): ?string
        {
            return (
                array_key_exists($name, $this->args)
                    ? $this->args[$name]
                    : null
            );
        }

        /**
         * Returns the request files
         * 
         * @return array Request files
         */
        public function getFiles(): array
        {
            return ($this->files);
        }

        /**
         * Returns a request file
         * 
         * @param string $name File name
         * 
         * @return ?\f2p2\requests\File File if exists, `null` otherwise
         */
        public function getFile(string $name): ?\f2p2\requests\File
        {
            return (
                isset($this->files[$name])
                    ? $this->files[$name]
                    : null
            );
        }

        /**
         * Returns all the request data
         * 
         * @return array Request data
         */
        public function getData(): array
        {
            return (array_merge($this->args, $this->files));
        }

        /**
         * Sets the request URL
         * 
         * @param string $url Request URL
         * 
         * @return Request Reference to this request
         */
        public function setURL(string $url): Request
        {
            $this->url = $url;
            return ($this);
        }

        /**
         * Sets the request method
         * 
         * @param string $method Request method
         * 
         * @return Request Reference to this request
         */
        public function setMethod(string $method): Request
        {
            $this->method = $method;
            return ($this);
        }

        /**
         * Sets the request port
         * 
         * @param int $port Request port
         * 
         * @return Request reference to this request
         */
        public function setPort(int $port): Request
        {
            $this->port = $port;
            return ($this);
        }

        /**
         * Sets the request headers
         * 
         * @param array $headers Request headers (name => value)
         * 
         * @return Request Reference to this request
         */
        public function setHeaders(array $headers): Request
        {
            foreach ($headers as $name => $value)
                $this->setHeader($name, $value);
            return $this;
        }

        /**
         * Sets a header of the request
         * 
         * @param string $name Header name
         * @param string|array $value Header value
         * 
         * @return Request Reference to this request
         */
        public function setHeader(string $name, string|array $value): Request
        {
            $this->headers[$name] = (
                is_array($value)
                    ? implode('; ', $value)
                    : $value
            );
            return ($this);
        }

        /**
         * Defines the GET or POST arguments of the request
         * 
         * @param array $args Request arguments (name => value)
         * 
         * @return Request Reference to this request
         */
        public function setArguments(array $args): Request
        {
            foreach ($args as $name => $value)
                $this->setArgument($name, $value);
            return ($this);
        }

        /**
         * Sets a GET or POST argument of the request
         * 
         * @param string $name  Argument name
         * @param string $value Argument value
         * 
         * @return Request Reference to this request
         */
        public function setArgument(string $name, string $value): Request
        {
            $this->args[$name] = $value;
            return ($this);
        }

        /**
         * Sets the request timeout
         * 
         * @param int $timeout Request timeout (ms)
         * 
         * @return Request Reference to this request
         */
        public function setTimeout(int $timeout): Request
        {
            $this->timeout = $timeout;
            return ($this);
        }

        /**
         * Sets the file transfer min speed
         * 
         * @param int $speed Transfer speed (o/s)
         * @param ?int $time Verification duration (s)
         * 
         * @return Request Reference to this request
         */
        public function setTransferMinSpeed(int $speed, ?int $time = null): Request
        {
            $this->transferMinSpeed = $speed;
            $this->transferMinTime = $time;
            return ($this);
        }

        /**
         * Sets the position of the transfer resume
         * 
         * @param int $position Position (o)
         * 
         * @return Request Reference to this request
         */
        public function setTransferResume(int $position): Request
        {
            $this->transferResume = $position;
            return ($this);
        }

        /**
         * Sets the download max speed
         * 
         * @param int $speed Speed (o/s)
         * 
         * @return Request Reference to this request
         */
        public function setDownloadMaxSpeed(int $speed): Request
        {
            $this->downloadMaxSpeed = $speed;
            return ($this);
        }

        /**
         * Sets the download max size
         * 
         * @param int $size Size (o)
         * 
         * @return Request Reference to this request
         */
        public function setDownloadMaxSize(int $size): Request
        {
            $this->downloadMaxSize = $size;
            return ($this);
        }

        /**
         * Sets the transfer file destination
         * 
         * @param string $path Destination path
         * 
         * @return Request Reference to this request
         */
        public function setDownloadPath(string $path): Request
        {
            $this->downloadPath = $path;
            return ($this);
        }

        /**
         * Sets the upload max speed
         * 
         * @param int $speed Speed (o/s)
         * 
         * @return Request Reference to this request
         */
        public function setUploadMaxSpeed(int $speed): Request
        {
            $this->uploadMaxSpeed = $speed;
            return ($this);
        }

        /**
         * Sets the upload size
         * 
         * @param int $size Size (o)
         * 
         * @return Request Reference to this request
         */
        public function setUploadSize(int $size): Request
        {
            $this->uploadSize = $size;
            return ($this);
        }

        /**
         * Defines the path of the file to upload
         * 
         * @param string $path Path of the file
         * 
         * @return Request Reference to this request
         */
        public function setUploadPath(string $path): Request
        {
            $this->uploadPath = $path;
            return ($this);
        }

        /**
         * Adds a file to the request
         * 
         * @param ?string $path Path to the file
         * @param ?string $content Content of the file if there is one
         * @param ?string $mime Mime type of the file
         * @param ?string $name Download name of the file
         * 
         * @return Request Reference to this request
         */
        public function addFile(?string $path, ?string $content, ?string $mime, ?string $name): Request
        {
            $this->files[] = (
                $path
                    ? new \CURLFile($path, $mime, $name)
                    : new \CURLStringFile($content, $mime, $name)
            );
            return ($this);
        }

        /**
         * Send the request
         * 
         * @return ?\f2p2\responses\Response Response received if success, `null` otherwise
         * 
         * @throws \Exception If URL is not defined
         */
        public function send(): ?\f2p2\responses\Response
        {
            if ($this->url === null)
                throw new \Exception('Cannot send the request because URL is not defined');
            $curl = curl_init($this->url . (
                $this->method === 'GET'
                    ? http_build_query($this->args, '?')
                    : ''
            ));
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            if ($this->downloadPath === null)
                curl_setopt($curl, CURLOPT_HEADER, true);
            curl_setopt($curl, CURLOPT_CUSTOMREQUEST, $this->method);
            curl_setopt($curl, CURLOPT_PORT, $this->port);
            $headers = [];
            foreach ($this->headers as $key => $value)
                $headers[] = "$key: $value";
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            if ($this->method === 'POST')
                curl_setopt($curl, CURLOPT_POSTFIELDS, $this->getData());
            foreach ([
                CURLOPT_TIMEOUT_MS => $this->timeout,
                CURLOPT_LOW_SPEED_LIMIT => $this->transferMinSpeed,
                CURLOPT_LOW_SPEED_TIME => $this->transferMinTime,
                CURLOPT_RESUME_FROM => $this->transferResume,
                CURLOPT_MAXFILESIZE_LARGE => $this->downloadMaxSize,
                CURLOPT_MAX_RECV_SPEED_LARGE => $this->downloadMaxSpeed,
                CURLOPT_INFILE => $this->uploadSize,
                CURLOPT_MAX_SEND_SPEED_LARGE => $this->uploadMaxSpeed,
            ] as $option => $value)
            {
                if ($value)
                    curl_setopt($curl, $option, $value);
            }
            $downloadFile = (
                $this->downloadPath
                    ? fopen($this->downloadPath, 'w')
                    : null
            );
            if ($downloadFile)
                curl_setopt($curl, CURLOPT_FILE, $downloadFile);
            $uploadFile = (
                $this->uploadPath
                    ? fopen($this->uploadPath, 'r')
                    : null
            );
            if ($uploadFile)
                curl_setopt($curl, CURLOPT_INFILE, $uploadFile);
            $responseTXT = curl_exec($curl);
            $this->error = curl_error($curl);
            if ($downloadFile)
                fclose($downloadFile);
            if ($uploadFile)
                fclose($uploadFile);
            if ($this->error === '')
            {
                $this->error = null;
                $headerSize = curl_getinfo($curl, CURLINFO_HEADER_SIZE);
                $response = (new \f2p2\responses\Response())
                    ->setCode(curl_getinfo($curl, CURLINFO_HTTP_CODE))
                    ->setBody(substr($responseTXT, $headerSize));
                foreach (explode("\n", substr($responseTXT, 0, $headerSize)) as $header)
                {
                    $parts = explode(': ', $header);
                    if (count($parts) === 2)
                        $response->setHeader($parts[0], $parts[1]);
                }
                return ($response);
            }
            else
                return (null);
        }

    }

?>
