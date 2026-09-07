<?php

    namespace f2p2;


    /**
     * Abstract base class for models
     */
    abstract class Model
    {

        /**
         * Table name
         */
        public const TABLE = '';

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
         * @return ?Model Entity if exists, `null` otherwise
         */
        public static function findOne(int $id): ?Model
        {
            $data = (new \f2p2\db\queries\Select(get_called_class()::TABLE))
                ->where('id = :id')
                ->execute(['id' => $id]);
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
        public static function findAll(): array
        {
            $data = (new \f2p2\db\queries\Select(get_called_class()::TABLE))->execute();
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
        public function getId(): ?int
        {
            return ($this->id);
        }

        /**
         * Returns wheter the entity is valid according to the table constraints
         * 
         * @return bool `true` if valid, `false` otherwise
         */
        public function isValid(): bool
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
         * @throws \Exception If entity is invalid
         */
        public function push(): void
        {
            if (!$this->isValid())
                throw new \Exception("Cannot push invalid element");
            $values = [];
            foreach ($this->data as $column => $value)
            {
                $values[$column] = (
                    get_class($this)::COLUMNS[$column]['type'] === 'SET'
                        ? implode(',', $value)
                        : $value
                );
            }
            if ($this->id)
            {
                $update = [];
                foreach ($this->data as $column => $value)
                    $update[$column] = ":$column";
                $values['id'] = $this->id;
                (new \f2p2\db\queries\Update(get_class($this)::TABLE))
                    ->update($update)
                    ->where('id = :id')
                    ->execute($values);
            }
            else
            {
                (new \f2p2\db\queries\Insert(get_class($this)::TABLE))
                    ->execute($values);
            }
        }

        /**
         * Remove the entity from the database (the entity must have a valid id)
         * 
         * @param bool $cascading Whether automatically remove entities that reference this entity
         * 
         * @throws \Exception If the entity is undefined
         */
        public function remove(bool $cascading = false): void
        {
            if ($this->id === null)
                throw new \Exception("Cannot remove undefined element from the database");
            $class = get_class($this);
            if ($cascading)
            {
                foreach (get_declared_classes() as $model)
                {
                    if (is_subclass_of($model, Model::class))
                    {
                        foreach ($model::COLUMNS as $name => $params)
                        {
                            if (array_key_exists('reference', $params) && $params['reference'] === $class)
                            {
                                $data = (new \f2p2\db\queries\Select($model::TABLE))
                                    ->where($name.' = :id')
                                    ->execute(['id' => $this->id]);
                                foreach ($data as $entry)
                                    (new $model($entry))->remove();
                            }
                        }
                    }
                }
            }
            (new \f2p2\db\queries\Delete($class::TABLE))
                ->where('id = :id')
                ->execute(['id' => $this->id]);
        }
    }

?>
