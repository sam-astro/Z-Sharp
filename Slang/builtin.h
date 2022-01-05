#ifndef BUILTIN_H
#define BUILTIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <boost/any.hpp>
#include "strops.h"
#include "graphics.h"
#include "anyops.h"

using namespace std;

vector<string> types = { "int", "float", "string", "bool", "void", "null" };

unordered_map<string, vector<string>> builtinFunctionValues;
unordered_map<string, any>& builtinVarVals;

Parser mainWindow;

// Initial script processing, which loads variables and functions from builtin
int GetBuiltins(string script)
{
	script = replace(script, "    ", "\t");

	vector<string> lines = split(script, '\n');
	vector<vector<string>> words;
	for (int i = 0; i < (int)lines.size(); i++)
	{
		words.push_back(split(lines[i], ' '));
	}

	// Go through entire script and iterate through all types to see if line is a
	// function declaration, then store it with it's value
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
					builtinFunctionValues[functName] = functionContents;
				}
				//Checks if it is variable
				else
				{
					if(words[lineNum][0] == "string")
						builtinVarVals[words[lineNum][1]] = words[lineNum][3];
					else if(words[lineNum][0] == "int")
						builtinVarVals[words[lineNum][1]] = stoi(words[lineNum][3]);
					else if(words[lineNum][0] == "float")
						builtinVarVals[words[lineNum][1]] = stof(words[lineNum][3]);
					else if(words[lineNum][0] == "bool")
						builtinVarVals[words[lineNum][1]] = stob(words[lineNum][3]);
					//cout << words[lineNum][1] << " is " << words[lineNum][3] << endl;
				}
			}

	return 0;
}

// Executes 
any CPPFunction(string name, vector<any> args)
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
		LogWarning("CPP function \'" + name + "\' does not exist.")

	return 0;
}

#endif
