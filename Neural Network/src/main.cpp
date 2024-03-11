#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "SDL.h"
#include "SDL_ttf.h"

#include "window_manager.hpp"
#include "network.hpp"
#include "time.hpp"

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;

int main(int argv, char* argc[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Checks for memory leaks
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1"); // Disables high DPI scaling

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	WindowManager window("Neural Network", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

	Network network = Network(SCREEN_WIDTH, SCREEN_HEIGHT).withLayers(2, 4, 8, 1); // Spacing between front and back ends uneven
	network.connectNodes();

	bool runNetwork = 1;
	
	int totalCalculated = 0;

	srand(time(NULL));
	std::string inputString = "";
	while (1)
	{
		network.forwardPropagate();

		network.backPropagate({ static_cast<float>(static_cast<int>(network.getNodes()[0][0].activation) ^ static_cast<int>(network.getNodes()[0][1].activation)) });
		++totalCalculated;
		
		if (totalCalculated % 100000 == 0)
		{
			window.update();
			if (window.quit)
				return 0;
			window.present(&network);
			std::cout << "Total calculated: " << totalCalculated << "\n";
			std::cout << "Cost: " << network.calculateCost({ static_cast<float>(static_cast<int>(network.getNodes()[0][0].activation) ^ static_cast<int>(network.getNodes()[0][1].activation)) }) << std::endl;
		}
	}

	return 0;
}