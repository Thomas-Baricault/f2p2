#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <dirent.h>


using namespace std;


string readFile(
    string path
) {
    string content;
    ifstream ifs(path.c_str());
    if (ifs.is_open())
    {
        stringstream buffer;
        buffer << ifs.rdbuf();
        ifs.close();
        content = buffer.str();
    }
    return content;
}

string escape(
    string str
) {
    string result;
    for (char c : str)
    {
        if (c == '\\' || c == '"' || c == '\n')
        {
            result += '\\';
        }
        if (c == '\n')
        {
            result += 'n';
        }
        else
        {
            result += c;
        }
    }
    return result;
}

void scan(
    string    start_path,
    string    path,
    ofstream& ofs
) {
    string full_path = start_path+path;
    cout << full_path << endl;
    DIR* dir = opendir(full_path.c_str());
    if (dir == NULL)
    {
        ofs
            << ""                                                  << endl
            << "ofs = ofstream(\"." << path << "\");"               << endl
            << "ofs << \"" << escape(readFile(full_path)) << "\";" << endl
            << "ofs.close();";
    }
    else
    {
        if (path.length() > 1)
        {
            ofs << endl << "mkdir(\"." << path << "\");";
        }
        struct dirent* element;
        while ((element = readdir(dir)))
        {
            if (
                strcmp(element->d_name, ".") != 0 &&
                strcmp(element->d_name, "..") != 0 &&
                strcmp(element->d_name, ".hold") != 0 &&
                strcmp(element->d_name, "tool") != 0 &&
                strcmp(element->d_name, "todo.txt") != 0
            ) {
                scan(start_path, path+"/"+element->d_name, ofs);
            }
        }
        closedir(dir);
    }
}

int main()
{
    ofstream ofs("initproject.txt");
    scan("..", "", ofs);
    ofs.close();
    return 0;
}