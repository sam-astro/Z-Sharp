
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
#include "main.h"
#include <SDL.h>
#include <SDL_image.h> 
#include <stdio.h>
#include <chrono>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <any>

using namespace std;
using namespace boost;

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;
int PIXEL_SCALE = 1;

unordered_map<string, int> KEYS =
{
	{"ESCAPE", 0},
	{"1", 0},
	{"2", 0},
	{"3", 0},
	{"4", 0},
	{"5", 0},
	{"6", 0},
	{"7", 0},
	{"8", 0},
	{"9", 0},
	{"TILDE", 0},
	{"MINUS", 0},
	{"EQUALS", 0},
	{"BACKSPACE", 0},
	{"TAB", 0},
	{"Q", 0},
	{"W", 0},
	{"E", 0},
	{"R", 0},
	{"T", 0},
	{"Y", 0},
	{"U", 0},
	{"I", 0},
	{"O", 0},
	{"P", 0},
	{"BRACKET_L", 0},
	{"BRACKET_R", 0},
	{"BACKSLASH", 0},
	{"A", 0},
	{"S", 0},
	{"D", 0},
	{"F", 0},
	{"G", 0},
	{"H", 0},
	{"J", 0},
	{"K", 0},
	{"L", 0},
	{"COLON", 0},
	{"QUOTE", 0},
	{"ENTER", 0},
	{"SHIFT_L", 0},
	{"Z", 0},
	{"X", 0},
	{"C", 0},
	{"V", 0},
	{"B", 0},
	{"N", 0},
	{"M", 0},
	{"COMMA", 0},
	{"PERIOD", 0},
	{"SLASH", 0},
	{"SHIFT_R", 0},
	{"CTRL_L", 0},
	{"ALT_L", 0},
	{"SPACE", 0},
	{"ALT_R", 0},
	{"CTRL_R", 0},
	{"LEFT", 0},
	{"UP", 0},
	{"RIGHT", 0},
	{"DOWN", 0}
};

enum Buttons
{
	PaddleOneUp = 0,
	PaddleOneDown,
	PaddleTwoUp,
	PaddleTwoDown,
};

const float PADDLE_SPEED = 1.0f;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The renderer we'll be rendering to
SDL_Renderer* gRenderer = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

bool running = true;
float dt = 0.0f;

float clamp(float v, float min, float max);


SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

class Vec2
{
public:
	Vec2()
		: x(0.0f), y(0.0f)
	{}

	Vec2(float x, float y)
		: x(x), y(y)
	{}

	Vec2 operator+(Vec2 const& rhs)
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2& operator+=(Vec2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	Vec2 operator-(Vec2 const& rhs)
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2& operator-=(Vec2 const& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;

		return *this;
	}

	Vec2 operator*(float rhs)
	{
		return Vec2(x * rhs, y * rhs);
	}

	Vec2 operator*=(float rhs)
	{
		x *= rhs;
		y *= rhs;

		return *this;
	}

	Vec2 operator/(float rhs)
	{
		return Vec2(x / rhs, y / rhs);
	}

	Vec2 operator/=(float rhs)
	{
		x /= rhs;
		y /= rhs;

		return *this;
	}

	bool operator==(Vec2 other)
	{
		return x == other.x && y == other.y;
	}

	boost::any SubComponent(std::string componentName)
	{
		if (componentName == "x")
			return x;
		if (componentName == "y")
			return y;
		return 0;
	}

	Vec2 EditSubComponent(std::string componentName, std::string oper, boost::any otherVal)
	{
		if (componentName == "x")
		{
			if (oper == "=")
				x = AnyAsFloat(otherVal);
			else if (oper == "+=")
				x += AnyAsFloat(otherVal);
			else if (oper == "-=")
				x -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				x *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				x /= AnyAsFloat(otherVal);
		}
		if (componentName == "y")
		{
			if (oper == "=")
				y = AnyAsFloat(otherVal);
			else if (oper == "+=")
				y += AnyAsFloat(otherVal);
			else if (oper == "-=")
				y -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				y *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				y /= AnyAsFloat(otherVal);
		}
		return *this;
	}

	float x, y;
};

struct _RotRect {
	Vec2 C;
	Vec2 S;
	float ang;
};

inline void RotateVec2Clockwise(Vec2* v, float ang)
{
	float t,
		cosa = cos(ang),
		sina = sin(ang);
	t = v->x; v->x = t * cosa + v->y * sina; v->y = -t * sina + v->y * cosa;
}

// Rotated Rectangles Collision Detection, Oren Becker, 2001
int RotRectsCollision(_RotRect* rr1, _RotRect* rr2)
{
	Vec2 A, B,   // vertices of the rotated rr2
		C,      // center of rr2
		BL, TR; // vertices of rr2 (bottom-left, top-right)

	float ang = rr1->ang - rr2->ang, // orientation of rotated rr1
		cosa = cos(ang),           // precalculated trigonometic -
		sina = sin(ang);           // - values for repeated use

	float t, x, a;      // temporary variables for various uses
	float dx;           // deltaX for linear equations
	float ext1, ext2;   // min/max vertical values

	// move rr2 to make rr1 cannonic
	C = rr2->C;
	C -= rr1->C;

	// rotate rr2 clockwise by rr2->ang to make rr2 axis-aligned
	RotateVec2Clockwise(&C, rr2->ang);

	// calculate vertices of (moved and axis-aligned := 'ma') rr2
	BL = TR = C;
	BL -= rr2->S;
	TR += rr2->S;

	// calculate vertices of (rotated := 'r') rr1
	A.x = -rr1->S.y * sina; B.x = A.x; t = rr1->S.x * cosa; A.x += t; B.x -= t;
	A.y = rr1->S.y * cosa; B.y = A.y; t = rr1->S.x * sina; A.y += t; B.y -= t;

	t = sina * cosa;

	// verify that A is vertical min/max, B is horizontal min/max
	if (t < 0)
	{
		t = A.x; A.x = B.x; B.x = t;
		t = A.y; A.y = B.y; B.y = t;
	}

	// verify that B is horizontal minimum (leftest-vertex)
	if (sina < 0) { B.x = -B.x; B.y = -B.y; }

	// if rr2(ma) isn't in the horizontal range of
	// colliding with rr1(r), collision is impossible
	if (B.x > TR.x || B.x > -BL.x) return 0;

	// if rr1(r) is axis-aligned, vertical min/max are easy to get
	if (t == 0) { ext1 = A.y; ext2 = -ext1; }
	// else, find vertical min/max in the range [BL.x, TR.x]
	else
	{
		x = BL.x - A.x; a = TR.x - A.x;
		ext1 = A.y;
		// if the first vertical min/max isn't in (BL.x, TR.x), then
		// find the vertical min/max on BL.x or on TR.x
		if (a * x > 0)
		{
			dx = A.x;
			if (x < 0) { dx -= B.x; ext1 -= B.y; x = a; }
			else { dx += B.x; ext1 += B.y; }
			ext1 *= x; ext1 /= dx; ext1 += A.y;
		}

		x = BL.x + A.x; a = TR.x + A.x;
		ext2 = -A.y;
		// if the second vertical min/max isn't in (BL.x, TR.x), then
		// find the local vertical min/max on BL.x or on TR.x
		if (a * x > 0)
		{
			dx = -A.x;
			if (x < 0) { dx -= B.x; ext2 -= B.y; x = a; }
			else { dx += B.x; ext2 += B.y; }
			ext2 *= x; ext2 /= dx; ext2 -= A.y;
		}
	}

	// check whether rr2(ma) is in the vertical range of colliding with rr1(r)
	// (for the horizontal range of rr2)
	return !((ext1 < BL.y&& ext2 < BL.y) ||
		(ext1 > TR.y && ext2 > TR.y));
}

class Sprite
{
public:
	Sprite(std::string path, Vec2 position, Vec2 scale, double angle)
		: position(position), angle(angle), path(path), scale(scale)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = scale.x;
		rect.h = scale.y;
		// Centers
		rect.x = position.x - (rect.w / 2);
		rect.y = position.y - (rect.h / 2);

		Load();
	}

	int Load()
	{
		SDL_Surface* surface = loadSurface(path);
		texture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);
		return 0;
	}

	int Update()
	{
		SDL_Surface* surface = loadSurface(path);
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);
		return 0;
	}

	int Draw()
	{
		SDL_RenderCopy(gRenderer, texture, NULL, &rect);
		// Centers
		rect.w = scale.x;
		rect.h = scale.y;
		rect.x = position.x - (rect.w / 2);
		rect.y = position.y - (rect.h / 2);
		return 0;
	}

	bool operator==(Sprite other)
	{
		return position == other.position && angle == other.angle && scale == other.scale && texture == other.texture;
	}

	boost::any SubComponent(std::string componentName)
	{
		if (componentName == "position")
			return position;
		if (componentName == "position.x")
			return position.x;
		if (componentName == "position.y")
			return position.y;
		if (componentName == "scale")
			return scale;
		if (componentName == "scale.x")
			return scale.x;
		if (componentName == "scale.y")
			return scale.y;
		return 0;
	}

	Sprite EditSubComponent(std::string componentName, std::string oper, boost::any otherVal)
	{
		//cout << ("ComponentName = " + componentName + " Op = " + oper + " OtherVal = " + AnyAsString(otherVal)) << endl;
		if (componentName == "position")
		{
			if (oper == "=")
				position = any_cast<Vec2>(otherVal);
			else if (oper == "+=")
				position += any_cast<Vec2>(otherVal);
			else if (oper == "-=")
				position -= any_cast<Vec2>(otherVal);
			else if (oper == "*=")
				position *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				position /= AnyAsFloat(otherVal);
		}
		else if (componentName == "position.x")
		{
			if (oper == "=")
				position.x = AnyAsFloat(otherVal);
			else if (oper == "+=")
				position.x += AnyAsFloat(otherVal);
			else if (oper == "-=")
				position.x -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				position.x *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				position.x /= AnyAsFloat(otherVal);
		}
		else if (componentName == "position.y")
		{
			if (oper == "=")
				position.y = AnyAsFloat(otherVal);
			else if (oper == "+=")
				position.y += AnyAsFloat(otherVal);
			else if (oper == "-=")
				position.y -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				position.y *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				position.y /= AnyAsFloat(otherVal);
		}

		else if (componentName == "scale")
		{
			if (oper == "=")
				scale = any_cast<Vec2>(otherVal);
			else if (oper == "+=")
				scale += any_cast<Vec2>(otherVal);
			else if (oper == "-=")
				scale -= any_cast<Vec2>(otherVal);
			else if (oper == "*=")
				scale *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				scale /= AnyAsFloat(otherVal);
		}
		else if (componentName == "scale.x")
		{
			if (oper == "=")
				scale.x = AnyAsFloat(otherVal);
			else if (oper == "+=")
				scale.x += AnyAsFloat(otherVal);
			else if (oper == "-=")
				scale.x -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				scale.x *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				scale.x /= AnyAsFloat(otherVal);
		}
		else if (componentName == "scale.y")
		{
			if (oper == "=")
				scale.y = AnyAsFloat(otherVal);
			else if (oper == "+=")
				scale.y += AnyAsFloat(otherVal);
			else if (oper == "-=")
				scale.y -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				scale.y *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				scale.y /= AnyAsFloat(otherVal);
		}
		// Centers
		rect.w = scale.x;
		rect.h = scale.y;
		rect.x = position.x - (rect.w / 2);
		rect.y = position.y - (rect.h / 2);
		return *this;
	}

	Vec2 position;
	Vec2 scale;
	double angle;

	std::string path;
	SDL_Rect rect{};
	SDL_Texture* texture;
};

class Text
{
public:
	Text(std::string content, std::string pathToFont, Vec2 position, float fontSize, double angle, Uint8 r, Uint8 g, Uint8 b, bool antialias)
		: position(position), angle(angle), content(content), pathToFont(pathToFont), fontSize(fontSize), r(r), g(g), b(b), antialias(antialias)
	{
		rect.x = position.x;
		rect.y = position.y;

		font = TTF_OpenFont(pathToFont.c_str(), fontSize);

		Load();
	}

	int Load()
	{
		SDL_Color color = { r, g, b };

		SDL_Surface* surface;
		if (antialias)
			surface = TTF_RenderText_Blended(font, content.c_str(), color);
		else
			surface = TTF_RenderText_Solid(font, content.c_str(), color);

		texture = SDL_CreateTextureFromSurface(gRenderer, surface);

		TTF_SizeText(font, content.c_str(), &rect.w, &rect.h);

		scale.x = rect.w;
		scale.y = rect.h;

		// Centers
		rect.x = position.x - (rect.w / 2);
		rect.y = position.y - (rect.h / 2);

		SDL_FreeSurface(surface);
		return 0;
	}

	int Update()
	{
		SDL_Color color = { r, g, b };

		SDL_Surface* surface;
		if (antialias)
			surface = TTF_RenderText_Blended(font, content.c_str(), color);
		else
			surface = TTF_RenderText_Solid(font, content.c_str(), color);

		SDL_DestroyTexture(texture);
		texture = SDL_CreateTextureFromSurface(gRenderer, surface);

		TTF_SizeText(font, content.c_str(), &rect.w, &rect.h);

		scale.x = rect.w;
		scale.y = rect.h;

		// Centers
		rect.x = position.x - (rect.w / 2);
		rect.y = position.y - (rect.h / 2);

		SDL_FreeSurface(surface);
		return 0;
	}

	int Draw()
	{
		SDL_RenderCopy(gRenderer, texture, NULL, &rect);
		return 0;
	}

	boost::any SubComponent(std::string componentName)
	{
		//cerr << componentName << endl;
		if (componentName == "position")
			return position;
		if (componentName == "position.x")
			return position.x;
		if (componentName == "position.y")
			return position.y;
		if (componentName == "scale")
			return scale;
		if (componentName == "scale.x")
			return scale.x;
		if (componentName == "scale.y")
			return scale.y;
		if (componentName == "fontSize")
			return fontSize;
		if (componentName == "content")
			return content;
		if (componentName == "pathToFont")
			return pathToFont;
		if (componentName == "antialias")
			return antialias;
		return 0;
	}

	Text EditSubComponent(const std::string componentName, const std::string oper, const boost::any otherVal)
	{
		//cerr << componentName << " " << AnyAsString(otherVal) << endl;
		if (componentName == "position")
		{
			if (oper == "=")
				position = any_cast<Vec2>(otherVal);
			else if (oper == "+=")
				position += any_cast<Vec2>(otherVal);
			else if (oper == "-=")
				position -= any_cast<Vec2>(otherVal);
			else if (oper == "*=")
				position *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				position /= AnyAsFloat(otherVal);
		}
		else if (componentName == "position.x")
		{
			if (oper == "=")
				position.x = AnyAsFloat(otherVal);
			else if (oper == "+=")
				position.x += AnyAsFloat(otherVal);
			else if (oper == "-=")
				position.x -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				position.x *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				position.x /= AnyAsFloat(otherVal);
		}
		else if (componentName == "position.y")
		{
			if (oper == "=")
				position.y = AnyAsFloat(otherVal);
			else if (oper == "+=")
				position.y += AnyAsFloat(otherVal);
			else if (oper == "-=")
				position.y -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				position.y *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				position.y /= AnyAsFloat(otherVal);
		}

		else if (componentName == "fontSize")
		{
			if (oper == "=")
				fontSize = AnyAsFloat(otherVal);
			else if (oper == "+=")
				fontSize += AnyAsFloat(otherVal);
			else if (oper == "-=")
				fontSize -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				fontSize *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				fontSize /= AnyAsFloat(otherVal);
		}

		else if (componentName == "r")
		{
			if (oper == "=")
				r = AnyAsFloat(otherVal);
			else if (oper == "+=")
				r += AnyAsFloat(otherVal);
			else if (oper == "-=")
				r -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				r *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				r /= AnyAsFloat(otherVal);
		}
		else if (componentName == "g")
		{
			if (oper == "=")
				g = AnyAsFloat(otherVal);
			else if (oper == "+=")
				g += AnyAsFloat(otherVal);
			else if (oper == "-=")
				g -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				g *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				g /= AnyAsFloat(otherVal);
		}
		else if (componentName == "b")
		{
			if (oper == "=")
				b = AnyAsFloat(otherVal);
			else if (oper == "+=")
				b += AnyAsFloat(otherVal);
			else if (oper == "-=")
				b -= AnyAsFloat(otherVal);
			else if (oper == "*=")
				b *= AnyAsFloat(otherVal);
			else if (oper == "/=")
				b /= AnyAsFloat(otherVal);
		}

		else if (componentName == "content")
		{
			if (oper == "=")
				content = AnyAsString(otherVal);
			else if (oper == "+=")
				content += AnyAsString(otherVal);
		}
		else if (componentName == "antialias")
		{
			if (oper == "=")
				antialias = AnyAsBool(otherVal);
		}

		// Updates changes to text
		Update();
		return *this;
	}

	bool antialias = true;

	Vec2 position;
	Vec2 scale;
	float fontSize;
	double angle;
	Uint8 r;
	Uint8 g;
	Uint8 b;
	std::string content;
	std::string pathToFont;

	TTF_Font* font;

	std::string path;
	SDL_Rect rect{};
	SDL_Texture* texture;
};

int cleanupGraphics()
{
	// Cleanup
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}

//void DrawPixel(int x, int y, int r, int g, int b) {
//	SDL_SetRenderDrawColor(gRenderer, r, g, b, 255);
//	SDL_RenderDrawPoint(gRenderer, x, y);
//	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
//}


int updateLoop()
{
	// Continue looping and processing events until user exits
	while (running)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					KEYS["ESCAPE"] = 1;
				else if (event.key.keysym.sym == SDLK_0)
					KEYS["0"] = 1;
				else if (event.key.keysym.sym == SDLK_1)
					KEYS["1"] = 1;
				else if (event.key.keysym.sym == SDLK_2)
					KEYS["2"] = 1;
				else if (event.key.keysym.sym == SDLK_3)
					KEYS["3"] = 1;
				else if (event.key.keysym.sym == SDLK_4)
					KEYS["4"] = 1;
				else if (event.key.keysym.sym == SDLK_5)
					KEYS["5"] = 1;
				else if (event.key.keysym.sym == SDLK_6)
					KEYS["6"] = 1;
				else if (event.key.keysym.sym == SDLK_7)
					KEYS["7"] = 1;
				else if (event.key.keysym.sym == SDLK_8)
					KEYS["8"] = 1;
				else if (event.key.keysym.sym == SDLK_MINUS)
					KEYS["MINUS"] = 1;
				else if (event.key.keysym.sym == SDLK_EQUALS)
					KEYS["EQUALS"] = 1;
				else if (event.key.keysym.sym == SDLK_AC_BACK)
					KEYS["BACKSPACE"] = 1;
				else if (event.key.keysym.sym == SDLK_TAB)
					KEYS["TAB"] = 1;
				else if (event.key.keysym.sym == SDLK_q)
					KEYS["Q"] = 1;
				else if (event.key.keysym.sym == SDLK_w)
					KEYS["W"] = 1;
				else if (event.key.keysym.sym == SDLK_e)
					KEYS["E"] = 1;
				else if (event.key.keysym.sym == SDLK_r)
					KEYS["R"] = 1;
				else if (event.key.keysym.sym == SDLK_t)
					KEYS["T"] = 1;
				else if (event.key.keysym.sym == SDLK_y)
					KEYS["Y"] = 1;
				else if (event.key.keysym.sym == SDLK_u)
					KEYS["U"] = 1;
				else if (event.key.keysym.sym == SDLK_i)
					KEYS["I"] = 1;
				else if (event.key.keysym.sym == SDLK_o)
					KEYS["O"] = 1;
				else if (event.key.keysym.sym == SDLK_p)
					KEYS["P"] = 1;
				else if (event.key.keysym.sym == SDLK_LEFTBRACKET)
					KEYS["BRACKET_L"] = 1;
				else if (event.key.keysym.sym == SDLK_RIGHTBRACKET)
					KEYS["BRACKET_R"] = 1;
				else if (event.key.keysym.sym == SDLK_BACKSLASH)
					KEYS["BACKSLASH"] = 1;
				else if (event.key.keysym.sym == SDLK_a)
					KEYS["A"] = 1;
				else if (event.key.keysym.sym == SDLK_s)
					KEYS["S"] = 1;
				else if (event.key.keysym.sym == SDLK_d)
					KEYS["D"] = 1;
				else if (event.key.keysym.sym == SDLK_f)
					KEYS["F"] = 1;
				else if (event.key.keysym.sym == SDLK_g)
					KEYS["G"] = 1;
				else if (event.key.keysym.sym == SDLK_h)
					KEYS["H"] = 1;
				else if (event.key.keysym.sym == SDLK_j)
					KEYS["J"] = 1;
				else if (event.key.keysym.sym == SDLK_k)
					KEYS["K"] = 1;
				else if (event.key.keysym.sym == SDLK_l)
					KEYS["L"] = 1;
				else if (event.key.keysym.sym == SDLK_COLON)
					KEYS["COLON"] = 1;
				else if (event.key.keysym.sym == SDLK_QUOTE)
					KEYS["QUOTE"] = 1;
				else if (event.key.keysym.sym == SDLK_RETURN)
					KEYS["ENTER"] = 1;
				else if (event.key.keysym.sym == SDLK_LSHIFT)
					KEYS["SHIFT_L"] = 1;
				else if (event.key.keysym.sym == SDLK_z)
					KEYS["Z"] = 1;
				else if (event.key.keysym.sym == SDLK_x)
					KEYS["X"] = 1;
				else if (event.key.keysym.sym == SDLK_c)
					KEYS["C"] = 1;
				else if (event.key.keysym.sym == SDLK_v)
					KEYS["V"] = 1;
				else if (event.key.keysym.sym == SDLK_b)
					KEYS["B"] = 1;
				else if (event.key.keysym.sym == SDLK_n)
					KEYS["N"] = 1;
				else if (event.key.keysym.sym == SDLK_m)
					KEYS["M"] = 1;
				else if (event.key.keysym.sym == SDLK_COMMA)
					KEYS["COMMA"] = 1;
				else if (event.key.keysym.sym == SDLK_PERIOD)
					KEYS["PERIOD"] = 1;
				else if (event.key.keysym.sym == SDLK_SLASH)
					KEYS["SLASH"] = 1;
				else if (event.key.keysym.sym == SDLK_RSHIFT)
					KEYS["SHIFT_R"] = 1;
				else if (event.key.keysym.sym == SDLK_LCTRL)
					KEYS["CTRL_L"] = 1;
				else if (event.key.keysym.sym == SDLK_LALT)
					KEYS["ALT_L"] = 1;
				else if (event.key.keysym.sym == SDLK_SPACE)
					KEYS["SPACE"] = 1;
				else if (event.key.keysym.sym == SDLK_RALT)
					KEYS["ALT_R"] = 1;
				else if (event.key.keysym.sym == SDLK_RCTRL)
					KEYS["CTRL_R"] = 1;
				else if (event.key.keysym.sym == SDLK_LEFT)
					KEYS["LEFT"] = 1;
				else if (event.key.keysym.sym == SDLK_UP)
					KEYS["UP"] = 1;
				else if (event.key.keysym.sym == SDLK_RIGHT)
					KEYS["RIGHT"] = 1;
				else if (event.key.keysym.sym == SDLK_DOWN)
					KEYS["DOWN"] = 1;
			}
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					KEYS["ESCAPE"] = 0;
				else if (event.key.keysym.sym == SDLK_0)
					KEYS["0"] = 0;
				else if (event.key.keysym.sym == SDLK_1)
					KEYS["1"] = 0;
				else if (event.key.keysym.sym == SDLK_2)
					KEYS["2"] = 0;
				else if (event.key.keysym.sym == SDLK_3)
					KEYS["3"] = 0;
				else if (event.key.keysym.sym == SDLK_4)
					KEYS["4"] = 0;
				else if (event.key.keysym.sym == SDLK_5)
					KEYS["5"] = 0;
				else if (event.key.keysym.sym == SDLK_6)
					KEYS["6"] = 0;
				else if (event.key.keysym.sym == SDLK_7)
					KEYS["7"] = 0;
				else if (event.key.keysym.sym == SDLK_8)
					KEYS["8"] = 0;
				else if (event.key.keysym.sym == SDLK_MINUS)
					KEYS["MINUS"] = 0;
				else if (event.key.keysym.sym == SDLK_EQUALS)
					KEYS["EQUALS"] = 0;
				else if (event.key.keysym.sym == SDLK_AC_BACK)
					KEYS["BACKSPACE"] = 0;
				else if (event.key.keysym.sym == SDLK_TAB)
					KEYS["TAB"] = 0;
				else if (event.key.keysym.sym == SDLK_q)
					KEYS["Q"] = 0;
				else if (event.key.keysym.sym == SDLK_w)
					KEYS["W"] = 0;
				else if (event.key.keysym.sym == SDLK_e)
					KEYS["E"] = 0;
				else if (event.key.keysym.sym == SDLK_r)
					KEYS["R"] = 0;
				else if (event.key.keysym.sym == SDLK_t)
					KEYS["T"] = 0;
				else if (event.key.keysym.sym == SDLK_y)
					KEYS["Y"] = 0;
				else if (event.key.keysym.sym == SDLK_u)
					KEYS["U"] = 0;
				else if (event.key.keysym.sym == SDLK_i)
					KEYS["I"] = 0;
				else if (event.key.keysym.sym == SDLK_o)
					KEYS["O"] = 0;
				else if (event.key.keysym.sym == SDLK_p)
					KEYS["P"] = 0;
				else if (event.key.keysym.sym == SDLK_LEFTBRACKET)
					KEYS["BRACKET_L"] = 0;
				else if (event.key.keysym.sym == SDLK_RIGHTBRACKET)
					KEYS["BRACKET_R"] = 0;
				else if (event.key.keysym.sym == SDLK_BACKSLASH)
					KEYS["BACKSLASH"] = 0;
				else if (event.key.keysym.sym == SDLK_a)
					KEYS["A"] = 0;
				else if (event.key.keysym.sym == SDLK_s)
					KEYS["S"] = 0;
				else if (event.key.keysym.sym == SDLK_d)
					KEYS["D"] = 0;
				else if (event.key.keysym.sym == SDLK_f)
					KEYS["F"] = 0;
				else if (event.key.keysym.sym == SDLK_g)
					KEYS["G"] = 0;
				else if (event.key.keysym.sym == SDLK_h)
					KEYS["H"] = 0;
				else if (event.key.keysym.sym == SDLK_j)
					KEYS["J"] = 0;
				else if (event.key.keysym.sym == SDLK_k)
					KEYS["K"] = 0;
				else if (event.key.keysym.sym == SDLK_l)
					KEYS["L"] = 0;
				else if (event.key.keysym.sym == SDLK_COLON)
					KEYS["COLON"] = 0;
				else if (event.key.keysym.sym == SDLK_QUOTE)
					KEYS["QUOTE"] = 0;
				else if (event.key.keysym.sym == SDLK_RETURN)
					KEYS["ENTER"] = 0;
				else if (event.key.keysym.sym == SDLK_LSHIFT)
					KEYS["SHIFT_L"] = 0;
				else if (event.key.keysym.sym == SDLK_z)
					KEYS["Z"] = 0;
				else if (event.key.keysym.sym == SDLK_x)
					KEYS["X"] = 0;
				else if (event.key.keysym.sym == SDLK_c)
					KEYS["C"] = 0;
				else if (event.key.keysym.sym == SDLK_v)
					KEYS["V"] = 0;
				else if (event.key.keysym.sym == SDLK_b)
					KEYS["B"] = 0;
				else if (event.key.keysym.sym == SDLK_n)
					KEYS["N"] = 0;
				else if (event.key.keysym.sym == SDLK_m)
					KEYS["M"] = 0;
				else if (event.key.keysym.sym == SDLK_COMMA)
					KEYS["COMMA"] = 0;
				else if (event.key.keysym.sym == SDLK_PERIOD)
					KEYS["PERIOD"] = 0;
				else if (event.key.keysym.sym == SDLK_SLASH)
					KEYS["SLASH"] = 0;
				else if (event.key.keysym.sym == SDLK_RSHIFT)
					KEYS["SHIFT_R"] = 0;
				else if (event.key.keysym.sym == SDLK_LCTRL)
					KEYS["CTRL_L"] = 0;
				else if (event.key.keysym.sym == SDLK_LALT)
					KEYS["ALT_L"] = 0;
				else if (event.key.keysym.sym == SDLK_SPACE)
					KEYS["SPACE"] = 0;
				else if (event.key.keysym.sym == SDLK_RALT)
					KEYS["ALT_R"] = 0;
				else if (event.key.keysym.sym == SDLK_RCTRL)
					KEYS["CTRL_R"] = 0;
				else if (event.key.keysym.sym == SDLK_LEFT)
					KEYS["LEFT"] = 0;
				else if (event.key.keysym.sym == SDLK_UP)
					KEYS["UP"] = 0;
				else if (event.key.keysym.sym == SDLK_RIGHT)
					KEYS["RIGHT"] = 0;
				else if (event.key.keysym.sym == SDLK_DOWN)
					KEYS["DOWN"] = 0;
			}
		}

		SDL_RenderClear(gRenderer);

		ExecuteFunction("Update", vector<boost::any> {dt});

		// Present the backbuffer
		SDL_RenderPresent(gRenderer);

		// Calculate frame time
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count() / 1000.0f;
		dt = clamp(dt, 0, 1000);
	}
	return 0;
}

int initGraphics(std::string windowTitle, int width, int height, int pixelScale)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
	PIXEL_SCALE = pixelScale;

	// Initialize SDL components
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	gWindow = SDL_CreateWindow(windowTitle.c_str(), 40, 40, WINDOW_WIDTH * PIXEL_SCALE, WINDOW_HEIGHT * PIXEL_SCALE, SDL_WINDOW_SHOWN | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	SDL_RenderSetLogicalSize(gRenderer, WINDOW_WIDTH * PIXEL_SCALE, WINDOW_HEIGHT * PIXEL_SCALE);
	SDL_RenderSetScale(gRenderer, PIXEL_SCALE, PIXEL_SCALE);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	ExecuteFunction("Start", vector<boost::any> {});

	updateLoop();

	cleanupGraphics();

	return 0;
}

#endif
