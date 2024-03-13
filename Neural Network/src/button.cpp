#include "button.hpp"

#define BUTTON_X_PADDING 2.0f // 20%
#define BUTTON_Y_PADDING 1.8f // 18%

ButtonManager::ButtonManager(SDL_Renderer* renderer, Text& text, SDL_Texture* renderTex, Mouse& mouse)
	: m_Text(text), m_Renderer(renderer), m_RenderTex(renderTex), backgroundColor({ 192, 192, 192, 255 }),
	m_Mouse(mouse), currentButtonIndex(0) {}

// Clears the buttons on the screen and resets the button index
void ButtonManager::clearButtonsOnScreen()
{
	currentButtonIndex = 0;
	m_Buttons.clear();
}

// Hashes the text to get a unique index for the button
int ButtonManager::hash(const char* text)
{
	int hash = 0;
	int c;

	while (c = *text++)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

// Doesn't actually check if the button was pressed, but this function is the main way to render a button
// Such as "if (buttonManager.buttonPressed("Play")) { ... }"
// The click and hover check is done in the button class's render function. This function just gets the button's text and dimensions and calls the button render function
bool ButtonManager::buttonPressed(const char* text, SDL_Rect rect)
{
	// Sets the render target to the desired render texture
	SDL_SetRenderTarget(m_Renderer, m_RenderTex);

	if (m_Buttons.find(text) == m_Buttons.end()) // If the button's text isn't in the current list, make a new button
	{
		auto [messageTexture, messageSurface] = m_Text.getTextTexture(m_Text.Arial, m_Text.White, text, currentButtonIndex <= 2 ? 30 : 30);

		// Renders the button halfway down and in the center of the screen
		SDL_Rect buttonDimensions(rect.x, rect.y, rect.w, rect.h);

		// Create new button instance
		m_Buttons.emplace(text, Button(currentButtonIndex, buttonDimensions, messageTexture, {0, 0, messageSurface->w, messageSurface->h}));
		++currentButtonIndex;

		// Frees the text surface
		SDL_FreeSurface(messageSurface);
	}

	// Sets the render color to the background color
	SDL_SetRenderDrawColor(m_Renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	// Renders the actual button with the desired dimensions
	// This throws
	m_Buttons.at(text).renderButton(m_Renderer, m_Mouse, m_Text);

	return (m_Buttons.at(text).hovered && m_Mouse.getNewClick());
}


Button::Button(const int index, SDL_Rect dimensions, SDL_Texture* texture, SDL_Rect textDimensions) : hovered(0), index(index), m_Dimensions(dimensions),
	m_TextTexture(texture), textDimensions(textDimensions) {}

void Button::renderButton(SDL_Renderer* renderer, Mouse& mouse, Text& text)
{
	this->hovered = SDL_HasIntersection(&(m_Dimensions), &(mouse.rect));

	// Draws and fills the button rect
	SDL_RenderDrawRect(renderer, &(m_Dimensions));
	SDL_RenderFillRect(renderer, &(m_Dimensions));

	if (this->hovered)
	{
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderDrawRect(renderer, &(m_Dimensions));
	}

	// Width and height are divided by the padding to get the text's dimensions relative to the whole button
	SDL_Rect textRenderDimensions({ m_Dimensions.x, m_Dimensions.y, textDimensions.w, textDimensions.h });

	// Centers text
	textRenderDimensions.x = textRenderDimensions.x + ((m_Dimensions.w - textDimensions.w) / 2);
	textRenderDimensions.y = textRenderDimensions.y + ((m_Dimensions.h - textDimensions.h) / 2);

	// Renders the text
	if (SDL_RenderCopy(renderer, m_TextTexture, NULL, &textRenderDimensions) == -1)
		std::cout << "Button text render failed: " << SDL_GetError() << index << "\n";
}