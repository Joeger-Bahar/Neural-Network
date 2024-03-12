#pragma once

#include <iostream>
#include <string>
#include <SDL.h>

enum MouseState
{
	LeftButton = 1 << 0,
	MiddleButton = 1 << 1,
	LeftMiddleButton = 1 << 2,
	RightButton = 1 << 3,
	LeftRightButton = 1 << 4,
	MiddleRightButton = 1 << 5,
	LeftMiddleRightButton = 1 << 6,
	NoButton = -2147483648
};

struct Mouse
{
	void update();
	bool getNewClick();
	bool getReleasedClick();

	SDL_Rect rect = { 0, 0, 1, 1 };
	MouseState state{};
};