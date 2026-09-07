<?php

    namespace f2p2\db\queries;


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
        abstract public function execute(array $values = []): array;

    }

?>
