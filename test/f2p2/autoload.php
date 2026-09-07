<?php

    require_once('f2p2/db/queries/Delete.php');
    require_once('f2p2/db/queries/Insert.php');
    require_once('f2p2/db/queries/Query.php');
    require_once('f2p2/db/queries/Select.php');
    require_once('f2p2/db/queries/Update.php');
    require_once('f2p2/db/Database.php');

    require_once('f2p2/form/inputs/Input.php');
    require_once('f2p2/form/inputs/Text.php');
    require_once('f2p2/form/inputs/Color.php');
    require_once('f2p2/form/inputs/Date.php');
    require_once('f2p2/form/inputs/DatetimeLocal.php');
    require_once('f2p2/form/inputs/Email.php');
    require_once('f2p2/form/inputs/File.php');
    require_once('f2p2/form/inputs/Hidden.php');
    require_once('f2p2/form/inputs/Month.php');
    require_once('f2p2/form/inputs/Number.php');
    require_once('f2p2/form/inputs/Password.php');
    require_once('f2p2/form/inputs/Range.php');
    require_once('f2p2/form/inputs/Search.php');
    require_once('f2p2/form/inputs/Submit.php');
    require_once('f2p2/form/inputs/Tel.php');
    require_once('f2p2/form/inputs/Time.php');
    require_once('f2p2/form/inputs/URL.php');
    require_once('f2p2/form/inputs/Week.php');
    require_once('f2p2/form/Field.php');
    require_once('f2p2/form/FieldContainer.php');
    require_once('f2p2/form/Fieldset.php');
    require_once('f2p2/form/Form.php');
    require_once('f2p2/form/FormComponent.php');
    require_once('f2p2/form/Label.php');
    require_once('f2p2/form/Legend.php');
    require_once('f2p2/form/Option.php');
    require_once('f2p2/form/Select.php');

    require_once('f2p2/mvc/Controller.php');
    require_once('f2p2/mvc/Model.php');
    require_once('f2p2/mvc/View.php');

    require_once('f2p2/requests/File.php');
    require_once('f2p2/requests/Request.php');

    require_once('f2p2/responses/Data.php');
    require_once('f2p2/responses/Download.php');
    require_once('f2p2/responses/Error.php');
    require_once('f2p2/responses/Page.php');
    require_once('f2p2/responses/RedirectRoute.php');
    require_once('f2p2/responses/RedirectURL.php');
    require_once('f2p2/responses/Response.php');

    require_once('f2p2/routing/Route.php');
    require_once('f2p2/routing/Router.php');

    require_once('f2p2/utils/Component.php');
    require_once('f2p2/utils/Email.php');
    require_once('f2p2/utils/Language.php');
    require_once('f2p2/utils/regex.php');
    require_once('f2p2/utils/Server.php');

    function _loadModels(string $path): void
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
