<?php
    namespace App;


    class WelcomeController extends \f2p2\Controller
    {
        public function index(
            \f2p2\requests\Request $request
        ): \f2p2\responses\Response
        {
            return new \f2p2\responses\Page(\f2p2\View::render('welcome.php'));
        }
    }
?>