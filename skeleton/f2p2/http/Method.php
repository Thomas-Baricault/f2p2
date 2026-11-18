<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\http;


enum Method: string
{

    case Connect = 'CONNECT';

    case Delete = 'DELETE';

    case Get = 'GET';

    case Head = 'HEAD';

    case Options = 'OPTIONS';

    case Patch = 'PATCH';

    case Post = 'POST';

    case Put = 'PUT';

    case Query = 'QUERY';

    case Trace = 'TRACE';

}
