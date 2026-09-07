<?php

    namespace f2p2\db\queries;


    use Override;


    /**
     * Insert query
     */
    class Insert extends \f2p2\db\queries\Query
    {

        #[Override]
        public function execute(array $values = []): array
        {
            $table = $this->table;
            $columns = [];
            $placeholders = [];
            foreach ($values as $column => $_)
            {
                $columns[] = $column;
                $placeholders[] = ":$column";
            }
            $columns = implode(', ', $columns);
            $placeholders = implode(', ', $placeholders);
            return (\f2p2\db\Database::execute(
                "INSERT INTO $table ($columns) VALUES ($placeholders);",
                $values
            ));
        }

    }

?>
