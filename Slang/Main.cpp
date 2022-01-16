
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <boost/any.hpp>
#include <unordered_map>
#include <stdio.h>
#include <codecvt>

#if defined(__unix__)
#include <unistd.h>
#elif defined(_MSC_VER)
#include <windows.h>
#endif

#include "eval.h"
#include "strops.h"
#include "builtin.h"
#include "main.h"
#include "anyops.h"

#include "SLB.h"

#define DEVELOPER_MESSAGES true
#define EXAMPLE_PROJECT false

using namespace std;
using namespace boost;

unordered_map<string, boost::any> globalVariableValues;
unordered_map<string, vector<vector<string>>> functionValues;

boost::any GetVariableValue(const string& varName, const unordered_map<string, boost::any>& variableValues)
{
	string classSubComponent;
	string baseName = trim(varName);

	if (count(varName, '.') > 0)
	{
		classSubComponent = trim(varName.substr(indexInStr(varName, '.') + 1, -1));
		baseName = trim(split(varName, '.')[0]);
	}

	boost::any outputValue = nullType;

	auto iA = variableValues.find(baseName);
	auto iB = globalVariableValues.find(baseName);
	if (iA != variableValues.end())
		outputValue = iA->second;
	else if (iB != globalVariableValues.end())
		outputValue = iB->second;
	else
		outputValue = varName;

	if (count(varName, '.') > 0 && !outputValue.empty())
		return GetClassSubComponent(outputValue, classSubComponent);
	else
		return outputValue;
}

bool IsVar(const string& varName, const unordered_map<string, boost::any>& variableValues)
{
	if (variableValues.find(split(varName, '.')[0]) != variableValues.end() && split(varName, '.')[0] != "SLB")
		return true;
	else
		return false;
}

vector<boost::any> VarValues(const vector<string>& varNames, unordered_map<string, boost::any>& variableValues)
{
	vector<boost::any> realValues;

	for (int varIndex = 0; varIndex < varNames.size(); varIndex++)
	{
		string varName = trim(varNames[varIndex]);

		//realValues.push_back(EvalExpression(varName, variableValues));
		auto iA = variableValues.find(varName);
		if (iA != variableValues.end())
		{
			realValues.push_back(iA->second);
		}
		else
		{
			auto iB = globalVariableValues.find(varName);
			if (iB != globalVariableValues.end())
				realValues.push_back(iB->second);
			else
				realValues.push_back(EvalExpression(varName, variableValues));
		}
	}

	return realValues;
}

bool IsFunction(const string& funcName)
{
	if (functionValues.find(funcName) != functionValues.end())
		return true;
	else
		return false;
}
bool IsSLBFunction(const string& funcName)
{
	if (funcName[0] == 'S' && funcName[1] == 'L' && funcName[2] == 'B' && funcName[2] == '.')
		return true;
	else
		return false;
}

boost::any EvalExpression(const string& ex, unordered_map<string, boost::any>& variableValues)
{
	string expression = trim(ex);
	bool inQuotes = false;

#if DEVELOPER_MESSAGES == true
	InterpreterLog("OLDEXPRESSION: |" + expression + "|");
#endif

	bool isFunc = IsFunction(split(expression, '(')[0]);
	bool isSLB = split(expression, '.')[0] == "SLB";
	// If no operations are applied, then return self
	if ((countOutsideParenthesis(expression, '+') == 0 && countOutsideParenthesis(expression, '-') == 0 && countOutsideParenthesis(expression, '*') == 0 && countOutsideParenthesis(expression, '/') == 0 && countOutsideParenthesis(expression, '^') == 0) || split(expression, '.')[0] == "SLB")
	{
		bool isFunc = IsFunction(split(expression, '(')[0]);
		if (isFunc && !inQuotes)
		{
			//cout << split(expression, '(')[0] << endl;
			string argContents = "";
			int y = indexInStr(expression, '(') + 1;
			while (y < expression.size() && expression[y] != ')')
			{
				argContents += expression[y];

				y++;
			}
			return ExecuteFunction(split(expression, '(')[0], VarValues(split(argContents, ','), variableValues));
		}
		else if (split(expression, '.')[0] == "SLB" && !inQuotes)
		{
			string argContents = "";
			int y = indexInStr(expression, '(') + 1;
			while (y < expression.size() && expression[y] != ')')
			{
				argContents += expression[y];

				y++;
			}
			return SLBFunction(split(expression, '(')[0], VarValues(split(argContents, ','), variableValues));
		}
		else
			return GetVariableValue(expression, variableValues);
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

			//string varVal = GetVariableValue(name, variables, variableValues);
			bool isFunc = IsFunction(name);
			if (isFunc && !inQuotes)
			{
				string argContents = "";
				i++;
				while (i < expression.size() && expression[i] != ')')
				{
					argContents += expression[i];

					i++;
				}
				string returnVal = AnyAsString(ExecuteFunction(name, VarValues(split(argContents, ','), variableValues)));
				newExpression += returnVal;
			}
			else if (split(name, '.')[0] == "SLB" && !inQuotes)
			{
				string argContents = "";
				int y = indexInStr(expression, '(') + 1;
				while (y < expression.size() && expression[y] != ')')
				{
					argContents += expression[y];

					y++;
				}
				//cout << split(expression, '(')[0] << " " << argContents << endl;
				string returnVal = AnyAsString(SLBFunction(split(name, '(')[0], VarValues(split(argContents, ','), variableValues)));
				newExpression += returnVal;
			}
			else
			{
				if (inQuotes)
					newExpression += name;
				else
					newExpression += AnyAsString(GetVariableValue(name, variableValues));
			}

			i--;
		}
		else
		{
			newExpression += expression[i];
		}
	}
#if DEVELOPER_MESSAGES == true
	InterpreterLog("NEW EXPRESSION: |" + newExpression + "|");
#endif

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
		return withoutParenthesis;
	}
	else
		return evaluate(newExpression);
}

bool BooleanLogic(const string& valA, const string& determinant, const string& valB, unordered_map<string, boost::any>& variableValues)
{
	boost::any valARealValue = EvalExpression(valA, variableValues);
	boost::any valBRealValue = EvalExpression(valB, variableValues);
#if DEVELOPER_MESSAGES == true
	InterpreterLog(AnyAsString(valARealValue) + " " + determinant + " " + AnyAsString(valBRealValue) + " : " + AnyAsString(valA) + " " + determinant + " " + AnyAsString(valB) + " : " + to_string(AnyAsString(valARealValue) == AnyAsString(valBRealValue)));
#endif
	if (determinant == "==")
		return any_compare(valARealValue, valBRealValue);
	else if (determinant == "!=")
		return !any_compare(valARealValue, valBRealValue);
	else if (determinant == ">=")
		return AnyAsFloat(valARealValue) >= AnyAsFloat(valBRealValue);
	else if (determinant == "<=")
		return AnyAsFloat(valARealValue) <= AnyAsFloat(valBRealValue);
	else if (determinant == ">")
		return AnyAsFloat(valARealValue) > AnyAsFloat(valBRealValue);
	else if (determinant == "<")
		return AnyAsFloat(valARealValue) < AnyAsFloat(valBRealValue);
	else
		LogWarning("unrecognized determinant \'" + determinant + "\'");

	return false;
}

int varOperation(const vector<string>& str, unordered_map<string, boost::any>& variableValues)
{
	if (IsVar(str[0], variableValues))
	{
		// Checks if type is simple, like int or string
		if (any_type(variableValues[str[0]]) <= 3)
		{
			if (str[1] == "=")
				variableValues[str[0]] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			else if (str[1] == "+=")
				variableValues[str[0]] = EvalExpression(str[0] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
			else if (str[1] == "-=")
				variableValues[str[0]] = AnyAsFloat(variableValues[str[0]]) - AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str[1] == "*=")
				variableValues[str[0]] = AnyAsFloat(variableValues[str[0]]) * AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str[1] == "/=")
				variableValues[str[0]] = AnyAsFloat(variableValues[str[0]]) / AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else
				LogWarning("unrecognized operator \'" + str[1] + "\'");
		}
		// Else it is a Vec2. No other complex class can be operated on it's base form (ex. you can't do: Sprite += Sprite)
		else if (any_type(variableValues[str[0]]) == 5)
		{
			boost::any otherExpression = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			if (str[1] == "=")
				variableValues[str[0]] = otherExpression;
			else if (str[1] == "+=")
				variableValues[str[0]] = AnyAsVec2(variableValues[str[0]]) + AnyAsVec2(otherExpression);
			else if (str[1] == "-=")
				variableValues[str[0]] = AnyAsVec2(variableValues[str[0]]) - AnyAsVec2(otherExpression);
			else if (str[1] == "*=")
				variableValues[str[0]] = AnyAsVec2(variableValues[str[0]]) * AnyAsFloat(otherExpression);
			else if (str[1] == "/=")
				variableValues[str[0]] = AnyAsVec2(variableValues[str[0]]) / AnyAsFloat(otherExpression);
			else
				LogWarning("unrecognized operator \'" + str[1] + "\'");
		}
		return 0;
	}
	else if (IsVar(str[0], globalVariableValues))
	{
		// Checks if type is simple, like int or string
		if (any_type(globalVariableValues[str[0]]) <= 3)
		{
			if (str[1] == "=")
				globalVariableValues[str[0]] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			else if (str[1] == "+=")
				globalVariableValues[str[0]] = EvalExpression(str[0] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
			else if (str[1] == "-=")
				globalVariableValues[str[0]] = AnyAsFloat(globalVariableValues[str[0]]) - AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str[1] == "*=")
				globalVariableValues[str[0]] = AnyAsFloat(globalVariableValues[str[0]]) * AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str[1] == "/=")
				globalVariableValues[str[0]] = AnyAsFloat(globalVariableValues[str[0]]) / AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else
				LogWarning("unrecognized operator \'" + str[1] + "\'");
		}
		// Else it is a Vec2. No other complex class can be operated on it's base form (ex. you can't do: Sprite += Sprite)
		else if (any_type(globalVariableValues[str[0]]) == 5)
		{
			boost::any otherExpression = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			if (str[1] == "=")
				globalVariableValues[str[0]] = otherExpression;
			else if (str[1] == "+=")
				globalVariableValues[str[0]] = AnyAsVec2(globalVariableValues[str[0]]) + AnyAsVec2(otherExpression);
			else if (str[1] == "-=")
				globalVariableValues[str[0]] = AnyAsVec2(globalVariableValues[str[0]]) - AnyAsVec2(otherExpression);
			else if (str[1] == "*=")
				globalVariableValues[str[0]] = AnyAsVec2(globalVariableValues[str[0]]) * AnyAsFloat(otherExpression);
			else if (str[1] == "/=")
				globalVariableValues[str[0]] = AnyAsVec2(globalVariableValues[str[0]]) / AnyAsFloat(otherExpression);
			else
				LogWarning("unrecognized operator \'" + str[1] + "\'");
		}
		return 0;
	}
	LogWarning("uninitialized variable or typo in \'" + str[0] + "\'");
	return 1;
}

boost::any ProcessLine(const vector<vector<string>>& words, int lineNum, unordered_map<string, boost::any>& variableValues)
{
	if (words[lineNum][0][0] == '/' && words[lineNum][0][1] == '/')
		return nullType;

	// If print statement (deprecated, now use SLB.System.Print() function)
	else if (words[lineNum][0] == "print")
	{
		cout << StringRaw(AnyAsString(EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variableValues))) << endl;
		return nullType;
	}

	// Check if function return
	else if (words[lineNum][0] == "return")
		return EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variableValues);

	// Check if it is SLB Builtin function
	else if (words[lineNum][0][0] == 'S' && words[lineNum][0][1] == 'L' && words[lineNum][0][2] == 'B' && words[lineNum][0][3] == '.')
		return EvalExpression(unWrapVec(words[lineNum]), variableValues);

	// Check if it is function
	else if (IsFunction(trim(split(words[lineNum][0], '(')[0])))
	{
		if (count(words[lineNum][0], '(') > 0 && count(words[lineNum][0], ')') > 0)
			ExecuteFunction(trim(split(words[lineNum][0], '(')[0]), vector<boost::any>());
		else
			ExecuteFunction(trim(split(words[lineNum][0], '(')[0]), VarValues(split(RMParenthesis("(" + split(unWrapVec(rangeInVec(words[lineNum], 0, (int)words[lineNum].size() - 1)), '(')[1]), ','), variableValues));
		return nullType;
	}

	// Check if global variable declaration
	else if (trim(words[lineNum][0]) == "global")
	{
		globalVariableValues[words[lineNum][2]] = EvalExpression(unWrapVec(slice(words[lineNum], 4, -1)), variableValues);
		return nullType;
	}

	// Iterate through all types to see if line inits or
	// re-inits a variable then store it with it's value
	else if (countInVector(types, trim(words[lineNum][0])) > 0)
	{
		variableValues[words[lineNum][1]] = EvalExpression(unWrapVec(slice(words[lineNum], 3, -1)), variableValues);
		return nullType;
	}

	// Check existing variables: If matches, then it means
	// the variables value is getting changed with an operator
	else if (count(words[lineNum][0], '.') == 0 && (IsVar(words[lineNum][0], variableValues) || IsVar(words[lineNum][0], globalVariableValues)))
	{
		// Evaluates what the operator (ex. '=', '+=') does to the value on the left by the value on the right
		varOperation(vector<string>(words[lineNum].begin(), words[lineNum].end()), variableValues);
		return nullType;
	}

	// Check existing variables: To see if class sub component matches
	else if (count(words[lineNum][0], '.') > 0 && IsVar(split(words[lineNum][0], '.')[0], variableValues) || IsVar(split(words[lineNum][0], '.')[0], globalVariableValues))
	{
		if (IsVar(split(words[lineNum][0], '.')[0], variableValues))
			variableValues[split(words[lineNum][0], '.')[0]] = EditClassSubComponent(variableValues[split(words[lineNum][0], '.')[0]], words[lineNum][1], EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 2, words[lineNum].end())), variableValues), split(words[lineNum][0], '.')[1]);
		else if (IsVar(split(words[lineNum][0], '.')[0], globalVariableValues))
			globalVariableValues[split(words[lineNum][0], '.')[0]] = EditClassSubComponent(globalVariableValues[split(words[lineNum][0], '.')[0]], words[lineNum][1], EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 2, words[lineNum].end())), variableValues), split(words[lineNum][0], '.')[1]);
		return nullType;
	}

	// Gathers while loop contents
	else if (words[lineNum][0] == "while")
	{
		vector<vector<string>> whileContents;
		vector<string> whileParameters;

		for (int w = 1; w < (int)words[lineNum].size(); w++)
			whileParameters.push_back(words[lineNum][w]);

		int numOfBrackets = 1;
		for (int p = lineNum + 2; p < (int)words.size(); p++)
		{
			numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
			if (numOfBrackets == 0)
				break;
			whileContents.push_back(words[p]);
		}
		whileContents = removeTabsWdArry(whileContents, 1);

		while (BooleanLogic(whileParameters[0], whileParameters[1], whileParameters[2], variableValues))
		{
			//Iterate through all lines in while loop
			for (int lineNum = 0; lineNum < (int)whileContents.size(); lineNum++)
			{
				boost::any returnVal = ProcessLine(whileContents, lineNum, variableValues);
				if (!returnVal.empty())
					return returnVal;
			}
		}
		return nullType;
	}

	// Gathers if statement contents
	else if (words[lineNum][0] == "if")
	{
		vector<vector<string>> ifContents;
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
			ifContents.push_back(words[lineNum]);
			lineNum++;
		}
		ifContents = removeTabsWdArry(ifContents, 1);

		if (BooleanLogic(ifParameters[0], ifParameters[1], ifParameters[2], variableValues))
		{
			//Iterate through all lines in if statement
			for (int l = 0; l < (int)ifContents.size(); l++)
			{
				boost::any returnVal = ProcessLine(ifContents, l, variableValues);
				if (!returnVal.empty())
					return returnVal;
			}
		}
		//else if (words.size() > lineNum + 1)
		//	if (words[lineNum + 1][0] == "else")
		//	{
		//		lineNum += 1;

		//		vector<string> elseContents;

		//		int numOfBrackets = 1;
		//		while (lineNum < (int)words.size())
		//		{
		//			numOfBrackets += countInVector(words[lineNum], "{") - countInVector(words[lineNum], "}");
		//			if (numOfBrackets == 0)
		//				break;
		//			elseContents.push_back("");
		//			for (int w = 0; w < (int)words[lineNum].size(); w++)
		//			{
		//				elseContents[(int)elseContents.size() - 1] += words[lineNum][w] + " ";
		//			}
		//			lineNum++;
		//		}
		//		elseContents = removeTabs(elseContents, 2);

		//		vector<vector<string>> innerWords;
		//		for (int i = 0; i < (int)elseContents.size(); i++)
		//			innerWords.push_back(split(elseContents[i], ' '));

		//		//Iterate through all lines in else statement
		//		for (int lineNum = 0; lineNum < (int)elseContents.size(); lineNum++)
		//		{
		//			ProcessLine(innerWords, lineNum, variableValues);
		//		}
		//		return nullType;
		//	}
		return nullType;
	}
	//// Gathers else statement contents
	//if (words[lineNum][0] == "else")
	//{
	//	
	//}

	return nullType;
}

boost::any ExecuteFunction(const string& functionName, const vector<boost::any>& inputVarVals)
{
	// Get contents of function
	vector<vector<string>> words = functionValues[functionName];

	unordered_map<string, boost::any> variableValues = {};
	vector<string> args = words[0];
	for (int i = 0; i < (int)inputVarVals.size(); i++) {

		variableValues[args[i]] = inputVarVals[i];
#if DEVELOPER_MESSAGES == true
		cout << functionName + "  \x1B[33m" << args[i] << " == " << AnyAsString(inputVarVals[i]) << "\033[0m\t\t" << endl;
#endif
	}

	//Iterate through all lines in function
	for (int lineNum = 1; lineNum < (int)words.size(); lineNum++)
	{
		try
		{
			boost::any returnVal = ProcessLine(words, lineNum, variableValues);
			if (!returnVal.empty())
				return returnVal;
		}
		catch (const std::exception&)
		{
			LogCriticalError("\'" + unWrapVec(words[lineNum]) + "\'\n                  In function: " + functionName + "\n                  Line: " + to_string(lineNum));
		}
	}
	return nullType;
}

int parseSlang(string script)
{
	script = replace(script, "    ", "\t");

	vector<string> lines = split(script, '\n');
	vector<vector<string>> words;
	for (int i = 0; i < (int)lines.size(); i++)
		words.push_back(split(lines[i], ' '));

	// First go through entire script and iterate through all types to see if line is a variable
	// or function declaration, then store it with it's value
	for (int lineNum = 0; lineNum < (int)words.size(); lineNum++)
	{
		//Checks if it is function
		if (words[lineNum][0] == "func")
		{
			vector<vector<string>> functionContents;

			string functName = split(words[lineNum][1], '(')[0];

			string args = "";
			if (indexInStr(unWrapVec(words[lineNum]), ')') - indexInStr(unWrapVec(words[lineNum]), '(') > 1)
				for (int w = 1; w < (int)words[lineNum].size(); w++) // Get all words from the instantiation line: these are the args
				{
					args += replace(replace(words[lineNum][w], "(", " "), ")", "");
				}

			args = trim(replace(args, functName + " ", ""));
			functionContents.push_back(split(args, ','));

			int numOfBrackets = 1;
			for (int p = lineNum + 2; p < (int)words.size(); p++)
			{
				numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
				if (numOfBrackets == 0)
					break;
				functionContents.push_back(removeTabs(words[p], 1));
			}
			functionValues[functName] = functionContents;
			//cout << functName << " is \n" << Vec2Str(functionContents) << endl << endl;
		}
		else
		{
			if (words[lineNum][0] == "string")
				globalVariableValues[words[lineNum][1]] = StringRaw(words[lineNum][3]);
			else if (words[lineNum][0] == "int")
				globalVariableValues[words[lineNum][1]] = stoi(words[lineNum][3]);
			else if (words[lineNum][0] == "float")
				globalVariableValues[words[lineNum][1]] = stof(words[lineNum][3]);
			else if (words[lineNum][0] == "bool")
				globalVariableValues[words[lineNum][1]] = stob(words[lineNum][3]);
			//else
			//	LogWarning("unrecognized type \'" + words[lineNum][0] + "\' on line: " + to_string(lineNum));
		}
	}

	// Executes main, which is the starting function
	ExecuteFunction("Main", vector<boost::any> {});

	return 0;
}

int main(int argc, char* argv[])
{
	// Gathers builtin functions and variables
	GetBuiltins(SLBContents);
	functionValues = builtinFunctionValues;
	globalVariableValues = builtinVarVals;

	stringstream scriptString;
#if EXAMPLE_PROJECT == false
	// If scriptname is supplied and not in developer mode
	if (argc > 1)
	{
		std::string scriptPath = argv[1];
#if DEVELOPER_MESSAGES == true
		cout << scriptPath << endl;
#endif
		std::string projectDirectory = scriptPath.substr(0, scriptPath.find_last_of("/\\"));
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(projectDirectory);

#if defined(__unix__)
		chdir(projectDirectory.c_str());
#elif defined(_MSC_VER)
		LPCWSTR s = wide.c_str();
		SetCurrentDirectory(s);
#endif

		// Get script contents
		ifstream script(scriptPath);
		scriptString << script.rdbuf();
	}
	else
	{
		LogWarning("No script provided! Please drag and drop .SLG file over interpreter executable file, or provide it's path as a command-line argument.");
		system("pause");
		exit(1);
	}
#else
	// If in developer mode
	std::string scriptPath = "./Pong-Example-Project/script.slg";
#if DEVELOPER_MESSAGES == true
	cout << scriptPath << endl;
#endif
	std::string projectDirectory = scriptPath.substr(0, scriptPath.find_last_of("/\\"));
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(projectDirectory);

#if defined(__unix__)
	chdir(projectDirectory.c_str());
#elif defined(_MSC_VER)
	LPCSTR s = projectDirectory.c_str();
	SetCurrentDirectory(s);
#endif
	// Get script contents
	ifstream script(scriptPath);
	scriptString << script.rdbuf();
#endif

	system("pause");

	parseSlang(scriptString.str());

	return 0;
}
