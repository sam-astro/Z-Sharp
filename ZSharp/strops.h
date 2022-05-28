#ifndef STROPS_H
#define STROPS_H

#include <boost/any.hpp>

using namespace std;


float AnyAsFloat(const boost::any& val);
string AnyAsString(const boost::any& val);
int AnyAsInt(const boost::any& val);
bool AnyAsBool(const boost::any& val);



bool isNumber(const string& str);

bool stob(const string& str);

string StringRaw(const string& s);

string Quoted(const string& s);

string RMParenthesis(const string& s);

string ltrim(const string& s);

string rtrim(const string& s);

string trim(const string& s);

vector<string> split(const string& str, const char& del);

string betweenChars(const string& str, const char& openChar, const char& closeChar);

vector<string> splitNoOverlap(const string& str, const char& splitBy, const char& openChar, const char& closeChar);

int count(const string& str, const char& ch);

int countNoOverlap(const string& str, const char& searchFor, const char& ch1, const char& ch2);

int countOutsideParenthesis(const string& str, const char& searchFor);

int indexInStr(const string& str, const char& ch);

int charIndexInVec(const vector<string>& str, const char& ch);

int countInVector(const vector<string>& str, const string& ch);

string Vec2Str(const vector<string>& str);

vector<vector<string>> removeTabsWdArry(const vector<vector<string>>& str, const int& amnt);

vector<string> removeTabs(const vector<string>& str, const int& amnt);

vector<string> rangeInVec(const vector<string>& str, const int& min, int max);

vector<string> slice(vector<string> const& v, int min, int max);

string rangeInStr(const string& str, const int& min, int max);

string unWrapVec(const vector<string>& vec);

float floatval(const string& s);

string replace(const string& str, const string& strToReplace, const string& replaceWith);

bool isEscaped(const string& str, int curChar);

bool startsWith(const string& str, const string& lookFor);

std::string escaped(const std::string& input);

#endif
