#include "TileButton.h"
#include <vector>

std::vector<TileButton*> TileButton::tileButtons;
TileButton * TileButton::mouseOverButton;
TileButton * TileButton::mouseDownButton;

TileButton::TileButton()
{
	isInstance = false;
}

TileButton::~TileButton()
{
	if (isInstance) return;
	for (int i = 0; i < tileButtons.size(); i++) {
		SDL_DestroyTexture(tileButtons[i]->textTexture);
		delete tileButtons[i];
	}
	tileButtons.clear();
}

TileButton * TileButton::add(std::string name, std::wstring text, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void(char)> clickEvent)
{
	return new TileButton(name, text, color, font, x, y, w, h, clickEvent);
}

TileButton::TileButton(std::string name, std::wstring text, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void(char)> clickEvent)
{
	this->name = name;
	this->text = text;
	this->color = color;
	textColor = SDL_Color{ 0, 61, 82, 255 };
	this->font = font;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->clickEvent = clickEvent;
	textTexture = nullptr;
	isInstance = true;
	tileButtons.push_back(this);
	mouseDownButton = nullptr;
	mouseOverButton = nullptr;
}

void TileButton::mouseEvent(SDL_Event * event, int mouseX, int mouseY)
{
	TileButton* targetButton = NULL;
	for (TileButton * button : tileButtons) {
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
				char letter = mouseDownButton->text[0];
				mouseDownButton->clickEvent(letter);
				mouseDownButton = NULL;
			}
		}
	}
	else if (event->type == SDL_MOUSEMOTION) {		// Hover action
		mouseOverButton = targetButton;
	}
}

void TileButton::draw(SDL_Renderer * renderer)
{
	//	Set Color
	if (mouseDownButton && this == mouseDownButton) {
		int r = this->color.r - 5;
		int g = this->color.g - 5;
		int b = this->color.b - 5;
		SDL_SetRenderDrawColor(renderer, r < 0 ? 0 : r, g < 0 ? 0 : g, b < 0 ? 0 : b, this->color.a);
	}
	else if (mouseOverButton && this == mouseOverButton) {
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

void TileButton::drawAll(SDL_Renderer * renderer)
{
	for (TileButton * button : tileButtons) {
		button->draw(renderer);
	}
}