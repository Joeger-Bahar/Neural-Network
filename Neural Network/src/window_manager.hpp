#pragma once

#include "SDL.h"

#include "network.hpp"

class WindowManager
{
public:
	WindowManager(const char* title, int x, int y, int w, int h, uint32_t flags)
	{
		window = SDL_CreateWindow(title, x, y, w, h, flags);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}

	void checkEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
				this->quit = 1;
			}
		}
	}

	void update()
	{
		checkEvents();
	}

	void present(Network* network)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		network->render(renderer);

		SDL_RenderPresent(renderer);
	}
private:
	SDL_Renderer* renderer;
	SDL_Window* window;
public:
	bool quit = 0;
};