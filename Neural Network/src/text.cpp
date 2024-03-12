#include "text.hpp"

Font::Font(const char* font, int size) : m_FontPath(font)
{
	loadFont(size);  // Load the font at the initial size
}

void Font::reRender(int size)
{
	if (fontSizes.find(size) != fontSizes.end())
	{
		// Font at the specified size is already in the cache
		return;
	}

	// Font at the specified size is not in the cache, so load it
	loadFont(size);
}

void Font::loadFont(int size)
{
	TTF_Font* newFont = TTF_OpenFont(m_FontPath, size);
	if (newFont)
	{
		// Font successfully generated
		fontSizes[size] = newFont;
	}
	else
	{
		// Font failed to load
		std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
	}
}

void Font::freeFonts()
{
	int x = 0;
	std::cout << "size: " << fontSizes.size() << "\n";
	// Release all fonts in the cache to avoid memory leaks
	for (auto& pair : fontSizes)
	{
		if (!pair.second)
			std::cout << "pair.second was null: " << pair.second << "\n";
		std::cout << x << "\n";
		std::cout << pair.first << "\n";
		TTF_CloseFont(pair.second);
		++x;
	}
	fontSizes.clear();
}


Text::Text(SDL_Renderer* renderer, SDL_Texture* renderTex) : m_Renderer(renderer), m_RenderTex(renderTex),
	Sans("Fonts/sans.ttf", 20), Arial("Fonts/arial.ttf", 20), Comic("Fonts/comic.ttf", 20),
	Times("Fonts/times.ttf", 20) {}

Text::~Text()
{
	Sans.freeFonts();
	Arial.freeFonts();
	Comic.freeFonts();
	Times.freeFonts();
	TTF_Quit();
}

void Text::RenderText(Font& font, const SDL_Color& color, const char* text, const SDL_Rect&& rect, const bool centered, uint32_t size)
{
	if (!text)
	{
		std::cout << "text was null: " << text << "\n";
		return;
	}

	if (font.fontSizes.find(size) == font.fontSizes.end())
	{
		font.reRender(size);
	}

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font.fontSizes[size], text, color);

	if (!surfaceMessage)
	{
		std::cerr << "Text render creation failed: " << SDL_GetError() << "\n";
		return;
	}

	SDL_Texture* Message = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);
	SDL_Rect messageRect;

	if (rect.w || rect.h)
	{
		messageRect.w = rect.w;
		messageRect.h = rect.h;
	}
	else
	{
		messageRect.w = surfaceMessage->w;
		messageRect.h = surfaceMessage->h;
	}

	if (centered)
	{
		messageRect.x = rect.x + ((rect.w - messageRect.w) / 2);
		messageRect.y = rect.y + ((rect.h - messageRect.h) / 2);
	}
	else
	{
		messageRect.x = rect.x;
		messageRect.y = rect.y;
	}

	renderInternal(Message, messageRect);

	SDL_FreeSurface(surfaceMessage);
}

// Returns the surface because the texture doesn't have dimension variables
std::tuple<SDL_Texture*, SDL_Surface*> Text::getTextTexture(Font& font, const SDL_Color& color, const char* text, uint32_t size)
{
	if (!text)
	{
		std::cout << "text was null: " << text << "\n";
		return { nullptr, nullptr };
	}

	if (font.fontSizes.find(size) == font.fontSizes.end())
	{
		font.reRender(size);
	}

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font.fontSizes[size], text, color);

	if (!surfaceMessage)
	{
		std::cout << "Text render creation failed: " << SDL_GetError() << "\n";
		return { nullptr, nullptr };
	}

	SDL_Texture* message = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);
	if (!message)
		std::cout << "Message creation failed: " << SDL_GetError() << "\n";

	return { message, surfaceMessage };
}

void Text::renderInternal(SDL_Texture* message, SDL_Rect& messageRect)
{
	SDL_SetRenderTarget(m_Renderer, m_RenderTex);

	if (SDL_RenderCopy(m_Renderer, message, nullptr, &messageRect) < 0)
	{
		std::cout << "Text rendering failed: " << SDL_GetError();
	}

	SDL_DestroyTexture(message);
}