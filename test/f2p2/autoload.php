<?php
    function loadDir(
        string $path,
        array  $ignore = []
    ) : void
    {
        if (is_dir($path))
        {
            foreach (scandir($path) as $file)
            {
                if ($file !== '.' && $file !== '..' && !in_array($file, $ignore))
                {
                    loadDir("$path/$file", $ignore);
                }
            }
        }
        else
        {
            require_once($path);
        }
    }


    loadDir('../f2p2', ['autoload.php']);
    loadDir('../app/models');
?>