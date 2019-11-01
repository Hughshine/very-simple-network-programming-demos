#include "common.h"
string to_upper(string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}
bool hasEnding(string const &str, string const &end)
{
    if (str.length() >= end.length())
    {
        return (0 == str.compare(str.length() - end.length(), str.length(), end));
    }
    else
    {
        return false;
    }
}
void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);

    // Repeat till end is reached
    while (pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos = data.find(toSearch, pos + replaceStr.size());
    }
}

const std::string currentTime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
    return buf;
}

const std::string currentDate()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return buf;
}