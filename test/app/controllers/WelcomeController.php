<?php
    namespace App;


    class WelcomeController extends \f2p2\Controller
    {
        public function index(
            \f2p2\Request $request
        ): \f2p2\Response
        {
            return new \f2p2\PageResponse(\f2p2\View::render('welcome.php'));
        }
    }
?>