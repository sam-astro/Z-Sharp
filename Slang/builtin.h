#ifndef BUILTIN_H
#define BUILTIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include "strops.h"
#include "graphics.h"

using namespace std;

vector<string> types = { "int", "float", "string", "bool", "void", "null" };

vector<string> builtinFunctions;
vector<vector<string>> builtinFunctionValues;
vector<string> builtinVars;
vector<string> builtinVarVals;

Parser mainWindow;

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
					builtinFunctions.push_back(functName);
					builtinFunctionValues.push_back(functionContents);
				}
				//Checks if it is variable
				else
				{
					builtinVars.push_back(words[lineNum][0] + " " + words[lineNum][1]);
					builtinVarVals.push_back((string)words[lineNum][3]);
					//cout << words[lineNum][1] << " is " << words[lineNum][3] << endl;
				}
			}

	return 0;
}

string CPPFunction(string name, vector<string> args)
{
	if (name == "CPP.Math.Sin")
		return to_string(sin(stof(args[0])));
	else if (name == "CPP.Math.Cos")
		return to_string(cos(stof(args[0])));
	else if (name == "CPP.Math.Tan")
		return to_string(tan(stof(args[0])));
	else if (name == "CPP.Math.Round")
		return to_string(round(stof(args[0])));
	else if (name == "CPP.Graphics.Init")
	{
		cout << "\x1B[32mInit graphics\033[0m\t\t" << endl;
		if (mainWindow.Construct(stoi(args[0]), stoi(args[1]), stoi(args[2]), stoi(args[2])))
			mainWindow.Start();
	}
	else if (name == "CPP.Graphics.SetPixel")
		mainWindow.Draw(stoi(args[0]), stoi(args[1]), olc::Pixel(stoi(args[2]), stoi(args[3]), stoi(args[4])));

	return "";
}

#endif