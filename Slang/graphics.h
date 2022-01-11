
#ifndef GRAPHICS_H
#define GRAPHICS_H

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
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

using namespace std;

class Parser
{
public:
	int Start(int SCREEN_W, int SCREEN_H)
	{
		

		////Initialize SDL
		//if (SDL_Init(SDL_INIT_VIDEO) < 0)
		//{
		//	printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		//}
		//else
		//{
		//	//Create window
		//	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
		//	if (window == NULL)
		//	{
		//		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		//	}
		//	else
		//	{
		//		//Get window surface
		//		screenSurface = SDL_GetWindowSurface(window);

		//		while (OnUpdate() == 0)
		//		{
		//			//Fill the surface white
		//			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00));
		//			
		//			//Update the surface
		//			SDL_UpdateWindowSurface(window);
		//		}
		//	}
		//}

		////Destroy window
		//SDL_DestroyWindow(window);

		//Quit SDL subsystems
		//SDL_Quit();

		return 0;
	}

	int OnUpdate()
	{
		ExecuteFunction("Update", vector<boost::any>());
		//cout  << "update" << endl;
		return 0;
	}
};

#endif