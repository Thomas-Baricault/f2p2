<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm;


enum DatabaseType: string
{

    case MySQL = 'mysql';

    case OCI = 'oci';

    case SQLite = 'sqlite';

}
