#include "SDL.h"
#include "SDL_ttf.h"
#include <functional>
#include <string>
#include <vector>

class TileButton : public SDL_Rect {
private:
	static std::vector<TileButton*> tileButtons;
	static TileButton* mouseDownButton;
	static TileButton* mouseOverButton;
	std::function<void(char letter)> clickEvent = NULL;

	std::string name;
	std::wstring text;
	SDL_Color color;
	TTF_Font * font;
	SDL_Color textColor;

	bool isInstance;
	SDL_Texture * textTexture;
	SDL_Rect textTextureRect;
public:
	static TileButton * add(std::string name, std::wstring text, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void(char)> clickEvent);
	TileButton();
	~TileButton();
	TileButton(std::string name, std::wstring text, SDL_Color color, TTF_Font * font, int x, int y, int w, int h, std::function<void(char)> clickEvent);
	static void mouseEvent(SDL_Event * event, int mouseX, int mouseY);
	void draw(SDL_Renderer * renderer);
	void drawAll(SDL_Renderer * renderer);
};