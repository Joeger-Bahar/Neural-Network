#pragma once

#include <vector>

#include "neuron.hpp"
#include "math.hpp"

#include "SDL_ttf.h"

class Network
{
public:
	Network(int screenWidth, int screenHeight);
	inline std::vector<std::vector<Neuron>>& getNodes() { return neurons; }

	Network& withLayers(int inputCount, int hiddenLayerCount, int hiddenLayerSize, int outputCount);

	void render(SDL_Renderer* renderer);

	void setActivation(float activation);

	void connectNodes();

private:
	std::vector<std::vector<float>> weights;
	std::vector<std::vector<Neuron>> neurons;
	int screenWidth, screenHeight;
};