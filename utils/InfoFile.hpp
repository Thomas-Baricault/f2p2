#ifndef INFOFILE
#define INFOFILE


#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


using namespace std;


namespace InfoFile
{
    class InfoTemplate;
    class InfoBlock;


    typedef map<string, bool>        TemplateProperties;
    typedef map<InfoTemplate*, bool> TemplateChildren;
    typedef map<string, string>      BlockProperties;
    typedef vector<InfoBlock*>       BlockChildren;


    class InfoTemplate
    {
        public:
            string             name;
            TemplateProperties properties;
            TemplateChildren   children;


            InfoTemplate(
                string name
            ) {
                this->name = name;
            }

            ~InfoTemplate()
            {
                for (auto [child, required] : this->children)
                {
                    delete child;
                }
            }

            InfoTemplate* addProperty(
                string name,
                bool   required = false
            ) {
                this->properties[name] = required;
                return this;
            }

            InfoTemplate* addChild(
                InfoTemplate* child,
                bool       required = false
            ) {
                this->children[child] = required;
                return this;
            }
    };


    class InfoBlock
    {
        public:
            InfoBlock(
                string name
            ) {
                this->name = name;
            }

            ~InfoBlock()
            {
                for (auto child : this->children)
                {
                    delete child;
                }
            }

            static InfoBlock* fromFile(
                string path
            ) {
                ifstream ifs(path.c_str());
                if (ifs.is_open())
                {
                    stringstream buffer;
                    buffer << ifs.rdbuf();
                    ifs.close();
                    return InfoBlock::fromString(buffer.str());
                }
                else
                {
                    return nullptr;
                }
            }

            static InfoBlock* fromString(
                string src
            ) {
                int i = 0;
                bool new_line = false;
                InfoBlock* block = new InfoBlock(InfoBlock::readNextWord(src, i, new_line));
                InfoBlock::read(src, i, block);
                return block;
            }

            void toFile(
                string path
            ) {
                ofstream ofs(path.c_str());
                if (ofs.is_open())
                {
                    ofs << this->toString();
                    ofs.close();
                }
            }

            string toString()
            {
                string result;
                this->write(result);
                return result;
            }

            InfoBlock* copy()
            {
                InfoBlock* copy = new InfoBlock(this->name);
                for (auto [name, value] : this->properties)
                {
                    copy->addProperty(name, value);
                }
                for (auto child : this->children)
                {
                    copy->addChild(child->copy());
                }
                return copy;
            }

            string getProperty(
                string name
            ) {
                if (this->properties.find(name) == this->properties.end())
                {
                    return "";
                }
                else
                {
                    return this->properties.at(name);
                }
            }

            BlockProperties& getProperties()
            {
                return this->properties;
            }

            InfoBlock* getChild(
                string name,
                string value
            ) {
                for (auto child : this->children)
                {
                    if (child->getProperty(name) == value)
                    {
                        return child;
                    }
                }
                return nullptr;
            }

            BlockChildren& getChildren()
            {
                return this->children;
            }

            InfoBlock* addProperty(
                string name,
                string value
            ) {
                this->properties[name] = value;
                return this;
            }

            InfoBlock* addChild(
                InfoBlock* child
            ) {
                this->children.push_back(child);
                return this;
            }

            void removeProperty(
                string name
            ) {
                if (this->properties.find(name) != this->properties.end())
                {
                    this->properties.erase(name);
                }
            }

            void removeChild(
                string name,
                string value
            ) {
                for (vector<InfoBlock*>::iterator child = this->children.begin(); child != this->children.end(); child++)
                {
                    if ((*child)->getProperty(name) == value)
                    {
                        delete (*child);
                        this->children.erase(child);
                        break;
                    }
                }
            }

            static InfoBlock* clean(
                InfoBlock*    block,
                InfoTemplate* template_
            ) {
                if (block)
                {
                    if (block->name == template_->name)
                    {
                        for (auto [property, required] : template_->properties)
                        {
                            if (required && block->properties.find(property) == block->properties.end())
                            {
                                delete block;
                                return nullptr;
                            }
                        }
                        for (auto [template_child, required] : template_->children)
                        {
                            if (required)
                            {
                                bool found = false;
                                for (auto block_child : block->children)
                                {
                                    if (block_child->name == template_child->name)
                                    {
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found)
                                {
                                    delete block;
                                    return nullptr;
                                }
                            }
                        }
                        vector<string> properties_to_remove;
                        for (auto [name, value] : block->properties)
                        {
                            if (template_->properties.find(name) == template_->properties.end())
                            {
                                properties_to_remove.push_back(name);
                            }
                        }
                        for (auto property : properties_to_remove)
                        {
                            block->properties.erase(property);
                        }
                        vector<int> children_to_remove;
                        int i = 0;
                        for (auto block_child : block->children)
                        {
                            bool found = false;
                            for (auto [template_child, required] : template_->children)
                            {
                                if (template_child->name == block_child->name)
                                {
                                    if (InfoBlock::clean(block_child, template_child) == nullptr)
                                    {
                                        children_to_remove.push_back(i);
                                    }
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                children_to_remove.push_back(i);
                            }
                            i++;
                        }
                        for (i = children_to_remove.size()-1; i >= 0; i--)
                        {
                            block->children.erase(block->children.begin()+children_to_remove.at(i));
                        }
                    }
                    else
                    {
                        delete block;
                        return nullptr;
                    }
                }
                return block;
            }


        private:
            string          name;
            BlockProperties properties;
            BlockChildren   children;


            static string readNextWord(
                string& src,
                int&    i,
                bool&   new_line
            ) {
                int length = src.length();
                while (i < length && (src.at(i) == ' ' || src.at(i) == '\n'))
                {
                    if (src.at(i) == '\n')
                    {
                        new_line = true;
                    }
                    i++;
                }
                int j = i;
                while (j < length && src.at(j) != ' ' && src.at(j) != '\n')
                {
                    j++;
                }
                string word = src.substr(i, j-i);
                i = j;
                return word;
            }

            static void read(
                string&    src,
                int&       i,
                InfoBlock* block
            ) {
                bool run = true;
                while (run)
                {
                    bool new_line = false;
                    string name = InfoBlock::readNextWord(src, i, new_line);
                    new_line = false;
                    int j = i;
                    string value = InfoBlock::readNextWord(src, j, new_line);
                    if (value.length() == 0)
                    {
                        run = false;
                    }
                    else if (new_line)
                    {
                        if (name == "END")
                        {
                            run = false;
                        }
                        else
                        {
                            InfoBlock* child = new InfoBlock(name);
                            block->addChild(child);
                            InfoBlock::read(src, i, child);
                        }
                    }
                    else
                    {
                        i = j;
                        block->addProperty(name, value);
                    }
                }
            }

            void write(
                string& dst,
                int     increment = 0
            ) {
                string space;
                for (int i = 0; i < increment*4; i++)
                {
                    space += ' ';
                }
                dst += space+this->name+'\n';
                for (auto& [name, value] : this->properties)
                {
                    dst += space+"    "+name+' '+value+'\n';
                }
                for (auto child : this->children)
                {
                    child->write(dst, increment+1);
                }
                dst += space+"END\n";
            }
    };
}


#endif