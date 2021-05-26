#include "SDL.h"
#include "SDL_ttf.h"
#include <functional>
#include <string>
#include <vector>

#ifndef BUTTON_H
#define BUTTON_H
class Button : public SDL_Rect {
private:
	static TTF_Font * tooltipFont;
	static std::vector<Button*> buttons;
	static Button* mouseDownButton;
	static Button* mouseOverButton;

	Button(std::string name, std::wstring text, std::string tooltip, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void()> clickEvent);

	std::string name;
	std::wstring text;
	std::string tooltip;
	SDL_Color color;
	TTF_Font * font;
	SDL_Color textColor;

	bool enabled;
	bool visible;
	bool isInstance;
	std::function<void()> clickEvent = NULL;
	SDL_Texture * textTexture;
	SDL_Rect textTextureRect;
public:
	Button();
	Button(std::wstring text, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void()> clickEvent);
	~Button();

	static Button * add(std::string name, std::wstring text, std::string tooltip, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void()> clickEvent);
	static void drawAll(SDL_Renderer * renderer);
	static void mouseEvent(SDL_Event * event, int mouseX, int mouseY);
	static void mouseEvent(SDL_Event * event, Button * button, int mouseX, int mouseY);

	void setEnabled(bool enabled);
	void setVisible(bool visible);
	void setClickEvent(std::function<void()> clickEvent);

	void draw(SDL_Renderer * renderer);
	void showTooltip(SDL_Renderer * renderer);
};
#endif // !BUTTON_H