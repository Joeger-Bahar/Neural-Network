#include "mouse.hpp"

void Mouse::update()
{
	static bool lastClicking = false;

	this->state = MouseState(SDL_BUTTON(SDL_GetMouseState(&(this->rect.x), &(this->rect.y)))); // Inside the inner most function call the mouse coords get updated

	bool clicking = (this->state & MouseState::LeftButton);

	lastClicking = clicking;
}

bool Mouse::getNewClick()
{
	static bool lastClicking = 0;
	bool clicking = (this->state & MouseState::LeftButton) | (this->state & MouseState::RightButton);

	// Rising edge case: click is true now but was false in the last frame
	bool newClick = clicking && !lastClicking;
	lastClicking = clicking;

	return newClick;
}

bool Mouse::getReleasedClick()
{
	static bool lastClicking = 0;
	bool clicking = (this->state & MouseState::LeftButton) | (this->state & MouseState::RightButton);

	// Falling edge case: click is false now but was true in the last frame
	bool releasedClick = !clicking && lastClicking;
	lastClicking = clicking;

	return releasedClick;
}