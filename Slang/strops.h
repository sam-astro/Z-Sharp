#ifndef STROPS_H
#define STROPS_H

using namespace std;

string ltrim(const string& s);

string rtrim(const string& s);

string trim(const string& s);

vector<string> split(string str, char del);

int count(string str, char ch);

int countNoOverlap(string str, char ch1, char ch2);

int indexInStr(string str, char ch);

int charIndexInVec(vector<string> str, char ch);

int countInVector(vector<string> str, string ch);

string Vec2Str(vector<string> str);

vector<string> removeTabs(vector<string> str, int amnt);

vector<string> rangeInVec(vector<string> str, int min, int max);

string rangeInStr(string str, int min, int max);

string unWrapVec(vector<string> vec);

float floatval(string s);

string replace(string str, string strToReplace, string replaceWith);

#endif