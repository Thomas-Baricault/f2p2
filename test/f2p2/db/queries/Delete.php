<?php

    namespace f2p2\db\queries;


    require_once('f2p2/db/Database.php');
    require_once('f2p2/db/queries/Query.php');


    /**
     * Delete query
     */
    class Delete extends \f2p2\db\queries\Query
    {

        /**
         * Where clause
         */
        private ?string $where = null;


        /**
         * Sets the where clause
         * 
         * @param string $value Where clause
         * 
         * @return Delete Reference to this query
         */
        public function where(string $value): Delete
        {
            $this->where = $value;
            return $this;
        }

        #[\Override]
        public function execute(array $values = []): array
        {
            $table = $this->table;
            $where = (
                $this->where
                    ? 'WHERE ' . $this->where
                   : ''
            );
            return (\f2p2\db\Database::execute(
                "DELETE FROM $table $where;",
                $values
            ));
        }

    }

?>
