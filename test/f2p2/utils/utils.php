<?php

    namespace f2p2\utils;


    function validateDate(string $value, string $format)
    {
        $date = \DateTimeImmutable::createFromFormat('!' . $format, $value);
        return ($date !== false && $date->format($format) === $value);
    }

?>
