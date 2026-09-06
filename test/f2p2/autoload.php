<?php

    require_once('f2p2/db/Database.php');
    require_once('f2p2/db/Query.php');

    require_once('f2p2/mvc/Controller.php');
    require_once('f2p2/mvc/Model.php');
    require_once('f2p2/mvc/View.php');

    require_once('f2p2/requests/Request.php');
    require_once('f2p2/requests/Response.php');

    require_once('f2p2/routing/Route.php');
    require_once('f2p2/routing/Router.php');

    require_once('f2p2/utils/Email.php');
    require_once('f2p2/utils/Language.php');
    require_once('f2p2/utils/Server.php');

    function _loadModels(string $path) : void
    {
        if (is_dir($path))
        {
            foreach (scandir($path) as $entry)
                if ($entry !== '.' && $entry != '..')
                    _loadModels("$path/$entry");
        }
        else
            require_once($path);
    }

    _loadModels('app/models');

?>
