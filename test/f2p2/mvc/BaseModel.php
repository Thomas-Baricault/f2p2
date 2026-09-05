<?php

    namespace F2P2;


    /**
     * Abstract base class for models
     */
    abstract class BaseModel
    {

        /**
         * Table name
         */
        public const TABLE   = '';

        /**
         * Table colums
         */
        public const COLUMNS = [];

        /**
         * Entity id
         */
        protected ?int $id = null;

        /**
         * Entity data
         */
        protected array $data = [];


        /**
         * Find one entity by id
         * 
         * @param int $id Entity id
         * 
         * @return ?object Entity if exists, `null` otherwise
         */
        static public function findOne(int $id) : ?object
        {
            $data = (new \F2P2\QuerySelect(get_called_class()::TABLE))
                ->setWhere('id = :id')
                ->setValues(['id' => $id])
            ->execute();
            return (
                count($data) === 0
                    ? null
                    : new (get_called_class())($data[0])
            );
        }

        /**
         * Returns all entities in the table
         * 
         * @return array Entities
         */
        static public function findAll() : array
        {
            $data = (new \F2P2\QuerySelect(get_called_class()::TABLE))->execute();
            $result = [];
            foreach ($data as $entry)
                $result[] = new (get_called_class())($entry);
            return ($result);
        }

        /**
         * Constructs an entity
         * 
         * @param array $data Initials values of the entity (column => value)
         */
        public function __construct(array $data = [])
        {
            if (array_key_exists('id', $data))
                $this->id = $data['id'];
            foreach (get_class($this)::COLUMNS as $name => $params)
            {
                if (array_key_exists($name, $data))
                {
                    switch ($params['type'])
                    {
                        case 'SET':
                        {
                            $this->data[$name] = explode(',', $data[$name]);
                            break;
                        }
                        default:
                        {
                            $this->data[$name] = $data[$name];
                            break;
                        }
                    }                    
                }
                else if (array_key_exists('default', $params))
                    $this->data[$name] = $params['default'];
                else
                    $this->data[$name] = null;
            }
        }

        /**
         * Returns the entity id
         * 
         * @return ?int The id
         */
        public function getId() : ?int
        {
            return ($this->id);
        }

        /**
         * Returns wheter the entity is valid according to the table constraints
         * 
         * @return bool `true` if valid, `false` otherwise
         */
        public function isValid() : bool
        {
            foreach (get_class($this)::COLUMNS as $name => $params)
            {
                if (
                    array_key_exists('null', $params) && $params['null'] !== true && $this->data[$name] === null ||
                    array_key_exists('size', $params) && in_array($params['type'], ['CHAR', 'VARCHAR', 'BINARY', 'VARBINARY']) && strlen($this->data[$name]) > $params['size']
                )
                    return (false);
            }
            return (true);
        }

        /**
         * Push the entity to the database (update if already exists)
         * 
         * @return void
         * 
         * @throws \Exception If entity is invalid
         */
        public function push() : void
        {
            if (!$this->isValid())
                throw new \Exception("Cannot push invalid element");
            $values = [];
            foreach ($this->data as $column => $value)
                $values[$column] = get_class($this)::COLUMNS[$column]['type'] === 'SET' ? implode(',', $value) : $value;
            if ($this->id)
            {
                $update = [];
                foreach ($this->data as $column => $value)
                    $update[$column] = ":$column";
                $values['id'] = $this->id;
                (new \F2P2\QueryUpdate(get_class($this)::TABLE))
                    ->setUpdate($update)
                    ->setWhere('id = :id')
                    ->setValues($values)
                ->execute();
            }
            else
            {
                (new \F2P2\QueryInsert(get_class($this)::TABLE))
                    ->setValues($values)
                ->execute();
            }
        }

        /**
         * Remove the entity from the database (the entity must have a valid id)
         * 
         * @param bool $cascading Whether automatically remove entities that reference this entity
         * 
         * @return void
         * 
         * @throws \Exception If the entity is undefined
         */
        public function remove(bool $cascading = false) : void
        {
            if ($this->id === null)
                throw new \Exception("Cannot remove undefined element from the database");
            $class = get_class($this);
            if ($cascading)
            {
                foreach (get_declared_classes() as $model)
                {
                    if (is_subclass_of($model, BaseModel::class))
                    {
                        foreach ($model::COLUMNS as $name => $params)
                        {
                            if (array_key_exists('reference', $params) && $params['reference'] === $class)
                            {
                                $data = (new \F2P2\QuerySelect($model::TABLE))
                                    ->setWhere($name.' = :id')
                                    ->setValues(['id' => $this->id])
                                ->execute();
                                foreach ($data as $entry)
                                    (new $model($entry))->remove();
                            }
                        }
                    }
                }
            }
            (new \F2P2\QueryDelete($class::TABLE))
                ->setWhere('id = :id')
                ->setValues(['id' => $this->id])
            ->execute();
        }
    }

?>
