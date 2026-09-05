<?php
    namespace F2P2;


    const REGEX_DATE          = '/(19|20)\d\d-(0[1-9]|1[012])-([012]\d|3[01])/';
    const REGEX_DATETIMELOCAL = '/(19|20)\d\d-(0[1-9]|1[012])-([012]\d|3[01])T([01]\d|2[0-3]):([0-5]\d)/';
    const REGEX_TIME          = '/([01]\d|2[0-3]):([0-5]\d)/';
    const REGEX_MONTH         = '/(19|20)\d\d-(0[1-9]|1[012])/';
    const REGEX_WEEK          = '/(19|20)\d\d-W([0-4]\d)|(5[0123])/';


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
     * Check if email is a valid one
     * 
     * @param  string $email The email
     * @return bool          If it's valid
     */
    function emailIsValid(
        string $email
    ) : bool
    {
        $parts = explode('@', $email);
        return filter_var($email, FILTER_VALIDATE_EMAIL) && count($parts) === 2 && checkdnsrr($parts[1]);
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
        $data = readJSON('../config/mime.json');
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
        $data = readJSON('../config/mime.json');
        return array_key_exists($mime, $data) ? (is_array($data[$mime]) ? $data[$mime][0] : $data[$mime]) : null;
    }
?>