<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2;


/**
 * Class that can generate and send an email
 */
final class Email
{

    /**
     * Sender email
     */
    private ?string $fromEmail = null;

    /**
     * Sender name
     */
    private ?string $fromName = null;

    /**
     * Email recipients
     */
    private array $to = [];

    /**
     * Email subject
     */
    private string $subject = '';

    /**
     * Email message
     */
    private string $message = '';

    /**
     * Email headers
     */
    private array $headers = [];

    /**
     * Whether content has to be interpreted as HTML
     */
    private bool $isHTML = false;


    /**
     * Checks whether an email is syntaxically valid
     */
    public static function validate(string $email): bool
    {
        $parts = explode('@', $email);
        return filter_var($email, FILTER_VALIDATE_EMAIL) && count($parts) === 2 && checkdnsrr($parts[1]);
    }

    /**
     * Sets the sender of the email
     * 
     * @param string $email Email of the sender
     * @param ?string $name Name of the sender
     * 
     * @return static Reference to this email
     */
    public function from(string $email, ?string $name = null): static
    {
        $this->fromEmail = $email;
        $this->fromName = $name;
        return $this;
    }

    /**
     * Add a recipient to the email
     * 
     * @param string $email Email of the recipient
     * @param ?string $name Name of the recipient
     * 
     * @return static Reference to this email
     */
    public function to(string $email, ?string $name = null): static
    {
        $this->to[$email] = $name;
        return $this;
    }

    /**
     * Sets the subject of the email
     * 
     * @param string $subject Email subject
     * 
     * @return static Reference to this email
     */
    public function subject(string $subject): static
    {
        $this->subject = $subject;
        return $this;
    }

    /**
     * Sets the message of the email
     * 
     * @param string $message Email message
     * 
     * @return static Reference to this email
     */
    public function message(string $message): static
    {
        $this->message = $message;
        return $this;
    }

    /**
     * Add a header to the email
     * 
     * @param string $name Header name
     * @param string $value Header value
     * 
     * @return static Reference to this email
     */
    public function header(string $name, string $value): static
    {
        $this->headers[$name] = $value;
        return $this;
    }

    /**
     * Enable HTML for the email
     * 
     * @return static Reference to this email
     */
    public function enableHTML(): static
    {
        $this->isHTML = true;
        return $this;
    }

    /**
     * Send the email
     * 
     * @return bool `true` if the email was sent successfully, `false` otherwise
     * 
     * @throws \RuntimeException If no recipient has been defined
     */
    public function send(): bool
    {
        if (count($this->to) === 0)
            throw new \RuntimeException("Cannot send email with no recipient");
        $headers = $this->headers;
        if ($this->isHTML)
        {
            $headers['MIME-Version'] = '1.0';
            $headers['Content-Type'] = 'text/html; charset=utf-8';
        }
        else
        {
            $headers['Content-Type'] = 'text/plain; charset=utf-8';
        }
        if ($this->fromEmail)
        {
            $headers['From'] = $this->fromName
                ? $this->fromName . ' <' . $this->fromEmail . '>'
                : $this->fromEmail;
        }
        $to = [];
        foreach ($this->to as $email => $name)
        {
            $to[] = $name
                ? $name . ' <' . $email . '>'
                : $email;
        }
        $headers['To'] = implode(', ', $to);
        return mail(
            implode(', ', array_keys($this->to)),
            $this->subject,
            $this->message,
            $headers
        );
    }

}
