#ifndef STRINGCASE
#define STRINGCASE


#include <string>


using namespace std;


namespace StringCase
{
    string pascalToSnake(
        string pascal_str
    ) {
        bool already_char = false;
        string snake_str;
        for (char c : pascal_str)
        {
            if (c == '_')
            {
                already_char = false;
                snake_str += c;
            }
            else if ('a' <= c && c <= 'z')
            {
                already_char = true;
                snake_str += c;
            }
            else if ('A' <= c && c <= 'Z')
            {
                if (already_char)
                {
                    snake_str += '_';
                }
                else
                {
                    already_char = true;
                }
                snake_str += c-'A'+'a';
            }
        }
        return snake_str;
    }

    string snakeToPascal(
        string snake_str
    ) {
        string pascal_str;
        bool need_upper = true;
        for (char c : snake_str)
        {
            if (c == '_')
            {
                need_upper = true;
            }
            else if ('a' <= c && c <= 'z')
            {
                if (need_upper)
                {
                    need_upper = false;
                    pascal_str += c-'a'+'A';
                }
                else
                {
                    pascal_str += c;
                }
            }
            else if ('A' <= c && c <= 'Z')
            {
                need_upper = false;
                pascal_str += c;
            }
        }
        return pascal_str;
    }
}


#endif