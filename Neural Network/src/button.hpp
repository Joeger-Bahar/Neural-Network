#pragma once

#include <map>
#include <SDL.h>

#include "mouse.hpp"
#include "text.hpp"
#include "math.hpp"

struct Button
{
public:
	Button() : index(-1), hovered(0), m_Dimensions({ NULL, NULL, NULL, NULL }), m_TextTexture(nullptr) {}
	Button(const int index, SDL_Rect dimensions, SDL_Texture* texture, SDL_Rect textDimensions);
	void renderButton(SDL_Renderer* renderer, Mouse& mouse, Text& text);

	bool hovered;
	int index;
	SDL_Rect m_Dimensions;
	SDL_Rect textDimensions;
	SDL_Texture* m_TextTexture;
};

class ButtonManager
{
public:
	ButtonManager(SDL_Renderer* renderer, Text& text, SDL_Texture* renderTex, Mouse& mouse);

	void clearButtonsOnScreen();
	bool buttonPressed(const char* text, SDL_Rect rect);
	int hash(const char* text);

private:
	int currentButtonIndex; // Also used for the amount of buttons on screen
	std::map<const char*, Button> m_Buttons;

	Mouse& m_Mouse;

	SDL_Renderer* m_Renderer;
	Text& m_Text;
	SDL_Texture* m_RenderTex;
	SDL_Rect windowDimensions;
	SDL_Color backgroundColor;
};