#pragma once

#include <iostream>
#include <tuple>
#include <unordered_map>
#include <SDL.h>
#include <SDL_ttf.h>

struct Font
{
	Font() = default;
	Font(const char* font, int size);
	void reRender(int size);
	void freeFonts();

	const char* m_FontPath = nullptr;
	std::unordered_map<int, TTF_Font*> fontSizes;

private:
	void loadFont(int size);
};

struct Text
{
	Text(SDL_Renderer* renderer, SDL_Texture* renderTex);
	~Text();

	void RenderText(Font& font, const SDL_Color& color, const char* text, const SDL_Rect&& rect, const bool centered = 0, uint32_t size = 20);
	std::tuple<SDL_Texture*, SDL_Surface*> getTextTexture(Font& font, const SDL_Color& color, const char* text, uint32_t size = 20);

	Font Sans;
	Font Arial;
	Font Comic;
	Font Times;

	SDL_Color White = { 255, 255, 255 };
	SDL_Color Red = { 255, 0, 0 };
	SDL_Color Green = { 0, 255, 0 };
	SDL_Color Blue = { 0, 0, 255 };
	SDL_Color Black = { 0, 0, 0 };


private:
	SDL_Renderer* m_Renderer;
	SDL_Texture* m_RenderTex;

	void renderInternal(SDL_Texture* message, SDL_Rect& Message_rect);
};