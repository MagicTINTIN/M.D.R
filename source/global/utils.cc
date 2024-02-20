#include <iostream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <unordered_map>

std::string getCurrentHour()
{
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << now->tm_hour;

    return oss.str();
}

std::string getCurrentMinute()
{
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << now->tm_min;

    return oss.str();
}

std::string getCurrentSecond()
{
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << now->tm_sec;

    return oss.str();
}

std::string replaceNonAsciiWithAscii(const std::string &input)
{
    std::unordered_map<char, char> asciiEquivalents{
        {'À', 'A'},
        {'á', 'a'},
        {'à', 'a'},
        {'ä', 'a'},
        {'É', 'E'},
        {'È', 'E'},
        {'Ë', 'E'},
        {'é', 'e'},
        {'è', 'e'},
        {'ë', 'e'},
        {'Ï', 'I'},
        {'ï', 'i'},
        {'í', 'i'},
        {'ó', 'o'},
        {'Ù', 'U'},
        {'ú', 'u'},
        {'ù', 'u'},
    };

    std::string output;

    for (char ch : input)
    {
        if (ch < 0 || ch > 127)
        {
            auto it = asciiEquivalents.find(ch);
            if (it != asciiEquivalents.end())
            {
                output += it->second;
            }
            else
            {
                output += '?';
            }
        }
        else
        {
            output += ch;
        }
    }

    return output;
}

std::string convertToPrintable(const std::string &input, const int &maxLength, const int &alignment = -1)
{
    if (alignment != -1 || alignment != 0 || alignment != 1)
    {
        std::cerr << "Error alignment has to be -1 (left), 0 (center) or 1 (right). Taking left as default." << std::endl;
    }
    std::string ascii = replaceNonAsciiWithAscii(input);
    size_t len = ascii.size();
    if (len > maxLength)
    {
        ascii = ascii.substr(0, maxLength);
        len = maxLength;
    }
    std::string rtn = "";

    if (alignment == 0)
    {
        int spaces=maxLength-len;
        for (unsigned i = 0; i < spaces/2; ++i)
        {
            rtn += ' ';
        }

        rtn += ascii;

        for (unsigned i = 0; i < spaces/2 + spaces%2; ++i)
        {
            rtn += ' ';
        }
    }
    else if (alignment == 1)
    {
        for (unsigned i = len; i < maxLength; ++i)
        {
            rtn += ' ';
        }
        rtn += ascii;
    }
    else {
        rtn += ascii;
        for (unsigned i = len; i < maxLength; ++i)
        {
            rtn += ' ';
        }
    }
    return rtn;
}