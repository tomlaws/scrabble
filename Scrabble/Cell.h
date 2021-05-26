#ifndef Cell_H
#define Cell_H

#include "SDL.h"
#include "ExtraPoint.h"
#include <ctime>

class Tile;

class Cell : public SDL_Rect {
private:
	bool occupied;				//	
	bool locked;				//	Only set to true when confirmed move
	char * letter;
	Tile * tilePlaced = NULL;
	ExtraPoint extraPoint = NO_EXTRA_POINT;
public:
	Cell();
	Cell(int x, int y, int w, int h);
	Cell(int col, int row, int x, int y, int w, int h);
	~Cell();

	int col, row;
	time_t lockedTime;
	bool getOccupied();
	bool getLocked();
	Tile * getTilePlaced();
	ExtraPoint getExtraPoint();

	void setOccupied(bool occupied);
	void setLocked(bool locked);
	void setTilePlaced(Tile * tile);
	void setExtraPoint(ExtraPoint extraPoint);
};

#endif // !Cell_H