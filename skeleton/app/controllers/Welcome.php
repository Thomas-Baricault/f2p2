<?php

namespace app\controllers;


class Welcome extends \f2p2\Controller
{

    #[\f2p2\routing\Route('welcome', '/')]
    public function index(): \f2p2\http\responses\Response
    {
        return self::render([
            'main' => \f2p2\View::render('welcome'),
        ]);
    }

}


return Welcome::class;
