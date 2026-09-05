<?php 
    namespace F2P2;


    /**
     * A class to interact with database
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Database
    {
        static private ?\PDO $pdo = null;


        /**
         * Execute a query on the database
         * 
         * @param  string $query  The query
         * @param  array  $values The values for the prepare
         * @return array          The result of the query
         */
        static public function execute(
            string $query,
            array  $values
        ) : array
        {
            if (self::$pdo === null)
            {
                try
                {
                    switch (DB_TYPE)
                    {
                        case 'sqlite': self::$pdo = new \PDO(DB_TYPE.':'.DB_NAME);                                           break;
                        case 'oci':    self::$pdo = new \PDO(DB_TYPE.'dbname='.DB_NAME, DB_USER, DB_PASS);                   break;
                        default:       self::$pdo = new \PDO(DB_TYPE.':host='.DB_HOST.';dbname='.DB_NAME, DB_USER, DB_PASS); break;
                    }
                }
                catch (\Throwable $th)
                {
                    die($th->getMessage());
                }
            }
            $stmt = self::$pdo->prepare($query);
            $stmt->execute($values);
            return $stmt->fetchAll(\PDO::FETCH_ASSOC);
        }
    }
?>