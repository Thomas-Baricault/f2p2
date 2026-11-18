<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http\requests;


require_once('f2p2/http/requests/File.php');
require_once('f2p2/http/Message.php');
require_once('f2p2/http/Method.php');
require_once('f2p2/utils/Server.php');


/**
 * HTTP request
 */
class Request extends \f2p2\http\Message
{

    /**
     * Request error
     */
    public ?string $error = null;

    /**
     * Request url
     */
    public ?string $url = null;

    /**
     * Request method
     */
    public \f2p2\http\Method $method = \f2p2\http\Method::Get;

    /**
     * Request port
     */
    public int $port = 80;

    /**
     * GET or POST arguments
     */
    public array $args = [];

    /**
     * Files
     */
    public array $files = [];

    /**
     * Request timeout
     */
    public ?int $timeout = null;

    /**
     * Transfer minimum speed
     */
    public ?int $transferMinSpeed = null;

    /**
     * Transfer speed verification time
     */
    public ?int $transferMinTime = null;

    /**
     * Transfer resume position
     */
    public ?int $transferResume = null;

    /**
     * Download max size
     */
    public ?int $downloadMaxSize = null;

    /**
     * Download max speed
     */
    public ?int $downloadMaxSpeed = null;

    /**
     * Download destination path
     */
    public ?string $downloadPath = null;

    /**
     * Upload size
     */
    public ?int $uploadSize = null;

    /**
     * Upload max speed
     */
    public ?int $uploadMaxSpeed = null;

    /**
     * Upload file path
     */
    public ?string $uploadPath = null;


    /**
     * Constructs a request from the actual session
     * 
     * @return Request Resulting request
     */
    public static function current(): Request
    {
        $request = (new Request())
            ->url(\f2p2\Server::currentURI())
            ->method(\f2p2\Server::requestMethod())
            ->port(\f2p2\Server::serverPort())
            ->headers(getallheaders())
            ->args(match (\f2p2\Server::requestMethod()) {
                \f2p2\http\Method::Get => $_GET,
                \f2p2\http\Method::Post => $_POST,
                default => [],
            });
        $request->files = \f2p2\http\requests\File::currents();
        return ($request);
    }

    /**
     * Returns all the request data
     * 
     * @return array Request data
     */
    public function data(): array
    {
        return (array_merge($this->args, $this->files));
    }

    /**
     * Sets the request URL
     * 
     * @param string $url Request URL
     * 
     * @return static Reference to this request
     */
    public function url(string $url): static
    {
        $this->url = $url;
        return ($this);
    }

    /**
     * Sets the request method
     * 
     * @param \f2p2\http\Method $method Request method
     * 
     * @return static Reference to this request
     */
    public function method(\f2p2\http\Method $method): static
    {
        $this->method = $method;
        return ($this);
    }

    /**
     * Sets the request port
     * 
     * @param int $port Request port
     * 
     * @return static reference to this request
     */
    public function port(int $port): static
    {
        $this->port = $port;
        return ($this);
    }

    public function arg(string $name, null|string|array $value = null): null|string|array|static
    {
        if ($value === null)
        {
            return array_key_exists($name, $this->args)
                ? $this->args[$name]
                : null;
        }
        $this->args[$name] = $value;
        return ($this);
    }

    /**
     * Defines the GET or POST arguments of the request
     * 
     * @param array $args Request arguments (name => value)
     * 
     * @return static Reference to this request
     */
    public function args(array $args): static
    {
        foreach ($args as $name => $value)
            $this->arg($name, $value);
        return ($this);
    }

    public function file(string $name): ?\f2p2\http\requests\File
    {
        return array_key_exists($name, $this->files)
            ? $this->files[$name]
            : null;
    }

    /**
     * Sets the request timeout
     * 
     * @param int $timeout Request timeout (ms)
     * 
     * @return static Reference to this request
     */
    public function timeout(int $timeout): static
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
     * @return static Reference to this request
     */
    public function transferMinSpeed(int $speed, ?int $time = null): static
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
     * @return static Reference to this request
     */
    public function transferResume(int $position): static
    {
        $this->transferResume = $position;
        return ($this);
    }

    /**
     * Sets the download max speed
     * 
     * @param int $speed Speed (o/s)
     * 
     * @return static Reference to this request
     */
    public function downloadMaxSpeed(int $speed): static
    {
        $this->downloadMaxSpeed = $speed;
        return ($this);
    }

    /**
     * Sets the download max size
     * 
     * @param int $size Size (o)
     * 
     * @return static Reference to this request
     */
    public function downloadMaxSize(int $size): static
    {
        $this->downloadMaxSize = $size;
        return ($this);
    }

    /**
     * Sets the transfer file destination
     * 
     * @param string $path Destination path
     * 
     * @return static Reference to this request
     */
    public function downloadPath(string $path): static
    {
        $this->downloadPath = $path;
        return ($this);
    }

    /**
     * Sets the upload max speed
     * 
     * @param int $speed Speed (o/s)
     * 
     * @return static Reference to this request
     */
    public function uploadMaxSpeed(int $speed): static
    {
        $this->uploadMaxSpeed = $speed;
        return ($this);
    }

    /**
     * Sets the upload size
     * 
     * @param int $size Size (o)
     * 
     * @return static Reference to this request
     */
    public function uploadSize(int $size): static
    {
        $this->uploadSize = $size;
        return ($this);
    }

    /**
     * Defines the path of the file to upload
     * 
     * @param string $path Path of the file
     * 
     * @return static Reference to this request
     */
    public function uploadPath(string $path): static
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
     * @return static Reference to this request
     */
    public function addFile(?string $path, ?string $content, ?string $mime, ?string $name): static
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
     * @return ?\f2p2\http\responses\Response Response received if success, `null` otherwise
     * 
     * @throws \RuntimeException If URL is not defined
     */
    public function send(): ?\f2p2\http\responses\Response
    {
        if ($this->url === null)
            throw new \RuntimeException('cannot send the request because URL is not defined');
        $curl = curl_init($this->url . (
            $this->method === \f2p2\http\Method::Get
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
        if ($this->method === \f2p2\http\Method::Post)
            curl_setopt($curl, CURLOPT_POSTFIELDS, $this->data());
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
            $response = (new \f2p2\http\responses\Response())
                ->code(curl_getinfo($curl, CURLINFO_HTTP_CODE))
                ->body(substr($responseTXT, $headerSize));
            foreach (explode("\n", substr($responseTXT, 0, $headerSize)) as $header)
            {
                $parts = explode(': ', $header);
                if (count($parts) === 2)
                    $response->header($parts[0], $parts[1]);
            }
            return ($response);
        }
        else
            return (null);
    }

}
