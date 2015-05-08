/*
 * name: stringUtils.cpp
 * OBJ: some useful methods applied to string object
 * author: jlpeng
 * create: 2012-07-10
 * update: 2012-07-11
 * dependence:
 *   None
 */
#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <vector>
#include <string>
using std::string;

std::vector<string> split(const string &str, const char *sep);
std::vector<string> split(const char *str, const char *sep);

string join(const std::vector<string> &str, const char *sep);
string join(char *str[], const char *sep, int n);

#endif
