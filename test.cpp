
#include <string>
#include <iostream>
#include <vector>
#include <map>

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

std::vector<std::string> splitString(std::string str, char delimiter, char end)
{
    std::vector<std::string> vec;
    size_t i;

    do
    {
        i = str.find(delimiter);
        if (i != 0)
            vec.push_back(str.substr(0, i));
        str.erase(0, i);
        while (str[0] == delimiter)
            str.erase(0, 1);
    }
    while (!str.empty() && i != std::string::npos && str[0] != end);

    if (!str.empty())
        vec.push_back(str);

    return (vec);
}

int main(int ac, char **av)
{
    if (ac == 1)
        return (0);

    std::string str(av[1]);
    std::vector<std::string> vec;

    vec = splitString(str, ' ', ':');

    vec.insert(vec.begin(), "coucou");
    // vec.erase(vec.begin() + 1);

    for (size_t i = 0; i < vec.size(); i++)
        std::cout << i << " : " << vec[i] << std::endl;

    str.clear();
    if (str.empty())
        std::cout << "str is empty" << std::endl;
    else
        std::cout << "str is NOT empty" << std::endl;

    std::map<std::string, int> _users;
    _users.insert(std::pair<std::string, int>("Sam", 1));
    _users.insert(std::pair<std::string, int>("Acomet", 2));
    _users.insert(std::pair<std::string, int>("Siobhan", 3));

    std::map<std::string, int>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }

    std::string message = ":Hello to everybody !!!";
    std::string name = "#chan";
    message.insert(0, name.substr(1));
    message.insert(0, "Sam@");
    std::cout << message << std::endl;

    return (0);
}