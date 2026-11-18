<!DOCTYPE html>
<html lang="<?= \f2p2\Language::current() ?>">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/png" href="<?= \f2p2\View::img('logo.png') ?>">
    <link rel="stylesheet" href="<?= \f2p2\View::css('main') ?>">
    <title>F2P2 - Fantastic Framework & Practical PHP</title>
</head>
<body>
    <?= \f2p2\View::render('header', $params) ?>
    <?= isset($params['main']) ? $params['main'] : '' ?>
    <?= \f2p2\View::render('footer', $params) ?>
</body>
</html>
