<?php

    namespace f2p2\form\inputs;


    use Override;


    class Email extends \f2p2\form\inputs\Text
    {

        public const TYPE = 'email';


        #[Override]
        public function validate(): bool
        {
            if (!parent::validate())
                return (false);
            if (!isset($this->attrs['value']))
                return (true);
            return (filter_var($this->attrs['value'], FILTER_VALIDATE_EMAIL) !== false);
        }

    }

?>
