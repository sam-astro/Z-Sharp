
#include <iostream>
#include <fstream>
#include <string>
//bool DEVELOPER_MESSAGES = true;
#define DEVELOPER_MESSAGES false
#define EXAMPLE_PROJECT false
#define NAMEVERSION "ZSharp v2.1.3-alpha"

#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#include <regex>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <boost/any.hpp>
#include <unordered_map>
#include <stdio.h>
#include <codecvt>
#include <thread>

#if UNIX
#include <unistd.h>
#include <filesystem>
#elif WINDOWS
#include <windows.h>
#endif

#include "eval.h"
#include "strops.h"
#include "builtin.h"
#include "main.h"
#include "anyops.h"

#include "ZS.h"

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

// Check if there is a variable with the specified name
bool IsVar(const string& varName, const unordered_map<string, boost::any>& variableValues)
{
	if (split(varName, '.')[0] == "ZS")
		return false;

	if (variableValues.find(split(varName, '.')[0]) != variableValues.end())
		return true;

	return false;
}

// Return a vector of values that correspond to a vector of input variable names
vector<boost::any> VarValues(const vector<string>& varNames, unordered_map<string, boost::any>& variableValues)
{
	vector<boost::any> realValues;

	for (int varIndex = 0; varIndex < varNames.size(); varIndex++)
	{
		string varName = trim(varNames.at(varIndex));

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

void printVarValues(const vector<string>& vec, unordered_map<string, boost::any>& variableValues)
{
	cout << "<";
	for (int i = 0; i < vec.size(); i++)
	{
		cout << AnyAsString(GetVariableValue(vec.at(i), globalVariableValues));
		cout << AnyAsString(GetVariableValue(vec.at(i), variableValues));
		cout << " | ";
	}
	cout << ">";
	cout << endl;
}


bool IsFunction(const string& funcName)
{
	if (functionValues.find(funcName) != functionValues.end())
		return true;
	else
		return false;
}
bool IsZSFunction(const string& funcName)
{
	return startsWith(funcName, "ZS.");
}

boost::any EvalExpression(const string& ex, unordered_map<string, boost::any>& variableValues)
{
	string expression = trim(ex);
	bool inQuotes = false;

#if DEVELOPER_MESSAGES == true
	//InterpreterLog("	old expression: |" + expression + "|");
#endif

	bool isFunc = IsFunction(split(expression, '(')[0]);
	bool isZS = split(expression, '.')[0] == "ZS";
	// If no operations are applied, then return self
	if ((countOutsideParenthesis(expression, '+') == 0 && countOutsideParenthesis(expression, '-') == 0 && countOutsideParenthesis(expression, '*') == 0 && countOutsideParenthesis(expression, '/') == 0 && countOutsideParenthesis(expression, '^') == 0) || split(expression, '.')[0] == "ZS")
	{
		//bool isFunc = IsFunction(split(expression, '(')[0]);
		if (isFunc && !inQuotes)
		{
			// start -> FuncCall(0, x, OtherFunc(a))
			// changeto -> 0, x, OtherFunc(a)
			string insideFunArgs = betweenChars(expression, '(', ')');
			vector<string> argList = splitNoOverlap(insideFunArgs, ',', '(', ')');
#if DEVELOPER_MESSAGES == true
			cout << split(expression, '(')[0] << "  [" << unWrapVec(argList) << "]" << endl;
			printVarValues(argList, variableValues);
#endif
			vector<boost::any> funcArgs = VarValues(argList, variableValues);
			return ExecuteFunction(split(expression, '(')[0], funcArgs);
		}
		else if (isZS && !inQuotes)
		{
			// start -> FuncCall(0, x, OtherFunc(a))
			// changeto -> 0, x, OtherFunc(a)
			string insideFunArgs = betweenChars(expression, '(', ')');
			vector<string> argList = splitNoOverlap(insideFunArgs, ',', '(', ')');
#if DEVELOPER_MESSAGES == true
			cout << split(expression, '(')[0] << "  [" << unWrapVec(argList) << "]" << endl;
			printVarValues(argList, variableValues);
#endif
			vector<boost::any> funcArgs = VarValues(argList, variableValues);
			return ZSFunction(split(expression, '(')[0], funcArgs);
		}
		else
			return GetVariableValue(expression, variableValues);
	}

	string newExpression = "";
	inQuotes = false;

	for (int i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '\"' && !isEscaped(newExpression, i))
			inQuotes = !inQuotes;

		if (isalpha(expression[i]) || expression[i] == '_')
		{
			string name = "";

			while (i < expression.size() && (isalpha(expression[i]) || expression[i] == '.' || expression[i] == '_'))
			{
				name += expression[i];
				i++;
			}

			//string varVal = GetVariableValue(name, variables, variableValues);
			bool isFunc = IsFunction(name);
			if (isFunc && !inQuotes)
			{
				// start -> FuncCall(0, x, OtherFunc(a))
				// changeto -> 0, x, OtherFunc(a)
				string insideFunArgs = betweenChars(expression, '(', ')');
				vector<string> argList = splitNoOverlap(insideFunArgs, ',', '(', ')');
#if DEVELOPER_MESSAGES == true
				cout << "[" << unWrapVec(argList) << "]" << endl;
				printVarValues(argList, variableValues);
#endif
				vector<boost::any> funcArgs = VarValues(argList, variableValues);
				string returnVal = AnyAsString(ExecuteFunction(split(expression, '(')[0], funcArgs));
				newExpression += returnVal;
			}
			else if (split(name, '.')[0] == "ZS" && !inQuotes)
			{
				// start -> FuncCall(0, x, OtherFunc(a))
				// changeto -> 0, x, OtherFunc(a)
				string insideFunArgs = betweenChars(expression, '(', ')');
				vector<string> argList = splitNoOverlap(insideFunArgs, ',', '(', ')');
#if DEVELOPER_MESSAGES == true
				cout << "[" << unWrapVec(argList) << "]" << endl;
				printVarValues(argList, variableValues);
#endif
				vector<boost::any> funcArgs = VarValues(argList, variableValues);
				string returnVal = AnyAsString(ZSFunction(split(expression, '(')[0], funcArgs));
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
	//InterpreterLog("	new expression: |" + newExpression + "|");
#endif

	bool addStrings = false;
	for (int i = 0; i < (int)newExpression.size(); i++)
		if (isalpha(newExpression[i]) || (newExpression[i] == '\"' && !isEscaped(newExpression, i)))
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
			if (newExpression[i] == '\"' && !isEscaped(newExpression, i))
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

bool BooleanLogic(const string& valA, const string& comparer, const string& valB, unordered_map<string, boost::any>& variableValues)
{
	boost::any valARealValue;
	boost::any valBRealValue;
	if (valA != "")
		valARealValue = EvalExpression(valA, variableValues);
	if (valB != "")
		valBRealValue = EvalExpression(valB, variableValues);
#if DEVELOPER_MESSAGES == true
	InterpreterLog(AnyAsString(valARealValue) + " " + comparer + " " + AnyAsString(valBRealValue) + " : " + AnyAsString(valA) + " " + comparer + " " + AnyAsString(valB) + " : " + to_string(AnyAsString(valARealValue) == AnyAsString(valBRealValue)));
#endif
	if (comparer == "==")
		return any_compare(valARealValue, valBRealValue);
	else if (comparer == "!=")
		return !any_compare(valARealValue, valBRealValue);
	else if (comparer == ">=")
		return AnyAsFloat(valARealValue) >= AnyAsFloat(valBRealValue);
	else if (comparer == "<=")
		return AnyAsFloat(valARealValue) <= AnyAsFloat(valBRealValue);
	else if (comparer == ">")
		return AnyAsFloat(valARealValue) > AnyAsFloat(valBRealValue);
	else if (comparer == "<")
		return AnyAsFloat(valARealValue) < AnyAsFloat(valBRealValue);
	else if (comparer == "")
		return AnyAsBool(valARealValue) == true;
	else
		LogWarning("unrecognized comparer \'" + comparer + "\'");

	return false;
}

int varOperation(const vector<string>& str, unordered_map<string, boost::any>& variableValues)
{
	if (IsVar(str.at(0), variableValues))
	{
		// Checks if type is simple, like int or string
		if (any_type(variableValues[str.at(0)]) <= 3)
		{
			if (str.at(1) == "=")
				variableValues[str.at(0)] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			else if (str.at(1) == "+=")
				variableValues[str.at(0)] = EvalExpression(str.at(0) + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
			else if (str.at(1) == "-=")
				variableValues[str.at(0)] = AnyAsFloat(variableValues[str.at(0)]) - AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str.at(1) == "*=")
				variableValues[str.at(0)] = AnyAsFloat(variableValues[str.at(0)]) * AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str.at(1) == "/=")
				variableValues[str.at(0)] = AnyAsFloat(variableValues[str.at(0)]) / AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else
				LogWarning("unrecognized operator \'" + str.at(1) + "\'");
		}
		// Else it is a Vec2. No other complex class can be operated on it's base form (ex. you can't do: Sprite += Sprite)
		else if (any_type(variableValues[str.at(0)]) == 5)
		{
			boost::any otherExpression = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			if (str.at(1) == "=")
				variableValues[str.at(0)] = otherExpression;
			else if (str.at(1) == "+=")
				variableValues[str.at(0)] = AnyAsVec2(variableValues[str.at(0)]) + AnyAsVec2(otherExpression);
			else if (str.at(1) == "-=")
				variableValues[str.at(0)] = AnyAsVec2(variableValues[str.at(0)]) - AnyAsVec2(otherExpression);
			else if (str.at(1) == "*=")
				variableValues[str.at(0)] = AnyAsVec2(variableValues[str.at(0)]) * AnyAsFloat(otherExpression);
			else if (str.at(1) == "/=")
				variableValues[str.at(0)] = AnyAsVec2(variableValues[str.at(0)]) / AnyAsFloat(otherExpression);
			else
				LogWarning("unrecognized operator \'" + str.at(1) + "\'");
		}
		return 0;
	}
	else if (IsVar(str.at(0), globalVariableValues))
	{
		// Checks if type is simple, like int or string
		if (any_type(globalVariableValues[str.at(0)]) <= 3)
		{
			if (str.at(1) == "=")
				globalVariableValues[str.at(0)] = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			else if (str.at(1) == "+=")
				globalVariableValues[str.at(0)] = EvalExpression(str.at(0) + "+(" + unWrapVec(vector<string>(str.begin() + 2, str.end())) + ")", variableValues);
			else if (str.at(1) == "-=")
				globalVariableValues[str.at(0)] = AnyAsFloat(globalVariableValues[str.at(0)]) - AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str.at(1) == "*=")
				globalVariableValues[str.at(0)] = AnyAsFloat(globalVariableValues[str.at(0)]) * AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else if (str.at(1) == "/=")
				globalVariableValues[str.at(0)] = AnyAsFloat(globalVariableValues[str.at(0)]) / AnyAsFloat(EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues));
			else
				LogWarning("unrecognized operator \'" + str.at(1) + "\'");
		}
		// Else it is a Vec2. No other complex class can be operated on it's base form (ex. you can't do: Sprite += Sprite)
		else if (any_type(globalVariableValues[str.at(0)]) == 5)
		{
			boost::any otherExpression = EvalExpression(unWrapVec(vector<string>(str.begin() + 2, str.end())), variableValues);
			if (str.at(1) == "=")
				globalVariableValues[str.at(0)] = otherExpression;
			else if (str.at(1) == "+=")
				globalVariableValues[str.at(0)] = AnyAsVec2(globalVariableValues[str.at(0)]) + AnyAsVec2(otherExpression);
			else if (str.at(1) == "-=")
				globalVariableValues[str.at(0)] = AnyAsVec2(globalVariableValues[str.at(0)]) - AnyAsVec2(otherExpression);
			else if (str.at(1) == "*=")
				globalVariableValues[str.at(0)] = AnyAsVec2(globalVariableValues[str.at(0)]) * AnyAsFloat(otherExpression);
			else if (str.at(1) == "/=")
				globalVariableValues[str.at(0)] = AnyAsVec2(globalVariableValues[str.at(0)]) / AnyAsFloat(otherExpression);
			else
				LogWarning("unrecognized operator \'" + str.at(1) + "\'");
		}
		return 0;
	}
	LogWarning("uninitialized variable or typo in \'" + str.at(0) + "\'");
	return 1;
}

boost::any ProcessLine(const vector<vector<string>>& words, int& lineNum, unordered_map<string, boost::any>& variableValues)
{
	//// Check if the first two chars are '//', which would make it a comment
	//if (startsWith(words.at(lineNum).at(0), "//"))
	//	return nullType;

	// If print statement (deprecated, now use ZS.System.Print() function)
	if (words.at(lineNum).at(0) == "print")
	{
		cout << StringRaw(AnyAsString(EvalExpression(unWrapVec(vector<string>(words.at(lineNum).begin() + 1, words.at(lineNum).end())), variableValues))) << endl;
		return nullType;
	}

	// Check if it is a function return
	else if (words.at(lineNum).at(0) == "return")
		return EvalExpression(unWrapVec(vector<string>(words.at(lineNum).begin() + 1, words.at(lineNum).end())), variableValues);

	// Check if it is ZS Builtin function call
	else if (startsWith(words.at(lineNum).at(0), "ZS."))
		return EvalExpression(unWrapVec(words.at(lineNum)), variableValues);

	// Check if it is function call
	else if (IsFunction(split(words.at(lineNum).at(0), '(')[0]))
	{
		// start -> FuncCall(0, x, OtherFunc(a))
		// changeto -> 0, x, OtherFunc(a)
		string insideFunArgs = betweenChars(unWrapVec(words.at(lineNum)), '(', ')');
		vector<string> argList = splitNoOverlap(insideFunArgs, ',', '(', ')');
#if DEVELOPER_MESSAGES == true
		cout << unWrapVec(argList) << endl;
		printVarValues(argList, variableValues);
#endif
		vector<boost::any> funcArgs = VarValues(argList, variableValues);
		ExecuteFunction(split(words.at(lineNum).at(0), '(')[0], funcArgs);
		return nullType;
	}

	// Check if it is a SplitThread call
	else if (startsWith(words.at(lineNum).at(0), "SplitThread"))
	{
		vector<string> lineContents = words.at(lineNum);
		cout << "New Thread: " << words.at(lineNum).at(0) << endl;
		//lineContents.at(0) = betweenChars(lineContents.at(0), '(', ')');

		//cout << "debug: " << lineContents.at(0) << endl;

		//if (betweenChars(lineContents.at(0), '(', ')') == "")
		//	std::thread thread_obj(ExecuteFunction, trim(split(lineContents.at(0), '(')[0]), vector<boost::any>());
		//else
		//	std::thread thread_obj(ExecuteFunction, trim(split(lineContents.at(0), '(')[0]), VarValues(split(RMParenthesis("(" + split(unWrapVec(rangeInVec(lineContents, 0, (int)lineContents.size() - 2)), '(')[1]), ','), variableValues));
		return nullType;
	}

	// Check if global variable declaration
	else if (words.at(lineNum).at(0) == "global")
	{
		try
		{
			globalVariableValues[words.at(lineNum).at(2)] = EvalExpression(unWrapVec(slice(words.at(lineNum), 4, -1)), variableValues);
		}
		catch (const std::exception&)
		{
			LogCriticalError("Error at line: " + to_string(lineNum) + ", " + unWrapVec(words.at(lineNum)) + ", couldn't initialize variable.");
		}
		return nullType;
	}

	// Iterate through all types to see if line inits or
	// re-inits a variable then store it with it's value
	else if (countInVector(types, words.at(lineNum).at(0)) > 0)
	{
		try
		{
			variableValues[words.at(lineNum).at(1)] = EvalExpression(unWrapVec(slice(words.at(lineNum), 3, -1)), variableValues);
		}
		catch (const std::exception&)
		{
			LogCriticalError("Error at line: " + to_string(lineNum) + ", " + unWrapVec(words.at(lineNum)) + ", couldn't initialize variable.");
		}
		return nullType;
	}

	// Check existing variables: If matches, then it means
	// the variables value is getting changed with an operator
	else if (count(words.at(lineNum).at(0), '.') == 0 && (IsVar(words.at(lineNum).at(0), variableValues) || IsVar(words.at(lineNum).at(0), globalVariableValues)))
	{
		// Evaluates what the operator (ex. '=', '+=') does to the value on the left by the value on the right
		varOperation(vector<string>(words.at(lineNum).begin(), words.at(lineNum).end()), variableValues);
		return nullType;
	}

	// Check existing variables: To see if accessing class sub component
	else if (count(words.at(lineNum).at(0), '.') > 0 && IsVar(split(words.at(lineNum).at(0), '.')[0], variableValues) || IsVar(split(words.at(lineNum).at(0), '.')[0], globalVariableValues))
	{
		if (IsVar(split(words.at(lineNum).at(0), '.')[0], variableValues))
			variableValues[split(words.at(lineNum).at(0), '.')[0]] = EditClassSubComponent(variableValues[split(words.at(lineNum).at(0), '.')[0]], words.at(lineNum).at(1), EvalExpression(unWrapVec(vector<string>(words.at(lineNum).begin() + 2, words.at(lineNum).end())), variableValues), split(words.at(lineNum).at(0), '.')[1]);
		else if (IsVar(split(words.at(lineNum).at(0), '.')[0], globalVariableValues))
			globalVariableValues[split(words.at(lineNum).at(0), '.')[0]] = EditClassSubComponent(globalVariableValues[split(words.at(lineNum).at(0), '.')[0]], words.at(lineNum).at(1), EvalExpression(unWrapVec(vector<string>(words.at(lineNum).begin() + 2, words.at(lineNum).end())), variableValues), split(words.at(lineNum).at(0), '.')[1]);
		return nullType;
	}

	// If declaring a while loop, loop until false
	else if (words.at(lineNum).at(0) == "while")
	{
		vector<vector<string>> whileContents;
		vector<string> whileParameters;

		// Gather the parameters that must be == true for the loop to run
		int numOfBrackets = 0;
		for (int w = 1; w < (int)words.at(lineNum).size(); w++) {
			if (count(words.at(lineNum).at(w), '{') == 0)
				whileParameters.push_back(words.at(lineNum)[w]);
			else
			{
				whileParameters.push_back(replace(words.at(lineNum)[w], "{", ""));
				numOfBrackets = 1;
				break;
			}
		}

		// If the statement is already false, don't bother gathering the contents
		if (BooleanLogic(whileParameters.at(0), whileParameters.at(1), whileParameters.at(2), variableValues) == false) {
			lineNum++;
			while (lineNum < (int)words.size())
			{
				numOfBrackets += countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
				if (numOfBrackets == 0)
					break;
				lineNum++;
			}
			return nullType;
		}

		// Gather the contents
		lineNum++;
		while (lineNum < (int)words.size())
		{
			numOfBrackets += countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
			if (numOfBrackets == 0)
				break;
			whileContents.push_back(words.at(lineNum));
			lineNum++;
		}

		//whileContents = removeTabsWdArry(whileContents, 1);

		// Loop while true
		while (BooleanLogic(whileParameters.at(0), whileParameters.at(1), whileParameters.at(2), variableValues))
		{
			//Iterate through all lines in while loop
			for (int lineNum = 0; lineNum < (int)whileContents.size(); lineNum++)
			{
				if (whileContents.at(lineNum).at(0) == "continue")
					break; // Stops iterating through lines and return to beginning
				if (whileContents.at(lineNum).at(0) == "break")
					return nullType; // Stops iterating through lines and leave while loop
				boost::any returnVal = ProcessLine(whileContents, lineNum, variableValues);
				if (!returnVal.empty()) {
					try
					{
						BREAK t = any_cast<BREAK>(returnVal);
						return nullType;
					}
					catch (boost::bad_any_cast)
					{
						return returnVal;
					}
				}
			}
		}
		return nullType;
	}

	// If declaring an if statement, only execute if true
	else if (words.at(lineNum).at(0) == "if")
	{
		vector<vector<string>> ifContents;
		vector<string> ifParameters;

		// Gather the parameters that must be == true for the loop to run
		int numOfBrackets = 0;
		for (int w = 1; w < (int)words.at(lineNum).size(); w++) {
			if (count(words.at(lineNum).at(w), '{') == 0)
				ifParameters.push_back(words.at(lineNum)[w]);
			else
			{
				ifParameters.push_back(replace(words.at(lineNum)[w], "{", ""));
				numOfBrackets = 1;
				break;
			}
		}

		// If the statement is already false, don't bother gathering the contents
		if (BooleanLogic(ifParameters.at(0), ifParameters.at(1), ifParameters.at(2), variableValues) == false) {
			lineNum++;
			while (lineNum < (int)words.size())
			{
				numOfBrackets += countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
				if (numOfBrackets == 0)
					break;
				lineNum++;
			}
			return nullType;
		}

		// Gather the contents
		lineNum++;
		while (lineNum < (int)words.size())
		{
			numOfBrackets += countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
			if (numOfBrackets == 0)
				break;
			ifContents.push_back(words.at(lineNum));
			lineNum++;
		}
		//ifContents = removeTabsWdArry(ifContents, 1);

		// Execute if true
		if (BooleanLogic(ifParameters.at(0), ifParameters.at(1), ifParameters.at(2), variableValues))
		{
			//Iterate through all lines in if statement
			for (int l = 0; l < (int)ifContents.size(); l++)
			{
				if (ifContents.at(l).at(0) == "break")
					return breakReOp; // Stops iterating through lines and leave while loop
				boost::any returnVal = ProcessLine(ifContents, l, variableValues);
				if (!returnVal.empty())
					return returnVal;
			}
		}
		else if (words.size() > lineNum)
		{
			if (words[lineNum].at(0) == "else")
			{
				vector<vector<string>> elseContents;
				vector<string> elseParameters;

				int numOfBrackets = 0;
				for (int w = 1; w < (int)words.at(lineNum).size(); w++) {
					if (count(words.at(lineNum).at(w), '{') != 0)
					{
						numOfBrackets = 1;
						break;
					}
				}

				lineNum++;
				while (lineNum < (int)words.size())
				{
					numOfBrackets += countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
					if (numOfBrackets == 0)
						break;
					elseContents.push_back(words.at(lineNum));
					lineNum++;
				}

				//elseContents = removeTabsWdArry(elseContents, 1);

				//Iterate through all lines in else statement
				for (int l = 0; l < (int)elseContents.size(); l++)
				{
					boost::any returnVal = ProcessLine(elseContents, l, variableValues);
					if (!returnVal.empty())
						return returnVal;
				}

			}
		}
		return nullType;
	}
	return nullType;
}

boost::any ExecuteFunction(const string& functionName, const vector<boost::any>& inputVarVals)
{
	// Get contents of function from global function map
	std::vector<std::vector<std::string>> words = functionValues[functionName];

	unordered_map<string, boost::any> variableValues = {};

	std::vector<std::string> funcArgs = words.at(0);
	// Set function variables equal to whatever inputs were provided
	for (int i = 0; i < (int)inputVarVals.size(); i++)
	{
		if (i < funcArgs.size())
		{
			variableValues[funcArgs[i]] = inputVarVals[i];
#if DEVELOPER_MESSAGES == true
			cout << "in " << functionName + "  " << funcArgs[i] << " == " << AnyAsString(inputVarVals[i]) << endl;
#endif
		}
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
			LogCriticalError("Error at line: " + to_string(lineNum) + ", " + unWrapVec(words.at(lineNum)));
		}
	}
	return nullType;
}

int parseZSharp(string script)
{
	//script = replace(script, "    ", "\t"); // Replace spaces with tabs (not really required, and will break purposefull whitespace in strings etc.)

	// Split the script by newline, signifying a line ending
	vector<string> beforeProcessLines = split(script, '\n');
	vector<string> lines;
	for (int i = 0; i < (int)beforeProcessLines.size(); i++) { // Then split said lines into indiviual words
		if (!startsWith(trim(beforeProcessLines.at(i)), "//") && trim(beforeProcessLines.at(i)) != "")
		{ // dont include line if it is a comment or if it is blank
			lines.push_back(trim(beforeProcessLines.at(i)));
		}
	}
#if DEVELOPER_MESSAGES
	InterpreterLog("Contents:\n");
#endif
	vector<vector<string>> words;
	for (int i = 0; i < (int)lines.size(); i++) // Then split said lines into indiviual words
	{
		words.push_back(split(lines.at(i), ' '));
#if DEVELOPER_MESSAGES
		cout << unWrapVec(words.at(i)) << endl;
#endif
	}

#if DEVELOPER_MESSAGES
	InterpreterLog("Gather variables & functions...");
#endif
	// Go through entire script and iterate through all types to see if line is a variable
	// or function declaration, then store it with it's value
	for (int lineNum = 0; lineNum < (int)words.size(); lineNum++)
	{
		//Checks if it is function
		if (words.at(lineNum).at(0) == "func")
		{
			vector<vector<string>> functionContents;

			string functName = split(words.at(lineNum).at(1), '(')[0];
#if DEVELOPER_MESSAGES == true
			InterpreterLog("Load script function " + functName + "...");
#endif

			//string args = "";
			//if (indexInStr(unWrapVec(words.at(lineNum)), ')') - indexInStr(unWrapVec(words.at(lineNum)), '(') > 1)
			//	for (int w = 0; w < (int)words.at(lineNum).size(); w++) // Get all words from the instantiation line: these are the args
			//	{
			//		if (count(words.at(lineNum).at(w), '{') == 0)
			//			args += replace(replace(words.at(lineNum).at(w), "(", " "), ")", "");
			//	}
			string args = betweenChars(unWrapVec(words.at(lineNum)), '(', ')');
			//cout << functName << "<" << args << ">" << endl;

			//args = trim(replace(args, functName, ""));
			functionContents.push_back(split(replace(args, " ", ""), ','));


			int numOfBrackets = countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
			// Gather the contents
			lineNum++;
			while (lineNum < (int)words.size())
			{
				numOfBrackets += countInVector(words.at(lineNum), "{") - countInVector(words.at(lineNum), "}");
				if (numOfBrackets == 0)
					break;
				functionContents.push_back(words.at(lineNum));
				//cout << functName << "<" << args << ">" << endl;
				lineNum++;
			}
			//functionContents = removeTabsWdArry(functionContents, 1);

			/*int numOfBrackets = 0;
			for (int w = 1; w < (int)words.at(lineNum).size(); w++) {
				if (count(words.at(lineNum).at(w), '{') != 0) {
					numOfBrackets = 1;
					break;
				}
			}

			for (int p = lineNum + 1; p < (int)words.size(); p++)
			{
				numOfBrackets += countInVector(words.at(p), "{") - countInVector(words.at(p), "}");
				if (numOfBrackets == 0)
					break;
				functionContents.push_back(removeTabs(words.at(p), 1));
			}*/
			functionValues[functName] = functionContents;
		}
		else
		{
			if (words.at(lineNum).at(0) == "include")
			{
				string scriptPath = StringRaw(words.at(lineNum).at(1));
				string scriptTextContents;
#if DEVELOPER_MESSAGES == true
				InterpreterLog("Including from " + words.at(lineNum).at(1) + "...");
#endif
#if UNIX
				// Get script contents as single string
				auto ss = ostringstream{};
				ifstream input_file(scriptPath);
				ss << input_file.rdbuf();
				scriptTextContents = ss.str();
#if DEVELOPER_MESSAGES
				InterpreterLog("Gather script contents...");
#endif
#elif WINDOWS
				// Get script contents as single string
				ifstream script(scriptPath);
				stringstream scriptString;
				scriptString << script.rdbuf();
				scriptTextContents = scriptString.str();
#if DEVELOPER_MESSAGES
				InterpreterLog("Gather script contents...");
#endif
#endif
				parseZSharp(scriptTextContents);
			}


			else if (words.at(lineNum).at(0) == "string") {
				globalVariableValues[words.at(lineNum).at(1)] = StringRaw(words.at(lineNum).at(3));
#if DEVELOPER_MESSAGES == true
				InterpreterLog("Load script variable " + words.at(lineNum).at(1) + "...");
#endif
			}

			// Iterate through all types to see if line inits or
			// re-inits a variable then store it with it's value
			else if (countInVector(types, trim(words.at(lineNum).at(0))) > 0)
			{
				//cout << words.at(lineNum).at(1) << "=" << unWrapVec(slice(words.at(lineNum), 3, -1)) << "=" << AnyAsString(EvalExpression(unWrapVec(slice(words.at(lineNum), 3, -1)), variableValues)) << endl;
				globalVariableValues[words.at(lineNum).at(1)] = EvalExpression(unWrapVec(slice(words.at(lineNum), 3, -1)), globalVariableValues);
			}
			//			else if (words.at(lineNum).at(0) == "int") {
			//				globalVariableValues[words.at(lineNum).at(1)] = stoi(words.at(lineNum).at(3));
			//#if DEVELOPER_MESSAGES == true
			//				InterpreterLog("Load script variable " + words.at(lineNum).at(1) + "...");
			//#endif
			//			}
			//			else if (words.at(lineNum).at(0) == "float") {
			//				globalVariableValues[words.at(lineNum).at(1)] = stof(words.at(lineNum).at(3));
			//#if DEVELOPER_MESSAGES == true
			//				InterpreterLog("Load script variable " + words.at(lineNum).at(1) + "...");
			//#endif
			//			}
			//			else if (words.at(lineNum).at(0) == "bool") {
			//				globalVariableValues[words.at(lineNum).at(1)] = stob(words.at(lineNum).at(3));
			//#if DEVELOPER_MESSAGES == true
			//				InterpreterLog("Load script variable " + words.at(lineNum).at(1) + "...");
			//#endif
			//			}
			else
				LogWarning("unrecognized type \'" + words.at(lineNum).at(0) + "\' on line: " + to_string(lineNum));
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	// Print the name of the interpreter and it's version in inverted black on white text
	PrintColored(NAMEVERSION, blackFGColor, whiteBGColor, false);
	cout << endl << endl;

	// Gathers builtin functions and variables
	parseZSharp(ZSContents);
	//functionValues = builtinFunctionValues;
	//globalVariableValues = builtinVarVals;

	std::string scriptTextContents;

	// If scriptname is supplied and not in developer mode
	if (argc > 1 || EXAMPLE_PROJECT)
	{
		std::string scriptPath;
		if (EXAMPLE_PROJECT)
			scriptPath = "D:\\Code\\Z-Sharp\\examples\\Platformer\\script.zs";
		else
			scriptPath = argv[1];
#if DEVELOPER_MESSAGES
		cout << scriptPath << endl;
#endif
		if (!fileExists(scriptPath))
			LogCriticalError("Failed to load script from path: \"" + scriptPath + "\"");

		std::string projectDirectory = scriptPath.substr(0, scriptPath.find_last_of("/\\"));
#if UNIX
		// Get script contents as single string
		auto ss = ostringstream{};
		ifstream input_file(scriptPath);
		ss << input_file.rdbuf();
		scriptTextContents = ss.str();
#if DEVELOPER_MESSAGES
		InterpreterLog("Gather script contents...");
#endif

		// Change the current working directory to that of the script
		int chErr = chdir(projectDirectory.c_str());
		if (chErr < 0)
			LogCriticalError("Failed to change directory to: \"" + projectDirectory + "\", error num: " + to_string(chErr));

#if DEVELOPER_MESSAGES
		InterpreterLog("Changed directory to " + projectDirectory + "...");
#endif

#elif WINDOWS
		// Get script contents as single string
		ifstream script(scriptPath);
		stringstream scriptString;
		scriptString << script.rdbuf();
		scriptTextContents = scriptString.str();
#if DEVELOPER_MESSAGES
		InterpreterLog("Gather script contents...");
#endif

		// Change the current working directory to that of the script
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(projectDirectory);
		LPCWSTR s = wide.c_str();
		SetCurrentDirectory(s);
#if DEVELOPER_MESSAGES
		InterpreterLog("Changed directory to " + projectDirectory + "...");
#endif
#endif
	}
	else
	{ // If no script is provided as an argument throw error
		LogWarning("No script provided! Please drag and drop .ZS file over interpreter executable file, or provide it's path as a command-line argument.");
		InterpreterLog("Press Enter to Exit...");
		cin.ignore();
		exit(1);
	}

#if DEVELOPER_MESSAGES
	InterpreterLog("Parsing...");
#endif
	// Parse the script
	parseZSharp(scriptTextContents);
#if DEVELOPER_MESSAGES
	InterpreterLog("Start Main()");
#endif
	try
	{
		// Executes main, which is the entry point function
		ExecuteFunction("Main", vector<boost::any> {});
	}
	catch (const std::exception&)
	{
		//Failed with error
	}

	// Entire script has been run, exit.
#if DEVELOPER_MESSAGES // If built with developer messages, then verify exit
	InterpreterLog("Press Enter to Exit...");
	cin.ignore();
	exit(1);
#else
	if (argc > 2)
	{
		string a = argv[2];
		std::transform(a.begin(), a.end(), a.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (a == "-ve") // If the '-ve' (verify exit) option is used, ask for verification on exit
		{
			InterpreterLog("Press Enter to Exit...");
			cin.ignore();
			exit(1);
		}
	}
	else if (AnyAsBool(GetVariableValue("EXIT_WHEN_DONE", globalVariableValues)) == false)
	{
		InterpreterLog("Press Enter to Exit...");
		cin.ignore();
		exit(1);
	}
#endif // Else exit automatically
	return 0;
}
