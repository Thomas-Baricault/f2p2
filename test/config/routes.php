<?php

    return [
        "welcome" => (new \f2p2\routing\Route())
            ->uri('/')
            ->method("GET")
            ->action('Welcome', 'index'),
    ];

?>
