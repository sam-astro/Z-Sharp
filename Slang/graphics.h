
#ifndef GRAPHICS_H
#define GRAPHICS_H

#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <algorithm>
#include <boost/any.hpp>
#include "strops.h"
#include "builtin.h"
#include "main.h"

using namespace std;

class Parser : public olc::PixelGameEngine
{
public:
	Parser()
	{
		sAppName = "Parser";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		ExecuteFunction("Update", vector<boost::any> {});
		 
		// Called once per frame
		//for (int x = 0; x < ScreenWidth(); x++)
		//	for (int y = 0; y < ScreenHeight(); y++)
		//		Draw(x, y, olc::Pixel(rand() % 128, rand() % 128, rand() % 128));
		return true;
	}
};

#endif