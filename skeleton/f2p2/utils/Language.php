<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2;


if (!defined('LANG'))
    define('LANG', 'en');


/**
 * The class that handles the language and translation of the website
 */
final class Language
{

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
    public static function init(): void
    {
        foreach (scandir('lang') as $entry)
            if ($entry !== '.' && $entry !== '..' && str_ends_with($entry, '.php'))
                self::$availables[] = substr($entry, 0, -4);
        sort(self::$availables, SORT_STRING | SORT_FLAG_CASE);
        if (self::available(LANG))
            self::$defaults = require_once('lang/' . LANG . '.php');
        if (isset($_SESSION['lang']))
            self::use($_SESSION['lang']);
        else
        {
            $langs = [];
            foreach (explode(',', $_SERVER['HTTP_ACCEPT_LANGUAGE']) as $index => $entry)
            {
                $parts = explode(';', $entry);
                $count = count($parts);
                if ($count < 1 || $count > 2)
                    continue;
                $length = strlen($parts[0]);
                if (!($length === 2 || $length === 5 && $parts[0][2] === '-'))
                    continue;
                $q = 1;
                if ($count === 2)
                {
                    if (str_starts_with($parts[1], 'q='))
                        $q = filter_var(substr($parts[1], 2), FILTER_VALIDATE_FLOAT);
                    else
                        continue;
                    if ($q === false || $q < 0 || $q > 1)
                        continue;
                }
                $langs[] = [$index, $parts[0], $q];
            }
            usort($langs, static function ($a, $b) {
                return $a[2] === $b[2]
                    ? $a[0] <=> $b[0]
                    : $a[2] <=> $b[2];
            });
            foreach ($langs as $lang)
            {
                if (self::available($lang[1]))
                    self::use($lang[1]);
                else if (strlen($lang[1]) === 5 && self::available(substr($lang[1], 0, 2)))
                    self::use(substr($lang[1], 0, 2));
                else
                    continue;
                break;
            }
        }
        return;
    }

    /**
     * Checks whether a language is available
     */
    public static function available(string $lang): bool
    {
        return in_array($lang, self::$availables);
    }

    /**
     * Returns available languages
     */
    public static function availables(): array
    {
        return self::$availables;
    }

    /**
     * Returns the current language used
     * 
     * @return string Language used
     */
    public static function current(): string
    {
        return isset($_SESSION['lang'])
            ? $_SESSION['lang']
            : LANG;
    }

    /**
     * Sets the language to use
     * 
     * @param string $lang Language to use
     */
    public static function use(string $lang): void
    {
        $_SESSION['lang'] = $lang;
        if ($lang !== LANG && self::available($lang))
            self::$texts = require("lang/$lang.php");
        else
            self::$texts = [];
        return;
    }

    /**
     * Returns a text from its identifier
     * 
     * @param string $id Text identifier
     * 
     * @return string Corresponding text
     */
    public static function get(string $id): string
    {
        if (isset($_SESSION['lang']) && $_SESSION['lang'] !== LANG && array_key_exists($id, self::$texts))
            return self::$texts[$id];
        if (array_key_exists($id, self::$defaults))
            return self::$defaults[$id];
        return $id;
    }

}
