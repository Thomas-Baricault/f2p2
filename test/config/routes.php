<?php

    return [
        "welcome" => (new \F2P2\Route())
            ->uri('/')
            ->method("GET")
            ->action('Welcome', 'index'),
    ];

?>
