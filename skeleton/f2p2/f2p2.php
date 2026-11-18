<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


require_once('f2p2/cache/Cache.php');
require_once('f2p2/cache/Infos.php');

require_once('f2p2/components/Component.php');

require_once('f2p2/http/requests/File.php');
require_once('f2p2/http/requests/Request.php');
require_once('f2p2/http/responses/Data.php');
require_once('f2p2/http/responses/Download.php');
require_once('f2p2/http/responses/Error.php');
require_once('f2p2/http/responses/JSON.php');
require_once('f2p2/http/responses/Page.php');
require_once('f2p2/http/responses/RedirectRoute.php');
require_once('f2p2/http/responses/RedirectURL.php');
require_once('f2p2/http/responses/Response.php');
require_once('f2p2/http/Message.php');
require_once('f2p2/http/Method.php');

require_once('f2p2/orm/queries/Delete.php');
require_once('f2p2/orm/queries/DMLQuery.php');
require_once('f2p2/orm/queries/DQLQuery.php');
require_once('f2p2/orm/queries/Insert.php');
require_once('f2p2/orm/queries/Query.php');
require_once('f2p2/orm/queries/Select.php');
require_once('f2p2/orm/queries/Update.php');
require_once('f2p2/orm/traits/GroupBy.php');
require_once('f2p2/orm/traits/Limit.php');
require_once('f2p2/orm/traits/Offset.php');
require_once('f2p2/orm/traits/OrderBy.php');
require_once('f2p2/orm/traits/Where.php');
require_once('f2p2/orm/Connection.php');
require_once('f2p2/orm/Constant.php');
require_once('f2p2/orm/DatabaseType.php');
require_once('f2p2/orm/DataType.php');
require_once('f2p2/orm/Transaction.php');

require_once('f2p2/form/inputs/Checkbox.php');
require_once('f2p2/form/inputs/Color.php');
require_once('f2p2/form/inputs/Date.php');
require_once('f2p2/form/inputs/DatetimeLocal.php');
require_once('f2p2/form/inputs/Email.php');
require_once('f2p2/form/inputs/File.php');
require_once('f2p2/form/inputs/Hidden.php');
require_once('f2p2/form/inputs/Input.php');
require_once('f2p2/form/inputs/Month.php');
require_once('f2p2/form/inputs/Number.php');
require_once('f2p2/form/inputs/Password.php');
require_once('f2p2/form/inputs/Radio.php');
require_once('f2p2/form/inputs/Range.php');
require_once('f2p2/form/inputs/Search.php');
require_once('f2p2/form/inputs/Submit.php');
require_once('f2p2/form/inputs/Tel.php');
require_once('f2p2/form/inputs/Text.php');
require_once('f2p2/form/inputs/Time.php');
require_once('f2p2/form/inputs/URL.php');
require_once('f2p2/form/inputs/Week.php');
require_once('f2p2/form/Field.php');
require_once('f2p2/form/Form.php');
require_once('f2p2/form/Select.php');
require_once('f2p2/form/TextArea.php');

require_once('f2p2/models/Cache.php');
require_once('f2p2/models/Field.php');
require_once('f2p2/models/Infos.php');
require_once('f2p2/models/Model.php');
require_once('f2p2/models/Reference.php');

require_once('f2p2/routing/Cache.php');
require_once('f2p2/routing/Route.php');
require_once('f2p2/routing/Router.php');

require_once('f2p2/utils/Controller.php');
require_once('f2p2/utils/Email.php');
require_once('f2p2/utils/Language.php');
require_once('f2p2/http/Method.php');
require_once('f2p2/utils/regex.php');
require_once('f2p2/utils/Server.php');
require_once('f2p2/utils/View.php');
