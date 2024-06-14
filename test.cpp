
#include <string>
#include <iostream>
#include <vector>

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

int main(int ac, char **av)
{
    if (ac == 1)
        return (0);

    std::string str(av[1]);
    std::vector<std::string> vec;

    vec = splitString(str, ' ');

    vec.insert(vec.begin(), "coucou");

    for (size_t i = 0; i < vec.size(); i++)
        std::cout << i << " : " << vec[i] << std::endl;

    return (0);
}