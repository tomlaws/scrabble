#ifndef BOARD_H
#define BOARD_H

#include "Move.h"
#include "SDL.h"
#include <vector>

class Cell;
class Dictionary;
class GUI;

class Board {
private:
	Dictionary * dictionary;
	std::vector <std::vector<Cell*>> cells;	//	15x15 Cells
	SDL_Texture * boardTexture;
	GUI * boardGUI;
public:
	Board(bool shuffleBonus = false);
	~Board();
	bool validateMove(std::vector<Move> & moves, Move::Direction & direction, std::string * resultMessage);
	int calculateScore(std::vector<Move>& moves, Move::Direction & direction, std::string * resultMessage);
	int getWholeWordAndScore(Tile * tile, Move::Direction checkDirection, std::string * word);
	void draw(GUI * gui);
	void updateTiles();
	Cell * getCell(int col, int row);
	Cell * findCell(int mouseX, int mouseY);
	std::vector<std::pair<Cell *, std::vector<std::string>>> getLockedCellRegex();
};

#endif // !BOARD_H