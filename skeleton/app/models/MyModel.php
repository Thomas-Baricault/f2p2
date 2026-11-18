<?php

namespace app\models;


#[\f2p2\models\Infos()]
class MyModel extends \f2p2\models\Model
{

    #[\f2p2\models\Field(\f2p2\orm\DataType::VarChar, [32])]
    public string $name;

}


return MyModel::class;
