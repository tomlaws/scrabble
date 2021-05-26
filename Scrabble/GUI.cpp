#include "GUI.h"
#include "Button.h"
#include "SDL.h"
#include "SDL_image.h"

GUI::GUI(bool* quit, int winWidth, int winHeight)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Scrabble", 100, 100, winWidth, winHeight, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		*quit = true;
		return;
	}
	renderer = NULL;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (TTF_Init() == -1) {
		*quit = true;
		return;
	}
	textureBackground = nullptr;
	fontExtraLarge = TTF_OpenFont("FreeSans.ttf", 32);
	fontLarge = TTF_OpenFont("FreeSans.ttf", 24);
	fontMedium = TTF_OpenFont("FreeSans.ttf", 16);
	fontSmall = TTF_OpenFont("FreeSans.ttf", 12);
	fontExtraSmall = TTF_OpenFont("FreeSans.ttf", 8);
	fontFa = TTF_OpenFont("FontAwesome.ttf", 18);
	fontLemonMilk = TTF_OpenFont("LemonMilklight.ttf", 18);
	fontLemonMilkLarge = TTF_OpenFont("LemonMilklight.ttf", 82);

	event = new SDL_Event();
	deltaTime = 0;
	hasEvent = false;
	windowWidth = winWidth;
	windowHeight = winHeight;
	enterButton = nullptr;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

GUI::~GUI(void)
{
	TTF_CloseFont(fontExtraLarge);
	TTF_CloseFont(fontLarge);
	TTF_CloseFont(fontMedium);
	TTF_CloseFont(fontSmall);
	TTF_CloseFont(fontExtraSmall);
	TTF_CloseFont(fontFa);
	TTF_CloseFont(fontLemonMilk);
	TTF_CloseFont(fontLemonMilkLarge);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	delete event;
	SDL_DestroyTexture(textureBackground);
}

SDL_Renderer* GUI::getRenderer()
{
	return renderer;
}

SDL_Event* GUI::getMainEvent()
{
	return event;
}

double GUI::getDeltaTime()
{
	return deltaTime;
}

bool GUI::getHasEvent()
{
	return hasEvent;
}

SDL_Window * GUI::getWindow()
{
	return window;
}

int GUI::getWindowWidth()
{
	return windowWidth;
}

int GUI::getWindowHeight()
{
	return windowHeight;
}

TTF_Font * GUI::getFontExtraLarge()
{
	return fontExtraLarge;
}

TTF_Font * GUI::getFontLarge()
{
	return fontLarge;
}

TTF_Font * GUI::getFontMedium()
{
	return fontMedium;
}

TTF_Font * GUI::getFontSmall()
{
	return fontSmall;
}

TTF_Font * GUI::getFontExtraSmall()
{
	return fontExtraSmall;
}

TTF_Font * GUI::getFontAwesome()
{
	return fontFa;
}

TTF_Font * GUI::getFontLemonMilk()
{
	return fontLemonMilk;
}

TTF_Font * GUI::getFontLemonMilkLarge()
{
	return fontLemonMilkLarge;
}

void GUI::drawBackground() {
	if (!textureBackground) {
		textureBackground = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, 1024, 768);
		SDL_SetTextureBlendMode(textureBackground, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, textureBackground);
		int size = 2;
		int num = windowHeight / size;
		for (int i = 0; i < num; i++) {
			SDL_Rect r;
			r.x = 0;
			r.y = size * i;
			r.w = windowWidth;
			r.h = size;
			SDL_SetRenderDrawColor(renderer, 0, 97 - ((float)i / num * 80), 133 - ((float)i / num * 80), 255);
			SDL_RenderFillRect(renderer, &r);
		}
		SDL_SetRenderTarget(renderer, NULL);
	}
	drawTexture(textureBackground, NULL);
}

Uint64 last, now;
void GUI::begin()
{
	last = now;
	now = SDL_GetPerformanceCounter();
	deltaTime = (double)((now - last) * 1000 / SDL_GetPerformanceFrequency());

	hasEvent = SDL_PollEvent(event);
	SDL_RenderClear(renderer);
}

void GUI::end()
{
	SDL_RenderPresent(renderer);
}

void GUI::drawObject(SDL_Rect * rect, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, rect);
}

void GUI::drawObject(int x, int y, int w, int h, SDL_Color color)
{
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	drawObject(&r, color);
}

void GUI::drawTexture(SDL_Texture * texture, SDL_Rect * rect)
{
	SDL_RenderCopy(renderer, texture, NULL, rect);
}

SDL_Texture * GUI::createTextTexture(SDL_Rect * rect, TTF_Font * font, const char * text, SDL_Color textColor)
{
	SDL_Surface * surface = TTF_RenderText_Blended(font, text, textColor);
	if (surface) {
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
		SDL_FreeSurface(surface);
		return texture;
	}
	return nullptr;
}

SDL_Texture * GUI::createCharTexture(SDL_Rect * rect, TTF_Font * font, const char * text, SDL_Color textColor)
{
	std::string s;
	s.push_back(*text);
	return createTextTexture(rect, font, s.c_str(), textColor);
}

SDL_Texture * GUI::createUnicodeTextTexture(SDL_Rect * rect, TTF_Font * font, const Uint16 * text, SDL_Color textColor)
{
	SDL_Surface * surface = TTF_RenderUNICODE_Blended(font, text, textColor);
	if (surface) {
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
		SDL_FreeSurface(surface);
		return texture;
	}
	return nullptr;
}

SDL_Texture * GUI::createWrappedTextTexture(SDL_Rect * rect, TTF_Font * font, const char * text, SDL_Color textColor, int wrappedWidth)
{
	SDL_Surface * surface = TTF_RenderText_Blended_Wrapped(font, text, textColor, wrappedWidth);
	if (surface) {
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
		SDL_FreeSurface(surface);
		return texture;
	}
	return nullptr;
}

GUI::Message * GUI::addMessage(std::string message, int type, int width)
{
	Message * m = new Message();
	m->message = message;
	m->type = type;
	m->width = width;
	m->time = 1500;
	messages.push_back(m);
	return m;
}
bool GUI::drawMessage()
{
	if (messages.size() == 0) return false;
	Message * m = messages.front();

	//	Draw text
	SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(getFontMedium(), m->message.c_str(), SDL_Color{ 255, 255, 255, 255 }, m->width);
	if (surface) {
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_Rect r;
		SDL_QueryTexture(texture, NULL, NULL, &r.w, &r.h);
		r.x = (windowWidth - r.w) / 2;
		r.y = (windowHeight - r.h) / 2;

		//	
		SDL_Rect bg;
		bg.w = r.w + 30 < m->width ? m->width : r.w + 30;
		bg.h = r.h + 40;
		bg.x = (1024 - bg.w) / 2;
		bg.y = (768 - bg.h) / 2;
		if (m->type == 0) {
			bg.h += 60;
			r.y -= 30;
			bg.y -= 30;
			drawObject(0, 0, 1024, 768, SDL_Color{ 0, 0, 0, 180 });
			drawObject(&bg, SDL_Color{ 0, 0, 0, 200 });
			if (!enterButton) {
				enterButton = new Button(L"OK", SDL_Color{ 0, 135, 184, 255 }, fontLemonMilk, 472, bg.y + bg.h - 45, 80, 30, std::bind(&GUI::button_enter, this));
			}
			
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			Button::mouseEvent(getMainEvent(), enterButton, mouseX, mouseY);

			enterButton->draw(renderer);
		}
		else {
			drawObject(&bg, SDL_Color{ 0, 0, 0, 200 });
			m->time -= deltaTime;
		}
		
		SDL_RenderCopy(renderer, texture, NULL, &r);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	if (m->time <= 0) {
		delete m;
		messages.erase(messages.begin());
		if (enterButton) {
			delete enterButton;
			enterButton = nullptr;
		}
	}
	if (m->type == 0)
		return true;
	else
		return false;
}

void GUI::clearMessage()
{
	if (messages.size() == 0) return;
	for (int i = 0; i < messages.size(); i++) {
		delete messages[i];
	}
	messages.clear();
	if (enterButton) {
		delete enterButton;
		enterButton = nullptr;
	}
}

void GUI::button_enter()
{
	if (enterButton) {
		messages.front()->time = 0;
	}
}