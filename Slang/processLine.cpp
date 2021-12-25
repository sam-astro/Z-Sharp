//#include <iostream>
//#include <fstream>
//#include <string>
//#include <regex>
//#include "Main.cpp"
//
//using namespace std;
//
//int ProcessLine(vector<vector<string>> words, int l, vector<string> variables, vector<string> variableValues)
//{
//	if (words[l][0] == "print") {
//		cout << GetRealValue(words[l][1], variables, variableValues) << endl;
//		return 0;
//	}
//
//	// Iterate through all functions
//	for (int t = 0; t < (int)functions.size(); t++)
//	{
//		if (words[l][0] == split(functions[t], ' ')[0])
//		{
//			ExecuteFunction(words[l][0], rangeInVec(words[l], 1, -1));
//			return 0;
//		}
//	}
//
//	// First iterate through all types to see if line
//	// is a variable then store it with it's value
//	for (int t = 0; t < (int)types.size(); t++)
//	{
//		if (words[l][0] == types[t])
//		{
//			//Checks if it is variable
//			variables.push_back(words[l][1]);
//			variableValues.push_back((string)words[l][3]);
//			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
//			return 0;
//		}
//	}
//	// Second, iterate all existing local variable names
//	for (int v = 0; v < (int)variables.size(); v++)
//	{
//		if (words[l][0] == variables[v])
//		{
//			if (words[l][1] == "=")
//				variableValues[v] = words[l][2];
//			else if (words[l][1] == "+=")
//				variableValues[v] = AddItem(variableValues[v], words[l][2]);
//			else if (words[l][1] == "-=")
//				variableValues[v] = to_string(stof(variableValues[v]) - stof(words[l][2]));
//			else if (words[l][1] == "*=")
//				variableValues[v] = to_string(stof(variableValues[v]) * stof(words[l][2]));
//			else if (words[l][1] == "/=")
//				variableValues[v] = to_string(stof(variableValues[v]) / stof(words[l][2]));
//
//			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
//			return 0;
//		}
//	}
//	// Third, iterate all existing global variable names
//	for (int v = 0; v < (int)globalVariables.size(); v++)
//	{
//		if (words[l][0] == globalVariables[v])
//		{
//			if (words[l][1] == "=")
//				globalVariableValues[v] = words[l][2];
//			else if (words[l][1] == "+=")
//				globalVariableValues[v] = AddItem(globalVariableValues[v], words[l][2]);
//			else if (words[l][1] == "-=")
//				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) - stof(words[l][2]));
//			else if (words[l][1] == "*=")
//				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) * stof(words[l][2]));
//			else if (words[l][1] == "/=")
//				globalVariableValues[v] = to_string(stof(globalVariableValues[v]) / stof(words[l][2]));
//
//			//cout << words[l][1] << " is " << words[l][3] << endl << endl;
//			return 0;
//		}
//	}
//	// Gathers while loop contents
//	if (words[l][0] == "while")
//	{
//		vector<string> whileContents;
//		vector<string> whileParameters;
//
//		for (int w = 1; w < (int)words[l].size(); w++)
//			whileParameters.push_back(words[l][w]);
//
//		int numOfBrackets = 1;
//		for (int p = l + 2; p < (int)words.size(); p++)
//		{
//			numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
//			if (numOfBrackets == 0)
//				break;
//			whileContents.push_back("");
//			for (int w = 0; w < (int)words[p].size(); w++)
//			{
//				whileContents[(int)whileContents.size() - 1] += words[p][w] + " ";
//			}
//		}
//		whileContents = removeTabs(whileContents, 1);
//
//		vector<vector<string>> words;
//		for (int i = 0; i < (int)whileContents.size(); i++)
//			words.push_back(split(whileContents[i], ' '));
//
//		while (BooleanLogic(whileParameters[0], whileParameters[1], whileParameters[2], variables, variableValues))
//		{
//			//Iterate through all lines in while loop
//			for (int l = 0; l < (int)whileContents.size(); l++)
//			{
//				ProcessLine(words, l, variables, variableValues);
//			}
//		}
//		return 0;
//	}
//	// Gathers if statement contents
//	if (words[l][0] == "if")
//	{
//		vector<string> ifContents;
//		vector<string> ifParameters;
//
//		for (int w = 1; w < (int)words[l].size(); w++)
//			ifParameters.push_back(words[l][w]);
//
//		int numOfBrackets = 1;
//		for (int p = l + 2; p < (int)words.size(); p++)
//		{
//			numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
//			if (numOfBrackets == 0)
//				break;
//			ifContents.push_back("");
//			for (int w = 0; w < (int)words[p].size(); w++)
//			{
//				ifContents[(int)ifContents.size() - 1] += words[p][w] + " ";
//			}
//		}
//		ifContents = removeTabs(ifContents, 1);
//
//		vector<vector<string>> words;
//		for (int i = 0; i < (int)ifContents.size(); i++)
//			words.push_back(split(ifContents[i], ' '));
//
//		if (BooleanLogic(ifParameters[0], ifParameters[1], ifParameters[2], variables, variableValues))
//		{
//			//Iterate through all lines in while loop
//			for (int l = 0; l < (int)ifContents.size(); l++)
//			{
//				ProcessLine(words, l, variables, variableValues);
//			}
//		}
//		return 0;
//	}
//
//	return 0;
//}