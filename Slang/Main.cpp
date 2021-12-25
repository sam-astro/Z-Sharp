#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

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

string AddItem(string variableContent, string addItem)
{
	if (isNumber(variableContent))
	{
		return to_string(stof(variableContent) + stof(addItem));
	}
	else
	{
		return variableContent + addItem;
	}
}

string GetRealValue(string var, vector<string>& variables, vector<string>& variableVals)
{
	if (!isNumber(var) && count(var, '\"') == 0)
	{
		// Checks against global vars
		for (int v = 0; v < (int)globalVariables.size(); v++)
			if (var == globalVariables[v])
			{
				return globalVariableValues[v];
			}
		// Checks against local vars
		for (int v = 0; v < (int)variables.size(); v++)
			if (var == variables[v])
			{
				return variableVals[v];
			}
	}
	else if (!isNumber(var) && count(var, '\"') > 0)
	{
		string withoutQuotes;
		for (int ch = 1; ch < (int)var.size() - 1; ch++)
		{
			withoutQuotes += var[ch];
		}
		return withoutQuotes;
	}

	return var;
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
		return stof(valARealValue) >= stof(valBRealValue);
	}
	if (determinant == "<=") {
		return stof(valARealValue) <= stof(valBRealValue);
	}
	if (determinant == ">") {
		return stof(valARealValue) > stof(valBRealValue);
	}
	if (determinant == "<") {
		return stof(valARealValue) < stof(valBRealValue);
	}
	return false;
}

string evalEqu(vector<string> str, vector<string>& variables, vector<string>& variableValues)
{
	// Second, iterate all existing local variable names
	for (int v = 0; v < (int)variables.size(); v++)
	{
		if (str[0] == variables[v])
		{
			if (str[1] == "=")
				variableValues[v] = str[2];
			else if (str[1] == "+=")
				variableValues[v] = AddItem(variableValues[v], GetRealValue(str[2], variables, variableValues));
			else if (str[1] == "-=")
				variableValues[v] = to_string(stof(variableValues[v]) - stof(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "*=")
				variableValues[v] = to_string(stof(variableValues[v]) * stof(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "/=")
				variableValues[v] = to_string(stof(variableValues[v]) / stof(GetRealValue(str[2], variables, variableValues)));

			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
			return 0;
		}
	}
	// Third, iterate all existing global variable names
	for (int v = 0; v < (int)globalVariables.size(); v++)
	{
		if (str[0] == globalVariables[v])
		{
			if (str[1] == "=")
				globalVariableValues[v] = str[2];
			else if (str[1] == "+=")
				globalVariableValues[v] = AddItem(globalVariableValues[v], GetRealValue(str[2], variables, variableValues));
			else if (str[1] == "-=")
				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) - stof(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "*=")
				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) * stof(GetRealValue(str[2], variables, variableValues)));
			else if (str[1] == "/=")
				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) / stof(GetRealValue(str[2], variables, variableValues)));

			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
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

int ProcessLine(vector<vector<string>> words, int l, vector<string>& variables, vector<string>& variableValues)
{
	if (words[l][0] == "print") {
		cout << GetRealValue(words[l][1], variables, variableValues) << endl;
		return 0;
	}

	// Iterate through all functions
	for (int t = 0; t < (int)functions.size(); t++)
	{
		if (words[l][0] == split(functions[t], ' ')[0])
		{
			ExecuteFunction(words[l][0], rangeInVec(words[l], 1, -1));
			return 0;
		}
	}

	// First iterate through all types to see if line
	// is a variable then store it with it's value
	for (int t = 0; t < (int)types.size(); t++)
	{
		if (words[l][0] == types[t])
		{
			//Checks if it is variable
			variables.push_back(words[l][1]);
			variableValues.push_back(GetRealValue((string)words[l][3], variables, variableValues));
			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
			return 0;
		}
	}
	// Second, iterate all existing local variable names
	for (int v = 0; v < (int)variables.size(); v++)
	{
		if (words[l][0] == variables[v])
		{
			if (words[l][1] == "=")
				variableValues[v] = words[l][2];
			else if (words[l][1] == "+=")
				variableValues[v] = AddItem(variableValues[v], GetRealValue(words[l][2], variables, variableValues));
			else if (words[l][1] == "-=")
				variableValues[v] = to_string(stof(variableValues[v]) - stof(GetRealValue(words[l][2], variables, variableValues)));
			else if (words[l][1] == "*=")
				variableValues[v] = to_string(stof(variableValues[v]) * stof(GetRealValue(words[l][2], variables, variableValues)));
			else if (words[l][1] == "/=")
				variableValues[v] = to_string(stof(variableValues[v]) / stof(GetRealValue(words[l][2], variables, variableValues)));

			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
			return 0;
		}
	}
	// Third, iterate all existing global variable names
	for (int v = 0; v < (int)globalVariables.size(); v++)
	{
		if (words[l][0] == globalVariables[v])
		{
			if (words[l][1] == "=")
				globalVariableValues[v] = words[l][2];
			else if (words[l][1] == "+=")
				globalVariableValues[v] = AddItem(globalVariableValues[v], GetRealValue(words[l][2], variables, variableValues));
			else if (words[l][1] == "-=")
				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) - stof(GetRealValue(words[l][2], variables, variableValues)));
			else if (words[l][1] == "*=")
				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) * stof(GetRealValue(words[l][2], variables, variableValues)));
			else if (words[l][1] == "/=")
				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) / stof(GetRealValue(words[l][2], variables, variableValues)));

			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
			return 0;
		}
	}
	// Gathers while loop contents
	if (words[l][0] == "while")
	{
		vector<string> whileContents;
		vector<string> whileParameters;

		for (int w = 1; w < (int)words[l].size(); w++)
			whileParameters.push_back(words[l][w]);

		int numOfBrackets = 1;
		for (int p = l + 2; p < (int)words.size(); p++)
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
			for (int l = 0; l < (int)whileContents.size(); l++)
			{
				ProcessLine(words, l, variables, variableValues);
			}
		}
		return 0;
	}
	// Gathers if statement contents
	if (words[l][0] == "if")
	{
		vector<string> ifContents;
		vector<string> ifParameters;

		for (int w = 1; w < (int)words[l].size(); w++)
			ifParameters.push_back(words[l][w]);

		int numOfBrackets = 1;
		for (int p = l + 2; p < (int)words.size(); p++)
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
			for (int l = 0; l < (int)ifContents.size(); l++)
			{
				ProcessLine(words, l, variables, variableValues);
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
	vector<string> functionNameParts = split(functions[functionIndex], ' ');
	for (int i = 1; i < (int)functionNameParts.size(); i++)
	{
		variables.push_back(replace(replace(functionNameParts[i], ",", ""), " ", ""));
		variableValues.push_back(inputVarVals[i - 1]);
	}
	vector<vector<string>> words;
	for (int i = 0; i < (int)functionLines.size(); i++)
		words.push_back(split(functionLines[i], ' '));

	//Iterate through all lines in function
	for (int l = 0; l < (int)functionLines.size(); l++)
	{
		ProcessLine(words, l, variables, variableValues);
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
					globalVariables.push_back(words[lineNum][1]);
					globalVariableValues.push_back((string)words[lineNum][3]);
					cout << words[lineNum][1] << " is " << words[lineNum][3] << endl << endl;
				}
			}

	// Executes main, which is the starting function
	ExecuteFunction("Main", vector<string> {"hi"});

	return 0;
}

int main(int argc, char* argv[])
{
	std::ifstream t("../Slang/script.sl");
	std::stringstream scriptString;
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