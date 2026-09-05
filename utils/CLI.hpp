#ifndef CLI
#define CLI


#include <string>
#include <vector>
#include <map>


using namespace std;


namespace CLI
{
    class Interface;
    class Command;


    enum ANSI
    {
        DEFAULT         = 0,
        BOLD            = 1,
        UNDERLINE       = 4,
        NOUNDERLINE     = 24,
        REVERSE         = 7,
        NOREVERSE       = 27,
        FG_BLACK        = 30,
        BG_BLACK        = 40,
        FG_GRAY         = 90,
        BG_GRAY         = 100,
        FG_LIGHTGRAY    = 37,
        BG_LIGHTGRAY    = 47,
        FG_WHITE        = 97,
        GB_WHITE        = 107,
        FG_RED          = 31,
        BG_RED          = 41,
        FG_LIGHTRED     = 91,
        BG_LIGHTRED     = 101,
        FG_GREEN        = 32,
        BG_GREEN        = 42,
        FG_LIGHTGREEN   = 92,
        BG_LIGHTGREEN   = 102,
        FG_YELLOW       = 33,
        BG_YELLOW       = 43,
        FG_LIGHTYELLOW  = 93,
        BG_LIGHTYELLOW  = 103,
        FG_BLUE         = 34,
        BG_BLUE         = 44,
        FG_LIGHTBLUE    = 94,
        BG_LIGHTBLUE    = 104,
        FG_MAGENTA      = 35,
        BG_MAGENTA      = 45,
        FG_LIGHTMAGENTA = 95,
        BG_LIGHTMAGENTA = 105,
        FG_CYAN         = 36,
        BG_CYAN         = 46,
        FG_LIGHTCYAN    = 96,
        BG_LIGHTCYAN    = 106,
    };

    enum Errors
    {
        ERR_NONE,
        ERR_PARAMETER_REQUIRE_ARGUMENT,
        ERR_TOO_FEW_ARGUMENTS,
        ERR_TOO_MANY_ARGUMENTS,
        ERR_UNKNOW_COMMAND,
    };


    typedef map<string, string> Parameters;
    typedef vector<string>      ParameterArray, ArgArray;
    typedef vector<Command*>    CommandArray;

    typedef void (*Action) (
        Command*,
        Parameters,
        Parameters,
        ParameterArray
    );


    string colorize(
        string text,
        int    ansi
    ) {
        return "\e["+to_string(ansi)+"m"+text+"\e[0m";
    }


    class Command
    {
        public:
            Command(
                string name,
                string description,
                Action function    = nullptr
            ) {
                this->name = name;
                this->description = description;
                this->function = function;
            }

            ~Command()
            {
                for (auto command : this->subcommands)
                {
                    delete command;
                }
            }

            string getName()
            {
                return this->name;
            }

            string getDescription()
            {
                return this->description;
            }

            Command* getCommand(
                ArgArray args
            ) {
                if (args.size() > 0 && args.at(0) == this->name)
                {
                    args.erase(args.begin());
                    for (auto command : this->subcommands)
                    {
                        Command* result = command->getCommand(args);
                        if (result)
                        {
                            return result;
                        }
                    }
                    return this;
                }
                else
                {
                    return nullptr;
                }
            }

            Command* addSubCommand(
                Command* command
            ) {
                this->subcommands.push_back(command);
                command->parent = this;
                return this;
            }

            Command* addRequiredParameter(
                string name,
                string description
            ) {
                this->required.push_back({
                    name,
                    description
                });
                return this;
            }

            Command* addOptionalParameter(
                string name,
                string description,
                string short_name    = "",
                string argument_name = "",
                string default_value = ""
            ) {
                this->optional.push_back({
                    name,
                    description,
                    short_name,
                    argument_name,
                    default_value
                });
                return this;
            }

            Command* allowParameterList(
                string name,
                string description
            ) {
                this->allow_list = true;
                this->list_name = name;
                this->list_description = description;
                return this;
            }

            int execute(
                ArgArray args
            ) {
                if (args.size() > 0 && args.at(0) == this->name)
                {
                    args.erase(args.begin());
                    for (auto command : this->subcommands)
                    {
                        int code = command->execute(args);
                        if (code != ERR_UNKNOW_COMMAND)
                        {
                            return code;
                        }
                    }
                    if (args.size() >= this->required.size())
                    {
                        Parameters required;
                        Parameters optional;
                        ParameterArray list;
                        for (struct Optional& o : this->optional)
                        {
                            optional[o.name] = o.default_value;
                        }
                        for (int i = 0; i < this->required.size(); i++)
                        {
                            required[this->required.at(i).name] = args.at(i);
                        }
                        args.erase(args.begin(), args.begin()+this->required.size());
                        int i = 0;
                        while (i < args.size())
                        {
                            bool found = false;
                            for (struct Optional& o : this->optional)
                            {
                                if (args.at(i) == "--"+o.name || !o.short_name.empty() && args.at(i) == "-"+o.short_name)
                                {
                                    found = true;
                                    if (o.argument_name.empty())
                                    {
                                        optional[o.name] = "defined";
                                        i++;
                                    }
                                    else
                                    {
                                        if (i == args.size()-1)
                                        {
                                            return ERR_PARAMETER_REQUIRE_ARGUMENT;
                                        }
                                        else
                                        {
                                            optional[o.name] = args.at(i+1);
                                            i += 2;
                                        }
                                    }
                                }
                                if (found)
                                {
                                    break;
                                }
                            }
                            if (!found)
                            {
                                if (this->allow_list)
                                {
                                    list.push_back(args.at(i));
                                    i++;
                                }
                                else
                                {
                                    return ERR_TOO_MANY_ARGUMENTS;
                                }
                            }
                        }
                        if (this->function)
                        {
                            this->function(this, required, optional, list);
                        }
                        return ERR_NONE;
                    }
                    else
                    {
                        return ERR_TOO_FEW_ARGUMENTS;
                    }
                }
                else
                {
                    return ERR_UNKNOW_COMMAND;
                }
            }

            static string commandsToString(
                CommandArray commands
            ) {
                int length = 0;
                for (auto command : commands)
                {
                    if (command->name.length() > length)
                    {
                        length = command->name.length();
                    }
                }
                string result;
                for (auto command : commands)
                {
                    result += colorize(command->name, FG_LIGHTYELLOW);
                    string spaces = "   ";
                    for (int i = command->name.length(); i < length; i++)
                    {
                        spaces += " ";
                    }
                    result += colorize(spaces+command->description, FG_WHITE)+"\n";
                }
                return result;
            }

            string toString()
            {
                string result = colorize(this->description, FG_WHITE)+"\n\n"+colorize("Usage", FG_LIGHTBLUE)+"\n"+colorize(this->getFullCommand(), FG_LIGHTYELLOW);
                for (auto& r : this->required)
                {
                    result += colorize(" <"+r.name+">", FG_LIGHTGRAY);
                }
                if (this->allow_list)
                {
                    result += colorize(" ["+this->list_name+"]", FG_GRAY);
                }
                for (auto& o : this->optional)
                {
                    result += colorize(" ["+Command::getOptionalUsage(o)+"]", FG_GRAY);
                }
                result += "\n";
                if (this->allow_list)
                {
                    result += "\n"+colorize(this->list_name, FG_LIGHTGRAY)+colorize("   "+this->list_description, FG_WHITE);
                }
                if (this->required.size() > 0)
                {
                    result += "\n"+colorize("Required Parameters", FG_LIGHTBLUE)+"\n"+Command::requiredToString(this->required);
                }
                if (this->optional.size() > 0)
                {
                    result += "\n"+colorize("Optional Parameters", FG_LIGHTBLUE)+"\n"+Command::optionalToString(this->optional);
                }
                if (this->subcommands.size() > 0)
                {
                    result += "\n"+colorize("Subcommands", FG_LIGHTBLUE)+"\n"+Command::commandsToString(this->subcommands);
                }
                return result+"\n";
            }


        private:
            struct Required
            {
                string name;
                string description;
            };

            struct Optional
            {
                string name;
                string description;
                string short_name;
                string argument_name;
                string default_value;
            };


            typedef vector<struct Required> RequiredArray;
            typedef vector<struct Optional> OptionalArray;


            Command*      parent           = nullptr;
            string        name;
            string        description;
            Action        function;
            CommandArray  subcommands;
            RequiredArray required;
            OptionalArray optional;
            bool          allow_list       = false;
            string        list_name;
            string        list_description;


            string getFullCommand()
            {
                if (this->parent)
                {
                    return this->parent->getFullCommand()+" "+this->name;
                }
                else
                {
                    return "f2p2 "+this->name;
                }
            }

            static string getOptionalUsage(
                struct Optional optional
            ) {
                string usage = "--"+optional.name;
                if (!optional.short_name.empty())
                {
                    usage += "|-"+optional.short_name;
                }
                if (!optional.argument_name.empty())
                {
                    usage += " <"+optional.argument_name+">";
                }
                return usage;
            }

            static string requiredToString(
                vector<struct Required> required
            ) {
                int length = 0;
                for (auto& r : required)
                {
                    if (r.name.length() > length)
                    {
                        length = r.name.length();
                    }
                }
                string result;
                for (auto& r : required)
                {
                    result += colorize(r.name, FG_LIGHTGRAY);
                    string spaces = "   ";
                    for (int i = r.name.length(); i < length; i++)
                    {
                        spaces += " ";
                    }
                    result += colorize(spaces+r.description, FG_WHITE)+"\n";
                }
                return result;
            }

            static string optionalToString(
                vector<struct Optional> optional
            ) {
                int length = 0;
                for (auto& o : optional)
                {
                    string usage = Command::getOptionalUsage(o);
                    if (usage.length() > length)
                    {
                        length = usage.length();
                    }
                }
                string result;
                for (auto& o : optional)
                {
                    string usage = Command::getOptionalUsage(o);
                    string spaces = "   ";
                    for (int i = usage.length(); i < length; i++)
                    {
                        spaces += " ";
                    }
                    result += colorize(usage, FG_GRAY)+colorize(spaces+o.description, FG_WHITE);
                    if (!o.default_value.empty())
                    {
                        result += colorize(" (default: ", FG_WHITE)+colorize(o.default_value, FG_WHITE)+colorize(")", FG_WHITE);
                    }
                    result += "\n";
                }
                return result;
            }
    };


    class Interface
    {
        public:
            Interface()
            {}

            ~Interface()
            {
                for (auto command : this->commands)
                {
                    delete command;
                }
            }

            Command* getCommand(
                ArgArray args
            ) {
                for (auto command : this->commands)
                {
                    Command* result = command->getCommand(args);
                    if (result)
                    {
                        return result;
                    }
                }
                return nullptr;
            }

            CommandArray& getCommands()
            {
                return this->commands;
            }

            Interface* addCommand(
                Command* command
            ) {
                this->commands.push_back(command);
                return this;
            }

            int execute(
                int   argc,
                char* argv[]
            ) {
                ArgArray args;
                for (int i = 1; i < argc; i++)
                {
                    args.push_back(argv[i]);
                }
                for (auto command : this->commands)
                {
                    int code = command->execute(args);
                    if (code != ERR_UNKNOW_COMMAND)
                    {
                        switch (code)
                        {
                            case ERR_PARAMETER_REQUIRE_ARGUMENT: cout << colorize("Parameter requires argument but none was given.", FG_LIGHTRED) << endl; break;
                            case ERR_TOO_FEW_ARGUMENTS:          cout << colorize("Too few arguments for this command.", FG_LIGHTRED)             << endl; break;
                            case ERR_TOO_MANY_ARGUMENTS:         cout << colorize("Too many arguments for this command.", FG_LIGHTRED)            << endl; break;
                        }
                        return code;
                    }
                }
                cout << colorize("Unknow command.", FG_LIGHTRED) << endl;
                return ERR_UNKNOW_COMMAND;
            }


        private:
            CommandArray commands;
    };
}


#endif