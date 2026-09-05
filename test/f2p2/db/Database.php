<?php

    namespace F2P2;


    /**
     * Singleton database interface
     */
    class Database
    {

        /**
         * Database handler
         */
        static private ?\PDO $pdo = null;


        /**
         * Executes a query on the database
         * 
         * @param string $query Query to execute
         * @param array $values Values for prepare
         * 
         * @return array Query result
         */
        static public function execute(string $query, array $values) : array
        {
            if (self::$pdo === null)
            {
                try
                {
                    switch (DB_TYPE)
                    {
                        case 'sqlite':
                        {
                            self::$pdo = new \PDO(DB_TYPE.':'.DB_NAME);
                            break;
                        }
                        case 'oci':
                        {
                            self::$pdo = new \PDO(DB_TYPE.'dbname='.DB_NAME, DB_USER, DB_PASS);
                            break;
                        }
                        default:
                        {
                            self::$pdo = new \PDO(DB_TYPE.':host='.DB_HOST.';dbname='.DB_NAME, DB_USER, DB_PASS);
                            break;
                        }
                    }
                }
                catch (\Throwable $th)
                {
                    die($th->getMessage());
                }
            }
            $stmt = self::$pdo->prepare($query);
            $stmt->execute($values);
            return ($stmt->fetchAll(\PDO::FETCH_ASSOC));
        }
    }

?>
