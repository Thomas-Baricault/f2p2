<?php
    namespace f2p2;

use DateTimeImmutable;

    /**
     * Read the JSON of a file
     * 
     * @param  string $path The file path
     * @return array        The JSON
     */
    function readJSON(
        string $path
    ) : array
    {
        return file_exists($path) ? (json_decode(file_get_contents($path), true) ?? []) : [];
    }


    /**
     * Returns the MIME equivalent for a file extension
     * 
     * @param  string  $extension The extension
     * @return ?string            The MIME type if found, otherwise null
     */
    function extensionToMIME(
        string $extension
    ) : ?string
    {
        $data = readJSON('config/mime.json');
        foreach ($data as $mime => $extensions)
        {
            if (is_array($extensions) && in_array($extension, $extensions) || !is_array($extensions) && $extensions === $extension)
            {
                return $mime;
            }
        }
        return null;
    }


    /**
     * Returns a file extension equivalent for a MIME type
     * 
     * @param  string  $mime The MIME type
     * @return ?string       The extension if found, otherwise null
     */
    function mimeToExtension(
        string $mime
    ) : ?string
    {
        $data = readJSON('config/mime.json');
        return array_key_exists($mime, $data) ? (is_array($data[$mime]) ? $data[$mime][0] : $data[$mime]) : null;
    }

    function validateDate(string $value, string $format)
    {
        $date = DateTimeImmutable::createFromFormat('!' . $format, $value);
        return ($date !== false && $date->format($format) === $value);
    }
?>
