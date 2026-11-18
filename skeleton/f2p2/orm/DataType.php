<?php

/*
 * Copyright (c) 2026-present Thomas Baricault
 *
 * SPDX-License-Identifier: MIT
 */


namespace f2p2\orm;


enum DataType: string
{

    case Binary = 'BINARY';

    case BigInt = 'BIGINT';

    case Bit = 'BIT';

    case Blob = 'BLOB';

    case Bool = 'BOOL';

    case Char = 'CHAR';

    case Date = 'DATE';

    case DateTime = 'DATETIME';

    case Decimal = 'DECIMAL';

    case Double = 'DOUBLE';

    case Enum = 'Enum';

    case Float = 'FLOAT';

    case Int = 'INT';

    case LongBlob = 'LONGBLOB';

    case LongText = 'LONGTEXT';

    case MediumBlob = 'MEDIUMBLOB';

    case MediumInt = 'MEDIUMINT';

    case MediumText = 'MEDIUMTEXT';

    case Set = 'SET';

    case SmallInt = 'SMALLINT';

    case Text = 'TEXT';

    case Time = 'TIME';

    case Timestamp = 'TIMESTAMP';

    case TinyBlob = 'TINYBLOB';

    case TinyInt = 'TINYINT';

    case TinyText = 'TINYTEXT';

    case VarBinary = 'VARBINARY';

    case VarChar = 'VARCHAR';

    case Year = 'YEAR';

}
