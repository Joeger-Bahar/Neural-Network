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
	Neuron() : coords({ 0, 0 }), activation(0), bias(0), layer(0), radius(0) {}
	Neuron(SDL_FPoint coords, float activation, float bias, int layer, int radius);
	void render(SDL_Renderer* renderer, TTF_Font* font);

	inline float getValue() { return activation; }
	inline void setValue(float value) { activation = value; }

	SDL_FPoint coords;
	float activation, bias, gradient;
	int radius, layer;
};