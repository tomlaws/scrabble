#include "Button.h"

std::vector<Button*> Button::buttons;
Button * Button::mouseOverButton;
Button * Button::mouseDownButton;
TTF_Font * Button::tooltipFont;

Button::Button()
{
	isInstance = false;
	tooltipFont = TTF_OpenFont("FreeSans.ttf", 14);
}

Button::~Button()
{
	if (isInstance) return;
	for (int i = 0; i < buttons.size(); i++) {
		SDL_DestroyTexture(buttons[i]->textTexture);
		delete buttons[i];
	}
	buttons.clear();
	TTF_CloseFont(tooltipFont);
}

Button * Button::add(std::string name, std::wstring text, std::string tooltip, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void()> clickEvent)
{
	return new Button(name, text, tooltip, color, font, x, y, w, h, clickEvent);
}

Button::Button(std::string name, std::wstring text, std::string tooltip, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void()> clickEvent)
{
	this->name = name;
	this->text = text;
	this->tooltip = tooltip;
	this->color = color;
	textColor = SDL_Color{ 255, 255, 255, 255 };
	this->font = font;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->enabled = true;
	visible = true;
	this->clickEvent = clickEvent;
	textTexture = nullptr;
	isInstance = true;
	buttons.push_back(this);
	mouseDownButton = nullptr;
	mouseOverButton = nullptr;
}

Button::Button(std::wstring text, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void()> clickEvent)
{
	this->text = text;
	this->color = color;
	textColor = SDL_Color{ 255, 255, 255, 255 };
	this->font = font;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->enabled = true;
	visible = true;
	this->clickEvent = clickEvent;
	textTexture = nullptr;
	isInstance = true;
	mouseDownButton = nullptr;
	mouseOverButton = nullptr;
}

void Button::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

void Button::setVisible(bool visible)
{
	this->visible = visible;
}

void Button::setClickEvent(std::function<void()> clickEvent)
{
	this->clickEvent = clickEvent;
}

void Button::draw(SDL_Renderer * renderer)
{
	if (!visible) return;

	//	Set Color
	if (mouseDownButton && this == mouseDownButton) {
		int r = this->color.r - 5;
		int g = this->color.g - 5;
		int b = this->color.b - 5;
		SDL_SetRenderDrawColor(renderer, r < 0 ? 0 : r, g < 0 ? 0 : g, b < 0 ? 0 : b, this->color.a);
	}
	else if (mouseOverButton && this == mouseOverButton) {
		showTooltip(renderer);
		int r = this->color.r + 15;
		int g = this->color.g + 15;
		int b = this->color.b + 15;
		SDL_SetRenderDrawColor(renderer, r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b, this->color.a);
	}
	else
		SDL_SetRenderDrawColor(renderer, this->color.r, this->color.g, this->color.b, this->color.a);

	//	Draw Rect
	SDL_RenderFillRect(renderer, this);
	//	Draw Text (Prevent creating texture every frame)
	if (!textTexture) {
		std::wstring s = text;
		SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, (Uint16*)s.c_str(), textColor);
		if (surface) {
			textTexture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_QueryTexture(textTexture, NULL, NULL, &textTextureRect.w, &textTextureRect.h);
			textTextureRect.x = x + (w - textTextureRect.w) / 2;
			textTextureRect.y = y + (h - textTextureRect.h) / 2;
			SDL_FreeSurface(surface);
		}
	}
	SDL_RenderCopy(renderer, textTexture, NULL, &textTextureRect);
}

void Button::showTooltip(SDL_Renderer * renderer)
{
	//	Tooltip for the button
	if (tooltip.length() > 0) {
		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface * surface = TTF_RenderText_Blended(tooltipFont, tooltip.c_str(), textColor);
		if (surface) {
			SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_Rect hints;
			SDL_QueryTexture(texture, NULL, NULL, &hints.w, &hints.h);

			int nx = (x + w / 2) - hints.w / 2;
			if (nx < 0)
				nx = 0 + 10;	//	Min is 0, plus padding
			if (nx + hints.w > 1024)
				nx = 1024 - hints.w - 10;	//	Max is win width - hint width, plus padding
			hints.x = nx;
			int ny = y - 25;
			hints.y = ny < 0 ? ny + 50 : ny;

			//	Draw background for hints
			SDL_Rect bg;
			bg.x = hints.x - 10;
			bg.y = hints.y - 3;
			bg.w = hints.w + 20;
			bg.h = hints.h + 6;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 155);
			SDL_RenderFillRect(renderer, &bg);

			//	Render text now
			SDL_RenderCopy(renderer, texture, NULL, &hints);
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
	}
}

/* Static members */

void Button::drawAll(SDL_Renderer * renderer)
{
	for (Button * button : buttons) {
		button->draw(renderer);
	}
}

void Button::mouseEvent(SDL_Event * event, int mouseX, int mouseY)
{
	Button* targetButton = NULL;
	for (Button * button : buttons) {
		if (!button->enabled || !button->visible) continue;
		int tx = button->x;
		int ty = button->y;
		int tw = button->w;
		int th = button->h;
		if (mouseX >= tx && mouseX <= tx + tw && mouseY >= ty && mouseY <= ty + th) {
			targetButton = button;
			break;
		}
	}
	if (targetButton == NULL) {
		mouseOverButton = NULL;
		mouseDownButton = NULL;
		return;
	}
	if (event->button.button == SDL_BUTTON_LEFT) {
		if (event->type == SDL_MOUSEBUTTONDOWN) {		//	Mouse down
			if (!mouseDownButton) {
				mouseDownButton = targetButton;
			}
		}
		else if (event->type == SDL_MOUSEBUTTONUP) {	//	Mouse up
			if (mouseDownButton) {
				mouseDownButton->clickEvent();
				mouseDownButton = NULL;
			}
		}
	}
	else if (event->type == SDL_MOUSEMOTION) {		// Hover action
		mouseOverButton = targetButton;
	}
}

void Button::mouseEvent(SDL_Event * event, Button * button, int mouseX, int mouseY)
{
	Button* targetButton = NULL;
	int tx = button->x;
	int ty = button->y;
	int tw = button->w;
	int th = button->h;
	if (mouseX >= tx && mouseX <= tx + tw && mouseY >= ty && mouseY <= ty + th) {
		targetButton = button;
	}
	if (targetButton == NULL) {
		mouseOverButton = NULL;
		mouseDownButton = NULL;
		return;
	}
	
	if (event->button.button == SDL_BUTTON_LEFT) {
		if (event->type == SDL_MOUSEBUTTONDOWN) {		//	Mouse down
			if (!mouseDownButton) {
				mouseDownButton = targetButton;
			}
		}
		else if (event->type == SDL_MOUSEBUTTONUP) {	//	Mouse up
			if (mouseDownButton) {
				mouseDownButton->clickEvent();
				mouseDownButton = NULL;
			}
		}
	}
	else if (event->type == SDL_MOUSEMOTION) {		// Hover action
		mouseOverButton = targetButton;
	}
}