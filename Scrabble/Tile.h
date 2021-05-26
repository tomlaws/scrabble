#include "SDL.h"
#include "GUI.h"

class Cell;

class Tile : public SDL_Rect {
private:
	char letter;
	int score;
	bool onBoard;
	bool draggable;
	bool visible;
	Cell * boundedCell;

	SDL_Texture * letterTexture;
	SDL_Rect letterTextureRect;
	SDL_Texture * scoreTexture;
	SDL_Rect scoreTextureRect;

	//	The texture used when the tile is placed on the board (shrinked)
	SDL_Texture * s_letterTexture;
	SDL_Rect s_letterTextureRect;
	SDL_Texture * s_scoreTexture;
	SDL_Rect s_scoreTextureRect;
public:
	Tile(char letter, int score);
	~Tile();

	int col;
	int row;
	int originalX, originalY;

	char getLetter();
	int getScore();
	bool getOnBoard();
	bool getDraggable();
	bool getVisible();

	void setLetter(char letter);
	void setOnBoard(bool onBoard);
	void setDraggable(bool draggable);
	void setVisible(bool visible);
	void boundToCell(Cell * cell);
	void draw(GUI * gui);
};