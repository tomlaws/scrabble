#ifndef TILEBAG_H
#define TILEBAG_H

#include <vector>

class Tile;

class TileBag {
private:
	const static int letterCount[27];
	const static int scoreDistributions[27];
	std::vector<Tile*> tiles;
public:
	TileBag();
	~TileBag();
	void reset();
	Tile * drawTile();
	Tile * exchangeTile(Tile * tile);
	int getRemainingTilesCount();
};

#endif // !TILEBAG_H