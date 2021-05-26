#include "MyDeck.h"

MyDeck::MyDeck()
{

}

MyDeck::~MyDeck()
{
}

void MyDeck::drawPanel() {
	SDL_Rect r;
	r.x = 0;
	r.y = GUI::getWindowHeight() - MYDECK_HEIGHT;
	r.w = GUI::getWindowWidth();
	r.h = 60;
	SDL_SetRenderDrawColor(GUI::getRenderer(), 0, 61, 82, 255);
	SDL_RenderFillRect(GUI::getRenderer(), &r);

	// Border
	r.y -= 1;
	r.h = 1;
	SDL_SetRenderDrawColor(GUI::getRenderer(), 0, 24, 31, 255);
	SDL_RenderFillRect(GUI::getRenderer(), &r);
}

void MyDeck::draw() {
	drawPanel();
}

