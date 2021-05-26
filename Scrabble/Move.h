#ifndef MOVE_H
#define MOVE_H

class Tile;

struct Move
{
	enum Direction {
		DIRECTION_NONE,
		DIRECTION_VERTICAL,
		DIRECTION_HORIZONTAL
	};
	Tile* tile;
	int col;
	int row;
};

#endif // !MOVE_H