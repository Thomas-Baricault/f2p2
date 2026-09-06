<?php

    namespace F2P2;


    if (!defined('LANG'))
        define('LANG', 'en');


    /**
     * The class that handles the language and translation of the website
     */
    class Language
    {

        /**
         * Current language used
         */
        private static string $lang = LANG;

        /**
         * Available languages
         */
        private static array $availables = [];

        /**
         * Default texts
         */
        private static array $defaults = [];

        /**
         * Current language texts
         */
        private static array $texts = [];


        /**
         * Initializes the languages service
         */
        public static function init() : void
        {
            if (isset($_SESSION['lang']))
                self::use($_SESSION['lang']);
            else
                self::$lang = LANG;
            $path = 'lang/' . LANG . '.php';
            if (file_exists($path))
                self::$defaults = require($path);
        }

        /**
         * Returns available languages
         */
        public static function availables() : array
        {
            return (self::$availables);
        }

        /**
         * Returns the current language used
         * 
         * @return string Language used
         */
        public static function current() : string
        {
            return (self::$lang);
        }

        /**
         * Sets the language to use
         * 
         * @param string $lang Language to use
         */
        public static function use(string $lang) : void
        {
            self::$lang = $lang;
            if ($lang == LANG)
            {
                unset($_SESSION['lang']);
                self::$texts = [];
            }
            else
            {
                $_SESSION['lang'] = $lang;
                $path = "lang/$lang.php";
                if (file_exists($path))
                    self::$texts = require($path);
                else
                    self::$texts = [];
            }
        }

        /**
         * Returns a text from its identifier
         * 
         * @param string $id Text identifier
         * 
         * @return string Corresponding text
         */
        public static function get(string $id) : string
        {
            if (self::$lang !== LANG && array_key_exists($id, self::$texts))
                return (self::$texts[$id]);
            if (array_key_exists($id, self::$defaults))
                return (self::$defaults[$id]);
            return ($id);
        }

    }

?>