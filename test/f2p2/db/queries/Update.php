<?php

    namespace f2p2\db\queries;


    require_once('f2p2/db/Database.php');
    require_once('f2p2/db/queries/Query.php');


    /**
     * Update query
     */
    class Update extends \f2p2\db\queries\Query
    {

        /**
         * New modifications
         */
        private ?array $update = null;

        /**
         * Where clause
         */
        private ?string $where  = null;


        /**
         * Sets update values
         * 
         * @param array $value Array of modifications (column => value)
         * 
         * @return Update Reference to this query
         */
        public function update(array $value): Update
        {
            $this->update = $value;
            return ($this);
        }

        /**
         * Sets the where clause
         * 
         * @param string $value Where clause
         * 
         * @return Update Reference to this query
         */
        public function where(string $value): Update
        {
            $this->where = $value;
            return ($this);
        }

        #[\Override]
        public function execute(array $values = []): array
        {
            $table = $this->table;
            $update = '';
            if ($this->update)
            {
                $update = [];
                foreach ($this->update as $column => $value)
                    $update[] = "$column = $value";
                $update = 'SET ' . implode(', ', $update);
            }
            $where = (
                $this->where
                    ? 'WHERE ' . $this->where
                    : ''
            );
            return (\f2p2\db\Database::execute(
                "UPDATE $table $update $where;",
                $values
            ));
        }

    }

?>
