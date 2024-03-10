#include "network.hpp"

Network::Network(int screenWidth, int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) {}

Network& Network::withLayers(int inputCount, int hiddenLayerCount, int hiddenLayerSize, int outputCount)
{
	if (hiddenLayerSize == 0)
		hiddenLayerCount = 0;
	// Reserve the space for the neurons
	neurons.reserve(hiddenLayerCount + 2);
	for (int i = 0; i < hiddenLayerCount + 2; i++)
		neurons.push_back(std::vector<Neuron>());

	int radius = 35;

	// Calculate the horizontal distance between the layers in the style of css flexbox space around
	int neuronWidth = radius * 2;
	int spaceAvailable = screenWidth - (neuronWidth * (hiddenLayerCount + 2));
	spaceAvailable -= neuronWidth; // Subtract the radius of the first and last neuron
	int horizontalDistance = spaceAvailable / (hiddenLayerCount + 2);

	// Create the input layer
	for (int i = 0; i < inputCount; i++)
	{
		int verticalDistance = screenHeight / (inputCount + 1);
		neurons[0].push_back(Neuron({ static_cast<float>((neuronWidth)), static_cast<float>((i + 1) * verticalDistance) }, Math::Random(0.0f, 1.0f), 0.0f, 2, radius));
	}

	int currentX = horizontalDistance + neuronWidth;
	// Create the hidden layers
	for (int i = 0; i < hiddenLayerCount; i++)
	{
		for (int j = 0; j < hiddenLayerSize; j++)
		{
			int verticalDistance = screenHeight / (hiddenLayerSize + 1);
			neurons[i + 1].push_back(Neuron({ static_cast<float>(currentX + neuronWidth), static_cast<float>((j + 1) * verticalDistance) }, Math::Random(0.0f, 1.0f), 0.0f, 2, radius));
		}
		currentX += horizontalDistance + neuronWidth;
	}

	// Create the output layer
	for (int i = 0; i < outputCount; i++)
	{
		int verticalDistance = screenHeight / (outputCount + 1);
		neurons[hiddenLayerCount + 1].emplace_back(Neuron({ static_cast<float>(currentX + neuronWidth), static_cast<float>((i + 1) * verticalDistance) }, Math::Random(0.0f, 1.0f), 0.0f, 2, radius));
	}


	weights.resize(neurons.size() - 1);
	for (size_t i = 0; i < weights.size(); ++i)
	{
		weights[i].resize(neurons[i].size() * neurons[i + 1].size());
	}

	return *this;
}

void drawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness);

void Network::render(SDL_Renderer* renderer)
{
	TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);

	// Render the connections
	for (int i = 0; i < neurons.size() - 1; i++)
	{
		for (int j = 0; j < neurons[i].size(); ++j)
		{
			for (int k = 0; k < neurons[i + 1].size(); ++k)
			{
				// Draw a line between the nodes
				if (weights[i][j * neurons[i + 1].size() + k] > 0)
					SDL_SetRenderDrawColor(renderer, 51, 184, 100, 255); // Nice green color
				else
					SDL_SetRenderDrawColor(renderer, 184, 51, 51, 255); // Nice red color

				drawThickLine(renderer, neurons[i][j].coords.x + neurons[i][j].radius, neurons[i][j].coords.y, neurons[i + 1][k].coords.x, neurons[i + 1][k].coords.y, 2);
			}
		}
	}
	// Render the nodes
	for (auto& row : neurons)
	{
		for (Neuron& node : row)
			node.render(renderer, font);
	}
}

void Network::setActivation(float activation)
{
	for (auto& row : neurons)
	{
		for (Neuron& node : row)
			node.activation = activation;
	}
}

void Network::connectNodes()
{
	for (size_t i = 0; i < neurons.size() - 1; ++i)
	{
		for (size_t j = 0; j < neurons[i].size(); ++j)
		{
			for (size_t k = 0; k < neurons[i + 1].size(); ++k)
			{
				float randomWeight = std::round(Math::Random(-1.0f, 1.0f) * 100) / 100;
				weights[i][j * neurons[i + 1].size() + k] = randomWeight;
			}
		}
	}
}

void drawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness)
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	float angle = atan2(dy, dx);
	float length = sqrt(dx * dx + dy * dy);

	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	for (int i = 0; i < length; i += thickness)
	{
		SDL_Rect rect;
		rect.x = (x1 + i * cos_angle) - (thickness / 2);
		rect.y = (y1 + i * sin_angle) - (thickness / 2);
		rect.w = thickness;
		rect.h = thickness;

		SDL_RenderFillRect(renderer, &rect);
	}
}