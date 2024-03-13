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

		networkTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screenWidth - 750, screenHeight);
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
					increment = 1;
					break;
				case SDLK_LEFT:
					increment = 1;
					break;
				}
				break;
			}
		}
	}

	void update(int imagesSize)
	{
		SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
		SDL_RenderClear(renderer);
		checkEvents(imagesSize);
	}

	void present(Network* network, SDL_Texture* number, float confidence, int predictedNumber, int correctNumber, bool correct, bool reDraw)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		if (reDraw)
		{
			SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
			SDL_SetRenderTarget(renderer, networkTexture);
			SDL_RenderClear(renderer);
			network->render(renderer);
			// Clear the information texture
	

			// Draw the number on the right side of the screen

			// Make a string with "Confidence: " and the confidence * 100 followed by a percent sign
			std::string confidenceString = "Confidence: " + std::to_string(confidence * 100) + "%";
			std::string predictedNumberString = "Predicted Number: " + std::to_string(predictedNumber);
			std::string correctString;
			std::string correctNumberString = "Correct Number: " + std::to_string(correctNumber);

			if (correct)
				correctString = "Correct";
			else
				correctString = "Incorrect";

			// Draw the confidence and cost below the number
			SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
			SDL_SetRenderTarget(renderer, informationTexture);
			SDL_RenderClear(renderer);

			SDL_Color color = { 255, 255, 255, 255 };

			TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 35);

			SDL_Surface* confidenceSurface = TTF_RenderText_Blended(font, confidenceString.c_str(), color);
			SDL_Texture* confidenceTexture = SDL_CreateTextureFromSurface(renderer, confidenceSurface);

			SDL_Rect confidenceRect = { 0, 0, confidenceSurface->w, confidenceSurface->h };
			SDL_RenderCopy(renderer, confidenceTexture, nullptr, &confidenceRect);

			SDL_Surface* predictedNumberSurface = TTF_RenderText_Blended(font, predictedNumberString.c_str(), color);
			SDL_Texture* predictedNumberTexture = SDL_CreateTextureFromSurface(renderer, predictedNumberSurface);

			SDL_Rect predictedNumberRect = { 0, confidenceSurface->h, predictedNumberSurface->w, predictedNumberSurface->h };
			SDL_RenderCopy(renderer, predictedNumberTexture, nullptr, &predictedNumberRect);

			SDL_Surface* correctNumberSurface = TTF_RenderText_Blended(font, correctNumberString.c_str(), color);
			SDL_Texture* correctNumberTexture = SDL_CreateTextureFromSurface(renderer, correctNumberSurface);

			SDL_Rect correctNumberRect = { 0, predictedNumberRect.h + confidenceRect.h, correctNumberSurface->w, correctNumberSurface->h };
			SDL_RenderCopy(renderer, correctNumberTexture, nullptr, &correctNumberRect);

			SDL_Surface* correctSurface = TTF_RenderText_Blended(font, correctString.c_str(), color);
			SDL_Texture* correctTexture = SDL_CreateTextureFromSurface(renderer, correctSurface);

			SDL_Rect correctRect = { 0, predictedNumberRect.h + confidenceRect.h + correctNumberRect.h, correctSurface->w, correctSurface->h };
			SDL_RenderCopy(renderer, correctTexture, nullptr, &correctRect);


			SDL_FreeSurface(confidenceSurface);
			SDL_DestroyTexture(confidenceTexture);
			SDL_FreeSurface(predictedNumberSurface);
			SDL_DestroyTexture(predictedNumberTexture);
			SDL_FreeSurface(correctSurface);
			SDL_DestroyTexture(correctTexture);
			SDL_FreeSurface(correctNumberSurface);
			SDL_DestroyTexture(correctNumberTexture);
		}

		SDL_Rect dst = { screenWidth - 635, screenHeight / 2 - 56, 28 * 16, 28 * 16};
		SDL_RenderCopy(renderer, number, nullptr, &dst);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_SetRenderTarget(renderer, nullptr);

		SDL_Rect networkRect = { 0, 0, screenWidth - 750, screenHeight };
		SDL_Rect informationRect = { screenWidth / 2 + 50, 0, 400, 300 };

		SDL_RenderCopy(renderer, networkTexture, nullptr, &networkRect);
		SDL_RenderCopy(renderer, informationTexture, nullptr, &informationRect);
		//SDL_RenderDrawRect(renderer, &informationRect);
		//SDL_RenderDrawRect(renderer, &networkRect);

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