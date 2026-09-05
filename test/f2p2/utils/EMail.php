<?php

    namespace F2P2;


    /**
     * The class that can generate and send an email
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class EMail
    {

        private ?string $from_email = null;
        private ?string $from_name  = null;
        private array   $to         = [];
        private string  $subject    = '';
        private string  $message    = '';
        private array   $headers    = [];
        private bool    $isHTML     = false;


        /**
         * Defines the sender of the email
         * 
         * @param  string  $email The email of the sender
         * @param  ?string $name  The name of the sender
         * @return EMail          The email
         */
        public function from(
            string  $email,
            ?string $name  = null
        ) : EMail
        {
            $this->from_email = $email;
            $this->from_name = $name;
            return $this;
        }

        /**
         * Add a recipient to the mail
         * 
         * @param  string  $email The email of the recipient
         * @param  ?string $name  The name of the recipient
         * @return EMail          The email
         */
        public function to(
            string  $email,
            ?string $name  = null
        ) : EMail
        {
            $this->to[$email] = $name;
            return $this;
        }

        /**
         * Defines the subject of the email
         * 
         * @param  string $subject The subject
         * @return EMail           The email
         */
        public function subject(
            string $subject
        ) : EMail
        {
            $this->subject = $subject;
            return $this;
        }

        /**
         * Defines the message of the email
         * 
         * @param  string $message The message
         * @return EMail           The email
         */
        public function message(
            string $message
        ) : EMail
        {
            $this->message = $message;
            return $this;
        }

        /**
         * Add a header to the email
         * 
         * @param  string $name  The header name
         * @param  string $value The header value
         * @return EMail         The email
         */
        public function header(
            string $name,
            string $value
        ) : EMail
        {
            $this->headers[$name] = $value;
            return $this;
        }

        /**
         * Enable HTML for the email
         * 
         * @param  string $message The message
         * @return EMail           The email
         */
        public function enableHTML() : EMail
        {
            $this->isHTML = true;
            return $this;
        }

        /**
         * Send the email
         * 
         * @return bool true if the email was sent successfully, otherwise false
         */
        public function send() : bool
        {
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
            if ($this->from_email)
            {
                $headers['From'] = $this->from_name ? $this->from_name.' <'.$this->from_email.'>' : $this->from_email;
            }
            if (count($this->to) > 0)
            {
                $to = [];
                foreach ($this->to as $email => $name)
                {
                    $to[] = $name ? $name.' <'.$email.'>' : $email;
                }
                $headers['To'] = implode(', ', $to);
            }
            return mail(implode(', ', array_keys($this->to)), $this->subject, $this->message, $headers);
        }
    }
?>
