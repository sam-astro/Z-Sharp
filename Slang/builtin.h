#ifndef BUILTIN_H
#define BUILTIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <any>
#include <unordered_map>
#include "strops.h"
#include "graphics.h"
#include "anyops.h"

using namespace std;

vector<string> types = { "int", "float", "string", "bool", "void", "null" };

unordered_map<string, vector<vector<string>>> builtinFunctionValues;
unordered_map<string, any> builtinVarVals;

Parser mainWindow;


int LogWarning(const string& warningText)
{
	cerr << "\x1B[33mWARNING: " << warningText << "\033[0m\t\t" << endl;
	return 1;
}

int LogCriticalError(const string& errorText)
{
	cerr << "\x1B[31mERROR: " << errorText << "\033[0m\t\t" << endl;
	exit(EXIT_FAILURE);
	return 2;
}

// Initial script processing, which loads variables and functions from builtin
int GetBuiltins(const string& s)
{
	string script = replace(s, "    ", "\t");

	vector<string> lines = split(script, '\n');
	vector<vector<string>> words;
	for (int i = 0; i < (int)lines.size(); i++)
	{
		words.push_back(split(lines[i], ' '));
	}

	// Go through entire script and iterate through all types to see if line is a
	// function declaration, then store it with it's value
	for (int lineNum = 0; lineNum < (int)words.size(); lineNum++)
	{
		//Checks if it is function
		if (words[lineNum][0] == "func")
		{
			vector<vector<string>> functionContents;

			string functName = split(words[lineNum][1], '(')[0];

			string args = "";
			for (int w = 1; w < (int)words[lineNum].size(); w++) {
				if (w < (int)words[lineNum].size() - 1)
				{
					args += replace(replace(words[lineNum][w], "(", " "), ")", "") + ",";
				}
				else
				{
					args += replace(replace(words[lineNum][w], "(", " "), ")", "");
				}
			}

			args = replace(args, functName + ",", "");
			functionContents.push_back(vector<string>{args});

			int numOfBrackets = 1;
			for (int p = lineNum + 3; p < (int)words.size(); p++)
			{
				numOfBrackets += countInVector(words[p], "{") - countInVector(words[p], "}");
				if (numOfBrackets == 0)
					break;
				functionContents.push_back(removeTabs(words[p], 1));
			}
			builtinFunctionValues[functName] = functionContents;
			//cout << functName << " is \n" << Vec2Str(functionContents) << endl << endl;
		}
		else
		{
			if (words[lineNum][0] == "string")
				builtinVarVals[words[lineNum][1]] = StringRaw(words[lineNum][3]);
			else if (words[lineNum][0] == "int")
				builtinVarVals[words[lineNum][1]] = stoi(words[lineNum][3]);
			else if (words[lineNum][0] == "float")
				builtinVarVals[words[lineNum][1]] = stof(words[lineNum][3]);
			else if (words[lineNum][0] == "bool")
				builtinVarVals[words[lineNum][1]] = stob(words[lineNum][3]);
			else
				LogWarning("unrecognized type \'" + words[lineNum][0] + "\' on line: " + to_string(lineNum));
		}
	}

	return 0;
}

// Executes 
any CPPFunction(const string& name, const vector<any>& args)
{
	if (name == "CPP.Math.Sin")
		return sin(AnyAsFloat(args[0]));
	else if (name == "CPP.Math.Cos")
		return cos(AnyAsFloat(args[0]));
	else if (name == "CPP.Math.Tan")
		return tan(AnyAsFloat(args[0]));
	else if (name == "CPP.Math.Round")
		return AnyAsInt(args[0]);
	else if (name == "CPP.Graphics.Init")
	{
		cout << "\x1B[32mInit graphics\033[0m\t\t" << endl;
		if (mainWindow.Construct(AnyAsInt(args[0]), AnyAsInt(args[1]), AnyAsInt(args[2]), AnyAsInt(args[2])))
			mainWindow.Start();
	}
	else if (name == "CPP.Graphics.SetPixel")
		mainWindow.Draw(AnyAsInt(args[0]), AnyAsInt(args[1]), olc::Pixel(AnyAsInt(args[2]), AnyAsInt(args[3]), AnyAsInt(args[4])));
	else if (name == "CPP.System.Print")
		cout << AnyAsString(args[0]);
	else if (name == "CPP.System.PrintLine")
		cout << AnyAsString(args[0]) << endl;
	else
		LogWarning("CPP function \'" + name + "\' does not exist.");

	return 0;
}

#endif
