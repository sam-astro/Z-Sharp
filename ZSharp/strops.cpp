#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <boost/any.hpp>
#include "strops.h"
//#include "builtin.h"
using namespace std;

const string WHITESPACE = " \t\f";
//const string WHITESPACE = " \n\r\t\f\v";


bool isNumber(const string& str)
{
	for (char const& c : str) {
		if (isdigit(c) == 0 && c != '.') return false;
	}
	return true;
}

bool stob(const string& str)
{
	bool b = trim(str) == "true";
	return b;
}

string unescape(const string& s)
{
	string res;
	string::const_iterator it = s.begin();
	while (it != s.end())
	{
		char c = *it++;
		if (c == '\\' && it != s.end())
		{
			switch (*it++) {
			case '\\': c = '\\'; break;
			case 'n': c = '\n'; break;
			case 't': c = '\t'; break;
				// all other escapes
			default:
				// invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
				continue;
			}
		}
		res += c;
	}

	return res;
}

std::string escaped(const std::string& input)
{
	std::string output;
	output.reserve(input.size());
	for (const char c : input) {
		switch (c) {
		case '\a':  output += "\\a";        break;
		case '\b':  output += "\\b";        break;
		case '\f':  output += "\\f";        break;
		case '\n':  output += "\\n";        break;
		case '\r':  output += "\\r";        break;
		case '\t':  output += "\\t";        break;
		case '\v':  output += "\\v";        break;
		default:    output += c;            break;
		}
	}

	return output;
}

string StringRaw(const string& s)
{
	string str = trim(s);

	if (str.size() < 3)
		return str;

	string withoutQuotes;

	if (str[0] != '\"')
		withoutQuotes += str[0];

	withoutQuotes += str.substr(1, str.size() - 2);

	if (str[str.size() - 1] != '\"')
		withoutQuotes += str[str.size() - 1];

	return unescape(withoutQuotes);
}

string Quoted(const string& s)
{
	string str = trim(s);

	string withQuotes;

	if (str[0] != '\"')
		withQuotes += '\"';

	withQuotes += str;

	if (str[str.size() - 1] != '\"')
		withQuotes += '\"';

	return withQuotes;
}

string RMParenthesis(const string& s)
{
	string str = trim(s);
	string withoutParenthesis;

	if (str[0] != '(')
		withoutParenthesis += str[0];

	withoutParenthesis += str.substr(1, str.size() - 2);

	if (str[str.size() - 1] != ')')
		withoutParenthesis += str[str.size() - 1];

	return withoutParenthesis;
}

string ltrim(const string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s) {
	return rtrim(ltrim(s));
}

vector<string> split(const string& str, const char& del) {
	if (count(str, del) == 0)
		return vector<string>{str};

	// declaring temp string to store the curr "word" upto del
	string temp = "";
	vector<string> splitWords;

	for (int i = 0; i < (int)str.size(); i++)
	{
		// If cur char is not del, then append it to the cur "word", otherwise
		// you have completed the word, print it, and start a new word.
		if (str[i] != del)
		{
			temp += str[i];
		}
		else
		{
			splitWords.push_back(temp);
			temp = "";
		}
	}
	splitWords.push_back(temp);

	return splitWords;
}

int count(const string& str, const char& ch) {
	int cnt = 0;

	for (int i = 0; i < (int)str.size(); i++)
		if (str[i] == ch)
			cnt++;

	return cnt;
}

int countNoOverlap(const string& str, const char& searchFor, const char& ch1, const char& ch2)
{
	int cnt = 0;

	bool waitingForClose = false;

	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == ch1)
			waitingForClose = true;
		else if (str[i] == ch2 && waitingForClose == true)
			waitingForClose = false;
		else if (str[i] == searchFor && waitingForClose == true)
			cnt++;
	}

	return cnt;
}

vector<string> splitNoOverlap(const string& str, const char& splitBy, const char& openChar, const char& closeChar)
{
	vector<string> newStr;

	int openCount = 0;

	string tmpStr = "";
	for (int i = 0; i < (int)str.size(); i++)
	{
		if (i == (int)str.size() - 1)
		{
			newStr.push_back(trim(tmpStr + str[i]));
			break;
		}

		if (str[i] == splitBy && openCount == 0)
		{
			newStr.push_back(trim(tmpStr));
			tmpStr = "";
			continue;
		}
		else if (str[i] == openChar) {
			tmpStr += str[i];
			openCount += 1;
		}
		else if (str[i] == closeChar) {
			tmpStr += str[i];
			openCount -= 1;
		}
		else
			tmpStr += str[i];
	}

	return newStr;
}

string betweenChars(const string& str, const char& openChar, const char& closeChar)
{
	string content = "";

	int startPos = 0;
	int endPos = (int)str.size();

	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == openChar){
			startPos = i+1;
			break;
		}
	}
	for (int i = (int)str.size()-1; i >=0; i--)
	{
		if (str[i] == closeChar){
			endPos = i-(startPos); // or startPos-1 idk I cant do math right now
			break;
		}
	}

	return str.substr(startPos, endPos);
}

bool startsWith(const string& str, const string& lookFor)
{
	if (str.empty() || lookFor.size() > str.size())
		return false;

	for (int i = 0; i < (int)lookFor.size(); i++)
	{
		if (str[i] != lookFor[i])
			return false;
	}

	return true;
}

int countOutsideParenthesis(const string& str, const char& searchFor)
{
	int cnt = 0;

	bool waitingForClose = false;

	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == '(')
			waitingForClose = true;
		else if (str[i] == ')' && waitingForClose == true)
			waitingForClose = false;
		else if (str[i] == searchFor && waitingForClose == false)
			cnt++;
	}

	return cnt;
}

int indexInStr(const string& str, const char& ch) {

	for (int i = 0; i < (int)str.size(); i++)
		if (str[i] == ch)
			return i;

	return -1;
}

int charIndexInVec(const vector<string>& str, const char& ch) {

	for (int i = 0; i < (int)str.size(); i++)
		for (int w = 0; w < (int)str[i].size(); w++)
			if (str[i][w] == ch)
				return i;

	return -1;
}

int countInVector(const vector<string>& str, const string& ch) {
	int cnt = 0;

	for (int i = 0; i < (int)str.size(); i++)
		if (str[i] == ch)
			cnt++;

	return cnt;
}

string Vec2Str(const vector<string>& str) {
	string outStr;

	for (int i = 0; i < (int)str.size(); i++)
		outStr += str[i] + "\n";

	return outStr;
}

vector<string> removeTabs(const vector<string>& str, const int& amnt) {
	vector<string> newStr;

	for (int i = 0; i < (int)str.size(); i++)
	{
		newStr.push_back("");

		for (int c = 0; c < (int)str[i].size(); c++)
		{
			if (str[i][c] != '\t' || c >= amnt)
				newStr[i] += str[i][c];
		}
	}

	return newStr;
}

vector<vector<string>> removeTabsWdArry(const vector<vector<string>>& str, const int& amnt) {
	vector<vector<string>> newWds;

	for (int i = 0; i < (int)str.size(); i++)
	{
		//newWds.push_back(rangeInVec(str[i], amnt, -1));
		newWds.push_back(vector<string>());

		for (int c = 0; c < (int)str[i].size(); c++)
		{
			if (str[i][c][0] != '\t' || c >= amnt)
				newWds[i].push_back(str[i][c]);
			else
				newWds[i].push_back(str[i][c].substr(1, -1));
		}
	}

	return newWds;
}

vector<string> rangeInVec(const vector<string>& str, const int& min, int max) {
	if (max == -1)
		max = (int)str.size();

	vector<string> newStr;

	for (int i = min; i < (int)str.size() && i < max; i++)
		newStr.push_back(str[i]);

	return newStr;
}

vector<string> slice(vector<string> const& v, int min, int max)
{
	if (max == -1)
		max = (int)v.size() - 1;

	auto first = v.cbegin() + min;
	auto last = v.cbegin() + max + 1;

	vector<string> vec(first, last);
	return vec;
}

string rangeInStr(const string& str, const int& min, int max) {
	if (max == -1)
		max = (int)str.size();

	string newStr;

	for (int i = min; i < (int)str.size() && i < max; i++)
		newStr += str[i];

	return newStr;
}

string unWrapVec(const vector<string>& vec) {
	string newStr;

	for (int i = 0; i < (int)vec.size(); i++)
	{
		newStr += vec[i];
		if (i != (int)vec.size() - 1)
			newStr += " ";
	}

	return newStr;
}

float floatval(const string& s)
{
	float outfloat;

	if (s == "inf")
		outfloat = numeric_limits<float>::max();
	else if (s == "-inf")
		outfloat = -numeric_limits<float>::max();
	else if (s == "")
		outfloat = 0;
	else
		outfloat = stof(s);

	return outfloat;
}

string replace(const string& str, const string& strToReplace, const string& replaceWith) {
	string newStr;
	string savedLetters;

	int sameLetters = 0;
	int startReplaceIndex = 0;
	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == strToReplace[sameLetters])
		{
			savedLetters += str[i];
			if (sameLetters == 0)
				startReplaceIndex = i;
			sameLetters++;

			if ((int)strToReplace.size() == sameLetters)
			{
				//cout << "replaced " << "\"" << strToReplace << "\"" << startReplaceIndex << endl;
				newStr += replaceWith;
				sameLetters = 0;
				savedLetters = "";
			}
		}
		else
		{
			newStr += savedLetters + str[i];
			sameLetters = 0;
			savedLetters = "";
		}
	}

	return newStr;
}

bool isEscaped(const string& str, int curChar)
{
	if (curChar > 0)
		if (str[curChar - 1] == '\\')
			return true;

	return false;
}