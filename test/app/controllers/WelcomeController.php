<?php
    namespace App\controllers;


    class WelcomeController extends \F2P2\BaseController
    {
        public function index(
            \F2P2\Request $request
        ) : \F2P2\Response
        {
            return new \F2P2\ResponsePage(\F2P2\View::render('welcome.php'));
        }
    }
?>