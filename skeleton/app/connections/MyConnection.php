<?php

namespace app\connections;


if (PRODUCTION)
{
    #[\f2p2\orm\Connection(
        \f2p2\orm\DatabaseType::MySQL,
        name: 'database',
        host: 'myhost.com',
        user: 'admin',
        pass: 'password',
    )]
    class MyConnection
    {}
}
else
{
    #[\f2p2\orm\Connection(
        \f2p2\orm\DatabaseType::MySQL,
        name: 'database',
        host: 'localhost',
        user: 'root',
        pass: '',
    )]
    class MyConnection
    {}
}
