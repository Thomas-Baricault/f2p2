<?php
    namespace F2P2;


    /**
     * The base class for the database query representations
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class Query
    {
        protected string $table;

        protected array $values = [];


        /**
         * @param string $table The table on which the query will be execute
         */
        public function __construct(
            string $table
        ) {
            $this->table = $table;
        }

        /**
         * Defines the prepare values
         * 
         * @param  array $values The values
         * @return Query The query
         */
        public function setValues(
            array $values
        ) : Query
        {
            $this->values = $values;
            return $this;
        }

        /**
         * Execute the query
         * 
         * @return array The result of the query
         */
        abstract public function execute() : array;
    }

    /**
     * The class that represent an insert query
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class QueryInsert extends Query
    {
        public function execute() : array
        {
            $table = $this->table;
            $columns = [];
            $placeholders = [];
            foreach ($this->values as $column => $value)
            {
                $columns[] = $column;
                $placeholders[] = ":$column";
            }
            $columns = implode(', ', $columns);
            $placeholders = implode(', ', $placeholders);
            return \F2P2\Database::execute(
                "INSERT INTO $table ($columns) VALUES ($placeholders);",
                $this->values
            );
        }
    }


    /**
     * The class that represent a select query
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class QuerySelect extends Query
    {
        private ?string $where   = null;
        private ?array  $orderBy = null;
        private ?array  $groupBy = null;
        private ?int    $limit   = null;
        private ?int    $offset  = null;


        public function execute() : array
        {
            $table = $this->table;
            $where = $this->where ? 'WHERE '.$this->where : '';
            $orderBy = '';
            if ($this->orderBy)
            {
                $orderBy = [];
                foreach ($this->orderBy as $column => $order)
                {
                    $orderBy[] = "$column $order";
                }
                $orderBy = 'ORDER BY '.implode(', ', $orderBy);
            }
            $groupBy = $this->groupBy ? 'GROUP BY '.implode(', ', $this->groupBy) : '';
            $limit = $this->limit ? 'LIMIT '.$this->limit : '';
            $offset = $this->offset ? 'OFFSET '.$this->offset : '';
            return \F2P2\Database::execute(
                "SELECT * FROM $table $where $orderBy $groupBy $limit $offset;",
                $this->values
            );
        }

        /**
         * Defines the where clause
         * 
         * @param  string      $where The where clause
         * @return QuerySelect        The query
         */
        public function setWhere(
            string $where
        ) : QuerySelect
        {
            $this->where = $where;
            return $this;
        }

        /**
         * Defines the order by clause
         * 
         * @param  array       $orderBy An array of columns
         * @return QuerySelect          The query
         */
        public function setOrderBy(
            array $orderBy
        ) : QuerySelect
        {
            $this->orderBy = $orderBy;
            return $this;
        }

        /**
         * Defines the group by clause
         * 
         * @param  array       $groupBy An array of columns
         * @return QuerySelect          The query
         */
        public function setGroupBy(
            array $groupBy
        ) : QuerySelect
        {
            $this->groupBy = $groupBy;
            return $this;
        }

        /**
         * Defines the limit clause
         * 
         * @param  int         $limit The limit
         * @return QuerySelect        The query
         */
        public function setLimit(
            int $limit
        ) : QuerySelect
        {
            $this->limit = $limit;
            return $this;
        }

        /**
         * Defines the offset clause
         * 
         * @param  int         $offset The offset
         * @return QuerySelect         The query
         */
        public function setOffset(
            int $offset
        ) : QuerySelect
        {
            $this->offset = $offset;
            return $this;
        }
    }


    /**
     * The class that represent an update query
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class QueryUpdate extends Query
    {
        private ?array  $update = null;
        private ?string $where  = null;


        public function execute() : array
        {
            $table = $this->table;
            $update = '';
            if ($this->update)
            {
                $update = [];
                foreach ($this->update as $column => $value)
                {
                    $update[] = "$column = $value";
                }
                $update = 'SET '.implode(', ', $update);
            }
            $where = $this->where ? 'WHERE '.$this->where : '';
            return \F2P2\Database::execute(
                "UPDATE $table $update $where;",
                $this->values
            );
        }

        /**
         * Defines the offset clause
         * 
         * @param  array       $update An array of modifications (column => value)
         * @return QueryUpdate         The query
         */
        public function setUpdate(
            array $update
        ) : QueryUpdate
        {
            $this->update = $update;
            return $this;
        }

        /**
         * Defines the where clause
         * 
         * @param  string      $where The where clause
         * @return QueryUpdate        The query
         */
        public function setWhere(
            string $where
        ) : QueryUpdate
        {
            $this->where = $where;
            return $this;
        }
    }


    /**
     * The class that represent a delete query
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class QueryDelete extends Query
    {
        private ?string $where = null;


        public function execute() : array
        {
            $table = $this->table;
            $where = $this->where ? 'WHERE '.$this->where : '';
            return \F2P2\Database::execute(
                "DELETE FROM $table $where;",
                $this->values
            );
        }

        /**
         * Defines the where clause
         * 
         * @param  string      $where The where clause
         * @return QueryDelete        The query
         */
        public function setWhere(
            string $where
        ) : QueryDelete
        {
            $this->where = $where;
            return $this;
        }
    }
?>