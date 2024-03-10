#pragma once

#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <array>

#include "SDL.h"
#include "SDL_ttf.h"

#include "math.hpp"

struct Neuron
{
public:
	Neuron(SDL_FPoint coords, float activation, float bias, int layer, int radius);
	void render(SDL_Renderer* renderer, TTF_Font* font);

	SDL_FPoint coords;
	float activation, bias;
	int radius, layer;
};