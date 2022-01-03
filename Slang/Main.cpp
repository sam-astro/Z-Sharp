
#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include "eval.h"
#include "strops.h"
#include "builtin.h"
#include "main.h"
#include <boost/any.hpp>

using namespace std;
using namespace boost;

unordered_map<string, any> globalVariableValues;
unordered_map<string, vector<string>> functionValues;

bool isNumber(const string& str)
{
	for (char const& c : str) {
		if (isdigit(c) == 0 && c != '.') return false;
	}
	return true;
}

string StringRaw(string str)
{
	str = trim(str);

	if (str.size() < 3)
		return str;

	string withoutQuotes;

	if (str[0] != '\"')
		withoutQuotes += str[0];

	for (int ch = 1; ch < (int)str.size() - 1; ch++)
		withoutQuotes += str[ch];

	if (str[str.size() - 1] != '\"')
		withoutQuotes += str[str.size() - 1];

	return withoutQuotes;
}

string Quoted(string str)
{
	str = trim(str);

	string withQuotes;

	if (str[0] != '\"')
		withQuotes += '\"';

	withQuotes += str;

	if (str[str.size() - 1] != '\"')
		withQuotes += '\"';

	return withQuotes;
}

string RMParenthesis(string str)
{
	str = trim(str);
	string withoutParenthesis;

	if (str[0] != '(')
		withoutParenthesis += str[0];

	for (int ch = 1; ch < (int)str.size() - 1; ch++)
		withoutParenthesis += str[ch];

	if (str[str.size() - 1] != ')')
		withoutParenthesis += str[str.size() - 1];

	return withoutParenthesis;
}

string GetVariableValue(string varName, unordered_map<string, any>& variableVals)
{
	auto iA = variableVals.find(varName);
	if (iA != variableVals.end())
	{
		return iA->second;
	}
	else
	{
		auto iB = globalVariableValues.find(varName);
		if (iB != globalVariableValues.end())
		{
			return iB->second;
		}
		else
		{
			return varName;
		}
	}
}

bool IsVar(string varName, unordered_map<string, any>& variableVals)
{
	auto iA = variableVals.find(varName);
	if (iA != variableVals.end())
	{
		return true;
	}
	return false;
}

vector<string> VarValues(vector<string> varNames, unordered_map<string, any>& variableVals)
{
	vector<any> realValues;

	for (int varIndex = 0; varIndex < varNames.size(); varIndex++)
	{
		varNames[varIndex] = trim(varNames[varIndex]);

		auto iA = variableVals.find(varNames[varIndex]);
		if (iA != variableVals.end())
		{
			realValues.push_back(iA->second);
		}
		else
		{
			auto iB = globalVariableValues.find(varNames[varIndex]);
			if (iB != globalVariableValues.end())
			{
				realValues.push_back(iB->second);
			}
			else
			{
				realValues.push_back(varNames[varIndex]);
			}
		}
	}

	return realValues;
}

string IsFunction(string funcName)
{
	auto iA = functionValues.find(funcName);
	if (iA != functionValues.end())
	{
		return iA->first;
	}
	else
	{
		return "";
	}
}

string EvalExpression(string expression, unordered_map<string, any>& variableVals)
{
	expression = trim(expression);
	bool inQuotes = false;

	// If no operations are applied, then return self
	if ((count(expression, '+') == 0 && count(expression, '-') == 0 && count(expression, '*') == 0 && count(expression, '/') == 0 && count(expression, '(') == 0 && count(expression, '^') == 0) || split(expression, '.')[0] == "CPP")
	{
		int funcIndex = IsFunction(split(expression, '(')[0]);
		if (funcIndex != -1 && !inQuotes)
		{
			//cout << split(expression, '(')[0] << endl;
			string argContents = "";
			int y = indexInStr(expression, '(') + 1;
			while (y < expression.size() && expression[y] != ')')
			{
				argContents += expression[y];

				y++;
			}
			//cout << split(expression, '(')[0] << "  " << argContents << endl;
			string returnVal = ExecuteFunction(split(expression, '(')[0], VarValues(split(argContents, ','), variableVals), funcIndex);
			return returnVal;
		}
		else if (split(expression, '.')[0] == "CPP" && !inQuotes)
		{
			string argContents = "";
			int y = indexInStr(expression, '(') + 1;
			while (y < expression.size() && expression[y] != ')')
			{
				argContents += expression[y];

				y++;
			}
			//cout << split(expression, '(')[0] << " " << unWrapVec(VarValues(split(argContents, ','), variables, variableVals)) << endl;
			string returnVal = CPPFunction(split(expression, '(')[0], VarValues(split(argContents, ','), variableVals));
			return returnVal;
		}
		else
			return GetVariableValue(expression, variableVals);
	}

	string newExpression = "";
	inQuotes = false;

	for (int i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '\"')
			inQuotes = !inQuotes;

		if (isalpha(expression[i]))
		{
			string name = "";

			while (i < expression.size() && (isalpha(expression[i]) || expression[i] == '.'))
			{
				name += expression[i];
				i++;
			}

			//string varVal = GetVariableValue(name, variables, variableVals);
			any funcPointer = IsFunction(name);
			if (funcIndex != -1 && !inQuotes)
			{
				string argContents = "";
				i++;
				while (i < expression.size() && expression[i] != ')')
				{
					argContents += expression[i];

					i++;
				}
				string returnVal = ExecuteFunction(name, VarValues(split(argContents, ','), variableVals), funcIndex);
				newExpression += returnVal;
				//cout << newExpression << endl;
			}
			else if (split(name, '.')[0] == "CPP" && !inQuotes)
			{
				string argContents = "";
				int y = indexInStr(expression, '(') + 1;
				while (y < expression.size() && expression[y] != ')')
				{
					argContents += expression[y];

					y++;
				}
				//cout << split(expression, '(')[0] << " " << argContents << endl;
				string returnVal = CPPFunction(split(name, '(')[0], VarValues(split(argContents, ',') variableVals));
				newExpression += returnVal;
			}
			else
			{
				if (inQuotes)
					newExpression += name;
				else
					newExpression += GetVariableValue(name, variableVals);
			}

			i--;
		}
		else
		{
			newExpression += expression[i];
		}
	}
	//cout << "NEW EXPRESSION: " << newExpression << endl;

	bool addStrings = false;
	for (int i = 0; i < (int)newExpression.size(); i++)
		if (isalpha(newExpression[i]) || newExpression[i] == '\"')
		{
			addStrings = true;
			break;
		}
	if (addStrings)
	{
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

		//cout << "NewSTRING = " << Quoted(withoutParenthesis) << endl;
		return Quoted(withoutParenthesis);
	}
	else
		return to_string(evaluate(newExpression));
}

bool BooleanLogic(string valA, string determinant, string valB, unordered_map<string, any>& variableVals)
{
	string valARealValue = EvalExpression(valA, variableVals);
	string valBRealValue = EvalExpression(valB, variableVals);

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

int evalEqu(vector<string> str, unordered_map<string, any>& variableValues)
{
	if (IsVar(str[0], variableValues))
	{
		if (str[1] == "=")
			variableValues[str[0]] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
		else if (str[1] == "+=")
			variableValues[str[0]] = EvalExpression(variableValues[str[0]] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "-=")
			variableValues[str[0]] = EvalExpression(variableValues[str[0]] + "-(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "*=")
			variableValues[str[0]] = EvalExpression(variableValues[str[0]] + "*(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "/=")
			variableValues[str[0]] = EvalExpression(variableValues[str[0]] + "/(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);

		//cout << variables[v] << " is " << variableValues[v] << endl;
		return 0;
	}
	else if (IsVar(str[0], globalVariableValues))
	{
		if (str[1] == "=")
			globalVariableValues[str[0]] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
		else if (str[1] == "+=")
			globalVariableValues[str[0]] = EvalExpression(variableValues[str[0]] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "-=")
			globalVariableValues[str[0]] = EvalExpression(variableValues[str[0]] + "-(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "*=")
			globalVariableValues[str[0]] = EvalExpression(variableValues[str[0]] + "*(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "/=")
			globalVariableValues[str[0]] = EvalExpression(variableValues[str[0]] + "/(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);

		//cout << variables[v] << " is " << variableValues[v] << endl;
		return 0;
	}
	return 1;
}

string ProcessLine(vector<vector<string>> words, int lineNum, unordered_map<string, any>& variableValues)
{
	if (words[lineNum][0][0] == '/' && words[lineNum][0][1] == '/')
		return "";

	if (words[lineNum][0] == "print")
	{
		cout << StringRaw(EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variables, variableValues)) << endl;
		return "";
	}

	if (words[lineNum][0] == "return") {
		//cout << StringRaw(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end()))) << endl;
		return EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variables, variableValues);
	}

	if (split(words[lineNum][0], '.')[0] == "CPP")
	{
		string output = EvalExpression(unWrapVec(words[lineNum]), variables, variableValues);
		return output;
	}

	// Iterate through all functions
	for (int t = 0; t < (int)functions.size(); t++)
	{
		if (split(words[lineNum][0], '(')[0] == split(functions[t], ' ')[0])
		{
			ExecuteFunction(split(functions[t], ' ')[0], VarValues(split(RMParenthesis(replace(unWrapVec(words[lineNum]), split(functions[t], ' ')[0], "")), ','), variables, variableValues), t);
			return "";
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

					return "";
				}
			}

			//Checks if it is variable
			variables.push_back(words[lineNum][0] + " " + words[lineNum][1]);
			variableValues.push_back(EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 3, words[lineNum].end())), variables, variableValues));
			//cout << variables[(int)variables.size() - 1] << " is " << variableValues[(int)variableValues.size() - 1] << endl;
			return "";
		}
	}
	// Second, iterate all existing variable names
	for (int v = 0; v < (int)variables.size(); v++)
	{
		if (words[lineNum][0] == split(variables[v], ' ')[1])
		{
			evalEqu(vector<string>(words[lineNum].begin(), words[lineNum].end()), variables, variableValues);

			return "";
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

		vector<vector<string>> innerWords;
		for (int i = 0; i < (int)whileContents.size(); i++)
			innerWords.push_back(split(whileContents[i], ' '));

		while (BooleanLogic(whileParameters[0], whileParameters[1], whileParameters[2], variables, variableValues))
		{
			//Iterate through all lines in while loop
			for (int lineNum = 0; lineNum < (int)whileContents.size(); lineNum++)
			{
				string returnVal = ProcessLine(innerWords, lineNum, variables, variableValues);
				if (returnVal != "")
					return returnVal;
			}
		}
		return "";
	}
	// Gathers if statement contents
	if (words[lineNum][0] == "if")
	{
		vector<string> ifContents;
		vector<string> ifParameters;

		for (int w = 1; w < (int)words[lineNum].size(); w++)
			ifParameters.push_back(words[lineNum][w]);

		int numOfBrackets = 1;
		lineNum += 2;
		while (lineNum < (int)words.size())
		{
			numOfBrackets += countInVector(words[lineNum], "{") - countInVector(words[lineNum], "}");
			if (numOfBrackets == 0)
				break;
			ifContents.push_back("");
			for (int w = 0; w < (int)words[lineNum].size(); w++)
			{
				ifContents[(int)ifContents.size() - 1] += words[lineNum][w] + " ";
			}
			lineNum++;
		}
		ifContents = removeTabs(ifContents, 1);

		vector<vector<string>> innerWords;
		for (int i = 0; i < (int)ifContents.size(); i++)
			innerWords.push_back(split(ifContents[i], ' '));

		if (BooleanLogic(ifParameters[0], ifParameters[1], ifParameters[2], variables, variableValues))
		{
			//Iterate through all lines in if statement
			for (int l = 0; l < (int)ifContents.size(); l++)
			{
				string returnVal = ProcessLine(innerWords, l, variables, variableValues);
				if (returnVal != "")
					return returnVal;
			}
		}
		else if (words.size() > lineNum + 1)
			if (words[lineNum + 1][0] == "else")
			{
				lineNum += 1;

				vector<string> elseContents;

				int numOfBrackets = 1;
				while (lineNum < (int)words.size())
				{
					numOfBrackets += countInVector(words[lineNum], "{") - countInVector(words[lineNum], "}");
					if (numOfBrackets == 0)
						break;
					elseContents.push_back("");
					for (int w = 0; w < (int)words[lineNum].size(); w++)
					{
						elseContents[(int)elseContents.size() - 1] += words[lineNum][w] + " ";
					}
					lineNum++;
				}
				elseContents = removeTabs(elseContents, 2);

				vector<vector<string>> innerWords;
				for (int i = 0; i < (int)elseContents.size(); i++)
					words.push_back(split(elseContents[i], ' '));

				//Iterate through all lines in else statement
				for (int lineNum = 0; lineNum < (int)elseContents.size(); lineNum++)
				{
					ProcessLine(innerWords, lineNum, variables, variableValues);
				}
				return "";
			}
		return "";
	}
	//// Gathers else statement contents
	//if (words[lineNum][0] == "else")
	//{
	//	
	//}

	return "";
}

string ExecuteFunction(string functionName, vector<string> inputVarVals, int functionIndex)
{
	//vector<string> inputVarVals = split(replace(inVals, " ", ""), ',');
	vector<string> functionLines;
	//Get index of function
	if (functionIndex == -1)
	{
		for (int f = 0; f < (int)functions.size(); f++)
			if (split(functions[f], ' ')[0] == functionName)
			{
				functionLines = functionValues[f];
				functionIndex = f;
				break;
			}
	}
	else
		functionLines = functionValues[functionIndex];

	unordered_map<string, any> variableValues;
	vector<string> functionNameParts = split(replace(functions[functionIndex], functionName + " ", ""), ',');
	for (int i = 0; i < (int)inputVarVals.size(); i++)
	{
		variables.push_back(trim(functionNameParts[i]));
		variableValues.push_back(EvalExpression(inputVarVals[i], variables, variableValues));
		//cout << variables[(int)variables.size() - 1] << " is " << variableValues[(int)variableValues.size() - 1] << endl;
	}
	vector<vector<string>> words;
	for (int i = 0; i < (int)functionLines.size(); i++)
		words.push_back(split(functionLines[i], ' '));

	//Iterate through all lines in function
	for (int lineNum = 0; lineNum < (int)functionLines.size(); lineNum++)
	{
		string returnVal = "";
		try
		{
			returnVal = ProcessLine(words, lineNum, variables, variableValues);
		}
		catch (const std::exception&)
		{
			cout << "\x1B[31mERROR: \'" << unWrapVec(words[lineNum]) << "\'\nIn function: " << functionName << "\nLine: " << lineNum << "\033[0m\t\t" << endl;
			exit(1);
		}
		if (returnVal != "")
			return returnVal;
	}
	return "";
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
					//cout << functName << " is \n" << Vec2Str(functionContents) << endl << endl;
				}
				//Checks if it is variable
				else
				{
					globalVariables.push_back(words[lineNum][0] + " " + words[lineNum][1]);
					globalVariableValues.push_back((string)words[lineNum][3]);
					//cout << words[lineNum][1] << " is " << words[lineNum][3] << endl;
				}
			}

	// Executes main, which is the starting function
	ExecuteFunction("Main", vector<string> {"\"hi\"", "0"}, -1);

	return 0;
}

int main(int argc, char* argv[])
{
	ifstream builtin("../Slang/builtin.slg");
	stringstream builtinString;
	builtinString << builtin.rdbuf();

	GetBuiltins(builtinString.str());
	functions = builtinFunctions;
	functionValues = builtinFunctionValues;
	globalVariables = builtinVars;
	globalVariableValues = builtinVarVals;

	ifstream script("../Slang/script.slg");
	stringstream scriptString;
	scriptString << script.rdbuf();

	while (true) {
		system("pause");
		break;
	}
	parseSlang(scriptString.str());

	return 0;
}
