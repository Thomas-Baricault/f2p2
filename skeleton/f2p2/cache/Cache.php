<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\cache;


require_once('f2p2/cache/Infos.php');


abstract class Cache
{

    private static array $infos = [];


    private static function infos(): \f2p2\cache\Infos
    {
        if (array_key_exists(static::class, self::$infos))
            return self::$infos[static::class];
        $reflection = new \ReflectionClass(static::class);
        $attributes = $reflection->getAttributes(\f2p2\cache\Infos::class);
        if (!$attributes)
            throw new \Exception('no cache infos attribute defined');
        self::$infos[static::class] = $attributes[0]->newInstance();
        return self::$infos[static::class];
    }

    public static function enabled(): bool
    {
        return static::infos()->enabled;
    }

    public static function load(): mixed
    {
        $infos = static::infos();
        if (!$infos->enabled)
            return null;
        $path = "var/cache/{$infos->path}.php";
        if (!is_file($path))
            return null;
        return require($path);
    }

    public static function save(mixed $data): void
    {
        $infos = static::infos();
        if (!$infos->enabled)
            return;
        $path = "var/cache/{$infos->path}.php";
        $directory = dirname($path);
        if (!is_dir($directory))
            mkdir($directory, 0777, true);
        file_put_contents($path, '<?php return ' . var_export($data, true) . ';', LOCK_EX);
        return;
    }

    public static function clear(): void
    {
        $infos = static::infos();
        $path = "var/cache/{$infos->path}.php";
        if (is_file($path))
            unlink($path);
        return;
    }

};
