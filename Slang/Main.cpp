#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>

using namespace std;

vector<string> types = { "int", "float", "string", "bool", "void", "null" };

vector<string> globalVariables;
vector<string> globalVariableValues;
vector<string> functions;
vector<vector<string>> functionValues;

class Parser : public olc::PixelGameEngine
{
public:
	Parser()
	{
		sAppName = "Parser";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 128, rand() % 128, rand() % 128));
		return true;
	}
};

int ExecuteFunction(string functionName, vector<string> inputVarVals);

bool isNumber(const string& str)
{
	for (char const& c : str) {
		if (isdigit(c) == 0 && c != '.') return false;
	}
	return true;
}

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

string AddItem(string varName, string variableContent, string addItem)
{
	string typ = split(varName, ' ')[0];

	if (typ == "int" || typ == "float" || typ == "double" && isNumber(addItem))
	{
		return to_string(floatval(to_string(floatval(variableContent) + floatval(addItem))));
	}
	else
	{
		return variableContent + addItem;
	}
}

string GetRealValue(string varName, vector<string>& variables, vector<string>& variableVals)
{
	string typ = "string";
	bool isVar = false;

	// Checks against global vars
	for (int v = 0; v < (int)globalVariables.size(); v++)
		if (varName == split(globalVariables[v], ' ')[1])
		{
			typ = split(globalVariables[v], ' ')[0];
			isVar = true;
		}
	// Checks against local vars
	for (int v = 0; v < (int)variables.size(); v++)
		if (varName == split(variables[v], ' ')[1])
		{
			typ = split(variables[v], ' ')[0];
			isVar = true;
		}

	//if ((typ == "int" || typ == "float" || typ == "double") && isVar)
	//{
	//	// Checks against global vars
	//	for (int v = 0; v < (int)globalVariables.size(); v++)
	//		if (varName == split(globalVariables[v], ' ')[1])
	//		{
	//			if (globalVariableValues[v] == "inf")
	//				globalVariableValues[v] = 2147483646;
	//			if (globalVariableValues[v] == "-inf")
	//				globalVariableValues[v] = -2147483646;
	//		}
	//	// Checks against local vars
	//	for (int v = 0; v < (int)variables.size(); v++)
	//		if (varName == split(variables[v], ' ')[1])
	//		{
	//			if (variableVals[v] == "inf")
	//				variableVals[v] = 2147483646;
	//			if (variableVals[v] == "-inf")
	//				variableVals[v] = -2147483646;
	//		}
	//}

	// If it is a var
	if (isVar)
	{
		// Checks against global vars
		for (int v = 0; v < (int)globalVariables.size(); v++)
			if (varName == split(globalVariables[v], ' ')[1])
			{
				return globalVariableValues[v];
			}
		// Checks against local vars
		for (int v = 0; v < (int)variables.size(); v++)
			if (varName == split(variables[v], ' ')[1])
			{
				return variableVals[v];
			}
	}
	else if (!isVar && count(varName, '\"') > 0)
	{
		string withoutQuotes;
		for (int ch = 1; ch < (int)varName.size() - 1; ch++)
		{
			withoutQuotes += varName[ch];
		}
		return withoutQuotes;
	}

	return varName;
}

bool BooleanLogic(string valA, string determinant, string valB, vector<string>& variables, vector<string>& variableVals)
{
	string valARealValue = GetRealValue(valA, variables, variableVals);
	string valBRealValue = GetRealValue(valB, variables, variableVals);

	if (determinant == "==") {
		return valARealValue == valBRealValue;
	}
	if (determinant == "!=") {
		return valARealValue != valBRealValue;
	}
	if (determinant == ">=") {
		return floatval(valARealValue) >= floatval(valBRealValue);
	}
	if (determinant == "<=") {
		return floatval(valARealValue) <= floatval(valBRealValue);
	}
	if (determinant == ">") {
		return floatval(valARealValue) > floatval(valBRealValue);
	}
	if (determinant == "<") {
		return floatval(valARealValue) < floatval(valBRealValue);
	}
	return false;
}

int evalEqu(vector<string> str, vector<string>& variables, vector<string>& variableValues)
{
	// Iterate all existing local variable names
	for (int v = 0; v < (int)variables.size(); v++)
	{
		if (str[0] == split(variables[v], ' ')[1])
		{
			if (str[1] == "=")
				variableValues[v] = GetRealValue(str[2], variables, variableValues);
			else if (str[1] == "+=")
				variableValues[v] = AddItem(variables[v], variableValues[v], GetRealValue(str[2], variables, variableValues));
			else if (str[1] == "-=")
				variableValues[v] = to_string(floatval(variableValues[v]) - floatval(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "*=")
				variableValues[v] = to_string(floatval(variableValues[v]) * floatval(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "/=")
				variableValues[v] = to_string(floatval(variableValues[v]) / floatval(GetRealValue(str[2], variables, variableValues)));

			//cout << words[lineNum][1] << " is " << words[lineNum][3] << endl << endl;
			return 0;
		}
	}
	// Iterate all existing global variable names
	for (int v = 0; v < (int)globalVariables.size(); v++)
	{
		if (str[0] == split(globalVariables[v], ' ')[1])
		{
			if (str[1] == "=")
				globalVariableValues[v] = GetRealValue(str[2], variables, variableValues);
			else if (str[1] == "+=")
				globalVariableValues[v] = AddItem(variables[v], globalVariableValues[v], GetRealValue(str[2], variables, variableValues));
			else if (str[1] == "-=")
				globalVariableValues[v] = to_string(floatval(globalVariableValues[v]) - floatval(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "*=")
				globalVariableValues[v] = to_string(floatval(globalVariableValues[v]) * floatval(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "/=")
				globalVariableValues[v] = to_string(floatval(globalVariableValues[v]) / floatval(GetRealValue(str[2], variables, variableValues)));

			//cout << words[lineNum][1] << " is " << words[lineNum][3] << endl << endl;
			return 0;
		}
	}
}

string PEMDAS(string equ, vector<string>& variables, vector<string>& variableValues)
{
	if (split(equ, ',').size() == 1)
		return equ;

	int parenthesisSetsCount = countNoOverlap(equ, '(', ')');

	vector<string> equationWords = split(equ, ' ');
	for (int s = 0; s < parenthesisSetsCount; s++)
	{
		int startOfNextParenthesis = 0;
		int numofParenthesis = 0;
		vector<string> insideParenthesis;
		for (int p = startOfNextParenthesis; p < (int)equationWords.size(); p++)
		{
			numofParenthesis += count(equationWords[p], '(') - count(equationWords[p], ')');
			if (numofParenthesis == 0)
			{
				startOfNextParenthesis = indexInStr(equationWords[charIndexInVec(equationWords, '(')], '(');
				break;
			}
			insideParenthesis.push_back("");
			for (int w = 0; w < (int)equationWords[p].size(); w++)
			{
				insideParenthesis[(int)insideParenthesis.size() - 1] += equationWords[p][w] + " ";
			}
		}

		equ = replace(equ, "(" + unWrapVec(insideParenthesis) + ")", PEMDAS(unWrapVec(insideParenthesis), variables, variableValues));
	}
}

int ProcessLine(vector<vector<string>> words, int lineNum, vector<string>& variables, vector<string>& variableValues)
{
	if (words[lineNum][0] == "print") {
		cout << GetRealValue(words[lineNum][1], variables, variableValues) << endl;
		return 0;
	}

	// Iterate through all functions
	for (int t = 0; t < (int)functions.size(); t++)
	{
		if (words[lineNum][0] == split(functions[t], ' ')[0])
		{
			ExecuteFunction(words[lineNum][0], split(unWrapVec(rangeInVec(words[lineNum], 1, -1)), ','));
			return 0;
		}
	}

	// First iterate through all types to see if line
	// is a variable then store it with it's value
	for (int t = 0; t < (int)types.size(); t++)
	{
		if (words[lineNum][0] == types[t])
		{
			//Checks if it is a re-init of an existing variable
			for (int v = 0; v < (int)variables.size(); v++)
			{
				if (words[lineNum][1] == split(variables[v], ' ')[1])
				{
					vector<string> inputs = { words[lineNum][1], words[lineNum][2], words[lineNum][3] };
					evalEqu(inputs, variables, variableValues);

					return 0;
				}
			}

			//Checks if it is variable
			variables.push_back(words[lineNum][0] + " " + words[lineNum][1]);
			variableValues.push_back(GetRealValue((string)words[lineNum][3], variables, variableValues));
			cout << variables[(int)variables.size() - 1] << " is " << variableValues[(int)variableValues.size() -1] << endl;
			return 0;
		}
	}
	// Second, iterate all existing variable names
	for (int v = 0; v < (int)variables.size(); v++)
	{
		if (words[lineNum][0] == split(variables[v], ' ')[1])
		{
			vector<string> inputs = { words[lineNum][0], words[lineNum][1], words[lineNum][2] };
			evalEqu(inputs, variables, variableValues);

			return 0;
		}
	}
	// Gathers while loop contents
	if (words[lineNum][0] == "while")
	{
		vector<string> whileContents;
		vector<string> whileParameters;

		for (int w = 1; w < (int)words[lineNum].size(); w++)
			whileParameters.push_back(words[lineNum][w]);

		int numOfBrackets = 1;
		for (int p = lineNum + 2; p < (int)words.size(); p++)
		{
			numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
			if (numOfBrackets == 0)
				break;
			whileContents.push_back("");
			for (int w = 0; w < (int)words[p].size(); w++)
			{
				whileContents[(int)whileContents.size() - 1] += words[p][w] + " ";
			}
		}
		whileContents = removeTabs(whileContents, 1);

		vector<vector<string>> words;
		for (int i = 0; i < (int)whileContents.size(); i++)
			words.push_back(split(whileContents[i], ' '));

		while (BooleanLogic(whileParameters[0], whileParameters[1], whileParameters[2], variables, variableValues))
		{
			//Iterate through all lines in while loop
			for (int lineNum = 0; lineNum < (int)whileContents.size(); lineNum++)
			{
				ProcessLine(words, lineNum, variables, variableValues);
			}
		}
		return 0;
	}
	// Gathers if statement contents
	if (words[lineNum][0] == "if")
	{
		vector<string> ifContents;
		vector<string> ifParameters;

		for (int w = 1; w < (int)words[lineNum].size(); w++)
			ifParameters.push_back(words[lineNum][w]);

		int numOfBrackets = 1;
		for (int p = lineNum + 2; p < (int)words.size(); p++)
		{
			numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
			if (numOfBrackets == 0)
				break;
			ifContents.push_back("");
			for (int w = 0; w < (int)words[p].size(); w++)
			{
				ifContents[(int)ifContents.size() - 1] += words[p][w] + " ";
			}
		}
		ifContents = removeTabs(ifContents, 1);

		vector<vector<string>> words;
		for (int i = 0; i < (int)ifContents.size(); i++)
			words.push_back(split(ifContents[i], ' '));

		if (BooleanLogic(ifParameters[0], ifParameters[1], ifParameters[2], variables, variableValues))
		{
			//Iterate through all lines in while loop
			for (int lineNum = 0; lineNum < (int)ifContents.size(); lineNum++)
			{
				ProcessLine(words, lineNum, variables, variableValues);
			}
		}
		return 0;
	}

	return 0;
}

int ExecuteFunction(string functionName, vector<string> inputVarVals)
{
	//vector<string> inputVarVals = split(replace(inVals, " ", ""), ',');

	vector<string> functionLines;
	int functionIndex = 0;
	//Get index of function
	for (int f = 0; f < (int)functions.size(); f++)
		if (split(functions[f], ' ')[0] == functionName)
		{
			functionLines = functionValues[f];
			functionIndex = f;
			break;
		}

	vector<string> variables;
	vector<string> variableValues;
	vector<string> functionNameParts = split(replace(functions[functionIndex], functionName + " ", ""), ',');
	for (int i = 0; i < (int)functionNameParts.size(); i++)
	{
		variables.push_back(trim(functionNameParts[i]));
		variableValues.push_back(GetRealValue(trim(inputVarVals[i]), variables, variableValues));
		cout << variables[(int)variables.size() - 1] << " is " << variableValues[(int)variableValues.size() - 1] << endl;
	}
	vector<vector<string>> words;
	for (int i = 0; i < (int)functionLines.size(); i++)
		words.push_back(split(functionLines[i], ' '));

	//Iterate through all lines in function
	for (int lineNum = 0; lineNum < (int)functionLines.size(); lineNum++)
	{
		ProcessLine(words, lineNum, variables, variableValues);
	}
	return 0;
}

int parseSlang(string script)
{
	script = replace(script, "    ", "\t");

	vector<string> lines = split(script, '\n');
	vector<vector<string>> words;
	for (int i = 0; i < (int)lines.size(); i++)
	{
		words.push_back(split(lines[i], ' '));
	}

	// First go through entire script and iterate through all types to see if line is a variable
	// or function declaration, then store it with it's value
	for (int lineNum = 0; lineNum < (int)words.size(); lineNum++)
		for (int t = 0; t < (int)types.size(); t++)
			if (words[lineNum][0] == types[t])
			{
				//Checks if it is function
				if (words[lineNum][(int)words[lineNum].size() - 1][(int)words[lineNum][(int)words[lineNum].size() - 1].size() - 1] == ')')
				{
					vector<string> functionContents;

					string functName;
					for (int w = 1; w < (int)words[lineNum].size(); w++) {
						if (w < (int)words[lineNum].size() - 1)
						{
							functName += replace(replace(words[lineNum][w], "(", " "), ")", "") + " ";
						}
						else
						{
							functName += replace(replace(words[lineNum][w], "(", " "), ")", "");
						}
					}

					int numOfBrackets = 1;
					for (int p = lineNum + 2; p < (int)words.size(); p++)
					{
						numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
						if (numOfBrackets == 0)
							break;
						functionContents.push_back("");
						for (int w = 0; w < (int)words[p].size(); w++)
						{
							functionContents[(int)functionContents.size() - 1] += words[p][w] + " ";
						}
					}
					functionContents = removeTabs(functionContents, 1);
					functions.push_back(functName);
					functionValues.push_back(functionContents);
					cout << functName << " is \n" << Vec2Str(functionContents) << endl << endl;
				}
				//Checks if it is variable
				else
				{
					globalVariables.push_back(words[lineNum][0] + " " + words[lineNum][1]);
					globalVariableValues.push_back((string)words[lineNum][3]);
					cout << words[lineNum][1] << " is " << words[lineNum][3] << endl;
				}
			}

	// Executes main, which is the starting function
	ExecuteFunction("Main", vector<string> {"hi", "7"});

	return 0;
}

int main(int argc, char* argv[])
{
	ifstream t("../Slang/script.slg");
	stringstream scriptString;
	scriptString << t.rdbuf();

	while (true) {
		system("pause");
		break;
	}
	parseSlang(scriptString.str());

	/*if (argc >= 2)
	{
		cout << argv[1];

		Parser window1;
		if (window1.Construct(128, 128, 2, 2))
			window1.Start();
		return 0;
	}
	else
	{
		return 1;
	}*/
	return 0;
}