/*
 * name: stringUtils.cpp
 * OBJ: definition of methods declared in `stringUtils.hpp`
 * author: jlpeng
 * create: 2012-07-10
 * update: 2012-07-12
 */
#include <stringUtils/stringUtils.hpp>
#include <vector>
#include <string>
#include <cstring>

using std::vector;
using std::string;

vector<string> split(const string &str, const char *sep)
{
    vector<string> str_split;
    string::size_type start_pos=0, find_pos;
    string::size_type sep_len = strlen(sep);
    while((start_pos+sep_len <= str.size()) && ((find_pos=str.find(sep, start_pos)) != string::npos))
    {
        if(find_pos == start_pos)  //sequential seperators are merged!!
            start_pos += sep_len;
        else {
            str_split.push_back(str.substr(start_pos, find_pos-start_pos));
            start_pos = find_pos + sep_len;
        }
    }
    if(start_pos < str.size())
        str_split.push_back(str.substr(start_pos));

    return str_split;
}
vector<string> split(const char *str, const char *sep)
{
    vector<string> str_split;
    const char *start_pos = str, *find_pos;
    int str_len = strlen(str), sep_len = strlen(sep);

    while((str+str_len-start_pos >= sep_len) && ((find_pos = strstr(start_pos, sep))) != NULL)
    {
        if(find_pos == start_pos)
            start_pos += sep_len;
        else {
            str_split.push_back(string(start_pos, find_pos-start_pos));
            start_pos = find_pos + sep_len;
        }
    }
    if((start_pos-str) < str_len)
        str_split.push_back(start_pos);

    return str_split;
}

string join(const vector<string> &str, const char *sep)
{
    string join_str(str[0]);
    for(vector<string>::size_type i=1; i < str.size(); i++)
        join_str += (string(sep) + str[i]);
    return join_str;
}
string join(char *str[], const char *sep, int n)
{
    string join_str(str[0]);
    for(int i=1; i<n; i++)
        join_str += (string(sep) + string(str[i]));
    return join_str;
}
