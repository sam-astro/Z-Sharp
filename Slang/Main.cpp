#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include "eval.h"
#include "strops.h"

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

string AddItem(string varName, string variableContent, string addItem)
{
	string typ = split(varName, ' ')[0];

	if (typ == "int" || typ == "float" || typ == "double" && isNumber(addItem))
		return to_string(floatval(to_string(floatval(variableContent) + floatval(addItem))));
	else
		return variableContent + addItem;
}

string StringRaw(string str)
{
	string withoutQuotes;

	for (int ch = 1; ch < (int)str.size() - 1; ch++)
		withoutQuotes += str[ch];

	return withoutQuotes;
}

string GetVariableValue(string varName, vector<string>& variables, vector<string>& variableVals)
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
	//else if (!isVar && count(varName, '\"') > 0)
	//{
	//	string withoutQuotes;

	//	for (int ch = 1; ch < (int)varName.size() - 1; ch++)
	//		withoutQuotes += varName[ch];

	//	return withoutQuotes;
	//}

	return varName;
}

string EvalExpression(string expression, vector<string>& variables, vector<string>& variableVals)
{
	expression = trim(expression);
	cout << "EXPRESSION: " << expression << endl;

	if (count(expression, '+') == 0 && count(expression, '-') == 0 && count(expression, '*') == 0 && count(expression, '/') == 0)
		return GetVariableValue(expression, variables, variableVals);

	string newExpression = "";
	bool inQuotes = false;

	for (int i = 0; i < expression.size(); i++)
	{
		if (isalpha(expression[i]))
		{
			string name = "";

			while (i < expression.size() && isalpha(expression[i]))
			{
				name += expression[i];

				i++;
			}

			newExpression += GetVariableValue(name, variables, variableVals);

			i--;
		}
		else
		{
			newExpression += expression[i];
		}
	}
	cout << "NEW EXPRESSION: " << newExpression << endl;

	bool addStrings = false;
	for (int i = 0; i < (int)newExpression.size(); i++)
		if (isalpha(newExpression[i]) || newExpression[i] == '\"')
		{
			addStrings = true;
			break;
		}
	if (addStrings)
	{
		string newStr = "";
		inQuotes = false;
		string withoutParenthesis = "";
		for (int i = 0; i < (int)newExpression.size(); i++)
		{
			if (newExpression[i] == '\"')
			{
				inQuotes = !inQuotes;
				continue;
			}
			if (inQuotes)
				withoutParenthesis += newExpression[i];
			if (!inQuotes && newExpression[i] != '(' && newExpression[i] != ')' && newExpression[i] != '+' && newExpression[i] != ' ')
				withoutParenthesis += newExpression[i];
		}

		cout << "NewSTRING = " << withoutParenthesis << endl;
		return withoutParenthesis;
	}
	else
		return to_string(evaluate(newExpression));
}

bool BooleanLogic(string valA, string determinant, string valB, vector<string>& variables, vector<string>& variableVals)
{
	string valARealValue = GetVariableValue(valA, variables, variableVals);
	string valBRealValue = GetVariableValue(valB, variables, variableVals);

	if (determinant == "==")
		return valARealValue == valBRealValue;
	if (determinant == "!=")
		return valARealValue != valBRealValue;
	if (determinant == ">=")
		return floatval(valARealValue) >= floatval(valBRealValue);
	if (determinant == "<=")
		return floatval(valARealValue) <= floatval(valBRealValue);
	if (determinant == ">")
		return floatval(valARealValue) > floatval(valBRealValue);
	if (determinant == "<")
		return floatval(valARealValue) < floatval(valBRealValue);

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
				variableValues[v] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variables, variableValues);
			else if (str[1] == "+=")
				variableValues[v] = EvalExpression(variableValues[v] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);
			else if (str[1] == "-=")
				variableValues[v] = EvalExpression(variableValues[v] + "-(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);
			else if (str[1] == "*=")
				variableValues[v] = EvalExpression(variableValues[v] + "*(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);
			else if (str[1] == "/=")
				variableValues[v] = EvalExpression(variableValues[v] + "/(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);

			cout << variables[v] << " is " << variableValues[v] << endl;
			return 0;
		}
	}
	// Iterate all existing global variable names
	for (int v = 0; v < (int)globalVariables.size(); v++)
	{
		if (str[0] == split(globalVariables[v], ' ')[1])
		{
			if (str[1] == "=")
				globalVariableValues[v] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variables, variableValues);
			else if (str[1] == "+=")
				globalVariableValues[v] = EvalExpression(variableValues[v] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);
			else if (str[1] == "-=")
				globalVariableValues[v] = EvalExpression(variableValues[v] + "-(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);
			else if (str[1] == "*=")
				globalVariableValues[v] = EvalExpression(variableValues[v] + "*(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);
			else if (str[1] == "/=")
				globalVariableValues[v] = EvalExpression(variableValues[v] + "/(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variables, variableValues);

			cout << variables[v] << " is " << variableValues[v] << endl;
			return 0;
		}
	}
}

int ProcessLine(vector<vector<string>> words, int lineNum, vector<string>& variables, vector<string>& variableValues)
{
	if (words[lineNum][0][0] == '/' && words[lineNum][0][1] == '/')
		return 0;

	if (words[lineNum][0] == "print") {
		cout << EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variables, variableValues) << endl;
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
	// inits a variable then store it with it's value
	for (int t = 0; t < (int)types.size(); t++)
	{
		if (words[lineNum][0] == types[t])
		{
			//Checks if it is a re-init of an existing variable
			for (int v = 0; v < (int)variables.size(); v++)
			{
				if (words[lineNum][1] == split(variables[v], ' ')[1])
				{
					evalEqu(vector<string>(words[lineNum].begin() + 1, words[lineNum].end()), variables, variableValues);

					return 0;
				}
			}

			//Checks if it is variable
			variables.push_back(words[lineNum][0] + " " + words[lineNum][1]);
			variableValues.push_back(EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 3, words[lineNum].end())), variables, variableValues));
			cout << variables[(int)variables.size() - 1] << " is " << variableValues[(int)variableValues.size() - 1] << endl;
			return 0;
		}
	}
	// Second, iterate all existing variable names
	for (int v = 0; v < (int)variables.size(); v++)
	{
		if (words[lineNum][0] == split(variables[v], ' ')[1])
		{
			evalEqu(vector<string>(words[lineNum].begin(), words[lineNum].end()), variables, variableValues);

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
		variableValues.push_back(EvalExpression(inputVarVals[i], variables, variableValues));
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
	ExecuteFunction("Main", vector<string> {"\"hi\"", "7"});

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