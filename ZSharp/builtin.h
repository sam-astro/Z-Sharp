#ifndef BUILTIN_H
#define BUILTIN_H

#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <boost/any.hpp>
#include <SDL.h>
#include <ctime>
#include <math.h>
#include <sys/stat.h>
#include <cstdlib> // for console command printing

#include "strops.h"
#include "graphics.h"
#include "anyops.h"
#if WINDOWS
#include "color.hpp"
#endif

//#define DEVELOPER_MESSAGES false

using namespace std;
using namespace boost;

vector<string> types = { "int", "float", "string", "bool", "void", "null", "Sprite", "Vec2", "Text" };

//unordered_map<string, vector<vector<string>>> builtinFunctionValues;
//unordered_map<string, boost::any> builtinVarVals;

// Foreground colors
const std::string blackFGColor = "\x1B[30m";
const std::string redFGColor = "\x1B[31m";
const std::string greenFGColor = "\x1B[32m";
const std::string yellowFGColor = "\x1B[33m";
const std::string blueFGColor = "\x1B[34m";
const std::string magentaFGColor = "\x1B[35m";
const std::string cyanFGColor = "\x1B[36m";
const std::string whiteFGColor = "\x1B[37m";
const std::string brightBlackFGColor = "\x1B[90m";
const std::string brightRedFGColor = "\x1B[91m";
const std::string brightGreenFGColor = "\x1B[92m";
const std::string brightYellowFGColor = "\x1B[93m";
const std::string brightBlueFGColor = "\x1B[94m";
const std::string brightMagentaFGColor = "\x1B[95m";
const std::string brightCyanFGColor = "\x1B[96m";
const std::string brightWhiteFGColor = "\x1B[97m";
//Background colors
const std::string blackBGColor = "\x1B[40m";
const std::string redBGColor = "\x1B[41m";
const std::string greenBGColor = "\x1B[42m";
const std::string yellowBGColor = "\x1B[43m";
const std::string blueBGColor = "\x1B[44m";
const std::string magentaBGColor = "\x1B[45m";
const std::string cyanBGColor = "\x1B[46m";
const std::string whiteBGColor = "\x1B[47m";
const std::string brightBlackBGColor = "\x1B[100m";
const std::string brightRedBGColor = "\x1B[101m";
const std::string brightGreenBGColor = "\x1B[102m";
const std::string brightYellowBGColor = "\x1B[103m";
const std::string brightBlueBGColor = "\x1B[104m";
const std::string brightMagentaBGColor = "\x1B[105m";
const std::string brightCyanBGColor = "\x1B[106m";
const std::string brightWhiteBGColor = "\x1B[107m";
// Reset color
const std::string resetColor = "\033[0m";

class NullType {
public:
	string type = "NULL";
};
class BREAK {
public:
	string type = "BREAK";
};

boost::any nullType;
boost::any breakReOp;

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

inline bool fileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void PrintColored(std::string text, std::string fgColor, std::string bgColor, bool isError)
{
#if WINDOWS
	if (fgColor == blackFGColor)
		cerr << hue::black;
	else if (fgColor == redFGColor)
		cerr << hue::red;
	else if (fgColor == greenFGColor)
		cerr << hue::green;
	else if (fgColor == yellowFGColor)
		cerr << hue::yellow;
	else if (fgColor == blueFGColor)
		cerr << hue::blue;
	else if (fgColor == magentaFGColor)
		cerr << hue::purple;
	else if (fgColor == cyanFGColor)
		cerr << hue::aqua;
	else if (fgColor == whiteFGColor)
		cerr << hue::white;

	if (bgColor == blackBGColor)
		cerr << hue::on_black;
	else if (bgColor == redBGColor)
		cerr << hue::on_red;
	else if (bgColor == greenBGColor)
		cerr << hue::on_green;
	else if (bgColor == yellowBGColor)
		cerr << hue::on_yellow;
	else if (bgColor == blueBGColor)
		cerr << hue::on_blue;
	else if (bgColor == magentaBGColor)
		cerr << hue::on_purple;
	else if (bgColor == cyanBGColor)
		cerr << hue::on_aqua;
	else if (bgColor == whiteBGColor)
		cerr << hue::on_white;

	std::cerr << text << hue::reset;
#else
	if (!isError)
		cout << fgColor + bgColor + text + resetColor;
	else
		cerr << fgColor + bgColor + text + resetColor;
#endif
}

int LogWarning(const string& warningText)
{
	PrintColored("WARNING: ", yellowFGColor, "", true);
	PrintColored(escaped(warningText), yellowFGColor, "", true);
	cerr << std::endl;
	//cout << "\x1B[33mWARNING: " << warningText << "\033[0m\t\t" << endl;
	return 1;
}

int InterpreterLog(const string& logText)
{
	int Hour = 0;
	int Min = 0;
	int Sec = 0;

	time_t timer = time(0);

	tm bt{};
#if UNIX
	time_t currentTime;
	struct tm* localTime;

	time(&currentTime);                   // Get the current time
	localTime = localtime(&currentTime);  // Convert the current time to the local time

	Hour = localTime->tm_hour;
	Min = localTime->tm_min;
	Sec = localTime->tm_sec;
#elif WINDOWS
	localtime_s(&bt, &timer);
	Hour = bt.tm_hour;
	Min = bt.tm_min;
	Sec = bt.tm_sec;
#else
	static mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *localtime(&timer);
#endif

	PrintColored("[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + "] ", blueFGColor, "", true);
	PrintColored("ZSharp: ", yellowFGColor, "", true);
	PrintColored(escaped(logText), greenFGColor, "", true);
	cout << std::endl;
	//cout << "\x1B[34m[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + "] \x1B[33mZSharp: \x1B[32m" << logText << "\033[0m\t\t" << endl;
	return 1;
}

int LogCriticalError(const string& errorText)
{
	int Hour = 0;
	int Min = 0;
	int Sec = 0;
	time_t timer = time(0);

	tm bt{};
#if UNIX
	time_t currentTime;
	struct tm* localTime;

	time(&currentTime);                   // Get the current time
	localTime = localtime(&currentTime);  // Convert the current time to the local time

	Hour = localTime->tm_hour;
	Min = localTime->tm_min;
	Sec = localTime->tm_sec;
#elif WINDOWS
	localtime_s(&bt, &timer);
	Hour = bt.tm_hour;
	Min = bt.tm_min;
	Sec = bt.tm_sec;
#else
	static mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *localtime(&timer);
#endif

	PrintColored("[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + "] ", blueFGColor, "", true);
	PrintColored("ZSharp: ", yellowFGColor, "", true);
	PrintColored(escaped(errorText), redFGColor, "", true);
	cerr << std::endl;
	InterpreterLog("Press Enter to Exit...");
	cin.ignore();
	exit(1);
	//cerr << "\x1B[34m[" + to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec) + "] \x1B[33mZSharp: \x1B[31mERROR: " << errorText << "\033[0m\t\t" << endl;
	//exit(EXIT_FAILURE);
	return 2;
}

boost::any GetClassSubComponent(boost::any value, string subComponentName)
{
	// If a Sprite Class
	if (any_type(value) == 4)
	{
		return any_cast<Sprite>(value).SubComponent(subComponentName);
	}
	// If a Vec2 Class
	if (any_type(value) == 5)
	{
		return any_cast<Vec2>(value).SubComponent(subComponentName);
	}
	// If a Text Class
	if (any_type(value) == 6)
	{
		return any_cast<Text>(value).SubComponent(subComponentName);
	}
	return nullType;
}

boost::any EditClassSubComponent(boost::any value, string oper, boost::any otherVal, string subComponentName)
{
	// If a Sprite Class
	if (any_type(value) == 4)
	{
		Sprite v = any_cast<Sprite>(value);
		v.EditSubComponent(subComponentName, oper, otherVal);
		return v;
	}
	// If a Vec2 Class
	if (any_type(value) == 5)
	{
		Vec2 v = any_cast<Vec2>(value);
		v.EditSubComponent(subComponentName, oper, otherVal);
		return v;
	}
	// If a Text Class
	if (any_type(value) == 6)
	{
		Text t = any_cast<Text>(value);
		t.EditSubComponent(subComponentName, oper, otherVal);
		return t;
	}
	return nullType;
}

bool AxisAlignedCollision(const Sprite& a, const Sprite& b) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = a.position.x + a.scale.x / 2 >= b.position.x - b.scale.x / 2 &&
		b.position.x + b.scale.x / 2 >= a.position.x - a.scale.x;
	// collision y-axis?
	bool collisionY = a.position.y + a.scale.y / 2 >= b.position.y - b.scale.y / 2 &&
		b.position.y + b.scale.y / 2 >= a.position.y - a.scale.y / 2;

	//// collision x-axis?
	//bool collisionX = a.position.x - a.scale.x / 2 >= b.position.x + b.scale.x / 2 ||
	//	a.position.x + a.scale.x / 2 <= b.position.x - b.scale.x / 2;
	//// collision y-axis?
	//bool collisionY = a.position.y - a.scale.y / 2 >= b.position.y + b.scale.y / 2 ||
	//	a.position.y + a.scale.y / 2 <= b.position.y - b.scale.y / 2;

	// collision only if on both axes
	return collisionX && collisionY;
}

//// Initial script processing, which loads variables and functions from builtin
//int GetBuiltins(std::string s)
//{
//	std::string script = replace(s, "    ", "\t");
//
//	vector<string> lines = split(script, '\n');
//	vector<vector<string>> words;
//	for (int i = 0; i < (int)lines.size(); i++)
//	{
//		words.push_back(split(lines.at(i), ' '));
//	}
//
//	// Go through entire script and iterate through all types to see if line is a
//	// function declaration, then store it with it's value
//	for (int lineNum = 0; lineNum < (int)words.size(); lineNum++)
//	{
//		//Checks if it is function
//		if (words.at(lineNum).at(0) == "func")
//		{
//			vector<vector<string>> functionContents;
//
//			string functName = split(words.at(lineNum).at(1), '(')[0];
//
//#if DEVELOPER_MESSAGES == true
//			InterpreterLog("Load builtin function " + functName + "...");
//#endif
//
//			string args = "";
//			for (int w = 1; w < (int)words.at(lineNum).size(); w++) // Get all words from the instantiation line: these are the args
//			{
//				args += replace(replace(words.at(lineNum).at(w), "(", " "), ")", "");
//			}
//
//			args = replace(args, functName + " ", "");
//			functionContents.push_back(split(args, ','));
//
//			int numOfBrackets = 1;
//			for (int p = lineNum + 2; p < (int)words.size(); p++)
//			{
//				numOfBrackets += countInVector(words.at(p), "{") - countInVector(words.at(p), "}");
//				if (numOfBrackets == 0)
//					break;
//				functionContents.push_back(removeTabs(words.at(p), 1));
//			}
//			builtinFunctionValues[functName] = functionContents;
//			//cout << functName << " is \n" << Vec2Str(functionContents) << endl << endl;
//		}
//		else
//		{
//			if (words.at(lineNum).at(0) == "string")
//			{
//				builtinVarVals[words.at(lineNum).at(1)] = StringRaw(words.at(lineNum).at(3));
//#if DEVELOPER_MESSAGES == true
//				InterpreterLog("Load builtin variable " + words.at(lineNum).at(1) + "...");
//#endif
//			}
//			else if (words.at(lineNum).at(0) == "int")
//			{
//				builtinVarVals[words.at(lineNum).at(1)] = stoi(words.at(lineNum).at(3));
//#if DEVELOPER_MESSAGES == true
//				InterpreterLog("Load builtin variable " + words.at(lineNum).at(1) + "...");
//#endif
//			}
//			else if (words.at(lineNum).at(0) == "float")
//			{
//				builtinVarVals[words.at(lineNum).at(1)] = stof(words.at(lineNum).at(3));
//#if DEVELOPER_MESSAGES == true
//				InterpreterLog("Load builtin variable " + words.at(lineNum).at(1) + "...");
//#endif
//			}
//			else if (words.at(lineNum).at(0) == "bool")
//			{
//				builtinVarVals[words.at(lineNum).at(1)] = stob(words.at(lineNum).at(3));
//#if DEVELOPER_MESSAGES == true
//				InterpreterLog("Load builtin variable " + words.at(lineNum).at(1) + "...");
//#endif
//			}
//			//else
//			//	LogWarning("unrecognized type \'" + words[lineNum][0] + "\' on line: " + to_string(lineNum));
//		}
//	}
//
//	return 0;
//}

// Executes 
boost::any ZSFunction(const string& name, const vector<boost::any>& args)
{
	if (name == "ZS.Math.Sin")
		return sin(AnyAsFloat(args.at(0)));
	else if (name == "ZS.Math.Cos")
		return cos(AnyAsFloat(args.at(0)));
	else if (name == "ZS.Math.Tan")
		return tan(AnyAsFloat(args.at(0)));
	else if (name == "ZS.Math.Round")
		return AnyAsInt(args.at(0));
	else if (name == "ZS.Math.Lerp")
		return lerp(AnyAsFloat(args.at(0)), AnyAsFloat(args.at(1)), AnyAsFloat(args.at(2)));
	else if (name == "ZS.Math.Abs")
		return abs(AnyAsFloat(args.at(0)));
	else if (name == "ZS.Graphics.Init")
	{
#if DEVELOPER_MESSAGES == true
		InterpreterLog("Init graphics");
#endif
		if (args.size() <= 3)
			initGraphics(StringRaw(AnyAsString(args.at(0))), AnyAsInt(args.at(1)), AnyAsInt(args.at(2)), 1);
		else
			initGraphics(StringRaw(AnyAsString(args.at(0))), AnyAsInt(args.at(1)), AnyAsInt(args.at(2)), AnyAsInt(args.at(3)));
	}
	else if (name == "ZS.Graphics.Sprite")
	{
		string path = StringRaw(AnyAsString(args.at(0)));
		if (count(path, '/') == 0)
			path = "./" + path;
		if (!fileExists(path))
			LogCriticalError("Failed to create 'Sprite' object: \"" + path + "\"");

		Sprite s(StringRaw(AnyAsString(args.at(0))), any_cast<Vec2>(args.at(1)), any_cast<Vec2>(args.at(2)), AnyAsFloat(args.at(3)));
		return s;
	}
	else if (name == "ZS.Graphics.DrawPixel")
	{
		SDL_SetRenderDrawColor(gRenderer, AnyAsInt(args.at(2)), AnyAsInt(args.at(3)), AnyAsInt(args.at(4)), 255);
		SDL_RenderDrawPoint(gRenderer, AnyAsInt(args.at(0)), AnyAsInt(args.at(1)));
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	}
		//DrawPixel(AnyAsInt(args.at(0)), AnyAsInt(args.at(1)), AnyAsInt(args.at(2)), AnyAsInt(args.at(3)), AnyAsInt(args.at(4)));
	else if (name == "ZS.Graphics.Draw")
		any_cast<Sprite>(args.at(0)).Draw();
	else if (name == "ZS.Graphics.Load")
		any_cast<Sprite>(args.at(0)).Load();
	else if (name == "ZS.Graphics.Text")
	{
		string path = StringRaw(AnyAsString(args.at(1)));
		if (count(path, '/') == 0)
			path = "./" + path;
		if (!fileExists(path))
			LogCriticalError("Failed to create 'Text' object: \"" + path + "\"");

		if (args.size() <= 8)
		{
			Text t(StringRaw(AnyAsString(args.at(0))), path, any_cast<Vec2>(args.at(2)), AnyAsFloat(args.at(3)), AnyAsFloat(args.at(4)), (Uint8)AnyAsFloat(args.at(5)), (Uint8)AnyAsFloat(args.at(6)), (Uint8)AnyAsFloat(args.at(7)), true);
			return t;
		}
		else
		{
			Text t(StringRaw(AnyAsString(args.at(0))), path, any_cast<Vec2>(args.at(2)), AnyAsFloat(args.at(3)), AnyAsFloat(args.at(4)), (Uint8)AnyAsFloat(args.at(5)), (Uint8)AnyAsFloat(args.at(6)), (Uint8)AnyAsFloat(args.at(7)), AnyAsBool(args.at(8)));
			return t;
		}
	}
	else if (name == "ZS.Graphics.DrawText")
		any_cast<Text>(args.at(0)).Draw();
	else if (name == "ZS.Graphics.LoadText")
		any_cast<Text>(args.at(0)).Load();
	else if (name == "ZS.Physics.AxisAlignedCollision")
	{
		return AxisAlignedCollision(any_cast<Sprite>(args.at(0)), any_cast<Sprite>(args.at(1)));
	}
	else if (name == "ZS.Input.GetKey")
		return KEYS[StringRaw(any_cast<string>(args.at(0)))] == 1;
	else if (name == "ZS.System.Print")
		cout << StringRaw(AnyAsString(args.at(0)));
	else if (name == "ZS.System.PrintLine")
		cout << StringRaw(AnyAsString(args.at(0))) << endl;
	else if (name == "ZS.System.Vec2")
	{
		Vec2 v(AnyAsFloat(args.at(0)), AnyAsFloat(args.at(1)));
		return v;
	}
	else if (name == "ZS.System.Command"){
		string command = StringRaw(AnyAsString(args.at(0)));
		//int command_len = command.length();
		//char* command_char_arr=new char[command_len + 1];
		//strcpy(command_char_arr, command.c_str()); // string into char arr
		int k = system(command.c_str());
	}
	else
		LogWarning("ZS function \'" + name + "\' does not exist.");

	return nullType;
}

#endif
