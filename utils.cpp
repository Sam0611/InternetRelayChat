
#include <vector>
#include <string>

std::vector<std::string> splitString(std::string str, char delimiter)
{
    std::vector<std::string> vec;
    size_t i;

    do
    {
        i = str.find(delimiter);
        vec.push_back(str.substr(0, i));
        str.erase(0, i);
        while (str[0] == delimiter)
            str.erase(0, 1);
    }
    while (!str.empty() && i != std::string::npos);

    return (vec);
}

std::vector<std::string> splitString(std::string str, char delimiter, int n)
{
    std::vector<std::string> vec;
    size_t i;

    do
    {
        i = str.find(delimiter);
        vec.push_back(str.substr(0, i));
        str.erase(0, i);
        while (str[0] == delimiter)
            str.erase(0, 1);
        n--;
    }
    while (!str.empty() && i != std::string::npos && n > 0);

    if (!str.empty())
        vec.push_back(str);

    return (vec);
}