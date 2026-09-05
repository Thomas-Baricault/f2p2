#define VERSION             "0.0.0"
#define LATEST_VERSION_URL  "http://localhost/latest-version"
#define DOWNLOAD_UPDATE_URL "http://localhost/download/v"
#define MIME_TYPES_URL      "http://svn.apache.org/repos/asf/httpd/httpd/trunk/docs/conf/mime.types"


#include <iostream>
#include <string>
#include <algorithm>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <mysql/mysql.h>
#include <windows.h>

#include "utils/stringcase.hpp"
#include "utils/CLI.hpp"
#include "utils/InfoFile.hpp"


using namespace std;


HANDLE CONSOLE_HOUT;
DWORD CONSOLE_OUTMODE;
CLI::Interface cli;
InfoFile::InfoTemplate* DATABASE_TEMPLATE;


struct ColumnInFile
{
    string content;
    bool   is_valid     = false;
    int    params_begin = -1;
    int    params_end   = -1;
    int    getter_begin = -1;
    int    getter_end   = -1;
    int    setter_begin = -1;
    int    setter_end   = -1;
    int    class_end    = -1;
};


void init();
void cleanup();



bool fileExists(
    string path
) {
    ifstream ifs(path.c_str());
    if (ifs.is_open())
    {
        ifs.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool controllerExists(
    string name
) {
    return fileExists("./app/controllers/"+name+"Controller.php");
}

bool modelExists(
    string name
) {
    return fileExists("./app/models/"+name+"Model.php");
}

void checkDir(
    string path
) {
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
    }
    else
    {
        mkdir(path.c_str());
    }
}

string readFile(
    string path,
    string default_content = ""
) {
    ifstream ifs(path.c_str());
    if (ifs.is_open())
    {
        stringstream buffer;
        buffer << ifs.rdbuf();
        ifs.close();
        string content = buffer.str();
        if (content.length() == 0)
        {
            return default_content;
        }
        else
        {
            return content;
        }
    }
    else
    {
        return default_content;
    }
}

void destroyFileIfExists(
    string path
) {
    if (fileExists(path))
    {
        remove(path.c_str());
    }
}

size_t curlWriteString(
    void*   contents,
    size_t  size,
    size_t  nmemb,
    string* str
) {
    size_t new_length = size*nmemb;
    str->append((char*)contents, new_length);
    return new_length;
}

size_t curlWriteFile(
    void*  ptr,
    size_t size,
    size_t nmemb,
    FILE*  file
) {
    size_t written = fwrite(ptr, size, nmemb, file);
    return written;
}

string curlToString(
    string url
) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl)
    {
        string result;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        CURLcode code = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (code == CURLE_OK)
        {
            return result;
        }
    }
    return "";
}

bool curlToFile(
    string url,
    string path
) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl)
    {
        FILE* file = fopen(path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        CURLcode code = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(file);
        if (code == CURLE_OK)
        {
            return true;
        }
        else
        {
            remove(path.c_str());
        }
    }
    return false;
}

string getLatestCLIVersion()
{
    string version = curlToString(LATEST_VERSION_URL);
    int i = version.find(".");
    if (i > 0)
    {
        int j = version.find(".", i+1);
        if (j > i+1)
        {
            bool valid = true;
            for (int k = 0; k < i; k++)
            {
                if (!('0' <= version.at(k) && version.at(k) <= '9'))
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {
                for (int k = i+1; k < j; k++)
                {
                    if (!('0' <= version.at(k) && version.at(k) <= '9'))
                    {
                        valid = false;
                        break;
                    }
                }
                if (valid)
                {
                    for (int k = j+1; k < version.length(); k++)
                    {
                        if (!('0' <= version.at(k) && version.at(k) <= '9'))
                        {
                            valid = false;
                            break;
                        }
                    }
                    if (valid)
                    {
                        return version;
                    }
                }
            }
        }
    }
    return "";
}

string mysqlTypeToPHPType(
    string type
) {
    if (
        type == "CHAR"       ||
        type == "VARCHAR"    ||
        type == "BINARY"     ||
        type == "VARBINARY"  ||
        type == "TINYTEXT"   ||
        type == "TEXT"       ||
        type == "MEDIUMTEXT" ||
        type == "LONGTEXT"   ||
        type == "DATE"       ||
        type == "DATETIME"   ||
        type == "TIMESTAMP"  ||
        type == "TIME"       ||
        type == "ENUM"
    ) {
        return "string";
    }
    else if (
        type == "TINYINT"   ||
        type == "SMALLINT"  ||
        type == "MEDIUMINT" ||
        type == "INTEGER"   ||
        type == "INT"       ||
        type == "BIGINT"    ||
        type == "YEAR"      ||
        type == "BIT"
    ) {
        return "int";
    }
    else if (
        type == "FLOAT"   ||
        type == "DOUBLE"  ||
        type == "DECIMAL" ||
        type == "DEC"
    ) {
        return "float";
    }
    else if (
        type == "BOOLEAN" ||
        type == "BOOL"
    ) {
        return "bool";
    }
    else if (
        type == "TINYBLOB"   ||
        type == "BLOB"       ||
        type == "MEDIUMBLOB" ||
        type == "LONGBLOB"
    ) {
        return "resource";
    }
    else if (
        type == "SET"
    ) {
        return "array";
    }
    else
    {
        return "";
    }
}

bool mysqlNeedSize(
    string type
) {
    return (
        type == "VARCHAR"   ||
        type == "VARBINARY"
    );
}

bool mysqlUseSize(
    string type
) {
    return (
        type == "CHAR"      ||
        type == "VARCHAR"   ||
        type == "BINARY"    ||
        type == "VARBINARY" ||
        type == "BIT"
    );
}

bool mysqlNeedAdditional(
    string type
) {
    return (
        type == "ENUM" ||
        type == "SET"
    );
}

bool mysqlUseAdditional(
    string type
) {
    return (
        type == "DOUBLE"  ||
        type == "DECIMAL" ||
        type == "DEC"     ||
        type == "ENUM"    ||
        type == "SET"
    );
}

bool getDBConfig(
    map<string, string>& result
) {
    ifstream ifs("./config/db.php");
    if (ifs.is_open())
    {
        stringstream buffer;
        buffer << ifs.rdbuf();
        ifs.close();
        string content = buffer.str();
        vector<string> vars = {"TYPE", "HOST", "NAME", "USER", "PASS"}; 
        for (auto var : vars)
        {
            string search = "define('DB_"+var+"',";
            int begin = content.find(search);
            if (begin == -1)
            {
                return false;
            }
            begin = content.find("'", begin+search.length());
            if (begin == -1)
            {
                return false;
            }
            begin++;
            int end = content.find("'", begin);
            if (end == -1)
            {
                return false;
            }
            result[var] = content.substr(begin, end-begin);
        }
        return true;
    }
    else
    {
        return false;
    }
}

InfoFile::InfoBlock* getDBInfos()
{
    return InfoFile::InfoBlock::clean(
        InfoFile::InfoBlock::fromFile("./app/db/draft.info"),
        DATABASE_TEMPLATE
    );
}

string getNowVersion()
{
    char buffer[15];
    time_t timestamp = time(NULL);
    struct tm* now = localtime(&timestamp);
    snprintf(
        buffer, 15, "%4d%02d%02d%02d%02d%02d",
        now->tm_year+1900,
        now->tm_mon+1,
        now->tm_mday,
        now->tm_hour,
        now->tm_min,
        now->tm_sec
    );
    return buffer;
}

void saveDBInfos(
    InfoFile::InfoBlock* infos,
    string               name  = "draft"
) {
    checkDir("./app");
    checkDir("./app/db");
    infos->toFile("./app/db/"+name+".info");
}

bool cmpColumnsInfos(
    InfoFile::InfoBlock* first,
    InfoFile::InfoBlock* second
) {
    for (auto [name, value] : first->getProperties())
    {
        if (second->getProperty(name) != value)
        {
            return false;
        }
    }
    return true;
}

vector<InfoFile::InfoBlock*> getModelReferences(
    InfoFile::InfoBlock* infos,
    InfoFile::InfoBlock* model
) {
    vector<InfoFile::InfoBlock*> result;
    for (auto column : model->getChildren())
    {
        if (!column->getProperty("REFERENCE").empty())
        {
            result.push_back(infos->getChild("NAME", column->getProperty("REFERENCE")));
        }
    }
    return result;
}

vector<InfoFile::InfoBlock*> getModelTargeted(
    InfoFile::InfoBlock* infos,
    InfoFile::InfoBlock* model
) {
    vector<InfoFile::InfoBlock*> result;
    for (auto child : infos->getChildren())
    {
        if (child != model)
        {
            for (auto column : child->getChildren())
            {
                if (column->getProperty("REFERENCE") == model->getProperty("NAME"))
                {
                    result.push_back(child);
                }
            }
        }
    }
    return result;
}

bool checkReferences(
    InfoFile::InfoBlock* infos
) {
    for (auto model : infos->getChildren())
    {
        vector<InfoFile::InfoBlock*> refs = getModelReferences(infos, model);
        if (find(refs.begin(), refs.end(), nullptr) != refs.end())
        {
            return false;
        }
    }
    return true;
}

bool checkReferenceLoop(
    InfoFile::InfoBlock* infos
) {
    for (auto model : infos->getChildren())
    {
        vector<InfoFile::InfoBlock*> refs = getModelReferences(infos, model);
        int i = infos->getChildren().size();
        while (refs.size() > 0)
        {
            vector<InfoFile::InfoBlock*> new_refs;
            for (auto ref : refs)
            {
                for (auto new_ref : getModelReferences(infos, ref))
                {
                    new_refs.push_back(new_ref);
                }
            }
            refs = new_refs;
            i--;
            if (i == 0)
            {
                return false;
            }
        }
    }
    return true;
}

void sortByReferences(
    InfoFile::InfoBlock* infos
) {
    vector<InfoFile::InfoBlock*> sorted;
    for (auto model : infos->getChildren())
    {
        int i = 0;
        while (i < sorted.size())
        {
            vector<InfoFile::InfoBlock*> refs = getModelReferences(infos, model);
            if (find(refs.begin(), refs.end(), sorted.at(i)) != refs.end())
            {
                break;
            }
            else
            {
                i++;
            }
        }
        sorted.insert(sorted.begin()+i, model);
    }
    infos->getChildren() = sorted;
}

bool addMethodInFile(
    string path,
    string code
) {
    string content = readFile(path);
    int i = content.length()-1;
    while (i >= 0 && content.at(i) != '}')
    {
        i--;
    }
    i--;
    while (i >= 0 && content.at(i) != '{' && content.at(i) != '}' && content.at(i) != ';')
    {
        i--;
    }
    if (i < 0)
    {
        return false;
    }
    ofstream ofs(path.c_str());
    ofs << content.substr(0, i+1);
    if (content.at(i) == '}')
    {
        ofs << endl;
    }
    else if (content.at(i) == ';')
    {
        ofs << endl << endl;
    }
    ofs << endl << code << content.substr(i+1);
    return true;
}

bool createControllerFile(
    string name
) {
    if (controllerExists(name))
    {
        return false;
    }
    checkDir("./app");
    checkDir("./app/controllers");
    ofstream ofs(("./app/controllers/"+name+"Controller.php").c_str());
    ofs
        << "<?php"                                                             << endl
        << "    namespace App\\Controllers;"                                   << endl
        << ""                                                                  << endl
        << ""                                                                  << endl
        << "    class " << name << "Controller extends \\F2P2\\BaseController" << endl
        << "    {"                                                             << endl
        << "    }"                                                             << endl
        << "?>";
    ofs.close();
    return true;
}

bool createModelFile(
    string pascal_case,
    string snake_case
) {
    if (modelExists(pascal_case))
    {
        return false;
    }
    checkDir("./app");
    checkDir("./app/models");
    ofstream ofs(("./app/models/"+pascal_case+"Model.php").c_str());
    ofs
        << "<?php"                                                          << endl
        << "    namespace App\\Models;"                                     << endl
        << ""                                                               << endl
        << ""                                                               << endl
        << "    class " << pascal_case << "Model extends \\F2P2\\BaseModel" << endl
        << "    {"                                                          << endl
        << "        public const TABLE   = '" << snake_case << "';"         << endl
        << "        public const COLUMNS = ["                               << endl
        << "        ];"                                                     << endl
        << "    }"                                                          << endl
        << "?>";
    ofs.close();
    return true;
}

bool addRouteInFile(
    string name,
    string method,
    string uri,
    string controller,
    string action,
    string logged,
    string permissions
) {
    string content = readFile("./config/routes.json", "[\n]");
    int i = content.length()-1;
    while (i >= 0 && content.at(i) != '[' && content.at(i) != '}')
    {
        i--;
    }
    if (i < 0)
    {
        return false;
    }
    checkDir("./config");
    ofstream ofs("./config/routes.json");
    ofs << content.substr(0, i+1);
    if (content.at(i) == '}')
    {
        ofs << ",";
    }
    string space;
    if (!permissions.empty())
    {
        space = " ";
    }
    ofs
        << ""                                                                  << endl
        << "    {"                                                             << endl
        << "        \"name\"      " << space << " : \"" << name       << "\"," << endl
        << "        \"method\"    " << space << " : \"" << method     << "\"," << endl
        << "        \"uri\"       " << space << " : \"" << uri        << "\"," << endl
        << "        \"controller\"" << space << " : \"" << controller << "\"," << endl
        << "        \"action\"    " << space << " : \"" << action     << "\"";
    if (!logged.empty() || !permissions.empty())
    {
        ofs << "," << endl << "        \"logged\"    " << space << " : ";
        if (logged == "TRUE" || !permissions.empty())
        {
            ofs << "true";
        }
        else
        {
            ofs << "false";
        }
    }
    if (!permissions.empty())
    {
        ofs << "," << endl << "        \"permissions\" : [";
        int i = 0;
        int j = 0;
        while (j < permissions.length())
        {
            j = permissions.find(",", i);
            if (j == -1)
            {
                j = permissions.length();
            }
            ofs << endl << "            \""+permissions.substr(i, j-i)+"\"";
            if (j < permissions.length())
            {
                ofs << ", ";
            }
            i = j+1;
        }
        ofs << endl << "        ]";
    }
    ofs
        << endl
        << "    }"
        << content.substr(i+1);
    return true;
}

bool addActionInFile(
    string controller,
    string action,
    string code       = "            return new \\F2P2\\ResponsePage('');\n"
) {
    return addMethodInFile("./app/controllers/"+controller+"Controller.php", "        public function "+action+"(\n            \\F2P2\\Request $request\n        ) : \\F2P2\\Response\n        {\n"+code+"        }");
}

struct ColumnInFile getColumnInFile(
    string model_pascal,
    string column_pascal,
    string column_snake
) {
    struct ColumnInFile result;
    result.content = readFile("./app/models/"+model_pascal+"Model.php");
    string pattern = "public const COLUMNS = [";
    int part_begin = result.content.find(pattern);
    if (part_begin == -1)
    {
        return result;
    }
    part_begin += pattern.length();
    int part_end = result.content.find("];", part_begin);
    if (part_end == -1)
    {
        return result;
    }
    bool found = false;
    result.params_begin = part_begin;
    result.params_end = part_begin;
    while (!found)
    {
        result.params_begin = result.content.find("'", result.params_begin);
        if (result.params_begin == -1 || result.params_begin > part_end)
        {
            break;
        }
        int name_end = result.content.find("'", result.params_begin+1);
        if (name_end == -1 || name_end > part_end)
        {
            break;
        }
        result.params_end = result.content.find("],", name_end+1);
        if (result.params_end == -1 || result.params_end > part_end)
        {
            break;
        }
        result.params_end += 2;
        if (result.content.substr(result.params_begin+1, name_end-result.params_begin-1) == column_snake)
        {
            found = true;
        }
        else
        {
            result.params_begin = result.params_end;
        }
    }
    if (!found)
    {
        result.params_begin = part_end;
    }
    result.params_begin--;
    while (result.content.at(result.params_begin) == ' ' || result.content.at(result.params_begin) == '\n')
    {
        result.params_begin--;
    }
    result.params_begin++;
    if (!found)
    {
        result.params_end = result.params_begin;
    }
    result.class_end = result.content.length()-1;
    while (result.class_end >= 0 && result.content.at(result.class_end) != '}')
    {
        result.class_end--;
    }
    result.class_end--;
    while (result.class_end >= 0 && result.content.at(result.class_end) != '{' && result.content.at(result.class_end) != '}' && result.content.at(result.class_end) != ';')
    {
        result.class_end--;
    }
    if (result.class_end < 0)
    {
        return result;
    }
    found = false;
    part_begin = result.content.find("public function get"+column_pascal);
    if (part_begin != -1)
    {
        part_end = result.content.find("}", part_begin);
        if (part_end != -1)
        {
            result.getter_begin = part_begin;
            result.getter_end = part_end+1;
            found = true;
        }
    }
    if (found)
    {
        result.getter_begin--;
    }
    else
    {
        result.getter_begin = result.class_end;
        result.getter_end = result.class_end;
    }
    while (result.content.at(result.getter_begin) == ' ' || result.content.at(result.getter_begin) == '\n')
    {
        result.getter_begin--;
    }
    result.getter_begin++;
    if (!found)
    {
        result.getter_end = result.getter_begin;
    }
    found = false;
    part_begin = result.content.find("public function set"+column_pascal);
    if (part_begin != -1)
    {
        part_end = result.content.find("}", part_begin);
        if (part_end != -1)
        {
            result.setter_begin = part_begin;
            result.setter_end = part_end+1;
            found = true;
        }
    }
    if (found)
    {
        result.setter_begin--;
    }
    else
    {
        result.setter_begin = result.class_end;
        result.setter_end = result.class_end;
    }
    while (result.content.at(result.setter_begin) == ' ' || result.content.at(result.setter_begin) == '\n')
    {
        result.setter_begin--;
    }
    result.setter_begin++;
    if (!found)
    {
        result.setter_end = result.setter_begin;
    }
    if (
        result.params_begin != -1 && (
            result.getter_begin != -1 && result.getter_begin < result.params_begin ||
            result.setter_begin != -1 && result.setter_begin < result.params_begin
        ) ||
        result.getter_begin != -1 && result.setter_begin != -1 && result.setter_begin < result.getter_begin
    ) {
        return result;
    }
    result.is_valid = true;
    return result;
}

bool addColumnInFile(
    string model_pascal,
    string column_pascal,
    string column_snake,
    string type,
    string null,
    string reference,
    string size,
    string additional
) {
    struct ColumnInFile data = getColumnInFile(model_pascal, column_pascal, column_snake);
    if (!data.is_valid)
    {
        return false;
    }
    ofstream ofs("./app/models/"+model_pascal+"Model.php");
    ofs
        << data.content.substr(0, data.params_begin)        << endl
        << "            '" << column_snake << "' => [";
    string space = "";
    if (!reference.empty())
    {
        space = "     ";
    }
    ofs << endl << "                'type'" << space << " => '" << type << "',";
    if (!null.empty())
    {
        if (null == "TRUE")
        {
            ofs << endl << "                'null'" << space << " => true,";
        }
        else
        {
            ofs << endl << "                'null'" << space << " => false,";
        }
    }
    if (!reference.empty())
    {
        ofs << endl << "                'reference' => '" << reference << "',";
    }
    if (mysqlUseSize(type) && !size.empty())
    {
        ofs << endl << "                'size'" << space << " => " << stoi(size) << ",";
    }
    ofs
        << endl
        << "            ],"
        << data.content.substr(data.params_end, data.getter_begin-data.params_end);
    if (data.content.at(data.getter_begin-1) == ';')
    {
        ofs << endl;
    }
    type = mysqlTypeToPHPType(type);
    ofs
        << ""                                                                << endl
        << ""                                                                << endl
        << "        public function get" << column_pascal << "() : " << type << endl
        << "        {"                                                       << endl
        << "            return $this->data['" << column_snake << "'];"       << endl
        << "        }"
        << data.content.substr(data.getter_end, data.setter_begin-data.getter_end)
        << ""                                                                             << endl
        << ""                                                                             << endl
        << "        public function set" << column_pascal << "("                          << endl
        << "            $" << column_snake                                                << endl
        << "        ) : void"                                                             << endl
        << "        {"                                                                    << endl
        << "            $this->data['" << column_snake << "'] = $" << column_snake << ";" << endl
        << "        }"
        << data.content.substr(data.setter_end);
    ofs.close();
    return true;
}

bool deleteColumnInFile(
    string model_pascal,
    string column_pascal,
    string column_snake
) {
    struct ColumnInFile data = getColumnInFile(model_pascal, column_pascal, column_snake);
    if (!data.is_valid)
    {
        return false;
    }
    ofstream ofs("./app/models/"+model_pascal+"Model.php");
    ofs
        << data.content.substr(0, data.params_begin)
        << data.content.substr(data.params_end, data.getter_begin-data.params_end)
        << data.content.substr(data.getter_end, data.setter_begin-data.getter_end)
        << data.content.substr(data.setter_end);
    ofs.close();
    return true;
}

string mysqlColumnToString(
    InfoFile::InfoBlock* column
) {
    string name = column->getProperty("NAME");
    string type = column->getProperty("TYPE");
    string null = column->getProperty("NULL");
    string size = column->getProperty("SIZE");
    string additional = column->getProperty("ADDITIONAL");
    string result = name+" "+type;
    if (mysqlUseSize(type) && !size.empty() || mysqlUseAdditional(type) && !additional.empty())
    {
        result += "(";
        if (mysqlUseSize(type) && !size.empty())
        {
            result += size;
            if (mysqlUseAdditional(type) && !additional.empty())
            {
                result += ", ";
            }
        }
        if (mysqlUseAdditional(type) && !additional.empty())
        {
            if (type == "ENUM" || type == "SET")
            {
                int i = 0;
                int j = 0;
                while (j < additional.length())
                {
                    j = additional.find(",", i);
                    if (j == -1)
                    {
                        j = additional.length();
                    }
                    result += "\""+additional.substr(i, j-i)+"\"";
                    if (j < additional.length())
                    {
                        result += ", ";
                    }
                    i = j+1;
                }
            }
            else
            {
                result += additional;
            }
        }
        result += ")";
    }
    if (null != "TRUE")
    {
        result += " NOT NULL";
    }
    return result;
}

bool mysqlQuery(
    MYSQL* con,
    string query
) {
    if (mysql_query(con, query.c_str()))
    {
        cout
            << CLI::colorize(query, CLI::FG_LIGHTRED)                                                              << endl
            << CLI::colorize("MySQL error : ", CLI::FG_LIGHTRED) << CLI::colorize(mysql_error(con), CLI::FG_LIGHTRED) << endl;
        return true;
    }
    else
    {
        cout
            << CLI::colorize(query, CLI::FG_LIGHTGREEN) << endl;
        return false;
    }
}

bool mysqlCreateModel(
    MYSQL*               con,
    InfoFile::InfoBlock* model
) {
    string model_name = model->getProperty("NAME");
    string query = "CREATE TABLE "+model_name+" (id INT NOT NULL AUTO_INCREMENT, PRIMARY KEY (id)";
    for (auto column : model->getChildren())
    {
        string column_name = column->getProperty("NAME");
        string column_reference = column->getProperty("REFERENCE");
        query += ", "+mysqlColumnToString(column);
        if (!column_reference.empty())
        {
            query += ", CONSTRAINT ref_"+model_name+"_"+column_name+" FOREIGN KEY "+model_name+"("+column_name+") REFERENCES "+column_reference+"(id)";
        }
    }
    query += ") ENGINE=INNODB";
    return mysqlQuery(con, query);
}

bool mysqlDropModel(
    MYSQL*               con,
    InfoFile::InfoBlock* model
) {
    return mysqlQuery(con, "DROP TABLE "+model->getProperty("NAME"));
}

bool mysqlAddColumn(
    MYSQL*               con,
    InfoFile::InfoBlock* model,
    InfoFile::InfoBlock* column
) {
    return mysqlQuery(con, "ALTER TABLE "+model->getProperty("NAME")+" ADD COLUMN "+mysqlColumnToString(column));
}

bool mysqlModifyColumn(
    MYSQL*               con,
    InfoFile::InfoBlock* model,
    InfoFile::InfoBlock* column
) {
    return mysqlQuery(con, "ALTER TABLE "+model->getProperty("NAME")+" MODIFY COLUMN "+mysqlColumnToString(column));
}

bool mysqlDropColumn(
    MYSQL*               con,
    InfoFile::InfoBlock* model,
    InfoFile::InfoBlock* column
) {
    string model_name = model->getProperty("NAME");
    string column_name = column->getProperty("NAME");
    return mysqlQuery(con, "ALTER TABLE "+model_name+" DROP COLUMN "+column_name);
}

bool mysqlAddReference(
    MYSQL*               con,
    InfoFile::InfoBlock* model,
    InfoFile::InfoBlock* column
) {
    string model_name = model->getProperty("NAME");
    string column_name = column->getProperty("NAME");
    string reference = column->getProperty("REFERENCE");
    return mysqlQuery(con, "ALTER TABLE "+model_name+" ADD CONSTRAINT ref_"+model_name+"_"+column_name+" FOREIGN KEY "+model_name+"("+column_name+") REFERENCES "+reference+"(id)");
}

bool mysqlDropReference(
    MYSQL*               con,
    InfoFile::InfoBlock* model,
    InfoFile::InfoBlock* column
) {
    string model_name = model->getProperty("NAME");
    string column_name = column->getProperty("NAME");
    return mysqlQuery(con, "ALTER TABLE "+model_name+" DROP FOREIGN KEY ref_"+model_name+"_"+column_name);
}

void hello()
{
    cout
        << CLI::colorize("              /////////////////////////////////////////////////////////////\\",         CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("           ///\\__________________________________________________________///\\",       CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("        ///\\_\\/                                                          \\///\\",    CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("       /// /   ///////////\\    ///////\\    /////////\\      ///////\\      /// /",    CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("      /// /   ///\\_______\\/ ///\\____///\\  ///\\____///\\  ///\\____///\\    /// /", CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("     /// /   /// /          \\_\\/   /// / /// /   /// /  \\_\\/   /// /   /// /",      CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("    /// /   /////////\\          ///\\_\\/ /////////\\_\\/       ///\\_\\/   /// /",    CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("   /// /   ///\\_____\\/       ///\\_\\/   ///\\_____\\/       ///\\_\\/     /// /",    CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("  /// /   /// /           ///\\_\\/     /// /           ///\\_\\/       /// /",         CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize(" /// /   /// /         ///////////\\  /// /         ///////////\\    /// /",            CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("/// /    \\_\\/          \\_________\\/  \\_\\/          \\_________\\/   /// /",       CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("\\///\\                                                          ///\\_\\/",            CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize(" \\/////////////////////////////////////////////////////////////\\_\\/",                CLI::FG_LIGHTCYAN) << endl
        << CLI::colorize("  \\___________________________________________________________\\/",                    CLI::FG_LIGHTCYAN) << endl
        << ""                                                                                                                       << endl
            << CLI::colorize("Thanks for using F2P2 (", FG_WHITE)
            << CLI::colorize("v"+((string)VERSION), FG_LIGHTGREEN)
            << CLI::colorize(") !", FG_WHITE)
            << endl
        << ""
            << CLI::colorize("Type ", FG_WHITE)
            << CLI::colorize("f2p2 help", FG_LIGHTYELLOW)
            << CLI::colorize(" for assistance.", FG_WHITE)
            << endl;
}

void help(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    if (list.size() == 0)
    {
        cout
            << CLI::Command::commandsToString(cli.getCommands())                                                                                                             << endl
            << CLI::colorize("Use ", CLI::FG_LIGHTCYAN) << CLI::colorize("f2p2 help command", CLI::FG_LIGHTYELLOW) << CLI::colorize(" to get more help for a command.", CLI::FG_LIGHTCYAN) << endl;
    }
    else
    {
        command = cli.getCommand(list);
        if (command)
        {
            cout << command->toString();
        }
        else
        {
            string command_str;
            for (auto part : list)
            {
                command_str += " "+part;
            }
            cout << CLI::colorize("Unknow command", FG_LIGHTRED) << CLI::colorize(command_str, FG_LIGHTYELLOW) << CLI::colorize(".", FG_LIGHTRED) << endl;
        }
    }
}

void update(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    string version = getLatestCLIVersion();
    if (version.empty() || version == VERSION)
    {
        cout << CLI::colorize("Already up to date ", CLI::FG_LIGHTCYAN) << CLI::colorize("(", CLI::FG_WHITE) << CLI::colorize("v"+((string)VERSION), CLI::FG_LIGHTGREEN) << CLI::colorize(")", CLI::FG_WHITE) << CLI::colorize(".", CLI::FG_LIGHTCYAN) << endl;
    }
    else if (curlToFile(DOWNLOAD_UPDATE_URL+((string)"/")+version, "f2p2-update.exe"))
    {
        rename("f2p2.exe", "f2p2-old.exe");
        rename("f2p2-update.exe", "f2p2.exe");
        cout << CLI::colorize("The f2p2 CLI has been successfully updated to ", CLI::FG_LIGHTGREEN) << CLI::colorize("v"+version, CLI::FG_LIGHTGREEN) << CLI::colorize(".", CLI::FG_LIGHTGREEN) << endl;
        cleanup();
        system("START /B \"\" f2p2.exe cleanup");
        exit(1);
    }
    else
    {
        cout << CLI::colorize("Failed to update the CLI.", CLI::FG_LIGHTRED) << endl;
    }
}

void createProject(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    string name = required.at("name");
    DIR* dir = opendir(name.c_str());
    if (dir)
    {
        closedir(dir);
        cout << CLI::colorize("Unable to create the project because a directory with the same name already exists.", CLI::FG_LIGHTRED) << endl;
    }
    else
    {
        mkdir(name.c_str());
        chdir(name.c_str());
        ofstream ofs = ofstream("./.htaccess");
        ofs << "<IfModule mod_rewrite.c>\n\n    Options +FollowSymLinks\n\n    RewriteEngine on\n\n    RewriteCond %{THE_REQUEST} ^[A-Z]{3,9}\\ /public/\n    RewriteRule ^public/(.*) /$1 [R=302,L]\n\n    RewriteCond %{REQUEST_URI} !^/public/\n    RewriteCond %{DOCUMENT_ROOT}/public/$1 !-d\n    RewriteRule ^(.*)/$ /$1 [R=302,L]\n\n    RewriteCond %{REQUEST_URI} ^/(.*)\n    RewriteRule !^public/ public/%1\n\n</IfModule>";
        ofs.close();
        mkdir("./app");
        mkdir("./app/controllers");
        ofs = ofstream("./app/controllers/WelcomeController.php");
        ofs << "<?php\n    namespace App\\controllers;\n\n\n    class WelcomeController extends \\F2P2\\BaseController\n    {\n        public function index(\n            \\F2P2\\Request $request\n        ) : \\F2P2\\Response\n        {\n            return new \\F2P2\\ResponsePage(\\F2P2\\View::render('welcome.php'));\n        }\n    }\n?>";
        ofs.close();
        mkdir("./app/db");
        mkdir("./app/models");
        mkdir("./app/views");
        ofs = ofstream("./app/views/welcome.php");
        ofs << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>F2P2</title>\n</head>\n<body>\n    <style>\n        body {\n            margin: 0;\n            padding: 0;\n            background-color: #062127;\n        }\n\n        main {\n            height: 100vh;\n            display: flex;\n            flex-direction: column;\n            justify-content: center;\n        }\n\n        pre {\n            font-size: 16px;\n            color: #6ce1e1;\n            width: fit-content;\n            margin: auto;\n        }\n\n        p, a {\n            font-family: sans-serif;\n            font-size: 20px;\n        }\n\n        p {\n            color: #dddddd;\n            text-align: center;\n        }\n\n        a {\n            color: #4183d9;\n            text-decoration: none;\n        }\n    </style>\n    <main>\n        <section>\n            <pre>\n              /////////////////////////////////////////////////////////////\\\n           ///\\__________________________________________________________///\\ \n        ///\\_\\/                                                          \\///\\\n       /// /   ///////////\\    ///////\\    /////////\\      ///////\\      /// /\n      /// /   ///\\_______\\/ ///\\____///\\  ///\\____///\\  ///\\____///\\    /// /\n     /// /   /// /          \\_\\/   /// / /// /   /// /  \\_\\/   /// /   /// /\n    /// /   /////////\\          ///\\_\\/ /////////\\_\\/       ///\\_\\/   /// /\n   /// /   ///\\_____\\/       ///\\_\\/   ///\\_____\\/       ///\\_\\/     /// /\n  /// /   /// /           ///\\_\\/     /// /           ///\\_\\/       /// /\n /// /   /// /         ///////////\\  /// /         ///////////\\    /// /\n/// /    \\_\\/          \\_________\\/  \\_\\/          \\_________\\/   /// /\n\\///\\                                                          ///\\_\\/\n \\/////////////////////////////////////////////////////////////\\_\\/\n  \\___________________________________________________________\\/\n            </pre>\n            <p>For help, see the <a href=\"https://f2p2.thomas-baricault.fr\">documentation</a></p>\n        </section>\n    </main>\n</body>\n</html>";
        ofs.close();
        mkdir("./config");
        ofs = ofstream("./config/db.php");
        ofs << "<?php\n    define('DB_TYPE', 'mysql');\n    define('DB_HOST', 'localhost');\n    define('DB_NAME', 'test');\n    define('DB_USER', 'root');\n    define('DB_PASS', '');\n?>";
        ofs.close();
        ofs = ofstream("./config/env.php");
        ofs << "<?php\n    define('DEFAULT_LANGUAGE', 'en');\n    define('USE_AUTH', false);\n?>";
        ofs.close();
        ofs = ofstream("./config/mime.json");
        ofs << "{\n    \"application/andrew-inset\" : \"ez\",\n    \"application/applixware\" : \"aw\",\n    \"application/atom+xml\" : \"atom\",\n    \"application/atomcat+xml\" : \"atomcat\",\n    \"application/atomsvc+xml\" : \"atomsvc\",\n    \"application/ccxml+xml\" : \"ccxml\",\n    \"application/cdmi-capability\" : \"cdmia\",\n    \"application/cdmi-container\" : \"cdmic\",\n    \"application/cdmi-domain\" : \"cdmid\",\n    \"application/cdmi-object\" : \"cdmio\",\n    \"application/cdmi-queue\" : \"cdmiq\",\n    \"application/cu-seeme\" : \"cu\",\n    \"application/davmount+xml\" : \"davmount\",\n    \"application/docbook+xml\" : \"dbk\",\n    \"application/dssc+der\" : \"dssc\",\n    \"application/dssc+xml\" : \"xdssc\",\n    \"application/ecmascript\" : \"ecma\",\n    \"application/emma+xml\" : \"emma\",\n    \"application/epub+zip\" : \"epub\",\n    \"application/exi\" : \"exi\",\n    \"application/font-tdpfr\" : \"pfr\",\n    \"application/gml+xml\" : \"gml\",\n    \"application/gpx+xml\" : \"gpx\",\n    \"application/gxf\" : \"gxf\",\n    \"application/hyperstudio\" : \"stk\",\n    \"application/inkml+xml\" : [\"ink\", \"inkml\"],\n    \"application/ipfix\" : \"ipfix\",\n    \"application/java-archive\" : \"jar\",\n    \"application/java-serialized-object\" : \"ser\",\n    \"application/java-vm\" : \"class\",\n    \"application/json\" : \"json\",\n    \"application/jsonml+json\" : \"jsonml\",\n    \"application/lost+xml\" : \"lostxml\",\n    \"application/mac-binhex40\" : \"hqx\",\n    \"application/mac-compactpro\" : \"cpt\",\n    \"application/mads+xml\" : \"mads\",\n    \"application/marc\" : \"mrc\",\n    \"application/marcxml+xml\" : \"mrcx\",\n    \"application/mathematica\" : [\"ma\", \"nb\", \"mb\"],\n    \"application/mathml+xml\" : \"mathml\",\n    \"application/mbox\" : \"mbox\",\n    \"application/mediaservercontrol+xml\" : \"mscml\",\n    \"application/metalink+xml\" : \"metalink\",\n    \"application/metalink4+xml\" : \"meta4\",\n    \"application/mets+xml\" : \"mets\",\n    \"application/mods+xml\" : \"mods\",\n    \"application/mp21\" : [\"m21\", \"mp21\"],\n    \"application/mp4\" : \"mp4s\",\n    \"application/msword\" : [\"doc\", \"dot\"],\n    \"application/mxf\" : \"mxf\",\n    \"application/octet-stream\" : [\"bin\", \"dms\", \"lrf\", \"mar\", \"so\", \"dist\", \"distz\", \"pkg\", \"bpk\", \"dump\", \"elc\", \"deploy\"],\n    \"application/oda\" : \"oda\",\n    \"application/oebps-package+xml\" : \"opf\",\n    \"application/ogg\" : \"ogx\",\n    \"application/omdoc+xml\" : \"omdoc\",\n    \"application/onenote\" : [\"onetoc\", \"onetoc2\", \"onetmp\", \"onepkg\"],\n    \"application/oxps\" : \"oxps\",\n    \"application/patch-ops-error+xml\" : \"xer\",\n    \"application/pdf\" : \"pdf\",\n    \"application/pgp-encrypted\" : \"pgp\",\n    \"application/pgp-signature\" : [\"asc\", \"sig\"],\n    \"application/pics-rules\" : \"prf\",\n    \"application/pkcs10\" : \"p10\",\n    \"application/pkcs7-mime\" : [\"p7m\", \"p7c\"],\n    \"application/pkcs7-signature\" : \"p7s\",\n    \"application/pkcs8\" : \"p8\",\n    \"application/pkix-attr-cert\" : \"ac\",\n    \"application/pkix-cert\" : \"cer\",\n    \"application/pkix-crl\" : \"crl\",\n    \"application/pkix-pkipath\" : \"pkipath\",\n    \"application/pkixcmp\" : \"pki\",\n    \"application/pls+xml\" : \"pls\",\n    \"application/postscript\" : [\"ai\", \"eps\", \"ps\"],\n    \"application/prs.cww\" : \"cww\",\n    \"application/pskc+xml\" : \"pskcxml\",\n    \"application/rdf+xml\" : \"rdf\",\n    \"application/reginfo+xml\" : \"rif\",\n    \"application/relax-ng-compact-syntax\" : \"rnc\",\n    \"application/resource-lists+xml\" : \"rl\",\n    \"application/resource-lists-diff+xml\" : \"rld\",\n    \"application/rls-services+xml\" : \"rs\",\n    \"application/rpki-ghostbusters\" : \"gbr\",\n    \"application/rpki-manifest\" : \"mft\",\n    \"application/rpki-roa\" : \"roa\",\n    \"application/rsd+xml\" : \"rsd\",\n    \"application/rss+xml\" : \"rss\",\n    \"application/rtf\" : \"rtf\",\n    \"application/sbml+xml\" : \"sbml\",\n    \"application/scvp-cv-request\" : \"scq\",\n    \"application/scvp-cv-response\" : \"scs\",\n    \"application/scvp-vp-request\" : \"spq\",\n    \"application/scvp-vp-response\" : \"spp\",\n    \"application/sdp\" : \"sdp\",\n    \"application/set-payment-initiation\" : \"setpay\",\n    \"application/set-registration-initiation\" : \"setreg\",\n    \"application/shf+xml\" : \"shf\",\n    \"application/smil+xml\" : [\"smi\", \"smil\"],\n    \"application/sparql-query\" : \"rq\",\n    \"application/sparql-results+xml\" : \"srx\",\n    \"application/srgs\" : \"gram\",\n    \"application/srgs+xml\" : \"grxml\",\n    \"application/sru+xml\" : \"sru\",\n    \"application/ssdl+xml\" : \"ssdl\",\n    \"application/ssml+xml\" : \"ssml\",\n    \"application/tei+xml\" : [\"tei\", \"teicorpus\"],\n    \"application/thraud+xml\" : \"tfi\",\n    \"application/timestamped-data\" : \"tsd\",\n    \"application/vnd.3gpp.pic-bw-large\" : \"plb\",\n    \"application/vnd.3gpp.pic-bw-small\" : \"psb\",\n    \"application/vnd.3gpp.pic-bw-var\" : \"pvb\",\n    \"application/vnd.3gpp2.tcap\" : \"tcap\",\n    \"application/vnd.3m.post-it-notes\" : \"pwn\",\n    \"application/vnd.accpac.simply.aso\" : \"aso\",\n    \"application/vnd.accpac.simply.imp\" : \"imp\",\n    \"application/vnd.acucobol\" : \"acu\",\n    \"application/vnd.acucorp\" : [\"atc\", \"acutc\"],\n    \"application/vnd.adobe.air-application-installer-package+zip\" : \"air\",\n    \"application/vnd.adobe.formscentral.fcdt\" : \"fcdt\",\n    \"application/vnd.adobe.fxp\" : [\"fxp\", \"fxpl\"],\n    \"application/vnd.adobe.xdp+xml\" : \"xdp\",\n    \"application/vnd.adobe.xfdf\" : \"xfdf\",\n    \"application/vnd.ahead.space\" : \"ahead\",\n    \"application/vnd.airzip.filesecure.azf\" : \"azf\",\n    \"application/vnd.airzip.filesecure.azs\" : \"azs\",\n    \"application/vnd.amazon.ebook\" : \"azw\",\n    \"application/vnd.americandynamics.acc\" : \"acc\",\n    \"application/vnd.amiga.ami\" : \"ami\",\n    \"application/vnd.android.package-archive\" : \"apk\",\n    \"application/vnd.anser-web-certificate-issue-initiation\" : \"cii\",\n    \"application/vnd.anser-web-funds-transfer-initiation\" : \"fti\",\n    \"application/vnd.antix.game-component\" : \"atx\",\n    \"application/vnd.apple.installer+xml\" : \"mpkg\",\n    \"application/vnd.apple.mpegurl\" : \"m3u8\",\n    \"application/vnd.aristanetworks.swi\" : \"swi\",\n    \"application/vnd.astraea-software.iota\" : \"iota\",\n    \"application/vnd.audiograph\" : \"aep\",\n    \"application/vnd.blueice.multipass\" : \"mpm\",\n    \"application/vnd.bmi\" : \"bmi\",\n    \"application/vnd.businessobjects\" : \"rep\",\n    \"application/vnd.chemdraw+xml\" : \"cdxml\",\n    \"application/vnd.chipnuts.karaoke-mmd\" : \"mmd\",\n    \"application/vnd.cinderella\" : \"cdy\",\n    \"application/vnd.claymore\" : \"cla\",\n    \"application/vnd.cloanto.rp9\" : \"rp9\",\n    \"application/vnd.clonk.c4group\" : [\"c4g\", \"c4d\", \"c4f\", \"c4p\", \"c4u\"],\n    \"application/vnd.cluetrust.cartomobile-config\" : \"c11amc\",\n    \"application/vnd.cluetrust.cartomobile-config-pkg\" : \"c11amz\",\n    \"application/vnd.commonspace\" : \"csp\",\n    \"application/vnd.contact.cmsg\" : \"cdbcmsg\",\n    \"application/vnd.cosmocaller\" : \"cmc\",\n    \"application/vnd.crick.clicker\" : \"clkx\",\n    \"application/vnd.crick.clicker.keyboard\" : \"clkk\",\n    \"application/vnd.crick.clicker.palette\" : \"clkp\",\n    \"application/vnd.crick.clicker.template\" : \"clkt\",\n    \"application/vnd.crick.clicker.wordbank\" : \"clkw\",\n    \"application/vnd.criticaltools.wbs+xml\" : \"wbs\",\n    \"application/vnd.ctc-posml\" : \"pml\",\n    \"application/vnd.cups-ppd\" : \"ppd\",\n    \"application/vnd.curl.car\" : \"car\",\n    \"application/vnd.curl.pcurl\" : \"pcurl\",\n    \"application/vnd.dart\" : \"dart\",\n    \"application/vnd.data-vision.rdz\" : \"rdz\",\n    \"application/vnd.dece.data\" : [\"uvf\", \"uvvf\", \"uvd\", \"uvvd\"],\n    \"application/vnd.dece.ttml+xml\" : [\"uvt\", \"uvvt\"],\n    \"application/vnd.dece.unspecified\" : [\"uvx\", \"uvvx\"],\n    \"application/vnd.dece.zip\" : [\"uvz\", \"uvvz\"],\n    \"application/vnd.denovo.fcselayout-link\" : \"fe_launch\",\n    \"application/vnd.dna\" : \"dna\",\n    \"application/vnd.dolby.mlp\" : \"mlp\",\n    \"application/vnd.dpgraph\" : \"dpg\",\n    \"application/vnd.dreamfactory\" : \"dfac\",\n    \"application/vnd.ds-keypoint\" : \"kpxx\",\n    \"application/vnd.dvb.ait\" : \"ait\",\n    \"application/vnd.dvb.service\" : \"svc\",\n    \"application/vnd.dynageo\" : \"geo\",\n    \"application/vnd.ecowin.chart\" : \"mag\",\n    \"application/vnd.enliven\" : \"nml\",\n    \"application/vnd.epson.esf\" : \"esf\",\n    \"application/vnd.epson.msf\" : \"msf\",\n    \"application/vnd.epson.quickanime\" : \"qam\",\n    \"application/vnd.epson.salt\" : \"slt\",\n    \"application/vnd.epson.ssf\" : \"ssf\",\n    \"application/vnd.eszigno3+xml\" : [\"es3\", \"et3\"],\n    \"application/vnd.ezpix-album\" : \"ez2\",\n    \"application/vnd.ezpix-package\" : \"ez3\",\n    \"application/vnd.fdf\" : \"fdf\",\n    \"application/vnd.fdsn.mseed\" : \"mseed\",\n    \"application/vnd.fdsn.seed\" : [\"seed\", \"dataless\"],\n    \"application/vnd.flographit\" : \"gph\",\n    \"application/vnd.fluxtime.clip\" : \"ftc\",\n    \"application/vnd.framemaker\" : [\"fm\", \"frame\", \"maker\", \"book\"],\n    \"application/vnd.frogans.fnc\" : \"fnc\",\n    \"application/vnd.frogans.ltf\" : \"ltf\",\n    \"application/vnd.fsc.weblaunch\" : \"fsc\",\n    \"application/vnd.fujitsu.oasys\" : \"oas\",\n    \"application/vnd.fujitsu.oasys2\" : \"oa2\",\n    \"application/vnd.fujitsu.oasys3\" : \"oa3\",\n    \"application/vnd.fujitsu.oasysgp\" : \"fg5\",\n    \"application/vnd.fujitsu.oasysprs\" : \"bh2\",\n    \"application/vnd.fujixerox.ddd\" : \"ddd\",\n    \"application/vnd.fujixerox.docuworks\" : \"xdw\",\n    \"application/vnd.fujixerox.docuworks.binder\" : \"xbd\",\n    \"application/vnd.fuzzysheet\" : \"fzs\",\n    \"application/vnd.genomatix.tuxedo\" : \"txd\",\n    \"application/vnd.geogebra.file\" : \"ggb\",\n    \"application/vnd.geogebra.slides\" : \"ggs\",\n    \"application/vnd.geogebra.tool\" : \"ggt\",\n    \"application/vnd.geometry-explorer\" : [\"gex\", \"gre\"],\n    \"application/vnd.geonext\" : \"gxt\",\n    \"application/vnd.geoplan\" : \"g2w\",\n    \"application/vnd.geospace\" : \"g3w\",\n    \"application/vnd.gmx\" : \"gmx\",\n    \"application/vnd.google-earth.kml+xml\" : \"kml\",\n    \"application/vnd.google-earth.kmz\" : \"kmz\",\n    \"application/vnd.grafeq\" : [\"gqf\", \"gqs\"],\n    \"application/vnd.groove-account\" : \"gac\",\n    \"application/vnd.groove-help\" : \"ghf\",\n    \"application/vnd.groove-identity-message\" : \"gim\",\n    \"application/vnd.groove-injector\" : \"grv\",\n    \"application/vnd.groove-tool-message\" : \"gtm\",\n    \"application/vnd.groove-tool-template\" : \"tpl\",\n    \"application/vnd.groove-vcard\" : \"vcg\",\n    \"application/vnd.hal+xml\" : \"hal\",\n    \"application/vnd.handheld-entertainment+xml\" : \"zmm\",\n    \"application/vnd.hbci\" : \"hbci\",\n    \"application/vnd.hhe.lesson-player\" : \"les\",\n    \"application/vnd.hp-hpgl\" : \"hpgl\",\n    \"application/vnd.hp-hpid\" : \"hpid\",\n    \"application/vnd.hp-hps\" : \"hps\",\n    \"application/vnd.hp-jlyt\" : \"jlt\",\n    \"application/vnd.hp-pcl\" : \"pcl\",\n    \"application/vnd.hp-pclxl\" : \"pclxl\",\n    \"application/vnd.hydrostatix.sof-data\" : \"sfd-hdstx\",\n    \"application/vnd.ibm.minipay\" : \"mpy\",\n    \"application/vnd.ibm.modcap\" : [\"afp\", \"listafp\", \"list3820\"],\n    \"application/vnd.ibm.rights-management\" : \"irm\",\n    \"application/vnd.ibm.secure-container\" : \"sc\",\n    \"application/vnd.iccprofile\" : [\"icc\", \"icm\"],\n    \"application/vnd.igloader\" : \"igl\",\n    \"application/vnd.immervision-ivp\" : \"ivp\",\n    \"application/vnd.immervision-ivu\" : \"ivu\",\n    \"application/vnd.insors.igm\" : \"igm\",\n    \"application/vnd.intercon.formnet\" : [\"xpw\", \"xpx\"],\n    \"application/vnd.intergeo\" : \"i2g\",\n    \"application/vnd.intu.qbo\" : \"qbo\",\n    \"application/vnd.intu.qfx\" : \"qfx\",\n    \"application/vnd.ipunplugged.rcprofile\" : \"rcprofile\",\n    \"application/vnd.irepository.package+xml\" : \"irp\",\n    \"application/vnd.is-xpr\" : \"xpr\",\n    \"application/vnd.isac.fcs\" : \"fcs\",\n    \"application/vnd.jam\" : \"jam\",\n    \"application/vnd.jcp.javame.midlet-rms\" : \"rms\",\n    \"application/vnd.jisp\" : \"jisp\",\n    \"application/vnd.joost.joda-archive\" : \"joda\",\n    \"application/vnd.kahootz\" : [\"ktz\", \"ktr\"],\n    \"application/vnd.kde.karbon\" : \"karbon\",\n    \"application/vnd.kde.kchart\" : \"chrt\",\n    \"application/vnd.kde.kformula\" : \"kfo\",\n    \"application/vnd.kde.kivio\" : \"flw\",\n    \"application/vnd.kde.kontour\" : \"kon\",\n    \"application/vnd.kde.kpresenter\" : [\"kpr\", \"kpt\"],\n    \"application/vnd.kde.kspread\" : \"ksp\",\n    \"application/vnd.kde.kword\" : [\"kwd\", \"kwt\"],\n    \"application/vnd.kenameaapp\" : \"htke\",\n    \"application/vnd.kidspiration\" : \"kia\",\n    \"application/vnd.kinar\" : [\"kne\", \"knp\"],\n    \"application/vnd.koan\" : [\"skp\", \"skd\", \"skt\", \"skm\"],\n    \"application/vnd.kodak-descriptor\" : \"sse\",\n    \"application/vnd.las.las+xml\" : \"lasxml\",\n    \"application/vnd.llamagraphics.life-balance.desktop\" : \"lbd\",\n    \"application/vnd.llamagraphics.life-balance.exchange+xml\" : \"lbe\",\n    \"application/vnd.lotus-1-2-3\" : \"123\",\n    \"application/vnd.lotus-approach\" : \"apr\",\n    \"application/vnd.lotus-freelance\" : \"pre\",\n    \"application/vnd.lotus-notes\" : \"nsf\",\n    \"application/vnd.lotus-organizer\" : \"org\",\n    \"application/vnd.lotus-screencam\" : \"scm\",\n    \"application/vnd.lotus-wordpro\" : \"lwp\",\n    \"application/vnd.macports.portpkg\" : \"portpkg\",\n    \"application/vnd.mcd\" : \"mcd\",\n    \"application/vnd.medcalcdata\" : \"mc1\",\n    \"application/vnd.mediastation.cdkey\" : \"cdkey\",\n    \"application/vnd.mfer\" : \"mwf\",\n    \"application/vnd.mfmp\" : \"mfm\",\n    \"application/vnd.micrografx.flo\" : \"flo\",\n    \"application/vnd.micrografx.igx\" : \"igx\",\n    \"application/vnd.mif\" : \"mif\",\n    \"application/vnd.mobius.daf\" : \"daf\",\n    \"application/vnd.mobius.dis\" : \"dis\",\n    \"application/vnd.mobius.mbk\" : \"mbk\",\n    \"application/vnd.mobius.mqy\" : \"mqy\",\n    \"application/vnd.mobius.msl\" : \"msl\",\n    \"application/vnd.mobius.plc\" : \"plc\",\n    \"application/vnd.mobius.txf\" : \"txf\",\n    \"application/vnd.mophun.application\" : \"mpn\",\n    \"application/vnd.mophun.certificate\" : \"mpc\",\n    \"application/vnd.mozilla.xul+xml\" : \"xul\",\n    \"application/vnd.ms-artgalry\" : \"cil\",\n    \"application/vnd.ms-cab-compressed\" : \"cab\",\n    \"application/vnd.ms-excel\" : [\"xls\", \"xlm\", \"xla\", \"xlc\", \"xlt\", \"xlw\"],\n    \"application/vnd.ms-excel.addin.macroenabled.12\" : \"xlam\",\n    \"application/vnd.ms-excel.sheet.binary.macroenabled.12\" : \"xlsb\",\n    \"application/vnd.ms-excel.sheet.macroenabled.12\" : \"xlsm\",\n    \"application/vnd.ms-excel.template.macroenabled.12\" : \"xltm\",\n    \"application/vnd.ms-fontobject\" : \"eot\",\n    \"application/vnd.ms-htmlhelp\" : \"chm\",\n    \"application/vnd.ms-ims\" : \"ims\",\n    \"application/vnd.ms-lrm\" : \"lrm\",\n    \"application/vnd.ms-officetheme\" : \"thmx\",\n    \"application/vnd.ms-pki.seccat\" : \"cat\",\n    \"application/vnd.ms-pki.stl\" : \"stl\",\n    \"application/vnd.ms-powerpoint\" : [\"ppt\", \"pps\", \"pot\"],\n    \"application/vnd.ms-powerpoint.addin.macroenabled.12\" : \"ppam\",\n    \"application/vnd.ms-powerpoint.presentation.macroenabled.12\" : \"pptm\",\n    \"application/vnd.ms-powerpoint.slide.macroenabled.12\" : \"sldm\",\n    \"application/vnd.ms-powerpoint.slideshow.macroenabled.12\" : \"ppsm\",\n    \"application/vnd.ms-powerpoint.template.macroenabled.12\" : \"potm\",\n    \"application/vnd.ms-project\" : [\"mpp\", \"mpt\"],\n    \"application/vnd.ms-word.document.macroenabled.12\" : \"docm\",\n    \"application/vnd.ms-word.template.macroenabled.12\" : \"dotm\",\n    \"application/vnd.ms-works\" : [\"wps\", \"wks\", \"wcm\", \"wdb\"],\n    \"application/vnd.ms-wpl\" : \"wpl\",\n    \"application/vnd.ms-xpsdocument\" : \"xps\",\n    \"application/vnd.mseq\" : \"mseq\",\n    \"application/vnd.musician\" : \"mus\",\n    \"application/vnd.muvee.style\" : \"msty\",\n    \"application/vnd.mynfc\" : \"taglet\",\n    \"application/vnd.neurolanguage.nlu\" : \"nlu\",\n    \"application/vnd.nitf\" : [\"ntf\", \"nitf\"],\n    \"application/vnd.noblenet-directory\" : \"nnd\",\n    \"application/vnd.noblenet-sealer\" : \"nns\",\n    \"application/vnd.noblenet-web\" : \"nnw\",\n    \"application/vnd.nokia.n-gage.data\" : \"ngdat\",\n    \"application/vnd.nokia.n-gage.symbian.install\" : \"n-gage\",\n    \"application/vnd.nokia.radio-preset\" : \"rpst\",\n    \"application/vnd.nokia.radio-presets\" : \"rpss\",\n    \"application/vnd.novadigm.edm\" : \"edm\",\n    \"application/vnd.novadigm.edx\" : \"edx\",\n    \"application/vnd.novadigm.ext\" : \"ext\",\n    \"application/vnd.oasis.opendocument.chart\" : \"odc\",\n    \"application/vnd.oasis.opendocument.chart-template\" : \"otc\",\n    \"application/vnd.oasis.opendocument.database\" : \"odb\",\n    \"application/vnd.oasis.opendocument.formula\" : \"odf\",\n    \"application/vnd.oasis.opendocument.formula-template\" : \"odft\",\n    \"application/vnd.oasis.opendocument.graphics\" : \"odg\",\n    \"application/vnd.oasis.opendocument.graphics-template\" : \"otg\",\n    \"application/vnd.oasis.opendocument.image\" : \"odi\",\n    \"application/vnd.oasis.opendocument.image-template\" : \"oti\",\n    \"application/vnd.oasis.opendocument.presentation\" : \"odp\",\n    \"application/vnd.oasis.opendocument.presentation-template\" : \"otp\",\n    \"application/vnd.oasis.opendocument.spreadsheet\" : \"ods\",\n    \"application/vnd.oasis.opendocument.spreadsheet-template\" : \"ots\",\n    \"application/vnd.oasis.opendocument.text\" : \"odt\",\n    \"application/vnd.oasis.opendocument.text-master\" : \"odm\",\n    \"application/vnd.oasis.opendocument.text-template\" : \"ott\",\n    \"application/vnd.oasis.opendocument.text-web\" : \"oth\",\n    \"application/vnd.olpc-sugar\" : \"xo\",\n    \"application/vnd.oma.dd2+xml\" : \"dd2\",\n    \"application/vnd.openofficeorg.extension\" : \"oxt\",\n    \"application/vnd.openxmlformats-officedocument.presentationml.presentation\" : \"pptx\",\n    \"application/vnd.openxmlformats-officedocument.presentationml.slide\" : \"sldx\",\n    \"application/vnd.openxmlformats-officedocument.presentationml.slideshow\" : \"ppsx\",\n    \"application/vnd.openxmlformats-officedocument.presentationml.template\" : \"potx\",\n    \"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet\" : \"xlsx\",\n    \"application/vnd.openxmlformats-officedocument.spreadsheetml.template\" : \"xltx\",\n    \"application/vnd.openxmlformats-officedocument.wordprocessingml.document\" : \"docx\",\n    \"application/vnd.openxmlformats-officedocument.wordprocessingml.template\" : \"dotx\",\n    \"application/vnd.osgeo.mapguide.package\" : \"mgp\",\n    \"application/vnd.osgi.dp\" : \"dp\",\n    \"application/vnd.osgi.subsystem\" : \"esa\",\n    \"application/vnd.palm\" : [\"pdb\", \"pqa\", \"oprc\"],\n    \"application/vnd.pawaafile\" : \"paw\",\n    \"application/vnd.pg.format\" : \"str\",\n    \"application/vnd.pg.osasli\" : \"ei6\",\n    \"application/vnd.picsel\" : \"efif\",\n    \"application/vnd.pmi.widget\" : \"wg\",\n    \"application/vnd.pocketlearn\" : \"plf\",\n    \"application/vnd.powerbuilder6\" : \"pbd\",\n    \"application/vnd.previewsystems.box\" : \"box\",\n    \"application/vnd.proteus.magazine\" : \"mgz\",\n    \"application/vnd.publishare-delta-tree\" : \"qps\",\n    \"application/vnd.pvi.ptid1\" : \"ptid\",\n    \"application/vnd.quark.quarkxpress\" : [\"qxd\", \"qxt\", \"qwd\", \"qwt\", \"qxl\", \"qxb\"],\n    \"application/vnd.realvnc.bed\" : \"bed\",\n    \"application/vnd.recordare.musicxml\" : \"mxl\",\n    \"application/vnd.recordare.musicxml+xml\" : \"musicxml\",\n    \"application/vnd.rig.cryptonote\" : \"cryptonote\",\n    \"application/vnd.rim.cod\" : \"cod\",\n    \"application/vnd.rn-realmedia\" : \"rm\",\n    \"application/vnd.rn-realmedia-vbr\" : \"rmvb\",\n    \"application/vnd.route66.link66+xml\" : \"link66\",\n    \"application/vnd.sailingtracker.track\" : \"st\",\n    \"application/vnd.seemail\" : \"see\",\n    \"application/vnd.sema\" : \"sema\",\n    \"application/vnd.semd\" : \"semd\",\n    \"application/vnd.semf\" : \"semf\",\n    \"application/vnd.shana.informed.formdata\" : \"ifm\",\n    \"application/vnd.shana.informed.formtemplate\" : \"itp\",\n    \"application/vnd.shana.informed.interchange\" : \"iif\",\n    \"application/vnd.shana.informed.package\" : \"ipk\",\n    \"application/vnd.simtech-mindmapper\" : [\"twd\", \"twds\"],\n    \"application/vnd.smaf\" : \"mmf\",\n    \"application/vnd.smart.teacher\" : \"teacher\",\n    \"application/vnd.solent.sdkm+xml\" : [\"sdkm\", \"sdkd\"],\n    \"application/vnd.spotfire.dxp\" : \"dxp\",\n    \"application/vnd.spotfire.sfs\" : \"sfs\",\n    \"application/vnd.stardivision.calc\" : \"sdc\",\n    \"application/vnd.stardivision.draw\" : \"sda\",\n    \"application/vnd.stardivision.impress\" : \"sdd\",\n    \"application/vnd.stardivision.math\" : \"smf\",\n    \"application/vnd.stardivision.writer\" : [\"sdw\", \"vor\"],\n    \"application/vnd.stardivision.writer-global\" : \"sgl\",\n    \"application/vnd.stepmania.package\" : \"smzip\",\n    \"application/vnd.stepmania.stepchart\" : \"sm\",\n    \"application/vnd.sun.xml.calc\" : \"sxc\",\n    \"application/vnd.sun.xml.calc.template\" : \"stc\",\n    \"application/vnd.sun.xml.draw\" : \"sxd\",\n    \"application/vnd.sun.xml.draw.template\" : \"std\",\n    \"application/vnd.sun.xml.impress\" : \"sxi\",\n    \"application/vnd.sun.xml.impress.template\" : \"sti\",\n    \"application/vnd.sun.xml.math\" : \"sxm\",\n    \"application/vnd.sun.xml.writer\" : \"sxw\",\n    \"application/vnd.sun.xml.writer.global\" : \"sxg\",\n    \"application/vnd.sun.xml.writer.template\" : \"stw\",\n    \"application/vnd.sus-calendar\" : [\"sus\", \"susp\"],\n    \"application/vnd.svd\" : \"svd\",\n    \"application/vnd.symbian.install\" : [\"sis\", \"sisx\"],\n    \"application/vnd.syncml+xml\" : \"xsm\",\n    \"application/vnd.syncml.dm+wbxml\" : \"bdm\",\n    \"application/vnd.syncml.dm+xml\" : \"xdm\",\n    \"application/vnd.tao.intent-module-archive\" : \"tao\",\n    \"application/vnd.tcpdump.pcap\" : [\"pcap\", \"cap\", \"dmp\"],\n    \"application/vnd.tmobile-livetv\" : \"tmo\",\n    \"application/vnd.trid.tpt\" : \"tpt\",\n    \"application/vnd.triscape.mxs\" : \"mxs\",\n    \"application/vnd.trueapp\" : \"tra\",\n    \"application/vnd.ufdl\" : [\"ufd\", \"ufdl\"],\n    \"application/vnd.uiq.theme\" : \"utz\",\n    \"application/vnd.umajin\" : \"umj\",\n    \"application/vnd.unity\" : \"unityweb\",\n    \"application/vnd.uoml+xml\" : \"uoml\",\n    \"application/vnd.vcx\" : \"vcx\",\n    \"application/vnd.visio\" : [\"vsd\", \"vst\", \"vss\", \"vsw\"],\n    \"application/vnd.visionary\" : \"vis\",\n    \"application/vnd.vsf\" : \"vsf\",\n    \"application/vnd.wap.wbxml\" : \"wbxml\",\n    \"application/vnd.wap.wmlc\" : \"wmlc\",\n    \"application/vnd.wap.wmlscriptc\" : \"wmlsc\",\n    \"application/vnd.webturbo\" : \"wtb\",\n    \"application/vnd.wolfram.player\" : \"nbp\",\n    \"application/vnd.wordperfect\" : \"wpd\",\n    \"application/vnd.wqd\" : \"wqd\",\n    \"application/vnd.wt.stf\" : \"stf\",\n    \"application/vnd.xara\" : \"xar\",\n    \"application/vnd.xfdl\" : \"xfdl\",\n    \"application/vnd.yamaha.hv-dic\" : \"hvd\",\n    \"application/vnd.yamaha.hv-script\" : \"hvs\",\n    \"application/vnd.yamaha.hv-voice\" : \"hvp\",\n    \"application/vnd.yamaha.openscoreformat\" : \"osf\",\n    \"application/vnd.yamaha.openscoreformat.osfpvg+xml\" : \"osfpvg\",\n    \"application/vnd.yamaha.smaf-audio\" : \"saf\",\n    \"application/vnd.yamaha.smaf-phrase\" : \"spf\",\n    \"application/vnd.yellowriver-custom-menu\" : \"cmp\",\n    \"application/vnd.zul\" : [\"zir\", \"zirz\"],\n    \"application/vnd.zzazz.deck+xml\" : \"zaz\",\n    \"application/voicexml+xml\" : \"vxml\",\n    \"application/wasm\" : \"wasm\",\n    \"application/widget\" : \"wgt\",\n    \"application/winhlp\" : \"hlp\",\n    \"application/wsdl+xml\" : \"wsdl\",\n    \"application/wspolicy+xml\" : \"wspolicy\",\n    \"application/x-7z-compressed\" : \"7z\",\n    \"application/x-abiword\" : \"abw\",\n    \"application/x-ace-compressed\" : \"ace\",\n    \"application/x-apple-diskimage\" : \"dmg\",\n    \"application/x-authorware-bin\" : [\"aab\", \"x32\", \"u32\", \"vox\"],\n    \"application/x-authorware-map\" : \"aam\",\n    \"application/x-authorware-seg\" : \"aas\",\n    \"application/x-bcpio\" : \"bcpio\",\n    \"application/x-bittorrent\" : \"torrent\",\n    \"application/x-blorb\" : [\"blb\", \"blorb\"],\n    \"application/x-bzip\" : \"bz\",\n    \"application/x-bzip2\" : [\"bz2\", \"boz\"],\n    \"application/x-cbr\" : [\"cbr\", \"cba\", \"cbt\", \"cbz\", \"cb7\"],\n    \"application/x-cdlink\" : \"vcd\",\n    \"application/x-cfs-compressed\" : \"cfs\",\n    \"application/x-chat\" : \"chat\",\n    \"application/x-chess-pgn\" : \"pgn\",\n    \"application/x-conference\" : \"nsc\",\n    \"application/x-cpio\" : \"cpio\",\n    \"application/x-csh\" : \"csh\",\n    \"application/x-debian-package\" : [\"deb\", \"udeb\"],\n    \"application/x-dgc-compressed\" : \"dgc\",\n    \"application/x-director\" : [\"dir\", \"dcr\", \"dxr\", \"cst\", \"cct\", \"cxt\", \"w3d\", \"fgd\", \"swa\"],\n    \"application/x-doom\" : \"wad\",\n    \"application/x-dtbncx+xml\" : \"ncx\",\n    \"application/x-dtbook+xml\" : \"dtb\",\n    \"application/x-dtbresource+xml\" : \"res\",\n    \"application/x-dvi\" : \"dvi\",\n    \"application/x-envoy\" : \"evy\",\n    \"application/x-eva\" : \"eva\",\n    \"application/x-font-bdf\" : \"bdf\",\n    \"application/x-font-ghostscript\" : \"gsf\",\n    \"application/x-font-linux-psf\" : \"psf\",\n    \"application/x-font-pcf\" : \"pcf\",\n    \"application/x-font-snf\" : \"snf\",\n    \"application/x-font-type1\" : [\"pfa\", \"pfb\", \"pfm\", \"afm\"],\n    \"application/x-freearc\" : \"arc\",\n    \"application/x-futuresplash\" : \"spl\",\n    \"application/x-gca-compressed\" : \"gca\",\n    \"application/x-glulx\" : \"ulx\",\n    \"application/x-gnumeric\" : \"gnumeric\",\n    \"application/x-gramps-xml\" : \"gramps\",\n    \"application/x-gtar\" : \"gtar\",\n    \"application/x-hdf\" : \"hdf\",\n    \"application/x-install-instructions\" : \"install\",\n    \"application/x-iso9660-image\" : \"iso\",\n    \"application/x-java-jnlp-file\" : \"jnlp\",\n    \"application/x-latex\" : \"latex\",\n    \"application/x-lzh-compressed\" : [\"lzh\", \"lha\"],\n    \"application/x-mie\" : \"mie\",\n    \"application/x-mobipocket-ebook\" : [\"prc\", \"mobi\"],\n    \"application/x-ms-application\" : \"application\",\n    \"application/x-ms-shortcut\" : \"lnk\",\n    \"application/x-ms-wmd\" : \"wmd\",\n    \"application/x-ms-wmz\" : \"wmz\",\n    \"application/x-ms-xbap\" : \"xbap\",\n    \"application/x-msaccess\" : \"mdb\",\n    \"application/x-msbinder\" : \"obd\",\n    \"application/x-mscardfile\" : \"crd\",\n    \"application/x-msclip\" : \"clp\",\n    \"application/x-msdownload\" : [\"exe\", \"dll\", \"com\", \"bat\", \"msi\"],\n    \"application/x-msmediaview\" : [\"mvb\", \"m13\", \"m14\"],\n    \"application/x-msmetafile\" : [\"wmf\", \"wmz\", \"emf\", \"emz\"],\n    \"application/x-msmoney\" : \"mny\",\n    \"application/x-mspublisher\" : \"pub\",\n    \"application/x-msschedule\" : \"scd\",\n    \"application/x-msterminal\" : \"trm\",\n    \"application/x-mswrite\" : \"wri\",\n    \"application/x-netcdf\" : [\"nc\", \"cdf\"],\n    \"application/x-nzb\" : \"nzb\",\n    \"application/x-pkcs12\" : [\"p12\", \"pfx\"],\n    \"application/x-pkcs7-certificates\" : [\"p7b\", \"spc\"],\n    \"application/x-pkcs7-certreqresp\" : \"p7r\",\n    \"application/x-rar-compressed\" : \"rar\",\n    \"application/x-research-info-systems\" : \"ris\",\n    \"application/x-sh\" : \"sh\",\n    \"application/x-shar\" : \"shar\",\n    \"application/x-shockwave-flash\" : \"swf\",\n    \"application/x-silverlight-app\" : \"xap\",\n    \"application/x-sql\" : \"sql\",\n    \"application/x-stuffit\" : \"sit\",\n    \"application/x-stuffitx\" : \"sitx\",\n    \"application/x-subrip\" : \"srt\",\n    \"application/x-sv4cpio\" : \"sv4cpio\",\n    \"application/x-sv4crc\" : \"sv4crc\",\n    \"application/x-t3vm-image\" : \"t3\",\n    \"application/x-tads\" : \"gam\",\n    \"application/x-tar\" : \"tar\",\n    \"application/x-tcl\" : \"tcl\",\n    \"application/x-tex\" : \"tex\",\n    \"application/x-tex-tfm\" : \"tfm\",\n    \"application/x-texinfo\" : [\"texinfo\", \"texi\"],\n    \"application/x-tgif\" : \"obj\",\n    \"application/x-ustar\" : \"ustar\",\n    \"application/x-wais-source\" : \"src\",\n    \"application/x-x509-ca-cert\" : [\"der\", \"crt\"],\n    \"application/x-xfig\" : \"fig\",\n    \"application/x-xliff+xml\" : \"xlf\",\n    \"application/x-xpinstall\" : \"xpi\",\n    \"application/x-xz\" : \"xz\",\n    \"application/x-zmachine\" : [\"z1\", \"z2\", \"z3\", \"z4\", \"z5\", \"z6\", \"z7\", \"z8\"],\n    \"application/xaml+xml\" : \"xaml\",\n    \"application/xcap-diff+xml\" : \"xdf\",\n    \"application/xenc+xml\" : \"xenc\",\n    \"application/xhtml+xml\" : [\"xhtml\", \"xht\"],\n    \"application/xml\" : [\"xml\", \"xsl\"],\n    \"application/xml-dtd\" : \"dtd\",\n    \"application/xop+xml\" : \"xop\",\n    \"application/xproc+xml\" : \"xpl\",\n    \"application/xslt+xml\" : \"xslt\",\n    \"application/xspf+xml\" : \"xspf\",\n    \"application/xv+xml\" : [\"mxml\", \"xhvml\", \"xvml\", \"xvm\"],\n    \"application/yang\" : \"yang\",\n    \"application/yin+xml\" : \"yin\",\n    \"application/zip\" : \"zip\",\n    \"audio/adpcm\" : \"adp\",\n    \"audio/basic\" : [\"au\", \"snd\"],\n    \"audio/midi\" : [\"mid\", \"midi\", \"kar\", \"rmi\"],\n    \"audio/mp4\" : [\"m4a\", \"mp4a\"],\n    \"audio/mpeg\" : [\"mpga\", \"mp2\", \"mp2a\", \"mp3\", \"m2a\", \"m3a\"],\n    \"audio/ogg\" : [\"oga\", \"ogg\", \"spx\", \"opus\"],\n    \"audio/s3m\" : \"s3m\",\n    \"audio/silk\" : \"sil\",\n    \"audio/vnd.dece.audio\" : [\"uva\", \"uvva\"],\n    \"audio/vnd.digital-winds\" : \"eol\",\n    \"audio/vnd.dra\" : \"dra\",\n    \"audio/vnd.dts\" : \"dts\",\n    \"audio/vnd.dts.hd\" : \"dtshd\",\n    \"audio/vnd.lucent.voice\" : \"lvp\",\n    \"audio/vnd.ms-playready.media.pya\" : \"pya\",\n    \"audio/vnd.nuera.ecelp4800\" : \"ecelp4800\",\n    \"audio/vnd.nuera.ecelp7470\" : \"ecelp7470\",\n    \"audio/vnd.nuera.ecelp9600\" : \"ecelp9600\",\n    \"audio/vnd.rip\" : \"rip\",\n    \"audio/webm\" : \"weba\",\n    \"audio/x-aac\" : \"aac\",\n    \"audio/x-aiff\" : [\"aif\", \"aiff\", \"aifc\"],\n    \"audio/x-caf\" : \"caf\",\n    \"audio/x-flac\" : \"flac\",\n    \"audio/x-matroska\" : \"mka\",\n    \"audio/x-mpegurl\" : \"m3u\",\n    \"audio/x-ms-wax\" : \"wax\",\n    \"audio/x-ms-wma\" : \"wma\",\n    \"audio/x-pn-realaudio\" : [\"ram\", \"ra\"],\n    \"audio/x-pn-realaudio-plugin\" : \"rmp\",\n    \"audio/x-wav\" : \"wav\",\n    \"audio/xm\" : \"xm\",\n    \"chemical/x-cdx\" : \"cdx\",\n    \"chemical/x-cif\" : \"cif\",\n    \"chemical/x-cmdf\" : \"cmdf\",\n    \"chemical/x-cml\" : \"cml\",\n    \"chemical/x-csml\" : \"csml\",\n    \"chemical/x-xyz\" : \"xyz\",\n    \"font/collection\" : \"ttc\",\n    \"font/otf\" : \"otf\",\n    \"font/ttf\" : \"ttf\",\n    \"font/woff\" : \"woff\",\n    \"font/woff2\" : \"woff2\",\n    \"image/bmp\" : \"bmp\",\n    \"image/cgm\" : \"cgm\",\n    \"image/g3fax\" : \"g3\",\n    \"image/gif\" : \"gif\",\n    \"image/ief\" : \"ief\",\n    \"image/jpeg\" : [\"jpeg\", \"jpg\", \"jpe\"],\n    \"image/ktx\" : \"ktx\",\n    \"image/png\" : \"png\",\n    \"image/prs.btif\" : \"btif\",\n    \"image/sgi\" : \"sgi\",\n    \"image/svg+xml\" : [\"svg\", \"svgz\"],\n    \"image/tiff\" : [\"tiff\", \"tif\"],\n    \"image/vnd.adobe.photoshop\" : \"psd\",\n    \"image/vnd.dece.graphic\" : [\"uvi\", \"uvvi\", \"uvg\", \"uvvg\"],\n    \"image/vnd.djvu\" : [\"djvu\", \"djv\"],\n    \"image/vnd.dvb.subtitle\" : \"sub\",\n    \"image/vnd.dwg\" : \"dwg\",\n    \"image/vnd.dxf\" : \"dxf\",\n    \"image/vnd.fastbidsheet\" : \"fbs\",\n    \"image/vnd.fpx\" : \"fpx\",\n    \"image/vnd.fst\" : \"fst\",\n    \"image/vnd.fujixerox.edmics-mmr\" : \"mmr\",\n    \"image/vnd.fujixerox.edmics-rlc\" : \"rlc\",\n    \"image/vnd.ms-modi\" : \"mdi\",\n    \"image/vnd.ms-photo\" : \"wdp\",\n    \"image/vnd.net-fpx\" : \"npx\",\n    \"image/vnd.wap.wbmp\" : \"wbmp\",\n    \"image/vnd.xiff\" : \"xif\",\n    \"image/webp\" : \"webp\",\n    \"image/x-3ds\" : \"3ds\",\n    \"image/x-cmu-raster\" : \"ras\",\n    \"image/x-cmx\" : \"cmx\",\n    \"image/x-freehand\" : [\"fh\", \"fhc\", \"fh4\", \"fh5\", \"fh7\"],\n    \"image/x-icon\" : \"ico\",\n    \"image/x-mrsid-image\" : \"sid\",\n    \"image/x-pcx\" : \"pcx\",\n    \"image/x-pict\" : [\"pic\", \"pct\"],\n    \"image/x-portable-anymap\" : \"pnm\",\n    \"image/x-portable-bitmap\" : \"pbm\",\n    \"image/x-portable-graymap\" : \"pgm\",\n    \"image/x-portable-pixmap\" : \"ppm\",\n    \"image/x-rgb\" : \"rgb\",\n    \"image/x-tga\" : \"tga\",\n    \"image/x-xbitmap\" : \"xbm\",\n    \"image/x-xpixmap\" : \"xpm\",\n    \"image/x-xwindowdump\" : \"xwd\",\n    \"message/rfc822\" : [\"eml\", \"mime\"],\n    \"model/iges\" : [\"igs\", \"iges\"],\n    \"model/mesh\" : [\"msh\", \"mesh\", \"silo\"],\n    \"model/vnd.collada+xml\" : \"dae\",\n    \"model/vnd.dwf\" : \"dwf\",\n    \"model/vnd.gdl\" : \"gdl\",\n    \"model/vnd.gtw\" : \"gtw\",\n    \"model/vnd.vtu\" : \"vtu\",\n    \"model/vrml\" : [\"wrl\", \"vrml\"],\n    \"model/x3d+binary\" : [\"x3db\", \"x3dbz\"],\n    \"model/x3d+vrml\" : [\"x3dv\", \"x3dvz\"],\n    \"model/x3d+xml\" : [\"x3d\", \"x3dz\"],\n    \"text/cache-manifest\" : \"appcache\",\n    \"text/calendar\" : [\"ics\", \"ifb\"],\n    \"text/css\" : \"css\",\n    \"text/csv\" : \"csv\",\n    \"text/html\" : [\"html\", \"htm\"],\n    \"text/javascript\" : [\"js\", \"mjs\"],\n    \"text/n3\" : \"n3\",\n    \"text/plain\" : [\"txt\", \"text\", \"conf\", \"def\", \"list\", \"log\", \"in\"],\n    \"text/prs.lines.tag\" : \"dsc\",\n    \"text/richtext\" : \"rtx\",\n    \"text/sgml\" : [\"sgml\", \"sgm\"],\n    \"text/tab-separated-values\" : \"tsv\",\n    \"text/troff\" : [\"t\", \"tr\", \"roff\", \"man\", \"me\", \"ms\"],\n    \"text/turtle\" : \"ttl\",\n    \"text/uri-list\" : [\"uri\", \"uris\", \"urls\"],\n    \"text/vcard\" : \"vcard\",\n    \"text/vnd.curl\" : \"curl\",\n    \"text/vnd.curl.dcurl\" : \"dcurl\",\n    \"text/vnd.curl.mcurl\" : \"mcurl\",\n    \"text/vnd.curl.scurl\" : \"scurl\",\n    \"text/vnd.dvb.subtitle\" : \"sub\",\n    \"text/vnd.fly\" : \"fly\",\n    \"text/vnd.fmi.flexstor\" : \"flx\",\n    \"text/vnd.graphviz\" : \"gv\",\n    \"text/vnd.in3d.3dml\" : \"3dml\",\n    \"text/vnd.in3d.spot\" : \"spot\",\n    \"text/vnd.sun.j2me.app-descriptor\" : \"jad\",\n    \"text/vnd.wap.wml\" : \"wml\",\n    \"text/vnd.wap.wmlscript\" : \"wmls\",\n    \"text/x-asm\" : [\"s\", \"asm\"],\n    \"text/x-c\" : [\"c\", \"cc\", \"cxx\", \"cpp\", \"h\", \"hh\", \"dic\"],\n    \"text/x-fortran\" : [\"f\", \"for\", \"f77\", \"f90\"],\n    \"text/x-java-source\" : \"java\",\n    \"text/x-nfo\" : \"nfo\",\n    \"text/x-opml\" : \"opml\",\n    \"text/x-pascal\" : [\"p\", \"pas\"],\n    \"text/x-setext\" : \"etx\",\n    \"text/x-sfv\" : \"sfv\",\n    \"text/x-uuencode\" : \"uu\",\n    \"text/x-vcalendar\" : \"vcs\",\n    \"text/x-vcard\" : \"vcf\",\n    \"video/3gpp\" : \"3gp\",\n    \"video/3gpp2\" : \"3g2\",\n    \"video/h261\" : \"h261\",\n    \"video/h263\" : \"h263\",\n    \"video/h264\" : \"h264\",\n    \"video/jpeg\" : \"jpgv\",\n    \"video/jpm\" : [\"jpm\", \"jpgm\"],\n    \"video/mj2\" : [\"mj2\", \"mjp2\"],\n    \"video/mp2t\" : [\"ts\", \"m2t\", \"m2ts\", \"mts\"],\n    \"video/mp4\" : [\"mp4\", \"mp4v\", \"mpg4\"],\n    \"video/mpeg\" : [\"mpeg\", \"mpg\", \"mpe\", \"m1v\", \"m2v\"],\n    \"video/ogg\" : \"ogv\",\n    \"video/quicktime\" : [\"qt\", \"mov\"],\n    \"video/vnd.dece.hd\" : [\"uvh\", \"uvvh\"],\n    \"video/vnd.dece.mobile\" : [\"uvm\", \"uvvm\"],\n    \"video/vnd.dece.pd\" : [\"uvp\", \"uvvp\"],\n    \"video/vnd.dece.sd\" : [\"uvs\", \"uvvs\"],\n    \"video/vnd.dece.video\" : [\"uvv\", \"uvvv\"],\n    \"video/vnd.dvb.file\" : \"dvb\",\n    \"video/vnd.fvt\" : \"fvt\",\n    \"video/vnd.mpegurl\" : [\"mxu\", \"m4u\"],\n    \"video/vnd.ms-playready.media.pyv\" : \"pyv\",\n    \"video/vnd.uvvu.mp4\" : [\"uvu\", \"uvvu\"],\n    \"video/vnd.vivo\" : \"viv\",\n    \"video/webm\" : \"webm\",\n    \"video/x-f4v\" : \"f4v\",\n    \"video/x-fli\" : \"fli\",\n    \"video/x-flv\" : \"flv\",\n    \"video/x-m4v\" : \"m4v\",\n    \"video/x-matroska\" : [\"mkv\", \"mk3d\", \"mks\"],\n    \"video/x-mng\" : \"mng\",\n    \"video/x-ms-asf\" : [\"asf\", \"asx\"],\n    \"video/x-ms-vob\" : \"vob\",\n    \"video/x-ms-wm\" : \"wm\",\n    \"video/x-ms-wmv\" : \"wmv\",\n    \"video/x-ms-wmx\" : \"wmx\",\n    \"video/x-ms-wvx\" : \"wvx\",\n    \"video/x-msvideo\" : \"avi\",\n    \"video/x-sgi-movie\" : \"movie\",\n    \"video/x-smv\" : \"smv\",\n    \"x-conference/x-cooltalk\" : \"ice\"\n}";
        ofs.close();
        ofs = ofstream("./config/routes.json");
        ofs << "[\n    {\n        \"name\"       : \"welcome\",\n        \"method\"     : \"GET\",\n        \"uri\"        : \"/\",\n        \"controller\" : \"Welcome\",\n        \"action\"     : \"index\"\n    }\n]";
        ofs.close();
        mkdir("./f2p2");
        ofs = ofstream("./f2p2/autoload.php");
        ofs << "<?php\n    function loadDir(\n        string $path,\n        array  $ignore = []\n    ) : void\n    {\n        if (is_dir($path))\n        {\n            foreach (scandir($path) as $file)\n            {\n                if ($file !== '.' && $file !== '..' && !in_array($file, $ignore))\n                {\n                    loadDir(\"$path/$file\", $ignore);\n                }\n            }\n        }\n        else\n        {\n            require_once($path);\n        }\n    }\n\n\n    loadDir('../f2p2', ['autoload.php']);\n    loadDir('../app/models');\n?>";
        ofs.close();
        mkdir("./f2p2/db");
        ofs = ofstream("./f2p2/db/Database.php");
        ofs << "<?php \n    namespace F2P2;\n\n\n    /**\n     * A class to interact with database\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Database\n    {\n        static private ?\\PDO $pdo = null;\n\n\n        /**\n         * Execute a query on the database\n         * \n         * @param  string $query  The query\n         * @param  array  $values The values for the prepare\n         * @return array          The result of the query\n         */\n        static public function execute(\n            string $query,\n            array  $values\n        ) : array\n        {\n            if (self::$pdo === null)\n            {\n                try\n                {\n                    switch (DB_TYPE)\n                    {\n                        case 'sqlite': self::$pdo = new \\PDO(DB_TYPE.':'.DB_NAME);                                           break;\n                        case 'oci':    self::$pdo = new \\PDO(DB_TYPE.'dbname='.DB_NAME, DB_USER, DB_PASS);                   break;\n                        default:       self::$pdo = new \\PDO(DB_TYPE.':host='.DB_HOST.';dbname='.DB_NAME, DB_USER, DB_PASS); break;\n                    }\n                }\n                catch (\\Throwable $th)\n                {\n                    die($th->getMessage());\n                }\n            }\n            $stmt = self::$pdo->prepare($query);\n            $stmt->execute($values);\n            return $stmt->fetchAll(\\PDO::FETCH_ASSOC);\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/db/Query.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The base class for the database query representations\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class Query\n    {\n        protected string $table;\n\n        protected array $values = [];\n\n\n        /**\n         * @param string $table The table on which the query will be execute\n         */\n        public function __construct(\n            string $table\n        ) {\n            $this->table = $table;\n        }\n\n        /**\n         * Defines the prepare values\n         * \n         * @param  array $values The values\n         * @return Query The query\n         */\n        public function setValues(\n            array $values\n        ) : Query\n        {\n            $this->values = $values;\n            return $this;\n        }\n\n        /**\n         * Execute the query\n         * \n         * @return array The result of the query\n         */\n        abstract public function execute() : array;\n    }\n\n    /**\n     * The class that represent an insert query\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class QueryInsert extends Query\n    {\n        public function execute() : array\n        {\n            $table = $this->table;\n            $columns = [];\n            $placeholders = [];\n            foreach ($this->values as $column => $value)\n            {\n                $columns[] = $column;\n                $placeholders[] = \":$column\";\n            }\n            $columns = implode(', ', $columns);\n            $placeholders = implode(', ', $placeholders);\n            return \\F2P2\\Database::execute(\n                \"INSERT INTO $table ($columns) VALUES ($placeholders);\",\n                $this->values\n            );\n        }\n    }\n\n\n    /**\n     * The class that represent a select query\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class QuerySelect extends Query\n    {\n        private ?string $where   = null;\n        private ?array  $orderBy = null;\n        private ?array  $groupBy = null;\n        private ?int    $limit   = null;\n        private ?int    $offset  = null;\n\n\n        public function execute() : array\n        {\n            $table = $this->table;\n            $where = $this->where ? 'WHERE '.$this->where : '';\n            $orderBy = '';\n            if ($this->orderBy)\n            {\n                $orderBy = [];\n                foreach ($this->orderBy as $column => $order)\n                {\n                    $orderBy[] = \"$column $order\";\n                }\n                $orderBy = 'ORDER BY '.implode(', ', $orderBy);\n            }\n            $groupBy = $this->groupBy ? 'GROUP BY '.implode(', ', $this->groupBy) : '';\n            $limit = $this->limit ? 'LIMIT '.$this->limit : '';\n            $offset = $this->offset ? 'OFFSET '.$this->offset : '';\n            return \\F2P2\\Database::execute(\n                \"SELECT * FROM $table $where $orderBy $groupBy $limit $offset;\",\n                $this->values\n            );\n        }\n\n        /**\n         * Defines the where clause\n         * \n         * @param  string      $where The where clause\n         * @return QuerySelect        The query\n         */\n        public function setWhere(\n            string $where\n        ) : QuerySelect\n        {\n            $this->where = $where;\n            return $this;\n        }\n\n        /**\n         * Defines the order by clause\n         * \n         * @param  array       $orderBy An array of columns\n         * @return QuerySelect          The query\n         */\n        public function setOrderBy(\n            array $orderBy\n        ) : QuerySelect\n        {\n            $this->orderBy = $orderBy;\n            return $this;\n        }\n\n        /**\n         * Defines the group by clause\n         * \n         * @param  array       $groupBy An array of columns\n         * @return QuerySelect          The query\n         */\n        public function setGroupBy(\n            array $groupBy\n        ) : QuerySelect\n        {\n            $this->groupBy = $groupBy;\n            return $this;\n        }\n\n        /**\n         * Defines the limit clause\n         * \n         * @param  int         $limit The limit\n         * @return QuerySelect        The query\n         */\n        public function setLimit(\n            int $limit\n        ) : QuerySelect\n        {\n            $this->limit = $limit;\n            return $this;\n        }\n\n        /**\n         * Defines the offset clause\n         * \n         * @param  int         $offset The offset\n         * @return QuerySelect         The query\n         */\n        public function setOffset(\n            int $offset\n        ) : QuerySelect\n        {\n            $this->offset = $offset;\n            return $this;\n        }\n    }\n\n\n    /**\n     * The class that represent an update query\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class QueryUpdate extends Query\n    {\n        private ?array  $update = null;\n        private ?string $where  = null;\n\n\n        public function execute() : array\n        {\n            $table = $this->table;\n            $update = '';\n            if ($this->update)\n            {\n                $update = [];\n                foreach ($this->update as $column => $value)\n                {\n                    $update[] = \"$column = $value\";\n                }\n                $update = 'SET '.implode(', ', $update);\n            }\n            $where = $this->where ? 'WHERE '.$this->where : '';\n            return \\F2P2\\Database::execute(\n                \"UPDATE $table $update $where;\",\n                $this->values\n            );\n        }\n\n        /**\n         * Defines the offset clause\n         * \n         * @param  array       $update An array of modifications (column => value)\n         * @return QueryUpdate         The query\n         */\n        public function setUpdate(\n            array $update\n        ) : QueryUpdate\n        {\n            $this->update = $update;\n            return $this;\n        }\n\n        /**\n         * Defines the where clause\n         * \n         * @param  string      $where The where clause\n         * @return QueryUpdate        The query\n         */\n        public function setWhere(\n            string $where\n        ) : QueryUpdate\n        {\n            $this->where = $where;\n            return $this;\n        }\n    }\n\n\n    /**\n     * The class that represent a delete query\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class QueryDelete extends Query\n    {\n        private ?string $where = null;\n\n\n        public function execute() : array\n        {\n            $table = $this->table;\n            $where = $this->where ? 'WHERE '.$this->where : '';\n            return \\F2P2\\Database::execute(\n                \"DELETE FROM $table $where;\",\n                $this->values\n            );\n        }\n\n        /**\n         * Defines the where clause\n         * \n         * @param  string      $where The where clause\n         * @return QueryDelete        The query\n         */\n        public function setWhere(\n            string $where\n        ) : QueryDelete\n        {\n            $this->where = $where;\n            return $this;\n        }\n    }\n?>";
        ofs.close();
        mkdir("./f2p2/mvc");
        ofs = ofstream("./f2p2/mvc/BaseController.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that all controllers inherit from\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class BaseController\n    {\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/mvc/BaseModel.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that all models inherit from\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class BaseModel\n    {\n        public const TABLE   = '';\n        public const COLUMNS = [];\n\n        protected ?int $id = null;\n\n        protected array $data = [];\n\n\n        /**\n         * @param array $data Initials values of the entity (column => value)\n         */\n        public function __construct(\n            array $data = []\n        ) {\n            if (array_key_exists('id', $data))\n            {\n                $this->id = $data['id'];\n            }\n            foreach (get_class($this)::COLUMNS as $name => $params)\n            {\n                if (array_key_exists($name, $data))\n                {\n                    switch ($params['type'])\n                    {\n                        case 'SET': $this->data[$name] = explode(',', $data[$name]); break;\n                        default:    $this->data[$name] = $data[$name];\n                    }                    \n                }\n                else if (array_key_exists('default', $params))\n                {\n                    $this->data[$name] = $params['default'];\n                }\n                else\n                {\n                    $this->data[$name] = null;\n                }\n            }\n        }\n\n        /**\n         * Returns the entity id\n         * \n         * @return ?int The id\n         */\n        public function getId() : ?int\n        {\n            return $this->id;\n        }\n\n        /**\n         * Returns if the entity is valid according to the table constraints\n         * \n         * @return bool If valid true else false\n         */\n        public function isValid() : bool\n        {\n            foreach (get_class($this)::COLUMNS as $name => $params)\n            {\n                if (\n                    array_key_exists('null', $params) && $params['null'] !== true && $this->data[$name] === null ||\n                    array_key_exists('size', $params) && in_array($params['type'], ['CHAR', 'VARCHAR', 'BINARY', 'VARBINARY']) && strlen($this->data[$name]) > $params['size']\n                ) {\n                    return false;\n                }\n            }\n            return true;\n        }\n\n        /**\n         * Push the entity to the database (update if already exists)\n         * \n         * @return void\n         */\n        public function push() : void\n        {\n            if (!$this->isValid())\n            {\n                throw new \\Exception(\"Cannot push invalid element\");\n            }\n            $values = [];\n            foreach ($this->data as $column => $value)\n            {\n                $values[$column] = get_class($this)::COLUMNS[$column]['type'] === 'SET' ? implode(',', $value) : $value;\n            }\n            if ($this->id)\n            {\n                $update = [];\n                foreach ($this->data as $column => $value)\n                {\n                    $update[$column] = \":$column\";\n                }\n                $values['id'] = $this->id;\n                (new \\F2P2\\QueryUpdate(get_class($this)::TABLE))\n                    ->setUpdate($update)\n                    ->setWhere('id = :id')\n                    ->setValues($values)\n                ->execute();\n            }\n            else\n            {\n                (new \\F2P2\\QueryInsert(get_class($this)::TABLE))\n                    ->setValues($values)\n                ->execute();\n            }\n        }\n\n        /**\n         * Remove the entity from the database (the entity must have a valid id)\n         * \n         * @param  bool $cascading If true, automatically remove entities that reference this entity\n         * @return void\n         */\n        public function remove(\n            bool $cascading = false\n        ) : void\n        {\n            if ($this->id === null)\n            {\n                throw new \\Exception(\"Cannot remove undefined element from the database\");\n            }\n            $class = get_class($this);\n            if ($cascading)\n            {\n                foreach (get_declared_classes() as $model)\n                {\n                    if (is_subclass_of($model, BaseModel::class))\n                    {\n                        foreach ($model::COLUMNS as $name => $params)\n                        {\n                            if (array_key_exists('reference', $params) && $params['reference'] === $class)\n                            {\n                                $data = (new \\F2P2\\QuerySelect($model::TABLE))\n                                    ->setWhere($name.' = :id')\n                                    ->setValues(['id' => $this->id])\n                                ->execute();\n                                foreach ($data as $entry)\n                                {\n                                    (new $model($entry))->remove();\n                                }\n                            }\n                        }\n                    }\n                }\n            }\n            (new \\F2P2\\QueryDelete($class::TABLE))\n                ->setWhere('id = :id')\n                ->setValues(['id' => $this->id])\n            ->execute();\n        }\n\n        /**\n         * Find one entity by id\n         * \n         * @param  int     $id The entity id\n         * @return ?object     The entity if found, otherwise null\n         */\n        static public function findOne(\n            int    $id\n        ) : ?object\n        {\n            $data = (new \\F2P2\\QuerySelect(get_called_class()::TABLE))\n                ->setWhere('id = :id')\n                ->setValues(['id' => $id])\n            ->execute();\n            return count($data) === 0 ? null : new (get_called_class())($data[0]);\n        }\n\n        /**\n         * Retourne toutes les entités de la table\n         * \n         * @return array The entities\n         */\n        static public function findAll() : array\n        {\n            $data = (new \\F2P2\\QuerySelect(get_called_class()::TABLE))->execute();\n            $result = [];\n            foreach ($data as $entry)\n            {\n                $result[] = new (get_called_class())($entry);\n            }\n            return $result;\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/mvc/View.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * An useful class for the views\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class View\n    {\n        /**\n         * Render a view\n         * \n         * @param  string $name   The name of the view\n         * @param  array  $params An array of values that can be called in the view file\n         * @return string         The html result\n         */\n        static public function render(\n            string $name,\n            array  $params = []\n        ) : string\n        {\n            ob_start();\n            require('../app/views/'.$name);\n            return ob_get_clean();\n        }\n\n        /**\n         * Returns the URL to an asset\n         * \n         * @param  string $name The name of the asset\n         * @return string       The URL\n         */\n        static public function asset(\n            string $name\n        ) : string\n        {\n            return \\F2P2\\Server::getBaseURL().'/'.$name;\n        }\n\n        /**\n         * Returns the URL to a CSS file\n         * \n         * @param  string $name The name of the CSS file\n         * @return string       The URL\n         */\n        static public function css(\n            string $name\n        ) : string\n        {\n            return self::asset(\"css/$name\");\n        }\n    \n        /**\n         * Returns the URL to a Javascript file\n         * \n         * @param  string $name The name of the Javascript file\n         * @return string       The URL\n         */\n        static public function js(\n            string $name\n        ) : string\n        {\n            return self::asset(\"js/$name\");\n        }\n    \n        /**\n         * Returns the URL to an image\n         * \n         * @param  string $name The name of the image\n         * @return string       The URL\n         */\n        static public function img(\n            string $name\n        ) : string\n        {\n            return self::asset(\"img/$name\");\n        }\n    \n        /**\n         * Returns the URL of a route\n         * \n         * @param  string $name   The name of the route\n         * @param  array  $params The array of the route required parameters\n         * @param  array  $get    An array of get values\n         * @return string         The URL\n         */\n        static public function route(\n            string $name,\n            array  $params = [],\n            array  $get    = []\n        ) : string\n        {\n            return \\F2P2\\Router::getRouteURL($name, $params, $get);\n        }\n    }\n?>";
        ofs.close();
        mkdir("./f2p2/requests");
        ofs = ofstream("./f2p2/requests/Request.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that represent a network request\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Request\n    {\n        private ?string $error            = null;\n        private ?string $url              = null;\n        private string  $method           = 'GET';\n        private array   $headers          = [];\n        private int     $port             = 80;\n        private array   $args             = [];\n        private array   $files            = [];\n        private ?int    $timeout          = null;\n        private ?int    $transferMinSpeed = null;\n        private ?int    $transferMinTime  = null;\n        private ?int    $transferResume   = null;\n        private ?int    $downloadMaxSize  = null;\n        private ?int    $downloadMaxSpeed = null;\n        private ?string $downloadPath     = null;\n        private ?int    $uploadSize       = null;\n        private ?int    $uploadMaxSpeed   = null;\n        private ?string $uploadPath       = null;\n\n\n        /**\n         * Initialize a request from the actual session\n         * \n         * @return Request The request\n         */\n        static public function fromCurrent() : Request\n        {\n            $request = (new Request())\n                ->setUrl(\\F2P2\\Server::getCurrentURI())\n                ->setMethod(\\F2P2\\Server::getRequestMethod())\n                ->setPort(\\F2P2\\Server::getServerPort())\n                ->setHeaders(getallheaders())\n                ->setArguments(\\F2P2\\Server::getRequestMethod() === 'GET' ? $_GET : (\\F2P2\\Server::getRequestMethod() === 'POST' ? $_POST : []));\n            foreach ($_FILES as $name => $file)\n            {\n                $request->addFile($file['tmp_name'], $file['type'], $name);\n            }\n            return $request;\n        }\n\n        /**\n         * Returns the error generated when the request was sent\n         * \n         * @return string The error if there is one, otherwise null\n         */\n        public function getError() : ?string\n        {\n            return $this->error;\n        }\n\n        /**\n         * Returns the request URL\n         * \n         * @return string The URL\n         */\n        public function getURL() : ?string\n        {\n            return $this->url;\n        }\n\n        /**\n         * Defines the request URL\n         * \n         * @param  string  $url The URL\n         * @return Request      The request\n         */\n        public function setURL(\n            string $url\n        ) : Request\n        {\n            $this->url = $url;\n            return $this;\n        }\n\n        /**\n         * Returns the request method\n         * \n         * @return string The method\n         */\n        public function getMethod() : string\n        {\n            return $this->method;\n        }\n\n        /**\n         * Defines the request method\n         * \n         * @param  string  $method The method\n         * @return Request         The request\n         */\n        public function setMethod(\n            string $method\n        ) : Request\n        {\n            $this->method = $method;\n            return $this;\n        }\n\n        /**\n         * Returns the request port\n         * \n         * @return string The port\n         */\n        public function getPort() : int\n        {\n            return $this->port;\n        }\n\n        /**\n         * Defines the request port\n         * \n         * @param  string  $url The URL\n         * @return Request      The request\n         */\n        public function setPort(\n            int $port\n        ) : Request\n        {\n            $this->port = $port;\n            return $this;\n        }\n\n        /**\n         * Returns the request headers\n         * \n         * @return array The headers (name => value)\n         */\n        public function getHeaders() : array\n        {\n            return $this->headers;\n        }\n\n        /**\n         * Returns a request header\n         * \n         * @param  string  $name The header name\n         * @return ?string       The header value if exists, otherwise null\n         */\n        public function getHeader(\n            string $name\n        ) : ?string\n        {\n            return array_key_exists($name, $this->headers) ? $this->headers[$name] : null;\n        }\n\n        /**\n         * Defines the request headers\n         * \n         * @param  array   $headers The headers (name => value)\n         * @return Request          The request\n         */\n        public function setHeaders(\n            array $headers\n        ) : Request\n        {\n            foreach ($headers as $name => $value)\n            {\n                $this->addHeader($name, $value);\n            }\n            return $this;\n        }\n\n        /**\n         * Add a header to the request\n         * \n         * @param  string       $name  The header name\n         * @param  string|array $value The header value\n         * @return Request             The request\n         */\n        public function addHeader(\n            string       $name,\n            string|array $value\n        ) : Request\n        {\n            $this->headers[$name] = is_array($value) ? implode('; ', $value) : $value;\n            return $this;\n        }\n\n        /**\n         * Returns all the request data\n         * \n         * @return array The data\n         */\n        public function getData() : array\n        {\n            return array_merge($this->args, $this->files);\n        }\n\n        /**\n         * Returns the request arguments\n         * \n         * @return array The arguments\n         */\n        public function getArguments() : array\n        {\n            return $this->args;\n        }\n\n        /**\n         * Returns a request argument\n         * \n         * @param  string  $name The argument name\n         * @return ?string       The argument value if exists, otherwise null\n         */\n        public function getArgument(\n            string $name\n        ) : ?string\n        {\n            return array_key_exists($name, $this->args) ? $this->args[$name] : null;\n        }\n\n        /**\n         * Defines the GET or POST arguments of the request\n         * \n         * @param  array   $args The arguments (name => value)\n         * @return Request       The request\n         */\n        public function setArguments(\n            array $args\n        ) : Request\n        {\n            foreach ($args as $name => $value)\n            {\n                $this->addArgument($name, $value);\n            }\n            return $this;\n        }\n\n        /**\n         * Add a GET or POST argument to the request\n         * \n         * @param  string  $name  The argument name\n         * @param  string  $value The argument value\n         * @return Request        The request\n         */\n        public function addArgument(\n            string $name,\n            string $value\n        ) : Request\n        {\n            $this->args[$name] = $value;\n            return $this;\n        }\n\n        /**\n         * Returns the request files\n         * \n         * @return array The files\n         */\n        public function getFiles() : array\n        {\n            return $this->files;\n        }\n\n        /**\n         * Returns a request file\n         * \n         * @param  string  $name The file name\n         * @return ?string       The file if exists, otherwise null\n         */\n        public function getFile(\n            string $name\n        ) : ?string\n        {\n            return array_key_exists($name, $this->files) ? $this->files[$name] : null;\n        }\n\n        /**\n         * Add a file to the request\n         * \n         * @param  string  $path   The path to the file if there is one, otherwise the content\n         * @param  ?string $mime   The mime type of the file\n         * @param  ?string $name   The download name of the file\n         * @param  bool    $isFile true if the path refers to a file\n         * @return Request         The request\n         */\n        public function addFile(\n            string  $path,\n            ?string $mime,\n            ?string $name,\n            bool    $isFile = true\n        ) : Request\n        {\n            $this->files[] = $isFile ? new \\CURLFile($path, $mime, $name) : new \\CURLStringFile($path, $mime, $name);\n            return $this;\n        }\n\n        /**\n         * Defines the request timeout\n         * \n         * @param  int     $timeout The timeout (ms)\n         * @return Request          The request\n         */\n        public function setTimeout(\n            int $timeout\n        ) : Request\n        {\n            $this->timeout = $timeout;\n            return $this;\n        }\n\n        /**\n         * Defines the file transfer min speed\n         * \n         * @param  int     $speed The speed (o/s)\n         * @param  ?int    $time  The duration of the verification (s)\n         * @return Request        The request\n         */\n        public function setTransferMinSpeed(\n            int  $speed,\n            ?int $time  = null\n        ) : Request\n        {\n            $this->transferMinSpeed = $speed;\n            $this->transferMinTime = $time;\n            return $this;\n        }\n\n        /**\n         * Defines the download max speed\n         * \n         * @param  int     $speed The speed (o/s)\n         * @return Request        The request\n         */\n        public function setDownloadMaxSpeed(\n            int $speed\n        ) : Request\n        {\n            $this->downloadMaxSpeed = $speed;\n            return $this;\n        }\n\n        /**\n         * Defines the upload max speed\n         * \n         * @param  int     $speed The speed (o/s)\n         * @return Request        The request\n         */\n        public function setUploadMaxSpeed(\n            int $speed\n        ) : Request\n        {\n            $this->uploadMaxSpeed = $speed;\n            return $this;\n        }\n\n        /**\n         * Defines the download max size\n         * \n         * @param  int     $size The size (o)\n         * @return Request       The request\n         */\n        public function setDownloadMaxSize(\n            int $size\n        ) : Request\n        {\n            $this->downloadMaxSize = $size;\n            return $this;\n        }\n\n        /**\n         * Defines the upload size\n         * \n         * @param  int     $size The size (o)\n         * @return Request       The request\n         */\n        public function setUploadSize(\n            int $size\n        ) : Request\n        {\n            $this->uploadSize = $size;\n            return $this;\n        }\n\n        /**\n         * Defines the position of the transfer resume\n         * \n         * @param  int     $position The position (o)\n         * @return Request           The request\n         */\n        public function setTransferResume(\n            int $position\n        ) : Request\n        {\n            $this->transferResume = $position;\n            return $this;\n        }\n\n        /**\n         * Defines where the file of the transfer will be downloaded\n         * \n         * @param  string  $path The path\n         * @return Request       The request\n         */\n        public function setDownloadPath(\n            string $path\n        ) : Request\n        {\n            $this->downloadPath = $path;\n            return $this;\n        }\n\n        /**\n         * Defines the path of the file that will be transfered\n         * \n         * @param  string  $path The path of the file\n         * @return Request       The request\n         */\n        public function setUploadPath(\n            string $path\n        ) : Request\n        {\n            $this->uploadPath = $path;\n            return $this;\n        }\n\n        /**\n         * Send the request\n         * \n         * @return ?\\F2P2\\Response The response received if success, otherwise null\n         */\n        public function send() : ?\\F2P2\\Response\n        {\n            if ($this->url === null)\n            {\n                throw new \\Exception('Cannot send the request because URL is not defined');\n            }\n            $curl = curl_init($this->url.($this->method === 'GET' ? http_build_query($this->args, '?') : ''));\n            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);\n            if ($this->downloadPath === null)\n            {\n                curl_setopt($curl, CURLOPT_HEADER, true);\n            }\n            curl_setopt($curl, CURLOPT_CUSTOMREQUEST, $this->method);\n            curl_setopt($curl, CURLOPT_PORT, $this->port);\n            $headers = [];\n            foreach ($this->headers as $key => $value)\n            {\n                $headers[] = \"$key: $value\";\n            }\n            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);\n            if ($this->method === 'POST')\n            {\n                curl_setopt($curl, CURLOPT_POSTFIELDS, $this->getData());\n            }\n            foreach ([\n                CURLOPT_TIMEOUT_MS           => $this->timeout,\n                CURLOPT_LOW_SPEED_LIMIT      => $this->transferMinSpeed,\n                CURLOPT_LOW_SPEED_TIME       => $this->transferMinTime,\n                CURLOPT_RESUME_FROM          => $this->transferResume,\n                CURLOPT_MAXFILESIZE_LARGE    => $this->downloadMaxSize,\n                CURLOPT_MAX_RECV_SPEED_LARGE => $this->downloadMaxSpeed,\n                CURLOPT_INFILE               => $this->uploadSize,\n                CURLOPT_MAX_SEND_SPEED_LARGE => $this->uploadMaxSpeed,\n            ] as $option => $value)\n            {\n                if ($value)\n                {\n                    curl_setopt($curl, $option, $value);\n                }\n            }\n            $downloadFile = $this->downloadPath ? fopen($this->downloadPath, 'w') : null;\n            if ($downloadFile)\n            {\n                curl_setopt($curl, CURLOPT_FILE, $downloadFile);\n            }\n            $uploadFile = $this->uploadPath ? fopen($this->uploadPath, 'r') : null;\n            if ($uploadFile)\n            {\n                curl_setopt($curl, CURLOPT_INFILE, $uploadFile);\n            }\n            $responseTXT = curl_exec($curl);\n            $this->error = curl_error($curl);\n            if ($downloadFile)\n            {\n                fclose($downloadFile);\n            }\n            if ($uploadFile)\n            {\n                fclose($uploadFile);\n            }\n            if ($this->error === '')\n            {\n                $this->error = null;\n                $headerSize = curl_getinfo($curl, CURLINFO_HEADER_SIZE);\n                $response = (new \\F2P2\\Response())\n                    ->setCode(curl_getinfo($curl, CURLINFO_HTTP_CODE))\n                    ->setBody(substr($responseTXT, $headerSize));\n                foreach (explode(\"\\n\", substr($responseTXT, 0, $headerSize)) as $header)\n                {\n                    $parts = explode(': ', $header);\n                    if (count($parts) === 2)\n                    {\n                        $response->addHeader($parts[0], $parts[1]);\n                    }\n                }\n                curl_close($curl);\n                return $response;\n            }\n            else\n            {\n                return null;\n            }\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/requests/Response.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that represent a network response\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Response\n    {\n        private int     $code    = 200;\n        private array   $headers = [];\n        private ?string $body    = null;\n\n\n        /**\n         * Returns the response status code\n         * \n         * @return int The status code\n         */\n        public function getCode() : int\n        {\n            return $this->code;\n        }\n\n        /**\n         * Defines the response status code\n         * \n         * @param  int      $code The code\n         * @return Response       The response\n         */\n        public function setCode(\n            int $code\n        ) : Response\n        {\n            $this->code = $code;\n            return $this;\n        }\n\n        /**\n         * Returns the response headers\n         * \n         * @return array The headers (name => value)\n         */\n        public function getHeaders() : array\n        {\n            return $this->headers;\n        }\n\n        /**\n         * Returns a response header\n         * \n         * @param  string  $name The header name\n         * @return ?string       The header value if exists, otherwise null\n         */\n        public function getHeader(\n            string $name\n        ) : ?string\n        {\n            return array_key_exists($name, $this->headers) ? $this->headers[$name] : null;\n        }\n\n        /**\n         * Defines the response headers\n         * \n         * @param  array    $headers The headers (name => value)\n         * @return Response          The response\n         */\n        public function setHeaders(\n            array $headers\n        ) : Response\n        {\n            foreach ($headers as $name => $value)\n            {\n                $this->addHeader($name, $value);\n            }\n            return $this;\n        }\n\n        /**\n         * Add a header to the response\n         * \n         * @param  string       $name  The header name\n         * @param  string|array $value The header value\n         * @return Response            The response\n         */\n        public function addHeader(\n            string       $name,\n            string|array $value\n        ) : Response\n        {\n            $this->headers[$name] = is_array($value) ? implode('; ', $value) : $value;\n            return $this;\n        }\n\n        /**\n         * Returns the response body content\n         * \n         * @return ?string The data\n         */\n        public function getBody() : ?string\n        {\n            return $this->code;\n        }\n\n        /**\n         * Defines the response body content\n         * \n         * @param  string   $body The data\n         * @return Response       The response\n         */\n        public function setBody(\n            string $body\n        ) : Response\n        {\n            $this->body = $body;\n            return $this;\n        }\n\n        /**\n         * Send the response\n         * \n         * @return void\n         */\n        public function send() : void\n        {\n            http_response_code($this->code);\n            foreach ($this->headers as $key => $value)\n            {\n                header(\"$key: $value\");\n            }\n            if ($this->body)\n            {\n                echo $this->body;\n            }\n            die();\n        }\n    }\n\n\n    /**\n     * The base class for data responses\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class ResponseData extends Response\n    {\n        /**\n         * @param string  $data      The data\n         * @param ?string $extension The extension defining the type of data\n         * @param bool    $isFile    true if the content of $data is a path to a file\n         */\n        public function __construct(\n            string  $data,\n            ?string $extension = null,\n            bool    $isFile    = false\n        ) {\n            if ($isFile && $extension === null)\n            {\n                $parts = explode('.', $data);\n                $extension = $parts[count($parts)-1];\n            }\n            $this\n                ->addHeader('Content-Type', [($extension ? \\F2P2\\extensionToMIME($extension) : null) ?? 'text/html', 'charset=utf-8'])\n                ->setBody($isFile ? file_get_contents($data) : $data);\n        }\n    }\n\n\n    /**\n     * The class that represent responses displayed as a webpage\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class ResponsePage extends ResponseData\n    {\n        public function __construct(\n            string  $data,\n            ?string $extension = null,\n            bool    $isFile    = false\n        ) {\n            parent::__construct($data, $extension, $isFile);\n            $this->addHeader('Content-Disposition', 'inline');\n        }\n    }\n\n\n    /**\n     * The class that represent responses that are downloads\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class ResponseDownload extends ResponseData\n    {\n        public function __construct(\n            string  $data,\n            ?string $extension = null,\n            bool    $isFile    = false,\n            ?string $fileName  = null\n        ) {\n            parent::__construct($data, $extension, $isFile);\n            if ($fileName)\n            {\n                $fileName = $fileName;\n            }\n            else if ($isFile)\n            {\n                $parts = explode('/', $data);\n                $fileName = $parts[count($parts)-1];\n            }\n            $this->addHeader('Content-Disposition', 'attachment'.($fileName ? '; filename=\"'.$fileName.'\"' : ''));\n        }\n    }\n\n\n    /**\n     * The class that represent a http error response\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class ResponseError extends Response\n    {\n        /**\n         * @param int $code The error code\n         */\n        public function __construct(\n            int $code\n        ) {\n            $this->setCode($code);\n        }\n    }\n\n\n    /**\n     * The class that represent a redirection to an URL\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class ResponseRedirectURL extends Response\n    {\n        /**\n         * @param string $url The redirection URL\n         */\n        public function __construct(\n            string $url\n        ) {\n            $this\n                ->setCode(302)\n                ->addHeader('Location', $url);\n        }\n    }\n\n\n    /**\n     * The class that represent a redirection to a route\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class ResponseRedirectRoute extends ResponseRedirectURL\n    {\n        /**\n         * @param string $route The redirection route name\n         */\n        public function __construct(\n            string $route,\n            array  $params = [],\n            array  $get    = []\n        ) {\n            parent::__construct(\\F2P2\\Router::getRouteURL($route, $params, $get));\n        }\n    }\n?>";
        ofs.close();
        mkdir("./f2p2/utils");
        ofs = ofstream("./f2p2/utils/EMail.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that can generate and send an email\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class EMail\n    {\n        private ?string $from_email = null;\n        private ?string $from_name  = null;\n        private array   $to         = [];\n        private string  $subject    = '';\n        private string  $message    = '';\n        private array   $headers    = [];\n        private bool    $isHTML     = false;\n\n\n        /**\n         * Defines the sender of the email\n         * \n         * @param  string  $email The email of the sender\n         * @param  ?string $name  The name of the sender\n         * @return EMail          The email\n         */\n        public function from(\n            string  $email,\n            ?string $name  = null\n        ) : EMail\n        {\n            $this->from_email = $email;\n            $this->from_name = $name;\n            return $this;\n        }\n\n        /**\n         * Add a recipient to the mail\n         * \n         * @param  string  $email The email of the recipient\n         * @param  ?string $name  The name of the recipient\n         * @return EMail          The email\n         */\n        public function to(\n            string  $email,\n            ?string $name  = null\n        ) : EMail\n        {\n            $this->to[$email] = $name;\n            return $this;\n        }\n\n        /**\n         * Defines the subject of the email\n         * \n         * @param  string $subject The subject\n         * @return EMail           The email\n         */\n        public function subject(\n            string $subject\n        ) : EMail\n        {\n            $this->subject = $subject;\n            return $this;\n        }\n\n        /**\n         * Defines the message of the email\n         * \n         * @param  string $message The message\n         * @return EMail           The email\n         */\n        public function message(\n            string $message\n        ) : EMail\n        {\n            $this->message = $message;\n            return $this;\n        }\n\n        /**\n         * Add a header to the email\n         * \n         * @param  string $name  The header name\n         * @param  string $value The header value\n         * @return EMail         The email\n         */\n        public function header(\n            string $name,\n            string $value\n        ) : EMail\n        {\n            $this->headers[$name] = $value;\n            return $this;\n        }\n\n        /**\n         * Enable HTML for the email\n         * \n         * @param  string $message The message\n         * @return EMail           The email\n         */\n        public function enableHTML() : EMail\n        {\n            $this->isHTML = true;\n            return $this;\n        }\n\n        /**\n         * Send the email\n         * \n         * @return bool true if the email was sent successfully, otherwise false\n         */\n        public function send() : bool\n        {\n            $headers = $this->headers;\n            if ($this->isHTML)\n            {\n                $headers['MIME-Version'] = '1.0';\n                $headers['Content-Type'] = 'text/html; charset=utf-8';\n            }\n            else\n            {\n                $headers['Content-Type'] = 'text/plain; charset=utf-8';\n            }\n            if ($this->from_email)\n            {\n                $headers['From'] = $this->from_name ? $this->from_name.' <'.$this->from_email.'>' : $this->from_email;\n            }\n            if (count($this->to) > 0)\n            {\n                $to = [];\n                foreach ($this->to as $email => $name)\n                {\n                    $to[] = $name ? $name.' <'.$email.'>' : $email;\n                }\n                $headers['To'] = implode(', ', $to);\n            }\n            return mail(implode(', ', array_keys($this->to)), $this->subject, $this->message, $headers);\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/utils/Form.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The base class for all form objects\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class FormObject\n    {\n        protected array $attrs    = [];\n\n\n        /**\n         * Add an attribute to the object\n         * \n         * @param  string $name  The attribute name\n         * @param  string $value The attribute value\n         * @return object        The form object\n         */\n        public function addAttribute(\n            string $name,\n            string $value\n        ) : object\n        {\n            $this->attrs[$name] = $value;\n            return $this;\n        }\n\n        /**\n         * Render the object\n         * \n         * @return string The HTML generated\n         */\n        abstract public function render() : string;\n\n        /**\n         * Render the object attributes\n         * \n         * @return string The HTML element attributes list generated\n         */\n        protected function renderAttributes() : string\n        {\n            $result = [];\n            foreach ($this->attrs as $name => $value)\n            {\n                $result[] = \"$name=\\\"$value\\\"\";\n            }\n            return implode(' ', $result);\n        }\n    }\n\n\n    /**\n     * The base class for form fields\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class Field extends FormObject\n    {\n        protected ?string $name     = null;\n        protected ?string $id       = null;\n        protected string  $type;\n        protected ?string $label    = null;\n        protected bool    $required = false;\n        protected ?string $value    = null;\n\n\n        /**\n         * @param string  $name The field name\n         * @param ?string $id   The field id\n         */\n        public function __construct(\n            string  $name,\n            ?string $id = null\n        ) {\n            $this->name = $name;\n            $this->id = $id ?? $name;\n        }\n\n        /**\n         * Check if the field value is valid\n         * \n         * @return bool true if valid, otherwise false\n         */\n        public function isValid() : bool\n        {\n            return !$this->required || $this->value;\n        }\n\n        /**\n         * Returns the field name\n         * \n         * @return ?string The field name\n         */\n        public function getName() : ?string\n        {\n            return $this->name;\n        }\n\n        /**\n         * Returns the field value\n         * \n         * @return ?string The field value\n         */\n        public function getValue() : string\n        {\n            return $this->value;\n        }\n\n        /**\n         * Defines the field value\n         * \n         * @param  ?string $value The value\n         * @return object         The field\n         */\n        public function setValue(\n            ?string $value\n        ) : object\n        {\n            $this->value = $value === '' ? null : $value;\n            return $this;\n        }\n\n        /**\n         * Defines the field label\n         * \n         * @param  string $label The label\n         * @return object        The field\n         */\n        public function setLabel(\n            string $label\n        ) : object\n        {\n            $this->label = $label;\n            return $this;\n        }\n\n        /**\n         * Make this field required\n         * \n         * @return object The field\n         */\n        public function setRequired() : object\n        {\n            $this->required = true;\n            return $this;\n        }\n\n        public function render() : string\n        {\n            return $this->renderLabel().'<input '.$this->renderAttributes().(' type=\"'.$this->type.'\"').' value=\"'.$this->value.'\">';\n        }\n\n        /**\n         * Render the field label\n         * \n         * @return string The HTML generated\n         */\n        protected function renderLabel() : string\n        {\n            return ($this->label ? '<label'.($this->id ? 'for=\"'.$this->id.'\"' : '').'>'.$this->label.'</label>' : '');\n        }\n        \n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().($this->id ? ' id=\"'.$this->id.'\"' : '').($this->name ? ' name=\"'.$this->name.'\"' : '').($this->required ? ' required' : '');\n        }\n    }\n\n\n    /**\n     * The base class for fields that need a value length verification\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class FieldTextVerifyLength extends Field\n    {\n        protected ?int    $minLength = null;\n        protected ?int    $maxLength = null;\n\n\n        public function isValid() : bool\n        {\n            return parent::isValid() && ($this->minLength === null || strlen($this->value) >= $this->minLength) && ($this->maxLength === null || strlen($this->value) <= $this->maxLength);\n        }\n\n        /**\n         * Defines the field value minimum length\n         * \n         * @param  int    $minLength The length\n         * @return object            The field\n         */\n        public function setMinLength(\n            int $minLength\n        ) : object\n        {\n            $this->minLength = $minLength;\n            return $this;\n        }\n\n        /**\n         * Defines the field value maximum length\n         * \n         * @param  int    $maxLength The length\n         * @return object            The field\n         */\n        public function setMaxLength(\n            int $maxLength\n        ) : object\n        {\n            $this->maxLength = $maxLength;\n            return $this;\n        }\n\n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().($this->minLength ? ' minlength=\"'.$this->minLength.'\"' : '').($this->maxLength ? ' maxlength=\"'.$this->maxLength.'\"' : '');\n        }\n    }\n\n\n    /**\n     * The base class for fields that need a value pattern verification\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class FieldTextVerifyPattern extends FieldTextVerifyLength\n    {\n        protected ?string $pattern   = null;\n\n\n        public function isValid() : bool\n        {\n            return parent::isValid() && ($this->pattern === null || preg_match('/'.$this->pattern.'/', $this->value));\n        }\n\n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().($this->pattern ? ' pattern=\"'.$this->pattern.'\"' : '');\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type text\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldText extends FieldTextVerifyPattern\n    {\n        protected string  $type = 'text';\n    }\n\n\n    /**\n     * The class that represent a input of type email\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldEmail extends FieldTextVerifyPattern\n    {\n        protected string $type = 'email';\n    }\n\n\n    /**\n     * The class that represent a input of type password\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldPassword extends FieldTextVerifyPattern\n    {\n        protected string $type = 'password';\n    }\n\n\n    /**\n     * The class that represent a input of type search\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldSearch extends FieldTextVerifyPattern\n    {\n        protected string $type = 'search';\n    }\n\n\n    /**\n     * The class that represent a input of type tel\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldTel extends FieldTextVerifyPattern\n    {\n        protected string $type = 'tel';\n    }\n\n\n    /**\n     * The class that represent a input of type url\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldURL extends FieldTextVerifyPattern\n    {\n        protected string $type = 'url';\n    }\n\n\n    /**\n     * The class that represent a textarea\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldTextArea extends FieldTextVerifyPattern\n    {\n        public function render() : string\n        {\n            return $this->renderLabel().'<textarea '.$this->renderAttributes().'>'.$this->value.'</textarea>';\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type number\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldNumber extends Field\n    {\n        protected string $type = 'number';\n        protected ?int   $min  = null;\n        protected ?int   $max  = null;\n        protected ?int   $step = null;\n\n\n        public function isValid() : bool\n        {\n            return parent::isValid() && ($this->min === null || $this->value >= $this->min) && ($this->max === null || $this->value <= $this->max) && ($this->step === null || ($this->min && ($this->value-$this->min)%$this->step === 0 || $this->min === null && ($this->value)%$this->step === 0));\n        }\n\n        /**\n         * Defines the field minimum value\n         * \n         * @param  int    $min The value\n         * @return object      The field\n         */\n        public function setMin(\n            int $min\n        ) : object\n        {\n            $this->min = $min;\n            return $this;\n        }\n\n        /**\n         * Defines the field maximum value\n         * \n         * @param  int    $max The value\n         * @return object      The field\n         */\n        public function setMax(\n            int $max\n        ) : object\n        {\n            $this->max = $max;\n            return $this;\n        }\n\n        /**\n         * Defines the field value step\n         * \n         * @param  int    $step The step\n         * @return object       The field\n         */\n        public function setStep(\n            int $step\n        ) : object\n        {\n            if ($step <= 0)\n            {\n                throw new \\Exception('Step must be greater than 0');\n            }\n            $this->step = $step;\n            return $this;\n        }\n\n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().($this->min ? ' min=\"'.$this->min.'\"' : '').($this->max ? ' max=\"'.$this->max.'\"' : '').($this->step ? ' step=\"'.$this->step.'\"' : '');\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type range\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldRange extends FieldNumber\n    {\n        protected string $type     = 'range';\n        protected bool   $required = true;\n    }\n\n\n    /**\n     * The class that represent a input of type color\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldColor extends Field\n    {\n        protected string $type     = 'color';\n        protected bool   $required = true;\n\n\n        public function isValid() : bool\n        {\n            return parent::isValid() && preg_match('/#[0-9A-Fa-f]{6}/', $this->value);\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type hidden\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldHidden extends Field\n    {\n        protected string $type     = 'hidden';\n        protected bool   $required = true;\n    }\n\n\n    /**\n     * The class that represent a input of type checkbox\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldCheckbox extends Field\n    {\n        protected string $type    = 'checkbox';\n        protected bool   $checked = false;\n\n\n        public function isValid() : bool\n        {\n            return !$this->required || $this->checked;\n        }\n\n        /**\n         * Check the input\n         * \n         * @return object The field\n         */\n        public function check() : object\n        {\n            $this->checked = true;\n            return $this;\n        }\n\n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().($this->checked ? ' checked' : '');\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type radio\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldRadio extends FieldCheckbox\n    {\n        protected string $type = 'radio';\n    }\n\n\n    /**\n     * The class that represent a select element\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldSelect extends Field\n    {\n        protected array $options = [];\n\n\n        /**\n         * Add an option to the selection\n         * \n         * @param  string $value    The option value\n         * @param  string $text     The text rendered for this option\n         * @param  bool   $selected If this option is the selected one by default\n         * @return object           The field\n         */\n        public function addOption(\n            string $value,\n            string $text,\n            bool   $selected = false\n        ) : object\n        {\n            $this->options[] = [\n                'value'    => $value,\n                'text'     => $text,\n                'selected' => $selected\n            ];\n            return $this;\n        }\n\n        public function render() : string\n        {\n            $html = $this->renderLabel().'<select '.$this->renderAttributes().'>';\n            foreach ($this->options as $option)\n            {\n                $html .= '<option value=\"'.$option['value'].'\"'.($option['selected'] ? ' selected' : '').'>'.$option['text'].'</option>';\n            }\n            return $html.'</select>';\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type date\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldDate extends Field\n    {\n        protected string  $type    = 'date';\n        protected string  $pattern = \\F2P2\\REGEX_DATE;\n        protected ?string $min     = null;\n        protected ?string $max     = null;\n\n\n        public function isValid() : bool\n        {\n            $value = $this->value ? new \\Datetime($this->value) : null;\n            $min = $this->min ? new \\Datetime($this->min) : null;\n            $max = $this->max ? new \\Datetime($this->max) : null;\n            return parent::isValid() && preg_match($this->pattern, $this->value) && ($min === null || $value >= $min) && ($max === null || $value <= $max);\n        }\n\n        /**\n         * Defines the field minimum date\n         * \n         * @param  int    $max The value\n         * @return object      The field\n         */\n        public function setMin(\n            string $min\n        ) : object\n        {\n            if (!preg_match($this->pattern, $min))\n            {\n                throw new \\Exception('Min value doesn\\'t match the date type pattern');\n            }\n            $this->min = $min;\n            return $this;\n        }\n\n        /**\n         * Defines the field maximum date\n         * \n         * @param  int    $max The value\n         * @return object      The field\n         */\n        public function setMax(\n            string $max\n        ) : object\n        {\n            if (!preg_match($this->pattern, $max))\n            {\n                throw new \\Exception('Max value doesn\\'t match the date type pattern');\n            }\n            $this->max = $max;\n            return $this;\n        }\n\n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().($this->min ? ' min=\"'.$this->min.'\"' : '').($this->max ? ' max=\"'.$this->max.'\"' : '');\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type datetime-local\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldDatetimeLocal extends FieldDate\n    {\n        protected string $type    = 'datetime-local';\n        protected string $pattern = \\F2P2\\REGEX_DATETIMELOCAL;\n    }\n\n\n    /**\n     * The class that represent a input of type time\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldTime extends FieldDate\n    {\n        protected string $type    = 'time';\n        protected string $pattern = \\F2P2\\REGEX_TIME;\n    }\n\n\n    /**\n     * The class that represent a input of type month\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldMonth extends FieldDate\n    {\n        protected string $type    = 'month';\n        protected string $pattern = \\F2P2\\REGEX_MONTH;\n    }\n\n\n    /**\n     * The class that represent a input of type week\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldWeek extends FieldDate\n    {\n        protected string $type        = 'week';\n        protected string $pattern     = \\F2P2\\REGEX_WEEK;\n    }\n\n\n    /**\n     * The class that represent a input of type file\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldFile extends Field\n    {\n        protected string $type    = 'file';\n        protected array  $accept  = [];\n        protected ?int   $maxSize = null;\n        protected array  $data    = [];\n\n\n        /**\n         * Save the file handled by the field\n         * \n         * @param  string $to The destination path\n         * @return bool       true if the file was successfully saved, otherwise false\n         */\n        public function saveFile(\n            string $to\n        ) : bool\n        {\n            return array_key_exists('tmp_name', $this->data) ? move_uploaded_file($this->data['tmp_name'], $to) : false;\n        }\n\n        public function isValid() : bool\n        {\n            if (count($this->data) === 0)\n            {\n                return parent::isValid();\n            }\n            else\n            {\n                foreach (['name', 'full_path', 'type', 'tmp_name', 'error', 'size'] as $key)\n                {\n                    if (!array_key_exists($key, $this->data))\n                    {\n                        return false;\n                    }\n                }\n                return parent::isValid() && $this->data['error'] === 0 && ($this->maxSize === null || $this->data['size'] <= $this->maxSize) && (count($this->accept) === 0 || in_array($this->data['type'], $this->accept));\n            }\n        }\n\n        /**\n         * Add a file type accepted in the field\n         * \n         * @param  string $type The type in MIME format\n         * @return object       The field\n         */\n        public function addAccept(\n            string $type\n        ) : object\n        {\n            $this->accept[] = $type;\n            return $this;\n        }\n\n        /**\n         * Define the file maximum size\n         * \n         * @param  int    $maxSize The size (o)\n         * @return object          The field\n         */\n        public function setMaxSize(\n            int $maxSize\n        ) : object\n        {\n            $this->maxSize = $maxSize;\n            return $this;\n        }\n\n        /**\n         * Define the actual file data\n         * \n         * @param  array  $data The data\n         * @return object       The field\n         */\n        public function setData(\n            array $data\n        ) : object\n        {\n            $this->data = $data;\n            return $this;\n        }\n\n        protected function renderAttributes() : string\n        {\n            return parent::renderAttributes().' accept=\"'.implode(', ', $this->accept).'\"';\n        }\n    }\n\n\n    /**\n     * The class that represent a input of type submit\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class FieldSubmit extends Field\n    {\n        protected string $type = 'submit';\n\n\n        public function __construct()\n        {\n            $this->name = 'submit';\n        }\n    }\n\n\n    /**\n     * The base class for form objets that can contain other form objects\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    abstract class FieldContainer extends FormObject\n    {\n        protected array $fields = [];\n\n\n        /**\n         * Check if all the children are valid\n         * \n         * @return bool true if valid, otherwise false\n         */\n        public function isValid() : bool\n        {\n            foreach ($this->getAllFields() as $field)\n            {\n                if ($field->isValid() === false)\n                {\n                    return false;\n                }\n            }\n            return true;\n        }\n\n        /**\n         * Add a field to the children\n         * \n         * @param  FormObject $field The field\n         * @return object            The container\n         */\n        public function addField(\n            FormObject $field\n        ) : object\n        {\n            $this->fields[] = $field;\n            return $this;\n        }\n\n        /**\n         * Returns all the children recursively\n         * \n         * @return array The children\n         */\n        protected function getAllFields() : array\n        {\n            $fields = [];\n            foreach ($this->fields as $field)\n            {\n                if (is_subclass_of($field, FieldContainer::class))\n                {\n                    $fields = array_merge($fields, $field->getAllFields());\n                }\n                else\n                {\n                    $fields[] = $field;\n                }\n            }\n            return $fields;\n        }\n\n        public function render() : string\n        {\n            $html = '';\n            foreach ($this->fields as $field)\n            {\n                $html .= $field->render();\n            }\n            return $html;\n        }\n    }\n\n\n    /**\n     * The class that represent a fieldset\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Fieldset extends FieldContainer\n    {\n        private ?string $legend = null;\n\n\n        /**\n         * Define the fieldset legend\n         * \n         * @param  string $legend The legend\n         * @return object         The fieldset\n         */\n        public function setLegend(\n            string $legend\n        ) : object\n        {\n            $this->legend = $legend;\n            return $this;\n        }\n\n        public function render() : string\n        {\n            return '<fieldset '.$this->renderAttributes().'>'.($this->legend ? '<legend>'.$this->legend.'</legend>' : '').parent::render().'</fieldset>';\n        }\n    }\n\n    /**\n     * The class that represent a form\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Form extends FieldContainer\n    {\n        private string  $method     = 'POST';\n        private ?string $action     = null;\n        private bool    $allowFiles = false;\n\n\n        /**\n         * Returns all the form data\n         * \n         * @return array The data (fieldname => value)\n         */\n        public function getData() : array\n        {\n            $data = [];\n            foreach ($this->getAllFields() as $field)\n            {\n                $data[$field->getName()] = $field->getValue();\n            }\n            return [];\n        }\n\n        /**\n         * Get a field using it's name\n         * \n         * @param  string|array $name The field name\n         * @return ?object            The field if found, otherwise null\n         */\n        public function getField(\n            string $name\n        ) : ?object\n        {\n            foreach ($this->getAllFields() as $field)\n            {\n                if ($name === $field->getName())\n                {\n                    return $field;\n                }\n            }\n            return null;\n        }\n\n        /**\n         * Define the form submit method\n         * \n         * @param  string $method The method\n         * @return object         The form\n         */\n        public function setMethod(\n            string $method\n        ) : object\n        {\n            $this->method = $method;\n            return $this;\n        }\n\n        /**\n         * Define the form action\n         * \n         * @param  string $action The action\n         * @return object         The form\n         */\n        public function setAction(\n            string $action\n        ) : object\n        {\n            $this->action = \\F2P2\\Router::getRouteURL($action);\n            return $this;\n        }\n\n        /**\n         * Allow the file upload in the form\n         * \n         * @return object The form\n         */\n        public function allowFiles() : object\n        {\n            $this->allowFiles = true;\n            return $this;\n        }\n\n        /**\n         * Fill the form with data\n         * \n         * @param  array $data The data (fieldname => value)\n         * @return void\n         */\n        public function fill(\n            array $data\n        ) : void\n        {\n            $data = array_reverse($data);\n            foreach ($this->getAllFields() as $field)\n            {\n                if (array_key_exists($field->getName(), $data))\n                {\n                    if (get_class($field) === FieldFile::class)\n                    {\n                        $field->setData($data[$field->getName()]);\n                    }\n                    else if (get_class($field) === FieldCheckbox::class || get_class($field) === FieldRadio::class)\n                    {\n                        $field->check();\n                    }\n                    else\n                    {\n                        $field->setValue($data[$field->getName()]);\n                    }\n                    unset($data[$field->getName()]);\n                }\n            }\n        }\n\n        public function render() : string\n        {\n            return '<form method=\"'.$this->method.'\"'.($this->action ? ' action=\"'.$this->action.'\"' : '').($this->allowFiles ? ' enctype=\"multipart/form-data\"' : '').' '.$this->renderAttributes().'>'.parent::render().'</form>';\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/utils/Language.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that handles the language and translation of the website\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Language\n    {\n        static private string $language = DEFAULT_LANGUAGE;\n        static private array  $data     = [];\n\n\n        /**\n         * Initialize the language data\n         * \n         * @param  array $data The data (language => id => text)\n         * @return void\n         */\n        static public function init(\n            array $data\n        ) : void\n        {\n            if (isset($_SESSION['language']))\n            {\n                self::$language = $_SESSION['language'];\n            }\n            self::$data = $data;\n        }\n\n        /**\n         * Returns the actual language used\n         * \n         * @return string The language\n         */\n        static public function getLanguage() : string\n        {\n            return self::$language;\n        }\n\n        /**\n         * Defines the language to use\n         * \n         * @param  string $language The language\n         * @return void\n         */\n        static public function setLanguage(\n            string $language\n        ) : void\n        {\n            self::$language = $language;\n            $_SESSION['language'] = $language;\n        }\n\n        /**\n         * Returns all the languages used\n         * \n         * @return array An array of languages\n         */\n        static public function getAllLanguages() : array\n        {\n            return array_keys(self::$data);\n        }\n\n        /**\n         * Returns a text corresponding to the actual language\n         * \n         * @param  string $id The id of the text\n         * @return string     The text\n         */\n        static public function getText(\n            string $id\n        ) : string\n        {\n            if (array_key_exists(self::$language, self::$data) && array_key_exists($id, self::$data[self::$language]))\n            {\n                return self::$data[self::$language][$id];\n            }\n            else if (array_key_exists(DEFAULT_LANGUAGE, self::$data) && array_key_exists($id, self::$data[DEFAULT_LANGUAGE]))\n            {\n                return self::$data[DEFAULT_LANGUAGE][$id];\n            }\n            else\n            {\n                return $id;\n            }\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/utils/Router.php");
        ofs << "<?php \n    namespace F2P2;\n\n\n    /**\n     * The class that manages and executes routes\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Router\n    {\n        static private array $routes = [];\n    \n\n        /**\n         * Add routes to the router\n         * \n         * @param  array $routes The routes\n         * @return void\n         */\n        static public function addRoutes(\n            array $routes\n        ) : void\n        {\n            foreach ($routes as $route)\n            {\n                self::addRoute(\n                    $route['name'],\n                    $route['method'],\n                    $route['uri'],\n                    $route['controller'],\n                    $route['action'],\n                    array_key_exists('logged', $route) ? $route['logged'] : null,\n                    array_key_exists('permissions', $route) ? $route['permissions'] : null,\n                );\n            }\n        }\n\n        /**\n         * Add a route to the router\n         * \n         * @param  string $name        The route name\n         * @param  string $method      The route access method\n         * @param  string $uri         The route URI\n         * @param  string $controller  The route controller name\n         * @param  string $action      The route action of the controller\n         * @param  ?bool  $logged      true if the user must be logged in to access this route\n         * @param  ?array $permissions An array of permissions needed to access this route\n         * @return void\n         */\n        static public function addRoute(\n            string $name,\n            string $method,\n            string $uri,\n            string $controller,\n            string $action,\n            ?bool  $logged      = null,\n            ?array $permissions = null\n        ) : void\n        {\n            self::$routes[$name] = [\n                'method'      => $method,\n                'uri'         => self::filterURI($uri),\n                'controller'  => $controller,\n                'action'      => $action,\n                'logged'      => $logged,\n                'permissions' => $permissions\n            ];\n        }\n\n        /**\n         * Generate URI for a route\n         * \n         * @param  string $name   The route name\n         * @param  array  $params The values to use for URI parameters\n         * @param  array  $get    The GET arguments\n         * @return string         The URI\n         */\n        static public function getRouteURI(\n            string $name,\n            array  $params = [],\n            array  $get    = []\n        ) : string\n        {\n            if (!isset(self::$routes[$name]))\n            {\n                throw new \\Exception(\"No route named \\\"$name\\\"\");\n            }\n            $uri = '';\n            foreach (self::$routes[$name]['uri'] as $arg)\n            {\n                if ($arg[0] === '$')\n                {\n                    $param = substr($arg, 1);\n                    if (!array_key_exists($param, $params))\n                    {\n                        throw new \\Exception(\"Undefined parameter \\\"$param\\\"\");\n                    }\n                    $uri .= '/'.$params[$param];\n                }\n                else\n                {\n                    $uri .= '/'.$arg;\n                }\n            }\n            if (count($get) > 0)\n            {\n                $uri .= '?';\n                $args = [];\n                foreach ($get as $var => $value)\n                {\n                    $args[] = \"$var=$value\";\n                }\n                $uri .= implode('&', $args);\n            }\n            return $uri;\n        }\n\n        /**\n         * Generate URL for a route\n         * \n         * @param  string $name   The route name\n         * @param  array  $params The values to use for URL parameters\n         * @param  array  $get    The GET arguments\n         * @return string         The URL\n         */\n        static public function getRouteURL(\n            string $name,\n            array  $params = [],\n            array  $get    = []\n        ) : string\n        {\n            return \\F2P2\\Server::getBaseURL().self::getRouteURI($name, $params, $get);\n        }\n\n        /**\n         * Process a request in the router\n         * \n         * @param  \\F2P2\\Request $request The request\n         * @return void\n         */\n        static public function run(\n            \\F2P2\\Request $request\n        ) : void\n        {\n            $uri = self::filterURI($request->getURL());\n            $length = count($uri);\n            foreach (self::$routes as $name => $route)\n            {\n                if ($length === count($route['uri']))\n                {\n                    $params = [];\n                    $valid = true;\n                    for ($i = 0; $i < $length; $i++)\n                    {\n                        if ($route['uri'][$i][0] === '$')\n                        {\n                            $params[substr($route['uri'][$i], 1)] = $uri[$i];\n                        }\n                        else if ($uri[$i] !== $route['uri'][$i])\n                        {\n                            $valid = false;\n                            break;\n                        }\n                    }\n                    if ($valid)\n                    {\n                        if ($request->getMethod() !== $route['method'])\n                        {\n                            self::runError($request, 405);\n                        }\n                        else\n                        {\n                            if (USE_AUTH && isset($route['logged']) && $route['logged'])\n                            {\n                                if (!isset($_SESSION['user']))\n                                {\n                                    self::runError($request, 401);\n                                }\n                                else if (isset($route['permissions']))\n                                {\n                                    foreach ($route['permissions'] as $permission)\n                                    {\n                                        if (!in_array($permission, $_SESSION['user']->getPermissions()))\n                                        {\n                                            self::runError($request, 401);\n                                        }\n                                    }\n                                }\n                            }\n                            require_once('../app/controllers/'.$route['controller'].'Controller.php');\n                            ((new ('\\\\App\\\\Controllers\\\\'.$route['controller'].'Controller')())->{$route['action']}($request, $params))->send();\n                        }\n                    }\n                }\n            }\n            self::runError($request, 404);\n        }\n\n        /**\n         * Process an error in the router\n         * \n         * @param  \\F2P2\\Request $request The request that generated the error\n         * @param  int           $code    The error code\n         * @return void\n         */\n        static private function runError(\n            \\F2P2\\Request $request,\n            int           $code\n        ) : void\n        {\n            if (file_exists('../app/controllers/ErrorController.php'))\n            {\n                require_once('../app/controllers/ErrorController.php');\n                if (class_exists('\\\\App\\\\Controllers\\\\ErrorController') && method_exists('\\\\App\\\\Controllers\\\\ErrorController', \"error$code\"))\n                {\n                    ((new ('\\\\App\\\\Controllers\\\\ErrorController')())->{\"error$code\"}($request))->send();\n                }\n            }\n            (new \\F2P2\\ResponseError($code))->send();\n        }\n\n        /**\n         * Returns an array of filtered URI parts\n         * \n         * @param  string $uri The URI\n         * @return array       The parts\n         */\n        static private function filterURI(\n            string $uri\n        ) : array\n        {\n            $length = strlen($uri);\n            $hasBeginSlash = $length > 0 && $uri[0] === '/';\n            $hasEndSlash = $length > 1 && $uri[$length-1] === '/';\n            $uri = substr($uri, $hasBeginSlash, $length-$hasBeginSlash-$hasEndSlash);\n            return $uri === '' ? [] : explode('/', $uri);\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/utils/Server.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    /**\n     * The class that represent the actual state of the server\n     * \n     * @link      link\n     * @since     Version 0.0\n     * @author    Thomas Baricault <thomas.baricault@gmail.com>\n     * @copyright Copyright (c) 2024, Thomas Baricault\n     * @license   https://opensource.org/license/bsd-2-clause\n     */\n    class Server\n    {\n        /**\n         * Returns the site base URL\n         * \n         * @return string The URL\n         */\n        static public function getBaseURL() : string\n        {\n            return $_SERVER['REQUEST_SCHEME'].'://'.$_SERVER['SERVER_NAME'];\n        }\n\n        /**\n         * Returns the current request URI\n         * \n         * @return string The URI\n         */\n        static public function getCurrentURI() : string\n        {\n            return $_SERVER['PATH_INFO'] ?? '/';\n        }\n\n        /**\n         * Returns the current request URL\n         * \n         * @return string The URL\n         */\n        static public function getCurrentURL() : string\n        {\n            return self::getBaseURL().self::getCurrentURI();\n        }\n\n        /**\n         * Returns the current request method\n         * \n         * @return string The method\n         */\n        static public function getRequestMethod() : string\n        {\n            return $_SERVER['REQUEST_METHOD'];\n        }\n\n        /**\n         * Returns the current request port\n         * \n         * @return int The port\n         */\n        static public function getServerPort() : int\n        {\n            return $_SERVER['SERVER_PORT'];\n        }\n    }\n?>";
        ofs.close();
        ofs = ofstream("./f2p2/utils/utils.php");
        ofs << "<?php\n    namespace F2P2;\n\n\n    const REGEX_DATE          = '/(19|20)\\d\\d-(0[1-9]|1[012])-([012]\\d|3[01])/';\n    const REGEX_DATETIMELOCAL = '/(19|20)\\d\\d-(0[1-9]|1[012])-([012]\\d|3[01])T([01]\\d|2[0-3]):([0-5]\\d)/';\n    const REGEX_TIME          = '/([01]\\d|2[0-3]):([0-5]\\d)/';\n    const REGEX_MONTH         = '/(19|20)\\d\\d-(0[1-9]|1[012])/';\n    const REGEX_WEEK          = '/(19|20)\\d\\d-W([0-4]\\d)|(5[0123])/';\n\n\n    /**\n     * Read the JSON of a file\n     * \n     * @param  string $path The file path\n     * @return array        The JSON\n     */\n    function readJSON(\n        string $path\n    ) : array\n    {\n        return file_exists($path) ? (json_decode(file_get_contents($path), true) ?? []) : [];\n    }\n\n\n    /**\n     * Check if email is a valid one\n     * \n     * @param  string $email The email\n     * @return bool          If it's valid\n     */\n    function emailIsValid(\n        string $email\n    ) : bool\n    {\n        $parts = explode('@', $email);\n        return filter_var($email, FILTER_VALIDATE_EMAIL) && count($parts) === 2 && checkdnsrr($parts[1]);\n    }\n\n\n    /**\n     * Returns the MIME equivalent for a file extension\n     * \n     * @param  string  $extension The extension\n     * @return ?string            The MIME type if found, otherwise null\n     */\n    function extensionToMIME(\n        string $extension\n    ) : ?string\n    {\n        $data = readJSON('../config/mime.json');\n        foreach ($data as $mime => $extensions)\n        {\n            if (is_array($extensions) && in_array($extension, $extensions) || !is_array($extensions) && $extensions === $extension)\n            {\n                return $mime;\n            }\n        }\n        return null;\n    }\n\n\n    /**\n     * Returns a file extension equivalent for a MIME type\n     * \n     * @param  string  $mime The MIME type\n     * @return ?string       The extension if found, otherwise null\n     */\n    function mimeToExtension(\n        string $mime\n    ) : ?string\n    {\n        $data = readJSON('../config/mime.json');\n        return array_key_exists($mime, $data) ? (is_array($data[$mime]) ? $data[$mime][0] : $data[$mime]) : null;\n    }\n?>";
        ofs.close();
        ofs = ofstream("./LICENSE");
        ofs << "BSD 2-Clause License\n\nCopyright (c) 2024, Thomas Baricault\n\nRedistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n\nRedistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n\nRedistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n\nTHIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";
        ofs.close();
        mkdir("./public");
        ofs = ofstream("./public/.htaccess");
        ofs << "<IfModule mod_rewrite.c>\n\n    Options +FollowSymLinks\n\n    RewriteEngine on\n\n    RewriteCond %{REQUEST_FILENAME} !-f\n    RewriteCond %{REQUEST_FILENAME} !-d\n    RewriteRule ^(.*)$ index.php/$1 [L]\n\n</IfModule>";
        ofs.close();
        mkdir("./public/css");
        mkdir("./public/fonts");
        mkdir("./public/img");
        ofs = ofstream("./public/index.php");
        ofs << "<?php\n    require('../config/env.php');\n    require('../config/db.php');\n    require('../f2p2/autoload.php');\n\n    \\F2P2\\Language::init(\\F2P2\\readJSON('../config/languages.json'));\n\n    \\F2P2\\Router::addRoutes(\\F2P2\\readJSON('../config/routes.json'));\n    \\F2P2\\Router::run(\\F2P2\\Request::fromCurrent());\n?>";
        ofs.close();
        mkdir("./public/js");
        ofs = ofstream("./README.md");
        ofs << "# F2P2 - Fantastic Framework & Practical PHP\n\nF2P2 is a lightweight PHP framework based on the MVC coding method.\n\n## Description\n\nF2P2 makes it easier to create an application or website in PHP. Inspired by Symfony, it allows database management and easier use of database entities. It works based on a CLI which manages the project. It also provides some other features like forms, email and languages.\n\n## Getting Started\n\n### Dependencies\n\n- PHP\n\n### Documentation\n\n<https://f2p2.thomas-baricault.fr>\n\n## Author\n\nThomas Baricault\n\n## Version History\n\n- 0.0\n    - Initial release\n\n## License\n\nThis project is licensed under the BSD 2-Clause License - see the LICENSE file for details\n\n## Acknowledgments\n\n- Inspirate by Symfony";
        ofs.close();
        cout << CLI::colorize("The project was created successfully.", CLI::FG_LIGHTGREEN) << endl;
    }
}

void createRoute(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    if (addRouteInFile(
        required.at("name"),
        optional.at("method"),
        optional.at("uri"),
        optional.at("controller"),
        optional.at("action"),
        optional.at("logged"),
        optional.at("permissions")
    )) {
        cout << CLI::colorize("The route has been successfully added to the configuration.", CLI::FG_LIGHTGREEN) << endl;
    }
    else
    {
        cout << CLI::colorize("Error adding the route to configuration.", CLI::FG_LIGHTRED) << endl;
    }
    if (!controllerExists(optional.at("controller")))
    {
        if (createControllerFile(optional.at("controller")))
        {
            cout << CLI::colorize("The controller was created successfully.", CLI::FG_LIGHTGREEN) << endl;
        }
        else
        {
            cout << CLI::colorize("Failed to create the controller.", CLI::FG_LIGHTRED) << endl;
        }
    }
    if (addActionInFile(optional.at("controller"), optional.at("action")))
    {
        cout << CLI::colorize("The action function was successfully added to the controller file.", CLI::FG_LIGHTGREEN) << endl;
    }
    else
    {
        cout << CLI::colorize("Failed to create action function.", CLI::FG_LIGHTRED) << endl;
    }
}

void errorRoute(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    if (!controllerExists("Error"))
    {
        if (createControllerFile("Error"))
        {
            cout << CLI::colorize("The error controller was created successfully.", CLI::FG_LIGHTGREEN) << endl;
        }
        else
        {
            cout << CLI::colorize("Failed to create the error controller.", CLI::FG_LIGHTRED) << endl;
        }
    }
    if (controllerExists("Error"))
    {
        if (addActionInFile("Error", "error"+required.at("code"), "            return new \\F2P2\\ResponseError("+required.at("code")+");\n"))
        {
            cout << CLI::colorize("The action function was successfully added to the error controller file.", CLI::FG_LIGHTGREEN) << endl;
        }
        else
        {
            cout << CLI::colorize("Failed to create action function.", CLI::FG_LIGHTRED) << endl;
        }
    }
}

void createController(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    if (required.at("name") == "Error" || required.at("name") == "User")
    {
        cout << CLI::colorize("Cannot use this name because it is reserved", CLI::FG_LIGHTRED) << endl;
    }
    else if (controllerExists(required.at("name")))
    {
        cout << CLI::colorize("A controller with the same name already exists.", CLI::FG_LIGHTRED) << endl;
    }
    else if (createControllerFile(required.at("name")))
    {
        cout << CLI::colorize("The controller was successfully created.", CLI::FG_LIGHTGREEN) << endl;
    }
    else
    {
        cout << CLI::colorize("Failed to create the controller.", CLI::FG_LIGHTRED) << endl;
    }
}

void initDatabase(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    checkDir("./config");
    ofstream ofs("./config/db.php");
    ofs
        << "<?php"                                                   << endl
        << "    define('DB_TYPE', '" << optional.at("type") << "');" << endl
        << "    define('DB_HOST', '" << optional.at("host") << "');" << endl
        << "    define('DB_NAME', '" << required.at("name") << "');" << endl
        << "    define('DB_USER', '" << optional.at("user") << "');" << endl
        << "    define('DB_PASS', '" << optional.at("pass") << "');" << endl
        << "?>";
    ofs.close();
    InfoFile::InfoBlock* infos = new InfoFile::InfoBlock("DATABASE");
    saveDBInfos(infos);
    delete infos;
    cout << CLI::colorize("The database was initialized successfully.", CLI::FG_LIGHTGREEN) << endl;
}

void updateDatabase(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    InfoFile::InfoBlock* infos = getDBInfos();
    if (infos)
    {
        map<string, string> config;
        if (getDBConfig(config))
        {
            if (config.at("TYPE") == "mysql")
            {
                MYSQL* con = mysql_init(nullptr);
                if (mysql_real_connect(con, config.at("HOST").c_str(), config.at("USER").c_str(), config.at("PASS").c_str(), config.at("NAME").c_str(), 0, nullptr, 0) == nullptr)
                {
                    cout << CLI::colorize("Unable to connect to MySQL host.", CLI::FG_LIGHTRED) << endl;
                }
                else
                {
                    cout << CLI::colorize("Connected to MySQL host.", CLI::FG_LIGHTGREEN) << endl;
                    bool error;
                    InfoFile::InfoBlock* latest = new InfoFile::InfoBlock("DATABASE");
                    MYSQL_RES* model_result = mysql_list_tables(con, nullptr);
                    MYSQL_ROW model_row;
                    cout << CLI::colorize("Found ", CLI::FG_LIGHTCYAN) << CLI::colorize(to_string(mysql_num_rows(model_result)), CLI::FG_LIGHTYELLOW) << CLI::colorize(" tables.", CLI::FG_LIGHTCYAN) << endl;
                    while (!error && (model_row = mysql_fetch_row(model_result)))
                    {
                        if (mysql_query(con, ("DESCRIBE "+((string)model_row[0])).c_str()))
                        {
                            cout
                                << CLI::colorize("Unable to get information from ", CLI::FG_LIGHTRED) << CLI::colorize(model_row[0], CLI::FG_LIGHTYELLOW) << CLI::colorize(" table.", CLI::FG_LIGHTRED) << endl
                                << CLI::colorize("MySQL error : ", CLI::FG_LIGHTRED) << CLI::colorize(mysql_error(con), CLI::FG_LIGHTRED)                                                           << endl;
                            error = true;
                        }
                        else
                        {
                            InfoFile::InfoBlock* model = (new InfoFile::InfoBlock("MODEL"))->addProperty("NAME", model_row[0]);
                            MYSQL_RES* column_result = mysql_store_result(con);
                            MYSQL_ROW column_row;
                            cout << CLI::colorize("Found ", CLI::FG_LIGHTCYAN) << CLI::colorize(to_string(mysql_num_rows(column_result)), CLI::FG_LIGHTYELLOW) << CLI::colorize(" columns in the table ", CLI::FG_LIGHTCYAN) << CLI::colorize(model_row[0], CLI::FG_LIGHTYELLOW) << CLI::colorize(".", CLI::FG_LIGHTCYAN) << endl;
                            while (!error && (column_row = mysql_fetch_row(column_result)))
                            {
                                if (((string)column_row[0]) != "id")
                                {
                                    InfoFile::InfoBlock* column = (new InfoFile::InfoBlock("COLUMN"))->addProperty("NAME", column_row[0]);
                                    string type_size = column_row[1];
                                    string type;
                                    int i = 0;
                                    while (i < type_size.length() && type_size.at(i) != '(')
                                    {
                                        type += type_size.at(i)-'a'+'A';
                                        i++;
                                    }
                                    column->addProperty("TYPE", type);
                                    if (mysqlUseSize(type) && i != type_size.length())
                                    {
                                        column->addProperty("SIZE", type_size.substr(i+1, type_size.length()-i-2));
                                    }
                                    if (((string)column_row[2]) == "YES")
                                    {
                                        column->addProperty("NULL", "TRUE");
                                    }
                                    else
                                    {
                                        column->addProperty("NULL", "FALSE");
                                    }
                                    if (((string)column_row[3]) == "MUL")
                                    {
                                        if (mysql_query(con, ("SELECT REFERENCED_TABLE_NAME FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE WHERE TABLE_SCHEMA = \""+config.at("NAME")+"\" AND TABLE_NAME = \""+model_row[0]+"\" AND COLUMN_NAME = \""+column_row[0]+"\"").c_str()))
                                        {
                                            cout
                                                << CLI::colorize("Failed to get the table targeted by the foreign key.", CLI::FG_LIGHTRED)             << endl
                                                << CLI::colorize("MySQL error : ", CLI::FG_LIGHTRED) << CLI::colorize(mysql_error(con), CLI::FG_LIGHTRED) << endl;
                                            error = true;
                                        }
                                        else
                                        {
                                            MYSQL_RES* reference_result = mysql_store_result(con);
                                            column->addProperty("REFERENCE", mysql_fetch_row(reference_result)[0]);
                                            mysql_free_result(reference_result);
                                        }
                                    }
                                    model->addChild(column);
                                }
                            }
                            mysql_free_result(column_result);
                            latest->addChild(model);
                        }
                    }
                    mysql_free_result(model_result);
                    error = !checkReferences(latest);
                    if (error)
                    {
                        cout << CLI::colorize("The actual database contains an invalid reference.", CLI::FG_LIGHTRED) << endl;
                    }
                    else
                    {
                        error = !checkReferences(infos);
                        if (error)
                        {
                            cout << CLI::colorize("The actual database contains an invalid reference.", CLI::FG_LIGHTRED) << endl;
                        }
                        else
                        {
                            error = !checkReferenceLoop(latest);
                            if (error)
                            {
                                cout << CLI::colorize("The actual database contains a reference loop.", CLI::FG_LIGHTRED) << endl;
                            }
                            else
                            {
                                error = !checkReferenceLoop(infos);
                                if (error)
                                {
                                    cout << CLI::colorize("The draft database contains a reference loop.", CLI::FG_LIGHTRED) << endl;
                                }
                                else
                                {
                                    sortByReferences(latest);
                                    sortByReferences(infos);
                                }
                            }
                        }
                    }
                    if (!error)
                    {
                        for (auto latest_model : latest->getChildren())
                        {
                            InfoFile::InfoBlock* new_model = infos->getChild("NAME", latest_model->getProperty("NAME"));
                            if (new_model)
                            {
                                for (auto latest_column : latest_model->getChildren())
                                {
                                    InfoFile::InfoBlock* new_column = new_model->getChild("NAME", latest_column->getProperty("NAME"));
                                    if (new_column)
                                    {
                                        if (!cmpColumnsInfos(latest_column, new_column))
                                        {
                                            if (latest_column->getProperty("REFERENCE") != new_column->getProperty("REFERENCE"))
                                            {
                                                if (!latest_column->getProperty("REFERENCE").empty())
                                                {
                                                    error = mysqlDropReference(con, latest_model, latest_column);
                                                }
                                                if (!error && !new_column->getProperty("REFERENCE").empty())
                                                {
                                                    error = mysqlAddReference(con, latest_model, new_column);
                                                }
                                            }
                                            if (!error)
                                            {
                                                error = mysqlModifyColumn(con, latest_model, new_column);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (!latest_column->getProperty("REFERENCE").empty())
                                        {
                                            error = mysqlDropReference(con, latest_model, latest_column);
                                        }
                                        if (!error)
                                        {
                                            error = mysqlDropColumn(con, latest_model, latest_column);
                                        }
                                    }
                                    if (error)
                                    {
                                        break;
                                    }
                                }
                                if (error)
                                {
                                    break;
                                }
                                for (auto new_column : new_model->getChildren())
                                {
                                    if (
                                        latest_model->getChild("NAME", new_column->getProperty("NAME")) == nullptr &&
                                        (
                                            (error = mysqlAddColumn(con, latest_model, new_column)) ||
                                            (
                                                !new_column->getProperty("REFERENCE").empty() &&
                                                (error = mysqlAddReference(con, latest_model, new_column))
                                            )
                                        )
                                    ) {
                                        break;
                                    }
                                }
                                if (error)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                error = mysqlDropModel(con, latest_model);
                            }
                            if (error)
                            {
                                break;
                            }
                        }
                        for (auto new_model : infos->getChildren())
                        {
                            if (
                                latest->getChild("NAME", new_model->getProperty("NAME")) == nullptr &&
                                (error = mysqlCreateModel(con, new_model))
                            ) {
                                break;
                            }
                        }
                    }
                    if (!error)
                    {
                        saveDBInfos(infos, getNowVersion());
                        cout << CLI::colorize("Successful database update.", CLI::FG_LIGHTGREEN) << endl;
                    }
                    delete latest;
                }
                mysql_close(con);
            }
            else
            {
                cout << CLI::colorize("Command line database update is only available for MySQL databases.", CLI::FG_LIGHTRED) << endl;
            }
        }
        else
        {
            cout << CLI::colorize("Unable to find database information in configuration.", CLI::FG_LIGHTRED) << endl;
        }
        delete infos;
    }
    else
    {
        cout << CLI::colorize("No database initialized.", CLI::FG_LIGHTRED) << endl;
    }
}

void createModel(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    InfoFile::InfoBlock* infos = getDBInfos();
    if (infos)
    {
        string snake_case = StringCase::pascalToSnake(required.at("name"));
        string pascal_case = StringCase::snakeToPascal(snake_case);
        if (pascal_case == "User")
        {
            cout << CLI::colorize("Cannot use this name because it is reserved", CLI::FG_LIGHTRED) << endl;
        }
        else
        {
            if (infos->getChild("NAME", snake_case))
            {
                cout << CLI::colorize("A model with the same name already exists in the database.", CLI::FG_LIGHTRED) << endl;
            }
            else if (modelExists(pascal_case))
            {
                cout << CLI::colorize("A model class with the same name already exists.", CLI::FG_LIGHTRED) << endl;
            }
            else if (createModelFile(pascal_case, snake_case))
            {
                infos->addChild(
                    (new InfoFile::InfoBlock("MODEL"))
                        ->addProperty("NAME", snake_case)
                );
                saveDBInfos(infos);
                cout << CLI::colorize("The model was created successfully.", CLI::FG_LIGHTGREEN) << endl;
            }
            else
            {
                cout << CLI::colorize("Failed to create the model class.", CLI::FG_LIGHTRED) << endl;
            }
        }
        delete infos;
    }
    else
    {
        cout << CLI::colorize("No database initialized.", CLI::FG_LIGHTRED) << endl;
    }
}

void deleteModel(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    InfoFile::InfoBlock* infos = getDBInfos();
    if (infos)
    {
        string snake_case = StringCase::pascalToSnake(required.at("name"));
        string pascal_case = StringCase::snakeToPascal(snake_case);
        InfoFile::InfoBlock* model;
        if (model && getModelTargeted(infos, model).size() > 0)
        {
            cout << CLI::colorize("Cannot delete this model because other models reference it.", CLI::FG_LIGHTRED) << endl;
        }
        else
        {
            if (infos->getChild("NAME", snake_case))
            {
                infos->removeChild("NAME", snake_case);
                saveDBInfos(infos);
                cout << CLI::colorize("The model has been successfully deleted from the database.", CLI::FG_LIGHTGREEN) << endl;
            }
            if (modelExists(pascal_case))
            {
                remove(("./app/models/"+pascal_case+"Model.php").c_str());
                cout << CLI::colorize("The class model was deleted successfully.", CLI::FG_LIGHTGREEN) << endl;
            }
        }
        delete infos;
    }
    else
    {
        cout << CLI::colorize("No database initialized.", CLI::FG_LIGHTRED) << endl;
    }
}

void setColumn(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    if (required.at("name") == "id")
    {
        cout << CLI::colorize("Column name cannot be ", CLI::FG_LIGHTRED) << CLI::colorize("id", CLI::FG_LIGHTYELLOW) << CLI::colorize(" .", CLI::FG_LIGHTRED) << endl;
    }
    else if (mysqlTypeToPHPType(optional.at("type")).empty())
    {
        cout << CLI::colorize("Invalid value type. Use ", CLI::FG_LIGHTRED) << CLI::colorize("f2p2 list dbtypes", CLI::FG_LIGHTYELLOW) << CLI::colorize(" to get the list of allowed types.", CLI::FG_LIGHTRED) << endl;
    }
    else if (!optional.at("reference").empty() && optional.at("type") != "INT")
    {
        cout << CLI::colorize("References can only be defined on a column of type ", CLI::FG_LIGHTRED) << CLI::colorize("INT", CLI::FG_LIGHTYELLOW) << endl;
    }
    else if (optional.at("size").empty() && mysqlNeedSize(optional.at("type")))
    {
        cout << CLI::colorize("The parameter size is required for this value type.", CLI::FG_LIGHTRED) << endl;
    }
    else if (optional.at("additional").empty() && mysqlNeedAdditional(optional.at("type")))
    {
        cout << CLI::colorize("The parameter additional is required for this value type.", CLI::FG_LIGHTRED) << endl;
    }
    else
    {
        InfoFile::InfoBlock* infos = getDBInfos();
        if (infos)
        {
            if (!optional.at("reference").empty() && infos->getChild("NAME", optional.at("reference")) == nullptr)
            {
                cout << CLI::colorize("The model targeted by the reference doesn't exist.", CLI::FG_LIGHTRED) << endl;
            }
            else
            {
                string model_snake = StringCase::pascalToSnake(required.at("model"));
                string model_pascal = StringCase::snakeToPascal(model_snake);
                string column_snake = StringCase::pascalToSnake(required.at("name"));
                string column_pascal = StringCase::snakeToPascal(column_snake);
                InfoFile::InfoBlock* model = infos->getChild("NAME", model_snake);
                if (model)
                {
                    if (!optional.at("reference").empty() && optional.at("reference") == model_snake)
                    {
                        cout << CLI::colorize("A model cannot have reference to itself.", CLI::FG_LIGHTRED) << endl;
                    }
                    else
                    {
                        if (!modelExists(model_pascal))
                        {
                            if (createModelFile(model_pascal, model_snake))
                            {
                                for (auto child : model->getChildren())
                                {
                                    if (child->getProperty("NAME") != column_snake)
                                    {
                                        addColumnInFile(
                                            model_pascal,
                                            StringCase::snakeToPascal(child->getProperty("NAME")),
                                            child->getProperty("NAME"),
                                            child->getProperty("TYPE"),
                                            child->getProperty("NULL"),
                                            child->getProperty("REFERENCE"),
                                            child->getProperty("SIZE"),
                                            child->getProperty("ADDITIONAL")
                                        );
                                    }
                                }
                                cout << CLI::colorize("The model class was created successfully.", CLI::FG_LIGHTGREEN) << endl;
                            }
                            else
                            {
                                cout << CLI::colorize("Failed to create the model class.", CLI::FG_LIGHTRED) << endl;
                            }
                        }
                        if (modelExists(model_pascal))
                        {
                            bool created = false;
                            InfoFile::InfoBlock* column = model->getChild("NAME", column_snake);
                            if (column)
                            {
                                column->removeProperty("TYPE");
                                column->removeProperty("NULL");
                                column->removeProperty("REFERENCE");
                                column->removeProperty("SIZE");
                                column->removeProperty("ADDITIONAL");
                            }
                            else
                            {
                                created = true;
                                column = (new InfoFile::InfoBlock("COLUMN"))
                                    ->addProperty("NAME", column_snake);
                                model->addChild(column);
                            }
                            if (!optional.at("type").empty())
                            {
                                column->addProperty("TYPE", optional.at("type"));
                            }
                            if (optional.at("null") == "TRUE")
                            {
                                column->addProperty("NULL", "TRUE");
                            }
                            else
                            {
                                column->addProperty("NULL", "FALSE");
                            }
                            if (!optional.at("reference").empty())
                            {
                                column->addProperty("REFERENCE", optional.at("reference"));
                            }
                            if (mysqlUseSize(optional.at("type")) && !optional.at("size").empty())
                            {
                                column->addProperty("SIZE", optional.at("size"));
                            }
                            if (mysqlUseAdditional(optional.at("type")) && !optional.at("additional").empty())
                            {
                                column->addProperty("ADDITIONAL", optional.at("additional"));
                            }
                            if (addColumnInFile(
                                model_pascal,
                                column_pascal,
                                column_snake,
                                optional.at("type"),
                                optional.at("null"),
                                optional.at("reference"),
                                optional.at("size"),
                                optional.at("additional")
                            )) {
                                saveDBInfos(infos);
                                if (created)
                                {
                                    cout
                                        << CLI::colorize("The column was successfully created in the database.", CLI::FG_LIGHTGREEN)    << endl
                                        << CLI::colorize("The column was successfully created in the model class.", CLI::FG_LIGHTGREEN) << endl;
                                }
                                else
                                {
                                    cout
                                        << CLI::colorize("The column was successfully updated in the database.", CLI::FG_LIGHTGREEN)    << endl
                                        << CLI::colorize("The column was successfully updated in the model class.", CLI::FG_LIGHTGREEN) << endl;
                                }
                            }
                            else if (created)
                            {
                                cout << CLI::colorize("Unable to add the column in the model class.", CLI::FG_LIGHTRED) << endl;
                            }
                            else
                            {
                                cout << CLI::colorize("Unable to update the column in the model class.", CLI::FG_LIGHTRED) << endl;
                            }
                        }
                    }
                }
                else
                {
                    cout << CLI::colorize("Unable to find the model in the database.", CLI::FG_LIGHTRED) << endl;
                }
            }
            delete infos;
        }
        else
        {
            cout << CLI::colorize("No database initialized.", CLI::FG_LIGHTRED) << endl;
        }
    }
}

void deleteColumn(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    InfoFile::InfoBlock* infos = getDBInfos();
    if (infos)
    {
        string model_snake = StringCase::pascalToSnake(required.at("model"));
        string model_pascal = StringCase::snakeToPascal(model_snake);
        string column_snake = StringCase::pascalToSnake(required.at("name"));
        string column_pascal = StringCase::snakeToPascal(column_snake);
        InfoFile::InfoBlock* model = infos->getChild("NAME", model_snake);
        if (model)
        {
            if (model->getChild("NAME", column_snake))
            {
                model->removeChild("NAME", column_snake);
                saveDBInfos(infos);
                cout << CLI::colorize("The column was successfully deleted from the database.", CLI::FG_LIGHTGREEN) << endl;
            }
            else
            {
                cout << CLI::colorize("Unable to find the column in the database.", CLI::FG_LIGHTRED) << endl;
            }
            if (deleteColumnInFile(model_pascal, column_pascal, column_snake))
            {
                cout << CLI::colorize("The column was successfully deleted from the class file.", CLI::FG_LIGHTGREEN) << endl;
            }
            else
            {
                cout << CLI::colorize("Unable to find the column in the class file.", CLI::FG_LIGHTRED) << endl;
            }
        }
        else
        {
            cout << CLI::colorize("Unable to find the model in the database.", CLI::FG_LIGHTRED) << endl;
        }
        delete infos;
    }
    else
    {
        cout << CLI::colorize("No database initialized.", CLI::FG_LIGHTRED) << endl;
    }
}

void initAuth(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    InfoFile::InfoBlock* infos = getDBInfos();
    if (infos)
    {
        if (modelExists("User"))
        {
            remove("./app/models/UserModel.php");
        }
        string identifier_snake = StringCase::pascalToSnake(optional.at("identifier"));
        string identifier_pascal = StringCase::snakeToPascal(identifier_snake);
        if (
            createModelFile("User", "user") &&
            addColumnInFile("User", identifier_pascal, identifier_snake, "VARCHAR", "FALSE", "", "64",  "") &&
            addColumnInFile("User", "Password", "password", "VARCHAR", "FALSE", "", "128",  "") &&
            addColumnInFile("User", "Permissions", "permissions", "SET", "FALSE", "", "",  optional.at("permissions")) &&
            createControllerFile("User") &&
            addRouteInFile("register", "POST", "register", "User", "register", "", "") &&
            addRouteInFile("login", "POST", "login", "User", "login", "", "") &&
            addRouteInFile("logout", "POST", "logout", "User", "logout", "TRUE", "") &&
            addActionInFile("User", "register") &&
            addActionInFile("User", "login") &&
            addActionInFile("User", "logout") &&
            addMethodInFile("./app/models/UserModel.php", "\
        public function hashPassword() : void\n\
        {\n\
            $this->data['password'] = password_hash($this->data['password'], PASSWORD_DEFAULT);\n\
        }\n\
\n\
        public function login() : bool\n\
        {\n\
            $data = (new \\F2P2\\QuerySelect(self::TABLE))\n\
                ->setWhere('"+identifier_snake+" = :"+identifier_snake+" AND password = :password')\n\
                ->setValues([\n\
                    '"+identifier_snake+"' => $this->get"+identifier_pascal+"(),\n\
                    'password' => $this->getPassword()\n\n\
                ])\n\
            ->execute();\n\
            if (count($data) === 0)\n\
            {\n\
                return false;\n\
            }\n\
            else\n\
            {\n\
                $_SESSION['user'] = $data[0];\n\
                return true;\n\
            }\n\
        }\n"
            )
        ) {
            infos->removeChild("NAME", "user");
            infos->addChild(
                (new InfoFile::InfoBlock("MODEL"))
                    ->addProperty("NAME", "user")
                    ->addChild(
                        (new InfoFile::InfoBlock("COLUMN"))
                            ->addProperty("NAME", identifier_snake)
                            ->addProperty("TYPE", "VARCHAR")
                            ->addProperty("SIZE", "64")
                    )
                    ->addChild(
                        (new InfoFile::InfoBlock("COLUMN"))
                            ->addProperty("NAME", "password")
                            ->addProperty("TYPE", "VARCHAR")
                            ->addProperty("SIZE", "128")
                    )
                    ->addChild(
                        (new InfoFile::InfoBlock("COLUMN"))
                            ->addProperty("NAME", "permissions")
                            ->addProperty("TYPE", "SET")
                            ->addProperty("ADDITIONAL", optional.at("permissions"))
                    )
            );
            saveDBInfos(infos);
            string content = readFile("./config/env.php");
            string search = "define('USE_AUTH',";
            int begin = content.find(search);
            if (begin != -1)
            {
                begin += search.length();
                while (begin < content.length() && content.at(begin) == ' ' || content.at(begin) == '\n')
                {
                    begin++;
                }
                int end = begin;
                while (end < content.length() && content.at(end) != ' ' || content.at(end) != '\n' || content.at(end) != ')')
                {
                    end++;
                }
                if (end < content.length())
                {
                    ofstream ofs("./config/env.php");
                    ofs
                        << content.substr(0, begin)
                        << "true"
                        << content.substr(end);
                }
            }
            cout << CLI::colorize("The auth system has been successfully initialized.", CLI::FG_LIGHTGREEN) << endl;
        }
        else
        {
            cout << CLI::colorize("Failed to initialize the auth system.", CLI::FG_LIGHTRED) << endl;
        }
        delete infos;
    }
    else
    {
        cout << CLI::colorize("No database initialized.", CLI::FG_LIGHTRED) << endl;
    }
}

void listDBTypes(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    cout
        << CLI::colorize("String Types", CLI::FG_LIGHTBLUE)                                                                              << endl
        << CLI::colorize("CHAR",         CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("VARCHAR",      CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("TINYTEXT",     CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("TEXT",         CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("MEDIUMTEXT",   CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("LONGTEXT",     CLI::FG_LIGHTYELLOW)                                                                            << endl
        << ""                                                                                                                        << endl
        << CLI::colorize("Time Types", CLI::FG_LIGHTBLUE)                                                                                << endl
        << CLI::colorize("DATE",       CLI::FG_LIGHTYELLOW)                                                                              << endl
        << CLI::colorize("DATETIME",   CLI::FG_LIGHTYELLOW)                                                                              << endl
        << CLI::colorize("TIMESTAMP",  CLI::FG_LIGHTYELLOW)                                                                              << endl
        << CLI::colorize("TIME",       CLI::FG_LIGHTYELLOW)                                                                              << endl
        << CLI::colorize("YEAR",       CLI::FG_LIGHTYELLOW)                                                                              << endl
        << ""                                                                                                                        << endl
        << CLI::colorize("Number Types", CLI::FG_LIGHTBLUE)                                                                              << endl
        << CLI::colorize("TINYINT",      CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("SMALLINT",     CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("MEDIUMINT",    CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("INTEGER", CLI::FG_LIGHTYELLOW) << CLI::colorize("|", CLI::FG_WHITE) << CLI::colorize("INT", CLI::FG_LIGHTYELLOW)  << endl
        << CLI::colorize("BIGINT",       CLI::FG_LIGHTYELLOW)                                                                            << endl
        << ""                                                                                                                        << endl
        << CLI::colorize("Float Types", CLI::FG_LIGHTBLUE)                                                                               << endl
        << CLI::colorize("FLOAT",       CLI::FG_LIGHTYELLOW)                                                                             << endl
        << CLI::colorize("DOUBLE",      CLI::FG_LIGHTYELLOW)                                                                             << endl
        << CLI::colorize("DECIMAL", CLI::FG_LIGHTYELLOW) << CLI::colorize("|", CLI::FG_WHITE) << CLI::colorize("DEC", CLI::FG_LIGHTYELLOW)  << endl
        << ""                                                                                                                        << endl
        << CLI::colorize("Boolean Types", CLI::FG_LIGHTBLUE)                                                                             << endl
        << CLI::colorize("BOOL", CLI::FG_LIGHTYELLOW) << CLI::colorize("|", CLI::FG_WHITE) << CLI::colorize("BOOLEAN", CLI::FG_LIGHTYELLOW) << endl
        << ""                                                                                                                        << endl
        << CLI::colorize("Binary Types", CLI::FG_LIGHTBLUE)                                                                              << endl
        << CLI::colorize("BINARY",       CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("VARBINARY",    CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("BIT",          CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("TINYBLOB",     CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("BLOB",         CLI::FG_LIGHTYELLOW)                                                                            << endl
        << CLI::colorize("LONGBLOB",     CLI::FG_LIGHTYELLOW)                                                                            << endl
        << ""                                                                                                                        << endl
        << CLI::colorize("Selection Types", CLI::FG_LIGHTBLUE)                                                                           << endl
        << CLI::colorize("ENUM",            CLI::FG_LIGHTYELLOW)                                                                         << endl
        << CLI::colorize("SET",             CLI::FG_LIGHTYELLOW)                                                                         << endl;
}

void updateMime(
    CLI::Command*       command,
    CLI::Parameters     required,
    CLI::Parameters     optional,
    CLI::ParameterArray list
) {
    cout << CLI::colorize("Retrieving data from ", CLI::FG_LIGHTCYAN) << CLI::colorize(MIME_TYPES_URL, CLI::FG_LIGHTYELLOW) << CLI::colorize(".", CLI::FG_LIGHTCYAN) << endl;
    string result = curlToString(MIME_TYPES_URL);
    if (result.empty())
    {
        cout << CLI::colorize("Failed to get data.", CLI::FG_LIGHTRED) << endl;
    }
    else
    {
        cout << CLI::colorize("Successfully obtaining list of MIME types.", CLI::FG_LIGHTGREEN) << endl;
        ofstream ofs("./config/mime.json");
        ofs << "{";
        bool error = false;
        bool first = true;
        int mime_begin = 0;
        while (!error && mime_begin < result.length())
        {
            if (result.at(mime_begin) == '#')
            {
                mime_begin = result.find("\n", mime_begin);
                if (mime_begin == -1)
                {
                    mime_begin = result.length();
                }
                else
                {
                    mime_begin++;
                }
            }
            else
            {
                int mime_end = mime_begin;
                while (mime_end < result.length() && result.at(mime_end) != ' ' && result.at(mime_end) != '\t' && result.at(mime_end) != '\n')
                {
                    mime_end++;
                }
                if (mime_end == result.length() || result.at(mime_end) == '\n')
                {
                    error = true;
                }
                else
                {
                    int extension_begin = mime_end;
                    while (extension_begin < result.length() && (result.at(extension_begin) == ' ' || result.at(extension_begin) == '\t'))
                    {
                        extension_begin++;
                    }
                    if (extension_begin == result.length())
                    {
                        error = true;
                    }
                    else
                    {
                        int extension_end = extension_begin;
                        while (extension_end < result.length() && result.at(extension_end) != '\n')
                        {
                            extension_end++;
                        }
                        string mime = result.substr(mime_begin, mime_end-mime_begin);
                        string extension = result.substr(extension_begin, extension_end-extension_begin);
                        string extensions;
                        int i = 0;
                        int j = 0;
                        int n = 0;
                        while (i < extension.length())
                        {
                            j = i;
                            while (j < extension.length() && extension.at(j) != ' ' && extension.at(j) != '\t')
                            {
                                j++;
                            }
                            if (extensions.length() > 0)
                            {
                                extensions += ", ";
                            }
                            extensions += "\""+extension.substr(i, j-i)+"\"";
                            i = j;
                            while (i < extension.length() && (extension.at(i) == ' ' || extension.at(i) == '\t'))
                            {
                                i++;
                            }
                            n++;
                        }
                        if (n > 1)
                        {
                            extensions = "["+extensions+"]";
                        }
                        if (first)
                        {
                            first = false;
                        }
                        else
                        {
                            ofs << ",";
                        }
                        ofs << endl << "    \"" << mime << "\" : " << extensions;
                        mime_begin = extension_end+1;
                    }
                }
            }
        }
        ofs << endl << "}";
        ofs.close();
        if (error)
        {
            cout << CLI::colorize("Failed to update the MIME JSON.", CLI::FG_LIGHTRED) << endl;
        }
        else
        {
            cout << CLI::colorize("The MIME JSON has been successfully updated.", CLI::FG_LIGHTGREEN) << endl;
        }
    }
}

void initCmd()
{
    cli.addCommand(
        (new CLI::Command(
            "help",
            "Show help for F2P2 commands",
            help
        ))->allowParameterList(
            "command",
            "Show help for this command"
        )
    )->addCommand(
        new CLI::Command(
            "update",
            "Update the f2p2 CLI",
            update
        )
    )->addCommand(
        (new CLI::Command(
            "project",
            "Execute actions on project"
        ))->addSubCommand(
            (new CLI::Command(
                "create",
                "Create a new F2P2 project",
                createProject
            ))->addRequiredParameter(
                "name",
                "The name of the project"
            )
        )
    )->addCommand(
        (new CLI::Command(
            "route",
            "Execute actions on routes"
        ))->addSubCommand(
            (new CLI::Command(
                "create",
                "Create a new route",
                createRoute
            ))->addRequiredParameter(
                "name",
                "The name of the route"
            )->addOptionalParameter(
                "method",
                "The route access method",
                "m",
                "GET|POST",
                "GET"
            )->addOptionalParameter(
                "uri",
                "The route URI",
                "u",
                "uri",
                "/"
            )->addOptionalParameter(
                "controller",
                "The route controller",
                "c",
                "controller",
                "Home"
            )->addOptionalParameter(
                "action",
                "The action of the route in the controller",
                "a",
                "action",
                "index"
            )->addOptionalParameter(
                "logged",
                "If the user have to be logged to access to this route",
                "l",
                "TRUE|FALSE"
            )->addOptionalParameter(
                "permissions",
                "The required permissions to access to this route",
                "p",
                "perm0,perm1,..."
            )
        )->addSubCommand(
            (new CLI::Command(
                "error",
                "Bind an error route",
                errorRoute
            ))->addRequiredParameter(
                "code",
                "The error code"
            )
        )
    )->addCommand(
        (new CLI::Command(
            "controller",
            "Execute actions on controllers"
        ))->addSubCommand(
            (new CLI::Command(
                "create",
                "Create a new controller",
                createController
            ))->addRequiredParameter(
                "name",
                "The name of the controller"
            )
        )
    )->addCommand(
        (new CLI::Command(
            "database",
            "Execute actions on the database"
        ))->addSubCommand(
            (new CLI::Command(
                "init",
                "Initialize the database of the project",
                initDatabase
            ))->addRequiredParameter(
                "name",
                "The name of the database"
            )->addOptionalParameter(
                "type",
                "The database type",
                "t",
                "type",
                "mysql"
            )->addOptionalParameter(
                "host",
                "The database host",
                "h",
                "host",
                "localhost"
            )->addOptionalParameter(
                "user",
                "The database user",
                "u",
                "user",
                "root"
            )->addOptionalParameter(
                "pass",
                "The database pass",
                "p",
                "pass"
            )
        )->addSubCommand(
            new CLI::Command(
                "update",
                "Update the database of the project",
                updateDatabase
            )
        )
    )->addCommand(
        (new CLI::Command(
            "model",
            "Execute actions on models"
        ))->addSubCommand(
            (new CLI::Command(
                "create",
                "Create a new model",
                createModel
            ))->addRequiredParameter(
                "name",
                "The name of the model"
            )
        )->addSubCommand(
            (new CLI::Command(
                "delete",
                "Delete an existing model",
                deleteModel
            ))->addRequiredParameter(
                "name",
                "The name of the model"
            )
        )
    )->addCommand(
        (new CLI::Command(
            "column",
            "Execute actions on model columns"
        ))->addSubCommand(
            (new CLI::Command(
                "set",
                "Create or update a column in a model",
                setColumn
            ))->addRequiredParameter(
                "model",
                "The name of the model"
            )->addRequiredParameter(
                "name",
                "The name of the column"
            )->addOptionalParameter(
                "type",
                "The value type of the column",
                "t",
                "type",
                "INT"
            )->addOptionalParameter(
                "null",
                "If the value can be null",
                "n",
                "TRUE|FALSE",
                "FALSE"
            )->addOptionalParameter(
                "reference",
                "Configure a reference to a column from another table",
                "r",
                "model"
            )->addOptionalParameter(
                "size",
                "The size of the column (only for certain types)",
                "s",
                "size"
            )->addOptionalParameter(
                "additional",
                "An additional column parameter (only for certain types)",
                "a",
                "value"
            )
        )->addSubCommand(
            (new CLI::Command(
                "delete",
                "Delete a column in a model",
                deleteColumn
            ))->addRequiredParameter(
                "model",
                "The name of the model"
            )->addRequiredParameter(
                "name",
                "The name of the column"
            )
        )
    )->addCommand(
        (new CLI::Command(
            "auth",
            "Execute actions on authentification"
        ))->addSubCommand(
            (new CLI::Command(
                "init",
                "Initialize the authentification system for the project",
                initAuth
            ))->addOptionalParameter(
                "identifier",
                "The name of the identifier",
                "i",
                "identifier",
                "email"
            )->addOptionalParameter(
                "permissions",
                "The list of permissions",
                "p",
                "perm0,perm1,...",
                "admin"
            )
        )
    )->addCommand(
        (new CLI::Command(
            "list",
            "Show informations"
        ))->addSubCommand(
            new CLI::Command(
                "dbtypes",
                "Show allowed database value types",
                listDBTypes
            )
        )
    )->addCommand(
        (new CLI::Command(
            "mime",
            "Execute actions about the MIME types"
        ))->addSubCommand(
            new CLI::Command(
                "update",
                "Update the MIME types JSON",
                updateMime
            )
        )
    );
}

void initInfoTemplates()
{
    DATABASE_TEMPLATE = (new InfoFile::InfoTemplate("DATABASE"))
        ->addChild(
            (new InfoFile::InfoTemplate("MODEL"))
                ->addProperty("NAME", true)
                ->addChild(
                    (new InfoFile::InfoTemplate("COLUMN"))
                        ->addProperty("NAME", true)
                        ->addProperty("TYPE")
                        ->addProperty("NULL")
                        ->addProperty("REFERENCE")
                        ->addProperty("SIZE")
                        ->addProperty("ADDITIONAL")
                )
        );
}

void init()
{
    CONSOLE_HOUT = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(CONSOLE_HOUT, &CONSOLE_OUTMODE);
    SetConsoleMode(CONSOLE_HOUT, CONSOLE_OUTMODE|ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    initCmd();
    initInfoTemplates();
}

void cleanup()
{
    delete DATABASE_TEMPLATE;
    SetConsoleMode(CONSOLE_HOUT, CONSOLE_OUTMODE);
}

int main(
    int   argc,
    char* argv[]
) {
    if (argc == 2 && ((string)argv[1]) == "cleanup")
    {
        destroyFileIfExists("f2p2-old.exe");
        destroyFileIfExists("f2p2-restart.bat");
    }
    else
    {
        init();
        if (!(argc == 2 && ((string)argv[1]) == "update"))
        {
            string version = getLatestCLIVersion();
            if (!version.empty() && version != VERSION)
            {
                cout
                    << CLI::colorize("A new version of the F2P2 CLI can be downloaded ", CLI::FG_LIGHTCYAN) << CLI::colorize("(", CLI::FG_WHITE) << CLI::colorize("v"+((string)VERSION), CLI::FG_LIGHTGREEN) << CLI::colorize(" => ", CLI::FG_WHITE) << CLI::colorize("v"+version, CLI::FG_LIGHTGREEN) << CLI::colorize(")", CLI::FG_WHITE) << CLI::colorize(".", CLI::FG_LIGHTCYAN) << endl
                    << CLI::colorize("Use ", CLI::FG_LIGHTCYAN) << CLI::colorize("f2p2 update", CLI::FG_LIGHTYELLOW) << CLI::colorize(" to download it.", CLI::FG_LIGHTCYAN)                                                                                                                                                                                                          << endl
                    << ""                                                                                                                                                                                                                                                                                                                                               << endl;
            }
        }
        if (argc == 1)
        {
            hello();
        }
        else
        {
            cli.execute(argc, argv);
        }
        cleanup();
    }
    return 0;
}