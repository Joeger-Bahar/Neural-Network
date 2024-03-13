#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>

#include "SDL.h"
#include "SDL_ttf.h"

#include "window_manager.hpp"
#include "network.hpp"
#include "button.hpp"
#include "time.hpp"

/*
ADD CORRECT NUMBER TO THE SCREEN
MAKE NEURONS SMALLER DEPENDING ON THE AMOUNT OF NEURONS
*/

constexpr int IMAGE_WIDTH = 28;
constexpr int IMAGE_HEIGHT = 28;

std::vector<std::vector<float>>* readMNISTImages(const std::string& filename, int numberOfImages)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        return {};
    }

    int magic_number = 0;
    int number_of_images = 0;
    int n_rows = 0;
    int n_cols = 0;

    file.read((char*)&magic_number, sizeof(magic_number));
    magic_number = SDL_SwapBE32(magic_number);

    if (magic_number != 2051)
    {
        std::cerr << "Invalid MNIST image file!" << std::endl;
        return {};
    }

    file.read((char*)&number_of_images, sizeof(number_of_images));
    number_of_images = SDL_SwapBE32(number_of_images);
    file.read((char*)&n_rows, sizeof(n_rows));
    n_rows = SDL_SwapBE32(n_rows);
    file.read((char*)&n_cols, sizeof(n_cols));
    n_cols = SDL_SwapBE32(n_cols);

    std::vector<std::vector<float>>* images = new std::vector<std::vector<float>>();
    images->reserve(numberOfImages);

    for (int i = 0; i < number_of_images && i < numberOfImages; ++i)
    {
        std::vector<float> image;
        image.reserve(IMAGE_WIDTH * IMAGE_HEIGHT);

        for (int r = 0; r < n_rows; ++r)
        {
            for (int c = 0; c < n_cols; ++c)
            {
                unsigned char pixel = 0;
                file.read((char*)&pixel, sizeof(pixel));
                image.push_back(static_cast<float>(pixel) / 255.0f); // Normalize pixel values
            }
        }

        images->push_back(image);
    }

    file.close();
    return images;
}

std::vector<std::string>* readMNISTLabels(const std::string& filename, int numberOfLabels)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        return {};
    }

    int magic_number = 0;
    int number_of_labels = 0;

    file.read((char*)&magic_number, sizeof(magic_number));
    magic_number = SDL_SwapBE32(magic_number);

    if (magic_number != 2049)
    {
        std::cerr << "Invalid MNIST label file!" << std::endl;
        return {};
    }

    file.read((char*)&number_of_labels, sizeof(number_of_labels));
    number_of_labels = SDL_SwapBE32(number_of_labels);

    std::vector<std::string>* labels = new std::vector<std::string>();
    labels->reserve(numberOfLabels);

    for (int i = 0; i < number_of_labels && i < numberOfLabels; ++i)
    {
        unsigned char label = 0;
        file.read((char*)&label, sizeof(label));
        labels->emplace_back(std::to_string(static_cast<int>(label)));
    }

    file.close();
    return labels;
}

SDL_Texture* createTextureFromMNISTImage(SDL_Renderer* renderer, const std::vector<float>& image)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, IMAGE_WIDTH, IMAGE_HEIGHT, 32, 0, 0, 0, 0);

    if (surface == nullptr)
    {
        std::cerr << "SDL_CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    for (int y = 0; y < IMAGE_HEIGHT; ++y)
    {
        for (int x = 0; x < IMAGE_WIDTH; ++x)
        {
            Uint32 pixel = SDL_MapRGB(surface->format, image[y * IMAGE_WIDTH + x] * 255, image[y * IMAGE_WIDTH + x] * 255, image[y * IMAGE_WIDTH + x] * 255);
            SDL_Rect rect = { x, y, 1, 1 };
            SDL_FillRect(surface, &rect, pixel);
        }
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 850;

int main(int argv, char* argc[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Checks for memory leaks
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1"); // Disables high DPI scaling

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	WindowManager window("Neural Network", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

	Network network = Network(SCREEN_WIDTH - 600, SCREEN_HEIGHT).withLayers(784, 4, 32, 10); // Spacing between front and back ends uneven
    Text text(window.getRenderer(), NULL);
    Mouse mouse;

    ButtonManager button(window.getRenderer(), text, NULL, mouse);

	network.connectNodes();

	bool runNetwork = 1;
	
	int totalCalculated = 0;

    std::vector<std::vector<float>>* mnistTrainImages = readMNISTImages("mnist_images.idx3-ubyte", 60000);
    std::vector<std::string>* mnistTrainLabels = readMNISTLabels("mnist_labels.idx1-ubyte", 60000);

    std::vector<std::vector<float>>* mnistTestImages = readMNISTImages("test_images.idx3-ubyte", 10000);
    std::vector<std::string>* mnistTestLabels = readMNISTLabels("test_labels.idx1-ubyte", 10000);

    SDL_assert(mnistTrainImages->size() == mnistTrainLabels->size());
    if (mnistTrainImages->empty())
    {
        std::cerr << "Failed to read MNIST images!" << std::endl;
        //return 1;
    }
    window.present(&network, createTextureFromMNISTImage(window.getRenderer(), (*mnistTrainImages)[window.imageIndex]), 0, 0, 0, 1);
	srand(time(NULL));
	std::string inputString = "";

    window.imageIndex = 0;

    int frames = 0;
    float confidence = 1.0f;
    int predictedNumber = 7;
    bool reDraw = 0;
    bool correct = 0;

	while (1)
	{
        ++frames;
		window.update((*mnistTestImages).size());
		if (window.quit)
			return 0;
        mouse.update();
        SDL_SetRenderTarget(window.getRenderer(), NULL);
        if (window.increment) // button.buttonPressed("Next", {SCREEN_WIDTH + 100, SCREEN_HEIGHT - 400})
        {
            window.increment = 0;
	        network.setInputs((*mnistTestImages)[window.imageIndex]);
	        network.forwardPropagate();

            std::vector<float> expected;
            expected.reserve(10);
            for (int i = 0; i < 10; ++i)
            {
				expected.push_back(0.0f);
			}
            expected[std::stoi((*mnistTestLabels)[window.imageIndex])] = 1.0f;

	        network.backPropagate(expected);
	        ++totalCalculated;

			confidence = 1 - network.calculateCost(expected);
            predictedNumber = 0;
            float prevHighest = 0;
            std::vector<float> output = network.getOutputs();
            for (int i = 0; i < output.size(); ++i)
            {
                if (output[i] > prevHighest)
                {
					prevHighest = output[i];
					predictedNumber = i;
				}
			}

            correct = std::stoi((*mnistTestLabels)[window.imageIndex]) == predictedNumber;

			window.present(&network, createTextureFromMNISTImage(window.getRenderer(), (*mnistTestImages)[window.imageIndex]), confidence, predictedNumber, correct, 1);
			std::cout << "Total calculated: " << totalCalculated << "\n";
        }
        SDL_SetRenderTarget(window.getRenderer(), NULL);
        if (button.buttonPressed("Save", { SCREEN_WIDTH - 310, 10, 250, 66 }))
        {
            network.save();
            std::cout << "Done saving" << std::endl;
        }
        SDL_SetRenderTarget(window.getRenderer(), NULL);
        if (button.buttonPressed("Load", { SCREEN_WIDTH - 310, 110, 250, 66 }))
        {
            network.load();
            std::cout << "Done loading" << std::endl;
            window.present(&network, createTextureFromMNISTImage(window.getRenderer(), (*mnistTestImages)[window.imageIndex]), confidence, predictedNumber, correct, 1);
        }
        SDL_SetRenderTarget(window.getRenderer(), NULL);
        if (button.buttonPressed("Train", { SCREEN_WIDTH - 310, 210, 250, 66 }))
        {
            for (int j = 0; j < 10; ++j)
            {
                window.present(&network, createTextureFromMNISTImage(window.getRenderer(), (*mnistTrainImages)[Math::Random(0, 59999)]), 0, 0, correct, 1);
                std::cout << "Training: " << j << '\n';
                window.imageIndex = Math::Random(0, 68999);
                for (int x = 0; x < 69000; ++x)
                {
                    if (window.imageIndex < 60000)
                        network.setInputs((*mnistTrainImages)[window.imageIndex]);
                    else
                        network.setInputs((*mnistTestImages)[window.imageIndex - 60000]);
                    network.forwardPropagate();

                    std::vector<float> expected;
                    expected.reserve(10);
                    for (int i = 0; i < 10; ++i)
                    {
                        expected.push_back(0.0f);
                    }
                    if (window.imageIndex < 60000)
                        expected[std::stoi((*mnistTrainLabels)[window.imageIndex])] = 1.0f;
                    else
                        expected[std::stoi((*mnistTestLabels)[window.imageIndex - 60000])] = 1.0f;

                    network.backPropagate(expected);
                    ++totalCalculated;
                    window.imageIndex = Math::Random(0, 68999);
                }
            }
            window.imageIndex = 0;
            std::cout << "Done training" << std::endl;
        }
        SDL_SetRenderTarget(window.getRenderer(), NULL);

        window.present(&network, createTextureFromMNISTImage(window.getRenderer(), (*mnistTestImages)[window.imageIndex]), confidence, predictedNumber, correct, 0);
	}
    delete mnistTrainImages;
    delete mnistTrainLabels;
    delete mnistTestImages;
    delete mnistTestLabels;

	return 0;
}