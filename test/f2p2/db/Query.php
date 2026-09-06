<?php

    namespace F2P2;


    /**
     * Abstract base class for database query representations
     */
    abstract class Query
    {

        /**
         * Table to affect
         */
        protected string $table;


        /**
         * Constructs a query
         * 
         * @param string $table Table on which the query will be execute
         */
        public function __construct(string $table)
        {
            $this->table = $table;
        }

        /**
         * Executes the query
         * 
         * @param array $values Prepare values
         * 
         * @return array Query result
         */
        abstract public function execute(array $values = []) : array;

    }

    /**
     * Insert query
     */
    class InsertQuery extends Query
    {

        public function execute(array $values = []) : array
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
            return (\F2P2\Database::execute(
                "INSERT INTO $table ($columns) VALUES ($placeholders);",
                $values
            ));
        }

    }


    /**
     * Select query
     */
    class SelectQuery extends Query
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
         * @param bool $distinct `true` if distinct, `false` otherwise
         * 
         * @return SelectQuery Reference to this query
         */

        /**
         * Sets the where clause
         * 
         * @param string $where Where clause
         * 
         * @return SelectQuery Reference to this query
         */
        public function where(string $where) : SelectQuery
        {
            $this->where = $where;
            return ($this);
        }

        /**
         * Sets the order by clause
         * 
         * @param  array $orderBy Columns to order by
         * 
         * @return SelectQuery Reference to this query
         */
        public function orderBy(array $orderBy) : SelectQuery
        {
            $this->orderBy = $orderBy;
            return ($this);
        }

        /**
         * Sets the group by clause
         * 
         * @param array $groupBy Columns to group by
         * 
         * @return SelectQuery Reference to this query
         */
        public function groupBy(array $groupBy) : SelectQuery
        {
            $this->groupBy = $groupBy;
            return ($this);
        }

        /**
         * Sets the limit clause
         * 
         * @param int $limit Limit
         * 
         * @return SelectQuery Reference to this query
         */
        public function limit(int $limit) : SelectQuery
        {
            $this->limit = $limit;
            return ($this);
        }

        /**
         * Sets the offset clause
         * 
         * @param int $offset Offset
         * 
         * @return SelectQuery Reference to this query
         */
        public function offset(int $offset) : SelectQuery
        {
            $this->offset = $offset;
            return ($this);
        }

        public function execute(array $values = []) : array
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
            return (\F2P2\Database::execute(
                "SELECT $distinct * FROM $table $where $orderBy $groupBy $limit $offset;",
                $values
            ));
        }

    }


    /**
     * Update query
     */
    class UpdateQuery extends Query
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
         * @param array $update Array of modifications (column => value)
         * 
         * @return UpdateQuery Reference to this query
         */
        public function update(array $update) : UpdateQuery
        {
            $this->update = $update;
            return ($this);
        }

        /**
         * Sets the where clause
         * 
         * @param string $where Where clause
         * 
         * @return UpdateQuery Reference to this query
         */
        public function where(string $where) : UpdateQuery
        {
            $this->where = $where;
            return ($this);
        }

        public function execute(array $values = []) : array
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
            return (\F2P2\Database::execute(
                "UPDATE $table $update $where;",
                $values
            ));
        }

    }


    /**
     * Delete query
     */
    class DeleteQuery extends Query
    {

        /**
         * Where clause
         */
        private ?string $where = null;


        /**
         * Defines the where clause
         * 
         * @param  string      $where The where clause
         * @return DeleteQuery        The query
         */
        public function where(string $where) : DeleteQuery
        {
            $this->where = $where;
            return $this;
        }

        public function execute(array $values = []) : array
        {
            $table = $this->table;
            $where = (
                $this->where
                    ? 'WHERE ' . $this->where
                    : ''
            );
            return (\F2P2\Database::execute(
                "DELETE FROM $table $where;",
                $values
            ));
        }

    }

?>
