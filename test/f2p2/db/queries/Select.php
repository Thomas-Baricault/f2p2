<?php

    namespace f2p2\db\queries;


    require_once('f2p2/db/Database.php');
    require_once('f2p2/db/queries/Query.php');


    /**
     * Select query
     */
    class Select extends \f2p2\db\queries\Query
    {

        /**
         * Whether results have to be distinct
         */
        private bool $distinct = false;

        /**
         * Where clause
         */
        private ?string $where = null;

        /**
         * Order by clause
         */
        private ?array $orderBy = null;

        /**
         * Group by clause
         */
        private ?array $groupBy = null;

        /**
         * Limit clause
         */
        private ?int $limit = null;

        /**
         * Offset clause
         */
        private ?int $offset = null;

    
        /**
         * Sets whether results have to be distinct
         * 
         * @param bool $value `true` if distinct, `false` otherwise
         * 
         * @return Select Reference to this query
         */
        public function distinct(bool $value): Select
        {
            $this->distinct = $value;
            return ($this);
        }

        /**
         * Sets the where clause
         * 
         * @param string $value Where clause
         * 
         * @return Select Reference to this query
         */
        public function where(string $value): Select
        {
            $this->where = $value;
            return ($this);
        }

        /**
         * Sets the order by clause
         * 
         * @param  array $value Columns to order by
         * 
         * @return Select Reference to this query
         */
        public function orderBy(array $value): Select
        {
            $this->orderBy = $value;
            return ($this);
        }

        /**
         * Sets the group by clause
         * 
         * @param array $value Columns to group by
         * 
         * @return Select Reference to this query
         */
        public function groupBy(array $value): Select
        {
            $this->groupBy = $value;
            return ($this);
        }

        /**
         * Sets the limit clause
         * 
         * @param int $value Limit
         * 
         * @return Select Reference to this query
         */
        public function limit(int $value): Select
        {
            $this->limit = $value;
            return ($this);
        }

        /**
         * Sets the offset clause
         * 
         * @param int $value Offset
         * 
         * @return Select Reference to this query
         */
        public function offset(int $value): Select
        {
            $this->offset = $value;
            return ($this);
        }

        #[\Override]
        public function execute(array $values = []): array
        {
            $table = $this->table;
            $distinct = (
                $this->distinct
                    ? 'DISTINCT'
                    : ''
            );
            $where = (
                $this->where
                    ? 'WHERE ' . $this->where
                    : ''
            );
            $orderBy = '';
            if ($this->orderBy)
            {
                $orderBy = [];
                foreach ($this->orderBy as $column => $order)
                    $orderBy[] = "$column $order";
                $orderBy = 'ORDER BY ' . implode(', ', $orderBy);
            }
            $groupBy = (
                $this->groupBy
                    ? 'GROUP BY ' . implode(', ', $this->groupBy)
                    : ''
            );
            $limit = (
                $this->limit
                    ? 'LIMIT ' . $this->limit
                    : ''
            );
            $offset = (
                $this->offset
                    ? 'OFFSET ' . $this->offset
                    : ''
            );
            return (\f2p2\db\Database::execute(
                "SELECT $distinct * FROM $table $where $orderBy $groupBy $limit $offset;",
                $values
            ));
        }

    }

?>
