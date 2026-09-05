<?php
    namespace F2P2;


    /**
     * The class that handles the language and translation of the website
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Language
    {
        static private string $language = DEFAULT_LANGUAGE;
        static private array  $data     = [];


        /**
         * Initialize the language data
         * 
         * @param  array $data The data (language => id => text)
         * @return void
         */
        static public function init(
            array $data
        ) : void
        {
            if (isset($_SESSION['language']))
            {
                self::$language = $_SESSION['language'];
            }
            self::$data = $data;
        }

        /**
         * Returns the actual language used
         * 
         * @return string The language
         */
        static public function getLanguage() : string
        {
            return self::$language;
        }

        /**
         * Defines the language to use
         * 
         * @param  string $language The language
         * @return void
         */
        static public function setLanguage(
            string $language
        ) : void
        {
            self::$language = $language;
            $_SESSION['language'] = $language;
        }

        /**
         * Returns all the languages used
         * 
         * @return array An array of languages
         */
        static public function getAllLanguages() : array
        {
            return array_keys(self::$data);
        }

        /**
         * Returns a text corresponding to the actual language
         * 
         * @param  string $id The id of the text
         * @return string     The text
         */
        static public function getText(
            string $id
        ) : string
        {
            if (array_key_exists(self::$language, self::$data) && array_key_exists($id, self::$data[self::$language]))
            {
                return self::$data[self::$language][$id];
            }
            else if (array_key_exists(DEFAULT_LANGUAGE, self::$data) && array_key_exists($id, self::$data[DEFAULT_LANGUAGE]))
            {
                return self::$data[DEFAULT_LANGUAGE][$id];
            }
            else
            {
                return $id;
            }
        }
    }
?>