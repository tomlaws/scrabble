#include "SDL.h"
#include "SDL_ttf.h"
#include <vector>

#ifndef GUI_H
#define GUI_H

class Button;

class GUI {
private:
	SDL_Window * window;
	SDL_Renderer * renderer;
	double deltaTime;
	bool hasEvent;
	SDL_Event * event;
	int windowWidth;
	int windowHeight;
	SDL_Texture * textureBackground;
	TTF_Font * fontExtraLarge;
	TTF_Font * fontLarge;
	TTF_Font * fontMedium;
	TTF_Font * fontSmall;
	TTF_Font * fontExtraSmall;
	TTF_Font * fontFa;
	TTF_Font * fontLemonMilk;
	TTF_Font * fontLemonMilkLarge;
	Button * enterButton;
	void button_enter();
public:
	struct Message {
		public:
			std::string message;
			int type;
			int width;
			double time;
	};
	GUI(bool *quit, int winWidth, int winHeight);
	~GUI(void);
	SDL_Renderer * getRenderer();
	SDL_Event * getMainEvent();
	double getDeltaTime();
	bool getHasEvent();
	SDL_Window * getWindow();
	int getWindowWidth();
	int getWindowHeight();
	TTF_Font * getFontExtraLarge();
	TTF_Font * getFontLarge();
	TTF_Font * getFontMedium();
	TTF_Font * getFontSmall();
	TTF_Font * getFontExtraSmall();
	TTF_Font * getFontAwesome();
	TTF_Font * getFontLemonMilk();
	TTF_Font * getFontLemonMilkLarge();
	void begin();
	void end();
	void drawBackground();
	void drawObject(SDL_Rect* rect, SDL_Color color);
	void drawObject(int x, int y, int w, int h, SDL_Color color);
	void drawTexture(SDL_Texture * texture, SDL_Rect * rect);
	SDL_Texture * createTextTexture(SDL_Rect * rect, TTF_Font * font, const char * text, SDL_Color textColor);
	SDL_Texture * createCharTexture(SDL_Rect * rect, TTF_Font * font, const char * text, SDL_Color textColor);
	SDL_Texture * createUnicodeTextTexture(SDL_Rect * rect, TTF_Font * font, const Uint16 * text, SDL_Color textColor);
	SDL_Texture * createWrappedTextTexture(SDL_Rect * rect, TTF_Font * font, const char * text, SDL_Color textColor, int wrappedWidth);

	Message * addMessage(std::string message, int type, int width);
	bool drawMessage();
	void clearMessage();
	std::vector<Message*> messages;
};

#endif // !GUI_H