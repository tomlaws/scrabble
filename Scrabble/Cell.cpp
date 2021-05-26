#include "Cell.h"
#include "GUI.h"
#include "Tile.h"

Cell::Cell()
{

}

Cell::~Cell() {
	delete letter;
	if (locked)
		delete tilePlaced;
}

// Initialize 7 Cells for player

Cell::Cell(int x, int y, int w, int h)
{
	occupied = true;
	locked = false;
	tilePlaced = nullptr;
	this->col = -1;
	this->row = -1;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

// Initialize 15x15 Cells on the board

Cell::Cell(int col, int row, int x, int y, int w, int h)
{
	occupied = false;
	locked = false;
	tilePlaced = nullptr;
	this->col = col;
	this->row = row;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

bool Cell::getOccupied()
{
	return occupied;
}

bool Cell::getLocked()
{
	return locked;
}

Tile * Cell::getTilePlaced()
{
	return tilePlaced;
}

ExtraPoint Cell::getExtraPoint()
{
	return extraPoint;
}

void Cell::setOccupied(bool occupied)
{
	this->occupied = occupied;
}

void Cell::setLocked(bool locked)
{
	this->locked = locked;
	if (locked)
		time(&lockedTime);
}

void Cell::setTilePlaced(Tile * tile)
{
	tilePlaced = tile;
}

void Cell::setExtraPoint(ExtraPoint extraPoint)
{
	this->extraPoint = extraPoint;
}
