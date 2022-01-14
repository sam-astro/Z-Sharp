#ifndef BUILTIN_H
#define BUILTIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include "strops.h"
#include "graphics.h"
#include "anyops.h"
#include <boost/any.hpp>
#include <SDL.h>
#include <ctime>

using namespace std;
using namespace boost;

vector<string> types = { "int", "float", "string", "bool", "void", "null", "Sprite", "Vec2" };

unordered_map<string, vector<vector<string>>> builtinFunctionValues;
unordered_map<string, boost::any> builtinVarVals;

class NullType {
public:
	string type = "NULL";
};

boost::any nullType;

float clamp(float v, float min, float max)
{
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

int LogWarning(const string& warningText)
{
	cerr << "\x1B[33mWARNING: " << warningText << "\033[0m\t\t" << endl;
	return 1;
}

int InterpreterLog(const string& logText)
{
	time_t timer = time(0);

	tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *localtime(&timer);
#endif

	int Hour = bt.tm_hour;
	int Min = bt.tm_min;
	int Sec = bt.tm_sec;

	cout << "\x1B[34m[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + "] \x1B[33mSlang: \x1B[32m" << logText << "\033[0m\t\t" << endl;
	return 1;
}

int LogCriticalError(const string& errorText)
{
	time_t timer = time(0);

	tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *localtime(&timer);
#endif

	int Hour = bt.tm_hour;
	int Min = bt.tm_min;
	int Sec = bt.tm_sec;

	cerr << "\x1B[34m[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + "] \x1B[33mSlang: \x1B[31mERROR: " << errorText << "\033[0m\t\t" << endl;
	exit(EXIT_FAILURE);
	return 2;
}

boost::any GetClassSubComponent(boost::any value, string subComponentName)
{
	// If a Vec2 Class
	if (any_type(value) == 5)
	{
		return any_cast<Vec2>(value).SubComponent(subComponentName);
	}
	// If a Sprite Class
	if (any_type(value) == 4)
	{
		return any_cast<Sprite>(value).SubComponent(subComponentName);
	}
	return nullType;
}

boost::any EditClassSubComponent(boost::any value, string oper, boost::any otherVal, string subComponentName)
{
	// If a Vec2 Class
	if (any_type(value) == 5)
	{
		Vec2 v = any_cast<Vec2>(value);
		v.EditSubComponent(subComponentName, oper, otherVal);
		return v;
	}
	// If a Sprite Class
	if (any_type(value) == 4)
	{
		Sprite v = any_cast<Sprite>(value);
		v.EditSubComponent(subComponentName, oper, otherVal);
		return v;
	}
	return nullType;
}

bool AxisAlignedCollision(const Sprite& a, const Sprite& b) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = a.position.x + a.scale.x >= b.position.x &&
        b.position.x + b.scale.x >= a.position.x;
    // collision y-axis?
    bool collisionY = a.position.y + a.scale.y >= b.position.y &&
        b.position.y + b.scale.y >= b.position.y;
    // collision only if on both axes
    return collisionX && collisionY;
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

			InterpreterLog("Load builtin function " + functName);

			string args = "";
			for (int w = 1; w < (int)words[lineNum].size(); w++) // Get all words from the instantiation line: these are the args
			{
				args += replace(replace(words[lineNum][w], "(", " "), ")", "");
			}

			args = replace(args, functName + " ", "");
			functionContents.push_back(split(args, ','));

			int numOfBrackets = 1;
			for (int p = lineNum + 2; p < (int)words.size(); p++)
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
			{
				builtinVarVals[words[lineNum][1]] = StringRaw(words[lineNum][3]);
				InterpreterLog("Load builtin variable " + words[lineNum][1]);
			}
			else if (words[lineNum][0] == "int")
			{
				builtinVarVals[words[lineNum][1]] = stoi(words[lineNum][3]);
				InterpreterLog("Load builtin variable " + words[lineNum][1]);
			}
			else if (words[lineNum][0] == "float")
			{
				builtinVarVals[words[lineNum][1]] = stof(words[lineNum][3]);
				InterpreterLog("Load builtin variable " + words[lineNum][1]);
			}
			else if (words[lineNum][0] == "bool")
			{
				builtinVarVals[words[lineNum][1]] = stob(words[lineNum][3]);
				InterpreterLog("Load builtin variable " + words[lineNum][1]);
			}
			//else
			//	LogWarning("unrecognized type \'" + words[lineNum][0] + "\' on line: " + to_string(lineNum));
		}
	}

	return 0;
}

// Executes 
boost::any CPPFunction(const string& name, const vector<boost::any>& args)
{
	if (name == "CPP.Math.Sin")
		return sin(AnyAsFloat(args[0]));
	else if (name == "CPP.Math.Cos")
		return cos(AnyAsFloat(args[0]));
	else if (name == "CPP.Math.Tan")
		return tan(AnyAsFloat(args[0]));
	else if (name == "CPP.Math.Round")
		return AnyAsInt(args[0]);
	else if (name == "CPP.Math.Lerp")
		return lerp(AnyAsFloat(args[0]), AnyAsFloat(args[1]), AnyAsFloat(args[2]));
	else if (name == "CPP.Math.Abs")
		return abs(AnyAsFloat(args[0]));
	else if (name == "CPP.Graphics.Init")
	{
		InterpreterLog("Init graphics");
		initGraphics(StringRaw(AnyAsString(args[0])), AnyAsInt(args[1]), AnyAsInt(args[2]));
	}
	else if (name == "CPP.Graphics.Sprite")
	{
		Sprite s(StringRaw(AnyAsString(args[0])), any_cast<Vec2>(args[1]), any_cast<Vec2>(args[2]), AnyAsFloat(args[3]));
		return s;
	}
	else if (name == "CPP.Graphics.Draw")
		any_cast<Sprite>(args[0]).Draw();
	else if (name == "CPP.Physics.AxisAlignedCollision")
	{
		return AxisAlignedCollision(any_cast<Sprite>(args[0]), any_cast<Sprite>(args[1]));
	}
	else if (name == "CPP.Input.GetKey")
		return KEYS[StringRaw(any_cast<string>(args[0]))] == 1;
	else if (name == "CPP.System.Print")
		cout << StringRaw(AnyAsString(args[0]));
	else if (name == "CPP.System.PrintLine")
		cout << StringRaw(AnyAsString(args[0])) << endl;
	else if (name == "CPP.System.Vec2")
	{
		Vec2 v(AnyAsFloat(args[0]), AnyAsFloat(args[1]));
		return v;
	}
	else
		LogWarning("CPP function \'" + name + "\' does not exist.");

	return nullType;
}

#endif
