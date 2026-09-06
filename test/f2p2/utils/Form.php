<?php
    namespace F2P2;


    /**
     * The base class for all form objects
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class FormObject
    {
        protected array $attrs    = [];


        /**
         * Add an attribute to the object
         * 
         * @param  string $name  The attribute name
         * @param  string $value The attribute value
         * @return object        The form object
         */
        public function addAttribute(
            string $name,
            string $value
        ) : object
        {
            $this->attrs[$name] = $value;
            return $this;
        }

        /**
         * Render the object
         * 
         * @return string The HTML generated
         */
        abstract public function render() : string;

        /**
         * Render the object attributes
         * 
         * @return string The HTML element attributes list generated
         */
        protected function renderAttributes() : string
        {
            $result = [];
            foreach ($this->attrs as $name => $value)
            {
                $result[] = "$name=\"$value\"";
            }
            return implode(' ', $result);
        }
    }


    /**
     * The base class for form fields
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class Field extends FormObject
    {
        protected ?string $name     = null;
        protected ?string $id       = null;
        protected string  $type;
        protected ?string $label    = null;
        protected bool    $required = false;
        protected ?string $value    = null;


        /**
         * @param string  $name The field name
         * @param ?string $id   The field id
         */
        public function __construct(
            string  $name,
            ?string $id = null
        ) {
            $this->name = $name;
            $this->id = $id ?? $name;
        }

        /**
         * Check if the field value is valid
         * 
         * @return bool true if valid, otherwise false
         */
        public function isValid() : bool
        {
            return !$this->required || $this->value;
        }

        /**
         * Returns the field name
         * 
         * @return ?string The field name
         */
        public function getName() : ?string
        {
            return $this->name;
        }

        /**
         * Returns the field value
         * 
         * @return ?string The field value
         */
        public function getValue() : string
        {
            return $this->value;
        }

        /**
         * Defines the field value
         * 
         * @param  ?string $value The value
         * @return object         The field
         */
        public function setValue(
            ?string $value
        ) : object
        {
            $this->value = $value === '' ? null : $value;
            return $this;
        }

        /**
         * Defines the field label
         * 
         * @param  string $label The label
         * @return object        The field
         */
        public function setLabel(
            string $label
        ) : object
        {
            $this->label = $label;
            return $this;
        }

        /**
         * Make this field required
         * 
         * @return object The field
         */
        public function setRequired() : object
        {
            $this->required = true;
            return $this;
        }

        public function render() : string
        {
            return $this->renderLabel().'<input '.$this->renderAttributes().(' type="'.$this->type.'"').' value="'.$this->value.'">';
        }

        /**
         * Render the field label
         * 
         * @return string The HTML generated
         */
        protected function renderLabel() : string
        {
            return ($this->label ? '<label'.($this->id ? 'for="'.$this->id.'"' : '').'>'.$this->label.'</label>' : '');
        }
        
        protected function renderAttributes() : string
        {
            return parent::renderAttributes().($this->id ? ' id="'.$this->id.'"' : '').($this->name ? ' name="'.$this->name.'"' : '').($this->required ? ' required' : '');
        }
    }


    /**
     * The base class for fields that need a value length verification
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class FieldTextVerifyLength extends Field
    {
        protected ?int    $minLength = null;
        protected ?int    $maxLength = null;


        public function isValid() : bool
        {
            return parent::isValid() && ($this->minLength === null || strlen($this->value) >= $this->minLength) && ($this->maxLength === null || strlen($this->value) <= $this->maxLength);
        }

        /**
         * Defines the field value minimum length
         * 
         * @param  int    $minLength The length
         * @return object            The field
         */
        public function setMinLength(
            int $minLength
        ) : object
        {
            $this->minLength = $minLength;
            return $this;
        }

        /**
         * Defines the field value maximum length
         * 
         * @param  int    $maxLength The length
         * @return object            The field
         */
        public function setMaxLength(
            int $maxLength
        ) : object
        {
            $this->maxLength = $maxLength;
            return $this;
        }

        protected function renderAttributes() : string
        {
            return parent::renderAttributes().($this->minLength ? ' minlength="'.$this->minLength.'"' : '').($this->maxLength ? ' maxlength="'.$this->maxLength.'"' : '');
        }
    }


    /**
     * The base class for fields that need a value pattern verification
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class FieldTextVerifyPattern extends FieldTextVerifyLength
    {
        protected ?string $pattern   = null;


        public function isValid() : bool
        {
            return parent::isValid() && ($this->pattern === null || preg_match('/'.$this->pattern.'/', $this->value));
        }

        protected function renderAttributes() : string
        {
            return parent::renderAttributes().($this->pattern ? ' pattern="'.$this->pattern.'"' : '');
        }
    }


    /**
     * The class that represent a input of type text
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldText extends FieldTextVerifyPattern
    {
        protected string  $type = 'text';
    }


    /**
     * The class that represent a input of type email
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldEmail extends FieldTextVerifyPattern
    {
        protected string $type = 'email';
    }


    /**
     * The class that represent a input of type password
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldPassword extends FieldTextVerifyPattern
    {
        protected string $type = 'password';
    }


    /**
     * The class that represent a input of type search
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldSearch extends FieldTextVerifyPattern
    {
        protected string $type = 'search';
    }


    /**
     * The class that represent a input of type tel
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldTel extends FieldTextVerifyPattern
    {
        protected string $type = 'tel';
    }


    /**
     * The class that represent a input of type url
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldURL extends FieldTextVerifyPattern
    {
        protected string $type = 'url';
    }


    /**
     * The class that represent a textarea
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldTextArea extends FieldTextVerifyPattern
    {
        public function render() : string
        {
            return $this->renderLabel().'<textarea '.$this->renderAttributes().'>'.$this->value.'</textarea>';
        }
    }


    /**
     * The class that represent a input of type number
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldNumber extends Field
    {
        protected string $type = 'number';
        protected ?int   $min  = null;
        protected ?int   $max  = null;
        protected ?int   $step = null;


        public function isValid() : bool
        {
            return parent::isValid() && ($this->min === null || $this->value >= $this->min) && ($this->max === null || $this->value <= $this->max) && ($this->step === null || ($this->min && ($this->value-$this->min)%$this->step === 0 || $this->min === null && ($this->value)%$this->step === 0));
        }

        /**
         * Defines the field minimum value
         * 
         * @param  int    $min The value
         * @return object      The field
         */
        public function setMin(
            int $min
        ) : object
        {
            $this->min = $min;
            return $this;
        }

        /**
         * Defines the field maximum value
         * 
         * @param  int    $max The value
         * @return object      The field
         */
        public function setMax(
            int $max
        ) : object
        {
            $this->max = $max;
            return $this;
        }

        /**
         * Defines the field value step
         * 
         * @param  int    $step The step
         * @return object       The field
         */
        public function setStep(
            int $step
        ) : object
        {
            if ($step <= 0)
            {
                throw new \Exception('Step must be greater than 0');
            }
            $this->step = $step;
            return $this;
        }

        protected function renderAttributes() : string
        {
            return parent::renderAttributes().($this->min ? ' min="'.$this->min.'"' : '').($this->max ? ' max="'.$this->max.'"' : '').($this->step ? ' step="'.$this->step.'"' : '');
        }
    }


    /**
     * The class that represent a input of type range
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldRange extends FieldNumber
    {
        protected string $type     = 'range';
        protected bool   $required = true;
    }


    /**
     * The class that represent a input of type color
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldColor extends Field
    {
        protected string $type     = 'color';
        protected bool   $required = true;


        public function isValid() : bool
        {
            return parent::isValid() && preg_match('/#[0-9A-Fa-f]{6}/', $this->value);
        }
    }


    /**
     * The class that represent a input of type hidden
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldHidden extends Field
    {
        protected string $type     = 'hidden';
        protected bool   $required = true;
    }


    /**
     * The class that represent a input of type checkbox
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldCheckbox extends Field
    {
        protected string $type    = 'checkbox';
        protected bool   $checked = false;


        public function isValid() : bool
        {
            return !$this->required || $this->checked;
        }

        /**
         * Check the input
         * 
         * @return object The field
         */
        public function check() : object
        {
            $this->checked = true;
            return $this;
        }

        protected function renderAttributes() : string
        {
            return parent::renderAttributes().($this->checked ? ' checked' : '');
        }
    }


    /**
     * The class that represent a input of type radio
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldRadio extends FieldCheckbox
    {
        protected string $type = 'radio';
    }


    /**
     * The class that represent a select element
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldSelect extends Field
    {
        protected array $options = [];


        /**
         * Add an option to the selection
         * 
         * @param  string $value    The option value
         * @param  string $text     The text rendered for this option
         * @param  bool   $selected If this option is the selected one by default
         * @return object           The field
         */
        public function addOption(
            string $value,
            string $text,
            bool   $selected = false
        ) : object
        {
            $this->options[] = [
                'value'    => $value,
                'text'     => $text,
                'selected' => $selected
            ];
            return $this;
        }

        public function render() : string
        {
            $html = $this->renderLabel().'<select '.$this->renderAttributes().'>';
            foreach ($this->options as $option)
            {
                $html .= '<option value="'.$option['value'].'"'.($option['selected'] ? ' selected' : '').'>'.$option['text'].'</option>';
            }
            return $html.'</select>';
        }
    }


    /**
     * The class that represent a input of type date
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldDate extends Field
    {
        protected string  $type    = 'date';
        protected string  $pattern = \F2P2\REGEX_DATE;
        protected ?string $min     = null;
        protected ?string $max     = null;


        public function isValid() : bool
        {
            $value = $this->value ? new \Datetime($this->value) : null;
            $min = $this->min ? new \Datetime($this->min) : null;
            $max = $this->max ? new \Datetime($this->max) : null;
            return parent::isValid() && preg_match($this->pattern, $this->value) && ($min === null || $value >= $min) && ($max === null || $value <= $max);
        }

        /**
         * Defines the field minimum date
         * 
         * @param  int    $max The value
         * @return object      The field
         */
        public function setMin(
            string $min
        ) : object
        {
            if (!preg_match($this->pattern, $min))
            {
                throw new \Exception('Min value doesn\'t match the date type pattern');
            }
            $this->min = $min;
            return $this;
        }

        /**
         * Defines the field maximum date
         * 
         * @param  int    $max The value
         * @return object      The field
         */
        public function setMax(
            string $max
        ) : object
        {
            if (!preg_match($this->pattern, $max))
            {
                throw new \Exception('Max value doesn\'t match the date type pattern');
            }
            $this->max = $max;
            return $this;
        }

        protected function renderAttributes() : string
        {
            return parent::renderAttributes().($this->min ? ' min="'.$this->min.'"' : '').($this->max ? ' max="'.$this->max.'"' : '');
        }
    }


    /**
     * The class that represent a input of type datetime-local
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldDatetimeLocal extends FieldDate
    {
        protected string $type    = 'datetime-local';
        protected string $pattern = \F2P2\REGEX_DATETIMELOCAL;
    }


    /**
     * The class that represent a input of type time
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldTime extends FieldDate
    {
        protected string $type    = 'time';
        protected string $pattern = \F2P2\REGEX_TIME;
    }


    /**
     * The class that represent a input of type month
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldMonth extends FieldDate
    {
        protected string $type    = 'month';
        protected string $pattern = \F2P2\REGEX_MONTH;
    }


    /**
     * The class that represent a input of type week
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldWeek extends FieldDate
    {
        protected string $type        = 'week';
        protected string $pattern     = \F2P2\REGEX_WEEK;
    }


    /**
     * The class that represent a input of type file
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldFile extends Field
    {
        protected string $type    = 'file';
        protected array  $accept  = [];
        protected ?int   $maxSize = null;
        protected array  $data    = [];


        /**
         * Save the file handled by the field
         * 
         * @param  string $to The destination path
         * @return bool       true if the file was successfully saved, otherwise false
         */
        public function saveFile(
            string $to
        ) : bool
        {
            return array_key_exists('tmp_name', $this->data) ? move_uploaded_file($this->data['tmp_name'], $to) : false;
        }

        public function isValid() : bool
        {
            if (count($this->data) === 0)
            {
                return parent::isValid();
            }
            else
            {
                foreach (['name', 'full_path', 'type', 'tmp_name', 'error', 'size'] as $key)
                {
                    if (!array_key_exists($key, $this->data))
                    {
                        return false;
                    }
                }
                return parent::isValid() && $this->data['error'] === 0 && ($this->maxSize === null || $this->data['size'] <= $this->maxSize) && (count($this->accept) === 0 || in_array($this->data['type'], $this->accept));
            }
        }

        /**
         * Add a file type accepted in the field
         * 
         * @param  string $type The type in MIME format
         * @return object       The field
         */
        public function addAccept(
            string $type
        ) : object
        {
            $this->accept[] = $type;
            return $this;
        }

        /**
         * Define the file maximum size
         * 
         * @param  int    $maxSize The size (o)
         * @return object          The field
         */
        public function setMaxSize(
            int $maxSize
        ) : object
        {
            $this->maxSize = $maxSize;
            return $this;
        }

        /**
         * Define the actual file data
         * 
         * @param  array  $data The data
         * @return object       The field
         */
        public function setData(
            array $data
        ) : object
        {
            $this->data = $data;
            return $this;
        }

        protected function renderAttributes() : string
        {
            return parent::renderAttributes().' accept="'.implode(', ', $this->accept).'"';
        }
    }


    /**
     * The class that represent a input of type submit
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class FieldSubmit extends Field
    {
        protected string $type = 'submit';


        public function __construct()
        {
            $this->name = 'submit';
        }
    }


    /**
     * The base class for form objets that can contain other form objects
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    abstract class FieldContainer extends FormObject
    {
        protected array $fields = [];


        /**
         * Check if all the children are valid
         * 
         * @return bool true if valid, otherwise false
         */
        public function isValid() : bool
        {
            foreach ($this->getAllFields() as $field)
            {
                if ($field->isValid() === false)
                {
                    return false;
                }
            }
            return true;
        }

        /**
         * Add a field to the children
         * 
         * @param  FormObject $field The field
         * @return object            The container
         */
        public function addField(
            FormObject $field
        ) : object
        {
            $this->fields[] = $field;
            return $this;
        }

        /**
         * Returns all the children recursively
         * 
         * @return array The children
         */
        protected function getAllFields() : array
        {
            $fields = [];
            foreach ($this->fields as $field)
            {
                if (is_subclass_of($field, FieldContainer::class))
                {
                    $fields = array_merge($fields, $field->getAllFields());
                }
                else
                {
                    $fields[] = $field;
                }
            }
            return $fields;
        }

        public function render() : string
        {
            $html = '';
            foreach ($this->fields as $field)
            {
                $html .= $field->render();
            }
            return $html;
        }
    }


    /**
     * The class that represent a fieldset
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Fieldset extends FieldContainer
    {
        private ?string $legend = null;


        /**
         * Define the fieldset legend
         * 
         * @param  string $legend The legend
         * @return object         The fieldset
         */
        public function setLegend(
            string $legend
        ) : object
        {
            $this->legend = $legend;
            return $this;
        }

        public function render() : string
        {
            return '<fieldset '.$this->renderAttributes().'>'.($this->legend ? '<legend>'.$this->legend.'</legend>' : '').parent::render().'</fieldset>';
        }
    }

    /**
     * The class that represent a form
     * 
     * @link      link
     * @since     Version 0.0
     * @author    Thomas Baricault <thomas.baricault@gmail.com>
     * @copyright Copyright (c) 2024, Thomas Baricault
     * @license   https://opensource.org/license/bsd-2-clause
     */
    class Form extends FieldContainer
    {
        private string  $method     = 'POST';
        private ?string $action     = null;
        private bool    $allowFiles = false;


        /**
         * Returns all the form data
         * 
         * @return array The data (fieldname => value)
         */
        public function getData() : array
        {
            $data = [];
            foreach ($this->getAllFields() as $field)
            {
                $data[$field->getName()] = $field->getValue();
            }
            return [];
        }

        /**
         * Get a field using it's name
         * 
         * @param  string|array $name The field name
         * @return ?object            The field if found, otherwise null
         */
        public function getField(
            string $name
        ) : ?object
        {
            foreach ($this->getAllFields() as $field)
            {
                if ($name === $field->getName())
                {
                    return $field;
                }
            }
            return null;
        }

        /**
         * Define the form submit method
         * 
         * @param  string $method The method
         * @return object         The form
         */
        public function setMethod(
            string $method
        ) : object
        {
            $this->method = $method;
            return $this;
        }

        /**
         * Define the form action
         * 
         * @param  string $action The action
         * @return object         The form
         */
        public function setAction(
            string $action
        ) : object
        {
            $this->action = \F2P2\Router::getRouteURL($action);
            return $this;
        }

        /**
         * Allow the file upload in the form
         * 
         * @return object The form
         */
        public function allowFiles() : object
        {
            $this->allowFiles = true;
            return $this;
        }

        /**
         * Fill the form with data
         * 
         * @param  array $data The data (fieldname => value)
         */
        public function fill(
            array $data
        ) : void
        {
            $data = array_reverse($data);
            foreach ($this->getAllFields() as $field)
            {
                if (array_key_exists($field->getName(), $data))
                {
                    if (get_class($field) === FieldFile::class)
                    {
                        $field->setData($data[$field->getName()]);
                    }
                    else if (get_class($field) === FieldCheckbox::class || get_class($field) === FieldRadio::class)
                    {
                        $field->check();
                    }
                    else
                    {
                        $field->setValue($data[$field->getName()]);
                    }
                    unset($data[$field->getName()]);
                }
            }
        }

        public function render() : string
        {
            return '<form method="'.$this->method.'"'.($this->action ? ' action="'.$this->action.'"' : '').($this->allowFiles ? ' enctype="multipart/form-data"' : '').' '.$this->renderAttributes().'>'.parent::render().'</form>';
        }
    }
?>