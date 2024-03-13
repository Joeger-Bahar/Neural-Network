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
	int neuronWidth = radius * 2;

	// Create the input layer
	for (int i = 0; i < inputCount; i++)
	{
		int verticalDistance = screenHeight / (inputCount + 1);
		neurons[0].push_back(Neuron({ static_cast<float>((neuronWidth)), static_cast<float>((i + 1) * verticalDistance) },
			Math::Random(0.0f, 1.0f), Math::Random(-0.1f, 0.1f), 0, radius));
	}

	// Calculate the horizontal distance between the layers in the style of css flexbox space around
	// Calculate the space available between the input and output layers
	int spaceBetween = screenWidth - 4 * neuronWidth;

	// Calculate the horizontal distance between the hidden layers
	int horizontalDistance = spaceBetween / (hiddenLayerCount);
	int currentX = neuronWidth;

	// Create the hidden layers
	for (int i = 0; i < hiddenLayerCount; i++)
	{
		for (int j = 0; j < hiddenLayerSize; j++)
		{
			//std::cout << "CurrentX: " << currentX << "\n";
			int verticalDistance = screenHeight / (hiddenLayerSize + 1);
			neurons[i + 1].push_back(Neuron({ static_cast<float>(currentX), static_cast<float>((j + 1) * verticalDistance) },
				Math::Random(0.0f, 1.0f), Math::Random(-0.1f, 0.1f), i + 1, radius));
		}
		currentX += horizontalDistance;
	}

	// Create the output layer
	for (int i = 0; i < outputCount; i++)
	{
		//std::cout << "CurrentX: " << currentX << "\n";
		int verticalDistance = screenHeight / (outputCount + 1);
		neurons[hiddenLayerCount + 1].emplace_back(Neuron({ static_cast<float>(currentX), static_cast<float>((i + 1) * verticalDistance) },
			Math::Random(0.0f, 1.0f), Math::Random(-0.1f, 0.1f), hiddenLayerCount + 2, radius));
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
	TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 35);

	// Render the connections
	for (int i = 1; i < neurons.size() - 1; i++)
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

				drawThickLine(renderer, neurons[i][j].coords.x, neurons[i][j].coords.y, neurons[i + 1][k].coords.x, neurons[i + 1][k].coords.y, 2);
			}
		}
	}
	// Render the nodes
	for (int i = 1; i < neurons.size(); ++i)
	{
		for (int j = 0; j < neurons[i].size(); ++j)
		{
			neurons[i][j].render(renderer, font);
		}
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
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

	for (size_t i = 0; i < neurons.size() - 1; ++i)
	{
		for (size_t j = 0; j < neurons[i].size(); ++j)
		{
			for (size_t k = 0; k < neurons[i + 1].size(); ++k)
			{
				float a = std::sqrt(6.0f / (neurons[i].size() + neurons[i + 1].size()));
				weights[i][j * neurons[i + 1].size() + k] = dis(gen) * a;
			}
		}
	}
}

void Network::setLayerActivation(std::vector<std::vector<float>>& weights, std::vector<std::vector<Neuron>>& neurons, int from_layer, int to_layer)
{
	for (size_t j = 0; j < neurons[to_layer].size(); ++j)
	{
		float weighted_sum = 0.0f;
		for (size_t k = 0; k < neurons[from_layer].size(); ++k)
		{
			weighted_sum += neurons[from_layer][k].activation * weights[from_layer][k * neurons[to_layer].size() + j];
		}
		// Add bias
		weighted_sum += neurons[to_layer][j].bias;
		// Apply Leaky ReLU activation function
		neurons[to_layer][j].activation = Math::sigmoid(weighted_sum);
	}
}

void Network::forwardPropagate()
{
	for (size_t i = 0; i < neurons.size() - 1; ++i)
	{
		setLayerActivation(weights, neurons, i, i + 1);
	}
	// Clamp output to 0 and 1
	for (size_t i = 0; i < neurons.back().size(); ++i)
	{
		neurons.back()[i].activation = std::max(neurons.back()[i].activation, 0.0f);
	}
}

void Network::backPropagate(std::vector<float> expected)
{
	// Calculate gradients for output layer
	for (int i = 0; i < neurons.back().size(); ++i)
	{
		float error = neurons.back()[i].activation - expected[i];
		float gradient = error * Math::sigmoidDerivative(neurons.back()[i].activation); // Use ReLU_Derivative here
		neurons.back()[i].gradient = gradient;
	}

	// Propagate gradients backwards
	for (int i = neurons.size() - 2; i > 0; --i)
	{
		for (int j = 0; j < neurons[i].size(); ++j)
		{
			float sum = 0.0f;
			for (int k = 0; k < neurons[i + 1].size(); ++k)
			{
				sum += neurons[i + 1][k].gradient * weights[i][j * neurons[i + 1].size() + k];
			}
			float gradient = sum * Math::sigmoidDerivative(neurons[i][j].activation); // Use ReLU_Derivative here
			neurons[i][j].gradient = gradient;
		}
	}

	// Update weights and biases using gradients
	for (int i = 0; i < weights.size(); ++i)
	{
		for (int j = 0; j < weights[i].size(); ++j)
		{
			weights[i][j] -= learningRate * neurons[i][j / neurons[i + 1].size()].activation * neurons[i + 1][j % neurons[i + 1].size()].gradient;
		}
	}
}

float Network::calculateCost(std::vector<float> expected)
{
	float cost = 0.0f;
	for (size_t i = 0; i < neurons.back().size(); ++i)
	{
		cost += (neurons.back()[i].activation - expected[i]) * (neurons.back()[i].activation - expected[i]);
	}
	return cost / neurons.back().size();
}

void Network::setInputs(const std::vector<float>& inputValues)
{
	// Check if the size of inputValues matches the input layer size
	if (inputValues.size() != neurons[0].size())
	{
		throw std::invalid_argument("Input size doesn't match the input layer size");
	}

	// Set input values to the neurons in the input layer
	for (size_t i = 0; i < inputValues.size(); ++i)
	{
		neurons[0][i].setValue(inputValues[i]);
	}
}

std::vector<float> Network::getOutputs()
{
	// Get the output values from the neurons in the output layer
	std::vector<float> outputValues(neurons.back().size());
	for (size_t i = 0; i < neurons.back().size(); ++i)
	{
		outputValues[i] = neurons.back()[i].getValue();
	}
	return outputValues;
}

void Network::save()
{
	std::ofstream file("network.txt");
	if (file.is_open())
	{
		file << neurons.size() << "\n";
		for (size_t i = 0; i < neurons.size(); ++i)
		{
			file << neurons[i].size() << "\n";
			for (size_t j = 0; j < neurons[i].size(); ++j)
			{
				file << neurons[i][j].bias << " " << neurons[i][j].activation << " " << neurons[i][j].coords.x << " " << neurons[i][j].coords.y << " " << neurons[i][j].radius << "\n";
			}
		}
		file << weights.size() << "\n";
		for (size_t i = 0; i < weights.size(); ++i)
		{
			file << weights[i].size() << "\n";
			for (size_t j = 0; j < weights[i].size(); ++j)
			{
				file << weights[i][j] << " ";
			}
		}
		file.close();
	}
}

void Network::load()
{
	std::ifstream file("network.txt");
	if (file.is_open())
	{
		int size;
		file >> size;
		neurons.resize(size);
		for (size_t i = 0; i < neurons.size(); ++i)
		{
			int size;
			file >> size;
			neurons[i].resize(size);
			for (size_t j = 0; j < neurons[i].size(); ++j)
			{
				file >> neurons[i][j].bias >> neurons[i][j].activation >> neurons[i][j].coords.x >> neurons[i][j].coords.y >> neurons[i][j].radius;
			}
		}
		file >> size;
		weights.resize(size);
		for (size_t i = 0; i < weights.size(); ++i)
		{
			int size;
			file >> size;
			weights[i].resize(size);
			for (size_t j = 0; j < weights[i].size(); ++j)
			{
				file >> weights[i][j];
			}
		}
		file.close();
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
