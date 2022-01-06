#ifndef STROPS_H
#define STROPS_H

#include <boost/any.hpp>

using namespace std;

bool isNumber(const string& str);

bool stob(const string& str);

string StringRaw(const string& s);

string Quoted(const string& s);

string RMParenthesis(const string& s);

string ltrim(const string& s);

string rtrim(const string& s);

string trim(const string& s);

vector<string> split(const string& str, const char& del);

int count(const string& str, const char& ch);

int countNoOverlap(const string& str, const char& ch1, const char& ch2);

int indexInStr(const string& str, const char& ch);

int charIndexInVec(const vector<string>& str, const char& ch);

int countInVector(const vector<string>& str, const string& ch);

string Vec2Str(const vector<string>& str);

vector<string> removeTabs(const vector<string>& str, const int& amnt);

vector<string> rangeInVec(const vector<string>& str, const int& min, const int& max);

string rangeInStr(const string& str, const int& min, const int& max);

string unWrapVec(const vector<string>& vec);

float floatval(const string& s);

string replace(const string& str, const string& strToReplace, const string& replaceWith);

#endif
