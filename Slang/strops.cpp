#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include "strops.h"
using namespace std;

const string WHITESPACE = " \n\r\t\f\v";

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

vector<string> split(string str, char del) {
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

int count(string str, char ch) {
	int cnt = 0;

	for (int i = 0; i < (int)str.size(); i++)
		if (str[i] == ch)
			cnt++;

	return cnt;
}

int countNoOverlap(string str, char ch1, char ch2) {
	int cnt = 0;

	bool waitingForClose = false;

	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == ch1)
			waitingForClose = true;
		else if (str[i] == ch2 && waitingForClose == true)
		{
			cnt++;
			waitingForClose = false;
		}
	}

	return cnt;
}

int indexInStr(string str, char ch) {

	for (int i = 0; i < (int)str.size(); i++)
		if (str[i] == ch)
			return i;

	return -1;
}

int charIndexInVec(vector<string> str, char ch) {

	for (int i = 0; i < (int)str.size(); i++)
		for (int w = 0; w < (int)str[i].size(); w++)
			if (str[i][w] == ch)
				return i;

	return -1;
}

int countInVector(vector<string> str, string ch) {
	int cnt = 0;

	for (int i = 0; i < (int)str.size(); i++)
		if (str[i] == ch)
			cnt++;

	return cnt;
}

string Vec2Str(vector<string> str) {
	string outStr;

	for (int i = 0; i < (int)str.size(); i++)
		outStr += str[i] + "\n";

	return outStr;
}

vector<string> removeTabs(vector<string> str, int amnt) {
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

vector<string> rangeInVec(vector<string> str, int min, int max) {
	if (max == -1)
		max = (int)str.size();

	vector<string> newStr;

	for (int i = min; i < (int)str.size() && i < max; i++)
		newStr.push_back(str[i]);

	return newStr;
}

string rangeInStr(string str, int min, int max) {
	if (max == -1)
		max = (int)str.size();

	string newStr;

	for (int i = min; i < (int)str.size() && i < max; i++)
		newStr += str[i];

	return newStr;
}

string unWrapVec(vector<string> vec) {
	string newStr;

	for (int i = 0; i < (int)vec.size(); i++)
	{
		newStr += vec[i];
		if (i != (int)vec.size() - 1)
			newStr += " ";
	}

	return newStr;
}

float floatval(string s)
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

string replace(string str, string strToReplace, string replaceWith) {
	string newStr;
	string savedLetters;;

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