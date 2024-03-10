#include <iostream>
#include <conio.h>
#include <string>

#include "SDL.h"
#include "SDL_ttf.h"

#include "window_manager.hpp"
#include "network.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;

int main(int argv, char* argc[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Checks for memory leaks
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1"); // Disables high DPI scaling

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	WindowManager window("Neural Network", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_HEIGHT, SCREEN_WIDTH, SDL_WINDOW_RESIZABLE);

	// Create a network with 2 input nodes, 2 hidden layers, 1 node per layer, 1 output node, and a screen size of 720x720
	Network network = Network(SCREEN_WIDTH, SCREEN_HEIGHT).withLayers(5, 7, 1, 1); // Problems with rendering nodes: 5, 7, 1, 1
	network.connectNodes();

	std::string inputString = "";
	while (1)
	{
		window.update();
		if (window.quit)
			return 0;
		window.present(&network);
		if (_kbhit())
		{
			auto key = _getch();

			if (isdigit(key) || key == '.')
			{
				inputString += key;
			}
			else if (key == '\r')
			{
				try {
					double number = std::stod(inputString);
					network.setActivation(number);
					inputString.clear(); // Clear the input buffer
				}
				catch (const std::invalid_argument& e) {
					std::cerr << "\nInvalid input. Please enter a valid number." << std::endl;
					inputString.clear(); // Clear the input buffer
				}
			}
		}
	}

	return 0;
}