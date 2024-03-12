#pragma once

#include <vector>

#include "SDL.h"

#include "network.hpp"
#include "math.hpp"

class WindowManager
{
public:
	WindowManager(const char* title, int x, int y, int w, int h, uint32_t flags) : screenWidth(w), screenHeight(h)
	{
		window = SDL_CreateWindow(title, x, y, w, h, flags);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		networkTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screenWidth - 600, screenHeight);
		informationTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 300);
	}

	inline SDL_Renderer* getRenderer() { return renderer; }
	inline SDL_Window* getWindow() { return window; }

	void checkEvents(int imagesSize)
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
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_RIGHT:
					imageIndex = Math::Random(0, 10000);
					increment = 1;
					break;
				case SDLK_LEFT:
					imageIndex = Math::Random(0, 10000);
					increment = 1;
					break;
				}
				break;
			}
		}
	}

	void update(int imagesSize)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		checkEvents(imagesSize);
	}

	void present(Network* network, SDL_Texture* number, float confidence, int predictedNumber, bool reDraw)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		if (reDraw)
		{
			SDL_SetRenderTarget(renderer, networkTexture);
			network->render(renderer);

			// Draw the number on the right side of the screen
			SDL_Rect dst = { screenWidth - (28 * 10), screenHeight / 2 - (28 * 5), 28 * 10, 28 * 10};

			SDL_RenderCopy(renderer, number, nullptr, &dst);

			// Make a string with "Confidence: " and the confidence * 100 followed by a percent sign
			std::string confidenceString = "Confidence: " + std::to_string(confidence * 100) + "%";
			std::string predictedNumberString = "Predicted Number: " + std::to_string(predictedNumber);

			// Draw the confidence and cost below the number
			SDL_SetRenderTarget(renderer, informationTexture);
			SDL_Color color = { 255, 255, 255, 255 };
			TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 35);
			SDL_Surface* confidenceSurface = TTF_RenderText_Solid(font, confidenceString.c_str(), color);
			SDL_Texture* confidenceTexture = SDL_CreateTextureFromSurface(renderer, confidenceSurface);
			SDL_Rect confidenceRect = { screenWidth / 2 + 200, 100, 366, 66 };
			SDL_RenderCopy(renderer, confidenceTexture, nullptr, &confidenceRect);
			SDL_FreeSurface(confidenceSurface);
			SDL_DestroyTexture(confidenceTexture);

			SDL_Surface* predictedNumberSurface = TTF_RenderText_Solid(font, predictedNumberString.c_str(), color);
			SDL_Texture* predictedNumberTexture = SDL_CreateTextureFromSurface(renderer, predictedNumberSurface);
			SDL_Rect predictedNumberRect = { screenWidth / 2 + 200, 200, 366, 66 };
			SDL_RenderCopy(renderer, predictedNumberTexture, nullptr, &predictedNumberRect);
			SDL_FreeSurface(predictedNumberSurface);
			SDL_DestroyTexture(predictedNumberTexture);
		}

		SDL_SetRenderTarget(renderer, nullptr);

		SDL_Rect networkRect = { 0, 0, screenWidth - 600, screenHeight };
		SDL_Rect informationRect = { screenWidth / 2 + 200, 100, 400, 300 };

		SDL_RenderCopy(renderer, networkTexture, nullptr, &networkRect);
		SDL_RenderCopy(renderer, informationTexture, nullptr, &informationRect);

		SDL_RenderPresent(renderer);
	}
private:
	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Texture* networkTexture;
	SDL_Texture* informationTexture;
	int screenWidth, screenHeight;
public:
	int imageIndex = 0;
	bool quit = 0, increment = 0;
};