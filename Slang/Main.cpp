
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

#include "eval.h"
#include "strops.h"
#include "builtin.h"
#include "main.h"
#include "anyops.h"

using namespace std;
using namespace boost;

unordered_map<string, boost::any> globalVariableValues;
unordered_map<string, vector<vector<string>>> functionValues;

boost::any GetVariableValue(const string& varName, const unordered_map<string, boost::any>& variableValues)
{
	auto iA = variableValues.find(varName);
	if (iA != variableValues.end())
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

bool IsVar(const string& varName, const unordered_map<string, boost::any>& variableValues)
{
	if (variableValues.find(varName) != variableValues.end())
		return true;
	else
		return false;
}

vector<boost::any> VarValues(const vector<string>& varNames, const unordered_map<string, boost::any>& variableValues)
{
	vector<boost::any> realValues;

	for (int varIndex = 0; varIndex < varNames.size(); varIndex++)
	{
		string varName = trim(varNames[varIndex]);
		//cout << varName << endl;

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
				realValues.push_back(varName);
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
bool IsCPPFunction(const string& funcName)
{
	if (funcName[0] == 'C' && funcName[1] == 'P' && funcName[2] == 'P' && funcName[2] == '.')
		return true;
	else
		return false;
}

boost::any EvalExpression(const string& ex, unordered_map<string, boost::any>& variableValues)
{
	string expression = trim(ex);
	bool inQuotes = false;

	//CompilerLog("OLDEXPRESSION: |" + expression + "|");

	// If no operations are applied, then return self
	if ((count(expression, '+') == 0 && count(expression, '-') == 0 && count(expression, '*') == 0 && count(expression, '/') == 0 && count(expression, '(') == 0 && count(expression, '^') == 0) || split(expression, '.')[0] == "CPP")
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
			//CompilerLog(split(expression, '(')[0] + " " + AnyAsString(GetVariableValue(split(argContents, ',')[0], variableValues)));
			return ExecuteFunction(split(expression, '(')[0], VarValues(split(argContents, ','), variableValues));
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
			//CompilerLog(split(expression, '(')[0] + " " + argContents);
			return CPPFunction(split(expression, '(')[0], VarValues(split(argContents, ','), variableValues));
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
				//CompilerLog(split(expression, '(')[0] + " " + AnyAsString(GetVariableValue(split(argContents, ',')[0], variableValues)));
				string returnVal = AnyAsString(ExecuteFunction(name, VarValues(split(argContents, ','), variableValues)));
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
				string returnVal = AnyAsString(CPPFunction(split(name, '(')[0], VarValues(split(argContents, ','), variableValues)));
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
	//CompilerLog("NEW EXPRESSION: |" + newExpression + "|");

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

	if (determinant == "==")
		return AnyAsString(valARealValue) == AnyAsString(valBRealValue);
	else if (determinant == "!=")
		return AnyAsString(valARealValue) != AnyAsString(valBRealValue);
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
		//cout << variables[v] << " is " << variableValues[v] << endl;
		return 0;
	}
	else if (IsVar(str[0], globalVariableValues))
	{
		if (str[1] == "=")
			globalVariableValues[str[0]] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
		else if (str[1] == "+=")
			globalVariableValues[str[0]] = EvalExpression(str[0] + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
		else if (str[1] == "-=")
			globalVariableValues[str[0]] = AnyAsFloat(variableValues[str[0]]) - AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
		else if (str[1] == "*=")
			globalVariableValues[str[0]] = AnyAsFloat(variableValues[str[0]]) * AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
		else if (str[1] == "/=")
			globalVariableValues[str[0]] = AnyAsFloat(variableValues[str[0]]) / AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
		else
			LogWarning("unrecognized operator \'" + str[1] + "\'");
		//cout << variables[v] << " is " << variableValues[v] << endl;
		return 0;
	}
	LogWarning("uninitialized variable or typo in \'" + str[0] + "\'");
	return 1;
}

boost::any ProcessLine(const vector<vector<string>>& words, int lineNum, unordered_map<string, boost::any>& variableValues)
{
	//CompilerLog(unWrapVec(words[lineNum]));
	//CompilerLog(AnyAsString(GetVariableValue("out", variableValues)));

	if (words[lineNum][0][0] == '/' && words[lineNum][0][1] == '/')
		return nullType;

	// If print statement (deprecated, now use CPP.System.Print() function)
	else if (words[lineNum][0] == "print")
	{
		cout << AnyAsString(EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variableValues)) << endl;
		return nullType;
	}

	// Check if function return
	else if (words[lineNum][0] == "return")
		return EvalExpression(unWrapVec(vector<string>(words[lineNum].begin() + 1, words[lineNum].end())), variableValues);

	// Check if it is CPP Builtin function
	else if (words[lineNum][0][0] == 'C' && words[lineNum][0][1] == 'P' && words[lineNum][0][2] == 'P' && words[lineNum][0][3] == '.')
		return EvalExpression(unWrapVec(words[lineNum]), variableValues);

	// Check if it is function
	else if (IsFunction(trim(split(words[lineNum][0], '(')[0])))
	{
		ExecuteFunction(trim(split(words[lineNum][0], '(')[0]), VarValues(split(RMParenthesis(replace(unWrapVec(words[lineNum]), trim(split(words[lineNum][0], '(')[0]), "")), ','), variableValues));
		return nullType;
	}

	// Iterate through all types to see if line inits or
	// re-inits a variable then store it with it's value
	else if (countInVector(types, trim(words[lineNum][0])) > 0)
	{
		variableValues[words[lineNum][1]] = EvalExpression(unWrapVec(slice(words[lineNum], 3, -1)), variableValues);
		//CompilerLog("new var :: " + words[lineNum][1] + " = " + AnyAsString(variableValues[words[lineNum][1]]));
		return nullType;
	}

	// Check existing variables: If matches, then it means
	// the variables value is getting changed with an operator
	else if (IsVar(words[lineNum][0], variableValues) || IsVar(words[lineNum][0], globalVariableValues))
	{
		// Evaluates what the operator (ex. '=', '+=') does to the value on the left by the value on the right
		varOperation(vector<string>(words[lineNum].begin(), words[lineNum].end()), variableValues);
		return nullType;
	}

	// Gathers while loop contents
	else if (words[lineNum][0] == "while")
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

		while (BooleanLogic(whileParameters[0], whileParameters[1], whileParameters[2], variableValues))
		{
			//Iterate through all lines in while loop
			for (int lineNum = 0; lineNum < (int)whileContents.size(); lineNum++)
			{
				boost::any returnVal = ProcessLine(innerWords, lineNum, variableValues);
				if (!returnVal.empty())
					return returnVal;
			}
		}
		return nullType;
	}

	// Gathers if statement contents
	else if (words[lineNum][0] == "if")
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

		if (BooleanLogic(ifParameters[0], ifParameters[1], ifParameters[2], variableValues))
		{
			//Iterate through all lines in if statement
			for (int l = 0; l < (int)ifContents.size(); l++)
			{
				boost::any returnVal = ProcessLine(innerWords, l, variableValues);
				if (!returnVal.empty())
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
					innerWords.push_back(split(elseContents[i], ' '));

				//Iterate through all lines in else statement
				for (int lineNum = 0; lineNum < (int)elseContents.size(); lineNum++)
				{
					ProcessLine(innerWords, lineNum, variableValues);
				}
				return nullType;
			}
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
		//cout << "\x1B[33m" << args[i] << " == " << AnyAsString(inputVarVals[i]) << "\033[0m\t\t" << endl;
	}

	//Iterate through all lines in function
	for (int lineNum = 1; lineNum < (int)words.size(); lineNum++)
	{
		boost::any returnVal = 0;
		try
		{
			returnVal = ProcessLine(words, lineNum, variableValues);
		}
		catch (const std::exception&)
		{
			LogCriticalError("\'" + unWrapVec(words[lineNum]) + "\'\nIn function: " + functionName + "\nLine: " + to_string(lineNum));
		}
		if (!returnVal.empty())
			return returnVal;
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
			for (int w = 1; w < (int)words[lineNum].size(); w++) // Get all words from the instantiation line: these are the args
			{
				args += replace(replace(words[lineNum][w], "(", " "), ")", "");
			}

			args = replace(args, functName + " ", "");
			//CompilerLog(args);
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
	ExecuteFunction("Main", vector<boost::any> {"hi", 0});

	return 0;
}
//
//int main(int argc, char* argv[])
//{
//	// Get builtin script contents
//	ifstream builtin("../Slang/builtin.slg");
//	stringstream builtinString;
//	builtinString << builtin.rdbuf();
//
//	// Gathers builtin functions and variables
//	GetBuiltins(builtinString.str());
//	functionValues = builtinFunctionValues;
//	globalVariableValues = builtinVarVals;
//
//	// Get default script contents
//	ifstream script("../Slang/script.slg");
//	stringstream scriptString;
//	scriptString << script.rdbuf();
//
//	while (true) {
//		system("pause");
//		break;
//	}
//	parseSlang(scriptString.str());
//
//	return 0;
//}
