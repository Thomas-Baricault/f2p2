<?php
    namespace App;


    class WelcomeController extends \F2P2\Controller
    {
        public function index(
            \F2P2\Request $request
        ) : \F2P2\Response
        {
            return new \F2P2\PageResponse(\F2P2\View::render('welcome.php'));
        }
    }
?>