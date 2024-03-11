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

	inline void setLearningRate(float learningRate) { this->learningRate = learningRate; }

	void render(SDL_Renderer* renderer);

	void setActivation(float activation);
	void connectNodes();

	void forwardPropagate();
	void backPropagate(std::vector<float> expected);

	float calculateCost(std::vector<float> expected);
	void setLayerActivation(std::vector<std::vector<float>>& weights, std::vector<std::vector<Neuron>>& neurons, int from_layer, int to_layer);

private:
	std::vector<std::vector<float>> weights;
	std::vector<std::vector<Neuron>> neurons;
	double learningRate = 0.1;
	int screenWidth, screenHeight;
};
